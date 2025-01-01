/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_OpenSSL_SocketStream_h_
#define _Stroika_Foundation_Cryptography_OpenSSL_SocketStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Cryptography/OpenSSL/ClientContext.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/LibraryContext.h"
#include "Stroika/Foundation/Cryptography/OpenSSL/ServerContext.h"
#include "Stroika/Foundation/Cryptography/SSL/SocketStream.h"

/**
 *  \file
 *
 *      TODO:
 *
 */

namespace Stroika::Foundation::Cryptography::OpenSSL::SocketStream {

#if qStroika_HasComponent_OpenSSL
    /**
        // very rough - going to need more stuff
     */
    using Ptr = Cryptography::SSL::SocketStream::Ptr;

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
