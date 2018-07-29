/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_h_
#define _Stroika_Foundation_IO_Network_SocketStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Streams/InputOutputStream.h"
#include "../../Streams/InternallySyncrhonizedInputOutputStream.h"

#include "ConnectionOrientedStreamSocket.h"

/**
 *  \file
 *
 *      TODO:
 *
 */

namespace Stroika::Foundation::IO::Network {

    /**
     *  A SocketStream wraps a a socket as a InputOutputStream - two separate but related streams.
     *
     *  The only real conneciton is that they share a common socket, and if it is closed,
     *  then the whole SocketStream will stop working.
     *
     *      \note   SocketStream aggregates its owned ConnectionOrientedStreamSocket, so that a Close () on SocketStream
     *              will Close that socket as well.
     *
     *              But SocketStream is an InputOutputStream - so you can close the input and output sides separately.
     *              If you call close on only one side of the input stream, Shutdown () will be used to shutdown
     *              just that end of the stream.
     */
    class SocketStream : public Streams::InputOutputStream<Memory::Byte> {
    public:
        SocketStream ()                    = delete;
        SocketStream (const SocketStream&) = delete;

    public:
        class Ptr;

    public:
        /**
         *  To copy a ExternallyOwnedMemoryInputStream, use ExternallyOwnedMemoryInputStream<T>::Ptr
         *
         *  \par Example Usage
         *      \code
         *           ConnectionOrientedStreamSocket::Ptr connectionSocket = from_somewhere;
         *           SocketStream::Ptr                   socketStream = SocketStream::New (connectionSocket);
         *           InputStream<Byte>::Ptr              in  = BufferedInputStream<Byte>::New (socketStream);  // not important, but a good idea, to avoid excessive kernel calls
         *           OutputStream<Byte>::Ptr             out = BufferedOutputStream<Byte>::New (socketStream); // more important so we dont write multiple packets
         *      \endcode
         */
        static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const ConnectionOrientedStreamSocket::Ptr& sd);
        static Ptr New (const ConnectionOrientedStreamSocket::Ptr& sd);

    private:
        class Rep_;

    protected:
        /**
         *  Utility to create a Ptr wrapper (to avoid having to subclass the Ptr class and access its protected constructor)
         */
        static Ptr _mkPtr (const shared_ptr<Rep_>& s);

    private:
        using InternalSyncRep_ = Streams::InternallySyncrhonizedInputOutputStream<Memory::Byte, SocketStream, SocketStream::Rep_>;
    };

    /**
     *  Ptr is a copyable smart pointer to a ExternallyOwnedMemoryInputStream.
     *
     *  TODO:
     *      @todo add method to retrieve underlying socket
     */
    class SocketStream::Ptr : public InputOutputStream<Memory::Byte>::Ptr {
    private:
        using inherited = InputOutputStream<Memory::Byte>::Ptr;

    public:
        /**
         *  \par Example Usage
         *      \code
         *            ConnectionOrientedStreamSocket::Ptr connectionSocket = from_somewhere;
         *            SocketStream::Ptr                   inOut = SocketStream::New (connectionSocket);
         *      \endcode
         */
        Ptr ()                = default;
        Ptr (const Ptr& from) = default;
        Ptr (nullptr_t);

    protected:
        Ptr (const shared_ptr<Rep_>& from);

    public:
        nonvirtual Ptr& operator= (const Ptr& rhs) = default;

    private:
        friend class SocketStream;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SocketStream.inl"

#endif /*_Stroika_Foundation_IO_Network_SocketStream_h_*/
