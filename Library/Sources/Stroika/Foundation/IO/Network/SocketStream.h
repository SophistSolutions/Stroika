/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_h_
#define _Stroika_Foundation_IO_Network_SocketStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Streams/InputOutputStream.h"
#include "ConnectionOrientedSocket.h"

/**
 *  \file
 *
 *      TODO:
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace IO {
            namespace Network {

                /**
                 *  A SocketStream wraps a a socket as a InputOutputStream - two separate but related streams.
                 *
                 *  The only real conneciton is that they share a common socket, and if it is closed,
                 *  then the whole SocketStream will stop working.
                 *
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
                     *           ConnectionOrientedSocket::Ptr connectionSocket = from_somewhere;
                     *           SocketStream                  socketStream{connectionSocket};
                     *           InputStream<Byte>::Ptr        in  = BufferedInputStream<Byte>{socketStream};  // not important, but a good idea, to avoid excessive kernel calls
                     *           OutputStream<Byte>::Ptr       out = BufferedOutputStream<Byte>{socketStream}; // more important so we dont write multiple packets
                     *      \endcode
                     */
                    static Ptr New (const ConnectionOrientedSocket::Ptr& sd);

                private:
                    class Rep_;
                };

                /**
                 *  Ptr is a copyable smart pointer to a ExternallyOwnedMemoryInputStream.
                 */
                class SocketStream::Ptr : public InputOutputStream<Memory::Byte>::Ptr {
                private:
                    using inherited = InputOutputStream<Memory::Byte>::Ptr;

                public:
                    /**
                     *  \par Example Usage
                     *      \code
                     *            ConnectionOrientedSocket::Ptr connectionSocket = from_somewhere;
                     *            SocketStream                  socketStream{connectionSocket};
                     *            SocketStream::Ptr             inOut = socketStream;
                     *      \endcode
                     */
                    Ptr ()                = default;
                    Ptr (const Ptr& from) = default;

                private:
                    Ptr (const shared_ptr<Rep_>& from);

                public:
                    nonvirtual Ptr& operator= (const Ptr& rhs) = default;

                private:
                    friend class SocketStream;
                };
            }
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SocketStream.inl"

#endif /*_Stroika_Foundation_IO_Network_SocketStream_h_*/
