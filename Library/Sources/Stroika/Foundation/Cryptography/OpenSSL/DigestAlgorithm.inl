/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::OpenSSL {

#if qHasFeature_OpenSSL

    /*
     ********************************************************************************
     ***************************** OpenSSL::DigestAlgorithm *************************
     ********************************************************************************
     */
    inline DigestAlgorithm::DigestAlgorithm (const DigestAlgorithm& src)
        : DigestAlgorithm{src.fDigester_}
    {
    }
    inline DigestAlgorithm& DigestAlgorithm::operator= (const DigestAlgorithm& src)
    {
        fDigester_ = src.fDigester_;
        return *this;
    }
    inline DigestAlgorithm::operator const EVP_MD* () const
    {
        return fDigester_;
    }
    inline auto DigestAlgorithm::operator<=> (const DigestAlgorithm& rhs) const
    {
        return fDigester_ <=> rhs.fDigester_;
    }
    inline bool DigestAlgorithm::operator== (const DigestAlgorithm& rhs) const
    {
        return fDigester_ == rhs.fDigester_;
    }
    inline String DigestAlgorithm::ToString () const
    {
        return String{::EVP_MD_name (fDigester_)};
    }
#endif

}
