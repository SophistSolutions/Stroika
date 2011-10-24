/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<map>

#include	"../Characters/Format.h"
#include	"../Debug/Trace.h"
#include	"CriticalSection.h"

#include	"Signals.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;



namespace	{
	CriticalSection	sCritSection_;

	map<SignalIDType,set<SignalHandlerType>>	sHandlers_;

	bool	IsSigIgnore_ (const set<SignalHandlerType>& sigSet)
		{
			return sigSet.size () == 1 and *sigSet.begin () == SignalHandlerRegistry::kIGNORED;
		}

	void	MyHandler_ (int signal)
		{
			Debug::TraceContextBumper trcCtx (TSTR ("Stroika::Foundation::Execution::Signals::{}::MyHandler_"));
			DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
			set<SignalHandlerType>	handlers;
			{
				AutoCriticalSection critSec (sCritSection_);
				map<SignalIDType,set<SignalHandlerType>>::const_iterator i = sHandlers_.find (signal);
				Assert (i != sHandlers_.end ());
				handlers = i->second;
			}
			for (set<SignalHandlerType>::const_iterator i = handlers.begin (); i != handlers.end (); ++i) {
				if (*i != SignalHandlerRegistry::kIGNORED) {
					(*i) (signal);
				}
			}
		}
}





/*
 ********************************************************************************
 ******************** Execution::SignalHandlerRegistry **************************
 ********************************************************************************
 */

const	SignalHandlerType	SignalHandlerRegistry::kIGNORED	=	SIG_IGN;

SignalHandlerRegistry&	SignalHandlerRegistry::Get ()
{
	static	SignalHandlerRegistry	sThe_;
	return sThe_;
}

SignalHandlerRegistry::SignalHandlerRegistry ()
{
}

set<SignalIDType>	SignalHandlerRegistry::GetHandledSignals () const
{
	set<SignalIDType>	result;
	{
		AutoCriticalSection critSec (sCritSection_);
		for (map<SignalIDType,set<SignalHandlerType>>::const_iterator i = sHandlers_.begin (); i != sHandlers_.end (); ++i) {
			result.insert (i->first);
		}
	}
	return result;
}

set<SignalHandlerType>	SignalHandlerRegistry::GetSignalHandlers (SignalIDType signal) const
{
	AutoCriticalSection critSec (sCritSection_);
	map<SignalIDType,set<SignalHandlerType>>::const_iterator i = sHandlers_.find (signal);
	if (i == sHandlers_.end ()) {
		return set<SignalHandlerType> ();
	}
	else {
		return i->second;
	}
}

void	SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal)
{
	SetSignalHandlers (signal, set<SignalHandlerType> ());
}

void	SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, SignalHandlerType handler)
{
	SetSignalHandlers (signal, set<SignalHandlerType> (&handler, &handler + 1));
}

void	SignalHandlerRegistry::SetSignalHandlers (SignalIDType signal, const set<SignalHandlerType>& handlers)
{
	Debug::TraceContextBumper trcCtx (TSTR ("Stroika::Foundation::Execution::Signals::{}::SetSignalHandlers"));
	DbgTrace (L"(signal = %s, handlers.size () = %d, ....)", SignalToName (signal).c_str (), handlers.size ());
	AutoCriticalSection critSec (sCritSection_);
	map<SignalIDType,set<SignalHandlerType>>::iterator i = sHandlers_.find (signal);
	if (i == sHandlers_.end ()) {
		if (not handlers.empty ()) {
			sHandlers_.insert (map<SignalIDType,set<SignalHandlerType>>::value_type (signal, handlers));
		}
	}
	else {
		i->second = handlers;
	}
	if (handlers.empty ()) {
		// nothing todo - empty list treated as not in sHandlers_ list
		(void)::signal (signal, SIG_DFL);
	}
	else if (IsSigIgnore_ (handlers)) {
		(void)::signal (signal, SIG_IGN);
	}
	else {
		(void)::signal (signal, MyHandler_);
	}
}

void	SignalHandlerRegistry::AddSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
	set<SignalHandlerType>	s	=	GetSignalHandlers (signal);
	s.insert (handler);
	SetSignalHandlers (signal, s);
}

void	SignalHandlerRegistry::RemoveSignalHandler (SignalIDType signal, SignalHandlerType handler)
{
	set<SignalHandlerType>	s	=	GetSignalHandlers (signal);
	Require (s.find (handler) != s.end ());
	s.erase (handler);
	SetSignalHandlers (signal, s);
}








/*
 ********************************************************************************
 ************************** Execution::SignalToName *****************************
 ********************************************************************************
 */
wstring	Execution::SignalToName (SignalIDType signal)
{
	switch (signal) {
		case	SIGINT:		return L"SIGINT";
		case	SIGILL:		return L"SIGILL";
		case	SIGSEGV:	return L"SIGSEGV";
		case	SIGABRT:	return L"SIGABRT";
	#if		defined (SIGUSR1)
		case	SIGUSR1:	return L"SIGUSR1";
	#endif
	#if		defined (SIGUSR2)
		case	SIGUSR2:	return L"SIGUSR2";
	#endif
	#if		defined (SIGHUP)
		case	SIGHUP:		return L"SIGHUP";
	#endif
		case	SIGTERM:	return L"SIGTERM";
		default:			return Characters::Format (L"Signal# %d", signal);
	}
}






/*
 ********************************************************************************
 **************************** Execution::SendSignal *****************************
 ********************************************************************************
 */
void	Execution::SendSignal (Thread::NativeHandleType h, SignalIDType signal)
{
	Debug::TraceContextBumper trcCtx (TSTR ("Stroika::Foundation::Execution::Signals::Execution::SendSignal"));
	DbgTrace (L"(signal = %s)", SignalToName (signal).c_str ());
	#if		qPlatform_POSIX
		Verify (pthread_kill (h, signal) == 0);
	#else
		AssertNotImplemented ();
	#endif
}
