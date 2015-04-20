/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_DerivedKey_h_ 1

#include    "../../StroikaPreComp.h"

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


                using   Memory::BLOB;
                using   Memory::Byte;
                using   Memory::Optional;


#if     qHasFeature_OpenSSL
                /**
                  */
                struct  DerivedKey {
                    BLOB    fKey;
                    BLOB    fIV;

                    /**
                     *  In OpenSSL, the Salt must either by an 8-byte array or omitted.
                     */
                    using   SaltType = std::array<Byte, 8>;

                    /*
                     * Gen key & IV. This requires the cipher algorithm (for the key / iv length) and the hash algorithm.
                     * nrounds is the number of times the we hash the material. More rounds are more secure but
                     * slower.
                     *
                     *  For the string and wstring overloads, we treat the strings as an array of bytes (len*sizeofchar or len*sizeof(wchar_t)) long.
                     */
                    DerivedKey (DigestAlgorithm digestAlgorithm, const EVP_CIPHER* cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, size_t keyLength, size_t ivLength, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, pair<const Byte*, const Byte*> passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, BLOB passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const string& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
                    DerivedKey (DigestAlgorithm digestAlgorithm, CipherAlgorithm cipherAlgorithm, const wstring& passwd, const Optional<SaltType>& salt = Optional<SaltType> (), unsigned int nRounds = 1);
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
