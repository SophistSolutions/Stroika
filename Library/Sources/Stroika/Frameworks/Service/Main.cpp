/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>
#include	<fstream>
#include	<sstream>

#if		qPlatform_POSIX
	#include	<sys/types.h>
	#include	<unistd.h>
	#include	<sys/stat.h>
	#include	<fcntl.h>
#endif

#include	"../../Foundation/Characters/Format.h"
#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Debug/Trace.h"
#include	"../../Foundation/Execution/CommandLine.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Execution/ErrNoException.h"
#include	"../../Foundation/Execution/Module.h"
#include	"../../Foundation/Execution/ThreadAbortException.h"
#include	"../../Foundation/Execution/Signals.h"
#include	"../../Foundation/IO/FileSystem/FileUtils.h"
#include	"../../Foundation/Memory/SmallStackBuffer.h"

#include	"Main.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Containers;
using	namespace	Stroika::Foundation::Memory;

using	namespace	Stroika::Frameworks;
using	namespace	Stroika::Frameworks::Service;








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

void	Main::IRep::OnStartRequest ()
{
	// This procedure ends when the entire service process ends...
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::IRep::OnStartRequest"));
	MainLoop ();
}

void	Main::IRep::OnStopRequest ()
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::IRep::OnStopRequest"));
	// default to using thread stuff to send us a signal to abort...
	fStopping_ = true;
}

void	Main::IRep::OnReReadConfigurationRequest ()
{
	fMustReReadConfig = true;
}

String	Main::IRep::GetServiceStatusMessage () const
{
	return String (); 
}

#if		qPlatform_POSIX
String	Main::IRep::GetPIDFileName () const
{
	return L"/tmp/" + GetServiceDescription ().fName + L".pid";
}
#endif

#if		qPlatform_POSIX
void	Main::IRep::SignalHandler (int signum)
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::IRep::SignalHandler"));
	DbgTrace (L"Signal #%d", signum);
	// VERY PRIMITIVE IMPL FOR NOW -- LGP 2011-09-24
	switch (signum) {
		case	SIGTERM:
			fStopping_ = true;
			break;
#if		qCompilerAndStdLib_Supports_constexpr
		case	kSIG_ReReadConfiguration:
#else
		case	SIGHUP:
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

const	wchar_t	Service::Main::CommandNames::kRunAsService[]		=	L"Run-As-Service";
const	wchar_t	Service::Main::CommandNames::kStart[]				=	L"Start";
const	wchar_t	Service::Main::CommandNames::kStop[]				=	L"Stop";
const	wchar_t	Service::Main::CommandNames::kKill[]				=	L"Kill";
const	wchar_t	Service::Main::CommandNames::kRestart[]				=	L"Restart";
const	wchar_t	Service::Main::CommandNames::kReloadConfiguration[]	=	L"Reload-Configuration";
const	wchar_t	Service::Main::CommandNames::kPause[]				=	L"Pause";
const	wchar_t	Service::Main::CommandNames::kContinue[]			=	L"Continue";

Memory::SharedPtr<Main::IRep>	Main::_sRep;

Main::Main (Memory::SharedPtr<IRep> rep)
{
	Ensure  (_sRep.IsNull ());
	_sRep = rep;
	#if		qPlatform_POSIX
		SetupSignalHanlders_ ();
	#endif
}



#if		qPlatform_POSIX
void	Main::SetupSignalHanlders_ ()
{
	Execution::SignalHandlerRegistry::Get ().AddSignalHandler (SIGTERM, SignalHandler);
	Execution::SignalHandlerRegistry::Get ().AddSignalHandler (kSIG_ReReadConfiguration, SignalHandler);
}
#endif

Main::State	Main::GetState () const
{
	#if		qPlatform_POSIX
	if (GetServicePID () != 0) {
		return eRunning;
	}
	#endif
	return eStopped;	// otherwise (esp on other platforms where not implemented) must  be stopped
}

#if		qPlatform_POSIX
pid_t	Main::GetServicePID () const
{
	ifstream	in (_sRep->GetPIDFileName ().AsTString ().c_str ());
	if (in) {
		pid_t	n = 0;
		in >> n;
		return n;
	}
	return 0;
}
#endif

String		Main::GetServiceStatusMessage () const
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::GetServiceStatusMessage"));
	const	wchar_t	kTAB[]	=	L"    ";	// use spaces instead of tab so formatting independent of tabstop settings
	ServiceDescription	svd	=	GetServiceDescription ();
	wstringstream	tmp;
	tmp << L"Service '" << svd.fName.As<wstring> () << "'" << endl;
	switch (this->GetState ()) {
		case	eStopped:	
			tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "STOPPED" << endl;
			break;
		case	eRunning:	
			tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "Running" << endl; 
			#if		qPlatform_POSIX
				tmp << kTAB << L"PID:    " << kTAB << kTAB << kTAB << kTAB << GetServicePID () << endl;
			#endif
			break;
		case	ePaused:	
			tmp << kTAB << L"State:  " << kTAB << kTAB << kTAB << kTAB << "PAUSED" << endl; 
			#if		qPlatform_POSIX
				tmp << kTAB << L"PID:    " << kTAB<< kTAB << kTAB << kTAB << GetServicePID () << endl;
			#endif
			break;
		default:
			AssertNotReached ();
	}
	tmp << _sRep->GetServiceStatusMessage ().As<wstring> ();
	DbgTrace (L"returning status: (%s)", tmp.str ().c_str ());
	return tmp.str ();
}

void	Main::RunAsService ()
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::RunAsService"));
	// VERY PRIMITIVE IMPL - WE NEED LOCKING on tmpfile stuff - add good tempfile supprot to fileuitls or use existing...

	try {
#if		qPlatform_POSIX
		ofstream	out (_sRep->GetPIDFileName ().AsTString ().c_str ());
		out << getpid () << endl;
#endif
		_sRep->OnStartRequest ();
	}
	catch (const Execution::ThreadAbortException& /*threadAbort*/) {
#if		qPlatform_POSIX
		unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
		// ignore this - just means service ended normally
	}
	catch (...) {
		DbgTrace (TSTR ("Unexpected exception ended running service"));
#if		qPlatform_POSIX
		unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
		throw;
	}
}

void	Main::Start ()
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Start"));
	// Check not already runnig, (someday) and then for and exec the 

#if		qPlatform_POSIX
	// REALLY should use GETSTATE - and return state based on if PID file exsits...
	if (GetServicePID ()  != 0) {
		Execution::DoThrow (Execution::StringException (L"Cannot Start service because its already running"));
	}
#endif
	Characters::TString	thisEXEPath	=	Execution::GetEXEPath ();
#if		qPlatform_POSIX
	pid_t	pid	=	fork ();
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

		int	r	=	execl (thisEXEPath.c_str (), thisEXEPath.c_str (), (String (L"--") + String (CommandNames::kRunAsService)).AsTString ().c_str (), nullptr);
		exit (-1);
	}
	else {
		// parent - in this case - no reason to wait - our work is done... Future versions might wait to
		// see if the 'pidfile' got created....
		//		--LGP 2011-09-23
	}
#endif
}

void	Main::Stop ()
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Stop"));
	// Send signal to server to stop
#if		qPlatform_POSIX
	Execution::ThrowErrNoIfNegative (kill (GetServicePID (), SIGTERM));
#endif
}

void	Main::Kill ()
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Kill"));
	Stop ();
	// Send signal to server to stop
#if		qPlatform_POSIX
	Execution::ThrowErrNoIfNegative (kill (GetServicePID (), SIGKILL));
	// REALY should WAIT for server to stop and only do this it fails - 
	unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
}

void	Main::Restart ()
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::Restart"));
	IgnoreExceptionsForCall (Stop ());
#if		qPlatform_POSIX
	// REALY should WAIT for server to stop and only do this it fails - 
	unlink (_sRep->GetPIDFileName ().AsTString ().c_str ());
#endif
	Start ();
}

void	Main::ReReadConfiguration ()
{
	Debug::TraceContextBumper traceCtx (TSTR ("Stroika::Frameworks::Service::Main::ReReadConfiguration"));
	// SEND APPROPRIATE SIGNAL
#if		qPlatform_POSIX
	pid_t	pid	=	GetServicePID ();
	Assert (pid != 0);	// maybe throw if non-zero???
	Execution::ThrowErrNoIfNegative (kill (GetServicePID (), kSIG_ReReadConfiguration));
#endif
}

void	Main::Pause ()
{
	AssertNotImplemented ();
}

void	Main::Continue ()
{
	AssertNotImplemented ();
}

Main::ServiceDescription	Main::GetServiceDescription () const
{
	return _sRep->GetServiceDescription ();
}

#if		qPlatform_POSIX
void	Main::SignalHandler (int signum)
{
	_sRep->SignalHandler (signum);
}
#endif

bool	Main::_HandleStandardCommandLineArgument (const String& arg)
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

