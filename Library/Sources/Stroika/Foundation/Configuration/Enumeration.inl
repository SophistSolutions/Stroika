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
            inline  constexpr   ENUM    Inc (ENUM e)
            {
                return ToEnum<ENUM> (ToInt (e) + 1);
            }


            /*
             ********************************************************************************
             ****************************** Configuration::ToInt ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
            inline  constexpr   typename underlying_type<ENUM>::type    ToInt (ENUM e)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require (ENUM::eSTART <= e and e <= ENUM::eEND);
#endif
                return static_cast<typename underlying_type<ENUM>::type> (e);
            }


            /*
             ********************************************************************************
             ***************************** Configuration::ToEnum ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
            inline   constexpr  ENUM   ToEnum (typename underlying_type<ENUM>::type e)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require (ENUM::eSTART <= static_cast<ENUM> (e) and static_cast<ENUM> (e) <= ENUM::eEND);
#endif
                return static_cast<ENUM> (e);
            }


            /*
             ********************************************************************************
             ******************** Configuration::OffsetFromStart ****************************
             ********************************************************************************
             */
            template    <typename   ENUM>
            inline  constexpr   typename make_unsigned<typename underlying_type<ENUM>::type>::type    OffsetFromStart (ENUM e)
            {
#if     !qCompilerAndStdLib_constexpr_functions_cpp14Constaints_Buggy
                Require (ENUM::eSTART <= e and e <= ENUM::eEND);
#endif
                return static_cast<typename make_unsigned<typename underlying_type<ENUM>::type>::type> (ToInt (e) - ToInt (ENUM::eSTART));
            }



            /*
             ********************************************************************************
             ************************ Configuration::EnumNames ******************************
             ********************************************************************************
             */
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::EnumNames (EnumNames&& src)
                : fEnumNames_ (move (src))
            {
            }
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::EnumNames (const initializer_list<EnumName<ENUM_TYPE>>& origEnumNames)
                : fEnumNames_ ()
            {
                // @todo find some way to INITIALZIE the static array.... - needed for constexpr function!
                auto oi = fEnumNames_.begin ();
                for (EnumName<ENUM_TYPE> i : origEnumNames) {
                    Require (oi != fEnumNames_.end ());
                    *oi = i;
                    ++oi;
                }
            }
            template     <typename ENUM_TYPE>
            template     <size_t N>
            inline
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr
#endif
            EnumNames<ENUM_TYPE>::EnumNames (const EnumName<ENUM_TYPE> origEnumNames[N])
                : fEnumNames_ (origEnumNames)
            {
            }
            template     <typename ENUM_TYPE>
            inline  EnumNames<ENUM_TYPE>::operator initializer_list<EnumName<ENUM_TYPE>> () const
            {
                return fEnumNames_;
            }
            template     <typename ENUM_TYPE>
            inline  typename EnumNames<ENUM_TYPE>::const_iterator  EnumNames<ENUM_TYPE>::begin () const
            {
                return fEnumNames_.begin ();
            }
            template     <typename ENUM_TYPE>
            inline  typename EnumNames<ENUM_TYPE>::const_iterator  EnumNames<ENUM_TYPE>::end () const
            {
                return fEnumNames_.end ();
            }
            template     <typename ENUM_TYPE>
            inline  size_t  EnumNames<ENUM_TYPE>::size () const
            {
                return fEnumNames_.size ();
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
