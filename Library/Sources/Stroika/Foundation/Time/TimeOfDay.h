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
 *		o	Get rid of conversion operators. Replace them with the As<> template pattern, and then lose the conversion
			operator
		o	Add POSIX type support (constructor and As<>) - we have time_t but add struct tm (others?)

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
			 */
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
