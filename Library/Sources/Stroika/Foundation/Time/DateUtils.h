/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_DateUtils_h_
#define	_Stroika_Foundation_Time_DateUtils_h_	1

#include	"../StroikaPreComp.h"

#include	<climits>
#include	<string>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Characters/TString.h"
#include	"../Configuration/Common.h"


/*
 * SHORT TERM - TODAY - TODO:

	+	Break into smaller file units (like old stroika - by calss
		+	Maintain (briefly) the DateUtils.h file only so HF will compile - and there just #include each sub-include
			currently pulled in here.


	+	add if POSIX to list of ifdefs we support and where easy support them and where not - oh well

	+	Replace (reserach firrst) use of name XML here with iso8601.
		+ maybe not quite. See http://www.w3.org/TR/xmlschema-2/#isoformats
			XML really is its own - nearly identical to iso8601, but see deviations...
			Maybe have iso8601, and XML support (and for our primitive purposes maybe the com eto the same thing?)

	+	Add regression test suite (TEST 7)

	+	Probably use times() for tickcount:
			#include <sys/times.h>
			clock_t times(struct tms *buf);

	+	Only test suite code to lift from old stroika is:



 */
#if 0


static	void	SimpleTimeTests ();
static	void	TestTime ();

int	main (int /*argc*/, char* /*argv*/[])
{
	cout << "Testing Time..." << endl;

	TestTime ();
	
	cout << "Done Testing Time" << endl;
	return (0);
}

void	SimpleTimeTests ()
{
#if 0
Real	x = 0.001;
x *= 1000000.0;
TestCondition (x == 1000.0);
TestCondition ((0.001 * 1000000.0) == 1000.0);
TestCondition ((0.01 * 1000000.0) == 10000.0);
TestCondition ((0.1 * 1000000.0) == 100000.0);
TestCondition ((0.0001 * 1000000.0) == 100.0);
TestCondition ((0.00001 * 1000000.0) == 10.0);
TestCondition ((0.000001 * 1000000.0) == 1.0);

TestCondition ((0.002 * 1000000.0) == 2000.0);
TestCondition ((0.003 * 1000000.0) == 3000.0);
TestCondition ((0.004 * 1000000.0) == 4000.0);
TestCondition ((0.005 * 1000000.0) == 5000.0);
TestCondition ((0.006 * 1000000.0) == 6000.0);
TestCondition ((0.007 * 1000000.0) == 7000.0);
TestCondition ((0.008 * 1000000.0) == 8000.0);
TestCondition ((0.009 * 1000000.0) == 9000.0);
TestCondition ((0.0010 * 1000000.0) == 1000.0);
TestCondition ((0.0011 * 1000000.0) == 1100.0);

TestCondition ((0.000001 * 100000.0) == 	.1);
TestCondition ((0.000001 * 10000.0) ==  	.01);
TestCondition ((0.000001 * 1000.0) == 		.001);
TestCondition ((0.000001 * 100.0) == 		.0001);
TestCondition ((0.000001 * 10.0) == 		.00001);
TestCondition ((0.000001 * 1.0) == 			.000001); 
TestCondition ((0.000001 * .1) == 			.0000001);

TestCondition ((0.0015 * 1000000.0) == 1500.0);
TestCondition ((0.000999 * 1000000.0) == 999.0);
#endif

	TestCondition (Time () == 0);
#if 0
//	TestCondition ((kOneMicroSecond * 1000) == kOneMilliSecond);
Time f = Time::kOneMicroSecond * 1000;
//TestCondition (f == kOneMilliSecond);
//cerr << "f = " << f.fSeconds << endl;
//cerr << "kOneMilliSecond = " << kOneMilliSecond.fSeconds << endl;
//	TestCondition ((kOneMilliSecond * 1000) == kOneSecond);
f = Time::kOneMilliSecond * 1000;
//TestCondition (f == kOneSecond);
	TestCondition ((Time::kOneMicroSecond * 1000000) == Time::kOneSecond);
f = Time::kOneMicroSecond * 1000000;
TestCondition (f == Time::kOneSecond);
	
//	cerr << "kOneMicroSecond = " << kOneMicroSecond.fSeconds << endl;
//	cerr << "kOneMicroSecond * 1000 = " << (kOneMicroSecond * 1000).fSeconds << endl;
//	cerr << "kOneMilliSecond = " << kOneMilliSecond.fSeconds << endl;
//	cerr << "kOneMilliSecond * 1000 = " << (kOneMilliSecond * 1000).fSeconds << endl;

	Time t = Time::kOneSecond - Time::kOneMicroSecond;
	TestCondition (t < Time::kOneSecond);
	TestCondition (t <= Time::kOneSecond);
//	t += kOneMicroSecond;
t = t + Time::kOneMicroSecond;
	TestCondition (t <= Time::kOneSecond);
	TestCondition (t == Time::kOneSecond);
	TestCondition (t >= Time::kOneSecond);
//	t += kOneMicroSecond;
t = t + Time::kOneMicroSecond;
	TestCondition (t > Time::kOneSecond);
	TestCondition (t >= Time::kOneSecond);
t = t - Time::kOneMicroSecond;
	TestCondition (t == Time::kOneSecond);
	t = t / 5;
	TestCondition (floor (t.GetSeconds ()) == 0);
//	TestCondition (t.fSeconds == 200000);
#endif
}

void	TestTime ()
{
	SimpleTimeTests ();
	
	Time t = GetCurrentTime ();
//	cerr << "current time = " << t.GetSeconds () << ", " << t.fSeconds << endl;
	Time t1 = GetCurrentTime ();
	Assert (t <= t1);
}
#endif


namespace	Stroika {
	namespace	Foundation {
		namespace	Time {

			using	Characters::TString;


			/*
			 * Based on Stroika code from 1992 (Date.hh/Date.cpp). From that - we have the comment:
			 *
			 * The Date class is based on SmallTalk-80, The Language & Its Implementation,
			 * page 108 (apx).
			 *
			 *		"...Date represents a specific data since the start of the Julian
			 *	calendar. Class Date knows about some obvious information:
			 *		->	there are seven days in a week, each day having a symbolic name and
			 * 			an index 1..7
			 *		->	there are twelve months in a year, each having a symbolic name and
			 *			an index 1..12.
			 *		->	months have 28..31 days and
			 *		->	a particular year might be a leap year."
			 *
			 *
			 */
			class	Date {
				public:
					enum	DayOfWeek {
						eMonday = 1,
						eTuesday = 2,
						eWednesday = 3,
						eThursday = 4,
						eFriday = 5,
						eSaturday = 6,
						eSunday = 7,
						eFirstDayOfWeek = eMonday,
						eLastDayOfWeek = eSunday,
					};
					enum	MonthOfYear {
						eEmptyMonthOfYear	= 0,		// only zero if date empty
						eJanuary = 1,
						eFebruary = 2,
						eMarch = 3,
						eApril = 4,
						eMay = 5,
						eJune = 6,
						eJuly = 7,
						eAugust = 8,
						eSeptember = 9,
						eOctober = 10,
						eNovember = 11,
						eDecember = 12,
						eFirstMonthOfYear = eJanuary,
						eLastMonthOfYear = eDecember,
					};
					enum	DayOfMonth {
						eEmptyDayOfMonth = 0,		// only zero if date empty
						eFirstDayOfMonth = 1,
						eLastDayOfMonth = 31,
					};
					enum	DayOfYear {
						eFirstDayOfYear = 1,
						eLastDayOfYear = 366,
					};
					enum	Year {
						eEmptyYear = SHRT_MIN,
						eFirstYear = 1752,		// Gregorian calendar started on Sep. 14, 1752.
						eLastfYear = SHRT_MAX,
					};
					typedef	unsigned int	JulianRepType;
					enum { kMinJulianRep = 0 };
					enum { kEmptyJulianRep = UINT_MAX };

				public:
					class	FormatException;

				public:
					Date ();
					explicit Date (JulianRepType julianRep);
					enum XML { eXML };
					enum Javascript { eJavascript };
					explicit Date (const wstring& rep, XML);
					explicit Date (const wstring& rep, Javascript);
					explicit Date (const wstring& rep);
				#if		qPlatform_Windows
					explicit Date (const wstring& rep, LCID lcid);
				#endif
					explicit Date (Year year, MonthOfYear month, DayOfMonth day);
				#if		qPlatform_Windows
					Date (const SYSTEMTIME& sysTime);
					Date (const FILETIME& fileTime);
				#endif

				public:
					static	Date	GetToday ();
					static	const	Date	kMin;
					static	const	Date	kMax;

				public:
					nonvirtual	bool	empty () const;

				public:
					nonvirtual	Year			GetYear () const;
					nonvirtual	MonthOfYear		GetMonth () const;
					nonvirtual	DayOfMonth		GetDayOfMonth () const;

				#if		qPlatform_Windows
				public:
					operator SYSTEMTIME () const;
				#endif

				public:
					nonvirtual	wstring	Format () const;
					#if		qPlatform_Windows
					nonvirtual	wstring	Format (LCID lcid) const;
					nonvirtual	wstring	Format (const TString& format, LCID lcid = LOCALE_USER_DEFAULT) const;				// See GetDateFormat () format args
					#endif
					nonvirtual	wstring	Format4XML () const;
					nonvirtual	wstring	Format4JScript () const;
					#if		qPlatform_Windows
					nonvirtual	wstring	LongFormat (LCID lcid = LOCALE_USER_DEFAULT) const;
					#endif

				public:
					nonvirtual	Date	AddDays (int dayCount);

				public:
					nonvirtual	JulianRepType	GetJulianRep () const;

				public:
					// returns number of days since this point - relative to NOW. Never less than zero
					nonvirtual	JulianRepType	DaysSince () const;

				private:
					static 		JulianRepType	jday (MonthOfYear month, DayOfMonth day, Year year);			// from NIHCL
					static 		JulianRepType	Safe_jday (MonthOfYear month, DayOfMonth day, Year year);
				public:
					nonvirtual	void			mdy (MonthOfYear* month, DayOfMonth* day, Year* year) const;	// from NIHCL

				private:
					JulianRepType	fJulianDateRep;
			};


			class	Date::FormatException {
			};



			bool operator< (const Date& lhs, const Date& rhs);
			bool operator<= (const Date& lhs, const Date& rhs);
			bool operator> (const Date& lhs, const Date& rhs);
			bool operator== (const Date& lhs, const Date& rhs);
			bool operator!= (const Date& lhs, const Date& rhs);
			int	DayDifference (const Date& lhs, const Date& rhs);
			int	YearDifference (const Date& lhs, const Date& rhs);
			float	YearDifferenceF (const Date& lhs, const Date& rhs);


			wstring	GetFormattedAge (const Date& birthDate, const Date& deathDate = Date ());	// returns ? if not a good src date
			wstring	GetFormattedAgeWithUnit (const Date& birthDate, const Date& deathDate = Date (), bool allowMonths = true, bool abbrevUnit = true);	// returns ? if not a good src date



			// a time value - which is assumed to be within a given day - e.g 2:30 pm
			class	TimeOfDay {
				public:
					TimeOfDay ();

					explicit TimeOfDay (unsigned int t);		// we normalize to be within a given day (seconds since midnight)
					explicit TimeOfDay (const wstring& rep);
				#if		qPlatform_Windows
					explicit TimeOfDay (const wstring& rep, LCID lcid);
				#endif

					/*
					 * If a full date, just grab the time part, and ignore the rest.
					 */
					enum XML { eXML };
					explicit TimeOfDay (const wstring& rep, XML);
					#if		qPlatform_Windows
					explicit TimeOfDay (const SYSTEMTIME& sysTime);
					explicit TimeOfDay (const FILETIME& fileTime);
					#endif

				public:
					static	TimeOfDay	Now ();

				public:
					class	FormatException;

				public:
					nonvirtual	unsigned int	GetAsSecondsCount () const;		// seconds since StartOfDay (midnight)
					nonvirtual	bool			empty () const;

				public:
					nonvirtual	void	ClearSecondsField ();

				public:
					#if		qPlatform_Windows
					nonvirtual	wstring	Format (LCID lcid = LOCALE_USER_DEFAULT) const;
					#endif
					nonvirtual	wstring	Format4XML () const;

				#if		qPlatform_Windows
				public:
					nonvirtual	operator SYSTEMTIME () const;
				#endif

				private:
					unsigned int	fTime;
			};

			bool operator< (const TimeOfDay& lhs, const TimeOfDay& rhs);
			bool operator> (const TimeOfDay& lhs, const TimeOfDay& rhs);
			bool operator== (const TimeOfDay& lhs, const TimeOfDay& rhs);
			bool operator!= (const TimeOfDay& lhs, const TimeOfDay& rhs);


			class	TimeOfDay::FormatException {
			};





			class	DateTime {
				private:
					Date		fDate;
					TimeOfDay	fTimeOfDay;

				public:
					DateTime (const Date& date = Date (), const TimeOfDay& timeOfDay = TimeOfDay ());
					explicit DateTime (const wstring& rep);
					#if		qPlatform_Windows
					explicit DateTime (const wstring& rep, LCID lcid);
					#endif
					enum XML { eXML };
					explicit DateTime (const wstring& rep, XML);
					#if		qPlatform_Windows
					DateTime (const SYSTEMTIME& sysTime);
					DateTime (const FILETIME& fileTime);
					#endif
					explicit DateTime (time_t unixTime);

				public:
					nonvirtual	bool	empty () const;

				public:
					static	DateTime	Now ();

				public:
					nonvirtual	wstring	Format () const;
					#if		qPlatform_Windows
					nonvirtual	wstring	Format (LCID lcid) const;
					#endif
					nonvirtual	wstring	Format4XML () const;

				public:
					// returns number of days since this point - relative to NOW. Never less than zero
					nonvirtual	Date::JulianRepType	DaysSince () const;

				#if		qPlatform_Windows
				public:
					nonvirtual	operator SYSTEMTIME () const;
				#endif

				public:
					nonvirtual	time_t	GetUNIXEpochTime () const;	// seconds since midnight 1970 (its independent of timezone)

				public:
					nonvirtual	Date		GetDate () const;		// careful of timezone issues? (always in current timezone - I guess)
					nonvirtual	TimeOfDay	GetTimeOfDay () const;	// ditto
					nonvirtual	void		SetDate (const Date& d);
					nonvirtual	void		SetTimeOfDay (const TimeOfDay& tod);
				public:
					nonvirtual	operator Date () const;
			};

			bool operator< (const DateTime& lhs, const DateTime& rhs);
			bool operator<= (const DateTime& lhs, const DateTime& rhs);
			bool operator> (const DateTime& lhs, const DateTime& rhs);
			bool operator>= (const DateTime& lhs, const DateTime& rhs);
			bool operator== (const DateTime& lhs, const DateTime& rhs);
			bool operator!= (const DateTime& lhs, const DateTime& rhs);




			/*
			 * (basic) support for ISO 8601 Durations
			 *		http://en.wikipedia.org/wiki/ISO_8601#Durations
			 *
			 *	Note: according to glibc docs - year is always 365 days, month always 30 days, etc, as far as these duration objects
			 *	go - at least for conversion to/from time_t. Seek a better reference for this claim!
			 *
			 * Note also - there are two iso 8601 duration formats - one date like (YYYY-MM....) and this one P...T...3S).
			 * As far as I know - XML always uses the later. For now - this implementation only supports the later.
			 *
			 *		See also: http://bugzilla/show_bug.cgi?id=468
			 */
			class	Duration {
				public:
					typedef	Date::FormatException	FormatException;
				public:
					// empty string () results in '0' time_t.
					// Throws (Date::FormatException) if bad format
					Duration ();
					explicit Duration (const wstring& durationStr);
					explicit Duration (time_t duration);

				public:
					nonvirtual	void	clear ();
					nonvirtual	bool	empty () const;
					nonvirtual	operator time_t () const;
					nonvirtual	operator wstring () const;
					nonvirtual	wstring PrettyPrint () const;

				private:
					static	time_t	ParseTime_ (const string& s);
					static	string	UnParseTime_ (time_t t);

				private:
					string	fDurationRep;
			};



		}
	}
}
#endif	/*_Stroika_Foundation_Time_DateUtils_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"DateUtils.inl"
