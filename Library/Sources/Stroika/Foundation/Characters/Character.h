﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Character_h_
#define _Stroika_Foundation_Characters_Character_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <compare>
#include <span>
#include <type_traits>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Enumeration.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   ToLower ('GERMAN ES-ZETT' or 'SHARP S') returns two esses ('ss') - and we return a single character.
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
    using CompareOptions::eCaseInsensitive;
    using CompareOptions::eWithCase;

    /**
     *  \brief Stroika's string/character classes treat 'char' as being an ASCII character
     * 
     *  This using declaration just documents that fact, without really enforcing anything.
     *  Prior to Stroika v3, the Stroika String classes basically prohibited the use of char
     *  because it was always UNCLEAR what character set to interpret it as.
     * 
     *  But a safe (and quite useful) assumption, is just that it is ASCII. If you assume its
     *  always ASCII, you can simplify a lot of pragmatic usage. So Stroika v3 does that,
     *  with checks to enforce.
     * 
     *  So generally - Stroika String (and Character) APIs - if given a 'char' REQUIRE that it be
     *  ASCII (unless otherwise documented in that API). Use u8string, or something else if you don't want
     *  to assume ASCII.
     */
    using ASCII = char;

    /**
     *  Internally, several algorithms and data structures operate on this one-byte subset of UNICODE.
     *  However, most Stroika public APIs don't expose this, because this is not any kind of standard for APIs.
     *  APIs use char8_t, char16_t, char32_t, ASCII (aka char).
     * 
     *  This refers to ASCII OR https://en.wikipedia.org/wiki/Latin-1_Supplement, so any UNICODE character code point
     *  less than U+00FF.
     * 
     *  \note Considered using Latin1 = uint8_t; But this is better since less likely accidentally used.
     */
    struct Latin1 {
        uint8_t        data;
        constexpr      operator uint8_t () const;
        constexpr bool operator== (const Latin1&) const  = default;
        constexpr auto operator<=> (const Latin1&) const = default;
    };
    static_assert (is_trivially_constructible_v<Latin1>);
    static_assert (is_trivially_destructible_v<Latin1>);
    static_assert (sizeof (Latin1) == 1); // so can re_reinterpret_cast<> between Latin1 and unsigned char/uint8_t;

    /**
     *  \brief check if T is char8_t, char16_t, char32_t - one of the three possible UNICODE UTF code-point classes.
     *
     *  IBasicUNICODECodePoint:
     *      o   char8_t
     *      o   char16_t
     *      o   char32_t
     */
    template <typename T>
    concept IBasicUNICODECodePoint = Configuration::IAnyOf<remove_cv_t<T>, char8_t, char16_t, char32_t>;

    /**
     *  \brief check if T is IBasicUNICODECodePoint or wchar_t (any basic code-point class)
     *
     *  IUCodePoint:
     *      o   char8_t     IBasicUNICODECodePoint
     *      o   char16_t    ""
     *      o   char32_t    ""
     *      o   wchar_t     added here
     */
    template <typename T>
    concept IUNICODECodePoint = IBasicUNICODECodePoint<T> or same_as<remove_cv_t<T>, wchar_t>;

    /**
     *  concept IStdBasicStringCompatibleCharacter tests if the 'T' argument is a legit CHARACTER argument to std::basic_string.
     * 
     *  \note ALL of these character types are ALSO legitimate arguments to Stroika's String class (array of span of such). In the case
     *        of char, the text must be ASCII.
     *  \note IStdBasicStringCompatibleCharacter<T> => IUNICODECanUnambiguouslyConvertFrom<T>
     */
    template <typename T>
    concept IStdBasicStringCompatibleCharacter = Configuration::IAnyOf<T, char, char8_t, char16_t, char32_t, wchar_t>;

    class Character;

    /**
     *  \brief UNICODE string can be always be converted into array of this type
     * 
     *  IUNICODECanAlwaysConvertTo:
     *      o   char8_t         IBasicUNICODECodePoint
     *      o   char16_t        ""
     *      o   char32_t        ""
     *      o   wchar_t         IUNICODECodePoint
     *      o   Character       added in IUNICODECanAlwaysConvertTo
     *  \note all these types are <= 4 bytes (size of char32_t)
     * 
     *  \note - ASCII and Latin1 are NOT included here because - though these strings
     *          can be unambiguously converted to UNICODE, the REVERSE is not true (since for example
     *          not all UNICODE strings are ascii).
     */
    template <typename T>
    concept IUNICODECanAlwaysConvertTo = IUNICODECodePoint<T> or same_as<remove_cv_t<T>, Character>;
    static_assert (IUNICODECanAlwaysConvertTo<char8_t>);
    static_assert (IUNICODECanAlwaysConvertTo<char16_t>);
    static_assert (IUNICODECanAlwaysConvertTo<char32_t>);
    static_assert (IUNICODECanAlwaysConvertTo<wchar_t>);
    //static_assert (IUNICODECanAlwaysConvertTo<Character>); true but not defined yet, so cannot assert here
    static_assert (not IUNICODECanAlwaysConvertTo<ASCII>);
    static_assert (not IUNICODECanAlwaysConvertTo<Latin1>);

    /*
     *  IPossibleCharacterRepresentation concept corresponds to any type which MIGHT be convertible back and forth into a Character (possibly with extra information).
     *  For example, ASCII, Latin1, wchar_t, and obviously char32_t, etc...
     */
    template <typename T>
    concept IPossibleCharacterRepresentation = convertible_to<T, char32_t> or same_as<remove_cv_t<T>, Character>;
    static_assert (IPossibleCharacterRepresentation<char8_t>);
    static_assert (IPossibleCharacterRepresentation<char16_t>);
    static_assert (IPossibleCharacterRepresentation<char32_t>);
    static_assert (IPossibleCharacterRepresentation<wchar_t>);
    //static_assert (IPossibleCharacterRepresentation<Character>); true but not defined yet, so cannot assert here
    static_assert (IPossibleCharacterRepresentation<ASCII>);
    static_assert (IPossibleCharacterRepresentation<Latin1>);
    static_assert (not IPossibleCharacterRepresentation<std::byte>);

    /**
     *  \brief IUNICODECanUnambiguouslyConvertFrom is any 'character representation type' where array of them unambiguously convertible to UNICODE string
     *
     *  IUNICODECanUnambiguouslyConvertFrom:
     *      o   char8_t             IUNICODECodePoint
     *      o   char16_t            ""
     *      o   char32_t            ""
     *      o   wchar_t             ""
     *      o   Character           added
     *      o   ASCII               added               (NOTE - this is a typedef for char - so CHAR* interpreted as ASCII ONLY)
     *      o   Latin1              added
     * 
     *  \note IUNICODECanUnambiguouslyConvertFrom means any 'basic character type' - size <= 4 bytes, which
     *        could reasonably, in context (so with extra info), could be safely converted into
     *        a Character object.
     * 
     *  \note Possible alias for this - Character_CanConditionallyConvertUNICODEStringToArrayOfThese
     *        for example, ASCII is one of these - any depending on what is in the UNICODE string
     *        you maybe able to (unambiguously) covnert to a string of this type.
     * 
     *  \see also IUNICODECanUnambiguouslyConvertTo
     */
    template <typename T>
    concept IUNICODECanUnambiguouslyConvertFrom = IUNICODECodePoint<T> or Configuration::IAnyOf<remove_cv_t<T>, Character, ASCII, Latin1>;
    static_assert (IUNICODECanUnambiguouslyConvertFrom<char8_t>);
    static_assert (IUNICODECanUnambiguouslyConvertFrom<char16_t>);
    static_assert (IUNICODECanUnambiguouslyConvertFrom<char32_t>);
    static_assert (IUNICODECanUnambiguouslyConvertFrom<wchar_t>);
    //static_assert (IUNICODECanUnambiguouslyConvertFrom<Character>); true but not defined yet, so cannot assert here
    static_assert (IUNICODECanUnambiguouslyConvertFrom<ASCII>);
    static_assert (IUNICODECanUnambiguouslyConvertFrom<Latin1>);

    /**
     *  \brief IUNICODECanUnambiguouslyConvertTo is any 'character representation type' you can unambiguously convert a UNICODE string into.
     *
     *  IUNICODECanUnambiguouslyConvertTo:
     *      o   char8_t             IUNICODECodePoint
     *      o   char16_t            ""
     *      o   char32_t            ""
     *      o   wchar_t             ""
     *      o   Character           added
     * 
     *  \see also IUNICODECanUnambiguouslyConvertFrom
     */
    template <typename T>
    concept IUNICODECanUnambiguouslyConvertTo = IUNICODECanUnambiguouslyConvertFrom<T> and not Configuration::IAnyOf<remove_cv_t<T>, ASCII, Latin1>;
    static_assert (IUNICODECanUnambiguouslyConvertTo<char8_t>);
    static_assert (IUNICODECanUnambiguouslyConvertTo<char16_t>);
    static_assert (IUNICODECanUnambiguouslyConvertTo<char32_t>);
    static_assert (IUNICODECanUnambiguouslyConvertTo<wchar_t>);
    //static_assert (IUNICODECanUnambiguouslyConvertTo<Character>); true but not defined yet, so cannot assert here
    static_assert (not IUNICODECanUnambiguouslyConvertTo<ASCII>);
    static_assert (not IUNICODECanUnambiguouslyConvertTo<Latin1>);

    /**
     *  \note Satisfies Concepts:
     *      o   static_assert (regular<Character>);
     * 
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   static_assert (totally_ordered<Character>)
     *      o   Character::EqualsComparer and Character::ThreeWayComparer provided with construction parameters to allow case insensitive compares 
     */
    class [[nodiscard]] Character {
    public:
        /**
         *  Default constructor produces a zero (null) character.
         *  Constructor with char32_t always produces a valid character.
         * 
         *  The overloads check for a valid character code-point and throw if given invalid data.
         *   - The overload taking a single char (ASCII) will throw if arg is not ASCII
         *   - The overload taking a single char16_t will throw if given a code-point not valid on its own (surrogate without pair)
         *   - The overload taking two char16_t surrogate pairs, may throw if given invalid code-points
         *   - The overload taking wchar_t will treat it as char16_t or char32_t constructor, depending on sizeof (wchar_t)
         * 
         *  To avoid checking, cast 'c' to char32_t, as any code-point will be considered valid (so no need to check).
         */
        constexpr Character () noexcept;
        constexpr Character (const Character&) noexcept = default;
        constexpr Character (Character&&) noexcept      = default;
        constexpr Character (ASCII c);
        constexpr Character (Latin1 c) noexcept;
        constexpr Character (char16_t c);
        constexpr Character (char16_t hiSurrogate, char16_t lowSurrogate);
        constexpr Character (char32_t c) noexcept;
        constexpr Character (wchar_t c) noexcept (sizeof (wchar_t) == 4);

    public:
        constexpr Character& operator= (const Character&) noexcept = default;
        constexpr Character& operator= (Character&&) noexcept      = default;

    public:
        /**
         *  \req IsASCII()
         */
        nonvirtual ASCII GetAsciiCode () const noexcept;

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
         * \brief /0 overload return the character as a char32_t  (or on systems where wchar_t is large enuf, as wchar_t)
         *        /1 overload return the character as a span<T> - for any IUNICODECodePoint - storing actual data in provided stack buffer.
         * 
         *  \note Before Stroika v3, As<> () always supported wchar_t - now only if its same as char32_t.
         */
        template <typename T>
        constexpr T As () const noexcept
            requires (same_as<T, char32_t> or (sizeof (wchar_t) == sizeof (char32_t) and same_as<T, wchar_t>));
        template <IUNICODECodePoint T>
        nonvirtual span<const T> As (Memory::StackBuffer<T>* buf) const;

    private:
        template <IUNICODECodePoint T>
        nonvirtual void AsHelper_ (Memory::StackBuffer<T>* buf) const;

    public:
        /**
         *  \brief Return true iff the given character (or all in span) is (are) in the ascii range [0..0x7f]
         * 
         *  \note unlike other uses of CHAR_T in other methods in this class, even if CHAR_T=ASCII
         *        the code still loops and checks the range of characters. This is because ASCII == char
         *        and you need some way to check a bunch of 'char' elements and see if they are ascii.
         */
        constexpr bool IsASCII () const noexcept;
        template <IPossibleCharacterRepresentation CHAR_T>
        static constexpr bool IsASCII (CHAR_T c) noexcept;
        template <IPossibleCharacterRepresentation CHAR_T>
        static constexpr bool IsASCII (span<const CHAR_T> s) noexcept;

    public:
        /**
         *  \brief if not IsASCII (arg) throw RuntimeException...
         */
        template <IPossibleCharacterRepresentation CHAR_T>
        static constexpr void CheckASCII (span<const CHAR_T> s);
        template <IPossibleCharacterRepresentation CHAR_T>
        static constexpr void CheckASCII (span<CHAR_T> s);

    public:
        /**
         *  \brief Return true iff the given character (or all in span) is (are) in the ascii/iso-latin range [0..0xff]
         * 
         *  This refers to ASCII OR https://en.wikipedia.org/wiki/Latin-1_Supplement, so any UNICODE character code point
         *  less than U+00FF.
         * 
         *  \note this pays close attention to the CHAR_T, and checks differently (especially for
         *        sizeof(CHAR_T)==1. If the type is ASCII or Latin1, there is nothing
         *        to check, and so this just returns true. For CHAR_T==char8_t, we walk the sequence of characters
         *        and verify carefully that the encoded characters all will fit in the ISO-Latin1 range (<= 256).
         * 
         *  @see Latin1
         * 
         */
        constexpr bool IsLatin1 () const noexcept;
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static constexpr bool IsLatin1 (span<const CHAR_T> s) noexcept;

    public:
        /**
         *  \see IsASCIIOrLatin1
         */
        enum class ASCIIOrLatin1Result {
            eNone,
            eASCII,
            eLatin1
        };

    public:
        /**
         *  Combines check for IsASCII and IsLatin1 in one call (performance). Returns flag indicating
         *  most specific possible answer for the entire span. So if all characters ascii, that's returned.
         *  If not, but all characters latin1, that's returned. Else returned none.
         * 
         *  \note, if CHAR_T == Latin1 or ASCII, then this will NEVER return none. Its equivalent to
         *         IsASCII. If CHAR_T==ASCII. we do like IsASCII(): and actually check the bytes in the
         *         ASCII change, despite the ASCII designation (rationale in IsASCII).
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static constexpr ASCIIOrLatin1Result IsASCIIOrLatin1 (span<const CHAR_T> s) noexcept;

    public:
        /**
         *  \brief if not IsLatin1 (arg) throw RuntimeException...
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
        static void CheckLatin1 (span<const CHAR_T> s);
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T>
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
         *       ISO 30112 defines POSIX space characters as UNICODE characters 
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
         *  ISO 30112 defines POSIX control characters as UNICODE characters U+0000..U+001F, 
         *  U+007F..U+009F, U+2028, and U+2029 (UNICODE classes Cc, Zl, and Zp)
         */
        constexpr bool IsControl () const noexcept;

    public:
        /**
         *      Note that this does NOT modify the character in place but returns the new desired
         * character.
         *
         *      It is not necessary to first check
         * if the argument character is uppercase or alphabetic. ToLowerCase () just returns the
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
         * if the argument character is lowercase or alphabetic. ToUpperCase () just returns the
         * original character if there is no sensible conversion.
         * 
         *  \todo @todo See https://www.open-std.org/JTC1/SC35/WG5/docs/30112d10.pdf
         */
        nonvirtual Character ToUpperCase () const noexcept;

    public:
        /**
         * Convert String losslessly into a standard C++ type.
         * If this source contains any invalid ASCII characters, this returns false, and otherwise true (with set into).
         * 
         *  \req into->empty ()
         * 
         *  Supported Types (RESULT_T):
         *      o   Memory::StackBuffer<ASCII>
         *      o   string
         *      o   u8string
         */
        template <typename RESULT_T = string, IPossibleCharacterRepresentation CHAR_T>
        static bool AsASCIIQuietly (span<const CHAR_T> fromS, RESULT_T* into)
            requires requires (RESULT_T* into) {
                {
                    into->empty ()
                } -> same_as<bool>;
                {
                    into->push_back (ASCII{0})
                };
            };

    public:
        /**
         * See https://en.wikipedia.org/wiki/Universal_Character_Set_characters#Surrogates
         * 
         *  \note - would be nice to use DiscreteRange for these, but hard todo given deadly embrace.
         */
        static constexpr char16_t kUNICODESurrogate_High_Start{0xD800}; // was UNI_SUR_HIGH_START
        static constexpr char16_t kUNICODESurrogate_High_End{0xDBFF};   // was UNI_SUR_HIGH_END
        static constexpr char16_t kUNICODESurrogate_Low_Start{0xDC00};  // was UNI_SUR_LOW_START
        static constexpr char16_t kUNICODESurrogate_Low_End{0xDFFF};    // was UNI_SUR_LOW_END

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
         *  \req IsSurrogatePair
         *  returns the high/low pseudo-characters of the character
         */
        constexpr pair<char16_t, char16_t> GetSurrogatePair () const;

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
         *  utility to compare an array of characters, like strcmp (), except with param saying if case sensitive or insensitive.
         *
         *  \todo   Consider if this should be somehow packaged with Character::ThreeWayComparer?
         */
        template <IUNICODECanUnambiguouslyConvertFrom CHAR_T, size_t E1, size_t E2>
        static constexpr strong_ordering Compare (span<const CHAR_T, E1> lhs, span<const CHAR_T, E2> rhs, CompareOptions co) noexcept;

    public:
        [[deprecated ("Since Stroika 3.0d1, use span based Compare")]] static strong_ordering
        Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co) noexcept
        {
            return Compare (span{lhsStart, lhsEnd}, span{rhsStart, rhsEnd}, co);
        }

    private:
        char32_t fCharacterCode_;
    };
    static_assert (totally_ordered<Character>);

    /**
     *  Like equal_to<Character> but allow optional case insensitive compares
     */
    struct Character::EqualsComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eEquals> {
        /**
         *  optional CompareOptions to CTOR allows for case insensitive compares
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
    struct Character::ThreeWayComparer : Common::ComparisonRelationDeclarationBase<Common::ComparisonRelationType::eThreeWayCompare> {
        /**
         *  optional CompareOptions to CTOR allows for case insensitive compares
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
