/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <charconv>
#include <cmath>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Cryptography/Encoding/Algorithm/Base64.h"

#include "DataTypes.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::UPnP;
using namespace Stroika::Frameworks::UPnP::DataTypes;

namespace {
    // an integer as UPnP writes it - decimal, a leading + and zeros allowed - whole, and in T's range
    template <integral T>
    optional<T> ParseInteger_ (const String& text)
    {
        string s = text.AsUTF8<string> ();
        if (s.starts_with ('+')) {
            s.erase (0, 1);
        }
        T result{};
        auto [end, ec] = from_chars (s.data (), s.data () + s.size (), result);
        if (ec != errc{} or end != s.data () + s.size () or s.empty ()) {
            return nullopt;
        }
        return result;
    }
}

/*
 ********************************************************************************
 ************************** UPnP::DataTypes::ToText *****************************
 ********************************************************************************
 */
template <IValue T>
String DataTypes::ToText (const T& v)
{
    if constexpr (same_as<T, bool>) {
        return v ? "1"sv : "0"sv;
    }
    else if constexpr (unsigned_integral<T>) {
        return "{}"_f(static_cast<uint64_t> (v));
    }
    else if constexpr (signed_integral<T>) {
        return "{}"_f(static_cast<int64_t> (v));
    }
    else if constexpr (floating_point<T>) {
        Require (std::isfinite (v)); // UPnP has no NaN or infinity
        // the fewest digits that read back exactly; and E before an exponent, as UPnP writes it
        return "{}"_f(v).ReplaceAll ("e"sv, "E"sv);
    }
    else if constexpr (same_as<T, Character>) {
        return String{v};
    }
    else if constexpr (same_as<T, String>) {
        return v;
    }
    else if constexpr (same_as<T, Time::Date>) {
        return v.Format (String{Time::Date::kISO8601Format});
    }
    else if constexpr (same_as<T, Time::DateTime>) {
        return v.Format (Time::DateTime::kISO8601Format);
    }
    else if constexpr (same_as<T, Time::TimeOfDay>) {
        return v.Format (String{Time::TimeOfDay::kISO8601Format});
    }
    else if constexpr (same_as<T, Memory::BLOB>) {
        using namespace Cryptography::Encoding::Algorithm;
        return String{Base64::Encode (v, Base64::Options{.fLineBreak = Base64::LineBreak::eNone_LB})};
    }
    else if constexpr (same_as<T, BinHex>) {
        return v.fBytes.AsHex ();
    }
    else if constexpr (same_as<T, IO::Network::URI>) {
        return v.template As<String> ();
    }
    else if constexpr (same_as<T, Common::GUID>) {
        return v.template As<String> ();
    }
}

/*
 ********************************************************************************
 ************************* UPnP::DataTypes::FromText ****************************
 ********************************************************************************
 */
template <IValue T>
optional<T> DataTypes::FromText (const String& text)
{
    if constexpr (same_as<T, String>) {
        return text;
    }
    else if constexpr (same_as<T, Character>) {
        return text.size () == 1 ? optional<T>{text[0]} : nullopt;
    }
    else {
        const String t = text.Trim ();
        if constexpr (same_as<T, bool>) {
            // 1 or 0 - and true or false, yes or no, which UPnP says are not to be sent but are to be read
            const String v = t.ToLowerCase ();
            if (v == "1"sv or v == "true"sv or v == "yes"sv) {
                return true;
            }
            if (v == "0"sv or v == "false"sv or v == "no"sv) {
                return false;
            }
            return nullopt;
        }
        else if constexpr (integral<T>) {
            return ParseInteger_<T> (t);
        }
        else if constexpr (floating_point<T>) {
            T v = FloatConversion::ToFloat<T> (t); // NaN if it is not a number
            return std::isfinite (v) ? optional<T>{v} : nullopt;
        }
        else if constexpr (same_as<T, Time::Date>) {
            return Time::Date::ParseQuietly (t, String{Time::Date::kISO8601Format});
        }
        else if constexpr (same_as<T, Time::DateTime>) {
            return Time::DateTime::ParseQuietly (t, Time::DateTime::kISO8601Format);
        }
        else if constexpr (same_as<T, Time::TimeOfDay>) {
            return Time::TimeOfDay::ParseQuietly (t, String{Time::TimeOfDay::kISO8601Format});
        }
        else {
            // the rest throw on what they cannot read: none of them blocks, so none is interrupted - any exception is a bad value
            try {
                if constexpr (same_as<T, Memory::BLOB>) {
                    return Cryptography::Encoding::Algorithm::Base64::Decode (t);
                }
                else if constexpr (same_as<T, BinHex>) {
                    return BinHex{Memory::BLOB::FromHex (t)};
                }
                else if constexpr (same_as<T, IO::Network::URI>) {
                    return IO::Network::URI::Parse (t);
                }
                else if constexpr (same_as<T, Common::GUID>) {
                    return Common::GUID{t};
                }
            }
            catch (...) {
                return nullopt;
            }
        }
    }
}

// each of IValue's types
#define InstantiateDataType_(T)                                                                                                            \
    template String      DataTypes::ToText<T> (const T&);                                                                                  \
    template optional<T> DataTypes::FromText<T> (const String&);
InstantiateDataType_ (bool);
InstantiateDataType_ (uint8_t);
InstantiateDataType_ (uint16_t);
InstantiateDataType_ (uint32_t);
InstantiateDataType_ (uint64_t);
InstantiateDataType_ (int8_t);
InstantiateDataType_ (int16_t);
InstantiateDataType_ (int32_t);
InstantiateDataType_ (int64_t);
InstantiateDataType_ (float);
InstantiateDataType_ (double);
InstantiateDataType_ (Character);
InstantiateDataType_ (String);
InstantiateDataType_ (Time::Date);
InstantiateDataType_ (Time::DateTime);
InstantiateDataType_ (Time::TimeOfDay);
InstantiateDataType_ (Memory::BLOB);
InstantiateDataType_ (BinHex);
InstantiateDataType_ (IO::Network::URI);
InstantiateDataType_ (Common::GUID);
#undef InstantiateDataType_
