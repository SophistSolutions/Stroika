/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<cstdlib>
#include	<cstring>
#include	<ctime>

#include	"../Configuration/Common.h"
#include	"../Debug/Assertions.h"
#include	"DateTime.h"

#include	"Timezone.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Time;




bool	Time::IsDaylightSavingsTime ()
{
	static	bool	sCalledOnce_ = false;
	if (not sCalledOnce_) {
		tzset ();
		sCalledOnce_ = true;
	}
	return daylight;
}

bool	Time::IsDaylightSavingsTime (const DateTime& d)
{
	struct	tm	asTM	=	d.As<struct tm> ();
	asTM.tm_isdst = -1;	// force calc of correct daylight savings time flag
	static	bool	sCalledOnce_ = false;
	if (not sCalledOnce_) {
		tzset ();
		sCalledOnce_ = true;
	}
	time_t	result	=	mktime (&asTM);
	// THINK this is true - not totally clear

	return asTM.tm_isdst;
}







time_t	Time::GetLocaltimeToGMTOffset (bool applyDST)
{
	#if		0
		// WRONG - but COULD use this API - but not sure needed
		#if		qPlatform_Windows
			TIME_ZONE_INFORMATION	tzInfo;
			memset (&tzInfo, 0, sizeof (tzInfo));
			(void)::GetTimeZoneInformation (&tzInfo);
			int unsignedBias	=	abs (tzInfo.Bias);
			int	hrs	=	unsignedBias / 60;
			int mins = unsignedBias - hrs * 60;
			tzBiasString = ::Format (L"%s%.2d:%.2d", (tzInfo.Bias >= 0? L"-": L"+"), hrs, mins);
		#endif
	#endif

	static	bool	sCalledOnce_ = false;
	if (not sCalledOnce_) {
		tzset ();
		sCalledOnce_ = true;
	}

	/*
	 * COULD this be cached? It SHOULD be - but what about when the timezone changes? there maybe a better way to compute this using the
	 * timezone global var???
	 */
	struct tm tm;
	memset (&tm, 0, sizeof(tm));
	tm.tm_year = 70;
	tm.tm_mon = 0;		// Jan
	tm.tm_mday = 1;
	tm.tm_isdst = applyDST;
	time_t	result	=	mktime (&tm);
	return result;
}
