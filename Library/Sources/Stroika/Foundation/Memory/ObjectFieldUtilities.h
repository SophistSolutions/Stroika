/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_ObjectFieldUtilities_h_
#define _Stroika_Foundation_Memory_ObjectFieldUtilities_h_ 1

#include "../StroikaPreComp.h"

#include <cstddef>

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Memory {

    using std::byte;

    /**
     *  \brief convert the given pointer to data member, and convert it to a size_t offset.
     *
     *  This is similar to offsetof(), but with pointer to member objects.
     * 
     *  Since - according to https://en.cppreference.com/w/cpp/types/offsetof - offsetof is
     *  not allowed on non-standard-layout objects, this probably fails there as well (undefined behavior).
     *  Not sure what todo about that.
     */
    template <typename OUTER_OBJECT, typename PROPERTY_TYPE>
    size_t ConvertPointerToDataMemberToOffset (PROPERTY_TYPE (OUTER_OBJECT::*dataMember));

    /**
     *  Given an objects field (given by pointer to member) and that owned object, run 'offsetof' backwards
     *  to get the original owning object.
     * 
     *  \par Example Usage
     *      \code
     *          struct Header {
     *              ReadOnlyProperty<int> p {[] (const auto* property) {
     *                  const Header* h = GetObjectOwningField(property, &Header::p);
     *                  // This should recover the pointer to the object Header from the pointer to its field 'p';
     *              }};
     *      \endcode
     * 
     *  \note - for this to work - OUTER_OBJECT (in the above example: Header) must be of stanard_layout type according
     *        to c++ standard, but I'm not sure what todo otherwise, it it seems to work otherwise.
     * 
     *  @todo void* aggregatedMember sb AGGREGATED_OBJECT_TYPE* aggregatedMember, but caused some trouble - debug...LGP 2021-01-26
     */
    template <typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    OUTER_OBJECT* GetObjectOwningField (void* aggregatedMember, AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember));
    template <typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    const OUTER_OBJECT* GetObjectOwningField (const void* aggregatedMember, AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember));

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectFieldUtilities.inl"

#endif /*_Stroika_Foundation_Memory_ObjectFieldUtilities_h_*/
