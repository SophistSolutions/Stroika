/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_POSIX
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/WaitForIOReady.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"
#include "Stroika/Foundation/IO/Network/Socket-Private_.h"
#include "Stroika/Foundation/Memory/BLOB.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using std::byte;

using namespace Stroika::Foundation::IO::Network::PRIVATE_;

using namespace ConnectionOrientedStreamSocket;

using Debug::AssertExternallySynchronizedMutex;

namespace {
    struct Rep_ : BackSocketImpl_<ConnectionOrientedStreamSocket::_IRep> {
        using inherited = BackSocketImpl_<ConnectionOrientedStreamSocket::_IRep>;
        Rep_ (Socket::PlatformNativeHandle sd)
            : inherited{sd}
        {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("Constructed BackSocketImpl_<ConnectionOrientedStreamSocket>::Rep_ with sd={:x}"_f, (int)sd);
#endif
        }
        Rep_ ()            = delete;
        Rep_ (const Rep_&) = delete;
        ~Rep_ ()
        {
            // need DTOR cuz one in base class wont call this override of Close
            if (fSD_ != kINVALID_NATIVE_HANDLE_) {
                Close ();
            }
        }
        virtual void Close () override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized};
            if (fSD_ != kINVALID_NATIVE_HANDLE_ and fAutomaticTCPDisconnectOnClose_) {
                Shutdown (Socket::ShutdownTarget::eWrites);
                Time::TimePointSeconds    timeOutAt = Time::GetTickCount () + *fAutomaticTCPDisconnectOnClose_;
                Execution::WaitForIOReady ioReady{fSD_};
                try {
                again:
                    (void)ioReady.WaitUntil (timeOutAt);
                    char data[1024];
#if qPlatform_POSIX
                    int nb = ::read (fSD_, data, NEltsOf (data));
#elif qPlatform_Windows
                    int flags = 0;
                    int nb    = ::recv (fSD_, data, (int)NEltsOf (data), flags);
#endif
                    if (nb > 0) {
                        DbgTrace ("Warning: {} unread bytes to be read on socket when it was closed."_f,
                                  nb); // SHOULD READ ZERO AFTER SHUTDOWN to indicate client other side of connection handled the close
                        goto again;
                    }
                }
                catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace (L"timeout closing down socket - not serious - just means client didn't send close ACK quickly enough");
#endif
                }
            }
            inherited::Close ();
        }
        nonvirtual void Connect_Sync_ (const SocketAddress& sockAddr) const
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            sockaddr_storage                               useSockAddr = sockAddr.As<sockaddr_storage> ();
#if qPlatform_POSIX
            Handle_ErrNoResultInterruption ([&] () -> int { return ::connect (fSD_, (sockaddr*)&useSockAddr, sockAddr.GetRequiredSize ()); });
#elif qPlatform_Windows
            ThrowWSASystemErrorIfSOCKET_ERROR (::connect (fSD_, (sockaddr*)&useSockAddr, static_cast<int> (sockAddr.GetRequiredSize ())));
#else
            AssertNotImplemented ();
#endif
        }
        nonvirtual void Connect_AsyncWTimeout_ (const SocketAddress& sockAddr, const Time::Duration& timeout) const
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            sockaddr_storage                               useSockAddr = sockAddr.As<sockaddr_storage> ();
#if qPlatform_POSIX
            // http://developerweb.net/viewtopic.php?id=3196.
            // and see https://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections/4182564#4182564 for why not using SO_RCVTIMEO/SO_SNDTIMEO
            long savedFlags{};
            ThrowPOSIXErrNoIfNegative (savedFlags = ::fcntl (fSD_, F_GETFL, nullptr));
            ThrowPOSIXErrNoIfNegative (::fcntl (fSD_, F_SETFL, savedFlags | O_NONBLOCK)); // non-blocking mode for select
            [[maybe_unused]] auto&& cleanup = Finally ([this, savedFlags] () noexcept {
                // Set to blocking mode again...
                if (::fcntl (fSD_, F_SETFL, savedFlags) < 0) {
                    AssertNotReached (); // cannot throw here
                }
            });
            while (::connect (fSD_, (sockaddr*)&useSockAddr, sockAddr.GetRequiredSize ()) < 0) {
                switch (errno) {
                    case EINTR:
                        break; // ignore - try again
                    case EINPROGRESS: {
                        fd_set myset;
                        FD_ZERO (&myset);
                        FD_SET (fSD_, &myset);
                        timeval time_out = timeout.As<timeval> ();
                        Handle_ErrNoResultInterruption ([&] () -> int {
                            auto r = ::select (fSD_ + 1, NULL, &myset, nullptr, &time_out);
                            if (r == 0) {
                                // https://man7.org/linux/man-pages/man2/select.2.html - "The return value may be zero if the timeout expired before any file descriptors became ready"
                                Execution::Throw (Execution::TimeOutException::kThe);
                            }
                            return r;
                        });
                        // Check the errno value returned...
                        if (auto err = getsockopt<int> (SOL_SOCKET, SO_ERROR)) {
                            Execution::ThrowSystemErrNo (err);
                        }
                        return; // else must have succeeded
                    } break;
                    default: {
                        Execution::ThrowSystemErrNo ();
                    } break;
                }
            }
#elif qPlatform_Windows
            // https://stackoverflow.com/questions/46045434/winsock-c-connect-timeout
            {
                u_long block = 1;
                if (::ioctlsocket (fSD_, FIONBIO, &block) == SOCKET_ERROR) {
                    Execution::ThrowSystemErrNo (::WSAGetLastError ());
                }
            }
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept {
                u_long block = 0; // should have saved old value, but not clear how to read?
                if (::ioctlsocket (fSD_, FIONBIO, &block) == SOCKET_ERROR) {
                    //Execution::ThrowSystemErrNo (::WSAGetLastError ());
                    Assert (false); //FAILURE SETTING BACK TO NOT BLOCKING - SERIOUS
                }
            });
            if (::connect (fSD_, (sockaddr*)&useSockAddr, static_cast<int> (sockAddr.GetRequiredSize ())) == SOCKET_ERROR) {
                if (::WSAGetLastError () != WSAEWOULDBLOCK) {
                    Execution::ThrowSystemErrNo (::WSAGetLastError ()); // connection failed
                }
                // connection pending
                fd_set setW;
                FD_ZERO (&setW);
                FD_SET (fSD_, &setW);
                fd_set setE;
                FD_ZERO (&setE);
                FD_SET (fSD_, &setE);
                timeval time_out = timeout.As<timeval> ();
                int     ret      = ::select (0, NULL, &setW, &setE, &time_out);
                if (ret <= 0) {
                    // select() failed or connection timed out
                    if (ret == 0) {
                        WSASetLastError (WSAETIMEDOUT);
                    }
                    Execution::ThrowSystemErrNo (::WSAGetLastError ()); // connection failed
                }
                // Check the errno value returned...
                if (auto err = getsockopt<int> (SOL_SOCKET, SO_ERROR)) {
                    Execution::ThrowSystemErrNo (err);
                }
                // else if got here >0 so succeeded with connection
            }
#else
            AssertNotImplemented ();
#endif
        }
        virtual void Connect (const SocketAddress& sockAddr, const optional<Time::Duration>& timeout) const override
        {
            Debug::TraceContextBumper ctx{"ConnectionOrientedStreamSocket_IMPL_::Connect", "sockAddr={}, timeout={}"_f, sockAddr, timeout};
            if (timeout) {
                Connect_AsyncWTimeout_ (sockAddr, *timeout);
            }
            else {
                Connect_Sync_ (sockAddr);
            }
        }
        virtual size_t Read (byte* intoStart, byte* intoEnd) const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};

#if qDebug
            Assert (fCurrentPendingReadsCount++ == 0);
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { Assert (--fCurrentPendingReadsCount == 0); });
#endif

#if qPlatform_POSIX
            return Handle_ErrNoResultInterruption (
                [this, &intoStart, &intoEnd] () -> int { return ::read (fSD_, intoStart, intoEnd - intoStart); });
#elif qPlatform_Windows
            int flags        = 0;
            int nBytesToRead = static_cast<int> (min<size_t> ((intoEnd - intoStart), numeric_limits<int>::max ()));
            return static_cast<size_t> (ThrowWSASystemErrorIfSOCKET_ERROR (::recv (fSD_, reinterpret_cast<char*> (intoStart), nBytesToRead, flags)));
#else
            AssertNotImplemented ();
#endif
        }
        virtual optional<size_t> ReadNonBlocking (byte* intoStart, byte* intoEnd) const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
#if qDebug
            Assert (fCurrentPendingReadsCount++ == 0);
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { Assert (--fCurrentPendingReadsCount == 0); });
#endif
#if qPlatform_POSIX or qPlatform_Windows
            {
                fd_set input;
                FD_ZERO (&input);
                FD_SET (fSD_, &input);
                struct timeval timeout {};
                if (::select (static_cast<int> (fSD_) + 1, &input, NULL, NULL, &timeout) == 1) {
                    if (intoStart == nullptr) {
                        // don't know how much, but doesn't matter, since read allows returning just one byte if thats all thats available
                        // But MUST check if is EOF or real data available
                        char buf[1024];
#if qPlatform_POSIX
                        int tmp = Handle_ErrNoResultInterruption ([&] () -> int { return ::recv (fSD_, buf, NEltsOf (buf), MSG_PEEK); });
#elif qPlatform_Windows
                        int tmp = ThrowWSASystemErrorIfSOCKET_ERROR (::recv (fSD_, buf, static_cast<int> (NEltsOf (buf)), MSG_PEEK));
#else
                        AssertNotImplemented ();
#endif
                        return tmp;
                    }
#if qDebug
                    --fCurrentPendingReadsCount; // reverse for inherited Read ()
                    [[maybe_unused]] auto&& cleanup2 = Finally ([this] () noexcept { ++fCurrentPendingReadsCount; });
#endif
                    return Read (intoStart, intoEnd);
                }
                else {
                    return {};
                }
            }
#else
            AssertNotImplemented ();
            return {};
#endif
        }
        virtual void Write (const byte* start, const byte* end) const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::Network::Socket...rep...::Write", L"end-start=%lld",
                                                                                         static_cast<long long> (end - start))};
#endif
#if qPlatform_POSIX
            /*
             *  https://linux.die.net/man/2/write says "writes up to count bytes". So handle case where we get partial writes.
             *  Actually, for most of the cases called out, we cannot really continue anyhow, so this maybe pointless, but the
             *  docs aren't fully clear, so play it safe --LGP 2017-04-13
             */
            BreakWriteIntoParts_<byte> (start, end, numeric_limits<int>::max (), [this] (const byte* start, const byte* end) -> size_t {
                Assert ((end - start) < numeric_limits<int>::max ());
                ssize_t n = Handle_ErrNoResultInterruption ([this, &start, &end] () -> ssize_t { return ::write (fSD_, start, end - start); });
                ThrowPOSIXErrNoIfNegative (n);
                Assert (0 <= n and n <= (end - start));
                return static_cast<size_t> (n);
            });
#elif qPlatform_Windows
            /*
             *  Note sure what the best way is here, but with WinSock, you cannot use write() directly. Sockets are not
             *  file descriptors in windows implementation.
             *      WONT WORK:
             *          int       n   =   ::_write (fSD_, start, end - start);
             */
            size_t maxSendAtATime = getsockopt<unsigned int> (SOL_SOCKET, SO_MAX_MSG_SIZE);
            BreakWriteIntoParts_<byte> (start, end, maxSendAtATime, [this, maxSendAtATime] (const byte* start, const byte* end) -> size_t {
                Require (static_cast<size_t> (end - start) <= maxSendAtATime);
                Assert ((end - start) < numeric_limits<int>::max ());
                int len   = static_cast<int> (end - start);
                int flags = 0;
                int n     = ThrowWSASystemErrorIfSOCKET_ERROR (::send (fSD_, reinterpret_cast<const char*> (start), len, flags));
                Assert (0 <= n and n <= (end - start));
                return static_cast<size_t> (n);
            });
#else
            AssertNotImplemented ();
#endif
        }
        virtual optional<IO::Network::SocketAddress> GetPeerAddress () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            struct sockaddr_storage                        radr;
            socklen_t                                      len = sizeof (radr);
            if (::getpeername (static_cast<int> (fSD_), (struct sockaddr*)&radr, &len) == 0) {
                IO::Network::SocketAddress sa{radr};
                return sa;
            }
            return nullopt;
        }
        virtual optional<Time::DurationSeconds> GetAutomaticTCPDisconnectOnClose () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            return fAutomaticTCPDisconnectOnClose_;
        }
        virtual void SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSeconds>& waitFor) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            fAutomaticTCPDisconnectOnClose_ = waitFor;
        }
        virtual KeepAliveOptions GetKeepAlives () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            KeepAliveOptions                               result;
            result.fEnabled = !!getsockopt<int> (SOL_SOCKET, SO_KEEPALIVE);
#if qPlatform_Linux
            // Only available if linux >= 2.4
            result.fMaxProbesSentBeforeDrop              = getsockopt<int> (SOL_TCP, TCP_KEEPCNT);
            result.fTimeIdleBeforeSendingKeepalives      = Time::DurationSeconds{getsockopt<int> (SOL_TCP, TCP_KEEPIDLE)};
            result.fTimeBetweenIndividualKeepaliveProbes = Time::DurationSeconds{getsockopt<int> (SOL_TCP, TCP_KEEPINTVL)};
#elif qPlatform_Windows
// WSAIoctl (..., SIO_KEEPALIVE_VALS) can be used to set some of these values, but I can find no way
// to fetch them --LGP 2017-02-27
#endif
            return result;
        }
        virtual void SetKeepAlives (const KeepAliveOptions& keepAliveOptions) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            setsockopt<int> (SOL_SOCKET, SO_KEEPALIVE, keepAliveOptions.fEnabled);
#if qPlatform_Linux
            // Only available if linux >= 2.4
            if (keepAliveOptions.fMaxProbesSentBeforeDrop) {
                setsockopt<int> (SOL_TCP, TCP_KEEPCNT, *keepAliveOptions.fMaxProbesSentBeforeDrop);
            }
            if (keepAliveOptions.fTimeIdleBeforeSendingKeepalives) {
                setsockopt<int> (SOL_TCP, TCP_KEEPIDLE, static_cast<int> (keepAliveOptions.fTimeIdleBeforeSendingKeepalives->count ()));
            }
            if (keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes) {
                setsockopt<int> (SOL_TCP, TCP_KEEPINTVL, static_cast<int> (keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes->count ()));
            }
#elif qPlatform_Windows
            // windows only allows setting these two, and both at the same time
            if (keepAliveOptions.fEnabled and
                (keepAliveOptions.fTimeIdleBeforeSendingKeepalives or keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes)) {
                tcp_keepalive alive{keepAliveOptions.fEnabled};
                // from https://msdn.microsoft.com/en-us/library/windows/desktop/dd877220(v=vs.85).aspx - "The default settings when a TCP socket is initialized sets the keep-alive timeout to 2 hours and the keep-alive interval to 1 second"
                alive.keepalivetime = Math::Round<ULONG> (keepAliveOptions.fTimeIdleBeforeSendingKeepalives.value_or (2 * 60 * 60s).count () * 1000.0);
                alive.keepaliveinterval = Math::Round<ULONG> (keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes.value_or (1s).count () * 1000.0);
                DWORD dwBytesRet{};
                if (::WSAIoctl (fSD_, SIO_KEEPALIVE_VALS, &alive, sizeof (alive), NULL, 0, &dwBytesRet, NULL, NULL) == SOCKET_ERROR) {
                    Execution::ThrowSystemErrNo (::WSAGetLastError ());
                }
            }
#endif
        }
        virtual bool GetTCPNoDelay () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            return static_cast<bool> (getsockopt<int> (IPPROTO_TCP, TCP_NODELAY));
        }
        virtual void SetTCPNoDelay (bool noDelay) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            setsockopt<int> (IPPROTO_TCP, TCP_NODELAY, noDelay);
        }
        optional<Time::DurationSeconds> fAutomaticTCPDisconnectOnClose_;
#if qDebug
        mutable atomic<int> fCurrentPendingReadsCount{};
#endif
    };
}

/*
 ********************************************************************************
 ********** Network::ConnectionOrientedStreamSocket::KeepAliveOptions ***********
 ********************************************************************************
 */
Characters::String Network::ConnectionOrientedStreamSocket::KeepAliveOptions::ToString () const
{
    Characters::StringBuilder sb;
    sb << "{"sv;
    sb << "Enabled: "sv << fEnabled << ","sv;
#if qPlatform_Linux or qPlatform_Windows
    if (fMaxProbesSentBeforeDrop) {
        sb << "Max-Probes-Sent-Before-Drop: "sv << fMaxProbesSentBeforeDrop << ","sv;
    }
    if (fTimeIdleBeforeSendingKeepalives) {
        sb << "Time-Idle-Before-Sending-Keepalives: "sv << fTimeIdleBeforeSendingKeepalives << ","sv;
    }
    if (fTimeBetweenIndividualKeepaliveProbes) {
        sb << "Time-Between-Individual-Keepalive-Probes: "sv << fTimeBetweenIndividualKeepaliveProbes << ","sv;
    }
#endif
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ******************** ConnectionOrientedStreamSocket ****************************
 ********************************************************************************
 */
ConnectionOrientedStreamSocket::Ptr ConnectionOrientedStreamSocket::New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol)
{
    return Ptr{make_shared<Rep_> (_Protected::mkLowLevelSocket_ (family, socketKind, protocol))};
}

ConnectionOrientedStreamSocket::Ptr ConnectionOrientedStreamSocket::Attach (PlatformNativeHandle sd)
{
    return Ptr{make_shared<Rep_> (sd)};
}

/*
 ********************************************************************************
 ******************** ConnectionOrientedStreamSocket::Ptr ***********************
 ********************************************************************************
 */
optional<int> ConnectionOrientedStreamSocket::Ptr::GetLinger () const
{
    linger lr = getsockopt<linger> (SOL_SOCKET, SO_LINGER);
    return lr.l_onoff ? lr.l_linger : optional<int>{};
}

void ConnectionOrientedStreamSocket::Ptr::SetLinger (const optional<int>& linger) const
{
    ::linger so_linger{};
    if (linger) {
        so_linger.l_onoff  = true;
        so_linger.l_linger = static_cast<u_short> (*linger);
    }
    setsockopt<::linger> (SOL_SOCKET, SO_LINGER, so_linger);
}

void ConnectionOrientedStreamSocket::Ptr::Write (const Memory::BLOB& b) const
{
    Write (b.begin (), b.end ());
}
