/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Character_h_
#define _Stroika_Foundation_Characters_Character_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <span>

#include "../Common/Compare.h"
#include "../Configuration/Enumeration.h"
#include "../Memory/StackBuffer.h"

/**
 * TODO:
 *
 *      @todo   ToLower ('GERMAN ES-ZETT' or 'SHARP S') returns two esses ('ss') - and we return a single chararcter.
 *              We COULD change return value, or simply document that issue here and define ToLower() of STRING todo
 *              the right thing for queer cases like this, and use this API for the most common cases.
 * 
 *              I HOPE - though am not sure - that this is now addressed in Stroika v3 - by encoding Character as utf32_t.
 */

namespace Stroika::Foundation::Characters {

    /**
     */
    enum class CompareOptions : uint8_t {
        eWithCase,
        eCaseInsensitive,

        Stroika_Define_Enum_Bounds (eWithCase, eCaseInsensitive)
    };

    /**
     *  \brief check if T is char8_t, char16_t, char32_t - one of the three possible unicode UTF code-point classes.
     */
    template <typename T>
    concept Character_IsBasicUnicodeCodePoint =
        is_same_v<remove_cv_t<T>, char8_t> or is_same_v<remove_cv_t<T>, char16_t> or is_same_v<remove_cv_t<T>, char32_t>;

    /**
     *  \brief check if T is Character_IsBasicUnicodeCodePoint or wchar_t (any basic code-point class)
     */
    template <typename T>
    concept Character_IsUnicodeCodePoint = Character_IsBasicUnicodeCodePoint<T> or is_same_v<remove_cv_t<T>, wchar_t>;

    /**
     *  \brief check if T is char8_t, char16_t, char32_t (Character_IsBasicUnicodeCodePoint) or wchar_t
     */
    template <typename T>
    concept Character_IsUnicodeCodePointOrPlainChar = Character_IsUnicodeCodePoint<T> or is_same_v<remove_cv_t<T>, char>;

    /// @TODO LOSE OrPlainChar busines sand simplfiy below???? Only if we ASSERT 'char' implies ASCIII, and check that with assertions
    // so assert each char <= 127.
    class Character;

    /**
     *  \brief char8_t, char16_t, char32_t, wchar_t, Character - something that can be always safely interpreted as a UNICODE Character.
     * 
     *  Character_SafelyCompatible:
     *      o   char8_t
     *      o   char16_t
     *      o   char32_t
     *      o   wchar_t
     *  \note all these types are <= 4 bytes (size of char32_t)
     */
    template <typename T>
    concept Character_SafelyCompatible = Character_IsUnicodeCodePoint<T> or is_same_v<remove_cv_t<T>, Character>;
    static_assert (Character_SafelyCompatible<char8_t>);
    static_assert (Character_SafelyCompatible<char16_t>);
    static_assert (Character_SafelyCompatible<char32_t>);
    static_assert (Character_SafelyCompatible<wchar_t>);

    /**
     *  \brief Stroika's string/character classes treat 'char' as being an ASCII character
     * 
     *  This using declaration just documents that fact, without really enforcing anything.
     *  Prior to Stroika v3, the Stroika String classes basically prohibited the use of char
     *  because it was always UNCLEAR what characterset to interpret it as.
     * 
     *  But a safe (and quite useful) assumption, is just that it is ASCII. If you assume its
     *  always ASCII, you can get alot done, and allow that case. So Stroika v3 does that,
     *  with checks to enforce.
     */
    using Character_ASCII = char;
    static_assert (not Character_SafelyCompatible<Character_ASCII>);

    /**
    * &&&& CONSIDER LOSING THIS AND REPLACING WITH Character_CompatibleIsh but CAREFULLY REVIEWING EACH CASE
    * 
     *  \brief Something that can be reasonably converted into a Unicode Character object (Character_SafelyCompatible<T> or T==char)
     * 
     *  \note all these types are <= 4 bytes (size of char32_t)
     * 
     *  \note for many apis, when Character_Compatible==char, the API will require the characters are ASCII (but see each API for details
     *        on this point).
     */
    template <typename T>
    concept Character_Compatible = Character_SafelyCompatible<T> or is_same_v<remove_cv_t<T>, Character_ASCII>;

    /**
     *  Internally, several algorithms and data structures operate on this one-byte subset of UNICODE.
     *  However, most Stroika public APIs don't expose this, because this is not any kind of standard for APIs.
     *  APIs use char8_t, char16_t, char32_t, or char for ASCIICHAR.
     * 
     *  This refers to ASCII OR https://en.wikipedia.org/wiki/Latin-1_Supplement, so any UNICODE characater code point
     *  less than U+00FF.
     * 
     *  One nice thing about modern C++ for our purposes, is C++ offers so many 'distinct' 'unsigned byte' basic types, we can
     *  just hijack one that won't be confused with a regular character (string) - thus the static_assert.
     */
    //    using Character_ISOLatin1 = uint8_t;
    struct Character_ISOLatin1 {
        uint8_t data;
    };
    static_assert (sizeof (Character_ISOLatin1) == 1);
    static_assert (not Character_Compatible<Character_ISOLatin1>);

    //static_assert (sizeof (Character_ISOLatin1x) == 1);

    /**
     *  \brief Character_CompatibleIsh extends Character_Compatible with  Character_ISOLatin1
     * 
     *  \note Character_CompatibleIsh means any 'basic character type' - size <= 4 bytes, which
     *        could reasonably, in context (so with extra info), could be safely converted into
     *        a Character object.
     * 
     *  Character_CompatibleIsh
     *      o   char8_t                 Character_SafelyCompatible
     *      o   char16_t                ""
     *      o   char32_t                ""
     *      o   wchar_t                 ""
     *      o   Character_ASCII (char)  must be ASCII
     *      o   Character_ISOLatin1
     */
    template <typename T>
    concept Character_CompatibleIsh = Character_Compatible<T> or is_same_v<remove_cv_t<T>, Character_ISOLatin1>;
    static_assert (Character_CompatibleIsh<char8_t>);
    static_assert (Character_CompatibleIsh<char16_t>);
    static_assert (Character_CompatibleIsh<char32_t>);
    static_assert (Character_CompatibleIsh<wchar_t>);
    static_assert (Character_CompatibleIsh<Character_ASCII>);
    static_assert (Character_CompatibleIsh<Character_ISOLatin1>);

    /**
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *      o   Character::EqualsComparer and Character::ThreeWayComparer provided with construction parameters to allow case insensitive compares 
     */
    class Character {
    public:
        /**
         *  Default constructor produces a zero character.
         *  Constructor with char32_t always produces a valid character.
         * 
         *  The overload taking two char16_t surrogate pairs, may throw if given invalid code-points
         */
        constexpr Character () noexcept;
        constexpr Character (char32_t c) noexcept;
        constexpr Character (char16_t hiSurrogate, char16_t lowSurrogate);

    public:
        /**
         *  \req IsASCII()
         */
        nonvirtual char GetAsciiCode () const noexcept;

    public:
        /**
         *  \brief Return the char32_t UNICODE code-point associated with this character.
         */
        constexpr char32_t GetCharacterCode () const noexcept;

    public:
        /**
         *  Explicit cuz creates too many ambiguities with things like c == '\0' where conversions can go both ways.
         */
        explicit constexpr operator char32_t () const noexcept;

    public:
        /*
         * \brief return the character as a char32_t  (or on systems where wchar_t is large enuf, as wchar_t)
         * 
         *  \note Before Stroika v3, this always supported wchar_t.
         */
        template <typename T>
        nonvirtual T As () const noexcept
            requires (is_same_v<T, char32_t> or (sizeof (wchar_t) == sizeof (char32_t) and is_same_v<T, wchar_t>));

    public:
        /**
         *  \brief Return true iff the given character (or all in span) is (are) in the ascii range [0..0x7f]
         */
        constexpr bool IsASCII () const noexcept;
        template <Character_Compatible CHAR_T>
        static constexpr bool IsASCII (span<const CHAR_T> s) noexcept;

    public:
        /**
         *  \brief if not IsASCII (arg) throw RuntimeException...
         */
        template <Character_Compatible CHAR_T>
        static void CheckASCII (span<const CHAR_T> s);
        template <Character_Compatible CHAR_T>
        static void CheckASCII (span<CHAR_T> s);

    public:
        /**
         *  \brief Return true iff the given character (or all in span) is (are) in the ascii/iso-latin range [0..0xff]
         */
        constexpr bool IsLatin1 () const noexcept;
        template <Character_CompatibleIsh CHAR_T>
        static constexpr bool IsLatin1 (span<const CHAR_T> s) noexcept;

    public:
        /**
         *  \brief if not IsLatin1 (arg) throw RuntimeException...
         */
        template <Character_CompatibleIsh CHAR_T>
        static void CheckLatin1 (span<const CHAR_T> s);
        template <Character_CompatibleIsh CHAR_T>
        static void CheckLatin1 (span<CHAR_T> s);

    public:
        /**
         * FROM https://en.cppreference.com/w/cpp/string/wide/iswspace:
         *      In the default (C) locale, the whitespace characters are the following:
         *          space (0x20, ' ')
         *          form feed (0x0c, '\f')
         *          line feed (0x0a, '\n')
         *          carriage return (0x0d, '\r')
         *          horizontal tab (0x09, '\t')
         *          vertical tab (0x0b, '\v')
         *       ...
         *       ISO 30112 defines POSIX space characters as Unicode characters 
         *          U+0009..U+000D, U+0020, U+1680, U+180E, U+2000..U+2006, U+2008..U+200A, U+2028, U+2029, U+205F, and U+3000.
         * 
         *  \note before Stroika v3.0d1, this just used iswspace()
         */
        constexpr bool IsWhitespace () const noexcept;

    public:
        nonvirtual bool IsDigit () const noexcept;

    public:
        nonvirtual bool IsHexDigit () const noexcept;

    public:
        nonvirtual bool IsAlphabetic () const noexcept;

    public:
        /**
         * Checks if the given character is uppper case. Can be called on any character.
         * Returns false if not alphabetic
         */
        nonvirtual bool IsUpperCase () const noexcept;

    public:
        /**
         * Checks if the given character is lower case. Can be called on any character.
         * Returns false if not alphabetic
         */
        nonvirtual bool IsLowerCase () const noexcept;

    public:
        /**
         */
        nonvirtual bool IsAlphaNumeric () const noexcept;

    public:
        /**
         */
        nonvirtual bool IsPunctuation () const noexcept;

    public:
        /**
         *  According to https://en.cppreference.com/w/cpp/string/wide/iswcntrl
         * 
         *  ISO 30112 defines POSIX control characters as Unicode characters U+0000..U+001F, 
         *  U+007F..U+009F, U+2028, and U+2029 (Unicode classes Cc, Zl, and Zp)
         */
        constexpr bool IsControl () const noexcept;

    public:
        /**
         *      Note that this does NOT modify the character in place but returns the new desired
         * character.
         *
         *      It is not necessary to first check
         * if the argument character is uppercase or alpabetic. ToLowerCase () just returns the
         * original character if there is no sensible conversion.
         * 
         *  \todo @todo See https://www.open-std.org/JTC1/SC35/WG5/docs/30112d10.pdf
         * 
         *  \see https://en.cppreference.com/w/cpp/string/wide/towlower
         *      Only 1:1 character mapping can be performed by this function, 
         *      e.g. the Greek uppercase letter 'Σ' has two lowercase forms, depending on the
         *      position in a word: 'σ' and 'ς'. A call to std::towlower cannot be used to
         *      obtain the correct lowercase form in this case.
         */
        nonvirtual Character ToLowerCase () const noexcept;

    public:
        /**
         *      Note that this does NOT modify the character in place but returns the new desired
         * character.
         *
         *      It is not necessary to first check
         * if the argument character is lowercase or alpabetic. ToUpperCase () just returns the
         * original character if there is no sensible conversion.
         * 
         *  \todo @todo See https://www.open-std.org/JTC1/SC35/WG5/docs/30112d10.pdf
         */
        nonvirtual Character ToUpperCase () const noexcept;

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         * Only specifically specialized variants are supported (right now just <string> supported).
         * If this source contains any invalid ASCII characters, this returns false, and otherwise true (with set into).
         * 
         *  Supported Types:
         *      o   Memory::StackBuffer<char>
         *      o   string
         */
        template <typename RESULT_T = string, Character_Compatible CHAR_T>
        static bool AsASCIIQuietly (span<const CHAR_T> fromS, RESULT_T* into)
            requires (is_same_v<RESULT_T, string> or is_same_v<RESULT_T, Memory::StackBuffer<char>>);

    public:
        /**
         * See https://en.wikipedia.org/wiki/Universal_Character_Set_characters#Surrogates
         */
        static constexpr char16_t UNI_SUR_HIGH_START{0xD800};
        static constexpr char16_t UNI_SUR_HIGH_END{0xDBFF};
        static constexpr char16_t UNI_SUR_LOW_START{0xDC00};
        static constexpr char16_t UNI_SUR_LOW_END{0xDFFF};

    public:
        /**
         *  Return true iff this Character (or argument codepoints) represent a
         *  character which would be represented in UCS-16 as a surrogate pair.
         */
        constexpr bool        IsSurrogatePair () const;
        static constexpr bool IsSurrogatePair (char16_t hiSurrogate, char16_t lowSurrogate);
        static constexpr bool IsSurrogatePair_Hi (char16_t hiSurrogate);
        static constexpr bool IsSurrogatePair_Lo (char16_t lowSurrogate);

    public:
        /**
         */
        constexpr bool operator== (const Character&) const noexcept = default;

    public:
        /**
         */
        constexpr strong_ordering operator<=> (const Character&) const noexcept = default;

    public:
        struct EqualsComparer;

    public:
        struct ThreeWayComparer;

    public:
        /**
         *  utility to compare an array of characters, like strcmp (), except with param saying if case sensative or insensitative.
         *
         *  \todo   Consider if this should be somehow packaged with Character::ThreeWayComparer?
         */
        template <Character_Compatible CHAR_T>
        static constexpr strong_ordering Compare (span<const CHAR_T> lhs, span<const CHAR_T> rhs, CompareOptions co) noexcept;

    public:
        [[deprecated ("Since Stroika 3.0d1, use span based Compare")]] static strong_ordering
        Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co) noexcept
        {
            return Compare (span{lhsStart, lhsEnd}, span{rhsStart, rhsEnd}, co);
        }

    private:
        char32_t fCharacterCode_;
    };

    /**
     *  Like equal_to<Character> but allow optional case insensitive compares
     */
    struct Character::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        /**
         *  optional CompareOptions to CTOR allows for case insensative compares
         */
        constexpr EqualsComparer (Stroika::Foundation::Characters::CompareOptions co = Stroika::Foundation::Characters::CompareOptions::eWithCase) noexcept;

        /**
         */
        constexpr bool operator() (Character lhs, Character rhs) const noexcept;

        Stroika::Foundation::Characters::CompareOptions fCompareOptions;
    };

    /**
     *  Like compare_three_way but allow optional case insensitive compares
     */
    struct Character::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        /**
         *  optional CompareOptions to CTOR allows for case insensative compares
         */
        constexpr ThreeWayComparer (Stroika::Foundation::Characters::CompareOptions co = Stroika::Foundation::Characters::CompareOptions::eWithCase) noexcept;

        /**
         */
        nonvirtual auto operator() (Stroika::Foundation::Characters::Character lhs, Stroika::Foundation::Characters::Character rhs) const noexcept;

        Stroika::Foundation::Characters::CompareOptions fCompareOptions;
    };

    [[deprecated ("UNSUPPORTED Since Stroika v3.0d1")]] const wchar_t* CVT_CHARACTER_2_wchar_t (const Character* c);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Character.inl"

#endif /*__Character__*/
