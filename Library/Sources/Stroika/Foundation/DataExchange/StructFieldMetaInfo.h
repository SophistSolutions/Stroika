/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_
#define _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <type_traits>
#include <typeindex>

#include "../Characters/String.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::DataExchange {

    /**
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
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
        size_t     fOffset;
        type_index fTypeInfo;

    public:
        /**
         *  This is only defined to be perfectly safe in C++ for classes of type is_standard_layout()
         *  But that is uselessly insanely overly restricive (e.g. having private data members makes a
         *  struct not standard_layout).
         * 
         *  This appears to always work for all cases I've tried (but avoid things like virtual base classes - that might not
         *  work).
         */
        StructFieldMetaInfo (size_t fieldOffset, type_index typeInfo);
        template <typename FIELD_VALUE_TYPE, typename OWNING_OBJECT>
        StructFieldMetaInfo (FIELD_VALUE_TYPE OWNING_OBJECT::*member);

#if !qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const StructFieldMetaInfo& rhs) const = default;
#endif

#if qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const StructFieldMetaInfo& rhs) const;

    public:
        /**
         */
        nonvirtual bool operator== (const StructFieldMetaInfo& rhs) const;
#endif

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
