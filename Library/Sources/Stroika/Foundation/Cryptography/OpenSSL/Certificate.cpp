/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "Certificate.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL
namespace {
    struct Rep_ : Cryptography::OpenSSL::Certificate::IRep {

        Rep_ (Memory::BLOB pubKey, Memory::BLOB privateKey)
        {
        }

        virtual const X509* Get_X509 () const override
        {
            return nullptr;
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL
auto Cryptography::OpenSSL::Certificate::New (Memory::BLOB pubKey, Memory::BLOB privateKey) -> Ptr
{
    return make_shared<Rep_> (pubKey, privateKey);
}
#endif
