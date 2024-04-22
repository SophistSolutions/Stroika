/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_ObjectFieldUtilities_inl_
#define _Stroika_Foundation_Memory_ObjectFieldUtilities_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "Stroika/Foundation/Debug/Assertions.h"

#include "Common.h"

namespace Stroika::Foundation::Memory {

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
        byte*     adjustedAggregatedMember = reinterpret_cast<byte*> (static_cast<AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t adjustment               = static_cast<ptrdiff_t> (OffsetOf (aggregatedPtrToMember));
        return reinterpret_cast<OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }
    template <typename APPARENT_MEMBER_TYPE, typename OUTER_OBJECT, typename AGGREGATED_OBJECT_TYPE>
    inline const OUTER_OBJECT* GetObjectOwningField (const APPARENT_MEMBER_TYPE* aggregatedMember,
                                                     AGGREGATED_OBJECT_TYPE (OUTER_OBJECT::*aggregatedPtrToMember))
    {
        RequireNotNull (aggregatedMember);
        RequireNotNull (aggregatedPtrToMember);
        const byte* adjustedAggregatedMember = reinterpret_cast<const byte*> (static_cast<const AGGREGATED_OBJECT_TYPE*> (aggregatedMember));
        ptrdiff_t adjustment = static_cast<ptrdiff_t> (OffsetOf (aggregatedPtrToMember));
        return reinterpret_cast<const OUTER_OBJECT*> (adjustedAggregatedMember - adjustment);
    }

    template <typename OUTER_OBJECT, typename DATA_MEMBER_TYPE>
    [[deprecated ("Since Stroika v3.0d2 - use Memory::OffsetOf()")]] inline constexpr size_t
    ConvertPointerToDataMemberToOffset (DATA_MEMBER_TYPE (OUTER_OBJECT::*dataMember))
    {
        return Memory::OffsetOf (dataMember);
    }

}
#endif /*_Stroika_Foundation_Memory_ObjectFieldUtilities_inl_*/
