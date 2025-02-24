/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_HasComponent_OpenSSL
#include <openssl/evp.h>
#endif

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Memory/Optional.h"

#include "DigestAlgorithm.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Cryptography;
using namespace Stroika::Foundation::Cryptography::Providers::OpenSSL;
using namespace Stroika::Foundation::Memory;

#if qStroika_HasComponent_OpenSSL && defined(_MSC_VER)
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

#if qStroika_HasComponent_OpenSSL

/*
 ********************************************************************************
 ************************** OpenSSL::DigestAlgorithm ****************************
 ********************************************************************************
 */
DigestAlgorithm::DigestAlgorithm (const ::EVP_MD* digester)
    : name{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> String {
        const DigestAlgorithm* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &DigestAlgorithm::name);
        AssertNotNull (EVP_MD_name (thisObj->fDigester_));
        return String{EVP_MD_name (thisObj->fDigester_)};
    }}
    , fDigester_{digester}
{
    RequireNotNull (digester);
}

DigestAlgorithm DigestAlgorithm::GetByName (const String& digestName)
{
    static const Execution::RuntimeErrorException kErr_{"No such digest"sv};
    auto                                          p = ::EVP_get_digestbyname (digestName.AsNarrowSDKString ().c_str ());
    Execution::ThrowIfNull (p, kErr_);
    return p;
}

optional<DigestAlgorithm> DigestAlgorithm::GetByNameQuietly (const String& digestName)
{
    auto tmp = ::EVP_get_digestbyname (digestName.AsNarrowSDKString ().c_str ());
    return tmp == nullptr ? optional<DigestAlgorithm>{} : tmp;
}

/*
 ********************************************************************************
 *********** Cryptography::Providers::OpenSSL::DigestAlgorithms *****************
 ********************************************************************************
 */
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kMD5{[] () { return ::EVP_md5 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA1{[] () { return ::EVP_sha1 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA1_224{[] () { return ::EVP_sha224 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA1_256{[] () { return ::EVP_sha256 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA1_384{[] () { return ::EVP_sha384 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA1_512{[] () { return ::EVP_sha512 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA3_224{[] () { return ::EVP_sha3_224 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA3_256{[] () { return ::EVP_sha3_256 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA3_384{[] () { return ::EVP_sha3_384 (); }};
const Execution::LazyInitialized<DigestAlgorithm> DigestAlgorithms::kSHA3_512{[] () { return ::EVP_sha3_512 (); }};
#endif
