/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require ((kMinJulianRep <= julianRep and julianRep <= kMaxJulianRep) or julianRep == kEmptyJulianRep);
#endif
            }
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
            constexpr
#endif
                inline Date::Date (Year year, MonthOfYear month, DayOfMonth day)
                : fJulianDateRep_ (jday_ (month, day, year))
            {
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                // Gregorian calendar started on Sep. 14, 1752
                Require (year >= Year::eFirstYear);
                Require (year > Year (1752) or (month > MonthOfYear::eSeptember) or (month == MonthOfYear::eSeptember and day >= DayOfMonth (14)));
#endif
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

            [[deprecated ("use Date::min ()")]] constexpr Date Date_kMin{Date::JulianRepType (Date::kMinJulianRep)};
            [[deprecated ("use Date::max ()")]] constexpr Date Date_kMax{Date::JulianRepType (UINT_MAX - 1)};

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
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template <>
            struct DefaultNames<Stroika::Foundation::Time::DayOfWeek> : EnumNames<Stroika::Foundation::Time::DayOfWeek> {
                static constexpr EnumNames<Stroika::Foundation::Time::DayOfWeek> k{
                    EnumNames<Stroika::Foundation::Time::DayOfWeek>::BasicArrayInitializer{
                        {
                            {Stroika::Foundation::Time::DayOfWeek::eMonday, L"Monday"},
                            {Stroika::Foundation::Time::DayOfWeek::eTuesday, L"Tuesday"},
                            {Stroika::Foundation::Time::DayOfWeek::eWednesday, L"Wednesday"},
                            {Stroika::Foundation::Time::DayOfWeek::eThursday, L"Thursday"},
                            {Stroika::Foundation::Time::DayOfWeek::eFriday, L"Friday"},
                            {Stroika::Foundation::Time::DayOfWeek::eSaturday, L"Saturday"},
                            {Stroika::Foundation::Time::DayOfWeek::eSunday, L"Sunday"},
                        }}};
                DefaultNames ()
                    : EnumNames<Stroika::Foundation::Time::DayOfWeek> (k)
                {
                }
            };
            template <>
            struct DefaultNames<Stroika::Foundation::Time::MonthOfYear> : EnumNames<Stroika::Foundation::Time::MonthOfYear> {
                static constexpr EnumNames<Stroika::Foundation::Time::MonthOfYear> k{
                    EnumNames<Stroika::Foundation::Time::MonthOfYear>::BasicArrayInitializer{
                        {
                            {Stroika::Foundation::Time::MonthOfYear::eJanuary, L"January"},
                            {Stroika::Foundation::Time::MonthOfYear::eFebruary, L"February"},
                            {Stroika::Foundation::Time::MonthOfYear::eMarch, L"March"},
                            {Stroika::Foundation::Time::MonthOfYear::eApril, L"April"},
                            {Stroika::Foundation::Time::MonthOfYear::eMay, L"May"},
                            {Stroika::Foundation::Time::MonthOfYear::eJune, L"June"},
                            {Stroika::Foundation::Time::MonthOfYear::eJuly, L"July"},
                            {Stroika::Foundation::Time::MonthOfYear::eAugust, L"August"},
                            {Stroika::Foundation::Time::MonthOfYear::eSeptember, L"September"},
                            {Stroika::Foundation::Time::MonthOfYear::eOctober, L"October"},
                            {Stroika::Foundation::Time::MonthOfYear::eNovember, L"November"},
                            {Stroika::Foundation::Time::MonthOfYear::eDecember, L"December"},
                        }}};
                DefaultNames ()
                    : EnumNames<Stroika::Foundation::Time::MonthOfYear> (k)
                {
                }
            };
            template <>
            struct DefaultNames<Stroika::Foundation::Time::Date::ParseFormat> : EnumNames<Stroika::Foundation::Time::Date::ParseFormat> {
                static constexpr EnumNames<Stroika::Foundation::Time::Date::ParseFormat> k{
                    EnumNames<Stroika::Foundation::Time::Date::ParseFormat>::BasicArrayInitializer{
                        {
                            {Stroika::Foundation::Time::Date::ParseFormat::eCurrentLocale, L"Current-Locale"},
                            {Stroika::Foundation::Time::Date::ParseFormat::eISO8601, L"ISO-8601"},
                            {Stroika::Foundation::Time::Date::ParseFormat::eXML, L"XML"},
                            {Stroika::Foundation::Time::Date::ParseFormat::eJavascript, L"Javascript"},
                        }}};
                DefaultNames ()
                    : EnumNames<Stroika::Foundation::Time::Date::ParseFormat> (k)
                {
                }
            };
            template <>
            struct DefaultNames<Stroika::Foundation::Time::Date::PrintFormat> : EnumNames<Stroika::Foundation::Time::Date::PrintFormat> {
                static constexpr EnumNames<Stroika::Foundation::Time::Date::PrintFormat> k{
                    EnumNames<Stroika::Foundation::Time::Date::PrintFormat>::BasicArrayInitializer{
                        {
                            {Stroika::Foundation::Time::Date::PrintFormat::eCurrentLocale, L"Current-Locale"},
                            {Stroika::Foundation::Time::Date::PrintFormat::eISO8601, L"ISO-8601"},
                            {Stroika::Foundation::Time::Date::PrintFormat::eXML, L"XML"},
                            {Stroika::Foundation::Time::Date::PrintFormat::eJavascript, L"Javascript"},
                            {Stroika::Foundation::Time::Date::PrintFormat::eCurrentLocale_WithZerosStripped, L"Current-Locale-With-Zeros-Stripped"},
                        }}};
                DefaultNames ()
                    : EnumNames<Stroika::Foundation::Time::Date::PrintFormat> (k)
                {
                }
            };
        }
    }
}
#endif /*_Stroika_Foundation_Time_Date_inl_*/
