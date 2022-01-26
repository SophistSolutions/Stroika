/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Cursor__
#define	__Cursor__

/*
 * $Header: /fuji/lewis/RCS/Cursor.hh,v 1.5 1992/09/01 15:34:49 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Cursor.hh,v $
 *		Revision 1.5  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  05:34:46  lewis
 *		Use Sequence_DoubleLinkList instead of Sequence_DoubleLinkListPtr which is now
 *		obsolete.
 *
 *		Revision 1.3  1992/07/02  03:36:44  lewis
 *		Renamed Sequence_DoublyLLOfPointers -> Sequence_DoubleLinkListPtr.
 *
 *		Revision 1.2  1992/07/01  03:57:21  lewis
 *		Got rid of unneeded String.hh include.
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.9  1992/04/07  10:48:38  lewis
 *		Support templates for PC port.
 *
 *
 */

#include	"Exception.hh"
#include	"Sequence.hh"
#include	"Time.hh"

#include	"Point.hh"



#if		qMacGDI
struct osCursor;
typedef	osCursor*	osCursorRep;
#elif	qXGDI
typedef	unsigned long osCursor;
typedef	osCursor	osCursorRep;
struct	osDisplay;
#endif	/*qMacGDI*/

class	Cursor {
	protected:
		Cursor (const Point& hotSpot);

	public:
		virtual ~Cursor ();

		virtual	void	Install () const;
		virtual	void	DeInstall () const;		// not necessary to call since done automatically by
												// install of another cursor.  Safe to call, even
												// if not installed, though pointless.

		virtual	Point	GetHotSpot () const;
		virtual	void	SetHotSpot (const Point& hotSpot);

#if		qMacGDI
		static	Exception	sCursorBusy;
#endif	/*qMacGDI*/

#if		qXGDI
		// X windows implementation detail - (DO NOT REFERENCE THIS CODE - SUBJECT TO CHANGE) -
		// we must have a display, and a window to install
		// the cursor into in the install method. The display is basically always the same, and the
		// window, we posit, is the input focus window. For now, we will leave it to the application
		// framework to set this variable as appropiate, and to re-call install on the current
		// cursor in the new window as needed.
		static	osDisplay*		sTheDisplay;
		static	unsigned long	sTheInputWindow;
		static	Exception		sNoDisplay;
		static	Exception		sNoWindow;
#endif	/*qXGDI*/

	public:
		static	const	Cursor*	GetInstalledCursor ();

	private:
		static	const	Cursor*	sInstalledCursor;

		Point	fHotSpot;
};


class	PixelMap;
class	String;
class	PixelMapCursor : public Cursor {
	public:
		static	const	Point	kDefaultHotSpot;

		/* 
		 * THIS IS A TEMP CTOR.  SOON, WHEN OTHER STUFF WORKS WELL, THIS
		 * WILL GO AWAY.  JUST USE SECOND CTOR, AND CONSTRUCT TEMP OBJECTS BASED
		 * ON MAC GLOBALS.
		 */
#if		qMacGDI || qXGDI
		PixelMapCursor (osCursorRep anOSCursor);
#if		!qXGDI
		PixelMapCursor (const String& resourceName);		// NB: uses cur res file, and searches map
#endif	/*!qXGDI*/
		PixelMapCursor (int resourceID);					// NB: uses cur res file, and searches map
#endif	/*qMacGDI || qXGDI*/

		/*
		 * Use the given data to make a cursor.  Either use a default algoritm to specify
		 * the hotspot, and mask, or specify them directly.
		 */
		PixelMapCursor (const PixelMap& data);
		PixelMapCursor (const PixelMap& data, const PixelMap& mask, const Point& hotSpot = kDefaultHotSpot);
		~PixelMapCursor ();


		override	void	Install () const;

#if		qMacGDI || qXGDI
		nonvirtual	osCursorRep	GetOSRepresentation () const;
#endif	/*qMacGDI || qXGDI*/

	private:
#if		qMacGDI || qXGDI
		osCursorRep	fOSRepresentation;
#if		qXGDI
		Boolean		fISOnlyCursorID;
#endif	/*qXGDI*/
#endif	/*qMacGDI || qXGDI*/
};




#if		!qRealTemplatesAvailable
	#if		qMPW_MacroOverflowProblem
		#define	PixelMapCursorPtr	PMCursorPtr
	#endif
	
	typedef	class	PixelMapCursor*	PixelMapCursorPtr;
	Declare (Iterator, PixelMapCursorPtr);
	Declare (Collection, PixelMapCursorPtr);
	Declare (AbSequence, PixelMapCursorPtr);
	Declare (Array, PixelMapCursorPtr);
	Declare (Sequence_Array, PixelMapCursorPtr);
	Declare (Sequence, PixelMapCursorPtr);
#endif


class	AnimatedCursor : public Cursor {
	public:
		static	const	Time	kDefaultFrameInterval;

		/*
		 * Note the cursors ptrs are copied, not thier data...
		 */
		AnimatedCursor (const Time& frameInterval = kDefaultFrameInterval);
#if		qRealTemplatesAvailable
		AnimatedCursor (const Collection<PixelMapCursor*>& frames, const Time& frameInterval = kDefaultFrameInterval);
#else
		AnimatedCursor (const Collection(PixelMapCursorPtr)& frames, const Time& frameInterval = kDefaultFrameInterval);
#endif
#if		qMacGDI
		AnimatedCursor (const String& resourceName);
		AnimatedCursor (int resourceID);
#endif	/*qMacGDI*/
		virtual ~AnimatedCursor ();

		override	void	ShowFrame (UInt16 frame);

		override	void	Install () const;
		override	void	DeInstall () const;

		nonvirtual	Time	GetFrameInterval () const;
		nonvirtual	void	SetFrameInterval (const Time& newFrameInterval);

#if		qRealTemplatesAvailable
		nonvirtual	const AbSequence<PixelMapCursor*>& 	GetPixelMapCursors () const;
#else
		nonvirtual	const AbSequence(PixelMapCursorPtr)& 	GetPixelMapCursors () const;
#endif

	private:
#if		qRealTemplatesAvailable
		Sequence<PixelMapCursor*> 	fPixelMapCursors;
#else
		Sequence(PixelMapCursorPtr) 	fPixelMapCursors;
#endif
		Boolean										fShowingFrame;
		Time										fFrameInterval;
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
#if		qMacGDI
inline	osCursor*	PixelMapCursor::GetOSRepresentation () const	{ return (fOSRepresentation); }
#endif	/*qMacGDI*/


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Cursor__*/
