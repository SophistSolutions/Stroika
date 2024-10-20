/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Foundation::Time {

    /*
     ********************************************************************************
     ************************************ TimeOfDay *********************************
     ********************************************************************************
     */
    inline constexpr TimeOfDay::TimeOfDay (uint32_t t)
        : fTime_{t}
    {
        Require (t < kMaxSecondsPerDay);
        Assert (fTime_ < kMaxSecondsPerDay);
    }
    inline constexpr TimeOfDay::TimeOfDay (unsigned int hour, unsigned int minute, unsigned int seconds)
        : TimeOfDay{static_cast<uint32_t> (((hour * 60) + minute) * 60 + seconds)}
    {
        Assert (fTime_ < kMaxSecondsPerDay);
    }
    inline constexpr TimeOfDay    TimeOfDay::kMin{0};
    inline constexpr TimeOfDay    TimeOfDay::kMax{TimeOfDay::kMaxSecondsPerDay - 1};
    inline constexpr unsigned int TimeOfDay::GetAsSecondsCount () const
    {
        Ensure (fTime_ < kMaxSecondsPerDay);
        return fTime_;
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
#if not(qCompilerAndStdLib_linkerLosesInlinesSoCannotBeSeenByDebugger_Buggy && qDebug)
    inline String TimeOfDay::ToString () const
    {
        return Format ();
    }
#endif

}
