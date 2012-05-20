/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_HTTPConnection_h_
#define _Stroika_Framework_WebServer_HTTPConnection_h_  1

#include    "../StroikaPreComp.h"

#include    <string>
#include    <vector>

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/IO/Network/SocketStream.h"
#include    "HTTPRequest.h"
#include    "HTTPResponse.h"



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

            class   HTTPConnection {
            public:
                explicit HTTPConnection (Socket s);
            public:
                ~HTTPConnection ();

            private:
                NO_DEFAULT_CONSTRUCTOR (HTTPConnection);
                NO_COPY_CONSTRUCTOR (HTTPConnection);
                NO_ASSIGNMENT_OPERATOR (HTTPConnection);

            public:
                // Must rethink this organization -but for now - call this once at start of connection to fill in details in
                // the HTTP Request object
                nonvirtual  void    ReadHeaders ();
// not sure we want this
                nonvirtual  void    Close ();


            public:
                nonvirtual  Socket              GetSocket () const;
                nonvirtual  HTTPRequest&        GetRequest ();
                nonvirtual  const HTTPRequest&  GetRequest () const;
                nonvirtual  HTTPResponse&       GetResponse ();
                nonvirtual  const HTTPResponse& GetResponse () const;

            private:
                Socket          fSocket_;
                SocketStream    fSocketStream_;
                HTTPRequest     fRequest_;
                HTTPResponse    fResponse_;
            };

        }
    }
}
#endif  /*_Stroika_Framework_WebServer_HTTPConnection_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "HTTPConnection.inl"
