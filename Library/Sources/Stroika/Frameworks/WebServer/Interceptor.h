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
 */

namespace   Stroika {
    namespace   Frameworks {
        namespace   WebServer {

            using   namespace   Stroika::Foundation;


            /**
             *  Each Interceptor envelope must be externally synchonized, but can be read from any thread, and
             *  the way these are used, they are only ever read, except on construction.
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
                 *  Called any interceptor which HandleMessage was invoked on, when a fault prevented completion of
                 *  the interceptor chain.
                 *
                 *  This function should NOT throw an exception - just do what it can to cleanup.
                 */
                nonvirtual  void    HandleFault (Message* m, const exception_ptr& e) noexcept;

            public:
                /**
                 *  Intercepts and handles a message. Typically this will read stuff from the Request and
                 *  add stuff to the Response.
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
                /**
                 *  Called any interceptor which HandleMessage was invoked on, when a fault prevented completion of
                 *  the interceptor chain.
                 *
                 *  This function should NOT throw an exception - just do what it can to cleanup.
                 */
                virtual void    HandleFault (Message* m, const exception_ptr& e) noexcept = 0;

                /**
                 *  Intercepts and handles a message. Typically this will read stuff from the Request and
                 *  add stuff to the Response.
                 */
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
