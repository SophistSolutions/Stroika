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
#if     qPlatform_Windows
#include    "../../Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include    "../../Foundation/Characters/Format.h"
#include    "../../Foundation/Characters/TString.h"
#include    "../../Foundation/Containers/Common.h"
#include    "../../Foundation/Debug/Assertions.h"
#include    "../../Foundation/Debug/Trace.h"
#include    "../../Foundation/Execution/CommandLine.h"
#include    "../../Foundation/Execution/Exceptions.h"
#include    "../../Foundation/Execution/ErrNoException.h"
#include    "../../Foundation/Execution/Finally.h"
#include    "../../Foundation/Execution/Module.h"
#include    "../../Foundation/Execution/OperationNotSupportedException.h"
#include    "../../Foundation/Execution/ProcessRunner.h"
#include    "../../Foundation/Execution/ThreadAbortException.h"
#include    "../../Foundation/Execution/Sleep.h"
#include    "../../Foundation/Execution/WaitTimedOutException.h"
#include    "../../Foundation/IO/FileSystem/WellKnownLocations.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "Main.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::Service;

using   Characters::TString;
using   Execution::Logger;







/*
 ********************************************************************************
 *********************** Service::Main::CommandArgs *****************************
 ********************************************************************************
 */
Main::CommandArgs::CommandArgs ()
    : fMajorOperation ()
    , fUnusedArguments ()
{
}

Main::CommandArgs::CommandArgs (const Sequence<String>& args)
    : fMajorOperation ()
    , fUnusedArguments ()
{
    bool didFirst = false;
    for (String si : args) {
        if (not didFirst) {
            // skip argv[0]
            // @todo - nice to try args.SubSequence (1)....
            didFirst = true;
            continue;
        }
        static  const   pair<String, MajorOperation> kPairs_[] = {
            pair<String, MajorOperation> (Main::CommandNames::kInstall, MajorOperation::eInstall),
            pair<String, MajorOperation> (Main::CommandNames::kUnInstall, MajorOperation::eUnInstall),
            pair<String, MajorOperation> (Main::CommandNames::kRunAsService, MajorOperation::eRunServiceMain),
            pair<String, MajorOperation> (Main::CommandNames::kStart, MajorOperation::eStart),
            pair<String, MajorOperation> (Main::CommandNames::kStop, MajorOperation::eStop),
            pair<String, MajorOperation> (Main::CommandNames::kForcedStop, MajorOperation::eForcedStop),
            pair<String, MajorOperation> (Main::CommandNames::kRestart, MajorOperation::eRestart),
            pair<String, MajorOperation> (Main::CommandNames::kForcedRestart, MajorOperation::eForcedRestart),
            pair<String, MajorOperation> (Main::CommandNames::kReloadConfiguration, MajorOperation::eReloadConfiguration),
            pair<String, MajorOperation> (Main::CommandNames::kPause, MajorOperation::ePause),
            pair<String, MajorOperation> (Main::CommandNames::kContinue, MajorOperation::eContinue),
        };
        bool    found   =   false;
        for (auto i : kPairs_) {
            if (Execution::MatchesCommandLineArgument (si, i.first)) {
                if (found) {
                    Execution::DoThrow (Execution::InvalidCommandLineArgument (L"Only one major command-line option can be specified at a time"));
                }
                found = true;
                fMajorOperation = i.second;
            }
        }
        if (not found) {
            fUnusedArguments.push_back (si);
        }
    }
}





/*
 ********************************************************************************
 ******************* Service::Main::IApplicationRep *****************************
 ********************************************************************************
 */
bool    Main::IApplicationRep::HandleCommandLineArgument (const String& s)
{
    return false;
}

void    Main::IApplicationRep::OnReReadConfigurationRequest ()
{
    //  fMustReReadConfig = true;
}

String  Main::IApplicationRep::GetServiceStatusMessage () const
{
    return String ();
}



/*
 ********************************************************************************
 ********************* Main::IServiceIntegrationRep *****************************
 ********************************************************************************
 */
bool    Main::IServiceIntegrationRep::HandleCommandLineArgument (const String& s)
{
    return _GetAttachedAppRep ()->HandleCommandLineArgument (s);
}



/*
 ********************************************************************************
 ************************************ Service::Main *****************************
 ********************************************************************************
 */
const   wchar_t Service::Main::CommandNames::kInstall[]             =   L"Install";
const   wchar_t Service::Main::CommandNames::kUnInstall[]           =   L"UnInstall";
const   wchar_t Service::Main::CommandNames::kRunAsService[]        =   L"Run-As-Service";
const   wchar_t Service::Main::CommandNames::kStart[]               =   L"Start";
const   wchar_t Service::Main::CommandNames::kStop[]                =   L"Stop";
const   wchar_t Service::Main::CommandNames::kForcedStop[]          =   L"ForcedStop";
const   wchar_t Service::Main::CommandNames::kRestart[]             =   L"Restart";
const   wchar_t Service::Main::CommandNames::kForcedRestart[]       =   L"ForcedRestart";
const   wchar_t Service::Main::CommandNames::kReloadConfiguration[] =   L"Reload-Configuration";
const   wchar_t Service::Main::CommandNames::kPause[]               =   L"Pause";
const   wchar_t Service::Main::CommandNames::kContinue[]            =   L"Continue";

Main*   Main::sTHIS_    =   nullptr;

shared_ptr<Main::IServiceIntegrationRep>    Main::mkDefaultServiceIntegrationRep ()
{
#if     qPlatform_POSIX
    return shared_ptr<IServiceIntegrationRep> (new BasicUNIXServiceImpl ());
#elif   qPlatform_Windows
    return shared_ptr<IServiceIntegrationRep> (new WindowsService ());
#else
    return shared_ptr<IServiceIntegrationRep> (new RunNoFrillsService ());
#endif
}

Main::Main (shared_ptr<IApplicationRep> rep, shared_ptr<IServiceIntegrationRep> serviceIntegrationRep)
    : fServiceRep_ (serviceIntegrationRep)
{
    Require (sTHIS_ == nullptr);    // singleton(ish)
    sTHIS_ = this;
    RequireNotNull (rep);
    RequireNotNull (serviceIntegrationRep);
    serviceIntegrationRep->_Attach (rep);
}

Main::~Main ()
{
    Require (sTHIS_ == this);
    sTHIS_ = nullptr;
    fServiceRep_->_Attach (nullptr);
}

void    Main::Run (const CommandArgs& args)
{
    for (String i : args.fUnusedArguments) {
        fServiceRep_->HandleCommandLineArgument (i);
    }
    if (args.fMajorOperation.empty ()) {
        Execution::DoThrow (Execution::InvalidCommandLineArgument (L"No recognized operation"));
    }
    switch (*args.fMajorOperation) {
        case CommandArgs::MajorOperation::eInstall: {
                Install ();
            }
            break;
        case CommandArgs::MajorOperation::eUnInstall: {
                UnInstall ();
            }
            break;
        case CommandArgs::MajorOperation::eRunServiceMain: {
                RunAsService ();
            }
            break;
        case CommandArgs::MajorOperation::eStart: {
                Start ();
            }
            break;
        case CommandArgs::MajorOperation::eStop: {
                Stop ();
            }
            break;
        case CommandArgs::MajorOperation::eForcedStop: {
                AssertNotImplemented ();
                //ForcedStop ();
            }
            break;
        case CommandArgs::MajorOperation::eRestart: {
                Restart ();
            }
            break;
        case CommandArgs::MajorOperation::eForcedRestart: {
                ForcedRestart ();
            }
            break;
        case CommandArgs::MajorOperation::eReloadConfiguration: {
                ReReadConfiguration ();
            }
            break;
        case CommandArgs::MajorOperation::ePause: {
                Pause ();
            }
            break;
        case CommandArgs::MajorOperation::eContinue: {
                Continue ();
            }
            break;
        default: {
                AssertNotReached ();
            }
            break;
    }
}

String      Main::GetServiceStatusMessage () const
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::GetServiceStatusMessage"));
    const   wchar_t kTAB[]  =   L"    ";    // use spaces instead of tab so formatting independent of tabstop settings
    ServiceDescription  svd =   GetServiceDescription ();
    wstringstream   tmp;
    tmp << L"Service '" << svd.fPrettyName.As<wstring> () << "'" << endl;
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
    DbgTrace (L"returning status: (%s)", tmp.str ().c_str ());
    return tmp.str ();
}

void    Main::RunAsService ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::RunAsService"));
    GetServiceRep_ ()._RunAsAservice ();
}

void   Main::ForcedRestart (Time::DurationSecondsType timeout, Time::DurationSecondsType unforcedStopTimeout)
{
    AssertNotImplemented ();
}

void    Main::ReReadConfiguration ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::ReReadConfiguration"));
#if qPlatform_Windows
    AssertNotImplemented ();
#elif   qPlatform_POSIX
    pid_t   pid =   GetServicePID ();
    Assert (pid != 0);  // maybe throw if non-zero???
    Execution::ThrowErrNoIfNegative (kill (GetServicePID (), Main::BasicUNIXServiceImpl::kSIG_ReReadConfiguration));
#else
    AssertNotImplemented ();
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
    return GetAppRep_ ().GetServiceDescription ();
}

void    Main::Restart (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Restart"));
    DbgTrace ("(timeout = %f)", timeout);

    /////// WRONG HANDLING OF TIMEOUT
    Stop (timeout);
    Start (timeout);
#if 0
    Time::DurationSecondsType endAt =   Time::GetTickCount () + timeout;
    IgnoreExceptionsForCall (Stop (timeout));
#if     qPlatform_POSIX
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_sAppRep->_GetPIDFileName ().AsTString ().c_str ());
#endif
    Start (endAt - Time::GetTickCount ());
#endif
}




/*
 ********************************************************************************
 **************** Service::Main::LoggerServiceWrapper ***************************
 ********************************************************************************
 */
Main::LoggerServiceWrapper::LoggerServiceWrapper (shared_ptr<Main::IServiceIntegrationRep> delegateTo)
    : fDelegateTo_ (delegateTo)
{
    RequireNotNull (delegateTo);
}

void    Main::LoggerServiceWrapper::_Attach (shared_ptr<IApplicationRep> appRep)
{
    fDelegateTo_->_Attach (appRep);
}

shared_ptr<Main::IApplicationRep>      Main::LoggerServiceWrapper::_GetAttachedAppRep () const
{
    return fDelegateTo_->_GetAttachedAppRep ();
}

Main::State     Main::LoggerServiceWrapper::_GetState () const
{
    return fDelegateTo_->_GetState ();
}

void    Main::LoggerServiceWrapper::_Install ()
{
    Logger::Get ().Log (Logger::Priority::eInfo, L"Installing Service...");
    try {
        fDelegateTo_->_Install ();
    }
    catch (...) {
        // @todo - capture useful message
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Failed to install - aborting...");
        Execution::DoReThrow ();
    }
}

void    Main::LoggerServiceWrapper::_UnInstall ()
{
    Logger::Get ().Log (Logger::Priority::eInfo, L"UnInstalling Service...");
    try {
        fDelegateTo_->_UnInstall ();
    }
    catch (...) {
        // @todo - capture useful message
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Failed to uninstall - aborting...");
        Execution::DoReThrow ();
    }
}

void    Main::LoggerServiceWrapper::_RunAsAservice ()
{
    Logger::Get ().Log (Logger::Priority::eInfo, L"Service Starting");
    try {
        fDelegateTo_->_RunAsAservice ();
    }
    catch (...) {
        Logger::Get ().Log (Logger::Priority::eCriticalError, L"Exception running service - aborting...");
        Execution::DoReThrow ();
    }
    Logger::Get ().Log (Logger::Priority::eInfo, L"Service Stopped Normally");
}

void  Main::LoggerServiceWrapper::_Start (Time::DurationSecondsType timeout)
{
    fDelegateTo_->_Start (timeout);
}

void    Main::LoggerServiceWrapper::_Stop (Time::DurationSecondsType timeout)
{
    fDelegateTo_->_Stop (timeout);
}

void    Main::LoggerServiceWrapper::_ForcedStop (Time::DurationSecondsType timeout)
{
    fDelegateTo_->_ForcedStop (timeout);
}

pid_t   Main::LoggerServiceWrapper::_GetServicePID () const
{
    return fDelegateTo_->_GetServicePID ();
}





/*
 ********************************************************************************
 ******************* Service::Main::RunTilIdleService ***************************
 ********************************************************************************
 */
Main::RunTilIdleService::RunTilIdleService ()
    : fAppRep_ ()
    , fRunThread_ ()
{
}

void    Main::RunTilIdleService::_Attach (shared_ptr<IApplicationRep> appRep)
{
    Require ((appRep == nullptr and fAppRep_ != nullptr) or
             (fAppRep_ == nullptr and fAppRep_ != appRep)
            );
    fRunThread_.Abort ();
    fRunThread_ = Execution::Thread ();
    fAppRep_ = appRep;
}

shared_ptr<Main::IApplicationRep>      Main::RunTilIdleService::_GetAttachedAppRep () const
{
    return fAppRep_;
}

Main::State             Main::RunTilIdleService::_GetState () const
{
    //tmphack.... must think through states...
    switch (fRunThread_.GetStatus ()) {
        case Execution::Thread::Status::eRunning:
            return Main::State::eRunning;
    }
    return Main::State::eStopped;
}

void    Main::RunTilIdleService::_Install ()
{
    Execution::DoThrow (Execution::OperationNotSupportedException (L"Install"));
}

void    Main::RunTilIdleService::_UnInstall ()
{
    Execution::DoThrow (Execution::OperationNotSupportedException (L"UnInstall"));
}

void        Main::RunTilIdleService::_RunAsAservice ()
{
    // VERY WEAK TO WRONG IMPL
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.Start ();
    float timeTilIdleHack = 3.0;
    IgnoreExceptionsExceptThreadAbortForCall (fRunThread_.WaitForDone (timeTilIdleHack));   //tmphack - as
}

void  Main::RunTilIdleService::_Start (Time::DurationSecondsType timeout)
{
    // VERY WEAK TO WRONG IMPL
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.Start ();
}

void            Main::RunTilIdleService::_Stop (Time::DurationSecondsType timeout)
{
    // VERY WEAK TO WRONG IMPL
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::RunTilIdleService::_Stop"));
    fRunThread_.AbortAndWaitForDone (timeout);
}

void            Main::RunTilIdleService::_ForcedStop (Time::DurationSecondsType timeout)
{
    // VERY WEAK TO WRONG IMPL
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::RunTilIdleService::_Stop"));
    fRunThread_.AbortAndWaitForDone (timeout);
}

pid_t   Main::RunTilIdleService::_GetServicePID () const
{
    // VERY WEAK TO WRONG IMPL
    return 0;
}






#if     qPlatform_POSIX
/*
 ********************************************************************************
 ******************* Service::Main::BasicUNIXServiceImpl ************************
 ********************************************************************************
 */
namespace {
    Execution::Thread sigHandlerThread2Abort_;
    shared_ptr<Main::IApplicationRep> sCurrApp_;
}

Main::BasicUNIXServiceImpl::BasicUNIXServiceImpl ()
    : fAppRep_ ()
    , fRunThread_ ()
{
}

void    Main::BasicUNIXServiceImpl::_Attach (shared_ptr<IApplicationRep> appRep)
{
    Require ((appRep == nullptr and fAppRep_ != nullptr) or
             (fAppRep_ == nullptr and fAppRep_ != appRep)
            );
    if (fAppRep_ != nullptr) {
        // CLEAR SIGNAL HANDLER
    }
    fAppRep_ = appRep;
    sCurrApp_ = appRep;
    if (appRep != nullptr) {
        SetupSignalHanlders_ ();
    }
    fRunThread_.Abort ();
    fRunThread_ = Execution::Thread ();
}

shared_ptr<Main::IApplicationRep>      Main::BasicUNIXServiceImpl::_GetAttachedAppRep () const
{
    return fAppRep_;
}

Main::State             Main::BasicUNIXServiceImpl::_GetState () const
{
    // @todo - maybe not qutie right - but a good approx ... review...
    if (_GetServicePID () != 0) {
        return State::eRunning;
    }
    return State::eStopped;
}

void    Main::BasicUNIXServiceImpl::_Install ()
{
    Execution::DoThrow (Execution::OperationNotSupportedException (L"Install"));
}

void    Main::BasicUNIXServiceImpl::_UnInstall ()
{
    Execution::DoThrow (Execution::OperationNotSupportedException (L"UnInstall"));
}

void    Main::BasicUNIXServiceImpl::_RunAsAservice ()
{
    // VERY WEAK IMPL
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.Start ();
    sigHandlerThread2Abort_ = fRunThread_;
    {
        ofstream    out (_GetPIDFileName ().AsTString ().c_str ());
        out << getpid () << endl;
    }
    try {
        fRunThread_.WaitForDone ();
        ::unlink (_GetPIDFileName ().AsTString ().c_str ());
    }
    catch (...) {
        ::unlink (_GetPIDFileName ().AsTString ().c_str ());
    }
}

void    Main::BasicUNIXServiceImpl::_Start (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Start"));
    DbgTrace ("(timeout = %f)", timeout);

    Time::DurationSecondsType timeoutAt =   Time::GetTickCount () + timeout;

    // Check not already runnig, (someday) and then for and exec the

    if (_IsServiceFailed ()) {
        _CleanupDeadService ();
    }

    // REALLY should use GETSTATE - and return state based on if PID file exsits...
    if (_GetServicePID ()  != 0) {
        Execution::DoThrow (Execution::StringException (L"Cannot Start service because its already running"));
    }

    (void)Execution::DetachedProcessRunner (Execution::GetEXEPath (), Sequence<String> ( {String (), (String (L"--") + String (CommandNames::kRunAsService))}));

    while (not _IsServiceActuallyRunning ()) {
        Execution::Sleep (0.5);
        if (Time::GetTickCount () > timeoutAt) {
            Execution::DoThrow (Execution::WaitTimedOutException ());
        }
    }
}

void            Main::BasicUNIXServiceImpl::_Stop (Time::DurationSecondsType timeout)
{
    bool kInProc_ = false;
    if (kInProc_) {
        /// kill running....
    }
    else {
        Time::DurationSecondsType timeoutAt =   Time::GetTickCount () + timeout;
        // Send signal to server to stop
        Execution::ThrowErrNoIfNegative (kill (_GetServicePID (), SIGTERM));
        while (_IsServiceActuallyRunning ()) {
            Execution::Sleep (0.5);
            if (Time::GetTickCount () > timeoutAt) {
                Execution::DoThrow (Execution::WaitTimedOutException ());
            }
        }
    }
}

void    Main::BasicUNIXServiceImpl::_ForcedStop (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::_ForcedStop"));
    // Send signal to server to stop
    Execution::ThrowErrNoIfNegative (kill (_GetServicePID (), SIGKILL));
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_GetPIDFileName ().AsTString ().c_str ());
}

pid_t   Main::BasicUNIXServiceImpl::_GetServicePID () const
{
    ifstream    in (_GetPIDFileName ().AsTString ().c_str ());
    if (in) {
        pid_t   n = 0;
        in >> n;
        return n;
    }
    return 0;
}

void    Main::BasicUNIXServiceImpl::SetupSignalHanlders_ ()
{
    Execution::SignalHandlerRegistry::Get ().AddSignalHandler (SIGTERM, SignalHandler_);
    Execution::SignalHandlerRegistry::Get ().AddSignalHandler (kSIG_ReReadConfiguration, SignalHandler_);
}

String  Main::BasicUNIXServiceImpl::_GetPIDFileName () const
{
    return IO::FileSystem::WellKnownLocations::GetTemporary () + fAppRep_->GetServiceDescription ().fRegistrationName + L".pid";
}

bool    Main::BasicUNIXServiceImpl::_IsServiceFailed ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::_IsServiceFailed"));
    pid_t   servicePID  =   _GetServicePID ();
    if (servicePID > 0) {
        return not _IsServiceActuallyRunning ();
    }
    return false;
}

void    Main::BasicUNIXServiceImpl::_CleanupDeadService ()
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::_CleanupDeadService"));
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_GetPIDFileName ().AsTString ().c_str ());
}

bool    Main::BasicUNIXServiceImpl::_IsServiceActuallyRunning ()
{
    pid_t   servicePID  =   _GetServicePID ();
    if (servicePID > 0) {
        int result  =   ::kill (servicePID, 0);
        return result == 0;
    }
    return false;
}

void    Main::BasicUNIXServiceImpl::SignalHandler_ (SignalIDType signum)
{
    // @todo        TOTALLY BAD/BUGGY - CANNOT ALLOCATE MEMORY FROM INSIDE SIGNAL HANDLER - FIX!!!!
    //

    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::SignalHandler_"));
    DbgTrace (L"(signal = %s)", Execution::SignalToName (signum).c_str ());
    // VERY PRIMITIVE IMPL FOR NOW -- LGP 2011-09-24
    switch (signum) {
        case    SIGTERM:
            DbgTrace ("setting fStopping_ to true");
            sigHandlerThread2Abort_.Abort ();
//            fStopping_ = true;
            break;
#if     qCompilerAndStdLib_Supports_constexpr
        case    kSIG_ReReadConfiguration:
#else
        case    SIGHUP:
#endif
            Assert (sCurrApp_.get () != nullptr);
            sCurrApp_->OnReReadConfigurationRequest ();
            break;
    }
}
#endif





#if     qPlatform_Windows
/*
 ********************************************************************************
 ************************* Service::Main::WindowsService ************************
 ********************************************************************************
 */
Main::WindowsService*       Main::WindowsService::s_SvcRunningTHIS_ =   nullptr;

Main::WindowsService::WindowsService ()
    : fAppRep_ ()
    , fServiceStatusHandle_ (nullptr)
    , fServiceStatus_ ()
{
    memset (&fServiceStatus_, 0, sizeof (fServiceStatus_));
    fServiceStatus_.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    fServiceStatus_.dwCurrentState = SERVICE_STOPPED;
    fServiceStatus_.dwControlsAccepted = SERVICE_ACCEPT_STOP;
}

void    Main::WindowsService::_Attach (shared_ptr<IApplicationRep> appRep)
{
    Require ((appRep == nullptr and fAppRep_ != nullptr) or
             (fAppRep_ == nullptr and fAppRep_ != appRep)
            );
    fAppRep_ = appRep;
}

shared_ptr<Main::IApplicationRep>   Main::WindowsService::_GetAttachedAppRep () const
{
    return fAppRep_;
}

Main::State     Main::WindowsService::_GetState () const
{
    AssertNotImplemented ();
    return Main::State::eStopped;
}

void    Main::WindowsService::_Install ()
{
    if (IsInstalled_ ()) {
        return;
    }

    String  cmdLineForRunSvc = L"\"" + Execution::GetEXEPath () + L"\" --" + CommandNames::kRunAsService;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        ::CloseServiceHandle (hSCM);
    });

    TString svcName = GetSvcName_ ();
    SC_HANDLE hService = ::CreateService (
                             hSCM, svcName.c_str (), svcName.c_str (),
                             SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                             SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
                             cmdLineForRunSvc.AsTString ().c_str (), NULL, NULL, _T("RPCSS\0"), NULL, NULL
                         );
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
}

void    Main::WindowsService::_UnInstall ()
{
    if (not IsInstalled_ ()) {
        return;
    }

    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        ::CloseServiceHandle (hSCM);
    });

    TString svcName = GetSvcName_ ();
    SC_HANDLE hService = ::OpenService (hSCM, svcName.c_str (), SERVICE_STOP | DELETE);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        ::CloseServiceHandle (hService);
    });

    {
        SERVICE_STATUS status;
        if (not ::ControlService (hService, SERVICE_CONTROL_STOP, &status)) {
            DWORD e = ::GetLastError ();
            if (e != ERROR_SERVICE_NOT_ACTIVE) {
                Execution::Platform::Windows::Exception::DoThrow (e);
            }
        }
    }

    Execution::Platform::Windows::ThrowIfFalseGetLastError (::DeleteService (hService));
}

void    Main::WindowsService::_RunAsAservice ()
{
    Assert (s_SvcRunningTHIS_ == nullptr);
    s_SvcRunningTHIS_ = this;

    // MSFT docs unclear on lifetime requirements on these args but for now assume data copied...
    TString svcName =   GetSvcName_ ();
    SERVICE_TABLE_ENTRY st[] = {
        { const_cast<TCHAR*> (svcName.c_str ()), StaticServiceMain_ },
        { nullptr, nullptr }
    };
    if (::StartServiceCtrlDispatcher (st) == 0) {
        fServiceStatus_.dwWin32ExitCode = ::GetLastError ();
        if (fServiceStatus_.dwWin32ExitCode ==      ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            //cerr << "fServiceStatus_.dwWin32ExitCode=" << "ERROR_FAILED_SERVICE_CONTROLLER_CONNECT" << endl;
        }
        //cerr << "fServiceStatus_.dwWin32ExitCode=" << fServiceStatus_.dwWin32ExitCode << endl;
    }
    //cerr << "exint WindowsService::_RunAsAservice" << endl;
}

void    Main::WindowsService::_Start (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::WindowsService::Start"));
    DbgTrace ("(timeout = %f)", timeout);

    // SEE UNIX IMPL - WE WANT REST OF CRAP THEY HAVE THERE TOO (except using processrunner)

#if      qCompilerAndStdLib_Supports_initializer_lists
    Execution::DetachedProcessRunner (Execution::GetEXEPath (), Sequence<String> ( {String (), (String (L"--") + String (CommandNames::kRunAsService))}));
#else
    Sequence<String>    tmp;
    tmp += String ();
    tmp += (String (L"--") + String (CommandNames::kRunAsService));
    Execution::DetachedProcessRunner (Execution::GetEXEPath (), tmp);
#endif




#if 0
    && & no - this must shellexec my exe with / run - as - service
#endif

}

void    Main::WindowsService::_Stop (Time::DurationSecondsType timeout)
{
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, SC_MANAGER_ALL_ACCESS);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        ::CloseServiceHandle (hSCM);
    });

    TString svcName = GetSvcName_ ();
    SC_HANDLE hService = ::OpenService (hSCM, svcName.c_str (), SERVICE_STOP | DELETE);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        ::CloseServiceHandle (hService);
    });

    {
        SERVICE_STATUS status;
        if (not ::ControlService (hService, SERVICE_CONTROL_STOP, &status)) {
            DWORD e = ::GetLastError ();
            if (e != ERROR_SERVICE_NOT_ACTIVE) {
                Execution::Platform::Windows::Exception::DoThrow (e);
            }
        }
    }
}

void    Main::WindowsService::_ForcedStop (Time::DurationSecondsType timeout)
{
    AssertNotImplemented ();
}

pid_t   Main::WindowsService::_GetServicePID () const
{
    return 0;
}

TString Main::WindowsService::GetSvcName_ () const
{
    RequireNotNull (fAppRep_);  // must attach first
    return fAppRep_->GetServiceDescription ().fRegistrationName.AsTString ();
}

bool    Main::WindowsService::IsInstalled_ () const noexcept
{
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, READ_CONTROL);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE   hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), SERVICE_QUERY_CONFIG);
    if (hService != nullptr) {
        ::CloseServiceHandle (hService);
    }
    return hService != NULL;
}

void    Main::WindowsService::SetServiceStatus_ (DWORD dwState) noexcept {
    DbgTrace ("SetServiceStatus_ (%d)", dwState);
    Assert (fServiceStatusHandle_ != nullptr);
    fServiceStatus_.dwCurrentState = dwState;
    ::SetServiceStatus (fServiceStatusHandle_, &fServiceStatus_);
}

void    Main::WindowsService::ServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept {
    ///@TODO - FIXUP EXCEPTION HANLDING HERE!!!

    // do file create stuff here
    //Logger::Get ().Log (Logger::Priority::eInfo, L"entering ServiceMain_");

    // Register the control request handler
    fServiceStatus_.dwCurrentState = SERVICE_START_PENDING;
    fServiceStatusHandle_ = ::RegisterServiceCtrlHandler (GetSvcName_ ().c_str (), StaticHandler_);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (fServiceStatusHandle_ != NULL);
    SetServiceStatus_ (SERVICE_START_PENDING);

    fServiceStatus_.dwWin32ExitCode = S_OK;
    fServiceStatus_.dwCheckPoint = 0;
    fServiceStatus_.dwWaitHint = 0;

    // When the Run function returns, the service has stopped.
    // about like this - FIX - KEEP SOMETHING SIMIALR
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.Start ();
    //Logger::Get ().Log (Logger::Priority::eInfo, L"in ServiceMain_ about to set SERVICE_RUNNING");
    SetServiceStatus_ (SERVICE_RUNNING);

    IgnoreExceptionsExceptThreadAbortForCall (fRunThread_.WaitForDone ());   //tmphack - as
    fServiceStatus_.dwWin32ExitCode = 0;

    //    fServiceStatus_.dwWin32ExitCode = ServiceRun_ ();


/// THIS BELEOW LOGIC BELONGS IN SERVICEMAIN!!!

    // Run ACTUAL code in separate thread. This must wiat on fStopServiceEvent_, if thread terminates otehrwise, it
    // must also set fStopServiceEvent_...
    // then this will bottom out.

    // At START of this code - we do file (PIDFILE) etc create, and after - PIDFILE cleanup

    //  fStopServiceEvent_.Wait ();

    /// DO ABORT ON THREAD HERE...AND WIAT TIL ITS DONE

#if 0
    ::DeleteFile (GetServerRunningFilePath_ ().c_str ());
#endif

    //Logger::EmitMessage (Logger::eInformation_MT, "Service stopped");
    SetServiceStatus_ (SERVICE_STOPPED);
}

void    WINAPI  Main::WindowsService::StaticServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept {
    AssertNotNull (s_SvcRunningTHIS_);
    s_SvcRunningTHIS_->ServiceMain_ (dwArgc, lpszArgv);
}

void    Main::WindowsService::OnStopRequest_ () noexcept {
    /*
     *  WARNING - this maybe a race about setting status!!! - what if we get stop request when already stopped.
     *  THIS CODE NEEDS THREAD LOCKS!!!
     */
    if (fServiceStatus_.dwCurrentState == SERVICE_RUNNING) {
        SetServiceStatus_ (SERVICE_STOP_PENDING);
        fRunThread_.Abort ();
    }
}

void    Main::WindowsService::Handler_ (DWORD dwOpcode) noexcept {
    switch (dwOpcode) {
        case SERVICE_CONTROL_STOP:
            OnStopRequest_  ();
            break;
        default:
            // others ignored for now
            break;
    }
}

void    WINAPI  Main::WindowsService::StaticHandler_ (DWORD dwOpcode) noexcept {
    AssertNotNull (s_SvcRunningTHIS_);
    s_SvcRunningTHIS_->Handler_ (dwOpcode);
}
#endif
