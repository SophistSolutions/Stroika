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
#include	"../Execution/StringException.h"



/*
 * TODO:
 *
 *		o	Do better job rounding. Right now we round (?)properly for seconds, but nothing else.
 *
 *		o	Add support for long double (and perhaps others?). And consider using long double for InternalNumericFormatType_;
 *
 *		o	Consider adding 'precision' property to PrettyPrintInfo. Think about precision support/design of
 *			boost (maybe use  bignum or rational?). Probably no - but document clearly why or why not.
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
					// Throws (FormatException) if bad format
					Duration ();
					explicit Duration (const wstring& durationStr);
					explicit Duration (int32_t duration);
					explicit Duration (int64_t duration);
					explicit Duration (double duration);

				public:
					nonvirtual	void	clear ();
					nonvirtual	bool	empty () const;

				public:
					/*
					 * Defined for 
					 *		time_t
					 *		wstring
					 *		double
					 *
					 *	Note - if 'emtpy' - As<> for numeric types returns 0.
					 */
					template	<typename T>
						nonvirtual	T	As () const;

				public:
					struct	PrettyPrintInfo {
						struct	Labels {
							wstring	fYear;
							wstring	fYears;
							wstring	fMonth;
							wstring	fMonths;
							wstring	fDay;
							wstring	fDays;
							wstring	fHour;
							wstring	fHours;
							wstring	fMinute;
							wstring	fMinutes;
							wstring	fSecond;
							wstring	fSeconds;
							wstring	fMilliSecond;
							wstring	fMilliSeconds;
							wstring	fMicroSecond;
							wstring	fMicroSeconds;
							wstring	fNanoSecond;
							wstring	fNanoSeconds;
						}	fLabels;
					};
					static	const	PrettyPrintInfo	kDefaultPrettyPrintInfo;
					nonvirtual	wstring PrettyPrint (const PrettyPrintInfo& prettyPrintInfo = kDefaultPrettyPrintInfo) const;


				public:
					// Unary negation
					nonvirtual	Duration	operator- () const;

				public:
					class	FormatException;

				public:
					// Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs. Note - for the purpose of
					// this comparison function - see the notes about 'empty' in the class description.
					nonvirtual	int	Compare (const Duration& rhs) const;

				private:
					typedef	double	InternalNumericFormatType_;
					static	InternalNumericFormatType_	ParseTime_ (const string& s);
					static	string						UnParseTime_ (InternalNumericFormatType_ t);

				private:
					string	fDurationRep;
			};
			template	<>
				time_t	Duration::As () const;
			template	<>
				wstring	Duration::As () const;
			template	<>
				double	Duration::As () const;

			bool operator< (const Duration& lhs, const Duration& rhs);
			bool operator<= (const Duration& lhs, const Duration& rhs);
			bool operator> (const Duration& lhs, const Duration& rhs);
			bool operator== (const Duration& lhs, const Duration& rhs);
			bool operator!= (const Duration& lhs, const Duration& rhs);


			class	Duration::FormatException : public Execution::StringException {
				public:
					FormatException ();
			};

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
