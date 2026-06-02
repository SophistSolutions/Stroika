/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Foundation_Common_Platform_POSIX
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
#include "Stroika/Foundation/Memory/BlockAllocated.h"

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
                    qStroika_ATTRIBUTE_INDETERMINATE char data[1024];
#if qStroika_Foundation_Common_Platform_POSIX
                    int nb = ::read (fSD_, data, std::size (data));
#elif qStroika_Foundation_Common_Platform_Windows
                    int flags = 0;
                    int nb    = ::recv (fSD_, data, (int)std::size (data), flags);
#endif
                    if (nb > 0) {
                        DbgTrace ("Warning: {} unread bytes to be read on socket when it was closed."_f,
                                  nb); // SHOULD READ ZERO AFTER SHUTDOWN to indicate client other side of connection handled the close
                        goto again;
                    }
                }
                catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("timeout closing down socket - not serious - just means client didn't send close ACK quickly enough"_f);
#endif
                }
            }
            inherited::Close ();
        }
        nonvirtual void Connect_Sync_ (const SocketAddress& sockAddr) const
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            sockaddr_storage                               useSockAddr = sockAddr.As<sockaddr_storage> ();
#if qStroika_Foundation_Common_Platform_POSIX
            Handle_ErrNoResultInterruption ([&] () -> int { return ::connect (fSD_, (sockaddr*)&useSockAddr, sockAddr.GetRequiredSize ()); });
#elif qStroika_Foundation_Common_Platform_Windows
            ThrowWSASystemErrorIfSOCKET_ERROR (::connect (fSD_, (sockaddr*)&useSockAddr, static_cast<int> (sockAddr.GetRequiredSize ())));
#else
            AssertNotImplemented ();
#endif
        }
        nonvirtual void Connect_AsyncWTimeout_ (const SocketAddress& sockAddr, const Time::Duration& timeout) const
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            sockaddr_storage                               useSockAddr = sockAddr.As<sockaddr_storage> ();
#if qStroika_Foundation_Common_Platform_POSIX
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
                        qStroika_ATTRIBUTE_INDETERMINATE fd_set myset;
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
#elif qStroika_Foundation_Common_Platform_Windows
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
                qStroika_ATTRIBUTE_INDETERMINATE fd_set setW;
                FD_ZERO (&setW);
                FD_SET (fSD_, &setW);
                qStroika_ATTRIBUTE_INDETERMINATE fd_set setE;
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
        virtual span<byte> Read (span<byte> into) const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};

#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (fCurrentPendingReadsCount++ == 0);
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { Assert (--fCurrentPendingReadsCount == 0); });
#endif
            if (fReadEOF_) {
                return span<byte>{}; // EOF
            }

#if qStroika_Foundation_Common_Platform_POSIX
            auto result = into.subspan (
                0, Handle_ErrNoResultInterruption ([this, &into] () -> int { return ::read (fSD_, into.data (), into.size ()); }));
#elif qStroika_Foundation_Common_Platform_Windows
            int  flags        = 0;
            int  nBytesToRead = static_cast<int> (min<size_t> (into.size (), numeric_limits<int>::max ()));
            auto result       = into.subspan (0, static_cast<size_t> (ThrowWSASystemErrorIfSOCKET_ERROR (
                                               ::recv (fSD_, reinterpret_cast<char*> (into.data ()), nBytesToRead, flags))));
#else
            AssertNotImplemented ();
            span<byte> result{};
#endif
            if (result.empty ()) {
                fReadEOF_ = true;
            }
            return result;
        }
        virtual optional<span<byte>> ReadNonBlocking (span<byte> into) const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
            if (AvailableToRead ().has_value ()) {
                return Read (into);
            }
            return nullopt;
        }
        virtual optional<size_t> AvailableToRead () const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (fCurrentPendingReadsCount++ == 0);
            [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { Assert (--fCurrentPendingReadsCount == 0); });
#endif
            if (fReadEOF_) {
                return 0;
            }
#if qStroika_Foundation_Common_Platform_POSIX or qStroika_Foundation_Common_Platform_Windows
            {
                qStroika_ATTRIBUTE_INDETERMINATE fd_set input;
                FD_ZERO (&input);
                FD_SET (fSD_, &input);
                struct timeval timeout{};
                if (::select (static_cast<int> (fSD_) + 1, &input, NULL, NULL, &timeout) == 1) {
                    // don't know how much, but doesn't matter, since read allows returning just one byte if thats all thats available
                    // But MUST check if is EOF or real data available
                    char buf[1024];
#if qStroika_Foundation_Common_Platform_POSIX
                    int tmp = Handle_ErrNoResultInterruption ([&] () -> int { return ::recv (fSD_, buf, std::size (buf), MSG_PEEK); });
#elif qStroika_Foundation_Common_Platform_Windows
                    int tmp = ThrowWSASystemErrorIfSOCKET_ERROR (::recv (fSD_, buf, static_cast<int> (std::size (buf)), MSG_PEEK));
#else
                    AssertNotImplemented ();
#endif
                    return tmp;
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
        virtual void Write (span<const byte> data) const override
        {
            AssertExternallySynchronizedMutex::ReadContext declareContext{this->fThisAssertExternallySynchronized};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{
                Stroika_Foundation_Debug_OptionalizeTraceArgs ("IO::Network::Socket...rep...::Write", "lwn={}"_f, data.size ())};
#endif
#if qStroika_Foundation_Common_Platform_POSIX
            /*
             *  https://linux.die.net/man/2/write says "writes up to count bytes". So handle case where we get partial writes.
             *  Actually, for most of the cases called out, we cannot really continue anyhow, so this maybe pointless, but the
             *  docs aren't fully clear, so play it safe --LGP 2017-04-13
             */
            BreakWriteIntoParts_<byte> (data, numeric_limits<int>::max (), [this] (span<const byte> data) -> size_t {
                Assert (data.size () < numeric_limits<int>::max ());
                ssize_t n =
                    Handle_ErrNoResultInterruption ([this, &data] () -> ssize_t { return ::write (fSD_, data.data (), data.size ()); });
                ThrowPOSIXErrNoIfNegative (n);
                Assert (0 <= n and n <= data.size ());
                return static_cast<size_t> (n);
            });
#elif qStroika_Foundation_Common_Platform_Windows
            /*
             *  Note sure what the best way is here, but with WinSock, you cannot use write() directly. Sockets are not
             *  file descriptors in windows implementation.
             *      WONT WORK:
             *          int       n   =   ::_write (fSD_, start, end - start);
             */
            size_t maxSendAtATime = getsockopt<unsigned int> (SOL_SOCKET, SO_MAX_MSG_SIZE);
            BreakWriteIntoParts_<byte> (data, maxSendAtATime, [this, maxSendAtATime] (span<const byte> data) -> size_t {
                Require (data.size () <= maxSendAtATime);
                Assert (data.size () < static_cast<size_t> (numeric_limits<int>::max ()));
                int len   = static_cast<int> (data.size ());
                int flags = 0;
                int n     = ThrowWSASystemErrorIfSOCKET_ERROR (::send (fSD_, reinterpret_cast<const char*> (data.data ()), len, flags));
                Assert (0 <= n and static_cast<size_t> (n) <= data.size ());
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
#if qStroika_Foundation_Common_Platform_Linux
            // Only available if linux >= 2.4
            result.fMaxProbesSentBeforeDrop              = getsockopt<int> (SOL_TCP, TCP_KEEPCNT);
            result.fTimeIdleBeforeSendingKeepalives      = Time::DurationSeconds{getsockopt<int> (SOL_TCP, TCP_KEEPIDLE)};
            result.fTimeBetweenIndividualKeepaliveProbes = Time::DurationSeconds{getsockopt<int> (SOL_TCP, TCP_KEEPINTVL)};
#elif qStroika_Foundation_Common_Platform_Windows
// WSAIoctl (..., SIO_KEEPALIVE_VALS) can be used to set some of these values, but I can find no way
// to fetch them --LGP 2017-02-27
#endif
            return result;
        }
        virtual void SetKeepAlives (const KeepAliveOptions& keepAliveOptions) override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{this->fThisAssertExternallySynchronized};
            setsockopt<int> (SOL_SOCKET, SO_KEEPALIVE, keepAliveOptions.fEnabled);
#if qStroika_Foundation_Common_Platform_Linux
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
#elif qStroika_Foundation_Common_Platform_Windows
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
        mutable bool                    fReadEOF_{false};
#if qStroika_Foundation_Debug_AssertionsChecked
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
    sb << "enabled: "sv << fEnabled;
#if qStroika_Foundation_Common_Platform_Linux or qStroika_Foundation_Common_Platform_Windows
    if (fMaxProbesSentBeforeDrop) {
        sb << ", maxProbesSentBeforeDrop: "sv << fMaxProbesSentBeforeDrop;
    }
    if (fTimeIdleBeforeSendingKeepalives) {
        sb << ", timeIdleBeforeSendingKeepalives: "sv << fTimeIdleBeforeSendingKeepalives;
    }
    if (fTimeBetweenIndividualKeepaliveProbes) {
        sb << ", timeBetweenIndividualKeepaliveProbes: "sv << fTimeBetweenIndividualKeepaliveProbes;
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
    return Ptr{Memory::MakeSharedPtr<Rep_> (_Protected::mkLowLevelSocket_ (family, socketKind, protocol))};
}

ConnectionOrientedStreamSocket::Ptr ConnectionOrientedStreamSocket::Attach (PlatformNativeHandle sd)
{
    return Ptr{Memory::MakeSharedPtr<Rep_> (sd)};
}

#include "ConnectionOrientedMasterSocket.h" //tmphack for kLowLevelSocketPairWorks_==false
auto ConnectionOrientedStreamSocket::NewPair (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol) -> tuple<Ptr, Ptr>
{
    constexpr bool kLowLevelSocketPairWorks_{true}; // for now only works on windows
    if constexpr (kLowLevelSocketPairWorks_) {
        auto sp = _Protected::mkLowLevelSocketPair_ (family, socketKind, protocol);
        return make_tuple (Attach (get<0> (sp)), Attach (get<1> (sp)));
    }
    else {
        // Create a Listening master socket, bind it, and get it listening
        // Just needed temporarily to create the socketpair, then it can be closed when it goes out of scope
        auto connectionOrientedMaster = ConnectionOrientedMasterSocket::New (family, socketKind, protocol);
        connectionOrientedMaster.Bind (SocketAddress{LocalHost (family)});
        connectionOrientedMaster.Listen (1);

        // now make a NEW socket, with the bound address and connect;
        auto one = ConnectionOrientedStreamSocket::NewConnection (*connectionOrientedMaster.GetLocalAddress ());
        auto two = connectionOrientedMaster.Accept ();
        return make_tuple (one, two);
    }
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
