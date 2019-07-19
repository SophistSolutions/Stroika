/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_CheckedConverter_h_
#define _Stroika_Foundation_DataExchange_CheckedConverter_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/String.h"

#include "BadFormatException.h"

/**
 *  \file
 *
 * TODO:
 *      @todo   UNDOCUMENTED - VERY PRELIMINARY DRAFT
 *
 *
 *  \em Design Note:
 */

namespace Stroika::Foundation::DataExchange {

    /**
     *  Throws BadFormatException if data bad. This is handy for 'file' reading, as most of the
     *  Stroika type constructors assert valid data (e.g. String::FromUTF8 ()).
     *
     *  \par Example Usage
     *      \code
     *          string tmpMaybeGoodUTF8FromFile;
     *          String s = CheckedConverter<String, UTF8> (tmpMaybeGoodUTF8FromFile);   // throws if invalid UTF8
     *      \endcode
     */
    template <typename TO, typename EXTRA_DATA, typename FROM>
    [[deprecated ("Use String::AsASCII , or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] TO CheckedConverter (FROM from, const EXTRA_DATA& extraData = EXTRA_DATA{});

    struct [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] UTF8{};
    struct [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] ASCII{};

    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    template <>
    [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] Characters::String CheckedConverter<Characters::String, UTF8, string> (string from, const UTF8& extraData);
    template <>
    [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] Characters::String CheckedConverter<Characters::String, UTF8, const char*> (const char* from, const UTF8& extraData);
    template <>
    [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] Characters::String CheckedConverter<Characters::String, ASCII, string> (string from, const ASCII& extraData);
    template <>
    [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] Characters::String CheckedConverter<Characters::String, ASCII, Characters::String> (Characters::String from, const ASCII& extraData);
    template <>
    [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] Characters::String CheckedConverter<Characters::String, ASCII, const char*> (const char* from, const ASCII& extraData);

    template <>
    [[deprecated ("Use String::AsASCII, or String::FromAscii, or String::FromUTF8 since Stroika v2.1d23")]] string CheckedConverter<string, ASCII, Characters::String> (Characters::String from, const ASCII& extraData);
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
    DISABLE_COMPILER_MSC_WARNING_END (4996);

    /**
     *  CheckedConverter_Range takes a lower and upper bound, and creates a new RANGE_TYPE
     *  object using that lower/upper bound, after validating (similar to @CheckedConverter_ValueInRange,
     *  but not exactly the same because of 'openness')
     *
     *  CheckedConverter_Range also checks that the endpoints are valid with respect to the type, and that s <= e.
     *
     *  CheckedConverter_Range () if any values are invald.
     *
     *  @see CheckedConverter_ValueInRange
     */
    template <typename RANGE_TYPE>
    RANGE_TYPE CheckedConverter_Range (const typename RANGE_TYPE::value_type& s, const typename RANGE_TYPE::value_type& e);

    /**
     *  CheckedConverter_ValueInRange () will throw BadFormatException () if the argument 'val' is outside
     *  the given argument range.
     *
     *  The 'range' defaults to the RANGE_TYPE::FullRange ().
     *
     *  Note - for floating point ElementType - if the value ('val') is 'really close' to an edge - it
     *  will be pinned to the range edge, not treated as out of range.
     *
     *  The returned value is always at least nearly identical to the 'val' passed in, but could be slightly different due to
     *  the above floating point near-edge adjustment.
     */
    template <typename RANGE_TYPE>
    typename RANGE_TYPE::ElementType CheckedConverter_ValueInRange (typename RANGE_TYPE::ElementType val, const RANGE_TYPE& range = RANGE_TYPE::FullRange ());

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CheckedConverter.inl"

#endif /*_Stroika_Foundation_DataExchange_CheckedConverter_h_*/
