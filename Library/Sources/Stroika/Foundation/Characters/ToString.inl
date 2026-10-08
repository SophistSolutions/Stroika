/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <chrono>
#include <exception>
#include <filesystem>
#include <functional>
#include <ranges>
#include <thread>
#include <typeindex>
#include <typeinfo>
#include <variant>
#include <wchar.h>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Math/Statistics.h"

namespace Stroika::Foundation::Time {
    class Duration;
}
namespace Stroika::Foundation::Characters {

    namespace Private_ {

        /**
         *  \brief checks if the given type has a .ToString () const method returning a string
         */
        template <typename T>
        concept has_ToStringMethod_v = requires (T t) {
            { t.ToString () } -> convertible_to<Characters::String>;
        };
        /**
         *  \brief this given type appears to be a 'pair' of some sort
         */
        template <typename T>
        concept has_pair_v = requires (T t) {
            t.first;
            t.second;
        };
        /**
         *  \brief this given type appears to be a 'KeyValuePair' of some sort
         */
        template <typename T>
        concept has_KeyValuePair_v = requires (T t) {
            t.fKey;
            t.fValue;
        };
        /**
         *  \brief this given type appears to be a 'CountedValue' of some sort
         */
        template <typename T>
        concept has_CountedValue_v = requires (T t) {
            t.fValue;
            t.fCount;
        };

        template <typename T>
        inline String num2Str_ (T t, ios_base::fmtflags flags)
        {
            static_assert (sizeof (t) <= sizeof (int));
            wchar_t buf[1024];
            switch (flags) {
                case ios_base::oct:
                    (void)::swprintf (buf, std::size (buf), L"%o", t);
                    break;
                case ios_base::dec:
                    (void)::swprintf (buf, std::size (buf), L"%d", t);
                    break;
                case ios_base::hex:
                    (void)::swprintf (buf, std::size (buf), L"0x%x", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }
        template <typename T>
        inline String num2Strl_ (T t, ios_base::fmtflags flags)
        {
            wchar_t buf[1024];
            static_assert (sizeof (t) == sizeof (long int));
            switch (flags) {
                case ios_base::oct:
                    (void)::swprintf (buf, std::size (buf), L"%lo", t);
                    break;
                case ios_base::dec:
                    (void)::swprintf (buf, std::size (buf), L"%ld", t);
                    break;
                case ios_base::hex:
                    (void)::swprintf (buf, std::size (buf), L"0x%lx", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }
        template <typename T>
        inline String num2Strll_ (T t, ios_base::fmtflags flags)
        {
            wchar_t buf[1024];
            static_assert (sizeof (t) == sizeof (long long int));
            switch (flags) {
                case ios_base::oct:
                    (void)::swprintf (buf, std::size (buf), L"%llo", t);
                    break;
                case ios_base::dec:
                    (void)::swprintf (buf, std::size (buf), L"%lld", t);
                    break;
                case ios_base::hex:
                    (void)::swprintf (buf, std::size (buf), L"0x%llx", t);
                    break;
                default:
                    AssertNotReached (); // @todo support octal
            }
            return buf;
        }

    }

    /**
     *  Collect all the default ToString() implementations, templates, overloads etc, all in one namespace.
     * 
     *  Users of the Stroika library may specialize Characters::ToString(), but (???) probably should not add overloads ot the
     *  ToStringDefaults namespace --LGP 2023-11-21.
     */
    namespace ToStringDefaults {

        // IN CPP FILE
        String ToString (const exception_ptr& t);
        String ToString (const exception& t);
        String ToString (const type_info& t);
        String ToString (const type_index& t);
        String ToString (const thread::id& t);

        template <Private_::has_ToStringMethod_v T>
        inline String ToString (const T& t)
        {
            return t.ToString ();
        }
        template <ranges::range T>
        String ToString (const T& t)
            requires (not Private_::has_ToStringMethod_v<T> and not is_convertible_v<T, String>)
        {
            StringBuilder sb;
            sb << "["sv;
            bool didFirst{false};
            for (const auto& i : t) {
                if (didFirst) {
                    sb << ", "_k;
                }
                else {
                    sb << " "_k;
                }
                sb << Characters::ToString (i);
                didFirst = true;
            }
            if (didFirst) {
                sb << " "_k;
            }
            sb << "]"_k;
            return sb;
        }
#if 0
        // Until Stroika 3.0d6x we did this - but new format specification stuff generally better ":.100" and I think leading <> etc for which to keep? Maybe not - no way to add elipsis, maybe I can add that?
        // @todo consider - but do differntly than this - so remove and then add somethign in spirit of new format code later
        template <typename T>
        inline String ToString (const T& t, StringShorteningPreference shortenPref = StringShorteningPreference::eDEFAULT, size_t maxLen2Display = 100)
            requires (is_convertible_v<T, String>)
        {
            return "'"sv + static_cast<String> (t).LimitLength (maxLen2Display, shortenPref) + "'"sv;
        }
#endif
        template <typename T>
        inline String ToString (const T& t)
            requires (is_convertible_v<T, String>)
        {
            return static_cast<String> (t);
        }
        template <typename T>
        inline String ToString ([[maybe_unused]] const T& t)
            requires (is_convertible_v<T, tuple<>>)
        {
            return "{}"sv;
        }
        template <IToString T1>
        inline String ToString (const atomic<T1>& t)
        {
            return Characters::ToString (t.load ());
        }
        template <typename T1>
        String ToString (const tuple<T1>& t)
        {
            StringBuilder sb;
            sb << "{"sv << Characters::ToString (get<0> (t)) << "}"sv;
            return sb;
        }
        template <typename T1, typename T2>
        String ToString (const tuple<T1, T2>& t)
        {
            StringBuilder sb;
            sb << "{"sv << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << "}"sv;
            return sb;
        }
        template <typename T1, typename T2, typename T3>
        String ToString (const tuple<T1, T2, T3>& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << ", "sv << Characters::ToString (get<2> (t));
            sb << "}"sv;
            return sb;
        }
        template <typename T1, typename T2, typename T3, typename T4>
        String ToString (const tuple<T1, T2, T3, T4>& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (get<0> (t)) << ", "sv << Characters::ToString (get<1> (t)) << ", "sv
               << Characters::ToString (get<2> (t)) << ", "sv << Characters::ToString (get<3> (t));
            sb << "}"sv;
            return sb;
        }
        template <typename... TYPES>
        String ToString (const variant<TYPES...>& v)
        {
            // Characters::ToString, qualified: an unqualified ToString here would see only the overloads declared above this
            // one - so an alternative whose overload comes later (the integers) would convert to bool, and print "true"
            return visit ([] (const auto& a) { return Characters::ToString (a); }, v);
        }
        template <typename T>
        String ToString (const T& t)
            requires (Private_::has_pair_v<T>)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (t.first) << ": "sv << Characters::ToString (t.second);
            sb << "}"sv;
            return sb;
        }
        template <Private_::has_KeyValuePair_v T>
        String ToString (const T& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << Characters::ToString (t.fKey) << ": "sv << Characters::ToString (t.fValue);
            sb << "}"sv;
            return sb;
        }
        template <Private_::has_CountedValue_v T>
        String ToString (const T& t)
        {
            StringBuilder sb;
            sb << "{"sv;
            sb << "'" << Characters::ToString (t.fValue) << "': "sv << Characters::ToString (t.fCount);
            sb << "}"sv;
            return sb;
        }
        template <typename T>
        inline String ToString (const T& t)
            requires (is_enum_v<T>)
        {
            if constexpr (not Common::IBoundedEnum<T>) {
                return Characters::ToString (underlying_type_t<T> (t));
            }
            else if constexpr (Common::DefaultNames<T>{}.size () == 0) {
                // emit as number if no EnumNames<> declared
                return Characters::ToString (underlying_type_t<T> (t));
            }
            else {
                return Common::DefaultNames<T>{}.GetName (t);
            }
        }
        template <typename OPTIONS>
        inline String ToString (const StringBuilder<OPTIONS>& sb)
        {
            return sb.template As<String> ();
        }
        template <floating_point T>
        inline String ToString (T t)
        {
            return FloatConversion::ToString (t);
        }
        template <floating_point T>
        inline String ToString (T t, FloatConversion::ToStringOptions o)
        {
            return FloatConversion::ToString (t, o);
        }
        template <typename T>
        inline String ToString (const Math::CommonStatistics<T>& t)
        {
            Characters::StringBuilder sb;

            sb << "{"sv;
            if (t.fMin) {
                sb << "min: " << *t.fMin;
            }
            if (t.fMax) {
                sb << "max: " << *t.fMax;
            }
            if (t.fMean) {
                sb << "mean: " << *t.fMean;
            }
            if (t.fMedian) {
                sb << "median: " << *t.fMedian;
            }
            if (t.fStandardDeviation) {
                sb << "standard-deviation: " << *t.fStandardDeviation;
            };
            sb << "}"sv;
            return sb;
        }
        template <typename T>
        inline String ToString (const shared_ptr<T>& pt)
        {
            if (pt == nullptr) {
                return "nullptr"sv;
            }
            if constexpr (IToString<decltype (*pt)>) {
                return String{Common::StdCompat::format (L"*{}", Characters::ToString (*pt))};
            }
            return String{Common::StdCompat::format (L"{}", static_cast<const void*> (pt.get ()))};
        }
        template <typename T>
        inline String ToString (const unique_ptr<T>& pt)
        {
            if (pt == nullptr) {
                return "nullptr"sv;
            }
            if constexpr (IToString<decltype (*pt)>) {
                return String{Common::StdCompat::format (L"*{}", Characters::ToString (*pt))};
            }
            return String{Common::StdCompat::format (L"{}", static_cast<const void*> (pt.get ()))};
        }
        // A raw pointer prints its address, as "{}"_f prints a void* (0x...) - never what it points to, which may dangle. Not a
        // character pointer (a C string: the is_convertible_v<T, String> overload), nor an array (taken by reference, an array
        // does not decay, so T is not a pointer)
        template <typename T>
        inline String ToString (const T& t)
            requires (is_pointer_v<T> and not is_function_v<remove_pointer_t<T>> and not is_convertible_v<T, String>)
        {
            return String{Common::StdCompat::format (L"{}", static_cast<const void*> (t))};
        }
        template <typename T>
        inline String ToString (const optional<T>& o)
        {
            return o.has_value () ? Characters::ToString (*o) : "[missing]"sv;
        }
        template <typename FUNCTION_SIGNATURE>
        inline String ToString (const function<FUNCTION_SIGNATURE>& f)
        {
            return Common::StdCompat::format (L"{}", static_cast<const void*> (f.template target<remove_cvref_t<FUNCTION_SIGNATURE>> ()));
        }
        inline String ToString (const chrono::duration<double>& t)
        {
            return Characters::ToString (t.count ()) + " seconds"sv;
        }
        String ToString (const Time::Duration& t, FloatConversion::SignificantFigures p);
        template <typename CLOCK_T>
        inline String ToString (const chrono::time_point<CLOCK_T, chrono::duration<double>>& t)
        {
            return Characters::ToString (t.time_since_epoch ().count ()) + " seconds"sv;
        }
        template <integral T>
        inline String ToString (T t, ios_base::fmtflags flags)
        {
            using namespace Private_;
            if constexpr (sizeof (T) < sizeof (long)) {
                return num2Str_ (t, flags);
            }
            else if constexpr (sizeof (T) == sizeof (long)) {
                return num2Strl_ (t, flags);
            }
            else if constexpr (sizeof (T) == sizeof (long long int)) {
                return num2Strll_ (t, flags);
            }
        }
        // A template, so nothing converts to bool to get here: an int, a pointer or a class with an operator bool must
        // find its own overload, or not compile - never print as true/false
        template <same_as<bool> T>
        inline String ToString (T t)
        {
            // static: copying a String only counts a reference; making one from "true"sv allocates a rep, each call
            static const String kTrue_{"true"sv};
            static const String kFalse_{"false"sv};
            return t ? kTrue_ : kFalse_;
        }
        template <signed_integral T>
        inline String ToString (T t)
        {
            return Characters::ToString (t, ios_base::dec);
        }
        template <unsigned_integral T>
        inline String ToString (T t)
            requires (not same_as<T, bool>) // bool is an unsigned_integral
        {
            // no overwhelmingly good reason todo it this way, but this matches what we had in Stroika 2.1, and its reasonable...
            if constexpr (sizeof (T) == 1) {
                return Characters::ToString (t, ios_base::hex);
            }
            else {
                return Characters::ToString (t, ios_base::dec);
            }
        }
        inline String ToString (byte t)
        {
            return Characters::ToString (static_cast<unsigned char> (t), ios_base::hex);
        }
#if 0
        inline String ToString (const filesystem::path& t, StringShorteningPreference shortenPref = StringShorteningPreference::ePreferKeepRight,
                                size_t maxLen2Display = 100)
        {
            return Characters::ToString (t.wstring (), shortenPref, maxLen2Display); // wrap in 'ToString' for surrounding quotes
        }
#endif
        inline String ToString (const filesystem::path& t)
        {
            return t.wstring ();
        }

    }

    /*
     ********************************************************************************
     *************************** Characters::ToString *******************************
     ********************************************************************************
     */
    template <typename T, typename... ARGS>
    inline String ToString (T&& t, ARGS... args)
    {
        return ToStringDefaults::ToString (forward<T> (t), forward<ARGS> (args)...);
    }

    /*
     ********************************************************************************
     *********************** Characters::UnoverloadedToString ***********************
     ********************************************************************************
     */
    template <typename T>
    inline String UnoverloadedToString (const T& t)
    {
        return ToString (t);
    }

    //DEPRECATED
    namespace Private_ {
        template <typename T>
        using has_ToString_t = decltype (static_cast<Characters::String> (declval<T&> ().ToString ()));
    }
    /*
     *  \brief Return true iff Characters::ToString (T) is well defined.
     */
    template <typename T>
    [[deprecated ("Since Stroika v3.0d5 use IToString")]] constexpr inline bool has_ToString_v =
        Common::is_detected_v<Private_::has_ToString_t, T>;

}

namespace Stroika::Foundation::Traversal {
    template <typename T>
    inline Characters::String Iterable<T>::Join (const Characters::String& separator) const
    {
        return Join (separator, nullopt);
    }
    template <typename T>
    inline Characters::String Iterable<T>::Join (const Characters::String& separator, const optional<Characters::String>& finalSeparator) const
    {
        using namespace Characters;
#if qCompilerAndStdLib_kDefaultToStringConverter_Buggy
        function<String (T)> cvt;
        if constexpr (same_as<T, String>) {
            cvt = Common::Identity{};
        }
        else {
            cvt = UnoverloadedToString<T>;
        }
        return this->Join (cvt, StringCombiner<String>{.fSeparator = separator, .fSpecialSeparatorForLastPair = finalSeparator});
#else
        return this->Join (kDefaultToStringConverter<String>,
                           StringCombiner<String>{.fSeparator = separator, .fSpecialSeparatorForLastPair = finalSeparator});
#endif
    }
    template <typename T>
    template <typename RESULT_T, invocable<T> CONVERT_TO_RESULT>
    inline RESULT_T Iterable<T>::Join (const CONVERT_TO_RESULT& convertToResult, const RESULT_T& separator, const optional<RESULT_T>& finalSeparator) const
        requires (convertible_to<invoke_result_t<CONVERT_TO_RESULT, T>, RESULT_T>)
    {
        return this->Join<RESULT_T> (
            convertToResult, Characters::StringCombiner<RESULT_T>{.fSeparator = separator, .fSpecialSeparatorForLastPair = finalSeparator});
    }

}
