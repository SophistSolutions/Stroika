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
    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    inline size_t ConvertPointerToDataMemberToOffset (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember))
    {
        //https://stackoverflow.com/questions/12141446/offset-from-member-pointer-without-temporary-instance
        return reinterpret_cast<char*> (&(((OUTER_OBJECT*)0)->*dataMember)) - reinterpret_cast<char*> (0);
    }

    /*
     ********************************************************************************
     *************************** GetObjectOwningField *******************************
     ********************************************************************************
     */
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline OUTER_OBJECT* GetObjectOwningField (APPARENT_MEMBER_TYPE* aggregatedMember, AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        std::byte* adjustedAggregatedMember = reinterpret_cast<std::byte*> (static_cast<AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t  adjustment               = static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember));
        return reinterpret_cast<OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline const OUTER_OBJECT* GetObjectOwningField (const APPARENT_MEMBER_TYPE* aggregatedMember, AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        const std::byte* adjustedAggregatedMember = reinterpret_cast<const std::byte*> (static_cast<const AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t        adjustment               = static_cast<ptrdiff_t> (ConvertPointerToDataMemberToOffset (aggregatedPtrToMember));
        return reinterpret_cast<const OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }

}
#endif /*_Stroika_Foundation_Memory_ObjectFieldUtilities_inl_*/
