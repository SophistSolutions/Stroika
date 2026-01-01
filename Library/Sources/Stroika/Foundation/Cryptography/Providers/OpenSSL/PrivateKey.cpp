/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "PrivateKey.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Providers;
using namespace Stroika::Foundation::Cryptography::Providers::OpenSSL;
using namespace Stroika::Foundation::Debug;

using Memory::BLOB;
using std::byte;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL

namespace {
    // https://linux.die.net/man/3/bio_s_mem
    struct BIO2String_ {
        BIO* b{nullptr};
        BIO2String_ ()
        {
            b = ::BIO_new (::BIO_s_mem ());
        }
        BIO2String_ (const BIO2String_&) = delete;
        String ToString ()
        {
            BUF_MEM* p;
            BIO_get_mem_ptr (b, &p);
            return String{span{p->data, p->length}}; // assume ascii???;
        }
        ~BIO2String_ ()
        {
            ::BIO_free (b);
        }
    };
}

namespace {
    struct Rep_ : OpenSSL::PrivateKey::IRep {

        OpenSSL::PrivateKey::LibRepType fKey_;

        Rep_ ()            = delete;
        Rep_ (const Rep_&) = delete;
        Rep_ (Rep_&&)      = default;
        Rep_ (OpenSSL::PrivateKey::LibRepType&& p)
            : fKey_{move (p)}
        {
        }
        virtual EVP_PKEY* Get_EVP_PKEY () const override
        {
            return fKey_.get ();
        }
        virtual int GetType () const override
        {
            return ::EVP_PKEY_get_base_id (fKey_.get ());
        }
        virtual unsigned int GetBits () const override
        {
            return ::EVP_PKEY_get_bits (fKey_.get ());
        }
        virtual String GetPrintSummary () const override
        {
            StringBuilder sb;
            // https://docs.openssl.org/3.0/man3/EVP_PKEY_print_private/
            {
                BIO2String_ b;
                if (::EVP_PKEY_print_public (b.b, fKey_.get (), 0, nullptr) == 1) {
                    sb << b.ToString ();
                }
            }
            {
                BIO2String_ b;
                if (::EVP_PKEY_print_private (b.b, fKey_.get (), 0, nullptr) == 1) {
                    sb << b.ToString ();
                }
            }
            {
                BIO2String_ b;
                if (::EVP_PKEY_print_params (b.b, fKey_.get (), 0, nullptr) == 1) {
                    sb << b.ToString ();
                }
            }
            return sb;
        }
    };
}
#endif

#if qStroika_HasComponent_OpenSSL
auto OpenSSL::PrivateKey::New (LibRepType&& p) -> Ptr
{
    return Memory::MakeSharedPtr<Rep_> (move (p));
}
#endif
