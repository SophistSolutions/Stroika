/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Tests_TestHarness_h_
#define	_Stroika_Foundation_Tests_TestHarness_h_	1

#include	"Stroika/Foundation/StroikaPreComp.h"

#include	"Stroika/Foundation/Configuration/Common.h"


namespace	Stroika {	
	namespace	TestHarness {

		void	Setup ();

		// print succeeded if it completes, and failed if exception caught
		void	PrintPassOrFail (void (*regressionTest) ());

	}
}
#endif	/*_Stroika_Foundation_Tests_TestHarness_h_*/

