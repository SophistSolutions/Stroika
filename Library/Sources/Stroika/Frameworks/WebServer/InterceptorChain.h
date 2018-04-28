/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_InterceptorChain_h_
#define _Stroika_Framework_WebServer_InterceptorChain_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Containers/Sequence.h"
#include "../../Foundation/Execution/Synchronized.h"

#include "Interceptor.h"

/*
 */

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
            using Stroika::Foundation::Containers::Sequence;

            /**
             *  The InterceptorChain hands a message to each interceptor in its list in order, from front to back.
             *
             *  Typically early interceptors quickly check and process results and then throw/abort if there is a problem (so auth 
             *  would be handled early in the interceptor chain)
             *
             *  And typically - your message router, or final handlers that return data - would be at the end of the interfceptor chain.
             *
             *  Note - exceptions are handled in the reverse order - passed backwards through the chain.
             *
             *  The InterceptorChain is internally synchonized. But - it assumes each Interceptor its given
             *  is itself internally synchonized.
             *
             *  So we can copy an Interceptor chain quickly and cheaply without (external) locks. Updating it
             *  maybe slower, but also can be done freely without locks.
             *
             *  InterceptorChains are 'by value' objects, so updating one in a Connection object - for example - doesn't affect
             *  other connections (update the one in the ConnectionManager for use in future connections).
             *
             *  \note   Use the ConnectionManager to more easily manage the Interceptors list
             *
             *
             *  \note   Inspired by, but quite different from
             *          @see https://cxf.apache.org/javadoc/latest/org/apache/cxf/phase/PhaseInterceptorChain.html
             *
             * \note A draft of the implementation to clarify
             *      \code
             *          void IntercetorChain::HandleMessage (Message* m)
             *          {
             *              size_t i = 0;
             *              for (; i < fInterceptors_.size (); ++i) {
             *                  try {
             *                      fInterceptors_[i].HandleMessage (m);
             *                  }
             *                  catch (...) {
             *                      exception_ptr e = current_exception ();
             *                      do {
             *                          fInterceptors_[i].HandleFault (m, e);
             *                      } while (i-- != 0);
             *                      Execution::ReThrow ();
             *                  }
             *              }
             *              for (; i > 0; --i) {
             *                  fInterceptors_[i-1].CompleteNormally (m);
             *              }
             *          }
             *      \endcode
             */
            class InterceptorChain {
            protected:
                class _IRep;

            public:
                /**
                 */
                InterceptorChain (const Sequence<Interceptor>& interceptors = {});
                InterceptorChain (const InterceptorChain&) = default;
                InterceptorChain (InterceptorChain&&)      = default;

            protected:
                InterceptorChain (const shared_ptr<_IRep>& rep);

            public:
                nonvirtual InterceptorChain& operator= (const InterceptorChain&) = default;

            public:
                /**
                 */
                nonvirtual Sequence<Interceptor> GetInterceptors () const;

            public:
                /**
                 */
                nonvirtual void SetInterceptors (const Sequence<Interceptor>& interceptors);

            public:
                /**
                 *  The 'before' interceptor must be in the list. The new interceptor is added just before it.
                 */
                nonvirtual void AddBefore (const Interceptor& interceptor2Add, const Interceptor& before);

            public:
                /**
                 *  The 'after' interceptor must be in the list. The new interceptor is added just after it.
                 */
                nonvirtual void AddAfter (const Interceptor& interceptor2Add, const Interceptor& after);

            public:
                /**
                 *  Send the given message to each interceptor in the chain. Each interceptor from start to end of the sequence
                 *  is sent the HandleMessage () call. If any throws an exception, all the HandleMessage () methods so far called
                 *  are called in reverse order.
                 *
                 *  Interceptors must not throw during the HandleFault. InterceptorChain::HandleMessage() simply rethrows the original
                 *  fault (exception) that triggered the unwind of the message.
                 */
                nonvirtual void HandleMessage (Message* m);

            private:
                Execution::RWSynchronized<shared_ptr<_IRep>> fRep_;

            private:
                struct Rep_;
            };

            /**
             */
            class InterceptorChain::_IRep {
            public:
                virtual ~_IRep () = default;

                virtual Sequence<Interceptor> GetInterceptors () const = 0;

                // note - this is const and returns a new _IRep - so that the actual rep can be immutable.
                virtual shared_ptr<_IRep> SetInterceptors (const Sequence<Interceptor>& interceptors) const = 0;

                // Intercepts a message, and handles exception logic - distributing to interceptors already called
                virtual void HandleMessage (Message* m) = 0;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InterceptorChain.inl"

#endif /*_Stroika_Framework_WebServer_Interceptor_h_*/
