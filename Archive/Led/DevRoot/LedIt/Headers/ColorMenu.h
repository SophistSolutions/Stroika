/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__ColorMenu_h__
#define	__ColorMenu_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/ColorMenu.h,v 1.5 2002/10/24 15:53:13 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: ColorMenu.h,v $
 *	Revision 1.5  2002/10/24 15:53:13  lewis
 *	SPR#0680- adjust the window margins so text looks a little better. Did for MacOS/XWin/Win - but only tested on Win so far
 *	
 *	Revision 1.4  2002/05/06 21:30:56  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.3  2001/11/27 00:28:08  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.2  2001/08/29 22:59:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:42  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */
#if		defined (WIN32)
	#include	<afxwin.h>
#endif

#include	"LedGDI.h"

#include	"LedItConfig.h"


#if		qWindows
class	ColorMenu : public CMenu {
	public:
		ColorMenu ();

	public:
		static	COLORREF	GetColor (UINT id);

		static	Led_Color	FontCmdToColor (UINT cmd);
		static	UINT		FontColorToCmd (Led_Color color);

	public:
		override	void	DrawItem (LPDRAWITEMSTRUCT lpDIS);
		override	void	MeasureItem (LPMEASUREITEMSTRUCT lpMIS);

};
#endif



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif	/*__ColorMenu_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

