/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_DefaultFaultInterceptor_h_
#define _Stroika_Framework_WebServer_DefaultFaultInterceptor_h_ 1

#include "../StroikaPreComp.h"

#include "Interceptor.h"

/*
 * TODO:
 *
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 *
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
        DefaultFaultInterceptor (const function<void(Message*, const exception_ptr&)>& handleFault);

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
