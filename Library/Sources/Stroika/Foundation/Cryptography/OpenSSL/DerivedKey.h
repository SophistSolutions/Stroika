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
                    enum    class   KeyDerivationStrategy {
                        //PKCS5_PBKDF1,
                        PKCS5_PBKDF2_HMAC,
                        EVP_BytesToKey,
                        CryptDeriveKey,
                    };

                    BLOB    fKey;
                    BLOB    fIV;

                    /**
                     *  In OpenSSL, the Salt must either by an 8-byte array or omitted.
                     */
                    using   SaltType = Memory::BLOB;

                    /*
                     * Gen key & IV. This requires the cipher algorithm (for the key / iv length) and the hash algorithm.
                     * nrounds is the number of times the we hash the material. More rounds are more secure but
                     * slower.
                     *
                     *  For the string overload, we treat the strings as an array of bytes (len bytes) long.
                     *  For the String overload, we convert to UTF8 and treat as string (so L"fred" and "fred" produce the same thing).
                     */
                    DerivedKey (KeyDerivationStrategy keyDerivationStrategy, DigestAlgorithm digestAlgorithm, const string& passwd, const Optional<SaltType>& salt, unsigned int nRounds);
                    DerivedKey (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, size_t keyLength, size_t ivLength, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, BLOB passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const string& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const String& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                };


                /**
                 *  CryptDeriveKey CAN be object sliced. Its a simple construction wrapper on a DerivedKey. CryptDeriveKey
                 *  creates a Microsoft-Windows format derived key, compatible with the Windows CryptDeriveKey API.
                 */
                struct  CryptDeriveKey : DerivedKey {
                };


                /**
                 *  EVP_BytesToKey CAN be object sliced. Its a simple construction wrapper on a DerivedKey. EVP_BytesToKey
                 *  creates an OpenSSL-default-format, old-style crypto derived key.
                 */
                struct  EVP_BytesToKey : DerivedKey {
                    /**
                     *  In OpenSSL, the Salt must either by an 8-byte array or omitted.
                     */
                    using   SaltType = std::array<Byte, 8>;

                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, size_t keyLength, size_t ivLength, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, BLOB passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const string& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    EVP_BytesToKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const String& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                };


                /**
                 *  PKCS5_PBKDF2_HMAC CAN be object sliced. Its a simple construction wrapper on a DerivedKey. PKCS5_PBKDF1
                 *  creates a PKCS5 PBKDF2 HMAC crypto derived key.
                 *
                 *  RFC 2898 suggests an iteration count of at least 1000
                 */
                struct  PKCS5_PBKDF2_HMAC : DerivedKey {
                    PKCS5_PBKDF2_HMAC (DigestAlgorithm digestAlgorithm, const string& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1000);
                    PKCS5_PBKDF2_HMAC (DigestAlgorithm digestAlgorithm, const String& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1000);
                };


                /**
                 *  PKCS5_PBKDF2_HMAC_SHA1 be object sliced. Its a simple construction wrapper on a DerivedKey. PKCS5_PBKDF2_HMAC_SHA1
                 *  creates a PKCS5_PBKDF2_HMAC_SHA1 crypto derived key.
                 *
                 *  RFC 2898 suggests an iteration count of at least 1000.
                 */
                struct  PKCS5_PBKDF2_HMAC_SHA1 : PKCS5_PBKDF2_HMAC {
                    PKCS5_PBKDF2_HMAC_SHA1 (const string& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1000);
                    PKCS5_PBKDF2_HMAC_SHA1 (const String& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1000);
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
