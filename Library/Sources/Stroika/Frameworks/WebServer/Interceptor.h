/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Interceptor_h_
#define _Stroika_Framework_WebServer_Interceptor_h_  1

#include    "../StroikaPreComp.h"

#include    "Message.h"


/*
 */

/*
 * TODO:
 *      @todo   VERY PRELIMINARY
 *
 *      @todo   Consider if HandleMessage (what CXF has) sb HandleMessagePre, HandleMessagePost, or have separate interceptors
 *              DOCUMENT/THINK OUT CHAIN...
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;


            /**
             */
            class   Interceptor {
            protected:
                class   _IRep;

            public:
                /**
                 */
                Interceptor () = delete;
                Interceptor (const Interceptor&) = default;
                Interceptor (Interceptor&&) = default;
            protected:
                Interceptor (const shared_ptr<_IRep>& rep);

            public:
                nonvirtual  Interceptor&    operator= (const Interceptor&) = default;

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

            protected:
                /**
                 */
                template    <typename T = _IRep>
                nonvirtual  const T &    _GetRep () const;

            private:
                shared_ptr<_IRep>   fRep_;
            };


            /**
             */
            class   Interceptor::_IRep {
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
#include    "Interceptor.inl"

#endif  /*_Stroika_Framework_WebServer_Interceptor_h_*/
