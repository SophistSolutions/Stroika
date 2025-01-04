/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_SocketStream_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_SocketStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/ClientContext.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/LibraryContext.h"
#include "Stroika/Foundation/Cryptography/Providers/OpenSSL/ServerContext.h"
#include "Stroika/Foundation/Cryptography/SSL/SocketStream.h"

/**
 *  \file
 *
 */

namespace Stroika::Foundation::Cryptography::Providers::OpenSSL::SocketStream {

#if qStroika_HasComponent_OpenSSL

    /**
     */
    struct LibRepType : unique_ptr<::SSL, decltype (&::SSL_free)> {
        using inherited = unique_ptr<::SSL, decltype (&::SSL_free)>;

        LibRepType (nullptr_t);
        LibRepType (LibRepType&&) = default;
        LibRepType (::SSL* p);
    };

    /**
     */
    struct IRep : SSL::SocketStream::IRep {
        virtual ::SSL* Get_SSL () const = 0;
    };

    /**
    * logically a combo of shared_ptr<IRep> - current rep - and 'subclass' of SSL::SocketStream::Ptr
     */
    struct Ptr : shared_ptr<IRep> {
        using inherited = shared_ptr<IRep>;
        /**
         *  (1) normal shared_ptr constructors supported
         *  (2) copy from const shared_ptr<IRep>&, to clarify overload avoid ambiguity
         *  (3) shared_ptr<SSL::SocketStream::IRep>& - a dynamic_pointer_cast - which only works - which throws if not the right type
         */
        using inherited::inherited;
        //Ptr (const shared_ptr<IRep>& p);
        Ptr (const shared_ptr<SSL::SocketStream::IRep>& p);

        operator SSL::SocketStream::Ptr () const;

        /**
         */
        nonvirtual ::SSL* Get_SSL () const;
    };

    /**
     */
    Ptr New (LibRepType&& r);
    Ptr New (const IO::Network::ConnectionOrientedStreamSocket::Ptr& sd, const ClientContext::Options& o);
    Ptr New (const IO::Network::ConnectionOrientedStreamSocket::Ptr& sd, const ServerContext::Options& o);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SocketStream.inl"

#endif /*_Stroika_Foundation_Cryptography_OpenSSL_SocketStream_h_*/
