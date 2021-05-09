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
    optional<String> GetCiphrName_ (CipherAlgorithm a)
    {
        if (auto i = ::EVP_CIPHER_name (a)) {
            return String::FromASCII (i);
        }
        return nullopt;
    }
    void AccumulateIntoSetOfCipherNames_ (const ::EVP_CIPHER* ciph, Set<String>* ciphers)
    {
        RequireNotNull (ciphers);
        if (ciph != nullptr) {
#if OPENSSL_VERSION_MAJOR >= 3
            DbgTrace (L"cipher: %p (name: %s), provider: %p", ciph, CipherAlgorithm{ciph}.pName ().c_str (), ::EVP_CIPHER_provider (ciph));
#else
            DbgTrace (L"cipher: %p (name: %s)", ciph, CipherAlgorithm{ciph}.pName ().c_str ());
#endif
            Assert (GetCiphrName_ (ciph));
            if (auto cipherName = GetCiphrName_ (ciph)) {

#if OPENSSL_VERSION_MAJOR >= 3
                if (auto provider = ::EVP_CIPHER_provider (ciph)) {
                    DbgTrace ("providername = %s", ::OSSL_PROVIDER_name (provider));
                }
#endif
                int flags = ::EVP_CIPHER_flags (ciph);
                DbgTrace ("flags=%x", flags);

                ciphers->Add (*cipherName);
            }
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
    : pAvailableAlgorithms{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Set<CipherAlgorithm> {
              const LibraryContext*                               thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &LibraryContext::pAvailableAlgorithms);
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
              DbgTrace (L"Found kAllLoadedCiphers=%s", Characters::ToString (ciphers).c_str ());

              Set<CipherAlgorithm> result{ciphers.Select<CipherAlgorithm> ([] (const String& n) -> optional<CipherAlgorithm> { return OpenSSL::GetCipherByNameQuietly (n); })};
              WeakAssert (result.size () == ciphers.size ());
              DbgTrace (L"Found result=%s", Characters::ToString (result).c_str ());
              return result;
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
