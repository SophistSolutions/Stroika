/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_
#define _Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::DataExchange {

    /*
     ********************************************************************************
     **************************** StructFieldMetaInfo *******************************
     ********************************************************************************
     */
    inline StructFieldMetaInfo::StructFieldMetaInfo (size_t fieldOffset, type_index typeInfo)
        : fOffset (fieldOffset)
        , fTypeInfo (typeInfo)
    {
    }

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

}

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_*/
