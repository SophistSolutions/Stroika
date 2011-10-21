/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Signals_h_
#define	_Stroika_Foundation_Execution_Signals_h_	1

#include	"../StroikaPreComp.h"

#include	<csignal>
#include	<set>

#include	"../Configuration/Common.h"

/*
 * Description:
 *
 *		This module defines support for POSIX (and std c++ defined) Signals (not to be confused with the 'Signals and slots'
 *	design pattern which is largely unrelated).
 *	
 *
 */


/*
 * TODO:
 *		o	THINK OUT AND DESCRIBE IN DETAIL HOW WE HANDLE THREADS
 *		o	Do overload (or some such) for (sa_sigaction)(int, siginfo_t *, void *); Allow these to be (more or less) interchangable with
 *			regular SignalHandlerType.
 *
 */

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			typedef	int		SignalIDType;
			typedef	void	(*SignalHandlerType) (SignalIDType);

			/*
			 *	Description:
			 *		SignalHandlerRegistry is a singleton object. If used - it itself registers signal handlers for each supported signal.
			 *
			 *		The user can then add in their own 'handlers' for those signals, and they are ALL called - one after the other (TDB how threads
			 *		work with this).
			 *
			 *		When an platform signal-handler is installed (via 'sigaction' for example) - and then later UNINSTALLED (due to Uninstall or
			 *		changes in GetHandledSignals - this code resets the signal handler to SIG_DFL (not the previous value).
			 *
			 *		This object is thread-safe.
			 */
			class	SignalHandlerRegistry {
				public:
					/*
					 * If this handler is set to the the ONLY handler for a given signal, then that signal handler is effectively ignored.
					 *
					 * To get the signal to be handled the DEFAULT way - remove all signal handlers.
					 */
					static	const	SignalHandlerType	kIGNORED;
				
				public:
					// Access singleton implementation. None exists until this is called.
					static	SignalHandlerRegistry&	Get ();

				private:
					SignalHandlerRegistry ();
					NO_COPY_CONSTRUCTOR (SignalHandlerRegistry);
					NO_ASSIGNMENT_OPERATOR (SignalHandlerRegistry);

				public:
					/*
					 * Because this mechanism can 'interfere' with other application of the POSIX signal API, use requires you
					 * explicitly call Install (or uninstall).
					 *
					 * This ONLY installs handlers for the signals returned from GetHandledSignals ().
					 *
					 * Illegal to call if already Installed ().
					 */
					nonvirtual	void	Install ();
					/*
					 * See Install (). Illegal to call if not Installed ();
					 */
					nonvirtual	void	Uninstall ();

					/*
					 * Returns true if Installed has been called more times than Uninstall ();
					 */
					nonvirtual	bool	Installed () const;

				public:
					/*
					 * Returns the set of signals trapped by the SignalHandlerRegistry registry. Note - if not 'Installed ()' - these 
					 * are tracked internally by Stroika but not actually installed in the OS.
					 */
					nonvirtual	set<SignalIDType>	GetHandledSignals () const;

				public:
					/*
					 * Returns the set of signals trapped by the SignalHandlerRegistry registry. This doesn't imply there is a handler.
					 * NB: A signal handler must be registered for a given signal number AND the signal number must be in GetHandledSignals () AND
					 * the SignalHandlerRegistry must be Installed () - to get the signal called.
					 *
					 * It is NOT an error to have a signal handler registered for a signal not in the set of GetHandledSignals () - or vice versa.
					 * Signals in the list of GetHandledSignals() with no handlers are effectively ignored.
					 */
					nonvirtual	set<SignalHandlerType>	GetSignalHandlers (SignalIDType signal) const;
					/*
					 * See GetSignalHandlers().
					 *
					 * SetSignalHandlers () with NO arguments uninstalls all Stroika signal handlers for this signal.
					 * SetSignalHandlers () with ONE argument makes Stroika take-over the signal handling - and sets the set of hanlders to be
					 * exactly the one given.
					 * SetSignalHandlers () with ONE a set of handlers registers all the givne handlers.
					 *
					 * Note - if through ANY combination of set/add/remvoe - you have NO signal handler - this reverts to SIG_DFL, and if you have
					 * exactly ONE signal handler - and its kIGNORED- the signal will be ignored.
					 */
					nonvirtual	void					SetSignalHandlers (SignalIDType signal);
					nonvirtual	void					SetSignalHandlers (SignalIDType signal, SignalHandlerType handler);
					nonvirtual	void					SetSignalHandlers (SignalIDType signal, const set<SignalHandlerType>& handlers);
					/*
					 * See GetSignalHandlers()
					 */
					nonvirtual	void					AddSignalHandler (SignalIDType signal, SignalHandlerType handler);
					/*
					 * See GetSignalHandlers()
					 */
					nonvirtual	void					RemoveSignalHandler (SignalIDType signal, SignalHandlerType handler);
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Signals_h_*/


