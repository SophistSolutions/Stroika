/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef __FontMenu_hh__
#define __FontMenu_hh__ 1

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Headers/FontMenu.h,v 2.10 2004/01/24 03:49:36 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *  $Log: FontMenu.h,v $
 *  Revision 2.10  2004/01/24 03:49:36  lewis
 *  *** empty log message ***
 *
 *  Revision 2.9  2004/01/23 23:11:15  lewis
 *  SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based
 *  context menus. Tons of new OLE APIs. Can create context menus from scratch, or built from
 *  building blocks of existing submenus (like font menu).
 *
 *  Revision 2.8  2002/05/06 21:34:19  lewis
 *  <=============================== Led 3.0.1 Released ==============================>
 *
 *  Revision 2.7  2001/11/27 00:32:29  lewis
 *  <=============== Led 3.0 Released ===============>
 *
 *  Revision 2.6  2001/08/30 00:35:56  lewis
 *  *** empty log message ***
 *
 *  Revision 2.5  2000/04/14 22:40:44  lewis
 *  SPR#0740- namespace support
 *
 *  Revision 2.4  1999/12/09 03:16:52  lewis
 *  spr#0645 - -D_UNICODE support
 *
 *  Revision 2.3  1998/11/02 13:23:17  lewis
 *  Changed from usin vector<char*> to vector<string> - and cleaned up font name list gen code slightly.
 *  And lost LedItView::SetWindowRect() overrides - SB very soon obsolete due to new margin/hscroll
 *  support in wordprocessor class.
 *
 *  Revision 2.2  1997/12/24 04:43:52  lewis
 *  *** empty log message ***
 *
 *  Revision 2.1  1997/07/27  15:59:52  lewis
 *  <===== Led 2.2 Released =====>
 *
 *  Revision 2.0  1997/06/28  17:43:14  lewis
 *  *** empty log message ***
 *
 *
 *
 *
 *  <========== BasedOnLedLineIt! 2.2b1 ==========>
 *
 *
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include <afx.h>

#include "ActiveLedItConfig.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

const vector<Led_SDK_String>& GetUsableFontNames ();
Led_SDK_String                CmdNumToFontName (UINT cmdNum);

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*__FontMenu_hh__*/
