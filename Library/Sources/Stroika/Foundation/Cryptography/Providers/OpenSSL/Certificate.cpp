/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#include <openssl/pem.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/Exception.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/PrivateKey.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "Certificate.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::PKI::Certificate;
using namespace Stroika::Foundation::Cryptography::Providers;
using namespace Stroika::Foundation::Cryptography::Providers::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL
namespace {
    struct Rep_ : OpenSSL::Certificate::IRep {

        OpenSSL::Certificate::LibRepType fCert_;

        Rep_ ()            = delete;
        Rep_ (const Rep_&) = delete;
        Rep_ (Rep_&&)      = default;
        Rep_ (OpenSSL::Certificate::LibRepType&& p)
            : fCert_{move (p)}
        {
        }
        virtual Range<DateTime> GetValidDates () const override
        {
            using Time::Timezone;
            struct tm from {};
            struct tm to {};
            Exception::ThrowLastErrorIfFailed (::ASN1_TIME_to_tm (X509_get_notBefore (fCert_.get ()), &from));
            Exception::ThrowLastErrorIfFailed (::ASN1_TIME_to_tm (X509_get_notAfter (fCert_.get ()), &to));
            return Range<DateTime>{DateTime{from, Timezone::kUTC}, DateTime{to, Timezone::kUTC}};
        }
        virtual SubjectInfo GetSubject () const override
        {
            SubjectInfo result;
            X509_NAME*  subject    = ::X509_get_subject_name (fCert_.get ());
            int         numEntries = ::X509_NAME_entry_count (subject);
            for (int i = 0; i < numEntries; ++i) {
                X509_NAME_ENTRY* entry = ::X509_NAME_get_entry (subject, i);
                ASN1_OBJECT*     nid   = ::X509_NAME_ENTRY_get_object (entry);
                if (::OBJ_cmp (nid, ::OBJ_nid2obj (NID_commonName)) == 0) {
                    unsigned char* cn  = X509_NAME_ENTRY_get_data (entry)->data;
                    result.fCommonName = String::FromUTF8 ((const char*)cn);
                }
                else if (::OBJ_cmp (nid, ::OBJ_nid2obj (NID_countryName)) == 0) {
                    unsigned char* cn = ::X509_NAME_ENTRY_get_data (entry)->data;
                    result.fCountry   = String::FromUTF8 ((const char*)cn);
                }
                else if (::OBJ_cmp (nid, ::OBJ_nid2obj (NID_organizationName)) == 0) {
                    unsigned char* cn    = ::X509_NAME_ENTRY_get_data (entry)->data;
                    result.fOrganization = String::FromUTF8 ((const char*)cn);
                }
            }
            return result;
        }
        virtual X509* Get_X509 () const override
        {
            return fCert_.get ();
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL
/*
 ********************************************************************************
 ****************************** OpenSSL::Certificate ****************************
 ********************************************************************************
 */
auto OpenSSL::Certificate::New (LibRepType&& x509) -> Ptr
{
    return make_shared<Rep_> (move (x509));
}

auto OpenSSL::Certificate::New (const SelfSignedCertParams& params) -> tuple<OpenSSL::PrivateKey::Ptr, Ptr>
{
    // Code adapted from https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl
    unique_ptr<EVP_PKEY, decltype (&::EVP_PKEY_free)> pkey{EVP_RSA_gen (2048), ::EVP_PKEY_free};

    LibRepType newCert{X509_new (), ::X509_free};

    Exception::ThrowLastErrorIfFailed (::ASN1_INTEGER_set (::X509_get_serialNumber (newCert.get ()), 1));

    ::ASN1_TIME_set (X509_get_notBefore (newCert.get ()), params.fValidDates.GetLowerBound ().AsUTC ().As<time_t> ());
    ::ASN1_TIME_set (X509_get_notAfter (newCert.get ()), params.fValidDates.GetUpperBound ().AsUTC ().As<time_t> ());

    // Set public key to be the key we generated earlier
    Exception::ThrowLastErrorIfFailed (::X509_set_pubkey (newCert.get (), pkey.get ()));

    X509_NAME* name    = ::X509_get_subject_name (newCert.get ());
    u8string   org     = params.fSubject.fOrganization.AsUTF8 ();
    u8string   cn      = params.fSubject.fCommonName.AsUTF8 ();
    u8string   country = params.fSubject.fCountry.AsUTF8 ();

    Exception::ThrowLastErrorIfFailed (::X509_NAME_add_entry_by_txt (name, "C", MBSTRING_UTF8, (unsigned char*)country.c_str (), -1, -1, 0));
    Exception::ThrowLastErrorIfFailed (::X509_NAME_add_entry_by_txt (name, "O", MBSTRING_UTF8, (unsigned char*)org.c_str (), -1, -1, 0));
    Exception::ThrowLastErrorIfFailed (::X509_NAME_add_entry_by_txt (name, "CN", MBSTRING_UTF8, (unsigned char*)cn.c_str (), -1, -1, 0));

    // Since this is a self-signed certificate, we set the name of the issuer to the name of the subject
    Exception::ThrowLastErrorIfFailed (::X509_set_issuer_name (newCert.get (), name));

    // Now sign with SHA1 digest
    Exception::ThrowLastErrorIfFailed (::X509_sign (newCert.get (), pkey.get (), ::EVP_sha1 ()));
    return make_tuple (OpenSSL::PrivateKey::New (move (pkey)), New (move (newCert)));
}
#endif
