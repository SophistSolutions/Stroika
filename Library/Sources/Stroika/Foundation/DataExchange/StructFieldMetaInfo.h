/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_
#define _Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include <type_traits>
#include <typeindex>

#include "../Characters/String.h"

/**
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   This is NOT safe (annoying offsetof rules). MAYBE possible to do with ptr-to-member, but I don't think
 *              so.
 */

namespace Stroika::Foundation::DataExchange {

    /**
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *        o C++20 only (for c++17 only supported == and operator<)
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

#if __cpp_impl_three_way_comparison >= 201907 && !qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const StructFieldMetaInfo& rhs) const = default;
#endif

#if __cpp_impl_three_way_comparison >= 201907 && qCompilerAndStdLib_template_template_argument_as_different_template_paramters_Buggy
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

#if __cpp_impl_three_way_comparison < 201907
    bool operator< (const StructFieldMetaInfo& lhs, const StructFieldMetaInfo& rhs);
    bool operator== (const StructFieldMetaInfo& lhs, const StructFieldMetaInfo& rhs);
#endif

    /**
     *  Regret adding a macro, but it just seems helpful (makes things much more terse,
     *  and avoids the possible bugs from specifing the CLASS twice).
     *  No need to use - but some may find it helpful...
     *
     *  I don't know of any way in C++ without macro - to capture a member name (for use in decltype
     *  thing and offsetof()).
     *
     *  \note
     *      This macro uses offsetof(). According to the C++11 spec, offsetof() is only supported
     *      for 'standard-layout' objects (e.g. those without virtual functions, or other fancy
     *      c++ stuff). As near as I can tell, this always works, but we may need to revisit
     *      the approach/question (could  we use pointer to member?).
     *
     *  \par Example Usage
     *      \code
     *          struct  Person {
     *              String  firstName;
     *              String  lastName;
     *          };
     *          StructFieldMetaInfo firstNameFieldInfo      =   Stroika_Foundation_DataExchange_StructFieldMetaInfo(Person, firstName)
     *          StructFieldMetaInfo lastNameFieldInfo       =   Stroika_Foundation_DataExchange_StructFieldMetaInfo(Person, lastName)
     *      \endcode
     */
#define Stroika_Foundation_DataExchange_StructFieldMetaInfo(CLASS, MEMBER) \
    Stroika::Foundation::DataExchange::StructFieldMetaInfo { Stroika::Foundation::Memory::OffsetOf (&CLASS::MEMBER), typeid (decltype (CLASS::MEMBER)) }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StructFieldMetaInfo.inl"

#endif /*_Stroika_Foundation_DataExchange_StructFieldMetaInfo_h_*/
