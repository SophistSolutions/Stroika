/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_ 1

#include "../../StroikaPreComp.h"

#include <string>
#include <string_view>

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Characters/String.h"
#include "../../Common/Property.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Mapping.h"
#include "../../Containers/Set.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"

#include "CipherAlgorithm.h"
#include "DigestAlgorithm.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   Redo Mapping<> to count stuff using Association
 */

#if qHasFeature_OpenSSL
namespace Stroika::Foundation::Cryptography::OpenSSL {

    using Characters::String;
    using Containers::Set;

    /**
     *  For now, provide no way to access additional library contexts, as I have no such need right now
     *  and the docs don't make it super clear/simple.
     */
    class LibraryContext : private Debug::AssertExternallySynchronizedLock {
    private:
        /**
         */
        LibraryContext ();

    public:
        LibraryContext (const LibraryContext&) = delete;

    public:
        /**
         */
        ~LibraryContext ();

    public:
        /**
         *  For now, only way to access this functionality is via the default library context.
         * 
         *  Docs not clear on this, but probably best to making any changes to this BEFORE making any other openssl calls
         */
        static LibraryContext sDefault;

    public:
        static inline constexpr wstring_view kDefaultProvider = L"default"sv;

    public:
        static inline constexpr wstring_view kLegacyProvider = L"legacy"sv;

    public:
        /**
         *  Loads the given provider, but tracks count, so if loaded 4 times, takes 4 unloads to remove.
         */
        nonvirtual void LoadProvider (const String& providerName);

    public:
        /**
         */
        nonvirtual void UnLoadProvider (const String& providerName);

    public:
        /**
         *  Stack based wrapper on LoadProvider/UnLoadProvider()
         */
        class TemporarilyAddProvider {
        public:
            TemporarilyAddProvider (LibraryContext* context, const String& providerName);
            ~TemporarilyAddProvider ();

        private:
            LibraryContext* fContext;
            String          fProviderName_;
        };

    public:
        Common::ReadOnlyProperty<Set<CipherAlgorithm>> pAvailableCipherAlgorithms;

    public:
        Common::ReadOnlyProperty<Set<CipherAlgorithm>> pStandardCipherAlgorithms;

    public:
        Common::ReadOnlyProperty<Set<DigestAlgorithm>> pAvailableDigestAlgorithms;

    public:
        Common::ReadOnlyProperty<Set<DigestAlgorithm>> pStandardDigestAlgorithms;

    private:
#if OPENSSL_VERSION_MAJOR >= 3
        Containers::Mapping<String, pair<::OSSL_PROVIDER*, unsigned int>> fLoadedProviders_;
#endif
    };

}

#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LibraryContext.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_*/
