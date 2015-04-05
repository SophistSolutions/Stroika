/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_
#define _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_ 1

#include    "../../StroikaPreComp.h"

#if     qHasFeature_OpenSSL
extern "C" {
    using  EVP_CIPHER_CTX   =   struct evp_cipher_ctx_st;
}
#endif

#include    "../../Configuration/Common.h"
#include    "../../Cryptography/SSL/Common.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/BLOB.h"
#include    "../../Streams/BinaryInputStream.h"
#include    "../../Streams/BinaryOutputStream.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   Very preliminary draft - not tested.
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
 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Encoding {


                /**
                 */
                enum    class   Direction {
                    eEncrypt,
                    eDecrypt,
                };


#if     qHasFeature_OpenSSL
                class   OpenSSLCryptoParams {
                public:
                    // use this CTOR and fill in parameters manually for EVP_EncryptInit_ex
                    OpenSSLCryptoParams (const function<void(EVP_CIPHER_CTX*, Direction d)>& f);


                    /**
                     *      @see http://linux.die.net/man/3/evp_cipher_ctx_init
                     */
                    enum    class   Algorithm {
                        eAES_128_CBC,
                        eAES_128_ECB,
                        eAES_128_OFB,
                        eAES_128_CFB1,
                        eAES_128_CFB8,
                        eAES_128_CFB128,
                        eAES_192_CBC,
                        eAES_192_ECB,
                        eAES_192_OFB,
                        eAES_192_CFB1,
                        eAES_192_CFB8,
                        eAES_192_CFB128,
                        eAES_256_CBC,
                        eAES_256_ECB,
                        eAES_256_OFB,
                        eAES_256_CFB1,
                        eAES_256_CFB8,
                        eAES_256_CFB128,

                        eBlowfish_CBC,
                        eBlowfish_ECB,
                        eBlowfish_CFB,
                        eBlowfish_OFB,

                        eRC2_CBC,
                        eRC2_ECB,
                        eRC2_CFB,
                        eRC2_OFB,

                        eRC4,

                        Stroika_Define_Enum_Bounds(eAES_128_CBC, eRC4)
                    };
                    // allowed Algorith's for this CTOR include eAES_*, eBlowfish_*, eRC2'
                    OpenSSLCryptoParams (Algorithm alg, Memory::BLOB key, Memory::BLOB initialIV = Memory::BLOB ());

                public:
                    function<void(EVP_CIPHER_CTX*, Direction)>  fInitializer;
                };
#endif


#if     qHasFeature_OpenSSL
                /**
                 *  @brief  OpenSSLInputStream is a BinaryInputStream which does OpenSSL-based decryption
                 *
                 *  OpenSSLInputStream is a BinaryInputStream which wraps another BinaryInputStream
                 *  and does OpenSSL-based decryption.
                 */
                class   OpenSSLInputStream : public Streams::BinaryInputStream {
                private:
                    class   IRep_;
                public:
                    OpenSSLInputStream (const OpenSSLCryptoParams& cryptoParams, Direction direction, const BinaryInputStream& realIn);
                };
#endif


#if     qHasFeature_OpenSSL
                /**
                 *  @brief  OpenSSLOutputStream is a BinaryOutputStream which does OpenSSL-based decryption
                 *
                 *  OpenSSLOutputStream is a BinaryOutputStream which wraps another BinaryOutputStream
                 *  and does OpenSSL-based encryption.
                 */
                class   OpenSSLOutputStream : public Streams::BinaryOutputStream {
                private:
                    class   IRep_;
                public:
                    OpenSSLOutputStream (const OpenSSLCryptoParams& cryptoParams, Direction direction, const BinaryOutputStream& realOut);
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
#include    "OpenSSLCryptoStream.inl"

#endif  /*_Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_*/
