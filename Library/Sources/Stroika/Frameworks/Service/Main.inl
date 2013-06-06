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
             ********************* Service::Main::IApplicationRep ***************************
             ********************************************************************************
             */
			inline	Main::IServiceIntegrationRep::IServiceIntegrationRep ()
			{
			}
			inline	Main::IServiceIntegrationRep::~IServiceIntegrationRep ()
			{
			}


            /*
             ********************************************************************************
             ************************************ Service::Main *****************************
             ********************************************************************************
             */
			inline	const Main::IServiceIntegrationRep&	Main::GetServiceRep_ () const
			{
				RequireNotNull (fServiceRep_);
                return *fServiceRep_;
			}
			inline	Main::IServiceIntegrationRep&	Main::GetServiceRep_ ()
			{
				RequireNotNull (fServiceRep_);
                return *fServiceRep_;
			}
			inline	const Main::IApplicationRep&		Main::GetAppRep_ () const
			{
				RequireNotNull (GetServiceRep_ ()._GetAttachedAppRep ());
                return *GetServiceRep_ ()._GetAttachedAppRep ();
			}
			inline	Main::IApplicationRep&		Main::GetAppRep_ ()
			{
				RequireNotNull (GetServiceRep_ ()._GetAttachedAppRep ());
                return *GetServiceRep_ ()._GetAttachedAppRep ();
			}
            inline  void    Main::Start (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Start"));
                DbgTrace ("(timeout = %f)", timeout);
                GetServiceRep_ ()._Start (timeout);
            }
            inline  void    Main::Stop (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Stop"));
                DbgTrace ("(timeout = %f)", timeout);
                GetServiceRep_ ()._Stop (timeout);
            }
            inline  void    Main::Restart (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Restart"));
                DbgTrace ("(timeout = %f)", timeout);
                GetServiceRep_ ()._Restart (timeout);
            }
			inline	void    Main::ForcedStop (Time::DurationSecondsType timeout)
			{
				Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::ForcedStop"));
				DbgTrace ("(timeout = %f)", timeout);
				GetServiceRep_ ()._ForcedStop (timeout);
			}
			inline	pid_t   Main::GetServicePID () const
			{
				return GetServiceRep_ ()._GetServicePID ();
			}


        }

    }
}
#endif  /*_Stroika_Frameworks_Service_Main_inl_*/



