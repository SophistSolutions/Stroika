/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_ 1

#include "../../StroikaPreComp.h"

#if qHasFeature_OpenSSL
#include <openssl/evp.h>
#endif

#include "../../Common/Property.h"
#include "../../Configuration/Common.h"
#include "../../Execution/Exceptions.h"
#include "../../Memory/Common.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   document/enforce thread safety checks
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

    using Characters::String;

#if qHasFeature_OpenSSL

    /**
     *  \brief object oriented wrapper on OpenSSL digest algoritms (const EVP_MD*)
     */
    class DigestAlgorithm {
    public:
        /**
         */
        DigestAlgorithm (const ::EVP_MD* digester);
        DigestAlgorithm (const DigestAlgorithm& src);

    public:
        nonvirtual DigestAlgorithm& operator= (const DigestAlgorithm& src);

    public:
        /**
         *  Wrapper on DigestAlgorithm
         * 
         *  @see https://linux.die.net/man/3/evp_get_digestbyname
         */
        static DigestAlgorithm           GetByName (const String& digestName);
        static optional<DigestAlgorithm> GetByNameQuietly (const String& digestName);

    public:
        nonvirtual operator const ::EVP_MD* () const;

    public:
        /**
         */
        Common::ReadOnlyProperty<String> pName;

    public:
#if __cpp_impl_three_way_comparison >= 201907
        nonvirtual auto operator<=> (const DigestAlgorithm& rhs) const;
#else
        nonvirtual auto operator< (const DigestAlgorithm& rhs) const;
#endif
        nonvirtual bool operator== (const DigestAlgorithm& rhs) const;

    public:
        /**
         */
        nonvirtual String ToString () const;

    private:
        const ::EVP_MD* fDigester_;
    };

    /**
     *      @see https://wiki.openssl.org/index.php/OpenSSL_3.0 section 9.3.3
     */
    namespace DigestAlgorithms {
        /**
         *  very widely used, but not secure
         */
        extern const Common::ConstantProperty<DigestAlgorithm> kMD5;

        /**
         *  very widely used, but not secure
         */
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA1;

        /**
         *  kSHA224, kSHA256, kSHA384, and kSHA512 are generally a good (secure) choice
         */
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA1_224;
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA1_256;
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA1_384;
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA1_512;

        /**
         *  https://www.veracode.com/blog/research/message-digests-aka-hashing-functions
         */
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA3_224;
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA3_256;
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA3_384;
        extern const Common::ConstantProperty<DigestAlgorithm> kSHA3_512;
    }

#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DigestAlgorithm.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_DigestAlgorithm_h_*/
