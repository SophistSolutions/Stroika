/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/KeyHandler.cc,v 1.3 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: KeyHandler.cc,v $
 *		Revision 1.3  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.9  1992/04/20  14:32:07  lewis
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *
 *		Revision 1.8  92/04/15  23:52:46  23:52:46  lewis (Lewis Pringle)
 *		Fix ApplyKeyCode () stuff in KeyHandler to Ignore the result of XLookupString(), and instead look at whether
 *		the keyCode at the end of the call is NonNil. This is undocumented, and possibly wrong, but the docs
 *		give no clue as to what is right - the trouble is that keystrokes like F1 have no printed representation, and
 *		so they return a buffer size of zero.
 *		Also, if we are building for motif, translate an F1 to kHelp, since the motif style guide specifies
 *		something like this. This is probably wrong, and we need to read about motif virutal keys, and get the motif virutal
 *		key for help - but this is a good enuf hack for the time being.
 *		
 *		Revision 1.7  92/03/16  16:08:42  16:08:42  sterling (Sterling Wight)
 *		lowlevel keycode interpretation for Mac
 *		
 *		Revision 1.6  1992/03/11  08:15:15  lewis
 *		Cleanup key dispatch code, and especially fixed sense of ? operator mappign isUp to KeyPress/KeyRelease.
 *
 *		Revision 1.5  1992/03/10  13:11:36  lewis
 *		Worked on (motif version of) ApplyKeyEvent.
 *
 *		Revision 1.4  1992/03/10  00:31:27  lewis
 *		Support new KeyHandler interface.
 *
 *
 */



#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Events.h>
#include	<Resources.h>
#elif	qXToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#include	<X11/keysym.h>
#endif	/*qToolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"KeyHandler.hh"










/*
 ********************************************************************************
 ************************************* KeyHandler *******************************
 ********************************************************************************
 */
Boolean	KeyHandler::HandleKeyStroke (const KeyStroke& /*keyStroke*/)
{
	return (False);
}

Boolean	KeyHandler::HandleKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
									KeyComposeState& composeState)
{
	/*
	 * Use a temporary to apply the key event so we guarentee that it is only applied once! If we
	 * are not the object to handle the event, then we do not apply this change!!!
	 */
	KeyComposeState	tmpKeyComposeState	=	composeState;
	KeyStroke		keyStroke;
	if (ApplyKeyEvent (keyStroke, code, isUp, keyBoardState, tmpKeyComposeState)) {
		if (HandleKeyStroke (keyStroke)) {
			composeState = tmpKeyComposeState;
			return (True);
		}
	}
	return (False);
}

Boolean	KeyHandler::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
									  KeyComposeState& composeState)
{
	return (HandleKeyEvent (code, isUp, keyBoardState, composeState));
}

Boolean	KeyHandler::ApplyKeyEvent (KeyStroke& keyStroke, KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
								   KeyComposeState& composeState)
{
#if		qMacToolkit
	Int16	keyCode = code;
	if (isUp) {
		keyCode |= 0x0080;
	}

	Set_BitString (KeyStrokeModifiers)	modifiers;
	if (keyBoardState.GetKey (KeyBoard::eControlKey)) {
		keyCode |= controlKey;
		modifiers.Add (KeyStroke::eControlKeyModifier);
	}
	if (keyBoardState.GetKey (KeyBoard::eOptionKey)) {
		keyCode |= optionKey;
		modifiers.Add (KeyStroke::eOptionKeyModifier);
	}
	if (keyBoardState.GetKey (KeyBoard::eCapsLockKey)) {
		keyCode |= alphaLock;
		// what to set in modifiers???
	}
	if (keyBoardState.GetKey (KeyBoard::eShiftKey)) {
		keyCode |= shiftKey;
		modifiers.Add (KeyStroke::eShiftKeyModifier);
	}
	if (keyBoardState.GetKey (KeyBoard::eCommandKey)) {
		keyCode |= cmdKey;
		modifiers.Add (KeyStroke::eCommandKeyModifier);
	}
	
	static	Int32 transState = 0;
	void** KCHRresource = ::GetResource ('KCHR', 0);
	AssertNotNil (KCHRresource);
	
	Int32 result = ::KeyTrans (*KCHRresource, keyCode, &transState);
	
	// KeyTrans supports 2 (16 bit) ASCII characters, so that a single keypress could produce
	// two characters -- our KeyStroke structure does not support this

	// as far as I can tell the documentation (IM vol V page 195) is in error, in that it inverts
	// the ordering of Ascii one and Ascii two
//	Int16 asciiOne = Int16 ((result & 0xFF00) >> 16);
	Int16 asciiOne = Int16 (result & 0x00FF);

	keyStroke = KeyStroke (asciiOne);
	keyStroke.SetModifiers (modifiers);
	
	// hack: no way on mac to tell if partial keystroke (events are only generated for
	// complete keystrokes so we never get partial keystrokes)
	return (Boolean (not isUp));
#elif	qXToolkit
	// the Way we deal with modifiers is ad-hoc and kludgy, and incomplete...

	extern	osDisplay* gDisplay;
	// code from Oreilly I-267
	const kBufferSize = 20;
	char buffer [kBufferSize];
	KeySym	keySym	=	0;
	int		charCount;
	XKeyEvent	xkeyevent;
	memset (&xkeyevent, 0, sizeof (xkeyevent));
	xkeyevent.type = isUp? KeyRelease: KeyPress;
	xkeyevent.display = gDisplay;
	xkeyevent.window = DefaultRootWindow (xkeyevent.display);
	xkeyevent.root = DefaultRootWindow (xkeyevent.display);
	xkeyevent.same_screen = True;
	// Also, fold this into the GetEffectiveKeyboard code in EventManager...
	if (keyBoardState.GetKey (KeyBoard::eShiftKey)) {
		xkeyevent.state |= ShiftMask;
	}
	if (keyBoardState.GetKey (KeyBoard::eControlKey)) {
		xkeyevent.state |= ControlMask;
	}
	xkeyevent.keycode = code;

	/*
	 * Hmm. Reading the docs on XLookupString in Orielly II-312, it is unclear how to tell that a keycode
	 * failed to map to a keystroke (keySym in their parlance). A good guess is that XLookupString returns
	 * 0 - but that turns out to be wrong in the case of function keys - they return zero, though they have
	 * valid keysyms. My next guess, is that you get a keysym of zero. Undocumented, probably un-thought out,
	 * and probably wrong, but what the hey....
	 */
	(void)::XLookupString (&xkeyevent, buffer, kBufferSize, &keySym, &composeState.ReferenceCompose ());

#if		qXmToolkit
	if (keySym == XK_F1) {
		keySym = XK_Help;		// since nobody else seems to be mapping it... Not sure what
		  						// I'm doing wrong - I thought F1 would be mapped to help automatically???
								// Maybe I need to read up on Motif Xm Virtual Keys better...
	}
#endif

	if (keySym == 0) {
		return (False);
	}
	else {
		// Assume - at least for now - that this is bad - seems to only happen with tabs??
		if (isUp) {
			return (False);
		}

		Set_BitString (KeyStrokeModifiers)	modifiers;
		if (keyBoardState.GetKey (KeyBoard::eShiftKey)) {
			modifiers.Add (KeyStroke::eShiftKeyModifier);
		}
		if (keyBoardState.GetKey (KeyBoard::eControlKey)) {
			modifiers.Add (KeyStroke::eControlKeyModifier);
		}
		keyStroke = KeyStroke (keySym);
		keyStroke.SetModifiers (modifiers);

		return (True);
	}
#endif	/*Toolkit*/
	return (False);
}





// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

