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



namespace {
    string          toPasswd_ (const String& s)
    {
        return s.AsUTF8 ();
    }
    Optional<BLOB>  toSalt_ (const Optional<string>& s)
    {
        // @todo NYI
        return Optional<BLOB> ();
    }
    Optional<BLOB>  toSalt_ (const Optional<String>& s)
    {
        // @todo NYI
        return Optional<BLOB> ();
    }
}




#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSL::EVP_BytesToKey **********************
 ********************************************************************************
 */
namespace {
    pair<BLOB, BLOB>    mkEVP_BytesToKey_ (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    {
        Require (nRounds >= 1);
        RequireNotNull (cipherAlgorithm);
        SmallStackBuffer<Byte> useKey   { static_cast<size_t> (cipherAlgorithm->key_len) };
        SmallStackBuffer<Byte> useIV    { static_cast<size_t> (cipherAlgorithm->iv_len) };
        int i = ::EVP_BytesToKey (cipherAlgorithm, Convert2OpenSSL (digestAlgorithm), salt ? salt.Value ().begin () : nullptr, passwd.first, static_cast<int> (passwd.second - passwd.first), nRounds, useKey.begin (), useIV.begin ());
        if (i == 0) {
            Cryptography::OpenSSL::Exception::ThrowLastError ();
        }
        return pair<BLOB, BLOB> (BLOB (useKey.begin (), useKey.end ()), BLOB (useIV.begin (), useIV.end ()));
    }
}
EVP_BytesToKey::EVP_BytesToKey (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : DerivedKey (mkEVP_BytesToKey_ (digestAlgorithm, cipherAlgorithm, passwd, nRounds, salt))
{
}

EVP_BytesToKey::EVP_BytesToKey (DigestAlgorithm digestAlgorithm, size_t keyLength, size_t ivLength, pair<const Byte*, const Byte*> passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : EVP_BytesToKey (digestAlgorithm, FakeCryptoAlgo_ (keyLength, ivLength), passwd, nRounds, salt)
{
}

EVP_BytesToKey::EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, pair<const Byte*, const Byte*> passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : EVP_BytesToKey (digestAlgorithm, Convert2OpenSSL (cipherAlgorithm), passwd, nRounds, salt)
{
}

EVP_BytesToKey::EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, BLOB passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : EVP_BytesToKey (digestAlgorithm, Convert2OpenSSL (cipherAlgorithm), passwd.As<pair<const Byte*, const Byte*>> (), nRounds, salt)
{
}

EVP_BytesToKey::EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const string& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : EVP_BytesToKey (digestAlgorithm, Convert2OpenSSL (cipherAlgorithm), pair<const Byte*, const Byte*> (reinterpret_cast<const Byte*> (passwd.c_str ()), reinterpret_cast<const Byte*> (passwd.c_str () + passwd.length ())), nRounds, salt)
{
}

EVP_BytesToKey::EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const String& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : EVP_BytesToKey (digestAlgorithm, cipherAlgorithm, toPasswd_ (passwd), nRounds, salt)
{
}




/*
 ********************************************************************************
 ****************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC ********************
 ********************************************************************************
 */
namespace {
    pair<BLOB, BLOB> mkPKCS5_PBKDF2_HMAC_ (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const string& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    {
        AssertNotImplemented ();    // almost - pretty easy
        SmallStackBuffer<Byte> outBuf   { static_cast<size_t> (keyLen + ivLen) };
        // @todo map salt
        Byte salt2[222];
        int a = ::PKCS5_PBKDF2_HMAC (passwd.c_str (), passwd.length (), salt2, sizeof(salt2), nRounds, Convert2OpenSSL (digestAlgorithm), keyLen + ivLen, outBuf.begin ());
        // todo fill in fKey/fIV!!! or throw if error
#if 0
        PKCS5_PBKDF2_HMAC(const char* pass, int passlen,
                          const unsigned char* salt, int saltlen, int iter,
                          const EVP_MD * digest, int keylen, unsigned char* out);
#endif
        const Byte* p = outBuf.begin ();
        return pair<BLOB, BLOB> (BLOB (p, p + keyLen), BLOB (p + keyLen, p + keyLen + ivLen));
    }

}
PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const string& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : DerivedKey (mkPKCS5_PBKDF2_HMAC_ (keyLen, ivLen, digestAlgorithm, passwd, nRounds, salt))
{
}

PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const String& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : PKCS5_PBKDF2_HMAC (keyLen, ivLen, digestAlgorithm, toPasswd_ (passwd), nRounds, salt)
{
}

PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const string& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : DerivedKey (mkPKCS5_PBKDF2_HMAC_ (Convert2OpenSSL (cipherAlgorithm)->key_len, Convert2OpenSSL (cipherAlgorithm)->iv_len, digestAlgorithm, passwd, nRounds, salt))
{
}

PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const String& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
    : PKCS5_PBKDF2_HMAC (Convert2OpenSSL (cipherAlgorithm)->key_len, Convert2OpenSSL (cipherAlgorithm)->iv_len, digestAlgorithm, toPasswd_ (passwd), nRounds, salt)
{
}
#endif
