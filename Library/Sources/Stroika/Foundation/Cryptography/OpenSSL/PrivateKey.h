/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_PrivateKey_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_PrivateKey_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/ssl.h>
#include <openssl/x509.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/PrivateKey.h"

namespace Stroika::Foundation::Cryptography::OpenSSL::PrivateKey {

    /**
     */
    struct IRep : Cryptography::PrivateKey::IRep {
        virtual EVP_PKEY* Get_EVP_PKEY () const = 0;
    };

    /**
    */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;

        Ptr (Cryptography::PrivateKey::Ptr p)
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

    // @todo VERY rough - needs optional private key, and TYPE info
    Ptr New (const PEMFile& pem);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_PrivateKey_h_*/
