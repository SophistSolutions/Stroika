/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Service_Main_inl_
#define _Stroika_Frameworks_Service_Main_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Execution/Thread.h"
#include    "../../Foundation/Execution/ThreadAbortException.h"

namespace   Stroika {
    namespace   Frameworks {
        namespace   Service {


            /*
             ********************************************************************************
             ********************* Service::Main::IApplicationRep ***************************
             ********************************************************************************
             */
            inline  void    Main::IApplicationRep::_CheckAndAbortThread () const
            {
                Execution::CheckForThreadAborting ();
                if (fStopping_) {
                    Execution::DoThrow (Execution::ThreadAbortException ());
                }
            }
            inline  bool    Main::IApplicationRep::_CheckShouldReReadConfig () const
            {
                return fMustReReadConfig;
            }
            inline  void    Main::IApplicationRep::_DidReReadConfig ()
            {
                fMustReReadConfig = false;
            }


            /*
             ********************************************************************************
             ************************************ Service::Main *****************************
             ********************************************************************************
             */
            inline  void    Main::Start (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Start"));
                DbgTrace ("(timeout = %f)", timeout);
                _sServiceRep->_Start (timeout);
            }
            inline  void    Main::Stop (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Start"));
                DbgTrace ("(timeout = %f)", timeout);
                _sServiceRep->_Stop (timeout);
            }
            inline  void    Main::Restart (Time::DurationSecondsType timeout)
            {
                _Restart (timeout);
            }


        }

    }
}
#endif  /*_Stroika_Frameworks_Service_Main_inl_*/



