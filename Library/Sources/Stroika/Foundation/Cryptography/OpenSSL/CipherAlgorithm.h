/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_ 1

#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Characters/ToString.h"
#include "../../Common/Property.h"
#include "../../Configuration/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Memory/Common.h"

/**
 *  \file
 *
 */

#if qHasFeature_OpenSSL
namespace Stroika::Foundation::Cryptography::OpenSSL {

    using Characters::String;

    /**
     *  \brief object oriented wrapper on OpenSSL cipher algoritms (const EVP_CIPHER*)
     */
    class CipherAlgorithm {
    public:
        /**
         */
        CipherAlgorithm (const ::EVP_CIPHER* cipher);
        CipherAlgorithm (const CipherAlgorithm& src);

    public:
        nonvirtual CipherAlgorithm& operator= (const CipherAlgorithm& src);

    public:
        /**
         *  Wrapper on CipherAlgorithm
         * 
         *  @see https://linux.die.net/man/3/evp_get_cipherbyname
         */
        static CipherAlgorithm           GetByName (const String& cipherName);
        static optional<CipherAlgorithm> GetByNameQuietly (const String& cipherName);

    public:
        nonvirtual operator const ::EVP_CIPHER* () const;

    public:
        /**
         */
        Common::ReadOnlyProperty<String> pName;

    public:
#if __cpp_impl_three_way_comparison >= 201907
        nonvirtual auto operator<=> (const CipherAlgorithm& rhs) const;
#else
        nonvirtual auto operator< (const CipherAlgorithm& rhs) const;
#endif
        nonvirtual bool operator== (const CipherAlgorithm& rhs) const;

    public:
        /**
         */
        nonvirtual size_t KeyLength () const;

    public:
        /**
         */
        nonvirtual size_t IVLength () const;

    public:
        /**
         */
        nonvirtual String ToString () const;

    private:
        const ::EVP_CIPHER* fCipher_;
    };

    /**
     *      @see https://wiki.openssl.org/index.php/OpenSSL_3.0 section 9.3.2
     */
    namespace CipherAlgorithms {
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_128_CBC;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_128_ECB;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_128_OFB;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_128_CFB1;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_128_CFB8;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_128_CFB128;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_192_CBC;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_192_ECB;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_192_OFB;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_192_CFB1;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_192_CFB8;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_192_CFB128;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_256_CBC;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_256_ECB;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_256_OFB;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_256_CFB1;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_256_CFB8;
        extern const Common::ConstantProperty<CipherAlgorithm> kAES_256_CFB128;

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
        extern const Common::ConstantProperty<CipherAlgorithm> kBlowfish_CBC;
        extern const Common::ConstantProperty<CipherAlgorithm> kBlowfish_ECB;
        extern const Common::ConstantProperty<CipherAlgorithm> kBlowfish_CFB;
        extern const Common::ConstantProperty<CipherAlgorithm> kBlowfish_OFB;
        extern const Common::ConstantProperty<CipherAlgorithm> kBlowfish;
        extern const Common::ConstantProperty<CipherAlgorithm> kRC2_CBC;
        extern const Common::ConstantProperty<CipherAlgorithm> kRC2_ECB;
        extern const Common::ConstantProperty<CipherAlgorithm> kRC2_CFB;
        extern const Common::ConstantProperty<CipherAlgorithm> kRC2_OFB;
        extern const Common::ConstantProperty<CipherAlgorithm> kRC4;
    }

}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CipherAlgorithm.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_h_*/
