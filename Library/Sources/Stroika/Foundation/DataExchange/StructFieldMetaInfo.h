/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_
#define _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <type_traits>
#include <typeindex>

#include "../Characters/String.h"
#include "../Memory/BLOB.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    /**
     *  \note This class is just a utility to capture information about fields relative to
     *        a class for the purpose of serialization.
     */
    struct StructFieldMetaInfo {
    public:
        /**
         *  This is only defined to be perfectly safe in C++ for classes of type is_standard_layout()
         *  But that is uselessly insanely overly restricive (e.g. having private data members makes a
         *  struct not standard_layout).
         * 
         *  This appears to always work for all cases I've tried (but avoid things like virtual base classes - that might not
         *  work).
         * 
         *  @todo DOCS
         */
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        StructFieldMetaInfo (FIELD_VALUE_TYPE OWNING_OBJECT::*member);

    public:
        nonvirtual type_index GetTypeInfo () const
        {
            return fTypeInfo_;
        }

    public:
        /**
         *  @todo DOCS
         */
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        inline const FIELD_VALUE_TYPE* GetAddressOfMember (const OWNING_OBJECT* object) const
        {
            auto p2m = GetP2M_<FIELD_VALUE_TYPE, OWNING_OBJECT> ();
            return &(object->*p2m);
        }
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        inline FIELD_VALUE_TYPE* GetAddressOfMember (OWNING_OBJECT* object) const
        {
            auto p2m = GetP2M_<FIELD_VALUE_TYPE, OWNING_OBJECT> ();
            return &(object->*p2m);
        }
        template <typename OWNING_OBJECT>
        inline const std::byte* GetAddressOfMember (const OWNING_OBJECT* object) const
        {
            return GetAddressOfMember<std::byte, OWNING_OBJECT> (object); // unsafe case
        }
        template <typename OWNING_OBJECT>
        inline std::byte* GetAddressOfMember (OWNING_OBJECT* object) const
        {
            return GetAddressOfMember<std::byte, OWNING_OBJECT> (object); // unsafe case
        }

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const StructFieldMetaInfo& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const StructFieldMetaInfo& rhs) const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;

    private:
        type_index                                          fTypeInfo_; // type of FIELD_VALUE_TYPE
        Memory::InlineBuffer<std::byte, 2 * sizeof (void*)> fPTR2MEM_;  // type-erased FIELD_VALUE_TYPE OWNING_OBJECT::*fMember;

    private:
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        inline FIELD_VALUE_TYPE OWNING_OBJECT::*GetP2M_ () const
        {
            Require (sizeof (FIELD_VALUE_TYPE OWNING_OBJECT::*) == fPTR2MEM_.size ());
            using X = FIELD_VALUE_TYPE OWNING_OBJECT::*;
            //return *reinterpret_cast<X*> (fPTR2MEM_.data ());
            return *(X*)(fPTR2MEM_.data ());
        }
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StructFieldMetaInfo.inl"

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_*/
