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
#include <string_view>
#include <type_traits>
#include <utility>
#include <version>

/**
 *  \brief qStroika_Foundation_Common_EnumNameText_DefineStaticString_ - the compiler has std::define_static_string (C++26,
 *         P3491), so a long EnumNameText can point at a copy of any text (private: an implementation choice, not an option)
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
#endif

#include "Stroika/Foundation/Common/Common.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   https://github.com/SophistSolutions/Stroika/issues/685 (STK-549) - static_cast usage confusion (ToInt, ToEnum).
 *
 *      @todo   https://github.com/SophistSolutions/Stroika/issues/1284 - enum names: names for an enum with no
 *              Stroika_Define_Enum_Bounds; C++26 reflection (P2996) and annotations (P3394) behind the same interface.
 */

namespace Stroika::Foundation::Common {

    /**
     *  \brief  Increment the given enumeration safely, without a bunch of casts.
     *
     *      \pre    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \pre    e >= typename ENUM::eSTART and e < typename ENUM::eEND
     */
    template <typename ENUM>
    constexpr ENUM Inc (ENUM e);

    /**
     *  \brief  Cast the given enum to an int (like static_cast<int>()) - but check range.
     *
     *      \pre    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \pre    e >= typename ENUM::eSTART and e < typename ENUM::eEND
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
     *              a, b, c
     *              Stroika_Define_Enum_Bounds(a, c)
     *          };
     *          unsigned int    eltsWithPriority[GetDistanceSpanned<Priority> ()];
     *      \endcode
     *
     */
    template <typename ENUM>
    constexpr make_unsigned_t<underlying_type_t<ENUM>> GetDistanceSpanned (ENUM e);

    /**
     *  \brief  Cast the given int to the given ENUM type - (like static_cast<int>()) - but check range.
     *
     *      \pre    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \pre    e >= typename ENUM::eSTART and e < typename ENUM::eEND
     *
     *  This function is handy since class enum's cannot be automatically promoted to integers.
     */
    template <typename ENUM>
    constexpr ENUM ToEnum (underlying_type_t<ENUM> e);

    /**
     *  \brief  offset of given enum from ENUM::eSTART
     *
     *      \pre    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \pre    e >= typename ENUM::eSTART and e < typename ENUM::eEND
     *
     *  @todo   See if there is some better way for this.
     */
    template <typename ENUM>
    constexpr make_unsigned_t<underlying_type_t<ENUM>> OffsetFromStart (ENUM e);

    /**
     *  \def Stroika_Define_Enum_Bounds
     *
     *      Define meta information on enums using standardized names, so you can generically
     *      write things like:
     *          for (auto i = X::eSTART; i != X::eEND; i = Inc (i));
     */
#define Stroika_Define_Enum_Bounds(FIRST_ITEM, LAST_ITEM)                                                                                  \
    eSTART = FIRST_ITEM, eEND = LAST_ITEM + 1, eLAST = LAST_ITEM, eCOUNT = eEND - eSTART,

    /**
     *  Check if T is an enum class, with Stroika_Define_Enum_Bounds called on it to specify bounds.
     */
    template <typename T>
    concept IBoundedEnum = is_enum_v<T> && requires (T t) {
        T::eSTART;
        T::eEND;
        T::eLAST;
        T::eCOUNT;
    };

    /**
     *  \brief An enumerator's name: UTF-8 - held in place if short, else pointing to text that lasts as long as the program
     *
     *  Written "..." (ASCII only: a char literal's bytes depend on the compiler's execution character set - cp1252 for MSVC
     *  without /utf-8), u8"..." (any text), or L"..." (UTF-16 or UTF-32, as wchar_t is - transcoded; so tables written for
     *  Stroika v3.0d25 and before work as written).
     *
     *  A name of at most kMaxInlineBytes (64) bytes is held in place - nearly all are. A longer one, by where it comes from:
     *      o   reflected (an enumerator's identifier, through its naming rule): works, any length - points at an exactly-sized
     *          copy of the identifier
     *      o   written u8"...": works, any length - points at the literal itself
     *      o   written "..." or L"...": fails the build (EnumNameText_TooLongToHoldInPlace_WriteItU8SoItIsPointedAt_ - write it
     *          u8"..."), except where the compiler has std::define_static_string (C++26), which copies it to point at
     *      o   made at run time: a Require failure - such a name is never pointed at
     *
     *  A "..." name that is not ASCII fails the same way: the build, at compile time; a Require, at run time.
     *
     *  Only the names themselves end up in the program: a table's L"..." text, and the compiler's function-signature text a
     *  reflected name is cut from, are read at compile time only - the program holds just each name's UTF-8.
     */
    class EnumNameText {
    public:
        /**
         *  \brief a name this long or shorter is held in place; a longer one points (see above)
         *
         *  Why 64: it is well above any real name - Stroika's run to about 34 bytes, its applications' to 26 (as of v3.0d26) - so
         *  the names that cannot point all but always fit: one written "..." or L"..." (without std::define_static_string), and
         *  one made at run time. It is the same on every compiler, so whether a name can be made never depends on which one
         *  builds it. The price is size: every name takes sizeof (EnumNameText) - 80 bytes on a typical 64-bit compiler (these
         *  64, a pointer and a length) - however short it is; each 8 bytes less here would save 8 per name.
         *
         *  \see EnumNames, "Size in the program"
         */
        static constexpr size_t kMaxInlineBytes = 64;

    public:
        constexpr EnumNameText () = default;
        template <size_t N>
        constexpr EnumNameText (const char (&s)[N]);
        template <size_t N>
        constexpr EnumNameText (const char8_t (&s)[N]); ///< \brief UTF-8, any text - a long one, at compile time, pointed at
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
         *  \brief a name pointing at s, not copying it, however long
         *
         *  \pre s lasts as long as the program - a string literal, or a static constexpr array
         */
        static constexpr EnumNameText FromStatic (u8string_view s);

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
        constexpr bool operator== (const EnumNameText& rhs) const;

    private:
        // s as UTF-8: held in place if short enough, else (at compile time) a copy std::define_static_string makes, if it can
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

    /**
     *  \brief The entry of a table written for Stroika v3.0d25 and before: an enumerator and its name, L"..."
     *
     *  \see EnumNames::BasicArrayInitializer
     */
    template <typename ENUM_TYPE>
    using EnumName = pair<ENUM_TYPE, const wchar_t*>;

    /**
     *  \brief An enum's enumerators and their names - for ToString, serialization (ObjectVariantMapper, ObjectReader), and
     *  looking a value up by its name
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
     *      Only an enum whose names are used - its kDefaultNames read at run time, by ToString, ObjectVariantMapper,
     *      ObjectReader or directly - has them in the program: one read-only constant, kCapacity x sizeof (Entry) +
     *      sizeof (size_t) bytes, whatever the names' lengths. On a typical 64-bit compiler an Entry is 88 bytes (the enum,
     *      padded to 8, and an 80 byte EnumNameText), so an enum of 6 enumerators costs about 536 bytes. A name longer than
     *      EnumNameText::kMaxInlineBytes adds its own bytes, pointed at. Nothing else is kept: not the compiler's signature
     *      text a reflected name comes from, nor a table's L"..." text. (A table written for Stroika v3.0d25 and before cost
     *      16 bytes an entry, plus its L"..." text - (length + 1) x sizeof (wchar_t): 2 on Windows, 4 elsewhere.) Compilers
     *      may lay these out differently, or fold identical constants together - the figures are a guide, not a promise.
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
         *  \brief the shape of a table written for Stroika v3.0d25 and before: k{{{ {E::eA, L"A"}, ... }}}
         */
        using BasicArrayInitializer = array<EnumName<ENUM_TYPE>, kCapacity>;

    public:
        /**
         *  \pre no value twice, nor two values with one name
         */
        constexpr EnumNames () = default;
        constexpr EnumNames (const BasicArrayInitializer& init);
        constexpr EnumNames (initializer_list<Entry> names);

    public:
        /**
         *  \brief each enumerator from eSTART to eLAST, named by ENUM_TYPE's naming rule (IdentifierNaming, unless an EnumNamingRule
         *  declaration beside the enum picks another), then by its EnumeratorName, if one is declared beside the enum
         *
         *  The identifiers are read from the compiler's text for a function template's signature - each value from eSTART to
         *  eLAST is tried; one that two enumerators share is named as the first (so eSTART and eLAST never are), and one that no
         *  enumerator has is skipped. An identifier longer than EnumNameText::kMaxInlineBytes is pointed at, in an exactly-sized
         *  copy of its own - so any length works. All at compile time: about 1 ms per value, in each file that uses the names.
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

    /**
     *  \brief ENUM_TYPE's default names, kDefaultNames<ENUM_TYPE>, as an object: DefaultNames<ENUM_TYPE>{}.GetName (e)
     *
     *  k is only for tables written for Stroika v3.0d25 and before, which specialize it - such a table is used as it is:
     *      \code
     *          namespace Stroika::Foundation::Common {
     *              template <>
     *              constexpr EnumNames<Priority> DefaultNames<Priority>::k{{{{Priority::eDebug, L"Debug"}, {Priority::eInfo, L"Info"}}}};
     *          }
     *      \endcode
     *  Otherwise k is never defined: read kDefaultNames<ENUM_TYPE>.
     *
     *  (Computing the default in k itself is not portable: a k initialized in the class, gcc lets nothing specialize - "duplicate
     *  initialization"; a k declared here and defined constexpr outside it, clang (to 22 at least) calls "unknown" in a constant
     *  expression.)
     */
    template <typename ENUM_TYPE>
    struct DefaultNames {
        static_assert (IBoundedEnum<ENUM_TYPE>, "DefaultNames needs an enum with Stroika_Define_Enum_Bounds");

        // undefined unless a table specializes it - and kDefaultNames asks which (for an enum in an anonymous namespace, clang
        // calls asking a use)
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wundefined-internal\"");
        [[deprecated ("Since Stroika v3.0d26 - name the enum by EnumNamingRule and EnumeratorName beside it (see kDefaultNames); read "
                      "kDefaultNames")]] static const EnumNames<ENUM_TYPE> k;
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wundefined-internal\"");

        constexpr operator const EnumNames<ENUM_TYPE>&() const; ///< \brief kDefaultNames<ENUM_TYPE>

        constexpr typename EnumNames<ENUM_TYPE>::const_iterator begin () const;
        constexpr typename EnumNames<ENUM_TYPE>::const_iterator end () const;
        constexpr size_t                                        size () const;
        constexpr optional<u8string_view>                       PeekName (ENUM_TYPE e) const;
        constexpr u8string_view                                 GetName (ENUM_TYPE e) const;
        template <typename NAME>
        constexpr optional<ENUM_TYPE> PeekValue (NAME&& name) const;
        template <typename... ARGS>
        constexpr ENUM_TYPE GetValue (ARGS&&... args) const;
    };

    namespace Private_ {
        template <IBoundedEnum ENUM_TYPE>
        constexpr EnumNames<ENUM_TYPE> ComputeDefaultNames_ ();
    }

    /**
     *  \brief ENUM_TYPE's default names: what ToString, ObjectVariantMapper and ObjectReader use, unless told otherwise
     *
     *  Made, in order:
     *      o   from a table written for Stroika v3.0d25 and before, specializing DefaultNames<ENUM_TYPE>::k, as it is
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
