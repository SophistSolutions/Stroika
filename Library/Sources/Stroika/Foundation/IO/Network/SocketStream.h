/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_h_
#define _Stroika_Foundation_IO_Network_SocketStream_h_ 1

#include "../../StroikaPreComp.h"

#include "../../Configuration/Common.h"
#include "../../Streams/InputOutputStream.h"
#include "Socket.h"

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
                 */
                class SocketStream : public Streams::InputOutputStream<Memory::Byte>::Ptr {
                private:
                    using inherited = Streams::InputOutputStream<Memory::Byte>::Ptr;

                public:
                    /**
                     */
                    SocketStream () = delete;
                    explicit SocketStream (ConnectionOrientedSocket sd);
                    SocketStream (const SocketStream&) = delete;

                public:
                    nonvirtual SocketStream& operator= (const SocketStream&) = delete;

                private:
                    class Rep_;
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
