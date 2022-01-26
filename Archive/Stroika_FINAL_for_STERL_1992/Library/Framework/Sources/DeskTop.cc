/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DeskTop.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DeskTop.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/07/08  04:33:39  lewis
 *		Renamed PointInside -> Contains.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.30  92/05/18  16:03:41  16:03:41  lewis (Lewis Pringle)
 *		Minor cleanups and get rid of compiler warnings.
 *		
 *		Revision 1.29  92/04/20  14:27:01  14:27:01  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		
 *		Revision 1.28  92/04/02  11:41:50  11:41:50  lewis (Lewis Pringle)
 *		Get rid of gDisplay param to constructing Tablet.
 *		
 *		Revision 1.27  92/03/17  03:03:08  03:03:08  lewis (Lewis Pringle)
 *		comment out beep for now.
 *		
 *		Revision 1.25  1992/03/11  08:12:35  lewis
 *		Cleanup Key dispatch code.
 *
 *		Revision 1.23  1992/03/10  13:09:47  lewis
 *		Hack stuff up a bit so menu commands accelerators for motif - at least sort of.
 *
 *		Revision 1.22  1992/03/09  23:47:06  lewis
 *		Use new HandleKeyStroke () interface.
 *		Not fully done - just quickly hacked in.
 *
 *		Revision 1.21  1992/03/05  18:26:41  lewis
 *		Use MenuBar_MacUI_Native::GetREALLYInstalledInSystemMBar () instead of Application::Get ().GetMenuBar ()
 *		for the mouse-press dispatch.
 *
 *		Revision 1.18  1992/02/15  05:31:37  sterling
 *		got rid of references t ActiveItem
 *
 *		Revision 1.17  1992/02/12  06:54:42  lewis
 *		Switch from OSConfig to GDIConfig and get rid of some debug messages, and other minor cleanups.
 *
 *		Revision 1.15  1992/01/31  16:41:08  sterling
 *		eliminated use of inherited typedef due to Mac compiler bugs
 *
 *		Revision 1.14  1992/01/28  22:17:29  lewis
 *		Added DesktopEvetnHandler to Desktop, and moved all the mouse tracking and press stuff from application to
 *		Desktop.
 *		Allowed for get method to build destop so no longer done in application. Also, added set method so users
 *		can subclass desktop and use different one (probably not working properly).
 *
 *		Revision 1.13  1992/01/19  21:44:17  lewis
 *		Set the size of desktop appropriately. Also, add new AlertPlane.
 *
 *		Revision 1.12  1992/01/18  09:11:15  lewis
 *		Added Backpallet.
 *
 *		Revision 1.11  1992/01/15  09:59:55  lewis
 *		Support for new class Enclosure, and reorganization with Panels.
 *
 *		Revision 1.10  1992/01/04  08:02:35  lewis
 *		Support Shell/Window changes.
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Windows.h>
#include	<SysEqu.h>			// for GrayRgn
#elif	qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"
#include	"Sound.hh"

#include	"EventManager.hh"
#include	"MenuBar.hh"
#include	"MenuItem.hh"
#include	"Window.hh"

#include	"DeskTop.hh"



// Not sure we need this...
#include	"Application.hh"


class	DeskTopEventHandler : public EventHandler {
	public:
		DeskTopEventHandler ();
		~DeskTopEventHandler ();

		override	void	ProcessMousePress (const osEventRecord& eventRecord, const Point& where, Boolean isDown,
											   const KeyBoard& keyBoardState, UInt8 clickCount);
		override	void	ProcessMouseMoved (const osEventRecord& eventRecord, const Point& newLocation,
											   const KeyBoard& keyBoardState, Region& newMouseRegion);
		override	void	ProcessKey (const osEventRecord& eventRecord, KeyBoard::KeyCode keyCode, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState);

	private:
		Region				fHelpRgn;
		Region				fCursorRgn;

//		typedef	EventHandler	inherited;
};





#if		qMacOS
static	osGrafPort*	GetTheDeskTopGrafPort ()
{
	/*
	 * DONE USE WMgrPort!!!
	 * VERY UNSAFE.  ALWAYS CREATE OUR OWN EQUIVILENT.
	 */
	if (GDIConfiguration ().ColorQDAvailable ()) {
		osCGrafPort*	wMgrCPort	=	new osCGrafPort;
		::OpenCPort (wMgrCPort);
		::CopyRgn (*(struct osRegion***)GrayRgn, wMgrCPort->visRgn); 
		// HERE WE MUST SUBTRACT OUT MENUBAR
		return ((osGrafPort*)wMgrCPort);
	}
	else {
//IS GrayRgn around on old systems?

		osGrafPort*	wMgrPort	=	new osGrafPort;
		::OpenPort (wMgrPort);
		::CopyRgn (*(struct osRegion***)GrayRgn, wMgrPort->visRgn); 
		// HERE WE MUST SUBTRACT OUT MENUBAR
		return (wMgrPort);
	}
}
#endif	/*qMacOS*/


#if		qXGDI
extern	osDisplay*      gDisplay;
#endif	/*qXGDI*/






/*
 ********************************************************************************
 ************************************* Desktop **********************************
 ********************************************************************************
 */

DeskTop*	DeskTop::sThe	=	Nil;

DeskTop::DeskTop ():
	fDeskTopTablet (Nil),
	fActivationStatusMayHaveChanged (True),
	fWindowPlanes (),
	fAlertPlane (Nil),
	fDialogPlane (Nil),
	fBackPalettePlane (Nil),
	fPalettePlane (Nil),
	fStandardPlane (Nil),
	fEventHandler (Nil)
{
	fEventHandler = new DeskTopEventHandler ();

#if		qMacGDI
	fDeskTopTablet = new Tablet (GetTheDeskTopGrafPort ());
#elif	qXGDI
	fDeskTopTablet = new Tablet (DefaultRootWindow (gDisplay));
#endif /*OS*/

// consider the origin of the thing to always be zero-zero.
// really this code needs re-thinking, but one thing is clear - the desktop origin
// must be zero-zero, for the global-to-local computations to work!!!
//	SetExtent (GetBounds ().GetBounds ());
//	SetSize (GetBounds ().GetBounds ().GetSize ());
#if		qMacOS
	SetSize (os_cvt (qd.screenBits.bounds).GetSize ());
#elif	qXGDI
	SetSize (Point (DisplayHeight (gDisplay, DefaultScreen (gDisplay)), DisplayWidth (gDisplay, DefaultScreen (gDisplay))));
#endif	/*qMacOS*/


sThe = this;		// hack so creating window planes does not infinite loop creating desktions - they
					// call DeskTop::Get () -- this code needs cleanup...
// note: order of construction very important (BAD DESIGN - FIX - LGP - JAN 18, 1992)
	fAlertPlane = new AlertWindowPlane ();
	fDialogPlane = new DialogWindowPlane ();
	fPalettePlane = new WindowPlane (not (WindowPlane::kModal), WindowPlane::eAllActive);
	fStandardPlane = new StandardWindowPlane ();
	fBackPalettePlane = new WindowPlane (not (WindowPlane::kModal), WindowPlane::eAllActive);
}

DeskTop::~DeskTop ()
{
	if (sThe == this) {		// in case someone deletes the desktop which is installed
		sThe = Nil;
	}
	delete (fEventHandler); fEventHandler = Nil;
	ForEach (WPlnPtr, it, MakePlaneIterator ()) {
		WindowPlane*	plane = it.Current ();
		delete plane;
	}
	Assert (fWindowPlanes.GetLength () == 0);

#if		qMacGDI
	if (GDIConfiguration ().ColorQDAvailable ()) {
		::CloseCPort ((osCGrafPort*)fDeskTopTablet->GetOSGrafPtr ());
	}
	else {
		::ClosePort (fDeskTopTablet->GetOSGrafPtr ());
	}
#endif	/*qMacOS*/
	delete (fDeskTopTablet);
}

DeskTop&	DeskTop::Get ()
{
	if (sThe == Nil) {
		SetThe (new DeskTop ());
	}
	RequireNotNil (sThe);
	return (*sThe);
}

void	DeskTop::SetThe (DeskTop* newDeskTop)
{
	if (sThe != newDeskTop) {
		delete (sThe); Assert (sThe == Nil);
		sThe = newDeskTop;		// can be nil!!!
	}
}

Region	DeskTop::GetBounds () const
{
#if	1
	return (fDeskTopTablet->GetBounds ());
#else
	return (*(struct osRegion***)GrayRgn);
#endif
}

Tablet*		DeskTop::GetTablet () const
{
	EnsureNotNil (fDeskTopTablet);
	return (fDeskTopTablet);
}

Panel*	DeskTop::GetParentPanel () const
{
	return (Nil);
}

void	DeskTop::DoSetupMenus ()
{
// bug - this really must iterate in the other direction, but that breaks modality.
// one fix (not great) is to iterate twice, and figure out how far to go and then itearte
// again to that point.
// Another fix would be to put next links in windowplane, and do stack style nexting there, much
// as we do with (view?? I think, or at least have done many times before).
	ForEach (WPlnPtr, it, MakePlaneIterator ()) {
		AssertNotNil (it.Current ());
		it.Current ()->DoSetupMenus ();
		if ((it.Current ()->GetModal ()) and (it.Current ()->GetFrontWindow () != Nil)) {
			break;
		}
	}
}

Boolean	DeskTop::DoCommand (const CommandSelection& selection)
{
	ForEach (WPlnPtr, it, MakePlaneIterator ()) {
		AssertNotNil (it.Current ());
		if (it.Current ()->DoCommand (selection)) {
			return (True);
		}
		if ((it.Current ()->GetModal ()) and (it.Current ()->GetFrontWindow () != Nil)) {
			return (False);
		}
	}
	return (False);
}

Boolean	DeskTop::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState)
{
	ForEach (WPlnPtr, it, MakePlaneIterator ()) {
		AssertNotNil (it.Current ());
		if (it.Current ()->DispatchKeyEvent (code, isUp, keyBoardState, composeState)) {
			return (True);
		}
	}
	return (False);
}

void	DeskTop::HandleActivation ()
{
	if (fActivationStatusMayHaveChanged) {
		/*
		 * First see which windows need to be de-activated, and then see which need to be
		 * activated.  Activate & Deactivate windows front-to-back (not sure how mac does?)
		 *
		 * The reason we bother with the two loops is cuz apple guarentees deactivates happen before
		 * activates??? I think this is so?
		 */
		ForEach (WPlnPtr, planeIt, MakePlaneIterator ()) {
			AssertNotNil (planeIt.Current ());
			ForEach (WindowPtr, it, planeIt.Current ()->MakeWindowIterator ()) {
				Window&	w	=	*it.Current ();
				AssertNotNil (it.Current ());
				if (w.GetActive () and not (w.GetPlane ().ShouldBeActive (&w))) {
					w.SetActive (not Window::kActive);
					Ensure (w.GetPlane ().ShouldBeActive (&w) == w.GetActive ());
				}
			}
		}

// Could simplify above and below use of ShouldBeActive if we put in asserts that w.GetPlane == paneIt.Current ()!!! Then use thos
// directly...
		ForEach (WPlnPtr, planeIt2, MakePlaneIterator ()) {
			AssertNotNil (planeIt2.Current ());
			ForEach (WindowPtr, it, planeIt2.Current ()->MakeWindowIterator ()) {
				Window&	w	=	*it.Current ();
				AssertNotNil (it.Current ());
				if (w.GetPlane ().ShouldBeActive (&w) and not (w.GetActive ())) {
					w.SetActive (Window::kActive);
				}
				Ensure (w.GetPlane ().ShouldBeActive (&w) == w.GetActive ());
			}
		}

		fActivationStatusMayHaveChanged = False;
	}
}

void	DeskTop::AddWindowPlane (WindowPlane* w)
{
	RequireNotNil (w);
	fWindowPlanes.Append (w);
}

void	DeskTop::RemoveWindowPlane (WindowPlane* w)
{
	fWindowPlanes.Remove (w);
}

void	DeskTop::ReorderWindowPlane (WindowPlane* w, CollectionSize index)
{
	CollectionSize	oldIndex = fWindowPlanes.IndexOf (w);
	fWindowPlanes.Remove (w);
	fWindowPlanes.InsertAt (w, ((oldIndex < index) ? index-1 : index));
}

void	DeskTop::ReorderWindowPlane (WindowPlane* w, WindowPlane* behindPlane)
{
	CollectionSize	index = fWindowPlanes.IndexOf (behindPlane);
	ReorderWindowPlane (w, ((index == kBadSequenceIndex) ? 1 : ++index));
}

SequenceIterator(WPlnPtr)*	DeskTop::MakePlaneIterator (SequenceDirection d) const
{
	return (fWindowPlanes.MakeSequenceIterator (d));
}

WindowPlane*	DeskTop::GetFrontWindowPlane () const
{
	ForEach (WPlnPtr, it, MakePlaneIterator ()) {
		AssertNotNil (it.Current ());
		Window*	w = it.Current ()->GetFrontWindow ();
		if (w != Nil) {
			return (it.Current ());
		}
	}
	return (Nil);
}

Window*	DeskTop::GetFrontWindow () const
{
	WindowPlane*	wp = GetFrontWindowPlane ();
	return ((wp == Nil) ? Nil : wp->GetFrontWindow ());
}

Boolean	DeskTop::GetModal () const
{
	WindowPlane*	wp = GetFrontWindowPlane ();
	return (Boolean ((wp == Nil) ? not (WindowPlane::kModal): wp->GetModal ()));
}

void	DeskTop::SynchronizeOSWindows ()
{
	fActivationStatusMayHaveChanged = True;
	Window*	w = Nil;
	ForEach (WPlnPtr, it, MakePlaneIterator ()) {
		AssertNotNil (it.Current ());
		it.Current ()->SynchronizeOSWindows (w);
		if (it.Current ()->GetBackWindow () != Nil) {
			w = it.Current ()->GetBackWindow ();
		}
	}
}

Window*	DeskTop::FindWindow (const Point& where)	const
{
	ForEach (WPlnPtr, it, MakePlaneIterator ()) {
		AssertNotNil (it.Current ());
		Window*	w = it.Current ()->FindWindow (it.Current ()->EnclosureToLocal (where));
		if (w != Nil) {
			return (w);
		}
		else if ((it.Current ()->GetModal ()) and (it.Current ()->GetFrontWindow () != Nil)) {
// LGP NOV 2 - THIS IS WRONG PLACE TO DO THIS (UNLESS MAYBE ADD PARAM TO THIS FUNCT)
// BUT LEAVE IT FOR NOW - CANNOT DO TOO MUCH AT ONCE!!!
			return (Nil);	// active modal planes swallow clicks
		}
	}
	return (Nil);
}

Rect	DeskTop::GetMainDeviceDeskBounds () const
{
#if		qMacOS
	return (Rect (Point (20, 0), os_cvt (qd.screenBits.bounds).GetSize ()));
#elif	qXGDI
	return (Rect (kZeroPoint,
				  Point (DisplayHeight (gDisplay, DefaultScreen (gDisplay)),
						 DisplayWidth (gDisplay, DefaultScreen (gDisplay)))));
#endif	/*GDI*/
}











/*
 ********************************************************************************
 ******************************* DeskTopEventHandler ****************************
 ********************************************************************************
 */

DeskTopEventHandler::DeskTopEventHandler ():
	EventHandler (),
	fHelpRgn (kEmptyRegion),
	fCursorRgn (kEmptyRegion)
{
	Activate ();
}

DeskTopEventHandler::~DeskTopEventHandler ()
{
	Deactivate ();
}

void	DeskTopEventHandler::ProcessMousePress (const osEventRecord& eventRecord, const Point& where, Boolean isDown,
												const KeyBoard& keyBoardState, UInt8 clickCount)
{
#if		qMacOS
	if (isDown) {
		osPoint	thePoint;
		osWindowPtr	whichWin;
		short	windowPart = ::FindWindow (os_cvt (where, thePoint), &whichWin);
		switch (windowPart) {
			case	inDesk: {
				/*
				 * Havent seen many of these - but have seen some
				 * what does it mean, and what do we do?
				 * Easy to get with no multifinder - just click in desktop and we ignore them there. I guess its
				 *  always safe to ignore them...
				 */
			}
			break;
			case	inMenuBar: {
				if (MenuBar_MacUI_Native::GetREALLYInstalledInSystemMBar () == Nil) {
#if		qDebug
					gDebugStream << "Click in menubar with none installed" << newline;
#endif
				}
				else {
					MenuItem*	selected = Nil;
					Application::Get ().SetupTheMenus ();
					if (MenuBar_MacUI_Native::GetREALLYInstalledInSystemMBar ()->DoMenuSelection (where, selected)) {
						AssertNotNil (selected);
						Application::Get ().ProcessCommand (*selected);
					}
				}
			}
			break;
			case	inSysWindow: {
				// we seem to get these quite a bit on things like activates??
				// not sure what we are to do???
			}
			break;

			case	inContent:
			case	inDrag:				
			case	inGrow:
			case	inGoAway:
			case	inZoomIn:
			case	inZoomOut: {
				Window*	w = DeskTop::Get ().FindWindow (where);
				// gross hack - really need to think this out better.  This case comes up when
				// we have a modal dialog, and perhaps in others too (thus the word gross).
				if (w == Nil) {
					Beep ();
				}
				else {
					AssertNotNil (w);
					AssertNotNil (whichWin);
					Assert (long (w) == ::GetWRefCon (whichWin));
					(void)w->TrackPress (MousePressInfo (w->GlobalToLocal (where), clickCount, keyBoardState));
				}
			}
			break;
			default: {
				AssertNotReached ();
			}
			break;
		}
	}
#elif	qXGDI
// SHOULD probably share this code with the above mac code ,and get rid of non-poratable
// part of mac code - just use our displach not apples FindWindow...
	Window*	w = DeskTop::Get ().FindWindow (where);
	// gross hack - really need to think this out better.  This case comes up when
	// we have a modal dialog, and perhaps in others too (thus the word gross).
	if (w == Nil) {
		osEventRecord	er;
		EventManager::Get ().GetCurrentEvent (er);		// hack - really should displatch - just a tst....
		::XtDispatchEvent (&er);

		Beep ();  // OK for now since we dont have window in our window list for APP SHELL!!!
	}
	else {
		AssertNotNil (w);
		if (not w->TrackPress (MousePressInfo (w->GlobalToLocal (where), clickCount, keyBoardState))) {
			osEventRecord	er;
			EventManager::Get ().GetCurrentEvent (er);		// hack - really should displatch - just a tst....
			::XtDispatchEvent (&er);
		}
	}
#endif	/*qMacOS*/
}

void	DeskTopEventHandler::ProcessMouseMoved (const osEventRecord& eventRecord, const Point& newLocation,
												const KeyBoard& keyBoardState, Region& newMouseRegion)
{
	Window*	w = DeskTop::Get ().FindWindow (newLocation);
	
	if (not fHelpRgn.Contains (newLocation)) {
		if ((w != Nil) and (w->TrackHelp (w->GlobalToLocal (newLocation), fHelpRgn))) {
			fHelpRgn = w->LocalToGlobal (fHelpRgn);
		}
		else {
			QuickHelp::HideHelp ();
			fHelpRgn = kEmptyRegion;
		}
	}

	if (not newMouseRegion.Contains (newLocation)) {
		if ((w != Nil) and (w->TrackMovement (w->GlobalToLocal (newLocation), fCursorRgn, keyBoardState))) {
			fCursorRgn = w->LocalToGlobal (fCursorRgn);
		}
		else {
			kArrowCursor.Install ();
			fCursorRgn = kEmptyRegion;	// is this what we want? or should we try and compute the region???
		}
	}
	
	newMouseRegion = fCursorRgn * fHelpRgn;
}

void	DeskTopEventHandler::ProcessKey (const osEventRecord& eventRecord, KeyBoard::KeyCode keyCode,
										Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState)
{
// This is kludy hack code - sort of workable for now... LGP Mar 10, 1992

#if		qMacUI || qMotifUI
	KeyComposeState		xxComposeState;
	KeyStroke			xxKeyStroke;
	Boolean				isCmdKey		=  Boolean (not (isUp) and KeyHandler::ApplyKeyEvent (xxKeyStroke, keyCode, isUp, keyBoardState, xxComposeState));

#if		qMacToolkit
	isCmdKey = Boolean (isCmdKey and keyBoardState.GetKey (KeyBoard::eCommandKey));
#elif	qXmToolkit
	isCmdKey = Boolean (isCmdKey and xxKeyStroke.GetModifiers ().Contains (KeyStroke::eControlKeyModifier));
#endif	/*Toolkit*/

	if (isCmdKey) {
		MenuItem*	selected = Nil;
		Application::Get ().SetupTheMenus ();
		if (MenuOwner::DoMenuSelection (xxKeyStroke.GetCharacter (), selected)) {
			AssertNotNil (selected);
			Application::Get ().ProcessCommand (*selected);
		}
		return;
	}
#endif	/*qMacUI*/

	if (not DeskTop::Get ().DispatchKeyEvent (keyCode, isUp, keyBoardState, composeState)) {
// a beep may be reasonable here, or some such place, but for now, somebody is neglecting to return right
// answer - comment out the beep til we fix it - about to do a release - LGP March 16, 1992
#if 0
		Beep ();		// not too sure, but sounds reasonable - should do this in an overrideable
						// method, however...
#endif

#if		0 && qDebug
		DebugMessage ("keyCode %d ignored", keyCode);
#endif	/*qDebug*/
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

