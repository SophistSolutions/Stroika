/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_inl_
#define _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_inl_ 1
/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Cryptography::OpenSSL {

#if qHasFeature_OpenSSL
    /*
     ********************************************************************************
     ********************** Cryptography::OpenSSL::DerivedKey ***********************
     ********************************************************************************
     */
    inline DerivedKey::DerivedKey (const BLOB& key, const BLOB& iv)
        : fKey{key}
        , fIV{iv}
    {
    }
    inline DerivedKey::DerivedKey (const pair<BLOB, BLOB>& keyAndIV)
        : fKey{keyAndIV.first}
        , fIV{keyAndIV.second}
    {
    }
    inline BLOB DerivedKey::NormalizePassword (const BLOB& passwd)
    {
        return passwd;
    }
    inline BLOB DerivedKey::NormalizePassword (const String& passwd)
    {
        string ascii = passwd.AsASCII ();
        return BLOB{reinterpret_cast<const byte*> (ascii.c_str ()), reinterpret_cast<const byte*> (ascii.c_str () + ascii.length ())};
    }

    /*
     ********************************************************************************
     ********************* Cryptography::OpenSSL::EVP_BytesToKey ********************
     ********************************************************************************
     */
    template <typename PASSWORD_TYPE>
    inline EVP_BytesToKey::EVP_BytesToKey (CipherAlgorithm cipherAlgorithm, DigestAlgorithm digestAlgorithm, const PASSWORD_TYPE& passwd,
                                           unsigned int nRounds, const optional<BLOB>& salt)
        : EVP_BytesToKey{cipherAlgorithm, digestAlgorithm, NormalizePassword (passwd), nRounds, salt}
    {
    }

    /*
     ********************************************************************************
     ******************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC ******************
     ********************************************************************************
     */
    template <typename PASSWORD_TYPE>
    inline PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (size_t keyLen, size_t ivLen, DigestAlgorithm digestAlgorithm, const PASSWORD_TYPE& passwd,
                                                 unsigned int nRounds, const optional<BLOB>& salt)
        : PKCS5_PBKDF2_HMAC{keyLen, ivLen, digestAlgorithm, NormalizePassword (passwd), nRounds, salt}
    {
    }
    template <typename PASSWORD_TYPE, typename CIPHER_ALGORITHM_TYPE>
    inline PKCS5_PBKDF2_HMAC::PKCS5_PBKDF2_HMAC (CIPHER_ALGORITHM_TYPE cipherAlgorithm, DigestAlgorithm digestAlgorithm,
                                                 const PASSWORD_TYPE& passwd, unsigned int nRounds, const optional<BLOB>& salt)
        : PKCS5_PBKDF2_HMAC{KeyLength (cipherAlgorithm), IVLength (cipherAlgorithm), digestAlgorithm, passwd, nRounds, salt}
    {
    }

    /*
     ********************************************************************************
     *************** Cryptography::OpenSSL::PKCS5_PBKDF2_HMAC_SHA1 ******************
     ********************************************************************************
     */
    template <typename PASSWORD_TYPE>
    inline PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (size_t keyLen, size_t ivLen, const PASSWORD_TYPE& passwd, unsigned int nRounds,
                                                           const optional<BLOB>& salt)
        : PKCS5_PBKDF2_HMAC{keyLen, ivLen, DigestAlgorithms::kSHA1, passwd, nRounds, salt}
    {
    }
    template <typename PASSWORD_TYPE, typename CIPHER_ALGORITHM_TYPE>
    inline PKCS5_PBKDF2_HMAC_SHA1::PKCS5_PBKDF2_HMAC_SHA1 (CIPHER_ALGORITHM_TYPE cipherAlgorithm, const PASSWORD_TYPE& passwd,
                                                           unsigned int nRounds, const optional<BLOB>& salt)
        : PKCS5_PBKDF2_HMAC{cipherAlgorithm, DigestAlgorithms::kSHA1, passwd, nRounds, salt}
    {
    }
#endif

}

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_inl_*/
