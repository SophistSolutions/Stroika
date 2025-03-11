/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#include <openssl/ssl.h>
#if OPENSSL_VERSION_MAJOR >= 3
#include <openssl/provider.h>
#endif
#endif

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

#include "LibraryContext.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Providers::OpenSSL;
using namespace Stroika::Foundation::Debug;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#if qStroika_HasComponent_OpenSSL

namespace {
    void AccumulateIntoSetOfCipherNames_ (const ::EVP_CIPHER* ciph, Set<String>* ciphers)
    {
        RequireNotNull (ciphers);
        if (ciph != nullptr) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
#if OPENSSL_VERSION_MAJOR >= 3
            DbgTrace ("cipher: {} (name: {}), provider: {} (name {})"_f, ciph, CipherAlgorithm{ciph}.name (), ::EVP_CIPHER_get0_provider (ciph),
                      (::EVP_CIPHER_get0_provider (ciph) == nullptr
                           ? L"null"
                           : String::FromNarrowSDKString (::OSSL_PROVIDER_get0_name (::EVP_CIPHER_get0_provider (ciph)))));
#else
            DbgTrace ("cipher: {} (name: {})"_f, ciph, CipherAlgorithm{ciph}.name ());
#endif
#endif
            ciphers->Add (CipherAlgorithm{ciph}.name ());
        }
    };
    void AccumulateIntoSetOfDigestNames_ (const ::EVP_MD* digest, Set<String>* digestNames)
    {
        RequireNotNull (digestNames);
        if (digest != nullptr) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
#if OPENSSL_VERSION_MAJOR >= 3
            DbgTrace ("digest: {} (name: {}), provider: {} (name {})"_f, digest, DigestAlgorithm{digest}.name (), ::EVP_MD_get0_provider (digest),
                      (::EVP_MD_get0_provider (digest) == nullptr
                           ? "null"_k
                           : String::FromNarrowSDKString (::OSSL_PROVIDER_get0_name (::EVP_MD_get0_provider (digest)))));
#else
            DbgTrace ("digest: {} (name: {})"_f, digest, DigestAlgorithm{digest}.name ());
#endif
#endif
            digestNames->Add (DigestAlgorithm{digest}.name ());
        }
    };
}

/*
 ********************************************************************************
 ************* Cryptography::Providers::OpenSSL::LibraryContext::LibraryInit_ **************
 ********************************************************************************
 */
LibraryContext::LibraryInit_::LibraryInit_ ()
{
    constexpr auto kOpts_ = OPENSSL_INIT_LOAD_SSL_STRINGS;
    Verify (::OPENSSL_init_ssl (kOpts_, nullptr) == 1);
}

/*
 ********************************************************************************
 ******************* Cryptography::Providers::OpenSSL::LibraryContext **********************
 ********************************************************************************
 */
LibraryContext::LibraryContext ()
    : availableCipherAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<CipherAlgorithm> {
        const LibraryContext* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::availableCipherAlgorithms);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
        Set<String>                                    cipherNames;
#if OPENSSL_VERSION_MAJOR >= 3
        ::EVP_CIPHER_do_all_provided (
            nullptr, [] (::EVP_CIPHER* ciph, void* arg) { AccumulateIntoSetOfCipherNames_ (ciph, reinterpret_cast<Set<String>*> (arg)); }, &cipherNames);
#else
        ::EVP_CIPHER_do_all_sorted ([] (const ::EVP_CIPHER* ciph, [[maybe_unused]] const char* from, [[maybe_unused]] const char* to,
                                        void* arg) { AccumulateIntoSetOfCipherNames_ (ciph, reinterpret_cast<Set<String>*> (arg)); },
                                    &cipherNames);
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Found availableCipherAlgorithms-FIRST-PASS (cnt={}): {}"_f, cipherNames.size (), cipherNames);
#endif

        Set<CipherAlgorithm> results{cipherNames.Map<Set<CipherAlgorithm>> (
            [] (const String& n) -> optional<CipherAlgorithm> { return OpenSSL::CipherAlgorithm::GetByNameQuietly (n); })};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Found availableCipherAlgorithms (cnt={}): {}"_f, results.size (), results);
#endif
        return results;
    }}
    , standardCipherAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<CipherAlgorithm> {
        const LibraryContext* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::standardCipherAlgorithms);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
        Set<CipherAlgorithm>                           results;

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

    /**
     * 
     *      @todo review openssl ciphers -s - and compare with above list - and cleanup - and maybe automate (find in driver source how it does
     * ciphers -s...)
     */
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
    , availableDigestAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<DigestAlgorithm> {
        const LibraryContext* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::availableDigestAlgorithms);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};

        Set<String> digestNames;
#if OPENSSL_VERSION_MAJOR >= 3
        ::EVP_MD_do_all_provided (
            nullptr, [] (::EVP_MD* md, void* arg) { AccumulateIntoSetOfDigestNames_ (md, reinterpret_cast<Set<String>*> (arg)); }, &digestNames);
#else
        ::EVP_MD_do_all_sorted ([] (const ::EVP_MD* md, [[maybe_unused]] const char* from, [[maybe_unused]] const char* to,
                                    void* arg) { AccumulateIntoSetOfDigestNames_ (md, reinterpret_cast<Set<String>*> (arg)); },
                                &digestNames);
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Found availableDigestAlgorithms-FIRST-PASS (cnt={}): {}"_f, digestNames.size (), digestNames);
#endif

        Set<DigestAlgorithm> results{digestNames.Map<Set<DigestAlgorithm>> (
            [] (const String& n) -> optional<DigestAlgorithm> { return OpenSSL::DigestAlgorithm::GetByNameQuietly (n); })};
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Found availableDigestAlgorithms (cnt={}): {}"_f, results.size (), results);
#endif
        return results;
    }}
    , standardDigestAlgorithms{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<DigestAlgorithm> {
        const LibraryContext* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::standardDigestAlgorithms);
        AssertExternallySynchronizedMutex::ReadContext declareContext{thisObj->fThisAssertExternallySynchronized_};
        Set<DigestAlgorithm>                           results;
        results += DigestAlgorithms::kMD5;
        results += DigestAlgorithms::kSHA1;
        results += DigestAlgorithms::kSHA1_224;
        results += DigestAlgorithms::kSHA1_256;
        results += DigestAlgorithms::kSHA1_384;
        results += DigestAlgorithms::kSHA1_512;
        results += DigestAlgorithms::kSHA3_224;
        results += DigestAlgorithms::kSHA3_256;
        results += DigestAlgorithms::kSHA3_384;
        results += DigestAlgorithms::kSHA3_512;
        return results;
    }}
{
    LoadProvider (kDefaultProvider);
}

LibraryContext ::~LibraryContext ()
{
    Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
#if OPENSSL_VERSION_MAJOR >= 3
    // reference counts dont matter here, just unload all the providers we loaded
    for (auto i : fLoadedProviders_.MappedValues ()) {
        Verify (::OSSL_PROVIDER_unload (i) == 1);
    }
#endif
}

void LibraryContext::LoadProvider ([[maybe_unused]] const String& providerName)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("OpenSSL::LibraryContext::LoadProvider", "{}"_f, providerName)};
    Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
#if OPENSSL_VERSION_MAJOR >= 3
    auto p = fLoadedProviders_.LookupOneValue (providerName);
    if (p == nullptr) {
        // really load cuz not already loaded
        DbgTrace ("calling OSSL_PROVIDER_load"_f);
        p = ::OSSL_PROVIDER_load (nullptr, providerName.AsNarrowSDKString ().c_str ());
        static const Execution::RuntimeErrorException kErr_{"No such SSL provider"sv};
        Execution::ThrowIfNull (p, kErr_);
    }
    fLoadedProviders_.Add (providerName, p); // add association (perhaps redundantly)
#else
    Require (providerName == kDefaultProvider or providerName == kLegacyProvider);
#endif
}

void LibraryContext ::UnLoadProvider ([[maybe_unused]] const String& providerName)
{
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs ("OpenSSL::LibraryContext::UnLoadProvider", "{}"_f, providerName)};
    Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fThisAssertExternallySynchronized_};
#if OPENSSL_VERSION_MAJOR >= 3
    Require (fLoadedProviders_.ContainsKey (providerName));
    auto providerToMaybeRemove = fLoadedProviders_.LookupOneValue (providerName);
    fLoadedProviders_.Remove (providerName);
    if (not fLoadedProviders_.ContainsKey (providerName)) {
        DbgTrace ("calling OSSL_PROVIDER_unload"_f);
        Verify (::OSSL_PROVIDER_unload (providerToMaybeRemove) == 1);
    }
#endif
}
#endif
