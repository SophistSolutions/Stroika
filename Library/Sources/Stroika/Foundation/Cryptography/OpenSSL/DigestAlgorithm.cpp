/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Common/Property.h"
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

/*
 ********************************************************************************
 ***************** Cryptography::OpenSSL::DigestAlgorithm ***********************
 ********************************************************************************
 */
DigestAlgorithm::DigestAlgorithm (const ::EVP_MD* digester)
    : pName{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> String {
              const DigestAlgorithm* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &DigestAlgorithm::pName);
              AssertNotNull (EVP_MD_name (thisObj->fDigester_));
              return String::FromASCII (EVP_MD_name (thisObj->fDigester_));
          }}
    , fDigester_{digester}
{
    RequireNotNull (digester);
}

/*
 ********************************************************************************
 ***************** Cryptography::OpenSSL::DigestAlgorithms **********************
 ********************************************************************************
 */
#if qCompilerAndStdLib_const_extern_declare_then_const_define_namespace_Buggy
namespace Stroika::Foundation::Cryptography::OpenSSL::DigestAlgorithms {
    const Execution::VirtualConstant<DigestAlgorithm> kMD5{[] () { return ::EVP_md5 (); }};
    const Execution::VirtualConstant<DigestAlgorithm> kSHA1{[] () { return ::EVP_sha1 (); }};
    const Execution::VirtualConstant<DigestAlgorithm> kSHA224{[] () { return ::EVP_sha224 (); }};
    const Execution::VirtualConstant<DigestAlgorithm> kSHA256{[] () { return ::EVP_sha256 (); }};
}
#else
const Execution::VirtualConstant<DigestAlgorithm> DigestAlgorithms::kMD5{[] () { return ::EVP_md5 (); }};
const Execution::VirtualConstant<DigestAlgorithm> DigestAlgorithms::kSHA1{[] () { return ::EVP_sha1 (); }};
const Execution::VirtualConstant<DigestAlgorithm> DigestAlgorithms::kSHA224{[] () { return ::EVP_sha224 (); }};
const Execution::VirtualConstant<DigestAlgorithm> DigestAlgorithms::kSHA256{[] () { return ::EVP_sha256 (); }};
#endif

#endif
