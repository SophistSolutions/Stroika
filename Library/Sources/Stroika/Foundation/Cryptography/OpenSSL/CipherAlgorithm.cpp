/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Containers/Common.h"
#include "../../Debug/Assertions.h"
#include "../../Debug/Trace.h"
#include "../../Execution/Common.h"
#include "../../Memory/Optional.h"

#include "CipherAlgorithm.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
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
 ***************** Cryptography::OpenSSL::CipherAlgorithms **********************
 ********************************************************************************
 */
CipherAlgorithm::CipherAlgorithm (const ::EVP_CIPHER* cipher)
    : pName{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> String {
              const CipherAlgorithm* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &CipherAlgorithm::pName);
              AssertNotNull (::EVP_CIPHER_name (thisObj->fCipher_));
              return String::FromASCII (::EVP_CIPHER_name (thisObj->fCipher_));
          }}
    , fCipher_{cipher}
{
    RequireNotNull (cipher);
}

CipherAlgorithm CipherAlgorithm::GetByName (const String& cipherName)
{
    static const Execution::RuntimeErrorException kErr_{L"No such cipher"sv};
    auto                                          p = ::EVP_get_cipherbyname (cipherName.AsNarrowSDKString ().c_str ());
    Execution::ThrowIfNull (p, kErr_);
    return p;
}

optional<CipherAlgorithm> CipherAlgorithm::GetByNameQuietly (const String& cipherName)
{
    //return Memory::OptionalFromNullable (EVP_get_cipherbyname (cipherName.AsNarrowSDKString ().c_str ()));
    auto tmp = EVP_get_cipherbyname (cipherName.AsNarrowSDKString ().c_str ());
    return tmp == nullptr ? optional<CipherAlgorithm>{} : tmp;
}

/*
 ********************************************************************************
 ***************** Cryptography::OpenSSL::CipherAlgorithms **********************
 ********************************************************************************
 */
#if qCompilerAndStdLib_const_extern_declare_then_const_define_namespace_Buggy
namespace Stroika::Foundation::Cryptography::OpenSSL::CipherAlgorithms {
    const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CBC{[] () { return ::EVP_aes_128_cbc (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_128_ECB{[] () { return ::EVP_aes_128_ecb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_128_OFB{[] () { return ::EVP_aes_128_ofb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CFB1{[] () { return ::EVP_aes_128_cfb1 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CFB8{[] () { return ::EVP_aes_128_cfb8 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CFB128{[] () { return ::EVP_aes_128_cfb128 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CBC{[] () { return ::EVP_aes_192_cbc (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_192_ECB{[] () { return ::EVP_aes_192_ecb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_192_OFB{[] () { return ::EVP_aes_192_ofb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CFB1{[] () { return ::EVP_aes_192_cfb1 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CFB8{[] () { return ::EVP_aes_192_cfb8 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CFB128{[] () { return ::EVP_aes_192_cfb128 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CBC{[] () { return ::EVP_aes_256_cbc (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_256_ECB{[] () { return ::EVP_aes_256_ecb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_256_OFB{[] () { return ::EVP_aes_256_ofb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CFB1{[] () { return ::EVP_aes_256_cfb1 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CFB8{[] () { return ::EVP_aes_256_cfb8 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CFB128{[] () { return ::EVP_aes_256_cfb128 (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_CBC{[] () { return ::EVP_bf_cbc (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kBlowfish{[] () { return kBlowfish_CBC (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_ECB{[] () { return ::EVP_bf_ecb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_CFB{[] () { return ::EVP_bf_cfb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_OFB{[] () { return ::EVP_bf_ofb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kRC2_CBC{[] () { return ::EVP_rc2_cbc (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kRC2_ECB{[] () { return ::EVP_rc2_ecb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kRC2_CFB{[] () { return ::EVP_rc2_cfb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kRC2_OFB{[] () { return ::EVP_rc2_ofb (); }};
    const Execution::VirtualConstant<CipherAlgorithm> kRC4{[] () { return ::EVP_rc4 (); }};
}
#else
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_128_CBC{[] () { return ::EVP_aes_128_cbc (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_128_ECB{[] () { return ::EVP_aes_128_ecb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_128_OFB{[] () { return ::EVP_aes_128_ofb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_128_CFB1{[] () { return ::EVP_aes_128_cfb1 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_128_CFB8{[] () { return ::EVP_aes_128_cfb8 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_128_CFB128{[] () { return ::EVP_aes_128_cfb128 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_192_CBC{[] () { return ::EVP_aes_192_cbc (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_192_ECB{[] () { return ::EVP_aes_192_ecb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_192_OFB{[] () { return ::EVP_aes_192_ofb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_192_CFB1{[] () { return ::EVP_aes_192_cfb1 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_192_CFB8{[] () { return ::EVP_aes_192_cfb8 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_192_CFB128{[] () { return ::EVP_aes_192_cfb128 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_256_CBC{[] () { return ::EVP_aes_256_cbc (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_256_ECB{[] () { return ::EVP_aes_256_ecb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_256_OFB{[] () { return ::EVP_aes_256_ofb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_256_CFB1{[] () { return ::EVP_aes_256_cfb1 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_256_CFB8{[] () { return ::EVP_aes_256_cfb8 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kAES_256_CFB128{[] () { return ::EVP_aes_256_cfb128 (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kBlowfish_CBC{[] () { return ::EVP_bf_cbc (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kBlowfish{[] () { return kBlowfish_CBC (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kBlowfish_ECB{[] () { return ::EVP_bf_ecb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kBlowfish_CFB{[] () { return ::EVP_bf_cfb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kBlowfish_OFB{[] () { return ::EVP_bf_ofb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kRC2_CBC{[] () { return ::EVP_rc2_cbc (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kRC2_ECB{[] () { return ::EVP_rc2_ecb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kRC2_CFB{[] () { return ::EVP_rc2_cfb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kRC2_OFB{[] () { return ::EVP_rc2_ofb (); }};
const Execution::VirtualConstant<CipherAlgorithm> CipherAlgorithms::kRC4{[] () { return ::EVP_rc4 (); }};
#endif

#endif
