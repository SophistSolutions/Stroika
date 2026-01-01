/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
#include "Stroika/Foundation/Cryptography/PKI/Certificate.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/PrivateKey.h"

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::Certificate {

    using namespace Cryptography::PKI::Certificate;

#if qStroika_HasComponent_OpenSSL
    /**
     */
    struct LibRepType : unique_ptr<::X509, decltype (&::X509_free)> {
        using inherited = unique_ptr<::X509, decltype (&::X509_free)>;

        LibRepType (nullptr_t);
        LibRepType (LibRepType&&) = default;
        LibRepType (X509* p);
    };

    /**
     */
    struct IRep : Cryptography::PKI::Certificate::IRep {
        virtual X509* Get_X509 () const = 0;
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
        Ptr (const shared_ptr<PKI::Certificate::IRep>& p);

        /**
         */
        X509* Get_X509 () const;
    };

    /**
     *  \brief Construct a Certificate object
     *      (1) from an adopted unique_ptr to the underlying library object
     *      (2) from generate a new self-signed certificate (and private key)
     *              \see https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl
     */
    Ptr                                  New (LibRepType&& x509);
    tuple<OpenSSL::PrivateKey::Ptr, Ptr> New (const SelfSignedCertParams& params);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Certificate.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_Certificate_h_*/
