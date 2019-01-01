/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#ifndef __ColorMenu_h__
#define __ColorMenu_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(WIN32)
#include <afxwin.h>
#endif

#include "Stroika/Frameworks/Led/GDI.h"

#include "LedItConfig.h"

#if qPlatform_Windows
class ColorMenu : public CMenu {
public:
    ColorMenu ();

public:
    static COLORREF GetColor (UINT id);

    static Led_Color FontCmdToColor (UINT cmd);
    static UINT      FontColorToCmd (Led_Color color);

public:
    virtual void DrawItem (LPDRAWITEMSTRUCT lpDIS) override;
    virtual void MeasureItem (LPMEASUREITEMSTRUCT lpMIS) override;
};
#endif

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*__ColorMenu_h__*/
