/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include "Stroika/Foundation/Cryptography/OpenSSL/PrivateKey.h"
#else
#include "Stroika/Foundation/Execution/RequiredComponentMissingException.h"
#endif
#include "PrivateKey.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
#if !qStroika_HasComponent_OpenSSL
using namespace Stroika::Foundation::Execution;
#endif

/*
 ********************************************************************************
 *********************** Cryptography::PrivateKey::Ptr **************************
 ********************************************************************************
 */
auto Cryptography::PrivateKey::Ptr::ToString () const -> String
{
    return String{}; //tmphack
}
#if 0
/*
 ********************************************************************************
 **************************** Cryptography::PrivateKey **************************
 ********************************************************************************
 */
auto Cryptography::PrivateKey::New (const PEMFile& pem) -> Ptr
{
#if qStroika_HasComponent_OpenSSL
    return Cryptography::OpenSSL::PrivateKey::New (pem);
#else
    Throw (RequiredComponentMissingException{"SSL providing service"sv});
#endif
}
#endif