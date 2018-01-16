/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_Date_inl_
#define _Stroika_Foundation_Time_Date_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"
#include "../Execution/ModuleInit.h"

namespace Stroika {
    namespace Foundation {
        namespace Time {

/*
             ********************************************************************************
             *************************************** Date ***********************************
             ********************************************************************************
             */
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
            constexpr
#endif
                inline Date::JulianRepType
                Date::jday_ (MonthOfYear month, DayOfMonth day, Year year)
            {
                /*
                 * Convert Gregorian calendar date to the corresponding Julian day number
                 * j.  Algorithm 199 from Communications of the ACM, Volume 6, No. 8,
                 * (Aug. 1963), p. 444.  Gregorian calendar started on Sep. 14, 1752.
                 * This function not valid before that.
                 *
                 * (This code originally from NIHCL)
                 */
                if (month == MonthOfYear::eEmptyMonthOfYear or day == DayOfMonth::eEmptyDayOfMonth or year == Year::eEmptyYear) {
                    return Date::kEmptyJulianRep;
                }

                Require (static_cast<int> (year) > 1752 or (static_cast<int> (year) == 1752 and (month > MonthOfYear::eSeptember or (month == MonthOfYear::eSeptember and static_cast<int> (day) >= 14))));

                if (static_cast<int> (month) > 2) {
                    month = static_cast<MonthOfYear> (static_cast<int> (month) - 3);
                }
                else {
                    month = static_cast<MonthOfYear> (static_cast<int> (month) + 9);
                    year  = static_cast<Year> (static_cast<int> (year) - 1);
                }
                Date::JulianRepType c  = static_cast<int> (year) / 100;
                Date::JulianRepType ya = static_cast<int> (year) - 100 * c;
                return (((146097 * c) >> 2) + ((1461 * ya) >> 2) + (153 * static_cast<int> (month) + 2) / 5 + static_cast<int> (day) + 1721119);
            }
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
            constexpr
#endif
                inline Date::JulianRepType
                Date::Safe_jday_ (MonthOfYear month, DayOfMonth day, Year year)
            {
                // 'Safe' version just avoids require that date values are legit for julian date range. If date would be invalid - return kEmptyJulianRep.

                if (month == MonthOfYear::eEmptyMonthOfYear or day == DayOfMonth::eEmptyDayOfMonth or year == Year::eEmptyYear) {
                    return Date::kEmptyJulianRep;
                }
                if (static_cast<int> (year) > 1752 or (static_cast<int> (year) == 1752 and (month > MonthOfYear::eSeptember or (month == MonthOfYear::eSeptember and static_cast<int> (day) >= 14)))) {
                    return jday_ (month, day, year);
                }
                else {
                    return Date::kEmptyJulianRep;
                }
            }
            inline constexpr Date::Date ()
                : fJulianDateRep_ (kEmptyJulianRep)
            {
            }
            inline constexpr Date::Date (JulianRepType julianRep)
                : fJulianDateRep_ (julianRep)
            {
            }
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
            constexpr
#endif
                inline Date::Date (Year year, MonthOfYear month, DayOfMonth day)
                : fJulianDateRep_ (jday_ (month, day, year))
            {
            }
            inline constexpr Date::JulianRepType Date::GetJulianRep () const
            {
                return fJulianDateRep_ == kEmptyJulianRep ? kMinJulianRep : fJulianDateRep_;
            }
            inline constexpr bool Date::empty () const
            {
                return fJulianDateRep_ == kEmptyJulianRep;
            }
            inline int Date::Compare (const Date& rhs) const
            {
                if (empty ()) {
                    return rhs.empty () ? 0 : -1;
                }
                else {
                    if (rhs.empty ()) {
                        return 1;
                    }
                    // careful of signed/unsigned converstions - esp because of kMax which is very large
                    JulianRepType l = GetJulianRep ();
                    JulianRepType r = rhs.GetJulianRep ();
                    if (l == r) {
                        return 0;
                    }
                    return l < r ? -1 : 1;
                }
            }
            inline String Date::ToString () const
            {
                return Format ();
            }
            inline Date& Date::operator++ ()
            {
                *this = this->AddDays (1);
                return *this;
            }
            inline Date Date::operator++ (int)
            {
                Date tmp = *this;
                *this    = this->AddDays (1);
                return *this;
            }
            inline Date Date::operator+ (SignedJulianRepType daysOffset) const
            {
                return this->AddDays (daysOffset);
            }
            inline auto Date::Difference (const Date& rhs) const -> SignedJulianRepType
            {
                return fJulianDateRep_ - rhs.fJulianDateRep_;
            }
            inline auto Date::operator- (const Date& rhs) const -> SignedJulianRepType
            {
                return Difference (rhs);
            }
            inline Date Date::operator- (SignedJulianRepType daysOffset) const
            {
                return this->AddDays (-daysOffset);
            }
            inline constexpr Date Date::min ()
            {
                return Date{kMinJulianRep};
            }
            inline constexpr Date Date::max ()
            {
                return Date{UINT_MAX - 1};
            }

            [[deprecated ("use min ()")]] constexpr Date Date_kMin{Date::JulianRepType (Date::kMinJulianRep)};
            [[deprecated ("use max ()")]] constexpr Date Date_kMax{Date::JulianRepType (UINT_MAX - 1)};
#if !qCompilerAndStdLib_static_constexpr_Of_Type_Being_Defined_Buggy
            [[deprecated ("use min ()")]] constexpr Date Date::kMin{Date::JulianRepType (Date::kMinJulianRep)};
            [[deprecated ("use max ()")]] constexpr Date Date::kMax{Date::JulianRepType (UINT_MAX - 1)};
#endif

            /*
             ********************************************************************************
             ************************* Time::Date operators *********************************
             ********************************************************************************
             */
            inline bool operator< (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline bool operator<= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline bool operator== (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) == 0;
            }
            inline bool operator!= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) != 0;
            }
            inline bool operator>= (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            inline bool operator> (const Date& lhs, const Date& rhs)
            {
                return lhs.Compare (rhs) > 0;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Time_Date_inl_*/
