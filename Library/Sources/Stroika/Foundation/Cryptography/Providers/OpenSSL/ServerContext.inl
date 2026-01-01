/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::ServerContext {

#if qStroika_HasComponent_OpenSSL
    /*
     ********************************************************************************
     ******************* OpenSSL::ServerContext::LibRepType *************************
     ********************************************************************************
     */
    inline LibRepType::LibRepType (nullptr_t)
        : inherited{nullptr, SSL_CTX_free}
    {
    }
    inline LibRepType::LibRepType (SSL_CTX* p)
        : inherited{p, &::SSL_CTX_free}
    {
    }

    /*
     ********************************************************************************
     ************************* OpenSSL::ServerContext::Ptr **************************
     ********************************************************************************
     */
    inline Ptr::Ptr (const shared_ptr<IRep>& p)
        : inherited{p}
    {
    }
    inline Ptr::Ptr (const shared_ptr<SSL::ServerContext::IRep>& p)
    {
        if (auto pp = dynamic_pointer_cast<IRep> (p)) [[likely]] {
            *this = Ptr{pp};
        }
        else {
            Execution::Throw (bad_cast{});
        }
    }
#endif

}
