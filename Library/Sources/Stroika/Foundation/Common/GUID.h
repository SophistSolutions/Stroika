/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_GUID_h_
#define _Stroika_Foundation_Common_GUID_h_ 1

#include "../StroikaPreComp.h"

#include <compare>

#if qPlatform_Windows
#include <guiddef.h>
#endif

#include "../Characters/ToString.h"
#include "../Configuration/Common.h"

/**
 */

namespace Stroika::Foundation::Memory {
    class BLOB; // Forward declare to avoid mutual include issues
}

namespace Stroika::Foundation::Common {

    /**
     *  A very common 16-byte opaque ID structure.
     *
     *  \note <a href="Coding Conventions.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    struct GUID {
        /**
         *  \note - when converting from a string, GUID allows the leading/trailing {} to be optionally provided.
         *        - format's supported {61e4d49d-8c26-3480-f5c8-564e155c67a6} 
         *                           or 61e4d49d-8c26-3480-f5c8-564e155c67a6
         *  no argument CTOR, creates an all-zero GUID.
         * 
         *  @todo maybe support more input formats, such as https://stackoverflow.com/questions/7775439/is-the-format-of-guid-always-the-same
         *  @todo - should allow input format of raw bytes (though unclear of endian interpretation that would be best
         *        in that case)
         */
        constexpr GUID () = default;
#if qPlatform_Windows
        constexpr GUID (const ::GUID& src);
#endif
        GUID (const string& src);
        GUID (const Memory::BLOB& src);
        GUID (const array<uint8_t, 16>& src);
        GUID (const Characters::String& src);

        uint32_t Data1{};
        uint16_t Data2{};
        uint16_t Data3{};
        uint8_t  Data4[8]{};

    public:
        /**
         *  Allow iterating and modifying in place of the GUID as a sequence of bytes
         */
        nonvirtual std::byte* begin ();
        nonvirtual const std::byte* begin () const;

    public:
        /**
         *  Allow iterating and modifying in place of the GUID as a sequence of bytes
         */
        nonvirtual std::byte* end ();
        nonvirtual const std::byte* end () const;

    public:
        /**
         */
        nonvirtual explicit operator Memory::BLOB () const;

    public:
        /**
         *  Like Windows UuidCreate, or CoCreateGuid - create a random GUID (but portably).
         */
        static GUID GenerateNew ();

#if !qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const GUID&) const = default;
#endif

    public:
        /**
         *  For now, only supported formats are
         *      o   String          -- format: {61e4d49d-8c26-3480-f5c8-564e155c67a6}
         *      o   string          -- ''
         *      o   BLOB
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;
    };
    static_assert (sizeof (GUID) == 16);

#if qCompilerAndStdLib_SpaceshipAutoGenForOpEqualsForCommonGUID_Buggy
    /**
     *  Basic operator overloads with the obvious meaning, and simply indirect to @GUID::ThreeWayComparer ()
     */
    bool operator< (const GUID& lhs, const GUID& rhs);
    bool operator<= (const GUID& lhs, const GUID& rhs);
    bool operator== (const GUID& lhs, const GUID& rhs);
    bool operator!= (const GUID& lhs, const GUID& rhs);
    bool operator>= (const GUID& lhs, const GUID& rhs);
    bool operator> (const GUID& lhs, const GUID& rhs);
#endif

    template <>
    Characters::String GUID::As () const;
    template <>
    string GUID::As () const;
    template <>
    Memory::BLOB GUID::As () const;

}

namespace Stroika::Foundation::DataExchange {
    template <typename T>
    struct DefaultSerializer; // Forward declare to avoid mutual include issues
    template <>
    struct DefaultSerializer<Stroika::Foundation::Common::GUID> {
        Memory::BLOB operator() (const Stroika::Foundation::Common::GUID& arg) const;
    };
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "GUID.inl"

#endif /*_Stroika_Foundation_Common_GUID_h_*/