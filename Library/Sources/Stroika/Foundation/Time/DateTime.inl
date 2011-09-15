/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Time_DateTime_inl_
#define	_Stroika_Foundation_Time_DateTime_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Time {

		//	class DateTime
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
	}
}
#endif	/*_Stroika_Foundation_Time_DateTime_inl_*/
