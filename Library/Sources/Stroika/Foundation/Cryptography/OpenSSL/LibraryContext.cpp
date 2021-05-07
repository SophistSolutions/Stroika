/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#include <openssl/provider.h>
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
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSL::LibraryContext **********************
 ********************************************************************************
 */
LibraryContext LibraryContext::sDefault;

LibraryContext::LibraryContext ()
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
