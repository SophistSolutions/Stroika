/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Frameworks_Service_Main_inl_
#define	_Stroika_Frameworks_Service_Main_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../../Foundation/Execution/Thread.h"
#include	"../../Foundation/Execution/ThreadAbortException.h"

namespace	Stroika {	
	namespace	Frameworks {
		namespace	Service {

		//	class	Servie::Main::IRep
			inline	void	Main::IRep::_CheckAndAbortThread () const
				{
					Execution::CheckForThreadAborting ();
					if (fStopping_) {
						Execution::DoThrow (Execution::ThreadAbortException ());
					}
				}
			inline	bool	Main::IRep::_CheckShouldReReadConfig () const
				{
					return fMustReReadConfig;
				}
			inline	void	Main::IRep::_DidReReadConfig ()
				{
					fMustReReadConfig = false;
				}

		}

	}
}
#endif	/*_Stroika_Frameworks_Service_Main_inl_*/



