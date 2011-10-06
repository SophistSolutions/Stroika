/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>

#if		qPlatform_Windows
	#include	<atlbase.h>		// For CComBSTR
#elif	qPlatform_POSIX
	#include <time.h>
#endif

#include	"../Characters/Format.h"
#include	"../Debug/Assertions.h"
#include	"../Execution/Exceptions.h"
#include	"../Memory/SmallStackBuffer.h"
#include	"../Linguistics/Words.h"
#include	"DateTime.h"

#include	"TimeOfDay.h"




using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Memory;
using	namespace	Stroika::Foundation::Time;

using	Debug::TraceContextBumper;

using	namespace	Time;



#if		qPlatform_Windows
namespace	{
	wstring	GetLocaleInfo_ (LCID Locale, LCTYPE LCType)
		{
			int	sizeNeeded	=	::GetLocaleInfoW (Locale, LCType, nullptr, 0);
			SmallStackBuffer<wchar_t> buf (sizeNeeded + 1);
			Verify (::GetLocaleInfoW (Locale, LCType, buf, sizeNeeded + 1));
			return wstring (buf);
		}
}
#endif


#if		qPlatform_Windows
namespace	{
	TimeOfDay	mkTimeOfDay_ (const SYSTEMTIME& sysTime)
		{
			WORD	hour = max (sysTime.wHour, static_cast<WORD> (0));
			hour = min (hour, static_cast<WORD> (23));
			WORD	minute = max (sysTime.wMinute, static_cast<WORD> (0));
			minute = min (minute, static_cast<WORD> (59));
			WORD	secs = max (sysTime.wSecond, static_cast<WORD> (0));
			secs = min (secs, static_cast<WORD> (59));
			return TimeOfDay ((hour * 60 + minute) * 60 + secs);
		}
}
#endif

#if		qPlatform_Windows
namespace	{
	wstring	GenTimeStr4TOD_ (unsigned int hour, unsigned int minutes, unsigned int seconds)
		{
// Consider rewriting using Win32 GetTimeFormat () - and just futzing with teh format string for the case
// of zero minutes/seconds?? That might be more robust in case of changes in adding special tokens for some cultures/etc?
//
// Anyhow - I think what i have no works OK...
//		-- LGP 2009-06-18
			Require (hour < 24);
			Require (minutes < 60);
			Require (seconds < 60);
		
			/*
			 * From Windows Vista Regional Settings control panel:
			 *
			 *		h = hour, m = minute, s = second
			 *		tt = A.M. or P.M.
			 *
			 *		h = 12 hour
			 *		H = 24 hour
			 *
			 *		hh,mm,ss = display leading zero
			 *		h,m,s = do not display leading zero.
			 */

			// we could keep recomputing this, but why pay the runtime cost? Restart app to get new locale info
			#if		qPlatform_Windows
				static	const	wstring	kFormatStr	=	GetLocaleInfo_ (LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT);
			#else
			#endif

			// This is what I couldn't get the MSFT default locale display stuff todo
			// We want to show 3pm, not 3:00:00pm.
			bool	skipSeconds	=	seconds == 0;
			bool	skipMinutes	=	skipSeconds and minutes == 0 and kFormatStr.find (L"tt") != wstring::npos;	// only do if showing AM/PM, else show 1300 for time - not 13...

			wstring	outBuf;
			outBuf.reserve (kFormatStr.length () + 4);
			for (wstring::const_iterator i = kFormatStr.begin (); i != kFormatStr.end (); ++i) {
				switch (*i) {
					case 'h': {
						bool	leadingZero	=	false;
						if (i + 1 != kFormatStr.end () and *(i+1) == 'h') {
							++i;
							leadingZero = true;
						}
						unsigned int	useHour	=	hour;
						if (useHour > 12) {
							useHour -= 12;
						}
						outBuf += Format (leadingZero? L"%02d": L"%d", useHour);
					}
					break;
					case 'H': {
						bool	leadingZero	=	false;
						if (i + 1 != kFormatStr.end () and *(i+1) == 'H') {
							++i;
							leadingZero = true;
						}
						outBuf += Format (leadingZero? L"%02d": L"%d", hour);
					}
					break;
					case 'm': {
						bool	leadingZero	=	false;
						if (i + 1 != kFormatStr.end () and *(i+1) == 'm') {
							++i;
							leadingZero = true;
						}
						// This is what I couldn't get the MSFT default locale display stuff todo
						// We want to show 3pm, not 3:00:00pm.
						if (not skipMinutes) {
							outBuf += Format (leadingZero? L"%02d": L"%d", minutes);
						}
					}
					break;
					case 's': {
						bool	leadingZero	=	false;
						if (i + 1 != kFormatStr.end () and *(i+1) == 's') {
							++i;
							leadingZero = true;
						}
						if (not skipSeconds) {
							outBuf += Format (leadingZero? L"%02d": L"%d", seconds);
						}
					}
					break;
					case 't': {
						if (i + 1 != kFormatStr.end () and *(i+1) == 't') {
							/*
							 *	From the WinSDK docs...
							 *
							 *	LOCALE_S1159 
							 *		String for the AM designator. The maximum number of characters allowed for this string is nine. 
							 *	LOCALE_S2359 
							 *		String for the PM designator. The maximum number of characters allowed for this string is nine. 
							 */
							static	const	wstring	kAMSTR	=	GetLocaleInfo_ (LOCALE_USER_DEFAULT, LOCALE_S1159);
							static	const	wstring	kPMSTR	=	GetLocaleInfo_ (LOCALE_USER_DEFAULT, LOCALE_S2359);
							++i;
							int pmFlag =	hour>=12;
							//outBuf += (pmFlag?L"PM":L"AM");
							outBuf += (pmFlag?kPMSTR:kAMSTR);
						}
						else {
							outBuf.push_back (*i);
						}
					}
					break;
					case ':': {
						// Skip ':' separator if we are going to omit the following
						// stuff...
						if (i + 1 != kFormatStr.end () and *(i+1) == 's' and skipSeconds) {
							// SKIP
						}
						else if (i + 1 != kFormatStr.end () and *(i+1) == 'm' and skipMinutes) {
							// SKIP
						}
						else {
							outBuf.push_back (*i);
						}
					}
					break;
					default: {
						outBuf.push_back (*i);
					}
					break;
				}
			}
			return outBuf;
		}
}
#endif





/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
const	TimeOfDay	TimeOfDay::kMin	=	TimeOfDay (0);
const	TimeOfDay	TimeOfDay::kMax	=	TimeOfDay (24 * 60 * 60 - 1);

TimeOfDay::TimeOfDay ()
  : fTime_ (-1)
{
}

TimeOfDay::TimeOfDay (uint32_t t)
  : fTime_ (t < 24 * 60 * 60? t: (24 * 60 * 60-1))
{
}

TimeOfDay	TimeOfDay::Parse (const wstring& rep, PrintFormat pf)
{
	if (rep.empty ()) {
		return TimeOfDay ();
	}
	switch (pf) {
		case	eCurrentLocale_PF:	{
			#if		qPlatform_Windows
				return Parse (rep, LOCALE_USER_DEFAULT);
			#elif	qPlatform_POSIX
				AssertNotImplemented ();
				return TimeOfDay ();
			#else
				AssertNotImplemented ();
				return TimeOfDay ();
			#endif
		}
		case	eXML_PF: {
			int	hour	=	0;
			int	minute	=	0;
			int secs	=	0;
			#pragma	warning (push)
			#pragma	warning (4 : 4996)		// MSVC SILLY WARNING ABOUT USING swscanf_s
			if (::swscanf (rep.c_str (), L"%d:%d:d", &hour, &minute, &secs) >= 2) {
				hour = max (hour, 0);
				hour = min (hour, 23);
				minute = max (minute, 0);
				minute = min (minute, 59);
				secs = max (secs, 0);
				secs = min (secs, 59);
				return TimeOfDay ((hour * 60 + minute) * 60 + secs);
			}
			#pragma	warning (pop)
			return TimeOfDay ();
		}
		default: {
			AssertNotReached ();
			return TimeOfDay ();
		}
	}
}

#if		qPlatform_Windows
TimeOfDay	TimeOfDay::Parse (const wstring& rep, LCID lcid)
{
	if (rep.empty ()) {
		return TimeOfDay ();
	}
	DATE		d;
	(void)::memset (&d, 0, sizeof (d));
	try {
		ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, VAR_TIMEVALUEONLY, &d));
	}
	catch (...) {
		// Apparently military time (e.g. 1300 hours - where colon missing) - is rejected as mal-formed.
		// Detect that - and try to interpret it appropriately.
		wstring	newRep = rep;
		if (newRep.length () == 4 and
			iswdigit (newRep[0]) and iswdigit (newRep[1]) and iswdigit (newRep[2]) and iswdigit (newRep[3])
			) {
			newRep = newRep.substr (0, 2) + L":" + newRep.substr (2, 2);
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (newRep.c_str ()), lcid, VAR_TIMEVALUEONLY, &d));
		}
		else {
			Execution::DoThrow (FormatException ());
		}
	}
	// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
	SYSTEMTIME	sysTime;
	memset (&sysTime, 0, sizeof (sysTime));
	Verify (::VariantTimeToSystemTime (d, &sysTime));
	return mkTimeOfDay_ (sysTime);
}
#endif

wstring	TimeOfDay::Format (PrintFormat pf) const
{
	if (empty ()) {
		return wstring ();
	}
	switch (pf) {
		case	eCurrentLocale_PF:	{
			#if		qPlatform_Windows
				return Format (LOCALE_USER_DEFAULT);
			#elif	qPlatform_POSIX
				AssertNotImplemented ();
				return wstring ();
			#else
				AssertNotImplemented ();
				return wstring ();
			#endif
		}
		case	eXML_PF: {
			uint32_t	hour = fTime_/(60*60);
			uint32_t	minutes = (fTime_ - hour * 60 * 60) / 60;
			uint32_t	secs = fTime_ - hour * 60 * 60 - minutes * 60;
			Assert (hour >= 0 and hour < 24);
			Assert (minutes >= 0 and minutes < 60);
			Assert (secs >= 0 and secs < 60);
			return ::Format (L"%02d:%02d:%02d", hour, minutes, secs);
		}
		default: {
			AssertNotReached ();
			return wstring ();
		}
	}
}

void	TimeOfDay::ClearSecondsField ()
{
	int hour = fTime_/(60*60);
	int minutes = (fTime_ - hour * 60 * 60) / 60;
	int secs = fTime_ - hour * 60 * 60 - minutes * 60;
	Assert (hour >= 0 and hour < 24);
	Assert (minutes >= 0 and minutes < 60);
	Assert (secs >= 0 and secs < 60);
	fTime_ -= secs;
}

#if		qPlatform_Windows
wstring	TimeOfDay::Format (LCID lcid) const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		uint32_t hour = fTime_/(60*60);
		uint32_t minutes = (fTime_ - hour * 60 * 60) / 60;
		uint32_t secs = fTime_ - hour * 60 * 60 - minutes * 60;
		Assert (hour >= 0 and hour < 24);
		Assert (minutes >= 0 and minutes < 60);
		Assert (secs >= 0 and secs < 60);
		return GenTimeStr4TOD_ (hour, minutes, secs);
	}
}
#endif

