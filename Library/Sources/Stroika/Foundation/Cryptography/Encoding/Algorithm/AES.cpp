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



#if     qHasFeature_OpenSSL
namespace {
    OpenSSLCryptoParams cvt_ (const Memory::BLOB& key, AESOptions options)
    {
        switch (options) {
            case AESOptions::e128_CBC:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_128_CBC, key);
            case AESOptions::e128_ECB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_128_ECB, key);
            case AESOptions::e128_OFB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_128_OFB, key);
            case AESOptions::e128_CFB1:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_128_CFB1, key);
            case AESOptions::e128_CFB8:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_128_CFB8, key);
            case AESOptions::e128_CFB128:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_128_CFB128, key);
            case AESOptions::e192_CBC:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_192_CBC, key);
            case AESOptions::e192_ECB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_192_ECB, key);
            case AESOptions::e192_OFB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_192_OFB, key);
            case AESOptions::e192_CFB1:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_192_CFB1, key);
            case AESOptions::e192_CFB8:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_192_CFB8, key);
            case AESOptions::e192_CFB128:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_192_CFB128, key);
            case AESOptions::e256_CBC:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_256_CBC, key);
            case AESOptions::e256_ECB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_256_ECB, key);
            case AESOptions::e256_OFB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_256_OFB, key);
            case AESOptions::e256_CFB1:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_256_CFB1, key);
            case AESOptions::e256_CFB8:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_256_CFB8, key);
            case AESOptions::e256_CFB128:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_256_CFB128, key);
            default:
                RequireNotReached ();
                return OpenSSLCryptoParams (OpenSSLCryptoParams::CipherAlgorithm::eAES_256_CFB128, key);
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
Streams::BinaryInputStream  Algorithm::DecodeAES (const Memory::BLOB& key, Streams::BinaryInputStream in, AESOptions options)
{
    return OpenSSLInputStream (cvt_ (key, options), Direction::eDecrypt, in);
}
Memory::BLOB  Algorithm::DecodeAES (const Memory::BLOB& key, const Memory::BLOB& in, AESOptions options)
{
    return DecodeAES (key, in.As<Streams::BinaryInputStream> (), options).ReadAll ();
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeAES ****************************
 ********************************************************************************
 */
Streams::BinaryInputStream  Algorithm::EncodeAES (const Memory::BLOB& key, Streams::BinaryInputStream in, AESOptions options)
{
    return OpenSSLInputStream (cvt_ (key, options), Direction::eEncrypt, in);
}
Memory::BLOB  Algorithm::EncodeAES (const Memory::BLOB& key, const Memory::BLOB& in, AESOptions options)
{
    return EncodeAES (key, in.As<Streams::BinaryInputStream> (), options).ReadAll ();
}
#endif







#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::AESEncoder *****************************
 ********************************************************************************
 */
Streams::BinaryOutputStream  Algorithm::AESDecoder (const Memory::BLOB& key, Streams::BinaryOutputStream out, AESOptions options)
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
Streams::BinaryOutputStream  Algorithm::AESEncoder (const Memory::BLOB& key, Streams::BinaryOutputStream out, AESOptions options)
{
    return OpenSSLOutputStream (cvt_ (key, options), Direction::eEncrypt, out);
}
#endif
