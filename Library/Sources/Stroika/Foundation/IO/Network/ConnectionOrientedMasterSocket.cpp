/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/ToString.h"

#include "Socket-Private_.h"

#include "Stroika/Foundation/IO/Network/ConnectionOrientedMasterSocket.h"

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::IO;
using namespace Stroika::Foundation::IO::Network;

using namespace Stroika::Foundation::IO::Network::PRIVATE_;

namespace {
    struct Rep_ : BackSocketImpl_<ConnectionOrientedMasterSocket::_IRep> {
        using inherited = BackSocketImpl_<ConnectionOrientedMasterSocket::_IRep>;
        Rep_ (Socket::PlatformNativeHandle sd)
            : inherited{sd}
        {
        }
        virtual void Listen (unsigned int backlog) override
        {
            Debug::TraceContextBumper                       ctx{"IO::Network::Socket::Listen", "backlog={}"_f, backlog};
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized};
#if qPlatform_POSIX
            Handle_ErrNoResultInterruption ([this, &backlog] () -> int { return ::listen (fSD_, backlog); });
#elif qPlatform_Windows
            ThrowWSASystemErrorIfSOCKET_ERROR (::listen (fSD_, backlog));
#else
            AssertNotImplemented ();
#endif
        }
        virtual ConnectionOrientedStreamSocket::Ptr Accept () override
        {
            AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized};
            sockaddr_storage                                peer{};
            socklen_t                                       sz = sizeof (peer);
#if qPlatform_POSIX
            return ConnectionOrientedStreamSocket::Attach (
                Handle_ErrNoResultInterruption ([&] () -> int { return ::accept (fSD_, reinterpret_cast<sockaddr*> (&peer), &sz); }));
#elif qPlatform_Windows
            return ConnectionOrientedStreamSocket::Attach (
                ThrowWSASystemErrorIfSOCKET_ERROR (::accept (fSD_, reinterpret_cast<sockaddr*> (&peer), &sz)));
#else
            AssertNotImplemented ();
#endif
        }
    };
}

/*
 ********************************************************************************
 ************************ ConnectionOrientedMasterSocket ************************
 ********************************************************************************
 */
ConnectionOrientedMasterSocket::Ptr ConnectionOrientedMasterSocket::New (SocketAddress::FamilyType family, Type socketKind, const optional<IPPROTO>& protocol)
{
    return Ptr{make_shared<Rep_> (_Protected::mkLowLevelSocket_ (family, socketKind, protocol))};
}

ConnectionOrientedMasterSocket::Ptr ConnectionOrientedMasterSocket::Attach (PlatformNativeHandle sd)
{
    return Ptr{make_shared<Rep_> (sd)};
}
