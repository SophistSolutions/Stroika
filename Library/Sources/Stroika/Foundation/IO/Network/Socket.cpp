/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Execution/Activity.h"

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
#if qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
    Socket::PlatformNativeHandle sfd;
#if qPlatform_POSIX
    sfd = Handle_ErrNoResultInterruption ([=] () -> int {
        return socket (static_cast<int> (family), static_cast<int> (socketKind), static_cast<int> (NullCoalesce (protocol)));
    });
#elif qPlatform_Windows
    DISABLE_COMPILER_MSC_WARNING_START (28193) // dump warning about examining sfd
    ThrowWSASystemErrorIfSOCKET_ERROR (
        sfd = ::socket (static_cast<int> (family), static_cast<int> (socketKind), static_cast<int> (NullCoalesce (protocol))));
    DISABLE_COMPILER_MSC_WARNING_END (28193)
#else
    AssertNotImplemented ();
#endif
    if (family == SocketAddress::FamilyType::INET6) {
        int useIPV6Only = not kUseDualStackSockets_;
#if qPlatform_Linux
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
#if qPlatform_Windows
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
