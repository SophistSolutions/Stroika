/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Service_Main_inl_
#define _Stroika_Frameworks_Service_Main_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Foundation/Execution/Thread.h"

namespace   Stroika {
    namespace   Frameworks {
        namespace   Service {


            /*
             ********************************************************************************
             ********************* Service::Main::IApplicationRep ***************************
             ********************************************************************************
             */
            inline  Main::IApplicationRep::IApplicationRep ()
            {
            }
            inline  Main::IApplicationRep::~IApplicationRep ()
            {
            }


            /*
             ********************************************************************************
             **************** Service::Main::IServiceIntegrationRep *************************
             ********************************************************************************
             */
            inline  Main::IServiceIntegrationRep::IServiceIntegrationRep ()
            {
            }
            inline  Main::IServiceIntegrationRep::~IServiceIntegrationRep ()
            {
            }


            /*
             ********************************************************************************
             ********************************* Service::Main ********************************
             ********************************************************************************
             */
            inline  const Main::IServiceIntegrationRep& Main::GetServiceRep_ () const
            {
                RequireNotNull (fServiceRep_);
                return *fServiceRep_;
            }
            inline  Main::IServiceIntegrationRep&   Main::GetServiceRep_ ()
            {
                RequireNotNull (fServiceRep_);
                return *fServiceRep_;
            }
            inline  const Main::IApplicationRep&        Main::GetAppRep_ () const
            {
                RequireNotNull (GetServiceRep_ ()._GetAttachedAppRep ());
                return *GetServiceRep_ ()._GetAttachedAppRep ();
            }
            inline  Main::IApplicationRep&      Main::GetAppRep_ ()
            {
                RequireNotNull (GetServiceRep_ ()._GetAttachedAppRep ());
                return *GetServiceRep_ ()._GetAttachedAppRep ();
            }
            inline  Containers::Set<Main::ServiceIntegrationFeatures>   Main::GetServiceIntegrationFeatures () const
            {
                return GetServiceRep_ ()._GetSupportedFeatures ();
            }
            inline  void    Main::Install ()
            {
                Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::Install"));
                GetServiceRep_ ()._Install ();
            }

            inline  void    Main::UnInstall ()
            {
                Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::UnInstall"));
                GetServiceRep_ ()._UnInstall ();
            }
            inline  void    Main::Start (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::Start"));
                DbgTrace ("(timeout = %f)", timeout);
                GetServiceRep_ ()._Start (timeout);
            }
            inline  void    Main::Stop (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::Stop"));
                DbgTrace ("(timeout = %f)", timeout);
                GetServiceRep_ ()._Stop (timeout);
            }
            inline  void    Main::ForcedStop (Time::DurationSecondsType timeout)
            {
                Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::ForcedStop"));
                DbgTrace ("(timeout = %f)", timeout);
                GetServiceRep_ ()._ForcedStop (timeout);
            }
            inline  pid_t   Main::GetServicePID () const
            {
                return GetServiceRep_ ()._GetServicePID ();
            }
            inline  Main::State    Main::GetState () const
            {
                return GetServiceRep_ ()._GetState ();
            }


        }

    }
}
#endif  /*_Stroika_Frameworks_Service_Main_inl_*/
