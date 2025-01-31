/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
    inline void InterceptorChain::HandleMessage (Message& m) const
    {
        fRep_.cget ().load ()->HandleMessage (m);
    }
    inline bool InterceptorChain::operator== (const InterceptorChain& rhs) const
    {
        return fRep_.cget ().load ()->GetInterceptors () == rhs.fRep_.cget ().load ()->GetInterceptors ();
    }

}
