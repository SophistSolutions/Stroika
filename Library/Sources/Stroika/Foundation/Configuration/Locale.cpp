/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "StroikaConfig.h"

#include    "Locale.h"

using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Configuration;





/*
********************************************************************************
*********** Configuration::UsePlatformDefaultLocaleAsDefaultLocale *************
********************************************************************************
*/
void    Configuration::UsePlatformDefaultLocaleAsDefaultLocale ()
{
    locale::global (GetPlatformDefaultLocale ());
}

