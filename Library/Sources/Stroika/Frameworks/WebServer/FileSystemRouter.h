/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_FileSystemRouter_h_
#define _Stroika_Framework_WebServer_FileSystemRouter_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/IO/Network/URL.h"

#include "Router.h"

/*
 * TODO:
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 */

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            using namespace Stroika::Foundation;

            using Characters::String;
            using Characters::RegularExpression;
            using Containers::Sequence;
            using IO::Network::URL;
            using Memory::Optional;

            class Router;

            /**
             *  @todo unclear if this should BE a Router or RequestHandler, but I htink Handler, and so rename!
             */
            class FileSystemRouter : public RequestHandler {
            public:
                /**
                 *  Any route to apply the handler, must match ALL argument constraints.
                 */
                FileSystemRouter (const String& filesystemRoot, const Optional<String>& urlPrefix2Strip);
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileSystemRouter.inl"

#endif /*_Stroika_Framework_WebServer_FileSystemRouter_h_*/
