/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
