/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../OpenSSLCryptoStream.h"

#include "AES.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Encoding;
using namespace Stroika::Foundation::Cryptography::Encoding::Algorithm;
using namespace Stroika::Foundation::Cryptography::OpenSSL;

using Memory::BLOB;

#if qHasFeature_OpenSSL
namespace {
    OpenSSLCryptoParams cvt_ (const OpenSSL::DerivedKey& key, AESOptions options)
    {
        switch (options) {
            case AESOptions::e128_CBC:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CBC, key);
            case AESOptions::e128_ECB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_ECB, key);
            case AESOptions::e128_OFB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_OFB, key);
            case AESOptions::e128_CFB1:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CFB1, key);
            case AESOptions::e128_CFB8:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CFB8, key);
            case AESOptions::e128_CFB128:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_128_CFB128, key);
            case AESOptions::e192_CBC:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CBC, key);
            case AESOptions::e192_ECB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_ECB, key);
            case AESOptions::e192_OFB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_OFB, key);
            case AESOptions::e192_CFB1:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CFB1, key);
            case AESOptions::e192_CFB8:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CFB8, key);
            case AESOptions::e192_CFB128:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_192_CFB128, key);
            case AESOptions::e256_CBC:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CBC, key);
            case AESOptions::e256_ECB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_ECB, key);
            case AESOptions::e256_OFB:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_OFB, key);
            case AESOptions::e256_CFB1:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB1, key);
            case AESOptions::e256_CFB8:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB8, key);
            case AESOptions::e256_CFB128:
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB128, key);
            default:
                RequireNotReached ();
                return OpenSSLCryptoParams (CipherAlgorithm::eAES_256_CFB128, key);
        }
    }
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ***************************** Algorithm::DecodeAES *****************************
 ********************************************************************************
 */
Streams::InputStream<byte>::Ptr Algorithm::DecodeAES (const OpenSSL::DerivedKey& key, const Streams::InputStream<byte>::Ptr& in, AESOptions options)
{
    return OpenSSLInputStream::New (cvt_ (key, options), Direction::eDecrypt, in);
}
Memory::BLOB Algorithm::DecodeAES (const OpenSSL::DerivedKey& key, const Memory::BLOB& in, AESOptions options)
{
    return DecodeAES (key, in.As<Streams::InputStream<byte>::Ptr> (), options).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::EncodeAES ****************************
 ********************************************************************************
 */
Streams::InputStream<byte>::Ptr Algorithm::EncodeAES (const OpenSSL::DerivedKey& key, const Streams::InputStream<byte>::Ptr& in, AESOptions options)
{
    return OpenSSLInputStream::New (cvt_ (key, options), Direction::eEncrypt, in);
}
Memory::BLOB Algorithm::EncodeAES (const OpenSSL::DerivedKey& key, const Memory::BLOB& in, AESOptions options)
{
    return EncodeAES (key, in.As<Streams::InputStream<byte>::Ptr> (), options).ReadAll ();
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 **************************** Algorithm::AESEncoder *****************************
 ********************************************************************************
 */
Streams::OutputStream<byte>::Ptr Algorithm::AESDecoder (const OpenSSL::DerivedKey& key, const Streams::OutputStream<byte>::Ptr& out, AESOptions options)
{
    return OpenSSLOutputStream::New (cvt_ (key, options), Direction::eDecrypt, out);
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ****************************** Algorithm::AESEncoder ***************************
 ********************************************************************************
 */
Streams::OutputStream<byte>::Ptr Algorithm::AESEncoder (const OpenSSL::DerivedKey& key, const Streams::OutputStream<byte>::Ptr& out, AESOptions options)
{
    return OpenSSLOutputStream::New (cvt_ (key, options), Direction::eEncrypt, out);
}
#endif
