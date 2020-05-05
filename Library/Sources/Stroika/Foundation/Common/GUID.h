/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_GUID_h_
#define _Stroika_Foundation_Common_GUID_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#if qPlatform_Windows
#include <guiddef.h>
#endif

#include "../Characters/ToString.h"
#include "../Configuration/Common.h"

/**
 */

namespace Stroika::Foundation::Common {

    /**
     *  A very common 16-byte opaque ID structure.
     */
    struct GUID {
        /**
         *  \note - when converting from a string, GUID allows the leading/trailing {} to be optionally provided.
         */
        constexpr GUID () = default;
#if qPlatform_Windows
        constexpr GUID (const ::GUID& src);
#endif
        GUID (const string& src);
        GUID (const array<uint8_t, 16>& src);
        GUID (const Characters::String& src);

        uint32_t Data1{};
        uint16_t Data2{};
        uint16_t Data3{};
        uint8_t  Data4[8]{};

    public:
        /**
         */
        static constexpr GUID Zero ();

    public:
        /**
         *  Like Windows UuidCreate, or CoCreateGuid - create a random GUID (but portably).
         */
        static GUID GenerateNew ();

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const GUID&) const = default;
#endif

    public:
        struct ThreeWayComparer;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;
    };

    /**
     *  @see Common::ThreeWayComparer<> template
     */
    struct GUID::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        nonvirtual Common::strong_ordering operator() (const GUID& lhs, const GUID& rhs) const;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @GUID::ThreeWayComparer ()
     */
    bool operator< (const GUID& lhs, const GUID& rhs);
    bool operator<= (const GUID& lhs, const GUID& rhs);
    bool operator== (const GUID& lhs, const GUID& rhs);
    bool operator!= (const GUID& lhs, const GUID& rhs);
    bool operator>= (const GUID& lhs, const GUID& rhs);
    bool operator> (const GUID& lhs, const GUID& rhs);
#endif

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "GUID.inl"

#endif /*_Stroika_Foundation_Common_GUID_h_*/