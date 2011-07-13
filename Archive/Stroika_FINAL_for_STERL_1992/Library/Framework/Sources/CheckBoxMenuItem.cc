/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CheckBoxMenuItem.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: CheckBoxMenuItem.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/03  00:21:36  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  07:57:35  sterling
 *		Sterl made lots of changes...
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.13  1992/04/24  08:57:12  lewis
 *		Instead of calling Applcaition::Get().ProcessCommand in motif callback, just set global
 *		varuiable thru MenuOwner;:SetMenuItemSelected (fred);
 *
 *		Revision 1.12  92/04/20  14:32:21  14:32:21  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		
 *		Revision 1.11  92/04/17  19:52:18  19:52:18  lewis (Lewis Pringle)
 *		Hack for popups under motif.
 *		
 *		Revision 1.10  92/04/02  13:07:23  13:07:23  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.9  92/03/10  00:03:28  00:03:28  lewis (Lewis Pringle)
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *		
 *		Revision 1.8  1992/03/02  15:40:46  lewis
 *		Cleanups, and fix includes for mac.
 *
 *		Revision 1.7  1992/02/21  20:12:47  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.6  1992/02/14  03:03:12  lewis
 *		Use GetOwner () instead of (now defunct) fNativeOwner.
 *
 *		Revision 1.5  1992/02/06  21:15:04  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.4  1992/02/04  16:55:21  lewis
 *		Override IsNativeItem for CheckBoxMenuItem native for mac so we can tell its a native item in
 *		AbstractMenu.
 *
 *		Revision 1.3  1992/02/04  05:03:12  lewis
 *		Added destructor to check that fWidget == Nil, for ~CheckBoxMenuItem_MotifUI_Native ().
 *
 *		Revision 1.2  1992/02/04  03:48:21  lewis
 *		Add native and gui intermediate classes, and CheckBoxMenuItem derived class to replace old
 *		#define/typedef.
 *
 *		Revision 1.1  1992/02/03  22:30:35  lewis
 *		Initial revision
 *
 *
 */



#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Menus.h>
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Xm.h>
#include	<Xm/ToggleBG.h>
#endif	/*GDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Menu.hh"
#include	"MenuOwner.hh"
#include	"StringMenuItem.hh"

#include	"CheckBoxMenuItem.hh"







/*
 ********************************************************************************
 ************************** AbstractCheckBoxMenuItem ****************************
 ********************************************************************************
 */
AbstractCheckBoxMenuItem::AbstractCheckBoxMenuItem (CommandNumber commandNumber):
	MenuItem (commandNumber)
{
}

void	AbstractCheckBoxMenuItem::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	MenuItem::SetOn_ (on, updateMode);
//	PlusChangeCount ();
}








/*
 ********************************************************************************
 **************************** CheckBoxMenuItem_MacUI ***************************
 ********************************************************************************
 */
CheckBoxMenuItem_MacUI::CheckBoxMenuItem_MacUI (CommandNumber commandNumber):
	AbstractCheckBoxMenuItem (commandNumber)
{
}









/*
 ********************************************************************************
 **************************** CheckBoxMenuItem_MotifUI *************************
 ********************************************************************************
 */

CheckBoxMenuItem_MotifUI::CheckBoxMenuItem_MotifUI (CommandNumber commandNumber):
	AbstractCheckBoxMenuItem (commandNumber)
{
}









#if		qMacToolkit

/*
 ********************************************************************************
 ************************ CheckBoxMenuItem_MacUI_Native ************************
 ********************************************************************************
 */
CheckBoxMenuItem_MacUI_Native::CheckBoxMenuItem_MacUI_Native (CommandNumber commandNumber):
	CheckBoxMenuItem_MacUI (commandNumber)
{
}

void	CheckBoxMenuItem_MacUI_Native::Draw (const Region& /*update*/)
{
	Coordinate	textV = (GetSize ().GetV () - (GetEffectiveFont ().GetFontHeight ()))/2;
	
	DrawText (GetName (), Point (textV, 0));
}

void	CheckBoxMenuItem_MacUI_Native::UpdateOSRep_ ()
{
	RequireNotNil (GetOwner ());
	CheckBoxMenuItem_MacUI::UpdateOSRep_ ();
	osStr255	tmpStr;
	::SetItem (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), GetName ().ToPString (tmpStr, sizeof (tmpStr)));
	if (GetOn ()) {
		::SetItemMark (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), StringMenuItem_MacUI::eCheckMark);
	}
	else {
		::SetItemMark (GetOwner ()->GetOSRepresentation (), short (GetIndex ()), StringMenuItem_MacUI::eNoMark);
	}
}

Boolean	CheckBoxMenuItem_MacUI_Native::IsNativeItem () const
{
	return (True);
}


#elif	qXmToolkit




/*
 ********************************************************************************
 ************************* CheckBoxMenuItem_MotifUI_Native *********************
 ********************************************************************************
 */


static void sCallBackProc (osWidget* /*w*/, caddr_t client_data, caddr_t /*call_data*/)
{
    MenuItem* fred = (MenuItem*)client_data;
    AssertNotNil (fred);
	MenuOwner::SetMenuItemSelected (fred);
}


CheckBoxMenuItem_MotifUI_Native::CheckBoxMenuItem_MotifUI_Native (CommandNumber commandNumber):
	CheckBoxMenuItem_MotifUI (commandNumber),
	fWidget (Nil)
{
}

CheckBoxMenuItem_MotifUI_Native::~CheckBoxMenuItem_MotifUI_Native ()
{
	Require (fWidget == Nil);		// be sure were unrealized
}

void	CheckBoxMenuItem_MotifUI_Native::Realize (osWidget* parent)
{
	RequireNotNil (parent);
	Require (fWidget == Nil);

	char	tmp[1000];
	GetName ().ToCStringTrunc (tmp, sizeof (tmp));

	fWidget = ::XmCreateToggleButtonGadget (parent, tmp, Nil, 0);

	::XtAddCallback (fWidget, XmNvalueChangedCallback, (XtCallbackProc)sCallBackProc, (XtPointer)this);
	::XtManageChild (fWidget);
}

void	CheckBoxMenuItem_MotifUI_Native::UpdateOSRep_ ()
{
	RequireNotNil (fWidget);
	
	CheckBoxMenuItem_MotifUI::UpdateOSRep_ ();
	char	tmp[1000];
	GetName ().ToCStringTrunc (tmp, sizeof (tmp));
	Arg	args [2];
	XmString mStrTmp = XmStringCreate (tmp, XmSTRING_DEFAULT_CHARSET);
	XtSetArg (args[0], XmNlabelString, mStrTmp);
	XtSetArg (args[1], XmNset, GetOn ());
	::XtSetValues (fWidget, args, 2);
	::XmStringFree (mStrTmp);
}

void	CheckBoxMenuItem_MotifUI_Native::UnRealize ()
{
	if (fWidget != Nil) {
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}

osWidget*	CheckBoxMenuItem_MotifUI_Native::GetWidget () const
{
	return (fWidget);
}

#endif		/*Toolkit*/







/*
 ********************************************************************************
 ********************************* CheckBoxMenuItem *****************************
 ********************************************************************************
 */
CheckBoxMenuItem::CheckBoxMenuItem (CommandNumber commandNumber):
#if		qMacToolkit
	CheckBoxMenuItem_MacUI_Native (commandNumber)
#elif	qXmToolkit
	CheckBoxMenuItem_MotifUI_Native (commandNumber)
#endif
{
}







// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

