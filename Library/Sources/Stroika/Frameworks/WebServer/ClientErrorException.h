/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_ClientErrorException_h_
#define _Stroika_Framework_WebServer_ClientErrorException_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/IO/Network/HTTP/ClientErrorException.h"

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    using ClientErrorException [[deprecated ("use IO::Network::HTTP::ClientErrorException - deprecated since 2.1d18")]] = IO::Network::HTTP::ClientErrorException;

}

#endif /*_Stroika_Framework_WebServer_ClientErrorException_h_*/
