/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_DateTime_h_
#define	_Stroika_Foundation_Time_DateTime_h_	1

#include	"../StroikaPreComp.h"

#include	<climits>
#include	<string>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Characters/TString.h"
#include	"../Configuration/Common.h"

#include	"Date.h"
#include	"TimeOfDay.h"
#include	"Timezone.h"


/*
 * TODO:
 *		
		o	Private memobers need '_'.

		o	Implement new (described below) 'empty' semantics.

		o	Do support for enum Kind { eLocalTimezone, eGMT, eUnkownTimezone }
			and then do ToLocalTimezone () and DoGMTTimezone () which proiduce new DateTime objects.
			When comparing - if KIND != - convert BOTH to GMT and compare (interally).
			STARTED  AsLocalTime - ETC - but INCOMPLETE AND WRONG!!!

		o	Do SAME sort of PrintFormat code - I implemented for TimeOfDay ---

		o	Get rid of conversion operators. Replace them with the As<> template pattern, and then lose the conversion
			operator
		o	Add POSIX type support (constructor and As<>) - we have time_t but add struct tm (others?)
		o	Maybe use 		wcsftime (buf, NEltsOf (buf), L"%I:%M %p", &temp);	 or related for formatting dates/time?
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
			 *	Description:
			 *
			 *
			 *
			 *		DateTime is more than just a combination of Date, and Time. It also introduces the notion of TIMEZONE.
			 *
			 *		There are 3 possabilities for timezone - LOCALTIME, GMT, and UNKNOWN.
			 *
			 *		'empty' concept
			 *			Treat it internally as DISTINCT from any other time. However, when converting it to a number of seconds since midnight or whatever,
			 *			treat empty as DateTime::kMin. For format routine, return empty string. (this defintion must propagate to DATE and TIMEOFDAY - and must be documetned
			 *			there as well. AND IMPLEMENTED. Already encoeded in one regression tests (which we now fail).
			 */
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
					static	Date		GetToday ();

				public:
					/*
					 * DateTime::kMin is the first date this DateTime class supports representing.
					 */
					static	const	DateTime	kMin;
					/*
					 * DateTime::kMin is the first date this DateTime class supports representing.
					 */
					static	const	DateTime	kMax;


				public:
					enum Timezone {
						eLocalTime_TZ,
						eGMT_TZ,
						eUnknown_TZ,
					};
					nonvirtual	Timezone	GetTimezone () const;
				private:
					Timezone	fTimezone_;
				public:
					// Creates a new DateTime object known to be in localtime. If this DateTime is unknown, then the
					// conversion is also unknown (but either treat Kind as localtime or GMT)
					nonvirtual	DateTime	AsLocalTime () const;
					// Creates a new DateTime object known to be in GMT. If this DateTime is unknown, then the
					// conversion is also unknown (but either treat Kind as localtime or GMT)
					nonvirtual	DateTime	AsGMT () const;

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
					/*
					 * 	Returns seconds since midnight 1970 (its independent of timezone)
					 */
					nonvirtual	time_t	GetUNIXEpochTime () const;

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




		}
	}
}
#endif	/*_Stroika_Foundation_Time_DateTime_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"DateTime.inl"
