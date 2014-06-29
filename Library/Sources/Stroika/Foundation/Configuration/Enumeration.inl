/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
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
            inline  typename underlying_type<ENUM>::type    ToInt (ENUM e)
            {
                Require (ENUM::eSTART <= e and e <= ENUM::eEND);
                return static_cast<typename underlying_type<ENUM>::type> (e);
            }


            /*
             ********************************************************************************
             ***************************** Configuration::ToEnum ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
            inline   ENUM   ToEnum (typename underlying_type<ENUM>::type e)
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
            inline  typename make_unsigned<typename underlying_type<ENUM>::type>::type    OffsetFromStart (ENUM e)
            {
                Require (ENUM::eSTART <= e and e <= ENUM::eEND);
                return static_cast<typename make_unsigned<typename underlying_type<ENUM>::type>::type> (ToInt (e) - ToInt (ENUM::eSTART));
            }



            /*
             ********************************************************************************
             ************************ Configuration::EnumNames ******************************
             ********************************************************************************
             */
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::EnumNames (const EnumNames<ENUM_TYPE>& src)
                : fEnumNames_ (src.fEnumNames_)
            {
            }
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::EnumNames (EnumNames<ENUM_TYPE>&& src)
                : fEnumNames_ (move (src.fEnumNames_))
            {
            }
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::EnumNames (const initializer_list<EnumName<ENUM_TYPE>>& origEnumNames)
                : fEnumNames_ (origEnumNames)
            {
            }
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::EnumNames (initializer_list<EnumName<ENUM_TYPE>>&& origEnumNames)
                : fEnumNames_ (move (origEnumNames))
            {
            }
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::operator initializer_list<EnumName<ENUM_TYPE>> () const
            {
                return fEnumNames_;
            }
            template     <typename ENUM_TYPE>
            const wchar_t*  EnumNames<ENUM_TYPE>::PeekName (ENUM_TYPE e) const
            {
                for (auto i : fEnumNames_) {
                    if (i.first == e) {
                        return i.second;
                    }
                }
                return nullptr;
            }
            template     <typename ENUM_TYPE>
            inline  const wchar_t*  EnumNames<ENUM_TYPE>::GetName (ENUM_TYPE e) const
            {
                auto tmp = PeekName (e);
                RequireNotNull (tmp);
                return tmp;
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Enumeration_inl_*/
