/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/StringMenuItem.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: StringMenuItem.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/16  16:59:36  sterling
 *		made portable Mac stuff invert correctly during track
 *
 *		Revision 1.3  1992/07/03  02:17:35  lewis
 *		Scope Mark in class StringMenuItem_MacUI.
 *
 *		Revision 1.2  1992/06/25  08:45:35  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_. And use new font utility for getting line/font height.
 *
 *		Revision 1.21  1992/06/03  21:30:37  lewis
 *		Make AbstractStringMenuItem::GetDisplayName() const.
 *
 *		Revision 1.20  1992/05/18  16:41:17  lewis
 *		Use osMenu instead of osMenuInfo for qMacToolkit.
 *
 *		Revision 1.19  92/04/24  09:09:58  09:09:58  lewis (Lewis Pringle)
 *		use MenuOwner::SetMenuItemSelected (fred); instead of calling app :: processcomamndselction directl.y from
 *		motif callback.
 *		
 *		Revision 1.18  92/04/20  14:25:27  14:25:27  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *		
 *		Revision 1.17  92/04/17  19:53:36  19:53:36  lewis (Lewis Pringle)
 *		Hack for motif popups.
 *		
 *		Revision 1.16  92/04/07  10:30:13  10:30:13  lewis (Lewis Pringle)
 *		Performance tweek based on mac profiling - in StringMenuItem::EffectiveFontChanged() dont bother calling refresh since
 *		we can never be displayed directly anyhow (ie always in a menu).
 *		
 *		Revision 1.15  92/04/02  13:07:36  13:07:36  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.14  92/03/28  03:26:59  03:26:59  lewis (Lewis Pringle)
 *		Override EffectiveFontChanged to inval layouts, etc since we depend on current font.
 *		
 *		Revision 1.12  1992/03/22  21:45:04  lewis
 *		Perform slight performance optimizations in mac specific code.
 *
 *		Revision 1.11  1992/03/10  00:03:46  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.9  1992/02/28  15:19:45  lewis
 *		Trimmed include files.
 *
 *		Revision 1.8  1992/02/15  00:39:29  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workarounds.
 *
 *		Revision 1.7  1992/02/14  03:29:05  lewis
 *		Use GetOwner () instead of (now defunct) fNativeOwner.
 *
 *		Revision 1.6  1992/02/06  21:26:57  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.4  1992/02/04  17:09:25  lewis
 *		Override IsNativeItem () in StringMenuItem_MacUI_Native to say that we are a native item.
 *		Also, fixed some typos made lastnight on mac code - should try to test mac code again soon!!!
 *
 *		Revision 1.3  1992/02/04  05:06:02  lewis
 *		Added dtor ~StringMenuItem_MotifUI_Native () to require we've been unrealized.
 *
 *		Revision 1.2  1992/02/04  03:51:35  lewis
 *		Add native / intermediate gui classes, and StringMenuItem derived class to replace old #define/
 *		typedef mechanism.
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
#elif	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/PushBG.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Menu.hh"

#include	"StringMenuItem.hh"





// for new motif callback hack...
#include "MenuOwner.hh"





/*
 ********************************************************************************
 *************************** AbstractStringMenuItem *****************************
 ********************************************************************************
 */
AbstractStringMenuItem::AbstractStringMenuItem (CommandNumber commandNumber):
	MenuItem (commandNumber),
	fExtended (False)
{
}
	
String	AbstractStringMenuItem::GetDisplayName () const
{
	String	text = GetName ();
	if (GetExtended ()) {
#if		qMacUI && qMacToolkit
		text += "\311";
#else
		text += "...";
#endif
	}
	return (text);
}

Point	AbstractStringMenuItem::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	const Font&	f = GetEffectiveFont ();
	return (Point (f.GetFontHeight (), TextWidth (GetDisplayName (), f, kZeroPoint)));
}

Boolean	AbstractStringMenuItem::GetExtended () const
{
	return (fExtended);
}	

void	AbstractStringMenuItem::SetExtended (Boolean extendedName)
{
	if (GetExtended () != extendedName) {
		SetExtended_ (extendedName);
	}
	Ensure (GetExtended () == extendedName);
}

void	AbstractStringMenuItem::SetExtended_ (Boolean extendedName)
{
	Require (fExtended != extendedName);
	fExtended = extendedName;
	PlusChangeCount ();
}











/*
 ********************************************************************************
 **************************** StringMenuItem_MacUI *****************************
 ********************************************************************************
 */
StringMenuItem_MacUI::StringMenuItem_MacUI (CommandNumber commandNumber):
	AbstractStringMenuItem (commandNumber)
{
}

StringMenuItem_MacUI::Mark	StringMenuItem_MacUI::GetMark () const
{
	return (GetMark_ ());
}	

void	StringMenuItem_MacUI::SetMark (Mark mark)
{
	if (GetMark () != mark) {
		SetMark_ (mark);
	}
	Ensure (GetMark () == mark);
}
		





/*
 ********************************************************************************
 ****************************** StringMenuItem_MotifUI *************************
 ********************************************************************************
 */


StringMenuItem_MotifUI::StringMenuItem_MotifUI (CommandNumber commandNumber):
	AbstractStringMenuItem (commandNumber)
{
}












#if		qMacToolkit

/*
 ********************************************************************************
 ************************* StringMenuItem_MacUI_Native *************************
 ********************************************************************************
 */
StringMenuItem_MacUI_Native::StringMenuItem_MacUI_Native (CommandNumber commandNumber):
	StringMenuItem_MacUI (commandNumber),
	fMark (eNoMark)
{
	SetFont (&kSystemFont);
}

void	StringMenuItem_MacUI_Native::Draw (const Region& /*update*/)
{
	const Font&	f = GetEffectiveFont ();
	Coordinate	textV = (GetSize ().GetV () - (f.GetFontHeight ()))/2;
	
	DrawText (GetDisplayName (), Point (textV, 0));
	if (GetOn ()) {
		Invert ();
	}
	if (not GetEffectiveLive ()) {
		Gray ();
	}
}

StringMenuItem_MacUI::Mark	StringMenuItem_MacUI_Native::GetMark_ () const
{
	return (fMark);
}	

void	StringMenuItem_MacUI_Native::SetMark_ (StringMenuItem_MacUI::Mark mark)
{
	fMark = mark;
	PlusChangeCount ();
}

void	StringMenuItem_MacUI_Native::UpdateOSRep_ ()
{
	Menu*	menu	=	GetOwner ();
	RequireNotNil (menu);
	StringMenuItem_MacUI::UpdateOSRep_ ();
	osStr255	tmpStr;
	osMenu**	theOSMenu	=	menu->GetOSRepresentation ();
	::SetItem (theOSMenu, short (GetIndex ()), GetDisplayName ().ToPString (tmpStr, sizeof (tmpStr)));
	::SetItemMark (theOSMenu, short (GetIndex ()), fMark);
}

#if		qMacToolkit
Boolean	StringMenuItem_MacUI_Native::IsNativeItem () const
{
	return (True);
}
#endif

void	StringMenuItem_MacUI_Native::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	StringMenuItem_MacUI::SetOn_ (on, updateMode);
	if (updateMode == eImmediateUpdate) {
		Invert ();
	}
	else {
		Refresh (updateMode);
	}
}

Boolean	StringMenuItem_MacUI_Native::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	Invert ();
	return (StringMenuItem_MacUI::Track (phase, mouseInButton));
}

void	StringMenuItem_MacUI_Native::EffectiveFontChanged (const Font& newFont, UpdateMode /*updateMode*/)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and COULD call Refresh () since our visual display depends on the current font.
	 * However, we don't bother calling refresh, since these guys cannot be displayed directly anyhow.
	 */
	StringMenuItem_MacUI::EffectiveFontChanged (newFont, eNoUpdate);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}
}


#elif	qXmToolkit



/*
 ********************************************************************************
 ************************** StringMenuItem_MotifUI_Native **********************
 ********************************************************************************
 */


static void sCallBackProc (osWidget* /*w*/, void* client_data, void* /*call_data*/)
{
    MenuItem* fred = (MenuItem*)client_data;
    AssertNotNil (fred);
	MenuOwner::SetMenuItemSelected (fred);
}

StringMenuItem_MotifUI_Native::StringMenuItem_MotifUI_Native (CommandNumber commandNumber):
	StringMenuItem_MotifUI (commandNumber),
	fWidget (Nil)
{
}

StringMenuItem_MotifUI_Native::~StringMenuItem_MotifUI_Native ()
{
	Require (fWidget == Nil);		// we must be unrealized...
}

void	StringMenuItem_MotifUI_Native::UpdateOSRep_ ()
{
	RequireNotNil (fWidget);
	
	StringMenuItem_MotifUI::UpdateOSRep_ ();
	char	tmp[1000];
	GetDisplayName ().ToCStringTrunc (tmp, sizeof (tmp));

	Arg	args [2];
	XmString mStrTmp = XmStringCreate (tmp, XmSTRING_DEFAULT_CHARSET);
	XtSetArg (args[0], XmNlabelString, mStrTmp);
	XtSetArg (args[1], XmNset, GetOn ());
	::XtSetValues (fWidget, args, 2);
	::XmStringFree (mStrTmp);
}

void	StringMenuItem_MotifUI_Native::Realize (osWidget* parent)
{
	RequireNotNil (parent);
	Require (fWidget == Nil);

	char	tmp[1000];
	GetDisplayName ().ToCStringTrunc (tmp, sizeof (tmp));

	fWidget = ::XmCreatePushButtonGadget (parent, tmp, Nil, 0);
	AssertNotNil (fWidget);
	::XtAddCallback (fWidget, XmNactivateCallback, (XtCallbackProc)sCallBackProc, (XtPointer)this);
	::XtManageChild (fWidget);
}

void	StringMenuItem_MotifUI_Native::UnRealize ()
{
	if (fWidget != Nil) {
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}

osWidget*	StringMenuItem_MotifUI_Native::GetWidget () const
{
	return (fWidget);
}

void	StringMenuItem_MotifUI_Native::EffectiveFontChanged (const Font& newFont, UpdateMode updateMode)
{
	/*
	 * On font changes, we invalidate parents layout since our CalcDefaultSize () changes,
	 * and Refresh () since our visual display depends on the current font.
	 */

	StringMenuItem_MotifUI::EffectiveFontChanged (newFont, updateMode);

	View*	parent	=	GetParentView ();
	if (parent != Nil) {
		parent->InvalidateLayout ();		// cuz our CalcDefaultSize () may change
	}

	Refresh (updateMode);					// cuz our display may change
}


#endif		/*Toolkit*/









/*
 ********************************************************************************
 *********************************** StringMenuItem *****************************
 ********************************************************************************
 */
StringMenuItem::StringMenuItem (CommandNumber commandNumber):
#if		qMacToolkit
	StringMenuItem_MacUI_Native (commandNumber)
#elif	qXmToolkit
	StringMenuItem_MotifUI_Native (commandNumber)
#endif
{
}







// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

