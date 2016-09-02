/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Interceptor_inl_
#define _Stroika_Frameworks_WebServer_Interceptor_inl_   1


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
             ***************************** WebServer::Interceptor ***************************
             ********************************************************************************
             */
            inline  Interceptor::Interceptor (const shared_ptr<_IRep>& rep)
                : fRep_ (rep)
            {
                RequireNotNull (rep);
            }
            inline  void    Interceptor::HandleFault (Message* m, const exception_ptr& e) noexcept
            {
                fRep_->HandleFault (m, e);
            }
            inline  void    Interceptor::HandleMessage (Message* m)
            {
                fRep_->HandleMessage (m);
            }
            template    <typename T>
            inline  auto    Interceptor::_GetRep () const -> const T&
            {
                EnsureMember (fRep_.get (), T);
                return *dynamic_cast<const T*> (fRep_.get ());
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_Interceptor_inl_*/
