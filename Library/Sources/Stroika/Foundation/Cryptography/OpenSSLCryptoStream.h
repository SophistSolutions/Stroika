/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_
#define _Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_ 1

#include    "../StroikaPreComp.h"

#if     qHas_OpenSSL
#include <openssl/evp.h>
#endif

#include    "../Configuration/Common.h"
#include    "../Cryptography/SSL/Common.h"
#include    "../Memory/Common.h"
#include    "../Streams/BinaryInputStream.h"
#include    "../Streams/BinaryOutputStream.h"



/**
 *  \file
 *
 *      @todo   Very preliminary draft - not tested, and probably not even compiling....
 *
 *      @todo   Generate exceptions on errors
 *
 *		@todo	this module includes <openssl> stuff in the header. Add additioanl modules inside
 *				Crypto that just are called 'Blowfish', and 'rc2', and these have classes that tkae
 *				constructors with just the needed data = maybe not even ctors - maybe functions - that
 *				take a stream, and return a decrpting (or encyrpting) stream - with arg params that make
 *				sense for that algoritjm. They are only defined #if qSSLAvail, but otherwise include
 *				...
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {


#if     qHas_OpenSSL
            class   OpenSSLCryptoParams {
            public:
                // use this CTOR and fill in parameters manually for EVP_EncryptInit_ex
                OpenSSLCryptoParams (const std::function<void(EVP_CIPHER_CTX*, bool)>& f);

                enum class Algorithm {
                    eBlowfish_CBC,
                    eRC2,
                };
                // allowed Algorith's for this CTOR include 'eBlowfish_CBC, eRC2'
                OpenSSLCryptoParams (Algorithm alg, Memory::BLOB key, Memory::BLOB initialIV = Memory::BLOB ());

            public:
                std::function<void(EVP_CIPHER_CTX*, bool)>  fInitializer;
            };
#endif


#if     qHas_OpenSSL
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
                OpenSSLInputStream (const OpenSSLCryptoParams& cryptoParams, const BinaryInputStream& realIn);
            };
#endif


#if     qHas_OpenSSL
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
                OpenSSLOutputStream (const OpenSSLCryptoParams& cryptoParams, const BinaryOutputStream& realOut);
            };
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_OpenSSLCryptoStream_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "OpenSSLCryptoStream.inl"
