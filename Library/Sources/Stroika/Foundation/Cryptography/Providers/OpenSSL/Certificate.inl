/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::Certificate {

#if qStroika_HasComponent_OpenSSL
    /*
     ********************************************************************************
     ********************* OpenSSL::Certificate::LibRepType *************************
     ********************************************************************************
     */
    inline LibRepType::LibRepType (nullptr_t)
        : inherited{nullptr, &::X509_free}
    {
    }
    inline LibRepType::LibRepType (X509* p)
        : inherited{p, &::X509_free}
    {
    }

    /*
     ********************************************************************************
     *************************** OpenSSL::Certificate::Ptr **************************
     ********************************************************************************
     */
    inline Ptr::Ptr (const shared_ptr<IRep>& p)
        : inherited{p}
    {
    }
    inline Ptr::Ptr (const shared_ptr<PKI::Certificate::IRep>& p)
    {
        if (auto pp = dynamic_pointer_cast<IRep> (p)) [[likely]] {
            *this = Ptr{pp};
        }
        else {
            Execution::Throw (bad_cast{});
        }
    }
    inline X509* Ptr::Get_X509 () const
    {
        return get ()->Get_X509 ();
    }
#endif

}
