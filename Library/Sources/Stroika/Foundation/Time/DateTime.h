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


/*
 * TODO:
 *		o	Get rid of conversion operators. Replace them with the As<> template pattern, and then lose the conversion
			operator
		o	Add POSIX type support (constructor and As<>) - we have time_t but add struct tm (others?)
		o	Maybe use 		wcsftime (buf, NEltsOf (buf), L"%I:%M %p", &temp);	 or related for formatting dates/time?
 */



namespace	Stroika {
	namespace	Foundation {
		namespace	Time {

			using	Characters::TString;


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
