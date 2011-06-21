/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
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
					explicit Date (const wstring& rep, LCID lcid = LOCALE_USER_DEFAULT);
					explicit Date (Year year, MonthOfYear month, DayOfMonth day);
					Date (const SYSTEMTIME& sysTime);
					Date (const FILETIME& fileTime);

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

				public:
					operator SYSTEMTIME () const;

				public:
					nonvirtual	wstring	Format (LCID lcid = LOCALE_USER_DEFAULT) const;
					nonvirtual	wstring	Format (const TString& format, LCID lcid = LOCALE_USER_DEFAULT) const;				// See GetDateFormat () format args
					nonvirtual	wstring	Format4XML () const;
					nonvirtual	wstring	Format4JScript () const;
					nonvirtual	wstring	LongFormat (LCID lcid = LOCALE_USER_DEFAULT) const;

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
					explicit TimeOfDay (const wstring& rep, LCID lcid = LOCALE_USER_DEFAULT);

					/*
					 * If a full date, just grab the time part, and ignore the rest.
					 */
					enum XML { eXML };
					explicit TimeOfDay (const wstring& rep, XML);
					explicit TimeOfDay (const SYSTEMTIME& sysTime);
					explicit TimeOfDay (const FILETIME& fileTime);

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
					nonvirtual	wstring	Format (LCID lcid = LOCALE_USER_DEFAULT) const;
					nonvirtual	wstring	Format4XML () const;

				public:
					nonvirtual	operator SYSTEMTIME () const;

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
					explicit DateTime (const wstring& rep, LCID lcid = LOCALE_USER_DEFAULT);
					enum XML { eXML };
					explicit DateTime (const wstring& rep, XML);
					DateTime (const SYSTEMTIME& sysTime);
					DateTime (const FILETIME& fileTime);
					explicit DateTime (time_t unixTime);

				public:
					nonvirtual	bool	empty () const;

				public:
					static	DateTime	Now ();

				public:
					nonvirtual	wstring	Format (LCID lcid = LOCALE_USER_DEFAULT) const;
					nonvirtual	wstring	Format4XML () const;

				public:
					// returns number of days since this point - relative to NOW. Never less than zero
					nonvirtual	Date::JulianRepType	DaysSince () const;

				public:
					nonvirtual	operator SYSTEMTIME () const;

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
