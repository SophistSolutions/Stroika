/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_ObjectFieldUtilities_inl_
#define _Stroika_Foundation_Memory_ObjectFieldUtilities_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Memory {

    /*
     ********************************************************************************
     *********************** ConvertPointerToDataMemberToOffset *********************
     ********************************************************************************
     */
    template <typename OUTER_OBJECT, typename PROPERTY_TYPE>
    inline size_t ConvertPointerToDataMemberToOffset (PROPERTY_TYPE (OUTER_OBJECT::*dataMember))
    {
        //https://stackoverflow.com/questions/12141446/offset-from-member-pointer-without-temporary-instance
        return reinterpret_cast<char*> (&(((OUTER_OBJECT*)nullptr)->*dataMember)) - reinterpret_cast<char*> (nullptr);
    }

    /*
     ********************************************************************************
     *************************** GetObjectOwningField *******************************
     ********************************************************************************
     */
    template <typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline OUTER_OBJECT* GetObjectOwningField (void* aggregatedMember, typename AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        return reinterpret_cast<OUTER_OBJECT*> (
            reinterpret_cast<std::byte*> (aggregatedMember) - static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember)));
    }
    template <typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline const OUTER_OBJECT* GetObjectOwningField (const void* aggregatedMember, typename AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        return reinterpret_cast<const OUTER_OBJECT*> (
            reinterpret_cast<const std::byte*> (aggregatedMember) - static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember)));
    }

}
#endif /*_Stroika_Foundation_Memory_ObjectFieldUtilities_inl_*/
