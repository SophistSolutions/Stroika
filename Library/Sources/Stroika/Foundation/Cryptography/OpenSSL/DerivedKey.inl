/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_inl_
#define _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Cryptography {
            namespace   OpenSSL {


#if     qHasFeature_OpenSSL
                /*
                 ********************************************************************************
                 ********************** Cryptography::OpenSSL::DerivedKey ***********************
                 ********************************************************************************
                 */
                inline  DerivedKey::DerivedKey (const BLOB& key, const BLOB& iv)
                    : fKey (key)
                    , fIV (iv)
                {
                }
                inline  DerivedKey::DerivedKey (const pair<BLOB, BLOB>& keyAndIV)
                    : fKey (keyAndIV.first)
                    , fIV (keyAndIV.second)
                {
                }




                /*
                 ********************************************************************************
                 *************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC_SHA1 ******************
                 ********************************************************************************
                 */
                inline  PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (size_t keyLen, size_t ivLen, const string& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
                    : PKCS5_PBKDF2_HMAC (keyLen, ivLen, DigestAlgorithm::eSHA1, passwd, nRounds, salt)
                {
                }
                inline  PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (size_t keyLen, size_t ivLen, const String& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
                    : PKCS5_PBKDF2_HMAC (keyLen, ivLen, DigestAlgorithm::eSHA1, passwd, nRounds, salt)
                {
                }
                inline  PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (CipherAlgorithm cipherAlgorithm, const string& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
                    : PKCS5_PBKDF2_HMAC (cipherAlgorithm, DigestAlgorithm::eSHA1, passwd, nRounds, salt)
                {
                }
                inline  PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (CipherAlgorithm cipherAlgorithm, const String& passwd, unsigned int nRounds, const Optional<BLOB>& salt)
                    : PKCS5_PBKDF2_HMAC (cipherAlgorithm, DigestAlgorithm::eSHA1, passwd, nRounds, salt)
                {
                }
#endif


            }
        }
    }
}
#endif  /*_Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_inl_*/
