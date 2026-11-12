/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"

#include "KeepAlive.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

/*
 ********************************************************************************
 ********************************* HTTP::KeepAlive ******************************
 ********************************************************************************
 */
KeepAlive KeepAlive::Parse (const String& headerValue)
{
    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Keep-Alive: timeout and max, in any order; a parameter whose
    // value is not a number is skipped
    KeepAlive r;
    for (const String& token : headerValue.Tokenize ({' ', ','})) {
        Containers::Sequence<String> kvp = token.Tokenize ({'='});
        if (kvp.length () == 2 and kvp[0] == "timeout"sv) {
            if (auto t = Characters::Number::Parse<double> (kvp[1]); t and *t >= 0) {
                r.fTimeout = Time::DurationSeconds{*t};
                continue;
            }
        }
        else if (kvp.length () == 2 and kvp[0] == "max"sv) {
            if (auto m = Characters::Number::Parse<unsigned int> (kvp[1])) {
                r.fMessages = *m;
                continue;
            }
        }
        DbgTrace ("Keep-Alive header bad: {}"_f, token);
    }
    return r;
}

optional<KeepAlive> KeepAlive::Merge (const optional<KeepAlive>& lhs, const optional<KeepAlive>& rhs)
{
    if (lhs and rhs) {
        KeepAlive r = *lhs;
        r.fMessages = Memory::NullCoalesce (lhs->fMessages, rhs->fMessages);
        r.fTimeout  = Memory::NullCoalesce (lhs->fTimeout, rhs->fTimeout);
        return r;
    }
    return Memory::NullCoalesce (lhs, rhs);
}

String KeepAlive::AsValue () const
{
    StringBuilder sb;
    if (fTimeout) {
        sb << "timeout="sv << "{}"_f(fTimeout->count ());
    }
    if (fMessages) {
        if (not sb.empty ()) {
            sb << ", "sv;
        }
        sb << "max="sv << "{}"_f(*fMessages);
    }
    return sb;
}

String KeepAlive::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fMessages) {
        sb << "messages: "sv << *fMessages;
    }
    if (fTimeout) {
        if (fMessages) {
            sb << ", "sv;
        }
        sb << "timeout: "sv << *fTimeout;
    }
    sb << "}"sv;
    return sb;
}