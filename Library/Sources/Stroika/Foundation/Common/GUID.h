/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
#include "../Configuration/Concepts.h"

/**
 */

namespace Stroika::Foundation::Memory {
    class BLOB; // Forward declare to avoid mutual include issues
}

namespace Stroika::Foundation::Common {

    /**
     *  A very common 16-byte opaque ID structure.
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     */
    struct GUID {
    public:
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
        constexpr GUID () noexcept = default;
#if qPlatform_Windows
        constexpr GUID (const ::GUID& src) noexcept;
#endif
        GUID (const string& src);
        GUID (const Memory::BLOB& src);
        GUID (const array<byte, 16>& src) noexcept;
        GUID (const array<uint8_t, 16>& src) noexcept;
        GUID (const Characters::String& src);

    public:
        uint32_t Data1{};
        uint16_t Data2{};
        uint16_t Data3{};
        uint8_t  Data4[8]{};

    public:
        using value_type = byte;

    public:
        /**
         *  \nb: Stroika v2.1 allowed iterating and modifying in place of the GUID as a sequence of bytes, but no more
         */
        nonvirtual const byte* begin () const noexcept;

    public:
        /**
         *  \nb: Stroika v2.1 allowed iterating and modifying in place of the GUID as a sequence of bytes, but no more
         */
        nonvirtual const byte* end () const noexcept;

    public:
        /**
         */
        constexpr size_t size () const noexcept;

    public:
        /**
         */
        nonvirtual explicit operator Memory::BLOB () const;

    public:
        /**
         *  Like Windows UuidCreate, or CoCreateGuid - create a random GUID (but portably).
         */
        static GUID GenerateNew () noexcept;

    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const GUID&) const noexcept = default;

    public:
        /**
         */
        nonvirtual const uint8_t* data () const noexcept;

    public:
        /**
         *  For now, only supported formats are
         *      o   String          -- format: {61e4d49d-8c26-3480-f5c8-564e155c67a6}
         *      o   string          -- same
         *      o   BLOB
         *      o   array<uint8_t, 16> or array<byte, 16>
         */
        template <typename T>
        nonvirtual T As () const
            requires (is_same_v<T, Characters::String> or is_same_v<T, std::string> or is_same_v<T, Memory::BLOB> or
                      is_same_v<T, array<byte, 16>> or is_same_v<T, array<uint8_t, 16>>);

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;
    };
    static_assert (sizeof (GUID) == 16);
    static_assert (ranges::range<GUID>);

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