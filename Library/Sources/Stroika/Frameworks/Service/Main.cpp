/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>

#if qPlatform_POSIX
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#if qPlatform_Windows
#include "../../Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/SDKString.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Debug/Trace.h"
#include "../../Foundation/Execution/CommandLine.h"
#include "../../Foundation/Execution/Exceptions.h"
#include "../../Foundation/Execution/Finally.h"
#include "../../Foundation/Execution/Module.h"
#include "../../Foundation/Execution/OperationNotSupportedException.h"
#include "../../Foundation/Execution/Process.h"
#include "../../Foundation/Execution/ProcessRunner.h"
#include "../../Foundation/Execution/SignalHandlers.h"
#include "../../Foundation/Execution/Sleep.h"
#include "../../Foundation/Execution/Synchronized.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/Execution/TimeOutException.h"
#include "../../Foundation/IO/FileSystem/PathName.h"
#include "../../Foundation/IO/FileSystem/WellKnownLocations.h"
#include "../../Foundation/Streams/iostream/FStreamSupport.h"

#include "Main.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::Service;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define   USE_NOISY_TRACE_IN_THIS_MODULE_       1

namespace {
    // safe to declare here because we cannot start the threads before main...
    const String kServiceRunThreadName_ = "Service 'Run' thread"sv;
}

/*
 ********************************************************************************
 *********************** Service::Main::CommandArgs *****************************
 ********************************************************************************
 */
Main::CommandArgs::CommandArgs (const Sequence<String>& args)
{
    bool didFirst = false;
    for (const String& si : args) {
        if (not didFirst) {
            // skip argv[0]
            // @todo - nice to try args.SubSequence (1)....
            didFirst = true;
            continue;
        }
        static const pair<String, MajorOperation> kPairs_[] = {
            pair<String, MajorOperation>{Main::CommandNames::kInstall, MajorOperation::eInstall},
            pair<String, MajorOperation>{Main::CommandNames::kUnInstall, MajorOperation::eUnInstall},
            pair<String, MajorOperation>{Main::CommandNames::kRunAsService, MajorOperation::eRunServiceMain},
            pair<String, MajorOperation>{Main::CommandNames::kRunDirectly, MajorOperation::eRunDirectly},
            pair<String, MajorOperation>{Main::CommandNames::kStart, MajorOperation::eStart},
            pair<String, MajorOperation>{Main::CommandNames::kStop, MajorOperation::eStop},
            pair<String, MajorOperation>{Main::CommandNames::kForcedStop, MajorOperation::eForcedStop},
            pair<String, MajorOperation>{Main::CommandNames::kRestart, MajorOperation::eRestart},
            pair<String, MajorOperation>{Main::CommandNames::kForcedRestart, MajorOperation::eForcedRestart},
            pair<String, MajorOperation>{Main::CommandNames::kReloadConfiguration, MajorOperation::eReloadConfiguration},
            pair<String, MajorOperation>{Main::CommandNames::kPause, MajorOperation::ePause},
            pair<String, MajorOperation>{Main::CommandNames::kContinue, MajorOperation::eContinue},
        };
        bool found = false;
        for (const auto& i : kPairs_) {
            if (Execution::MatchesCommandLineArgument (si, i.first)) {
                if (found) {
                    Execution::Throw (
                        Execution::InvalidCommandLineArgument{L"Only one major command-line option can be specified at a time"});
                }
                found           = true;
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
bool Main::IApplicationRep::HandleCommandLineArgument (const String& /*s*/)
{
    return false;
}

void Main::IApplicationRep::OnReReadConfigurationRequest ()
{
    //  fMustReReadConfig = true;
}

String Main::IApplicationRep::GetServiceStatusMessage () const
{
    return String{};
}

/*
 ********************************************************************************
 ********************* Main::IServiceIntegrationRep *****************************
 ********************************************************************************
 */
bool Main::IServiceIntegrationRep::HandleCommandLineArgument (const String& s)
{
    return _GetAttachedAppRep ()->HandleCommandLineArgument (s);
}

/*
 ********************************************************************************
 ********************************* Service::Main ********************************
 ********************************************************************************
 */
shared_ptr<Main::IServiceIntegrationRep> Main::mkDefaultServiceIntegrationRep ()
{
#if qPlatform_POSIX
    return make_shared<BasicUNIXServiceImpl> ();
#elif qPlatform_Windows
    return make_shared<WindowsService> ();
#else
    return make_shared<RunNoFrillsService> ();
#endif
}

Main::Main (const shared_ptr<IApplicationRep>& rep, const shared_ptr<IServiceIntegrationRep>& serviceIntegrationRep)
    : fServiceRep_{serviceIntegrationRep}
{
    RequireNotNull (rep);
    RequireNotNull (serviceIntegrationRep);
    serviceIntegrationRep->_Attach (rep);
}

Main::~Main ()
{
    fServiceRep_->_Attach (nullptr);
}

void Main::Run (const CommandArgs& args, const Streams::OutputStream<Characters::Character>::Ptr& out)
{
    for (const String& i : args.fUnusedArguments) {
        fServiceRep_->HandleCommandLineArgument (i);
    }
    if (not args.fMajorOperation.has_value ()) {
        Execution::Throw (Execution::InvalidCommandLineArgument{"No recognized operation"sv});
    }
    switch (*args.fMajorOperation) {
        case CommandArgs::MajorOperation::eInstall: {
            if (out != nullptr) {
                out.Write ("Installing..."sv);
            }
            Install ();
            if (out != nullptr) {
                out.Write ("done\n"sv);
            }
        } break;
        case CommandArgs::MajorOperation::eUnInstall: {
            if (out != nullptr) {
                out.Write ("UnInstalling..."sv);
            }
            UnInstall ();
            if (out != nullptr) {
                out.Write ("done\n"sv);
            }
        } break;
        case CommandArgs::MajorOperation::eRunServiceMain: {
            RunAsService ();
        } break;
        case CommandArgs::MajorOperation::eRunDirectly: {
            optional<Time::Duration> runFor;
            if (args.fUnusedArguments.size () >= 1) {
                runFor = Time::Duration{Characters::FloatConversion::ToFloat<Time::DurationSecondsType> (args.fUnusedArguments[0])};
            }
            RunDirectly (runFor);
        } break;
        case CommandArgs::MajorOperation::eStart: {
            if (out != nullptr) {
                out.Write ("Starting..."sv);
            }
            constexpr Time::DurationSecondsType kTimeOut_{30.0}; // a vaguely reasonable default - apps can override by handling before calling Run
            Start (kTimeOut_);
            if (out != nullptr) {
                out.Write ("done\n"sv);
            }
        } break;
        case CommandArgs::MajorOperation::eStop: {
            if (out != nullptr) {
                out.Write ("Stopping..."sv);
            }
            constexpr Time::DurationSecondsType kTimeOut_{30.0}; // a vaguely reasonable default - apps can override by handling before calling Run
            Stop (kTimeOut_);
            if (out != nullptr) {
                out.Write ("done\n"sv);
            }
        } break;
        case CommandArgs::MajorOperation::eForcedStop: {
            AssertNotImplemented ();
            //ForcedStop ();
        } break;
        case CommandArgs::MajorOperation::eRestart: {
            if (out != nullptr) {
                out.Write ("Restarting..."sv);
            }
            constexpr Time::DurationSecondsType kTimeOut_{30.0}; // a vaguely reasonable default - apps can override by handling before calling Run
            Restart (kTimeOut_);
            if (out != nullptr) {
                out.Write ("done\n"sv);
            }
        } break;
        case CommandArgs::MajorOperation::eForcedRestart: {
            ForcedRestart ();
        } break;
        case CommandArgs::MajorOperation::eReloadConfiguration: {
            ReReadConfiguration ();
        } break;
        case CommandArgs::MajorOperation::ePause: {
            Pause ();
        } break;
        case CommandArgs::MajorOperation::eContinue: {
            Continue ();
        } break;
        default: {
            AssertNotReached ();
        } break;
    }
}

String Main::GetServiceStatusMessage () const
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::GetServiceStatusMessage"};
    const wchar_t             kTAB[] = L"    "; // use spaces instead of tab so formatting independent of tabstop settings
    ServiceDescription        svd    = GetServiceDescription ();
    wstringstream             tmp;
    tmp << L"Service '" << svd.fPrettyName.As<wstring> () << "'" << endl;
    switch (GetState ()) {
        case State::eStopped:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "STOPPED" << endl;
            break;
        case State::eRunning:
            tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "Running" << endl;
            if (GetServiceIntegrationFeatures ().Contains (ServiceIntegrationFeatures::eGetServicePID)) {
                tmp << kTAB << L"PID:    " << kTAB << kTAB << kTAB << kTAB << GetServicePID () << endl;
            }
            break;
        case State::ePaused:
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

void Main::RunAsService ()
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::RunAsService"};
    GetServiceRep_ ()._RunAsService ();
}

void Main::RunDirectly (const optional<Time::Duration>& runFor)
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::RunDirectly"};
    GetServiceRep_ ()._RunDirectly (runFor);
}

void Main::ForcedRestart ([[maybe_unused]] Time::DurationSecondsType timeout, [[maybe_unused]] Time::DurationSecondsType unforcedStopTimeout)
{
    AssertNotImplemented ();
}

void Main::ReReadConfiguration ()
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::ReReadConfiguration"};
#if qPlatform_Windows
    AssertNotImplemented ();
#elif qPlatform_POSIX
    [[maybe_unused]] pid_t pid = GetServicePID ();
    Assert (pid != 0); // maybe throw if non-zero???
    Execution::ThrowPOSIXErrNoIfNegative (::kill (GetServicePID (), Main::BasicUNIXServiceImpl::kSIG_ReReadConfiguration));
#else
    AssertNotImplemented ();
#endif
}

void Main::Pause ()
{
    AssertNotImplemented ();
}

void Main::Continue ()
{
    AssertNotImplemented ();
}

Main::ServiceDescription Main::GetServiceDescription () const
{
    return GetAppRep_ ().GetServiceDescription ();
}

void Main::Restart (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx{L"Stroika::Frameworks::Service::Main::Restart", L"timeout = %e", timeout};

    /////// @TODO FIX - WRONG HANDLING OF TIMEOUT
    Stop (timeout);
    Start (timeout);
#if 0
    Time::DurationSecondsType endAt =   Time::GetTickCount () + timeout;
    IgnoreExceptionsForCall (Stop (timeout));
#if qPlatform_POSIX
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
Main::LoggerServiceWrapper::LoggerServiceWrapper (const shared_ptr<Main::IServiceIntegrationRep>& delegateTo)
    : fDelegateTo_{delegateTo}
{
    RequireNotNull (delegateTo);
}

void Main::LoggerServiceWrapper::_Attach (const shared_ptr<IApplicationRep>& appRep)
{
    fDelegateTo_->_Attach (appRep);
}

shared_ptr<Main::IApplicationRep> Main::LoggerServiceWrapper::_GetAttachedAppRep () const
{
    return fDelegateTo_->_GetAttachedAppRep ();
}

Set<Main::ServiceIntegrationFeatures> Main::LoggerServiceWrapper::_GetSupportedFeatures () const
{
    return fDelegateTo_->_GetSupportedFeatures ();
}

Main::State Main::LoggerServiceWrapper::_GetState () const
{
    return fDelegateTo_->_GetState ();
}

void Main::LoggerServiceWrapper::_Install ()
{
    Logger::sThe.Log (Logger::eNotice, L"Installing service...");
    try {
        fDelegateTo_->_Install ();
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        Logger::sThe.Log (Logger::eError, L"Failed to install - %s - aborting...", exceptMsg.c_str ());
        Execution::ReThrow ();
    }
}

void Main::LoggerServiceWrapper::_UnInstall ()
{
    Logger::sThe.Log (Logger::eNotice, L"UnInstalling service...");
    try {
        fDelegateTo_->_UnInstall ();
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        Logger::sThe.Log (Logger::eError, L"Failed to uninstall - %s - aborting...", exceptMsg.c_str ());
        Execution::ReThrow ();
    }
}

void Main::LoggerServiceWrapper::_RunAsService ()
{
    Logger::sThe.Log (Logger::eInfo, L"Service starting..."); // only info level cuz inside app RunAs
    try {
        fDelegateTo_->_RunAsService ();
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        Logger::sThe.Log (Logger::eError, L"Exception running service - %s - aborting...", exceptMsg.c_str ());
        Execution::ReThrow ();
    }
    Logger::sThe.Log (Logger::eNotice, L"Service stopped normally");
}

void Main::LoggerServiceWrapper::_RunDirectly (const optional<Time::Duration>& runFor)
{
    Logger::sThe.Log (Logger::eNotice, L"Service starting in Run-Direct (non service) mode.");
    try {
        fDelegateTo_->_RunDirectly (runFor);
    }
    catch (...) {
        String exceptMsg = Characters::ToString (current_exception ());
        Logger::sThe.Log (Logger::eError, L"Exception running service service - %s - aborting...", exceptMsg.c_str ());
        Execution::ReThrow ();
    }
    Logger::sThe.Log (Logger::eNotice, L"Service stopped normally");
}

void Main::LoggerServiceWrapper::_Start (Time::DurationSecondsType timeout)
{
    fDelegateTo_->_Start (timeout);
}

void Main::LoggerServiceWrapper::_Stop (Time::DurationSecondsType timeout)
{
    fDelegateTo_->_Stop (timeout);
}

void Main::LoggerServiceWrapper::_ForcedStop (Time::DurationSecondsType timeout)
{
    fDelegateTo_->_ForcedStop (timeout);
}

pid_t Main::LoggerServiceWrapper::_GetServicePID () const
{
    return fDelegateTo_->_GetServicePID ();
}

#if qPlatform_POSIX
/*
 ********************************************************************************
 ******************* Service::Main::BasicUNIXServiceImpl ************************
 ********************************************************************************
 */
Main::BasicUNIXServiceImpl::BasicUNIXServiceImpl ()
    : fOurSignalHandler_{[this] (SignalID signum) { SignalHandler_ (signum); }}
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Main::BasicUNIXServiceImpl::BasicUNIXServiceImpl: this=%p", this);
#endif
}

Main::BasicUNIXServiceImpl::~BasicUNIXServiceImpl ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Main::BasicUNIXServiceImpl::~BasicUNIXServiceImpl: this=%p", this);
#endif
    Require (fAppRep_.load () == nullptr);
}

void Main::BasicUNIXServiceImpl::_Attach (const shared_ptr<IApplicationRep>& appRep)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::_Attach"};
#endif
    Execution::Thread::SuppressInterruptionInContext suppressInterruption; // this must run to completion - it only blocks waiting for subsidiary thread to finish
    Require ((appRep == nullptr and fAppRep_.load () != nullptr) or (fAppRep_.load () == nullptr and fAppRep_.load () != appRep));
    Thread::Ptr p = fRunThread_.load ();
    if (p != nullptr) {
        p.AbortAndWaitForDone ();
        fRunThread_.store (nullptr);
    }
    fAppRep_ = appRep;
}

shared_ptr<Main::IApplicationRep> Main::BasicUNIXServiceImpl::_GetAttachedAppRep () const
{
    return fAppRep_;
}

Set<Main::ServiceIntegrationFeatures> Main::BasicUNIXServiceImpl::_GetSupportedFeatures () const
{
    Set<Main::ServiceIntegrationFeatures> result;
    result.Add (Main::ServiceIntegrationFeatures::eGetServicePID);
    return result;
}

Main::State Main::BasicUNIXServiceImpl::_GetState () const
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::_GetState"};
#endif
    // @todo - maybe not qutie right - but a good approx ... review...
    if (_GetServicePID () > 0) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("State::eRunning");
#endif
        return State::eRunning;
    }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("State::eStopped");
#endif
    return State::eStopped;
}

void Main::BasicUNIXServiceImpl::_Install ()
{
    Execution::Throw (Execution::OperationNotSupportedException{"Install"sv});
}

void Main::BasicUNIXServiceImpl::_UnInstall ()
{
    Execution::Throw (Execution::OperationNotSupportedException{"UnInstall"sv});
}

void Main::BasicUNIXServiceImpl::_RunAsService ()
{
    Debug::TraceContextBumper ctx{"Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::_RunAsService"};
    if (_GetServicePID () > 0) {
        Execution::Throw (Execution::Exception{"Service Already Running"sv});
    }

    shared_ptr<IApplicationRep> appRep = fAppRep_;
    RequireNotNull (appRep);

    [[maybe_unused]] auto&& cleanupSigHanders = Execution::Finally ([this] () noexcept {
        Thread::SuppressInterruptionInContext suppressThreadInterupts;
        SetupSignalHanlders_ (false);
    });
    SetupSignalHanlders_ (true);

    RequireNotNull (appRep); // must call Attach_ first
    fRunThread_.store (Execution::Thread::New ([appRep] () { appRep->MainLoop ([] () {}); }, Execution::Thread::eAutoStart, kServiceRunThreadName_));
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([this] () noexcept {
        Thread::SuppressInterruptionInContext suppressThreadInterupts;
        (void)::unlink (_GetPIDFileName ().c_str ());
    });
    {
        ofstream out;
        Streams::iostream::OpenOutputFileStream (&out, _GetPIDFileName ());
        out << Execution::GetCurrentProcessID () << endl;
    }
    if (_GetServicePID () <= 0) {
        Execution::Throw (Execution::Exception{
            Characters::Format (L"Unable to create process ID tracking file %s", IO::FileSystem::FromPath (_GetPIDFileName ()).c_str ())});
    }
    if (_GetServicePID () != Execution::GetCurrentProcessID ()) {
        Execution::Throw (Execution::Exception{Characters::Format (L"Unable to create process ID tracking file %s (race?)",
                                                                   IO::FileSystem::FromPath (_GetPIDFileName ()).c_str ())});
    }
    fRunThread_.load ().Join ();
}

void Main::BasicUNIXServiceImpl::_RunDirectly (const optional<Time::Duration>& runFor)
{
    shared_ptr<IApplicationRep> appRep = fAppRep_;
    RequireNotNull (appRep); // must call Attach_ first
    fRunThread_.store (Execution::Thread::New ([appRep] () { appRep->MainLoop ([] () {}); }, Execution::Thread::eAutoStart, kServiceRunThreadName_));
    Thread::Ptr        t = fRunThread_.load ();
    Thread::CleanupPtr stopper{Thread::CleanupPtr::eAbortBeforeWaiting}; // another thread to stop the mainloop after runFor
    if (runFor) {
        stopper = Execution::Thread::New (
            [&t, &runFor] () {
                Execution::Sleep (*runFor);
                t.Abort ();
            },
            Execution::Thread::eAutoStart);
    }
    t.Join ();
}

void Main::BasicUNIXServiceImpl::_Start (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx{L"Stroika::Frameworks::Service::Main::Start", L"timeout = %e", timeout};

    Time::DurationSecondsType timeoutAt = Time::GetTickCount () + timeout;

    // REALLY should use GETSTATE - and return state based on if PID file exsits...
    if (_GetServicePID () > 0) {
        Execution::Throw (Execution::Exception{L"Cannot Start service because its already running"sv});
    }

    (void)Execution::DetachedProcessRunner (Execution::GetEXEPath (), Sequence<String>{{String{}, ("--"sv + String{CommandNames::kRunAsService})}});

    while (_GetServicePID () <= 0) {
        Execution::Sleep (500ms);
        Execution::ThrowTimeoutExceptionAfter (timeoutAt);
    }
}

void Main::BasicUNIXServiceImpl::_Stop (Time::DurationSecondsType timeout)
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper traceCtx{L"Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::_Stop", L"timeout=%e", timeout};
#endif
    bool kInProc_ = false;
    if (kInProc_) {
        /// kill running....
    }
    else {
        Time::DurationSecondsType timeoutAt = Time::GetTickCount () + timeout;
        // Send signal to server to stop
        if (_GetServicePID () > 0) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("Service running - so sending SIGTERM signal");
#endif
            Execution::ThrowPOSIXErrNoIfNegative (::kill (_GetServicePID (), SIGTERM));

            Time::DurationSecondsType waitFor = 0.001; // wait just a little at first but then progressively longer (avoid busy wait)
            while (_GetServicePID () > 0) {
                Execution::Sleep (waitFor);
                if (waitFor < timeout and waitFor < 5) {
                    waitFor *= 2;
                }
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("still waiting for timeout/completion");
#endif
                Execution::ThrowTimeoutExceptionAfter (timeoutAt);
            }
        }
        // in case not cleanly stopped before
        // @todo RETHINK - not really necessary and a possible race (if lots of starts/stops done)
        (void)::unlink (_GetPIDFileName ().c_str ());
    }
}

void Main::BasicUNIXServiceImpl::_ForcedStop (Time::DurationSecondsType timeout)
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::_ForcedStop"};
    // Send signal to server to stop
    pid_t svcPID = _GetServicePID ();
    if (svcPID > 0) {
        Execution::ThrowPOSIXErrNoIfNegative (::kill (_GetServicePID (), SIGKILL));
    }
    // REALY should WAIT for server to stop and only do this it fails -
    (void)::unlink (_GetPIDFileName ().c_str ());
}

pid_t Main::BasicUNIXServiceImpl::_GetServicePID () const
{
    ifstream in (_GetPIDFileName ().c_str ());
    if (in) {
        pid_t n = 0;
        in >> n;
        if (Execution::IsProcessRunning (n)) {
            return n;
        }
    }
    return 0;
}

void Main::BasicUNIXServiceImpl::SetupSignalHanlders_ (bool install)
{
    if (install) {
        Execution::SignalHandlerRegistry::Get ().AddSignalHandler (SIGINT, fOurSignalHandler_);
        Execution::SignalHandlerRegistry::Get ().AddSignalHandler (SIGTERM, fOurSignalHandler_);
        Execution::SignalHandlerRegistry::Get ().AddSignalHandler (kSIG_ReReadConfiguration, fOurSignalHandler_);
    }
    else {
        Execution::SignalHandlerRegistry::Get ().RemoveSignalHandler (SIGINT, fOurSignalHandler_);
        Execution::SignalHandlerRegistry::Get ().RemoveSignalHandler (SIGTERM, fOurSignalHandler_);
        Execution::SignalHandlerRegistry::Get ().RemoveSignalHandler (kSIG_ReReadConfiguration, fOurSignalHandler_);
    }
}

filesystem::path Main::BasicUNIXServiceImpl::_GetPIDFileName () const
{
    return IO::FileSystem::WellKnownLocations::GetRuntimeVariableData () /
           IO::FileSystem::ToPath (fAppRep_.load ()->GetServiceDescription ().fRegistrationName + L".pid"sv);
}

void Main::BasicUNIXServiceImpl::_CleanupDeadService ()
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::_CleanupDeadService"};
    // REALY should WAIT for server to stop and only do this it fails -
    (void)::unlink (_GetPIDFileName ().c_str ());
}

void Main::BasicUNIXServiceImpl::SignalHandler_ (SignalID signum)
{
    // NOTE - this is only safe due to the use of SignalHandlerRegistry::SafeSignalsManager
    Debug::TraceContextBumper traceCtx{Stroika_Foundation_Debug_OptionalizeTraceArgs (
        L"Stroika::Frameworks::Service::Main::BasicUNIXServiceImpl::SignalHandler_", L"signal = %s", Execution::SignalToName (signum).c_str ())};
    // VERY PRIMITIVE IMPL FOR NOW -- LGP 2011-09-24
    switch (signum) {
        case SIGINT:
        case SIGTERM: {
            DbgTrace (L"Calling sigHandlerThread2Abort (thread: %s).Abort ()", fRunThread_.load ().ToString ().c_str ());
            fRunThread_.load ().Abort ();
        } break;
        case kSIG_ReReadConfiguration: {
            DbgTrace ("Invoking fAppRep->OnReReadConfigurationRequest ()");
            fAppRep_.load ()->OnReReadConfigurationRequest ();
        } break;
    }
}
#endif

#if qPlatform_Windows
/*
 ********************************************************************************
 ************************* Service::Main::WindowsService ************************
 ********************************************************************************
 */
Main::WindowsService* Main::WindowsService::s_SvcRunningTHIS_ = nullptr;

Main::WindowsService::WindowsService ()
    : fServiceStatus_{}
{
    fServiceStatus_.dwServiceType      = SERVICE_WIN32_OWN_PROCESS;
    fServiceStatus_.dwCurrentState     = SERVICE_STOPPED;
    fServiceStatus_.dwControlsAccepted = SERVICE_ACCEPT_STOP;
}

void Main::WindowsService::_Attach (const shared_ptr<IApplicationRep>& appRep)
{
    Require ((appRep == nullptr and fAppRep_ != nullptr) or (fAppRep_ == nullptr and fAppRep_ != appRep));
    fAppRep_ = appRep;
}

shared_ptr<Main::IApplicationRep> Main::WindowsService::_GetAttachedAppRep () const
{
    return fAppRep_;
}

Set<Main::ServiceIntegrationFeatures> Main::WindowsService::_GetSupportedFeatures () const
{
    Set<Main::ServiceIntegrationFeatures> result;
    result.Add (Main::ServiceIntegrationFeatures::eGetServicePID);
    result.Add (Main::ServiceIntegrationFeatures::eInstall);
    return result;
}

Main::State Main::WindowsService::_GetState () const
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::WindowsService::_GetState"};
    const DWORD               kServiceMgrAccessPrivs = SERVICE_QUERY_STATUS;
    SC_HANDLE                 hSCM                   = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hSCM);
    [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([hSCM] () noexcept {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });
    SC_HANDLE               hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hService);
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([hService] () noexcept {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    const bool kUseQueryServiceStatusEx_ = false;
    if (kUseQueryServiceStatusEx_) {
        SERVICE_STATUS_PROCESS serviceProcess{};
        DWORD                  ignored = 0;
        Execution::Platform::Windows::ThrowIfZeroGetLastError (::QueryServiceStatusEx (
            hService, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE> (&serviceProcess), sizeof (serviceProcess), &ignored));
        switch (serviceProcess.dwCurrentState) {
            case SERVICE_RUNNING:
                return Main::State::eRunning;
            case SERVICE_PAUSED:
                return Main::State::ePaused;
        }
    }
    else {
        SERVICE_STATUS serviceStatus{};
        Execution::Platform::Windows::ThrowIfZeroGetLastError (::QueryServiceStatus (hService, &serviceStatus));
        switch (serviceStatus.dwCurrentState) {
            case SERVICE_RUNNING:
                return Main::State::eRunning;
            case SERVICE_PAUSED:
                return Main::State::ePaused;
        }
    }
    return Main::State::eStopped;
}

void Main::WindowsService::_Install ()
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::WindowsService::_Install"};

    const DWORD kServiceMgrAccessPrivs = SC_MANAGER_CREATE_SERVICE;
    String      cmdLineForRunSvc = L"\""sv + IO::FileSystem::FromPath (Execution::GetEXEPath ()) + L"\" --"sv + CommandNames::kRunAsService;
    SC_HANDLE   hSCM             = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hSCM);
    [[maybe_unused]] auto&& cleanup = Execution::Finally ([hSCM] () noexcept {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    DbgTrace (L"registering with command-line: '%s', serviceName: '%s'", cmdLineForRunSvc.c_str (), GetSvcName_ ().c_str ());
    SC_HANDLE hService = ::CreateService (hSCM, GetSvcName_ ().c_str (), fAppRep_->GetServiceDescription ().fPrettyName.AsSDKString ().c_str (),
                                          kServiceMgrAccessPrivs, SERVICE_WIN32_OWN_PROCESS, SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
                                          cmdLineForRunSvc.AsSDKString ().c_str (), NULL, NULL, _T("RPCSS\0"), NULL, NULL);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hService);
}

void Main::WindowsService::_UnInstall ()
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::WindowsService::_UnInstall"};

    const DWORD kServiceMgrAccessPrivs = SERVICE_STOP | DELETE;
    SC_HANDLE   hSCM                   = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hSCM);
    [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([hSCM] () noexcept {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hService);
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([hService] () noexcept {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    {
        SERVICE_STATUS status;
        if (not ::ControlService (hService, SERVICE_CONTROL_STOP, &status)) {
            DWORD e = ::GetLastError ();
            if (e != ERROR_SERVICE_NOT_ACTIVE) {
                Execution::ThrowSystemErrNo (e);
            }
        }
    }

    Execution::Platform::Windows::ThrowIfZeroGetLastError (::DeleteService (hService));
}

void Main::WindowsService::_RunAsService ()
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::WindowsService::_RunAsService"};
    Assert (s_SvcRunningTHIS_ == nullptr);
    s_SvcRunningTHIS_ = this;

    // MSFT docs unclear on lifetime requirements on these args but for now assume data copied...
    SDKString           svcName = GetSvcName_ ();
    SERVICE_TABLE_ENTRY st[]    = {{const_cast<TCHAR*> (svcName.c_str ()), StaticServiceMain_}, {nullptr, nullptr}};
    if (::StartServiceCtrlDispatcher (st) == FALSE) {
        fServiceStatus_.dwWin32ExitCode = ::GetLastError ();
        if (fServiceStatus_.dwWin32ExitCode == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            DbgTrace ("fServiceStatus_.dwWin32ExitCode = ERROR_FAILED_SERVICE_CONTROLLER_CONNECT");
        }
        Execution::ThrowSystemErrNo (fServiceStatus_.dwWin32ExitCode); // nb: set to getlasterror result above
    }
}

void Main::WindowsService::_RunDirectly (const optional<Time::Duration>& runFor)
{
    shared_ptr<IApplicationRep> appRep = fAppRep_;
    fRunThread_ = Execution::Thread::New ([appRep] () { appRep->MainLoop ([] () {}); }, Execution::Thread::eAutoStart, kServiceRunThreadName_);
    Thread::CleanupPtr stopper{Thread::CleanupPtr::eAbortBeforeWaiting}; // another thread to stop the mainloop after runFor
    if (runFor) {
        stopper = Execution::Thread::New (
            [&] () {
                Execution::Sleep (*runFor);
                fRunThread_.Abort ();
            },
            Execution::Thread::eAutoStart);
    }
    fRunThread_.Join ();
}

void Main::WindowsService::_Start (Time::DurationSecondsType timeout)
{
    // @todo - timeout not supported
    Debug::TraceContextBumper traceCtx{L"Stroika::Frameworks::Service::Main::WindowsService::Start", L"timeout = %e", timeout};

    const DWORD kServiceMgrAccessPrivs = SERVICE_START;
    SC_HANDLE   hSCM                   = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hSCM);
    [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([hSCM] () noexcept {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });
    SC_HANDLE               hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hService);
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([hService] () noexcept {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    DWORD    dwNumServiceArgs    = 0;
    LPCTSTR* lpServiceArgVectors = nullptr;
    Execution::Platform::Windows::ThrowIfZeroGetLastError (::StartService (hService, dwNumServiceArgs, lpServiceArgVectors));
}

void Main::WindowsService::_Stop ([[maybe_unused]] Time::DurationSecondsType timeout)
{
    // @todo - timeout not supported
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::WindowsService::_Stop"};
    const DWORD               kServiceMgrAccessPrivs = SERVICE_STOP;
    SC_HANDLE                 hSCM                   = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hSCM);
    [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([hSCM] () noexcept {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hService);
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([hService] () noexcept {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    {
        SERVICE_STATUS status;
        if (not ::ControlService (hService, SERVICE_CONTROL_STOP, &status)) {
            DWORD e = ::GetLastError ();
            if (e != ERROR_SERVICE_NOT_ACTIVE) {
                Execution::ThrowSystemErrNo (e);
            }
        }
    }
}

void Main::WindowsService::_ForcedStop ([[maybe_unused]] Time::DurationSecondsType timeout)
{
    AssertNotImplemented ();
}

pid_t Main::WindowsService::_GetServicePID () const
{
    const DWORD kServiceMgrAccessPrivs = SERVICE_QUERY_STATUS;
    SC_HANDLE   hSCM                   = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hSCM);
    [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([hSCM] () noexcept {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });
    SC_HANDLE               hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hService);
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([hService] () noexcept {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });

    SERVICE_STATUS_PROCESS serviceProcess{};
    DWORD                  ignored = 0;
    Execution::Platform::Windows::ThrowIfZeroGetLastError (
        ::QueryServiceStatusEx (hService, SC_STATUS_PROCESS_INFO, reinterpret_cast<LPBYTE> (&serviceProcess), sizeof (serviceProcess), &ignored));
    return serviceProcess.dwProcessId;
}

SDKString Main::WindowsService::GetSvcName_ () const
{
    RequireNotNull (fAppRep_); // must attach first
    return fAppRep_->GetServiceDescription ().fRegistrationName.AsSDKString ();
}

bool Main::WindowsService::IsInstalled_ () const noexcept
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::WindowsService::IsInstalled_"};
    const DWORD               kServiceMgrAccessPrivs = SERVICE_QUERY_CONFIG;
    SC_HANDLE                 hSCM                   = ::OpenSCManager (NULL, NULL, kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hSCM);
    [[maybe_unused]] auto&& cleanup1 = Execution::Finally ([hSCM] () noexcept {
        AssertNotNull (hSCM);
        ::CloseServiceHandle (hSCM);
    });

    SC_HANDLE hService = ::OpenService (hSCM, GetSvcName_ ().c_str (), kServiceMgrAccessPrivs);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (hService);
    [[maybe_unused]] auto&& cleanup2 = Execution::Finally ([hService] () noexcept {
        AssertNotNull (hService);
        ::CloseServiceHandle (hService);
    });
    return hService != NULL;
}

void Main::WindowsService::SetServiceStatus_ (DWORD dwState) noexcept
{
    DbgTrace ("SetServiceStatus_ (%d)", dwState);
    Assert (fServiceStatusHandle_ != nullptr);
    fServiceStatus_.dwCurrentState = dwState;
    ::SetServiceStatus (fServiceStatusHandle_, &fServiceStatus_);
}

void Main::WindowsService::ServiceMain_ ([[maybe_unused]] DWORD dwArgc, [[maybe_unused]] LPTSTR* lpszArgv) noexcept
{
    Debug::TraceContextBumper traceCtx{"Stroika::Frameworks::Service::Main::WindowsService::ServiceMain_"};
    ///@TODO - FIXUP EXCEPTION HANLDING HERE!!!

    // do file create stuff here
    //Logger::sThe.Log (Logger::eInfo, L"entering ServiceMain_");

    // Register the control request handler
    fServiceStatus_.dwCurrentState = SERVICE_START_PENDING;
    fServiceStatusHandle_          = ::RegisterServiceCtrlHandler (GetSvcName_ ().c_str (), StaticHandler_);
    Execution::Platform::Windows::ThrowIfZeroGetLastError (fServiceStatusHandle_);
    SetServiceStatus_ (SERVICE_START_PENDING);

    fServiceStatus_.dwWin32ExitCode = S_OK;
    fServiceStatus_.dwCheckPoint    = 0;
    fServiceStatus_.dwWaitHint      = 0;

    // When the Run function returns, the service has stopped.
    // about like this - FIX - KEEP SOMETHING SIMIALR
    shared_ptr<IApplicationRep> appRep = fAppRep_;
    fRunThread_ = Execution::Thread::New ([appRep] () { appRep->MainLoop ([] () {}); }, Execution::Thread::eAutoStart, kServiceRunThreadName_);
    //Logger::sThe.Log (Logger::eInfo, L"in ServiceMain_ about to set SERVICE_RUNNING");
    SetServiceStatus_ (SERVICE_RUNNING);

    try {
        fRunThread_.Join ();
    }
    catch (...) {
        DbgTrace (L"mapping run-thread.Join () exception %s to dwWin32ExitCode=1", Characters::ToString (current_exception ()).c_str ());
        fServiceStatus_.dwWin32ExitCode = 1; // some non-zero exit code
    }
    SetServiceStatus_ (SERVICE_STOPPED);
}

void WINAPI Main::WindowsService::StaticServiceMain_ (DWORD dwArgc, LPTSTR* lpszArgv) noexcept
{
    AssertNotNull (s_SvcRunningTHIS_);
    s_SvcRunningTHIS_->ServiceMain_ (dwArgc, lpszArgv);
}

void Main::WindowsService::OnStopRequest_ () noexcept
{
    /*
     *  WARNING - this maybe a race about setting status!!! - what if we get stop request when already stopped.
     *  THIS CODE NEEDS THREAD LOCKS!!!
     */
    if (fServiceStatus_.dwCurrentState == SERVICE_RUNNING) {
        SetServiceStatus_ (SERVICE_STOP_PENDING);
        fRunThread_.Abort ();
    }
}

void Main::WindowsService::Handler_ (DWORD dwOpcode) noexcept
{
    switch (dwOpcode) {
        case SERVICE_CONTROL_STOP:
            OnStopRequest_ ();
            break;
        default:
            // others ignored for now
            break;
    }
}

void WINAPI Main::WindowsService::StaticHandler_ (DWORD dwOpcode) noexcept
{
    AssertNotNull (s_SvcRunningTHIS_);
    s_SvcRunningTHIS_->Handler_ (dwOpcode);
}
#endif
