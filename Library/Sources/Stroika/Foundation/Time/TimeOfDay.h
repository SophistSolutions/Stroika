/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_TimeOfDay_h_
#define	_Stroika_Foundation_Time_TimeOfDay_h_	1

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
 *		o	Consider getting rid of empty () method and empty state. Instead - in DateTime code - use Optional<>

 */



namespace	Stroika {
	namespace	Foundation {
		namespace	Time {

			using	Characters::TString;



			/*
			 * Description:
			 *			A time value - which is assumed to be within a given day - e.g 2:30 pm.
			 *	
			 *			NB: this implies NO NOTION of timezone. Its a time relative to midnight of a given day.
			 *
			 *	TimeOfDay can also be (it is by default) the specail sentinal value 'empty'. This is useful - with a DateTime object for example,
			 *	to represent the idea that a time has not been specified.
			 *
			 *	(NOTE - that ALSO coudl have been done with Optional - but we don't want to change too much at once. Future direction - we may
			 *		loise the 'empty' property).
			 */
			class	TimeOfDay {
				public:
					TimeOfDay ();

					// If value out of range - pinned to kMax
					// we normalize to be within a given day (seconds since midnight)
					explicit TimeOfDay (uint32_t t);


					enum	PrintFormat {
						eCurrentLocale_PF,
						eCurrentXML_PF,
					};
					static	TimeOfDay	Parse (const wstring& rep, PrintFormat pf);
				#if		qPlatform_Windows
					static	TimeOfDay	Parse (const wstring& rep, LCID lcid);
				#endif

#if 0
//RENAME AS PARSE STATIUC METHOD?
explicit TimeOfDay (const wstring& rep);
#if		qPlatform_Windows
explicit TimeOfDay (const wstring& rep, LCID lcid);
#endif

//LOSE THESE?
/*
* If a full date, just grab the time part, and ignore the rest.
*/
enum XML { eXML };
explicit TimeOfDay (const wstring& rep, XML);

#if 0
					#if		qPlatform_Windows
					explicit TimeOfDay (const SYSTEMTIME& sysTime);
					explicit TimeOfDay (const FILETIME& fileTime);
					#endif
#endif
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
					nonvirtual	wstring	Format (PrintFormat pf = eCurrentLocale_PF) const;

					#if		qPlatform_Windows
					nonvirtual	wstring	Format (LCID lcid) const;
					#endif
//nonvirtual	wstring	Format4XML () const { return Format (eCurrentXML_PF); }

#if 0
				#if		qPlatform_Windows
				public:
					nonvirtual	operator SYSTEMTIME () const;
				#endif
#endif

				private:
					uint32_t	fTime;
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
