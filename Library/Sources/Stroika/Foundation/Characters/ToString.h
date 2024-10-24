/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_h_
#define _Stroika_Foundation_Characters_ToString_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <chrono>
#include <ios>
#include <optional>
#include <queue>
#include <sstream>
#include <stack>
#include <thread>
#include <tuple>
#include <typeindex>
#include <typeinfo>
#include <utility>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/CountedValue.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Configuration/StdCompat.h"

#if __cpp_lib_stacktrace >= 202011 && !qCompiler_clangNotCompatibleWithLibStdCPPStackTrace_Buggy
#include <stacktrace>
#endif

/**
 *  \file Convert arbitrary objects to String form, for the purpose of debugging (not reversible).
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 *  TODO:
 *      @todo   ToString(tuple) should use variadic templates and support multiple (past 3) args
 */

#if qHasFeature_fmtlib && (FMT_VERSION >= 110000)
namespace Stroika::Foundation::Common {
    struct GUID;
}
namespace Stroika::Foundation::DataExchange {
    class VariantValue;
}
namespace Stroika::Foundation::Memory {
    class BLOB;
}
#endif

namespace Stroika::Foundation::Characters {

    /**
     *  \brief  Return a debug-friendly, display version of the argument: not guaranteed parsable or usable except for debugging
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
     *      o   atomic<T> where T is a ToStringable type
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
     *  \brief Check if legal to call Characters::ToString(T)...
     */
    template <typename T>
    concept IToString = requires (T t) {
        {
            ToString (t)
        } -> convertible_to<Characters::String>;
    };

    /**
     *  \brief same as ToString()/1 - but without the potentially confusing multi-arg overloads (confused some template expansions).
     */
    template <typename T>
    String UnoverloadedToString (const T& t);

    //// @todo read https://stackoverflow.com/questions/59909102/stdformat-of-user-defined-types
    // I THINK I want to SUBCLASS from std::formatter<std::string> - or something close to that...
    /**
     *  define a 'std::formatter' for Stroika (and other types) for which formatter not already predefined - which use
     *  Characters::ToString (T)
     */
    template <IToString T>
    struct ToStringFormatter /* : std::formatter<std::wstring>*/ {
        qStroika_Foundation_Characters_FMT_PREFIX_::formatter<String, wchar_t> fDelegate2_;

        template <class ParseContext>
        constexpr typename ParseContext::iterator parse (ParseContext& ctx)
        {
            return fDelegate2_.parse (ctx);
        }

        template <class FmtContext>
        typename FmtContext::iterator format (T s, FmtContext& ctx) const
        {
            return fDelegate2_.format (UnoverloadedToString (s), ctx);
#if 0

            std::wstringstream out;
            out << UnoverloadedToString (s);
#if __cpp_lib_ranges >= 202207L
            return std::ranges::copy (std::move (out).str (), ctx.out ()).out;
#else
            return Configuration::StdCompat::format_to (ctx.out (), L"{}", String{out.str ()});
#endif
#endif
        }
    };
    template <IToString T>
    struct ToStringFormatterASCII {

        ToStringFormatter<T> fDelegate2_;
        template <class ParseContext>
        constexpr typename ParseContext::iterator parse (ParseContext& ctx)
        {
            // Not clear how to forward...
            auto it = ctx.begin ();
            while (it != ctx.end ()) {
                ++it;
#if 0
                if (it == ctx.end ()) {
                    throw Configuration::StdCompat::format_error{"Invalid format args (missing }) for ToStringFormatterASCII."};
                }
#endif
                if (*it == '}') {
                    return it;
                }
            }
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

}

namespace Stroika::Foundation::Characters::Private_ {

    /**
     *  \brief IStdFormatterPredefinedFor_<T> = what formattable<T,wchar_t> WOULD have returned if it could be evaluated safely without its value being memoized.
     * 
     * \see       \see https://stackoverflow.com/questions/78774217/how-to-extend-stdformatter-without-sometimes-introducing-conflicts-can-concep
     * 
     * \see https://en.cppreference.com/w/cpp/utility/format/formatter    std::formatter predefined for
     *
     *      template <typename T, typename TUnique = decltype ([] () -> void {})>
     *      concept IStdFormatterPredefinedFor_ = requires (T t) {
     *          typename TUnique;
     *          Stroika::Foundation::Configuration::StdCompat::formattable<T, wchar_t>;
     *      };
     */
    template <typename T>
    concept IStdFormatterPredefinedFor_ =
        // clang-format off

        // C++-20
        Configuration::IAnyOf<decay_t<T>, char, wchar_t> or Configuration::IAnyOf<T, char*, const char*, wchar_t*, const wchar_t*> 
        or requires { []<typename TRAITS, typename ALLOCATOR> (type_identity<std::basic_string<char, TRAITS, ALLOCATOR>>) {}(type_identity<T> ());  } 
        or requires { []<typename TRAITS, typename ALLOCATOR> (type_identity<std::basic_string<wchar_t, TRAITS, ALLOCATOR>>) {}(type_identity<T> ());  } 
        or requires { []<typename TRAITS> (type_identity<std::basic_string_view<char, TRAITS>>) {}(type_identity<T> ()); } 
        or requires { []<typename TRAITS> (type_identity<std::basic_string_view<wchar_t, TRAITS>>) {}(type_identity<T> ()); } 
        or requires { []<size_t N> (type_identity<wchar_t[N]>) {}(type_identity<T> ()); } 
        or std::is_arithmetic_v<T> 
        or  Configuration::IAnyOf<decay_t<T>, nullptr_t, void*, const void*>
        // chrono
        or Configuration::IDuration<T> 
        or requires { []<typename DURATION> (type_identity<std::chrono::sys_time<DURATION>>) {}(type_identity<T> ()); } 
#if !defined(_LIBCPP_VERSION) or _LIBCPP_VERSION > 189999
        or requires { []<typename DURATION> (type_identity<std::chrono::utc_time<DURATION>>) {}(type_identity<T> ()); } 
        or requires { []<typename DURATION> (type_identity<std::chrono::tai_time<DURATION>>) {}(type_identity<T> ()); } 
        or requires { []<typename DURATION> (type_identity<std::chrono::gps_time<DURATION>>) {}(type_identity<T> ()); } 
#endif
        or requires { []<typename DURATION> (type_identity<std::chrono::file_time<DURATION>>) {}(type_identity<T> ()); } 
        or requires { []<typename DURATION> (type_identity<std::chrono::local_time<DURATION>>) {}(type_identity<T> ()); } 
        or Configuration::IAnyOf<decay_t<T>, chrono::day, chrono::month, chrono::year, 
            chrono::weekday, chrono::weekday_indexed, chrono::weekday_last,
            chrono::month_day, chrono::month_day_last, chrono::month_weekday, chrono::month_weekday_last, 
            chrono::year_month, chrono::year_month_day, chrono::year_month_day_last, chrono::year_month_weekday,chrono::year_month_weekday_last 
#if (not defined (_GLIBCXX_RELEASE) or _GLIBCXX_RELEASE > 12) and (!defined(_LIBCPP_VERSION) or _LIBCPP_VERSION > 189999)
            , chrono::sys_info, chrono::local_info
#endif
        >
        or requires { []<typename DURATION> (type_identity<chrono::hh_mm_ss<DURATION>>) {}(type_identity<T> ()); } 
#if (not defined (_GLIBCXX_RELEASE) or _GLIBCXX_RELEASE > 12) and (!defined(_LIBCPP_VERSION) or _LIBCPP_VERSION > 189999)
        or requires { []<typename DURATION, typename TimeZonePtr> (type_identity<chrono::zoned_time<DURATION, TimeZonePtr>>) {}(type_identity<T> ()); } 
#endif

        // C++23
        //
        // sadly MSFT doesn't support __cplusplus with right value
        // 202302L is right value (of __cplusplus) to check for C++ 23, but 202101L needed for clang++16 ;-(
        // value with clang++16 was 202101L and cpp2b and libc++ (ubuntu 23.10 and 24.04) flag... and it had at least the pair<> code supported.
        // this stuff needed for clang++-18-debug-libstdc++-c++23
        //
#if __cpp_lib_format_ranges
        or ranges::range<decay_t<T>>
#endif
#if (__cplusplus > 202101L or _LIBCPP_STD_VER >= 23 or (_MSVC_LANG >= 202004 and _MSC_VER >= _MSC_VER_2k22_17Pt11_)) and not (defined (_GLIBCXX_RELEASE) and _GLIBCXX_RELEASE <= 14)
        or Configuration::IPair<remove_cvref_t<T>>  or Configuration::ITuple<remove_cvref_t<T>>
#endif
#if __cplusplus > 202101L or _LIBCPP_STD_VER >= 23 or _MSVC_LANG >= 202004
        or Configuration::IAnyOf<remove_cvref_t<T>, thread::id>
#endif
#if __cpp_lib_stacktrace >= 202011 && !qCompiler_clangNotCompatibleWithLibStdCPPStackTrace_Buggy
        or Configuration::IAnyOf<remove_cvref_t<T>, stacktrace_entry>
        or requires { []<typename ALLOCATOR> (type_identity<basic_stacktrace<ALLOCATOR>>) {}(type_identity<T> ()); } 
#endif
#if __cplusplus > 202101L or _LIBCPP_STD_VER >= 23 or _MSVC_LANG >= 202004
        or requires { []<typename TT> (type_identity<stack<TT>>) {}(type_identity<T> ()); } 
        or requires { []<typename TT> (type_identity<queue<TT>>) {}(type_identity<T> ()); } 
#endif

        // C++26
#if __cplusplus > 202400L or _LIBCPP_STD_VER >= 26 or _MSVC_LANG >= 202400L
        // unsure what to check - __cpp_lib_format - test c++26  __cpp_lib_formatters < 202601L  -- 202302L  is c++23
        or Configuration::IAnyOf<remove_cvref_t<T>, std::filesystem::path>
#endif

        // AND throw in other libraries Stroika is built with (this is why the question in https://stackoverflow.com/questions/78774217/how-to-extend-stdformatter-without-sometimes-introducing-conflicts-can-concep
        // is so important to better resolve!
#if qHasFeature_fmtlib 
        or Configuration::IAnyOf<decay_t<T>, qStroika_Foundation_Characters_FMT_PREFIX_::string_view, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view>
#if (FMT_VERSION >= 110000)
        // Workaround issue with fmtlib 11 ranges support - it matches these
        or Configuration::IAnyOf<decay_t<T>,Common::GUID, Memory::BLOB>
        // FMT_VERSION has pair/tuple support after FMT version 11
         or Configuration::IPair<remove_cvref_t<T>> or Configuration::ITuple<remove_cvref_t<T>>
#endif
#endif
        ;
    // clang-format on

// Debug hack to spot-check IStdFormatterPredefinedFor_
#if _MSVC_LANG == 202004 && (_MSC_VER < _MSC_VER_2k22_17Pt11_)
    static_assert (not IStdFormatterPredefinedFor_<std::pair<int, char>>);
    static_assert (not IStdFormatterPredefinedFor_<std::tuple<int>>);
    static_assert (IStdFormatterPredefinedFor_<std::thread::id>);
    static_assert (not IStdFormatterPredefinedFor_<std::type_index>);
    static_assert (not IStdFormatterPredefinedFor_<std::exception_ptr>);
#endif
#if _MSVC_LANG == 202004 && (_MSC_VER == _MSC_VER_2k22_17Pt11_)
    static_assert (IStdFormatterPredefinedFor_<std::pair<int, char>>);
    static_assert (IStdFormatterPredefinedFor_<std::tuple<int>>);
    static_assert (IStdFormatterPredefinedFor_<std::thread::id>);
    static_assert (not IStdFormatterPredefinedFor_<std::type_index>);
    static_assert (not IStdFormatterPredefinedFor_<std::exception_ptr>);
#endif
#if __cplusplus == 202002L && _GLIBCXX_RELEASE == 13
    static_assert (not IStdFormatterPredefinedFor_<std::pair<int, char>>);
    static_assert (not IStdFormatterPredefinedFor_<std::tuple<int>>);
    static_assert (not IStdFormatterPredefinedFor_<std::thread::id>);
    static_assert (not IStdFormatterPredefinedFor_<std::type_index>);
    static_assert (not IStdFormatterPredefinedFor_<std::exception_ptr>);
#endif
#if __cplusplus == 202002L && _GLIBCXX_RELEASE == 14
    static_assert (not IStdFormatterPredefinedFor_<std::pair<int, char>>);
    static_assert (not IStdFormatterPredefinedFor_<std::tuple<int>>);
    static_assert (not IStdFormatterPredefinedFor_<std::thread::id>);
    static_assert (not IStdFormatterPredefinedFor_<std::type_index>);
    static_assert (not IStdFormatterPredefinedFor_<std::exception_ptr>);
#endif
#if __cplusplus == 202302L && _GLIBCXX_RELEASE == 14
    static_assert (not IStdFormatterPredefinedFor_<std::pair<int, char>>);
    static_assert (not IStdFormatterPredefinedFor_<std::tuple<int>>);
    static_assert (IStdFormatterPredefinedFor_<std::thread::id>);
    static_assert (not IStdFormatterPredefinedFor_<std::type_index>);
    static_assert (not IStdFormatterPredefinedFor_<std::exception_ptr>);
#endif
#if defined(__APPLE__) && __clang_major__ == 15
    static_assert (not IStdFormatterPredefinedFor_<std::pair<int, char>>);
    static_assert (not IStdFormatterPredefinedFor_<std::tuple<int>>);
    static_assert (not IStdFormatterPredefinedFor_<std::thread::id>);
    static_assert (not IStdFormatterPredefinedFor_<std::type_index>);
    static_assert (not IStdFormatterPredefinedFor_<std::exception_ptr>);
#endif

// Debugging hacks to make sure IStdFormatterPredefinedFor_ defined properly
//
//      CRAZY - but cannot check (at least on visual studio) here: checking NOW causes
//      this to FAIL later (i guess compiler caches results
//      cuz thinks its constant). if this worked, I'd add more static_asserts to check...
//
// Just use briefly - to debug IStdFormatterPredefinedFor_ -  to verify we fail AFTER this point;
// enable #if below and just look if these static_asserts fail - ignore any issues which come after (which is why this cannot be left #if 1)
#if 0
    static_assert (IStdFormatterPredefinedFor_<std::type_index> == Configuration::StdCompat::formattable<std::type_index, wchar_t>);
    static_assert (IStdFormatterPredefinedFor_<std::pair<int, char>> == Configuration::StdCompat::formattable<std::pair<int, char>, wchar_t>);
    static_assert (IStdFormatterPredefinedFor_<std::tuple<int>> == Configuration::StdCompat::formattable<std::tuple<int>, wchar_t>);
    static_assert (IStdFormatterPredefinedFor_<std::thread::id> == Configuration::StdCompat::formattable<std::thread::id, wchar_t>);
    static_assert (IStdFormatterPredefinedFor_<std::filesystem::path> == Configuration::StdCompat::formattable<std::filesystem::path, wchar_t>);
    static_assert (IStdFormatterPredefinedFor_<std::exception_ptr> == Configuration::StdCompat::formattable<std::exception_ptr, wchar_t>);
#endif

    /*
     *  \brief roughly !formattable<T> and IToString<T> ; but cannot do this cuz then formattable<T> would change meaning. So really mean 'formattable so far'
     * 
     *  \see https://en.cppreference.com/w/cpp/utility/format/formatter
     *  \see https://stackoverflow.com/questions/78774217/how-to-extend-stdformatter-without-sometimes-introducing-conflicts-can-concep
     */
    template <typename T>
    concept IUseToStringFormatterForFormatter_ =

        // If Characters::ToString() would work
        IToString<T>

#if !qCompiler_IUseToStringFormatterForFormatter_Buggy
        // But NOT anything std c++ defined to already support (else we get ambiguity error)
        and not IStdFormatterPredefinedFor_<T>
#else
        and (requires (T t) {
                {
                    t.ToString ()
                } -> convertible_to<Characters::String>;
            } or Common::IKeyValuePair<remove_cvref_t<T>> or Common::ICountedValue<remove_cvref_t<T>>
#if !__cpp_lib_format_ranges
#if !qHasFeature_fmtlib or (FMT_VERSION < 110000)
             or (ranges::range<decay_t<T>> and
                 not Configuration::IAnyOf<decay_t<T>, string, wstring, string_view, wstring_view, const char[], const wchar_t[],
                                           qStroika_Foundation_Characters_FMT_PREFIX_::string_view, qStroika_Foundation_Characters_FMT_PREFIX_::wstring_view>)
#endif
#endif
#if _MSC_VER || __cplusplus < 202101L /*202302L 202100L 202300L*/ || (__clang__ != 0 && __GLIBCXX__ != 0 && __GLIBCXX__ <= 20240412) ||    \
    (!defined(__clang__) && __cplusplus == 202302L && __GLIBCXX__ <= 20240412) and (!defined(_LIBCPP_STD_VER) || _LIBCPP_STD_VER < 23)
#if !qHasFeature_fmtlib or (FMT_VERSION < 110000)
             // available in C++23
             or Configuration::IPair<remove_cvref_t<T>> or
             Configuration::ITuple<remove_cvref_t<T>>
#endif
#endif
#if (!defined(__cpp_lib_formatters) || __cpp_lib_formatters < 202302L) and (!defined(_LIBCPP_STD_VER) || _LIBCPP_STD_VER < 23)
             // available in C++23
             or Configuration::IAnyOf<remove_cvref_t<T>, thread::id>
#endif
#if __cplusplus < 202400L
             or Configuration::IAnyOf<remove_cvref_t<T>, std::filesystem::path>
#endif
             or is_enum_v<remove_cvref_t<T>> or Configuration::IOptional<remove_cvref_t<T>> or Configuration::IVariant<remove_cvref_t<T>> or
             same_as<T, std::chrono::time_point<chrono::steady_clock, chrono::duration<double>>> or
             Configuration::IAnyOf<remove_cvref_t<T>, exception_ptr, type_index> or derived_from<T, exception>);
#endif /*qCompiler_IUseToStringFormatterForFormatter_Buggy*/
        ;

}

/**
 *  add ToStringFormatter to the std::formatter object - so all std::format (and Stroika Format, and _f etc) format calls will
 *  apply ToString() as appropriate.
 * 
 *  And a few static_asserts to verify this is working as expected.
 * 
 *  This should allow all the formattable features of up to C++26 - to work on compilers with older settings (such as C++20, or c++23)
 *  where possible (like cannot format stacktrace if it its impl doesn't exist, but can always format filesystem::path - just using Stroika
 *  formatter instead of std-c++ formatter).
 */
template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, wchar_t> : Stroika::Foundation::Characters::ToStringFormatter<T> {};
template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, char> : Stroika::Foundation::Characters::ToStringFormatterASCII<T> {};

/*
 *  If any of these static_asserts trigger, it means you are using a newer compiler I don't have 
 *  propper IUseToStringFormatterForFormatter_ or IStdFormatterPredefinedFor_ settings for. Adjust those settings above so these tests pass.
 */
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::exception_ptr, wchar_t>);
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::filesystem::path, wchar_t>);
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::optional<int>, wchar_t>);
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::pair<int, char>, wchar_t>);
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::thread::id, wchar_t>);
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::type_index, wchar_t>); // note not type_info (result of typeid) - because formattable requires copyable, and type_info not copyable
#if !qCompilerAndStdLib_FormatThreadId_Buggy
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::thread::id, wchar_t>);
#endif
#if qCompilerAndStdLib_formattable_of_tuple_Buggy
static_assert (Stroika::Foundation::Configuration::ITuple<std::remove_cvref_t<std::tuple<int>>>);
#else
static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<std::tuple<int>, wchar_t>);
#endif
// true, but don't #include just for this
//static_assert (Stroika::Foundation::Configuration::StdCompat::formattable<Time::TimePointInSeconds, wchar_t>);
//static_assert (Stroika::Foundation::Configuration<Stroika::Foundation::IO::Network::URI, wchar_t>);

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ToString.inl"

#endif /*_Stroika_Foundation_Characters_ToString_h_*/
