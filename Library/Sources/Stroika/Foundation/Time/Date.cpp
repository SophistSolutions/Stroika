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

#include	"Date.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Memory;
using	namespace	Stroika::Foundation::Time;

using	Debug::TraceContextBumper;

using	namespace	Time;










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

#if		qPlatform_Windows
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
#endif

#if		qPlatform_POSIX
namespace	{
	// VERY PRIMITIVE UNIX
	void convert_iso8601 (const char *time_string, struct tm *tm_data)
		{
			tzset();

			struct tm ctime;
			memset(&ctime, 0, sizeof(struct tm));
			strptime(time_string, "%FT%T%z", &ctime);

			long ts = mktime(&ctime) - timezone;
			localtime_r (&ts, tm_data);
		}
}
#endif

Date::Date (const wstring& rep, XML)
	: fJulianDateRep (kEmptyJulianRep)
{
// SHOULD TAKE INTO ACCOUNT TIMEZONE FIELD - IF ANY - AND NOT CURRENT TIMEZONE!!! (LOCALE_USER_DEFAULT IS WRONG) -  LGP 2005-10-31
	if (not rep.empty ()) {
#if		qPlatform_Windows
		DATE		d;
		(void)::memset (&d, 0, sizeof (d));
		try {
			ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), LOCALE_USER_DEFAULT, VAR_DATEVALUEONLY, &d));
		}
		catch (...) {
			Execution::DoThrow (FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
#elif	qPlatform_POSIX
		struct tm tm;
		memset(&tm, 0, sizeof(struct tm));
// horrible hack - very bad... but hopefully gets us limping along...
		string tmp = WideStringToASCII (rep);
		convert_iso8601 (tmp.c_str (), &tm);
		fJulianDateRep = Safe_jday (MonthOfYear (tm.tm_mon+1), DayOfMonth (tm.tm_mday), Year (tm.tm_year+1900));
#else
		AssertNotImplemented ();
#endif
	}
}

Date::Date (const wstring& rep, Javascript)
	: fJulianDateRep (kEmptyJulianRep)
{
#if		qPlatform_Windows
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
			Execution::DoThrow (FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
	}
#else
	AssertNotImplemented ();
#endif
}

Date::Date (const wstring& rep)
	: fJulianDateRep (kEmptyJulianRep)
{
	if (not rep.empty ()) {
		#if		qPlatform_Wiindows
			LCID lcid = LOCALE_USER_DEFAULT;
			DATE		d;
			(void)::memset (&d, 0, sizeof (d));
			try {
				ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, VAR_DATEVALUEONLY, &d));
			}
			catch (...) {
				Execution::DoThrow (FormatException ());
			}
			// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
			SYSTEMTIME	sysTime;
			memset (&sysTime, 0, sizeof (sysTime));
			Verify (::VariantTimeToSystemTime (d, &sysTime));
			fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
		#else
			AssertNotImplemented ();
		#endif
	}
}

#if		qPlatform_Windows
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
			Execution::DoThrow (FormatException ());
		}
		// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
		SYSTEMTIME	sysTime;
		memset (&sysTime, 0, sizeof (sysTime));
		Verify (::VariantTimeToSystemTime (d, &sysTime));
		fJulianDateRep = Safe_jday (MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay), Year (sysTime.wYear));
	}
}
#endif

Date	Date::GetToday ()
{
#if		qPlatform_Windows
	SYSTEMTIME	st;
	memset (&st, 0, sizeof (st));
	::GetLocalTime (&st);

	return Date (Year (st.wYear), MonthOfYear (st.wMonth), DayOfMonth (st.wDay));
#else
	Assert (false);
	return Date ();
#endif
}

wstring	Date::Format () const
{
	#if		qPlatform_Windows
		return Format (LOCALE_USER_DEFAULT);
	#else
		AssertNotImplemented ();
		return wstring ();
	#endif
}

#if		qPlatform_Windows
wstring	Date::Format (LCID lcid) const
{
	SYSTEMTIME	st	=	*this;
	int	nTChars	=	::GetDateFormat (lcid, DATE_SHORTDATE, &st, nullptr, nullptr, 0);
	if (nTChars == 0) {
		return wstring ();
	}
	else {
		SmallStackBuffer<TCHAR> buf (nTChars + 1);
		(void)::GetDateFormat (lcid, DATE_SHORTDATE, &st, nullptr, buf, nTChars+1);
		return TString2Wide (static_cast<const TCHAR*> (buf));
	}
}

wstring	Date::Format (const TString& format, LCID lcid) const
{
	SYSTEMTIME	st	=	*this;
	int	nTChars	=	::GetDateFormat (lcid, 0, &st, format.c_str (), nullptr, 0);
	if (nTChars == 0) {
		return wstring ();
	}
	else {
		SmallStackBuffer<TCHAR> buf (nTChars + 1);
		(void)::GetDateFormat (lcid, 0, &st, format.c_str (), buf, nTChars+1);
		return TString2Wide (static_cast<const TCHAR*> (buf));
	}
}
#endif

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
		::swprintf (buf, NEltsOf (buf), L"%04d-%02d-%02d", y, m, d);
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
		::swprintf (buf, NEltsOf (buf), L"%02d/%02d/%04d", m, d, y);
		return buf;
	}
}

#if		qPlatform_Windows
wstring	Date::LongFormat (LCID lcid) const
{
	SYSTEMTIME	st	=	*this;
	int	nTChars	=	::GetDateFormat (lcid, DATE_LONGDATE, &st, nullptr, nullptr, 0);
	if (nTChars == 0) {
		return wstring ();
	}
	else {
		SmallStackBuffer<TCHAR> buf (nTChars + 1);
		(void)::GetDateFormat (lcid, DATE_LONGDATE, &st, nullptr, buf, nTChars+1);
		return TString2Wide (static_cast<const TCHAR*> (buf));
	}
}
#endif

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

#if		qPlatform_Windows
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
#endif

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
	RequireNotNull (month);
	RequireNotNull (day);
	RequireNotNull (year);
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

int	Time::YearDifference (const Date& lhs, const Date& rhs)
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

float	Time::YearDifferenceF (const Date& lhs, const Date& rhs)
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
wstring	Time::GetFormattedAge (const Date& birthDate, const Date& deathDate)
{
	if (birthDate.empty ()) {
		return L"?";
	}
	else {
		int	yearDiff	=	deathDate.empty ()? YearDifference (Date::GetToday (), birthDate): YearDifference (deathDate, birthDate);
		return Format (L"%d", yearDiff);
	}
}

wstring	Time::GetFormattedAgeWithUnit (const Date& birthDate, const Date& deathDate, bool allowMonths, bool abbrevUnit)
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
			return Format (L"%d %s", months, Linguistics::PluralizeNoun (unitBase, months).c_str ());
		}
		else {
			wstring	unitBase	=	abbrevUnit? L"yr": L"year";
			return Format (L"%d %s", yearDiff, Linguistics::PluralizeNoun (unitBase, yearDiff).c_str ());
		}
	}
}











