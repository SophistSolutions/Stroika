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
    using LibRepType = unique_ptr<::EVP_PKEY, decltype (&::EVP_PKEY_free)>;

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
         */
        using inherited::inherited;

        // @todo decide if throw or assert???
        Ptr (const shared_ptr<IRep>& p)
            : inherited{p}
        {
        }
        Ptr (const shared_ptr<PKI::PrivateKey::IRep>& p)
        {
            if (auto pp = dynamic_pointer_cast<IRep> (p)) {
                *this = Ptr{pp};
            }
            else {
                throw ("oops");
            }
        }

        EVP_PKEY* Get_EVP_PKEY () const
        {
            return get ()->Get_EVP_PKEY ();
        }
    };

    /**
     */
    Ptr New (LibRepType&& pkey);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_PrivateKey_h_*/
