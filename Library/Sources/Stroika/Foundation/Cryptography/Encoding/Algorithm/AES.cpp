/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../../../StroikaPreComp.h"

#include    "../OpenSSLCryptoStream.h"

#include    "AES.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::Encoding;
using   namespace   Stroika::Foundation::Cryptography::Encoding::Algorithm;



#if     qHas_OpenSSL
namespace {
    OpenSSLCryptoParams cvt_ (const Memory::BLOB& key, AESOptions options, OpenSSLCryptoParams::Direction direction)
    {
        switch (options) {
            case AESOptions::e128_CBC:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_128_CBC, key, direction);
            case AESOptions::e128_ECB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_128_ECB, key, direction);
            case AESOptions::e128_OFB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_128_OFB, key, direction);
            case AESOptions::e128_CFB1:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_128_CFB1, key, direction);
            case AESOptions::e128_CFB8:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_128_CFB8, key, direction);
            case AESOptions::e128_CFB128:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_128_CFB128, key, direction);
            case AESOptions::e192_CBC:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_192_CBC, key, direction);
            case AESOptions::e192_ECB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_192_ECB, key, direction);
            case AESOptions::e192_OFB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_192_OFB, key, direction);
            case AESOptions::e192_CFB1:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_192_CFB1, key, direction);
            case AESOptions::e192_CFB8:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_192_CFB8, key, direction);
            case AESOptions::e192_CFB128:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_192_CFB128, key, direction);
            case AESOptions::e256_CBC:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_256_CBC, key, direction);
            case AESOptions::e256_ECB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_256_ECB, key, direction);
            case AESOptions::e256_OFB:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_256_OFB, key, direction);
            case AESOptions::e256_CFB1:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_256_CFB1, key, direction);
            case AESOptions::e256_CFB8:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_256_CFB8, key, direction);
            case AESOptions::e256_CFB128:
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_256_CFB128, key, direction);
            default:
                RequireNotReached ();
                return OpenSSLCryptoParams (OpenSSLCryptoParams::Algorithm::eAES_256_CFB128, key, direction);
        }
    }
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 ***************************** Algorithm::DecodeAES *****************************
 ********************************************************************************
 */
Streams::BinaryInputStream  Algorithm::DecodeAES (const Memory::BLOB& key, Streams::BinaryInputStream in, AESOptions options)
{
    return OpenSSLInputStream (cvt_ (key, options, OpenSSLCryptoParams::Direction::eDecrypt), in);
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeAES ****************************
 ********************************************************************************
 */
Streams::BinaryInputStream  Algorithm::EncodeAES (const Memory::BLOB& key, Streams::BinaryInputStream in, AESOptions options)
{
    return OpenSSLInputStream (cvt_ (key, options, OpenSSLCryptoParams::Direction::eEncrypt), in);
}
#endif







#if     qHas_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::AESEncoder *****************************
 ********************************************************************************
 */
Streams::BinaryOutputStream  Algorithm::AESDecoder (const Memory::BLOB& key, Streams::BinaryOutputStream out, AESOptions options)
{
    return OpenSSLOutputStream (cvt_ (key, options, OpenSSLCryptoParams::Direction::eDecrypt), out);
}
#endif





#if     qHas_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::AESEncoder ***************************
 ********************************************************************************
 */
Streams::BinaryOutputStream  Algorithm::AESEncoder (const Memory::BLOB& key, Streams::BinaryOutputStream out, AESOptions options)
{
    return OpenSSLOutputStream (cvt_ (key, options, OpenSSLCryptoParams::Direction::eEncrypt), out);
}
#endif










