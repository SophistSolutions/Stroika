/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <string>
#include <string_view>

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"

#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/CipherAlgorithm.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/DigestAlgorithm.h"

/**
 *  \file
 */

#if qStroika_HasComponent_OpenSSL
namespace Stroika::Foundation::Cryptography::Providers::OpenSSL {

    using Characters::String;
    using Containers::Set;

    /**
     *  For now, provide no way to access additional library contexts, as I have no such need right now
     *  and the docs don't make it super clear/simple.
     */
    class LibraryContext {
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
        static inline constexpr string_view kDefaultProvider = "default"sv;

    public:
        static inline constexpr string_view kLegacyProvider = "legacy"sv;

    public:
        /**
         *  Loads the given provider, but tracks count, so if loaded 4 times, takes 4 unloads to remove.
         */
        nonvirtual void LoadProvider (const String& providerName);

    public:
        /**
         * \pre the provider has been successfully loaded with LoadProvider, and the number of loads is greater than the number of
         *      unloads before this.
         */
        nonvirtual void UnLoadProvider (const String& providerName);

    public:
        /**
         *  Stack based wrapper on LoadProvider/UnLoadProvider()
         */
        class TemporarilyAddProvider {
        public:
            TemporarilyAddProvider ()                              = delete;
            TemporarilyAddProvider (const TemporarilyAddProvider&) = delete;
            TemporarilyAddProvider (LibraryContext* context, const String& providerName);
            ~TemporarilyAddProvider ();

        private:
            LibraryContext* fContext_;
            String          fProviderName_;
        };

    public:
        /**
         * @brief Return all the cipher algorithms provided by (the linked copy of) openssl.
         */
        Common::ReadOnlyProperty<Set<CipherAlgorithm>> availableCipherAlgorithms;

    public:
        /**
         * @brief Return all the standard/safe (tbd) cipher algorithms provided by (the linked copy of) openssl.
         */
        Common::ReadOnlyProperty<Set<CipherAlgorithm>> standardCipherAlgorithms;

    public:
        /**
         * @brief Return all the digest algorithms provided by (the linked copy of) openssl.
         */
        Common::ReadOnlyProperty<Set<DigestAlgorithm>> availableDigestAlgorithms;

    public:
        /**
         * @brief Return all the standard/safe (tbd) digest algorithms provided by (the linked copy of) openssl.
         */
        Common::ReadOnlyProperty<Set<DigestAlgorithm>> standardDigestAlgorithms;

    private:
        struct LibraryInit_ {
            LibraryInit_ ();
            ~LibraryInit_ () = default;
        };

    private:
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS LibraryInit_ fLibraryInit_;
        qStroika_ATTRIBUTE_NO_UNIQUE_ADDRESS Debug::AssertExternallySynchronizedMutex fThisAssertExternallySynchronized_;

#if OPENSSL_VERSION_MAJOR >= 3
        Containers::Association<String, ::OSSL_PROVIDER*> fLoadedProviders_; // providers maybe listed multiple times here, if loaded multiple times by callers
#endif
    };
    inline LibraryContext LibraryContext::sDefault;

}

#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LibraryContext.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_*/
