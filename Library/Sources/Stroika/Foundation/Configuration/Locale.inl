/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Locale_inl_
#define _Stroika_Foundation_Configuration_Locale_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Configuration {


            /*
             ********************************************************************************
             **************** Configuration::GetPlatformDefaultLocale ***********************
             ********************************************************************************
             */
            inline  std::locale GetPlatformDefaultLocale ()
            {
                return std::locale ("");
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Configuration_Locale_inl_*/
