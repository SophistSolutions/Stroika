/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Debug/Assertions.h"

#include "Realtime.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Time;

namespace {
    template <class Clock, class Duration>
    inline DurationSecondsType cvt_ (const time_point<Clock, Duration>& tp)
    {
        return chrono::duration<DurationSecondsType>{tp.time_since_epoch ()}.count ();
    }
}

namespace Stroika {
    namespace Foundation {
        namespace Time {
            namespace Private_ {
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
                    Stroika_Foundation_Debug_ValgrindDisableHelgrind (kFirstTC_); // ignore warning because C++ guarantees magic statics -- LGP 2017-09-14
                    return kFirstTC_;
                }
            }
        }
    }
}
