/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <charconv>
#include <cmath>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
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

    // text read as data type t, whose C++ type is T
    template <IValue T>
    optional<T> Read_ (const String& text, DataType t)
    {
        if constexpr (same_as<T, String>) {
            return text;
        }
        else if constexpr (same_as<T, Character>) {
            return text.size () == 1 ? optional<T>{text[0]} : nullopt;
        }
        else {
            const String s = text.Trim ();
            if constexpr (same_as<T, bool>) {
                // 1 or 0 - and true or false, yes or no, which UPnP says are not to be sent but are to be read
                const String v = s.ToLowerCase ();
                if (v == "1"sv or v == "true"sv or v == "yes"sv) {
                    return true;
                }
                if (v == "0"sv or v == "false"sv or v == "no"sv) {
                    return false;
                }
                return nullopt;
            }
            else if constexpr (integral<T>) {
                return ParseInteger_<T> (s);
            }
            else if constexpr (floating_point<T>) {
                T v = FloatConversion::ToFloat<T> (s); // NaN if it is not a number
                return std::isfinite (v) ? optional<T>{v} : nullopt;
            }
            else if constexpr (same_as<T, Time::Date>) {
                return Time::Date::ParseQuietly (s, String{Time::Date::kISO8601Format});
            }
            else if constexpr (same_as<T, Time::DateTime>) {
                return Time::DateTime::ParseQuietly (s, Time::DateTime::kISO8601Format);
            }
            else if constexpr (same_as<T, Time::TimeOfDay>) {
                String time = s;
                if (t == DataType::eTimeTZ) {
                    // its time zone - Z, or +hh:mm or -hh:mm - after it: dropped, as TimeOfDay has none
                    const string u = s.AsUTF8<string> ();
                    time           = String{u.substr (0, u.find_first_of ("Z+-"))};
                }
                return Time::TimeOfDay::ParseQuietly (time, String{Time::TimeOfDay::kISO8601Format});
            }
            else {
                // the rest throw on what they cannot read: none of them blocks, so none is interrupted - any exception is a bad value
                try {
                    if constexpr (same_as<T, Memory::BLOB>) {
                        return t == DataType::eBinHex ? Memory::BLOB::FromHex (s) : Cryptography::Encoding::Algorithm::Base64::Decode (s);
                    }
                    else if constexpr (same_as<T, IO::Network::URI>) {
                        return IO::Network::URI::Parse (s);
                    }
                    else {
                        static_assert (same_as<T, Common::GUID>);
                        return Common::GUID{s};
                    }
                }
                catch (...) {
                    return nullopt;
                }
            }
        }
    }
}

/*
 ********************************************************************************
 ************************** UPnP::DataTypes::ToText *****************************
 ********************************************************************************
 */
String DataTypes::ToText (const Value& v)
{
    return visit ([&]<typename T> (const T&) { return ToText (v, DefaultDataType<T> ()); }, v);
}

String DataTypes::ToText (const Value& v, DataType t)
{
    return visit (
        [t]<typename T> (const T& x) -> String {
            Require (IsTypeOf<T> (t));
            if constexpr (same_as<T, bool>) {
                return x ? "1"sv : "0"sv;
            }
            else if constexpr (unsigned_integral<T>) {
                return "{}"_f(static_cast<uint64_t> (x));
            }
            else if constexpr (signed_integral<T>) {
                return "{}"_f(static_cast<int64_t> (x));
            }
            else if constexpr (floating_point<T>) {
                Require (std::isfinite (x)); // UPnP has no NaN or infinity
                if (t == DataType::eFixed14_4) {
                    Require (std::abs (x) < 1e14);
                    // 4 digits after the point, then the zeros at the end dropped - and the point, if they were all
                    String s = "{:.4f}"_f(x);
                    while (s.EndsWith ("0"sv)) {
                        s = s.SubString (0, s.size () - 1);
                    }
                    return s.EndsWith ("."sv) ? s.SubString (0, s.size () - 1) : s;
                }
                // the fewest digits that read back exactly; and E before an exponent, as UPnP writes it
                return "{}"_f(x).ReplaceAll ("e"sv, "E"sv);
            }
            else if constexpr (same_as<T, Character>) {
                return String{x};
            }
            else if constexpr (same_as<T, String>) {
                return x;
            }
            else if constexpr (same_as<T, Time::Date>) {
                return x.Format (String{Time::Date::kISO8601Format});
            }
            else if constexpr (same_as<T, Time::DateTime>) {
                if (t == DataType::eDateTime) {
                    // dateTime has no time zone: its date, and time of day, as they are
                    StringBuilder result{x.GetDate ().Format (String{Time::Date::kISO8601Format})};
                    if (optional<Time::TimeOfDay> tod = x.GetTimeOfDay ()) {
                        result << "T"sv << tod->Format (String{Time::TimeOfDay::kISO8601Format});
                    }
                    return result;
                }
                return x.Format (Time::DateTime::kISO8601Format);
            }
            else if constexpr (same_as<T, Time::TimeOfDay>) {
                return x.Format (String{Time::TimeOfDay::kISO8601Format});
            }
            else if constexpr (same_as<T, Memory::BLOB>) {
                if (t == DataType::eBinHex) {
                    return x.AsHex ();
                }
                using namespace Cryptography::Encoding::Algorithm;
                return String{Base64::Encode (x, Base64::Options{.fLineBreak = Base64::LineBreak::eNone_LB})};
            }
            else {
                static_assert (same_as<T, IO::Network::URI> or same_as<T, Common::GUID>);
                return x.template As<String> ();
            }
        },
        v);
}

/*
 ********************************************************************************
 ************************* UPnP::DataTypes::FromText ****************************
 ********************************************************************************
 */
optional<Value> DataTypes::FromText (const String& text, DataType t)
{
    // read as t's C++ type
    auto as = [&]<IValue T> () -> optional<Value> {
        if (optional<T> v = Read_<T> (text, t)) {
            return Value{in_place_type<T>, *v};
        }
        return nullopt;
    };
    switch (t) {
        case DataType::eUI1:
            return as.template operator()<uint8_t> ();
        case DataType::eUI2:
            return as.template operator()<uint16_t> ();
        case DataType::eUI4:
            return as.template operator()<uint32_t> ();
        case DataType::eUI8:
            return as.template operator()<uint64_t> ();
        case DataType::eI1:
            return as.template operator()<int8_t> ();
        case DataType::eI2:
            return as.template operator()<int16_t> ();
        case DataType::eI4:
        case DataType::eInt:
            return as.template operator()<int32_t> ();
        case DataType::eI8:
            return as.template operator()<int64_t> ();
        case DataType::eR4:
            return as.template operator()<float> ();
        case DataType::eR8:
        case DataType::eNumber:
        case DataType::eFixed14_4:
        case DataType::eFloat:
            return as.template operator()<double> ();
        case DataType::eChar:
            return as.template operator()<Character> ();
        case DataType::eString:
            return as.template operator()<String> ();
        case DataType::eDate:
            return as.template operator()<Time::Date> ();
        case DataType::eDateTime:
        case DataType::eDateTimeTZ:
            return as.template operator()<Time::DateTime> ();
        case DataType::eTime:
        case DataType::eTimeTZ:
            return as.template operator()<Time::TimeOfDay> ();
        case DataType::eBoolean:
            return as.template operator()<bool> ();
        case DataType::eBinBase64:
        case DataType::eBinHex:
            return as.template operator()<Memory::BLOB> ();
        case DataType::eURI:
            return as.template operator()<IO::Network::URI> ();
        case DataType::eUUID:
            return as.template operator()<Common::GUID> ();
    }
    AssertNotReached ();
    return nullopt;
}
