/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include <cctype>
#include <charconv>
#include <chrono>
#include <cmath>
#include <exception>
#include <filesystem>
#include <functional>
#include <ranges>
#include <sstream>
#include <thread>
#include <typeindex>
#include <typeinfo>
#include <variant>
#include <wchar.h>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/Enumeration.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Math/Common.h"
#include "Stroika/Foundation/Math/Statistics.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

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

        // t as printf's %d, %o or 0x%x writes it - a negative value in octal or hexadecimal as its two's complement
        template <integral T>
        inline String IntegerToString_ (T t, ios_base::fmtflags flags)
        {
            if constexpr (same_as<T, bool>) {
                return IntegerToString_ (static_cast<int> (t), flags);
            }
            else {
                char  buf[numeric_limits<T>::digits + 4]; // enough for octal, a sign or the 0x
                char* p = std::begin (buf);
                switch (flags) {
                    case ios_base::dec:
                        return String{span<const char>{std::begin (buf), to_chars (p, std::end (buf), t).ptr}};
                    case ios_base::oct:
                        return String{
                            span<const char>{std::begin (buf), to_chars (p, std::end (buf), static_cast<make_unsigned_t<T>> (t), 8).ptr}};
                    case ios_base::hex:
                        *p++ = '0';
                        *p++ = 'x';
                        return String{
                            span<const char>{std::begin (buf), to_chars (p, std::end (buf), static_cast<make_unsigned_t<T>> (t), 16).ptr}};
                    default:
                        AssertNotReached ();
                        return String{};
                }
            }
        }

    }

    /**
     *  Collect all the default ToString() implementations, templates, overloads etc, all in one namespace.
     * 
     *  Users of the Stroika library may specialize Characters::ToString(), but (???) probably should not add overloads ot the
     *  ToStringDefaults namespace --LGP 2023-11-21.
     */
    /*
     *  Writing a floating point number - ToString (f, Number::FloatToStringOptions). Before Stroika v3.0d26 this was FloatConversion::ToString.
     *
     *  @todo   REWRITE much of this, once 3.0 alpha or beta starts: the API is OK, but the implementation weak (and probably slow).
     *  @todo   Then maybe lose the ios_base::fmtflags option (perhaps keep it as an argument, but just grab those fields) - say an
     *          option for "FIXEDWIDTH".
     */
    namespace Private_ {
        inline void TrimTrailingZeros_NotSci_ (String* strResult)
        {
            // @todo THIS could be more efficient: use StringBuilder and Peek()/Character::AsASCIIQuietly
            RequireNotNull (strResult);
            // strip trailing zeros (after decimal point - before they may indicate magnatude)
            // And don't do if ends with exponential notation e+40 shouldnt get shortned to e+4!
            Require (not strResult->Find ('e', eCaseInsensitive).has_value ());
            size_t pastDot = strResult->find ('.');
            if (pastDot != String::npos) {
                ++pastDot;
                size_t len           = strResult->length ();
                size_t pPastLastZero = len;
                for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                    if ((*strResult)[pPastLastZero - 1] != '0') {
                        break;
                    }
                }
                if (pPastLastZero == pastDot + 1 and (*strResult)[pPastLastZero - 1] == '0') {
                    pPastLastZero -= 2; // dont map 3.000 to 3.0, map it to 3
                }
                if (len != pPastLastZero) [[unlikely]] { // check common case of no change, but this substring and assign already pretty optimized (not sure helps performance)
                    *strResult = strResult->SubString (0, pPastLastZero);
                }
            }
        }
        inline void TrimTrailingZeros_MaybeSci_ (String* strResult)
        {
            // @todo THIS could be more efficient: use StringBuilder and Peek()/Character::AsASCIIQuietly
            RequireNotNull (strResult);
            // strip trailing zeros (after decimal point - before they may indicate magnatude)
            // And don't do if ends with exponential notation e+40 shouldnt get shortned to e+4!
            if (optional<size_t> whereIsE = strResult->Find ('e', eCaseInsensitive)) {
                size_t pastDot = strResult->find ('.');
                if (pastDot != String::npos) {
                    ++pastDot;
                    size_t len           = *whereIsE;
                    size_t pPastLastZero = len;
                    for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                        if ((*strResult)[pPastLastZero - 1] != '0') {
                            break;
                        }
                    }
                    if (len != pPastLastZero) [[unlikely]] { // check common case of no change, but this substring and assign already pretty optimized (not sure helps performance)
                        *strResult = strResult->SubString (0, pPastLastZero) + strResult->SubString (*whereIsE);
                    }
                }
            }
            else {
                size_t pastDot = strResult->find ('.');
                if (pastDot != String::npos) {
                    ++pastDot;
                    size_t len           = strResult->length ();
                    size_t pPastLastZero = len;
                    for (; (pPastLastZero - 1) > pastDot; --pPastLastZero) {
                        if ((*strResult)[pPastLastZero - 1] != '0') {
                            break;
                        }
                    }
                    if (pPastLastZero == pastDot + 1 and (*strResult)[pPastLastZero - 1] == '0') {
                        pPastLastZero -= 2; // dont map 3.000 to 3.0, map it to 3
                    }
                    if (len != pPastLastZero) [[unlikely]] { // check common case of no change, but this substring and assign already pretty optimized (not sure helps performance)
                        *strResult = strResult->SubString (0, pPastLastZero);
                    }
                }
            }
        }
    }

    namespace Private_ {
        template <floating_point FLOAT_TYPE>
        String FloatToString_OptimizedForCLocaleAndNoStreamFlags_ (FLOAT_TYPE f, Number::SignificantFigures precision)
        {
            using namespace Memory;
            size_t sz = numeric_limits<FLOAT_TYPE>::max_digits10 + numeric_limits<FLOAT_TYPE>::max_exponent10 + 5; // source? "-1.##e+##\0"
            StackBuffer<char> buf{Memory::eUninitialized, sz};
            ptrdiff_t         resultStrLen;
            unsigned int      effectivePrecision = precision.GetEffectiveSignificantFigures<FLOAT_TYPE> ();

            // XCode 15 still doesn't define __cpp_lib_to_chars, as well as _LIBCPP_VERSION < 190000, I believe --LGP 2024-07-13
#if defined(__cpp_lib_to_chars)
            // empirically, on MSVC, to_chars() is much faster than snprintf (appears 3x apx faster) -- LGP 2021-11-04
            if (precision == Number::SignificantFigures::kFullPrecision) {
                resultStrLen = to_chars (buf.begin (), buf.end (), f, chars_format::general).ptr - buf.begin ();
            }
            else {
                resultStrLen = to_chars (buf.begin (), buf.end (), f, chars_format::general, effectivePrecision).ptr - buf.begin ();
            }
#else
            // to_chars with a precision is defined as printf's %.*g. kFullPrecision's max_digits10 reads back as the same number,
            // but is not always the shortest text that does, as to_chars without a precision writes
            if constexpr (same_as<FLOAT_TYPE, long double>) {
                resultStrLen = ::snprintf (buf.data (), buf.size (), "%.*Lg", static_cast<int> (effectivePrecision), f);
            }
            else {
                resultStrLen = ::snprintf (buf.data (), buf.size (), "%.*g", static_cast<int> (effectivePrecision), static_cast<double> (f));
            }
#endif

            Verify (resultStrLen > 0 and resultStrLen < static_cast<int> (sz));
#if qStroika_Foundation_Debug_AssertionsChecked
            Assert (precision == Number::SignificantFigures::kFullPrecision or
                    Number::SignificantFigures::Calculate (span<const char>{buf.data (), static_cast<size_t> (resultStrLen)}) <= effectivePrecision);
#endif
            return String{span{buf.data (), static_cast<size_t> (resultStrLen)}};
        }
    }

    namespace Private_ {
        // format "{:.{}f}" - val in fixed notation, with precision digits after the decimal point
        template <floating_point T>
        inline string formatFixed_ (T val, unsigned int precision)
        {
#if qCompilerAndStdLib_format_dynamic_spec_Buggy
            return Common::StdCompat::vformat ("{:.{}f}", Common::StdCompat::make_format_args (val, precision));
#else
            return Common::StdCompat::format ("{:.{}f}", val, precision);
#endif
        }
        template <floating_point T>
        inline String formatNonScientific_ (T val, unsigned int nSignificantFigures)
        {
            auto compute = [&] () {
                if (val == 0.0) {
                    // special case for zero cuz cannot compute log10(0)
                    return String{formatFixed_ (0.0, nSignificantFigures)};
                }
                else {
                    // Calculate digits before the decimal point
                    int digits_before = static_cast<int> (floor (log10 (abs (val)))) + 1;

                    // Precision for 'f' (fixed) is the number of digits AFTER the decimal
                    unsigned int precision = static_cast<unsigned int> (max (0, static_cast<int> (nSignificantFigures) - digits_before));

                    return String{formatFixed_ (val, precision)};
                }
            };
            String r = compute ();
            // @todo RECONSIDER
            // for example, there is no way for 31200000 to be represented with 6 significant digits (in standard form, you can with scientific)
            // 212312345.0 with 6 significant digits is 212312000, but we need the extra digits for magnatitude, so not
            // sure worth losing the action digits we have? CONSIDER!!!!
            // but for now no ensures --Ensure (Number::SignificantFigures::Calculate (span<const wchar_t>{r.As<wstring> ()}) <= nSignificantFigures);
            return r;
        }
    }

    namespace Private_ {
        template <floating_point FLOAT_TYPE>
        String FloatToString_GeneralCase_ (FLOAT_TYPE f, const Number::FloatToStringOptions& options)
        {
            unsigned int usePrecision =
                options.GetSignificantFigures ().value_or (Number::SignificantFigures{}).GetEffectiveSignificantFigures<FLOAT_TYPE> ();
            Number::FloatFormatType usingFormat = options.GetFloatFormat ().value_or (Number::FloatFormatType::eDEFAULT);

            if (usingFormat == Number::FloatFormatType::eStandard) {
                Assert (options.GetIOSFmtFlags () == nullopt); // doable in principle, looking/mapping each feature, but not trivial, and probably not needed anytime soon
                                                               // --LGP 2026-02-10
                return formatNonScientific_ (f, usePrecision);
            }

            // expensive to construct, and slightly cheaper to just use thread_local version of
            // the same stringstream each time (only one per thread can be in use)
            static thread_local stringstream s;
            static const ios_base::fmtflags  kDefaultIOSFmtFlags_ = s.flags (); // Just copy out of the first constructed stringstream

            s.str (string{});
            s.clear ();

            s.imbue (locale::classic ()); // the "C" locale's text: FloatToString_ punctuates it

            //  must set explicitly (even if defaulted)  because of the thread_local thing
            s.flags (options.GetIOSFmtFlags ().value_or (kDefaultIOSFmtFlags_));

            {
                switch (usingFormat) {
                    case Number::FloatFormatType::eScientific:
                    case Number::FloatFormatType::eScientificWithWhitespaceTrimmed:
                        s.setf (ios_base::scientific, ios_base::floatfield);
                        Assert (usePrecision >= 2);
                        s.precision (usePrecision - 1); // scientific format is n.mmm with m digits of 'precision' - so 1 + m is # of significat digits
                        break;
                    case Number::FloatFormatType::eDefaultFloat:
                        s.unsetf (ios_base::floatfield); // see std::defaultfloat - not same as ios_base::fixed
                        /*
                         * Precision Field: The stream's precision setting (managed by std::setprecision or std::ios_base::precision()) 
                         * specifies the maximum number of meaningful digits to display in total (both before and after the decimal point),
                         * not a fixed number of digits after the decimal point (so same as Number::SignificantFigures)
                         */
                        s.precision (usePrecision);
                        break;
                    case Number::FloatFormatType::eFixedPoint:
                    case Number::FloatFormatType::eFixedPointWithWhitespaceTrimmed:
                        s.setf (ios_base::fixed, ios_base::floatfield);
                        s.precision (usePrecision); // I think wrong but test -
                        break;
                    case Number::FloatFormatType::eStandard:
                        AssertNotReached (); // handled above
                        break;
                        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
                        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
                        DISABLE_COMPILER_MSC_WARNING_START (4996)
                    case Number::FloatFormatType::eAutomaticScientific: {
                        s.precision (usePrecision);
                        bool useScientificNotation = abs (f) >= pow (10, usePrecision / 2) or
                                                     (f != 0 and abs (f) < pow (10, -static_cast<int> (usePrecision) / 2)); // scientific preserves more precision - but non-scientific looks better
                        if (useScientificNotation) {
                            s.setf (ios_base::scientific, ios_base::floatfield);
                        }
                        else {
                            s.unsetf (ios_base::floatfield); // see std::defaultfloat - not same as ios_base::fixed
                            s.precision (usePrecision);
                        }
                    } break;
                        DISABLE_COMPILER_MSC_WARNING_END (4996)
                        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
                        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
                    default:
                        RequireNotReached ();
                        break;
                }
            }

            s << f;
            string ss = s.str ();

            return String{ss};
        }
    }

    namespace Private_ {
        // digits, with p's digit separator between their groups (p.fGroupSize digits at the right, p.fSecondaryGroupSize - or
        // fGroupSize - in each further left) - or as they are, where p gives no group size
        inline void AppendGrouped_ (StringBuilder<>& sb, string_view digits, const Number::Punctuation& p)
        {
            size_t n = digits.size ();
            if (not p.fDigitSeparator or p.fGroupSize == 0 or n <= p.fGroupSize) {
                sb << digits;
                return;
            }
            size_t secondary = p.fSecondaryGroupSize != 0 ? p.fSecondaryGroupSize : p.fGroupSize;
            size_t left      = n - p.fGroupSize; // the digits left of the rightmost group
            size_t first     = left % secondary == 0 ? secondary : left % secondary;
            sb << digits.substr (0, first);
            for (size_t i = first; i < left; i += secondary) {
                sb << *p.fDigitSeparator << digits.substr (i, secondary);
            }
            sb << *p.fDigitSeparator << digits.substr (left);
        }
        // text - a number in the "C" locale's format (ASCII: a sign, digits, '.', an exponent; or nan or inf) - punctuated as p says:
        // its whole part's digits grouped, and its '.' made p.fDecimalPoint
        inline String Punctuate_ (const String& text, const Number::Punctuation& p)
        {
            if (p == Number::Punctuation{}) {
                return text;
            }
            string t = text.AsASCII ();
            size_t b = not t.empty () and (t[0] == '-' or t[0] == '+') ? 1 : 0;
            size_t e = b;
            while (e < t.size () and isdigit (static_cast<unsigned char> (t[e]))) {
                ++e;
            }
            StringBuilder<> sb;
            sb << string_view{t}.substr (0, b);
            AppendGrouped_ (sb, string_view{t}.substr (b, e - b), p);
            for (size_t i = e; i < t.size (); ++i) {
                if (t[i] == '.') {
                    sb << p.fDecimalPoint;
                }
                else {
                    sb << t[i];
                }
            }
            return sb.str ();
        }
        template <floating_point FLOAT_TYPE>
        String FloatToString_ (FLOAT_TYPE f, const Number::FloatToStringOptions& options)
        {
            auto floatFormat = options.GetFloatFormat ().value_or (Number::FloatFormatType::eDEFAULT);
            // written as the "C" locale writes it, and punctuated as the options say at the end
            auto result = (not options.GetIOSFmtFlags () and floatFormat == Number::FloatFormatType::eDefaultFloat)
                              ? Private_::FloatToString_OptimizedForCLocaleAndNoStreamFlags_ (
                                    f, options.GetSignificantFigures ().value_or (Number::SignificantFigures{}))
                              : Private_::FloatToString_GeneralCase_ (f, options);
            if (floatFormat == Number::FloatFormatType::eFixedPointWithWhitespaceTrimmed or floatFormat == Number::FloatFormatType::eStandard) {
                TrimTrailingZeros_NotSci_ (&result);
            }
            if (floatFormat == Number::FloatFormatType::eScientificWithWhitespaceTrimmed) {
                TrimTrailingZeros_MaybeSci_ (&result);
            }
            return Punctuate_ (result, options.GetPunctuation ());
        }
    }

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
            return Private_::FloatToString_ (t, Number::FloatToStringOptions{});
        }
        template <floating_point T>
        inline String ToString (T t, const Number::FloatToStringOptions& o)
        {
            return Private_::FloatToString_ (t, o);
        }
        template <floating_point T>
        inline String ToString (T t, Number::SignificantFigures sf)
        {
            return Private_::FloatToString_ (t, Number::FloatToStringOptions{sf});
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
        String ToString (const Time::Duration& t, Number::SignificantFigures p);
        template <typename CLOCK_T>
        inline String ToString (const chrono::time_point<CLOCK_T, chrono::duration<double>>& t)
        {
            return Characters::ToString (t.time_since_epoch ().count ()) + " seconds"sv;
        }
        template <integral T>
        inline String ToString (T t, ios_base::fmtflags flags)
        {
            return Private_::IntegerToString_ (t, flags);
        }
        template <integral T>
        String ToString (T t, const Number::IntegerToStringOptions& o)
            requires (not same_as<T, bool>)
        {
            Require (2 <= o.fRadix and o.fRadix <= 36);
            char digits[numeric_limits<T>::digits + 2]; // enough for base 2, and a sign
            auto [ptr, ec] = to_chars (std::begin (digits), std::end (digits), t, static_cast<int> (o.fRadix));
            Assert (ec == errc{});
            const char* d = digits;
            string      r;
            if (*d == '-') {
                r += *d++; // the prefix comes after the sign, as Number::Parse reads it
            }
            if (o.fEmitRadixPrefix and (o.fRadix == 16 or o.fRadix == 8 or o.fRadix == 2)) {
                r += '0';
                r += o.fRadix == 16 ? 'x' : (o.fRadix == 8 ? 'o' : 'b');
            }
            r.append (d, static_cast<size_t> (ptr - d));
            if (o.fUpperCase) {
                for (char& c : r) {
                    c = static_cast<char> (toupper (static_cast<unsigned char> (c)));
                }
            }
            Number::Punctuation p = Number::ResolvePunctuation (o.fPunctuation);
            if (not p.fDigitSeparator or p.fGroupSize == 0) {
                return String{r};
            }
            size_t          head = r.size () - static_cast<size_t> (ptr - d); // the sign and prefix, before the digits
            StringBuilder<> sb;
            sb << string_view{r}.substr (0, head);
            Private_::AppendGrouped_ (sb, string_view{r}.substr (head), p);
            return sb.str ();
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
        inline String ToString (const filesystem::path& t)
        {
            return t.wstring ();
        }

    }

    namespace Private_ {
        // IToString's test - here, after the last ToStringDefaults overload: a qualified name in a template is looked up where
        // the template is defined, so any earlier would miss the overloads declared after it
        template <typename T>
        struct HasToStringDefault_ {
            static constexpr bool value = requires (T t) { ToStringDefaults::ToString (t); };
        };
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
