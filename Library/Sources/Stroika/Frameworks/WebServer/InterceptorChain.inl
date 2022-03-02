/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
    inline InterceptorChain::InterceptorChain (const InterceptorChain& src)
        : InterceptorChain{src.fRep_.load ()}
    {
    }
    inline InterceptorChain::InterceptorChain (InterceptorChain&& src)
        : InterceptorChain{src.fRep_.load ()}
    {
    }
    inline void InterceptorChain::HandleMessage (Message* m) const
    {
        RequireNotNull (m);
        fRep_.cget ().load ()->HandleMessage (m);
    }

}
#endif /*_Stroika_Frameworks_WebServer_InterceptorChain_inl_*/
