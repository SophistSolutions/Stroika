/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_DefaultFaultInterceptor_h_
#define _Stroika_Framework_WebServer_DefaultFaultInterceptor_h_ 1

#include    "../StroikaPreComp.h"

#include    "Interceptor.h"


/*
 * TODO:
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;


            /**
             */
            class   DefaultFaultInterceptor : public Interceptor {
            private:
                using   inherited = Interceptor;

            public:
                DefaultFaultInterceptor ();
                DefaultFaultInterceptor (const function<void (Message*, const exception_ptr&) noexcept>& handleFault);

            private:
                struct  Rep_;
                struct  Rep_Explicit_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DefaultFaultInterceptor.inl"

#endif  /*_Stroika_Framework_WebServer_DefaultFaultInterceptor_h_*/
