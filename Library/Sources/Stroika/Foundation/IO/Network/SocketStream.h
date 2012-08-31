/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_SocketStream_h_
#define _Stroika_Foundation_IO_Network_SocketStream_h_  1

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Streams/BinaryInputOutputStream.h"
#include    "Socket.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

#if 0
                // Platform Socket descriptor - file descriptor on unix (something like this on windoze)
                typedef int SocketDescriptor;
#endif


                /*
                 * A SocketStream wraps a a socket as a Binary Stream (input and output) - but note that the two steams are totally
                 * separate, and and have almost nothing todo with one another. Writes to the output dont appear in the input.
                 *
                 * The only real conneciton is that they share a common socket, and if IT is closed, then the whole SocketStream will stop working.
                 */
                class   SocketStream : public Streams::BinaryInputOutputStream {
				private:
					class   IRep_;
                public:
                    // Note - socket is CLOSED (filesystem close for now) in DTOR
                    // TODO:
                    //          We will need an abstract Socket object, and maybe have  it refernce counted so close can happen when last refernce goes
                    //  away!
                    //
                    explicit SocketStream (Socket sd);
                };

            }
        }
    }
}
#endif  /*_Stroika_Foundation_IO_Network_SocketStream_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SocketStream.inl"
