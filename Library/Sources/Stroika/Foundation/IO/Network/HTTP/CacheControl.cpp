/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Streams/TextReader.h"
#include "Stroika/Foundation/Traversal/DiscreteRange.h"

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
    for (const String& v : headerValue.Tokenize ({',', ' '})) {
        // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Cache-Control case insensitive
        Assert (not v.empty ());
        Sequence<String> vv = v.Tokenize ({'='});
        Assert (not vv.empty ()); // cuz the first tokenize will never return an empty item
        String token = vv[0];
        for (const Cacheability& sr : DiscreteRange<Cacheability>::FullRange ()) {
            if (String::EqualsComparer{eCaseInsensitive}(token, DefaultNames<Cacheability>{}.GetName (sr))) {
                r.fCacheability = sr;
                goto DoneWithV;
            }
        }
        if (String::EqualsComparer{eCaseInsensitive}(token, "must-revalidate"sv)) {
            r.fMustRevalidate = true;
        }
        else if (String::EqualsComparer{eCaseInsensitive}(token, "no-transform"sv)) {
            r.fNoTransform = true;
        }
        else if (String::EqualsComparer{eCaseInsensitive}(token, "only-if-cached"sv)) {
            r.fOnlyIfCached = true;
        }
        else if (String::EqualsComparer{eCaseInsensitive}(token, "age"sv) && vv.length () >= 2) {
            r.fAge = parseInt (vv[1]);
        }
        else if (String::EqualsComparer{eCaseInsensitive}(token, "max-age"sv) && vv.length () >= 2) {
            r.fMaxAge = parseInt (vv[1]);
        }
        else if (String::EqualsComparer{eCaseInsensitive}(token, "s-max-age"sv) && vv.length () >= 2) {
            r.fSharedMaxAge = parseInt (vv[1]);
        }
        else if (String::EqualsComparer{eCaseInsensitive}(token, "max-stale"sv)) {
            MaxStale m;
            if (vv.length () >= 2) {
                m.fAmount = parseInt (vv[1]);
            }
            r.fMaxStale = m;
        }
        else if (String::EqualsComparer{eCaseInsensitive}(token, "min-fresh"sv) && vv.length () >= 2) {
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
            sb << ", "sv;
        }
    };
    if (fCacheability) {
        sb << DefaultNames<Cacheability>{}.GetName (*fCacheability);
    }
    if (fMustRevalidate) {
        handleComma ();
        sb << "must-revalidate"sv;
    }
    if (fImmutable) {
        handleComma ();
        sb << "immutable"sv;
    }
    if (fNoTransform) {
        handleComma ();
        sb << "no-transform"sv;
    }
    if (fOnlyIfCached) {
        handleComma ();
        sb << "only-if-cached"sv;
    }
    if (fAge) {
        handleComma ();
        sb << Characters::Format ("age={}"_f, *fAge);
    }
    if (fMaxAge) {
        handleComma ();
        sb << Characters::Format ("max-age={}"_f, *fMaxAge);
    }
    if (fSharedMaxAge) {
        handleComma ();
        sb << Characters::Format ("s-max-age={}"_f, *fSharedMaxAge);
    }
    if (fMaxStale) {
        handleComma ();
        sb << "max-stale"sv;
        if (fMaxStale->fAmount) {
            sb << Characters::Format ("={}"_f, *fMaxStale->fAmount);
        }
    }
    if (fMinFresh) {
        handleComma ();
        sb << Characters::Format ("min-fresh={}"_f, *fMinFresh);
    }
    return sb.str ();
}
