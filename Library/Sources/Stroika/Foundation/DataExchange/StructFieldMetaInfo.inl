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
        : fTypeInfo_{typeid (FIELD_VALUE_TYPE)}
    {
        fPTR2MEM_.push_back (as_bytes (span{&member, 1}));
        Assert (fPTR2MEM_.size () <= 2 * sizeof (void*)); // not required - just a tmphack to test/verify
    }
    inline type_index StructFieldMetaInfo::GetTypeInfo () const
    {
        return fTypeInfo_;
    }
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    inline const FIELD_VALUE_TYPE* StructFieldMetaInfo::GetAddressOfMember (const OWNING_OBJECT* object) const
    {
        auto p2m = GetP2M_<FIELD_VALUE_TYPE, OWNING_OBJECT> ();
        return &(object->*p2m);
    }
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    inline FIELD_VALUE_TYPE* StructFieldMetaInfo::GetAddressOfMember (OWNING_OBJECT* object) const
    {
        auto p2m = GetP2M_<FIELD_VALUE_TYPE, OWNING_OBJECT> ();
        return &(object->*p2m);
    }
    template <typename OWNING_OBJECT>
    inline const byte* StructFieldMetaInfo::GetAddressOfMember (const OWNING_OBJECT* object) const
    {
        return GetAddressOfMember<byte, OWNING_OBJECT> (object); // unsafe case (see comments in method description)
    }
    template <typename OWNING_OBJECT>
    inline byte* StructFieldMetaInfo::GetAddressOfMember (OWNING_OBJECT* object) const
    {
        return GetAddressOfMember<byte, OWNING_OBJECT> (object); // unsafe case (see comments in method description)
    }
    inline strong_ordering StructFieldMetaInfo::operator<=> (const StructFieldMetaInfo& rhs) const
    {
        strong_ordering r = Memory::MemCmp (span{this->fPTR2MEM_}, span{rhs.fPTR2MEM_});
        if (r == strong_ordering::equal) {
            if (fTypeInfo_ < rhs.fTypeInfo_) {
                r = strong_ordering::less;
            }
            else if (fTypeInfo_ == rhs.fTypeInfo_) {
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
        return fTypeInfo_ == rhs.fTypeInfo_ and Memory::MemCmp (span{this->fPTR2MEM_}, span{rhs.fPTR2MEM_}) == strong_ordering::equal;
    }
    template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
    inline FIELD_VALUE_TYPE OWNING_OBJECT ::*const StructFieldMetaInfo::GetP2M_ () const
    {
        Require (sizeof (const FIELD_VALUE_TYPE OWNING_OBJECT::*) == fPTR2MEM_.size ());
        using X                        = FIELD_VALUE_TYPE OWNING_OBJECT::*const;
        X*                         aaa = reinterpret_cast<X*> (fPTR2MEM_.data ());
        return *aaa;
        // return *(X*)(fPTR2MEM_.data ());
    }

}

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_inl_*/
