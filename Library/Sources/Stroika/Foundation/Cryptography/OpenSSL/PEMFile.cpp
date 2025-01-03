/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/Certificate.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/PrivateKey.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "PEMFile.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Cryptography::PKI::PEMFile;
using namespace Stroika::Foundation::Streams;

using Memory::BLOB;

namespace {
    struct Rep_ : Cryptography::OpenSSL::PEMFile::IRep {
        BLOB                fData_;
        Sequence<EntryType> fEntries_;
        Rep_ (const BLOB& b)
            : fData_{b}
        {
            auto d  = fData_.As<span<const uint8_t>> ();
            auto dd = d.data ();

            // cannot find docs on b2i_PrivateKey API
            if (auto pp = ::b2i_PrivateKey (&dd, static_cast<long> (d.size ()))) {
                Cryptography::PKI::PrivateKey::Ptr pk1 = OpenSSL::PrivateKey::New (OpenSSL::PrivateKey::LibRepType{pp, EVP_PKEY_free});
                fEntries_ += pk1;
            }

            BIO* bio = ::BIO_new_mem_buf (dd, static_cast<int> (d.size ())); // example had sz -1, but seems probably wrong here
            auto r   = ::PEM_read_bio_X509 (bio, NULL, NULL, NULL);
            ::BIO_free (bio);
            if (r != nullptr) {
                Cryptography::PKI::Certificate::Ptr c1 = OpenSSL::Certificate::New (OpenSSL::Certificate::LibRepType{r, &::X509_free});
                fEntries_ += c1;
            }
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
 ********************* Cryptography::OpenSSL::PEMFile ***************************
 ********************************************************************************
 */

#if qStroika_HasComponent_OpenSSL
auto Cryptography::OpenSSL::PEMFile::New (const Memory::BLOB& pemData) -> Ptr
{
    return make_shared<Rep_> (pemData);
}
#endif
