/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_h_ 1

#include    "../../StroikaPreComp.h"

#include    "../../Characters/String.h"
#include    "../../Configuration/Common.h"
#include    "../../Memory/BLOB.h"
#include    "../../Memory/Common.h"
#include    "../../Memory/Optional.h"

#include    "CipherAlgorithm.h"
#include    "DigestAlgorithm.h"



/**
 *  \file
 *          \note VERY ROUGH PRELIMINARY DRAFT - NOT USEABLE YET
 *
 *  TODO:
 *
 *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace OpenSSL {


                using   Characters::String;
                using   Memory::BLOB;
                using   Memory::Byte;
                using   Memory::Optional;


#if     qHasFeature_OpenSSL
                /**
                 */
                struct  DerivedKey {
                    BLOB    fKey;
                    BLOB    fIV;

                    /*
                     * Gen key & IV. This requires the cipher algorithm (for the key / iv length) and the hash algorithm.
                     * nrounds is the number of times the we hash the material. More rounds are more secure but
                     * slower.
                     *
                     *  For the string overload, we treat the strings as an array of bytes (len bytes) long.
                     *  For the String overload, we convert to UTF8 and treat as string (so L"fred" and "fred" produce the same thing).
                     */
                    DerivedKey (const BLOB& key, const BLOB& iv);
                    DerivedKey (const pair<BLOB, BLOB>& keyAndIV);
                };


                /**
                 *  CryptDeriveKey CAN be object sliced. Its a simple construction wrapper on a DerivedKey. CryptDeriveKey
                 *  creates a Microsoft-Windows format derived key, compatible with the Windows CryptDeriveKey API.
                 *
                 *  \note for new code - PKCS5_PBKDF2_HMAC is the preferred DerviveKey subclass to use
                 */
                struct  CryptDeriveKey : DerivedKey {
                    CryptDeriveKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const string& passwd, const Optional<BLOB>& salt = Optional<BLOB> ());
                };


                /**
                 *  EVP_BytesToKey CAN be object sliced. Its a simple construction wrapper on a DerivedKey. EVP_BytesToKey
                 *  creates an OpenSSL-default-format, old-style crypto derived key.
                 *
                 *  \note for new code - PKCS5_PBKDF2_HMAC is the preferred DerviveKey subclass to use
                 */
                struct  EVP_BytesToKey : DerivedKey {
                    /**
                     *  In EVP_BytesToKey, the Salt must either by an 8-byte array or omitted.
                     */
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, unsigned int nRounds = 1, const Optional<BLOB>& salt = Optional<BLOB> ());
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, size_t keyLength, size_t ivLength, pair<const Byte*, const Byte*> passwd, unsigned int nRounds = 1, const Optional<BLOB>& salt = Optional<BLOB> ());
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, pair<const Byte*, const Byte*> passwd, unsigned int nRounds = 1, const Optional<BLOB>& salt = Optional<BLOB> ());
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, BLOB passwd, unsigned int nRounds = 1, const Optional<BLOB>& salt = Optional<BLOB> ());
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const string& passwd, unsigned int nRounds = 1, const Optional<BLOB>& salt = Optional<BLOB> ());
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const String& passwd, unsigned int nRounds = 1, const Optional<BLOB>& salt = Optional<BLOB> ());
                };


                /**
                 *  PKCS5_PBKDF2_HMAC CAN be object sliced. Its a simple construction wrapper on a DerivedKey. PKCS5_PBKDF1
                 *  creates a PKCS5 PBKDF2 HMAC crypto derived key.
                 *
                 *  RFC 2898 suggests an iteration count of at least 1000
                 *
                 *  This function needs to know the keyLen and ivLen. You can pass those in explicitly, or pass in the cipher algorithm for the sole
                 *  purpose of capturing those lengths.
                 *
                 *  \note for new code - PKCS5_PBKDF2_HMAC is the preferred DerviveKey subclass to use
                 */
                struct  PKCS5_PBKDF2_HMAC : DerivedKey {
                    PKCS5_PBKDF2_HMAC (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const string& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                    PKCS5_PBKDF2_HMAC (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const String& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                    PKCS5_PBKDF2_HMAC (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const string& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                    PKCS5_PBKDF2_HMAC (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const String& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                };


                /**
                 *  PKCS5_PBKDF2_HMAC_SHA1 be object sliced. Its a simple construction wrapper on a DerivedKey. PKCS5_PBKDF2_HMAC_SHA1
                 *  creates a PKCS5_PBKDF2_HMAC_SHA1 crypto derived key.
                 *
                 *  RFC 2898 suggests an iteration count of at least 1000.
                 *
                 *  \note for new code - PKCS5_PBKDF2_HMAC (or PKCS5_PBKDF2_HMAC_SHA1) is the preferred DerviveKey subclass to use
                 */
                struct  PKCS5_PBKDF2_HMAC_SHA1 : PKCS5_PBKDF2_HMAC {
                    PKCS5_PBKDF2_HMAC_SHA1 (size_t keyLen, size_t ivLen, const string& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                    PKCS5_PBKDF2_HMAC_SHA1 (size_t keyLen, size_t ivLen, const String& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                    PKCS5_PBKDF2_HMAC_SHA1 (CipherAlgorithm cipherAlgorithm, const string& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                    PKCS5_PBKDF2_HMAC_SHA1 (CipherAlgorithm cipherAlgorithm, const String& passwd, unsigned int nRounds = 1000, const Optional<BLOB>& salt = Optional<BLOB> ());
                };
#endif


            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DerivedKey.inl"

#endif  /*_Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_h_*/
