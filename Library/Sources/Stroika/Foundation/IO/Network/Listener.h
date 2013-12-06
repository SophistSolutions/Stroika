/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_Listener_h_
#define _Stroika_Foundation_IO_Network_Listener_h_  1

#include    "../../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../Characters/String.h"

#include    "Socket.h"
#include    "SocketAddress.h"



/*
 * TODO:
 *      (o)
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

                /*
                 *  This maintains thread inside. VERY ROUGH DRAFT API.
                 *
                 *
                */
                class   Listener {
                public:
                    // Define params object for stuff like listen backlog (for now hardwire)
                    Listener (const SocketAddress& addr, const function<void(Socket newConnection)>& newConnectionAcceptor);
                    Listener (const Listener&) = delete;
                public:
                    const Listener& operator= (const Listener&) = delete;

                private:
                    struct  Rep_;
                    shared_ptr<Rep_>    fRep_;
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
#include    "Listener.inl"

#endif  /*_Stroika_Foundation_IO_Network_Listener_h_*/
