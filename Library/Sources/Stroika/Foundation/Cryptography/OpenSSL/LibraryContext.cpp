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
#if 0
            int flags = ::EVP_CIPHER_flags (ciph);
            DbgTrace ("flags=%x", flags);
#endif

            ciphers->Add (CipherAlgorithm{ciph}.pName ());
        }
    };
    void AccumulateIntoSetOfDigestNames_ (const ::EVP_MD* digest, Set<String>* digestNames)
    {
        RequireNotNull (digestNames);
        if (digest != nullptr) {
#if OPENSSL_VERSION_MAJOR >= 3
            DbgTrace (L"digest: %p (name: %s), provider: %p", digest, DigestAlgorithm{digest}.pName ().c_str (), ::EVP_MD_provider (digest));
#else
            DbgTrace (L"digest: %p (name: %s)", digest, DigestAlgorithm{digest}.pName ().c_str ());
#endif
#if OPENSSL_VERSION_MAJOR >= 3
            if (auto provider = ::EVP_MD_provider (digest)) {
                DbgTrace ("providername = %s", ::OSSL_PROVIDER_name (provider));
            }
#endif
            digestNames->Add (DigestAlgorithm{digest}.pName ());
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
              Set<String>                                         cipherNames;
#if OPENSSL_VERSION_MAJOR >= 3
              ::EVP_CIPHER_do_all_provided (
                  nullptr,
                  [] (::EVP_CIPHER* ciph, void* arg) { AccumulateIntoSetOfCipherNames_ (ciph, reinterpret_cast<Set<String>*> (arg)); },
                  &cipherNames);
#else
              ::EVP_CIPHER_do_all_sorted (
#if qCompilerAndStdLib_maybe_unused_in_lambda_ignored_Buggy
                  [] (const ::EVP_CIPHER* ciph, const char*, const char*, void* arg) { AccumulateIntoSetOfCipherNames_ (ciph, reinterpret_cast<Set<String>*> (arg)); },
#else
                  [] (const ::EVP_CIPHER* ciph, [[maybe_unused]] const char* from, [[maybe_unused]] const char* to, void* arg) { AccumulateIntoSetOfCipherNames_ (ciph, reinterpret_cast<Set<String>*> (arg)); },
#endif
                  &cipherNames);
#endif
              DbgTrace (L"Found pAvailableCipherAlgorithms-FIRST-PASS (cnt=%d): %s", cipherNames.size (), Characters::ToString (cipherNames).c_str ());

              Set<CipherAlgorithm> results{cipherNames.Select<CipherAlgorithm> ([] (const String& n) -> optional<CipherAlgorithm> { return OpenSSL::CipherAlgorithm::GetByNameQuietly (n); })};
              DbgTrace (L"Found pAvailableCipherAlgorithms (cnt=%d): %s", results.size (), Characters::ToString (results).c_str ());
              return results;
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
#if OPENSSL_VERSION_MAJOR < 3
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
#endif

        return results;
    }}
    , pAvailableDigestAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<DigestAlgorithm> {
        const LibraryContext*                               thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::pAvailableDigestAlgorithms);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};

        Set<String> digestNames;
#if OPENSSL_VERSION_MAJOR >= 3
        ::EVP_MD_do_all_provided (
            nullptr,
            [] (::EVP_MD* md, void* arg) { AccumulateIntoSetOfDigestNames_ (md, reinterpret_cast<Set<String>*> (arg)); },
            &digestNames);
#else
        ::EVP_MD_do_all_sorted (
#if qCompilerAndStdLib_maybe_unused_in_lambda_ignored_Buggy
            [] (const ::EVP_MD* md, const char*, const char*, void* arg) { AccumulateIntoSetOfDigestNames_ (md, reinterpret_cast<Set<String>*> (arg)); },
#else
            [] (const ::EVP_MD* md, [[maybe_unused]] const char* from, [[maybe_unused]] const char* to, void* arg) { AccumulateIntoSetOfDigestNames_ (md, reinterpret_cast<Set<String>*> (arg)); },
#endif
            &digestNames);
#endif
        DbgTrace (L"Found pAvailableDigestAlgorithms-FIRST-PASS (cnt=%d): %s", digestNames.size (), Characters::ToString (digestNames).c_str ());

        Set<DigestAlgorithm> results{digestNames.Select<DigestAlgorithm> ([] (const String& n) -> optional<DigestAlgorithm> { return OpenSSL::DigestAlgorithm::GetByNameQuietly (n); })};
        DbgTrace (L"Found pAvailableDigestAlgorithms (cnt=%d): %s", results.size (), Characters::ToString (results).c_str ());

        return results;
    }}
    , pStandardDigestAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<DigestAlgorithm> {
        const LibraryContext*                               thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::pStandardDigestAlgorithms);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        Set<DigestAlgorithm>                                results;
        results += DigestAlgorithms::kMD5;
        results += DigestAlgorithms::kSHA1;
        results += DigestAlgorithms::kSHA224;
        results += DigestAlgorithms::kSHA256;
        results += DigestAlgorithms::kSHA384;
        results += DigestAlgorithms::kSHA512;
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
