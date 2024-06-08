/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_h_
#define _Stroika_Foundation_IO_Network_SocketStream_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/IO/Network/ConnectionOrientedStreamSocket.h"
#include "Stroika/Foundation/Streams/InputOutputStream.h"

/**
 *  \file
 *
 *      TODO:
 *
 */

namespace Stroika::Foundation::IO::Network::SocketStream {

    /**
     *  \par Example Usage
     *      \code
     *            ConnectionOrientedStreamSocket::Ptr connectionSocket = from_somewhere;
     *            SocketStream::Ptr                   inOut = SocketStream::New (connectionSocket);
     *      \endcode
     */
    using Ptr = Streams::InputOutputStream::Ptr<byte>;
    /**
     *  A SocketStream wraps a a socket as a InputOutputStream - two separate but related streams.
     *
     *  The only real connection is that they share a common socket, and if it is closed,
     *  then the whole SocketStream will stop working.
     *
     *      \note   SocketStream aggregates its owned ConnectionOrientedStreamSocket, so that a Close () on SocketStream
     *              will Close that socket as well.
     *
     *              But SocketStream is an InputOutputStream - so you can close the input and output sides separately.
     *              If you call close on only one side of the input stream, Shutdown () will be used to shutdown
     *              just that end of the stream.
     */

    /**
     *  To copy a SocketStream, use SocketStream<T>::Ptr
     *
     *  \par Example Usage
     *      \code
     *           ConnectionOrientedStreamSocket::Ptr connectionSocket = from_somewhere;
     *           SocketStream::Ptr                   socketStream = SocketStream::New (connectionSocket);
     *           InputStream::Ptr<byte>              in  = BufferedInputStream::New<byte> (socketStream);  // not important, but a good idea, to avoid excessiveos read/write calls
     *           OutputStream::Ptr<byte>             out = BufferedOutputStream::New<byte> (socketStream); // more important so we don't write multiple packets
     *      \endcode
     */
    Ptr New (Execution::InternallySynchronized internallySynchronized, const ConnectionOrientedStreamSocket::Ptr& sd);
    Ptr New (const ConnectionOrientedStreamSocket::Ptr& sd);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SocketStream.inl"

#endif /*_Stroika_Foundation_IO_Network_SocketStream_h_*/
