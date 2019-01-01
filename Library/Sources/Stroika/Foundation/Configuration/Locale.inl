/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Configuration_Locale_inl_
#define _Stroika_Foundation_Configuration_Locale_inl_ 1

#include "../Execution/Exceptions.h"

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace Stroika::Foundation::Configuration {

    /*
     ********************************************************************************
     **************** Configuration::GetPlatformDefaultLocale ***********************
     ********************************************************************************
     */
    inline std::locale GetPlatformDefaultLocale ()
    {
        // From https://en.cppreference.com/w/cpp/locale/setlocale
        //      Can be "" for the user-preferred locale or "C" for the minimal locale
        // But https://en.cppreference.com/w/cpp/locale/locale/locale doesn't have similar language. So not clear
        // this is guarnateed to work. But it seems to ...
        //      -- LGP 2018-10-15
        return std::locale ("");
    }

    /*
     ********************************************************************************
     ********************** Configuration::ScopedUseLocale **************************
     ********************************************************************************
     */
    inline ScopedUseLocale::ScopedUseLocale (const locale& l)
        : fPrev_{locale::global (l)}
    {
    }
    inline ScopedUseLocale::~ScopedUseLocale ()
    {
        IgnoreExceptionsForCall (locale::global (fPrev_));
    }

}

#endif /*_Stroika_Foundation_Configuration_Locale_inl_*/
