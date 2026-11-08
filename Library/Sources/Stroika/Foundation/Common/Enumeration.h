/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Common_Enumeration_h_
#define _Stroika_Foundation_Common_Enumeration_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <version>

/**
 *  \brief qStroika_Foundation_Common_EnumNameText_DefineStaticString_ - the compiler has std::define_static_string (C++26,
 *         P3491), which Named (an annotation's name) needs (private: an implementation choice, not an option)
 */
#ifndef qStroika_Foundation_Common_EnumNameText_DefineStaticString_
#if defined(__cpp_lib_define_static) and __cpp_lib_define_static >= 202506L
#define qStroika_Foundation_Common_EnumNameText_DefineStaticString_ 1
#else
#define qStroika_Foundation_Common_EnumNameText_DefineStaticString_ 0
#endif
#endif
#if qStroika_Foundation_Common_EnumNameText_DefineStaticString_
#include <meta>
#include <string>
#include <vector>
#endif

/**
 *  \brief qStroika_Foundation_Common_EnumNames_Annotations_ - the compiler has C++26 reflection (P2996), and with it
 *         annotations (P3394), so an enum's naming rule and an enumerator's name can be annotations on them (private: code
 *         writing them tests __cpp_impl_reflection, the compiler's own macro for them)
 */
#ifndef qStroika_Foundation_Common_EnumNames_Annotations_
#if defined(__cpp_impl_reflection) and __cpp_impl_reflection >= 202506L and qStroika_Foundation_Common_EnumNameText_DefineStaticString_
#define qStroika_Foundation_Common_EnumNames_Annotations_ 1
#else
#define qStroika_Foundation_Common_EnumNames_Annotations_ 0
#endif
#endif

#include "Stroika/Foundation/Common/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   https://github.com/SophistSolutions/Stroika/issues/1284 - enum names: names for an enum with no
 *              Stroika_Define_Enum_Bounds.
 */

namespace Stroika::Foundation::Common {

    /**
     *  \def Stroika_Define_Enum_Bounds
     *
     *  \brief Makes an enum an IBoundedEnum: written as its last item, it adds eSTART, eEND (one past the last), eLAST and
     *  eCOUNT, for the values from FIRST_ITEM to LAST_ITEM
     *
     *  \par Example Usage
     *      \code
     *          enum class Color {
     *              eRed,
     *              eGreen,
     *              eBlue,
     *              Stroika_Define_Enum_Bounds (eRed, eBlue)
     *          };
     *          for (Color c = Color::eSTART; c != Color::eEND; c = Inc (c)) {
     *              ...
     *          }
     *      \endcode
     *
     *  What reads the bounds requires an IBoundedEnum: GetDistanceSpanned, OffsetFromStart, and the enum's names (kDefaultNames -
     *  so ToString and ObjectVariantMapper).
     *
     *  \note Stroika_Define_Enum_Bounds will be deprecated in the Stroika release that requires C++26, whose reflection (P2996)
     *        finds an enum's enumerators itself. Until then, it is what makes an enum bounded.
     *
     *  \see IBoundedEnum
     */
#define Stroika_Define_Enum_Bounds(FIRST_ITEM, LAST_ITEM)                                                                                  \
    eSTART = FIRST_ITEM, eEND = LAST_ITEM + 1, eLAST = LAST_ITEM, eCOUNT = eEND - eSTART,

    /**
     *  Check if T is an enum class, with Stroika_Define_Enum_Bounds called on it to specify bounds.
     *
     *  \see Stroika_Define_Enum_Bounds
     */
    template <typename T>
    concept IBoundedEnum = is_enum_v<T> && requires (T t) {
        T::eSTART;
        T::eEND;
        T::eLAST;
        T::eCOUNT;
    };

    /**
     *  \brief  Increment the given enumeration safely, without a bunch of casts.
     *
     *      \pre    for an IBoundedEnum: ENUM::eSTART <= e and e < ENUM::eEND - so the result is at most eEND (checked, by ToEnum)
     */
    template <typename ENUM>
    constexpr ENUM Inc (ENUM e);

    /**
     *  \brief  Cast the given enum to an int (like static_cast<int>()) - but check range.
     *
     *      \pre    for an IBoundedEnum: ENUM::eSTART <= e and e <= ENUM::eEND (eEND too: a loop ends on it)
     *
     *  This function is handy since class enum's cannot be automatically promoted to integers.
     *
     *  \note This function could have been called Ord () - to be more like Pascal.
     *
     *  @todo   See if there is some better way for this.
     */
    template <typename ENUM>
    constexpr underlying_type_t<ENUM> ToInt (ENUM e);

    /**
     *  \brief  return the distance spanned by an enum, e.g. for use in an array
     *
     *  \par Example Usage
     *      \code
     *          enum class Priority {
     *              eLow,
     *              eMedium,
     *              eHigh,
     *              Stroika_Define_Enum_Bounds (eLow, eHigh)
     *          };
     *          unsigned int eltsWithPriority[GetDistanceSpanned<Priority> ()];
     *      \endcode
     */
    template <IBoundedEnum ENUM>
    constexpr make_unsigned_t<underlying_type_t<ENUM>> GetDistanceSpanned ();

    /**
     *  \brief  Cast the given int to the given ENUM type - (like static_cast<int>()) - but check range.
     *
     *      \pre    for an IBoundedEnum:  ENUM::eSTART <= e and e <= ENUM::eEND
     *
     *  This function is handy since class enum's cannot be automatically promoted to integers.
     */
    template <typename ENUM>
    constexpr ENUM ToEnum (underlying_type_t<ENUM> e);

    /**
     *  \brief  offset of given enum from ENUM::eSTART
     *
     *      \pre    ENUM::eSTART <= e and e <= ENUM::eEND (checked, by ToInt) - eEND's offset is eCOUNT
     *
     *  @todo   See if there is some better way for this.
     */
    template <IBoundedEnum ENUM>
    constexpr make_unsigned_t<underlying_type_t<ENUM>> OffsetFromStart (ENUM e);

    /**
     *  \brief An enumerator's name: UTF-8 - held in place if short, else pointing to text with application lifetime
     *
     *  Written "..." (ASCII only: a char literal's bytes depend on the compiler's execution character set - cp1252 for MSVC
     *  without /utf-8), u8"..." (any text), or L"..." (UTF-16 or UTF-32, as wchar_t is - transcoded; so tables written for
     *  Stroika v3.0d25 and before work as written).
     *
     *  \par Example Usage
     *      \code
     *          // naming one enumerator, beside the enum (see kDefaultNames) - where an application most often writes one
     *          constexpr Common::EnumNameText EnumeratorName (Color c, Common::EnumNameText byRule)
     *          {
     *              return c == Color::eGreen ? Common::EnumNameText{"Verde"} : byRule;
     *          }
     *          // a table of names, for another format: "...", u8"..." or L"..."
     *          constexpr Common::EnumNames<Color> kOther{{Color::eRed, "R"}, {Color::eGreen, u8"G"}, {Color::eBlue, L"B"}};
     *          // made at run time, from text with application lifetime: any length
     *          Common::EnumNameText n = Common::EnumNameText::FromStringConstant (u8"a name of any length"sv);
     *          u8string_view        v = n.View ();
     *      \endcode
     *
     *  \par Argument name requirements:
     *      Any text, of any length, works as a name:
     *          o   made automatically, by reflection - an enumerator's identifier, through its naming rule
     *          o   written u8"..." - a string literal, at compile time or at run time
     *          o   made by EnumNameText::FromStringConstant (s), at compile time or at run time - which requires s to have
     *              application lifetime (a string literal does)
     *      Any other name - written "..." or L"...", or made from text the program made - must be at most kMaxInlineBytes (64)
     *      bytes of UTF-8.
     *      A longer one:
     *          o   in a constant expression: it does not compile.
     *              The compiler's error is a somewhat confusingly named reference to a function whose name is the error
     *              message: EnumNameText_TooLongToHoldInPlace_WriteItU8SoItIsPointedAt_.
     *              If this happens, the workaround is simple: write it as u8"..." instead.
     *          o   if the EnumNameText is constructed at runtime, from text the program made (rare - copying an EnumNameText is
     *              always fine): Require() failure (so undefined in release builds)
     *      A "..." name must also be ASCII, or the same happens (the function named is EnumNameText_ACharNameMustBeASCII_WriteItU8_).
     *
     *  \par What a name costs
     *      A name of at most kMaxInlineBytes bytes is held in place: it takes nothing beyond the EnumNameText itself. A longer one
     *      points at its text - the literal itself, or an exactly-sized copy (of a reflected identifier, or of a C++26 Named
     *      annotation's text) - which takes its own bytes, once. EnumNames, "Size in the program", has the figures.
     *
     *  \par Copying
     *      Always safe, at any length, and cheap - a trivial copy of sizeof (EnumNameText) bytes: a short name's bytes are copied;
     *      a long one's pointer is, and what it points at has application lifetime, as every long name's text must.
     *
     *  \note Satisfies Concepts:
     *      o   regular<EnumNameText> - copyable, default constructible (an empty name), and equality comparable
     */
    class EnumNameText {
    public:
        /**
         *  \brief a name of at most this many bytes of UTF-8 is held in place (see "What a name costs", above)
         *
         *  Why 64: it is well above any real name - Stroika's run to about 30 bytes, 9 on average, its applications' to 26 (as of
         *  v3.0d26) - so the names that cannot point all but always fit: one written "..." or L"...", and one made from text the
         *  program made. It is the same on every compiler, so whether a name can be made never depends on which one builds it. The price:
         *  every name takes all 64 bytes, however short.
         *
         *  \see EnumNames, "Size in the program"
         */
        static constexpr size_t kMaxInlineBytes = 64;

    public:
        constexpr EnumNameText ()                    = default; ///< \brief an empty name - as EnumNames holds for a slot it has not filled
        constexpr EnumNameText (const EnumNameText&) = default; ///< \brief always safe, at any length: see "Copying", above
        template <size_t N>
        constexpr EnumNameText (const char (&s)[N]);
        template <size_t N>
        constexpr EnumNameText (const char8_t (&s)[N]); ///< \brief UTF-8, any text - a long one is pointed at (a literal)
        template <size_t N>
        constexpr EnumNameText (const wchar_t (&s)[N]);     ///< \brief UTF-16 or UTF-32, as wchar_t is - transcoded to UTF-8
        constexpr explicit EnumNameText (string_view s);    ///< \pre s is ASCII
        constexpr explicit EnumNameText (u8string_view s);  ///< \brief UTF-8, any text - copied, not pointed at
        constexpr explicit EnumNameText (wstring_view s);   ///< \brief UTF-16 or UTF-32, as wchar_t is - transcoded to UTF-8
        constexpr explicit EnumNameText (const wchar_t* s); ///< \brief UTF-16 or UTF-32, as wchar_t is - transcoded to UTF-8

    public:
        /**
         *  \brief the bytes, taken as UTF-8 unchecked (as a compiler writes an identifier) - copied
         */
        static constexpr EnumNameText FromUTF8 (string_view utf8);

    public:
        /**
         *  \brief s as a name: held in place if short enough, else pointing at s - not copying it, however long
         *
         *  \pre s has application lifetime (@see Design-Overview.md, "Application lifetime") - a string literal, or a static
         *       constexpr array - where it is pointed at; Debug builds check it at run time, where Stroika can tell
         */
        static constexpr EnumNameText FromStringConstant (u8string_view s);

    public:
        /**
         *  \brief the name, as UTF-8 - valid while this EnumNameText is (kDefaultNames' are, for the whole program)
         */
        constexpr u8string_view View () const;
        constexpr               operator u8string_view () const; ///< \brief View ()

    public:
        /**
         *  \brief the same text as s: ASCII (char), UTF-8 (char8_t), or UTF-16 or UTF-32 (wchar_t)
         */
        constexpr bool Equals (string_view s) const;
        constexpr bool Equals (u8string_view s) const; ///< \brief the same text as s
        constexpr bool Equals (wstring_view s) const;  ///< \brief the same text as s

    public:
        constexpr EnumNameText& operator= (const EnumNameText&) = default; ///< \brief always safe, at any length: see "Copying", above

    public:
        constexpr bool operator== (const EnumNameText& rhs) const;

    private:
        // s as UTF-8, held in place - a longer one fails: the build, at compile time; a Require, at run time
        constexpr void SetCopyOf_ (u8string_view s);
        // the code point at s[i], advancing i past it - nullopt if s is not valid UTF-16 (or UTF-32) there
        static constexpr optional<char32_t> NextCodePoint_ (wstring_view s, size_t& i);
        // c as UTF-8, into out (1 to 4 bytes): how many
        static constexpr size_t EncodeUTF8_ (char32_t c, char8_t* out);

    private:
        const char8_t*                  fPointsTo_{nullptr}; // a long name: where it is
        size_t                          fLength_{0};
        array<char8_t, kMaxInlineBytes> fInline_{}; // a short name
    };
    static_assert (regular<EnumNameText> and is_trivially_copyable_v<EnumNameText>); // see Satisfies Concepts, and Copying

    /**
     *  \brief A naming rule: how an enum's enumerators are named from their identifiers, as written ("eWithCase"), at compile
     *  time - a type with a static constexpr EnumNameText Apply (const EnumNameText& identifier)
     *
     *  An enum picks its rule by a declaration beside it, found by argument-dependent lookup and never defined - only its
     *  return type is used (for an enum in a class, a friend declaration in the class):
     *      \code
     *          enum class Color { eRed, eGreen, eBlue, Stroika_Define_Enum_Bounds (eRed, eBlue) };
     *          Common::StroikaNaming EnumNamingRule (Color);       // names "Red", "Green", "Blue"
     *      \endcode
     *  An enum with none is named by IdentifierNaming. EnumeratorName, beside the enum too, may then rename any enumerator.
     *
     *  \see kDefaultNames
     */
    template <typename T>
    concept IEnumNamingRule = requires (const EnumNameText& identifier) {
        { T::Apply (identifier) } -> convertible_to<EnumNameText>;
    };

    /**
     *  \brief A naming rule, and the default: an enumerator's name is its identifier, as written - eWithCase is "eWithCase"
     *
     *  \note Satisfies Concepts:
     *      o   IEnumNamingRule<IdentifierNaming>
     */
    struct IdentifierNaming {
        static constexpr EnumNameText Apply (const EnumNameText& identifier);
    };
    static_assert (IEnumNamingRule<IdentifierNaming>); // see Satisfies Concepts

    /**
     *  \brief A naming rule: Stroika's convention - the identifier without its leading e (an e before an upper case letter or
     *  a digit) - eWithCase is "WithCase"; any other identifier is as it is
     *
     *  \note Satisfies Concepts:
     *      o   IEnumNamingRule<StroikaNaming>
     */
    struct StroikaNaming {
        static constexpr EnumNameText Apply (const EnumNameText& identifier);
    };
    static_assert (IEnumNamingRule<StroikaNaming>); // see Satisfies Concepts

#if qStroika_Foundation_Common_EnumNames_Annotations_
    /**
     *  \brief An enumerator's name, as a C++26 annotation (P3394) on it - the same as an EnumeratorName case for it - where the
     *  compiler has annotations (__cpp_impl_reflection)
     *
     *  On the enum, a naming rule's value is an annotation too - the same as an EnumNamingRule declaration:
     *      \code
     *          enum class [[= Common::StroikaNaming{}]] Color {
     *              eRed,
     *              eGreen [[= Common::Named{"Vert"}]],
     *              eBlue,
     *              Stroika_Define_Enum_Bounds (eRed, eBlue)
     *          };                                                      // "Red", "Vert", "Blue"
     *      \endcode
     *  An enum picks its rule one way: by annotation, or by EnumNamingRule.
     *
     *  Written "..." (ASCII) or u8"..." (any text), any length. An annotation's value must be of a structural type - its
     *  members all public - so a name is a Named here, not an EnumNameText.
     */
    struct Named {
        template <size_t N>
        consteval Named (const char (&s)[N]);
        template <size_t N>
        consteval Named (const char8_t (&s)[N]); ///< \brief UTF-8, any text

        /**
         *  \brief the name, as UTF-8 - with application lifetime
         */
        constexpr u8string_view View () const;

        // public only because an annotation's type must be structural: read View ()
        const char8_t* fText;
        size_t         fLength;
    };
#endif

    /**
     *  \brief An enum's enumerators and their names - for ToString, serialization (ObjectVariantMapper, ObjectReader), and
     *  looking a value up by its name.
     *
     *  This aggregates an array of Entry (pair<ENUM_TYPE, EnumNameText>).
     *
     *  The usual one is the enum's default, kDefaultNames<ENUM_TYPE>. Another can be made for one use - another format's names -
     *  from a list, or as a copy of another with changes (With).
     *
     *  \par Example Usage
     *      \code
     *          constexpr EnumNames<Color> kXMLNames = kDefaultNames<Color>.With (Color::eRed, "rouge");
     *          constexpr EnumNames<Color> kOther{{Color::eRed, "R"}, {Color::eGreen, u8"G"}, {Color::eBlue, L"B"}};
     *      \endcode
     *
     *  \par Size in the program
     *      Only an enum whose names are used at run time - by ToString, ObjectVariantMapper, ObjectReader, or read directly -
     *      has them in the program: its kDefaultNames, one read-only constant. Its size depends on how many enumerators the
     *      enum has, not on how long their names are:
     *          kCapacity x sizeof (Entry) + sizeof (size_t)
     *      On a typical 64-bit compiler an Entry is 88 bytes: the enum (4 bytes, padded to 8) and an EnumNameText (80 bytes -
     *      its 64 in-place bytes, a pointer and a length). So:
     *          o   a 15 character name costs 88 bytes - and so does a 3 or a 60 character one
     *          o   an enum of 6 enumerators costs 6 x 88 + 8 = 536 bytes
     *          o   a name longer than EnumNameText::kMaxInlineBytes costs 88 bytes plus its own text, pointed at
     *      Nothing else is kept: not the compiler's signature text a reflected name is cut from, nor a table's L"..." text.
     *
     *      For comparison, a table written for Stroika v3.0d25 and before cost 16 bytes an entry (the enum and a pointer),
     *      plus its L"..." text, (length + 1) x sizeof (wchar_t): for a 15 character name, 16 + 16 x 4 = 80 bytes on Linux
     *      and macOS, 16 + 16 x 2 = 48 on Windows.
     *
     *      Compilers may lay these out differently, or fold identical constants together - the figures are a guide, not a
     *      promise.
     *
     *  \note Why ENUM_TYPE must be an IBoundedEnum (Stroika_Define_Enum_Bounds): an EnumNames holds a slot for each value from
     *        eSTART to eLAST (kCapacity), and Reflected () finds the enumerators by trying each value in that range - C++20 has no
     *        other way to find them. C++26 reflection does (std::meta::enumerators_of gives an enum's enumerators, and so their
     *        count), so neither needs the bounds there: this requirement goes when Stroika requires C++26, as
     *        Stroika_Define_Enum_Bounds does.
     *
     *  \note   Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename ENUM_TYPE>
    class EnumNames {
        // not a constraint on ENUM_TYPE: code written for Stroika v3.0d25 and before declares template <typename ENUM_TYPE> class EnumNames;
        static_assert (IBoundedEnum<ENUM_TYPE>, "EnumNames needs an enum with Stroika_Define_Enum_Bounds");

    public:
        /**
         *  \brief a value and its name, as written to make an EnumNames: {E::eRed, "Red"} - "...", u8"..." or L"..."
         */
        using Entry = pair<ENUM_TYPE, EnumNameText>;

    public:
        /**
         *  \brief a value and its name, as iterating an EnumNames gives them - the name a view, as GetName gives it (so a
         *  pair converts to pair<ENUM_TYPE, String>, as for a Containers::Bijection<ENUM_TYPE, String>)
         */
        using value_type = pair<ENUM_TYPE, u8string_view>;

    public:
        class const_iterator;

    public:
        /**
         *  \brief the most names one EnumNames holds: one for each value from eSTART to eLAST
         */
        DISABLE_COMPILER_MSC_WARNING_START (4996); // an enum deprecated whole - as WaitableEvent::ResetType - has its names too
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        static constexpr size_t kCapacity = static_cast<size_t> (ENUM_TYPE::eCOUNT);
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    public:
        /**
         *  \brief DEPRECATED (since Stroika v3.0d26) - the shape of a table written for Stroika v3.0d25 and before:
         *  k{{{ {E::eA, L"A"}, ... }}} - written {{E::eA, "A"}, ...} now (initializer_list<Entry>)
         */
        using BasicArrayInitializer [[deprecated ("Since Stroika v3.0d26 - write an EnumNames as {{E::eA, \"A\"}, ...}, or name the enum "
                                                  "by EnumNamingRule and EnumeratorName (see kDefaultNames)")]] =
            array<pair<ENUM_TYPE, const wchar_t*>, kCapacity>;

    public:
        /**
         *  \pre no value twice, nor two values with one name
         */
        constexpr EnumNames () = default;
        constexpr EnumNames (initializer_list<Entry> names);
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        [[deprecated ("Since Stroika v3.0d26 - write an EnumNames as {{E::eA, \"A\"}, ...}, or name the enum by EnumNamingRule and "
                      "EnumeratorName (see kDefaultNames)")]] constexpr EnumNames (const BasicArrayInitializer& init);
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    public:
        /**
         *  \brief each enumerator from eSTART to eLAST, named by ENUM_TYPE's naming rule (IdentifierNaming, unless an EnumNamingRule
         *  declaration beside the enum picks another), then by its EnumeratorName, if one is declared beside the enum
         *
         *  The identifiers are read from the compiler's text for a function template's signature - each value from eSTART to
         *  eLAST is tried; one that two enumerators share is named as the first (so eSTART and eLAST never are), and one that no
         *  enumerator has is skipped. An identifier longer than EnumNameText::kMaxInlineBytes is pointed at, in an exactly-sized
         *  copy of its own - so any length works. All at compile time.
         */
        static constexpr EnumNames Reflected ();

    public:
        constexpr const_iterator begin () const;
        constexpr const_iterator end () const;
        constexpr size_t         size () const;

    public:
        /**
         *  \brief e's name, as UTF-8 - valid while this EnumNames is (kDefaultNames' are, for the whole program)
         */
        constexpr optional<u8string_view> PeekName (ENUM_TYPE e) const;

    public:
        /**
         *  \pre PeekName (e)
         */
        constexpr u8string_view GetName (ENUM_TYPE e) const;

    public:
        /**
         *  \brief the value with this name, if any: name ASCII (char), UTF-8 (char8_t), or UTF-16 or UTF-32 (wchar_t)
         */
        constexpr optional<ENUM_TYPE> PeekValue (string_view name) const;
        constexpr optional<ENUM_TYPE> PeekValue (u8string_view name) const; ///< \brief the value with this name, if any
        constexpr optional<ENUM_TYPE> PeekValue (wstring_view name) const;  ///< \brief the value with this name, if any

    public:
        /**
         *  \brief PeekValue (name), which the /1 overloads require be there, and the /2 overloads throw notFoundException if not
         */
        constexpr ENUM_TYPE GetValue (string_view name) const;
        constexpr ENUM_TYPE GetValue (u8string_view name) const; ///< \pre PeekValue (name)
        constexpr ENUM_TYPE GetValue (wstring_view name) const;  ///< \pre PeekValue (name)
        template <typename NOT_FOUND_EXCEPTION>
        ENUM_TYPE GetValue (string_view name, const NOT_FOUND_EXCEPTION& notFoundException) const; ///< \brief throws notFoundException if not found
        template <typename NOT_FOUND_EXCEPTION>
        ENUM_TYPE GetValue (u8string_view name, const NOT_FOUND_EXCEPTION& notFoundException) const; ///< \brief throws notFoundException if not found
        template <typename NOT_FOUND_EXCEPTION>
        ENUM_TYPE GetValue (wstring_view name, const NOT_FOUND_EXCEPTION& notFoundException) const; ///< \brief throws notFoundException if not found

    public:
        /**
         *  \brief a copy, with e named name - another list, for another format's names
         *
         *  \pre e has a name here, and no other value is named name
         */
        constexpr EnumNames With (ENUM_TYPE e, const EnumNameText& name) const;

    private:
        array<Entry, kCapacity> fNames_{};
        size_t                  fSize_{0};
    };

    /**
     *  \brief iterates an EnumNames' values and names: each a value_type - pair<ENUM_TYPE, u8string_view> - made as it is read
     */
    template <typename ENUM_TYPE>
    class EnumNames<ENUM_TYPE>::const_iterator {
    public:
        using iterator_concept  = forward_iterator_tag;
        using iterator_category = input_iterator_tag; // * gives a value, not a reference
        using value_type        = pair<ENUM_TYPE, u8string_view>;
        using difference_type   = ptrdiff_t;
        using reference         = value_type;
        using pointer           = void;

    public:
        constexpr const_iterator () = default;
        constexpr explicit const_iterator (const Entry* e);

    public:
        constexpr value_type      operator* () const;
        constexpr const_iterator& operator++ ();
        constexpr const_iterator  operator++ (int);
        constexpr bool            operator== (const const_iterator&) const = default;

    private:
        const Entry* fEntry_{nullptr};
    };

    namespace Private_ {
        template <IBoundedEnum ENUM_TYPE>
        constexpr EnumNames<ENUM_TYPE> ComputeDefaultNames_ ();
    }

    /**
     *  \brief ENUM_TYPE's default names: what ToString, ObjectVariantMapper and ObjectReader use, unless told otherwise
     *
     *  Made, in order:
     *      o   from a table written for Stroika v3.0d25 and before, specializing DefaultNames<ENUM_TYPE>::k (DEPRECATED), as it is
     *      o   otherwise as EnumNames<ENUM_TYPE>::Reflected () - so by the enum's naming rule, then its EnumeratorName:
     *          \code
     *              enum class Color { eRed, eGreen, eBlue, Stroika_Define_Enum_Bounds (eRed, eBlue) };
     *              Common::StroikaNaming EnumNamingRule (Color);                                   // "Red", "Green", "Blue" - and
     *              constexpr Common::EnumNameText EnumeratorName (Color c, Common::EnumNameText byRule)
     *              {
     *                  return c == Color::eGreen ? Common::EnumNameText{"Verde"} : byRule;      // "Red", "Verde", "Blue"
     *              }
     *          \endcode
     *  Declare these right after the enum - in its header - before anything uses its names.
     *
     *  \par Example Usage
     *      \code
     *          DbgTrace ("color={}"_f, Color::eGreen);                                         // color=Verde
     *          String          s = Characters::ToString (Color::eRed);                         // "Red"
     *          u8string_view   n = Common::kDefaultNames<Color>.GetName (Color::eBlue);        // u8"Blue"
     *          optional<Color> c = Common::kDefaultNames<Color>.PeekValue ("Verde");          // Color::eGreen
     *      \endcode
     *
     *  \note The names are made at compile time - kDefaultNames is a constant - so making them costs nothing at run time,
     *        and a static_assert can check one.
     */
    template <IBoundedEnum ENUM_TYPE>
    inline constexpr EnumNames<ENUM_TYPE> kDefaultNames = Private_::ComputeDefaultNames_<ENUM_TYPE> ();

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Enumeration.inl"

#endif /*_Stroika_Foundation_Common_Enumeration_h_*/
