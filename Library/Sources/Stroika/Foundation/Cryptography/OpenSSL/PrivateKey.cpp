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

        using EVP_KEY_UPTR_ = unique_ptr<::EVP_PKEY, decltype (&::EVP_PKEY_free)>;
        EVP_KEY_UPTR_ fKey_;

        Rep_ ()            = delete;
        Rep_ (const Rep_&) = delete;
        Rep_ (Rep_&&)      = default;
        Rep_ (EVP_KEY_UPTR_&& p)
            : fKey_{move (p)}
        {
        }
        virtual EVP_PKEY* Get_EVP_PKEY () const override
        {
            return fKey_.get ();
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL
auto Cryptography::OpenSSL::PrivateKey::New (unique_ptr<::EVP_PKEY, decltype (&::EVP_PKEY_free)>&& p) -> Ptr
{
    return make_shared<Rep_> (move (p));
}
#endif
