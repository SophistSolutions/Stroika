/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/Certificate.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/PrivateKey.h"
#include "Stroika/Foundation/Streams/TextReader.h"

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

namespace {
    struct Rep_ : Cryptography::Providers::OpenSSL::PEMFile::IRep {
        BLOB                fData_;
        Sequence<EntryType> fEntries_;

        Rep_ (const BLOB& b)
            : fData_{b}
        {
            auto d  = fData_.As<span<const uint8_t>> ();
#if 0
            // cannot find docs on b2i_PrivateKey API
            if (auto pp = ::b2i_PrivateKey (&dd, static_cast<long> (d.size ()))) {
                Cryptography::PKI::PrivateKey::Ptr pk1 = OpenSSL::PrivateKey::New (OpenSSL::PrivateKey::LibRepType{pp, EVP_PKEY_free});
                fEntries_ += pk1;
            }
#endif
            BIO* bio     = ::BIO_new_mem_buf (d.data (), static_cast<int> (d.size ())); // example had sz -1, but seems probably wrong here
            auto cleanup = Execution::Finally ([&] () { ::BIO_free (bio); });

            {
                EVP_PKEY* pp = nullptr;
                if (auto r = ::PEM_read_bio_PrivateKey (bio, &pp, nullptr, nullptr)) {
                    fEntries_ += OpenSSL::PrivateKey::New (OpenSSL::PrivateKey::LibRepType{pp, EVP_PKEY_free});
                }
            }
            {
                auto r = ::PEM_read_bio_X509 (bio, nullptr, nullptr, nullptr);
                if (r != nullptr) {
                    fEntries_ += OpenSSL::Certificate::New (OpenSSL::Certificate::LibRepType{r, &::X509_free});
                }
            }
        }
        Rep_ (const Sequence<EntryType>& entries)
            : fEntries_{entries}
        {
            AssertNotImplemented ();
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
 ********************* Cryptography::Providers::OpenSSL::PEMFile ***************************
 ********************************************************************************
 */

#if qStroika_HasComponent_OpenSSL
auto Cryptography::Providers::OpenSSL::PEMFile::New (const Memory::BLOB& pemData) -> Ptr
{
    return make_shared<Rep_> (pemData);
}
auto Cryptography::Providers::OpenSSL::PEMFile::New (const Sequence<EntryType>& entries) -> Ptr
{
    return make_shared<Rep_> (entries);
}
#endif
