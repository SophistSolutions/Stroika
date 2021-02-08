/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "InterceptorChain.h"

using namespace Stroika::Foundation;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

// This class MUST be re-entrant (internally synchronized)- and is STATELESS - so no syncro needed
struct InterceptorChain::Rep_ : InterceptorChain::_IRep {
    Rep_ (const Sequence<Interceptor>& interceptors)
        : fInterceptors_{interceptors}
    {
    }
    virtual Sequence<Interceptor> GetInterceptors () const override
    {
        return fInterceptors_;
    }
    virtual shared_ptr<_IRep> SetInterceptors (const Sequence<Interceptor>& interceptors) const override
    {
        return make_shared<Rep_> (interceptors);
    }
    virtual void HandleMessage (Message* m) const override
    {
        size_t sz = fInterceptors_.size ();
        size_t i  = 0;
        for (; i < sz; ++i) {
            try {
                fInterceptors_[i].HandleMessage (m);
            }
            catch (...) {
                exception_ptr e = current_exception ();
                do {
                    fInterceptors_[i].HandleFault (m, e);
                } while (i-- != 0);
                Execution::ReThrow ();
            }
        }
        for (; i > 0; --i) {
            fInterceptors_[i - 1].CompleteNormally (m);
        }
    }
    const Sequence<Interceptor> fInterceptors_; // no synchro needed because always readonly
};

/*
 ********************************************************************************
 *********************** WebServer::InterceptorChain ****************************
 ********************************************************************************
 */
InterceptorChain::InterceptorChain (const Sequence<Interceptor>& interceptors)
    : InterceptorChain{make_shared<Rep_> (interceptors)}
{
}

InterceptorChain::InterceptorChain (const shared_ptr<_IRep>& rep)
    : interceptors{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Sequence<Interceptor> {
              const InterceptorChain* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &InterceptorChain::interceptors);
              return thisObj->fRep_.cget ().load ()->GetInterceptors ();
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const Sequence<Interceptor>& interceptors) {
              InterceptorChain* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &InterceptorChain::interceptors);
              auto              rwLock  = thisObj->fRep_.rwget ();
              rwLock.store (rwLock->get ()->SetInterceptors (interceptors));
          }}
    , fRep_{rep}
{
}

void InterceptorChain::InterceptorChain::AddBefore (const Interceptor& interceptor2Add, const Interceptor& before)
{
    auto                  rwLock = fRep_.rwget ();
    [[maybe_unused]] bool found{false};
    Sequence<Interceptor> newInterceptors = rwLock->get ()->GetInterceptors ();
    for (size_t i = 0; i < newInterceptors.size (); ++i) {
        if (newInterceptors[i] == before) {
            newInterceptors.Insert (i, interceptor2Add);
            found = true;
            break;
        }
    }
    Require (found);
    rwLock.store (rwLock->get ()->SetInterceptors (newInterceptors));
}

void InterceptorChain::InterceptorChain::AddAfter (const Interceptor& interceptor2Add, const Interceptor& after)
{
    auto rwLock = fRep_.rwget ();

    [[maybe_unused]] bool found{false};
    Sequence<Interceptor> newInterceptors = rwLock->get ()->GetInterceptors ();
    for (size_t i = 0; i < newInterceptors.size (); ++i) {
        if (newInterceptors[i] == after) {
            newInterceptors.Insert (i + 1, interceptor2Add);
            found = true;
            break;
        }
    }
    Require (found);
    rwLock.store (rwLock->get ()->SetInterceptors (newInterceptors));
}
