/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "PrivateKey.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL
namespace {
    struct Rep_ : Cryptography::OpenSSL::PrivateKey::IRep {

        EVP_PKEY* fKey_{nullptr};

        Rep_ (const PEMFile& pem)
        {
            auto d  = pem.fData.As<span<const uint8_t>> ();
            auto dd = d.data ();
            fKey_   = ::b2i_PrivateKey (&dd, static_cast<long> (d.size ())); // cannot find docs on this API
        }
        ~Rep_ ()
        {
            ::EVP_PKEY_free (fKey_);
        }
        virtual EVP_PKEY* Get_EVP_PKEY () const override
        {
            return fKey_;
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL
auto Cryptography::OpenSSL::PrivateKey::New (const PEMFile& pem) -> Ptr
{
    return make_shared<Rep_> (pem);
}
#endif
