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
			inline	DateTime::DateTime (const Date& date, const TimeOfDay& timeOfDay, Timezone tz)
				: fTimezone_ (tz)
				, fDate_ (date)
				, fTimeOfDay_ (timeOfDay)
				{
				}
			inline	bool	DateTime::empty () const
				{
					// Risky change so late in the game - but this logic seems wrong (and causes some trouble).
					// DateTime is NOT empty just because date part is empty. We CAN use a DateTime record to store JUST a time!
					//		-- LGP 2006-04-26
					//			return fDate_.empty () and fTimeOfDay_.empty ();
					//
					// Maybe also risky - but see my comments in the header for this funciton. I Think it DOES make sense to treat the DateTime as empty
					// if the Date part is empty...
					return fDate_.empty ();
				}
			inline	Date	DateTime::GetDate () const
				{
					return fDate_;
				}
			inline	TimeOfDay	DateTime::GetTimeOfDay () const
				{
					return fTimeOfDay_;
				}
			template	<>
				inline	Date	DateTime::As () const
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
