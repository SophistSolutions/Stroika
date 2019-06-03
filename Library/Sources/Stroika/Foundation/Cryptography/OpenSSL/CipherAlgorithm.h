/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_ 1

#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Configuration/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Memory/Common.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   AddLookupByName functions.
 *
 *      @todo   document/enforce thread safety checks
 *
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

#if qHasFeature_OpenSSL
    /**
     *      @see http://linux.die.net/man/3/evp_cipher_ctx_init
     */
    enum class CipherAlgorithm {
        eAES_128_CBC,
        eAES_128_ECB,
        eAES_128_OFB,
        eAES_128_CFB1,
        eAES_128_CFB8,
        eAES_128_CFB128,
        eAES_192_CBC,
        eAES_192_ECB,
        eAES_192_OFB,
        eAES_192_CFB1,
        eAES_192_CFB8,
        eAES_192_CFB128,
        eAES_256_CBC,
        eAES_256_ECB,
        eAES_256_OFB,
        eAES_256_CFB1,
        eAES_256_CFB8,
        eAES_256_CFB128,

        eBlowfish_CBC,
        eBlowfish = eBlowfish_CBC,
        eBlowfish_ECB,
        eBlowfish_CFB,
        eBlowfish_OFB,

        eRC2_CBC,
        eRC2_ECB,
        eRC2_CFB,
        eRC2_OFB,

        eRC4,

        Stroika_Define_Enum_Bounds (eAES_128_CBC, eRC4)
    };
#endif

#if qHasFeature_OpenSSL
    /**
     *  \req valid algorithm from above enum, and \ens not nullptr.
     *
     *  \note - the returned pointer is immutable, and the data remains valid until the end of the program.
     */
    const EVP_CIPHER* Convert2OpenSSL (CipherAlgorithm hashAlg);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
//#include    "CipherAlgorithm.inl"
#if qHasFeature_OpenSSL
namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<Cryptography::OpenSSL::CipherAlgorithm> : EnumNames<Cryptography::OpenSSL::CipherAlgorithm> {
        static constexpr EnumNames<Cryptography::OpenSSL::CipherAlgorithm> k{
            EnumNames<Cryptography::OpenSSL::CipherAlgorithm>::BasicArrayInitializer{
                {
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CBC, L"AES_128_CBC"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_128_ECB, L"AES_128_ECB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_128_OFB, L"AES_128_OFB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CFB1, L"AES_128_CFB1"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CFB8, L"AES_128_CFB8"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CFB128, L"AES_128_CFB128"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CBC, L"AES_192_CBC"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_192_ECB, L"AES_192_ECB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_192_OFB, L"AES_192_OFB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CFB1, L"AES_192_CFB1"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CFB8, L"AES_192_CFB8"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CFB128, L"AES_192_CFB128"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CBC, L"AES_256_CBC"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_256_ECB, L"AES_256_ECB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_256_OFB, L"AES_256_OFB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CFB1, L"AES_256_CFB1"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CFB8, L"AES_256_CFB8"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CFB128, L"AES_256_CFB128"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_CBC, L"Blowfish_CBC"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_ECB, L"Blowfish_ECB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_CFB, L"Blowfish_CFB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_OFB, L"Blowfish_OFB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eRC2_CBC, L"RC2_CBC"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eRC2_ECB, L"RC2_ECB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eRC2_CFB, L"RC2_CFB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eRC2_OFB, L"RC2_OFB"},
                    {Cryptography::OpenSSL::CipherAlgorithm::eRC4, L"RC4"},
                }}};
        constexpr DefaultNames ()
            : EnumNames<Cryptography::OpenSSL::CipherAlgorithm> (k)
        {
        }
    };
}
#endif
#endif /*_Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_*/
