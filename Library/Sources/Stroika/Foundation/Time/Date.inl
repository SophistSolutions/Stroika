/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_inl_
#define _Stroika_Foundation_Time_Date_inl_  1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Execution/Exceptions.h"
#include    "../Execution/ModuleInit.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Time {


            /*
             ********************************************************************************
             *************************************** Date ***********************************
             ********************************************************************************
             */
            inline  constexpr Date::Date ()
                : fJulianDateRep_ (kEmptyJulianRep)
            {
            }
            inline  constexpr Date::Date (JulianRepType julianRep)
                : fJulianDateRep_ (julianRep)
            {
            }
            inline  constexpr   Date::JulianRepType Date::GetJulianRep () const
            {
                return fJulianDateRep_ == kEmptyJulianRep ? kMinJulianRep : fJulianDateRep_;
            }
            inline  constexpr   bool    Date::empty () const
            {
                return fJulianDateRep_ == kEmptyJulianRep;
            }
            inline  int Date::Compare (const Date& rhs) const
            {
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
            inline  String  Date::ToString () const
            {
                return Format ();
            }
            inline  Date& Date::operator++ ()
            {
                *this = this->AddDays (1);
                return *this;
            }
            inline  Date Date::operator++ (int)
            {
                Date tmp = *this;
                *this = this->AddDays (1);
                return *this;
            }
            inline  Date   Date::operator+ (SignedJulianRepType daysOffset) const
            {
                return this->AddDays (daysOffset);
            }
            inline  Date   Date::operator- (SignedJulianRepType daysOffset) const
            {
                return this->AddDays (-daysOffset);
            }
#if     qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            constexpr   Date    Date_kMin  { Date::JulianRepType (Date::kMinJulianRep) };
            constexpr   Date    Date_kMax  { Date::JulianRepType (UINT_MAX - 1) };
#else
            constexpr   Date    Date::kMin  { Date::JulianRepType (Date::kMinJulianRep) };
            constexpr   Date    Date::kMax  { Date::JulianRepType (UINT_MAX - 1) };
#endif


            /*
             ********************************************************************************
             ************************* Time::Date operators *********************************
             ********************************************************************************
             */
            inline  bool    operator< (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline  bool    operator<= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline  bool    operator== (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) == 0;
            }
            inline  bool    operator!= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) != 0;
            }
            inline  bool    operator>= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            inline  bool    operator> (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }
        }


        namespace   Execution {
            template    <>
            [[noreturn]]    inline  void    Throw (const Time::Date::FormatException& e2Throw)
            {
                DbgTrace (L"Throwing Date::FormatException");
                throw e2Throw;
            }
        }


    }
}
#endif  /*_Stroika_Foundation_Time_Date_inl_*/
