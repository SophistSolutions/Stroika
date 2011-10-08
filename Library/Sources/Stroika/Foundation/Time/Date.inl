/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_Date_inl_
#define	_Stroika_Foundation_Time_Date_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Time {



			//	class Date
				inline	Date::JulianRepType	Date::GetJulianRep () const
					{
						return (fJulianDateRep_ == kEmptyJulianRep? kMinJulianRep: fJulianDateRep_);
					}
				inline	bool	Date::empty () const
					{
						return fJulianDateRep_ == kEmptyJulianRep;
					}
				inline	int	Date::Compare (const Date& rhs) const
					{
						if (empty ()) {
							return rhs.empty ()? 0: -1;
						}
						else {
							return rhs.empty ()? 1 : (GetJulianRep () - rhs.GetJulianRep ());
						}
					}

				inline	int	DayDifference (const Date& lhs, const Date& rhs)
					{
						Require (not lhs.empty ());
						Require (not rhs.empty ());		// since unclear what diff would mean
						return lhs.GetJulianRep () - rhs.GetJulianRep ();
					}


				inline	bool operator<= (const Date& lhs, const Date& rhs)
					{
						return lhs.Compare (rhs) <= 0;
					}
				inline	bool operator< (const Date& lhs, const Date& rhs)
					{
						return lhs.Compare (rhs) < 0;
					}
				inline	bool operator> (const Date& lhs, const Date& rhs)
					{
						return lhs.Compare (rhs) > 0;
					}
				inline	bool operator== (const Date& lhs, const Date& rhs)
					{
						return lhs.Compare (rhs) == 0;
					}
				inline	bool operator!= (const Date& lhs, const Date& rhs)
					{
						return lhs.Compare (rhs) != 0;
					}

		}


		namespace	Execution {
			template	<>
				inline	void	_NoReturn_	DoThrow (const Time::Date::FormatException& e2Throw)
					{
						DbgTrace (L"Throwing Date::FormatException");
						throw e2Throw;
					}
		}

	}
}
#endif	/*_Stroika_Foundation_Time_Date_inl_*/
