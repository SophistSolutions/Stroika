/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_FileSystemRouter_h_
#define _Stroika_Framework_WebServer_FileSystemRouter_h_ 1

#include "../StroikaPreComp.h"

#include "FileSystemRequestHandler.h"

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using Characters::RegularExpression;
    using Characters::String;
    using Containers::Sequence;

    class Router;

    using FileSystemRouter [[deprecated ("Since Stroika 2.1b10, use FileSystemRequestHandler")]] = FileSystemRequestHandler;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Framework_WebServer_FileSystemRouter_h_*/
