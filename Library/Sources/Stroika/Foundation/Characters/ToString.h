/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_h_
#define _Stroika_Foundation_Characters_ToString_h_ 1

#include "../StroikaPreComp.h"

#include <ios>

#include "../Configuration/Concepts.h"

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

}

#if __cpp_lib_format >= 202207L && 0
// SUPER PRIMITIVE ROUGH FIRST DRAFT
// maybe wrong concept filter- maybe need to use specificatlly has ToString() method!!! Others count on stdlib handling?
// test stuff like vector<IO::Network::InternetAddress>
template <Stroika::Foundation::Characters::IToString T>
struct std::formatter<T, wchar_t> {
    bool quoted = false;

    template <class ParseContext>
    constexpr ParseContext::iterator parse (ParseContext& ctx)
    {
        auto it = ctx.begin ();
        if (it == ctx.end ())
            return it;

        if (*it == '#') {
            quoted = true;
            ++it;
        }
        if (*it != '}')
            throw std::format_error ("Invalid format args for QuotableString.");

        return it;
    }

    template <class FmtContext>
    FmtContext::iterator format (T s, FmtContext& ctx) const
    {
        using namespace Stroika::Foundation::Characters;
        std::wstringstream out;
        out << UnoverloadedToString (s);
        return std::ranges::copy (std::move (out).str (), ctx.out ()).out;
    }
};
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ToString.inl"

#endif /*_Stroika_Foundation_Characters_ToString_h_*/
