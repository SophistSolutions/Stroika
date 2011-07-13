/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CascadeMenuItem__
#define	__CascadeMenuItem__

/*
 * $Header: /fuji/lewis/RCS/CascadeMenuItem.hh,v 1.2 1992/07/21 19:59:01 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: CascadeMenuItem.hh,v $
 *		Revision 1.2  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/03/09  23:43:20  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.6  1992/02/12  06:22:05  lewis
 *		Deleted workarounds for qMPW_SymbolTableOverflowProblem since its now fixed,
 *		and changed name from AbstractMenu*, to Menu*.
 *
 *		Revision 1.5  1992/02/11  00:30:37  lewis
 *		Moved toward support just 1 menu class - get rid of hack where CascadeMenuItem
 *		took different type args under different toolkits...
 *
 *		Revision 1.4  1992/02/06  20:43:53  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.3  1992/02/04  16:50:29  lewis
 *		Override IsNativeItem for mac items so we can later do more magic with menus.
 *
 *		Revision 1.2  1992/02/03  23:30:10  lewis
 *		Added abstract base classes, and CascadeMenuItem class instead of typedefs and #defines at the end.
 *
 *		Revision 1.1  1992/02/03  22:29:38  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"MenuItem.hh"




class	AbstractCascadeMenuItem : public MenuItem {
	protected:
		AbstractCascadeMenuItem ();
};




class	CascadeMenuItem_MacUI : public AbstractCascadeMenuItem {
	protected:
		CascadeMenuItem_MacUI ();
};




class	CascadeMenuItem_MotifUI : public AbstractCascadeMenuItem {
	protected:
		CascadeMenuItem_MotifUI ();
};




#if		qMacToolkit

class	CascadeMenuItem_MacUI_Native : public CascadeMenuItem_MacUI {
	public:
		CascadeMenuItem_MacUI_Native (Menu* cascadeMenu);
		~CascadeMenuItem_MacUI_Native ();

	protected:
		override	void	UpdateOSRep_ ();

	private:
		class	CascadeMenuTitle*	fMenuTitle;

	private:
		override	Boolean	IsNativeItem () const;
};

#elif	qXmToolkit

class	CascadeMenuItem_MotifUI_Native : public CascadeMenuItem_MotifUI {
	public:
		CascadeMenuItem_MotifUI_Native (Menu* cascadeMenu);
		~CascadeMenuItem_MotifUI_Native ();

	protected:
		override 	void		Realize (osWidget* parent);
		override	void		UnRealize ();
		override	osWidget*	GetWidget () const;
	private:
		osWidget*	fWidget;
	protected:
		override	void	UpdateOSRep_ ();
	private:
		class CascadeMenuTitle*	fMenuTitle;
};

#endif	/*Tooklkit*/




class	CascadeMenuItem : public
#if		qMacToolkit
	CascadeMenuItem_MacUI_Native
#elif	qXmToolkit
	CascadeMenuItem_MotifUI_Native
#endif
	{
		public:
			CascadeMenuItem (Menu* cascadeMenu);
};






// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__MenuItem__*/
