/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Cryptography_SSL_SocketStream_h_
#define _Stroika_Foundation_Cryptography_SSL_SocketStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Cryptography/SSL/ClientContext.h"
#include "Stroika/Foundation/Cryptography/SSL/ServerContext.h"
#include "Stroika/Foundation/IO/Network/SocketStream.h"

/**
 *  \file
 *
 *      TODO:
 *
 */

namespace Stroika::Foundation::Cryptography::SSL::SocketStream {

    /**
        // very rough - going to need more stuff
     */
    using Ptr = IO::Network::SocketStream::Ptr;

    Ptr New (const IO::Network::ConnectionOrientedStreamSocket::Ptr& sd, const ClientContext::Options& o);
    Ptr New (const IO::Network::ConnectionOrientedStreamSocket::Ptr& sd, const ServerContext::Options& o);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SocketStream.inl"

#endif /*_Stroika_Foundation_IO_Network_SSL_SocketStream_h_*/
