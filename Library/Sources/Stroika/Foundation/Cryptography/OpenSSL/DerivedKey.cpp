/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../../StroikaPreComp.h"

#if     qHasFeature_OpenSSL
#include    <openssl/evp.h>
#include    <openssl/md5.h>
#endif

#include    "../../Characters/StringBuilder.h"
#include    "../../Characters/ToString.h"
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
    RequireNotNull (cipherAlgorithm);
    return cipherAlgorithm->key_len;
}

size_t  DerivedKey::IVLength (CipherAlgorithm cipherAlgorithm)
{
    return Convert2OpenSSL (cipherAlgorithm)->iv_len;
}

size_t  DerivedKey::IVLength (const EVP_CIPHER* cipherAlgorithm)
{
    RequireNotNull (cipherAlgorithm);
    return cipherAlgorithm->iv_len;
}

String  DerivedKey::ToString () const
{
    Characters::StringBuilder  result;
    result += L"{";
    result += L"fKey: " + Characters::ToString (fKey);
    result += L", ";
    result += L"fIV: " + Characters::ToString (fIV);
    result += L"}";
    return result.str ();
}
#endif





/*
 ********************************************************************************
 **************** Cryptography::OpenSSL::WinCryptDeriveKey **********************
 ********************************************************************************
 */
#if     qHasFeature_OpenSSL
namespace {
    pair<BLOB, BLOB>    mkWinCryptDeriveKey_ (size_t keyLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd, const Optional<BLOB>& salt)
    {
        // @todo https://stroika.atlassian.net/browse/STK-192
        /*
         *  From http://msdn2.microsoft.com/en-us/library/aa379916.aspx
         *
         *      o   Form a 64-byte buffer by repeating the constant 0x36 64 times.
         *          Let k be the length of the hash value that is represented by the
         *          input parameter hBaseData. Set the first k bytes of the buffer to the result
         *          of an XOR operation of the first k bytes of the buffer with the hash value that
         *          is represented by the input parameter hBaseData.
         *      o   Form a 64-byte buffer by repeating the constant 0x5C 64 times.
         *          Set the first k bytes of the buffer to the result of an XOR operation of the
         *          first k bytes of the buffer with the hash value that is represented by the input parameter hBaseData.
         *      o   Hash the result of step 1 by using the same hash algorithm as that used to compute the
         *          hash value that is represented by the hBaseData parameter.
         *      o   Hash the result of step 2 by using the same hash algorithm as that used
         *          to compute the hash value that is represented by the hBaseData parameter.
         *      o   Concatenate the result of step 3 with the result of step 4.
         *      o   Use the first n bytes of the result of step 5 as the derived key.
         */
        size_t      usePWDLen = min (passwd.length (), 64u);
        const Byte* passwordBytes = passwd.begin ();
        unsigned char buf1[64];
        {
            std::fill_n (buf1, NEltsOf (buf1), 0x36);
            for (unsigned long i = 0; i < usePWDLen; ++i) {
                buf1[i] ^= passwordBytes[i];
            }
        }
        unsigned char buf2[64];
        {
            std::fill_n (buf2, NEltsOf (buf2), 0x5C);
            for (unsigned long i = 0; i < usePWDLen; ++i) {
                buf2[i] ^= passwordBytes[i];
            }
        }
        Require (digestAlgorithm == DigestAlgorithm::eMD5); // else NYI
        Byte md5OutputBuf[2 * MD5_DIGEST_LENGTH];
        (void)::MD5 (buf1, NEltsOf (buf1), md5OutputBuf);
        (void)::MD5 (buf2, NEltsOf (buf2), md5OutputBuf + MD5_DIGEST_LENGTH);
        Assert (keyLen <= NEltsOf (md5OutputBuf));      // NYI otherwise - but we could zero fill
        BLOB    resultKey { begin (md5OutputBuf), begin (md5OutputBuf) + std::min (NEltsOf (md5OutputBuf), keyLen) };
        BLOB    iv;
        return pair<BLOB, BLOB> { resultKey, iv };
    }
    size_t  mkDefKeyLen_ (WinCryptDeriveKey::Provider provider, CipherAlgorithm cipherAlgorithm)
    {
        // @todo see table https://msdn.microsoft.com/en-us/library/aa379916.aspx
        return 128 / 8;
    }
}
WinCryptDeriveKey::WinCryptDeriveKey (size_t keyLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd, const Optional<BLOB>& salt)
    : DerivedKey (mkWinCryptDeriveKey_ (keyLen, digestAlgorithm, passwd, salt))
{
}

WinCryptDeriveKey::WinCryptDeriveKey (Provider provider, CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const BLOB& passwd, const Optional<BLOB>& salt)
    : DerivedKey (WinCryptDeriveKey (mkDefKeyLen_ (provider, cipherAlgorithm), digestAlgorithm, passwd, salt))
{
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
        Assert (i == keyLen);
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
