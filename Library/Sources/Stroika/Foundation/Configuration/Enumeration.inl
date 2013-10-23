/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Enumeration_inl_
#define _Stroika_Foundation_Configuration_Enumeration_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../Debug/Assertions.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /*
             ********************************************************************************
             ******************************** Configuration::Inc ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
            inline  ENUM    Inc (ENUM e)
            {
                return ToEnum<ENUM> (ToInt (e) + 1);
            }


            /*
             ********************************************************************************
             ****************************** Configuration::ToInt ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
            inline  typename underlying_type<ENUM>::type    ToInt (ENUM e)
#else
            inline  int    ToInt (ENUM e)
#endif
            {
                Require (ENUM::eSTART <= e and e <= ENUM::eEND);
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
                return static_cast<typename underlying_type<ENUM>::type> (e);
#else
                return static_cast<int> (e);
#endif
            }


            /*
             ********************************************************************************
             ***************************** Configuration::ToEnum ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
            inline   ENUM   ToEnum (typename underlying_type<ENUM>::type e)
#else
            inline   ENUM   ToEnum (int e)
#endif
            {
                Require (ENUM::eSTART <= static_cast<ENUM> (e) and static_cast<ENUM> (e) <= ENUM::eEND);
                return static_cast<ENUM> (e);
            }


            /*
             ********************************************************************************
             ******************** Configuration::OffsetFromStart ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
            inline  typename make_unsigned<typename underlying_type<ENUM>::type>::type    OffsetFromStart (ENUM e)
#else
            inline  int    OffsetFromStart (ENUM e)
#endif
            {
                Require (ENUM::eSTART <= e and e <= ENUM::eEND);
#if     qCompilerAndStdLib_Supports_TypeTraits_underlying_type
                return static_cast<typename make_unsigned<typename underlying_type<ENUM>::type>::type> (ToInt (e) - ToInt (ENUM::eSTART));
#else
                return static_cast<int> (ToInt (e) - ToInt (ENUM::eSTART));
#endif
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Enumeration_inl_*/
