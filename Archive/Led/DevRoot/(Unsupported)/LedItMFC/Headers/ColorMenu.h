/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__ColorMenu_h__
#define	__ColorMenu_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/ColorMenu.h,v 1.3 1999/11/15 21:35:26 lewis Exp $
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
 *	Revision 1.3  1999/11/15 21:35:26  lewis
 *	react to some small Led changes so that Led itself no longer deends on MFC (though this demo app clearly DOES)
 *	
 *	Revision 1.2  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  1997/12/24  04:07:11  lewis
 *	Initial revision
 *
 *
 *
 *	<<Based on MFC / MSDEVSTUDIO 5.0 Sample Wordpad - LGP 971212>>
 *
 *
 */
#include	<afxwin.h>

#include	"LedGDI.h"

#include	"LedItConfig.h"



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

