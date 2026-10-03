/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_GUID_h_
#define _Stroika_Foundation_Common_GUID_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>

#if qStroika_Platform_MacOS
#include <uuid/uuid.h>
#elif qStroika_Platform_Windows
#include <guiddef.h>
#endif

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Concepts.h"

/**
 */

namespace Stroika::Foundation::Memory {
    class BLOB; // Forward declare to avoid mutual include issues
}

namespace Stroika::Foundation::Common {

    /**
     *  A very common 16-byte opaque ID structure.
     *
     *  \note Satisfies Concepts:
     *      o   sizeof (GUID) == 16
     *      o   ranges::range<GUID>
     *      o   regular<GUID>
     *      o   totally_ordered<GUID>
     * 
     *  \note <a href="Design-Overview.md#Comparisons">Comparisons</a>:
     *      o   static_assert (totally_ordered<GUID>);
     *      o   by field - Data1, then Data2, Data3 and Data4's bytes - so in the same order as its text (As<String> (), which
     *          writes it in lower case) and its RFC 9562 bytes (AsRFC9562Bytes ()); NOT necessarily as its in-memory bytes (As<Memory::BLOB> ()).
     *
     *  \note Byte order: its 16 bytes come in two orders, and each API taking or giving them says which:
     *      o   RFC 9562's - network byte order, the order its text is written in. As a uuid_t (<uuid/uuid.h>) holds them, and as
     *          RFC 9562's algorithms (a name-based UUID's, say) take them: FromRFC9562Bytes (), AsRFC9562Bytes ().
     *      o   this struct's own, in memory - Windows' GUID layout: its first three fields (Data1, Data2, Data3 - its first 8
     *          bytes) in the machine's byte order, its last 8 as written. The Memory::BLOB and array<> constructors, As<Memory::BLOB> ()
     *          and As<array<>> (), begin ()/end ()/data () - so too a GUID serialized as a BLOB. On a little-endian machine (as
     *          all those Stroika is tested on are, as of v3.0d25) its first 8 bytes differ from RFC 9562's; on a big-endian one
     *          the two orders would be the same.
     */
    struct GUID {
    private:
        static GUID mk_ (const string& src);

    public:
        /**
         *  \note - when converting from a string, GUID allows the leading/trailing {} to be optionally provided.
         *        - format's supported {61e4d49d-8c26-3480-f5c8-564e155c67a6} 
         *                           or 61e4d49d-8c26-3480-f5c8-564e155c67a6
         *  no argument CTOR, creates an all-zero GUID.
         * 
         *  \see GUID::GenerateNew () to create a new random GUID.
         * 
         *  \note The Memory::BLOB and array<> constructors take the 16 bytes in this struct's in-memory layout; for them in the
         *        order the text is written in (RFC 9562's - as a uuid_t holds them), @see FromRFC9562Bytes () - and GUID's note
         *        on byte order.
         *
         *  @todo maybe support more input formats, such as https://stackoverflow.com/questions/7775439/is-the-format-of-guid-always-the-same
         */
        constexpr GUID () noexcept = default;
#if qStroika_Platform_MacOS
        GUID (const uuid_t& src) noexcept; ///< \brief from a uuid_t (<uuid/uuid.h>) - its bytes in RFC 9562's order (@see FromRFC9562Bytes)
#elif qStroika_Platform_Windows
        constexpr GUID (const ::GUID& src) noexcept;
#endif
        template <Characters::IConvertibleToString STRISH_TYPE>
        GUID (STRISH_TYPE&& src);
        GUID (const Memory::BLOB& src);
        GUID (const array<byte, 16>& src) noexcept;
        GUID (const array<uint8_t, 16>& src) noexcept;

    public:
        /**
         *  Like Windows UuidCreate, or CoCreateGuid - create a random GUID (but portably): an RFC 9562 version 4 UUID, random
         *  but for its version and variant bits. (Before Stroika v3.0d25 all 128 bits were random - unique just the same, but
         *  not marked as version 4.)
         *
         *  \alias CreateNew(), CTOR
         * 
         *  \par Example Usage
         *      \code
         *          String newUUIDAsString = GUID::GenerateNew ().As<String> ();
         *      \endcode
         */
        static GUID GenerateNew () noexcept;

    public:
        /**
         *  \brief From its 16 bytes in RFC 9562's order: network byte order, the order its text is written in - as a uuid_t
         *         (<uuid/uuid.h>) holds them, and as RFC 9562's algorithms (a name-based UUID's, say) take them.
         *
         *  \note Not the Memory::BLOB and array<> constructors, which take the bytes in this struct's in-memory layout (@see
         *        GUID's note on byte order).
         *
         *  \par Example Usage
         *      \code
         *          array<byte, 16> b{};    // 00 01 02 ... 0f
         *          for (size_t i = 0; i < b.size (); ++i) {
         *              b[i] = static_cast<byte> (i);
         *          }
         *          Assert (GUID::FromRFC9562Bytes (b).As<String> () == "00010203-0405-0607-0809-0a0b0c0d0e0f");
         *      \endcode
         */
        static constexpr GUID FromRFC9562Bytes (span<const byte, 16> bytes) noexcept;

    public:
        /**
         *  \brief Its 16 bytes in RFC 9562's order - the order its text is written in (@see FromRFC9562Bytes).
         *
         *  \note Not As<Memory::BLOB> () or As<array<>> (), which give them in this struct's in-memory layout.
         */
        constexpr array<byte, 16> AsRFC9562Bytes () const noexcept;

    public:
        uint32_t Data1{};
        uint16_t Data2{};
        uint16_t Data3{};
        uint8_t  Data4[8]{};

    public:
        using value_type = byte;

    public:
        /**
         *  Its bytes in this struct's in-memory layout (@see GUID's note on byte order).
         *
         *  \nb: Stroika v2.1 allowed iterating and modifying in place of the GUID as a sequence of bytes, but no more
         */
        nonvirtual const byte* begin () const noexcept;

    public:
        /**
         *  \nb: Stroika v2.1 allowed iterating and modifying in place of the GUID as a sequence of bytes, but no more
         */
        nonvirtual const byte* end () const noexcept; ///< \brief @see begin ()

    public:
        /**
         */
        constexpr size_t size () const noexcept;

    public:
        /**
         *  Its bytes in this struct's in-memory layout - as As<Memory::BLOB> () (@see GUID's note on byte order).
         */
        nonvirtual explicit operator Memory::BLOB () const;

    public:
        /**
         *  By field - so in the same order as its text, and its RFC 9562 bytes; NOT as its in-memory bytes (@see GUID's note on
         *  comparisons).
         */
        nonvirtual strong_ordering operator<=> (const GUID&) const noexcept = default;

    public:
        /**
         *  Its bytes in this struct's in-memory layout (@see GUID's note on byte order).
         */
        nonvirtual const uint8_t* data () const noexcept;

    public:
        /**
         *  For now, only supported formats are
         *      o   String          -- format: 61e4d49d-8c26-3480-f5c8-564e155c67a6
         *      o   string          -- same
         *      o   BLOB
         *      o   array<uint8_t, 16> or array<byte, 16>
         *
         *  \note As<Memory::BLOB> () and As<array<>> () give the 16 bytes in this struct's in-memory layout - Windows' GUID layout,
         *        whose first three fields are in the machine's byte order. For them in the order the text is written in (RFC 9562's
         *        - as a uuid_t holds them), @see AsRFC9562Bytes () - and GUID's note on byte order.
         */
        template <IAnyOf<Characters::String, std::string, Memory::BLOB, array<byte, 16>, array<uint8_t, 16>> T>
        nonvirtual T As () const;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual Characters::String ToString () const;
    };
    static_assert (sizeof (GUID) == 16);
    static_assert (ranges::range<GUID>);
    static_assert (regular<GUID>);
    static_assert (totally_ordered<GUID>);

}

/*
 *  Already default-implemented in ToString() code, but this implementation is better (because by default
 *  'range version' used) and this takes precedence.
 */
template <>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Common::GUID, wchar_t>
    : qStroika_Foundation_Characters_FMT_PREFIX_::formatter<std::wstring, wchar_t> {
    using inherited = qStroika_Foundation_Characters_FMT_PREFIX_::formatter<std::wstring, wchar_t>;
    template <class FmtContext>
    typename FmtContext::iterator format (const Stroika::Foundation::Common::GUID& s, FmtContext& ctx) const;
};
template <>
struct qStroika_Foundation_Characters_FMT_PREFIX_::formatter<Stroika::Foundation::Common::GUID, char>
    : qStroika_Foundation_Characters_FMT_PREFIX_::formatter<std::string, char> {
    using inherited = qStroika_Foundation_Characters_FMT_PREFIX_::formatter<std::string, char>;
    template <class FmtContext>
    typename FmtContext::iterator format (const Stroika::Foundation::Common::GUID& s, FmtContext& ctx) const;
};

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