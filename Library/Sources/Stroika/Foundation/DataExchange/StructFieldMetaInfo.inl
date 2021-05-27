/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_
#define _Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Memory/Common.h"

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     **************************** StructFieldMetaInfo *******************************
     ********************************************************************************
     */
    inline StructFieldMetaInfo::StructFieldMetaInfo (size_t fieldOffset, type_index typeInfo)
        : fOffset{fieldOffset}
        , fTypeInfo{typeInfo}
    {
    }
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    inline StructFieldMetaInfo::StructFieldMetaInfo (FIELD_VALUE_TYPE OWNING_OBJECT::*member)
        : fOffset{Memory::OffsetOf (member)}
        , fTypeInfo{typeid (FIELD_VALUE_TYPE)}
    {
    }

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ********************* StructFieldMetaInfo operators ****************************
     ********************************************************************************
     */
    inline bool operator< (const StructFieldMetaInfo& lhs, const StructFieldMetaInfo& rhs)
    {
        if (lhs.fOffset < rhs.fOffset) {
            return true;
        }
        else if (lhs.fOffset > rhs.fOffset) {
            return false;
        }
        return lhs.fTypeInfo < rhs.fTypeInfo;
    }
    inline bool operator== (const StructFieldMetaInfo& lhs, const StructFieldMetaInfo& rhs)
    {
        return lhs.fOffset == rhs.fOffset and lhs.fTypeInfo == rhs.fTypeInfo;
    }
#endif

#if __cpp_impl_three_way_comparison >= 201907 && qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
    inline strong_ordering StructFieldMetaInfo::operator<=> (const StructFieldMetaInfo& rhs) const
    {
        auto r = fOffset <=> rhs.fOffset;
        if (r == 0) {
            if (fTypeInfo < rhs.fTypeInfo) {
                r = strong_ordering::less;
            }
            else if (fTypeInfo == rhs.fTypeInfo) {
                r = strong_ordering::equal;
            }
            else {
                r = strong_ordering::greater;
            }
        }
        return r;
    }

    inline bool StructFieldMetaInfo::operator== (const StructFieldMetaInfo& rhs) const
    {
        return fOffset == rhs.fOffset and fTypeInfo == rhs.fTypeInfo;
    }
#endif

}

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_*/
