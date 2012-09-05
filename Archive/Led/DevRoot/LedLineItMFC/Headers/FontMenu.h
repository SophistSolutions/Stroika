/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__FontMenu_hh__
#define	__FontMenu_hh__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/FontMenu.h,v 2.9 2002/05/06 21:31:13 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: FontMenu.h,v $
 *	Revision 2.9  2002/05/06 21:31:13  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.8  2001/11/27 00:28:19  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.7  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2000/04/14 22:41:02  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.5  2000/04/04 15:13:21  lewis
 *	Use new Led_InstalledFonts class instead of manually walking font list
 *	
 *	Revision 2.4  1999/12/09 17:35:26  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.3  1999/12/09 03:29:07  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.2  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedLineIt! 2.1 ==========>
 *
 *
 *
 *
 */

#include	<afx.h>

#include	"LedSupport.h"

#include	"LedLineItConfig.h"


class	CMenu;
void	FixupFontMenu (CMenu* fontMenu);

Led_SDK_String	CmdNumToFontName (UINT cmdNum);


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif	/*__FontMenu_hh__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

