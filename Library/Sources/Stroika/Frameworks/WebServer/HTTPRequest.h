/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_HTTPRequest_h_
#define _Stroika_Framework_WebServer_HTTPRequest_h_ 1

#include    "../StroikaPreComp.h"

#include    <map>
#include    <string>
#include    <vector>

#include    "../../Foundation/Characters/String.h"
#include    "../../Foundation/Configuration/Common.h"
#include    "../../Foundation/DataExchangeFormat/InternetMediaType.h"
#include    "../../Foundation/IO/Network/URL.h"
#include    "../../Foundation/Streams/BinaryInputStream.h"
#include    "../../Foundation/Streams/TextInputStreamBinaryAdapter.h"


/*
 * TODO:
 *      (o)
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;
            using   Characters::String;
            using   DataExchangeFormat::InternetMediaType;

            // Maybe associated TextStream, and maybe readline method goes here
            struct  HTTPRequest {
            public:
                HTTPRequest (Streams::BinaryInputStream& inStream);

            private:
                NO_DEFAULT_CONSTRUCTOR (HTTPRequest);
                NO_COPY_CONSTRUCTOR (HTTPRequest);
                NO_ASSIGNMENT_OPERATOR (HTTPRequest);

            public:
                Streams::BinaryInputStream&             fInputStream;
                Streams::TextInputStreamBinaryAdapter   fInputTextStream;

            public:
                String                                  fHTTPVersion;
                String                                  fMethod;
                IO::Network::URL                        fURL;
                map<String, String>                      fHeaders;
            };

        }
    }
}
#endif  /*_Stroika_Framework_WebServer_HTTPRequest_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "HTTPRequest.inl"
