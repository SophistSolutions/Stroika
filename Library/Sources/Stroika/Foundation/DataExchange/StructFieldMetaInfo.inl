/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    inline StructFieldMetaInfo::StructFieldMetaInfo (FIELD_VALUE_TYPE OWNING_OBJECT::*member)
        : fTypeInfo{typeid (FIELD_VALUE_TYPE)}
    {
        fPTR2MEM_.push_back (as_bytes (span{&member, 1}));
    }
    inline strong_ordering StructFieldMetaInfo::operator<=> (const StructFieldMetaInfo& rhs) const
    {
        strong_ordering r = Memory::MemCmp (span{this->fPTR2MEM_}, span{rhs.fPTR2MEM_});
        if (r == strong_ordering::equal) {
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
        return fTypeInfo == rhs.fTypeInfo and Memory::MemCmp (span{this->fPTR2MEM_}, span{rhs.fPTR2MEM_}) == strong_ordering::equal;
    }

}

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_*/
