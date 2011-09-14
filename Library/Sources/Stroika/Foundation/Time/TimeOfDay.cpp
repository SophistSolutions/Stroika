/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>

#if		qPlatform_Windows
	#include	<atlbase.h>		// For CComBSTR
#elif	qPlatform_POSIX
	#include <time.h>
	//not sure needed?#include	<sys/times.h>
#endif

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






/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
TimeOfDay::TimeOfDay ()
  : fTime (-1)
{
}

TimeOfDay::TimeOfDay (unsigned int t)
  : fTime (-1)
{
	fTime = t;
	fTime %= 60*60*24;				// assure small enuf to fit within a day
}

TimeOfDay::TimeOfDay (const wstring& rep)
	: fTime (-1)
{
	if (rep.empty ()) {
		Ensure (empty ());
		return;		// if empty string - just no time specified...
	}
#if		qPlatform_Windows
	LCID lcid = LOCALE_USER_DEFAULT;
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
	*this = TimeOfDay (sysTime);
#else
	AssertNotImplemented ();
#endif
}

#if		qPlatform_Windows
TimeOfDay::TimeOfDay (const wstring& rep, LCID lcid)
	: fTime (-1)
{
	if (rep.empty ()) {
		Ensure (empty ());
		return;		// if empty string - just no time specified...
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
	*this = TimeOfDay (sysTime);
}
#endif

TimeOfDay::TimeOfDay (const wstring& rep, XML):
	fTime (-1)
{
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
		fTime = (hour * 60 + minute) * 60 + secs;
	}
	#pragma	warning (pop)
}

#if		qPlatform_Windows
TimeOfDay::TimeOfDay (const SYSTEMTIME& sysTime):
	fTime (-1)
{
	WORD	hour = max (sysTime.wHour, static_cast<WORD> (0));
	hour = min (hour, static_cast<WORD> (23));
	WORD	minute = max (sysTime.wMinute, static_cast<WORD> (0));
	minute = min (minute, static_cast<WORD> (59));
	WORD	secs = max (sysTime.wSecond, static_cast<WORD> (0));
	secs = min (secs, static_cast<WORD> (59));
	fTime = (hour * 60 + minute) * 60 + secs;
}
#endif

TimeOfDay	TimeOfDay::Now ()
{
	return DateTime::Now ().GetTimeOfDay ();
}

void	TimeOfDay::ClearSecondsField ()
{
	int hour = fTime/(60*60);
	int minutes = (fTime - hour * 60 * 60) / 60;
	int secs = fTime - hour * 60 * 60 - minutes * 60;
	Assert (hour >= 0 and hour < 24);
	Assert (minutes >= 0 and minutes < 60);
	Assert (secs >= 0 and secs < 60);
	fTime -= secs;
}

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
			static	const	wstring	kFormatStr	=	GetLocaleInfo_ (LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT);

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
wstring	TimeOfDay::Format (LCID lcid) const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		#if	1
			int hour = fTime/(60*60);
			int minutes = (fTime - hour * 60 * 60) / 60;
			int secs = fTime - hour * 60 * 60 - minutes * 60;
			Assert (hour >= 0 and hour < 24);
			Assert (minutes >= 0 and minutes < 60);
			Assert (secs >= 0 and secs < 60);
			return GenTimeStr4TOD_ (hour, minutes, secs);
		#elif	1
			// Simpler and maybe safer than GenTimeStr4TOD_ - but alas - doesn't have the ability to
			// remove trailing zeros...
			DATE	variantDate	=	0.0;
			ThrowIfErrorHRESULT (::SystemTimeToVariantTime (&operator SYSTEMTIME (), &variantDate));
			BSTR	tmpStr	=	nullptr;
			ThrowIfErrorHRESULT (::VarBstrFromDate (variantDate, lcid, VAR_TIMEVALUEONLY, &tmpStr));
			wstring	result	=	BSTR2wstring (tmpStr);
			::SysFreeString (tmpStr);
			return result;
		#elif 1
			int hour = fTime/(60*60);
			int minutes = (fTime - hour * 60 * 60) / 60;
			int secs = fTime - hour * 60 * 60 - minutes * 60;
			Assert (hour >= 0 and hour < 24);
			Assert (minutes >= 0 and minutes < 60);
			Assert (secs >= 0 and secs < 60);
			int pmFlag =	hour>=12;
			if (pmFlag) {
				hour -= 12;
			}
			if (hour == 0) {
				hour = 12;
			}
			const wchar_t* pmFlagStr	=	pmFlag?L"PM":L"AM";
			if (secs == 0 and minutes == 0) {
				return ::Format (L"%d %s", hour, pmFlagStr);
			}
			else if (secs == 0) {
				return ::Format (L"%d:%02d %s", hour, minutes, pmFlagStr);
			}
			else {
				return ::Format (L"%d:%02d:%02d %s", hour, minutes, secs, pmFlagStr);
			}
		#else
			wchar_t	buf[1024];
			buf[0] = 0;
			__time64_t	t	=	fTime;
			struct tm	 temp;
			memset (&temp, 0, sizeof (temp));
			temp.tm_hour = fTime/(60*60);
			temp.tm_min = fTime - temp.tm_hour * 60 * 60;
			wcsftime (buf, NEltsOf (buf), L"%I:%M %p", &temp);
			return buf;
		#endif
	}
}
#endif

wstring	TimeOfDay::Format4XML () const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		int hour = fTime/(60*60);
		int minutes = (fTime - hour * 60 * 60) / 60;
		int secs = fTime - hour * 60 * 60 - minutes * 60;
		Assert (hour >= 0 and hour < 24);
		Assert (minutes >= 0 and minutes < 60);
		Assert (secs >= 0 and secs < 60);
		return ::Format (L"%02d:%02d:%02d", hour, minutes, secs);
	}
}

#if		qPlatform_Windows
TimeOfDay::operator SYSTEMTIME () const
{
	SYSTEMTIME	t;
	memset (&t, 0, sizeof (t));
	if (not empty ()) {
		unsigned int	seconds	=	fTime;
		unsigned int	minutes	=	seconds / 60;
		unsigned int	hours	=	minutes / 60;

		hours = min (hours, 23U);
		t.wHour = hours;

		minutes -= hours * 60;
		minutes = min (minutes, 59U);
		t.wMinute = minutes;

		seconds -= (60*60 * hours + 60 * minutes);
		seconds = min (seconds, 59U);
		t.wSecond = seconds;
	}
	return t;
}
#endif

