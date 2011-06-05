/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"StroikaPreComp.h"

#include	<algorithm>

#include	<atlbase.h>		// For CComBSTR

#include	"Debug/Assertions.h"
#include	"Execution/Exceptions.h"
#include	"Memory/SmallStackBuffer.h"

#include	"DateUtils.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;

using	Debug::Trace::TraceContextBumper;

using	namespace	Exceptions;
using	namespace	StringUtils;



namespace	{
	wstring	GetLocaleInfo_ (LCID Locale, LCTYPE LCType)
		{
			int	sizeNeeded	=	::GetLocaleInfoW (Locale, LCType, NULL, 0);
			SmallStackBuffer<wchar_t> buf (sizeNeeded + 1);
			Verify (::GetLocaleInfoW (Locale, LCType, buf, sizeNeeded + 1));
			return wstring (buf);
		}
}




/*
 ********************************************************************************
 ************************************** Date ************************************
 ********************************************************************************
 */
const	Date	Date::kMin	=	Date (Date::JulianRepType (Date::kMinJulianRep));
const	Date	Date::kMax	=	Date (Date::JulianRepType (UINT_MAX-1));

Date::Date ()
	: fJulianDateRep (kEmptyJulianRep)
{
}

Date::Date (JulianRepType julianRep)
	: fJulianDateRep (julianRep)
{
}

Date::Date (Year year, MonthOfYear month, DayOfMonth day)
	: fJulianDateRep (jday (month, day, year))
{
}

Date::Date (const SYSTEMTIME& sysTime)
	: fJulianDateRep (Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear)))
{
}

Date::Date (const FILETIME& fileTime)
	: fJulianDateRep (kEmptyJulianRep)
{
	FILETIME localTime;
	(void)::memset (&localTime, 0, sizeof (localTime));
	if (::FileTimeToLocalFileTime (&fileTime, &localTime)) {
		SYSTEMTIME sysTime;
		(void)::memset (&sysTime, 0, sizeof (sysTime));
		if (::FileTimeToSystemTime (&localTime, &sysTime)) {
			fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
		}
	}
}

Date::Date (const wstring& rep, XML)
	: fJulianDateRep (kEmptyJulianRep)
{
// SHOULD TAKE INTO ACCOUNT TIMEZONE FIELD - IF ANY - AND NOT CURRENT TIMEZONE!!! (LOCALE_USER_DEFAULT IS WRONG) -  LGP 2005-10-31
	if (not rep.empty ()) {
		DATE		d;
		(void)::memset (&d, 0, sizeof (d));
		try {
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), LOCALE_USER_DEFAULT, VAR_DATEVALUEONLY, &d));
		}
		catch (...) {
			Exceptions::DoThrow (FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
	}
}

Date::Date (const wstring& rep, Javascript)
	: fJulianDateRep (kEmptyJulianRep)
{
	/*
	 *	See also Format4JScript for javascript format info
	 */
	const	LCID	kUS_ENGLISH_LOCALE	=	MAKELCID (MAKELANGID (LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT);
	if (not rep.empty ()) {
		DATE		d;
		(void)::memset (&d, 0, sizeof (d));
		try {
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), kUS_ENGLISH_LOCALE, VAR_DATEVALUEONLY, &d));
		}
		catch (...) {
			Exceptions::DoThrow (FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
	}
}

Date::Date (const wstring& rep, LCID lcid)
	: fJulianDateRep (kEmptyJulianRep)
{
	if (not rep.empty ()) {
		DATE		d;
		(void)::memset (&d, 0, sizeof (d));
		try {
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, VAR_DATEVALUEONLY, &d));
		}
		catch (...) {
			Exceptions::DoThrow (FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
	}
}

Date	Date::GetToday ()
{
	SYSTEMTIME	st;
	memset (&st, 0, sizeof (st));
	::GetLocalTime (&st);

	return Date (Year (st.wYear), MonthOfYear (st.wMonth), DayOfMonth (st.wDay));
}

wstring	Date::Format (LCID lcid) const
{
	SYSTEMTIME	st	=	*this;
	int	nTChars	=	::GetDateFormat (lcid, DATE_SHORTDATE, &st, NULL, NULL, 0);
	if (nTChars == 0) {
		return wstring ();
	}
	else {
		SmallStackBuffer<TCHAR> buf (nTChars + 1);
		(void)::GetDateFormat (lcid, DATE_SHORTDATE, &st, NULL, buf, nTChars+1);
		return tstring2Wide (static_cast<const TCHAR*> (buf));
	}
}

wstring	Date::Format (const tstring& format, LCID lcid) const
{
	SYSTEMTIME	st	=	*this;
	int	nTChars	=	::GetDateFormat (lcid, 0, &st, format.c_str (), NULL, 0);
	if (nTChars == 0) {
		return wstring ();
	}
	else {
		SmallStackBuffer<TCHAR> buf (nTChars + 1);
		(void)::GetDateFormat (lcid, 0, &st, format.c_str (), buf, nTChars+1);
		return tstring2Wide (static_cast<const TCHAR*> (buf));
	}
}

wstring	Date::Format4XML () const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		wchar_t	buf[20];
		MonthOfYear	m	=	eEmptyMonthOfYear;
		DayOfMonth	d	=	eEmptyDayOfMonth;
		Year		y	=	eEmptyYear;
		mdy (&m, &d, &y);
		::swprintf_s (buf, L"%04d-%02d-%02d", y, m, d);
		return buf;
	}
}

wstring	Date::Format4JScript () const
{
	/*
	 *	From 
	 *		http://msdn.microsoft.com/library/default.asp?url=/library/en-us/script56/html/ed737e50-6398-4462-8779-2af3c03f8325.asp
	 *
	 *			parse Method (JScript 5.6)  
	 *			...
	 *			The following rules govern what the parse method can successfully parse: 
	 *			Short dates can use either a "/" or "-" date separator, but must follow the month/day/year format, for example "7/20/96". 
	 *
	 *	See also 		explicit Date (const wstring& rep, Javascript);
	 */
	if (empty ()) {
		return wstring ();
	}
	else {
		wchar_t	buf[20];
		MonthOfYear	m	=	eEmptyMonthOfYear;
		DayOfMonth	d	=	eEmptyDayOfMonth;
		Year		y	=	eEmptyYear;
		mdy (&m, &d, &y);
		::swprintf_s (buf, L"%02d/%02d/%04d", m, d, y);
		return buf;
	}
}

wstring	Date::LongFormat (LCID lcid) const
{
	SYSTEMTIME	st	=	*this;
	int	nTChars	=	::GetDateFormat (lcid, DATE_LONGDATE, &st, NULL, NULL, 0);
	if (nTChars == 0) {
		return wstring ();
	}
	else {
		SmallStackBuffer<TCHAR> buf (nTChars + 1);
		(void)::GetDateFormat (lcid, DATE_LONGDATE, &st, NULL, buf, nTChars+1);
		return tstring2Wide (static_cast<const TCHAR*> (buf));
	}
}

Date	Date::AddDays (int dayCount)
{
	if (empty ()) {
		// then assume was supposed to be relative to today
		*this = GetToday ();
	}
	fJulianDateRep += dayCount;
	return *this;
}

Date::JulianRepType	Date::DaysSince () const
{
	int	r	=	DayDifference (GetToday (), *this);
	if (r < 0) {
		return 0;
	}
	else {
		return r;
	}
}

Date::operator SYSTEMTIME () const
{
	SYSTEMTIME	st;
	memset (&st, 0, sizeof (st));
	MonthOfYear	m	=	eEmptyMonthOfYear;
	DayOfMonth	d	=	eEmptyDayOfMonth;
	Year		y	=	eEmptyYear;
	mdy (&m, &d, &y);
	st.wYear = y;
	st.wMonth = m;
	st.wDay = d;
	return st;
}

Date::Year	Date::GetYear () const
{
	MonthOfYear	m	=	eEmptyMonthOfYear;
	DayOfMonth	d	=	eEmptyDayOfMonth;
	Year		y	=	eEmptyYear;
	mdy (&m, &d, &y);
	return y;
}

Date::MonthOfYear	Date::GetMonth () const
{
	MonthOfYear	m	=	eEmptyMonthOfYear;
	DayOfMonth	d	=	eEmptyDayOfMonth;
	Year		y	=	eEmptyYear;
	mdy (&m, &d, &y);
	Ensure (empty () or 1 <= m and m <= 12);
	Ensure (0 <= m and m <= 12);
	return m;
}

Date::DayOfMonth	Date::GetDayOfMonth () const
{
	MonthOfYear	m	=	eEmptyMonthOfYear;
	DayOfMonth	d	=	eEmptyDayOfMonth;
	Year		y	=	eEmptyYear;
	mdy (&m, &d, &y);
	Ensure (empty () or 1 <= d and d <= 31);
	Ensure (0 <= d and d <= 31);
	return d;
}

/*
 * Convert Gregorian calendar date to the corresponding Julian day number
 * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
 * This function not valid before that.
 *
 * (This code originally from NIHCL)
 */
Date::JulianRepType	Date::jday (MonthOfYear month, DayOfMonth day, Year year)
{
	if (month == eEmptyMonthOfYear or day == eEmptyDayOfMonth or year == eEmptyYear) {
		return kEmptyJulianRep;
	}

	Require (year > 1752 or (year == 1752 and (month > Date::eSeptember or (month == Date::eSeptember and day >= 14))));

	JulianRepType	c;
	JulianRepType	ya;
	if (month > 2) {
		month = static_cast<MonthOfYear> (month - 3);
	}
	else {
		month = static_cast<MonthOfYear> (month + 9);
		year = static_cast<Year> (year - 1);
	}
	c = year / 100;
	ya = year - 100*c;
	return (((146097*c)>>2) + ((1461*ya)>>2) + (153*month + 2)/5 + day + 1721119);
}

Date::JulianRepType	Date::Safe_jday (MonthOfYear month, DayOfMonth day, Year year)
{
	// 'Safe' version just avoids require that date values are legit for julian date range. If date would be invalid - return kEmptyJulianRep.

	if (month == eEmptyMonthOfYear or day == eEmptyDayOfMonth or year == eEmptyYear) {
		return kEmptyJulianRep;
	}
	if (year > 1752 or (year == 1752 and (month > Date::eSeptember or (month == Date::eSeptember and day >= 14)))) {
		return jday (month, day, year);
	}
	else {
		return kEmptyJulianRep;
	}
}

/*
 * Convert a Julian day number to its corresponding Gregorian calendar
 * date.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
 * This function not valid before that.
 *
 * (This code originally from NIHCL)
 */
void	Date::mdy (MonthOfYear* month, DayOfMonth* day, Year* year) const
{
	RequireNotNil (month);
	RequireNotNil (day);
	RequireNotNil (year);
	if (fJulianDateRep == kEmptyJulianRep) {
		*month = eEmptyMonthOfYear;
		*day = eEmptyDayOfMonth;
		*year = eEmptyYear;
		return;
	}
	JulianRepType	m;
	JulianRepType	d;
	JulianRepType	y;

	JulianRepType j = fJulianDateRep - 1721119;
	y = (((j<<2) - 1) / 146097);
	j = (j<<2) - 1 - 146097*y;
	d = (j>>2);
	j = ((d<<2) + 3) / 1461;
	d = ((d<<2) + 3 - 1461*j);
	d = (d + 4)>>2;
	m = (5*d - 3)/153;
	d = 5*d - 3 - 153*m;
	d = (d + 5)/5;
	y = (100*y + j);
	if (m < 10) {
		m += 3;
	}
	else {
		m -= 9;
		y++;
	}
	Ensure (1 <= m and m <= 12);
	*month = static_cast<MonthOfYear> (m);
	Ensure (1 <= d and d <= 31);
	*day = static_cast<DayOfMonth> (d);
	*year = static_cast<Year> (y);
}

int	Stroika::Foundation::YearDifference (const Date& lhs, const Date& rhs)
{
	Require (not lhs.empty ());		// since meaning of diff wouldn't make much sense
	Require (not rhs.empty ());		// ditto

	Date::MonthOfYear	lm	=	Date::eEmptyMonthOfYear;
	Date::DayOfMonth	ld	=	Date::eEmptyDayOfMonth;
	Date::Year			ly	=	Date::eEmptyYear;
	lhs.mdy (&lm, &ld, &ly);
	
	Date::MonthOfYear	rm	=	Date::eEmptyMonthOfYear;
	Date::DayOfMonth	rd	=	Date::eEmptyDayOfMonth;
	Date::Year			ry	=	Date::eEmptyYear;
	rhs.mdy (&rm, &rd, &ry);

	int	diff	=	ly - ry;

	if (lm < rm or (lm == rm and ld < rd)) {
		diff--;
	}
	return diff;
}

float	Stroika::Foundation::YearDifferenceF (const Date& lhs, const Date& rhs)
{
	Require (not lhs.empty ());		// since meaning of diff wouldn't make much sense
	Require (not rhs.empty ());		// ditto
	return DayDifference (lhs, rhs) / 365.25f;	//tmphack
}






/*
 ********************************************************************************
 ***************************** GetFormattedAge **********************************
 ********************************************************************************
 */
wstring	Stroika::Foundation::GetFormattedAge (const Date& birthDate, const Date& deathDate)
{
	if (birthDate.empty ()) {
		return L"?";
	}
	else {
		int	yearDiff	=	deathDate.empty ()? YearDifference (Date::GetToday (), birthDate): YearDifference (deathDate, birthDate);
		return Format (L"%d", yearDiff);
	}
}

wstring	Stroika::Foundation::GetFormattedAgeWithUnit (const Date& birthDate, const Date& deathDate, bool allowMonths, bool abbrevUnit)
{
	if (birthDate.empty ()) {
		return L"?";
	}
	else {
		int	yearDiff	=	deathDate.empty ()? YearDifference (Date::GetToday (), birthDate): YearDifference (deathDate, birthDate);
		if (yearDiff >= 0 and yearDiff < 2) {
			float	yearDiffF	=	deathDate.empty ()? YearDifferenceF (Date::GetToday (), birthDate): YearDifferenceF (deathDate, birthDate);
			int		months		=	int (yearDiffF * 12.0f + 0.4999f);
			wstring	unitBase	=	abbrevUnit? L"mo": L"month";
			return Format (L"%d %s", months, PluralizeNoun (unitBase, months).c_str ());
		}
		else {
			wstring	unitBase	=	abbrevUnit? L"yr": L"year";
			return Format (L"%d %s", yearDiff, PluralizeNoun (unitBase, yearDiff).c_str ());
		}
	}
}












/*
 ********************************************************************************
 *********************************** TimeOfDay **********************************
 ********************************************************************************
 */
TimeOfDay::TimeOfDay ():
	fTime (-1)
{
}

TimeOfDay::TimeOfDay (unsigned int t):
	fTime (-1)
{
	fTime = t;
	fTime %= 60*60*24;				// assure small enuf to fit within a day
}

TimeOfDay::TimeOfDay (const wstring& rep, LCID lcid):
	fTime (-1)
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
			Exceptions::DoThrow (FormatException ());
		}
	}
	// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
	SYSTEMTIME	sysTime;
	memset (&sysTime, 0, sizeof (sysTime));
	Verify (::VariantTimeToSystemTime (d, &sysTime));
	*this = TimeOfDay (sysTime);
}

TimeOfDay::TimeOfDay (const wstring& rep, XML):
	fTime (-1)
{
	int	hour	=	0;
	int	minute	=	0;
	int secs	=	0;
	if (::swscanf_s (rep.c_str (), L"%d:%d:d", &hour, &minute, &secs) >= 2) {
		hour = max (hour, 0);
		hour = min (hour, 23);
		minute = max (minute, 0);
		minute = min (minute, 59);
		secs = max (secs, 0);
		secs = min (secs, 59);
		fTime = (hour * 60 + minute) * 60 + secs;
	}
}

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
			BSTR	tmpStr	=	NULL;
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













/*
 ********************************************************************************
 *********************************** DateTime ***********************************
 ********************************************************************************
 */
DateTime::DateTime (const wstring& rep, LCID lcid):
	fDate (),
	fTimeOfDay ()
{
	if (not rep.empty ()) {
		DATE		d;
		(void)::memset (&d, 0, sizeof (d));
		try {
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, 0, &d));
		}
		catch (...) {
			// though COULD be time format exception?
			Exceptions::DoThrow (Date::FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fDate = Date (sysTime);
		fTimeOfDay = TimeOfDay (sysTime);
	}
}

DateTime::DateTime (const wstring& rep, XML):
	fDate (),
	fTimeOfDay ()
{
	if (not rep.empty ()) {
		int	year	=	0;
		int	month	=	0;
		int	day		=	0;
		int	hour	=	0;
		int	minute	=	0;
		int	second	=	0;
		int	tzHr	=	0;
		int	tzMn	=	0;
		int	nItems	=	::swscanf_s (rep.c_str (), L"%d-%d-%dT%d:%d:%d-%d:%d", &year, &month, &day, &hour, &minute, &second, &tzHr, &tzMn);
		if (nItems >= 3) {
			fDate = Date (::Format (L"%d-%d-%d", year, month, day), Date::eXML);
		}
		if (nItems >= 5) {
			fTimeOfDay = TimeOfDay (hour * 60 * 60 + minute * 60 + second);
		}
		if (nItems >= 8) {
			// CHECK TZ
			// REALLY - must check TZ - but must adjust value if currentmachine timezone differs from one found in file...
		}
	}
}

DateTime::DateTime (const SYSTEMTIME& sysTime):
	fDate (sysTime),
	fTimeOfDay (sysTime)
{
}

DateTime::DateTime (const FILETIME& fileTime):
	fDate (),
	fTimeOfDay ()
{
	FILETIME localTime;
	(void)::memset (&localTime, 0, sizeof (localTime));
	if (::FileTimeToLocalFileTime (&fileTime, &localTime)) {
		SYSTEMTIME sysTime;
		(void)::memset (&sysTime, 0, sizeof (sysTime));
		if (::FileTimeToSystemTime (&localTime, &sysTime)) {
			fDate= Date (sysTime);
			fTimeOfDay = TimeOfDay (sysTime);
		}
	}
}

DateTime::DateTime (time_t unixTime):
	fDate (),
	fTimeOfDay ()
{
	// From http://support.microsoft.com/kb/167296
	FILETIME	ft;
	LONGLONG ll;
	ll = Int32x32To64 (unixTime, 10000000) + 116444736000000000;
	ft.dwLowDateTime = (DWORD)ll;
	ft.dwHighDateTime = static_cast<DWORD> (ll >> 32);
	*this = DateTime (ft);
}


bool	DateTime::empty () const
{
	// Risky change so late in the game - but this logic seems wrong (and causes some trouble).
	// DateTime is NOT empty just because date part is empty. We CAN use a DateTime record to store JUST a time!
	//		-- LGP 2006-04-26
	return fDate.empty () and fTimeOfDay.empty ();
}

DateTime	DateTime::Now ()
{
	SYSTEMTIME	st;
	memset (&st, 0, sizeof (st));
	::GetLocalTime (&st);
	return DateTime (st);
}

wstring	DateTime::Format (LCID lcid) const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		wstring	r	=	fDate.Format (lcid);
		Assert (not r.empty ());
		wstring	tod	=	fTimeOfDay.Format (lcid);
		if (not tod.empty ()) {
			r += L" " + tod;
		}
		return r;
	}
}

wstring	DateTime::Format4XML () const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		wstring	r	=	fDate.Format4XML ();
		if (not fTimeOfDay.empty ()) {
			// be sure using DateWithOptionalTime

			// someting like append T22:33:11 - apx...
			wchar_t	buf[1024];
			buf[0] = 0;
			unsigned int	t	=	fTimeOfDay.GetAsSecondsCount ();
			struct tm	 temp;
			memset (&temp, 0, sizeof (temp));
			temp.tm_hour = t/(60*60);
			temp.tm_min = (t - temp.tm_hour * 60 * 60) / 60;
			temp.tm_sec = (t - temp.tm_hour * 60 * 60 - temp.tm_min * 60);
			wcsftime (buf, NEltsOf (buf), L"%H:%M:%S", &temp);

			wstring	tzBiasString;
			{
				TIME_ZONE_INFORMATION	tzInfo;
				memset (&tzInfo, 0, sizeof (tzInfo));
				(void)::GetTimeZoneInformation (&tzInfo);
				int unsignedBias	=	abs (tzInfo.Bias);
				int	hrs	=	unsignedBias / 60;
				int mins = unsignedBias - hrs * 60;
				tzBiasString = ::Format (L"%s%.2d:%.2d", (tzInfo.Bias >= 0? L"-": L"+"), hrs, mins);
			}
			r += wstring (L"T") + buf + tzBiasString;
		}
		Assert (DateTime (r, eXML) == *this);
		return r;
	}
}

DateTime::operator SYSTEMTIME () const
{
	SYSTEMTIME	d	=	(SYSTEMTIME)fDate;
	SYSTEMTIME	t	=	(SYSTEMTIME)fTimeOfDay;
	SYSTEMTIME	r	=	d;
	r.wHour = t.wHour;
	r.wMinute = t.wMinute;
	r.wSecond = t.wSecond;
	r.wMilliseconds = t.wMilliseconds;
	return r;
}

Date::JulianRepType	DateTime::DaysSince () const
{
	int	r	=	DayDifference (Date::GetToday (), *this);
	if (r < 0) {
		return 0;
	}
	else {
		return r;
	}
}

time_t	DateTime::GetUNIXEpochTime () const
{
	SYSTEMTIME	st	=	*this;
	struct tm tm;
	memset(&tm, 0, sizeof(tm));
	tm.tm_year = st.wYear - 1900;
	tm.tm_mon = st.wMonth - 1;
	tm.tm_mday = st.wDay;
	tm.tm_hour = st.wHour;
	tm.tm_min = st.wMinute;
	tm.tm_sec = st.wSecond;
	return mktime (&tm);
}

void	DateTime::SetDate (const Date& d)
{
	fDate = d;
}

void		DateTime::SetTimeOfDay (const TimeOfDay& tod)
{
	fTimeOfDay = tod;
}

bool Stroika::Foundation::operator< (const DateTime& lhs, const DateTime& rhs)
{
	if (lhs.GetDate () < rhs.GetDate ()) {
		return true;
	}
	if (lhs.GetDate () == rhs.GetDate ()) {
		return lhs.GetTimeOfDay () < rhs.GetTimeOfDay ();
	}
	return false;
}

bool Stroika::Foundation::operator<= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs < rhs or lhs == rhs;
}

bool Stroika::Foundation::operator> (const DateTime& lhs, const DateTime& rhs)
{
	if (lhs.GetDate () > rhs.GetDate ()) {
		return true;
	}
	if (lhs.GetDate () == rhs.GetDate ()) {
		return lhs.GetTimeOfDay () > rhs.GetTimeOfDay ();
	}
	return false;
}

bool Stroika::Foundation::operator>= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs == rhs or lhs > rhs;
}

bool Stroika::Foundation::operator== (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.GetDate () == rhs.GetDate () and lhs.GetTimeOfDay () == rhs.GetTimeOfDay ();
}

bool Stroika::Foundation::operator!= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.GetDate () != rhs.GetDate () or lhs.GetTimeOfDay () != rhs.GetTimeOfDay ();
}







/*
 ********************************************************************************
 **************************** DateUtils::Duration *******************************
 ********************************************************************************
 */
DateUtils::Duration::Duration ()
	: fDurationRep ()
{
}

DateUtils::Duration::Duration (const wstring& durationStr)
	: fDurationRep (WideStringToASCII (durationStr))
{
	(void)ParseTime_ (fDurationRep);	// call for the side-effect of throw if bad format src string
}

DateUtils::Duration::Duration (time_t duration)
	: fDurationRep (UnParseTime_ (duration))
{
}

void	DateUtils::Duration::clear ()
{
	fDurationRep.clear ();
}

bool	DateUtils::Duration::empty () const
{
	return fDurationRep.empty ();
}

DateUtils::Duration::operator time_t () const
{
	return ParseTime_ (fDurationRep);		// could cache value, but ... well - maybe not worth the effort/cost of extra data etc.
}

DateUtils::Duration::operator wstring () const
{
	return ASCIIStringToWide (fDurationRep);
}

namespace	{
	string::const_iterator	SkipWhitespace_ (string::const_iterator i, string::const_iterator end)
		{
			// GNU LIBC code (header) says that whitespace is allowed (though I've found no external docs to support this).
			// Still - no harm in accepting this - so long as we don't ever generate it...
			while (i != end and isspace (*i)) {
				++i;
			}
			return i;
		}
	string::const_iterator	FindFirstNonDigit_ (string::const_iterator i, string::const_iterator end)
		{
			while (i != end and isdigit (*i)) {
				++i;
			}
			return i;
		}

	const	time_t	kSecondsPerMinute	=	60;
	const	time_t	kSecondsPerHour		=	kSecondsPerMinute * 60;
	const	time_t	kSecondsPerDay		=	kSecondsPerHour * 24;
	const	time_t	kSecondsPerWeek		=	kSecondsPerDay * 7;
	const	time_t	kSecondsPerMonth	=	kSecondsPerDay * 30;
	const	time_t	kSecondsPerYear		=	kSecondsPerDay * 365;
}

wstring DateUtils::Duration::PrettyPrint () const
{
	time_t	t			=	static_cast<time_t> (*this);
	bool	isNeg		=	(t < 0);
	time_t	timeLeft	=	t < 0? -t : t;
	wstring	result;
	result.reserve (50);
	if (timeLeft >= kSecondsPerYear) {
		time_t	nYears = timeLeft / kSecondsPerYear;
		if (nYears != 0) {
			if (not result.empty ()) {
				result += L", ";
			}
			result += Format (L"%d ", nYears) + StringUtils::PluralizeNoun (L"year", static_cast<int> (nYears));
			timeLeft -= nYears * kSecondsPerYear;
		}
	}
	if (timeLeft >= kSecondsPerMonth) {
		time_t	nMonths = timeLeft / kSecondsPerMonth;
		if (nMonths != 0) {
			if (not result.empty ()) {
				result += L", ";
			}
			result += Format (L"%d ", nMonths) + StringUtils::PluralizeNoun (L"month", static_cast<int> (nMonths));
			timeLeft -= nMonths * kSecondsPerMonth;
		}
	}
	if (timeLeft >= kSecondsPerDay) {
		time_t	nDays = timeLeft / kSecondsPerDay;
		if (nDays != 0) {
			if (not result.empty ()) {
				result += L", ";
			}
			result += Format (L"%d ", nDays) + StringUtils::PluralizeNoun (L"day", static_cast<int> (nDays));
			timeLeft -= nDays * kSecondsPerDay;
		}
	}
	if (timeLeft != 0) {
		if (timeLeft >= kSecondsPerHour) {
			time_t	nHours = timeLeft / kSecondsPerHour;
			if (nHours != 0) {
				if (not result.empty ()) {
					result += L", ";
				}
				result += Format (L"%d ", nHours) + StringUtils::PluralizeNoun (L"hour", static_cast<int> (nHours));
				timeLeft -= nHours * kSecondsPerHour;
			}
		}
		if (timeLeft >= kSecondsPerMinute) {
			time_t	nMinutes = timeLeft / kSecondsPerMinute;
			if (nMinutes != 0) {
				if (not result.empty ()) {
					result += L", ";
				}
				result += Format (L"%d ", nMinutes) + StringUtils::PluralizeNoun (L"minute", static_cast<int> (nMinutes));
				timeLeft -= nMinutes * kSecondsPerMinute;
			}
		}
		if (timeLeft != 0) {
			if (not result.empty ()) {
				result += L", ";
			}
			result += Format (L"%d ", timeLeft) + StringUtils::PluralizeNoun (L"second", static_cast<int> (timeLeft));
		}
	}
	if (isNeg) {
		result = L"-" + result;
	}
	return result;
}

time_t	DateUtils::Duration::ParseTime_ (const string& s)
{
	if (s.empty ()) {
		return 0;
	}
	time_t	curVal	=	0;
	bool	isNeg	=	false;
	// compute and throw if bad...
	string::const_iterator	i	=	SkipWhitespace_ (s.begin (), s.end ());
	if  (*i == '-') {
		isNeg = true;
		i = SkipWhitespace_ (i+1, s.end ());
	}
	if (*i == 'P') {
		i = SkipWhitespace_ (i+1, s.end ());
	}
	else {
		Exceptions::DoThrow (FormatException ());
	}
	bool	timePart	=	false;
	while (i != s.end ()) {
		if (*i == 'T') {
			timePart = true;
			i = SkipWhitespace_ (i+1, s.end ());
			continue;
		}
		string::const_iterator	firstDigitI	=	i;
		string::const_iterator	lastDigitI	=	FindFirstNonDigit_ (i, s.end ());
		if (lastDigitI == s.end ()) {
			Exceptions::DoThrow (FormatException ());
		}
		if (firstDigitI == lastDigitI) {
			Exceptions::DoThrow (FormatException ());
		}
		int	n	=	atoi (string (firstDigitI, lastDigitI).c_str ());
		switch (*lastDigitI) {
			case	'Y':	curVal += n * kSecondsPerYear; break;
			case	'M':	curVal += n * (timePart? kSecondsPerMinute: kSecondsPerMonth); break;
			case	'W':	curVal += n * kSecondsPerWeek; break;
			case	'D':	curVal += n * kSecondsPerDay; break;
			case	'H':	curVal += n * kSecondsPerHour; break;
			case	'S':	curVal += n; break;
		}
		i = SkipWhitespace_ (lastDigitI + 1, s.end ());
	}
	return isNeg ? -curVal : curVal;
}

string	DateUtils::Duration::UnParseTime_ (time_t t)
{
	bool	isNeg		=	(t < 0);
	time_t	timeLeft	=	t < 0? -t : t;
	string	result	=	"P";
	result.reserve (50);
	if (timeLeft >= kSecondsPerYear) {
		time_t	nYears = timeLeft / kSecondsPerYear;
		if (nYears != 0) {
			result += Format ("%dY", nYears);
			timeLeft -= nYears * kSecondsPerYear;
		}
	}
	if (timeLeft >= kSecondsPerMonth) {
		time_t	nMonths = timeLeft / kSecondsPerMonth;
		if (nMonths != 0) {
			result += Format ("%dM", nMonths);
			timeLeft -= nMonths * kSecondsPerMonth;
		}
	}
	if (timeLeft >= kSecondsPerDay) {
		time_t	nDays = timeLeft / kSecondsPerDay;
		if (nDays != 0) {
			result += Format ("%dD", nDays);
			timeLeft -= nDays * kSecondsPerDay;
		}
	}
	if (timeLeft != 0) {
		result += "T";
		if (timeLeft >= kSecondsPerHour) {
			time_t	nHours = timeLeft / kSecondsPerHour;
			if (nHours != 0) {
				result += Format ("%dH", nHours);
				timeLeft -= nHours * kSecondsPerHour;
			}
		}
		if (timeLeft >= kSecondsPerMinute) {
			time_t	nMinutes = timeLeft / kSecondsPerMinute;
			if (nMinutes != 0) {
				result += Format ("%dM", nMinutes);
				timeLeft -= nMinutes * kSecondsPerMinute;
			}
		}
		if (timeLeft != 0) {
			result += Format ("%dS", timeLeft);
		}
	}
	if (isNeg) {
		result = "-" + result;
	}
	return result;
}

