/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_h_
#define _Stroika_Foundation_IO_Network_SocketStream_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryInputOutputStream.h"
#include    "Socket.h"



/**
 *  \file
 *
 *      TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {


                /**
                 *  A SocketStream wraps a a socket as a BinaryTiedStreams - two separate by related streams.
                 *
                 *  The only real conneciton is that they share a common socket, and if IT is closed,
                 *  then the whole SocketStream will stop working.
                 */
                class   SocketStream : public Streams::BinaryInputOutputStream {
                private:
                    using   inherited = Streams::BinaryInputOutputStream;

                public:
                    // Note - socket is CLOSED (filesystem close for now) in DTOR
                    // TODO:
                    //          We will need an abstract Socket object, and maybe have  it refernce counted so close can happen when last refernce goes
                    //  away!
                    //
                    explicit SocketStream (Socket sd);

                private:
                    class   Rep_;
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
#include    "SocketStream.inl"

#endif  /*_Stroika_Foundation_IO_Network_SocketStream_h_*/
