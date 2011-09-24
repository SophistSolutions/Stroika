/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cstdlib>

#if		qPlatform_POSIX
	#include	<sys/types.h>
	#include	<unistd.h>
#endif

#include	"../../Foundation/Containers/Common.h"
#include	"../../Foundation/DataExchangeFormat/BadFormatException.h"
#include	"../../Foundation/Debug/Assertions.h"
#include	"../../Foundation/Debug/Trace.h"
#include	"../../Foundation/Execution/CommandLine.h"
#include	"../../Foundation/Execution/Exceptions.h"
#include	"../../Foundation/Execution/Module.h"
#include	"../../Foundation/Execution/ThreadAbortException.h"
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
	// default to using thread stuff to send us a signal to abort...
}

void	Main::IRep::OnReReadConfigurationRequest ()
{
}










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

Main::Main (Memory::SharedPtr<IRep> rep)
	: _fRep (rep)
{
}

void	Main::RunAsService ()
{
	try {
		_fRep->OnStartRequest ();
	}
	catch (const Execution::ThreadAbortException& /*threadAbort*/) {
		// ignore this - just means service ended normally
	}
	catch (...) {
		DbgTrace (TSTR ("Unexpected exception ended running service"));
		throw;
	}
}

void	Main::Start ()
{
	// Check not already runnig, (someday) and then for and exec the 

	Characters::TString	thisEXEPath	=	Execution::GetEXEPath ();
#if		qPlatform_POSIX
	pid_t	pid	=	fork ();
	if (pid == 0) {
		// Child - exec
		int	r	=	execl (thisEXEPath.c_str (), thisEXEPath.c_str (), String (CommandNames::kRunAsService).AsTString ().c_str (), nullptr);
		exit (-1);
	}
	else if (pid < 0) {
		// failed to fork - serious error
		Execution::ThrowIfError_errno_t ();
		Assert (false);	// errno SB set!!!
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
	// Send signal to server to stop
}

void	Main::Restart ()
{
	IgnoreExceptionsForCall (Stop ());
	Start ();
}

void	Main::ReReadConfiguration ()
{
	// SEND APPROPRIATE SIGNAL
}

Main::ServiceDescription	Main::GetServiceDescription () const
{
	return _fRep->GetServiceDescription ();
}

bool	Main::_HandleStandardCommandLineArgument (const String& arg)
{
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
		AssertNotImplemented ();
		return true;
	}
	else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kRestart)) {
		AssertNotImplemented ();
		return true;
	}
	else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kReloadConfiguration)) {
		AssertNotImplemented ();
		return true;
	}
	else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kPause)) {
		AssertNotImplemented ();
		return true;
	}
	else if (Execution::MatchesCommandLineArgument (arg, CommandNames::kContinue)) {
		AssertNotImplemented ();
		return true;
	}
	/// MANY more neeeded, and fix to use named constants...
	return false;
}

