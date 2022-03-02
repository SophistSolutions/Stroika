/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CheckBoxMenuItem__
#define	__CheckBoxMenuItem__

/*
 * $Header: /fuji/lewis/RCS/CheckBoxMenuItem.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 *		This file contains all of the checkbox menu item classes. AbstractCheckBoxMenuItem is a kind
 * of MenuItem which defines all the properties of checkable/togglebox menu items. We define subclasses
 * to define the abstract interface to each of the GUIs, and, at least, native implementations for each.
 *
 *		Then, we define CheckBoxMenuItem to be the default kind created according the the current values
 * of the configuration macro qNative, and qXXXGUI.
 *
 * TODO:
 *		Consider renaming this class/file to ToggleMenuItem.
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: CheckBoxMenuItem.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.8  1992/03/09  23:44:48  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.7  1992/02/21  19:39:26  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.6  1992/02/06  20:48:43  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.5  1992/02/04  16:51:48  lewis
 *		Override IsNativeItem () for mac native guy to say it is native.
 *
 *		Revision 1.4  1992/02/04  05:02:23  lewis
 *		Add destructor to CheckBoxMenuItem_MotifUI_Native. And some comments.
 *
 *		Revision 1.3  1992/02/04  03:47:01  lewis
 *		Add native and gui intermediate classes, and CheckBoxMenuItem derived class to replace old
 *		#define/typedef.
 *
 *		Revision 1.2  1992/02/03  23:31:40  lewis
 *		changed typedef to #define for motif as temp hack til I get around to doing real class.
 *
 *		Revision 1.1  1992/02/03  22:29:57  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"MenuItem.hh"





class	AbstractCheckBoxMenuItem : public MenuItem {
	public:
		AbstractCheckBoxMenuItem (CommandNumber commandNumber);	

	protected:
		override	void	SetOn_ (Boolean on, UpdateMode updateMode);
};




class	CheckBoxMenuItem_MacUI : public AbstractCheckBoxMenuItem {
	public:
		CheckBoxMenuItem_MacUI (CommandNumber commandNumber);	
};




class	CheckBoxMenuItem_MotifUI : public AbstractCheckBoxMenuItem {
	public:
		CheckBoxMenuItem_MotifUI (CommandNumber commandNumber);
};







#if		qMacToolkit

class	CheckBoxMenuItem_MacUI_Native : public CheckBoxMenuItem_MacUI {
	public:
		CheckBoxMenuItem_MacUI_Native (CommandNumber commandNumber);	
		override	void	Draw (const Region& update);

	protected:
		override	void	UpdateOSRep_ ();

	private:
		override	Boolean	IsNativeItem () const;
};

#elif	qXmToolkit

class	CheckBoxMenuItem_MotifUI_Native : public CheckBoxMenuItem_MotifUI {
	public:
		CheckBoxMenuItem_MotifUI_Native (CommandNumber commandNumber);
		~CheckBoxMenuItem_MotifUI_Native ();

	protected:
		override 	void		Realize (osWidget* parent);
		override	void		UnRealize ();
		override	osWidget*	GetWidget () const;
	private:
		osWidget*	fWidget;
	protected:
		override	void	UpdateOSRep_ ();
};

#endif	/*Toolkit*/



class	CheckBoxMenuItem : public
#if		qMacToolkit
	CheckBoxMenuItem_MacUI_Native
#elif	qXmToolkit
	CheckBoxMenuItem_MotifUI_Native
#endif
	{
		public:
			CheckBoxMenuItem (CommandNumber commandNumber);
};




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__CheckBoxMenuItem__*/
