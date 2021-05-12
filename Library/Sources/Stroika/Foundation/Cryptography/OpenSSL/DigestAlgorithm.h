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
#include "../../Execution/VirtualConstant.h"
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

    namespace DigestAlgorithms {
        /**
         *  very widely used, but not secure
         */
        extern const Execution::VirtualConstant<DigestAlgorithm> kMD5;

        /**
         *  very widely used, but not secure
         */
        extern const Execution::VirtualConstant<DigestAlgorithm> kSHA1;

        /**
         *  kSHA224, kSHA256, kSHA384, and kSHA512 are generally a good (secure) choice
         */
        extern const Execution::VirtualConstant<DigestAlgorithm> kSHA224;
        extern const Execution::VirtualConstant<DigestAlgorithm> kSHA256;
        extern const Execution::VirtualConstant<DigestAlgorithm> kSHA384;
        extern const Execution::VirtualConstant<DigestAlgorithm> kSHA512;
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
