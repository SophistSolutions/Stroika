/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::PrivateKey {

#if qStroika_HasComponent_OpenSSL
    /*
     ********************************************************************************
     ********************* OpenSSL::PrivateKey::LibRepType **************************
     ********************************************************************************
     */
    inline LibRepType::LibRepType (nullptr_t)
        : inherited{nullptr, &::EVP_PKEY_free}
    {
    }
    inline LibRepType::LibRepType (EVP_PKEY* p)
        : inherited{p, &::EVP_PKEY_free}
    {
    }

    /*
     ********************************************************************************
     *************************** OpenSSL::PrivateKey::Ptr ***************************
     ********************************************************************************
     */
    inline Ptr::Ptr (const shared_ptr<IRep>& p)
        : inherited{p}
    {
    }
    inline Ptr::Ptr (const shared_ptr<PKI::PrivateKey::IRep>& p)
    {
        if (auto pp = dynamic_pointer_cast<IRep> (p)) [[likely]] {
            *this = Ptr{pp};
        }
        else {
            Execution::Throw (bad_cast{});
        }
    }
    inline EVP_PKEY* Ptr::Get_EVP_PKEY () const
    {
        return get ()->Get_EVP_PKEY ();
    }

#endif

}
