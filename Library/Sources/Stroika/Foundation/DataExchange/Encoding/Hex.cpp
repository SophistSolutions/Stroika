/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <cctype>

#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

#include "Hex.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::DataExchange;

/*
 ********************************************************************************
 ************************** DataExchange::Encoding::Hex *************************
 ********************************************************************************
 */
string Encoding::Hex::Encode (span<const byte> from)
{
    constexpr char kDigits_[] = "0123456789abcdef";
    string         result;
    result.reserve (2 * from.size ());
    for (byte b : from) {
        result += kDigits_[to_integer<unsigned int> (b) >> 4];
        result += kDigits_[to_integer<unsigned int> (b) & 0xf];
    }
    return result;
}

string Encoding::Hex::Encode (const Memory::BLOB& from)
{
    return Encode (from.As<span<const byte>> ());
}

Memory::BLOB Encoding::Hex::Decode (span<const char> s)
{
    auto digit = [] (char c) -> byte {
        if ('0' <= c and c <= '9') [[likely]] {
            return static_cast<byte> (c - '0');
        }
        if ('a' <= c and c <= 'f') [[likely]] {
            return static_cast<byte> ((c - 'a') + 10);
        }
        if ('A' <= c and c <= 'F') [[likely]] {
            return static_cast<byte> ((c - 'A') + 10);
        }
        static const BadFormatException kException_{"not a hex digit"sv};
        Execution::Throw (kException_);
    };
    Memory::StackBuffer<byte> buf;
    const char*               e = s.data () + s.size ();
    for (const char* i = s.data (); i < e; ++i) {
        if (isspace (static_cast<unsigned char> (*i))) [[unlikely]] {
            continue;
        }
        byte b = digit (*i);
        ++i;
        if (i == e) [[unlikely]] {
            static const BadFormatException kException_{"hex: a byte's second digit missing"sv};
            Execution::Throw (kException_);
        }
        buf.push_back (byte (uint8_t (b << 4) + uint8_t (digit (*i))));
    }
    return Memory::BLOB{buf.begin (), buf.end ()};
}

Memory::BLOB Encoding::Hex::Decode (const char* s)
{
    RequireNotNull (s);
    return Decode (string_view{s});
}

Memory::BLOB Encoding::Hex::Decode (string_view s)
{
    return Decode (span<const char>{s});
}

Memory::BLOB Encoding::Hex::Decode (const string& s)
{
    return Decode (span<const char>{s});
}

Memory::BLOB Encoding::Hex::Decode (const Characters::String& s)
{
    if (optional<span<const Characters::ASCII>> ps = s.PeekData<Characters::ASCII> ()) [[likely]] {
        return Decode (*ps);
    }
    if (optional<string> ascii = s.AsASCIIQuietly ()) {
        return Decode (span<const char>{*ascii});
    }
    static const BadFormatException kException_{"not a hex digit"sv};
    Execution::Throw (kException_);
}
