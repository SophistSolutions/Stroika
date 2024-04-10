/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_h_
#define _Stroika_Foundation_Characters_ToString_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ios>
#include <optional>
#include <thread>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include "Format.h"
#include "Stroika/Foundation/Common/CountedValue.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Configuration/Concepts.h"

#include "String.h"

/**
 *  TODO:
 *      @todo   ToString(tuple) should use variadic templates and support multiple (past 3) args
 */

namespace Stroika::Foundation::Characters {

    /**
     *  \brief  Return a debug-friendly, display version of the argument: not guaranteed parseable or usable except for debugging
     *
     *  Convert an instance of the given object to a printable string representation. This representation
     *  is not guaranteed, pretty, or parsable. This feature is generally for debugging purposes, but can be used
     *  to render/emit objects in any informal setting where you just need a rough sense of the object (again,
     *  the only case I can think of here would be for debugging).
     *
     *  \note *AKA*
     *      o   DUMP
     *      o   PrettyPrint
     *
     *  Patterned after:
     *      Java:
     *          From the Object.toString() docs:
     *          Returns a string representation of the object. In general, the toString method
     *          returns a string that "textually represents" this object.
     *      Javascript:
     *          The toString() method returns a string representing object.
     *          Every object has a toString() method that is automatically called when the object is
     *          to be represented as a text value or when an object is referred to in a manner in which
     *          a string is expected.
     *
     *  \note   Built-in or std types intrinsically supported:
     *      o   type_index, type_traits, or anything with a .name () which returns a const SDKChar* string.
     *      o   is_array<T>
     *      o   is_enum<T>
     *      o   std::exception
     *      o   std::tuple
     *      o   std::variant
     *      o   std::pair
     *      o   std::optional
     *      o   std::wstring
     *      o   std::filesystem::path
     *      o   exception_ptr
     *      o   POD types (int, bool, double, etc)
     *      o   anything with .begin (), .end () - so any container/iterable
     *      o   anything class(or struct) with a ToString () method
     *
     *  \note   Other types automatically supported
     *      o   KeyValuePair
     *      o   CountedValue
     *      o   String -- printed as 'the-string' (possibly length limited)
     *
     *  \note *Implementation Note*
     *      This implementation defaults to calling T{}.ToString ().
     * 
     *  \note @see IToString to check if Characters::ToString () well defined.
     */
    template <typename T, typename... ARGS>
    String ToString (T&& t, ARGS... args);

    /**
     * \brief flags may be std::dec, std::oct, or std::hex
     * 
     *  Defaults to decimal for all types except uint8_t (where it defaults to hex).
     *  \note Prior to 2.1b6, all ToString<unsigned integer types> were all hex.
     * 
     *   @see https://en.cppreference.com/w/cpp/io/ios_base/fmtflags
     */
    template <integral T>
    String ToString (T t, ios_base::fmtflags flags);

    /**
     *  Check if legal to call Characters::ToString(T)...
     */
    template <typename T>
    concept IToString = requires (T t) {
        {
            ToString (t)
        } -> convertible_to<Characters::String>;
    };

    /**
     */
    template <typename T>
    String UnoverloadedToString (const T& t);

    template <Stroika::Foundation::Characters::IToString T>
    struct ToStringFormatter {
        bool quoted = false;

        template <class ParseContext>
        constexpr typename ParseContext::iterator parse (ParseContext& ctx)
        {
            auto it = ctx.begin ();
            if (it == ctx.end ())
                return it;

            if (*it == '#') {
                quoted = true;
                ++it;
            }
            if (*it != '}')
                throw Configuration::StdCompat::format_error{"Invalid format args for QuotableString."};

            return it;
        }

        template <class FmtContext>
        typename FmtContext::iterator format (T s, FmtContext& ctx) const
        {
            using namespace Stroika::Foundation::Characters;
            std::wstringstream out;
            out << UnoverloadedToString (s);
#if __cpp_lib_ranges >= 202207L
            return std::ranges::copy (std::move (out).str (), ctx.out ()).out;
#else
            return Configuration::StdCompat::format_to (ctx.out (), L"{}", String{out.str ()});
#endif
        }
    };
    template <Stroika::Foundation::Characters::IToString T>
    struct ToStringFormatterASCII {

        ToStringFormatter<T> fDelegate2_;
        template <class ParseContext>
        constexpr typename ParseContext::iterator parse (ParseContext& ctx)
        {
            auto it = ctx.begin ();
            if (it == ctx.end ())
                return it;

            if (*it == '#') {
                //  quoted = true;
                ++it;
            }
            if (*it != '}')
                throw Configuration::StdCompat::format_error{"Invalid format args for QuotableString."};

            return it;
        }

        template <class FmtContext>
        typename FmtContext::iterator format (T s, FmtContext& ctx) const
        {
            using namespace Stroika::Foundation::Characters;
            std::wstringstream out;
            out << UnoverloadedToString (s);

            // @todo delegate to string version so we can use its ignore errors code......
#if __cpp_lib_ranges >= 202207L
            return std::ranges::copy (String{out.str ()}.AsNarrowSDKString (eIgnoreErrors), ctx.out ()).out;
#else
            return Configuration::StdCompat::format_to (ctx.out (), "{}", String{out.str ()}.AsNarrowSDKString (eIgnoreErrors));
#endif
        }
    };

    #ifndef qCOMPILER_BUG_TIMEPOINT_FLOAT_BUGGY
    #if defined (__clang__)
    #define qCOMPILER_BUG_TIMEPOINT_FLOAT_BUGGY 1
    #else
    #define qCOMPILER_BUG_TIMEPOINT_FLOAT_BUGGY 0
    #endif
    #endif

    namespace Private_ {

        /*
         *  \brief roughly !formattable<T> and IToString<T> ; but cannot do this cuz then formattable<T> would change meaning. So really mean 'formattable so far'
         * 
         *  \see https://en.cppreference.com/w/cpp/utility/format/formatter
         *
         *  Idea is to TRY to capture all the cases we support to Characters::ToString() - except those already done
         *  by std c++ lib (and String which we special case). If I overlap at all, we get very confusing messages from compiler
         *  about duplicate / overlapping formatter definitions.
         * 
         *  \todo Would be nice to support type_info/typeid, but for tricky fact that type_info(const type_info&)=deleted, makes it largely not
         *        work. Probably missing some perfect forwarding someplace? And with that fixed maybe can work??? --LGP 2024-04-10
         */
        template <typename T>
        concept IUseToStringFormatterForFormatter_ =
            // most user-defined types captured by this rule - just add a ToString() method!
            requires (T t) {
                {
                    t.ToString ()
                } -> convertible_to<Characters::String>;
            }

            // Stroika types not captured by std-c++ rules
            or Common::IKeyValuePair<remove_cvref_t<T>> or Common::ICountedValue<remove_cvref_t<T>>

        // c++ 23 features which may not be present with current compilers
        // value with clang++16 was 202101L and cpp2b and libc++ (ubuntu 23.10 and 24.04) flag... and it had at least the pair<> code supported.
        // this stuff needed for clang++-18-debug-libstdc++-c++23
#if !__cpp_lib_format_ranges
            or (ranges::range<decay_t<T>> and
                not Configuration::IAnyOf<decay_t<T>, string, wstring, string_view, wstring_view, const char[], const wchar_t[],
                                          qStroika_Foundation_Characters_FMT_PREFIX_::string_view, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view>)
#endif
        // sadly MSFT doesnt support all, and doesnt support __cplusplus with right value
#if _MSC_VER
            // available in C++23
            or Configuration::IPair<remove_cvref_t<T>> or Configuration::ITuple<remove_cvref_t<T>>
#elif __cplusplus < 202100L /*202300L*/ || (__clang__ != 0 && __GLIBCXX__ != 0 && __GLIBCXX__ <= 20240315)
            // available in C++23
            or Configuration::IPair<remove_cvref_t<T>> or
            Configuration::ITuple<remove_cvref_t<T>>
            // available in C++23
            or Configuration::IAnyOf<remove_cvref_t<T>, thread::id>
#if 0
            // ranges available in C++23, but range<T> matches some stuff that IS already pre-included by std-c++ formatters
            or (ranges::range<decay_t<T>> and
                not Configuration::IAnyOf<decay_t<T>, string, wstring, string_view, wstring_view, const char[], const wchar_t[],
                                          qStroika_Foundation_Characters_FMT_PREFIX_::string_view, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view>)
#endif
#endif

        // features added in C++26
        // unsure what to check - __cpp_lib_format - test c++26  __cpp_lib_formatters < 202601L  -- 202302L  is c++23
#if __cplusplus < 202400L
            or Configuration::IAnyOf<remove_cvref_t<T>, std::filesystem::path>
#endif

            // Features from std-c++ that probably should have been added
            // NOTE - we WANT to support type_info (so typeid works directly) - but run into trouble because type_info(const type_info)=delete, so not
            // sure how to make that work with formatters (besides wrapping in type_index).
            or is_enum_v<remove_cvref_t<T>> or Configuration::IOptional<remove_cvref_t<T>> or Configuration::IVariant<remove_cvref_t<T>>
        
            // want this but causes trouble with CLANG!!!
            #if !qCOMPILER_BUG_TIMEPOINT_FLOAT_BUGGY
             or Configuration::ITimePoint<T> 
             #endif
            or Configuration::IAnyOf<remove_cvref_t<T>, exception_ptr, exception, type_index>;
    }

}

template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, wchar_t> : Stroika::Foundation::Characters::ToStringFormatter<T> {};
template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, char> : Stroika::Foundation::Characters::ToStringFormatterASCII<T> {};

#if __cplusplus > 202200L || _HAS_CXX23
// various examples of things Stroika provides / assure support for regardless of C++ version
static_assert (std::formattable<std::type_index, wchar_t>);
static_assert (std::formattable<std::thread::id, wchar_t>);
static_assert (std::formattable<std::exception_ptr, wchar_t>);
static_assert (std::formattable<std::type_index, wchar_t>);
static_assert (std::formattable<std::filesystem::path, wchar_t>);
static_assert (std::formattable<std::optional<int>, wchar_t>);
static_assert (std::formattable<std::pair<int, char>, wchar_t>);
static_assert (std::formattable<std::tuple<int>, wchar_t>);
//static_assert (std::formattable<Stroika::Foundation::IO::Network::URI, wchar_t>); // true, but dont #include just for this
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ToString.inl"

#endif /*_Stroika_Foundation_Characters_ToString_h_*/
