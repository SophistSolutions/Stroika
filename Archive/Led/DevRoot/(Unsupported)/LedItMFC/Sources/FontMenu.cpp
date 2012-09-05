/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/FontMenu.cpp,v 2.14 2000/04/04 15:13:02 lewis Exp $
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
 *	Revision 2.14  2000/04/04 15:13:02  lewis
 *	Use new Led_InstalledFonts class instead of manually walking font list
 *	
 *	Revision 2.13  1999/12/09 17:29:20  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.12  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 2.11  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/12/24  04:13:35  lewis
 *	Use STL vector/string stuff instead of old Led utilites.
 *	New GetUsabelFontNames () shared function (so we get same list in both menu and toolbar).
 *	And use fontnamelist algorithm from WordPad - leaving out raster fonts.
 *
 *	Revision 2.9  1997/09/29  16:00:32  lewis
 *	Lose qLedFirstIndex support
 *
 *	Revision 2.8  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.7  1997/06/18  03:41:46  lewis
 *	qIncludePrefixFile
 *
 *	Revision 2.6  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.5  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/09/30  14:58:29  lewis
 *	Sort font names in font menu.
 *
 *	Revision 2.3  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/06/01  02:42:59  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1996/04/18  15:50:23  lewis
 *	plug (innocuous) memory leak on program exit, just so any REAL memory leaks
 *	show up better in leak detector.
 *
 *	Revision 2.0  1996/01/04  00:58:05  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1996/01/04  00:57:19  lewis
 *	Initial revision
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

#include	"LedGDI.h"
#include	"Resource.h"

#include	"FontMenu.h"



static	Led_InstalledFonts	sInstalledFonts;	// Keep a static copy for speed, and so font#s are static throughout the life of the applet


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
 *********************************** FixupFontMenu ******************************
 ********************************************************************************
 */
void	FixupFontMenu (CMenu* fontMenu)
{
	Led_AssertMember (fontMenu, CMenu);
	const vector<Led_SDK_String>&	fontNames	=	GetUsableFontNames ();

	// delete all menu items
	while (fontMenu->DeleteMenu (0, MF_BYPOSITION) != 0) {
		;
	}

	for (size_t i = 0; i < fontNames.size (); i++) {
		int	cmdNum	=	cmdFontMenuFirst + i;
		fontMenu->AppendMenu (MF_STRING, cmdNum, fontNames[i].c_str ());
	}
}

Led_SDK_String	CmdNumToFontName (UINT cmdNum)
{
	const vector<Led_SDK_String>&	fontNames	=	GetUsableFontNames ();
	return (fontNames[cmdNum-cmdFontMenuFirst]);
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
