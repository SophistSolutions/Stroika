/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */

#ifndef	__ColorMenu_h__
#define	__ColorMenu_h__	1

#if		defined (WIN32)
	#include	<afxwin.h>
#endif

#include	"Stroika/Frameworks/Led/GDI.h"

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

