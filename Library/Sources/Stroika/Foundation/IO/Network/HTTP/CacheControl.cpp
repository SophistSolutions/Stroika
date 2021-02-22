/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/Format.h"
#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Streams/TextReader.h"
#include "../../../Traversal/DiscreteRange.h"

#include "CacheControl.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Configuration;
using namespace Stroika::Foundation::IO::Network::HTTP;
using namespace Stroika::Foundation::Traversal;

/*
 ********************************************************************************
 *************************** HTTP::CacheControl *********************************
 ********************************************************************************
 */
CacheControl CacheControl::Parse (const String& headerValue)
{
    CacheControl r;
    auto         parseInt = [] (const String& i) -> optional<uint32_t> {
        // @todo validate/tweak/keep in range
        auto v = String2Int<int32_t> (i);
        return v < 0 ? optional<uint32_t>{} : static_cast<uint32_t> (v);
    };
    for (String v : headerValue.Tokenize ({',', ' '})) {
        // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control case insensitive
        Assert (not v.empty ());
        Sequence<String> vv = v.Tokenize ({'='});
        Assert (not vv.empty ()); // cuz the first tokenize will never return an empty item
        String token = vv[0];
        for (StoreRestriction sr : DiscreteRange<StoreRestriction>::FullRange ()) {
            if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, DefaultNames<StoreRestriction>{}.GetName (sr))) {
                r.fStoreRestriction = sr;
                goto DoneWithV;
            }
        }
        for (Visibility vis : DiscreteRange<Visibility>::FullRange ()) {
            if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, DefaultNames<Visibility>{}.GetName (vis))) {
                r.fVisibility = vis;
                goto DoneWithV;
            }
        }
        if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, L"must-revalidate")) {
            r.fMustRevalidate = true;
        }
        else if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, L"immutable")) {
            r.fImmutable = true;
        }
        else if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, L"age") && vv.length () >= 2) {
            r.fAge = parseInt (vv[1]);
        }
        else if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, L"max-age") && vv.length () >= 2) {
            r.fMaxAge = parseInt (vv[1]);
        }
        else if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, L"s-max-age") && vv.length () >= 2) {
            r.fSharedMaxAge = parseInt (vv[1]);
        }
        else if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, L"max-stale")) {
            MaxStale m;
            if (vv.length () >= 2) {
                m.fAmount = parseInt (vv[1]);
            }
            r.fMaxStale = m;
        }
        else if (String::EqualsComparer{CompareOptions::eCaseInsensitive}(token, L"min-fresh") && vv.length () >= 2) {
            r.fMinFresh = parseInt (vv[1]);
        }
    DoneWithV:;
    }
    return r;
}

template <>
String CacheControl::As () const
{
    StringBuilder sb;
    auto          handleComma = [&] () {
        if (not sb.empty ()) {
            sb += L", ";
        }
    };
    if (fStoreRestriction) {
        sb += DefaultNames<StoreRestriction>{}.GetName (*fStoreRestriction);
    }
    if (fVisibility) {
        handleComma ();
        sb += DefaultNames<Visibility>{}.GetName (*fVisibility);
    }
    if (fMustRevalidate) {
        handleComma ();
        sb += L"must-revalidate"sv;
    }
    if (fImmutable) {
        handleComma ();
        sb += L"immutable"sv;
    }
    if (fAge) {
        handleComma ();
        sb += Characters::Format (L"age=%d", *fAge);
    }
    if (fMaxAge) {
        handleComma ();
        sb += Characters::Format (L"max-age=%d", *fMaxAge);
    }
    if (fSharedMaxAge) {
        handleComma ();
        sb += Characters::Format (L"s-max-age=%d", *fSharedMaxAge);
    }
    if (fMaxStale) {
        handleComma ();
        sb += L"max-stale";
        if (fMaxStale->fAmount) {
            sb += Characters::Format (L"=%d", *fMaxStale->fAmount);
        }
    }
    if (fMinFresh) {
        handleComma ();
        sb += Characters::Format (L"min-fresh=%d", *fMinFresh);
    }
    return sb.str ();
}
