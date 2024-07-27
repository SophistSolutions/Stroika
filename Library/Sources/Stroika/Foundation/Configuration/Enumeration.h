/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_h_
#define _Stroika_Foundation_Configuration_Enumeration_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <array>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "Stroika/Foundation/Configuration/Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 * TODO:
 *
 *      @todo   http://stroika-bugs.sophists.com/browse/STK-549 - RequireItemsOrderedByEnumValue and
 *              static_cast usage confusion.
 *
 *      @todo   I tried using EnumNames<> as an alias for initialzer_list, but then I couldnt add the
 *              GetNames () method. I tried subclassing, but then I ran into lifetime issues. I tried aggregation,
 *              but this has the same lifetime issues with subclassing std::initializer_list. In the end I had
 *              to copy. That maybe a poor tradeoff. The only reason for not using aliases was to add
 *              the Peek/GetName methods, but those could have been global functions? Hmmm.
 */

namespace Stroika::Foundation::Configuration {

#ifndef qCANNOT_FIGURE_OUT_HOW_TO_INIT_STD_ARRAY_FROM_STD_INITIALIZER_
#define qCANNOT_FIGURE_OUT_HOW_TO_INIT_STD_ARRAY_FROM_STD_INITIALIZER_ 1
#endif

    /**
     *  \brief  Increment the given enumeration safely, without a bunch of casts.
     *
     *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
     */
    template <typename ENUM>
    constexpr ENUM Inc (ENUM e);

    /**
     *  \brief  Cast the given enum to an int (like static_cast<int>()) - but check range.
     *
     *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
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
     *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
     *
     *  This function is handy since class enum's cannot be automatically promoted to integers.
     */
    template <typename ENUM>
    constexpr ENUM ToEnum (underlying_type_t<ENUM> e);

    /**
     *  \brief  offset of given enum from ENUM::eSTART
     *
     *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
     *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
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
     */
    template <typename ENUM_TYPE>
    using EnumName = pair<ENUM_TYPE, const wchar_t*>;

    /**
     *  The purpose of this class is to capture meta-information about enumerations, principally the purpose
     *  of serialization, and or debugging printouts of data (e.g. DbgTrace).
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *          This class fully supports multiple readers, but it is not designed to support update while ongoing access
     *          is going on.
     *
     *  \note   Important requirement on type ENUM_TYPE - it must use the Stroika_Define_Enum_Bounds() macro
     *          or otherwise define eSTART,eEND,eCOUNT,eLAST
     *
     *  \note   Important requirement on type ENUM_TYPE - it must be 'densely packed' - that is - have no gaps.
     *          it need not start at any particular number, but the number of enumerators must equal eCOUNT,
     *          and a name and enumerator must be provided for each value from eSTART to eEND.
     *          (future versions MAY lift that requirement).
     *          \req RequireItemsOrderedByEnumValue_
     */
    template <typename ENUM_TYPE>
    class EnumNames {
    public:
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        static_assert (is_enum_v<decltype (ENUM_TYPE::eCOUNT)>,
                       "Missing eCOUNT - typically Use Stroika_Define_Enum_Bounds inside the enum");
        static_assert (is_enum_v<decltype (ENUM_TYPE::eSTART)>,
                       "Missing eSTART - typically Use Stroika_Define_Enum_Bounds inside the enum");
        static_assert (is_enum_v<decltype (ENUM_TYPE::eEND)>, "Missing eEND - typically Use Stroika_Define_Enum_Bounds inside the enum");

    private:
        using EnumNamesHolderType_ = array<EnumName<ENUM_TYPE>, static_cast<size_t> (ENUM_TYPE::eCOUNT)>;

    public:
        using BasicArrayInitializer = array<EnumName<ENUM_TYPE>, static_cast<size_t> (ENUM_TYPE::eCOUNT)>;

        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

    public:
        using const_iterator = typename EnumNamesHolderType_::const_iterator;

    public:
        using value_type = typename EnumNamesHolderType_::value_type;

    public:
        /**
         */
#if qCANNOT_FIGURE_OUT_HOW_TO_INIT_STD_ARRAY_FROM_STD_INITIALIZER_
        EnumNames () = default; //hack to allow CTOR EnumNames (const initializer_list<EnumName<ENUM_TYPE>>& origEnumNames)
#else
        EnumNames () = delete;
#endif

        constexpr EnumNames (EnumNames&& src)      = default;
        constexpr EnumNames (const EnumNames& src) = default;
        constexpr EnumNames (const BasicArrayInitializer& init);
        EnumNames (const initializer_list<EnumName<ENUM_TYPE>>& origEnumNames);
        template <size_t N>
        constexpr EnumNames (const EnumName<ENUM_TYPE> origEnumNames[N]);

    public:
        /**
         */
        nonvirtual EnumNames& operator= (EnumNames&& rhs)      = default;
        nonvirtual EnumNames& operator= (const EnumNames& rhs) = default;

    public:
        /**
         */
        explicit operator initializer_list<EnumName<ENUM_TYPE>> () const;

    public:
        /**
         */
        nonvirtual const_iterator begin () const;

    public:
        /**
         */
        nonvirtual const_iterator end () const;

    public:
        /**
         */
        nonvirtual constexpr size_t size () const;

    public:
        /**
         *  The argument 'e' must be a valid enumerator entry. If its 'eEND' this function will return nullptr;
         *  Otherwise it will return a valid const wchar_t* pointer to that enumerators name.
         *
         *  @see GetName ();
         */
        nonvirtual constexpr const wchar_t* PeekName (ENUM_TYPE e) const;

    public:
        /**
         *  The argument 'e' must be a valid enumerator entry between eSTART, and eEND (not including eEND).
         *  It returns a pointer to a valid const wchar_t* string of the name of the enumerator.
         *
         *  \req ENUM_TYPE::eStart <= e and e < ENUM_TYPE::eEnd
         *
         *  @see PeekName ();
         */
        nonvirtual const wchar_t* GetName (ENUM_TYPE e) const;

    public:
        /**
         *  Returns nullopt if not found.
         */
        nonvirtual optional<ENUM_TYPE> PeekValue (const wchar_t* name) const;

    public:
        /**
         *  The /1 overload requires the name is present, and asserts if not found.
         *  The /2 overload throws the argument exceptio iff the name is not found.
         */
        nonvirtual ENUM_TYPE GetValue (const wchar_t* name) const;
        template <typename NOT_FOUND_EXCEPTION>
        nonvirtual ENUM_TYPE GetValue (const wchar_t* name, const NOT_FOUND_EXCEPTION& notFoundException) const;

    private:
        nonvirtual constexpr void RequireItemsOrderedByEnumValue_ () const;

    private:
        // SHOULD BE ABLE TO USE CONST HERE qCANNOT_FIGURE_OUT_HOW_TO_INIT_STD_ARRAY_FROM_STD_INITIALIZER_
        // but then one (std::initializer_list) CTOR doesn't compile
        //const EnumNamesHolderType_   fEnumNames_;
        EnumNamesHolderType_ fEnumNames_;
    };

    /**
     *  Use DefaultNames<> to register the EnumNames<> mapping in a common place that can be used by
     *  other templates to automatically lookup enum names.
     *
     *  \par Example Usage
     *      \code
     *          enum class Priority {
     *              a, b, c
     *              Stroika_Define_Enum_Bounds(a, c)
     *          };
     *
     *          // this template specialization must be located in the Stroika::Configuration namespace
     *          namespace Stroika::Foundation::Configuration {
     *              template<>
     *              constexpr EnumNames<Priority> DefaultNames<Priority>::k{{{
     *                  {Priority::a, L"Debug"},
     *                  {Priority::b, L"Info"},
     *                  {Priority::eNotice, L"Notice"},
     *              }}};
     *          }
     *
     *          // Then use
     *          const wchar_t* aStr = DefaultNames<Priority>{}.PeekName (Priority::a);
     *          Priority p = DefaultNames<Priority>{}.GetValue (L"invalid", Execution::Exception<> (L"OutOfRange"));   // this will throw - out of range
     *      \endcode
     *
     */
    template <typename ENUM_TYPE>
    struct DefaultNames : EnumNames<ENUM_TYPE> {
        static const EnumNames<ENUM_TYPE> k;
        constexpr DefaultNames ();
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Enumeration.inl"

#endif /*_Stroika_Foundation_Configuration_Enumeration_h_*/
