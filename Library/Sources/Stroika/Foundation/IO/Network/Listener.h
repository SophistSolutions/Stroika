/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
 *      @todo   Add CTOR taking a LIST of sockaddr/callbacks (one for each).
 *
 *      @todo   One trivial impl is to do one listner thread for each, but thats bad. Instead should
 *              use select (need to add Socket::Select(set<Socket> s) to Socket class - static method.
 *
 *              Note - maybe use pselect, or poll() or epoll() etc... Read carefully what is best.
 *
 *      @todo   Documenent carefully any threading issues (most likely requiremnt that callback goes
 *              quickly - does little more than spin off another thread. Perhaps add internal feature
 *              for this class to protect itself by running those callbacks on a new or dedicated thread?
 *
 *      @todo   Need CTOR params for listen backlog
 *
 *      @todo   Possibly need CTOR variant where user passes in Socket, instead of SockAddr?
 *
 *      @todo   Document clearly idea that lifetime of this object CONTROLS thread (doing listen). Destory
 *              object auto-destroys listening thread (and any running accepts callbacks).
 *
 *              May want a shutdown method - so user can control timeout?
 *
 *      @todo   Need to handle SSL? Could be having a special SSL-socket? that does the opessl ssl_accpet()?
 *              Or ??? not sure where best.
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   IO {
            namespace   Network {

                /*
                 *  This maintains thread inside. VERY ROUGH DRAFT API. See Todo above
                 *
                 *
                */
                class   Listener {
                public:
                    // Define params object for stuff like listen backlog (for now hardwire)
                    Listener (const SocketAddress& addr, const function<void(Socket newConnection)>& newConnectionAcceptor);
                    Listener (const Listener&& rhs);
                    Listener (const Listener&) = delete;
                public:
                    Listener& operator= (const Listener&) = delete;

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
