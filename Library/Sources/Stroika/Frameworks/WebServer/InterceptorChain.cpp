/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "InterceptorChain.h"


using   namespace   Stroika::Foundation;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;



// This class MUST be re-entrant (internally synchonized)- and is STATELESS - so no syncro needed
struct  InterceptorChain::Rep_ : InterceptorChain::_IRep {
    Rep_ (const Sequence<Interceptor>& interceptors)
        : fInterceptors_ (interceptors)
    {
    }
    virtual Sequence<Interceptor>   GetInterceptors () const override
    {
        return fInterceptors_;
    }
    virtual shared_ptr<_IRep>       SetInterceptors (const Sequence<Interceptor>& interceptors) const override
    {
        return make_shared<Rep_> (interceptors);
    }
    virtual void    HandleMessage (Message* m) override
    {
        size_t  sz = fInterceptors_.size ();
        for (size_t i = 0; i < sz; ++i) {
            try {
                fInterceptors_[i].HandleMessage (m);
            }
            catch (...) {
                exception_ptr   e   =   current_exception ();
                do {
                    fInterceptors_[i].HandleFault (m, e);
                }
                while (i-- != 0);
                Execution::ReThrow ();
            }
        }
    }
    const Sequence<Interceptor>   fInterceptors_;       // no synchro needed because always readonly
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
