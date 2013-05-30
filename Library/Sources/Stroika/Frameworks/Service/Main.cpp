/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    <algorithm>
#include    <cstdlib>
#include    <fstream>
#include    <sstream>

#if     qPlatform_POSIX
#include    <sys/types.h>
#include    <unistd.h>
#include    <sys/stat.h>
#include    <fcntl.h>
#endif

#include    "../../Foundation/Characters/Format.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Debug/Trace.h"
#include    "../../Foundation/Execution/CommandLine.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Execution/ErrNoException.h"
#include    "../../Foundation/Execution/Module.h"
#include    "../../Foundation/Execution/ThreadAbortException.h"
#include    "../../Foundation/Execution/Signals.h"
#include    "../../Foundation/Execution/Sleep.h"
#include    "../../Foundation/Execution/WaitTimedOutException.h"
#include    "../../Foundation/IO/FileSystem/FileUtils.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "Main.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::Service;





#if 0
// CODE BASE TO LIFT FROM FOR WINDOWS API - SO BY COMMAND LINE (OR SOME SUCH) WE START AS SERVICE OR RUNNING INDEPENDENTLY

namespace   {
    class   MyAppBase {
    public:
        MyAppBase (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
        }

    protected:
        void    StartRefContentMgr () {
            CompiledIntegratedContent::ReferenceContentManager::Get ().Open (Execution::GetEXEDir () + _T ("Reference Content\\"));
        }
        void    StopRefContentMgr () {
            CompiledIntegratedContent::ReferenceContentManager::Get ().Close ();
        }
        void    InitializeAppDataRepositoryDirectories () {
            const TCHAR kDefaultHealthFrameRelPath[]    =       _T("Records For Living, Inc\\HealthFrameWorks Server v1\\");
            OpenHealthServiceDefinitionRepository::InitializeRepositoryDir (IO::FileSystem::WellKnownLocations::GetApplicationData () + kDefaultHealthFrameRelPath);
        }
        nonvirtual  void    InitMoreModules () {
            ImportExport::CCR::SetInfoSystem2ReportOnExport (
                ImportExport::CCR::InfoSystem2ReportOnExport (L"HealthFrameWorks Server", L"Medical Record Analytics and Storage Server", TString2Wide (HealthFrameWorksServer::kVersion.AsHFPrettyVersionString ()))
            );
        }

    protected:
        vector<HFWServerBase::ServiceConfiguration> fEffectiveServiceConfigurations;
        vector<shared_ptr<HFWServerBase::Base> >    fServiceObjs;

    protected:
        void    StartServiceThreads () {
            {
                vector<Options::Service>    services    =   Options::Mgr::Get ().GetServices ();
                for (vector<Options::Service>::const_iterator si = services.begin (); si != services.end (); ++si) {
                    if (si->fEnabled) {
                        switch (si->fServiceType) {
                            case    Options::eServerConfigurationWebUI_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW MyConfigWebUI_Server::ServiceObj (*si, &fServiceObjs)));
                                }
                                break;
                            case    Options::eHSTP_V1_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW MyHSTP_V1_Server::ServiceObj (*si)));
                                }
                                break;
                            case    Options::eHTTPGETRecord_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW MyHTTPGET_Server::ServiceObj (*si)));
                                }
                                break;
                            case    Options::eProvisioning_V1_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW MyRepositoryProvisioning_V1_Server::ServiceObj (*si)));
                                }
                                break;
                            case    Options::eRepository_V1_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW MyRepository_V1_Server::ServiceObj (*si, services)));
                                }
                                break;
                            case    Options::ePHRDB_V1_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW MyPHRDB_V1_Server::ServiceObj (*si, services)));
                                }
                                break;
                            case    Options::eReferenceContent_V1_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW MyReferenceContent_V1_Server::ServiceObj (*si)));
                                }
                                break;
                            case    Options::eDirectoryService_ST: {
                                    fServiceObjs.push_back (shared_ptr<HFWServerBase::Base> (DEBUG_NEW DirectoryOfServicesPage::ServiceObj (*si, &fEffectiveServiceConfigurations)));
                                }
                                break;
                        }
                    }
                }
            }

            fEffectiveServiceConfigurations.clear ();
            for (vector<shared_ptr<HFWServerBase::Base> >::const_iterator i = fServiceObjs.begin (); i != fServiceObjs.end (); ++i) {
                fEffectiveServiceConfigurations.push_back ((*i)->GetServiceConfiguration ());
            }
            {
                wstring msgBuf;
                for (vector<shared_ptr<HFWServerBase::Base> >::const_iterator i = fServiceObjs.begin (); i != fServiceObjs.end (); ++i) {
                    HFWServerBase::ServiceConfiguration sc  =   (*i)->GetServiceConfiguration ();
                    wstring svcMsg;
                    switch (sc.fServiceType) {
                        case    Options::eServerConfigurationWebUI_ST: {
                                svcMsg = L"\tHealthFrameWorks Configuraiton Interface:\r\n";
                            }
                            break;
                        case    Options::eProvisioning_V1_ST: {
                                svcMsg = L"\tProvisioning (V1): " + sc.fDescription + L"\r\n";
                            }
                            break;
                        case    Options::eHSTP_V1_ST: {
                                svcMsg = L"\tHSTP (V1): " + sc.fDescription + L"\r\n";
                            }
                            break;
                        case    Options::eHTTPGETRecord_ST: {
                                svcMsg = L"\tHTTPGET Service: " + sc.fDescription + L"\r\n";
                            }
                            break;
                        case    Options::eRepository_V1_ST: {
                                svcMsg = L"\tRepository_V1: " + sc.fDescription + L"\r\n";
                            }
                            break;
                        case    Options::ePHRDB_V1_ST: {
                                svcMsg = L"\tPHRDB_V1: " + sc.fDescription + L"\r\n";
                            }
                            break;
                        case    Options::eReferenceContent_V1_ST: {
                                svcMsg = L"\tReferenceContent_V1: " + sc.fDescription + L"\r\n";
                            }
                            break;
                        case    Options::eDirectoryService_ST: {
                                svcMsg = L"\tDirectory Of Services: " + sc.fDescription + L"\r\n";
                            }
                            break;
                        default: {
                                svcMsg = Format (L"\tUNKNOWN (%d): %s\r\n", sc.fServiceType, sc.fDescription.c_str ());
                            }
                            break;
                    }
                    typedef Options::Service::NetworkBinding    NetworkBinding;
                    for (vector<NetworkBinding>::const_iterator nbi = sc.fNetworkBindings.begin (); nbi != sc.fNetworkBindings.end (); ++nbi) {
                        wstring hostName    =   nbi->fHostName;
                        if (hostName.empty ()) {
                            hostName = L"localhost";
                        }
                        svcMsg += Format (L"\t\t%s:%d\r\n", hostName.c_str (), nbi->fPort);
                    }
                    msgBuf += svcMsg;
                }
                Logger::EmitMessage (Logger::eInformation_MT, L"HealthFrameWorks Server Started Services:\r\n%s", msgBuf.c_str ());
            }
        }
        void    StopServices () {
            TraceContextBumper ctx (_T ("StopServices ()"));
            const float kMaxShutDownTimeWaitTime    =   15.0f;
            for (vector<shared_ptr<HFWServerBase::Base> >::const_iterator i = fServiceObjs.begin (); i != fServiceObjs.end (); ++i) {
                (*i)->Stop ();
            }
            DbgTrace (_T ("Finished sending STOP messages, and now doing WAITs"));
            Time::DurationSecondsType   timeDone    = Time::GetTickCount () + kMaxShutDownTimeWaitTime;
            bool    timedOut    =   false;
            for (vector<shared_ptr<HFWServerBase::Base> >::const_iterator i = fServiceObjs.begin (); i != fServiceObjs.end (); ++i) {
                Time::DurationSecondsType   timeLeft    =   timeDone - Time::GetTickCount ();
                if (timeLeft > 0.0) {
                    try {
                        (*i)->WaitForDone (timeLeft);
                    }
                    catch (const Execution::WaitTimedOutException&) {
                        timedOut = true;
                    }
                    catch (...) {
                        Logger::EmitMessage (Logger::eError_MT, L"Exception stopping services");
                    }
                }
                else {
                    timedOut = true;
                }
            }
            DbgTrace (_T ("Finished waiting on services threads"));
            if (timedOut) {
                Logger::EmitMessage (Logger::eError_MT, L"Timeout stopping services");
            }
        }

    public:
        virtual void    Run () = 0;
    };


    class   MyApp_Standalone : public MyAppBase {
    private:
        typedef MyAppBase   inherited;
    public:
        MyApp_Standalone (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow, unsigned int time2RunBeforeTimeout):
            inherited (hInstance, hPrevInstance, lpCmdLine, nCmdShow),
            fTime2RunBeforeTimeout (time2RunBeforeTimeout) {
        }

    private:
        unsigned int fTime2RunBeforeTimeout;

    public:
        virtual     void    Run () override {
            try {
                InitializeAppDataRepositoryDirectories ();
                InitMoreModules ();

                StartRefContentMgr ();

                StartServiceThreads ();
            }
            HealthFrameWorks_LogMessageHelper (_T ("While starting up HealthFrameWorks Server"));

            // ask each service thread amount  of time since last activity.
            // then wait maxwait-that #, and try again
            while (true) {
                Time::DurationSecondsType   timeSinceLastActivity   =   -1.0;
                for (vector<shared_ptr<HFWServerBase::Base> >::const_iterator i = fServiceObjs.begin (); i != fServiceObjs.end (); ++i) {
                    if (timeSinceLastActivity == -1.0) {
                        timeSinceLastActivity = (*i)->GetTimeSinceLastActivity ();
                    }
                    else {
                        timeSinceLastActivity = min (timeSinceLastActivity, (*i)->GetTimeSinceLastActivity ());
                    }
                }

                if (timeSinceLastActivity < fTime2RunBeforeTimeout) {
                    Time::DurationSecondsType   remaining = fTime2RunBeforeTimeout - timeSinceLastActivity;
                    Execution::Sleep (remaining);
                }
                else {
                    break;  // been inactive long enough - bye bye...
                }
            }

            StopServices ();
        }
    };



    class   MyApp_Service : public MyAppBase {
    private:
        typedef MyAppBase   inherited;
    public:
        static  const   TCHAR kServiceName[];

    public:
        MyApp_Service (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow):
            inherited (hInstance, hPrevInstance, lpCmdLine, nCmdShow),
            fStopServiceEvent (),
            fServiceStatusHandle (NULL)
            //fServiceStatus ()
        {
            memset (&fServiceStatus, 0, sizeof (fServiceStatus));
            fServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
            fServiceStatus.dwCurrentState = SERVICE_STOPPED;
            fServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
            Assert (sTHIS == NULL);
            sTHIS = this;
        }
        ~MyApp_Service () {
            Assert (sTHIS != NULL);
            sTHIS = NULL;
        }

    private:
        static  TString GetServerRunningFilePath_ () {
            return IO::FileSystem::WellKnownLocations::GetApplicationData () + Options::kServiceConfigAppRelPathDir +  _T ("HFWServer.running");
        }

    public:
        virtual     void    Run () override {
            SERVICE_TABLE_ENTRY st[] = {
                { const_cast<TCHAR*> (kServiceName), _ServiceMain },
                { NULL, NULL }
            };
            if (::StartServiceCtrlDispatcher (st) == 0) {
                fServiceStatus.dwWin32ExitCode = GetLastError ();
            }
        }

    private:
        void    ServiceMain (DWORD dwArgc, LPTSTR* lpszArgv) throw() {
            // Register the control request handler
            fServiceStatus.dwCurrentState = SERVICE_START_PENDING;
            fServiceStatusHandle = ::RegisterServiceCtrlHandler (kServiceName, _Handler);
            if (fServiceStatusHandle == NULL) {
                Logger::EmitMessage (Logger::eError_MT, "Handler not installed");
                return;
            }
            SetServiceStatus (SERVICE_START_PENDING);

            fServiceStatus.dwWin32ExitCode = S_OK;
            fServiceStatus.dwCheckPoint = 0;
            fServiceStatus.dwWaitHint = 0;

            // When the Run function returns, the service has stopped.
            fServiceStatus.dwWin32ExitCode = ServiceRun_ ();

            Logger::EmitMessage (Logger::eInformation_MT, "Service stopped");
            SetServiceStatus (SERVICE_STOPPED);
        }
        static void WINAPI _ServiceMain (DWORD dwArgc, LPTSTR* lpszArgv) throw () {
            sTHIS->ServiceMain (dwArgc, lpszArgv);
        }

        DWORD   ServiceRun_ () throw () {
            try {
                InitializeAppDataRepositoryDirectories ();
                InitMoreModules ();
                InitializeSecurity_ ();

                StartRefContentMgr ();
                StartServiceThreads ();

                {
                    TString runningFile =   GetServerRunningFilePath_ ();
                    if (IO::FileSystem::FileExists (runningFile)) {
                        Logger::EmitMessage (Logger::eWarning_MT, "HealthFrameWorks Server appears to have not shutdown cleanly last time it was run");
                    }
                    else {
                        IO::FileSystem::CreateDirectoryForFile (runningFile);
                        IO::FileSystem::FileWriter writer (runningFile.c_str ());
                    }
                }

                SetServiceStatus (SERVICE_RUNNING);

                //wait on semaphore set by the STOP call
                DbgTrace ("Waiting for stop-service event");
                fStopServiceEvent.Wait ();
                DbgTrace ("Wait for stop-service event complete, so exiting from ServiceRun_");
            }
            HealthFrameWorks_LogMessageHelper (_T ("While starting up HealthFrameWorks Server"));
            return 0;
        }

        void    InitializeSecurity_ () {
            // For now - nothing todo (see atl version of this call to see if there is any point)
        }

        void    OnStop () throw() {
            SetServiceStatus (SERVICE_STOP_PENDING);
            StopServices ();
            StopRefContentMgr ();
            ::DeleteFile (GetServerRunningFilePath_ ().c_str ());
            fStopServiceEvent.Set ();
        }

        void    OnPause () throw() {
            // for now - ignore - don't support
        }

        void    OnContinue () throw() {
            // for now - ignore - don't support
        }

        void    OnInterrogate () throw() {
            // Don't even know what this is supposed to do?
        }

        void    OnShutdown() throw() {
        }

        void    OnUnknownRequest (DWORD /*dwOpcode*/) throw() {
            Logger::EmitMessage (Logger::eError_MT, "Bad service request");
        }

        void    Handler (DWORD dwOpcode) throw() {
            switch (dwOpcode) {
                case SERVICE_CONTROL_STOP:
                    OnStop ();
                    break;
                case SERVICE_CONTROL_PAUSE:
                    OnPause ();
                    break;
                case SERVICE_CONTROL_CONTINUE:
                    OnContinue ();
                    break;
                case SERVICE_CONTROL_INTERROGATE:
                    OnInterrogate ();
                    break;
                case SERVICE_CONTROL_SHUTDOWN:
                    OnShutdown ();
                    break;
                default:
                    OnUnknownRequest (dwOpcode);
                    break;
            }
        }
        static void WINAPI _Handler (DWORD dwOpcode) throw () {
            sTHIS->Handler (dwOpcode);
        }

    public:
        static  bool    IsInstalled () throw() {
            bool    result  =   false;
            SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (hSCM != NULL) {
                SC_HANDLE   hService = ::OpenService (hSCM, kServiceName, SERVICE_QUERY_CONFIG);
                if (hService != NULL) {
                    result = true;
                    ::CloseServiceHandle (hService);
                }
                ::CloseServiceHandle (hSCM);
            }
            return result;
        }
    public:
        static  bool    Install () throw () {
            if (IsInstalled ()) {
                Logger::EmitMessage (Logger::eInformation_MT, "Service was already installed");
                return true;
            }

            TString commandLine2ExecForSvc = _T ("\"") + Execution::GetEXEPath () + _T ("\"") + _T (" /RunAsService");
            SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (hSCM == NULL) {
                Logger::EmitMessage (Logger::eError_MT, "Could not open Service Manager");
                return false;
            }

            SC_HANDLE hService = ::CreateService (
                                     hSCM, kServiceName, kServiceName,
                                     SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                                     SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
                                     commandLine2ExecForSvc.c_str (), NULL, NULL, _T("RPCSS\0"), NULL, NULL
                                 );
            if (hService == NULL) {
                ::CloseServiceHandle (hSCM);
                Logger::EmitMessage (Logger::eError_MT, "Could not create service");
                return false;
            }
            ::CloseServiceHandle (hService);
            ::CloseServiceHandle (hSCM);
            Logger::EmitMessage (Logger::eInformation_MT, "Successfully registered service");
            return true;
        }

        static  bool    Uninstall () throw () {
            if (not IsInstalled ()) {
                Logger::EmitMessage (Logger::eInformation_MT, "Service was already uninstalled");
                return true;
            }

            SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
            if (hSCM == NULL) {
                Logger::EmitMessage (Logger::eError_MT, "Could not open Service Manager");
                return false;
            }
            SC_HANDLE hService = ::OpenService (hSCM, kServiceName, SERVICE_STOP | DELETE);
            if (hService == NULL) {
                ::CloseServiceHandle (hSCM);
                Logger::EmitMessage (Logger::eError_MT, "Could not open service");
                return false;
            }
            SERVICE_STATUS status;
            BOOL bRet = ::ControlService (hService, SERVICE_CONTROL_STOP, &status);
            if (!bRet) {
                DWORD dwError = GetLastError ();
                if (!((dwError == ERROR_SERVICE_NOT_ACTIVE) or (dwError == ERROR_SERVICE_CANNOT_ACCEPT_CTRL and status.dwCurrentState == SERVICE_STOP_PENDING))) {
                    Logger::EmitMessage (Logger::eError_MT, "Could not stop service");
                }
            }

            BOOL bDelete = ::DeleteService (hService);
            ::CloseServiceHandle (hService);
            ::CloseServiceHandle (hSCM);
            if (bDelete) {
                Logger::EmitMessage (Logger::eInformation_MT, "Successfully uninstalled service");
                return true;
            }
            else {
                Logger::EmitMessage (Logger::eError_MT, "Could not delete service");
                return false;
            }
        }
    protected:
        void    SetServiceStatus (DWORD dwState) throw () {
            DbgTrace ("SetServiceStatus (%d)", dwState);
            fServiceStatus.dwCurrentState = dwState;
            ::SetServiceStatus (fServiceStatusHandle, &fServiceStatus);
        }

    private:
        static  MyApp_Service*  sTHIS;
        Execution::Event        fStopServiceEvent;
        SERVICE_STATUS_HANDLE   fServiceStatusHandle;
        SERVICE_STATUS          fServiceStatus;
    };
    const   TCHAR MyApp_Service::kServiceName[] =   _T ("HealthFrame Works Server");
    MyApp_Service*  MyApp_Service::sTHIS    =   NULL;
}
int APIENTRY    _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    DbgTrace (L"Entering _tWinMain (version='%s', APP_BUILD_DATE='%s')...",
              TString2Wide (HealthFrameWorksServer::kVersion.AsHFPrettyVersionString ()).c_str (),
              NarrowSDKStringToWide (__DATE__).c_str ()
             );

#if     qTraceToFile && qDefaultTracingOn
    DbgTrace (_T ("\tLogfile: %s"), Stroika::Foundation::Debug::Emitter::Get ().GetTraceFileName ().c_str ());
#endif

#if     qUseVisualStudioCLeakDetector
    _CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_EVERY_1024_DF | _CrtSetDbgFlag (_CRTDBG_REPORT_FLAG));
#endif

#if     qTraceToFile && qDefaultTracingOn
    DumpSysInfo2Trace ();
#endif

    Debug::RegisterDefaultFatalErrorHandlers (_FatalErorrHandler_);

    Execution::Platform::Windows::RegisterDefaultHandler_invalid_parameter ();
    Execution::Platform::Windows::StructuredException::RegisterHandler ();      // map win32 structured exceptions to our own C++ class (mainly for better
    // error reporting - otherwise largely irrelevant (see Dick Davis bug report 2006-12-31)

    LoadImageResources_ (hInstance);
    LoadXSDResources_ (hInstance);

    ParsedCmdLine   cmdLine =   ParseCommandLine (lpCmdLine);

#if     qTraceToFile && qDefaultTracingOn
    Logger::EmitMessage (Logger::eInformation_MT, _T ("Trace LogFile = '%s'."), Stroika::Foundation::Debug::Emitter::Get ().GetTraceFileName ().c_str ());
#endif

    if (cmdLine.fRunStyle == eInstallService) {
        MyApp_Service::Install ();
    }
    else if (cmdLine.fRunStyle == eUnInstallService) {
        MyApp_Service::Uninstall ();
    }
    else {
        if (cmdLine.fConfigFileName.empty () and cmdLine.fRunStyle == eRunAsService) {
            cmdLine.fConfigFileName = GetServiceConfigFilePath_ ();
        }

        if (cmdLine.fConfigFileName.empty () or not IO::FileSystem::FileExists (cmdLine.fConfigFileName)) {
            Logger::EmitMessage (Logger::eWarning_MT, _T ("Couldn't find configuration file ('%s')- so using default configuration."), cmdLine.fConfigFileName.c_str ());
            try {
                Options::Mgr::Get ().OpenDefault ();
            }
            catch (...) {
                Logger::EmitMessage (Logger::eError_MT, _T ("Failed to load DefaultConfigurationFile.xml - aborting..."));
                return -1;
            }
        }
        else {
            try {
                Options::Mgr::Get ().Open (cmdLine.fConfigFileName);
            }
            catch (...) {
                Logger::EmitMessage (Logger::eError_MT, _T ("Failed to open configuration file ('%s')- so using default configuration."), cmdLine.fConfigFileName.c_str ());
                try {
                    Options::Mgr::Get ().OpenDefault ();
                }
                catch (...) {
                    Logger::EmitMessage (Logger::eError_MT, _T ("Failed to load DefaultConfigurationFile.xml - aborting..."));
                    return -1;
                }
            }
        }

        {
            shared_ptr<MyAppBase>   app;
            switch (cmdLine.fRunStyle) {
                case    eRunAsService:
                    app = shared_ptr<MyApp_Service> (DEBUG_NEW MyApp_Service (hInstance, hPrevInstance, lpCmdLine, nCmdShow));
                    break;
                case    eRunDirectly:
                    app = shared_ptr<MyApp_Standalone> (DEBUG_NEW MyApp_Standalone (hInstance, hPrevInstance, lpCmdLine, nCmdShow, cmdLine.fRunDirectlySecondCount));
                    break;
            }
            app->Run ();
        }
    }

#if     qTraceToFile && qDefaultTracingOn
    ::ShellExecute (::GetDesktopWindow (), _T("open"), Stroika::Foundation::Debug::Emitter::Get ().GetTraceFileName ().c_str (), NULL, NULL, SW_SHOWNORMAL);
#endif

    return 0;
}

#endif






/*
 ********************************************************************************
 ****************************** Service::Main::IRep *****************************
 ********************************************************************************
 */
Main::IRep::IRep ()
    : fStopping_ (false)
    , fMustReReadConfig (false)
{
}

Main::IRep::~IRep ()
{
}

void    Main::IRep::OnStartRequest ()
{
    // This procedure ends when the entire service process ends...
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::IRep::OnStartRequest"));
    MainLoop ();
}

void    Main::IRep::OnStopRequest ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::IRep::OnStopRequest"));
    // default to using thread stuff to send us a signal to abort...
    fStopping_ = true;
}

void    Main::IRep::OnReReadConfigurationRequest ()
{
    fMustReReadConfig = true;
}

String  Main::IRep::GetServiceStatusMessage () const
{
    return String ();
}

#if     qPlatform_POSIX
String  Main::IRep::GetPIDFileName () const
{
    return L"/tmp/" + GetServiceDescription ().fName + L".pid";
}
#endif

#if     qPlatform_POSIX
void    Main::IRep::SignalHandler (int signum)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::IRep::SignalHandler"));
    DbgTrace (L"(signal = %s)", Execution::SignalToName (signum).c_str ());
    // VERY PRIMITIVE IMPL FOR NOW -- LGP 2011-09-24
    switch (signum) {
        case    SIGTERM:
            DbgTrace ("setting fStopping_ to true");
            fStopping_ = true;
            break;
#if     qCompilerAndStdLib_Supports_constexpr
        case    kSIG_ReReadConfiguration:
#else
        case    SIGHUP:
#endif
            OnReReadConfigurationRequest ();
            break;
    }
}
#endif








/*
 ********************************************************************************
 ************************************ Service::Main *****************************
 ********************************************************************************
 */

const   wchar_t Service::Main::CommandNames::kRunAsService[]        =   L"Run-As-Service";
const   wchar_t Service::Main::CommandNames::kStart[]               =   L"Start";
const   wchar_t Service::Main::CommandNames::kStop[]                =   L"Stop";
const   wchar_t Service::Main::CommandNames::kKill[]                =   L"Kill";
const   wchar_t Service::Main::CommandNames::kRestart[]             =   L"Restart";
const   wchar_t Service::Main::CommandNames::kReloadConfiguration[] =   L"Reload-Configuration";
const   wchar_t Service::Main::CommandNames::kPause[]               =   L"Pause";
const   wchar_t Service::Main::CommandNames::kContinue[]            =   L"Continue";

shared_ptr<Main::IRep>   Main::_sRep;

Main::Main (shared_ptr<IRep> rep)
{
    Require  (_sRep.get () == nullptr);     // singleton
    _sRep = rep;
#if     qPlatform_POSIX
    SetupSignalHanlders_ ();
#endif
}

#if     qPlatform_POSIX
void    Main::SetupSignalHanlders_ ()
{
    Execution::SignalHandlerRegistry::Get ().AddSignalHandler (SIGTERM, SignalHandler);
    Execution::SignalHandlerRegistry::Get ().AddSignalHandler (kSIG_ReReadConfiguration, SignalHandler);
}
#endif

Main::State Main::GetState () const
{
#if     qPlatform_POSIX
    if (GetServicePID () != 0) {
        return State::eRunning;
    }
#endif
    return State::eStopped;    // otherwise (esp on other platforms where not implemented) must  be stopped
}

#if     qPlatform_POSIX
pid_t   Main::GetServicePID () const
{
    ifstream    in (_sRep->GetPIDFileName ().AsTString ().c_str ());
    if (in) {
        pid_t   n = 0;
        in >> n;
        return n;
    }
    return 0;
}
#endif

String      Main::GetServiceStatusMessage () const
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::GetServiceStatusMessage"));
    const   wchar_t kTAB[]  =   L"    ";    // use spaces instead of tab so formatting independent of tabstop settings
    ServiceDescription  svd =   GetServiceDescription ();
    wstringstream   tmp;
    tmp << L"Service '" << svd.fName.As<wstring> () << "'" << endl;
    switch (this->GetState ()) {
        case    State::eStopped:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "STOPPED" << endl;
            break;
        case    State::eRunning:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "Running" << endl;
#if     qPlatform_POSIX
            tmp << kTAB << L"PID:    " << kTAB << kTAB << kTAB << kTAB << GetServicePID () << endl;
#endif
            break;
        case    State::ePaused:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "PAUSED" << endl;
#if     qPlatform_POSIX
            tmp << kTAB << L"PID:    " << kTAB << kTAB << kTAB << kTAB << GetServicePID () << endl;
#endif
            break;
        default:
            AssertNotReached ();
    }
    tmp << _sRep->GetServiceStatusMessage ().As<wstring> ();
    DbgTrace (L"returning status: (%s)", tmp.str ().c_str ());
    return tmp.str ();
}

void    Main::RunAsService ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::RunAsService"));
    // VERY PRIMITIVE IMPL - WE NEED LOCKING on tmpfile stuff - add good tempfile supprot to fileuitls or use existing...

    try {
#if     qPlatform_POSIX
        ofstream    out (_sRep->GetPIDFileName ().AsTString ().c_str ());
        out << getpid () << endl;
#endif
        _sRep->OnStartRequest ();
    }
    catch (const Execution::ThreadAbortException& /*threadAbort*/) {
#if     qPlatform_POSIX
        unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
        // ignore this - just means service ended normally
    }
    catch (...) {
        DbgTrace (TSTR ("Unexpected exception ended running service"));
#if     qPlatform_POSIX
        unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
        throw;
    }
}

void    Main::_Start (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Start"));
    DbgTrace ("(timeout = %f)", timeout);

    Time::DurationSecondsType timeoutAt =   Time::GetTickCount () + timeout;

    // Check not already runnig, (someday) and then for and exec the

    if (_IsServiceFailed ()) {
        _CleanupDeadService ();
    }

#if     qPlatform_POSIX
    // REALLY should use GETSTATE - and return state based on if PID file exsits...
    if (GetServicePID ()  != 0) {
        Execution::DoThrow (Execution::StringException (L"Cannot Start service because its already running"));
    }
#endif

    Characters::TString thisEXEPath =   Execution::GetEXEPath ();
#if     qPlatform_POSIX
    pid_t   pid =   fork ();
    Execution::ThrowErrNoIfNegative (pid);
    if (pid == 0) {
        /*
         * Very primitive code to detatch the console. No error checking cuz frankly we dont care.
         *
         * Possibly should close more descriptors?
         */
        for (int i = 0; i < 3; ++i) {
            ::close (i);
        }
        int id = open ("/dev/null", O_RDWR);
        dup2 (id, 0);
        dup2 (id, 1);
        dup2 (id, 2);

        int r   =   execl (thisEXEPath.c_str (), thisEXEPath.c_str (), (String (L"--") + String (CommandNames::kRunAsService)).AsTString ().c_str (), nullptr);
        exit (-1);
    }
    else {
        // parent - in this case - no reason to wait - our work is done... Future versions might wait to
        // see if the 'pidfile' got created....
        //      --LGP 2011-09-23
    }
#endif

    while (not _IsServiceActuallyRunning ()) {
        Execution::Sleep (0.5);
        if (Time::GetTickCount () > timeoutAt) {
            Execution::DoThrow (Execution::WaitTimedOutException ());
        }
    }
}

void    Main::_Stop (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Stop"));
    DbgTrace ("(timeout = %f)", timeout);

    Time::DurationSecondsType timeoutAt =   Time::GetTickCount () + timeout;
    // Send signal to server to stop
#if     qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (kill (GetServicePID (), SIGTERM));
#endif
    while (_IsServiceActuallyRunning ()) {
        Execution::Sleep (0.5);
        if (Time::GetTickCount () > timeoutAt) {
            Execution::DoThrow (Execution::WaitTimedOutException ());
        }
    }
}

void    Main::Kill ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Kill"));
    Stop ();
    // Send signal to server to stop
#if     qPlatform_POSIX
    Execution::ThrowErrNoIfNegative (kill (GetServicePID (), SIGKILL));
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
}

void    Main::_Restart (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Restart"));
    DbgTrace ("(timeout = %f)", timeout);

    Time::DurationSecondsType endAt =   Time::GetTickCount () + timeout;
    IgnoreExceptionsForCall (Stop (timeout));
#if     qPlatform_POSIX
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
    Start (endAt - Time::GetTickCount ());
}

bool    Main::_IsServiceFailed ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::_IsServiceFailed"));
#if     qPlatform_POSIX
    pid_t   servicePID  =   GetServicePID ();
    if (servicePID > 0) {
        return not _IsServiceActuallyRunning ();
    }
#endif
    return false;
}

void    Main::_CleanupDeadService ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::_CleanupDeadService"));
#if     qPlatform_POSIX
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
}

bool    Main::_IsServiceActuallyRunning ()
{
#if     qPlatform_POSIX
    pid_t   servicePID  =   GetServicePID ();
    if (servicePID > 0) {
        int result  =   ::kill (servicePID, 0);
        return result == 0;
    }
#endif
    return false;
}

void    Main::ReReadConfiguration ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::ReReadConfiguration"));
    // SEND APPROPRIATE SIGNAL
#if     qPlatform_POSIX
    pid_t   pid =   GetServicePID ();
    Assert (pid != 0);  // maybe throw if non-zero???
    Execution::ThrowErrNoIfNegative (kill (GetServicePID (), kSIG_ReReadConfiguration));
#endif
}

void    Main::Pause ()
{
    AssertNotImplemented ();
}

void    Main::Continue ()
{
    AssertNotImplemented ();
}

Main::ServiceDescription    Main::GetServiceDescription () const
{
    return _sRep->GetServiceDescription ();
}

#if     qPlatform_POSIX
void    Main::SignalHandler (int signum)
{
    _sRep->SignalHandler (signum);
}
#endif

bool    Main::_HandleStandardCommandLineArgument (const String& arg)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::_HandleStandardCommandLineArgument"));
    if (Execution::MatchesCommandLineArgument (arg, CommandNames::kRunAsService)) {
        RunAsService ();
        return true;
    }
    else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kStart)) {
        Start ();
        return true;
    }
    else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kStop)) {
        Stop ();
        return true;
    }
    else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kKill)) {
        Kill ();
        return true;
    }
    else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kRestart)) {
        Restart ();
        return true;
    }
    else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kReloadConfiguration)) {
        ReReadConfiguration ();
        return true;
    }
    else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kPause)) {
        Pause ();
        return true;
    }
    else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kContinue)) {
        Continue ();
        return true;
    }
    /// MANY more neeeded, and fix to use named constants...
    return false;
}

