/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_ClientContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_ClientContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/SSL/ClientContext.h"

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::ClientContext {

#if qStroika_HasComponent_OpenSSL

    /**
     *  \brief openssl library 'unique_ptr' representation of a ssl client context
     */
    struct LibRepType : unique_ptr<::SSL_CTX, decltype (&::SSL_CTX_free)> {
        using inherited = unique_ptr<::SSL_CTX, decltype (&::SSL_CTX_free)>;

        LibRepType (nullptr_t);
        LibRepType (LibRepType&&) = default;
        LibRepType (SSL_CTX* p);
    };

    /**
     */
    struct IRep : SSL::ClientContext::IRep {
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
        Ptr (const shared_ptr<SSL::ClientContext::IRep>& p);
    };

    struct Options : SSL::ClientContext::Options {};

    /**
     */
    Ptr New (const Options& o = {});
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include "ClientContext.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_ClientContext_h_*/
