/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<ctime>
#include	<sstream>

#if		qPlatform_Windows
	#include	<atlbase.h>		// For CComBSTR
#elif	qPlatform_POSIX
#endif

#include	"../Characters/Format.h"
#include	"../Debug/Assertions.h"
#include	"../Execution/Exceptions.h"
#include	"../Memory/SmallStackBuffer.h"
#include	"../Linguistics/Words.h"

#include	"DateTime.h"

using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Memory;
using	namespace	Stroika::Foundation::Time;

using	Debug::TraceContextBumper;




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
	Date	mkDate_ (const SYSTEMTIME& sysTime)
		{
			return Date (Year (sysTime.wYear), MonthOfYear (sysTime.wMonth), DayOfMonth (sysTime.wDay));
		}
}
#endif



namespace	{
#if		qPlatform_Windows
	SYSTEMTIME	toSysTime_ (TimeOfDay tod)
		{
			SYSTEMTIME	t;
			memset (&t, 0, sizeof (t));
			if (not tod.empty ()) {
				unsigned int	seconds	=	tod.GetAsSecondsCount ();
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
}


namespace	{
#if		qPlatform_Windows
	SYSTEMTIME toSYSTEM_ (const Date& date)
		{
			SYSTEMTIME	st;
			memset (&st, 0, sizeof (st));
			MonthOfYear	m	=	eEmptyMonthOfYear;
			DayOfMonth	d	=	eEmptyDayOfMonth;
			Year		y	=	eEmptyYear;
			date.mdy (&m, &d, &y);
			st.wYear = y;
			st.wMonth = m;
			st.wDay = d;
			return st;
		}
#endif
}









/*
 ********************************************************************************
 *********************************** DateTime ***********************************
 ********************************************************************************
 */
namespace	{
	// Once we have all our compilers supporting constexp - this can go in header (Date.inl) - and then be shared
	// across OBJS
	const	Date	kMin_	=	Date (Date::JulianRepType (Date::kMinJulianRep));	//year eFirstYear  - January
	const	Date	kMax_	=	Date (Date::JulianRepType (UINT_MAX-1));

	const	TimeOfDay	kMinT_	=	TimeOfDay (0);
	const	TimeOfDay	kMaxT_	=	TimeOfDay (24 * 60 * 60 - 1);
}
const	DateTime	DateTime::kMin	=	DateTime (kMin_, kMinT_);		//	unclear if this should use empty time or midnight?
const	DateTime	DateTime::kMax	=	DateTime (kMax_, kMaxT_);		//	unclear if this should use end of day time or not?

DateTime::DateTime (time_t unixTime, Timezone tz)
	: fTimezone_ (tz)
	, fDate_ ()
	, fTimeOfDay_ ()
{
	#if		qPlatform_Windows
		// From http://support.microsoft.com/kb/167296
		FILETIME	ft;
		LONGLONG ll;
		ll = Int32x32To64 (unixTime, 10000000) + 116444736000000000;
		ft.dwLowDateTime = (DWORD)ll;
		ft.dwHighDateTime = static_cast<DWORD> (ll >> 32);
		*this = DateTime (ft, tz);
	#else
		struct	tm	tmTime;
		memset (&tmTime, 0, sizeof (tmTime));
		if (tz == eLocalTime_TZ or tz == eUnknown_TZ) {
			(void)::localtime_r  (&unixTime, &tmTime);
		}
		else {
			(void)::gmtime_r  (&unixTime, &tmTime);
		}
		fDate_ = Date (Year (tmTime.tm_year+1900), MonthOfYear (tmTime.tm_mon+1), DayOfMonth (tmTime.tm_mday));
		fTimeOfDay_ = TimeOfDay (tmTime.tm_sec + (tmTime.tm_min * 60) + (tmTime.tm_hour * 60 * 60));
		Assert (false);
	#endif
}

DateTime::DateTime (struct tm tmTime, Timezone tz)
	: fTimezone_ (tz)
	, fDate_ (Year (tmTime.tm_year + 1900), MonthOfYear (tmTime.tm_mon + 1), DayOfMonth (tmTime.tm_mday))
	, fTimeOfDay_ ((tmTime.tm_hour * 60 + tmTime.tm_min) * 60 + tmTime.tm_sec)
{
}

#if		qPlatform_Windows
DateTime::DateTime (const SYSTEMTIME& sysTime, Timezone tz)
	: fTimezone_ (tz)
	, fDate_ (mkDate_ (sysTime))
	, fTimeOfDay_ (mkTimeOfDay_ (sysTime))
{
}

DateTime::DateTime (const FILETIME& fileTime, Timezone tz)
	: fTimezone_ (tz)
	, fDate_ ()
	, fTimeOfDay_ ()
{
	/*
	 * Not sure we should call FileTimeToLocalFileTime () ??? _ That maybe forces tz to localtime?
	 *		-- LGP 2011-10-08
	 */
	FILETIME localTime;
	(void)::memset (&localTime, 0, sizeof (localTime));
	if (::FileTimeToLocalFileTime (&fileTime, &localTime)) {
		SYSTEMTIME sysTime;
		(void)::memset (&sysTime, 0, sizeof (sysTime));
		if (::FileTimeToSystemTime (&localTime, &sysTime)) {
			fDate_= mkDate_ (sysTime);
			fTimeOfDay_ = mkTimeOfDay_ (sysTime);
		}
	}
}
#endif

DateTime	DateTime::Parse (const wstring& rep, PrintFormat pf)
{
	if (rep.empty ()) {
		return Date ();
	}
	switch (pf) {
		case	eCurrentLocale_PF: {
			#if			qPlatform_Windows
				Assert (Parse (rep, LOCALE_USER_DEFAULT) == Parse (rep, locale ()));	// not a REAL assert, but for debugging - to see what diffs there are - probably none
																						// added to test 2011-10-07
				return Parse (rep, LOCALE_USER_DEFAULT);
			#else
				return Parse (rep, locale ());
			#endif
			return DateTime ();
		}
		break;
		case	eXML_PF: {
			int	year	=	0;
			int	month	=	0;
			int	day		=	0;
			int	hour	=	0;
			int	minute	=	0;
			int	second	=	0;
			int	tzHr	=	0;
			int	tzMn	=	0;
			#pragma	warning (push)
			#pragma	warning (4 : 4996)		// MSVC SILLY WARNING ABOUT USING swscanf_s
			int	nItems	=	0;
			bool	tzKnown	=	false;
			if (rep[rep.length ()-1] == 'Z') {
				nItems = ::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%dZ", &year, &month, &day, &hour, &minute, &second);
				tzKnown = true;
			}
			else {
				nItems = ::swscanf (rep.c_str (), L"%d-%d-%dT%d:%d:%d-%d:%d", &year, &month, &day, &hour, &minute, &second, &tzHr, &tzMn);
				tzKnown = (nItems >= 7);
			}
			#pragma warning (pop)
			Date		d;
			TimeOfDay	t;
			if (nItems >= 3) {
				d = Date::Parse (::Format (L"%d-%d-%d", year, month, day), Date::eXML_PF);
			}
			if (nItems >= 5) {
				t = TimeOfDay (hour * 60 * 60 + minute * 60 + second);
			}
			Timezone	tz	=	eUnknown_TZ;
			if (tzKnown) {
				tz = eGMT_TZ;	// really wrong - should map given time to UTC??? - check HR value ETC

				// CHECK TZ
				// REALLY - must check TZ - but must adjust value if currentmachine timezone differs from one found in file...
				// not sure what todo if READ tz doesn't match localtime? Maybe convert to GMT??
			}
			else {
				tz = eLocalTime_TZ;
			}
			return DateTime (d, t, tz);
		}
		break;
		default: {
			AssertNotReached ();
			return Date ();
		}
		break;
	}
}

DateTime	DateTime::Parse (const wstring& rep, const locale& l)
{
	if (rep.empty ()) {
		return Date ();
	}
	const time_get<wchar_t>& tmget = use_facet <time_get<wchar_t> > (l);
	ios::iostate state	=	ios::goodbit;
	wistringstream iss (rep);
	istreambuf_iterator<wchar_t> itbegin (iss);  // beginning of iss
	istreambuf_iterator<wchar_t> itend;          // end-of-stream
	tm when;
	memset (&when, 0, sizeof (when));
	tmget.get_date (itbegin, itend, iss, state, &when);
	#if		qCompilerAndStdLib_LocaleDateParseBugOffBy1900OnYear
		when.tm_year -= 1900;
	#endif
	return DateTime (when);
}

#if		qPlatform_Windows
DateTime	DateTime::Parse (const wstring& rep, LCID lcid)
{
	if (rep.empty ()) {
		return Date ();
	}
	DATE		d;
	(void)::memset (&d, 0, sizeof (d));
	try {
		ThrowIfErrorHRESULT (::VarDateFromStr (CComBSTR (rep.c_str ()), lcid, 0, &d));
	}
	catch (...) {
		// though COULD be time format exception?
		Execution::DoThrow (Date::FormatException ());
	}
	// SHOULD CHECK ERR RESULT (not sure if/when this can fail - so do a Verify for now)
	SYSTEMTIME	sysTime;
	memset (&sysTime, 0, sizeof (sysTime));
	Verify (::VariantTimeToSystemTime (d, &sysTime));
	return DateTime (mkDate_ (sysTime),  mkTimeOfDay_ (sysTime));
}
#endif

DateTime	DateTime::AsLocalTime () const
{
	if (GetTimezone () == eGMT_TZ) {
		DateTime	tmp	=	AddSeconds (-GetLocaltimeToGMTOffset ());
		return DateTime (tmp.GetDate (), tmp.GetTimeOfDay (), eLocalTime_TZ);
	}
	else {
		// treat BOTH unknown and localetime as localtime
		return *this;
	}
}

DateTime	DateTime::AsGMT () const
{
	if (GetTimezone () == eGMT_TZ) {
		return *this;
	}
	else {
		DateTime	tmp	=	AddSeconds (GetLocaltimeToGMTOffset ());
		return DateTime (tmp.GetDate (), tmp.GetTimeOfDay (), eGMT_TZ);
	}
}

DateTime	DateTime::Now ()
{
	#if		qPlatform_Windows
		SYSTEMTIME	st;
		memset (&st, 0, sizeof (st));
		::GetLocalTime (&st);
		return DateTime (st, eLocalTime_TZ);
	#elif	qPlatform_POSIX
		return DateTime (time (nullptr), eUnknown_TZ);
	#else
		AssertNotImplemented ();
		return DateTime ();
	#endif
}

wstring	DateTime::Format (PrintFormat pf) const
{
	if (empty ()) {
		return wstring ();
	}
	switch (pf) {
		case	eCurrentLocale_PF: {
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
		break;
		case	eXML_PF: {
			wstring	r	=	fDate_.Format (Date::eXML_PF);
			if (not fTimeOfDay_.empty ()) {
				// be sure using DateWithOptionalTime

				// something like append T22:33:11 - apx...
				wchar_t	buf[1024];
				buf[0] = 0;
				unsigned int	t	=	fTimeOfDay_.GetAsSecondsCount ();
				struct tm	 temp;
				memset (&temp, 0, sizeof (temp));
				temp.tm_hour = t/(60*60);
				temp.tm_min = (t - temp.tm_hour * 60 * 60) / 60;
				temp.tm_sec = (t - temp.tm_hour * 60 * 60 - temp.tm_min * 60);
				wcsftime (buf, NEltsOf (buf), L"%H:%M:%S", &temp);

				wstring	tzBiasString;
				if (GetTimezone () == eGMT_TZ) {
					tzBiasString = L"Z";
				}
				else {
					#if		1
						{
							// TRY TODO PORTABLY...
							time_t	tzBias		=	GetLocaltimeToGMTOffset ();
							int minuteBias		=	abs (static_cast<int> (tzBias)) / 60;
							int	hrs				=	minuteBias / 60;
							int mins			=	minuteBias - hrs * 60;
							tzBiasString = ::Format (L"%s%.2d:%.2d", (tzBias < 0? L"-": L"+"), hrs, mins);
						}
					#elif	qPlatform_Windows
						TIME_ZONE_INFORMATION	tzInfo;
						memset (&tzInfo, 0, sizeof (tzInfo));
						(void)::GetTimeZoneInformation (&tzInfo);
						int unsignedBias	=	abs (tzInfo.Bias);
						int	hrs	=	unsignedBias / 60;
						int mins = unsignedBias - hrs * 60;
						tzBiasString = ::Format (L"%s%.2d:%.2d", (tzInfo.Bias >= 0? L"-": L"+"), hrs, mins);
					#elif	qPlatform_POSIX
						//AssertNotImplemented ();
						// WRONG - but let things limp along for a little while...
						//		--:LGP 2011-09-28
					#else
						AssertNotImplemented ();
					#endif
				}
				r += wstring (L"T") + buf + tzBiasString;
			}
			#if		qDebug
				{
					// TODO:
					//		This probably shouldn't be needed!!! - think through more carefully.
					//			--LGP 2011-10-07
					DateTime	parsed	=	DateTime::Parse (r, eXML_PF);
					if (parsed.GetTimezone () != GetTimezone ()) {
						parsed = DateTime (parsed.GetDate (), parsed.GetTimeOfDay (), parsed.GetTimezone ());
					}
					Assert (parsed == *this);
				}
			#endif
			return r;
		}
		break;
		default: {
			AssertNotReached ();
			return wstring ();
		}
		break;
	}
}

wstring	DateTime::Format (const locale& l) const
{
	if (empty ()) {
		return wstring ();
	}
	// http://new.cplusplus.com/reference/std/locale/time_put/put/
	const time_put<wchar_t>& tmput = use_facet <time_put<wchar_t> > (l);
	tm when	=	As<struct tm> ();
	wostringstream oss;
	// Read docs - not sure how to use this to get the local-appropriate format
	// %X MAYBE just what we want  - locale DEPENDENT!!!
	wchar_t pattern[]=L"%x%X";
	tmput.put (oss, oss, ' ', &when, StartOfArray (pattern), EndOfArray (pattern));
	return oss.str ();
}

#if		qPlatform_Windows
wstring	DateTime::Format (LCID lcid) const
{
	if (empty ()) {
		return wstring ();
	}
	else {
		wstring	r	=	fDate_.Format (lcid);
		Assert (not r.empty ());
		wstring	tod	=	fTimeOfDay_.Format (lcid);
		if (not tod.empty ()) {
			r += L" " + tod;
		}
		return r;
	}
}
#endif

Date::JulianRepType	DateTime::DaysSince () const
{
	int	r	=	DayDifference (GetToday (), As<Date> ());
	if (r < 0) {
		return 0;
	}
	else {
		return r;
	}
}

#if		qPlatform_Windows
namespace	{
	time_t	OLD_GetUNIXEpochTime_ (const DateTime& dt)
		{
			SYSTEMTIME	st	=	dt.As<SYSTEMTIME> ();
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
}
#endif


template	<>
	time_t	DateTime::As () const
		{
			struct tm tm;
			memset(&tm, 0, sizeof(tm));
			tm.tm_year = fDate_.GetYear () - 1900;
			tm.tm_mon = fDate_.GetMonth () - 1;
			tm.tm_mday = fDate_.GetDayOfMonth ();
			unsigned int	totalSecondsRemaining	=	fTimeOfDay_.GetAsSecondsCount ();
			tm.tm_hour = totalSecondsRemaining / (60 * 60);
			totalSecondsRemaining -= tm.tm_hour * 60 * 60;
			tm.tm_min = totalSecondsRemaining / 60;
			totalSecondsRemaining -= tm.tm_min * 60;
			tm.tm_sec = totalSecondsRemaining;
			time_t	result	=	mktime (&tm);
			#if		qPlatform_Windows
			Ensure (result == OLD_GetUNIXEpochTime_ (*this));		// OLD WINDOZE code was WRONG - neglecting the coorect for mktime () timezone nonsense
			#endif
			/*
			 * This is PURELY to correct for the fact that mktime() uses the current timezone - and has NOTHING todo with the timezone assocaited with teh given
			 * DateTime() object.
			 */
			result += Time::GetLocaltimeToGMTOffset ();
			return result;
		}

template	<>
	tm	DateTime::As () const
		{
			struct tm tm;
			memset(&tm, 0, sizeof(tm));
			tm.tm_year = fDate_.GetYear () - 1900;
			tm.tm_mon = fDate_.GetMonth () - 1;
			tm.tm_mday = fDate_.GetDayOfMonth ();
			unsigned int	totalSecondsRemaining	=	fTimeOfDay_.GetAsSecondsCount ();
			tm.tm_hour = totalSecondsRemaining / (60 * 60);
			totalSecondsRemaining -= tm.tm_hour * 60 * 60;
			tm.tm_min = totalSecondsRemaining / 60;
			totalSecondsRemaining -= tm.tm_min * 60;
			tm.tm_sec = totalSecondsRemaining;
			Ensure (0 >= tm.tm_hour and tm.tm_hour <= 23);
			Ensure (0 >= tm.tm_min and tm.tm_min <= 59);
			Ensure (0 >= tm.tm_sec and tm.tm_sec <= 59);
			return tm;
		}

#if		qPlatform_Windows
template	<>
	SYSTEMTIME	DateTime::As () const
		{
			// CAN GET RID OF toSYSTEM_/toSysTime_ and just inline logic here...
			SYSTEMTIME	d	=	toSYSTEM_ (fDate_);
			SYSTEMTIME	t	=	toSysTime_ (fTimeOfDay_);
			SYSTEMTIME	r	=	d;
			r.wHour = t.wHour;
			r.wMinute = t.wMinute;
			r.wSecond = t.wSecond;
			r.wMilliseconds = t.wMilliseconds;
			return r;
		}
#endif

void	DateTime::SetDate (const Date& d)
{
	fDate_ = d;
}

void		DateTime::SetTimeOfDay (const TimeOfDay& tod)
{
	fTimeOfDay_ = tod;
}

DateTime	DateTime::AddDays (int days) const
{
	Date	d	=	GetDate ();
	d = d.AddDays (days);
	return DateTime (d, GetTimeOfDay (), GetTimezone ());
}

DateTime	DateTime::AddSeconds (time_t seconds) const
{
	/* TODO - SHOULD BE MORE CAREFUL ABOUT OVERFLOW */
	time_t	n	=	GetTimeOfDay ().GetAsSecondsCount ();
	n += seconds;
	int	dayDiff	=	0;
	if (n < 0) {
		dayDiff = int (- (-n + time_t (TimeOfDay::kMaxSecondsPerDay) - 1) / time_t (TimeOfDay::kMaxSecondsPerDay));
		Assert (dayDiff < 0);
	}
	n -= dayDiff * TimeOfDay::kMaxSecondsPerDay;
	Assert (n >= 0);

	// Now see if we overflowed
	if (n > TimeOfDay::kMaxSecondsPerDay) {
		Assert (dayDiff == 0);
		dayDiff = int (n / time_t (TimeOfDay::kMaxSecondsPerDay));
		n -= dayDiff * TimeOfDay::kMaxSecondsPerDay;
	}
	Assert (n >= 0);

	Ensure (0 <= n and n < TimeOfDay::kMaxSecondsPerDay);
	return DateTime (GetDate ().AddDays (dayDiff), TimeOfDay (n), GetTimezone ());
}

int	DateTime::Compare (const DateTime& rhs) const
{
	if (empty ()) {
		return rhs.empty ()? 0: -1;
	}
	else {
		if (rhs.empty ()) {
			return 1;
		}
	}
	Assert (not empty () and not rhs.empty ());
	if (GetTimezone () == rhs.GetTimezone ()) {
		int	cmp	=	GetDate ().Compare (rhs.GetDate ());
		if (cmp == 0) {
			cmp	=	GetTimeOfDay ().Compare (rhs.GetTimeOfDay ());
		}
		return cmp;
	}
	else {
		return AsGMT ().Compare (rhs.AsGMT ());
	}
}

bool Time::operator< (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.Compare (rhs) < 0;
}

bool Time::operator<= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.Compare (rhs) <= 0;
}

bool Time::operator> (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.Compare (rhs) > 0;
}

bool Time::operator>= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.Compare (rhs) >= 0;
}

bool Time::operator== (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.Compare (rhs) == 0;
}

bool Time::operator!= (const DateTime& lhs, const DateTime& rhs)
{
	return lhs.Compare (rhs) != 0;
}


