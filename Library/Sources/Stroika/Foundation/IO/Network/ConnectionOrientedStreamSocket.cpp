/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../../Characters/StringBuilder.h"
#include "../../Characters/ToString.h"
#include "../../Execution/WaitForIOReady.h"

#include "Socket-Private_.h"

#include "ConnectionOrientedStreamSocket.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using std::byte;

using namespace Stroika::Foundation::IO::Network::PRIVATE_;

namespace {
    struct ConnectionOrientedStreamSocket_IMPL_ : ConnectionOrientedStreamSocket {
        struct Rep_ : BackSocketImpl_<ConnectionOrientedStreamSocket>::Rep_ {
            using inherited = BackSocketImpl_<ConnectionOrientedStreamSocket>::Rep_;
            Rep_ (Socket::PlatformNativeHandle sd)
                : inherited (sd)
            {
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
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                if (fSD_ != kINVALID_NATIVE_HANDLE_ and fAutomaticTCPDisconnectOnClose_) {
                    Shutdown (Socket::ShutdownTarget::eWrites);
                    Time::DurationSecondsType timeOutAt = Time::GetTickCount () + *fAutomaticTCPDisconnectOnClose_;
                    Execution::WaitForIOReady ioReady{fSD_};
                    try {
                    again:
                        ioReady.WaitUntil (timeOutAt);
                        char data[1024];
#if qPlatform_POSIX
                        int nb = ::read (fSD_, data, NEltsOf (data));
#elif qPlatform_Windows
                        int flags = 0;
                        int nb    = ::recv (fSD_, data, (int)NEltsOf (data), flags);
#endif
                        if (nb > 0) {
                            DbgTrace (L"Warning: %d unread bytes to be read on socket when it was closed.", nb); // SHOULD READ ZERO AFTER SHUTDOWN to indicate client other side of connection handled the close
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
            virtual void Connect (const SocketAddress& sockAddr) const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                sockaddr_storage                                    useSockAddr = sockAddr.As<sockaddr_storage> ();
#if qPlatform_POSIX
                ThrowPOSIXErrNoIfNegative (Handle_ErrNoResultInterruption ([&]() -> int { return ::connect (fSD_, (sockaddr*)&useSockAddr, sizeof (useSockAddr)); }));
#elif qPlatform_Windows
                ThrowPOSIXErrNoIfNegative<Socket::PlatformNativeHandle> (::connect (fSD_, (sockaddr*)&useSockAddr, sizeof (useSockAddr)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual size_t Read (byte* intoStart, byte* intoEnd) const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};

                Assert (fCurrentPendingReadsCount++ == 0);
#if qDebug
                [[maybe_unused]] auto&& cleanup = Finally ([=]() noexcept { Assert (--fCurrentPendingReadsCount == 0); });
#endif

#if qPlatform_POSIX
                return ThrowPOSIXErrNoIfNegative (Handle_ErrNoResultInterruption ([this, &intoStart, &intoEnd]() -> int { return ::read (fSD_, intoStart, intoEnd - intoStart); }));
#elif qPlatform_Windows
                int flags = 0;
                int nBytesToRead = static_cast<int> (min<size_t> ((intoEnd - intoStart), numeric_limits<int>::max ()));
                return static_cast<size_t> (ThrowPOSIXErrNoIfNegative<Socket::PlatformNativeHandle> (::recv (fSD_, reinterpret_cast<char*> (intoStart), nBytesToRead, flags)));
#else
                AssertNotImplemented ();
#endif
            }
            virtual optional<size_t> ReadNonBlocking (byte* intoStart, byte* intoEnd) const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                Assert (fCurrentPendingReadsCount++ == 0);
#if qDebug
                [[maybe_unused]] auto&& cleanup = Finally ([=]() noexcept { Assert (--fCurrentPendingReadsCount == 0); });
#endif
#if qPlatform_POSIX || qPlatform_Windows
                {
                    fd_set input;
                    FD_ZERO (&input);
                    FD_SET (fSD_, &input);
                    struct timeval timeout {
                    };
                    if (::select (static_cast<int> (fSD_) + 1, &input, NULL, NULL, &timeout) == 1) {
                        if (intoStart == nullptr) {
                            // don't know how much, but doesn't matter, since read allows returning just one byte if thats all thats available
                            // But MUST check if is EOF or real data available
                            char buf[1024];
#if qPlatform_POSIX
                            int tmp = ThrowPOSIXErrNoIfNegative (Handle_ErrNoResultInterruption ([&]() -> int { return ::recv (fSD_, buf, NEltsOf (buf), MSG_PEEK); }));
#elif qPlatform_Windows
                            int tmp = ThrowPOSIXErrNoIfNegative<int> (::recv (fSD_, buf, static_cast<int> (NEltsOf (buf)), MSG_PEEK));
#else
                            AssertNotImplemented ();
#endif
                            return tmp;
                        }
#if qDebug
                        --fCurrentPendingReadsCount; // reverse for inherited Read ()
                        [[maybe_unused]] auto&& cleanup2 = Finally ([=]() noexcept { ++fCurrentPendingReadsCount; });
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
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::Network::Socket...rep...::Write", L"end-start=%lld", static_cast<long long> (end - start))};
#endif
#if qPlatform_POSIX
                /*
                 *  https://linux.die.net/man/2/write says "writes up to count bytes". So handle case where we get partial writes.
                 *  Actually, for most of the cases called out, we cannot really continue anyhow, so this maybe pointless, but the
                 *  docs aren't fully clear, so play it safe --LGP 2017-04-13
                 */
                BreakWriteIntoParts_<byte> (
                    start,
                    end,
                    numeric_limits<int>::max (),
                    [this](const byte* start, const byte* end) -> size_t {
                        Assert ((end - start) < numeric_limits<int>::max ());
                        int     len = static_cast<int> (end - start);
                        ssize_t n   = Handle_ErrNoResultInterruption ([this, &start, &end]() -> ssize_t { return ::write (fSD_, start, end - start); });
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
                BreakWriteIntoParts_<byte> (
                    start,
                    end,
                    maxSendAtATime,
                    [this, maxSendAtATime](const byte* start, const byte* end) -> size_t {
                        Require (static_cast<size_t> (end - start) <= maxSendAtATime);
                        Assert ((end - start) < numeric_limits<int>::max ());
                        int len = static_cast<int> (end - start);
                        int flags = 0;
                        int n = ::send (fSD_, reinterpret_cast<const char*> (start), len, flags);
                        ThrowPOSIXErrNoIfNegative<Socket::PlatformNativeHandle> (n);
                        Assert (0 <= n and n <= (end - start));
                        return static_cast<size_t> (n);
                    });
#else
                AssertNotImplemented ();
#endif
            }
            virtual optional<IO::Network::SocketAddress> GetPeerAddress () const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                struct sockaddr_storage                             radr;
                socklen_t                                           len = sizeof (radr);
                if (::getpeername (static_cast<int> (fSD_), (struct sockaddr*)&radr, &len) == 0) {
                    IO::Network::SocketAddress sa{radr};
                    return sa;
                }
                return nullopt;
            }
            virtual optional<Time::DurationSecondsType> GetAutomaticTCPDisconnectOnClose () const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                return fAutomaticTCPDisconnectOnClose_;
            }
            virtual void SetAutomaticTCPDisconnectOnClose (const optional<Time::DurationSecondsType>& waitFor) override
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                fAutomaticTCPDisconnectOnClose_ = waitFor;
            }
            virtual KeepAliveOptions GetKeepAlives () const override
            {
                shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
                KeepAliveOptions                                    result;
                result.fEnabled = !!getsockopt<int> (SOL_SOCKET, SO_KEEPALIVE);
#if qPlatform_Linux
                // Only available if linux >= 2.4
                result.fMaxProbesSentBeforeDrop              = !!getsockopt<int> (SOL_TCP, TCP_KEEPCNT);
                result.fTimeIdleBeforeSendingKeepalives      = !!getsockopt<int> (SOL_TCP, TCP_KEEPIDLE);
                result.fTimeBetweenIndividualKeepaliveProbes = !!getsockopt<int> (SOL_TCP, TCP_KEEPINTVL);
#elif qPlatform_Windows
// WSAIoctl (..., SIO_KEEPALIVE_VALS) can be used to set some of these values, but I can find no way
// to fetch them --LGP 2017-02-27
#endif
                return result;
            }
            virtual void SetKeepAlives (const KeepAliveOptions& keepAliveOptions) override
            {
                lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
                setsockopt<int> (SOL_SOCKET, SO_KEEPALIVE, keepAliveOptions.fEnabled);
#if qPlatform_Linux
                // Only available if linux >= 2.4
                if (keepAliveOptions.fMaxProbesSentBeforeDrop) {
                    setsockopt<int> (SOL_TCP, TCP_KEEPCNT, *keepAliveOptions.fMaxProbesSentBeforeDrop);
                }
                if (keepAliveOptions.fTimeIdleBeforeSendingKeepalives) {
                    setsockopt<int> (SOL_TCP, TCP_KEEPIDLE, *keepAliveOptions.fTimeIdleBeforeSendingKeepalives);
                }
                if (keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes) {
                    setsockopt<int> (SOL_TCP, TCP_KEEPINTVL, *keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes);
                }
#elif qPlatform_Windows
                // windows only allows setting these two, and both at the same time
                if (keepAliveOptions.fEnabled and (keepAliveOptions.fTimeIdleBeforeSendingKeepalives or keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes)) {
                    tcp_keepalive alive{keepAliveOptions.fEnabled};
                    // from https://msdn.microsoft.com/en-us/library/windows/desktop/dd877220(v=vs.85).aspx - "The default settings when a TCP socket is initialized sets the keep-alive timeout to 2 hours and the keep-alive interval to 1 second"
                    alive.keepalivetime = Math::Round<ULONG> (keepAliveOptions.fTimeIdleBeforeSendingKeepalives.value_or (2 * 60 * 60) * 1000.0);
                    alive.keepaliveinterval = Math::Round<ULONG> (keepAliveOptions.fTimeBetweenIndividualKeepaliveProbes.value_or (1) * 1000.0);
                    DWORD dwBytesRet{};
                    if (::WSAIoctl (fSD_, SIO_KEEPALIVE_VALS, &alive, sizeof (alive), NULL, 0, &dwBytesRet, NULL, NULL) == SOCKET_ERROR) {
                        Execution::ThrowSystemErrNo (::WSAGetLastError ());
                    }
                }
#endif
            }
            optional<Time::DurationSecondsType> fAutomaticTCPDisconnectOnClose_;
#if qDebug
            mutable atomic<int> fCurrentPendingReadsCount{};
#endif
        };
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
    sb += L"{";
    sb += L"Enabled: " + Characters::ToString (fEnabled) + L",";
#if qPlatform_Linux or qPlatform_Windows
    if (fMaxProbesSentBeforeDrop) {
        sb += L"Max-Probes-Sent-Before-Drop: " + Characters::ToString (fMaxProbesSentBeforeDrop) + L",";
    }
    if (fTimeIdleBeforeSendingKeepalives) {
        sb += L"Time-Idle-Before-Sending-Keepalives: " + Characters::ToString (fTimeIdleBeforeSendingKeepalives) + L",";
    }
    if (fTimeBetweenIndividualKeepaliveProbes) {
        sb += L"Time-Between-Individual-Keepalive-Probes: " + Characters::ToString (fTimeBetweenIndividualKeepaliveProbes) + L",";
    }
#endif
    sb += L"}";
    return sb.str ();
}

/*
 ********************************************************************************
 ******************** ConnectionOrientedStreamSocket ****************************
 ********************************************************************************
 */
ConnectionOrientedStreamSocket::Ptr ConnectionOrientedStreamSocket::New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol)
{
    return Ptr{make_shared<ConnectionOrientedStreamSocket_IMPL_::Rep_> (mkLowLevelSocket_ (family, socketKind, protocol))};
}

ConnectionOrientedStreamSocket::Ptr ConnectionOrientedStreamSocket::Attach (PlatformNativeHandle sd)
{
    return Ptr{make_shared<ConnectionOrientedStreamSocket_IMPL_::Rep_> (sd)};
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
