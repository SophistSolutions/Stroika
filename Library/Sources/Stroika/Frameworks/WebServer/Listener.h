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
