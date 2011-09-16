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
	#include	<time.h>
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
							timespec	ts;
							ts.tv_sec = int (seconds2Wait);
							const	long	kNanoSecondsPerSecond	=	1000L * 1000L * 1000L;
							ts.tv_nsec = static_cast<time_t> (kNanoSecondsPerSecond * (seconds2Wait - ts.tv_sec));
							Assert (0 >= ts.tv_nsec and ts.tv_nsec < kNanoSecondsPerSecond);
							(void)::nanosleep (&ts, nullptr);
						#else
							AssertNotImplemented ();
						#endif
					}
				inline	void	Sleep (Time::DurationSecondsType seconds2Wait, Time::DurationSecondsType* remainingInSleep)
					{
						Require (seconds2Wait >= 0.0);
						RequireNotNull (remainingInSleep);	// else call the over overload
						#if		qPlatform_Windows
							Time::DurationSecondsType	tc	=	GetTickCount ();
							if (::SleepEx (static_cast<int> (seconds2Wait * 1000), true) == 0) {
								*remainingInSleep = 0;
							}
							else {
								Time::DurationSecondsType	remaining	= (tc + seconds2Wait) - GetTickCount ();
								if (remaining < 0) {
									remaining = 0;
								}
								*remainingInSleep = remaining;
							}
						#elif		qPlatform_POSIX
							timespec	ts;
							ts.tv_sec = int (seconds2Wait);
							const	long	kNanoSecondsPerSecond	=	1000L * 1000L * 1000L;
							ts.tv_nsec = static_cast<time_t> (kNanoSecondsPerSecond * (seconds2Wait - ts.tv_sec));
							Assert (0 >= ts.tv_nsec and ts.tv_nsec < kNanoSecondsPerSecond);
							timespec	nextTS;
							(void)::nanosleep (&ts, &nextTS);
							*remainingInSleep = nextTS.tv_nsec + static_cast<Time::DurationSecondsType> (ts.tv_nsec) / kNanoSecondsPerSecond;
						#else
							AssertNotImplemented ();
						#endif
					}
		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Sleep_inl_*/
