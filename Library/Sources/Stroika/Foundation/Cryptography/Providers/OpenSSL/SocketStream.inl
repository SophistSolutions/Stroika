/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::SocketStream {

#if qStroika_HasComponent_OpenSSL
    /*
     ********************************************************************************
     ********************* OpenSSL::SocketStream::LibRepType ************************
     ********************************************************************************
     */
    inline LibRepType::LibRepType (nullptr_t)
        : inherited{nullptr, &::SSL_free}
    {
    }
    inline LibRepType::LibRepType (::SSL* p)
        : inherited{p, &::SSL_free}
    {
    }

    /*
     ********************************************************************************
     *************************** OpenSSL::SocketStream::Ptr *************************
     ********************************************************************************
     */
    /*inline Ptr::Ptr (const shared_ptr<IRep>& p)
        : inherited{p}
    {
    }*/
    inline Ptr::Ptr (const shared_ptr<SSL::SocketStream::IRep>& p)
    {
        if (auto pp = dynamic_pointer_cast<IRep> (p)) [[likely]] {
            *this = Ptr{pp};
        }
        else {
            Execution::Throw (bad_cast{});
        }
    }
    inline Ptr::operator SSL::SocketStream::Ptr () const
    {
        if (auto pp = dynamic_pointer_cast<SSL::SocketStream::IRep> (*this)) [[likely]] {
            return SSL::SocketStream::Ptr{pp};
        }
        AssertNotReached ();
        return nullptr;
    }
    inline ::SSL* Ptr::Get_SSL () const
    {
        return get ()->Get_SSL ();
    }
#endif

}
