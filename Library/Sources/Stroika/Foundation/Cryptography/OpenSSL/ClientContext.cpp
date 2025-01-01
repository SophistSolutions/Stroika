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
            : fCtx_{SSL_CTX_new (TLS_client_method ()), SSL_CTX_free}
        {
            if (o.fClientCertificate) {
                // Load client certificate and key (if required)
                // if (SSL_CTX_use_certificate_file(ctx.get(), "client.crt", SSL_FILETYPE_PEM) <= 0) {
                //     std::cerr << "Error loading client certificate: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
                //     return 1;
                // }
                // if (SSL_CTX_use_PrivateKey_file(ctx.get(), "client.key", SSL_FILETYPE_PEM) <= 0) {
                //     std::cerr << "Error loading client private key: " << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
                //     return 1;
                // }
            }
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
