/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_InterceptorChain_h_
#define _Stroika_Framework_WebServer_InterceptorChain_h_ 1

#include "../StroikaPreComp.h"

#include "../../Foundation/Common/Property.h"
#include "../../Foundation/Containers/Sequence.h"
#include "../../Foundation/Execution/Synchronized.h"

#include "Interceptor.h"

/*
 */

/*
 *  \version    <a href="Code-Status.md#Alpha-Early">Alpha-Early</a>
 */

namespace Stroika::Frameworks::WebServer {

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
     *  The InterceptorChain is internally synchronized. But - it assumes each Interceptor its given
     *  is itself internally synchronized.
     *
     *  So we can copy an Interceptor chain quickly and cheaply without (external) locks. Updating it
     *  maybe slower, but also can be done freely without locks.
     *
     *  InterceptorChains are 'by value' objects, so updating one in a Connection object - for example - doesn't affect
     *  other connections (update the one in the ConnectionManager for use in future connections).
     *
     *  \note   Use the ConnectionManager to more easily manage the Interceptors list
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
     * 
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized">C++-Standard-Thread-Safety-For-Envelope-Letter-Internally-Synchronized</a>
     *              But note that HandleMessage() is a const method, so it can safely be called from any number of threads
     *              simultaneously.
     */
    class InterceptorChain {
    protected:
        class _IRep;

    public:
        /**
         */
        InterceptorChain (const Sequence<Interceptor>& interceptors = {});
        InterceptorChain (const InterceptorChain& src);
        InterceptorChain (InterceptorChain&& src);

    protected:
        InterceptorChain (const shared_ptr<_IRep>& rep);

    public:
        nonvirtual InterceptorChain& operator= (const InterceptorChain&) = default;

    public:
        Common::Property<Sequence<Interceptor>> interceptors;

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
         *  
         *  HandleMessage() sends a HandleMessage() call to each interceptor in turn.
         *  For each interceptor in the chain, if it succeeds (doesn't throw) - it will THEN get a CompleteNormally () message.
         *  If it faulted (or a fault occurred later in the chain) - then it will INSTEAD get a HandleFault () message.
         *  BUT - no matter what - each intercetor called with HandleMessage () with EITHER:
         *      o   Get its HandleFault() called
         *      o OR get its HandleComplete() called
         *     but not both and not neither.
         *
         *  Send the given message to each interceptor in the chain. Each interceptor from start to end of the sequence
         *  is sent the HandleMessage () call.
         *
         *  Interceptors must not throw during the HandleFault. InterceptorChain::HandleMessage() simply rethrows the original
         *  fault (exception) that triggered the unwind of the message.
         */
        nonvirtual void HandleMessage (Message* m) const;

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
        virtual void HandleMessage (Message* m) const = 0;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "InterceptorChain.inl"

#endif /*_Stroika_Framework_WebServer_Interceptor_h_*/
