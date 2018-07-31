/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Router_inl_
#define _Stroika_Frameworks_WebServer_Router_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ******************************** WebServer::Route ******************************
     ********************************************************************************
     */
    inline Route::Route (const RegularExpression& verbMatch, const RegularExpression& pathMatch, const RequestHandler& handler)
        : fVerbMatch_ (verbMatch)
        , fPathMatch_ (pathMatch)
        , fHandler_ (handler)
    {
    }
    inline Route::Route (const RegularExpression& pathMatch, const RequestHandler& handler)
        : fPathMatch_ (pathMatch)
        , fHandler_ (handler)
    {
    }
    inline Route::Route (const function<bool(const Request&)>& requestMatcher, const RequestHandler& handler)
        : fRequestMatch_ (requestMatcher)
        , fHandler_ (handler)
    {
    }

}

#endif /*_Stroika_Frameworks_WebServer_Router_inl_*/
