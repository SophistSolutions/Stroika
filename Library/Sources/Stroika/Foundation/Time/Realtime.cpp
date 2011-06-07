/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>

#if		defined(_WIN32)
#include	<Windows.h>
#endif

#include	"../Debug/Assertions.h"
#include	"../Memory/SmallStackBuffer.h"

#include	"RealTime.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;




/*
@METHOD:		Led_GetTickCount
@DESCRIPTION:	<p>Get the number of seconds since some constant, system-specified reference time. This is used
			to tell how much time has elapsed since a particular event.</p>
				<p>Note - though this is based on the same reference time as an time values packed into event records,
			it maybe differently normalized. These times are all in seconds, whereas event records are often in
			other units (ticks - 1/60 of a second, or milliseconds).</p>
				<p>In the case of X-Windows - this business is very complicated because there are two different times
			one might be intersted in. There is the time on the client (where Led is running) and the time on the X-Server
			(users computer screen). Alas - X11R4 appears to have quite weak support for time - and offers no way I've found
			to accurately get the time from the users computer. As a result - with X-Windows - you must arrange to call
			@'SyncronizeLedXTickCount' for each event that specifies a time value (as soon as that event arrives). This
			data - together with time values from the client (where Led is running) computer will be used to provide a
			better approximation of the true elapsed time.</p>
*/
float	Stroika::Foundation::Time::GetTickCount ()
{
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
		return static_cast<float> (static_cast<double> (counter.QuadPart) / static_cast<double> (sPerformanceFrequency.QuadPart));
	}
}


