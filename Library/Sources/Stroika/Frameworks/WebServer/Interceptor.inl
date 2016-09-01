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
             ***************************** WebServer::Interceptor ************************
             ********************************************************************************
             */
            inline  Interceptor::Interceptor (const shared_ptr<_IRep>& rep)
                : fRep_ (rep)
            {
            }
            inline  void    Interceptor::HandleFault (Message* m, const exception_ptr& e)
            {
                fRep_->HandleFault (m, e);
            }
            inline  void    Interceptor::HandleMessage (Message* m)
            {
                fRep_->HandleMessage (m);
            }


        }
    }
}
#endif  /*_Stroika_Frameworks_WebServer_Interceptor_inl_*/
