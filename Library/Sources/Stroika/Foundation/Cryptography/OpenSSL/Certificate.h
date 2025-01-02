/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_Certificate_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_Certificate_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <memory>

#if qStroika_HasComponent_OpenSSL
#include <openssl/ssl.h>
#include <openssl/x509.h>
#endif

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Cryptography/Certificate.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/PrivateKey.h"

namespace Stroika::Foundation::Cryptography::OpenSSL::Certificate {

    /**
     */
    struct IRep : Cryptography::Certificate::IRep {
        virtual X509* Get_X509 () const = 0;
    };

    /**
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         */
        using inherited::inherited;

        Ptr (unique_ptr<::X509, decltype (&::X509_free)>&& p);
        Ptr (Cryptography::Certificate::Ptr p)
        {
            if (auto pp = dynamic_pointer_cast<IRep> (p)) {
                *this = Ptr{pp};
            }
            else {
                throw ("oops");
            }
        }

        X509* Get_X509 () const
        {
            return get ()->Get_X509 ();
        }
    };

    /**
     *  \brief Construct a Certificate object (for now just supported from PEMFile)
     */
    Ptr New (const PEMFile& pemFile);

    /**
     *  \brief generate a new self-signed certificate (and private key)
     *  \see https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl
     */
    tuple<OpenSSL::PrivateKey::Ptr, Ptr> NewSelfSigned ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_Certificate_h_*/
