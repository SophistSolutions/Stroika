/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__KeyHandler__
#define	__KeyHandler__

/*
 * $Header: /fuji/lewis/RCS/KeyHandler.hh,v 1.2 1992/09/01 15:42:04 sterling Exp $
 *
 *
 * Description:
 *
 *		A KeyHandler is a mixin class that defines the Prefered API for recieving key events.
 * (It is also possible to do so by adding an EventHandler, or directly overriding the relevant methods
 * of the eventManager, but those mechanisms are not advised).
 *
 *		KeyHandlers have two virtual methods they can override, HandleKeyStroke, and HandleKeyEvent.
 *
 *		HandleKeyStroke is the high level interface- you use this interface by overriding this method
 * to recieve high level keystrokes (explained elsewhere HOW you recieve them) (KeyStrokes themselves
 * are described in KeyBoard.hh). If you handle the event, return True.
 *
 *		HandleKeyEvent is the low level interface. It has considerably more information passed to it
 * than HandleKeyStroke(). Use this interface if you want all that extra information, or if
 * you want to pass control of the given keystroke event to some other KeyHandler ("the target chain
 * mechanism" - explained in more depth - ELSEWHERE). The reason that you override the low level
 * interface is that then whomever you pass the key event to will have both interfaces available.
 * If you only override the High Level interface to pass the key along, only the High Level interface
 * will be available to the desired target. If you handle the event, HandleKeyStroke() returns True.
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: KeyHandler.hh,v $
 *		Revision 1.2  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/03/09  23:51:22  lewis
 *		Work on new KeyHandler interface. Removed old HandleKey method, and replaced it with three methods, HandleKeyStroke(),
 *		HandleKeyEvent(), and DispatchKeyEvent(), and also new static method ApplyKeyEvent.
 *
 *		Revision 1.5  1992/02/21  19:43:32  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *
 */
#include	"KeyBoard.hh"

#include	"Command.hh"


#if		qCanFreelyUseVirtualBaseClasses
class	KeyHandler: public virtual CommandHandler {
#else
class	KeyHandler {
#endif
	public:
		KeyHandler ();
		virtual ~KeyHandler ();

// OLD INTERFACE
//		virtual	Boolean	HandleKey (Character c, const KeyBoard& keyBoardState);

	/*
	 * Override these methods to handle keystrokes.
	 */
	public:
		virtual	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);
		virtual	Boolean	HandleKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
			KeyComposeState& composeState);
		virtual	Boolean	DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
			KeyComposeState& composeState);

	/*
	 * Routine to portably interpret the key event adding info to the ComposeState, and filling in the keyStroke
	 * parameter if the keystroke is complete (Return True - iff keyStroke complete).
	 */
	public:
		static	Boolean	ApplyKeyEvent (KeyStroke& keyStroke, KeyBoard::KeyCode code, Boolean isUp,
			const KeyBoard& keyBoardState, KeyComposeState& composeState);
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	KeyHandler::KeyHandler ()	{	}
inline	KeyHandler::~KeyHandler ()	{	}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__KeyHandler__*/
