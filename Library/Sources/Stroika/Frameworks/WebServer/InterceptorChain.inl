/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_InterceptorChain_inl_
#define _Stroika_Frameworks_WebServer_InterceptorChain_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Debug/Assertions.h"



namespace   Stroika {
    namespace   Frameworks  {
        namespace   WebServer {


            /*
             ********************************************************************************
             ***************************** WebServer::InterceptorChain **********************
             ********************************************************************************
             */
            inline  InterceptorChain::InterceptorChain (const shared_ptr<_IRep>& rep)
                : fRep_ (rep)
            {
            }
            inline  Sequence<Interceptor>   InterceptorChain::GetInterceptors () const
            {
                return fRep_->GetInterceptors ();
            }
            inline  void                    InterceptorChain::SetInterceptors (const Sequence<Interceptor>& interceptors)
            {
                fRep_->SetInterceptors (interceptors);
            }
            inline  void    InterceptorChain::HandleFault (Message* m, const exception_ptr& e)
            {
                fRep_->HandleFault (m, e);
            }
            inline  void    InterceptorChain::HandleMessage (Message* m)
            {
                fRep_->HandleMessage (m);
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_InterceptorChain_inl_*/
