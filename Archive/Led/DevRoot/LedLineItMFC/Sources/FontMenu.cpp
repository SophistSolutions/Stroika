/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/FontMenu.cpp,v 2.12 2002/05/06 21:31:17 lewis Exp $
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
 *	Revision 2.12  2002/05/06 21:31:17  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.11  2001/11/27 00:28:21  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.10  2001/08/30 01:02:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2000/04/04 15:13:21  lewis
 *	Use new Led_InstalledFonts class instead of manually walking font list
 *	
 *	Revision 2.8  1999/12/09 17:35:26  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.7  1999/12/09 03:29:07  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.6  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable instaed of const cast, new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.5  1998/05/05  00:37:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1997/09/29  18:03:50  lewis
 *	Lose qLedFirstIndex
 *
 *	Revision 2.2  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/06/18  03:46:41  lewis
 *	qIncludePrefixFile/cleanups
 *
 *	Revision 2.0  1997/01/10  03:38:30  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedLineIt! 2.1 ==========>
 *
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxwin.h>

#include	<algorithm>

#include	"LedGDI.h"
#include	"Resource.h"

#include	"FontMenu.h"




static	Led_InstalledFonts	sInstalledFonts;	// Keep a static copy for speed, and so font#s are static throughout the life of the applet











/*
 ********************************************************************************
 *********************************** FixupFontMenu ******************************
 ********************************************************************************
 */
void	FixupFontMenu (CMenu* fontMenu)
{
	Led_AssertMember (fontMenu, CMenu);

	// delete all menu items
	while (fontMenu->DeleteMenu (0, MF_BYPOSITION) != 0) {
		;
	}

	vector<Led_SDK_String>	fontNames	=	sInstalledFonts.GetUsableFontNames ();
	for (size_t i = 0; i < fontNames.size (); i++) {
		int	cmdNum	=	cmdFontMenuFirst + i;
		fontMenu->AppendMenu (MF_STRING, cmdNum, fontNames[i].c_str ());
	}
}

Led_SDK_String	CmdNumToFontName (UINT cmdNum)
{
	vector<Led_SDK_String>	fontNames	=	sInstalledFonts.GetUsableFontNames ();
	return (fontNames[cmdNum-cmdFontMenuFirst]);
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
