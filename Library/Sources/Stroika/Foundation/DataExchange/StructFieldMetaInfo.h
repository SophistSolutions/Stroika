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
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *        o C++20 only (for c++17 only supported == and operator<)
     * 
     *  \note This class is just a utility to capture information about fields relative to
     *        a class for the purpose of serialization. It may not work for all class types
     *        due to C++ issues with pointers to members.
     *        @see Memory::OffsetOf() for details.
     * 
     *        But - when you cannot use this, you can still always use a custom serializer
     *        within ObjectVariantMapper;
     */
    struct StructFieldMetaInfo {
    public:
        type_index fTypeInfo; // type of FIELD_VALUE_TYPE

    private:
        // @todo FOR PERFORMANCE - probably avoid shared_ptr and instaead use inlinebuffer
        // save type-erased copy of pointer-to-member
        // just virtual DTOR so it can free used memory? NOt sure needed
        struct IRep {
            virtual ~IRep ()                               = default;
            virtual span<const std::byte> AsBytes () const = 0;
        };
        shared_ptr<IRep> fPointerToMember;
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        struct X : IRep {
            X (const X&) = default;
            X (FIELD_VALUE_TYPE OWNING_OBJECT::*member)
                : fMember{member}
            {
            }
            FIELD_VALUE_TYPE OWNING_OBJECT::*fMember;
            virtual span<const std::byte>    AsBytes () const override
            {
                return std::as_bytes (span{&fMember, 1});
            }
        };

    public:
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        inline const FIELD_VALUE_TYPE* GetAddressOfMember (const OWNING_OBJECT* object) const
        {
            auto p2m = reinterpret_cast<X<FIELD_VALUE_TYPE, OWNING_OBJECT>*> (fPointerToMember.get ())->fMember;
            return &(object->*p2m);
        }
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        inline FIELD_VALUE_TYPE* GetAddressOfMember (OWNING_OBJECT* object) const
        {
            auto p2m = reinterpret_cast<X<FIELD_VALUE_TYPE, OWNING_OBJECT>*> (fPointerToMember.get ())->fMember;
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
         *  This is only defined to be perfectly safe in C++ for classes of type is_standard_layout()
         *  But that is uselessly insanely overly restricive (e.g. having private data members makes a
         *  struct not standard_layout).
         * 
         *  This appears to always work for all cases I've tried (but avoid things like virtual base classes - that might not
         *  work).
         */
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        StructFieldMetaInfo (FIELD_VALUE_TYPE OWNING_OBJECT::*member);

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
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StructFieldMetaInfo.inl"

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_*/
