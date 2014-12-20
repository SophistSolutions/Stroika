/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_h_
#define _Stroika_Foundation_Configuration_Enumeration_h_  1

#include    "../StroikaPreComp.h"

#include    <array>
#include    <type_traits>
#include    <utility>
#include    <vector>

#include    "Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   Make EnumNames<ENUM_TYPE>::PeekName (ENUM_TYPE e) constexpr;
 *
 *      @todo   I tried using EnumNames<> as an alias for initialzer_list, but then I couldnt add the
 *              GetNames () method. I tried subclassing, but then I ran into lifetime issues. I tried aggregation,
 *              but this has the same lifetime issues with subclassing std::initializer_list. In the end I had
 *              to copy. That maybe a poor tradeoff. The only reason for not using aliases was to add
 *              the Peek/GetName methods, but those could have been global functions? Hmmm.
 *
 *      @todo   Figure out why we need todo
 *              Configuration::EnumNames<FileAccessMode>::BasicArrayInitializer arg to CTOR - see
 *                  constexpr   Configuration::EnumNames<FileAccessMode>    Stroika_Enum_Names(FileAccessMode) {
 *                      Configuration::EnumNames<FileAccessMode>::BasicArrayInitializer {
 *                      {
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /**
             *  \brief  Increment the given enumeration safely, without a bunch of casts.
             *
             *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
             *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
             */
            template    <typename   ENUM>
            constexpr   ENUM    Inc (ENUM e);


            /**
             *  \brief  Cast the given enum to an int (like static_cast<int>()) - but check range.
             *
             *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
             *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
             *
             *  This function is handy since class enum's cannot be automatically promoted to integers.
             *
             *  @todo   See if there is some better way for this.
             */
            template    <typename   ENUM>
            constexpr   typename underlying_type<ENUM>::type    ToInt (ENUM e);


            /**
             *  \brief  Cast the given int to the given ENUM type - (like static_cast<int>()) - but check range.
             *
             *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
             *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
             *
             *  This function is handy since class enum's cannot be automatically promoted to integers.
             */
            template    <typename   ENUM>
            constexpr   ENUM    ToEnum (typename underlying_type<ENUM>::type e);


            /**
             *  \brief  offset of given enum from ENUM::eSTART
             *
             *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
             *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
             *
             *  @todo   See if there is some better way for this.
             */
            template    <typename   ENUM>
            constexpr   typename make_unsigned<typename underlying_type<ENUM>::type>::type  OffsetFromStart (ENUM e);
            template    <typename   ENUM>
            constexpr   ENUM                                                                OffsetFromStart (typename make_unsigned<typename underlying_type<ENUM>::type>::type offset);


            /**
             *  \def Stroika_Define_Enum_Bounds
             *
             *      Define meta information on enums using standardized names, so you can generically
             *      write things like:
             *          for (auto i = X::eSTART; i != X::eEND; i = Inc (i));
             */
#define Stroika_Define_Enum_Bounds(FIRST_ITEM,LAST_ITEM)\
    eSTART      =   FIRST_ITEM,\
                    eEND        =   LAST_ITEM + 1,\
                                    eLAST        =  LAST_ITEM,\
                                            eCOUNT      =   eEND - eSTART,


            /**
             *  \def Stroika_Enum_Names
             *
             *
             *  EXAMPLE USAGE:
             *      enum class Priority { a, b, c };
             *      const EnumNames<Priority>   Stroika_Enum_Names(Priority);
             *      const EnumNames<Logger::Priority>   Logger::Stroika_Enum_Names(Priority) = {
             *          { Priority::a, L"a" },
             *          { Priority::b, L"b" },
             *          { Priority::c, L"c" },
             *  };
             */
#define Stroika_Enum_Names(ENUMNAME)\
    ENUMNAME##_EnumNames


            /**
             */
            template <typename ENUM_TYPE>
            using   EnumName = pair<ENUM_TYPE, const wchar_t*>;


            /**
             *  The purpose of this class is to capture meta-information about enumerations, principally the purpose
             *  of serialization, and or debugging printouts of data (e.g. DbgTrace).
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#POD-Level-Thread-Safety">POD-Level-Thread-Safety</a>
             *          This class fully supports multiple readers, but it is not designed to support update while ongoing access
             *          is going on.
             */
            template <typename ENUM_TYPE>
            class   EnumNames {
            private:
                using EnumNamesHolderType_ = array<EnumName<ENUM_TYPE>, static_cast<size_t> (ENUM_TYPE::eCOUNT)>;

            public:
                using BasicArrayInitializer = array<EnumName<ENUM_TYPE>, static_cast<size_t> (ENUM_TYPE::eCOUNT)>;

            public:
                /**
                 */
                EnumNames () = delete;
                constexpr EnumNames (const EnumNames& src) = default;
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr EnumNames (EnumNames&& src) = default;
#endif
                constexpr EnumNames (const BasicArrayInitializer& init);
                EnumNames (const initializer_list<EnumName<ENUM_TYPE>>& origEnumNames);
                template     <size_t N>
                constexpr   EnumNames (const EnumName<ENUM_TYPE> origEnumNames[N]);

            public:
                /**
                 */
                nonvirtual  EnumNames& operator= (const EnumNames& rhs) = default;

            public:
                /**
                 */
                explicit operator initializer_list<EnumName<ENUM_TYPE>> () const;

            public:
                using   const_iterator  =   typename EnumNamesHolderType_::const_iterator;

            public:
                /**
                 */
                nonvirtual  const_iterator  begin () const;

            public:
                /**
                 */
                nonvirtual  const_iterator  end () const;

            public:
                /**
                 */
                nonvirtual  constexpr   size_t  size () const;

            public:
                /**
                 *  The argument 'e' must be a valid enumerator entry. If its 'eEND' this function will return nullptr;
                 *  Otherwise it will return a valid const wchar_t* pointer to that enumerators name.
                 *
                 *  @see GetName ();
                 */
                nonvirtual  const wchar_t*  PeekName (ENUM_TYPE e) const;

            public:
                /**
                 *  The argument 'e' must be a valid enumerator entry between eSTART, and eEND (not including eEND).
                 *  It returns a pointer to a valid const wchar_t* string of the name of the enumerator.
                 *
                 *  @see PeekName ();
                 */
                nonvirtual  const wchar_t*  GetName (ENUM_TYPE e) const;

            public:
                /**
                 *  Returns nullptr if not found.
                 */
                nonvirtual  const ENUM_TYPE*  PeekValue (const wchar_t* name) const;

            public:
                /**
                 *  The /1 overload requires the name is present, and asserts if not found.
                 *  The /2 overload throws the argument exceptio iff the name is not found.
                 */
                nonvirtual  ENUM_TYPE  GetValue (const wchar_t* name) const;
                template    <typename   NOT_FOUND_EXCEPTION>
                nonvirtual  ENUM_TYPE  GetValue (const wchar_t* name, const NOT_FOUND_EXCEPTION& notFoundException) const;

#if     qDebug && (qCompilerAndStdLib_constexpr_Buggy || !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy)
            private:
                nonvirtual  constexpr   void    RequireItemsOrderedByEnumValue_ () const;
#endif

            private:
                EnumNamesHolderType_   fEnumNames_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Enumeration.inl"

#endif  /*_Stroika_Foundation_Configuration_Enumeration_h_*/
