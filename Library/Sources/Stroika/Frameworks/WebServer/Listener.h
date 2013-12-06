/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Listener_h_
#define _Stroika_Framework_WebServer_Listener_h_  1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/IO/Network/Socket.h"
#include    "../../Foundation/IO/Network/SocketAddress.h"
#include    "Request.h"
#include    "Response.h"



/*
 * TODO:
 *      (o)
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;
            using   namespace   Stroika::Foundation::IO;
            using   Characters::String;
            using   IO::Network::SocketAddress;

            using   IO::Network::Socket;


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




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Listener.inl"

#endif  /*_Stroika_Framework_WebServer_Listener_h_*/
