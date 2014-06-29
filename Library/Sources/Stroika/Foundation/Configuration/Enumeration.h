/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_h_
#define _Stroika_Foundation_Configuration_Enumeration_h_  1

#include    "../StroikaPreComp.h"

#include    <type_traits>
#include    <utility>

#include    "Common.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 * TODO:
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
            ENUM    Inc (ENUM e);


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
            typename underlying_type<ENUM>::type    ToInt (ENUM e);


            /**
             *  \brief  Cast the given int to the given ENUM type - (like static_cast<int>()) - but check range.
             *
             *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
             *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
             *
             *  This function is handy since class enum's cannot be automatically promoted to integers.
             */
            template    <typename   ENUM>
            ENUM    ToEnum (typename underlying_type<ENUM>::type e);


            /**
             *  \brief  offset of given enum from ENUM::eSTART
             *
             *      \req    ENUM uses  Stroika_Define_Enum_Bounds() to define eSTART, eEND
             *      \req    e >= typename ENUM::eSTART and e < typename ENUM::eEND
             *
             *  @todo   See if there is some better way for this.
             */
            template    <typename   ENUM>
            typename make_unsigned<typename underlying_type<ENUM>::type>::type    OffsetFromStart (ENUM e);


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
             */
#define Stroika_Enum_Names(ENUMNAME)\
    ENUMNAME##_EnumNames


            /**
             */
            template <typename ENUM_TYPE>
            using EnumName = pair<ENUM_TYPE, const wchar_t*>;


            /**
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#POD-Level-Thread-Safety">POD-Level-Thread-Safety</a>
             *          This class fully supports multiple readers, but it is not designed to support update while ongoing access
             *          is going on.
             */
            template <typename ENUM_TYPE>
            class   EnumNames : public initializer_list<EnumName<ENUM_TYPE>> {
            private:
                using inherited = initializer_list<EnumName<ENUM_TYPE>>;

            public:
                /**
                 */
                EnumNames () = delete;
                EnumNames (const EnumNames& src);
                EnumNames (EnumNames&& src);
                EnumNames (const initializer_list<EnumName<ENUM_TYPE>>& origEnumNames);
                EnumNames (initializer_list<EnumName<ENUM_TYPE>>&& origEnumNames);

            public:
                /**
                 */
                nonvirtual  EnumNames& operator= (const EnumNames& rhs) = default;

            public:
                /**
                 */
                nonvirtual  const wchar_t*  PeekName (ENUM_TYPE e) const;

            public:
                /**
                 */
                nonvirtual  const wchar_t*  GetName (ENUM_TYPE e) const;
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
