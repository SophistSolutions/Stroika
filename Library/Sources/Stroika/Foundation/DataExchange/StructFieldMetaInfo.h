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
         *  Grab the meta info associated with a pointer to member - its type info, and its pointer to member address.
         * 
         *  \note - unlike eariler versions of Stroika which used std::offsetof() (or similar) - and therefore only
         *          worked (guaranteed) on std-layout objects, this should always work portably.
         */
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        StructFieldMetaInfo (FIELD_VALUE_TYPE OWNING_OBJECT::*member);

    public:
        /**
         *  returns typeid (constructed FIELD_VALUE_TYPE)
         */
        nonvirtual type_index GetTypeInfo () const;

    public:
        /**
         *  Given an object of the same type as the StructFieldMetaInfo was constructed with (or a subclass), return a pointer
         *  to the field value corresponding to the field this StructFieldMetaInfo was constructed with. Note this is not checked
         *  since no way to check subclassof from type_index.
         * 
         *  Respect const of the OWNING_OBJECT, propagating that to this functions result.
         * 
         *  If the FIELD_VALUE_TYPE is not known by the caller the actual address will be statically cast to std::byte* (or const of that).
         * 
         *  \note - doing THAT PROBABLY INVOKES C++ UNDEFINED BEHAVIOR - so just moves the 'hole' / 'issue' we had before Stroika v3.0
         *          to this 'cast' (https://stackoverflow.com/questions/12141446/offset-from-member-pointer-without-temporary-instance). 
         *          Still hunting for a good way around that. But where this is used in ObjectVariantMapper, we don't really
         *          know those types. --LGP 2023-09=25
         */
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        nonvirtual const FIELD_VALUE_TYPE* GetAddressOfMember (const OWNING_OBJECT* object) const;
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        nonvirtual FIELD_VALUE_TYPE* GetAddressOfMember (OWNING_OBJECT* object) const;
        template <typename OWNING_OBJECT>
        nonvirtual const std::byte* GetAddressOfMember (const OWNING_OBJECT* object) const;
        template <typename OWNING_OBJECT>
        nonvirtual std::byte* GetAddressOfMember (OWNING_OBJECT* object) const;

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
        inline FIELD_VALUE_TYPE OWNING_OBJECT::*const GetP2M_ () const;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StructFieldMetaInfo.inl"

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_*/
