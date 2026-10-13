/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
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
#if defined(__cpp_lib_format_ranges)
#include <format> // std::format_kind, which Stroika specializes for its own ranges
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Concepts.h"
#include "Stroika/Foundation/Common/CountedValue.h"
#include "Stroika/Foundation/Common/KeyValuePair.h"
#include "Stroika/Foundation/Common/StdCompat.h"

#if defined(__cpp_lib_stacktrace)
#include <stacktrace>
#endif

/**
 *  \file
 *       Convert arbitrary objects to String form, for the purpose of debugging (not reversible).
 * 
 *  \note Code-Status:  <a href="Code-Status.md#Alpha">Alpha</a>
 * 
 *  TODO:
 *      @todo   ToString(tuple) should use variadic templates and support multiple (past 4) args
 */

#if qStroika_HasComponent_fmtlib && (FMT_VERSION >= 110000)
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
     *      o   shared_ptr<T> (but not unique_ptr<> because its not regular, which std::format requires)
     *      o   std::exception
     *      o   std::tuple
     *      o   std::variant
     *      o   std::pair
     *      o   std::optional
     *      o   std::wstring
     *      o   std::filesystem::path
     *      o   exception_ptr
     *      o   POD types (int, bool, double, etc)
     *      o   raw pointers - the address, as std::format prints a void* (0x...); but a character pointer is a C string
     *      o   anything with .begin (), .end () - so any container/iterable
     *      o   anything class(or struct) with a ToString () method
     *
     *  \note   Other types automatically supported
     *      o   KeyValuePair
     *      o   CountedValue
     *      o   String -- printed as 'the-string' (possibly length limited)
     * 
     *  \note Extra arguments/overloads
     *      o   ToString (float, FloatConversion::ToStringOptions)      // any floating point argument
     *      o   ToString (float, FloatConversion::SignificantFigures)            // ''
     *      o   ToString (integral T, ios_base::fmtflags flags);        // flags may be std::dec, std::oct, or std::hex
     *                                                                  // @see https://en.cppreference.com/w/cpp/io/ios_base/fmtflags
     *      o   ToString (Duration, FloatConversion::SignificantFigures)         // forwards to Duration::As<String> (SignificantFigures)
     *
     *  \note *Implementation Note*
     *      This implementation defaults to calling T{}.ToString ().
     * 
     *  \note @see IToString to check if Characters::ToString () well defined.
     */
    template <typename T, typename... ARGS>
    String ToString (T&& t, ARGS... args);

    namespace Private_ {
        template <typename T>
        struct HasToStringDefault_; // defined in ToString.inl, after every ToStringDefaults overload
    }

    /**
     *  \brief Check if legal to call Characters::ToString(T)...
     *
     *  True just where a ToStringDefaults overload takes a T. Not requires { ToString (t) }: the dispatcher above takes any type
     *  (it is its body that fails), so that was true of everything. And not tested here, where none of the overloads is declared yet.
     */
    template <typename T>
    concept IToString = Private_::HasToStringDefault_<T>::value;

    /**
     *  \brief same as ToString()/1 - but without the potentially confusing multi-arg overloads (confused some template expansions).
     */
    template <typename T>
    String UnoverloadedToString (const T& t);

    /**
     *  define a 'std::formatter' for Stroika (and other types) for which formatter not already predefined - which use
     *  Characters::ToString (T)
     */
    template <IToString T>
    struct ToStringFormatter {
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
        }
    };
    template <IToString T>
    struct ToStringFormatterASCII {
        qStroika_Foundation_Characters_FMT_PREFIX_::formatter<String, char> fDelegate2_;

        template <class ParseContext>
        constexpr typename ParseContext::iterator parse (ParseContext& ctx)
        {
            return fDelegate2_.parse (ctx);
        }

        template <class FmtContext>
        typename FmtContext::iterator format (T s, FmtContext& ctx) const
        {
            return fDelegate2_.format (UnoverloadedToString (s), ctx);
        }
    };

}

namespace Stroika::Foundation::Traversal {
    template <typename T>
    class Iterable;
}

namespace Stroika::Foundation::Characters::Private_ {

    template <typename ELT>
    void IsIterable_ (const Traversal::Iterable<ELT>*); // declared only: what it accepts is the question

    /**
     *  \brief one of Stroika's containers, or a class derived from one: anything derived from a Traversal::Iterable
     *
     *  \note Not with a lambda in the requires-expression: clang 15 to 18 then take the formatter partial specialization below as
     *        matching every type - "ambiguous partial specializations of 'formatter<int, wchar_t>'".
     */
    template <typename T>
    concept IStroikaIterable_ = requires (const T* t) { IsIterable_ (t); };

    /**
     *  \brief a type Stroika registers its ToString-based std::formatter for: Stroika's own types, and an application's with a
     *         ToString () method - never a standard library type
     *
     *  The standard allows specializing its templates only for program-defined types ([namespace.std]), and formats its own types
     *  itself. Every other type Characters::ToString can print, Stroika's own formatting (_f, Format, DbgTrace) still formats - as
     *  its ToString (@see Characters::IFormattable).
     */
    template <typename T>
    concept IUseToStringFormatterForFormatter_ = requires (const T& t) {
        { t.ToString () } -> convertible_to<String>;
    } or Common::IKeyValuePair<remove_cvref_t<T>> or Common::ICountedValue<remove_cvref_t<T>> or IStroikaIterable_<T>;

}

/**
 *  ToStringFormatter as the std::formatter (fmtlib's, where Stroika formats with fmtlib) of each IUseToStringFormatterForFormatter_
 *  type - so std::format itself formats them, as their Characters::ToString.
 */
template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, wchar_t> : Stroika::Foundation::Characters::ToStringFormatter<T> {};
template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, char> : Stroika::Foundation::Characters::ToStringFormatterASCII<T> {};

#if defined(__cpp_lib_format_ranges)
namespace std {
    /**
     *  Those that are ranges (Stroika's containers, Memory::BLOB ...) are formatted the same in every standard - as their ToString,
     *  not by the standard's range formatter (C++23), which would match them too.
     */
    template <ranges::input_range R>
        requires (same_as<R, remove_cvref_t<R>> and Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_<R>)
    constexpr range_format format_kind<R> = range_format::disabled;
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ToString.inl"

#endif /*_Stroika_Foundation_Characters_ToString_h_*/
