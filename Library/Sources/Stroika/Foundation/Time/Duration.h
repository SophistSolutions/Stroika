/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_Duration_h_
#define	_Stroika_Foundation_Time_Duration_h_	1

#include	"../StroikaPreComp.h"

#include	<climits>
#include	<string>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Configuration/Common.h"

#include	"Date.h"


/*
 * SHORT TERM - TODAY - TODO:
 *
 *		o	needs ability to return like time_t – but as DOUBLE – because can be fractional.
 *			Needs to be able to represent times in milliseconds.
 *			Should also interoperate with Time::DiurationInSeconds
 *
 *
 *
 */


namespace	Stroika {
	namespace	Foundation {
		namespace	Time {


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
//nonvirtual	operator time_t () const;
//nonvirtual	operator wstring () const;

				public:
					/*
					 * Defined for 
					 *		time_t
					 *		wstring
					 *		double
					 */
					template	<typename T>
						T	As () const;

				public:
					nonvirtual	wstring PrettyPrint () const;

				private:
					static	double	ParseTime_ (const string& s);
					static	string	UnParseTime_ (time_t t);

				private:
					string	fDurationRep;
			};
			template	<>
				time_t	Duration::As () const;
			template	<>
				wstring	Duration::As () const;
			template	<>
				double	Duration::As () const;



		}
	}
}
#endif	/*_Stroika_Foundation_Time_Duration_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Duration.inl"
