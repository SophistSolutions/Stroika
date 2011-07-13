/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/WindowPlane.cc,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: WindowPlane.cc,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.5  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/15  22:03:39  lewis
 *		Fix typo and cleanup switch statment to sterls new format.
 *
 *		Revision 1.3  1992/07/08  03:52:52  lewis
 *		Use Contains isntead of PointInside, and AssertNotReached etc instead of
 *		SwitchFallThru ().
 *
 *		Revision 1.2  1992/07/02  05:04:08  lewis
 *		Renamed Sequence_DoublyLLOfPointers->Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.18  92/04/10  02:43:22  02:43:22  lewis (Lewis Pringle)
 *		Only make the desired origin set flag true for mac - not motif for standard window planes.
 *		For Dialog/Alerts, OK to set DesriedOriginSet flag.
 *		
 *		Revision 1.17  92/03/10  00:05:21  00:05:21  lewis (Lewis Pringle)
 *		Use new DispatchKeyEvent () method instead of old HandleKey ().
 *		
 *		Revision 1.16  1992/01/29  05:25:49  sterling
 *		changed WindowHints
 *
 *		Revision 1.14  1992/01/19  21:41:31  lewis
 *		Fix use of CollectionSize instead of UInt32.
 *		Added AlertWindowPlane and have it center 1/3 from top of screen.
 *
 *		Revision 1.9  1992/01/06  10:12:20  lewis
 *		Use new GetVisible/GetReallyVisible ().
 *
 *		Revision 1.7  1992/01/04  07:27:44  lewis
 *		Supported Window/Shell cleanups and start at new placement strategy.
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Windows.h>
#endif	/*qMacToolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"DeskTop.hh"
#include	"Shell.hh"
#include	"Window.hh"

#include	"WindowPlane.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, WPlnPtr);
	Implement (Collection, WPlnPtr);
	Implement (AbSequence, WPlnPtr);
	Implement (Array, WPlnPtr);
	Implement (Sequence_Array, WPlnPtr);
	Implement (Sequence, WPlnPtr);
#endif




/*
 ********************************************************************************
 *********************************** WindowPlane ********************************
 ********************************************************************************
 */

const	Boolean	WindowPlane::kModal	=	True;

WindowPlane::WindowPlane (Boolean modal, ActivationKind activationKind):
	fModal (modal),
	fActivationKind (activationKind),
	fWindows ()
{
	Require (not GetModal () or (fActivationKind == eOneActive));
	DeskTop::Get ().AddWindowPlane (this);
	SetSize (DeskTop::Get ().GetSize ());
}

WindowPlane::~WindowPlane ()
{
	ForEach (WindowPtr, it, MakeWindowIterator (eSequenceBackward)) {
		Window*	w = it.Current ();
		AssertNotNil (w);
		delete (w);
	}
	Ensure (fWindows.GetLength () == 0);
	DeskTop::Get ().RemoveWindowPlane (this);
}

Tablet*	WindowPlane::GetTablet () const
{
	return (DeskTop::Get ().GetTablet ());
}

Panel*	WindowPlane::GetParentPanel () const
{
	return (&DeskTop::Get ());
}

void	WindowPlane::DoSetupMenus ()
{
	switch (GetActivationKind ()) {
		case eAllActive: {
			Assert (not GetModal ());
			ForEach (WindowPtr, it, MakeWindowIterator (eSequenceBackward)) {
				it.Current ()->DoSetupMenus ();
			}
		}
		break;
		
		case eOneActive: {
			Window*	w = GetFrontWindow ();
			if (w != Nil) {
				w->DoSetupMenus ();
			}
		}
		break;
		
		case eNoneActive: {
		}
		break;

		default:	RequireNotReached ();
	}
}

Boolean		WindowPlane::DoCommand (const CommandSelection& selection)
{
	switch (GetActivationKind ()) {
		case eAllActive: {
			Assert (not GetModal ());
			ForEach (WindowPtr, it, MakeWindowIterator ()) {
				if (it.Current ()->GetVisible () and it.Current ()->DoCommand (selection)) {
					return (True);
				}
			}
			return (False);
		}
		break;

		case eOneActive: {
			Window*	w = GetFrontWindow ();
			return Boolean ((w == Nil)? False: w->DoCommand (selection));
		}
		break;

		case eNoneActive: {
			return (False);
		}
		break;

		default:	AssertNotReached ();
	}
	AssertNotReached (); return (False);
}

Boolean	WindowPlane::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState)
{
	switch (GetActivationKind ()) {
		case eAllActive: {
			Assert (not GetModal ());
			ForEach (WindowPtr, it, MakeWindowIterator ()) {
				if (it.Current ()->DispatchKeyEvent (code, isUp, keyBoardState, composeState)) {
					return (True);
				}
			}
			return (False);
		}
		break;
		
		case eOneActive: {
			Window*	w = GetFrontWindow ();
			return Boolean ((w != Nil)? ((w->DispatchKeyEvent (code, isUp, keyBoardState, composeState))? True: GetModal ()) : False);
		}
		break;
		
		case eNoneActive: {
			return (False);
		}
		break;

		default:	AssertNotReached ();
	}
	AssertNotReached (); return (False);
}

void	WindowPlane::AddWindow (Window* w)
{
	RequireNotNil (w);
	fWindows.Prepend (w);			// make new windows frontmost within the layer
}

void	WindowPlane::RemoveWindow (Window* w)
{
	fWindows.Remove (w);
}

void	WindowPlane::ReorderWindow (Window* w, CollectionSize index)
{
	UInt32	oldIndex = fWindows.IndexOf (w);
	Assert (oldIndex != kBadSequenceIndex);
	if (index != oldIndex) {
		fWindows.Remove (w);
		fWindows.InsertAt (w, ((oldIndex < index) ? index-1 : index));
		DeskTop::Get ().SynchronizeOSWindows ();
	}
}

void	WindowPlane::ReorderWindow (Window* w, Window* behindWindow)
{
	CollectionSize	index = fWindows.IndexOf (behindWindow);
	ReorderWindow (w, ((index == kBadSequenceIndex) ? 1 : ++index));
}

SequenceIterator(WindowPtr)*	WindowPlane::MakeWindowIterator (SequenceDirection d) const
{
	return (fWindows.MakeSequenceIterator (d));
}

Window*	WindowPlane::GetFrontWindow ()	const
{
	ForEach (WindowPtr, it, MakeWindowIterator ()) {
		AssertNotNil (it.Current ());
		if (it.Current ()->GetVisible ()) {
			return (it.Current ());
		}
	}
	return (Nil);
}

Window*	WindowPlane::GetBackWindow ()	const
{
	ForEach (WindowPtr, it, MakeWindowIterator (eSequenceBackward)) {
		AssertNotNil (it.Current ());
		if (it.Current ()->GetVisible ()) {
			return (it.Current ());
		}
	}
	return (Nil);
}

Window*	WindowPlane::FindWindow (const Point& where)	const
{
	ForEach (WindowPtr, it, MakeWindowIterator ()) {
		AssertNotNil (it.Current ());
		if (it.Current ()->GetVisible () and (it.Current ()->Contains (where))) {
			return (it.Current ());
		}
	}
	return (Nil);
}

void	WindowPlane::Tile ()
{
	AssertNotReached ();	//NYI
}

void	WindowPlane::Stack ()
{
	AssertNotReached ();	//NYI
}

void	WindowPlane::AdjustWindowShellHints (WindowShellHints& originalHints)
{
}

void	WindowPlane::SynchronizeOSWindows (Window* placeBehind)
{
#if		qMacToolkit
	ForEach (WindowPtr, it, MakeWindowIterator ()) {
		AssertNotNil (it.Current ());
		Window&			w	=	*it.Current ();
		osWindowPtr		osw		=	osWindowPtr (w.GetOSRepresentation ());
		osWindowPeek	oswp	=	osWindowPeek (osw);
		AssertNotNil (osw);

		//  make sure window is in correct order on screen
		if (w.GetVisible ()) {
			if (placeBehind == Nil) {
				::BringToFront (osw);
			}
			else {
				if (osWindowPeek (placeBehind->GetOSRepresentation ())->nextWindow != oswp) {
					// IM I-286 (NOTE: IM Documentation appears incorrect!!!)
					::SendBehind (osw, osWindowPtr (placeBehind->GetOSRepresentation ()));
					if (oswp->visible != w.GetVisible ()) {
						// do this here so paint calcultions are correct
						::ShowHide (osw, w.GetVisible ());
					}
					::PaintOne (oswp, oswp->strucRgn);
					::CalcVisBehind (oswp, oswp->strucRgn);
				}
			}
		// OLD	::HiliteWindow (osw, w.ShouldBeActive ());
			Assert (&w.GetPlane () == this);
			::HiliteWindow (osw, ShouldBeActive (&w));
			placeBehind = &w;
		}
		else {
			::SendBehind (osw, Nil);
		}
		if (oswp->visible != w.GetVisible ()) {
			::ShowHide (osw, w.GetVisible ());
		}
	}
#endif	/*qMacToolkit*/
}

Boolean	WindowPlane::ShouldBeActive (const Window* w) const
{
	RequireNotNil (w);
	Require (fWindows.Contains ((Window*)w));		// not sure why cast necessary???
	Require (&w->GetPlane () == this);
	switch (GetActivationKind ()) {
		case eAllActive:
			return (w->GetVisible ());

		case eOneActive:
			return (Boolean (w == GetFrontWindow ()));

		case eNoneActive:
			return (False);

		default:	AssertNotReached ();
	}
	AssertNotReached (); return (False);
}






/*
 ********************************************************************************
 ***************************** AlertWindowPlane *********************************
 ********************************************************************************
 */

AlertWindowPlane::AlertWindowPlane ():
	WindowPlane (kModal, eOneActive)
{
}

void	AlertWindowPlane::AdjustWindowShellHints (WindowShellHints& hints)
{
	/*
	 * Center the dialog if the user has not given us an origin.
	 */
	if (not hints.DesiredOriginSet ()) {
		Rect	windowRect	=	Rect (kZeroPoint, hints.GetDesiredSize ());
		windowRect = CenterRectAroundPoint (windowRect, CalcRectPoint (GetExtent (), 0.33, 0.5));
		hints.SetDesiredOrigin (windowRect.GetOrigin ());
	}
}







/*
 ********************************************************************************
 ***************************** DialogWindowPlane ********************************
 ********************************************************************************
 */


DialogWindowPlane::DialogWindowPlane ():
	WindowPlane (kModal, eOneActive)
{
}

void	DialogWindowPlane::AdjustWindowShellHints (WindowShellHints& hints)
{
	/*
	 * Center the dialog if the user has not given us an origin.
	 */
	if (not hints.DesiredOriginSet ()) {
		Rect	windowRect	=	Rect (kZeroPoint, hints.GetDesiredSize ());
		windowRect = CenterRectAroundRect (windowRect, GetExtent ());
		hints.SetDesiredOrigin (windowRect.GetOrigin ());
	}
}









/*
 ********************************************************************************
 ***************************** StandardWindowPlane ******************************
 ********************************************************************************
 */


StandardWindowPlane::StandardWindowPlane ():
	WindowPlane (not kModal, eOneActive)
{
}

void	StandardWindowPlane::Tile ()
{
	AssertNotReached ();
}

void	StandardWindowPlane::Stack ()
{
	AssertNotReached ();
}

void	StandardWindowPlane::AdjustWindowShellHints (WindowShellHints& hints)
{
	/*
	 * Stagger the windows
	 */
	if (not hints.DesiredOriginSet ()) {
		Rect	mainDevBounds	=	DeskTop::Get ().GetMainDeviceDeskBounds (); // minus mbar - hack code
	
		const Point kWindowOffset = Point (20, 20);
		Point windowSize = hints.GetDesiredSize ();

		/*
		 * Wind thru windows in this plane, and try to place the new one at a nice spot not already
		 * occupied.
		 */
		Point		tryAt 			=	mainDevBounds.GetOrigin ();
		Coordinate	secondarySluff	=	0;
		Boolean		anyWindowsMatch =	False;
		do {
			anyWindowsMatch = False;
			ForEach (WindowPtr, it, MakeWindowIterator (eSequenceBackward)) {
				Window*	w = it.Current ();
				AssertNotNil (w);
				if ((w->GetVisible ()) and (w->GetOrigin () == tryAt)) {
					anyWindowsMatch = True;
					break;
				}
			}
			if (anyWindowsMatch) {
				tryAt += kWindowOffset;
			}
			// If were too far down, try back at top again, and just assume its OK!
			if (not (tryAt + kWindowOffset + Point (50, 50) < mainDevBounds.GetBotRight ())) {
				tryAt = mainDevBounds.GetOrigin () + Point (secondarySluff, secondarySluff);
				anyWindowsMatch = True;
				secondarySluff ++;
			}
			if (secondarySluff > 10) {
				anyWindowsMatch = False;			// punt out - assume OK
			}
		} while (anyWindowsMatch);
	
		/*
		 * Be sure window fits into main device bounds.
		 */
		Rect	newExtent	=	Rect (tryAt, windowSize);
		newExtent = newExtent * mainDevBounds;

#if		qMotifUI
		// on motif, we let the window manager decide this - only force the issue in subclasses... (like dialogs).
		hints.SetDesiredOrigin (newExtent.GetOrigin (), False);
#else
		hints.SetDesiredOrigin (newExtent.GetOrigin ());
#endif
		if (not hints.DesiredSizeSet ()) {
			hints.SetDesiredSize (newExtent.GetSize ());			// make smaler so will fit on screen
		}
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


