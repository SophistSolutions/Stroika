/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/MD5.h"
#include "Stroika/Foundation/Cryptography/Digest/Digester.h"
#include "Stroika/Foundation/Cryptography/Digest/Hash.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "Exception.h"

#include "DerivedKey.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Memory;

using Memory::BLOB;
using Memory::StackBuffer;

#if qHasFeature_OpenSSL && defined(_MSC_VER)
// Use #pragma comment lib instead of explicit entry in the lib entry of the project file
#if OPENSSL_VERSION_NUMBER < 0x1010000fL
#pragma comment(lib, "libeay32.lib")
#pragma comment(lib, "ssleay32.lib")
#else
#pragma comment(lib, "libcrypto.lib")
#pragma comment(lib, "libssl.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "crypt32.lib")
#endif
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ********************** Cryptography::OpenSSL::DerivedKey ***********************
 ********************************************************************************
 */
String DerivedKey::ToString () const
{
    Characters::StringBuilder result;
    result << "{"sv;
    result << "key: "sv << fKey << ", "sv;
    result << "IV: "sv << fIV;
    result << "}"sv;
    return result.str ();
}
#endif

/*
 ********************************************************************************
 **************** Cryptography::OpenSSL::WinCryptDeriveKey **********************
 ********************************************************************************
 */
#if qHasFeature_OpenSSL
namespace {
    pair<BLOB, BLOB> mkWinCryptDeriveKey_ (size_t keyLen, [[maybe_unused]] DigestAlgorithm digestAlgorithm, const BLOB& passwd)
    {
        // @todo http://stroika-bugs.sophists.com/browse/STK-192
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
        size_t      usePWDLen     = min (passwd.length (), static_cast<size_t> (64));
        const byte* passwordBytes = passwd.begin ();
        byte        buf1[64];
        {
            std::fill_n (buf1, NEltsOf (buf1), static_cast<byte> (0x36));
            for (unsigned long i = 0; i < usePWDLen; ++i) {
                buf1[i] ^= passwordBytes[i];
            }
        }
        byte buf2[64];
        {
            std::fill_n (buf2, NEltsOf (buf2), static_cast<byte> (0x5C));
            for (unsigned long i = 0; i < usePWDLen; ++i) {
                buf2[i] ^= passwordBytes[i];
            }
        }
        Require (digestAlgorithm == DigestAlgorithms::kMD5); // else NYI
        Digest::Algorithm::DigesterAlgorithm<Digest::Algorithm::MD5>::ReturnType encodedResults[] = {
            Digest::ComputeDigest<Digest::Algorithm::MD5> (begin (buf1), end (buf1)),
            Digest::ComputeDigest<Digest::Algorithm::MD5> (begin (buf2), end (buf2))};
        Assert (keyLen <= sizeof (encodedResults)); // NYI otherwise - but we could zero fill
        const byte* encodedResultBytes = reinterpret_cast<const byte*> (begin (encodedResults));
        BLOB        resultKey{encodedResultBytes, encodedResultBytes + std::min (sizeof (encodedResults), keyLen)};
        BLOB        iv;
        return pair<BLOB, BLOB>{resultKey, iv};
    }
    size_t mkDefKeyLen_ (WinCryptDeriveKey::Provider provider, CipherAlgorithm cipherAlgorithm)
    {
        // @todo see table https://msdn.microsoft.com/en-us/library/aa379916.aspx

        switch (provider) {
            case WinCryptDeriveKey::Provider::Base: {
                if (cipherAlgorithm == CipherAlgorithms::kRC2_CBC () or cipherAlgorithm == CipherAlgorithms::kRC2_CFB () or
                    cipherAlgorithm == CipherAlgorithms::kRC2_ECB () or cipherAlgorithm == CipherAlgorithms::kRC2_OFB () or
                    cipherAlgorithm == CipherAlgorithms::kRC4 ()) {
                    return 40 / 8;
                }
#if 0
                        case    CipherAlgorithm::eDES {
                                return 56 / 8;
                            }
#endif
            } break;
            case WinCryptDeriveKey::Provider::Enhanced: {
                if (cipherAlgorithm == CipherAlgorithms::kRC2_CBC () or cipherAlgorithm == CipherAlgorithms::kRC2_CFB () or
                    cipherAlgorithm == CipherAlgorithms::kRC2_ECB () or cipherAlgorithm == CipherAlgorithms::kRC2_OFB () or
                    cipherAlgorithm == CipherAlgorithms::kRC4 ()) {
                    return 128 / 8;
                }
            } break;
        }
        AssertNotImplemented (); // incomplete set of defautl see above table
        return 128 / 8;
    }
}
WinCryptDeriveKey::WinCryptDeriveKey (size_t keyLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd)
    : DerivedKey{mkWinCryptDeriveKey_ (keyLen, digestAlgorithm, passwd)}
{
}

WinCryptDeriveKey::WinCryptDeriveKey (Provider provider, CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const BLOB& passwd)
    : DerivedKey{WinCryptDeriveKey{mkDefKeyLen_ (provider, cipherAlgorithm), digestAlgorithm, passwd}}
{
}
#endif

#if qHasFeature_OpenSSL
/*
 ********************************************************************************
 ******************* Cryptography::OpenSSL::EVP_BytesToKey **********************
 ********************************************************************************
 */
namespace {
    pair<BLOB, BLOB> mkEVP_BytesToKey_ (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const BLOB& passwd,
                                        unsigned int nRounds, const optional<BLOB>& salt)
    {
        Require (nRounds >= 1);
        StackBuffer<byte> useKey{Memory::eUninitialized, cipherAlgorithm.KeyLength ()};
        StackBuffer<byte> useIV{Memory::eUninitialized, cipherAlgorithm.IVLength ()};
        if (salt and salt->GetSize () != 8) [[unlikely]] {
            // Could truncate and fill to adapt to different sized salt...
            Execution::Throw (Execution::Exception{"only 8-byte salt with EVP_BytesToKey"sv});
        }
        int i = ::EVP_BytesToKey (cipherAlgorithm, digestAlgorithm,
                                  reinterpret_cast<const unsigned char*> (salt ? NullCoalesce (salt).begin () : nullptr),
                                  reinterpret_cast<const unsigned char*> (passwd.begin ()), static_cast<int> (passwd.size ()), nRounds,
                                  reinterpret_cast<unsigned char*> (useKey.begin ()), reinterpret_cast<unsigned char*> (useIV.begin ()));
        Assert (i >= 0);
        if (i == 0) {
            Cryptography::OpenSSL::Exception::ThrowLastError ();
        }
        Assert (i == static_cast<int> (cipherAlgorithm.KeyLength ()));
        return pair<BLOB, BLOB>{BLOB{useKey.begin (), useKey.end ()}, BLOB{useIV.begin (), useIV.end ()}};
    }
}
template <>
EVP_BytesToKey::EVP_BytesToKey (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const BLOB& passwd, unsigned int nRounds,
                                const optional<BLOB>& salt)
    : DerivedKey{mkEVP_BytesToKey_ (cipherAlgorithm, digestAlgorithm, passwd, nRounds, salt)}
{
}

/*
 ********************************************************************************
 ****************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC ********************
 ********************************************************************************
 */
namespace {
    pair<BLOB, BLOB> mkPKCS5_PBKDF2_HMAC_ (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd,
                                           unsigned int nRounds, const optional<BLOB>& salt)
    {
        StackBuffer<byte> outBuf{Memory::eUninitialized, keyLen + ivLen};
        Assert (keyLen + ivLen < size_t (numeric_limits<int>::max ())); // for static cast below
        int a = ::PKCS5_PBKDF2_HMAC (reinterpret_cast<const char*> (passwd.begin ()), static_cast<int> (passwd.length ()),
                                     reinterpret_cast<const unsigned char*> (salt ? salt->begin () : nullptr),
                                     static_cast<int> (salt ? salt->size () : 0), nRounds, digestAlgorithm,
                                     static_cast<int> (keyLen + ivLen), reinterpret_cast<unsigned char*> (outBuf.begin ()));
        if (a == 0) [[unlikely]] {
            Execution::Throw (Execution::Exception{"PKCS5_PBKDF2_HMAC error"sv});
        }
        const byte* p = outBuf.begin ();
        return pair<BLOB, BLOB> (BLOB{p, p + keyLen}, BLOB{p + keyLen, p + keyLen + ivLen});
    }
}
template <>
PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const BLOB& passwd,
                                      unsigned int nRounds, const optional<BLOB>& salt)
    : DerivedKey{mkPKCS5_PBKDF2_HMAC_ (keyLen, ivLen, digestAlgorithm, passwd, nRounds, salt)}
{
}
#endif
