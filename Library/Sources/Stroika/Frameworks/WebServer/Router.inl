/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ******************************** WebServer::Route ******************************
     ********************************************************************************
     */
    inline Route::Route (const RegularExpression& verbMatch, const RegularExpression& pathMatch, const RequestHandler& handler)
        : fVerbAndPathMatch_{make_pair (verbMatch, pathMatch)}
        , fHandler_{handler}
    {
    }
    inline Route::Route (const RegularExpression& pathMatch, const RequestHandler& handler)
        : Route{IO::Network::HTTP::MethodsRegEx::kGet, pathMatch, handler}
    {
    }
    inline Route::Route (const function<bool (const String& method, const String& hostRelPath, const Request& request)>& requestMatcher,
                         const RequestHandler&                                                                           handler)
        : fRequestMatch_{requestMatcher}
        , fHandler_{handler}
    {
    }

}
