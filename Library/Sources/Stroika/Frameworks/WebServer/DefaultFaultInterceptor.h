/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_DefaultFaultInterceptor_h_
#define _Stroika_Framework_WebServer_DefaultFaultInterceptor_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include "Stroika/Frameworks/WebServer/Interceptor.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;

    /**
     */
    class DefaultFaultInterceptor : public Interceptor {
    private:
        using inherited = Interceptor;

    public:
        DefaultFaultInterceptor ();
        DefaultFaultInterceptor (const function<void (Message&, const exception_ptr&)>& handleFault);
        [[deprecated ("Since Stroika v3.0d12 use Message&")]] DefaultFaultInterceptor (const function<void (Message*, const exception_ptr&)>& handleFault);

    private:
        struct Rep_;
        struct Rep_Explicit_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DefaultFaultInterceptor.inl"

#endif /*_Stroika_Framework_WebServer_DefaultFaultInterceptor_h_*/
