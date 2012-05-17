/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_inl_
#define _Stroika_Foundation_Time_Date_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {



            //  class Date
            inline  Date::JulianRepType Date::GetJulianRep () const {
                return (fJulianDateRep_ == kEmptyJulianRep ? kMinJulianRep : fJulianDateRep_);
            }
            inline  bool    Date::empty () const {
                return fJulianDateRep_ == kEmptyJulianRep;
            }
            inline  int Date::Compare (const Date& rhs) const {
                if (empty ()) {
                    return rhs.empty () ? 0 : -1;
                }
                else {
                    if (rhs.empty ()) {
                        return 1;
                    }
                    // careful of signed/unsigned converstions - esp because of kMax which is very large
                    JulianRepType   l   =   GetJulianRep ();
                    JulianRepType   r   =   rhs.GetJulianRep ();
                    if (l == r) {
                        return 0;
                    }
                    return l < r ? -1 : 1;
                }
            }


            inline  bool operator<= (const Date& lhs, const Date& rhs) {
                return lhs.Compare (rhs) <= 0;
            }
            inline  bool operator< (const Date& lhs, const Date& rhs) {
                return lhs.Compare (rhs) < 0;
            }
            inline  bool operator> (const Date& lhs, const Date& rhs) {
                return lhs.Compare (rhs) > 0;
            }
            inline  bool operator== (const Date& lhs, const Date& rhs) {
                return lhs.Compare (rhs) == 0;
            }
            inline  bool operator!= (const Date& lhs, const Date& rhs) {
                return lhs.Compare (rhs) != 0;
            }

        }


        namespace   Execution {
            template    <>
            inline  void    _NoReturn_  DoThrow (const Time::Date::FormatException& e2Throw) {
                DbgTrace (L"Throwing Date::FormatException");
                throw e2Throw;
            }
        }

    }
}
#endif  /*_Stroika_Foundation_Time_Date_inl_*/
