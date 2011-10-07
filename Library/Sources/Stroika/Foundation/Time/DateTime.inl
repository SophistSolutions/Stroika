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
			inline	DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay)
				: fTimezone_ (eUnknown_TZ)
				, fDate_ (date)
				, fTimeOfDay_ (timeOfDay)
				{
				}
			inline	Date	DateTime::GetDate () const
				{
					return fDate_;
				}
			inline	TimeOfDay	DateTime::GetTimeOfDay () const
				{
					return fTimeOfDay_;
				}
			inline	DateTime::operator Date () const
				{
					return fDate_;
				}
			inline	Date	DateTime::GetToday ()
				{
					return Now ().GetDate ();
				}
			inline	DateTime::Timezone	DateTime::GetTimezone () const
				{
					return fTimezone_;
				}

		}
	}
}
#endif	/*_Stroika_Foundation_Time_DateTime_inl_*/
