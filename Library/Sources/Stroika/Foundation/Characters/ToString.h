/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_h_
#define _Stroika_Foundation_Characters_ToString_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ios>
#include <optional>
#include <thread>
#include <typeindex>
#include <typeinfo>

#include "Format.h"
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

    namespace Private_ {
#ifndef qCOMPILER_BUG_MAYBE_TEMPLATE_OPTIONAL_CONCEPT_MATCHER
// rbeaks on xcode and clang++-17-debug-libc++.xml
#if defined(__clang__)
#define qCOMPILER_BUG_MAYBE_TEMPLATE_OPTIONAL_CONCEPT_MATCHER 1
#else
#define qCOMPILER_BUG_MAYBE_TEMPLATE_OPTIONAL_CONCEPT_MATCHER 0
#endif
#endif
        /*
         *  https://en.cppreference.com/w/cpp/utility/format/formatter
        // Tricky - cuz different versions of stdc++ include different ones of these... - and we cannot include if stdc++ already does!

        *
         *  Idea is to TRY to capture all the cases we support to Characters::ToString() - except those already done
         *  by std c++ lib (and String which we special case).
         *
         *  roughly - thopugh this doesnt work - its
         *      concept = !formattable<T> and IToString<T>;
         */
        template <typename T>
        concept IUseToStringFormatterForFormatter_ =
            requires (T t) {
                {
                    t.ToString ()
                } -> convertible_to<Characters::String>;
            }
#if !qCOMPILER_BUG_MAYBE_TEMPLATE_OPTIONAL_CONCEPT_MATCHER
            or
            requires (T t) {
                {
                    []<typename X> (optional<X>) {}(t)
                };
            }
#endif
            or Configuration::IAnyOf<remove_cvref_t<T>, exception_ptr, exception, type_info, type_index, thread::id>;

        static_assert (IUseToStringFormatterForFormatter_<exception_ptr> and IUseToStringFormatterForFormatter_<type_info>); // etc
#if !qCOMPILER_BUG_MAYBE_TEMPLATE_OPTIONAL_CONCEPT_MATCHER
        static_assert (IUseToStringFormatterForFormatter_<optional<int>>);
#endif
    }

}

// SUPER PRIMITIVE ROUGH FIRST DRAFT
template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, wchar_t> : Stroika::Foundation::Characters::ToStringFormatter<T> {};
template <Stroika::Foundation::Characters::Private_::IUseToStringFormatterForFormatter_ T>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<T, char> : Stroika::Foundation::Characters::ToStringFormatterASCII<T> {};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ToString.inl"

#endif /*_Stroika_Foundation_Characters_ToString_h_*/
