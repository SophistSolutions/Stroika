/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "InterceptorChain.h"


using   namespace   Stroika::Foundation;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;



struct  InterceptorChain::Rep_ : InterceptorChain::_IRep {
    Rep_ (const Sequence<Interceptor>& interceptors)
        : fInterceptors_ (interceptors)
    {
    }
    virtual Sequence<Interceptor>   GetInterceptors () const override
    {
        return fInterceptors_;
    }
    virtual void                    SetInterceptors (const Sequence<Interceptor>& interceptors) override
    {
        fInterceptors_ = interceptors;
    }
    virtual void    HandleFault (Message* m, const exception_ptr& e) override
    {
        //@todo tmphack - needs locking - really Stroika needs reverse-iterator for Sequence<>
        for (size_t i = fInterceptors_.size (); i > 0; i--) {
            fInterceptors_[i - 1].HandleFault (m, e);
        }
    }
    virtual void    HandleMessage (Message* m) override
    {
        fInterceptors_.Apply ([m] (Interceptor i) { i.HandleMessage (m); });
    }
    Sequence<Interceptor>   fInterceptors_;
};


/*
 ********************************************************************************
 *********************** WebServer::InterceptorChain ****************************
 ********************************************************************************
 */
InterceptorChain::InterceptorChain (const Sequence<Interceptor>& interceptors)
    : fRep_ (make_shared<Rep_> (interceptors))
{
}
