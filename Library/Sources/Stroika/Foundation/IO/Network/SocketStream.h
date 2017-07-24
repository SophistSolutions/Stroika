/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_h_
#define _Stroika_Foundation_IO_Network_SocketStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Streams/InputOutputStream.h"
#include "../../Streams/InternallySyncrhonizedInputOutputStream.h"

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
                     *           SocketStream::Ptr             socketStream = SocketStream::New (connectionSocket);
                     *           InputStream<Byte>::Ptr        in  = BufferedInputStream<Byte>::New (socketStream);  // not important, but a good idea, to avoid excessive kernel calls
                     *           OutputStream<Byte>::Ptr       out = BufferedOutputStream<Byte>::New (socketStream); // more important so we dont write multiple packets
                     *      \endcode
                     */
                    static Ptr New (Execution::InternallySyncrhonized internallySyncrhonized, const ConnectionOrientedSocket::Ptr& sd);
                    static Ptr New (const ConnectionOrientedSocket::Ptr& sd);

                private:
                    class Rep_;

                private:
                    template <typename X>
                    using BLAH_            = SocketStream;
                    using InternalSyncRep_ = Streams::InternallySyncrhonizedInputOutputStream<Memory::Byte, BLAH_, Network::SocketStream::Rep_>;
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
                     *            SocketStream::Ptr             inOut = SocketStream::New (connectionSocket);
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
