/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "InterceptorChain.h"


using   namespace   Stroika::Foundation;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::WebServer;



struct InterceptorChain::Rep_ : InterceptorChain::_IRep {
    virtual void    HandleFault (Message* m, const exception_ptr& e) override
    {
    }
    virtual void    HandleMessage (Message* m) override
    {
    }

};


/*
 ********************************************************************************
 *********************** WebServer::InterceptorChain ****************************
 ********************************************************************************
 */

InterceptorChain::InterceptorChain ()
    : fRep_ (make_shared<Rep_> ())
{
}
