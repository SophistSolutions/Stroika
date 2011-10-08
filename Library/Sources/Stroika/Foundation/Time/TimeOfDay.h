/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_TimeOfDay_h_
#define	_Stroika_Foundation_Time_TimeOfDay_h_	1

#include	"../StroikaPreComp.h"

#include	<climits>
#include	<string>
#include	<locale>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Characters/TString.h"
#include	"../Configuration/Common.h"




/*
 * TODO:
 *		
 *		o	Consider getting rid of empty () method and empty state. Instead - in DateTime code - use Optional<>
 *		
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
			 * Description:
			 *		A time value - which is assumed to be within a given day - e.g 2:30 pm.
			 *	
			 *		NB: this implies NO NOTION of timezone. Its a time relative to midnight of a given day.
			 *
			 *		TimeOfDay can also be (it is by default) the specail sentinal value 'empty'. This is useful - with a DateTime object for example,
			 *	to represent the idea that a time has not been specified.
			 *
			 *		'empty' concept:
			 *			Treat it as DISTINCT from any other time. However, when converting it to a number of seconds,
			 *			treat empty as TimeOfDay::kMin. For format routine, return empty string. And for COMPARIONS (=,<,<=, etc) treat it as LESS THAN TimeOfDay::kMin.
			 *			This is a bit like the floating point concept of negative infinity.
			 */
			class	TimeOfDay {
				public:
					/*
					 * NB: The maximum value in a TimeOfDay struct is one less than kMaxSecondsPerDay
					 *
					 *		SB constexpr uint32_t	!!
					 */
					enum { kMaxSecondsPerDay = 60 * 60 * 24 };

				public:
					TimeOfDay ();

					// If value out of range - pinned to kMax
					// we normalize to be within a given day (seconds since midnight)
					explicit TimeOfDay (uint32_t t);

					enum	PrintFormat {
						eCurrentLocale_PF,
						eXML_PF,
					};
					static	TimeOfDay	Parse (const wstring& rep, PrintFormat pf);
					static	TimeOfDay	Parse (const wstring& rep, const locale& l);
				#if		qPlatform_Windows
					static	TimeOfDay	Parse (const wstring& rep, LCID lcid);
				#endif


				public:
					/*
					 * TimeOfDay::kMin is the first date this TimeOfDay class supports representing.
					 */
					static	const	TimeOfDay	kMin;
					/*
					 * TimeOfDay::kMin is the first date this TimeOfDay class supports representing.
					 */
					static	const	TimeOfDay	kMax;

				public:
					class	FormatException;

				public:
					// In specail case of empty - this also returns 0
					nonvirtual	uint32_t	GetAsSecondsCount () const;		// seconds since StartOfDay (midnight)
					nonvirtual	bool		empty () const;

				public:
					nonvirtual	void	ClearSecondsField ();

				public:
					// returns 0..23
					nonvirtual	uint8_t	GetHours () const;
					// returns 0..59
					nonvirtual	uint8_t	GetMinutes () const;
					// returns 0..59
					nonvirtual	uint8_t	GetSeconds () const;

				public:
					nonvirtual	wstring	Format (PrintFormat pf = eCurrentLocale_PF) const;
					nonvirtual	wstring	Format (const locale& l) const;

					#if		qPlatform_Windows
					nonvirtual	wstring	Format (LCID lcid) const;
					#endif

				public:
					// Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
					// this comparison function - see the notes about 'empty' in the class description.
					nonvirtual	int	Compare (const TimeOfDay& rhs) const;

				private:
					uint32_t	fTime_;
			};

			bool operator< (const TimeOfDay& lhs, const TimeOfDay& rhs);
			bool operator> (const TimeOfDay& lhs, const TimeOfDay& rhs);
			bool operator== (const TimeOfDay& lhs, const TimeOfDay& rhs);
			bool operator!= (const TimeOfDay& lhs, const TimeOfDay& rhs);


			class	TimeOfDay::FormatException {
			};


		}
	}
}
#endif	/*_Stroika_Foundation_Time_TimeOfDay_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"TimeOfDay.inl"
