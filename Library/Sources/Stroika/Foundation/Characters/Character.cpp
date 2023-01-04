/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <cstring>

#include "../Containers/Common.h"

#include "Character.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

namespace {
    inline strong_ordering Compare_CS_ (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd)
    {
        // TODO: Need a more efficient implementation - but this should do for starters...
        Assert (lhsStart <= lhsEnd);
        size_t         lLen                    = lhsEnd - lhsStart;
        size_t         rLen                    = rhsEnd - rhsStart;
        size_t         length                  = min (lLen, rLen);
        for (size_t i = 0; i < length; ++i) {
            if (lhsStart[i].GetCharacterCode () != rhsStart[i].GetCharacterCode ()) {
                return lhsStart[i].GetCharacterCode () <=> rhsStart[i].GetCharacterCode ();
            }
        }
        return Common::CompareResultNormalizer (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
    }
    inline strong_ordering Compare_CI_ (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd)
    {
        // TODO: Need a more efficient implementation - but this should do for starters...
        Assert (lhsStart <= lhsEnd);
        // Not sure wcscasecmp even helps because of convert to c-str
        //return ::wcscasecmp (l.c_str (), r.c_str ());;
        size_t lLen   = lhsEnd - lhsStart;
        size_t rLen   = rhsEnd - rhsStart;
        size_t length = min (lLen, rLen);
        for (size_t i = 0; i < length; ++i) {
            Character lc = lhsStart[i].ToLowerCase ();
            Character rc = rhsStart[i].ToLowerCase ();
            if (lc.GetCharacterCode () != rc.GetCharacterCode ()) {
                return lc.GetCharacterCode () <=> rc.GetCharacterCode ();
            }
        }
        return Common::CompareResultNormalizer (static_cast<ptrdiff_t> (lLen) - static_cast<ptrdiff_t> (rLen));
    }
}

strong_ordering Character::Compare (span<const Character> lhs, span<const Character> rhs, CompareOptions co) noexcept
{
    Require (co == CompareOptions::eWithCase or co == CompareOptions::eCaseInsensitive);
    switch (co) {
        case CompareOptions::eWithCase:
            return Compare_CS_ (lhs.data (), lhs.data () + lhs.size (), rhs.data (), rhs.data () + rhs.size ());
        case CompareOptions::eCaseInsensitive:
            return Compare_CI_ (lhs.data (), lhs.data () + lhs.size (), rhs.data (), rhs.data () + rhs.size ());
        default:
            AssertNotReached ();
            return strong_ordering::equal;
    }
}

strong_ordering Character::Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co) noexcept
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
            return strong_ordering::equal;
    }
}
