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
 *      @todo   REDO THIS SO ALWAYS USES char32_t - NOT wchar_t!!!! But DON'T DO until I have a STRING class
 *              implementation based on UTF-8, to minimize the performance costs...
 *              https://stroika.atlassian.net/browse/STK-534
 *
 *      @todo   Use UTFConvert code directly to properly handle constructors from various codepoint types
 *
 *      @todo   Biggest thing todo is to work out 'surrogates' - and whether or not they are needed
 *              (depending on the size of wchar_t - which right now - we PRESUME is the same as the size
 *              of Character.
 *
 *      @todo   ToLower ('GERMAN ES-ZETT' or 'SHARP S') returns two esses ('ss') - and we return a single chararcter.
 *              We COULD change return value, or simply document that issue here and define ToLower() of STRING todo
 *              the right thing for queer cases like this, and use this API for the most common cases.
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
        is_same_v < remove_cv_t<T>,
    char8_t > or is_same_v<remove_cv_t<T>, char16_t> or is_same_v<remove_cv_t<T>, char32_t>;

    /**
     *  \brief check if T is Character_IsBasicUnicodeCodePoint or wchar_t (any basic code-point class)
     */
    template <typename T>
    concept Character_IsUnicodeCodePoint = Character_IsBasicUnicodeCodePoint<T> or is_same_v < remove_cv_t<T>,
    wchar_t > ;

    /**
     *  \brief check if T is char8_t, char16_t, char32_t (Character_IsBasicUnicodeCodePoint) or wchar_t
     */
    template <typename T>
    concept Character_IsUnicodeCodePointOrPlainChar = Character_IsUnicodeCodePoint<T> or is_same_v < remove_cv_t<T>,
    char > ;

    /// @TODO LOSE OrPlainChar busines sand simplfiy below???? Only if we ASSERT 'char' implies ASCIII, and check that with assertions
    // so assert each char <= 127.
    class Character;

    /**
     *  \brief char8_t, char16_t, char32_t, wchar_t, Character - something that can be always safely interpreted as a UNICODE Character.
     * 
     *  \note all these types are <= 4 bytes (size of char32_t)
     */
    template <typename T>
    concept Character_SafelyCompatible = Character_IsUnicodeCodePoint<T> or is_same_v < remove_cv_t<T>,
    Character > ;

    /**
     *  \brief Something that can be reasonably converted into a Unicode Character object (Character_SafelyCompatible<T> or T==char)
     * 
     *  \note all these types are <= 4 bytes (size of char32_t)
     * 
     *  \note for many apis, when Character_Compatible==char, the API will require the characters are ASCII (but see each API for details
     *        on this point).
     */
    template <typename T>
    concept Character_Compatible = Character_SafelyCompatible<T> or is_same_v < remove_cv_t<T>,
    char > ;

    /**
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *      o   Character::EqualsComparer and Character::ThreeWayComparer provided with construction parameters to allow case insensitive compares 
     */
    class Character {
    public:
        /**
         */
        constexpr Character () noexcept;
        constexpr Character (char c) noexcept;
        constexpr Character (char16_t c) noexcept;
        constexpr Character (char32_t c) noexcept; // @todo decide how to handle surrogates
        constexpr Character (wchar_t wc) noexcept;

    public:
        /**
         *  \req IsASCII()
         */
        nonvirtual char GetAsciiCode () const noexcept;

    public:
        // @todo deprecate this and replace wtih GetCodePoint<CHAR_T> -> optional<CHAR_T> with charT required to be uncide-code-point concept
        // return has-value if convertible
        nonvirtual wchar_t GetCharacterCode () const noexcept;

    public:
        /**
         *  Explicit cuz creates too many ambiguities with things like c == '\0' where conversions can go both ways.
         */
        explicit operator char32_t () const noexcept
        {
            return GetCharacterCode ();
        }

    public:
        /*
         * @todo    NOT SURE WE WANT THIS FOR wchar_t etc - maybe just get rid of this!!! TRICKYYY;
         *          IF we go with design based on char32_t - then thats all we can ever safely return.
         *          We need diff API to return up to 2 wchar_t's!!!
         */
        template <typename T>
        nonvirtual T As () const noexcept
            requires (is_same_v<T, char32_t> or is_same_v<T, wchar_t>);

    public:
        nonvirtual bool IsASCII () const noexcept;

    public:
        nonvirtual bool IsWhitespace () const noexcept;

    public:
        nonvirtual bool IsDigit () const noexcept;

    public:
        nonvirtual bool IsHexDigit () const noexcept;

    public:
        nonvirtual bool IsAlphabetic () const noexcept;

    public:
        // Checks if the given character is uppper case. Can be called on any character.
        // Returns false if not alphabetic
        nonvirtual bool IsUpperCase () const noexcept;

    public:
        // Checks if the given character is lower case. Can be called on any character.
        // Returns false if not alphabetic
        nonvirtual bool IsLowerCase () const noexcept;

    public:
        nonvirtual bool IsAlphaNumeric () const noexcept;

    public:
        nonvirtual bool IsPunctuation () const noexcept;

    public:
        nonvirtual bool IsControl () const noexcept;

    public:
        /**
         *      Note that this does NOT modify the character in place but returns the new desired
         * character.
         *
         *      It is not necessary to first check
         * if the argument character is uppercase or alpabetic. ToLowerCase () just returns the
         * original character if there is no sensible conversion.
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
         */
        nonvirtual Character ToUpperCase () const noexcept;

    public:
        /**
         */
        template <Character_Compatible CHAR_T>
        static bool IsASCII (span<const CHAR_T> s) noexcept;

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
         */
        nonvirtual strong_ordering operator<=> (const Character&) const noexcept = default;

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
        [[deprecated ("Since Stroika 3.0d1, use span based Compare")]] static strong_ordering Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co) noexcept
        {
            return Compare (span{lhsStart, lhsEnd}, span{rhsStart, rhsEnd}, co);
        }

    private:
        wchar_t fCharacterCode_;
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

    /// NOT GOOD IDEA/NOT GOOD PRACTICE - BUT AT LEAST MODULARIZE THE BAD PRACTICE
    /// SO I CAN SEARCH FOR IT AND FIX IT WHEN I HAVE A GOOD IDEA HOW.
    //
    // ASSUME sizeof(wchar_t) same as sizeof (Character) everwhere so the cast between
    // them is safe
    inline const wchar_t* CVT_CHARACTER_2_wchar_t (const Character* c)
    {
        return reinterpret_cast<const wchar_t*> (c);
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Character.inl"

#endif /*__Character__*/
