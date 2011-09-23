/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_Service_Main_h_
#define	_Stroika_Framework_Service_Main_h_	1

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
	namespace	Framework {
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
							virtual	void				MainLoop () = 0;

							virtual	void				OnStartRequest ();
							virtual	void				OnStopRequest ();
							virtual	void				OnReReadConfigurationRequest ();
							virtual	ServiceDescription	GetServiceDescription () const;
					};
				public:
					Main (Memory::SharedPtr<IRep> rep);

				public:
					nonvirtual	void				Start ();
					nonvirtual	void				Stop ();
					nonvirtual	void				Restart ();
					nonvirtual	void				ReReadConfiguration ();
					nonvirtual	ServiceDescription	GetServiceDescription () const;
			};

		}
	}
}
#endif	/*_Stroika_Framework_Service_Main_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Main.inl"
