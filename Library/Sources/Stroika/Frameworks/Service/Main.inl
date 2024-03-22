/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Service_Main_inl_
#define _Stroika_Frameworks_Service_Main_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Service {

    /*
     ********************************************************************************
     ********************************* Service::Main ********************************
     ********************************************************************************
     */
    inline const Main::IServiceIntegrationRep& Main::GetServiceRep_ () const
    {
        RequireNotNull (fServiceRep_);
        return *fServiceRep_;
    }
    inline Main::IServiceIntegrationRep& Main::GetServiceRep_ ()
    {
        RequireNotNull (fServiceRep_);
        return *fServiceRep_;
    }
    inline const Main::IApplicationRep& Main::GetAppRep_ () const
    {
        RequireNotNull (GetServiceRep_ ()._GetAttachedAppRep ());
        return *GetServiceRep_ ()._GetAttachedAppRep ();
    }
    inline Main::IApplicationRep& Main::GetAppRep_ ()
    {
        RequireNotNull (GetServiceRep_ ()._GetAttachedAppRep ());
        return *GetServiceRep_ ()._GetAttachedAppRep ();
    }
    inline Containers::Set<Main::ServiceIntegrationFeatures> Main::GetServiceIntegrationFeatures () const
    {
        return GetServiceRep_ ()._GetSupportedFeatures ();
    }
    inline void Main::Install ()
    {
        Debug::TraceContextBumper            ctx{"Stroika::Frameworks::Service::Main::Install"};
        static constexpr Execution::Activity kInstallingService_{"installing service"sv};
        Execution::DeclareActivity           activity{&kInstallingService_};
        GetServiceRep_ ()._Install ();
    }
    inline void Main::UnInstall ()
    {
        Debug::TraceContextBumper            ctx{"Stroika::Frameworks::Service::Main::UnInstall"};
        static constexpr Execution::Activity kUnInstallingService_{"uninstalling service"sv};
        Execution::DeclareActivity           activity{&kUnInstallingService_};
        GetServiceRep_ ()._UnInstall ();
    }
    inline void Main::Start (Time::DurationSeconds timeout)
    {
        using namespace Characters;
        Debug::TraceContextBumper            ctx{"Stroika::Frameworks::Service::Main::Start", "timeout = {}"_f, timeout.count ()};
        static constexpr Execution::Activity kStartingService_{"staring service"sv};
        Execution::DeclareActivity           activity{&kStartingService_};
        GetServiceRep_ ()._Start (timeout);
    }
    inline void Main::Stop (Time::DurationSeconds timeout)
    {
        using namespace Characters;
        Debug::TraceContextBumper            ctx{"Stroika::Frameworks::Service::Main::Stop", "timeout = {}"_f, timeout.count ()};
        static constexpr Execution::Activity kStoppingService_{"stopping service"sv};
        Execution::DeclareActivity           activity{&kStoppingService_};
        GetServiceRep_ ()._Stop (timeout);
    }
    inline void Main::ForcedStop (Time::DurationSeconds timeout)
    {
        using namespace Characters;
        Debug::TraceContextBumper            ctx{"Stroika::Frameworks::Service::Main::ForcedStop", "timeout = {}"_f, timeout.count ()};
        static constexpr Execution::Activity kForcedStoppingService_{"forced stopping service"sv};
        Execution::DeclareActivity           activity{&kForcedStoppingService_};
        GetServiceRep_ ()._ForcedStop (timeout);
    }
    inline pid_t Main::GetServicePID () const
    {
        return GetServiceRep_ ()._GetServicePID ();
    }
    inline Main::State Main::GetState () const
    {
        return GetServiceRep_ ()._GetState ();
    }

}

#endif /*_Stroika_Frameworks_Service_Main_inl_*/
