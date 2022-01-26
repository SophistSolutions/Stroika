/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Shell.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Shell.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/16  16:57:49  sterling
 *		hachacked windoid stuff to work (as standard style window -- sigh)
 *
 *		Revision 1.2  1992/06/25  08:38:11  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_. And did set backgrounds (not sure why/where).
 *
 *		Revision 1.31  92/05/13  13:03:29  13:03:29  lewis (Lewis Pringle)
 *		STERL - questionable change to ZoomEvent stuff on mac - old stuff maybe wrong but I think its a bad idea
 *		to call SetSize/SetOrigin - better to call SetExtent () since we could there optimize draw to be less flickery.
 *		Whole are of Shell stuff needs lots of work.
 *
 *		Revision 1.30  92/04/20  14:24:26  14:24:26  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *
 *		Revision 1.29  92/04/10  02:44:08  02:44:08  lewis (Lewis Pringle)
 *		Heavy hacking to force proper placement of dialogs, and alerts. This code is in major need of cleanup.
 *
 *		Revision 1.28  92/04/02  13:07:35  13:07:35  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *
 *		Revision 1.27  92/04/02  11:42:09  11:42:09  lewis (Lewis Pringle)
 *		Use gDisplay rather than Tablet::GetOSDisplay ().
 *
 *		Revision 1.26  92/03/26  09:51:12  09:51:12  lewis (Lewis Pringle)
 *		Made GetVisibleArea () const.
 *
 *		Revision 1.24  1992/02/18  03:55:31  lewis
 *		Added support for about box menu item in system menu for UNIX.
 *
 *		Revision 1.23  1992/02/12  07:15:27  lewis
 *		Use GDIConfig rather than OSConfig.
 *
 *		Revision 1.22  1992/02/11  01:21:23  lewis
 *		Fixed zoomwindow.
 *
 *		Revision 1.21  1992/02/01  19:32:23  lewis
 *		Use XtAppCreateShell instead of XtCreateWidget, since then we dont need to specify a parent. That
 *		fixed the problem that resizing the main app shell, resized the other shells, but caused problems
 *		with making the shells visible/invisible. The fix/workaround to that was set MappedWhenManaged to FALSE
 *		before realizing them, and then make them visible/invisble with XtMap/UnMap Widget.
 *		Also, got rid of use of global varisbles gShell, and gOSDisplay (using Tablet::GetOSDisplay () instead for display).
 *
 *		Revision 1.20  1992/01/31  22:53:08  lewis
 *		Call XtMap/Unmap widget for the shells rather than managetchild/unmanagchild
 *		since managing also makes the toevel shell resize its children when it
 *		is resized.
 *
 *		Revision 1.19  1992/01/29  15:09:49  lewis
 *		Put back comment that sterl deleted, and fixed bug with setting desired size for main window shells.
 *		Code is still unsafe, in that a zero size for desired size will cause a crash... Also, Sterls changes
 *		to inherit from ImplementationShell are a VERY bad idea. Using inheritance to help out slightly in the
 *		implementation is just asking for trouble!!! Sad.  Also, he used this pointer in an illicit maner before
 *		being constucted, and its just dumb-luck it doesnt crash horribly - must get rid of his GetDefaultHints(),
 *		or whatever - cannot do things that way!!!
 *
 *		Revision 1.18  1992/01/29  07:33:53  lewis
 *		Fixed MainAppWindow after sterl merge to keep around class and app name in instance variables til we need them in
 *		BuildOSRep call.
 *
 *		Revision 1.17  1992/01/29  04:55:59  sterling
 *		massive changes
 *
 *		Revision 1.16  1992/01/27  23:37:25  lewis
 *		Added, then commented out since it didn't seem necessary, code to SetWMProtocols for WM_DELETE_WINDOW client message.
 *
 *		Revision 1.15  1992/01/27  04:14:43  sterling
 *		minor Mac fixups, still needs work
 *
 *		Revision 1.14  1992/01/20  14:15:48  lewis
 *		Worked on ImplementationShell::SetWindowShellHints () - made it not reset the origin/width every time
 *		you reset any attribute - like title.
 *
 *		Revision 1.12  1992/01/19  21:29:56  lewis
 *		Added StandardAlertWindowShell.
 *		Added XtSetargs for the various window hints in ImplemtantionWindowShell.
 *
 *		Revision 1.10  1992/01/18  09:12:47  lewis
 *		Moved  window hints stuff to Implementation Shell. Did inserter for WindowShellHints.
 *		Re-organized how we do window placement - still needs some work.
 *
 *		Revision 1.9  1992/01/08  06:35:43  lewis
 *		Sterl - fixed bug with ImplementationShell:GetOrigin () - someplace - I think TextEdit we do a Toolbox
 *		SetOrigin() call and that is bad - or at least caused trouble here so wsaved old origin, and then restored
 *		it after the call to LocalToGlobal...
 *
 *		Revision 1.8  1992/01/07  04:19:43  lewis
 *		Fixed mac bug in ImplementationShell:SetVisible ().
 *
 *		Revision 1.7  1992/01/07  00:49:26  lewis
 *		Fixed GetContentOrigin for Xt (use XTranslateCoords - not get geometry - since we are not directly owned
 *		by the root window.
 *		Also, Add GetLocalContentRegion and GetLocalContentOrigin () to avoid a race condition in the conmputation
 *		of these values (GetContentOrigin () then GlobalToLocal () is a race - since we re-get the value of
 *		the origin used in both functions each time - the server may (and often does) provide a different value).
 *
 *		Revision 1.6  1992/01/06  10:07:09  lewis
 *		Support GetReallyVisible.
 *
 *		Revision 1.5  1992/01/04  07:41:31  lewis
 *		Alot of work offloading more window functionality to shell, and
 *		getting window to not subclass from View. Also started work on
 *		new placement stragegy.
 *		Lost changes made on UNIX side for hacking desiredSize for dialogs -they
 *		should not be necessary anymore and if they are, I'd better debug
 *		the problems - not just hack around them...
 *
 *		Revision 1.4  1992/01/03  07:18:42  lewis
 *		Hacked desiredsize of shell so we can get alerts up under unix - just a hack and it will go away with the
 *		new stragy for placewindows (as soon as I integrate with the mac code).
 *	
 *		Revision 1.3  1991/12/27  17:02:31  lewis
 *		Get mostly working on mac/unix.
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Memory.h>
#include	<QuickDraw.h>
#include	<SysEqu.h>
#include	<ToolUtils.h>
#include	<Windows.h>
#elif	qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
// not really right - really motif toolkit?
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/IntrinsicP.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<Xm/Xm.h>
#include	<Xm/Protocols.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"
#include    "Pallet.hh"
#include	"Region.hh"
#include	"Sound.hh"
#include	"Tablet.hh"

#include	"Application.hh"
#include	"Command.hh"
#include	"DeskTop.hh"
#include	"MenuCommandHandler.hh"
#include	"MenuOwner.hh"
#include	"WindowPlane.hh"

#include	"Shell.hh"









/*
 *		What these methods do is genearlly trival wrappers on the toolkit, and very little magic
 *	to make a common interface. For example, since there is no striaght forward way on the mac to
 *	specify the origin of a window (just the origin of its content region), we make that the interface
 *	here in ImplementationWindowShell. Simmilary for the size. That may be different under Xt, as convient.
 *
 */
 
/*
 ********************************************************************************
 ****************************** ImplementationWindowShell ***********************
 ********************************************************************************
 */


#if		qXtToolkit
static	PixelMap	GetDefaultIcon ()
{
	static	UInt16	kDefaultIcon_Data [] = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
	};
	static	const PixelMap	kDefaultIcon	=	PixelMap (Rect (kZeroPoint, Point (16, 16)), kDefaultIcon_Data);
	return (kDefaultIcon);
}
#endif



ImplementationWindowShell::ImplementationWindowShell (const WindowShellHints& hints) :
	fWindowShellHints (hints),
	fWindow (Nil),
	fWindowPlane (Nil),
	fTopLeftBorder (kZeroPoint),
	fBotRightBorder (kZeroPoint),
	fVisible (False),
#if		qMacToolkit
	fOSWindow (Nil)
#elif	qXtToolkit
	fShellWidget (Nil)
#endif	/*qGDI*/
{
}

ImplementationWindowShell::~ImplementationWindowShell ()
{
#if		qMacToolkit
	::DisposeWindow (GetOSRepresentation ());
#elif	qXGDI
//	fMainViewWidget = Nil;		// should do some delete call!!!
#endif	/*qMacUI && qNativeAvailable*/
	
	DeskTop::Get ().SynchronizeOSWindows ();			// not sure needed here.??? and even so only on mac...
}

#if		qMacToolkit

void	ImplementationWindowShell::SetOSRepresentation (osGrafPort* theOSWindow, const Point& topLeftBorder)
{
	RequireNotNil (theOSWindow);
	Require (fOSWindow == Nil);

	fTopLeftBorder = topLeftBorder;
	fOSWindow = theOSWindow;
}

#elif	qXtToolkit

void	ImplementationWindowShell::SetOSRepresentation (osWidget* theOSWindow)
{
	RequireNotNil (theOSWindow);
	Require (fShellWidget == Nil);
	
	fShellWidget = theOSWindow;

	Point	desiredContentSize		=	GetWindowShellHints ().GetDesiredSize ();
	Arg	args [4];
	XtSetArg (args[0], XtNwidth, desiredContentSize.GetH ());
	XtSetArg (args[1], XtNheight, desiredContentSize.GetV ());
	XtSetArg (args[2], XtNmappedWhenManaged, False);			// hack to make window INVISIBLE when realized.
																// this hack is totally undocumented, and I've found
																// no documented way to do this...
	XtSetArg (args[3], XmNdeleteResponse, XmDO_NOTHING);	// we handle it elsewhere...
	::XtSetValues (fShellWidget, args, 4);
	::XtRealizeWidget (fShellWidget);		// not sure necessary - others dont do it...
}

#endif	/*qGDI*/

Window&		ImplementationWindowShell::GetWindow () const
{
	RequireNotNil (fWindow);
	return (*fWindow);
}

WindowPlane&	ImplementationWindowShell::GetWindowPlane () const
{
	RequireNotNil (fWindowPlane);
	return (*fWindowPlane);
}

void	ImplementationWindowShell::SetWindowPlane (WindowPlane* windowPlane)
{
	RequireNotNil (windowPlane);
	Require (fWindowPlane == Nil);
	fWindowPlane = windowPlane;
}

WindowShellHints	ImplementationWindowShell::GetWindowShellHints () const
{
	return (fWindowShellHints);
}

void	ImplementationWindowShell::SetWindowShellHints (const WindowShellHints& hints)
{
	fWindowShellHints = hints;
}

void	ImplementationWindowShell::ApplyWindowShellHints (WindowShellHints& hints)
{
	RequireNotNil (GetOSRepresentation ());

	hints.SetTitle (SetTitle_ (hints.GetTitle ()));

#if		qXtToolkit
	// important to get shell set to some reasonable size before calling setorigin or setsize
	Arg	args [8];
	XtSetArg (args[0], XtNx, hints.GetDesiredOrigin ().GetH ());
	XtSetArg (args[1], XtNy, hints.GetDesiredOrigin ().GetV ());
	XtSetArg (args[2], XtNminWidth, hints.GetMinSize ().GetH ());
	XtSetArg (args[3], XtNminHeight, hints.GetMinSize ().GetV ());
	XtSetArg (args[4], XtNmaxWidth, hints.GetMaxSize ().GetH ());
	XtSetArg (args[5], XtNmaxHeight, hints.GetMaxSize ().GetV ());
	XtSetArg (args[6], XtNwidth, hints.GetDesiredSize ().GetH ());
	XtSetArg (args[7], XtNheight, hints.GetDesiredSize ().GetV ());
	::XtSetValues (GetOSRepresentation (), args, 8);
#else
	SetOrigin (hints.GetDesiredOrigin ());	
	SetSize (hints.GetDesiredSize ());
#endif

}

Point	ImplementationWindowShell::GetOrigin () const
{
#if		qXtToolkit
	((ImplementationWindowShell*)this)->CheckBorders ();			// cast away const to fill cache...
#endif	/*GDI*/
	return (GetContentOrigin () - fTopLeftBorder);
}

void	ImplementationWindowShell::SetOrigin (const Point& newOrigin)
{
#if		qXtToolkit
	((ImplementationWindowShell*)this)->CheckBorders ();			// cast away const to fill cache...
#endif	/*GDI*/
	SetContentOrigin (newOrigin + fTopLeftBorder);
#if		qDebug && qXtToolkit && 0
	gDebugStream << "after ImplementationWindowShell::SetOrigin (" << newOrigin << "), with fTopLeftBorder = " << fTopLeftBorder << newline;
#endif
}

Point	ImplementationWindowShell::GetSize () const
{
#if		qXtToolkit
	((ImplementationWindowShell*)this)->CheckBorders ();			// cast away const to fill cache...
#endif	/*GDI*/
	return (GetContentSize () + fTopLeftBorder + fBotRightBorder);
}

void	ImplementationWindowShell::SetSize (const Point& newSize)
{
#if		qXtToolkit
	((ImplementationWindowShell*)this)->CheckBorders ();			// cast away const to fill cache...
#endif	/*GDI*/
	SetContentSize (newSize - fTopLeftBorder - fBotRightBorder);
}

Rect	ImplementationWindowShell::GetExtent () const
{
	return (Rect (GetOrigin (), GetSize ()));
}

void	ImplementationWindowShell::SetExtent (const Rect& extent)
{
	SetOrigin (extent.GetOrigin ());
	SetSize (extent.GetSize ());
}

Point	ImplementationWindowShell::GetContentSize () const
{
#if		qMacToolkit
	Point	size	=	os_cvt (GetOSRepresentation ()->portRect).GetSize ();
	Ensure (size.GetV () >= 0);
	Ensure (size.GetH () >= 0);
	return (size);
#elif	qXtToolkit
	unsigned	int	height;
	unsigned	int	width;

	if (XtIsRealized (GetOSRepresentation ())) {
		osWindow		root;
		int				x;
		int				y;
		unsigned	int	borderWidth;
		unsigned	int	depth;
		/*
		 * Note that the width/height is of the content area for this function - Orielly II-221
		 */
		if (! ::XGetGeometry (XtDisplay (GetOSRepresentation ()), XtWindow (GetOSRepresentation ()),
			&root, &x, &y, &width, &height, &borderWidth, &depth) ) {
			AssertNotReached ();		// should raise xectpion on error!!!
		}
	}
	else {
		Arg sizeArgs[2];
		XtSetArg (sizeArgs[0], XtNwidth, &width);
		XtSetArg (sizeArgs[1], XtNheight, &height);
		::XtGetValues (GetOSRepresentation (), sizeArgs, 2);

		Assert (height == GetOSRepresentation ()->core.height);
		Assert (width == GetOSRepresentation ()->core.width);
	}
	Ensure (height < 10000);		// sanity check...
	Ensure (width  < 10000);		// sanity check...
	return (Point (height, width));
#endif	/*Toolkit*/
}

void	ImplementationWindowShell::SetContentSize (const Point& newSize)
{
	Require (newSize >= kZeroPoint);
#if		qMacToolkit
	::SizeWindow (GetOSRepresentation (), (short) newSize.GetH (), (short) newSize.GetV (), False);
#elif	qXtToolkit
	::XtResizeWidget (GetOSRepresentation (), (int)newSize.GetH (), (int)newSize.GetV (), GetOSRepresentation ()->core.border_width);
#endif	/*Toolkit*/
}

Point	ImplementationWindowShell::GetLocalContentOrigin () const
{
#if		qXtToolkit
	((ImplementationWindowShell*)this)->CheckBorders ();			// cast away const to fill cache...
#endif	/*GDI*/
	return (fTopLeftBorder);
}

Point	ImplementationWindowShell::GetContentOrigin () const
{
#if		qMacToolkit
	osGrafPort*	oldPort;
	::GetPort (&oldPort);
	::SetPort (GetOSRepresentation ());
	osPoint	osp = { 0, 0 };
	// See IM - Vol 1 - Page 166 - This is actually right way to 'GetOrigin ()' for port!!!
	osPoint oldOrigin = *(osPoint*)&qd.thePort->portRect;
	::SetOrigin (0, 0);	// this is the fix, but can I always munge the origin here?
	::LocalToGlobal (&osp);
	::SetOrigin (oldOrigin.h, oldOrigin.v);
	::SetPort (oldPort);
	return (os_cvt (osp));
#elif	qXtToolkit
	int	x	=	0;
	int	y	=	0;
	if (XtIsRealized (GetOSRepresentation ())) {
		osWindow		child		=	0;	// ignored ...
		osWidget*		w		=	GetOSRepresentation ();
		if (! ::XTranslateCoordinates (XtDisplay (w), XtWindow (w), DefaultRootWindow (XtDisplay (w)), 0, 0,
					       &x, &y, &child)) {
			AssertNotReached ();		// should raise xectpion on error!!!
		}
	}
	else {
		Arg sizeArgs[2];
		XtSetArg (sizeArgs[0], XtNx, &x);
		XtSetArg (sizeArgs[1], XtNy, &y);
		::XtGetValues (GetOSRepresentation (), sizeArgs, 2);
	}
	return (Point (y, x));
#endif	/*Toolkit*/
}

void	ImplementationWindowShell::SetContentOrigin (const Point& newOrigin)
{
#if		qMacToolkit
	Assert ((short) newOrigin.GetH () == newOrigin.GetH ());		// no overflow....
	Assert ((short) newOrigin.GetV () == newOrigin.GetV ());		// no overflow....
	::MoveWindow (GetOSRepresentation (), (short) newOrigin.GetH (), (short) newOrigin.GetV (), False);
#elif	qXtToolkit
	::XtMoveWidget (GetOSRepresentation (), (int)newOrigin.GetH (), (int)newOrigin.GetV ());
#endif	/*GDI*/
}

Region	ImplementationWindowShell::GetLocalContentRegion () const
{
	return (Rect (GetLocalContentOrigin (), GetContentSize ()));
}

Region	ImplementationWindowShell::GetContentRegion () const
{
	return (Rect (GetContentOrigin (), GetContentSize ()));			// good enuf for now - for most doc windows - really correct anyway...
}

Boolean	ImplementationWindowShell::GetVisible () const
{
	return (fVisible);
}

Boolean	ImplementationWindowShell::GetReallyVisible () const
{
#if		qMacToolkit
	return Boolean (!!((WindowRecord*)GetOSRepresentation ())->visible);
#elif	qXtToolkit
	return (fVisible);	// really must do some X query here !!!
#endif	/*GDI*/
}

void	ImplementationWindowShell::SetVisible (Boolean visible)
{
	RequireNotNil (fWindow);
	RequireNotNil (fWindowPlane);

	Boolean	visibilityChanged = Boolean (fVisible != visible);
	
	fVisible = visible;

	if (visibilityChanged and fVisible) {
		WindowShellHints	hints = GetWindowShellHints ();
		GetWindow ().AdjustWindowShellHints (hints);
		GetWindowPlane ().AdjustWindowShellHints (hints);
		ApplyWindowShellHints (hints);
		SetWindowShellHints (hints);
	}

#if		qXtToolkit
	//MAYBE SHOULD DO THIS IN DeskTop::Get ().SynchronizeOSWindows ()
	// YES!! Cuz we also have to sync our window list using raisewindow, etc...

	if (visible) {

#if 0
gDebugStream << "hacking mappedWhenManaged" << newline;
Arg	args [1];
XtSetArg (args[0], XtNmappedWhenManaged, True);		// megahack?? Guess why we cannot set origin???
::XtSetValues (GetOSRepresentation (), args, 1);







if (fWindowShellHints.DesiredOriginSet ()) {
// try to force it - somebody's not paying attention!!!
gDebugStream << "trhing to force origin before map " << newline;
	SetOrigin (fWindowShellHints.GetDesiredOrigin ());
}

gDebugStream << "Calling XtMapWidget" << newline;
#endif
		XtMapWidget (fShellWidget);

#if 0
if (fWindowShellHints.DesiredOriginSet ()) {
// try to force it - somebody's not paying attention!!!
gDebugStream << "trhing to force origin before map " << newline;
	SetOrigin (fWindowShellHints.GetDesiredOrigin ());
}
#endif


if (fWindowShellHints.DesiredOriginSet ()) {
//gDebugStream << "HEAVY GUNNS" << newline;
AssertNotNil (XtWindow (GetOSRepresentation ()));
extern osDisplay* gDisplay;
XMoveWindow (gDisplay, XtWindow (GetOSRepresentation ()),
			 fWindowShellHints.GetDesiredOrigin ().GetH (), 
			 fWindowShellHints.GetDesiredOrigin ().GetV ());
}


	}
	else {
		XtUnmapWidget (fShellWidget);
	}
#endif
	DeskTop::Get ().SynchronizeOSWindows ();			// not sure if we want to do this with X???
}

String	ImplementationWindowShell::GetTitle () const
{
	return (GetWindowShellHints ().GetTitle ());
}

void	ImplementationWindowShell::SetTitle (const String& title)
{
	fWindowShellHints.SetTitle (title, True);
	SetTitle_ (title);
}

Boolean		ImplementationWindowShell::GetCloseable () const
{
	return (fWindowShellHints.GetCloseable ());
}

Boolean		ImplementationWindowShell::GetMoveable () const
{
	return (fWindowShellHints.GetMoveable ());
}

Boolean		ImplementationWindowShell::GetResizeable () const
{
	return (fWindowShellHints.GetResizeable ());
}

Boolean		ImplementationWindowShell::GetZoomable () const
{
	return (fWindowShellHints.GetZoomable ());
}

#if		qMotifUI
PixelMap	ImplementationWindowShell::GetIcon () const
{
	return (fWindowShellHints.GetIcon ());
}
#endif

String	ImplementationWindowShell::SetTitle_ (const String& title)
{
	String	newTitle = title;
	if ((GetOSRepresentation () != Nil) and (GetVisible ())) {	
#if		qMacToolkit
		osStr255	tmpStr;
		::SetWTitle (GetOSRepresentation (), newTitle.ToPStringTrunc (tmpStr, sizeof (tmpStr)));
#elif	qXtToolkit
		Arg		args [1];
		char	titleBuf[1024];
		newTitle.ToCString (titleBuf, sizeof (titleBuf));
		XtSetArg (args[0], XtNtitle, titleBuf);
		::XtSetValues (GetOSRepresentation (), args, 1);
#endif	/*Toolkit*/
	}
	return (newTitle);
}

Region	ImplementationWindowShell::GetVisibleArea () const
{
	return (GetExtent ());
}

#if		qXtToolkit
void	ImplementationWindowShell::CheckBorders ()
{
	unsigned	int	borderWidth		=	GetOSRepresentation ()->core.border_width;
#if		qDebug
	/*
	 * Check that the real realized value is the same as the one the widget gives us.
	 */
	osWindow		root;
	int				x;
	int				y;
	unsigned	int	height;
	unsigned	int	width;
	unsigned	int	depth;
	unsigned	int	real_border;
	if (XtIsRealized (GetOSRepresentation ())) {
		if (! ::XGetGeometry (XtDisplay (GetOSRepresentation ()), XtWindow (GetOSRepresentation ()),
			&root, &x, &y, &width, &height, &real_border, &depth) ) {
			AssertNotReached ();		// should raise xectpion on error!!!
		}
		if (borderWidth != real_border) {
			gDebugStream << "In Implementation::CheckBorders: real_border= " <<
				real_border << " but the widget value is " << borderWidth << newline;
		}
	}
#endif	/*qDebug*/
	fTopLeftBorder = Point (borderWidth, borderWidth);
	fBotRightBorder = Point (borderWidth, borderWidth);
}
#endif	/*qXtToolkit*/

#if		qMacGDI
osGrafPort*	ImplementationWindowShell::GetOSRepresentation () const
{
	return (fOSWindow);
}
#elif	qXGDI
osWidget*	ImplementationWindowShell::GetOSRepresentation () const
{
	return (fShellWidget);
}
#endif

void	ImplementationWindowShell::BeInWindow (Window* window)
{
	fWindow = window;

	if (GetOSRepresentation () == Nil) {
		BuildOSRepresentation (fWindowShellHints);
	}
	EnsureNotNil (GetOSRepresentation ());
}

WindowShellHints	ImplementationWindowShell::GetDefaultHints ()
{
	WindowShellHints	hints;
	
	hints.SetTitle (kEmptyString, False);
	hints.SetDesiredOrigin (kZeroPoint, False);
	hints.SetMinSize (Point (100, 100), False);
	hints.SetMaxSize (Point (1000, 1000), False);
	hints.SetDesiredSize (Point (250, 250), False);
	hints.SetCloseable (True, False);
	hints.SetMoveable (True, False);
	hints.SetResizeable (True, False);
	hints.SetZoomable (True, False);
#if		qMotifUI
	hints.SetIcon (GetDefaultIcon (), False);
#endif
	
	return (hints);
}


#if		qMacToolkit
static	struct	osGrafPort*	MakeMacWin (short theWindowType, Boolean closeBox)
{
	osRect	kMacWinRect		=	{0, 0, 0, 0};
	osWindowPtr	osw	=	(GDIConfiguration ().ColorQDAvailable ())?
			::NewCWindow (Nil, &kMacWinRect, "\p", False, theWindowType, osWindowPtr (-1), closeBox, 0):
			::NewWindow (Nil, &kMacWinRect, "\p", False,  theWindowType, osWindowPtr (-1), closeBox, 0);
	EnsureNotNil (osw);
	return (osw);
}
#endif







/*
 ********************************************************************************
 ************************************ WindowShell *******************************
 ********************************************************************************
 */

WindowShellHints::WindowShellHints () :
	fTitleSet (False),
	fTitle (kEmptyString),
	fDesiredOriginSet (False),
	fDesiredOrigin (kZeroPoint),
	fMinSizeSet (False),
	fMinSize (kZeroPoint),
	fMaxSizeSet (False),
	fMaxSize (kZeroPoint),
	fDesiredSizeSet (False),
	fDesiredSize (kZeroPoint),
	fCloseableSet (False),
	fCloseable (False),
	fMoveableSet (False),
	fMoveable (False),
	fResizeableSet (False),
	fResizeable (False),
	fZoomableSet (False),
	fZoomable (False)
#if		qMotifUI
	,fIconSet (False)
	,fIcon (GetDefaultIcon ())
#endif
{
}

void	WindowShellHints::SetTitle (const String& title, Boolean userSet)
{
	if (userSet) {
		fTitleSet = True;
	}
	fTitle = title;
}

void	WindowShellHints::SetDesiredOrigin (const Point& desiredOrigin, Boolean userSet)
{
	if (userSet) {
		fDesiredOriginSet = True;
	}
	fDesiredOrigin = desiredOrigin;
}

void	WindowShellHints::SetMinSize (const Point& minSize, Boolean userSet)
{
	if (userSet) {
		fMinSizeSet = True;
	}
	fMinSize = minSize;
}

void	WindowShellHints::SetMaxSize (const Point& maxSize, Boolean userSet)
{
	if (userSet) {
		fMaxSizeSet = True;
	}
	fMaxSize = maxSize;
}

void	WindowShellHints::SetDesiredSize (const Point& desiredSize, Boolean userSet)
{
	if (userSet) {
		fDesiredSizeSet = True;
	}
	fDesiredSize = desiredSize;
}

void	WindowShellHints::SetCloseable (Boolean closeable, Boolean userSet)
{
	if (userSet) {
		fCloseableSet = True;
	}
	fCloseable = closeable;
}

void	WindowShellHints::SetMoveable (Boolean moveable, Boolean userSet)
{
	if (userSet) {
		fMoveableSet = True;
	}
	fMoveable = moveable;
}

void	WindowShellHints::SetResizeable (Boolean resizeable, Boolean userSet)
{
	if (userSet) {
		fResizeableSet = True;
	}
	fResizeable = resizeable;
}

void	WindowShellHints::SetZoomable (Boolean zoomable, Boolean userSet)
{
	if (userSet) {
		fZoomableSet = True;
	}
	fZoomable = zoomable;
}

#if		qMotifUI
void	WindowShellHints::SetIcon (const PixelMap& icon, Boolean userSet)
{
	if (userSet) {
		fIconSet = True;
	}
	fIcon = icon;
}
#endif

Boolean	WindowShellHints::TitleSet () const
{
	return (fTitleSet);
}

String	WindowShellHints::GetTitle () const
{
	return (fTitle);
}

Boolean	WindowShellHints::DesiredOriginSet () const
{
	return (fDesiredOriginSet);
}

Point	WindowShellHints::GetDesiredOrigin () const
{
	return (fDesiredOrigin);
}

Boolean	WindowShellHints::MinSizeSet () const
{
	return (fMinSizeSet);
}

Point	WindowShellHints::GetMinSize () const
{
	return (fMinSize);
}

Boolean	WindowShellHints::MaxSizeSet () const
{
	return (fMaxSizeSet);
}

Point	WindowShellHints::GetMaxSize () const
{
	return (fMaxSize);
}

Boolean	WindowShellHints::DesiredSizeSet () const
{
	return (fDesiredSizeSet);
}

Point	WindowShellHints::GetDesiredSize () const
{
	return (fDesiredSize);
}

Boolean	WindowShellHints::CloseableSet () const
{
	return (fCloseableSet);
}

Boolean	WindowShellHints::GetCloseable () const
{
	return (fCloseable);
}

Boolean	WindowShellHints::MoveableSet () const
{
	return (fMoveableSet);
}

Boolean	WindowShellHints::GetMoveable () const
{
	return (fMoveable);
}

Boolean	WindowShellHints::ResizeableSet () const
{
	return (fResizeableSet);
}

Boolean	WindowShellHints::GetResizeable () const
{
	return (fResizeable);
}

Boolean	WindowShellHints::ZoomableSet () const
{
	return (fZoomableSet);
}

Boolean	WindowShellHints::GetZoomable () const
{
	return (fZoomable);
}

#if		qMotifUI
Boolean		WindowShellHints::IconSet () const
{
	return (fIconSet);
}

PixelMap	WindowShellHints::GetIcon () const
{
	return (fIcon);
}
#endif








/*
 ********************************************************************************
 ********************************* io stream support ****************************
 ********************************************************************************
 */
class ostream&	operator<< (class ostream& out, const WindowShellHints& h)
{
// break up into multiple lines cuz HP compiler barfs on it with -O LGP Jan 18, 1992
// broken up even further by SSW cuz Mac C compiler not happy either Jan 22 1992
	out << "(" << newline;
	out << tab << "fTitleSet=" << h.TitleSet () << ", fTitle=" << h.GetTitle () << newline;
	out << tab << "fDesiredOriginSet=" << h.DesiredOriginSet () << ", fDesiredOrigin=" << h.GetDesiredOrigin () << newline;
	out << tab << "fMinSizeSet=" << h.MinSizeSet () << ", fMinSize=" << h.GetMinSize () << newline;
	out << tab << "fMaxSizeSet=" << h.MaxSizeSet () << ", fMaxSize=" << h.GetMaxSize () << newline;
	out << tab << "fDesiredSizeSet=" << h.DesiredSizeSet () << ", fDesiredSize=" << h.GetDesiredSize () << newline;

	out << tab << "fClosableSet=" << h.CloseableSet () << ", fClosable=" << h.GetCloseable () << newline;
	out << tab << "fMoveableSet" << h.MoveableSet () << ", fMoveable=" << h.GetMoveable () << newline;
	out << tab << "fResizeableSet" << h.ResizeableSet () << ", fResizeable=" << h.GetResizeable () << newline;
	out << tab << "fZoomableSet" << h.ZoomableSet () << ", fZoomeable=" << h.GetZoomable () << newline;
#if		qMotifUI
	out << tab << "fIconSet" << h.IconSet () << ", fIcon=" << h.GetIcon () << newline;
#endif
	out << ")" << newline;
	
	return (out);
}

class istream&	operator>> (class istream& in, WindowShellHints& h)
{
	AssertNotImplemented ();
	return (in);
}








/*
 ********************************************************************************
 ************************************ WindowShell *******************************
 ********************************************************************************
 */
WindowShell::WindowShell ()
{
}

WindowShell::~WindowShell ()
{
}

#if		qMacToolkit
void	WindowShell::DragEvent (const Point& pressAt, const KeyBoard& keyBoardState)
{
	osPoint	thePoint;
	os_cvt (GetWindow ().LocalToGlobal (pressAt), thePoint);

	Rect	bounds = DeskTop::Get ().GetBounds ().GetBounds ();
	osRect	osBounds;
	osRect	osSlop;
	os_cvt (bounds, osBounds);
	os_cvt (bounds.InsetBy (-4, -4), osSlop);
	Region	winStruct = Region (((osWindowPeek) GetOSRepresentation ())->strucRgn);
	
	osGrafPort*	oldPort = Nil;
	osGrafPort* winPort = Nil;
	::GetPort (&oldPort);
	::GetWMgrPort (&winPort);
	
	AssertNotNil (oldPort);
	AssertNotNil (winPort);
	::SetPort (winPort);
	long theResult = ::DragGrayRgn (winStruct.GetOSRegion (), thePoint, &osBounds, &osSlop, noConstraint, Nil);
	::SetPort (oldPort);				

	Coordinate	offsetV = HiWord (theResult);
	Coordinate	offsetH = LoWord (theResult);
	if (theResult != 0x80008000) {
		GetWindow ().SetOrigin (GetWindow ().GetOrigin () + Point (offsetV, offsetH));
	}				

	if (not keyBoardState.GetKey (KeyBoard::eCommandKey)) {
		GetWindow ().Select ();					
	}
}

void	WindowShell::GrowEvent (const Point& pressAt)
{
	osPoint	thePoint;
	os_cvt (GetWindow ().LocalToGlobal (pressAt), thePoint);

	osRect osSizeRect;
	osSizeRect.top = GetWindowShellHints ().GetMinSize ().GetV ();
	osSizeRect.left = GetWindowShellHints ().GetMinSize ().GetH ();
	osSizeRect.bottom = 10000;	// we don't currently have a GetMaxSize
	osSizeRect.right = 10000;	// we don't currently have a GetMaxSize

	long	result = ::GrowWindow (GetOSRepresentation (), thePoint, &osSizeRect);
	if (result != 0) {
		GetWindow ().SetContentSize (Point (::HiWord (result), ::LoWord (result)));
	}
}

void	WindowShell::GoAwayEvent (const Point& pressAt)
{
	osPoint	thePoint;
	os_cvt (GetWindow ().LocalToGlobal (pressAt), thePoint);
	if (::TrackGoAway (GetOSRepresentation (), thePoint)) {
		GetWindow ().DoCommand (CommandSelection (CommandHandler::eClose));
	}
}

void	WindowShell::ZoomEvent (const Point& pressAt, Boolean zoomIn)
{
	osPoint	thePoint;
	os_cvt (GetWindow ().LocalToGlobal (pressAt), thePoint);
	if (::TrackBox (GetOSRepresentation (), thePoint, ((zoomIn) ? inZoomIn : inZoomOut))) {
		osPoint	oldOrigin = {0, 0};
		osGrafPort*	oldPort	=	qd.thePort;
		::SetPort (GetOSRepresentation ());
			::LocalToGlobal (&oldOrigin);
			::ZoomWindow (GetOSRepresentation (), ((zoomIn) ? inZoomIn : inZoomOut), False);
			Coordinate	offsetH = GetOSRepresentation ()->portRect.right - GetOSRepresentation ()->portRect.left;
			Coordinate	offsetV = GetOSRepresentation ()->portRect.bottom - GetOSRepresentation ()->portRect.top;
			osPoint	newOrigin = {0, 0};
			::LocalToGlobal (&newOrigin);
		::SetPort (oldPort);
		Rect	extent	=	Rect (GetWindow ().GetOrigin () + Point (newOrigin.v - oldOrigin.v, newOrigin.h - oldOrigin.h), Point (offsetV, offsetH));
		SetOrigin (extent.GetOrigin ());
		GetWindow ().SetContentSize (extent.GetSize ());
	}
}

Point	WindowShell::LocalToTablet (const Point& p)	const
{
// hack for now - MAYBE should have windowhsell be a panel???
	const Point kDiff = GetOrigin () - GetContentOrigin ();
	return (p + kDiff);
}

Point	WindowShell::TabletToLocal (const Point& p)	const
{
// hack for now - MAYBE should have windowhsell be a panel???
	const Point kDiff = GetOrigin () - GetContentOrigin ();
	return (p - kDiff);
}

#endif	/*qMacToolkit*/











/*
 ********************************************************************************
 **************************** StandardDocumentWindowShell ***********************
 ********************************************************************************
 */
StandardDocumentWindowShell::StandardDocumentWindowShell () :
	ImplementationWindowShell (GetDefaultHints ())
{
	SetWindowPlane (DeskTop::Get ().GetStandardPlane ());
}

StandardDocumentWindowShell::StandardDocumentWindowShell (const WindowShellHints& hints) :
	ImplementationWindowShell (hints)
{
	SetWindowPlane (DeskTop::Get ().GetStandardPlane ());
}

StandardDocumentWindowShell::~StandardDocumentWindowShell ()
{
}

WindowShellHints	StandardDocumentWindowShell::GetDefaultHints ()
{
	WindowShellHints	hints = ImplementationWindowShell::GetDefaultHints ();
	return (hints);
}

void	StandardDocumentWindowShell::BuildOSRepresentation (WindowShellHints& hints)
{
#if		qXtToolkit
	extern	osDisplay*	gDisplay;
	osWidget*	w = ::XtAppCreateShell (Nil, "", topLevelShellWidgetClass, gDisplay, Nil, 0);
	SetOSRepresentation (w);
#else
	osGrafPort* w			= 	MakeMacWin (hints.GetResizeable ()? (documentProc + 8): noGrowDocProc, hints.GetCloseable ());
	SetOSRepresentation (w, Point (20, 0));
#endif

}









/*
 ********************************************************************************
 ****************************** StandardAlertWindowShell ************************
 ********************************************************************************
 */
StandardAlertWindowShell::StandardAlertWindowShell ():
	ImplementationWindowShell (GetDefaultHints ())
{
	SetWindowPlane (DeskTop::Get ().GetAlertPlane ());
}

StandardAlertWindowShell::StandardAlertWindowShell (const WindowShellHints& hints):
	ImplementationWindowShell (hints)
{
	SetWindowPlane (DeskTop::Get ().GetAlertPlane ());
}

StandardAlertWindowShell::~StandardAlertWindowShell ()
{
}

WindowShellHints	StandardAlertWindowShell::GetDefaultHints ()
{
	WindowShellHints	hints = ImplementationWindowShell::GetDefaultHints ();
	hints.SetCloseable (False, False);
	hints.SetMoveable (False, False);
	hints.SetResizeable (False, False);
	hints.SetZoomable (False, False);
	return (hints);
}

void	StandardAlertWindowShell::BuildOSRepresentation (WindowShellHints& hints)
{
#if		qXtToolkit
	extern	osDisplay*	gDisplay;
	osWidget*	w = ::XtAppCreateShell (Nil, "", transientShellWidgetClass, gDisplay, Nil, 0);
	SetOSRepresentation (w);
#else
	osGrafPort* w = MakeMacWin (hints.GetMoveable () ? movableDBoxProc: dBoxProc, False);
	SetOSRepresentation (w, hints.GetMoveable () ? Point (24, 4): Point (4, 4));
#endif
}








/*
 ********************************************************************************
 ****************************** StandardDialogWindowShell ***********************
 ********************************************************************************
 */
StandardDialogWindowShell::StandardDialogWindowShell ():
	ImplementationWindowShell (GetDefaultHints ())
{
	SetWindowPlane (DeskTop::Get ().GetDialogPlane ());
}

StandardDialogWindowShell::StandardDialogWindowShell (const WindowShellHints& hints):
	ImplementationWindowShell (hints)
{
	SetWindowPlane (DeskTop::Get ().GetDialogPlane ());
}

StandardDialogWindowShell::~StandardDialogWindowShell ()
{
}

WindowShellHints	StandardDialogWindowShell::GetDefaultHints ()
{
	WindowShellHints	hints = ImplementationWindowShell::GetDefaultHints ();
	hints.SetCloseable (False, False);
	hints.SetMoveable (False, False);
	hints.SetResizeable (False, False);
	hints.SetZoomable (False, False);
	return (hints);
}

void	StandardDialogWindowShell::BuildOSRepresentation (WindowShellHints& hints)
{
#if		qXtToolkit
	extern	osDisplay*	gDisplay;
	osWidget*	w = ::XtAppCreateShell (Nil, "", transientShellWidgetClass, gDisplay, Nil, 0);
	SetOSRepresentation (w);
#else
	Boolean moveable = 	hints.GetMoveable ();
	osGrafPort* w = MakeMacWin (moveable? movableDBoxProc: dBoxProc, False);
	SetOSRepresentation (w, moveable? Point (24, 4): Point (4, 4));
#endif
}







/*
 ********************************************************************************
 *********************************** PlainWindowShell ***************************
 ********************************************************************************
 */
PlainWindowShell::PlainWindowShell () :
	ImplementationWindowShell (GetDefaultHints ())
{
	SetWindowPlane (DeskTop::Get ().GetStandardPlane ());
}

PlainWindowShell::PlainWindowShell (const WindowShellHints& hints) :
	ImplementationWindowShell (hints)
{
	SetWindowPlane (DeskTop::Get ().GetStandardPlane ());
}

PlainWindowShell::~PlainWindowShell ()
{
}

WindowShellHints	PlainWindowShell::GetDefaultHints ()
{
	WindowShellHints	hints = ImplementationWindowShell::GetDefaultHints ();
	return (hints);
}

void	PlainWindowShell::BuildOSRepresentation (WindowShellHints& hints)
{
#if		qXtToolkit
	// SB Translient SHell??? or maybe override shell??
	extern	osDisplay*	gDisplay;
	osWidget*	w = ::XtAppCreateShell (Nil, "", topLevelShellWidgetClass, gDisplay, Nil, 0);
	SetOSRepresentation (w);
#else
	Boolean moveable = 	hints.GetMoveable ();
	osGrafPort* w = MakeMacWin (plainDBox, False);
	SetOSRepresentation (w, kZeroPoint);
#endif
}






/*
 ********************************************************************************
 ****************************** StandardPalletWindowShell ***********************
 ********************************************************************************
 */
StandardPalletWindowShell::StandardPalletWindowShell () :
	ImplementationWindowShell (GetDefaultHints ())
{
	SetWindowPlane (DeskTop::Get ().GetPalettePlane ());
}

StandardPalletWindowShell::StandardPalletWindowShell (const WindowShellHints& hints) :
	ImplementationWindowShell (hints)
{
	SetWindowPlane (DeskTop::Get ().GetPalettePlane ());
}

StandardPalletWindowShell::~StandardPalletWindowShell ()
{
}

WindowShellHints	StandardPalletWindowShell::GetDefaultHints ()
{
	WindowShellHints	hints = ImplementationWindowShell::GetDefaultHints ();
	hints.SetResizeable (False, False);
	hints.SetZoomable (False, False);
	return (hints);
}


#if		qMacUI && 0
	struct	TitlebarButtonController: ButtonController {
		TitlebarButtonController (class TitledWindow_MacUI_Portable& tw);
		~TitlebarButtonController ();
		TitledWindow_MacUI_Portable&	fTitledWindow;
		override	void	ButtonPressed (AbstractButton* button);
	};
	// sb scoped inside titlebar
	class	TitleBarButton : public Button {
		protected:
			TitleBarButton (ButtonController& owner);
		public:
			~TitleBarButton ();

		public:
			override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
	
			override	void	DrawFeedbackState (Boolean feedBackOn);
			override	Point	CalcDefaultSize_ (const Point& defaultSize = kZeroPoint) const;
			override	void	Draw (const Region& update);
	};

	// sb scoped inside titlebar
	class	ZoomBox : public TitleBarButton {
		public:
			ZoomBox (ButtonController& owner);
			~ZoomBox ();
			override	void	Draw (const Region& update);
			Rect	fOldWindowExtent;
	};
	
	// sb scoped inside titlebar
	class CloseBox : public TitleBarButton {
		public:
			CloseBox (ButtonController& owner);
			virtual ~CloseBox ();
	};

	class	TitleBar : public View {
		public:
			TitleBar (TitledWindow_MacUI_Portable& owner, const String& label, Boolean closeBox, Boolean zoomBox);
			~TitleBar ();
			
			override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
			override	void	Draw (const Region& update);
			override	Point	CalcDefaultSize_ (const Point& defaultSize = kZeroPoint) const;
			
			nonvirtual	String	GetLabel ()	const;
			virtual		void	SetLabel (const String& label, UpdateMode update = eDelayedUpdate);
		
		protected:
			TitleBar (TitledWindow_MacUI_Portable& owner, const String& label, CloseBox* closeBox, ZoomBox* zoomBox);
		
			override	void	Layout ();
		
		private:
			nonvirtual	void	Initialize ();
		
			String							fLabel;
			TitledWindow_MacUI_Portable&	fWindow;
			ZoomBox*						fZoomBox;
			CloseBox*						fCloseBox;
			
			friend	struct	TitlebarButtonController;
			friend	class	TitleBarController;	// peeks at fZoomBox and fCloseBox
			friend	class	TitledWindow_MacUI_Portable;	// peeks at fZoomBox and fCloseBox
	};

	class	WindoidTitleBar : public TitleBar {
		public:
			WindoidTitleBar (TitledWindow_MacUI_Portable& owner, Boolean closeBox);
	
			override	void	Draw (const Region& update);
			override	Point	CalcDefaultSize_ (const Point& defaultSize = kZeroPoint) const;
	};
	WindoidTitleBar::WindoidTitleBar (TitledWindow_MacUI_Portable& owner, Boolean closeBox):
		TitleBar (owner, kEmptyString, closeBox, False)
	{
   		const Tile kMyTile = PalletManager::Get ().MakeTileFromColor (kLightGrayColor);
		SetBackground (&kMyTile);
		SetFont (Nil);
	}
	
	void	WindoidTitleBar::Draw (const Region& /*update*/)
	{
		Point	s	=	GetSize ();
		OutLine (Line (Point (s.GetV ()-1, 0), Point (s.GetV ()-1, s.GetH ())));
	}
	
	Point	WindoidTitleBar::CalcDefaultSize_ (const Point& defaultSize) const
	{
		return (Point (11, defaultSize.GetH ()));
	}
#endif	/*qMacUI*/

void	StandardPalletWindowShell::BuildOSRepresentation (WindowShellHints& hints)
{
#if		qXtToolkit
	extern	osDisplay*	gDisplay;
	osWidget*	w = ::XtAppCreateShell (Nil, "", topLevelShellWidgetClass, gDisplay, Nil, 0);
	SetOSRepresentation (w);
#else
//	osGrafPort* w = MakeMacWin (plainDBox, hints.GetCloseable ());
//	SetOSRepresentation (w, kZeroPoint);

	osGrafPort* w	= 	MakeMacWin (hints.GetResizeable ()? (documentProc + 8): noGrowDocProc, hints.GetCloseable ());
	SetOSRepresentation (w, Point (20, 0));
#endif
}

#if		qMacToolkit
Point	StandardPalletWindowShell::LocalToTablet (const Point& p)	const
{
//	return (p);
	const Point kDiff = GetOrigin () - GetContentOrigin ();
	return (p + kDiff);
}

Point	StandardPalletWindowShell::TabletToLocal (const Point& p)	const
{
//	return (p);
	const Point kDiff = GetOrigin () - GetContentOrigin ();
	return (p - kDiff);
}
#endif	/*qMacToolkit*/






#if		qMotifUI
/*
 ********************************************************************************
 ******************************* MainApplicationShell ***************************
 ********************************************************************************
 */
MainApplicationShell::MainApplicationShell (const String& appName, const String& appClass, osDisplay* display):
	ImplementationWindowShell (GetDefaultHints ()),
	fAppName (appName),
	fAppClass (appClass),
	fOSDisplay (display)
{
	WindowShellHints hints = GetWindowShellHints ();
	hints.SetDesiredSize (Point (100, 400), False);		// for now it just holds a menubar, and this is about right...

	BuildOSRepresentation (hints);
	SetWindowShellHints (hints);

	SetWindowPlane (DeskTop::Get ().GetBackPalettePlane ());
}

MainApplicationShell::MainApplicationShell (const WindowShellHints& startHints, const String& appName, const String& appClass, osDisplay* display):
	ImplementationWindowShell (startHints),
	fAppName (appName),
	fAppClass (appClass),
	fOSDisplay (display)
{
	WindowShellHints hints = GetWindowShellHints ();
	BuildOSRepresentation (hints);
	SetWindowShellHints (hints);

	SetWindowPlane (DeskTop::Get ().GetBackPalettePlane ());
}

MainApplicationShell::~MainApplicationShell ()
{
}

WindowShellHints	MainApplicationShell::GetDefaultHints ()
{
	WindowShellHints	hints = ImplementationWindowShell::GetDefaultHints ();
	
	return (hints);
}

static	void	MyAboutCallBack (osWidget* w, char* client_data, char* iForget)
{
  	Application::Get ().ProcessCommand (CommandSelection (CommandHandler::eAboutApplication));
}

void	MainApplicationShell::BuildOSRepresentation (WindowShellHints& hints)
{
	char*	appName		=	fAppName.ToCString ();
	char*	appClass	=	fAppClass.ToCString ();
	osWidget*	w = ::XtAppCreateShell (appName, appClass, applicationShellWidgetClass, fOSDisplay, Nil, 0);
	SetOSRepresentation (w);
	delete (appName);
	delete (appClass);


	/*
	 * On an experimental basis, try adding an "about app" item to the main app shell.
	 * This is documented on page 601, of Orielly Volume 6. We do no error checking here
	 * since the example in the book didn't. Try under openlook, and see what happens?
	 */
	Atom	motifMsgs	=	::XmInternAtom (XtDisplay (w), "_MOTIF_WM_MESSAGES", False);
	Atom	myProtocol	=	::XmInternAtom (XtDisplay (w), "_AboutBoxProtocol", False);
	::XmAddProtocols (w, motifMsgs, &myProtocol, 1);
	::XmAddProtocolCallback (w, motifMsgs, myProtocol, MyAboutCallBack, Nil);
	char	buf [1024];
	Arg		args [1];
	OStringStream	ob;
	ob << "no-label				f.separator" << newline;		// seperator first...
	ob << "About\\ Application... f.send_msg " << int (myProtocol);
	XtSetArg (args[0], XmNmwmMenu, String (ob).ToCStringTrunc (buf, sizeof (buf)));
	::XtSetValues (w, args, 1);
}

#endif	/*qMotifUI*/





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
