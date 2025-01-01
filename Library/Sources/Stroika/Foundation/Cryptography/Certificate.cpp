/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/OpenSSL/Certificate.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif
#include "Certificate.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
#if !qStroika_HasComponent_OpenSSL
using namespace Stroika::Foundation::Execution;
#endif

/*
 ********************************************************************************
 **************************** Cryptography::Certificate *************************
 ********************************************************************************
 */
auto Cryptography::Certificate::New () -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return Cryptography::OpenSSL::Certificate::New ();
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
