/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
     *      string tmpMaybeGoodUTF8FromFile;
     *      String s = CheckedConverter<String, UTF8> (tmpMaybeGoodUTF8FromFile);   // throws if invalid UTF8
     *      \endcode
     */
    template <typename TO, typename EXTRA_DATA, typename FROM>
    TO CheckedConverter (FROM from, const EXTRA_DATA& extraData = EXTRA_DATA ());

    struct UTF8 {
    };
    struct ASCII {
    };

    template <>
    Characters::String CheckedConverter<Characters::String, UTF8, const string&> (const string& from, const UTF8& extraData);
    template <>
    Characters::String CheckedConverter<Characters::String, UTF8, string> (string from, const UTF8& extraData);
    template <>
    Characters::String CheckedConverter<Characters::String, UTF8, const char*> (const char* from, const UTF8& extraData);
    template <>
    Characters::String CheckedConverter<Characters::String, UTF8, char*> (char* from, const UTF8& extraData);
    template <>
    Characters::String CheckedConverter<Characters::String, ASCII, const string&> (const string& from, const ASCII& extraData);
    template <>
    Characters::String CheckedConverter<Characters::String, ASCII, string> (string from, const ASCII& extraData);
    template <>
    Characters::String CheckedConverter<Characters::String, ASCII, const char*> (const char* from, const ASCII& extraData);
    template <>
    Characters::String CheckedConverter<Characters::String, ASCII, char*> (char* from, const ASCII& extraData);

    template <>
    string CheckedConverter<string, ASCII, const Characters::String&> (const Characters::String& from, const ASCII& extraData);

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
     *  The returned value is always at least nearly identical to the 'val' passed in, but could be slightly differnt due to
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
