/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Execution/Throw.h"

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
