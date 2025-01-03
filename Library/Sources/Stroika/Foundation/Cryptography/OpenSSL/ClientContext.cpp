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

#include "ClientContext.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL
namespace {
    using Cryptography::OpenSSL::ClientContext::Options;
    struct Rep_ : Cryptography::OpenSSL::ClientContext::IRep {
        unique_ptr<SSL_CTX, decltype (&SSL_CTX_free)> fCtx_;

        Rep_ (const Options& o)
            : fCtx_{::SSL_CTX_new (::TLS_client_method ()), ::SSL_CTX_free}
        {
            if (o.fClientCertificate) {
                Cryptography::PKI::Certificate::Ptr clientCert = get<Cryptography::PKI::Certificate::Ptr> (*o.fClientCertificate);
                OpenSSL::Exception::ThrowLastErrorIfFailed (::SSL_CTX_use_certificate (fCtx_.get (), OpenSSL::Certificate::Ptr{clientCert}.Get_X509 ()));
                Cryptography::PKI::PrivateKey::Ptr pkey = get<Cryptography::PKI::PrivateKey::Ptr> (*o.fClientCertificate);
                OpenSSL::Exception::ThrowLastErrorIfFailed (::SSL_CTX_use_PrivateKey (fCtx_.get (), OpenSSL::PrivateKey::Ptr{pkey}.Get_EVP_PKEY ()));
            }
        }
        SSL_CTX* Get_SSL_CTX () const override
        {
            return fCtx_.get ();
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL
auto Cryptography::OpenSSL::ClientContext::New (const Options& o) -> Ptr
{
    return make_shared<Rep_> (o);
}
#endif
