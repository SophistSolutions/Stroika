/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
#include    "../../Foundation/Characters/String_Constant.h"
#include    "../../Foundation/Characters/SDKString.h"
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
#include    "../../Foundation/Execution/SignalHandlers.h"
#include    "../../Foundation/IO/FileSystem/WellKnownLocations.h"
#include    "../../Foundation/Memory/SmallStackBuffer.h"

#include    "Main.h"


using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Containers;
using   namespace   Stroika::Foundation::Memory;

using   namespace   Stroika::Frameworks;
using   namespace   Stroika::Frameworks::Service;

using   Characters::SDKString;
using   Characters::String_Constant;
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
            pair<String, MajorOperation> (Main::CommandNames::kRunDirectly, MajorOperation::eRunDirectly),
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

void    Main::IServiceIntegrationRep::_RunDirectly ()
{
}



/*
 ********************************************************************************
 ************************************ Service::Main *****************************
 ********************************************************************************
 */
const   wchar_t Service::Main::CommandNames::kInstall[]             =   L"Install";
const   wchar_t Service::Main::CommandNames::kUnInstall[]           =   L"UnInstall";
const   wchar_t Service::Main::CommandNames::kRunAsService[]        =   L"Run-As-Service";
const   wchar_t Service::Main::CommandNames::kRunDirectly[]         =   L"Run-Directly";
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
    if (args.fMajorOperation.IsMissing ()) {
        Execution::DoThrow (Execution::InvalidCommandLineArgument (String_Constant (L"No recognized operation")));
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
        case CommandArgs::MajorOperation::eRunDirectly: {
                RunDirectly ();
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
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::GetServiceStatusMessage"));
    const   wchar_t kTAB[]  =   L"    ";    // use spaces instead of tab so formatting independent of tabstop settings
    ServiceDescription  svd =   GetServiceDescription ();
    wstringstream   tmp;
    tmp << L"Service '" << svd.fPrettyName.As<wstring> () << "'" << endl;
    switch (GetState ()) {
        case    State::eStopped:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "STOPPED" << endl;
            break;
        case    State::eRunning:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "Running" << endl;
            if (GetServiceIntegrationFeatures ().Contains (ServiceIntegrationFeatures::eGetServicePID)) {
                tmp << kTAB << L"PID:    " << kTAB << kTAB << kTAB << kTAB << GetServicePID () << endl;
            }
            break;
        case    State::ePaused:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "PAUSED" << endl;
            if (GetServiceIntegrationFeatures ().Contains (ServiceIntegrationFeatures::eGetServicePID)) {
                tmp << kTAB << L"PID:    " << kTAB << kTAB << kTAB << kTAB << GetServicePID () << endl;
            }
            break;
        default:
            AssertNotReached ();
    }
    DbgTrace (L"returning status: (%s)", tmp.str ().c_str ());
    return tmp.str ();
}

void    Main::RunAsService ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::RunAsService"));
    GetServiceRep_ ()._RunAsAsService ();
}

void    Main::RunDirectly ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::RunDirectly"));
    GetServiceRep_ ()._RunDirectly ();
}

void   Main::ForcedRestart (Time::DurationSecondsType timeout, Time::DurationSecondsType unforcedStopTimeout)
{
    AssertNotImplemented ();
}

void    Main::ReReadConfiguration ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::ReReadConfiguration"));
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
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::Restart"));
    DbgTrace ("(timeout = %f)", timeout);

    /////// WRONG HANDLING OF TIMEOUT
    Stop (timeout);
    Start (timeout);
#if 0
    Time::DurationSecondsType endAt =   Time::GetTickCount () + timeout;
    IgnoreExceptionsForCall (Stop (timeout));
#if     qPlatform_POSIX
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_sAppRep->_GetPIDFileName ().AsSDKString ().c_str ());
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

Set<Main::ServiceIntegrationFeatures>   Main::LoggerServiceWrapper::_GetSupportedFeatures () const
{
    return fDelegateTo_->_GetSupportedFeatures ();
}

Main::State     Main::LoggerServiceWrapper::_GetState () const
{
    return fDelegateTo_->_GetState ();
}

void    Main::LoggerServiceWrapper::_Install ()
{
    Logger::Log (Logger::Priority::eNotice, L"Installing Service...");
    try {
        fDelegateTo_->_Install ();
    }
    catch (...) {
        // @todo - capture useful message
        Logger::Log (Logger::Priority::eCriticalError, L"Failed to install - aborting...");
        Execution::DoReThrow ();
    }
}

void    Main::LoggerServiceWrapper::_UnInstall ()
{
    Logger::Log (Logger::Priority::eNotice, L"UnInstalling Service...");
    try {
        fDelegateTo_->_UnInstall ();
    }
    catch (...) {
        // @todo - capture useful message
        Logger::Log (Logger::Priority::eCriticalError, L"Failed to uninstall - aborting...");
        Execution::DoReThrow ();
    }
}

void    Main::LoggerServiceWrapper::_RunAsAsService ()
{
    Logger::Log (Logger::Priority::eNotice, L"Service Starting");
    try {
        fDelegateTo_->_RunAsAsService ();
    }
    catch (...) {
        Logger::Log (Logger::Priority::eCriticalError, L"Exception running service - aborting...");
        Execution::DoReThrow ();
    }
    Logger::Log (Logger::Priority::eNotice, L"Service Stopped Normally");
}

void    Main::LoggerServiceWrapper::_RunDirectly ()
{
    Logger::Log (Logger::Priority::eNotice, L"Service Starting in Run-Direct (non service) mode.");
    try {
        fDelegateTo_->_RunDirectly ();
    }
    catch (...) {
        Logger::Log (Logger::Priority::eCriticalError, L"Exception running service directly - aborting...");
        Execution::DoReThrow ();
    }
    Logger::Log (Logger::Priority::eNotice, L"Service Stopped Normally");
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

Set<Main::ServiceIntegrationFeatures>   Main::RunTilIdleService::_GetSupportedFeatures () const
{
    return Set<Main::ServiceIntegrationFeatures> ();
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

void        Main::RunTilIdleService::_RunAsAsService ()
{
    // VERY WEAK TO WRONG IMPL
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.SetThreadName (L"Service 'Run' thread");
    fRunThread_.Start ();
    float timeTilIdleHack = 3.0;
    IgnoreExceptionsExceptThreadAbortForCall (fRunThread_.WaitForDone (timeTilIdleHack));   //tmphack - as
}

void    Main::RunTilIdleService::_RunDirectly ()
{
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.SetThreadName (L"Service 'Run' thread");
    fRunThread_.Start ();
    IgnoreExceptionsExceptThreadAbortForCall (fRunThread_.WaitForDone ());
}

void  Main::RunTilIdleService::_Start (Time::DurationSecondsType timeout)
{
    // VERY WEAK TO WRONG IMPL
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.SetThreadName (L"Service 'Run' thread");
    fRunThread_.Start ();
}

void            Main::RunTilIdleService::_Stop (Time::DurationSecondsType timeout)
{
    // VERY WEAK TO WRONG IMPL
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::RunTilIdleService::_Stop"));
    fRunThread_.AbortAndWaitForDone (timeout);
}

void            Main::RunTilIdleService::_ForcedStop (Time::DurationSecondsType timeout)
{
    // VERY WEAK TO WRONG IMPL
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::RunTilIdleService::_Stop"));
    fRunThread_.AbortAndWaitForDone (timeout);
}

pid_t   Main::RunTilIdleService::_GetServicePID () const
{
    Execution::DoThrow (Execution::OperationNotSupportedException (String_Constant (L"GetServicePID")));
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

Main::BasicUNIXServiceImpl::~BasicUNIXServiceImpl ()
{
    // SHOULD CLEAN THIS CODE UP SO YOU COULD DESTROY AND RECREATE - PROBABLY SHOULD LOSE sCurrApp_
    Require (fAppRep_.get () == nullptr);
    Require (sCurrApp_.get () == nullptr);
}

void    Main::BasicUNIXServiceImpl::_Attach (shared_ptr<IApplicationRep> appRep)
{
    Execution::Thread::SuppressAbortInContext  suppressAbort;       // this must run to completion - it only blocks waiting for subsidiary thread to finish
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
    fRunThread_.AbortAndWaitForDone ();
    fRunThread_ = Execution::Thread ();
}

shared_ptr<Main::IApplicationRep>      Main::BasicUNIXServiceImpl::_GetAttachedAppRep () const
{
    return fAppRep_;
}

Set<Main::ServiceIntegrationFeatures>   Main::BasicUNIXServiceImpl::_GetSupportedFeatures () const
{
    Set<Main::ServiceIntegrationFeatures>   result;
    result.Add (Main::ServiceIntegrationFeatures::eGetServicePID);
    return result;
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
    Execution::DoThrow (Execution::OperationNotSupportedException (String_Constant (L"Install")));
}

void    Main::BasicUNIXServiceImpl::_UnInstall ()
{
    Execution::DoThrow (Execution::OperationNotSupportedException (String_Constant (L"UnInstall")));
}

void    Main::BasicUNIXServiceImpl::_RunAsAsService ()
{
    if (_IsServiceActuallyRunning ()) {
        Execution::DoThrow (Execution::StringException (String_Constant (L"Service Already Running")));
    }

    // VERY WEAK IMPL
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.SetThreadName (L"Service 'Run' thread");
    sigHandlerThread2Abort_ = fRunThread_;
    fRunThread_.Start ();
    Execution::Finally cleanup ([this] () {
        ::unlink (_GetPIDFileName ().AsSDKString ().c_str ());
    });
    {
        ofstream    out (_GetPIDFileName ().AsSDKString ().c_str ());
        out << getpid () << endl;
    }
    fRunThread_.WaitForDone ();
}

void    Main::BasicUNIXServiceImpl::_RunDirectly ()
{
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.SetThreadName (L"Service 'Run' thread");
    fRunThread_.Start ();
    IgnoreExceptionsExceptThreadAbortForCall (fRunThread_.WaitForDone ());
}

void    Main::BasicUNIXServiceImpl::_Start (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::Start"));
    DbgTrace ("(timeout = %f)", timeout);

    Time::DurationSecondsType timeoutAt =   Time::GetTickCount () + timeout;

    // Check not already runnig, (someday) and then for and exec the

    if (_IsServiceFailed ()) {
        _CleanupDeadService ();
    }

    // REALLY should use GETSTATE - and return state based on if PID file exsits...
    if (_GetServicePID ()  != 0) {
        Execution::DoThrow (Execution::StringException (String_Constant (L"Cannot Start service because its already running")));
    }

    (void)Execution::DetachedProcessRunner (Execution::GetEXEPath (), Sequence<String> ( {String (), (String (L"--") + String (CommandNames::kRunAsService))}));

    while (not _IsServiceActuallyRunning ()) {
        Execution::Sleep (0.5);
        Execution::ThrowTimeoutExceptionAfter (timeoutAt);
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
        if (_IsServiceActuallyRunning ()) {
            Execution::ThrowErrNoIfNegative (kill (_GetServicePID (), SIGTERM));

            Time::DurationSecondsType   waitFor = 0.001;    // wait just a little at first but then progressively longer (avoid busy wait)
            while (_IsServiceActuallyRunning ()) {
                Execution::Sleep (waitFor);
                if (waitFor < timeout and waitFor < 5) {
                    waitFor *= 2;
                }
                Execution::ThrowTimeoutExceptionAfter (timeoutAt);
            }
        }
        // in case not cleanly stopped before
        (void)unlink (_GetPIDFileName ().AsSDKString ().c_str ());
    }
}

void    Main::BasicUNIXServiceImpl::_ForcedStop (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::_ForcedStop"));
    // Send signal to server to stop
    Execution::ThrowErrNoIfNegative (kill (_GetServicePID (), SIGKILL));
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_GetPIDFileName ().AsSDKString ().c_str ());
}

pid_t   Main::BasicUNIXServiceImpl::_GetServicePID () const
{
    ifstream    in (_GetPIDFileName ().AsSDKString ().c_str ());
    if (in) {
        pid_t   n = 0;
        in >> n;
        return n;
    }
    return 0;
}

void    Main::BasicUNIXServiceImpl::SetupSignalHanlders_ ()
{
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGINT, SignalHandler_);
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGTERM, SignalHandler_);
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (kSIG_ReReadConfiguration, SignalHandler_);
}

String  Main::BasicUNIXServiceImpl::_GetPIDFileName () const
{
    return IO::FileSystem::WellKnownLocations::GetTemporary () + fAppRep_->GetServiceDescription ().fRegistrationName + String_Constant (L".pid");
}

bool    Main::BasicUNIXServiceImpl::_IsServiceFailed ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::_IsServiceFailed"));
    pid_t   servicePID  =   _GetServicePID ();
    if (servicePID > 0) {
        return not _IsServiceActuallyRunning ();
    }
    return false;
}

void    Main::BasicUNIXServiceImpl::_CleanupDeadService ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::_CleanupDeadService"));
    // REALY should WAIT for server to stop and only do this it fails -
    unlink (_GetPIDFileName ().AsSDKString ().c_str ());
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

void    Main::BasicUNIXServiceImpl::SignalHandler_ (SignalID signum)
{
    // @todo        TOTALLY BAD/BUGGY - CANNOT ALLOCATE MEMORY FROM INSIDE SIGNAL HANDLER - FIX!!!!
    //

    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::SignalHandler_"));
    DbgTrace (L"(signal = %s)", Execution::SignalToName (signum).c_str ());
    // VERY PRIMITIVE IMPL FOR NOW -- LGP 2011-09-24
    switch (signum) {
        case    SIGINT:
        case    SIGTERM:
            DbgTrace ("setting fStopping_ to true");
            sigHandlerThread2Abort_.Abort ();
//            fStopping_ = true;
            break;
#if     !qCompilerAndStdLib_constexpr_Buggy
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

Set<Main::ServiceIntegrationFeatures>   Main::WindowsService::_GetSupportedFeatures () const
{
    Set<Main::ServiceIntegrationFeatures>   result;
    result.Add (Main::ServiceIntegrationFeatures::eGetServicePID);
    result.Add (Main::ServiceIntegrationFeatures::eInstall);
    return result;
}

Main::State     Main::WindowsService::_GetState () const
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::_GetState"));
    const DWORD   kServiceMgrAccessPrivs   =   SERVICE_QUERY_STATUS;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup1 ([hSCM] () {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });
    SC_HANDLE   hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    const bool  kUseQueryServiceStatusEx_   =    false;
    if (kUseQueryServiceStatusEx_) {
        SERVICE_STATUS_PROCESS  serviceProcess;
        memset (&serviceProcess, 0, sizeof (serviceProcess));
        DWORD ignored = 0;
        Execution::Platform::Windows::ThrowIfFalseGetLastError (::QueryServiceStatusEx (hService, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE> (&serviceProcess), sizeof (serviceProcess), &ignored));
        switch (serviceProcess.dwCurrentState) {
            case SERVICE_RUNNING:
                return Main::State::eRunning;
            case SERVICE_PAUSED:
                return Main::State::ePaused;
        }
    }
    else {
        SERVICE_STATUS  serviceStatus;
        memset (&serviceStatus, 0, sizeof (serviceStatus));
        Execution::Platform::Windows::ThrowIfFalseGetLastError (::QueryServiceStatus (hService, &serviceStatus));
        switch (serviceStatus.dwCurrentState) {
            case SERVICE_RUNNING:
                return Main::State::eRunning;
            case SERVICE_PAUSED:
                return Main::State::ePaused;
        }
    }
    return Main::State::eStopped;
}

void    Main::WindowsService::_Install ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::_Install"));

    const DWORD   kServiceMgrAccessPrivs   =   SC_MANAGER_CREATE_SERVICE;
    String  cmdLineForRunSvc = Characters::String_Constant (L"\"") + Execution::GetEXEPath () + String_Constant (L"\" --") + CommandNames::kRunAsService;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE hService = ::CreateService (
                             hSCM, GetSvcName_ ().c_str (), GetSvcName_ ().c_str (),
                             kServiceMgrAccessPrivs, SERVICE_WIN32_OWN_PROCESS,
                             SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
                             cmdLineForRunSvc.AsSDKString ().c_str (), NULL, NULL, _T("RPCSS\0"), NULL, NULL
                         );
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
}

void    Main::WindowsService::_UnInstall ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::_UnInstall"));

    const DWORD   kServiceMgrAccessPrivs   =   SERVICE_STOP | DELETE;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        AssertNotNull (hService);
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

void    Main::WindowsService::_RunAsAsService ()
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::_RunAsAsService"));
    Assert (s_SvcRunningTHIS_ == nullptr);
    s_SvcRunningTHIS_ = this;

    // MSFT docs unclear on lifetime requirements on these args but for now assume data copied...
    SDKString svcName =   GetSvcName_ ();
    SERVICE_TABLE_ENTRY st[] = {
        { const_cast<TCHAR*> (svcName.c_str ()), StaticServiceMain_ },
        { nullptr, nullptr }
    };
    if (::StartServiceCtrlDispatcher (st) == 0) {
        fServiceStatus_.dwWin32ExitCode = ::GetLastError ();
        if (fServiceStatus_.dwWin32ExitCode ==      ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            DbgTrace ("fServiceStatus_.dwWin32ExitCode = ERROR_FAILED_SERVICE_CONTROLLER_CONNECT");
        }
        DbgTrace ("fServiceStatus_.dwWin32ExitCode = %d", fServiceStatus_.dwWin32ExitCode);
    }
}

void    Main::WindowsService::_RunDirectly ()
{
    auto appRep = fAppRep_;
    fRunThread_ = Execution::Thread ([appRep] () {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.SetThreadName (L"Service 'Run' thread");
    fRunThread_.Start ();
    IgnoreExceptionsExceptThreadAbortForCall (fRunThread_.WaitForDone ());
}

void    Main::WindowsService::_Start (Time::DurationSecondsType timeout)
{
    // @todo - timeout not supported
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::Start"));
    DbgTrace ("(timeout = %f)", timeout);

    const DWORD   kServiceMgrAccessPrivs   =   SERVICE_START;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });
    SC_HANDLE   hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    DWORD dwNumServiceArgs = 0;
    LPCTSTR* lpServiceArgVectors = nullptr;
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::StartService (hService, dwNumServiceArgs, lpServiceArgVectors));
}

void    Main::WindowsService::_Stop (Time::DurationSecondsType timeout)
{
    // @todo - timeout not supported
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::_Stop"));
    const DWORD   kServiceMgrAccessPrivs   =   SERVICE_STOP;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup ([hSCM] () {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        AssertNotNull (hService);
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
    const DWORD   kServiceMgrAccessPrivs   =   SERVICE_QUERY_STATUS;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup1 ([hSCM] () {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });
    SC_HANDLE   hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    SERVICE_STATUS_PROCESS  serviceProcess;
    memset (&serviceProcess, 0, sizeof (serviceProcess));
    DWORD ignored = 0;
    Execution::Platform::Windows::ThrowIfFalseGetLastError (::QueryServiceStatusEx (hService, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE> (&serviceProcess), sizeof (serviceProcess), &ignored));
    return serviceProcess.dwProcessId;
}

SDKString Main::WindowsService::GetSvcName_ () const
{
    RequireNotNull (fAppRep_);  // must attach first
    return fAppRep_->GetServiceDescription ().fRegistrationName.AsSDKString ();
}

bool    Main::WindowsService::IsInstalled_ () const noexcept
{
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::IsInstalled_"));
    const DWORD   kServiceMgrAccessPrivs   =   SERVICE_QUERY_CONFIG;
    SC_HANDLE hSCM = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hSCM != NULL);
    Execution::Finally cleanup1 ([hSCM] () {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE   hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfFalseGetLastError (hService != NULL);
    Execution::Finally cleanup2 ([hService] () {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });
    return hService != NULL;
}

void    Main::WindowsService::SetServiceStatus_ (DWORD dwState) noexcept {
    DbgTrace ("SetServiceStatus_ (%d)", dwState);
    Assert (fServiceStatusHandle_ != nullptr);
    fServiceStatus_.dwCurrentState = dwState;
    ::SetServiceStatus (fServiceStatusHandle_, &fServiceStatus_);
}

void    Main::WindowsService::ServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept {
    Debug::TraceContextBumper traceCtx (SDKSTR ("Stroika::Frameworks::Service::Main::WindowsService::ServiceMain_"));
    ///@TODO - FIXUP EXCEPTION HANLDING HERE!!!

    // do file create stuff here
    //Logger::Log (Logger::Priority::eInfo, L"entering ServiceMain_");

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
    fRunThread_ = Execution::Thread ([appRep] ()
    {
        appRep->MainLoop ([] () {});
    });
    fRunThread_.SetThreadName (L"Service 'Run' thread");
    fRunThread_.Start ();
    //Logger::Log (Logger::Priority::eInfo, L"in ServiceMain_ about to set SERVICE_RUNNING");
    SetServiceStatus_ (SERVICE_RUNNING);

    IgnoreExceptionsExceptThreadAbortForCall (fRunThread_.WaitForDone ());   //tmphack - as
    fServiceStatus_.dwWin32ExitCode = 0;

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
    if (fServiceStatus_.dwCurrentState == SERVICE_RUNNING)
    {
        SetServiceStatus_ (SERVICE_STOP_PENDING);
        fRunThread_.Abort ();
    }
}

void    Main::WindowsService::Handler_ (DWORD dwOpcode) noexcept {
    switch (dwOpcode)
    {
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
