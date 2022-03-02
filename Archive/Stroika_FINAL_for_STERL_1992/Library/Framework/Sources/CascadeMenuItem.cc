/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CascadeMenuItem.cc,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *			-	This item should be enabled iff its submenu is enabled...
 *
 *
 * Changes:
 *	$Log: CascadeMenuItem.cc,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.18  1992/04/24  08:56:12  lewis
 *		On Unrealize of motif cascademenuitem native must unrealize menu in addition to our widget.
 *
 *		Revision 1.17  92/04/20  14:27:51  14:27:51  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		And got rid of some obsolete header includes for motif.
 *		
 *		Revision 1.16  92/04/02  17:41:32  17:41:32  lewis (Lewis Pringle)
 *		Added a couple of Requires that the menu being associated with the cascadeMenuItem is not already
 *		a subview someplace.
 *		
 *		Revision 1.15  92/04/02  13:07:21  13:07:21  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.14  92/03/16  16:01:06  16:01:06  sterling (Sterling Wight)
 *		moved add to menubar, to fix race condition prob at construction time
 *		
 *		Revision 1.13  1992/03/10  00:03:06  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.12  1992/02/27  21:44:36  lewis
 *		Get rid of seperate CascadeMenuTitle implementations for mac and motif.
 *		Get rid of fNativeMenu
 *		Make the menutitle be a subview of the cascademenu item (mac and motif).
 *
 *		Revision 1.11  1992/02/16  16:33:10  lewis
 *		Fixup includes.
 *
 *		Revision 1.9  1992/02/14  03:00:26  lewis
 *		Use GetOwner () instead of (now defunct) fNativeOwner.
 *
 *		Revision 1.8  1992/02/12  06:44:45  lewis
 *		Deleted workarounds for qMPW_SymbolTableOverflowProblem since it is now a fixed bug. Also, changed AbstractMenu* to
 *		Menu*.
 *
 *		Revision 1.6  1992/02/11  00:52:53  lewis
 *		Moved toward support just 1 menu class - get rid of hack where CascadeMenuIte
 *		took different type args under different toolkits...
 *
 *		Revision 1.5  1992/02/06  21:09:37  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.4  1992/02/04  16:51:00  lewis
 *		Override IsNativeItem for mac cascade item and say we are native.
 *
 *		Revision 1.3  1992/02/04  02:31:45  lewis
 *		Added CascadeMenuItem class implementation.
 *
 *		Revision 1.2  1992/02/03  23:30:58  lewis
 *		Added abstract base classes, and a concreate defining class CascaseMenuItem instead of
 *		#defines and typedefs.
 *
 *		Revision 1.1  1992/02/03  22:30:35  lewis
 *		Initial revision
 *
 *
 */



#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Menus.h>
#elif	qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Xm.h>
#include	<Xm/CascadeBG.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Menu.hh"
#include	"MenuBar.hh"
#include	"PullDownMenu.hh"

#include	"CascadeMenuItem.hh"






// Consider putting this into the headers???
// Also, why AbstractPullDownMenuTitle instead of MenuTitle???
class	CascadeMenuTitle : public AbstractPullDownMenuTitle {
	public:
		CascadeMenuTitle (Menu* menu);
		~CascadeMenuTitle ();

	protected:
		override	Boolean	DoMenuSelection_ (const Point& startPt, MenuItem*& selected);
};









/*
 ********************************************************************************
 ***************************** AbstractCascadeMenuItem **************************
 ********************************************************************************
 */
AbstractCascadeMenuItem::AbstractCascadeMenuItem ():
	MenuItem (CommandHandler::eCascadeItem)
{
}







/*
 ********************************************************************************
 ***************************** CascadeMenuItem_MacUI ***************************
 ********************************************************************************
 */
CascadeMenuItem_MacUI::CascadeMenuItem_MacUI ():
	AbstractCascadeMenuItem ()
{
}







/*
 ********************************************************************************
 ***************************** CascadeMenuItem_MacUI ***************************
 ********************************************************************************
 */
CascadeMenuItem_MotifUI::CascadeMenuItem_MotifUI ():
	AbstractCascadeMenuItem ()
{
}








#if		qMacToolkit

/*
 ********************************************************************************
 *************************** CascadeMenuItem_MacUI_Native **********************
 ********************************************************************************
 */
CascadeMenuItem_MacUI_Native::CascadeMenuItem_MacUI_Native (Menu* cascadeMenu):
	CascadeMenuItem_MacUI (),
	fMenuTitle (Nil)
{
	RequireNotNil (cascadeMenu);
	AddSubView (fMenuTitle = new CascadeMenuTitle (cascadeMenu));
}

CascadeMenuItem_MacUI_Native::~CascadeMenuItem_MacUI_Native ()
{
	if (fMenuTitle != Nil) {
		RemoveSubView (fMenuTitle);
	}
	delete fMenuTitle;
}

void	CascadeMenuItem_MacUI_Native::UpdateOSRep_ ()
{
	RequireNotNil (GetOwner ());
	MenuItem::UpdateOSRep_ ();
	osStr255	tmpStr;
	::SetItem (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), GetName ().ToPString (tmpStr, sizeof (tmpStr)));

// quick hack to get things working
	short cmd = 0;
	::GetItemCmd (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), &cmd);
	if (cmd != 0x1b) {
		// must be first time
		::InsertMenu (fMenuTitle->GetMenu ()->GetOSRepresentation (), -1);
		MenuBar_MacUI_Native::sCascadeMenus.Append (fMenuTitle);
		::SetItemCmd (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), 0x1b);
		::SetItemMark (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), (*fMenuTitle->GetMenu ()->GetOSRepresentation ())->menuID);
	}
}

Boolean	CascadeMenuItem_MacUI_Native::IsNativeItem () const
{
	return (True);
}



#elif	qXmToolkit



/*
 ********************************************************************************
 *********************** CascadeMenuItem_MotifUI_Native ************************
 ********************************************************************************
 */

CascadeMenuItem_MotifUI_Native::CascadeMenuItem_MotifUI_Native (Menu* cascadeMenu):
	CascadeMenuItem_MotifUI (),
	fMenuTitle (Nil),
	fWidget (Nil)
{
	RequireNotNil (cascadeMenu);
	Require (cascadeMenu->GetParentView () == Nil);		// not already installed someplaces...
	AddSubView (fMenuTitle = new CascadeMenuTitle (cascadeMenu));
}

CascadeMenuItem_MotifUI_Native::~CascadeMenuItem_MotifUI_Native ()
{
	if (fMenuTitle != Nil) {
		RemoveSubView (fMenuTitle);
	}
	delete fMenuTitle;
}

void	CascadeMenuItem_MotifUI_Native::Realize (osWidget* parent)
{
	RequireNotNil (fMenuTitle);
	RequireNotNil (fMenuTitle->GetMenu ());
	Require (fWidget == Nil);
	
	fMenuTitle->GetMenu ()->Realize (parent);
	Arg	args [1];
	XtSetArg (args[0], XmNsubMenuId, fMenuTitle->GetMenu ()->GetOSRepresentation ());

	char	tmp[1000];
	GetName ().ToCStringTrunc (tmp, sizeof (tmp));

	osWidget* widget = ::XmCreateCascadeButtonGadget (parent, tmp, args, 1);
	::XtManageChild (widget);

	fWidget = widget;
	EnsureNotNil (fWidget);
}

void	CascadeMenuItem_MotifUI_Native::UnRealize ()
{
	RequireNotNil (fMenuTitle);
	RequireNotNil (fMenuTitle->GetMenu ());
	fMenuTitle->GetMenu ()->UnRealize ();
	if (fWidget != Nil) {
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}

osWidget*	CascadeMenuItem_MotifUI_Native::GetWidget () const
{
	return (fWidget);
}

void	CascadeMenuItem_MotifUI_Native::UpdateOSRep_ ()
{
	RequireNotNil (fWidget);
	
	MenuItem::UpdateOSRep_ ();
	char	tmp[1000];
	GetName ().ToCStringTrunc (tmp, sizeof (tmp));
	Arg	args [1];
	XmString mStrTmp = ::XmStringCreate (tmp, XmSTRING_DEFAULT_CHARSET);
	XtSetArg (args[0], XmNlabelString, mStrTmp);
	::XtSetValues (fWidget, args, 1);
	::XmStringFree (mStrTmp);
}

#endif		/*Toolkit*/








/*
 ********************************************************************************
 ******************************** CascadeMenuItem *******************************
 ********************************************************************************
 */
CascadeMenuItem::CascadeMenuItem (Menu* cascadeMenu):
#if		qMacToolkit
	CascadeMenuItem_MacUI_Native (cascadeMenu)
#elif	qXmToolkit
	CascadeMenuItem_MotifUI_Native (cascadeMenu)
#endif
{
}











	
/*
 ********************************************************************************
 ******************************** CascadeMenuTitle ******************************
 ********************************************************************************
 */

CascadeMenuTitle::CascadeMenuTitle (Menu* menu):
	AbstractPullDownMenuTitle (kEmptyString, menu, Nil)
{
	RequireNotNil (menu);
	Assert (menu->GetParentView () == this);		// by now base class MenuTitle:: should have called SetMenu (menu)...

	InstallMenu ();
}

CascadeMenuTitle::~CascadeMenuTitle ()
{
#if		qMacToolkit
	::DeleteMenu ((*GetMenu ()->GetOSRepresentation ())->menuID);
#endif
	DeinstallMenu ();
#if		qMacToolkit
	MenuBar_MacUI_Native::sCascadeMenus.Remove (this);
#endif
}

Boolean	CascadeMenuTitle::DoMenuSelection_ (const Point& startPt, MenuItem*& selected)
{
	AssertNotReached ();
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

