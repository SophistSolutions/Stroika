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
 ******************* Cryptography::OpenSSL::DerivedKey **************************
 ********************************************************************************
 */
DerivedKey::DerivedKey (KeyDerivationStrategy keyDerivationStrategy, DigestAlgorithm digestAlgorithm, const string& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
{
	if (keyDerivationStrategy == KeyDerivationStrategy::PKCS5_PBKDF2_HMAC) {
		size_t k = 1024;
		Byte buf[1024];
		Byte salt2[222];
		int a = ::PKCS5_PBKDF2_HMAC (passwd.c_str (), passwd.length (), salt2, sizeof(salt2), nRounds, Convert2OpenSSL (digestAlgorithm), k, buf);
#if 0
			PKCS5_PBKDF2_HMAC(const char *pass, int passlen,
                      const unsigned char *salt, int saltlen, int iter,
                      const EVP_MD *digest, int keylen, unsigned char *out);
#endif
	}
	else {
		AssertNotReached ();
	}
}

DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt, unsigned int nRounds)
{
    Require (nRounds >= 1);
    RequireNotNull (cipherAlgorithm);
    SmallStackBuffer<Byte> useKey   { static_cast<size_t> (cipherAlgorithm->key_len) };
    SmallStackBuffer<Byte> useIV    { static_cast<size_t> (cipherAlgorithm->iv_len) };
    int i = ::EVP_BytesToKey (cipherAlgorithm, Convert2OpenSSL (digestAlgorithm), salt ? &salt.Value ().at (0) : nullptr, passwd.first, static_cast<int> (passwd.second - passwd.first), nRounds, useKey.begin (), useIV.begin ());
    if (i == 0) {
        Cryptography::OpenSSL::Exception::ThrowLastError ();
    }
    fKey = BLOB (useKey.begin (), useKey.end ());
    fIV = BLOB (useIV.begin (), useIV.end ());
}

DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, size_t keyLength, size_t ivLength, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt, unsigned int nRounds)
    : DerivedKey (digestAlgorithm, FakeCryptoAlgo_ (keyLength, ivLength), passwd, salt, nRounds)
{
}

DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt, unsigned int nRounds)
    : DerivedKey (digestAlgorithm, Convert2OpenSSL (cipherAlgorithm), passwd, salt, nRounds)
{
}

DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, BLOB passwd, const Optional<SaltType>& salt, unsigned int nRounds)
    : DerivedKey (digestAlgorithm, cipherAlgorithm, passwd.As<pair<const Byte*, const Byte*>> (), salt, nRounds)
{
}

DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const string& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
    : DerivedKey (digestAlgorithm, Convert2OpenSSL (cipherAlgorithm), pair<const Byte*, const Byte*> (reinterpret_cast<const Byte*> (passwd.c_str ()), reinterpret_cast<const Byte*> (passwd.c_str () + passwd.length ())), salt, nRounds)
{
}

DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const String& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
    : DerivedKey (digestAlgorithm, cipherAlgorithm, passwd.AsUTF8 (), salt, nRounds)
{
}







/*
 ********************************************************************************
 *************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC_SHA1 ******************
 ********************************************************************************
 */
PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (DigestAlgorithm digestAlgorithm, const string& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
	: DerivedKey (KeyDerivationStrategy::PKCS5_PBKDF2_HMAC, digestAlgorithm, passwd, salt, nRounds)
{
}

PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (DigestAlgorithm digestAlgorithm, const String& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
	: DerivedKey (KeyDerivationStrategy::PKCS5_PBKDF2_HMAC, digestAlgorithm, passwd.AsUTF8 (), salt, nRounds)
{
}





/*
 ********************************************************************************
 *************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC_SHA1 ******************
 ********************************************************************************
 */
PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (const string& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
	: PKCS5_PBKDF2_HMAC (DigestAlgorithm::eSHA1, passwd, salt, nRounds)
{
}
PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (const String& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
	: PKCS5_PBKDF2_HMAC (DigestAlgorithm::eSHA1, passwd, salt, nRounds)
{
}
#endif
