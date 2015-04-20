/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
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
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSL::DerivedKey **************************
 ********************************************************************************
 */
DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt, unsigned int nRounds)
{
    Require (nRounds >= 1);
    RequireNotNull (cipherAlgorithm);

    SmallStackBuffer<Byte> useKey   { static_cast<size_t> (cipherAlgorithm->key_len) };
    SmallStackBuffer<Byte> useIV    { static_cast<size_t> (cipherAlgorithm->iv_len) };

    int i = ::EVP_BytesToKey (cipherAlgorithm, Convert2OpenSSL (digestAlgorithm), salt ? &salt.Value ().at (0) : nullptr, passwd.first, passwd.second - passwd.first, nRounds, useKey.begin (), useIV.begin ());
    if (i == 0) {
        Cryptography::OpenSSL::Exception::DoThrowLastError ();
    }
    fKey = BLOB (useKey.begin (), useKey.end ());
    fIV = BLOB (useIV.begin (), useIV.end ());
}

namespace {
    // This trick counts on the fact that EVP_BytesToKey() only ever looks at key_len and iv_len
    struct  FakeCryptoAlgo_ : EVP_CIPHER {
        FakeCryptoAlgo_ (size_t keyLength, size_t ivLength)
        {
            memset (this, 0, sizeof (*this));
            this->key_len = keyLength;
            this->iv_len = ivLength;
        }
        operator const EVP_CIPHER* () const
        {
            return this;
        }
    };
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

DerivedKey::DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const wstring& passwd, const Optional<SaltType>& salt, unsigned int nRounds)
    : DerivedKey (digestAlgorithm, Convert2OpenSSL (cipherAlgorithm), pair<const Byte*, const Byte*> (reinterpret_cast<const Byte*> (passwd.c_str ()), reinterpret_cast<const Byte*> (passwd.c_str () + passwd.length ())), salt, nRounds)
{
}
#endif
