/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__FontMenu_hh__
#define	__FontMenu_hh__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/FontMenu.h,v 2.9 2000/04/14 22:40:52 lewis Exp $
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
 *	Revision 2.9  2000/04/14 22:40:52  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.8  1999/12/09 17:29:20  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.7  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  1997/12/24  04:04:58  lewis
 *	New GetUsableFontNames9) function here - to share with toolbar code. Use string class, etc.
 *
 *	Revision 2.5  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.4  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.3  1996/12/05  21:16:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/02/26  22:45:14  lewis
 *	Include <afx.h>
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1996/01/04  00:55:22  lewis
 *	Initial revision
 *
 *
 *
 *
 */
#include	<afx.h>

#include	<vector>
#include	<string>

#include	"LedSupport.h"

#include	"LedItConfig.h"


class	CMenu;
void	FixupFontMenu (CMenu* fontMenu);

Led_SDK_String	CmdNumToFontName (UINT cmdNum);

const vector<Led_SDK_String>&	GetUsableFontNames ();	// perform whatever filtering will be done on sys installed fonts and return the names



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

