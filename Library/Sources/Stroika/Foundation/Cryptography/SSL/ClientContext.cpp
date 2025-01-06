/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/ClientContext.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif
#include "ClientContext.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Providers;
#if !qStroika_HasComponent_OpenSSL
using namespace Stroika::Foundation::Execution;
#endif

/*
 ********************************************************************************
 ************************ Cryptography::SSL::ClientContext **********************
 ********************************************************************************
 */
auto Cryptography::SSL::ClientContext::New ([[maybe_unused]] const Options& options) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return OpenSSL::ClientContext::New (OpenSSL::ClientContext::Options{options});
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
