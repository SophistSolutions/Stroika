/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#if OPENSSL_VERSION_MAJOR >= 3
#include <openssl/provider.h>
#endif
#endif

#include "../../Debug/Assertions.h"
#include "../../Execution/Exceptions.h"

#include "LibraryContext.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;

#if qHasFeature_OpenSSL && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#else
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#endif
#endif

#if qHasFeature_OpenSSL

namespace {
    void AccumulateIntoSetOfCipherNames_ (const ::EVP_CIPHER* ciph, Set<String>* ciphers)
    {
        RequireNotNull (ciphers);
        if (ciph != nullptr) {
#if OPENSSL_VERSION_MAJOR >= 3
            DbgTrace (L"cipher: %p (name: %s), provider: %p", ciph, CipherAlgorithm{ciph}.pName ().c_str (), ::EVP_CIPHER_provider (ciph));
#else
            DbgTrace (L"cipher: %p (name: %s)", ciph, CipherAlgorithm{ciph}.pName ().c_str ());
#endif
#if OPENSSL_VERSION_MAJOR >= 3
            if (auto provider = ::EVP_CIPHER_provider (ciph)) {
                DbgTrace ("providername = %s", ::OSSL_PROVIDER_name (provider));
            }
#endif
            int flags = ::EVP_CIPHER_flags (ciph);
            DbgTrace ("flags=%x", flags);

            ciphers->Add (CipherAlgorithm{ciph}.pName ());
        }
    };
}

/*
 ********************************************************************************
 ******************* Cryptography::OpenSSL::LibraryContext **********************
 ********************************************************************************
 */
LibraryContext LibraryContext::sDefault;

LibraryContext::LibraryContext ()
    : pAvailableCipherAlgorithms{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<CipherAlgorithm> {
              const LibraryContext*                               thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::pAvailableCipherAlgorithms);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              Set<String>                                         ciphers;
#if OPENSSL_VERSION_MAJOR >= 3
              ::EVP_CIPHER_do_all_provided (
                  nullptr,
                  [] (::EVP_CIPHER* ciph, void* arg) { AccumulateIntoSetOfCipherNames_ (ciph, reinterpret_cast<Set<String>*> (arg)); },
                  &ciphers);
#else
              ::EVP_CIPHER_do_all_sorted (
                  [] (const ::EVP_CIPHER* ciph, [[maybe_unused]] const char* from, [[maybe_unused]] const char* to, void* arg) { AccumulateIntoSetOfCipherNames_ (ciph, reinterpret_cast<Set<String>*> (arg)); },
                  &ciphers);
#endif
              DbgTrace (L"Found pAvailableCipherAlgorithms-FIRST-PASS (cnt=%d): %s", ciphers.size (), Characters::ToString (ciphers).c_str ());

              Set<CipherAlgorithm> result{ciphers.Select<CipherAlgorithm> ([] (const String& n) -> optional<CipherAlgorithm> { return OpenSSL::GetCipherByNameQuietly (n); })};
              WeakAssert (result.size () == ciphers.size ());
              DbgTrace (L"Found pAvailableCipherAlgorithms (cnt=%d): %s", result.size (), Characters::ToString (result).c_str ());
              return result;
          }}
    , pStandardCipherAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<CipherAlgorithm> {
        const LibraryContext*                               thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::pStandardCipherAlgorithms);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        Set<CipherAlgorithm>                                results;

        results += CipherAlgorithms::kAES_128_CBC;
        results += CipherAlgorithms::kAES_128_ECB;
        results += CipherAlgorithms::kAES_128_OFB;
        results += CipherAlgorithms::kAES_128_CFB1;
        results += CipherAlgorithms::kAES_128_CFB8;
        results += CipherAlgorithms::kAES_128_CFB128;
        results += CipherAlgorithms::kAES_192_CBC;
        results += CipherAlgorithms::kAES_192_ECB;
        results += CipherAlgorithms::kAES_192_OFB;
        results += CipherAlgorithms::kAES_192_CFB1;
        results += CipherAlgorithms::kAES_192_CFB8;
        results += CipherAlgorithms::kAES_192_CFB128;
        results += CipherAlgorithms::kAES_256_CBC;
        results += CipherAlgorithms::kAES_256_ECB;
        results += CipherAlgorithms::kAES_256_OFB;
        results += CipherAlgorithms::kAES_256_CFB1;
        results += CipherAlgorithms::kAES_256_CFB8;
        results += CipherAlgorithms::kAES_256_CFB128;

        /*
         * @todo mark these below as deprecated...??? in openssl3?
         */
        results += CipherAlgorithms::kBlowfish_CBC;
        results += CipherAlgorithms::kBlowfish_ECB;
        results += CipherAlgorithms::kBlowfish_CFB;
        results += CipherAlgorithms::kBlowfish_OFB;
        results += CipherAlgorithms::kBlowfish;
        results += CipherAlgorithms::kRC2_CBC;
        results += CipherAlgorithms::kRC2_ECB;
        results += CipherAlgorithms::kRC2_CFB;
        results += CipherAlgorithms::kRC2_OFB;
        results += CipherAlgorithms::kRC4;

        return results;
    }}
    , pAvailableDigestAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<DigestAlgorithm> {
        const LibraryContext*                               thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::pAvailableDigestAlgorithms);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        Set<DigestAlgorithm>                                results;
        results += DigestAlgorithm::eMD5;
        results += DigestAlgorithm::eSHA1;
        results += DigestAlgorithm::eSHA224;
        results += DigestAlgorithm::eSHA256;
        return results;
    }}
    , pStandardDigestAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<DigestAlgorithm> {
        const LibraryContext*                               thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::pStandardDigestAlgorithms);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        Set<DigestAlgorithm>                                results;
        results += DigestAlgorithm::eMD5;
        results += DigestAlgorithm::eSHA1;
        results += DigestAlgorithm::eSHA224;
        results += DigestAlgorithm::eSHA256;
        return results;
    }}
{
    LoadProvider (kDefaultProvider);
}

LibraryContext ::~LibraryContext ()
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if OPENSSL_VERSION_MAJOR >= 3
    for (auto i : fLoadedProviders_) {
        Verify (::OSSL_PROVIDER_unload (i.fValue.first) == 1);
    }
#endif
}

void LibraryContext::LoadProvider ([[maybe_unused]] const String& providerName)
{
    DbgTrace (L"OpenSSL::LibraryContext::LoadProvider", L"%s", providerName.c_str ());
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if OPENSSL_VERSION_MAJOR >= 3
    OSSL_PROVIDER*                                p = ::OSSL_PROVIDER_load (nullptr, providerName.AsNarrowSDKString ().c_str ());
    static const Execution::RuntimeErrorException kErr_{L"No such SSL provider"sv};
    Execution::ThrowIfNull (p, kErr_);
    if (auto l = fLoadedProviders_.Lookup (providerName)) {
        l->second++;
        fLoadedProviders_.Add (providerName, *l);
    }
    else {
        fLoadedProviders_.Add (providerName, {p, 1});
    }
#else
    Require (providerName == kDefaultProvider or providerName == kLegacyProvider);
#endif
}

void LibraryContext ::UnLoadProvider ([[maybe_unused]] const String& providerName)
{
    DbgTrace (L"OpenSSL::LibraryContext::UnLoadProvider", L"%s", providerName.c_str ());
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if OPENSSL_VERSION_MAJOR >= 3
    Require (fLoadedProviders_.ContainsKey (providerName));
    auto l = fLoadedProviders_.Lookup (providerName);
    Assert (l);
    l->second--;
    if (l->second == 0) {
        fLoadedProviders_.Remove (providerName);
        Verify (::OSSL_PROVIDER_unload (l->first) == 1);
    }
    else {
        fLoadedProviders_.Add (providerName, *l);
    }
#endif
}
#endif
