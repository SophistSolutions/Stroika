/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_Date_h_
#define	_Stroika_Foundation_Time_Date_h_	1

#include	"../StroikaPreComp.h"

#include	<climits>
#include	<string>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Characters/TString.h"
#include	"../Configuration/Common.h"



/*
 * TODO:
 *
 
		o	Do Same PrintFormat () coding I did in TimeOfDay

		o	Get rid of various CONSTURCTOR and conversiton ooeprators (as I did with TimeOfDay - replacing with Parse and Format osing PrintFormat enum.

		o	get rid of CONVERT OPERTOR SYSTEMTIME (support only for DateTime code).
 
 
		o	Get rid of conversion operators. Replace them with the As<> template pattern, and then lose the conversion
			operator
		o	Add POSIX type support (constructor and As<>) - we have time_t but add struct tm (others?)
			(PROBABLY NOT time_t - since that doesn't make a ton of snese - but DO include struct tm support)!!!
		o	add if POSIX to list of ifdefs we support and where easy support them and where not - oh well
		o	Replace (research first) use of name XML here with iso8601.
			+ maybe not quite. See http://www.w3.org/TR/xmlschema-2/#isoformats
				XML really is its own - nearly identical to iso8601, but see deviations...
				Maybe have iso8601, and XML support (and for our primitive purposes maybe the com eto the same thing?)
		o	Add regression test suite (TEST 7)
		o	Consider using strptime/strftime - and possibly use that to replace windows formatting APIs?
 *		
 *		o	Should we PIN or throw OVERFLOW exception on values/requests which are out of range?
 *
 */



namespace	Stroika {
	namespace	Foundation {
		namespace	Time {

			using	Characters::TString;


			/*
			 * Description:
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
			 *			NB: Date implies NO NOTION of timezone.
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
					explicit Date (Year year, MonthOfYear month, DayOfMonth day);

				public:
					enum	PrintFormat {
						eCurrentLocale_PF,
						eXML_PF,
						eJavascript_PF,
					};
					static	Date	Parse (const wstring& rep, PrintFormat pf);
				#if		qPlatform_Windows
					static	Date	Parse (const wstring& rep, LCID lcid);
				#endif

				public:
					/*
					 * Date::kMin is the first date this Date class supports representing.
					 */
					static	const	Date	kMin;
					/*
					 * Date::kMax is the last date this Date class supports representing.
					 */
					static	const	Date	kMax;

				public:
					nonvirtual	bool	empty () const;

				public:
					nonvirtual	Year			GetYear () const;
					nonvirtual	MonthOfYear		GetMonth () const;
					nonvirtual	DayOfMonth		GetDayOfMonth () const;



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
					static 		JulianRepType	jday_ (MonthOfYear month, DayOfMonth day, Year year);			// from NIHCL
					static 		JulianRepType	Safe_jday_ (MonthOfYear month, DayOfMonth day, Year year);
				public:
					nonvirtual	void			mdy (MonthOfYear* month, DayOfMonth* day, Year* year) const;	// from NIHCL

				private:
					JulianRepType	fJulianDateRep_;
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

		}
	}
}
#endif	/*_Stroika_Foundation_Time_Date_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Date.inl"
