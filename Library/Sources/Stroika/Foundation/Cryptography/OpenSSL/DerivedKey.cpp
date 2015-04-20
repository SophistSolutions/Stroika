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
DerivedKey::DerivedKey (CipherAlgorithm alg, DigestAlgorithm hashAlg, pair<const Byte*, const Byte*> passwd, unsigned int nRounds)
{
    Require (nRounds >= 1);
    const unsigned char* salt = nullptr;   // null or 8byte value

    const EVP_CIPHER* useOpenSSLCipher    =   Convert2OpenSSL (alg);
    AssertNotNull (useOpenSSLCipher);

    Memory::SmallStackBuffer<Byte> useKey   { static_cast<size_t> (useOpenSSLCipher->key_len) };
    Memory::SmallStackBuffer<Byte> useIV    { static_cast<size_t> (useOpenSSLCipher->iv_len) };

    int i = ::EVP_BytesToKey (useOpenSSLCipher, Convert2OpenSSL (hashAlg), salt, passwd.first, passwd.second - passwd.first, nRounds, useKey.begin (), useIV.begin ());
    if (i == 0) {
        Cryptography::OpenSSL::Exception::DoThrowLastError ();
    }
    fKey = BLOB (useKey.begin (), useKey.end ());
    fIV = BLOB (useIV.begin (), useIV.end ());
}

DerivedKey::DerivedKey (CipherAlgorithm alg, DigestAlgorithm hashAlg, const string& passwd, unsigned int nRounds)
    : DerivedKey (alg, hashAlg, pair<const Byte*, const Byte*> (reinterpret_cast<const Byte*> (passwd.c_str ()), reinterpret_cast<const Byte*> (passwd.c_str ()) + passwd.length ()), nRounds)
{
}
#endif
