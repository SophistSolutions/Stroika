/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_PrivateKey_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_PrivateKey_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/PKI/PrivateKey.h"

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::PrivateKey {

#if qStroika_HasComponent_OpenSSL
    /**
     */
    struct LibRepType : unique_ptr<::EVP_PKEY, decltype (&::EVP_PKEY_free)> {
        using inherited = unique_ptr<::EVP_PKEY, decltype (&::EVP_PKEY_free)>;

        LibRepType (nullptr_t);
        LibRepType (LibRepType&&) = default;
        LibRepType (EVP_PKEY* p);
    };

    /**
     */
    struct IRep : Cryptography::PKI::PrivateKey::IRep {
        virtual EVP_PKEY* Get_EVP_PKEY () const = 0;
    };

    /**
    */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         *  (1) normal shared_ptr constructors supported
         *  (2) copy from const shared_ptr<IRep>&, to clarify overload avoid ambiguity
         *  (3) shared_ptr<PKI::PrivateKey::IRep>& - a dynamic_pointer_cast - which only works - which throws if not the right type
         */
        using inherited::inherited;
        Ptr (const shared_ptr<IRep>& p);
        Ptr (const shared_ptr<PKI::PrivateKey::IRep>& p);
        EVP_PKEY* Get_EVP_PKEY () const;
    };

    /**
     *  \brief Construct a PrivateKey object
     *      (1) from an adopted unique_ptr to the underlying library object
     */
    Ptr New (LibRepType&& pkey);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "PrivateKey.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_PrivateKey_h_*/
