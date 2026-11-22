/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_LibraryContext_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>
#include <optional>
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
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedChecker.h"

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
     *  \brief OpenSSL's library context - which providers are loaded, and so which algorithms a program can use
     *
     *  Since OpenSSL 3, each algorithm comes from a provider: a named set of algorithm implementations. OpenSSL looks an
     *  algorithm up (a CipherAlgorithm, a DigestAlgorithm) among the providers loaded, so one whose provider is not loaded
     *  is unavailable - availableCipherAlgorithms and availableDigestAlgorithms list those of the providers loaded. Stroika
     *  names two:
     *      o   kDefaultProvider: nearly every algorithm a program needs - AES, ChaCha20, triple DES, SHA-1, SHA-2, SHA-3,
     *          MD5, ... sDefault loads it as it is constructed, so it is always there.
     *      o   kLegacyProvider: the algorithms OpenSSL calls legacy - insecure or out of use, kept for reading and writing old
     *          data: Blowfish, CAST, single DES, IDEA, RC2, RC4, SEED; MD4, MDC2, Whirlpool, RIPEMD160; PBKDF1. Among Stroika's
     *          OpenSSL::CipherAlgorithms: kBlowfish*, kRC2* and kRC4. Loaded only when asked for (LoadProvider,
     *          TemporarilyAddProvider).
     *  LoadProvider takes any other name OpenSSL knows too ("base", "fips", ...).
     *
     *  The legacy provider is a module (legacy.so, legacy.dll ...), which OpenSSL loads from the directory set by
     *  SetProviderModuleDirectory, else the one the environment variable OPENSSL_MODULES names, else the one compiled into
     *  OpenSSL (MODULESDIR: where an installed OpenSSL keeps them - for Stroika's own OpenSSL build, OpenSSL's default,
     *  /usr/local/lib/ossl-modules or C:/Program Files/OpenSSL/lib/ossl-modules, though it puts them in the configuration's
     *  lib/ossl-modules) - unless OpenSSL was configured no-module, which builds it into
     *  libcrypto (and leaves out FIPS, whose provider is always a module). With no module there to load,
     *  LoadProvider (kLegacyProvider) throws.
     *
     *  Before OpenSSL 3 there are no providers: every algorithm is built in, and LoadProvider and UnLoadProvider do nothing.
     *
     *  For now, only OpenSSL's default library context (sDefault): Stroika provides no way to make others.
     *
     *  \par Example Usage
     *      \code
     *          using namespace Cryptography::Providers;
     *          // data encrypted with Blowfish - in OpenSSL 3, only the legacy provider has it - loaded while legacy is in scope
     *          OpenSSL::LibraryContext::TemporarilyAddProvider legacy{&OpenSSL::LibraryContext::sDefault, OpenSSL::LibraryContext::kLegacyProvider};
     *          OpenSSL::CipherAlgorithm cipher = OpenSSL::CipherAlgorithms::kBlowfish_CBC;
     *          Encoding::OpenSSLCryptoParams cryptoParams{cipher, OpenSSL::EVP_BytesToKey{cipher, OpenSSL::DigestAlgorithms::kMD5, passphrase}};
     *          BLOB decrypted = Encoding::OpenSSLInputStream::New (cryptoParams, Encoding::Direction::eDecrypt,
     *                                                              encrypted.As<Streams::InputStream::Ptr<byte>> ())
     *                               .ReadAll ();
     *      \endcode
     *
     *  \par Example Usage
     *      \code
     *          // or for the rest of the program
     *          using namespace Cryptography::Providers;
     *          OpenSSL::LibraryContext::sDefault.LoadProvider (OpenSSL::LibraryContext::kLegacyProvider);
     *      \endcode
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a> - so
     *          loading or unloading a provider while another thread uses sDefault needs the caller's synchronization
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
        /**
         *  \brief OpenSSL's standard algorithms (@see LibraryContext) - always loaded in sDefault
         */
        static inline constexpr string_view kDefaultProvider = "default"sv;

    public:
        /**
         *  \brief OpenSSL's old, insecure algorithms, kept for old data (@see LibraryContext) - loaded only when asked for
         */
        static inline constexpr string_view kLegacyProvider = "legacy"sv;

    public:
        /**
         *  \brief The directory OpenSSL loads a provider's module from (@see LibraryContext) - or nullopt: OpenSSL's choice, the
         *         directory OPENSSL_MODULES names, else the one compiled into OpenSSL
         *
         *  One directory (OpenSSL's 'default search path'), for the providers loaded after it is set. A program that ships
         *  OpenSSL's modules - Stroika's OpenSSL build puts them in its lib/ossl-modules - sets it to where it put them.
         *
         *  Before OpenSSL 3 there are no provider modules: it is recorded, and changes nothing.
         *
         *  \par Example Usage
         *      \code
         *          // the modules shipped beside the program, in ../lib/ossl-modules
         *          using namespace Cryptography::Providers;
         *          OpenSSL::LibraryContext::sDefault.SetProviderModuleDirectory (Execution::GetEXEDir ().parent_path () / "lib" / "ossl-modules");
         *          OpenSSL::LibraryContext::sDefault.LoadProvider (OpenSSL::LibraryContext::kLegacyProvider);
         *      \endcode
         */
        nonvirtual void SetProviderModuleDirectory (const optional<filesystem::path>& directory);

    public:
        /**
         *  \brief The directory SetProviderModuleDirectory set, if any: nullopt leaves it to OpenSSL
         */
        nonvirtual optional<filesystem::path> GetProviderModuleDirectory () const;

    public:
        /**
         *  \brief Make the named provider's algorithms available (@see LibraryContext), until a matching UnLoadProvider
         *
         *  Counted: loaded 4 times, it takes 4 unloads to remove.
         *
         *  Throws (runtime_error) if OpenSSL cannot load it - an unknown name, or the legacy provider's module not found.
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
        Stroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE LibraryInit_ fLibraryInit_;
        Stroika_ATTRIBUTE_NO_UNIQUE_ADDRESS_VCFORCE Debug::AssertExternallySynchronizedChecker fThisAssertExternallySynchronized_;
        optional<filesystem::path>                                                             fProviderModuleDirectory_;

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
