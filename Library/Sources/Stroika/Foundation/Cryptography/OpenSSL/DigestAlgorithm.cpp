/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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

DigestAlgorithm DigestAlgorithm::GetByName (const String& digestName)
{
    static const Execution::RuntimeErrorException kErr_{L"No such digest"sv};
    auto                                          p = ::EVP_get_digestbyname (digestName.AsNarrowSDKString ().c_str ());
    Execution::ThrowIfNull (p, kErr_);
    return p;
}

optional<DigestAlgorithm> DigestAlgorithm::GetByNameQuietly (const String& digestName)
{
    //return Memory::OptionalFromNullable (::EVP_get_digestbyname (digestName.AsNarrowSDKString ().c_str ()));
    auto tmp = ::EVP_get_digestbyname (digestName.AsNarrowSDKString ().c_str ());
    return tmp == nullptr ? optional<DigestAlgorithm>{} : tmp;
}

/*
 ********************************************************************************
 ***************** Cryptography::OpenSSL::DigestAlgorithms **********************
 ********************************************************************************
 */
#if qCompilerAndStdLib_const_extern_declare_then_const_define_namespace_Buggy
namespace Stroika::Foundation::Cryptography::OpenSSL::DigestAlgorithms {
    const Common::ConstantProperty<DigestAlgorithm> kMD5{[] () { return ::EVP_md5 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA1{[] () { return ::EVP_sha1 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA1_224{[] () { return ::EVP_sha224 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA1_256{[] () { return ::EVP_sha256 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA1_384{[] () { return ::EVP_sha384 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA1_512{[] () { return ::EVP_sha512 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA3_224{[] () { return ::EVP_sha3_224 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA3_256{[] () { return ::EVP_sha3_256 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA3_384{[] () { return ::EVP_sha3_384 (); }};
    const Common::ConstantProperty<DigestAlgorithm> kSHA3_512{[] () { return ::EVP_sha3_512 (); }};
}
#else
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kMD5{[] () { return ::EVP_md5 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA1{[] () { return ::EVP_sha1 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA1_224{[] () { return ::EVP_sha224 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA1_256{[] () { return ::EVP_sha256 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA1_384{[] () { return ::EVP_sha384 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA1_512{[] () { return ::EVP_sha512 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA3_224{[] () { return ::EVP_sha3_224 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA3_256{[] () { return ::EVP_sha3_256 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA3_384{[] () { return ::EVP_sha3_384 (); }};
const Common::ConstantProperty<DigestAlgorithm> DigestAlgorithms::kSHA3_512{[] () { return ::EVP_sha3_512 (); }};

#endif

#endif
