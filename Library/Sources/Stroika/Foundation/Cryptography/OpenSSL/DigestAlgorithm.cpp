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

#include    "DigestAlgorithm.h"


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
            const EnumNames<Cryptography::OpenSSL::DigestAlgorithm> DefaultNames<Cryptography::OpenSSL::DigestAlgorithm>::k
#if     qCompilerAndStdLib_const_Array_Init_wo_UserDefined_Buggy
                =
#endif
            {
                EnumNames<Cryptography::OpenSSL::DigestAlgorithm>::BasicArrayInitializer  {
                    {
                        { Cryptography::OpenSSL::DigestAlgorithm::eDSS, L"eDSS" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eMD5, L"eMD5" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eSHA1, L"eSHA1" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eSHA224, L"eSHA224" },
                        { Cryptography::OpenSSL::DigestAlgorithm::eSHA256, L"eSHA256" },
                    }
                }
            };
#else
            constexpr   EnumNames<Cryptography::OpenSSL::DigestAlgorithm>   DefaultNames<Cryptography::OpenSSL::DigestAlgorithm>::k;
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
const EVP_MD* OpenSSL::Convert2OpenSSL (DigestAlgorithm digestAlgorithm)
{
    switch (digestAlgorithm) {
        case DigestAlgorithm::eDSS:
            return ::EVP_dss ();
        case DigestAlgorithm::eMD5:
            return ::EVP_md5 ();
        case DigestAlgorithm::eSHA1:
            return ::EVP_sha1 ();
        case DigestAlgorithm::eSHA224:
            return ::EVP_sha224 ();
        case DigestAlgorithm::eSHA256:
            return ::EVP_sha256 ();
        default:
            RequireNotReached ();
            return nullptr;
    }
}
#endif
