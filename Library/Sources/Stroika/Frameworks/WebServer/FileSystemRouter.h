/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_FileSystemRouter_h_
#define _Stroika_Framework_WebServer_FileSystemRouter_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../../Foundation/IO/Network/URL.h"

#include "Router.h"

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
 * TODO:
 *      @todo   Add optional logger feature, to log requests (possibly in standard https://www.w3.org/TR/WD-logfile)
 *
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using Characters::RegularExpression;
    using Characters::String;
    using Containers::Sequence;
    using IO::Network::URL;

    class Router;

    /**
     *  @todo unclear if this should BE a Router or RequestHandler, but I htink Handler, and so rename!
     */
    class FileSystemRouter : public RequestHandler {
    public:
        /**
         *  Any route to apply the handler, must match ALL argument constraints.
         */
        FileSystemRouter (const String& filesystemRoot, const optional<String>& urlPrefix2Strip, const Sequence<String>& defaultIndexFileNames = {});
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "FileSystemRouter.inl"

#endif /*_Stroika_Framework_WebServer_FileSystemRouter_h_*/
