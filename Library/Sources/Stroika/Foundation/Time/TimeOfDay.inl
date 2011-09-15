/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_TimeOfDay_inl_
#define	_Stroika_Foundation_Time_TimeOfDay_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Time {


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

		}


		namespace	Execution {
			template	<>
				inline	void	_NoReturn_	DoThrow (const Time::TimeOfDay::FormatException& e2Throw)
					{
						DbgTrace (L"Throwing TimeOfDay::FormatException");
						throw e2Throw;
					}
		}

	}
}
#endif	/*_Stroika_Foundation_Time_TimeOfDay_inl_*/
