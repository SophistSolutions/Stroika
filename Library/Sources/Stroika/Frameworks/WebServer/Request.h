/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Request_h_
#define _Stroika_Framework_WebServer_Request_h_ 1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <string>
#include    <vector>

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/DataExchange/InternetMediaType.h"
#include    "../../Foundation/IO/Network/URL.h"
#include    "../../Foundation/Streams/InputStream.h"


/*
 * TODO:
 *      @todo   Redo fHeaders as Stroika Association (not Mapping, cuz things like Set-Cookie headers can appear more than once).
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   DataExchange::InternetMediaType;

            // Maybe associated TextStream, and maybe readline method goes here
            struct  Request {
            public:
                Request () = delete;
                Request (const Request&) = delete;
                Request (const Streams::InputStream<Memory::Byte>& inStream);

            public:
                nonvirtual  const Request& operator= (const Request&) = delete;

            public:
                Streams::InputStream<Memory::Byte>      fInputStream;

            public:
                String                                  fHTTPVersion;
                String                                  fMethod;
                IO::Network::URL                        fURL;
                map<String, String>                     fHeaders;
            };

        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Request.inl"

#endif  /*_Stroika_Framework_WebServer_Request_h_*/
