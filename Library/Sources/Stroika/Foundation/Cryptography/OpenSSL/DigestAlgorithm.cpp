/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Containers/Common.h"
#include "../../Debug/Assertions.h"
#include "../../Execution/Common.h"
#include "../../Execution/Synchronized.h"
#include "../../Memory/SmallStackBuffer.h"

#include "DigestAlgorithm.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::OpenSSL;
using namespace Stroika::Foundation::Memory;

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

#if qCompiler_cpp17InlineStaticMemberOfTemplateLinkerUndefined_Buggy
/*
 ********************************************************************************
 **************************** Configuration::DefaultNames ***********************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {
    template<>
    constexpr EnumNames<Cryptography::OpenSSL::DigestAlgorithm> DefaultNames<Cryptography::OpenSSL::DigestAlgorithm>::k;
}
#endif

/*
 ********************************************************************************
 ************** Cryptography::OpenSSL::Convert2OpenSSL **************************
 ********************************************************************************
 */
const EVP_MD* OpenSSL::Convert2OpenSSL (DigestAlgorithm digestAlgorithm)
{
    switch (digestAlgorithm) {
#if 0
        case DigestAlgorithm::eDSS:
            return ::EVP_dss ();
#endif
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
