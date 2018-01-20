/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_InterceptorChain_inl_
#define _Stroika_Frameworks_WebServer_InterceptorChain_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Foundation/Debug/Assertions.h"

namespace Stroika {
    namespace Frameworks {
        namespace WebServer {

            /*
             ********************************************************************************
             *************************** WebServer::InterceptorChain ************************
             ********************************************************************************
             */
            inline InterceptorChain::InterceptorChain (const shared_ptr<_IRep>& rep)
                : fRep_ (rep)
            {
                RequireNotNull (rep);
            }
            inline Sequence<Interceptor> InterceptorChain::GetInterceptors () const
            {
                return fRep_.load ()->GetInterceptors ();
            }
            inline void InterceptorChain::SetInterceptors (const Sequence<Interceptor>& interceptors)
            {
                auto rwLock = fRep_.rwget ();
                rwLock.store (rwLock->get ()->SetInterceptors (interceptors));
            }
            inline void InterceptorChain::HandleMessage (Message* m)
            {
                fRep_.load ()->HandleMessage (m);
            }
        }
    }
}
#endif /*_Stroika_Frameworks_WebServer_InterceptorChain_inl_*/
