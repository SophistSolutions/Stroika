/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Connection_h_
#define _Stroika_Framework_WebServer_Connection_h_  1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/IO/Network/SocketStream.h"
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

            using   Stroika::Foundation::IO::Network::Socket;
            using   Stroika::Foundation::IO::Network::SocketStream;

            class   Connection {
            public:
                Connection () = delete;
                Connection (const Connection&) = delete;
                explicit Connection (Socket s);
            public:
                ~Connection ();

            public:
                nonvirtual  const Connection& operator= (const Connection&) = delete;

            public:
                // Must rethink this organization -but for now - call this once at start of connection to fill in details in
                // the  Request object
                nonvirtual  void    ReadHeaders ();
// not sure we want this
                nonvirtual  void    Close ();


            public:
                nonvirtual  Socket              GetSocket () const;
                nonvirtual  Request&        GetRequest ();
                nonvirtual  const Request&  GetRequest () const;
                nonvirtual  Response&       GetResponse ();
                nonvirtual  const Response& GetResponse () const;

            private:
                Socket          fSocket_;
                SocketStream    fSocketStream_;
                Request     fRequest_;
                Response    fResponse_;
            };

        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Connection.inl"

#endif  /*_Stroika_Framework_WebServer_Connection_h_*/
