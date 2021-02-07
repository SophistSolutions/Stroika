/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_InterceptorChain_inl_
#define _Stroika_Frameworks_WebServer_InterceptorChain_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     *************************** WebServer::InterceptorChain ************************
     ********************************************************************************
     */
    inline void InterceptorChain::HandleMessage (Message* m) const
    {
        fRep_.cget ().load ()->HandleMessage (m);
    }

}
#endif /*_Stroika_Frameworks_WebServer_InterceptorChain_inl_*/
