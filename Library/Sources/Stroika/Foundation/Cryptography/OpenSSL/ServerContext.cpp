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

#include "ServerContext.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL
namespace {
    struct Rep_ : Cryptography::SSL::ServerContext::IRep {
        unique_ptr<SSL_CTX, decltype (&SSL_CTX_free)> fCtx_;

        Rep_ ()
            : fCtx_{SSL_CTX_new (TLS_client_method ()), SSL_CTX_free}   // wrong method and will need args
        {
        }
    };
}
#endif


#if qStroika_HasComponent_OpenSSL
auto Cryptography::OpenSSL::ServerContext::New () -> Ptr
{
    return make_shared < Rep_>();
}
#endif
