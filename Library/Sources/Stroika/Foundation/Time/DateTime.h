/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_DateTime_h_
#define	_Stroika_Foundation_Time_DateTime_h_	1

#include	"../StroikaPreComp.h"

#include	<climits>
#include	<ctime>
#include	<locale>
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
 *		
 *		o	Need PORTABLE/POSIX IMPLEMENTATION (locale/format/parse)
		o	Implement new (described below) 'empty' semantics.





		o	Do support for enum Timezone { eLocalTimezone, eGMT, eUnkownTimezone }
			and then do ToLocalTimezone () and DoGMTTimezone () which proiduce new DateTime objects.
			When comparing - if KIND != - convert BOTH to GMT and compare (interally).
			STARTED  AsLocalTime - ETC - but INCOMPLETE AND WRONG!!!

			>>> MAYBE TRICKY DOING TZ CONVERSIONS? I know how to find the CURRENT TZ OFFSET. But I THINK thats the WRONG OFFSET to use. I THINK you must use the
					OFFSET AS OF THE GIVEN DATE (which maybe ambiguous as it could change).
					o Say you are in DaylightSavingsTime (and are temporarly GMT-4) - and store a date in XML. But then later - after the timezone change - you re-read teh
					date. The TZ offset changed, and so the apparent localtime offset changed.
					o THe crux of the issue is that SOMETIMES when you store a date in XML - you don't want to say that its a LOCALETIME time (not be to adjusted for tz)?
					 Not sure this makes sense - must think through more carefully!


		o	Maybe use 		wcsftime (buf, NEltsOf (buf), L"%I:%M %p", &temp);	 or related for formatting dates/time?
		o	Consider using strptime/strftime - and possibly use that to replace windows formatting APIs?
<<<SEE COMMENTS IN DATE>>>



 *		o	Should we PIN or throw OVERFLOW exception on values/requests which are out of range?
 *
 *		o	Consider replacing eXML_PF with eISO8601_PF?  Not 100% sure they are the same. Maybe we should support BOTH here?
 *			Maybe where they differ doesn't matter for this class?
 *
 */



namespace	Stroika {
	namespace	Foundation {
		namespace	Time {

			using	Characters::TString;


			/*
			 *	Description:
			 *
			 *		DateTime is more than just a combination of Date, and Time. It also introduces the notion of TIMEZONE.
			 *
			 *		There are 3 possabilities for timezone - LOCALTIME, GMT, and UNKNOWN.
			 *
			 *		'empty' concept:
			 *			Treat it as DISTINCT from any other DateTime. However, when converting it to a number of seconds or days (JulienRep),
			 *			treat empty as DateTime::kMin. For format routine, return empty string. And for COMPARIONS (=,<,<=, etc) treat it as LESS THAN DateTime::kMin.
			 *			This is a bit like the floating point concept of negative infinity.
			 */
			class	DateTime {
				public:
					/*
					 * Most of the time applications will utilize localtime. But occasionally its useful to use differnt timezones, and our approach to this
					 * is to simply convert everything to GMT.
					 *
					 * eUnknown_TZ - for the most part - is treated as if it were localtime. However - the "Kind" function retunrs Unknown in case your application wants to
					 * treat it differently.
					 */
					enum Timezone {
						eLocalTime_TZ,
						eGMT_TZ,
						eUnknown_TZ,
					};

				public:
					DateTime (const Date& date = Date (), const TimeOfDay& timeOfDay = TimeOfDay (), Timezone tz = eUnknown_TZ);

				public:
					explicit DateTime (time_t unixTime);
					explicit DateTime (struct tm tmTime);

				#if		qPlatform_Windows
				public:
					explicit DateTime (const SYSTEMTIME& sysTime);
					explicit DateTime (const FILETIME& fileTime);
				#endif

				public:
					enum	PrintFormat {
						eCurrentLocale_PF,
						eXML_PF,
					};
					static	DateTime	Parse (const wstring& rep, PrintFormat pf);
					static	DateTime	Parse (const wstring& rep, const locale& l);
				#if		qPlatform_Windows
					static	DateTime	Parse (const wstring& rep, LCID lcid);
				#endif

				public:
					// If BOTH date and time are empty - this is empty. Timezone is ignored for the purpose of 'empty' check.
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
					nonvirtual	Timezone	GetTimezone () const;

				public:
					// Creates a new DateTime object known to be in localtime. If this DateTime is unknown, then the
					// conversion is also unknown (but either treat Kind as localtime or GMT)
					nonvirtual	DateTime	AsLocalTime () const;
					// Creates a new DateTime object known to be in GMT. If this DateTime is unknown, then the
					// conversion is also unknown (but either treat Kind as localtime or GMT)
					nonvirtual	DateTime	AsGMT () const;

				public:
					nonvirtual	wstring	Format (PrintFormat pf = eCurrentLocale_PF) const;
					nonvirtual	wstring	Format (const locale& l) const;

					#if		qPlatform_Windows
					nonvirtual	wstring	Format (LCID lcid) const;
					#endif

				public:
					// returns number of days since this point - relative to NOW. Never less than zero
					nonvirtual	Date::JulianRepType	DaysSince () const;

				public:
					/*
					 * Defined for 
					 *		time_t
					 *		struct tm
					 *		SYSTEMTIME			(WINDOWS ONLY)
					 *		Date
					 */
					template	<typename T>
						T	As () const;


				public:
					nonvirtual	Date		GetDate () const;		// careful of timezone issues? (always in current timezone - I guess)
					nonvirtual	TimeOfDay	GetTimeOfDay () const;	// ditto
					nonvirtual	void		SetDate (const Date& d);
					nonvirtual	void		SetTimeOfDay (const TimeOfDay& tod);
#if 0
			public:
					nonvirtual	operator Date () const;
#endif

				private:
					Date		fDate_;
					TimeOfDay	fTimeOfDay_;
					Timezone	fTimezone_;
			};
			/*
			 * 	Returns seconds since midnight 1970 (its independent of timezone). This is UNIX 'Epoch time'.
			 */
			template	<>
				time_t	DateTime::As () const;
			template	<>
				tm	DateTime::As () const;
			#if		qPlatform_Windows
			template	<>
				SYSTEMTIME	DateTime::As () const;
			#endif
			template	<>
				Date	DateTime::As () const;


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
