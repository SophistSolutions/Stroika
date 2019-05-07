/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Time_TimeOfDay_inl_
#define _Stroika_Foundation_Time_TimeOfDay_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ************************************ TimeOfDay *********************************
     ********************************************************************************
     */
    inline constexpr TimeOfDay::TimeOfDay ()
        : fTime_{static_cast<unsigned int> (-1)}
    {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        Assert (empty () or fTime_ < kMaxSecondsPerDay);
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    }
    inline constexpr TimeOfDay::TimeOfDay (uint32_t t)
        : fTime_ (t < kMaxSecondsPerDay ? t : (kMaxSecondsPerDay - 1))
    {
        Require (t < kMaxSecondsPerDay); // required added v2.0a227 - so still leave in check for a short while
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        Assert (empty () or fTime_ < kMaxSecondsPerDay);
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    }
    inline constexpr TimeOfDay::TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds)
        : TimeOfDay (static_cast<uint32_t> (((hour * 60) + minute) * 60 + seconds))
    {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        Assert (empty () or fTime_ < kMaxSecondsPerDay);
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
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
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        Ensure ((empty () ? 0 : fTime_) < kMaxSecondsPerDay);
        return empty () ? 0 : fTime_;
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    }
    constexpr uint8_t TimeOfDay::GetHours () const
    {
        uint32_t n = GetAsSecondsCount () / (60 * 60);
        Ensure (0 <= n and n <= 23);
        return static_cast<uint8_t> (n);
    }
    constexpr uint8_t TimeOfDay::GetMinutes () const
    {
        uint32_t n = GetAsSecondsCount ();
        n -= GetHours () * 60 * 60;
        n /= 60;
        Ensure (0 <= n and n <= 59);
        return static_cast<uint8_t> (n);
    }
    constexpr uint8_t TimeOfDay::GetSeconds () const
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
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        if (empty ()) {
            return rhs.empty () ? 0 : -1;
        }
        else {
            return rhs.empty () ? 1 : (GetAsSecondsCount () - rhs.GetAsSecondsCount ());
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    }
    inline int TimeOfDay::Compare (const optional<TimeOfDay>& lhs, const optional<TimeOfDay>& rhs)
    {
        if (lhs.has_value ()) {
            return rhs.has_value () ? (lhs->GetAsSecondsCount () - rhs->GetAsSecondsCount ()) : 1;
        }
        else {
            return rhs.has_value () ? -1 : 0;
        }
    }

    /*
     ********************************************************************************
     *************************** TimeOfDay::ThreeWayComparer ************************
     ********************************************************************************
     */
    inline int TimeOfDay::ThreeWayComparer::operator() (const TimeOfDay& lhs, const TimeOfDay& rhs) const
    {
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        if (lhs.empty ()) {
            return rhs.empty () ? 0 : -1;
        }
        else {
            return rhs.empty () ? 1 : (lhs.GetAsSecondsCount () - rhs.GetAsSecondsCount ());
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    }

    /*
     ********************************************************************************
     ************************* TimeOfDay operators **********************************
     ********************************************************************************
     */
    inline bool operator< (TimeOfDay lhs, TimeOfDay rhs)
    {
        return TimeOfDay::ThreeWayComparer{}(lhs, rhs) < 0;
    }
    inline bool operator<= (TimeOfDay lhs, TimeOfDay rhs)
    {
        return TimeOfDay::ThreeWayComparer{}(lhs, rhs) <= 0;
    }
    inline bool operator== (TimeOfDay lhs, TimeOfDay rhs)
    {
        return TimeOfDay::ThreeWayComparer{}(lhs, rhs) == 0;
    }
    inline bool operator!= (TimeOfDay lhs, TimeOfDay rhs)
    {
        return TimeOfDay::ThreeWayComparer{}(lhs, rhs) != 0;
    }
    inline bool operator>= (TimeOfDay lhs, TimeOfDay rhs)
    {
        return TimeOfDay::ThreeWayComparer{}(lhs, rhs) >= 0;
    }
    inline bool operator> (TimeOfDay lhs, TimeOfDay rhs)
    {
        return TimeOfDay::ThreeWayComparer{}(lhs, rhs) > 0;
    }

}

#endif /*_Stroika_Foundation_Time_TimeOfDay_inl_*/
