/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
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
