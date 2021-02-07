/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#include "../../../Characters/String2Int.h"
#include "../../../Characters/StringBuilder.h"
#include "../../../Characters/ToString.h"
#include "../../../Containers/Set.h"
#include "../../../Streams/TextReader.h"

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
    KeepAlive r;
    for (String token : headerValue.Tokenize ({' ', ','})) {
        Containers::Sequence<String> kvp = token.Tokenize ({'='});
        if (kvp.length () == 2) {
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Keep-Alive
            if (kvp[0] == L"timeout"sv) {
                Time::DurationSecondsType toAt = Characters::String2Float<> (kvp[1]);
                r.fTimeoutAt                   = Time::GetTickCount () + toAt;
                return r;
            }
            else if (kvp[0] == L"max"sv) {
                unsigned int maxMsg = Characters::String2Int<unsigned int> (kvp[1]);
                r.fMessages         = maxMsg;
                return r;
            }
            else {
                DbgTrace (L"Keep-Alive header bad: %s", token.c_str ());
            }
        }
        else {
            DbgTrace (L"Keep-Alive header bad: %s", token.c_str ());
        }
    }
    return r;
}

optional<KeepAlive> KeepAlive::Merge (const optional<KeepAlive>& lhs, const optional<KeepAlive>& rhs)
{
    if (lhs and rhs) {
        KeepAlive r  = *lhs;
        r.fMessages  = Memory::NullCoalesce (lhs->fMessages, rhs->fMessages);
        r.fTimeoutAt = Memory::NullCoalesce (lhs->fTimeoutAt, rhs->fTimeoutAt);
        return r;
    }
    return Memory::NullCoalesce (lhs, rhs);
}

String KeepAlive::AsValue () const
{
    StringBuilder sb;

    return sb.str ();
}

String KeepAlive::ToString () const
{
    StringBuilder sb;
    sb += L"{";
    if (fMessages) {
        sb += L"Messages: " + Characters::ToString (*fMessages) + L", ";
    }
    if (fTimeoutAt) {
        sb += L"Timeout-At: " + Characters::ToString (*fTimeoutAt) + L", ";
    }
    sb += L"}";
    return sb.str ();
}