/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Cursor.cc,v 1.7 1992/09/11 18:41:55 sterling Exp $
 *
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: Cursor.cc,v $
 *		Revision 1.7  1992/09/11  18:41:55  sterling
 *		new Shape stuff, got rid of String Peek references
 *
 *		Revision 1.6  1992/09/05  16:14:25  lewis
 *		Renamed Nil->Nil.
 *
 *		Revision 1.5  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  05:39:19  lewis
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/10  22:34:00  lewis
 *		Scoped Time constants.
 *
 *		Revision 1.2  1992/07/02  03:41:32  lewis
 *		Renamed Sequence_DoublyLLOfPointers -> SequencePtr.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.15  1992/05/13  00:34:55  lewis
 *		Minor syntax error.
 *
 *		Revision 1.14  92/05/10  00:13:51  00:13:51  lewis (Lewis Pringle)
 *		Fixed minor syntax errors porting to BC++/templates. And minor changes to compile under qWinGDI.
 *		
 *		Revision 1.13  92/04/20  14:15:36  14:15:36  lewis (Lewis Pringle)
 *		added typedef char* caddr_t conditional on qSnake && _POSIX_SOURCE, before include of Xutil.h, since that
 *		file references caddr_t which doesn't appear to be part of POSIX.
 *		
 *		Revision 1.12  92/04/07  11:02:13  11:02:13  lewis (Lewis Pringle)
 *		PC port changes.
 *		
 *		Revision 1.10  1992/01/21  21:01:05  lewis
 *		Use the ForEach() macro for iterator.
 *
 *
 */


#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<OSUtils.h>
#include	<Memory.h>
#include	<QuickDraw.h>
#include	<Resources.h>
#include	<SysEqu.h>
#elif	qXGDI
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#endif	/*qXGDI || qMacGDI*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"File.hh"
#include	"OSConfiguration.hh"
#include	"Scheduler.hh"
#include	"Task.hh"

#include	"Cursor.hh"



#if		!qRealTemplatesAvailable
	Implement (Iterator, PixelMapCursorPtr);
	Implement (Collection, PixelMapCursorPtr);
	Implement (AbSequence, PixelMapCursorPtr);
	Implement (Array, PixelMapCursorPtr);
	Implement (Sequence_Array, PixelMapCursorPtr);
	Implement (Sequence, PixelMapCursorPtr);
#endif





/*
 ********************************************************************************
 *************************************** Cursor *********************************
 ********************************************************************************
 */

#if		qMacGDI
Exception	Cursor::sCursorBusy;
#endif	/*qMacGDI*/

#if		qXGDI
osDisplay*		Cursor::sTheDisplay			=	Nil;
unsigned long	Cursor::sTheInputWindow		=	Nil;
Exception		Cursor::sNoDisplay;
Exception		Cursor::sNoWindow;
#endif	/*qXGDI*/

const	Cursor*	Cursor::sInstalledCursor	=	Nil;

Cursor::Cursor (const Point& hotSpot):
	fHotSpot (hotSpot)
{
}

Cursor::~Cursor ()
{
	DeInstall ();
}

void	Cursor::Install () const
{
	if ((sInstalledCursor != Nil) and (sInstalledCursor != this)) {
		sInstalledCursor->DeInstall ();
	}
	sInstalledCursor = this;
}

void	Cursor::DeInstall () const
{
	/*
	 * No-Op if not installed.
	 */
	if (sInstalledCursor == this) {
		sInstalledCursor = Nil;
	}
}

Point	Cursor::GetHotSpot () const
{
	return (fHotSpot);
}

void	Cursor::SetHotSpot (const Point& hotSpot)
{
	fHotSpot = hotSpot;
}

const	Cursor*	Cursor::GetInstalledCursor ()
{
	return (sInstalledCursor);
}







/*
 ********************************************************************************
 ********************************** PixelMapCursor ******************************
 ********************************************************************************
 */


#if		qMacGDI || qXGDI
/* 
 * THIS IS A TEMP CTOR.  SOON, WHEN OTHER STUFF WORKS WELL, THIS
 * WILL GO AWAY.  JUST USE SECOND CTOR, AND CONTRUCT TEMP OBJECTS BASED
 * ON MAC GLOBALS.
 */
PixelMapCursor::PixelMapCursor (osCursorRep anOSCursor):
#if		qMacGDI
	Cursor (os_cvt (anOSCursor->hotSpot)),
#elif	qXGDI
	Cursor (kZeroPoint),
#endif	/*qMacGDI || qXGDI*/
	fOSRepresentation (Nil)
#if		qXGDI
	,fISOnlyCursorID (False)
#endif	/*qXGDI*/
{
#if		qMacGDI
	fOSRepresentation = new osCursor;
	RequireNotNil (anOSCursor);
	memcpy (fOSRepresentation, anOSCursor, sizeof (osCursor));
#elif	qXGDI
	fOSRepresentation = anOSCursor;
#endif	/*qMacGDI || qXGDI*/
}

#if		!qXGDI
PixelMapCursor::PixelMapCursor (const String& resourceName):
	Cursor (kZeroPoint),
	fOSRepresentation (Nil)
{
	osStr255	name;
	resourceName.ToPStringTrunc (name, sizeof (name));
	osHandle	h			=	::GetNamedResource ('CURS', name);		// currently no support for color cursors!
	if (h == Nil) {
		FileSystem::sResourceNotFound.Raise ();
	}
	fOSRepresentation = new osCursor;
	char		oldState	=	::HGetState (h);
	::HLock (h);
	memcpy (fOSRepresentation, *h, sizeof (osCursor));
	::HSetState (h, oldState);
}
#endif	/*!qXGDI*/

PixelMapCursor::PixelMapCursor (int resourceID):
	Cursor (kZeroPoint),
	fOSRepresentation (Nil)
#if		qXGDI
	,fISOnlyCursorID (True)
#endif	/*qXGDI*/
{
#if		qMacGDI
	osHandle	h = ::GetResource ('CURS', resourceID);
	if (h == Nil) {
		FileSystem::sResourceNotFound.Raise ();
	}
	fOSRepresentation = new osCursor;
	char		oldState	=	::HGetState (h);
	::HLock (h);
	memcpy (fOSRepresentation, *h, sizeof (osCursor));
	::HSetState (h, oldState);
#elif	qXGDI
	fOSRepresentation = resourceID;
	if (sTheDisplay != Nil) {			// if its still nil, we'll do this later, just wanted to
										// fail as early as possible
		fOSRepresentation = ::XCreateFontCursor (sTheDisplay, fOSRepresentation);
		// check for errors and raise exception if any!!!!
	}
#endif	/*qMacGDI || qXGDI*/
}
#endif	/*qMacGDI || qXGDI*/

PixelMapCursor::PixelMapCursor (const PixelMap& data):
	Cursor (kZeroPoint)
#if		qMacGDI || qXGDI
	,fOSRepresentation (Nil)
#endif
#if		qXGDI
	,fISOnlyCursorID (False)
#endif	/*qXGDI*/
{
#if		qMacGDI
	fOSRepresentation = new osCursor;
	AssertNotImplemented ();		// NYI
#elif	qXGDI
	AssertNotImplemented ();		// NYI
#endif	/*qMacGDI || qXGDI*/
}

PixelMapCursor::PixelMapCursor (const PixelMap& data, const PixelMap& mask, const Point& hotSpot):
	Cursor (hotSpot)
#if		qMacGDI || qXGDI
	,fOSRepresentation (Nil)
#endif
#if		qXGDI
	,fISOnlyCursorID (False)
#endif	/*qXGDI*/
{
	if (hotSpot == kDefaultHotSpot) {		// magic bad # so we know default asked for
		SetHotSpot (kZeroPoint);			// should do better, but this will do for now
	}
#if		qMacGDI
	fOSRepresentation = new osCursor;
	AssertNotImplemented ();		// NYI
#elif	qXGDI
	AssertNotImplemented ();		// NYI
#endif	/*qMacGDI || qXGDI*/
}

PixelMapCursor::~PixelMapCursor ()
{
#if		qMacGDI
	delete (fOSRepresentation);
#elif	qXGDI
	if (not fISOnlyCursorID) {
		RequireNotNil (sTheDisplay);
		::XFreeCursor (sTheDisplay, fOSRepresentation);
	}
#endif	/*qMacGDI || qXGDI*/
}

void	PixelMapCursor::Install () const
{
#if		qMacGDI
	if (*(unsigned char*)CrsrBusy) {		// see develop magazine Vol 2, Issue 1, Page 34
		sCursorBusy.Raise ();
	}

	Cursor::Install ();
	
	::osSetCursor (GetOSRepresentation ());
#elif	qXGDI
	if (sTheDisplay == Nil) {
		sNoDisplay.Raise ();
	}
	if (sTheInputWindow == Nil) {
		sNoWindow.Raise ();
	}
	::XDefineCursor (sTheDisplay, sTheInputWindow, GetOSRepresentation ());
	Cursor::Install ();
#endif	/*qMacGDI*/
}

#if		qXGDI
osCursorRep	PixelMapCursor::GetOSRepresentation () const
{
	if (fISOnlyCursorID) { 
		if (sTheDisplay == Nil) {
			sNoDisplay.Raise ();
		}
		volatile PixelMapCursor*	non_const_this	=	this;	// we are conceptually const - just computing a cache...
		non_const_this->fOSRepresentation = ::XCreateFontCursor (sTheDisplay, fOSRepresentation);
		// check for errors!!!
	}
	return (fOSRepresentation);
}
#endif	/*qXGDI*/




/*
 ********************************************************************************
 ********************************** AnimatedCursor ******************************
 ********************************************************************************
 */

class	CursorTask : public Task {
	public:
		CursorTask ():
			fCurrentFrameIndex (1),
			fAnimatedCursor (Nil)
			{
			}

		nonvirtual	void	SetAnimatedCursor (AnimatedCursor* ac, const Time& frameInterval)
			{
#if		qMacGDI
				if (fAnimatedCursor != ac) {
					if (fAnimatedCursor != Nil) {
						TimeManagerQueue::Get ().Remove (this);
					}
					fAnimatedCursor = ac;
					if (fAnimatedCursor != Nil) {
						TimeManagerQueue::Get ().Add (this, frameInterval);
					}
				}
#endif	/*qMacGDI*/
			}

		override	void	RunABit ()
			{
				if (fAnimatedCursor != Nil) {
					Assert (fCurrentFrameIndex >= 1);
					if (fCurrentFrameIndex <= fAnimatedCursor->GetPixelMapCursors ().GetLength ()) {
						Try {
							fAnimatedCursor->ShowFrame (fCurrentFrameIndex);
						}
#if		qMacGDI
						Catch1 (Cursor::sCursorBusy) {
							return;					// no problem - just don't increment current cursor pointer...
						}
#endif	/*qMacGDI*/
						Catch () {
							_this_catch_->Raise ();	// anything else is propably bad...
						}
					}
					if (++fCurrentFrameIndex > fAnimatedCursor->GetPixelMapCursors ().GetLength ()) {
						fCurrentFrameIndex = 1;
					}
				}
			}

	private:
		UInt16			fCurrentFrameIndex;
		AnimatedCursor*	fAnimatedCursor;
};
static	CursorTask	sCursorTask;


AnimatedCursor::AnimatedCursor (const Time& frameInterval):
	Cursor (kZeroPoint),
	fPixelMapCursors (),
	fFrameInterval (frameInterval),
	fShowingFrame (False)
{
}

#if		qRealTemplatesAvailable
AnimatedCursor::AnimatedCursor (const Collection<PixelMapCursor*>& frames, const Time& frameInterval):
#else
AnimatedCursor::AnimatedCursor (const Collection(PixelMapCursorPtr)& frames, const Time& frameInterval):
#endif
	Cursor (kZeroPoint),
	fPixelMapCursors (),
	fFrameInterval (frameInterval),
	fShowingFrame (False)
{
#if		qRealTemplatesAvailable
	ForEach (PixelMapCursor*, it, frames) {
		RequireNotNil (it.Current ());
		fPixelMapCursors.Append (it.Current ());
	}
#else
	ForEach (PixelMapCursorPtr, it, frames) {
		RequireNotNil (it.Current ());
		fPixelMapCursors.Append (it.Current ());
	}
#endif
	if (fPixelMapCursors.GetLength () > 0) {
		AssertNotNil (fPixelMapCursors[1]);
		SetHotSpot (fPixelMapCursors[1]->GetHotSpot ());	// good default
															// we ignore hotspot of subcursors, and
															// only use our value.

// actually to make things work right on the mac we need to either force all hotspots to be the
// same, or probably ignore OUT hotspot, and only use that from frame????

	}
}

#if		qMacGDI

AnimatedCursor::AnimatedCursor (const String& resourceName):
	Cursor (kZeroPoint),
	fPixelMapCursors (),
	fFrameInterval (kDefaultFrameInterval),
	fShowingFrame (False)
{
	osStr255	name;
	resourceName.ToPStringTrunc (name, sizeof (name));
	osHandle	h = ::GetNamedResource ('acur', name);		// currently no support for color cursors!
	if (h == Nil) {
		FileSystem::sResourceNotFound.Raise ();
	}
	short	frameCount	=	**(short**)h;
	size_t	aLen		=	::GetHandleSize (h);
	short*	ac	=	(short*)new char [aLen];
	memcpy (ac, *h, aLen);
		for (int i = 1; i <= frameCount; i++) {
			fPixelMapCursors.Append (new PixelMapCursor (ac[i*2]));
		}
	delete (ac);

	if (fPixelMapCursors.GetLength () > 0) {
		AssertNotNil (fPixelMapCursors[1]);
		SetHotSpot (fPixelMapCursors[1]->GetHotSpot ());	// good default
															// we ignore hotspot of subcursors, and
															// only use our value.

// actually to make things work right on the mac we need to either force all hotspots to be the
// same, or probably ignore OUT hotspot, and only use that from frame????

	}
}

AnimatedCursor::AnimatedCursor (int resourceID):
	Cursor (kZeroPoint),
	fPixelMapCursors (),
	fFrameInterval (kDefaultFrameInterval),
	fShowingFrame (False)
{
	osHandle	h = ::GetResource ('acur', resourceID);
	if (h == Nil) {
		FileSystem::sResourceNotFound.Raise ();
	}
	short	frameCount	=	**(short**)h;
	size_t	aLen		=	::GetHandleSize (h);
	short*	ac	=	(short*)new char [aLen];
	memcpy (ac, *h, aLen);
		for (int i = 1; i <= frameCount; i++) {
			fPixelMapCursors.Append (new PixelMapCursor (ac[i*2]));
		}
	delete (ac);

	if (fPixelMapCursors.GetLength () > 0) {
		AssertNotNil (fPixelMapCursors[1]);
		SetHotSpot (fPixelMapCursors[1]->GetHotSpot ());	// good default
															// we ignore hotspot of subcursors, and
															// only use our value.

// actually to make things work right on the mac we need to either force all hotspots to be the
// same, or probably ignore OUT hotspot, and only use that from frame????

	}
}

#endif	/*qMacGDI*/

AnimatedCursor::~AnimatedCursor ()
{
	if (GetInstalledCursor () == this) {
		Cursor::DeInstall ();
		sCursorTask.SetAnimatedCursor (Nil, 0);
	}
}

void	AnimatedCursor::Install () const
{
	Require (fPixelMapCursors.GetLength () > 0);
	Cursor::Install ();
	sCursorTask.SetAnimatedCursor ((AnimatedCursor*)this, GetFrameInterval ());
}

void	AnimatedCursor::DeInstall () const
{
	if (!fShowingFrame) {
		Cursor::DeInstall ();
		sCursorTask.SetAnimatedCursor (Nil, 0);
	}
}

void	AnimatedCursor::ShowFrame (UInt16 frame)
{
	Exception*	anException	=	Nil;
	AssertNotNil (fPixelMapCursors [frame]);
	Assert (!fShowingFrame);
	fShowingFrame = True;					// to prevent de-install
	Try {
		fPixelMapCursors [frame]->Install ();
	}
	Catch () {
		anException = _this_catch_;
	}
	Cursor::Install ();						// to put us back
	fShowingFrame = False;
	if (anException != Nil) {
		anException->Raise ();
	}
}

Time	AnimatedCursor::GetFrameInterval () const
{
	return (fFrameInterval);
}

void	AnimatedCursor::SetFrameInterval (const Time& newFrameInterval)
{
	Require (newFrameInterval >= 0);
	fFrameInterval = newFrameInterval;
}

#if		qRealTemplatesAvailable
const AbSequence <PixelMapCursor*>& 	AnimatedCursor::GetPixelMapCursors () const
#else
const AbSequence(PixelMapCursorPtr)& 	AnimatedCursor::GetPixelMapCursors () const
#endif
{
	return (fPixelMapCursors);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

