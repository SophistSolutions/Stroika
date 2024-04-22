/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
    inline auto CipherAlgorithm::operator<=> (const CipherAlgorithm& rhs) const
    {
        return fCipher_ <=> rhs.fCipher_;
    }
    inline bool CipherAlgorithm::operator== (const CipherAlgorithm& rhs) const
    {
        return fCipher_ == rhs.fCipher_;
    }
    inline String CipherAlgorithm::ToString () const
    {
        return String{::EVP_CIPHER_name (fCipher_)};
    }
    inline size_t CipherAlgorithm::KeyLength () const
    {
        return ::EVP_CIPHER_key_length (fCipher_);
    }
    inline size_t CipherAlgorithm::IVLength () const
    {
        return ::EVP_CIPHER_iv_length (fCipher_);
    }
#endif

}
