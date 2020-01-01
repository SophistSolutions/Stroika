/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstring>

#include "../Containers/Common.h"

#include "Character.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

namespace {
    inline int Compare_CS_ (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd)
    {
        // TODO: Need a more efficient implementation - but this should do for starters...
        Assert (lhsStart <= lhsEnd);
        size_t         lLen                    = (lhsEnd - lhsStart);
        size_t         rLen                    = (rhsEnd - rhsStart);
        size_t         length                  = min (lLen, rLen);
        constexpr bool kUseMemCmpAsSpeedTweek_ = true; // In Visual studio.net 2k17 (15.8.4) this made a big difference (but on a small time so unclear) in one benchmark
        if constexpr (kUseMemCmpAsSpeedTweek_) {
            int tmp = ::memcmp (lhsStart, rhsStart, length * sizeof (Character));
            if (tmp != 0) {
                return Containers::CompareResultNormalizeHelper<int, int> (tmp);
            }
        }
        else {
            for (size_t i = 0; i < length; i++) {
                if (lhsStart[i].GetCharacterCode () != rhsStart[i].GetCharacterCode ()) {
                    return (lhsStart[i].GetCharacterCode () - rhsStart[i].GetCharacterCode ());
                }
            }
        }
        return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
    }

    inline int Compare_CI_ (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd)
    {
        // TODO: Need a more efficient implementation - but this should do for starters...
        Assert (lhsStart <= lhsEnd);
        // Not sure wcscasecmp even helps because of convert to c-str
        //return ::wcscasecmp (l.c_str (), r.c_str ());;
        size_t lLen   = (lhsEnd - lhsStart);
        size_t rLen   = (rhsEnd - rhsStart);
        size_t length = min (lLen, rLen);
        for (size_t i = 0; i < length; i++) {
            Character lc = lhsStart[i].ToLowerCase ();
            Character rc = rhsStart[i].ToLowerCase ();
            if (lc.GetCharacterCode () != rc.GetCharacterCode ()) {
                return (lc.GetCharacterCode () - rc.GetCharacterCode ());
            }
        }
        return Containers::CompareResultNormalizeHelper<ptrdiff_t, int> (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
    }
}

int Character::Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co)
{
    Require (co == CompareOptions::eWithCase or co == CompareOptions::eCaseInsensitive);
    Require (lhsStart <= lhsEnd);
    Require (rhsStart <= rhsEnd);
    switch (co) {
        case CompareOptions::eWithCase:
            return Compare_CS_ (lhsStart, lhsEnd, rhsStart, rhsEnd);
        case CompareOptions::eCaseInsensitive:
            return Compare_CI_ (lhsStart, lhsEnd, rhsStart, rhsEnd);
        default:
            AssertNotReached ();
            return 0;
    }
}
