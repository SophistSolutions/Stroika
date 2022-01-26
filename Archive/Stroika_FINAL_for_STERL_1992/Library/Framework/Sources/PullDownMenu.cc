/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PullDownMenu.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 *
 * TODO:
 *
 *
 *
 * Changes:
 *	$Log: PullDownMenu.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/06/25  08:27:35  sterling
 *		React to DrawText change. And renamed CalcDefaultSize to CalcDefaultSize_. +++
 *
 *		Revision 1.17  1992/04/20  14:33:38  lewis
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *
 *		Revision 1.16  92/04/13  00:59:05  00:59:05  lewis (Lewis Pringle)
 *		Got rid of hack with motif pulldown menus returning nil - really return widgit in GetWidget().
 *		
 *		Revision 1.15  92/04/08  15:28:22  15:28:22  lewis (Lewis Pringle)
 *		ported to mac.
 *		
 *		Revision 1.14  92/04/07  15:58:27  15:58:27  sterling (Sterling Wight)
 *		fixed Calcdefaultsize for Motif
 *		
 *		Revision 1.12  92/04/02  13:07:30  13:07:30  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.11  92/03/26  18:34:29  18:34:29  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged () since we our layout etc depends on effective font.
 *		
 *		Revision 1.9  1992/03/19  16:57:00  lewis
 *		Got rid of hack override of SetTitle_.
 *
 *		Revision 1.8  1992/03/17  17:06:31  lewis
 *		Override GetWidget () since should be done for PullDownMenuTitle_MotifUI. But had to hack
 *		and return nil for now, since causes crash.. Investigate soon.
 *		Override SetTitle_ () as hack - get rid of when we fix base class SetTitle_.
 *
 *		Revision 1.7  1992/03/16  16:11:48  sterling
 *		added draw and calcdefaultsize code for mac portable imlementation
 *
 *		Revision 1.6  1992/03/10  00:09:17  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.4  1992/02/28  22:33:25  lewis
 *		Change realize/unrealize code to reflect changes in View definitions.
 *
 *		Revision 1.3  1992/02/28  15:17:56  lewis
 *		Got rid of MenuTitle_Mac_Native - moved relevant mac specific functionality into MenuTitle.
 *		PullDownMenuTitle_MotifUI and PullDownMenuTitle_MacUI_Portable no longer inherit from View, and EnableView respectively since
 *		that functionality is inherited in a common base.
 *
 *		Revision 1.2  1992/02/15  06:53:37  lewis
 *		Get rid of fNativeMenu, and cleanup includes.
 *
 *		Revision 1.1  1992/02/15  00:48:30  lewis
 *		Initial revision
 *
 *
 *
 */


#include	<ctype.h>
#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Memory.h>
#include	<Menus.h>
#include	<ToolUtils.h>			// just for HiWord/LoWord
#elif	qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#if		qXmToolkit
#include	<Xm/CascadeBG.h>
#include	<Xm/RowColumn.h>
#endif
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Menu.hh"
#include	"MenuItem.hh"
#include	"PushButton.hh"

#include	"MenuBar.hh"












/*
 ********************************************************************************
 **************************** AbstractPullDownMenuTitle *************************
 ********************************************************************************
 */

AbstractPullDownMenuTitle::AbstractPullDownMenuTitle (const String& title, Menu* menu, ButtonController* controller):
	MenuTitle (title, menu)
{
	SetController (controller);
}










#if		qMacToolkit
/*
 ********************************************************************************
 ************************** PullDownMenuTitle_MacUI_Portable *******************
 ********************************************************************************
 */

PullDownMenuTitle_MacUI_Portable::PullDownMenuTitle_MacUI_Portable (const String& title, Menu* menu,
		ButtonController* controller):
	AbstractPullDownMenuTitle (title, menu, controller),
	fButton (Nil)
{
	fButton = new PushButton (title, this);
	SetController (controller);
	SetFont (&kSystemFont);
}

PullDownMenuTitle_MacUI_Portable::PullDownMenuTitle_MacUI_Portable (const String& title, ButtonController* controller):
	AbstractPullDownMenuTitle (title, Nil, controller),
	fButton (Nil)
{
	fButton = new PushButton (title, this);
	SetController (controller);
	SetFont (&kSystemFont);
}

Boolean	PullDownMenuTitle_MacUI_Portable::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
AssertNotImplemented ();
	return (False);
}

void	PullDownMenuTitle_MacUI_Portable::Draw (const Region& update)
{
	const Font&	f = GetEffectiveFont ();
	Coordinate	textV = (GetSize ().GetV () - (f.GetFontHeight ()))/2;
	DrawText (GetTitle () , Point (textV, (GetSize ().GetH () - TextWidth (GetTitle (), f, kZeroPoint)) / 2));
}

Point	PullDownMenuTitle_MacUI_Portable::CalcDefaultSize_ (const Point& defaultSize) const
{
	// gross hack - way off - needs research - use global MBarHeight!!! LGP Mar 5, 1992
	return (Point (20, TextWidth (GetTitle ()) + 5));
}

void	PullDownMenuTitle_MacUI_Portable::EffectiveFontChanged (const Font& newFont, UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */
	AbstractPullDownMenuTitle::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	Refresh (updateMode);					// cuz our display may change
}




#elif qXmToolkit




/*
 ********************************************************************************
 ************************** PullDownMenuTitle_MotifUI **************************
 ********************************************************************************
 */

PullDownMenuTitle_MotifUI::PullDownMenuTitle_MotifUI (const String& title, Menu* menu):
	AbstractPullDownMenuTitle (title, menu, Nil),
	fWidget (Nil)
{
}

PullDownMenuTitle_MotifUI::~PullDownMenuTitle_MotifUI ()
{
	Require (fWidget == Nil);		// View unrealizes before destroying...
}

void	PullDownMenuTitle_MotifUI::Realize (osWidget* parent)
{
	Require (fWidget == Nil);
	RequireNotNil (GetMenu ());

	char	tmp[1024];
	osWidget* widget = ::XmCreateCascadeButtonGadget (parent, GetTitle ().ToCStringTrunc (tmp, sizeof (tmp)), Nil, 0);

	/*
	 * Although one might intuitively expect the menu to have as its parent, the CascadeButton, according to 
	 * the Motif Programmers Reference (hardback) 1-378:
	 * ...
	 *	o	If the Pulldown MenuPane is to be pulled down from a MenuBar, its parent must be the MenuBar.
	 * ...
	 */
	Require (XmIsRowColumn (parent));	// Also, asert its a MenuBar...
	GetMenu ()->Realize (parent);
	Arg	args [1];
	XtSetArg (args[0], XmNsubMenuId, GetMenu ()->GetOSRepresentation ());
	::XtSetValues (widget, args, 1);

	::XtRealizeWidget (widget);
	::XtManageChild (widget);
	fWidget = widget;

	EnsureNotNil (fWidget);
}

void	PullDownMenuTitle_MotifUI::UnRealize ()
{
	RequireNotNil (GetMenu ());
	GetMenu ()->UnRealize ();
	if (fWidget != Nil) {
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}

osWidget*	PullDownMenuTitle_MotifUI::GetWidget () const
{
	return (fWidget);
}

Point	PullDownMenuTitle_MotifUI::CalcDefaultSize_ (const Point& defaultSize) const
{
	return (Point (34, TextWidth (GetTitle ()) + 22));
}

Boolean	PullDownMenuTitle_MotifUI::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
	AssertNotReached ();
}

void	PullDownMenuTitle_MotifUI::EffectiveFontChanged (const Font& newFont, UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */

	AbstractPullDownMenuTitle::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	Refresh (updateMode);					// cuz our display may change
}

#endif    /*Toolkit*/









/*
 ********************************************************************************
 ***************************************** CheckBox *****************************
 ********************************************************************************
 */
PullDownMenuTitle::PullDownMenuTitle (const String& title, Menu* menu):
#if		qMacToolkit
	PullDownMenuTitle_MacUI_Portable (title, menu)
#elif	qXmToolkit
	PullDownMenuTitle_MotifUI (title, menu)
#endif
{
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

