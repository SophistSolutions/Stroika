/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
    for (const String& token : headerValue.Tokenize ({' ', ','})) {
        Containers::Sequence<String> kvp = token.Tokenize ({'='});
        if (kvp.length () == 2) {
            // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Keep-Alive
            if (kvp[0] == "timeout"sv) {
                r.fTimeout = Time::DurationSeconds{Characters::FloatConversion::ToFloat<> (kvp[1])};
                return r;
            }
            else if (kvp[0] == "max"sv) {
                unsigned int maxMsg = Characters::String2Int<unsigned int> (kvp[1]);
                r.fMessages         = maxMsg;
                return r;
            }
            else {
                DbgTrace (L"Keep-Alive header bad: %s", token.As<wstring> ().c_str ());
            }
        }
        else {
            DbgTrace (L"Keep-Alive header bad: %s", token.As<wstring> ().c_str ());
        }
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
    StringBuilder sb; // ? is this a BUG or needs some explanation - LGP 2023-09-24
    return sb.str ();
}

String KeepAlive::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fMessages) {
        sb << "Messages: "sv << Characters::ToString (*fMessages) << ", "sv;
    }
    if (fTimeout) {
        sb << "Timeout: "sv << Characters::ToString (*fTimeout);
    }
    sb << "}"sv;
    return sb.str ();
}