/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Debug/Assertions.h"

#include "Realtime.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Time;

namespace {
    template <typename Clock, class Duration>
    inline DurationSecondsType cvt_ (const time_point<Clock, Duration>& tp)
    {
        return chrono::duration<DurationSecondsType>{tp.time_since_epoch ()}.count ();
    }
}

namespace Stroika::Foundation::Time::Private_ {
    /*
     ********************************************************************************
     *********************** Time::Private::GetClockTickountOffset_ *****************
     ********************************************************************************
     */
    template <>
    DurationSecondsType GetClockTickountOffset_<chrono::steady_clock> ()
    {
        // in CPP file, so that we can guarantee kFirstTC_ done exactly once, and have value not depend on which module compiled against/in what order etc.
        // And must be done in function - not file scope - cuz must be done first time referenced, and nearly always before main ()
        static const DurationSecondsType kFirstTC_ = cvt_ (chrono::steady_clock::now ());
        return kFirstTC_;
    }
}

namespace {
    inline DurationSecondsTimePoint::duration GetAppStartOffset_ ()
    {
        // @todo not sure this is right --LGP 2023-11-16 - at least if it is needs better comment/explanation...
        static const DurationSecondsTimePoint kTimeAppStarted_ =
            chrono::time_point_cast<DurationSecondsTimePoint::duration> (chrono::steady_clock::now ());
        return kTimeAppStarted_.time_since_epoch ();
    }
}

DurationSecondsTimePoint Time::FromAppStartRelative (const DurationSecondsTimePoint& tp)
{
    return tp + GetAppStartOffset_ ();
}

DurationSecondsTimePoint Time::ToAppStartRelative (const DurationSecondsTimePoint& tp)
{
    return tp - GetAppStartOffset_ ();
}