/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_ToString_h_
#define _Stroika_Foundation_Characters_ToString_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Concepts.h"

#include "String.h"

/**
 *  TODO:
 *      @todo   https://stroika.atlassian.net/browse/STK-566 ToString (...elipsis) support, and cleanup use of templates in Private_
 *
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
     *      o   std::pair
     *      o   std::optional
     *      o   exception_ptr
     *      o   POD types (int, bool, double, etc)
     *      o   anything with .begin (), .end () - so any contrainer/iterable
     *      o   anything class(or struct) with a ToString () method
     *
     *  \note   Other types automatically supported
     *      o   KeyValuePair
     *      o   CountedValue
     *      o   String
     *
     *  \note *Implementation Note*
     *      This implementation defaults to calling T().ToString ().
     */
    template <typename T>
    String ToString (const T& t);

}

namespace Stroika::Foundation::Configuration {
    /*
     *  has_ToString<T>::value is true iff T has a begin/end method
     *  @todo fix so checks results act more like iterators - subclass from iterator_tag>
     */
    STROIKA_FOUNDATION_CONFIGURATION_DEFINE_HAS (ToString, (Characters::ToString (x)));
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "ToString.inl"

#endif /*_Stroika_Foundation_Characters_ToString_h_*/
