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
#elif	qPlatform_POSIX
	#include	<unistd.h>
#endif

#include	"../Debug/Assertions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			// class	Sleep
				inline	void	Sleep (Time::DurationSecondsType seconds2Wait)
					{
						Require (seconds2Wait >= 0.0);
						#if		qPlatform_Windows
							::SleepEx (static_cast<int> (seconds2Wait * 1000), true);
						#elif		qPlatform_POSIX
							//usleep ();
							timespec	ts;
							ts.tv_sec = int (seconds2Wait);
							ts.tv_nsec = int (1000 * 1000 * 1000 * (seconds2Wait - ts.tv_sec));
							(void)::nanosleep (&ts, nullptr);
						#else
							AssertNotImplemented ();
						#endif
					}

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Sleep_inl_*/
