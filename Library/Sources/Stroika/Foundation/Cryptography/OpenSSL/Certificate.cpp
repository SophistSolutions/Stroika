/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Cryptography/OpenSSL/PrivateKey.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "Certificate.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL
namespace {
    struct Rep_ : Cryptography::OpenSSL::Certificate::IRep {

        unique_ptr<::X509, decltype (&::X509_free)> fCert_;

        Rep_ (unique_ptr<::X509, decltype (&::X509_free)>&& p)
            : fCert_{move (p)}
        {
        }
        Rep_ (const PEMFile& pem)
            : fCert_{[&] () {
                         auto d  = pem.fData.As<span<const uint8_t>> ();
                         auto dd = d.data ();
                         BIO* bio = ::BIO_new_mem_buf (dd, static_cast<int> (d.size ())); // example had sz -1, but seems probably wrong here
                         auto r = ::PEM_read_bio_X509 (bio, NULL, NULL, NULL);
                         ::BIO_free (bio);
                         if (fCert_ == nullptr) {
                             throw ("oops");
                             // fprintf(stderr, "Error reading certificate: %s\n", ERR_error_string(ERR_get_error(), NULL));
                         }
                         return r;
                     }(),
                     &::X509_free}
        {
        }

        virtual X509* Get_X509 () const override
        {
            return fCert_.get ();
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL

Cryptography::OpenSSL::Certificate::Ptr::Ptr (unique_ptr<::X509, decltype (&::X509_free)>&& p)
    : Ptr{make_shared<Rep_> (move (p))}
{
}

auto Cryptography::OpenSSL::Certificate::New (const PEMFile& pem) -> Ptr
{
    return make_shared<Rep_> (pem);
}

auto OpenSSL::Certificate::NewSelfSigned () -> tuple<OpenSSL::PrivateKey::Ptr, Ptr>
{
    // Code adapted from https://stackoverflow.com/questions/256405/programmatically-create-x509-certificate-using-openssl
    unique_ptr<EVP_PKEY, decltype (&::EVP_PKEY_free)> pkey{EVP_PKEY_new (), ::EVP_PKEY_free};

    RSA* rsa = ::RSA_generate_key (2048,   /* number of bits for the key - 2048 is a sensible value */
                                   RSA_F4, /* exponent - RSA_F4 is defined as 0x10001L */
                                   NULL,   /* callback - can be NULL if we aren't displaying progress */
                                   NULL    /* callback argument - not needed in this case */
    );
    EVP_PKEY_assign_RSA (pkey.get (), rsa); // adopts rsa memory so no need to free

    unique_ptr<X509, decltype (&::X509_free)> newCert{X509_new (), ::X509_free};

    ASN1_INTEGER_set (X509_get_serialNumber (newCert.get ()), 1);

    X509_gmtime_adj (X509_get_notBefore (newCert.get ()), 0);
    constexpr auto kSecondsValid_ = 60 * 60 * 24 * 365; // 1 yr
    X509_gmtime_adj (X509_get_notAfter (newCert.get ()), kSecondsValid_);

    // Set public key to be the key we generated earlier
    X509_set_pubkey (newCert.get (), pkey.get ());

    X509_NAME* name = X509_get_subject_name (newCert.get ());

    ::X509_NAME_add_entry_by_txt (name, "C", MBSTRING_ASC, (unsigned char*)"US", -1, -1, 0);
    ::X509_NAME_add_entry_by_txt (name, "O", MBSTRING_ASC, (unsigned char*)"MyCompany Inc.", -1, -1, 0);
    ::X509_NAME_add_entry_by_txt (name, "CN", MBSTRING_ASC, (unsigned char*)"localhost", -1, -1, 0);

    // Since this is a self-signed certificate, we set the name of the issuer to the name of the subject
    ::X509_set_issuer_name (newCert.get (), name);

    // Now sign with SHA1 digest
    ::X509_sign (newCert.get (), pkey.get (), EVP_sha1 ());
    return make_tuple (Cryptography::OpenSSL::PrivateKey::Ptr{move (pkey)}, Ptr{move (newCert)});
}
#endif
