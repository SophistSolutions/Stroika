/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Execution/Throw.h"

namespace Stroika::Foundation::Common {

    /*
     ********************************************************************************
     ***************************** Common::ResolveLocale ****************************
     ********************************************************************************
     */
    inline locale ResolveLocale (const LocaleSpecifier& l)
    {
        if (const PredefinedLocale* p = get_if<PredefinedLocale> (&l)) {
            return *p == PredefinedLocale::eUseCLocale ? locale::classic () : locale{};
        }
        return get<locale> (l);
    }

    /*
     ********************************************************************************
     *********************** Common::GetPlatformDefaultLocale ***********************
     ********************************************************************************
     */
    inline locale GetPlatformDefaultLocale ()
    {
        // From https://en.cppreference.com/w/cpp/locale/setlocale
        //      Can be "" for the user-preferred locale or "C" for the minimal locale
        // But https://en.cppreference.com/w/cpp/locale/locale/locale doesn't have similar language. So not clear
        // this is guaranteed to work. But it seems to ...
        //      -- LGP 2018-10-15
        return locale{""};
    }

    /*
     ********************************************************************************
     ***************************** Common::ScopedUseLocale **************************
     ********************************************************************************
     */
    inline ScopedUseLocale::ScopedUseLocale (const optional<locale>& l)
    {
        if (l) {
            fPrev_ = locale::global (*l);
        }
    }
    inline ScopedUseLocale::~ScopedUseLocale ()
    {
        if (fPrev_) {
            IgnoreExceptionsForCall (locale::global (*fPrev_));
        }
    }

}
