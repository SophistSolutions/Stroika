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
              ::EVP_CIPHER_do_all_provided (
                  nullptr,
                  [] (EVP_CIPHER* ciph, void* arg) {
                      Set<String>* ciphers = reinterpret_cast<Set<String>*> (arg);
                      if (ciph != nullptr) {
                          DbgTrace (L"cipher: %p (name: %s), provider: %p", ciph, CipherAlgorithm{ciph}.pName ().c_str (), ::EVP_CIPHER_provider (ciph));
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
                  },
                  &ciphers);
              DbgTrace (L"Found kAllLoadedCiphers=%s", Characters::ToString (ciphers).c_str ());

              auto fn = [] (const String& n) -> optional<CipherAlgorithm> { return OpenSSL::GetCipherByNameQuietly (n); };

              Traversal::Iterable<int> yyy{};
              Set<int>                 resultyyy{yyy};

              using namespace Configuration;
              static_assert (IsIterable_v<Traversal::Iterable<CipherAlgorithm>>);

              using ITERABLE_OF_T = Traversal::Iterable<CipherAlgorithm>;
    //Configuration::Private::  IsIterableOfT_Impl2_<set<int>, int> aa;

#if 1

              Set<CipherAlgorithm> result{ciphers.Select<CipherAlgorithm> ([] (const String& n) -> optional<CipherAlgorithm> { return OpenSSL::GetCipherByNameQuietly (n); })};
              WeakAssert (result.size () == ciphers.size ());
#endif
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

void LibraryContext::LoadProvider (const String& providerName)
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
#if OPENSSL_VERSION_MAJOR >= 3
    OSSL_PROVIDER*                                p = ::OSSL_PROVIDER_load (NULL, providerName.AsNarrowSDKString ().c_str ());
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

void LibraryContext ::UnLoadProvider (const String& providerName)
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
