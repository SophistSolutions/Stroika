/*
* Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
*/
#include "../../StroikaPreComp.h"

#include "../../Characters/ToString.h"
#include "../../IO/FileAccessException.h"

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
*  So assume no duel sockets. That seems best --LGP 2017-04-24
*/
namespace {
    constexpr bool kUseDualStackSockets_ = false; // opposite of IPV6_V6ONLY
}

/*
 ********************************************************************************
 ******************************** Network::Socket *******************************
 ********************************************************************************
 */
Socket::PlatformNativeHandle Socket::mkLowLevelSocket_ (SocketAddress::FamilyType family, Socket::Type socketKind, const Optional<IPPROTO>& protocol)
{
#if qPlatform_Windows
    IO::Network::Platform::Windows::WinSock::AssureStarted ();
#endif
    Socket::PlatformNativeHandle sfd;
#if qPlatform_POSIX
    ThrowErrNoIfNegative (sfd = Handle_ErrNoResultInterruption ([=]() -> int { return socket (static_cast<int> (family), static_cast<int> (socketKind), static_cast<int> (protocol.Value ())); }));
#elif qPlatform_Windows
    DISABLE_COMPILER_MSC_WARNING_START (28193) // dump warning about examining sfd
    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (sfd = ::socket (static_cast<int> (family), static_cast<int> (socketKind), static_cast<int> (protocol.Value ())));
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
Socket::Ptr::Ptr (const shared_ptr<_IRep>& rep)
    : fRep_ (rep)
{
}

Socket::Ptr::Ptr (shared_ptr<_IRep>&& rep)
    : fRep_ (std::move (rep))
{
}

Socket::PlatformNativeHandle Socket::Ptr::Detach ()
{
    PlatformNativeHandle h = kINVALID_NATIVE_HANDLE_;
    if (fRep_ != nullptr) {
        h = fRep_->GetNativeSocket ();
    }
    fRep_.reset ();
    return h;
}

Socket::Type Socket::Ptr::GetType () const
{
    return getsockopt<Type> (SOL_SOCKET, SO_TYPE);
}

void Socket::Ptr::Bind (const SocketAddress& sockAddr, BindFlags bindFlags)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"IO::Network::Socket::Bind", L"sockAddr=%s bindFlags.fReUseAddr=%s", Characters::ToString (sockAddr).c_str (), Characters::ToString (bindFlags.fReUseAddr).c_str ())};
    RequireNotNull (fRep_); // Construct with Socket::Kind::SOCKET_STREAM?

    // Indicates that the rules used in validating addresses supplied in a bind(2) call should allow
    // reuse of local addresses. For AF_INET sockets this means that a socket may bind, except when
    // there is an active listening socket bound to the address. When the listening socket is bound
    // to INADDR_ANY with a specific port then it is not possible to bind to this port for any local address.
    setsockopt<int> (SOL_SOCKET, SO_REUSEADDR, bindFlags.fReUseAddr ? 1 : 0);

    sockaddr_storage     useSockAddr = sockAddr.As<sockaddr_storage> ();
    PlatformNativeHandle sfd         = fRep_->GetNativeSocket ();
#if qPlatform_Windows
    ThrowErrNoIfNegative<Socket::PlatformNativeHandle> (::bind (sfd, (sockaddr*)&useSockAddr, sizeof (useSockAddr)));
#else
    // EACCESS reproted as FileAccessException - which is crazy confusing.
    // @todo - find a better way, but for now remap this...
    try {
        ThrowErrNoIfNegative (Handle_ErrNoResultInterruption ([&sfd, &useSockAddr]() -> int { return ::bind (sfd, (sockaddr*)&useSockAddr, sizeof (useSockAddr)); }));
    }
    catch (const IO::FileAccessException&) {
        Throw (StringException (L"Cannot Bind to port"));
    }
#endif
}

bool Socket::Ptr::IsOpen () const
{
    if (fRep_ != nullptr) {
        return fRep_->GetNativeSocket () != kINVALID_NATIVE_HANDLE_;
    }
    return false;
}

/*
 ********************************************************************************
 ******************** Execution::ThrowErrNoIfNegative ***************************
 ********************************************************************************
 */
#if qPlatform_Windows
namespace Stroika {
    namespace Foundation {
        namespace Execution {
            // this specialization needed because the winsock type for SOCKET is UNSIGNED so < 0 test doesn't work
            template <>
            IO::Network::Socket::PlatformNativeHandle ThrowErrNoIfNegative (IO::Network::Socket::PlatformNativeHandle returnCode)
            {
                if (returnCode == kINVALID_NATIVE_HANDLE_) {
                    Execution::Platform::Windows::Exception::Throw (::WSAGetLastError ());
                }
                return returnCode;
            }
        }
    }
}
#endif
