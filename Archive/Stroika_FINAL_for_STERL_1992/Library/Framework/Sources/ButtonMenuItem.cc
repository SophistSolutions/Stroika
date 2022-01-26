/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ButtonMenuItem.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *		-	Probably should be renamed ToggleMenuItem...Talk to sterl first about new toggle class?
 *
 * Changes:
 *	$Log: ButtonMenuItem.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/03  00:19:50  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  07:50:46  sterling
 *		Lots of changes including from Button based to Toggle based. Probably should rename
 *		to ToggleMenuItem. (LGP checked in so not quite sure what changed - lots).
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/04/20  14:22:48  lewis
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *		Also, got rid of other caddr_t usage for Xt callbacks.
 *
 *		Revision 1.11  92/04/09  02:26:36  02:26:36  lewis (Lewis Pringle)
 *		Call inherited realize in ButtonMenuItem_MotifUI since we could have subview which was motif
 *		widget. Also, we called inherited in our unrealize call.
 *		
 *		Revision 1.10  92/04/02  13:07:20  13:07:20  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.9  92/04/02  11:40:57  11:40:57  lewis (Lewis Pringle)
 *		OOps - serious bug - too bad it lasted so long - forgot to initialize fWidget (Nil) in ButtonMenuItem_MotifUI.
 *		Caused random Xt crahses!!!
 *		
 *		Revision 1.7  1992/03/10  00:02:45  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.5  1992/02/17  05:11:07  lewis
 *		Made callback proc member function for ButtonMenuItem_MotifUI_Native.
 *		Also, call inherited unrealize in ButtonMenuItem_MotifUI_Native::Realize since we may have
 *		a button with a widget.
 *
 *		Revision 1.4  1992/02/15  00:33:09  lewis
 *		Get rid of workarounds for qMPW_SymbolTableOverflowProblem since apple fixed the bug.
 *		Also, add overrides for Realize, etc, and build DrawnButton widget.
 *
 *		Revision 1.3  1992/02/06  21:06:25  lewis
 *		Added workaround for qMPW_SymbolTableOverflowProblem bug.
 *
 *		Revision 1.2  1992/02/04  03:46:02  lewis
 *		Add native and intermediate buttonmenuitem classes, and class ButtonMenuItem to replacer
 *		#defines and typedefs.
 *
 *		Revision 1.1  1992/02/03  22:30:35  lewis
 *		Initial revision
 *
 *
 */



#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacOS
#include	<Events.h>				// just for cmdKey
#include	<Memory.h>
#include	<Menus.h>
#include	<OSUtils.h>				// for trap caching
#include	<Resources.h>			// to snag default mdef
#include	<Traps.h>				// for trap caching
#include	<ToolUtils.h>			// just for HiWord/LoWord
#include	<QuickDraw.h>
#include	<Windows.h>
#include	<SysEqu.h>				// for GrayRgn
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Xm.h>
#include	<Xm/DrawnB.h>		// hack for now...
#endif	/*GDI*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"ButtonMenuItem.hh"









/*
 ********************************************************************************
 ***************************** AbstractButtonMenuItem ***************************
 ********************************************************************************
 */
AbstractButtonMenuItem::AbstractButtonMenuItem (Toggle* button, CommandNumber commandNumber):
	MenuItem (commandNumber),
	fButton (Nil)
{
	SetButton (button);
}

Point	AbstractButtonMenuItem::CalcDefaultSize_ (const Point& defaultSize) const
{
	RequireNotNil (fButton);
	return (fButton->CalcDefaultSize (defaultSize));
}

Toggle*	AbstractButtonMenuItem::GetButton ()
{
	return (fButton);
}

void	AbstractButtonMenuItem::SetButton (Toggle* button)
{
	if (fButton != button) {
		SetButton_ (button);
	}
	Ensure (fButton == button);
}

void	AbstractButtonMenuItem::SetButton_ (Toggle* button)
{
	if (fButton != Nil) {
		RemoveSubView (fButton);
AssertNotReached ();		// trouble is that assumes heap based and directly alloced.
// Since were moving away from that assumption in view, we should elsewhere!!!
// LGP Feb 27, 1992
		delete fButton;
		fButton = Nil;
	}
	fButton = button;
	if (fButton != Nil) {
		AddSubView (fButton);
	}
}

void	AbstractButtonMenuItem::SetEnabled_ (Boolean enabled, Panel::UpdateMode updateMode)
{
	RequireNotNil (fButton);
	MenuItem::SetEnabled_ (enabled, updateMode);
	fButton->SetEnabled (enabled, eNoUpdate);
}

void	AbstractButtonMenuItem::SetOn_ (Boolean on, Panel::UpdateMode updateMode)
{
	RequireNotNil (fButton);
	MenuItem::SetOn_ (on, updateMode);
	fButton->SetOn (on, updateMode);
}

void	AbstractButtonMenuItem::Layout ()
{
	RequireNotNil (fButton);
	Assert (fButton->GetOrigin () == kZeroPoint);
	fButton->SetSize (GetSize ());
	MenuItem::Layout ();
}

void	AbstractButtonMenuItem::UpdateOSRep_ ()
{
	MenuItem::UpdateOSRep_ ();
	RequireNotNil (fButton);
	fButton->Refresh ();
}









/*
 ********************************************************************************
 **************************** ButtonMenuItem_MacUI *****************************
 ********************************************************************************
 */
ButtonMenuItem_MacUI::ButtonMenuItem_MacUI (Toggle* button, CommandNumber commandNumber):
	AbstractButtonMenuItem (button, commandNumber)
{
}








/*
 ********************************************************************************
 ****************************** ButtonMenuItem_MotifUI *************************
 ********************************************************************************
 */
ButtonMenuItem_MotifUI::ButtonMenuItem_MotifUI (Toggle* button, CommandNumber commandNumber):
	AbstractButtonMenuItem (button, commandNumber)
{
}








#if		qMacToolkit
/*
 ********************************************************************************
 ************************ ButtonMenuItem_MacUI_Native **************************
 ********************************************************************************
 */
ButtonMenuItem_MacUI_Native::ButtonMenuItem_MacUI_Native (Toggle* button, CommandNumber commandNumber):
	ButtonMenuItem_MacUI (button, commandNumber)
{
}


#elif	qXmToolkit




/*
 ********************************************************************************
 ************************** ButtonMenuItem_MotifUI_Native **********************
 ********************************************************************************
 */



void	ButtonMenuItem_MotifUI_Native::sCallBackProc (osWidget* w, void* client_data, void* call_data)
{
    RequireNotNil (w);
    RequireNotNil (client_data);
    RequireNotNil (call_data);
    ButtonMenuItem_MotifUI_Native*		fred		= (ButtonMenuItem_MotifUI_Native*)client_data;
	XmDrawnButtonCallbackStruct&		call_value	=	*(XmDrawnButtonCallbackStruct*)call_data;

	switch (call_value.reason) {
		case	XmCR_EXPOSE:		fred->Render (fred->GetLocalExtent ()); break;
#if		qDebug
		case	XmCR_ARM:			gDebugStream << "arm" << newline; break;
		case	XmCR_ACTIVATE:		gDebugStream << "XmCR_ACTIVATE" << newline; break;
		case	XmCR_DISARM:		gDebugStream << "XmCR_DISARM" << newline; break;
#endif
	}
}


ButtonMenuItem_MotifUI_Native::ButtonMenuItem_MotifUI_Native (Toggle* button, CommandNumber commandNumber):
	ButtonMenuItem_MotifUI (button, commandNumber),
	fWidget (Nil)
{
}

ButtonMenuItem_MotifUI_Native::~ButtonMenuItem_MotifUI_Native ()
{
	Require (fWidget == Nil);		// we must be unrealized...
}

void	ButtonMenuItem_MotifUI_Native::UpdateOSRep_ ()
{
	RequireNotNil (fWidget);
	
	ButtonMenuItem_MotifUI::UpdateOSRep_ ();
	Arg	args [3];
	XtSetArg (args[0], XmNset, GetOn ());
	XtSetArg (args[1], XmNwidth, GetSize ().GetH ());
	XtSetArg (args[2], XmNheight, GetSize ().GetV ());
	::XtSetValues (fWidget, args, 3);
}

void	ButtonMenuItem_MotifUI_Native::Realize (osWidget* parent)
{
	RequireNotNil (parent);
	Require (fWidget == Nil);

	fWidget = ::XmCreateDrawnButton (parent, "ButtonMenuItem_MotifUI_Native", Nil, 0);
	AssertNotNil (fWidget);
	::XtAddCallback (fWidget, XmNactivateCallback, (XtCallbackProc)sCallBackProc, (XtPointer)this);
	::XtAddCallback (fWidget, XmNarmCallback, (XtCallbackProc)sCallBackProc, (XtPointer)this);
	::XtAddCallback (fWidget, XmNdisarmCallback, (XtCallbackProc)sCallBackProc, (XtPointer)this);
	::XtAddCallback (fWidget, XmNexposeCallback, (XtCallbackProc)sCallBackProc, (XtPointer)this);
	::XtManageChild (fWidget);
	ButtonMenuItem_MotifUI::Realize (fWidget);		// call inherited since our button subview could be a widget!!!
}

void	ButtonMenuItem_MotifUI_Native::UnRealize ()
{
	ButtonMenuItem_MotifUI::UnRealize ();		// call inherited since our button subview could be a widget!!!
	if (fWidget != Nil) {
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}

osWidget*	ButtonMenuItem_MotifUI_Native::GetWidget () const
{
	return (fWidget);
}

#endif		/*Toolkit*/











/*
 ********************************************************************************
 ********************************* ButtonMenuItem *******************************
 ********************************************************************************
 */
ButtonMenuItem::ButtonMenuItem (Toggle* button, CommandNumber commandNumber):
#if		qMacToolkit
	ButtonMenuItem_MacUI_Native (button, commandNumber)
#elif	qXmToolkit
	ButtonMenuItem_MotifUI_Native (button, commandNumber)
#endif
{
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

