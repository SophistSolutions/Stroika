/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/ServerContext.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif
#include "ServerContext.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
#if qStroika_HasComponent_OpenSSL
using namespace Stroika::Foundation::Cryptography::Providers;
#else
using namespace Stroika::Foundation::Execution;
#endif

/*
 ********************************************************************************
 ************************ Cryptography::SSL::ServerContext **********************
 ********************************************************************************
 */
auto Cryptography::SSL::ServerContext::New ([[maybe_unused]] const Options& o) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return OpenSSL::ServerContext::New (OpenSSL::ServerContext::Options{o});
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
