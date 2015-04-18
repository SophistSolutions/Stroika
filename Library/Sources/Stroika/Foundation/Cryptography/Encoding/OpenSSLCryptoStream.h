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
#include    "../../Execution/StringException.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/BLOB.h"
#include    "../../Streams/BinaryInputStream.h"
#include    "../../Streams/BinaryOutputStream.h"



/**
 *  \file
 *
 *  TODO:
 *      @todo   new test26 openssl warnings to investigate - memory leak reported by valgrind.
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
 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   Encoding {


                using   Memory::BLOB;
                using   Memory::Byte;


#if     qHasFeature_OpenSSL
                /**
                 *  @todo maybe move elsewhere?
                 */
                class   OpenSSLException : public Execution::StringException {
                public:
                    using   InternalErrorCodeType = unsigned long;

                public:
                    OpenSSLException (InternalErrorCodeType errorCode);

                public:
                    nonvirtual  InternalErrorCodeType   GetErrorCode () const;

                public:
                    static  Characters::String  GetMessage (InternalErrorCodeType errorCode);

                public:
                    /*
                     * DoThrowLastErrorIfFailed throws if status is not = 1
                     */
                    static  void    DoThrowLastErrorIfFailed (int status);

                public:
                    /*
                     * DoThrowLastError () throws error in ERR_get_error
                     */
                    static  void    DoThrowLastError ();

                private:
                    InternalErrorCodeType   fErrorCode_;
                };
#endif


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
                    enum    class   CipherAlgorithm {
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
                    // allowed CipherAlgorithm's for this CTOR include eAES_*, eBlowfish_*, eRC2'
                    OpenSSLCryptoParams (CipherAlgorithm alg, BLOB key, BLOB initialIV = BLOB ());

					/// ROUGH DRAFT 
                    enum HashAlg { eMD5, eSHA1 };
                    static  pair<BLOB, BLOB> DoDerviveKey (HashAlg hashAlg, CipherAlgorithm alg, pair<const Byte*, const Byte*> passwd, unsigned int keyLen);
                    static  pair<BLOB, BLOB> DoDerviveKey (HashAlg hashAlg, CipherAlgorithm alg, const string& passwd, unsigned int keyLen);

                public:
                    function<void(EVP_CIPHER_CTX*, Direction)>  fInitializer;
                };
#endif


#if     qHasFeature_OpenSSL
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
                 *  @brief  OpenSSLOutputStream is a BinaryOutputStream which does OpenSSL-based encryption or decryption (depending on direction arg)
                 *
                 *  OpenSSLOutputStream is a BinaryOutputStream which wraps another BinaryOutputStream
                 *  and does OpenSSL-based  encryption or decryption (depending on direction arg).
                 *
                 *  Use OpenSSLOutputStream is you wish to produce an artifact (e.g. external file) as a result of incrementally writing
                 *  to a stream.
                 *
                 *  @see OpenSSLInputStream
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
