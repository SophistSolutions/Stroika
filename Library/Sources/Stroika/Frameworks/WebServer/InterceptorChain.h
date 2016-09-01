/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_InterceptorChain_h_
#define _Stroika_Framework_WebServer_InterceptorChain_h_  1

#include    "../StroikaPreComp.h"

#include    "Interceptor.h"


/*
 */

/*
 * TODO:
 *      @todo   VERY PRELIMINARY
 *
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;


            /**
             */
            class   InterceptorChain {
            protected:
                class   _IRep;

            public:
                InterceptorChain ();
                InterceptorChain (const Interceptor&) = delete;
                InterceptorChain (InterceptorChain&&) = default;
            protected:
                InterceptorChain (const shared_ptr<_IRep>& rep);

            public:
                /**
                 *  Called for all interceptors (in reverse order) on which handleMessage had been successfully invoked,
                 *  when normal execution of the chain was aborted for some reason.
                 */
                nonvirtual  void    HandleFault (Message* m, const exception_ptr& e);

            public:
                /**
                 *  Intercepts a message
                 */
                nonvirtual  void    HandleMessage (Message* m);

            private:
                shared_ptr<_IRep>   fRep_;

            private:
                struct  Rep_;
            };


            /**
             */
            class   InterceptorChain::_IRep {
            public:
                // Called for all interceptors (in reverse order) on which handleMessage had been successfully invoked, when normal execution of the chain was aborted for some reason.
                virtual void    HandleFault (Message* m, const exception_ptr& e) = 0;

                // Intercepts a message
                virtual void    HandleMessage (Message* m) = 0;
            };


        }
    }
}




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "InterceptorChain.inl"

#endif  /*_Stroika_Framework_WebServer_Interceptor_h_*/
