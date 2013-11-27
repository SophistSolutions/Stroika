/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_h_
#define _Stroika_Foundation_Configuration_Enumeration_h_  1

#include    "../StroikaPreComp.h"

#include    <type_traits>

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
