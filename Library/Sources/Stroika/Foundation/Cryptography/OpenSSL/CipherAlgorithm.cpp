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

#include    "CipherAlgorithm.h"


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
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {
#if     qCompilerAndStdLib_constexpr_Buggy
            template    <>
            const EnumNames<Cryptography::OpenSSL::CipherAlgorithm> DefaultNames<Cryptography::OpenSSL::CipherAlgorithm>::k
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
                =
#endif
            {
                EnumNames<Cryptography::OpenSSL::CipherAlgorithm>::BasicArrayInitializer  {
                    {
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CBC, L"AES_128_CBC" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_128_ECB, L"AES_128_ECB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_128_OFB, L"AES_128_OFB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CFB1, L"AES_128_CFB1" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CFB8, L"AES_128_CFB8" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_128_CFB128, L"AES_128_CFB128" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CBC, L"AES_192_CBC" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_192_ECB, L"AES_192_ECB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_192_OFB, L"AES_192_OFB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CFB1, L"AES_192_CFB1" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CFB8, L"AES_192_CFB8" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_192_CFB128, L"AES_192_CFB128" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CBC, L"AES_256_CBC" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_256_ECB, L"AES_256_ECB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_256_OFB, L"AES_256_OFB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CFB1, L"AES_256_CFB1" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CFB8, L"AES_256_CFB8" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eAES_256_CFB128, L"AES_256_CFB128" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_CBC, L"Blowfish_CBC" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_ECB, L"Blowfish_ECB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_CFB, L"Blowfish_CFB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eBlowfish_OFB, L"Blowfish_OFB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eRC2_CBC, L"RC2_CBC" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eRC2_ECB, L"RC2_ECB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eRC2_CFB, L"RC2_CFB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eRC2_OFB, L"RC2_OFB" },
                        { Cryptography::OpenSSL::CipherAlgorithm::eRC4, L"RC4" },
                    }
                }
            };
#else
            constexpr   EnumNames<Cryptography::OpenSSL::CipherAlgorithm>   DefaultNames<Cryptography::OpenSSL::CipherAlgorithm>::k;
#endif
        }
    }
}
#endif





#if     qHasFeature_OpenSSL
/*
 ********************************************************************************
 ************** Cryptography::OpenSSL::Convert2OpenSSL **************************
 ********************************************************************************
 */
const EVP_CIPHER* OpenSSL::Convert2OpenSSL (CipherAlgorithm alg)
{
    switch (alg) {
        case CipherAlgorithm::eAES_128_CBC:
            return ::EVP_aes_128_cbc ();
        case CipherAlgorithm::eAES_128_ECB:
            return ::EVP_aes_128_ecb ();
        case CipherAlgorithm::eAES_128_OFB:
            return ::EVP_aes_128_ofb ();
        case CipherAlgorithm::eAES_128_CFB1:
            return ::EVP_aes_128_cfb1 ();
        case CipherAlgorithm::eAES_128_CFB8:
            return ::EVP_aes_128_cfb8 ();
        case CipherAlgorithm::eAES_128_CFB128:
            return ::EVP_aes_128_cfb128 ();
        case CipherAlgorithm::eAES_192_CBC:
            return ::EVP_aes_192_cbc ();
        case CipherAlgorithm::eAES_192_ECB:
            return ::EVP_aes_192_ecb ();
        case CipherAlgorithm::eAES_192_OFB:
            return ::EVP_aes_192_ofb ();
        case CipherAlgorithm::eAES_192_CFB1:
            return ::EVP_aes_192_cfb1 ();
        case CipherAlgorithm::eAES_192_CFB8:
            return ::EVP_aes_192_cfb8 ();
        case CipherAlgorithm::eAES_192_CFB128:
            return ::EVP_aes_192_cfb128 ();
        case CipherAlgorithm::eAES_256_CBC:
            return ::EVP_aes_256_cbc ();
        case CipherAlgorithm::eAES_256_ECB:
            return ::EVP_aes_256_ecb ();
        case CipherAlgorithm::eAES_256_OFB:
            return ::EVP_aes_256_ofb ();
        case CipherAlgorithm::eAES_256_CFB1:
            return ::EVP_aes_256_cfb1 ();
        case CipherAlgorithm::eAES_256_CFB8:
            return ::EVP_aes_256_cfb8 ();
        case CipherAlgorithm::eAES_256_CFB128:
            return ::EVP_aes_256_cfb128 ();
        case CipherAlgorithm::eBlowfish_CBC:
            return ::EVP_bf_cbc ();
        case CipherAlgorithm::eBlowfish_ECB:
            return ::EVP_bf_ecb ();
        case CipherAlgorithm::eBlowfish_CFB:
            return ::EVP_bf_cfb ();
        case CipherAlgorithm::eBlowfish_OFB:
            return ::EVP_bf_ofb ();
        case CipherAlgorithm::eRC2_CBC:
            return ::EVP_rc2_cbc ();
        case CipherAlgorithm::eRC2_ECB:
            return ::EVP_rc2_ecb ();
        case CipherAlgorithm::eRC2_CFB:
            return ::EVP_rc2_cfb ();
        case CipherAlgorithm::eRC2_OFB:
            return ::EVP_rc2_ofb ();
        case CipherAlgorithm::eRC4:
            return ::EVP_rc4 ();
        default:
            RequireNotReached();
            return nullptr;
    }
}
#endif
