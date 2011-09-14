/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Debug/Assertions.h"
#include	"../Memory/SmallStackBuffer.h"

#include	"Realtime.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Time;










DurationSecondsType	Stroika::Foundation::Time::GetTickCount ()
{
	#if		qPlatform_Windows
		static	bool			sInited	=	false;
		static	LARGE_INTEGER	sPerformanceFrequency;
		if (not sInited) {
			if (not ::QueryPerformanceFrequency (&sPerformanceFrequency)) {
				sPerformanceFrequency.QuadPart = 0;
			}
			sInited = true;
		}
		if (sPerformanceFrequency.QuadPart == 0) {
			return (float (::GetTickCount ()) / 1000.0f);
		}
		else {
			LARGE_INTEGER	counter;
			counter.QuadPart = 0;
			Verify (::QueryPerformanceCounter (&counter));
			return static_cast<DurationSecondsType> (static_cast<double> (counter.QuadPart) / static_cast<double> (sPerformanceFrequency.QuadPart));
		}
	#else
		return time (0);	//tmphack... not good but better than assert erorr
		AssertNotReached ();
		return 0;
	#endif
}


