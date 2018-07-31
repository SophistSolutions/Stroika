/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_TimeOfDay_inl_
#define _Stroika_Foundation_Time_TimeOfDay_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Execution/Exceptions.h"

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ************************************ TimeOfDay *********************************
     ********************************************************************************
     */
    inline constexpr TimeOfDay::TimeOfDay ()
        : fTime_{static_cast<unsigned int> (-1)}
    {
        Assert (empty () or fTime_ < kMaxSecondsPerDay);
    }
    inline constexpr TimeOfDay::TimeOfDay (uint32_t t)
        : fTime_ (t < kMaxSecondsPerDay ? t : (kMaxSecondsPerDay - 1))
    {
        Require (t < kMaxSecondsPerDay); // required added v2.0a227 - so still leave in check for a short while
        Assert (empty () or fTime_ < kMaxSecondsPerDay);
    }
    inline constexpr TimeOfDay::TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds)
        : TimeOfDay (static_cast<uint32_t> (((hour * 60) + minute) * 60 + seconds))
    {
        Assert (empty () or fTime_ < kMaxSecondsPerDay);
    }
    inline constexpr TimeOfDay TimeOfDay::min ()
    {
        return TimeOfDay{0};
    }
    inline constexpr TimeOfDay TimeOfDay::max ()
    {
        return TimeOfDay{kMaxSecondsPerDay - 1};
    }
    inline constexpr bool TimeOfDay::empty () const
    {
        return fTime_ == static_cast<unsigned int> (-1);
    }
    inline constexpr unsigned int TimeOfDay::GetAsSecondsCount () const
    {
        Ensure ((empty () ? 0 : fTime_) < kMaxSecondsPerDay);
        return empty () ? 0 : fTime_;
    }
    inline uint8_t TimeOfDay::GetHours () const
    {
        uint32_t n = GetAsSecondsCount () / (60 * 60);
        Ensure (0 <= n and n <= 23);
        return static_cast<uint8_t> (n);
    }
    inline uint8_t TimeOfDay::GetMinutes () const
    {
        uint32_t n = GetAsSecondsCount ();
        n -= GetHours () * 60 * 60;
        n /= 60;
        Ensure (0 <= n and n <= 59);
        return static_cast<uint8_t> (n);
    }
    inline uint8_t TimeOfDay::GetSeconds () const
    {
        uint32_t n = GetAsSecondsCount ();
        n -= GetHours () * 60 * 60;
        n -= GetMinutes () * 60;
        Ensure (0 <= n and n <= 59);
        return static_cast<uint8_t> (n);
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

#endif /*_Stroika_Foundation_Time_TimeOfDay_inl_*/
