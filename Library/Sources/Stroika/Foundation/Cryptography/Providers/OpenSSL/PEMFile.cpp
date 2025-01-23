/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/Certificate.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/Exception.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/PrivateKey.h"
#include "Stroika/Foundation/Streams/ToText.h"

#include "PEMFile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Providers;
using namespace Stroika::Foundation::Cryptography::Providers::OpenSSL;
using namespace Stroika::Foundation::Cryptography::PKI::PEMFile;
using namespace Stroika::Foundation::Streams;

using Memory::BLOB;

#if qStroika_HasComponent_OpenSSL
namespace {
    // https://linux.die.net/man/3/bio_s_mem
    struct BIO2BLOB_ {
        BIO* fBIO{nullptr};
        BIO2BLOB_ ()
        {
            fBIO = ::BIO_new (::BIO_s_mem ());
        }
        BIO2BLOB_ (const BIO2BLOB_&) = delete;
        BLOB ToBLOB ()
        {
            BUF_MEM* p;
            BIO_get_mem_ptr (fBIO, &p);
            return BLOB{span{p->data, p->length}};
        }
        ~BIO2BLOB_ ()
        {
            ::BIO_free (fBIO);
        }
    };
}

namespace {
    struct Rep_ : Cryptography::Providers::OpenSSL::PEMFile::IRep {
        BLOB                fData_;
        Sequence<EntryType> fEntries_;

        Rep_ (const BLOB& b)
            : fData_{b}
        {
            auto d = fData_.As<span<const uint8_t>> ();
#if 0
            // cannot find docs on b2i_PrivateKey API
            if (auto pp = ::b2i_PrivateKey (&dd, static_cast<long> (d.size ()))) {
                Cryptography::PKI::PrivateKey::Ptr pk1 = OpenSSL::PrivateKey::New (OpenSSL::PrivateKey::LibRepType{pp});
                fEntries_ += pk1;
            }
#endif
            BIO* bio     = ::BIO_new_mem_buf (d.data (), static_cast<int> (d.size ())); // example had sz -1, but seems probably wrong here
            auto cleanup = Execution::Finally ([&] () { ::BIO_free (bio); });

            {
                EVP_PKEY* pp = nullptr;
                if (auto r = ::PEM_read_bio_PrivateKey (bio, &pp, nullptr, nullptr)) {
                    Assert (r == pp);
                    fEntries_ += OpenSSL::PrivateKey::New (OpenSSL::PrivateKey::LibRepType{r});
                }
            }
            if (X509* r = ::PEM_read_bio_X509 (bio, nullptr, nullptr, nullptr)) {
                fEntries_ += OpenSSL::Certificate::New (OpenSSL::Certificate::LibRepType{r});
            }
        }
        Rep_ (const Sequence<EntryType>& entries)
            : fEntries_{entries}
        {
            BIO2BLOB_ b;
            for (auto e : entries) {
                if (auto oc = get_if<PKI::Certificate::Ptr> (&e)) {
                    auto cert = Certificate::Ptr{*oc}.Get_X509 ();
                    Exception::ThrowLastErrorIfFailed (::PEM_write_bio_X509 (b.fBIO, cert));
                }
                else if (auto op = get_if<PKI::PrivateKey::Ptr> (&e)) {
                    auto key = PrivateKey::Ptr{*op}.Get_EVP_PKEY ();
                    Exception::ThrowLastErrorIfFailed (::PEM_write_bio_PrivateKey (b.fBIO, key, nullptr, nullptr, 0, 0, nullptr));
                }
            }
            fData_ = b.ToBLOB ();
        }
        virtual BLOB GetData () const override
        {
            return fData_;
        }
        virtual Sequence<EntryType> GetEntries () const override
        {
            return fEntries_;
        }
    };
}

/*
 ********************************************************************************
 ********************* Cryptography::Providers::OpenSSL::PEMFile ****************
 ********************************************************************************
 */
auto Cryptography::Providers::OpenSSL::PEMFile::New (const Memory::BLOB& pemData) -> Ptr
{
    return make_shared<Rep_> (pemData);
}
auto Cryptography::Providers::OpenSSL::PEMFile::New (const Sequence<EntryType>& entries) -> Ptr
{
    return make_shared<Rep_> (entries);
}
#endif
