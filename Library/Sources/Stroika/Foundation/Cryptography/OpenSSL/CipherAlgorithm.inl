/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_inl_
#define _Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

#if qHasFeature_OpenSSL

    /*
     ********************************************************************************
     ***************************** OpenSSL::CipherAlgorithm *************************
     ********************************************************************************
     */
    inline CipherAlgorithm::CipherAlgorithm (const CipherAlgorithm& src)
        : CipherAlgorithm{src.fCipher_}
    {
    }
    inline CipherAlgorithm& CipherAlgorithm::operator= (const CipherAlgorithm& src)
    {
        fCipher_ = src.fCipher_;
        return *this;
    }
    inline CipherAlgorithm::operator const EVP_CIPHER* () const
    {
        return fCipher_;
    }
#if __cpp_impl_three_way_comparison >= 201907
    inline auto CipherAlgorithm::operator<=> (const CipherAlgorithm& rhs) const
    {
        return fCipher_ <=> rhs.fCipher_;
    }
#else
    inline auto CipherAlgorithm::operator< (const CipherAlgorithm& rhs) const
    {
        return fCipher_ < rhs.fCipher_;
    }
#endif
    inline bool CipherAlgorithm::operator== (const CipherAlgorithm& rhs) const
    {
        return fCipher_ == rhs.fCipher_;
    }
    inline String CipherAlgorithm::ToString () const
    {
        return String::FromASCII (::EVP_CIPHER_name (fCipher_));
    }

    [[deprecated ("Since Stroika 2.1b12")]] inline const EVP_CIPHER* Convert2OpenSSL (CipherAlgorithm alg)
    {
        return alg;
    }
#endif

}

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_CipherAlgorithm_inl_*/
