/*
* Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
*/
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/Activity.h"
#include "Stroika/Foundation/Execution/Finally.h"

#include "Socket-Private_.h"

#include "Socket.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Foundation::IO::Network::PRIVATE_;

using Socket::PlatformNativeHandle;

/*
 * Notes:
 *      http://stackoverflow.com/questions/2693709/what-was-the-motivation-for-adding-the-ipv6-v6only-flag
 *  Windows:
 *      https://msdn.microsoft.com/en-us/library/windows/desktop/bb513665(v=vs.85).aspx
 *      Windows Vista and later only
 *
 *  not sure how to handle this best cuz not every OS will support dual-stack (or will it?) 
 *
 *  So assume no dual-stack sockets. That seems best --LGP 2017-04-24
 */
namespace {
    constexpr bool kUseDualStackSockets_ = false; // opposite of IPV6_V6ONLY
}

/*
 ********************************************************************************
 ******************************** Network::Socket *******************************
 ********************************************************************************
 */
Socket::PlatformNativeHandle Socket::_Protected::mkLowLevelSocket_ (SocketAddress::FamilyType family, Socket::Type socketKind,
                                                                    const optional<IPPROTO>& protocol)
{
#if qStroika_Foundation_Common_Platform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
    Socket::PlatformNativeHandle sfd;
#if qStroika_Foundation_Common_Platform_POSIX
    sfd = Handle_ErrNoResultInterruption ([=] () -> int {
        return socket (static_cast<int> (family), static_cast<int> (socketKind), static_cast<int> (NullCoalesce (protocol)));
    });
#elif qStroika_Foundation_Common_Platform_Windows
    DISABLE_COMPILER_MSC_WARNING_START (28193) // dump warning about examining sfd
    ThrowWSASystemErrorIfSOCKET_ERROR (
        sfd = ::socket (static_cast<int> (family), static_cast<int> (socketKind), static_cast<int> (NullCoalesce (protocol))));
    DISABLE_COMPILER_MSC_WARNING_END (28193)
#else
    AssertNotImplemented ();
#endif
    if (family == SocketAddress::FamilyType::INET6) {
        int useIPV6Only = not kUseDualStackSockets_;
#if qStroika_Foundation_Common_Platform_Linux
        // Linux follows the RFC, and uses dual-stack mode by default
        constexpr bool kOSDefaultIPV6Only_{false};
        bool           mustSet = useIPV6Only != kOSDefaultIPV6Only_;
#elif qPlatfom_Windows
        // Windows defaults to NOT dual sockets, so nothing todo for windows
        constexpr bool kOSDefaultIPV6Only_{true};
        bool           mustSet = useIPV6Only != kOSDefaultIPV6Only_;
#else
        bool mustSet = true;
#endif
        if (mustSet) {
            if (::setsockopt (sfd, IPPROTO_IPV6, IPV6_V6ONLY, reinterpret_cast<const char*> (&useIPV6Only), sizeof (useIPV6Only)) < 0) {
                AssertNotReached ();
            }
        }
    }
    return sfd;
}

namespace {
    // Not all operating systems support all flavors of socketpair (e.g. MacOS only supports for AF_UNIX)
    auto mkLowLevelSocketPair_BackCompat_ (SocketAddress::FamilyType family, Socket::Type socketKind,
                                           const optional<IPPROTO>& protocol) -> tuple<PlatformNativeHandle, PlatformNativeHandle>
    {
        // auto connectionOrientedMaster = ConnectionOrientedMasterSocket::New (SocketAddress::FamilyType::INET, Socket::Type::STREAM);
        PlatformNativeHandle masterSocket = Socket::_Protected::mkLowLevelSocket_ (family, socketKind, protocol);
#if qStroika_Foundation_Common_Platform_POSIX
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { ::close (masterSocket); });
#elif qStroika_Foundation_Common_Platform_Windows
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { ::closesocket (masterSocket); });
#endif

        // connectionOrientedMaster.Bind (SocketAddress{IO::Network::V4::kLocalhost});
        sockaddr_storage localhost = (family == SocketAddress::INET or family == SocketAddress::INET6)
                                         ? SocketAddress{LocalHost (family)}.As<sockaddr_storage> ()
                                         : sockaddr_storage{};

#if 0
    {
        int one = 1;
        Verify (::setsockopt (masterSocket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*> (&one), sizeof (one)) == 0) ;
    }
#endif
#if qStroika_Foundation_Common_Platform_POSIX
        Handle_ErrNoResultInterruption (
            [masterSocket, &localhost] () -> int { return ::bind (masterSocket, (sockaddr*)&localhost, sizeof (localhost)); });
#elif qStroika_Foundation_Common_Platform_Windows
        ThrowWSASystemErrorIfSOCKET_ERROR (::bind (masterSocket, (sockaddr*)&localhost, static_cast<int> (sizeof (localhost))));
#endif

        // connectionOrientedMaster.Listen (1);
#if qStroika_Foundation_Common_Platform_POSIX
        Handle_ErrNoResultInterruption ([masterSocket] () -> int { return ::listen (masterSocket, 1); });
#elif qStroika_Foundation_Common_Platform_Windows
        ThrowWSASystemErrorIfSOCKET_ERROR (::listen (masterSocket, 1));
#endif

        // fReadSocket_  = ConnectionOrientedStreamSocket::NewConnection (*connectionOrientedMaster.GetLocalAddress ());
        struct sockaddr masterSocketLocalAddress;
        {
            socklen_t len = sizeof (masterSocketLocalAddress);
            Verify (::getsockname (static_cast<int> (masterSocket), &masterSocketLocalAddress, &len) == 0);
        }
        PlatformNativeHandle    endOne    = Socket::_Protected::mkLowLevelSocket_ (family, socketKind, protocol);
        bool                    succeeded = false;
        [[maybe_unused]] auto&& cleanup2  = Execution::Finally ([&] () noexcept {
            if (not succeeded) {
#if qStroika_Foundation_Common_Platform_POSIX
                ::close (endOne);
#elif qStroika_Foundation_Common_Platform_Windows
                ::closesocket (endOne);
#endif
            }
        });
#if qStroika_Foundation_Common_Platform_POSIX
        Handle_ErrNoResultInterruption ([&] () -> int {
            return ::connect (endOne, (sockaddr*)&masterSocketLocalAddress, static_cast<int> (sizeof (masterSocketLocalAddress)));
        });
#elif qStroika_Foundation_Common_Platform_Windows
        ThrowWSASystemErrorIfSOCKET_ERROR (
            ::connect (endOne, (sockaddr*)&masterSocketLocalAddress, static_cast<int> (sizeof (masterSocketLocalAddress))));
#endif

        // fWriteSocket_ = connectionOrientedMaster.Accept ();
        sockaddr_storage peer{};
        socklen_t        sz = sizeof (peer);
#if qStroika_Foundation_Common_Platform_POSIX
        auto endTwo =
            Handle_ErrNoResultInterruption ([&] () -> int { return ::accept (masterSocket, reinterpret_cast<sockaddr*> (&peer), &sz); });
#elif qStroika_Foundation_Common_Platform_Windows
        auto endTwo = ThrowWSASystemErrorIfSOCKET_ERROR (::accept (masterSocket, reinterpret_cast<sockaddr*> (&peer), &sz));
#endif
        succeeded = true; // so endOne not closed
        return make_tuple (endOne, endTwo);
    }
}

auto Socket::_Protected::mkLowLevelSocketPair_ (SocketAddress::FamilyType family, Socket::Type socketKind,
                                                const optional<IPPROTO>& protocol) -> tuple<PlatformNativeHandle, PlatformNativeHandle>
{
#if qStroika_Foundation_Common_Platform_POSIX
    // docs in https://man7.org/linux/man-pages/man2/socketpair.2.html suggest dont have ot worry about EINTR
    int  sfd[2];
    auto r = ::socketpair (static_cast<int> (family), static_cast<int> (socketKind), static_cast<int> (NullCoalesce (protocol)), sfd);
    if (r == -1 and errno == EOPNOTSUPP) {
        return mkLowLevelSocketPair_BackCompat_ (family, socketKind, protocol);
    }
    ThrowPOSIXErrNo ();
    return make_tuple (sfd[0], sfd[1]);
#elif qStroika_Foundation_Common_Platform_Windows
    return mkLowLevelSocketPair_BackCompat_ (family, socketKind, protocol);
#endif
}

/*
 ********************************************************************************
 ***************************** Network::Socket::Ptr *****************************
 ********************************************************************************
 */
Socket::PlatformNativeHandle Socket::Ptr::Detach ()
{
    Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
    PlatformNativeHandle                                   h = kINVALID_NATIVE_HANDLE_;
    if (fRep_ != nullptr) {
        h = fRep_->Detach ();
    }
    fRep_.reset ();
    return h;
}

Socket::Type Socket::Ptr::GetType () const
{
    Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
    return getsockopt<Type> (SOL_SOCKET, SO_TYPE);
}

void Socket::Ptr::Bind (const SocketAddress& sockAddr, BindFlags bindFlags)
{
    Debug::TraceContextBumper ctx{"IO::Network::Socket::Bind", "sockAddr={} bindFlags.fReUseAddr={}"_f, sockAddr, bindFlags.fSO_REUSEADDR};
    Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{this->_fThisAssertExternallySynchronized};
    RequireNotNull (fRep_); // Construct with Socket::Kind::SOCKET_STREAM?

    auto bindingActivity =
        Execution::LazyEvalActivity{[&] () -> Characters::String { return "binding to "sv + Characters::ToString (sockAddr); }};
    [[maybe_unused]] auto&& declareActivity = Execution::DeclareActivity{&bindingActivity};

    // Indicates that the rules used in validating addresses supplied in a bind(2) call should allow
    // reuse of local addresses. For AF_INET sockets this means that a socket may bind, except when
    // there is an active listening socket bound to the address. When the listening socket is bound
    // to INADDR_ANY with a specific port then it is not possible to bind to this port for any local address.
    setsockopt<int> (SOL_SOCKET, SO_REUSEADDR, bindFlags.fSO_REUSEADDR ? 1 : 0);

    sockaddr_storage     useSockAddr = sockAddr.As<sockaddr_storage> ();
    PlatformNativeHandle sfd         = fRep_->GetNativeSocket ();
    try {
#if qStroika_Foundation_Common_Platform_Windows
        ThrowWSASystemErrorIfSOCKET_ERROR (::bind (sfd, (sockaddr*)&useSockAddr, static_cast<int> (sockAddr.GetRequiredSize ())));
#else
        Handle_ErrNoResultInterruption (
            [sfd, &useSockAddr, &sockAddr] () -> int { return ::bind (sfd, (sockaddr*)&useSockAddr, sockAddr.GetRequiredSize ()); });
#endif
    }
    catch (const Execution::SystemErrorException<>& e) {
        if (e.code () == errc::permission_denied) {
            Throw (SystemErrorException<>{e.code (), e.GetBasicErrorMessage () + "(probably already bound with SO_EXCLUSIVEADDRUSE)"sv});
        }
        else {
            ReThrow ();
        }
    }
    catch (const system_error& e) {
        if (e.code () == errc::permission_denied) {
            Throw (SystemErrorException<>{e.code (), "(probably already bound with SO_EXCLUSIVEADDRUSE)"sv});
        }
        else {
            ReThrow ();
        }
    }
}

bool Socket::Ptr::IsOpen () const
{
    Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
    if (fRep_ != nullptr) {
        return fRep_->GetNativeSocket () != kINVALID_NATIVE_HANDLE_;
    }
    return false;
}

String Socket::Ptr::ToString () const
{
    Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{this->_fThisAssertExternallySynchronized};
    StringBuilder                                         sb;
    if (fRep_ == nullptr) {
        sb << "nullptr"sv;
    }
    else {
        sb << "{"sv;
        sb << "Native-Socket: "sv
           << ((fRep_->GetNativeSocket () == kINVALID_NATIVE_HANDLE_) ? "CLOSED"sv : Characters::ToString (fRep_->GetNativeSocket ())) << ", "sv;
        if (auto ola = GetLocalAddress ()) {
            sb << "Local-Address: "sv << *ola;
        }
        sb << "}"sv;
    }
    return sb;
}
