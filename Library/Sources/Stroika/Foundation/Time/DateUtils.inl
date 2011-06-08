/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_DateUtils_inl_
#define	_Stroika_Foundation_Time_DateUtils_inl_	1


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
						return (fJulianDateRep);
					}
				inline	bool	Date::empty () const
					{
						return fJulianDateRep == kEmptyJulianRep;
					}

				inline	int	DayDifference (const Date& lhs, const Date& rhs)
					{
						Require (not lhs.empty ());
						Require (not rhs.empty ());		// since unclear what diff would mean
						return lhs.GetJulianRep () - rhs.GetJulianRep ();
					}

				inline	bool operator<= (const Date& lhs, const Date& rhs)
					{
						return lhs.GetJulianRep () <= rhs.GetJulianRep ();
					}
				inline	bool operator< (const Date& lhs, const Date& rhs)
					{
						return lhs.GetJulianRep () < rhs.GetJulianRep ();
					}
				inline	bool operator> (const Date& lhs, const Date& rhs)
					{
						return lhs.GetJulianRep () >rhs.GetJulianRep ();
					}
				inline	bool operator== (const Date& lhs, const Date& rhs)
					{
						return lhs.GetJulianRep () == rhs.GetJulianRep ();
					}
				inline	bool operator!= (const Date& lhs, const Date& rhs)
					{
						return lhs.GetJulianRep () != rhs.GetJulianRep ();
					}




			//	class TimeOfDay
				inline	bool	TimeOfDay::empty () const
					{
						return fTime == static_cast<unsigned int> (-1);
					}
				inline	unsigned int	TimeOfDay::GetAsSecondsCount () const
					{
						if (empty ()) {
							return 0;
						}
						return fTime;
					}
				inline	bool operator< (const TimeOfDay& lhs, const TimeOfDay& rhs)
					{
						return lhs.GetAsSecondsCount () < rhs.GetAsSecondsCount ();
					}
				inline	bool operator> (const TimeOfDay& lhs, const TimeOfDay& rhs)
					{
						return lhs.GetAsSecondsCount () > rhs.GetAsSecondsCount ();
					}
				inline	bool operator== (const TimeOfDay& lhs, const TimeOfDay& rhs)
					{
						return lhs.GetAsSecondsCount () == rhs.GetAsSecondsCount ();
					}
				inline	bool operator!= (const TimeOfDay& lhs, const TimeOfDay& rhs)
					{
						return lhs.GetAsSecondsCount () != rhs.GetAsSecondsCount ();
					}




		//	class TimeOfDay
			inline	DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay):
				fDate (date),
				fTimeOfDay (timeOfDay)
				{
				}
			inline	Date	DateTime::GetDate () const
				{
					return fDate;
				}
			inline	TimeOfDay	DateTime::GetTimeOfDay () const
				{
					return fTimeOfDay;
				}
			inline	DateTime::operator Date () const
				{
					return fDate;
				}

		}



		template	<>
			inline	__declspec(noreturn)	void	Execution::DoThrow (const Time::Date::FormatException& e2Throw)
				{
					DbgTrace (L"Throwing Date::FormatException");
					throw e2Throw;
				}
		template	<>
			inline	__declspec(noreturn)	void	Execution::DoThrow (const Time::TimeOfDay::FormatException& e2Throw)
				{
					DbgTrace (L"Throwing TimeOfDay::FormatException");
					throw e2Throw;
				}

	}
}
#endif	/*_Stroika_Foundation_Time_DateUtils_inl_*/
