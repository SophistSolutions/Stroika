/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__MouseHandler__
#define	__MouseHandler__

/*
 * $Header: /fuji/lewis/RCS/MouseHandler.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		It is in this class that we introduce the notion of global coordinates... (that will probably DIE).
 *		Also, we try to encapsulate in a MousePressInfo () structure all the information needed to handle
 *		a mouse press, and have portable accessors to deal with the fact that different GUI's use different
 *		gestures to refer to common actions (eg double click may mean open on some system, but right mouse button
 *		might mean that on another).
 *
 *
 * TODO:
 *
 *		Get rid of global coordiantes (use desktop coords instead - as minor detail).
 *
 *		Think out more carefully qMacToolkit vs. qMacUI for much of the mouseInfo stuff...
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: MouseHandler.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.7  1992/01/15  10:00:22  lewis
 *		No longer inerhit from Panel, and eliminate Tablet & Global Coord related methods.
 *
 *		Revision 1.6  1992/01/14  06:01:17  lewis
 *		Take into account fact that TabletOwner stuff broken out of panel, and MouseHandlker is no longer a tablet owner.
 *
 *		Revision 1.5  1992/01/03  07:16:25  lewis
 *		Use qMacToolkit/qXtToolkit, defines rather than macos, and also, have for x use of doubleclicking to mean
 *		open - for now use same interpretation as on mac.
 *
 *
 *
 *
 *
 */

#include	"KeyBoard.hh"

#include	"Command.hh"
#include	"Panel.hh"


/*
 * Instead of passing various bits of mouse press info along as parameters to TrackPress,
 * we package them up in a structure so that when we add or remove bits of info, less code needs
 * to be changed, and we can more easily send along different bits of info for different
 * windowing systems without affecting the overload/override attempts of subclasses.
 */


// SB scoped inside MouseHandler but for bevvy of compiler bugs...
class	MousePressInfo {
	public:
		MousePressInfo ();
		MousePressInfo (const MousePressInfo& from);
		MousePressInfo (const MousePressInfo& from, const Point& newPressAt);
		MousePressInfo (const Point& newPressAt);
#if		qMacToolkit || qXtToolkit
		MousePressInfo (const Point& pressAt, UInt16 clickCount, const KeyBoard& keyBoard);
#endif	/*Toolkit*/

		const MousePressInfo& operator= (const MousePressInfo& from);

		/*
		 * Allow direct access to fields, for ease of use. No sense in wrapping since just as
		 * packaging up of what would have been parameters.
		 */
		Point		fPressAt;
#if		qMacToolkit || qXtToolkit
		UInt16		fClickCount;
		KeyBoard	fKeyBoard;		// At the time of click
#endif	/*Toolkit*/
// maybe say if motif, then pass along bitmask of what buttons pressed?


	/*
	 * Once we considered passing.... Finish commenting soon.
	 */

		nonvirtual	Boolean	IsSimpleSelection () const;			// Mac SINGLE-CLICK
		nonvirtual	Boolean	IsExtendedSelection () const;		// Mac SHIFT-CLICK
		nonvirtual	Boolean	IsOpenSelection () const;			// Mac DOUBLE-CLICK
};



class	MouseHandler {
	protected:
		MouseHandler ();

	public:
		virtual	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState)		=	Nil;
		virtual	Boolean	TrackPress (const MousePressInfo& mouseInfo)												=	Nil;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__MouseHandler__*/
