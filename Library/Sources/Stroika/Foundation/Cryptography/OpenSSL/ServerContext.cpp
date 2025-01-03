/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Cryptography/OpenSSL/Certificate.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/Exception.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/PrivateKey.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "ServerContext.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::PKI;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL
namespace {
    using OpenSSL::ServerContext::Options;
    struct Rep_ : OpenSSL::ServerContext::IRep {
        unique_ptr<SSL_CTX, decltype (&::SSL_CTX_free)> fCtx_;

        Rep_ (const Options& o)
            : fCtx_{SSL_CTX_new (o.fMethod), ::SSL_CTX_free}
        {
            RequireNotNull (get<Cryptography::PKI::Certificate::Ptr> (o.fCertificate));
            OpenSSL::Exception::ThrowLastErrorIfFailed (::SSL_CTX_use_certificate (
                fCtx_.get (), OpenSSL::Certificate::Ptr{get<Cryptography::PKI::Certificate::Ptr> (o.fCertificate)}.Get_X509 ()));
            RequireNotNull (get<Cryptography::PKI::PrivateKey::Ptr> (o.fCertificate));
            OpenSSL::Exception::ThrowLastErrorIfFailed (::SSL_CTX_use_PrivateKey (
                fCtx_.get (), OpenSSL::PrivateKey::Ptr{get<Cryptography::PKI::PrivateKey::Ptr> (o.fCertificate)}.Get_EVP_PKEY ()));
        }
        virtual SSL_CTX* Get_SSL_CTX () const override
        {
            return fCtx_.get ();
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL
auto Cryptography::OpenSSL::ServerContext::New (const Options& o) -> Ptr
{
    return make_shared<Rep_> (o);
}
#endif
