/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afxwin.h>

#include <algorithm>

#include "Stroika/Frameworks/Led/GDI.h"

#include "Resource.h"

#include "FontMenu.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

static InstalledFonts sInstalledFonts; // Keep a static copy for speed, and so font#s are static throughout the life of the applet

/*
 ********************************************************************************
 *********************************** FixupFontMenu ******************************
 ********************************************************************************
 */
void FixupFontMenu (CMenu* fontMenu)
{
    AssertMember (fontMenu, CMenu);

    // delete all menu items
    while (fontMenu->DeleteMenu (0, MF_BYPOSITION) != 0) {
        ;
    }

    vector<SDKString> fontNames = sInstalledFonts.GetUsableFontNames ();
    for (UINT i = 0; i < fontNames.size (); ++i) {
        int cmdNum = cmdFontMenuFirst + i;
        fontMenu->AppendMenu (MF_STRING, cmdNum, fontNames[i].c_str ());
    }
}

SDKString CmdNumToFontName (UINT cmdNum)
{
    vector<SDKString> fontNames = sInstalledFonts.GetUsableFontNames ();
    return (fontNames[cmdNum - cmdFontMenuFirst]);
}
