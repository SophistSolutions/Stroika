/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#pragma warning(push)
#pragma warning(disable : 5054)
#include <afxwin.h>
#pragma warning(pop)

#include "Stroika/Frameworks/Led/GDI.h"

#include "Resource.h"

#include "FontMenu.h"

namespace {

    Led_InstalledFonts sInstalledFonts; // Keep a static copy for speed, and so font#s are static throughout the life of the applet
}

/*
 ********************************************************************************
 *********************************** GetUsableFontNames *************************
 ********************************************************************************
 */
const vector<Led_SDK_String>& GetUsableFontNames ()
{
    return sInstalledFonts.GetUsableFontNames ();
}

/*
 ********************************************************************************
 *********************************** CmdNumToFontName ***************************
 ********************************************************************************
 */
Led_SDK_String CmdNumToFontName (UINT cmdNum)
{
    Require (cmdNum >= kBaseFontNameCmd);
    Require (cmdNum <= kLastFontNameCmd);
    const vector<Led_SDK_String>& fontNames = GetUsableFontNames ();
    return (fontNames[cmdNum - kBaseFontNameCmd]);
}
