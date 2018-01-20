/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            using namespace Stroika::Foundation;

            /**
             */
            class DefaultFaultInterceptor : public Interceptor {
            private:
                using inherited = Interceptor;

            public:
                DefaultFaultInterceptor ();
#if qCompilerAndStdLib_noexcept_declarator_in_std_function_Buggy
                DefaultFaultInterceptor (const function<void(Message*, const exception_ptr&)>& handleFault);
#else
                DefaultFaultInterceptor (const function<void(Message*, const exception_ptr&) noexcept>& handleFault);
#endif

            private:
                struct Rep_;
                struct Rep_Explicit_;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DefaultFaultInterceptor.inl"

#endif /*_Stroika_Framework_WebServer_DefaultFaultInterceptor_h_*/
