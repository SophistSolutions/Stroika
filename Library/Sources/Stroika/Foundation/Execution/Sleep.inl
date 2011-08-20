/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Sleep_inl_
#define	_Stroika_Foundation_Execution_Sleep_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#if		qPlatform_Windows
	#include	<windows.h>
#endif

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			// class	Sleep
				inline	void	Sleep (Time::DurationSecondsType seconds2Wait)
					{
						#if		qPlatform_Windows
							::SleepEx (static_cast<int> (seconds2Wait * 1000), true);
						#else
							AssertNotImplemented ();
						#endif
					}

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Sleep_inl_*/
