/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/SSL/ServerContext.h"

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::ServerContext {

#if qStroika_HasComponent_OpenSSL

    /**
     */
    struct LibRepType : unique_ptr<::SSL_CTX, decltype (&::SSL_CTX_free)> {
        using inherited = unique_ptr<::SSL_CTX, decltype (&::SSL_CTX_free)>;

        LibRepType (nullptr_t);
        LibRepType (LibRepType&&) = default;
        LibRepType (SSL_CTX* p);
    };

    /**
     */
    struct IRep : Cryptography::SSL::ServerContext::IRep {
        virtual SSL_CTX* Get_SSL_CTX () const = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         *  (1) normal shared_ptr constructors supported
         *  (2) copy from const shared_ptr<IRep>&, to clarify overload avoid ambiguity
         *  (3) shared_ptr<PKI::Certificate::IRep>& - a dynamic_pointer_cast - which only works - which throws if not the right type
         */
        using inherited::inherited;
        Ptr (const shared_ptr<IRep>& p);
        Ptr (const shared_ptr<SSL::ServerContext::IRep>& p);
    };

    struct Options : SSL::ServerContext::Options {
        const SSL_METHOD* fMethod{::TLS_server_method ()};
    };

    /**
     */
    Ptr New (const Options& o);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ServerContext.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_ServerContext_h_*/
