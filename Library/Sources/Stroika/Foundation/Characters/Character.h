/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Character_h_
#define _Stroika_Foundation_Characters_Character_h_ 1

#include "../StroikaPreComp.h"

#if defined(__cpp_impl_three_way_comparison)
#include <compare>
#endif

#include "../Common/Compare.h"
#include "../Configuration/Enumeration.h"

/**
 * TODO:
 *
 *      @todo   REDO THIS SO ALWAYS USES char32_t - NOT wchar_t!!!! But DON'T DO until I have a STRING class
 *              implementation based on UTF-8, to minimize the performance costs...
 *              https://stroika.atlassian.net/browse/STK-534
 *
 *      @todo       KEY ISSUES TO DECIDE:
 *          o   Can we use a class with no loss of performance (or must we use typdef wchar_t Character)
 *
 *          o   How do we handle char16_t versus char32_t - Windows uses 16bit, UNIX 32-bit. UNCLEAR how to
 *              handle here. (LEANING TOWARDS FORCING USE OF 16bit char??) - maybe irrlevelnt if our STRING
 *              class internally stores stuff as utf8
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
     *  \note Comparisons:
     *        o Standard Stroika Comparison support (operator<=>,operator==, etc);
     *        o Character::EqualsComparer and Character::ThreeWayComparer provided with construction parameters to allow case insensitive compares 
     */
    class Character {
    public:
        /**
         */
        constexpr Character ();
        constexpr Character (char c);
        constexpr Character (char16_t c);
        constexpr Character (char32_t c); // @todo decide how to handle surrogates
        constexpr Character (wchar_t wc);

    public:
        /**
         *  \req IsASCII()
         */
        nonvirtual char GetAsciiCode () const;

    public:
        nonvirtual wchar_t GetCharacterCode () const;

    public:
        /*
         * @todo    NOT SURE WE WANT THIS FOR wchar_t etc - maybe just get rid of this!!! TRICKYYY;
         *          IF we go with design based on char32_t - then thats all we can ever safely return.
         *          We need diff API to return up to 2 wchar_t's!!!
         */
        template <typename T>
        nonvirtual T As () const;

    public:
        nonvirtual bool IsASCII () const;

    public:
        nonvirtual bool IsWhitespace () const;

    public:
        nonvirtual bool IsDigit () const;

    public:
        nonvirtual bool IsHexDigit () const;

    public:
        nonvirtual bool IsAlphabetic () const;

    public:
        // Checks if the given character is uppper case. Can be called on any character.
        // Returns false if not alphabetic
        nonvirtual bool IsUpperCase () const;

    public:
        // Checks if the given character is lower case. Can be called on any character.
        // Returns false if not alphabetic
        nonvirtual bool IsLowerCase () const;

    public:
        nonvirtual bool IsAlphaNumeric () const;

    public:
        nonvirtual bool IsPunctuation () const;

    public:
        nonvirtual bool IsControl () const;

    public:
        /**
         *      Note that this does NOT modify the character in place but returns the new desired
         * character.
         *
         *      It is not necessary to first check
         * if the argument character is uppercase or alpabetic. ToLowerCase () just returns the
         * original character if there is no sensible conversion.
         */
        nonvirtual Character ToLowerCase () const;

    public:
        /**
         *      Note that this does NOT modify the character in place but returns the new desired
         * character.
         *
         *      It is not necessary to first check
         * if the argument character is lowercase or alpabetic. ToUpperCase () just returns the
         * original character if there is no sensible conversion.
         */
        nonvirtual Character ToUpperCase () const;

#if __cpp_impl_three_way_comparison >= 201907
    public:
        /**
         */
        nonvirtual strong_ordering operator<=> (const Character&) const = default;
#endif

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
        static Common::strong_ordering Compare (const Character* lhsStart, const Character* lhsEnd, const Character* rhsStart, const Character* rhsEnd, CompareOptions co);

    private:
        wchar_t fCharacterCode_;
    };

    template <>
    wchar_t Character::As () const;
    template <>
    char32_t Character::As () const;

    /**
     *  Like equal_to<Character> but allow optional case insensitive compares
     */
    struct Character::EqualsComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals> {
        /**
         *  optional CompareOptions to CTOR allows for case insensative compares
         */
        constexpr EqualsComparer (Stroika::Foundation::Characters::CompareOptions co = Stroika::Foundation::Characters::CompareOptions::eWithCase);

        /**
         */
        constexpr bool operator() (Character lhs, Character rhs) const;

        Stroika::Foundation::Characters::CompareOptions fCompareOptions;
    };

    /**
     *  Like std::compare_three_way<Character,Character> but allow optional case insensitive compares
     */
    struct Character::ThreeWayComparer : Common::ComparisonRelationDeclaration<Common::ComparisonRelationType::eThreeWayCompare> {
        /**
         *  optional CompareOptions to CTOR allows for case insensative compares
         */
        constexpr ThreeWayComparer (Stroika::Foundation::Characters::CompareOptions co = Stroika::Foundation::Characters::CompareOptions::eWithCase);

        /**
         */
        nonvirtual auto operator() (Stroika::Foundation::Characters::Character lhs, Stroika::Foundation::Characters::Character rhs) const;

        Stroika::Foundation::Characters::CompareOptions fCompareOptions;
    };

#if __cpp_impl_three_way_comparison < 201907
    /**
     *  Basic operator overloads with the obivous meaning, and simply indirect to @Character::ThreeWayComparer{}
     */
    bool operator< (Character lhs, Character rhs);
    bool operator<= (Character lhs, Character rhs);
    bool operator== (Character lhs, Character rhs);
    bool operator!= (Character lhs, Character rhs);
    bool operator>= (Character lhs, Character rhs);
    bool operator> (Character lhs, Character rhs);
#endif

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

#if __cpp_lib_three_way_comparison < 201907L
namespace Stroika::Foundation::Common {
    // specialization not needed, but more efficient
    template <>
    struct compare_three_way<Characters::Character, Characters::Character> : Characters::Character::ThreeWayComparer {
    };
}
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Character.inl"

#endif /*__Character__*/
