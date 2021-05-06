/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_ 1

#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Characters/ToString.h"
#include "../../Configuration/Common.h"
#include "../../Containers/Set.h"
#include "../../Execution/Exceptions.h"
#include "../../Execution/VirtualConstant.h"
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

#if qHasFeature_OpenSSL
namespace Stroika::Foundation::Cryptography::OpenSSL {

    using Containers::Set;

    using CipherAlgorithm = const EVP_CIPHER*;

    /**
     *      @see http://linux.die.net/man/3/evp_cipher_ctx_init
     */
    namespace CipherAlgorithms {
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CBC;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_128_ECB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_128_OFB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CFB1;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CFB8;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_128_CFB128;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CBC;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_192_ECB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_192_OFB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CFB1;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CFB8;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_192_CFB128;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CBC;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_256_ECB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_256_OFB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CFB1;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CFB8;
        extern const Execution::VirtualConstant<CipherAlgorithm> kAES_256_CFB128;

        /*
         * NB: In OpenSSL v3, the algorithms:
         *  o   eBlowfish
         *  o   eRC2,
         *  o   eRC4,
         *  o   (and others listed in https://wiki.openssl.org/index.php/OpenSSL_3.0 but we never supported them before)
         * are only available in the LEGACY provider, and it is not recommended to use, so dont make that easy
         * from Stroika. By default just assume default provider is in use.
         *  @see https://stroika.atlassian.net/browse/STK-735
         * 
         * @todo mark these below as deprecated...
         */
        extern const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_CBC;
        extern const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_ECB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_CFB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kBlowfish_OFB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kBlowfish;
        extern const Execution::VirtualConstant<CipherAlgorithm> kRC2_CBC;
        extern const Execution::VirtualConstant<CipherAlgorithm> kRC2_ECB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kRC2_CFB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kRC2_OFB;
        extern const Execution::VirtualConstant<CipherAlgorithm> kRC4;
    }

    extern const Execution::VirtualConstant<Set<CipherAlgorithm>> kAllCiphers;
    extern const Execution::VirtualConstant<Set<CipherAlgorithm>> kAllLoadedCiphers;
    extern const Execution::VirtualConstant<Set<CipherAlgorithm>> kDefaultProviderCiphers;
    extern const Execution::VirtualConstant<Set<CipherAlgorithm>> kLegacyProviderCiphers;

}

namespace Stroika::Foundation::Characters {
    template <>
    String ToString (const Cryptography::OpenSSL::CipherAlgorithm& v);
}

#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CipherAlgorithm.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_*/
