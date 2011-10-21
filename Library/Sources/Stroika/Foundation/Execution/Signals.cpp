/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<map>

#include	"../Debug/Trace.h"
#include	"CriticalSection.h"

#include	"Signals.h"



using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;



namespace	{
	CriticalSection	sCritSection_;

	bool										sInstalled_		=	false;
	map<SignalIDType,set<SignalHandlerType>>	sHandlers_;

	bool	IsSigIgnore_ (const set<SignalHandlerType>& sigSet)
		{
			return sigSet.size () == 1 and *sigSet.begin () == SignalHandlerRegistry::kIGNORED;
		}

	void	MyHandler_ (int signal)
		{
			Debug::TraceContextBumper trcCtx (_T ("Stroika::Foundation::Execution::Signals::{}::MyHandler_"));
			DbgTrace ("(signal = %d)", signal);
			set<SignalHandlerType>	handlers;
			{
				AutoCriticalSection critSec (sCritSection_);
				map<SignalIDType,set<SignalHandlerType>>::const_iterator i = sHandlers_.find (signal);
				Assert (i != sHandlers_.end ());
				handlers = i->second;
			}
			for (set<SignalHandlerType>::const_iterator i = handlers.begin (); i != handlers.end (); ++i) {
				(*i) (signal);
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

void	SignalHandlerRegistry::Install ()
{
	Debug::TraceContextBumper trcCtx (_T ("SignalHandlerRegistry::Install"));
	AutoCriticalSection critSec (sCritSection_);
	Require (not sInstalled_);
	sInstalled_ = true;
	for (map<SignalIDType,set<SignalHandlerType>>::const_iterator i = sHandlers_.begin (); i != sHandlers_.end (); ++i) {
		if (IsSigIgnore_ (i->second)) {
			signal (i->first, SIG_IGN);
		}
		else {
			signal (i->first, MyHandler_);
		}
	}
}

void	SignalHandlerRegistry::Uninstall ()
{
	Debug::TraceContextBumper trcCtx (_T ("SignalHandlerRegistry::Uninstall"));
	AutoCriticalSection critSec (sCritSection_);
	Require (sInstalled_);
	sInstalled_ = false;
	for (map<SignalIDType,set<SignalHandlerType>>::const_iterator i = sHandlers_.begin (); i != sHandlers_.end (); ++i) {
		signal (i->first, SIG_DFL);
	}
}

bool	SignalHandlerRegistry::Installed () const
{
	return sInstalled_;
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
	Debug::TraceContextBumper trcCtx (_T ("Stroika::Foundation::Execution::Signals::{}::MyHandler_"));
	DbgTrace ("(signal = %d, ....)", signal);
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
	if (sInstalled_) {
		if (handlers.empty ()) {
			// nothing todo - empty list treated as not in sHandlers_ list
			::signal (signal, SIG_DFL);
		}
		else if (IsSigIgnore_ (handlers)) {
			::signal (signal, SIG_IGN);
		}
		else {
			::signal (signal, MyHandler_);
		}
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
