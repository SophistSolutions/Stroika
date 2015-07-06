/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../OpenSSLCryptoStream.h"

#include    "AES.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Encoding;
using   namespace   Stroika::Foundation::Cryptography::Encoding::Algorithm;
using   namespace   Stroika::Foundation::Cryptography::OpenSSL;



using   Memory::BLOB;

#if     qHasFeature_OpenSSL
namespace {
    OpenSSLCryptoParams cvt_ (const BLOB& key, AESOptions options)
    {
        switch (options) {
            case AESOptions::e128_CBC:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CBC, key, BLOB ());
            case AESOptions::e128_ECB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_ECB, key, BLOB ());
            case AESOptions::e128_OFB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_OFB, key, BLOB ());
            case AESOptions::e128_CFB1:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CFB1, key, BLOB ());
            case AESOptions::e128_CFB8:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CFB8, key, BLOB ());
            case AESOptions::e128_CFB128:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CFB128, key, BLOB ());
            case AESOptions::e192_CBC:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CBC, key, BLOB ());
            case AESOptions::e192_ECB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_ECB, key, BLOB ());
            case AESOptions::e192_OFB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_OFB, key, BLOB ());
            case AESOptions::e192_CFB1:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CFB1, key, BLOB ());
            case AESOptions::e192_CFB8:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CFB8, key, BLOB ());
            case AESOptions::e192_CFB128:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CFB128, key, BLOB ());
            case AESOptions::e256_CBC:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CBC, key, BLOB ());
            case AESOptions::e256_ECB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_ECB, key, BLOB ());
            case AESOptions::e256_OFB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_OFB, key, BLOB ());
            case AESOptions::e256_CFB1:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB1, key, BLOB ());
            case AESOptions::e256_CFB8:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB8, key, BLOB ());
            case AESOptions::e256_CFB128:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB128, key, BLOB ());
            default:
                RequireNotReached ();
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB128, key, BLOB ());
        }
    }
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ***************************** Algorithm::DecodeAES *****************************
 ********************************************************************************
 */
Streams::InputStream<Byte>  Algorithm::DecodeAES (const Memory::BLOB& key, Streams::InputStream<Byte> in, AESOptions options)
{
    return OpenSSLInputStream (cvt_ (key, options), Direction::eDecrypt, in);
}
Memory::BLOB  Algorithm::DecodeAES (const Memory::BLOB& key, const Memory::BLOB& in, AESOptions options)
{
    return DecodeAES (key, in.As<Streams::InputStream<Byte>> (), options).ReadAll ();
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeAES ****************************
 ********************************************************************************
 */
Streams::InputStream<Byte>  Algorithm::EncodeAES (const Memory::BLOB& key, Streams::InputStream<Byte> in, AESOptions options)
{
    return OpenSSLInputStream (cvt_ (key, options), Direction::eEncrypt, in);
}
Memory::BLOB  Algorithm::EncodeAES (const Memory::BLOB& key, const Memory::BLOB& in, AESOptions options)
{
    return EncodeAES (key, in.As<Streams::InputStream<Byte>> (), options).ReadAll ();
}
#endif







#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::AESEncoder *****************************
 ********************************************************************************
 */
Streams::OutputStream<Byte>  Algorithm::AESDecoder (const Memory::BLOB& key, Streams::OutputStream<Byte> out, AESOptions options)
{
    return OpenSSLOutputStream (cvt_ (key, options), Direction::eDecrypt, out);
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::AESEncoder ***************************
 ********************************************************************************
 */
Streams::OutputStream<Byte>  Algorithm::AESEncoder (const Memory::BLOB& key, Streams::OutputStream<Byte> out, AESOptions options)
{
    return OpenSSLOutputStream (cvt_ (key, options), Direction::eEncrypt, out);
}
#endif
