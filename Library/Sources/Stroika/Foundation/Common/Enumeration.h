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
#include <limits>
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
 */

namespace Stroika::Foundation::Common {

    /**
     *  \brief An enum's bounds: its first and its last value - every value from one to the other is in range, an enumerator or
     *  not
     *
     *  An enum gets its bounds from a function, EnumBoundsOf, declared in the enum's namespace - or, for an enum in a class, as
     *  a friend in the class - and found by argument-dependent lookup:
     *      \code
     *          enum class Color { eRed, eGreen, eBlue };
     *          constexpr Common::EnumBounds<Color> EnumBoundsOf (Color) { return {Color::eRed, Color::eBlue}; }
     *
     *          struct Light {
     *              enum class State { eOff, eDim, eOn };
     *              friend constexpr Common::EnumBounds<State> EnumBoundsOf (State) { return {State::eOff, State::eOn}; }
     *          };
     *      \endcode
     *  Write it in the enum's own header, right after the enum, so everything that uses the enum sees it. Read the bounds with
     *  kEnumBounds, never by calling EnumBoundsOf. An enum declared in a function cannot have one - a function cannot be defined
     *  in a function - so declare such an enum at namespace scope.
     *
     *  An enum with bounds is an IBoundedEnum: it has names (kDefaultNames - for ToString and ObjectVariantMapper), it can be
     *  iterated (Traversal::DiscreteRange), and ToInt and ToEnum check its values:
     *      \code
     *          for (Color c : Traversal::DiscreteRange<Color>::FullRange ().Elements ()) { ... }
     *          array<int, Common::kEnumBounds<Color>.Count ()> perColor{};
     *      \endcode
     *
     *  \note Satisfies Concepts:
     *      o   regular<EnumBounds<ENUM>> - and it is structural, so it can be a template argument
     */
    template <typename ENUM>
    struct EnumBounds {
        static_assert (is_enum_v<ENUM>, "EnumBounds are an enum's");

        ENUM fFirst{};
        ENUM fLast{};

        /**
         *  \brief how many values from fFirst to fLast, enumerators or not
         */
        constexpr size_t Count () const;

        /**
         *  \brief is e from fFirst to fLast?
         */
        constexpr bool Contains (ENUM e) const;

        constexpr bool operator== (const EnumBounds&) const = default;
    };

    /**
     *  \brief Where an enum's bounds come from: declare one for your enum, after it (see EnumBounds) - not this one, which is here
     *         only to document the name
     *
     *      \code
     *          enum class Color { eRed, eGreen, eBlue };
     *          constexpr Common::EnumBounds<Color> EnumBoundsOf (Color) { return {Color::eRed, Color::eBlue}; }
     *      \endcode
     *
     *  To use this for an enum, define an overload of EnumBoundsOf, typically just after the enum (for an enum in a class, a
     *  friend in the class - see EnumBounds).
     *
     *  kEnumBounds then finds it by argument-dependent lookup, and prefers it to this one, always. This one is deleted, so an
     *  enum without one of its own has no bounds. Do not specialize it.
     *
     *  EnumNamingRule's docs say how the lookup works, and why this one is declared at all.
     */
    void EnumBoundsOf (auto) = delete;

    namespace Private_ {
        // kEnumBounds of an enum with none
        struct NoEnumBounds_ {};
        template <typename ENUM>
        constexpr auto ComputeEnumBounds_ ();
    }

    /**
     *  \brief ENUM's bounds, wherever they are declared - read them here
     *
     *  kEnumBounds<E> is E's bounds, from:
     *      o   an EnumBoundsOf function, after the enum - the usual way (see EnumBounds)
     *      o   Stroika_Define_Enum_Bounds, in the enum (DEPRECATED)
     *      o   an explicit specialization of kEnumBounds itself (see below)
     *  An enum with two of these, which disagree, fails the build. An enum with none has no bounds: kEnumBounds<E> is not an
     *  EnumBounds<E>, and IBoundedEnum<E> is false.
     *
     *  \par Why this, and not EnumBoundsOf (E{})
     *      An enum's bounds can come from more than one place, and code reading them should not care which. And one of those
     *      places is not a function: argument-dependent lookup finds EnumBoundsOf only in the enum's own namespace, and a program
     *      may not add declarations to namespace std - so an enum of the standard library's gets bounds by a specialization of
     *      kEnumBounds, in Stroika's namespace:
     *      \code
     *          namespace Stroika::Foundation::Common {
     *              template <>
     *              inline constexpr auto kEnumBounds<std::memory_order> =
     *                  EnumBounds<std::memory_order>{std::memory_order::relaxed, std::memory_order::seq_cst};
     *          }
     *      \endcode
     *      Write such a specialization in one header, included before anything reads that enum's bounds: a file that reads
     *      kEnumBounds<E> without having seen it gets the unspecialized value - different bounds in different files, which no
     *      compiler is required to catch. Any other enum - even a third-party library's - is better given an EnumBoundsOf.
     *
     *  \par An enum declared in a function
     *      can have neither: a function cannot be defined in a function, and a specialization cannot name the enum. Declare it
     *      at namespace scope instead. (Once Stroika requires C++26, kEnumBounds can find any enum's bounds by reflection - a
     *      function's enums too.)
     *
     *  \note Under C++26, reflection (std::meta::enumerators_of) could give any enum bounds. Stroika does not use it for that,
     *        so whether an enum has bounds - and so names - never depends on the compiler.
     */
    template <typename ENUM>
    inline constexpr auto kEnumBounds = Private_::ComputeEnumBounds_<ENUM> ();

    /**
     *  \brief Does T have bounds (kEnumBounds) - an enum with an EnumBoundsOf function, say?
     *
     *  \see EnumBounds
     */
    template <typename T>
    concept IBoundedEnum = is_enum_v<T> and same_as<remove_cv_t<decltype (kEnumBounds<T>)>, EnumBounds<T>>;

    namespace Private_ {
        template <typename T>
        [[deprecated (
            "Since Stroika v3.0d26 - in place of Stroika_Define_Enum_Bounds, declare after the enum: constexpr "
            "Common::EnumBounds<E> EnumBoundsOf (E) { return {first, last}; } (see EnumBounds), and read kEnumBounds<E>")]] constexpr T
        Stroika_Define_Enum_Bounds_ (T t)
        {
            return t;
        }
    }

    /**
     *  \def Stroika_Define_Enum_Bounds
     *
     *  \brief DEPRECATED (since Stroika v3.0d26) - written as an enum's last item, it gave the enum bounds: enumerators eSTART,
     *  eEND (one past the last), eLAST and eCOUNT. Declare an EnumBoundsOf function after the enum instead (see EnumBounds).
     *
     *  Writing it warns, and so does reading eSTART, eEND or eLAST: E::eSTART is kEnumBounds<E>.fFirst, E::eLAST
     *  kEnumBounds<E>.fLast, E::eCOUNT kEnumBounds<E>.Count (), and a loop to E::eEND is over
     *  Traversal::DiscreteRange<E>::FullRange ().Elements (). (eCOUNT carries the warning where the macro is written - a
     *  compiler does not warn inside a deprecated declaration - so reading it does not warn.)
     */
    // clang-format off
#define Stroika_Define_Enum_Bounds(FIRST_ITEM, LAST_ITEM)                                                                      \
    eSTART [[deprecated ("Since Stroika v3.0d26 - kEnumBounds<E>.fFirst")]] = FIRST_ITEM,                                      \
    eEND [[deprecated ("Since Stroika v3.0d26 - loop over Traversal::DiscreteRange<E>::FullRange ().Elements ()")]] = LAST_ITEM + 1, \
    eLAST [[deprecated ("Since Stroika v3.0d26 - kEnumBounds<E>.fLast")]] = LAST_ITEM,                                         \
    eCOUNT = ::Stroika::Foundation::Common::Private_::Stroika_Define_Enum_Bounds_ (LAST_ITEM + 1 - FIRST_ITEM),
    // clang-format on

    /**
     *  \brief  Increment the given enumeration safely, without a bunch of casts.
     *
     *      \pre    for an IBoundedEnum, e is in kEnumBounds<ENUM> - so the result is at most one past its last (checked, by ToEnum)
     */
    template <typename ENUM>
    constexpr ENUM Inc (ENUM e);

    /**
     *  \brief  e as its underlying type - as C++23's std::to_underlying - checking, for an IBoundedEnum, that e is in range
     *
     *      \pre    for an IBoundedEnum, e is in kEnumBounds<ENUM>, or one past its last (where a loop ends)
     *
     *  This function is handy since class enum's cannot be automatically promoted to integers.
     *
     *  \note This function could have been called Ord () - to be more like Pascal.
     */
    template <typename ENUM>
    constexpr underlying_type_t<ENUM> ToInt (ENUM e);

    /**
     *  \brief  return the distance spanned by an enum, e.g. for use in an array - kEnumBounds<ENUM>.Count ()
     *
     *  \par Example Usage
     *      \code
     *          enum class Priority { eLow, eMedium, eHigh };
     *          constexpr Common::EnumBounds<Priority> EnumBoundsOf (Priority) { return {Priority::eLow, Priority::eHigh}; }
     *          unsigned int eltsWithPriority[GetDistanceSpanned<Priority> ()];
     *      \endcode
     */
    template <IBoundedEnum ENUM>
    constexpr make_unsigned_t<underlying_type_t<ENUM>> GetDistanceSpanned ();

    /**
     *  \brief  e as an ENUM - as static_cast<ENUM> (e) - checking, for an IBoundedEnum, that it is in range
     *
     *      \pre    for an IBoundedEnum, e is in kEnumBounds<ENUM>, or one past its last (where a loop ends)
     *
     *  This function is handy since class enum's cannot be automatically promoted to integers.
     */
    template <typename ENUM>
    constexpr ENUM ToEnum (underlying_type_t<ENUM> e);

    /**
     *  \brief  offset of given enum from kEnumBounds<ENUM>.fFirst
     *
     *      \pre    e is in kEnumBounds<ENUM>, or one past its last (checked, by ToInt) - whose offset is Count ()
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
     *  An enum picks its rule by an EnumNamingRule declaration beside it, never defined - only its return type is used (see
     *  EnumNamingRule: how Stroika finds it, and the form for an enum in a class):
     *      \code
     *          enum class Color { eRed, eGreen, eBlue };
     *          constexpr Common::EnumBounds<Color> EnumBoundsOf (Color) { return {Color::eRed, Color::eBlue}; }
     *          Common::StroikaNaming EnumNamingRule (Color);       // names "Red", "Green", "Blue"
     *      \endcode
     *  An enum with no EnumNamingRule is named by IdentifierNaming.
     *  EnumeratorName, beside the enum too, may then rename any enumerator.
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

    /**
     *  \brief How an enum picks its naming rule: declare a function of this name for your enum, beside it - not this one, which
     *         is here only to document the name, and so that an enum without one is named by IdentifierNaming
     *
     *  Yours is a different function from this one - an overload of the name, in your enum's namespace - taking your enum, and
     *  returning the rule (an IEnumNamingRule). Declare it, and do not define it: Stroika only reads its return type (decltype),
     *  never calls it, so it needs no body:
     *      \code
     *          namespace MyApp {
     *              enum class Color { eRed, eGreen, eBlue };
     *              constexpr Common::EnumBounds<Color> EnumBoundsOf (Color) { return {Color::eRed, Color::eBlue}; }
     *              Common::StroikaNaming EnumNamingRule (Color); // names "Red", "Green", "Blue"
     *          }
     *      \endcode
     *  For an enum in a class, declare it in the class, as a friend - argument-dependent lookup finds a class's friends through
     *  an enum that is a member of the class:
     *      \code
     *          struct Light {
     *              enum class State { eOff, eDim, eOn };
     *              friend constexpr Common::EnumBounds<State> EnumBoundsOf (State) { return {State::eOff, State::eOn}; }
     *              friend Common::StroikaNaming EnumNamingRule (State); // names "Off", "Dim", "On"
     *          };
     *      \endcode
     *
     *  How Stroika finds yours: its code, in this namespace, asks whether EnumNamingRule (e) - e of your enum type - names a
     *  function. Argument-dependent lookup adds the functions of that name beside the enum (in its namespace, and the friends
     *  of its class) to this one, and yours is not a template, so it is preferred, always. With no declaration of yours, the
     *  call finds only this one, which is deleted: the call does not compile, which Stroika reads as "no rule", and the enum is
     *  named by IdentifierNaming. And because this one is declared here, ordinary lookup stops here: no other function named
     *  EnumNamingRule, in an enclosing namespace, is found by mistake.
     *
     *  EnumBoundsOf and EnumeratorName are found the same way, for the same reasons - except that yours are defined (constexpr),
     *  since Stroika calls them, at compile time. Never specialize any of the three: write your own beside the enum.
     */
    void EnumNamingRule (auto) = delete;

    /**
     *  \brief How an enum renames single enumerators, after its naming rule: define one for your enum, beside it (see
     *         kDefaultNames) - not this one, which is here only to document the name
     *
     *      \code
     *          constexpr Common::EnumNameText EnumeratorName (Color c, Common::EnumNameText byRule)
     *          {
     *              return c == Color::eGreen ? Common::EnumNameText{"Verde"} : byRule; // "Red", "Verde", "Blue"
     *          }
     *      \endcode
     *
     *  For an enum in a class, define it in the class, as a friend:
     *      \code
     *          struct Light {
     *              enum class State { eOff, eDim, eOn };
     *              friend constexpr Common::EnumBounds<State> EnumBoundsOf (State) { return {State::eOff, State::eOn}; }
     *              friend constexpr Common::EnumNameText EnumeratorName (State s, Common::EnumNameText byRule)
     *              {
     *                  return s == State::eDim ? Common::EnumNameText{"Dimmed"} : byRule; // "eOff", "Dimmed", "eOn"
     *              }
     *          };
     *      \endcode
     *
     *  To use this for an enum, define an overload of EnumeratorName, typically just after the enum (for an enum in a class, a
     *  friend in the class, as above).
     *
     *  Stroika then finds it by argument-dependent lookup, and prefers it to this one, always. This one is deleted, so an enum
     *  without one of its own keeps its naming rule's names. Do not specialize it.
     *
     *  EnumNamingRule's docs say how the lookup works, and why this one is declared at all.
     */
    void EnumeratorName (auto, auto) = delete;

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
     *          };                                                      // "Red", "Vert", "Blue"
     *          constexpr Common::EnumBounds<Color> EnumBoundsOf (Color) { return {Color::eRed, Color::eBlue}; }
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
     *  \note Why ENUM_TYPE must be an IBoundedEnum (see EnumBounds): an EnumNames holds a slot for each value in its bounds
     *        (kCapacity), and Reflected () finds the enumerators by trying each value in them - C++20 has no other way to find
     *        them. C++26 reflection does (std::meta::enumerators_of gives an enum's enumerators, and so their count), so neither
     *        would need the bounds there.
     *
     *  \note   Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename ENUM_TYPE>
    class EnumNames {
        // not a constraint on ENUM_TYPE: code written for Stroika v3.0d25 and before declares template <typename ENUM_TYPE> class EnumNames;
        static_assert (IBoundedEnum<ENUM_TYPE>,
                       "EnumNames needs an enum with bounds - an EnumBoundsOf function beside it (see EnumBounds)");

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
         *  \brief the most names one EnumNames holds: one for each value in kEnumBounds<ENUM_TYPE>
         */
        DISABLE_COMPILER_MSC_WARNING_START (4996); // an enum deprecated whole - as WaitableEvent::ResetType - has its names too
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        static constexpr size_t kCapacity = kEnumBounds<ENUM_TYPE>.Count ();
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
         *  \brief each enumerator in kEnumBounds<ENUM_TYPE>, named by ENUM_TYPE's naming rule (IdentifierNaming, unless an EnumNamingRule
         *  declaration beside the enum picks another), then by its EnumeratorName, if one is declared beside the enum
         *
         *  The identifiers are read from the compiler's text for a function template's signature - each value in the bounds is
         *  tried; one that two enumerators share is named as the first (so an alias, as eDEFAULT, never is), and one that no
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
     *              enum class Color { eRed, eGreen, eBlue };
     *              constexpr Common::EnumBounds<Color> EnumBoundsOf (Color) { return {Color::eRed, Color::eBlue}; }
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
