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
    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    size_t ConvertPointerToDataMemberToOffset (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember));

    /**
     *  Given an objects field (given by pointer to member) and that owned object, run 'offsetof' backwards
     *  to get the original owning object.
     * 
     *  \par Example Usage
     *      \code
     *          void test ();
     *          struct X1 {
     *              int a;
     *              int b;
     *          };
     *          struct X2 {
     *          public:
     *              int a;
     *          private:
     *              int b;
     *          private:
     *              friend void test();
     *          };
     *          void DoTest ()
     *          {
     *              {
     *                  assert (ConvertPointerToDataMemberToOffset (&X1::a) == 0 or ConvertPointerToDataMemberToOffset (&X1::b) == 0);
     *                  assert (ConvertPointerToDataMemberToOffset (&X1::a) != 0 or ConvertPointerToDataMemberToOffset (&X1::b) != 0);
     *              }
     *              {
     *                  X1 t;
     *                  static_assert (is_standard_layout_v<X1>);
     *                  void* aAddr = &t.a;
     *                  void* bAddr = &t.b;
     *                  assert (GetObjectOwningField (aAddr, &X1::a) == &t);
     *                  assert (GetObjectOwningField (bAddr, &X1::b) == &t);
     *              }
     *              {
     *                  // Check and warning but since X2 is not standard layout, this isn't guaranteed to work
     *                  X2 t;
     *                  static_assert (not is_standard_layout_v<X2>);
     *                  void* aAddr = &t.a;
     *                  void* bAddr = &t.b;
     *                  assert (GetObjectOwningField (aAddr, &X2::a) == &t);
     *                  assert (GetObjectOwningField (bAddr, &X2::b) == &t);
     *              }
     *          }
     *      \endcode
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
     *        Posted to stackoverflow to find a better, more portable way: https://stackoverflow.com/questions/65940393/c-why-the-restriction-on-offsetof-for-non-standard-layout-objects-or-how-t
     */
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    OUTER_OBJECT* GetObjectOwningField (APPARENT_MEMBER_TYPE* aggregatedMember, AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember));
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    const OUTER_OBJECT* GetObjectOwningField (const APPARENT_MEMBER_TYPE* aggregatedMember, AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember));

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "ObjectFieldUtilities.inl"

#endif /*_Stroika_Foundation_Memory_ObjectFieldUtilities_h_*/
