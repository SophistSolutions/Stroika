/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_
#define _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_ 1

#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
using EVP_CIPHER_CTX = struct evp_cipher_ctx_st;
#endif

#include "../../Configuration/Common.h"
#include "../../Cryptography/SSL/Common.h"
#include "../../Execution/StringException.h"
#include "../../Memory/BLOB.h"
#include "../../Memory/Common.h"
#include "../../Streams/InputStream.h"
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
 *               BLOB ((const Byte*)kSrc4_, (const Byte*)kSrc4_ + ::strlen(kSrc4_)),
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

namespace Stroika {
    namespace Foundation {
        namespace Cryptography {
            namespace Encoding {

                using Memory::BLOB;
                using Memory::Byte;

                /**
                 */
                enum class Direction {
                    eEncrypt,
                    eDecrypt,
                };

#if qHasFeature_OpenSSL
                using Cryptography::OpenSSL::CipherAlgorithm;
                using Cryptography::OpenSSL::DigestAlgorithm;
                using Cryptography::OpenSSL::DerivedKey;
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
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
                 */
                class OpenSSLInputStream : public Streams::InputStream<Byte> {
                public:
                    OpenSSLInputStream () = delete;
                    OpenSSLInputStream (const OpenSSLCryptoParams& cryptoParams, Direction direction, const Streams::InputStream<Byte>::Ptr& realIn);
                    OpenSSLInputStream (const OpenSSLInputStream&) = delete;

                public:
                    class Ptr;

                public:
                    /**
                    *  You can construct, but really not use an FileInputStream object. Convert
                    *  it to a Ptr - to be able to use it.
                    */
                    nonvirtual operator Ptr () const;

                private:
                    class Rep_;

                private:
                    shared_ptr<Rep_> fRep_;
                };

                /**
                 *  Ptr is a copyable smart pointer to a OpenSSLInputStream.
                 */
                class OpenSSLInputStream::Ptr : public Streams::InputStream<Memory::Byte>::Ptr {
                private:
                    using inherited = Streams::InputStream<Memory::Byte>::Ptr;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *      \endcode
                     */
                    Ptr ()                = default;
                    Ptr (const Ptr& from) = default;

                private:
                    Ptr (const shared_ptr<Rep_>& from);

                public:
                    nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                    nonvirtual Ptr& operator                   = (const OpenSSLInputStream& rhs);

                private:
                    friend class OpenSSLInputStream;
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
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter">C++-Standard-Thread-Safety-Plus-Must-Externally-Synchronize-Letter</a>
                 */
                class OpenSSLOutputStream : public Streams::OutputStream<Byte> {
                public:
                    OpenSSLOutputStream () = delete;
                    OpenSSLOutputStream (const OpenSSLCryptoParams& cryptoParams, Direction direction, const Streams::OutputStream<Byte>::Ptr& realOut);
                    OpenSSLOutputStream (const OpenSSLOutputStream&) = delete;

                public:
                    class Ptr;

                public:
                    /**
                     *  You can construct, but really not use an OpenSSLOutputStream object. Convert
                     *  it to a Ptr - to be able to use it.
                     */
                    nonvirtual operator Ptr () const;

                private:
                    class Rep_;

                private:
                    shared_ptr<Rep_> fRep_;
                };

                /**
                 *  Ptr is a copyable smart pointer to a OpenSSLOutputStream.
                 */
                class OpenSSLOutputStream::Ptr : public Streams::OutputStream<Memory::Byte>::Ptr {
                private:
                    using inherited = Streams::OutputStream<Memory::Byte>::Ptr;

                public:
                    /**
                    *  \par Example Usage
                    *      \code
                    *      \endcode
                    */
                    Ptr ()                = default;
                    Ptr (const Ptr& from) = default;

                private:
                    Ptr (const shared_ptr<Rep_>& from);

                public:
                    nonvirtual Ptr& operator= (const Ptr& rhs) = default;
                    nonvirtual Ptr& operator                   = (const OpenSSLOutputStream& rhs);

                private:
                    friend class OpenSSLOutputStream;
                };

#endif
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "OpenSSLCryptoStream.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_*/
