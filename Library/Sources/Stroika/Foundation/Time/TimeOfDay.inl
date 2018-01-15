/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_TimeOfDay_inl_
#define _Stroika_Foundation_Time_TimeOfDay_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace Stroika {
    namespace Foundation {
        namespace Time {

            /*
             ********************************************************************************
             ************************************ TimeOfDay *********************************
             ********************************************************************************
             */
            inline constexpr TimeOfDay::TimeOfDay ()
                : fTime_ (-1)
            {
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Assert (empty () or fTime_ < kMaxSecondsPerDay);
#endif
            }
            inline constexpr TimeOfDay::TimeOfDay (uint32_t t)
                : fTime_ (t < kMaxSecondsPerDay ? t : (kMaxSecondsPerDay - 1))
            {
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Assert (empty () or fTime_ < kMaxSecondsPerDay);
#endif
            }
            inline TimeOfDay::TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds)
                : TimeOfDay (static_cast<uint32_t> (((hour * 60) + minute) * 60 + seconds))
            {
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Assert (empty () or fTime_ < kMaxSecondsPerDay);
#endif
            }
            inline constexpr bool TimeOfDay::empty () const
            {
                return fTime_ == static_cast<unsigned int> (-1);
            }
            inline constexpr unsigned int TimeOfDay::GetAsSecondsCount () const
            {
#if !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Ensure ((empty () ? 0 : fTime_) < kMaxSecondsPerDay);
#endif
                return empty () ? 0 : fTime_;
            }
            inline uint8_t TimeOfDay::GetHours () const
            {
                uint32_t n = GetAsSecondsCount () / (60 * 60);
                Ensure (0 <= n and n <= 23);
                return n;
            }
            inline uint8_t TimeOfDay::GetMinutes () const
            {
                uint32_t n = GetAsSecondsCount ();
                n -= GetHours () * 60 * 60;
                n /= 60;
                Ensure (0 <= n and n <= 59);
                return n;
            }
            inline uint8_t TimeOfDay::GetSeconds () const
            {
                uint32_t n = GetAsSecondsCount ();
                n -= GetHours () * 60 * 60;
                n -= GetMinutes () * 60;
                Ensure (0 <= n and n <= 59);
                return n;
            }
            inline String TimeOfDay::ToString () const
            {
                return Format ();
            }
            inline int TimeOfDay::Compare (const TimeOfDay& rhs) const
            {
                if (empty ()) {
                    return rhs.empty () ? 0 : -1;
                }
                else {
                    return rhs.empty () ? 1 : (GetAsSecondsCount () - rhs.GetAsSecondsCount ());
                }
            }
            constexpr TimeOfDay TimeOfDay_kMin{0};
            constexpr TimeOfDay TimeOfDay_kMax{TimeOfDay::kMaxSecondsPerDay - 1};
            constexpr TimeOfDay TimeOfDay::kMin{0};
            constexpr TimeOfDay TimeOfDay::kMax{TimeOfDay::kMaxSecondsPerDay - 1};

            /*
             ********************************************************************************
             ************************* TimeOfDay operators **********************************
             ********************************************************************************
             */
            inline bool operator< (TimeOfDay lhs, TimeOfDay rhs)
            {
                return lhs.Compare (rhs) < 0;
            }
            inline bool operator<= (TimeOfDay lhs, TimeOfDay rhs)
            {
                return lhs.Compare (rhs) <= 0;
            }
            inline bool operator== (TimeOfDay lhs, TimeOfDay rhs)
            {
                return lhs.Compare (rhs) == 0;
            }
            inline bool operator!= (TimeOfDay lhs, TimeOfDay rhs)
            {
                return lhs.Compare (rhs) != 0;
            }
            inline bool operator>= (TimeOfDay lhs, TimeOfDay rhs)
            {
                return lhs.Compare (rhs) >= 0;
            }
            inline bool operator> (TimeOfDay lhs, TimeOfDay rhs)
            {
                return lhs.Compare (rhs) > 0;
            }
        }
    }
}
#endif /*_Stroika_Foundation_Time_TimeOfDay_inl_*/
