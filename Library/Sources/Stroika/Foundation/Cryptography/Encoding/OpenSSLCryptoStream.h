/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_
#define _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_ 1

#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
using EVP_CIPHER_CTX = struct evp_cipher_ctx_st;
#endif

#include "../../Configuration/Common.h"
#include "../../Cryptography/SSL/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Memory/BLOB.h"
#include "../../Memory/Common.h"
#include "../../Streams/InputStream.h"
#include "../../Streams/InternallySynchronizedInputStream.h"
#include "../../Streams/InternallySynchronizedOutputStream.h"
#include "../../Streams/OutputStream.h"

#include "../OpenSSL/CipherAlgorithm.h"
#include "../OpenSSL/DerivedKey.h"
#include "../OpenSSL/DigestAlgorithm.h"
#include "../OpenSSL/Exception.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   OpenSSLInputStream and OpenSSLOutputStream must be fixed to use Debug::AssertExternallySynchronized
 *
 *      @todo   Review https://www.openssl.org/docs/crypto/EVP_EncryptInit.html and things like setkeylength etc to get
 *              rc4 working (idnetically with windows version)
 *
 *              See nt do_crypt(FILE *in, FILE *out, int do_encrypt) example.,
 *
 *              From examples, one bug maybe the value of inital_iv - EVP_CipherInit_ex - that the call may expect
 *              its the right lenght but the length is user-specified.. I may need to init to zero (or some such), and
 *              copy in user data.
 *
 *              I think for BOTH key and iv we must look at expected keylen/iplen and pass in data that matches the
 *              length. Maybe set what hack we did for CipherAlgorithm::eRC4?? set key length based on whats passed in?
 *
 *              We maybe can (mostly/always) set iv to NULL???
 *
 *              #if 0
 *                // DEBUG WHY THIS FAILS - I THINK WE NEED TO ENABLE PADDING FOR SOME CYPHERS!
 *               BLOB ((const byte*)kSrc4_, (const byte*)kSrc4_ + ::strlen(kSrc4_)),
 *          #endif
 *
 *      @todo   Generate exceptions on errors
 *
 *      @todo   this module includes <openssl> stuff in the header. Add additioanl modules inside
 *              Crypto that just are called 'Blowfish', and 'rc2', and these have classes that tkae
 *              constructors with just the needed data = maybe not even ctors - maybe functions - that
 *              take a stream, and return a decrpting (or encyrpting) stream - with arg params that make
 *              sense for that algoritjm. They are only defined #if qSSLAvail, but otherwise include
 *
 *              DONE for AES - BUt do the others - just like that - and maybe cleanup Base64/MD5 APIs to
 *              be done like for AES...
 *              ...
 *
 *
 */

namespace Stroika::Foundation::Cryptography::Encoding {

    using Memory::BLOB;

    /**
     */
    enum class Direction {
        eEncrypt,
        eDecrypt,
    };

#if qHasFeature_OpenSSL
    using Cryptography::OpenSSL::CipherAlgorithm;
    using Cryptography::OpenSSL::DerivedKey;
    using Cryptography::OpenSSL::DigestAlgorithm;
#endif

#if qHasFeature_OpenSSL
    class OpenSSLCryptoParams {
    public:
        // use this CTOR and fill in parameters manually for EVP_EncryptInit_ex
        OpenSSLCryptoParams (const function<void(EVP_CIPHER_CTX*, Direction d)>& f);
        // allowed CipherAlgorithm's for this CTOR include eAES_*, eBlowfish_*, eRC2'
        OpenSSLCryptoParams (CipherAlgorithm alg, BLOB key, BLOB initialIV);
        OpenSSLCryptoParams (CipherAlgorithm alg, const DerivedKey& derivedKey);

    public:
        function<void(EVP_CIPHER_CTX*, Direction)> fInitializer;
    };
#endif

#if qHasFeature_OpenSSL
    /**
     *  @brief  OpenSSLInputStream is a BinaryInputStream which does OpenSSL-based encryption or decryption (depending on direction arg)
     *
     *  OpenSSLInputStream is a BinaryInputStream which wraps another BinaryInputStream
     *  and does OpenSSL-based  encryption or decryption (depending on direction arg).
     *
     *  Use OpenSSLInputStream is you wish to use the result of encryption in your program, so you prefer to structure
     *  your conversion code as a process of reading.
     *
     *  @see OpenSSLOutputStream
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
     */
    class OpenSSLInputStream : public Streams::InputStream<std::byte> {
    public:
        OpenSSLInputStream ()                          = delete;
        OpenSSLInputStream (const OpenSSLInputStream&) = delete;

    public:
        using typename InputStream<std::byte>::Ptr;

    public:
        /**
         */
        static Ptr New (const OpenSSLCryptoParams& cryptoParams, Direction direction, const Streams::InputStream<std::byte>::Ptr& realIn);
        static Ptr New (Execution::InternallySynchronized internallySynchronized, const OpenSSLCryptoParams& cryptoParams, Direction direction, const Streams::InputStream<std::byte>::Ptr& realIn);

    private:
        class Rep_;

    private:
        using InternalSyncRep_ = Streams::InternallySynchronizedInputStream<std::byte, OpenSSLInputStream, OpenSSLInputStream::Rep_>;
    };

#endif

#if qHasFeature_OpenSSL
    /**
     *  @brief  OpenSSLOutputStream is a BinaryOutputStream which does OpenSSL-based encryption or decryption (depending on direction arg)
     *
     *  OpenSSLOutputStream is a BinaryOutputStream which wraps another BinaryOutputStream
     *  and does OpenSSL-based  encryption or decryption (depending on direction arg).
     *
     *  Use OpenSSLOutputStream is you wish to produce an artifact (e.g. external file) as a result of incrementally writing
     *  to a stream.
     *
     *  @see OpenSSLInputStream
     *
     *  \note   OpenSSLOutputStream aggregates its owned substream, so that a Close () on OpenSSLOutputStream
     *          will Close that substream.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
     */
    class OpenSSLOutputStream : public Streams::OutputStream<std::byte> {
    public:
        OpenSSLOutputStream ()                           = delete;
        OpenSSLOutputStream (const OpenSSLOutputStream&) = delete;

    public:
        using typename OutputStream<std::byte>::Ptr;

    public:
        /**
         */
        static Ptr New (const OpenSSLCryptoParams& cryptoParams, Direction direction, const Streams::OutputStream<std::byte>::Ptr& realOut);
        static Ptr New (Execution::InternallySynchronized internallySynchronized, const OpenSSLCryptoParams& cryptoParams, Direction direction, const Streams::OutputStream<std::byte>::Ptr& realOut);

    private:
        class Rep_;

    private:
        using InternalSyncRep_ = Streams::InternallySynchronizedOutputStream<std::byte, OpenSSLOutputStream, OpenSSLOutputStream::Rep_>;
    };
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "OpenSSLCryptoStream.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_*/
