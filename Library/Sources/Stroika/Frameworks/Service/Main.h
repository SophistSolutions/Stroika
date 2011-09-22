/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Framework_Service_Main_h_
#define	_Stroika_Framework_Service_Main_h_	1

#include	"../StroikaPreComp.h"

#include	<string>
#include	<vector>

#include	"../../Foundation/Configuration/Common.h"


namespace	Stroika {	
	namespace	Framework {
		namespace	Service {


			// very very rough draft - 
			class	Main {
			public:

				// USE IRunnable?
				virtual	void	MainLoop () = 0;


				// totally UN THOUGHT OUT
				virtual	void	OnStartRequest () = 0;
				virtual	void	OnStopRequest () = 0;


			public:
				void	Start ();
				void	Stop ();
				void	Restart ();
				void	ReReadConfiguration ();

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
