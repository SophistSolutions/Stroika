/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Frameworks_Service_Main_h_
#define	_Stroika_Frameworks_Service_Main_h_	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<vector>

#include	"../../Foundation/Characters/String.h"
#include	"../../Foundation/Configuration/Common.h"
#include	"../../Foundation/Memory/SharedPtr.h"


/*
 * TODO:
 *		Very early stage draft.
 *
 *		(o)	First priority is getting this working on UNIX.
 *
 *		(o)	Though UNIX/Windows are differnt - empahize similaries for how the both work.
 *
 *		(o)	For UNIX
 *
 *			(o)	Store in file system file with current PID.
 *
 *			(o)	Start/Stop work by sending SIGNALS
 *
 *			(o)	Must have generic signal handler registered (at least for TERMINATE/STOP/CONTINUE, SIGNIT (re-read conf))
 *
 *			(o)	
 *
 *		(o)	Later do Windoze implementation - supproting the richer set of control mechanism.
 */

namespace	Stroika {	
	namespace	Frameworks {
		namespace	Service {

			using	namespace	Stroika::Foundation;
			using	Characters::String;

			// very very rough draft - 
			class	Main {
				public:
					struct	ServiceDescription {
						String	fName;
					};
				public:
					/*
					 * To use this class you must implement your own Rep (to represent the running service).
					 *
					 *	MainLoop () is automatically setup to run on its own thread. Betware, the OnXXX events maybe called on this object, but from any thread
					 *	so be careful of thread safety!
					 */
					class	IRep {
						public:
							virtual ~IRep ();

							// This should be overridden by each service, and should  never return until the service is done (stop request).
							virtual	void				MainLoop () = 0;

							virtual	void				OnStartRequest ();
							virtual	void				OnStopRequest ();
							virtual	void				OnReReadConfigurationRequest ();
							virtual	ServiceDescription	GetServiceDescription () const = 0;
					};
				public:
					Main (Memory::SharedPtr<IRep> rep);

				protected:
					Memory::SharedPtr<IRep>	_fRep;

				public:
					/*
					 *	Checks the state of the given service. NOTE - this works ACROSS PROCESSES. It can be called to ask in a controller exe
					 *	if the serviceMain EXE is running. It also - COULD give the wrong answer - given races, so use with care.
					 */
					enum	State { 
						eStopped,
						eRunning,
						ePaused,		// STOPPED in unix
					};
					nonvirtual	State				GetState () const;

				public:
					/*
					 *	RunAsService () will not return - until the service has terminated. It runs the service 'MainLoop'.
					 */
					nonvirtual	void				RunAsService ();
					/*
					 */
					nonvirtual	void				Start ();
					/*
					 */
					nonvirtual	void				Stop ();
					/*
					 */
					nonvirtual	void				Restart ();
					/*
					 */
					nonvirtual	void				ReReadConfiguration ();
					/*
					 */
					nonvirtual	ServiceDescription	GetServiceDescription () const;

				public:
					struct	CommandNames {
						/*
						 *  The kRunAsService command is about the only command that tends to NOT be called by users on the command line.
						 *	it tells the code to run indefinitely, (until told to stop) - running the service loop.
						 *
						 *	This is typically called INDRECTLY via a specail fork/exec as a result of a kStart command, or its called from
						 *	init as part of system startup.
						 */
						static	const	wchar_t	kRunAsService[];
						/*
						 *  The kStart command tells the service to start running. It returns an error
						 *  if the service is already started.
						 */
						static	const	wchar_t	kStart[];
						/*
						 *  The kStop command tells the service to start terminate
						 */
						static	const	wchar_t	kStop[];
						//DOCUMENT EACH
						//NEATLY
						// KILL termiantes (kill-9)
						//
						static	const	wchar_t	kKill[];
						// restart synonmy for stop (no error if not already running), and then start
						static	const	wchar_t	kRestart[];
						// If service knows how to find its own config files - recheck them
						static	const	wchar_t	kReloadConfiguration[];
						// SIGSTOP
						static	const	wchar_t	kPause[];
						// SIGCONT
						static	const	wchar_t	kContinue[];
					};

				protected:
					/*
					 * Will handle the given command line argument, and return true if it recognized it, and handled it. It will return false otherwise.
					 */
					nonvirtual	bool	_HandleStandardCommandLineArgument (const String& arg);
			};

		}
	}
}
#endif	/*_Stroika_Frameworks_Service_Main_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Main.inl"
