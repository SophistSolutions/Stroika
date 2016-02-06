/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qHasFeature_OpenSSL
#include    <openssl/evp.h>
#endif

#include    "../../Containers/Common.h"
#include    "../../Debug/Assertions.h"
#include    "../../Execution/Common.h"
#include    "../../Execution/Synchronized.h"
#include    "../../Memory/SmallStackBuffer.h"

#include    "Exception.h"

#include    "DerivedKey.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Cryptography;
using   namespace   Stroika::Foundation::Cryptography::OpenSSL;
using   namespace   Stroika::Foundation::Memory;

using   Memory::BLOB;
using   Memory::SmallStackBuffer;


#if     qHasFeature_OpenSSL && defined (_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#pragma comment (lib, "libeay32.lib")
#pragma comment (lib, "ssleay32.lib")
#endif





#if     qHasFeature_OpenSSL
namespace {
    // This trick counts on the fact that EVP_BytesToKey() only ever looks at key_len and iv_len
    struct  FakeCryptoAlgo_ : EVP_CIPHER {
        FakeCryptoAlgo_ (size_t keyLength, size_t ivLength)
        {
            (void)::memset (this, 0, sizeof (*this));
            DISABLE_COMPILER_MSC_WARNING_START (4267)
            this->key_len = keyLength;
            this->iv_len = ivLength;
            DISABLE_COMPILER_MSC_WARNING_END (4267)
        }
        operator const EVP_CIPHER* () const
        {
            return this;
        }
    };
}
#endif







#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ********************** Cryptography::OpenSSL::DerivedKey ***********************
 ********************************************************************************
 */
size_t  DerivedKey::KeyLength (CipherAlgorithm cipherAlgorithm)
{
    return Convert2OpenSSL (cipherAlgorithm)->key_len;
}
size_t  DerivedKey::KeyLength (const EVP_CIPHER* cipherAlgorithm)
{
    return cipherAlgorithm->key_len;
}

size_t  DerivedKey::IVLength (CipherAlgorithm cipherAlgorithm)
{
    return Convert2OpenSSL (cipherAlgorithm)->iv_len;
}
size_t  DerivedKey::IVLength (const EVP_CIPHER* cipherAlgorithm)
{
    return cipherAlgorithm->iv_len;
}
#endif






#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSL::EVP_BytesToKey **********************
 ********************************************************************************
 */
namespace {
    pair<BLOB, BLOB>    mkEVP_BytesToKey_ (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    {
        Require (nRounds >= 1);
        SmallStackBuffer<Byte> useKey   { keyLen };
        SmallStackBuffer<Byte> useIV    { ivLen };
        if (salt and salt->GetSize () != 8) {
            // Could truncate and fill to adapt to differnt sized salt...
            Execution::Throw (Execution::StringException (L"only 8-byte salt with EVP_BytesToKey"));
        }
        int i = ::EVP_BytesToKey (FakeCryptoAlgo_ (keyLen, ivLen), Convert2OpenSSL (digestAlgorithm), salt ? salt.Value ().begin () : nullptr, passwd.begin (), static_cast<int> (passwd.size ()), nRounds, useKey.begin (), useIV.begin ());
        if (i == 0) {
            Cryptography::OpenSSL::Exception::ThrowLastError ();
        }
        return pair<BLOB, BLOB> (BLOB (useKey.begin (), useKey.end ()), BLOB (useIV.begin (), useIV.end ()));
    }
}
template    <>
EVP_BytesToKey::EVP_BytesToKey (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : DerivedKey (mkEVP_BytesToKey_ (keyLen, ivLen, digestAlgorithm, passwd, nRounds, salt))
{
}





/*
 ********************************************************************************
 ****************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC ********************
 ********************************************************************************
 */
namespace {
    pair<BLOB, BLOB> mkPKCS5_PBKDF2_HMAC_ (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    {
        SmallStackBuffer<Byte> outBuf   { keyLen + ivLen };
        int a = ::PKCS5_PBKDF2_HMAC (reinterpret_cast<const char*> (passwd.begin ()), passwd.length (), salt ? salt->begin () : nullptr, salt ? salt->size () : 0, nRounds, Convert2OpenSSL (digestAlgorithm), keyLen + ivLen, outBuf.begin ());
        if (a == 0) {
            Execution::Throw (Execution::StringException (L"PKCS5_PBKDF2_HMAC error"));
        }
        const Byte* p = outBuf.begin ();
        return pair<BLOB, BLOB> (BLOB (p, p + keyLen), BLOB (p + keyLen, p + keyLen + ivLen));
    }
}

template    <>
PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : DerivedKey (mkPKCS5_PBKDF2_HMAC_ (keyLen, ivLen, digestAlgorithm, passwd, nRounds, salt))
{
}
#endif
