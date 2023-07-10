/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxwin.h>
DISABLE_COMPILER_MSC_WARNING_END (5054)

#include "Stroika/Frameworks/Led/GDI.h"

#include "Resource.h"

#include "FontMenu.h"

namespace {

    InstalledFonts sInstalledFonts; // Keep a static copy for speed, and so font#s are static throughout the life of the applet
}

/*
 ********************************************************************************
 *********************************** GetUsableFontNames *************************
 ********************************************************************************
 */
const vector<SDKString>& GetUsableFontNames () { return sInstalledFonts.GetUsableFontNames (); }

/*
 ********************************************************************************
 *********************************** CmdNumToFontName ***************************
 ********************************************************************************
 */
SDKString CmdNumToFontName (UINT cmdNum)
{
    Require (cmdNum >= kBaseFontNameCmd);
    Require (cmdNum <= kLastFontNameCmd);
    const vector<SDKString>& fontNames = GetUsableFontNames ();
    return (fontNames[cmdNum - kBaseFontNameCmd]);
}
