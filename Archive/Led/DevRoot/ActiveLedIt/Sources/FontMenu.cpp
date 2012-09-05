/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveLedIt/Sources/FontMenu.cpp,v 2.13 2004/01/24 03:49:11 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: FontMenu.cpp,v $
 *	Revision 2.13  2004/01/24 03:49:11  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.12  2004/01/23 23:11:25  lewis
 *	SPR#1617: Added preliminary (largely functional) implementation of OLE automation-based
 *	context menus. Tons of new OLE APIs. Can create context menus from scratch, or built from
 *	building blocks of existing submenus (like font menu).
 *	
 *	Revision 2.11  2003/05/08 16:12:49  lewis
 *	SPR#1467: Added Select Menu (moving find/select all under it in context menu).
 *	Also fixed bug with cmd#s (internal# vs led# with CmdToFontName()
 *	
 *	Revision 2.10  2003/04/04 19:36:52  lewis
 *	SPR#1407: convert ActiveLedIt to using new command-number vectoring etc scheme (not
 *	template based). Revised cmd #define names in a few cases for greater consistency across
 *	demo apps.
 *	
 *	Revision 2.9  2002/05/06 21:34:31  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.8  2001/11/27 00:32:37  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/08/30 00:35:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2000/04/04 15:12:46  lewis
 *	Use new Led_InstalledFonts class instead of manually walking font list
 *	
 *	Revision 2.5  1999/12/09 03:16:52  lewis
 *	spr#0645 - -D_UNICODE support
 *	
 *	Revision 2.4  1998/11/02 13:23:25  lewis
 *	Changed from usin vector<char*> to vector<string> - and cleaned up font name list gen code slightly.
 *	And lost LedItView::SetWindowRect() overrides - SB very soon obsolete due to new margin/hscroll
 *	support in wordprocessor class.
 *	
 *	Revision 2.3  1997/12/24 04:44:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1997/09/29  18:12:10  lewis
 *	Lose qLedFirstIndex
 *
 *	Revision 2.1  1997/07/27  16:00:23  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.0  1997/06/28  17:45:14  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 *	<========== BasedOnLedLineIt! 2.2b1 ==========>
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxwin.h>

#include	"LedGDI.h"
#include	"Resource.h"

#include	"FontMenu.h"


#ifdef _DEBUG
	#define new DEBUG_NEW
#endif



namespace	{
	
	Led_InstalledFonts	sInstalledFonts;	// Keep a static copy for speed, and so font#s are static throughout the life of the applet

}







/*
 ********************************************************************************
 *********************************** GetUsableFontNames *************************
 ********************************************************************************
 */
const vector<Led_SDK_String>&	GetUsableFontNames ()
{
	return sInstalledFonts.GetUsableFontNames ();
}






/*
 ********************************************************************************
 *********************************** CmdNumToFontName ***************************
 ********************************************************************************
 */
Led_SDK_String	CmdNumToFontName (UINT cmdNum)
{
	Led_Require (cmdNum >= kBaseFontNameCmd);
	Led_Require (cmdNum <= kLastFontNameCmd);
	const vector<Led_SDK_String>&	fontNames	=	GetUsableFontNames ();
	return (fontNames[cmdNum-kBaseFontNameCmd]);
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
