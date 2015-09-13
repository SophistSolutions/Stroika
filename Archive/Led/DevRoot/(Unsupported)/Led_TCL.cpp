/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1997.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/Led_TCL.cpp,v 2.19 1997/06/18 20:08:49 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Led_TCL.cpp,v $
 *	Revision 2.19  1997/06/18 20:08:49  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.18  1997/01/20  05:25:45  lewis
 *	Mostly support for new eDefaultUpdate UpdateMode.
 *
 *	Revision 2.17  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.16  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1996/09/30  14:26:23  lewis
 *	qSupportEnterIdleCallback. And minor changes for new scrollbar code changes.
 *
 *	Revision 2.14  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.13  1996/08/05  06:06:42  lewis
 *	A number of changes since we haven't tried to compile this in
 *	months. FontSpec was teh main change. And no more const_cast() macro.
 *
 *	Revision 2.12  1996/03/04  07:47:01  lewis
 *	support for new ProcessSimpleClick() helper/hook which
 *	allows for supporting open/obj select for clicks in embeddings.
 *
 *	Revision 2.11  1996/02/26  22:36:03  lewis
 *	Cleaned up cut and paste stuff somewhat - based on TextInteractor changes.
 *	Renamed TextInteracter --> TextInteractor.
 *
 *	Revision 2.10  1996/02/05  04:19:29  lewis
 *	Get rid of SpecifyTextStore and arg TextStore stuff. Now handled in
 *	class Led.
 *
 *	Revision 2.9  1996/01/22  05:21:40  lewis
 *	When clicking - doa GetCommandHandler ()->BreakInGroupedCommands ().
 *
 *	Revision 2.8  1996/01/11  08:18:16  lewis
 *	New word selection code, including bug fixes, like sometimes dropped double
 *	clicks, and added new conditional support for qDoubleClickSelectsSpaceAfterWord.
 *
 *	Revision 2.7  1995/12/15  02:38:21  lewis
 *	A couple minor fixes for AsQDRect stuff.
 *
 *	Revision 2.6  1995/12/13  05:56:51  lewis
 *	Moved a bunch of caret support from TextInteractor to here, and unified
 *	under a single paradigm of InvalidateCaret() as we've always done
 *	on the PC.
 *
 *	Revision 2.5  1995/11/25  00:26:05  lewis
 *	Hacked StandardStyledWordWrappedLed_TCL::UpdateMenus () to support
 *	paste of embedded objects, and similarly
 *	StandardStyledWordWrappedLed_TCL::OnPasteCommand(). Not real happy
 *	about how this is organized, so may wnat to redo it.
 *
 *	Revision 2.4  1995/11/02  22:28:09  lewis
 *	Get rid of initializing default font - now done by TextImager::Get??DefaultFont
 *	Check if winodwRect.IsEmptu() in calls UpdateWindowRect and RefreshWnidowRect.
 *	Optimization in case underlying toolkits don't handle these cases efficeintly
 *	Reworked nPasteCommand () to share code, and support style runs.
 *	OnCopyCommand supports style runs.
 *	GetTextStyle() override so TCL updating of FONT menu etc chekcmarks work for
 *	styled text.
 *
 *	Revision 2.3  1995/10/19  22:22:28  lewis
 *	Add StandardStyledWordWrappedLed_TCL.
 *	FIxup SetFont code so doesn't copy old GetDefaultFont() and just marks
 *	valid things being set.
 *	And other cleanups.
 *
 *	Revision 2.2  1995/10/09  22:34:20  lewis
 *	Added override of Led_TCL_::NotificationOf_Scrolling to call
 *	updatescorllbar. Eliminated old code in textInteractor that did this
 *	by default.
 *	And lost now uneeded overide of Replace.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.20  1995/06/08  05:16:16  lewis
 *	Code cleanups
 *
 *	Revision 1.19  1995/06/02  06:21:49  lewis
 *	Fix UpdateModes for DoCursorEdit calls (SPR # 0307).
 *	Fix what we select on autoscroll out of window (SPR#145).
 *
 *	Revision 1.18  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.17  1995/05/29  23:49:28  lewis
 *	Mainly using const_cast () macro instead of manual cast - SPR0289.
 *
 *	Revision 1.16  1995/05/21  17:51:57  lewis
 *	Use qLedFirstIndex - SPR 0272
 *	Moved qLotsOfSubclassesFromMixinPointYeildTemporarilyBadVTable back here
 *	since need concrete TextStore.
 *	Lose InsertAfter/DeleteAfter (SPR 0270).
 *
 *	Revision 1.15  1995/05/20  05:06:32  lewis
 *	Huge changes and cleanups. Mainly for SPR 0261- uses WordWrappedTextIteracter
 *	to sahre code.
 *	Also some cahnges for 0263 - WindowRect/LayourRect cahnge.
 *	Many cleanups - 0262..
 *
 *	Revision 1.14  1995/05/16  06:43:38  lewis
 *	Have various overload/overrides of SetSelection all vector through one routine
 *	(the one iwth the UpdateMode) so in subclasses we only need to override
 *	the one.
 *
 *	Revision 1.13  1995/05/09  23:41:06  lewis
 *	Lose unused withWhat parameter to Pre/PostReplace - SPR 0240.
 *	Call OnTypedNormalCharacter on character messages, and provide
 *	QueryInputKeyStrokesPending () override so we can vector our on-typing
 *	code from Led_TCL/Led_MFC to TextInteractor_.
 *	LedSPR#0242.
 *
 *	Revision 1.12  1995/05/08  03:23:28  lewis
 *	Lots of fixes to ScrollSoSowing etc for SPR 0236.
 *	Lose Led_TCL_::NotificationOf_Scrolling as part of SPR 0236.
 *
 *	Revision 1.11  1995/05/06  19:52:12  lewis
 *	qPeekForMoreCharsOnUserTyping fix - SPR 0231.
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *
 *	Revision 1.10  1995/05/05  19:50:20  lewis
 *	Led_SmallStackBuffer now templated class
 *
 *	Revision 1.9  1995/04/20  06:47:46  lewis
 *	New MWERKS compiler has bad headers for ::GetScript function. No matter -
 *	looks like we should be using ::GetScriptVariable().
 *
 *	Revision 1.8  1995/04/18  00:15:41  lewis
 *	Simplify code in call to SetCaretShownAfterPos in DoClick() routine.
 *
 *	Revision 1.7  1995/04/16  19:32:00  lewis
 *	Get rid of SimpleTextInteractor - SPR # 205.
 *	Support for setting flag about where to display caret - SPR 204.
 *	Fix to CopyTextRange - thanx Sterl - SPR 201.
 *	SPR 203.
 *
 *	Revision 1.6  1995/03/29  21:08:14  lewis
 *	Fixed InsertTextPtr () - SPR # 1088.
 *
 *	Revision 1.5  1995/03/23  03:53:34  lewis
 *	Fixed SimpleLed_TCL::SelectWholeLineAfter/SimpleLed_ChunkedArray_TCL::SelectWholeLineAfter.
 *	See SPR 177.
 *
 *	Revision 1.4  1995/03/17  03:07:05  lewis
 *	Override thumb drag routine for scrollpane cuz way it calls (indirectly)
 *	Scroll() loses information.
 *	Also cleaned up and debugged scrolling (SPR 171, 147).
 *
 *	Revision 1.3  1995/03/13  03:21:48  lewis
 *	Renamed a few routines to have Led_ prefix to avoid name
 *	conflicts.
 *	And fixed problem with GetTextHandle() method so it would use second
 *	buffer since otherwise we got error in NL2Native call.
 *
 *	Revision 1.2  1995/03/02  05:46:10  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:46  lewis
 *	Initial revision
 *
 *
 *
 */
#include	"CBartender.h"
#include	"CClipboard.h"
#include	"CScrollBar.h"
#include	"CScrollPane.h"
#include	"CWindow.h"

#include	"SimpleTextStore.h"

#include	"Led_TCL.h"





/*
 *	BIG PICTURE for integrating into TCL Panrorama stuff/Scrolling.
 *
 *		The plan is to use a horizontal unit of pixels, and vertical unit
 *	of character number of first character in row at top-left of screen.
 *
 *		One implication of this is that the vScale field of CPanorama
 *	is nonsense, and so we must override all uses of it.
 */




extern	CClipboard*		gClipboard;
extern	CBartender*		gBartender;
extern 	short			gClicks;
extern	EventRecord		gLastMouseUp;
extern	CBureaucrat*	gGopher;





// From THINK Reference "Keyboard Compatability"
const	kLeftArrowCharCode	=	0x1c;
const	kRightArrowCharCode	=	0x1d;
const	kUpArrowCharCode	=	0x1e;
const	kDownArrowCharCode	=	0x1f;





/*
 ********************************************************************************
 *********************** LedSupport class lib support ***************************
 ********************************************************************************
 */

void	Led_ThrowOutOfMemoryException ()
{
	FailNIL (0);
	Led_Assert (false);	// better never reach this point!
}

void	Led_ThrowBadFormatDataException ()
{
	FailOSErr (memFullErr);
	Led_Assert (false);	// better never reach this point!
}

void	Led_ThrowOSErr (OSErr err)
{
	if (err != noErr) {
		FailOSErr (err);
		Led_Assert (false);	// better never reach this point!
	}
}








/*
 ********************************************************************************
 *********************************** Led_TCL_ ***********************************
 ********************************************************************************
 */
Led_TCL_::Led_TCL_ (CView *anEnclosure, CBureaucrat *aSupervisor,
					short aWidth, short aHeight,
					short aHEncl, short aVEncl,
					SizingOption aHSizing,
					SizingOption aVSizing,
					short aLineWidth, Boolean aScrollHoriz
		):
	TextImager_ (),
	TextInteractor_ (),
	CAbstractText (anEnclosure, aSupervisor, aWidth, aHeight, aHEncl, aVEncl, aHSizing, aVSizing, aLineWidth, aScrollHoriz),
#if		qDebug
	fAcquireCount (0),
#endif
	fDragAnchor (0),
	fLastDrawnOn (false),
	fTickCountAtLastBlink (0),
	fSpacingCmd (cmdSingleSpace),
	fAlignCmd (cmdAlignLeft)
{
	if (aLineWidth == -1) {		// In TCL - -1 special flag which means full window...
		aLineWidth = aWidth;
	}
	Led_Assert (aLineWidth > 0);
	SetLayoutWidth (aLineWidth);
	UseLongCoordinates (true);

	/*
	 *	Not really sure why this is necessary since it doesn't appear to be done
	 *	by TCL in CEditText. But - if you don't do it, and include multiple
	 *	Led edittexts in a window - they all come up active at the start.
	 */
	active = false;
}

Led_TCL_:: ~Led_TCL_ ()
{
	Led_Assert (fAcquireCount == 0);
}

void	Led_TCL_::SetTextPtr (Ptr textPtr, long numChars)
{
	Led_SmallStackBuffer<Led_tChar>	buf (numChars);
	Led_NativeToNL (textPtr, numChars, buf, numChars);

	/*
	 *	Note that by doing a Delete followed by an Insert() instead of just doing
	 *	a replace - we are in-effect interpretting this as destroying all
	 *	markers. And - also - this SetTextPtr () operation could fail in the middle
	 *	leaving an empty text buffer as the result.
	 */
	Replace (qLedFirstIndex, GetLength () + qLedFirstIndex, "", 0);
	Replace (qLedFirstIndex, qLedFirstIndex, buf, numChars);
}

Handle		Led_TCL_::GetTextHandle ()
{
	/*
	 *	Note use of this routine is deprecated - since the CEditText implementation
	 *	returns a handle which is never freed.  This is pathetic API design! But what can
	 *	I do but live with it and try to hack around it.
	 *
	 *	To as closely as possible mimic this brain-damage - I keep a static handle around
	 *	which contains the appropriate text. And I just keep returning that handle.
	 *	This means that if anybody ever uses this API - it will waste a bit of memory.
	 *	Also - if multiple instances of a Led edit text are manipluated - they will share
	 *	the same handle. This is roughly equivilent - but not exactly - to the semantics you
	 *	would have had with CEditText.
	 *
	 *	Bottom line: this should work OK for nearly all usages, but you are MUCH better off
	 *	using different APIs.
	 */
	static	Handle	h	=	::NewHandle (0);
	FailNIL (h);
	unsigned char	oldState	=	::HGetState (h);

	size_t	len	=	GetLength ();

	Led_SmallStackBuffer<Led_tChar>	buf (len);
	CopyOut (qLedFirstIndex, len, buf);

	::HUnlock (h);
	::SetHandleSize (h, len);
	::FailMemError ();
	::HLock (h);
	Led_NLToNative (buf, len, *h, len);

	::HSetState (h, oldState);

	return (h);
}

Handle		Led_TCL_::CopyTextRange (long start, long end)
{
	Led_Assert (start >= 0);
	Led_Assert (start <= end);
	Led_Assert (start <= GetLength ());
	end = Min (end, GetLength ());		// do the pin here cuz CEditText::CopyTextRange does!
	Led_Assert (start <= end);
	size_t	len = end - start;
	Handle	h	=	::NewHandle (len);
	FailNIL (h);
	if (len != 0) {
		Led_SmallStackBuffer<Led_tChar>	buf (len);
		CopyOut (start + qLedFirstIndex, len, buf);
		HLock (h);
		Led_NLToNative (buf, len, *h, len);
		HUnlock (h);
	}
	return (h);
}

void	Led_TCL_::InsertTextPtr (Ptr text, long length, Boolean /*fRedraw*/)
{
	/*
	 *	We are trying to replicate the semantics of what TCL does with CEditText. It calls
	 *	TEPaste(). Reading the docs on TEPaste () - they insert the next just BEFORE
	 *	the beginning of the selection, leaving the same text selected as before.
	 *	The docs are vague if this means the same text (as we implement here) or
	 *	the same numeric range (my guess is that is what they really mean - but we'll
	 *	interpret them literally til proven otherwise - LGP 950326)/
	 */
	Led_SmallStackBuffer<Led_tChar>	buf (length);
	length = Led_NormalizeTextToNL (text, length, buf, length);			// silently strip any bad characters or funny NLs
	Replace (GetSelectionStart (), GetSelectionStart (), buf, length);
	// Do update regardless of flag from TCL - since they pass wrong flag
	// on undo of text operation!!! LGP 941018
	Refresh ();
}

void		Led_TCL_::SetFontNumber (short aFontNumber)
{
//	FontSpecification	defaultFont	=	GetDefaultFont ();
	Led_IncrementalFontSpecification	defaultFont;
	defaultFont.SetFontNameSpecifier (aFontNumber);
	SetDefaultFont (defaultFont);
}

void		Led_TCL_::SetFontStyle (short aStyle)
{
//	FontSpecification	defaultFont	=	GetDefaultFont ();
	Led_IncrementalFontSpecification	defaultFont;
//	defaultFont.fFontStyle = aStyle;
//	defaultFont.fFontStyleValid = true;
	if (aStyle == 0) {
		defaultFont.SetStyle_Plain ();
	}
	defaultFont.SetStyle_Bold (aStyle & bold);
	defaultFont.SetStyle_Italic (aStyle & italic);
	defaultFont.SetStyle_Underline (aStyle & underline);
	defaultFont.SetStyle_Outline (aStyle & outline);
	defaultFont.SetStyle_Shadow (aStyle & shadow);
	defaultFont.SetStyle_Condensed (aStyle & condense);
	defaultFont.SetStyle_Extended (aStyle & extend);
	SetDefaultFont (defaultFont);
}

void		Led_TCL_::SetFontSize (short aSize)
{
//	FontSpecification	defaultFont	=	GetDefaultFont ();
	Led_IncrementalFontSpecification	defaultFont;
	defaultFont.SetPointSize (aSize);
	SetDefaultFont (defaultFont);
}

void	Led_TCL_::SetTextMode (short aMode)
{
	Led_Assert (false);	// NYI
}

void	Led_TCL_::SetAlignCmd (long anAlignment)
{
	Led_Assert (false);	// NYI
}

long	Led_TCL_::GetAlignCmd ()
{
	return (fAlignCmd);
}

void	Led_TCL_::SetSpacingCmd (long aSpacingCmd)
{
	Led_Assert (false);	// NYI
}

long	Led_TCL_::GetSpacingCmd ()
{
	return (fSpacingCmd);
}

long	Led_TCL_::GetHeight (long startLine, long endLine)
{
	Led_Assert (startLine >= 0);
	Led_Assert (endLine >= 0);
	Led_Assert (startLine <= endLine);

	startLine = Min (startLine, GetRowCount ()-qLedFirstIndex);
	endLine = Min (endLine, GetRowCount ()-qLedFirstIndex);

	/*
	 *	Parameters here are zero-based, and it is assumed that you mean start/endline
	 *	inclusive - so if the two are equal, we expect the height of 1 line.
	 *	TCL Lines map to what we refer to as 'rows' in Led (LInes are LF terminated).
	 *
	 *	Also, note we measure top of first line to bottom of last. This neglects interline
	 *	distance for ONLY the last line.
	 */
	size_t	firstCharOfStartLine	=	GetStartOfRow (startLine+qLedFirstIndex);
	size_t	firstCharOfEndLine		=	GetStartOfRow (endLine+qLedFirstIndex);
	return (GetCharLocation (firstCharOfEndLine).bottom - GetCharLocation (firstCharOfStartLine).top);
}

long	Led_TCL_::GetCharOffset (LongPt* aPt)
{
	Led_AssertNotNil (aPt);
	return (GetCharAtLocation (AsLedPoint (*aPt)) - qLedFirstIndex);
}

void	Led_TCL_::GetCharPoint (long offset, LongPt* aPt)
{
	Led_AssertNotNil (aPt);
	Led_Rect	result	=	GetCharLocation (offset + qLedFirstIndex);
	aPt->v = result.top;
	aPt->h = result.left;
}

void	Led_TCL_::GetCharStyle (long charOffset, TextStyle* theStyle)
{
	Led_Assert (false);	// NYI
}

void	Led_TCL_::GetTextStyle (short* whichAttributes, TextStyle* aStyle)
{
	Led_AssertNotNil (whichAttributes);
	Led_AssertNotNil (aStyle);
	*whichAttributes = doFont | doSize | doFace;
	Led_IncrementalFontSpecification	fontSpec	=	GetDefaultFont ();
	//aStyle->tsFont = fontSpec.fFontID;
	//aStyle->tsSize = fontSpec.fFontSize;
	//aStyle->tsFace = fontSpec.fFontStyle;
	fontSpec.GetOSRep (&aStyle->tsFont, &aStyle->tsSize, &aStyle->tsFace);
}

long	Led_TCL_::FindLine (long charPos)
{
	return (GetRowContainingPosition (charPos + qLedFirstIndex));
}

long	Led_TCL_::GetLength ()
{
	const	Led_TCL_*	constThis	=	this;
	return (constThis->GetLength ());
}

size_t	Led_TCL_::GetLength () const
{
	return GetTextStore ().GetLength ();
}

long	Led_TCL_::GetNumLines ()
{
	// TCL Lines map to what we refer to as 'rows' in Led (LInes are LF terminated).
	return (GetRowCount ());
}

void	Led_TCL_::DoKeyDown (char theChar, Byte keyCode, EventRecord* macEvent)
{
	bool	shiftPressed	=	!!(macEvent->modifiers&shiftKey);
	/*
	 *	Behavior here as listed in Mac. Human Interface Guidelines - See Led SPR#0122
	 */
    if (macEvent->modifiers & cmdKey) {
		switch (theChar) {
			case kLeftArrowCharCode: {
				DoSingleCharCursorEdit (eCursorToStart, eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
				#if		qPeekForMoreCharsOnUserTyping
					UpdateIfNoKeysPending ();
				#endif
				return;
			}
			break;
	
			case kRightArrowCharCode: {
				DoSingleCharCursorEdit (eCursorToEnd, eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
				#if		qPeekForMoreCharsOnUserTyping
					UpdateIfNoKeysPending ();
				#endif
				return;
			}
			break;
	
			case kUpArrowCharCode: {
				DoSingleCharCursorEdit (eCursorToStart, eCursorByWindow, shiftPressed? eCursorExtendingSelection: eCursorMoving, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
				#if		qPeekForMoreCharsOnUserTyping
					UpdateIfNoKeysPending ();
				#endif
				return;
			}
			break;
	
			case kDownArrowCharCode: {
				DoSingleCharCursorEdit (eCursorToEnd, eCursorByWindow, shiftPressed? eCursorExtendingSelection: eCursorMoving, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
				#if		qPeekForMoreCharsOnUserTyping
					UpdateIfNoKeysPending ();
				#endif
				return;
			}
			break;
		}
    }
	CAbstractText::DoKeyDown (theChar, keyCode, macEvent);
}

void	Led_TCL_::TypeChar (char theChar, short theModifers)
{
	// Bob Swerdlow says you are supposed todo this - seems silly to me... LGP 950212...
	::ObscureCursor ();

	Led_Assert (GetSelectionEnd () <= GetLength () + qLedFirstIndex);

	/*
	 *	Behavior here as listed in Mac. Human Interface Guidelines - See Led SPR#0122
	 */
	bool	optionPressed	=	!!(theModifers&optionKey);
	bool	shiftPressed	=	!!(theModifers&shiftKey);
	bool	commandPressed	=	!!(theModifers&cmdKey);


	/*
	 *	Be sure codes for arrow keys don't conflict with second bytes. If so - we might
	 *	get confused on users typing dbcs characters! Check no conlfict with Led_BYTE too.
	 *	Any overlap could cause confusion.
	 */
	#if		qMultiByteCharacters
		Led_Assert (not Led_IsLeadByte (kLeftArrowCharCode));
		Led_Assert (not Led_IsValidSecondByte (kLeftArrowCharCode));

		Led_Assert (not Led_IsLeadByte (kRightArrowCharCode));
		Led_Assert (not Led_IsValidSecondByte (kRightArrowCharCode));

		Led_Assert (not Led_IsLeadByte (kUpArrowCharCode));
		Led_Assert (not Led_IsValidSecondByte (kUpArrowCharCode));

		Led_Assert (not Led_IsLeadByte (kDownArrowCharCode));
		Led_Assert (not Led_IsValidSecondByte (kDownArrowCharCode));
	#endif

	switch (theChar) {
		case kLeftArrowCharCode: {
			CursorMovementDirection	dir		=	commandPressed? eCursorToStart: eCursorBack;
			CursorMovementUnit		unit	=	optionPressed? eCursorByWord: eCursorByChar;
			if (commandPressed) {
				unit = eCursorByRow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case kRightArrowCharCode: {
			CursorMovementDirection	dir		=	commandPressed? eCursorToEnd: eCursorForward;
			CursorMovementUnit		unit	=	optionPressed? eCursorByWord: eCursorByChar;
			if (commandPressed) {
				unit = eCursorByRow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case kUpArrowCharCode: {
			CursorMovementDirection	dir		=	(commandPressed or optionPressed)? eCursorToStart: eCursorBack;
			CursorMovementUnit		unit	=	optionPressed? eCursorByLine: eCursorByRow;
			if (commandPressed) {
				unit = eCursorByWindow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case kDownArrowCharCode: {
			CursorMovementDirection	dir		=	(commandPressed or optionPressed)? eCursorToEnd: eCursorForward;
			CursorMovementUnit		unit	=	optionPressed? eCursorByLine: eCursorByRow;
			if (commandPressed) {
				unit = eCursorByWindow;	// NB: ambiguity in case both optionPressed AND commandPressed
			}
			CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;

			DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate);
			#if		qPeekForMoreCharsOnUserTyping
				UpdateIfNoKeysPending ();
			#endif
		}
		break;

		case	'\b':
		default: {
			if (theChar == '\r') {
				theChar = '\n';
			}
			OnTypedNormalCharacter (theChar);
			return;
		}
		break;
	}
}

void	Led_TCL_::PerformEditCommand (long theCommand)
{
	switch (theCommand) {
		case cmdCut: {
			OnCutCommand ();
		}
		break;

		case cmdCopy: {
			OnCopyCommand ();
		}
		break;

		case cmdPaste: {
			OnPasteCommand ();
		}
		break;

		case cmdClear: {
			OnClearCommand ();
		}
		break;

		default: {
			Led_Assert (false);	// mostlt for debugging
							// probably Ok if there are any others we don't
							// handle - but I'd like to find out if they exist...
		}
		break;
	}
}

void	Led_TCL_::SetSelection (long selStart, long selEnd, Boolean fRedraw)
{
	SetSelection (size_t (selStart+qLedFirstIndex), size_t (selEnd+qLedFirstIndex), fRedraw? eDefaultUpdate: eNoUpdate);
//	Led_Assert (false);	// NotReached- handle thunking in mixin
}

void	Led_TCL_::SetSelection (size_t start, size_t end, UpdateMode updateMode)
{
	TextInteractor_::SetSelection (start, end, updateMode);
}

void	Led_TCL_::NotificationOf_SelectionChanged ()
{
	/*
	 *	Simply call TCL hook function.
	 */
	SelectionChanged ();
}

void	Led_TCL_::UpdateScrollBars ()
{
	#if		qLed_UseCharOffsetsForTCLScrollAPI
		long  	newHeight	=	 GetLength ();
	#else
		long	newHeight	=	GetHeight (0, MAXINT);
	#endif

	// Syncronize OUR and TCL's data structures better!!!
	lineWidth = GetLayoutWidth ();

	LongRect	newBounds	=	AsLongRect (Led_Rect (0, 0, newHeight, lineWidth));
	SetBounds (&newBounds);

	SetTCLPanoramPositionFromLedPosition ();
	if (itsScrollPane != NULL) {
		itsScrollPane->AdjustScrollMax ();
		itsScrollPane->Calibrate ();
	}
}

Boolean		Led_TCL_::HitSamePart (Point pointA, Point pointB)
{
	// So TCL knows what to consider a double-click...
	LongPt	frameA;
	LongPt	frameB;
	WindToFrame (pointA, &frameA);
	WindToFrame (pointB, &frameB);

	return (GetCharAtClickLocation (AsLedPoint (frameA)) == GetCharAtClickLocation (AsLedPoint (frameB)));
}

void	Led_TCL_::GetSelection (long* selStart, long* selEnd)
{
	Led_AssertNotNil (selStart);
	Led_AssertNotNil (selEnd);
	TextInteractor_*	ourLed	=	this;
	size_t	start	=	0;
	size_t	end		=	0;
	ourLed->GetSelection (&start, &end);
	*selStart = start-qLedFirstIndex;
	*selEnd = end-qLedFirstIndex;
}

void	Led_TCL_::HideSelection (Boolean hide, Boolean redraw)
{
	SetSelectionShown (not hide, redraw? eDefaultUpdate: eNoUpdate);
}

void	Led_TCL_::Draw (Rect* area)
{
	Led_Assert (macPort == qd.thePort);	// we should be pre-prepared...

	#if		qSupportEnterIdleCallback
		CallEnterIdleCallback ();
	#endif

	TextImager_*	imager	=	this;
	imager->Draw (AsLedRect (*area), false);
	if (GetCaretShown ()) {
		DrawCaret_ ();
	}
}

void	Led_TCL_::SetBounds (LongRect* aBounds)
{
	Led_AssertNotNil (aBounds);

	if (position.v > aBounds->bottom) {		// just to avoid silly TCL assert...
		position.v = aBounds->bottom;
	}
	CAbstractText::SetBounds (aBounds);

	Led_Assert (aBounds->right > aBounds->left);
	SetLayoutWidth (aBounds->right - aBounds->left);
}

void	Led_TCL_::Refresh_ (UpdateMode updateMode) const
{
	RefreshWindowRect_ (AsLedRect (aperture), updateMode);
}

void	Led_TCL_::Refresh_ (const Led_Rect& area, UpdateMode updateMode) const
{
	// Use of this is not encouraged since it requires (at least in some cases)
	// wordwrapping text...
	Led_Assert (false);					// NYI must convert to window coords!!!
	RefreshWindowRect_ (area, updateMode);
}

void	Led_TCL_::RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const
{
	updateMode = RealUpdateMode (updateMode);
	switch (updateMode) {
		case	eDelayedUpdate: {
			if (not windowRectArea.IsEmpty ()) {
				LongRect	tmp	=	AsLongRect (windowRectArea);
				const_cast<Led_TCL_*> (this)->RefreshLongRect (&tmp);
			}
		}
		break;
		case	eImmediateUpdate: {
			if (not windowRectArea.IsEmpty ()) {
				Rect	tempQDRect;
				LongRect	tmpArea	=	AsLongRect (windowRectArea);
				const_cast<Led_TCL_*> (this)->FrameToWindR (&tmpArea, &tempQDRect);
				const_cast<Led_TCL_*> (this)->DrawAll (&tempQDRect);
				::ValidRect (&tempQDRect);
			}
		}
		break;
	}
}

void	Led_TCL_::Update_ () const
{
	UpdateWindowRect_ (GetWindowRect ());
}

void	Led_TCL_::Update_ (const Led_Rect& area) const
{
	// Use of this is not encouraged since it requires (at least in some cases)
	// wordwrapping text...
	Led_Assert (false);	// NYI must convert to window coords!!!
}

void	Led_TCL_::UpdateWindowRect_ (const Led_Rect& windowRectArea) const
{
	if (not windowRectArea.IsEmpty ()) {
		/*
		 *	This routine could dearly use some review. I'm not at all sure I have
		 *	my TCL coordinate hacking correct. It SEEMS to work for all the cases I've
		 *	tried so far, but I am concerned there may still be glitches.
		 */
	
		// Note that the updateRgn is in GlobalCoordinates!!!
		CWindow*	w	=	const_cast<Led_TCL_*> (this)->GetWindow ();
		Led_AssertNotNil (w);
		Led_AssertNotNil (w->macPort);
		Led_AssertNotNil (WindowPeek (w->macPort)->updateRgn);
		Rect	updateArea	=	(**(WindowPeek (w->macPort)->updateRgn)).rgnBBox;
		{
			Point	updateOrigin	=	::GetRectOrigin (updateArea);
			Point	updateSize		=	::GetRectSize (updateArea);
			const_cast<Led_TCL_*> (this)->Prepare ();
			::GlobalToLocal (&updateOrigin);
			updateArea	=	AsQDRect (Led_Rect (AsLedPoint (updateOrigin), AsLedSize (updateSize)));
		}
	
	
		/*
		 *	I really deeply dislike the way TCL deals with coordinates. It is unnecessarily
		 *	baroque. So sad. Anyhow - I hope this is right... LGP 941022
		 */
		Rect	qdWindowRectArea;
		LongRect	workaroundTCLConstSlopiness	=	AsLongRect (windowRectArea);
		const_cast<Led_TCL_*> (this)->FrameToWindR (&workaroundTCLConstSlopiness, &qdWindowRectArea);
		Rect	reallyUpdateRect;
		OffsetRect(&updateArea, -hOrigin, -vOrigin);
	
		if (::SectRect (&qdWindowRectArea, &updateArea, &reallyUpdateRect)) {
			const_cast<Led_TCL_*> (this)->DrawAll (&reallyUpdateRect);
			const_cast<Led_TCL_*> (this)->Prepare ();
	
			OffsetRect (&reallyUpdateRect, hOrigin, vOrigin);		// not sure why this is needed
																	// nor if it is always right,
																	// but otherwise we are slightly
																	// off in what we validate, and as
																	// a result - the below assert
																	// about EmptyRect of updateRgn
																	// fails...
			::ValidRect (&reallyUpdateRect);
	
	#if 0
	// LGP 941204- sometimes happens from within LVEJ-Mac, and not worth worrying about since the comment
	// says assert not really right...
	
			// tmp hack for debugging...
			// Not really gauranteed to be true - but should be most of the time.
			// If not - maybe investigate, maybe ignore... Feel free to
			// comemnt this out....
			Rect	xxx	=	(**(WindowPeek (w->macPort)->updateRgn)).rgnBBox;
			Led_Assert (EmptyRect (&xxx));
	#endif
		}
	}
}

void	Led_TCL_::OnCutCommand ()
{
	TextInteractor_::OnCutCommand ();
	Led_AssertNotNil (gClipboard);
	gClipboard->UpdateDisplay ();
}

void	Led_TCL_::OnCopyCommand_After ()
{
	TextInteractor_::OnCopyCommand_After ();
	Led_AssertNotNil (gClipboard);
	gClipboard->UpdateDisplay ();
}

void	Led_TCL_::Dawdle (long* maxSleep)
{
	Led_AssertNotNil (maxSleep);

	#if		qSupportEnterIdleCallback
		CallEnterIdleCallback ();	// not good enuf if we aren't the focus!!! - LGP 960915
	#endif

	BlinkCaretIfNeeded ();
	*maxSleep = GetTickCountBetweenBlinks ();
}

void	Led_TCL_::Activate ()
{
	CAbstractText::Activate ();
	SetCaretShown (GetWantsClicks ());
	SetSelectionShown (true);
}

void	Led_TCL_::Deactivate ()
{
	CAbstractText::Deactivate ();
	SetCaretShown (false);
	SetSelectionShown (false);
}

void	Led_TCL_::SetWantsClicks (Boolean aWantsClicks)
{
	CAbstractText::SetWantsClicks (aWantsClicks);
	SetCaretShown (GetWantsClicks () and IsActive ());
}

void	Led_TCL_::DoClick (Point hitPt, short modifierKeys, long /*when*/)
{
	if (GetCommandHandler () != NULL) {
		GetCommandHandler ()->BreakInGroupedCommands ();
	}

	bool	extendSelection	=	bool (modifierKeys & shiftKey);

	if (not ProcessSimpleClick (AsLedPoint (hitPt), gClicks, extendSelection)) {
		return;
	}

	switch (gClicks) {
		case	1: {
			size_t	newPos			=	GetCharAtClickLocation (AsLedPoint (hitPt));

#if		1
// useing new ProcessSimpleClick() code...
				size_t	newSelStart		=	GetSelectionStart ();
				size_t	newSelEnd		=	GetSelectionEnd ();
				
#else

			size_t	newSelStart		=	newPos;
			size_t	newSelEnd		=	newPos;
			if (extendSelection) {
				newSelStart = Min (newSelStart, GetSelectionStart ());
				newSelEnd = Max (newSelEnd, GetSelectionEnd ());
			}

			// Set flag for how to display caret based on where we clicked
			if (not extendSelection) {
				SetCaretShownAfterPos (GetCharWindowLocation (newPos).top <= hitPt.v);
			}


			SetSelection (newSelStart, newSelEnd);
#endif

			/*
			 *	Note that we do this just AFTER setting the first selection. This is to avoid
			 *	(if we had done it earlier) displaying the OLD selection and then quickly
			 *	erasing it (flicker). We do an Update () rather than simply use eImmediateUpdate
			 *	mode on the SetSelection () because at this point we want to redisplay the entire
			 *	window if part of it needed it beyond just the part within the selection.
			 */
			Update ();

			fDragAnchor = (newPos > newSelStart)? newSelStart: newSelEnd;

			while (::StillDown ()) {
				Point	newPt;
				Prepare ();
				::GetMouse (&newPt);
				size_t	rhsPos	=	GetCharAtClickLocation (AsLedPoint (newPt));

				/*
 				 *	Handle autoscrolling, if necessary.
				 */
				if (rhsPos < GetMarkerPositionOfStartOfWindow ()) {
					ScrollByIfRoom (-1);
					rhsPos = GetMarkerPositionOfStartOfWindow ();
				}
				else if (rhsPos > GetMarkerPositionOfEndOfWindow ()) {
					ScrollByIfRoom (1);
					rhsPos = FindNextCharacter (GetMarkerPositionOfEndOfWindow ());		// So we select past to end of window
				}

				/*
				 *	Update the selection.
				 */
				if (rhsPos > fDragAnchor) {
					SetSelection (fDragAnchor, rhsPos, eImmediateUpdate);
				}
				else {
					SetSelection (rhsPos, fDragAnchor, eImmediateUpdate);
				}
			}
		}
		break;

		case	2: {
			size_t	wordStart	=	0;
			size_t	wordEnd		=	0;
			bool	wordReal	=	false;
			GetTextStore ().FindWordBreaks (fDragAnchor, &wordStart, &wordEnd, &wordReal);
			fDragAnchor = Min (fDragAnchor, wordStart);
			#if		qDoubleClickSelectsSpaceAfterWord
				if (wordReal) {
					// select the space forward...
					size_t	xWordStart	=	0;
					size_t	xWordEnd	=	0;
					bool	xWordReal	=	false;
					GetTextStore ().FindWordBreaks (wordEnd, &xWordStart, &xWordEnd, &xWordReal);
					if (not xWordReal) {
						wordEnd = xWordEnd;
					}
				}
				else {
					// select the WORD behind...
					wordStart = GetTextStore ().FindFirstWordStartStrictlyBeforePosition (wordStart);
				}
			#endif
			SetSelection (wordStart, wordEnd, eImmediateUpdate);
			while (::StillDown ()) {
				Point	newPt;
				Prepare ();
				::GetMouse (&newPt);
				size_t	rhsPos	=	GetCharAtClickLocation (AsLedPoint (newPt));
		
				/*
 				 *	Handle autoscrolling, if necessary.
				 */
				if (rhsPos < GetMarkerPositionOfStartOfWindow ()) {
					ScrollByIfRoom (-1);
					rhsPos = GetMarkerPositionOfStartOfWindow ();
				}
				else if (rhsPos > GetMarkerPositionOfEndOfWindow ()) {
					ScrollByIfRoom (1);
					rhsPos = FindNextCharacter (GetMarkerPositionOfEndOfWindow ());		// So we select past to end of window
				}

				/*
				 *	Update the selection.
				 */
				if (rhsPos > fDragAnchor) {
					#if		qDoubleClickSelectsSpaceAfterWord	
						SetSelection (wordStart, GetTextStore ().FindFirstWordStartAfterPosition (rhsPos), eImmediateUpdate);
					#else
						SetSelection (fDragAnchor, GetTextStore ().FindFirstWordEndAfterPosition (rhsPos, false), eImmediateUpdate);
					#endif
				}
				else {
					SetSelection (GetTextStore ().FindFirstWordStartStrictlyBeforePosition (rhsPos, qDoubleClickSelectsSpaceAfterWord), wordEnd, eImmediateUpdate);
				}
			}
		}
		break;

		case	3: {
			SelectWholeLineAfter (GetSelectionStart (), false, eImmediateUpdate);
		}
		break;
	}


	// Borrowed more or less verbatim from the end of TCL CEditText::DoClick()
	{
		// CSwitchboard will never see the mouse up that ended
		// the drag, so we stuff gLastMouseUp here to allow
		// multi-click counting to work.
		gLastMouseUp.what = mouseUp;
		gLastMouseUp.when = ::TickCount ();
		gLastMouseUp.where = hitPt;
		::LocalToGlobal(&gLastMouseUp.where);
		gLastMouseUp.modifiers = modifierKeys;

		SelectionChanged ();

		//
		// For non-editable but selectable text, we want to display
		// a selection range so the user can copy, but we don't
		// want to display a caret. In that case, we resign the
		// gopher to our supervisor, which will deactivate this text
		if (!editable && (gGopher == this)) {
			long selStart, selEnd;
			GetSelection(&selStart, &selEnd);
			if (selStart == selEnd) {
				itsSupervisor->BecomeGopher (TRUE);
			}
		}
	}
}

void	Led_TCL_::ResizeFrame (Rect* delta)
{
	// This call to CAbstractText::ResizeFrame(delta); does lots of calcs I'm not
	// too sure about - reconsider... LGP 941020
	CAbstractText::ResizeFrame (delta);

	Led_Rect	winRect	=	AsLedRect (frame);
	SetWindowRect (winRect);


// ONLY TRUE FOR WORDWRAP MODE...
	Led_Assert (::GetRectWidth (winRect) > 0);
	SetLayoutWidth (::GetRectWidth (winRect));
}

Led_Tablet	Led_TCL_::AcquireTablet () const
{
#if		qDebug
	const_cast<Led_TCL_*> (this)->fAcquireCount++;
	Led_Assert (fAcquireCount < 100);	// not really a requirement - but hard to see how this could happen in LEGIT usage...
										// almost certainly a bug...
#endif

CView::cPreparedView = NULL;
	const_cast<Led_TCL_*> (this)->Prepare ();
	Led_Assert (qd.thePort == macPort);
	return (macPort);
}

void	Led_TCL_::ReleaseTablet (Led_Tablet tablet) const
{
	Led_Assert (fAcquireCount > 0);
#if		qDebug
	const_cast<Led_TCL_*> (this)->fAcquireCount--;
#endif
	Led_Assert (tablet == macPort);
}

void	Led_TCL_::Scroll (long hDelta, long vDelta, Boolean redraw)
{
	if (hDelta != 0 or vDelta != 0) {
		// Since we require that the vScrollPos be a valid character number
		// starting a row - we must fudge the vDelta on entry to make sure it
		// is OK. This is because some routines like CScrollPane::DoThumbDrag()
		// might give us bad values...
	
	
		long	hPixels	=	hDelta * hScale;
		// Not sure this is how I want to handle things in the long run. But since
		// LEC doesn't need horizontal scrolling - we'll leave it at this...
		if (hPixels != 0) {
			OffsetLongRect (&frame, hPixels, 0);
			OffsetLongRect (&aperture, hPixels, 0);
		}
	
		position.h += hDelta;
		hOrigin += hPixels;
	
		ForceNextPrepare ();

		#if		qLed_UseCharOffsetsForTCLScrollAPI
			long	newPos	=	long (GetMarkerPositionOfStartOfWindow ()-qLedFirstIndex) + vDelta;
			newPos = Max (newPos, 0);
			newPos = Min (newPos, GetLength ());
			SetTopRowInWindowByMarkerPosition (newPos + qLedFirstIndex, redraw? eImmediateUpdate: eDefaultUpdate);
		#else
			long	newTopRowInWindow = GetTopRowInWindow () + vDelta;
			newTopRowInWindow = Max (newTopRowInWindow, qLedFirstIndex);
			Led_Assert (GetRowCount () > 0);
			newTopRowInWindow = Min (newTopRowInWindow, (GetRowCount ()-1+qLedFirstIndex));
			SetTopRowInWindow (newTopRowInWindow);
		#endif

		// Not sure this is right - sterling added... Test later... LGP 941104
		// Hard to test now since my only (currently working) imager does wordwrap...
		if ((hPixels != 0) and (vDelta == 0)) {
			Refresh (redraw? eImmediateUpdate: eDefaultUpdate);
		}
	}
}

void	Led_TCL_::SetTCLPanoramPositionFromLedPosition ()
{
	#if		qLed_UseCharOffsetsForTCLScrollAPI
		position.v = GetMarkerPositionOfStartOfWindow () - 1;
	#else
		size_t	firstCharOfTopOfWindow		=	GetStartOfRow (GetTopRowInWindow ());
		position.v = GetCharLocation (firstCharOfTopOfWindow).top;
	#endif
}

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::DoHorizScroll (short whichPart)
{
	long				ticks;			// Tick count at end of Delay
	short				theHStep	=	1;	// perhaps later change to use the ave char width?

	register long	delta	=	0;			// Number of pixels to scroll
	switch (whichPart) {
		case inUpButton:
			delta = -theHStep;
			break;

		case inDownButton:
			delta = theHStep;
			break;

		case inPageUp:
			Delay(PAGE_DELAY, &ticks);
			delta = Min(itsScrollPane->hOverlap - itsScrollPane->hSpan, -1);
			break;

		case inPageDown:
			Delay(PAGE_DELAY, &ticks);
			delta = Max(itsScrollPane->hSpan - itsScrollPane->hOverlap, 1);
			break;
	}

	LongPt	currPos;
	GetFramePosition(&currPos.h, &currPos.v);
	register long	minmax;			// Minimum or Maximum delta
	if (delta < 0) {
		minmax = -currPos.h;
		delta = Max(delta, minmax);
	}
	else {
		minmax = itsScrollPane->hExtent - currPos.h - (long) itsScrollPane->hSpan;
		if (minmax < 0)									// TCL 1.1.3 11/30/92 BF
			minmax = 0;
		delta = Min(delta, minmax);
	}
	if (delta != 0) {
		Scroll (delta, 0, true);
		itsScrollPane->AdjustScrollMax ();
		itsScrollPane->Calibrate ();
		itsScrollPane->itsHorizSBar->Prepare ();
	}
}
#endif

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::DoVertScroll (short whichPart)
{
	Led_AssertNotNil (itsScrollPane);

	if (whichPart == inUpButton or whichPart == inDownButton or
		whichPart == inPageUp or whichPart == inPageDown) {
		if (not DelaySomeForScrollBarClick ()) {
			return;
		}
	}

	switch (whichPart) {
		case inUpButton:
			ScrollByIfRoom (-1, eImmediateUpdate);
			break;

		case inDownButton:
			ScrollByIfRoom (1, eImmediateUpdate);
			break;

		case inPageUp:
			ScrollByIfRoom (- long (GetTotalRowsInWindow ()), eImmediateUpdate);
			break;

		case inPageDown:
			ScrollByIfRoom (GetTotalRowsInWindow (), eImmediateUpdate);
			break;
	}

	itsScrollPane->itsVertSBar->Prepare ();				// In case we changed the port/origin/settings... SBAR expects things unchanged...
}
#endif

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::DoThumbDrag (short newHPos, short newVPos)
{
	Led_AssertNotNil (itsScrollPane);
	Led_AssertNotNil (itsScrollPane->itsVertSBar);
	long	newPos	=	long (newVPos) * itsScrollPane->vUnit;

	newPos = Max (newPos, 0);
	newPos = Min (newPos, GetLength ());

	/*
	 *	Be careful of special case where vUnit > 1 and so we get truncation of values,
	 *	and may not be able to get a value of the end of buffer. So we check for that
	 *	special case and correct.
	 */
	if (newVPos == itsScrollPane->itsVertSBar->GetMaxValue ()) {
		newPos = GetLength ();
	}

	(void)SetTopRowInWindowByMarkerPosition (newPos + qLedFirstIndex);

	itsScrollPane->itsVertSBar->Prepare ();				// In case we changed the port/origin/settings... SBAR expects things unchanged...
}
#endif

void	Led_TCL_::ScrollToSelection ()
{
	TextInteractor_::ScrollToSelection ();
}

void	Led_TCL_::ScrollToSelection (UpdateMode updateMode)
{
	TextInteractor_::ScrollToSelection (updateMode);
}

void	Led_TCL_::GetSteps (short* hStep, short* vStep)
{
#if		qLed_UseCharOffsetsForTCLScrollAPI
	*hStep = 0;				// avoid compiler warnings
	*vStep = 0;				// ''
	Led_Assert (false);		// DONT USE THIS API - override of BAD TCL API
#else
	short	phStep;
	short	pvStep;

	CPanorama::GetSteps (&phStep, &pvStep);

	// Don't scroll by one pixel!
	if (phStep == 1 && pvStep == 1 && hScale == 1 && vScale == 1) {
		Prepare ();
		Led_IncrementalFontSpecification	fontSpec	=	GetDefaultFont ();
		::TextFont (fontSpec.fFontID);
		::TextSize (fontSpec.fFontSize);
		::TextFace (fontSpec.fFontStyle);
		FontInfo	fontInfo;
		::GetFontInfo (&fontInfo);
		*hStep = fontInfo.widMax << 1;
		*vStep = fontInfo.ascent + fontInfo.descent + fontInfo.leading;
	}
	else {
		*hStep = phStep;
		*vStep = pvStep;
	}
#endif
}

bool	Led_TCL_::QueryInputKeyStrokesPending () const
{
	EventRecord	eventRecord;
	return (::OSEventAvail (keyDownMask, &eventRecord));
}

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::GetExtent (long* theHExtent, long* theVExtent)
{
	*theHExtent = bounds.right - bounds.left;
	*theVExtent = GetLength ();
}
#endif

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::GetFrameSpan (short* theHSpan, short* theVSpan)
{
	*theHSpan = (frame.right - frame.left) / hScale;

	*theVSpan = GetMarkerPositionOfEndOfWindow () - GetMarkerPositionOfStartOfWindow ();
}
#endif

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::SetScales (short /*aHScale*/, short /*aVScale*/)
{
	Led_Assert (false);		// better not use this!!!
}
#endif

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::GetScales (short* /*theHScale*/, short* /*theVScale*/)
{
	Led_Assert (false);		// better not use this!!!
}
#endif

#if		qLed_UseCharOffsetsForTCLScrollAPI
void	Led_TCL_::GetPixelExtent (long* hExtent, long* vExtent)
{
	// Shoud FIX THIS!!!
	Led_Assert (false);	// til somebody fixes this - is it called/???
	*hExtent = (bounds.right - bounds.left) * hScale;
	*vExtent = (bounds.bottom - bounds.top) * vScale;
}
#endif

void	Led_TCL_::GetCharBefore (long* aPosition, tCharBuf charBuf)
{
	/*
	 *	This implemenation is based on CAbstractText::GetCharBefore () - and was
	 *	modified mainly because the TCL 2.0.3 implemenation called GetTextHandle()
	 *	which at one time caused a memory leak. But even after that is fixed is needlessly
	 *	slow and sloppy.
	 *
	 *	Also - this routine's API appeared to handle the case where *aPosition split
	 *	a muti-byte character. We currently do not support that case (really no reason for
	 *	anyone to ever do it. We simply assume/assert that it doesn't happen.
	 */

	Led_AssertNotNil (aPosition);
	size_t	beforePos	=	*aPosition + qLedFirstIndex;
	Led_Assert (beforePos >= qLedFirstIndex);
	Led_Assert (beforePos <= GetLength () + qLedFirstIndex);
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (beforePos);
#endif

	size_t	prevCharStart	=	FindPreviousCharacter (beforePos);
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (prevCharStart);
#endif
	Led_Assert (beforePos >= prevCharStart);
	size_t	len				=	beforePos - prevCharStart;
	charBuf[0] = len;
	CopyOut (prevCharStart, len, (Led_tChar*)&charBuf[1]);
	*aPosition = prevCharStart - qLedFirstIndex;
}

void	Led_TCL_::GetCharAfter (long* aPosition, tCharBuf charBuf)
{
	/*
	 *	This implemenation is based on CAbstractText::GetCharAfter () - and was
	 *	modified mainly because the TCL 2.0.3 implemenation called GetTextHandle()
	 *	which at one time caused a memory leak. But even after that is fixed is needlessly
	 *	slow and sloppy.
	 *
	 *	Also - this routine's API appeared to handle the case where *aPosition split
	 *	a muti-byte character. We currently do not support that case (really no reason for
	 *	anyone to ever do it. We simply assume/assert that it doesn't happen.
	 */

	Led_AssertNotNil (aPosition);
	size_t	afterPos	=	*aPosition + qLedFirstIndex;
	Led_Assert (afterPos >= qLedFirstIndex);
	Led_Assert (afterPos <= GetLength () + qLedFirstIndex);
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (afterPos);
#endif

	size_t	nextCharStart	=	FindNextCharacter (afterPos);
#if		qMultiByteCharacters
	Assert_CharPosDoesNotSplitCharacter (nextCharStart);
#endif
	Led_Assert (afterPos <= nextCharStart);
	size_t	len				=	nextCharStart - afterPos;
	charBuf[0] = len;
	CopyOut (afterPos, len, (Led_tChar*)&charBuf[1]);
	*aPosition = nextCharStart - qLedFirstIndex;
}

void	Led_TCL_::BlinkCaretIfNeeded ()
{
	if (GetCaretShown ()) {
		long	now	=	::TickCount ();
		if (now > fTickCountAtLastBlink + GetTickCountBetweenBlinks ()) {
//			DrawCaret (not fLastDrawnOn);

Led_Rect	cr	=	CalculateCaretRect ();
RefreshWindowRect (cr);

fLastDrawnOn = not fLastDrawnOn;
			fTickCountAtLastBlink = now;
		}
	}
}

void	Led_TCL_::DrawCaret_ ()
{
	DrawCaret_ (fLastDrawnOn);
}

void	Led_TCL_::DrawCaret_ (bool on)
{
	Tablet_Acquirer	tablet (this);
	Led_AssertNotNil (tablet);
	Led_Assert (tablet == qd.thePort);
	// Note: the reason we don't just use the GetCaretShown () flag here, and
	// instead we check that the selection is empty is because we want to reserve that
	// flag for users (client programmers) use. If we used it internally (like in SetSelection()
	// then the users values would get overwritten...
	Led_Assert (GetCaretShown ());
	if (GetSelectionStart () == GetSelectionEnd ()) {
		Led_Rect	caretRect	=	CalculateCaretRect ();
		fLastDrawnOn = on;
		Rect	qdCaretRect	=	AsQDRect (caretRect);
		if (on) {
			::FillRect (&qdCaretRect, &qd.black);
		}
		else {
			::EraseRect (&qdCaretRect);
		}
	}
}












/*
 ********************************************************************************
 ********************************* SimpleLed_TCL ********************************
 ********************************************************************************
 */
SimpleLed_TCL::SimpleLed_TCL (CView *anEnclosure, CBureaucrat *aSupervisor,
								short aWidth, short aHeight,
								short aHEncl, short aVEncl,
								SizingOption aHSizing,
								SizingOption aVSizing,
								short aLineWidth, Boolean aScrollHoriz
								):
	TextImager_ (),
	Led_TCL_ (anEnclosure, aSupervisor, aWidth, aHeight, aHEncl, aVEncl, aHSizing, aVSizing, aLineWidth, aScrollHoriz),
	WordWrappedTextInteractor ()
{
}

void	SimpleLed_TCL::SetSelection (long selStart, long selEnd, Boolean fRedraw)
{
	SetSelection (size_t (selStart+qLedFirstIndex), size_t (selEnd+qLedFirstIndex), fRedraw? eDefaultUpdate: eNoUpdate);
}

void	SimpleLed_TCL::SetSelection (size_t start, size_t end, UpdateMode updateMode)
{
	WordWrappedTextInteractor::SetSelection (start, end, updateMode);
}










/*
 ********************************************************************************
 *************************** StandardStyledWordWrappedLed_TCL *******************
 ********************************************************************************
 */
StandardStyledWordWrappedLed_TCL::StandardStyledWordWrappedLed_TCL (CView *anEnclosure, CBureaucrat *aSupervisor,
								short aWidth, short aHeight,
								short aHEncl, short aVEncl,
								SizingOption aHSizing,
								SizingOption aVSizing,
								short aLineWidth, Boolean aScrollHoriz
								):
	TextImager_ (),
	Led_TCL_ (anEnclosure, aSupervisor, aWidth, aHeight, aHEncl, aVEncl, aHSizing, aVSizing, aLineWidth, aScrollHoriz),
	StandardStyledWordWrappedTextInteractor (NULL)
{
}

void	StandardStyledWordWrappedLed_TCL::SetSelection (long selStart, long selEnd, Boolean fRedraw)
{
	SetSelection (size_t (selStart+qLedFirstIndex), size_t (selEnd+qLedFirstIndex), fRedraw? eDefaultUpdate: eNoUpdate);
}

void	StandardStyledWordWrappedLed_TCL::SetSelection (size_t start, size_t end, UpdateMode updateMode)
{
	StandardStyledWordWrappedTextInteractor::SetSelection (start, end, updateMode);
}

void	StandardStyledWordWrappedLed_TCL::GetTextStyle (short* whichAttributes, TextStyle* aStyle)
{
	Led_Assert (GetSelectionEnd () >= GetSelectionStart ());
	size_t	selectionLength	=	GetSelectionEnd ()-GetSelectionStart ();
	DoContinuousStyle_Mac (GetSelectionStart (), selectionLength, whichAttributes, aStyle);
}

void	StandardStyledWordWrappedLed_TCL::UpdateMenus ()
{
	Led_TCL_::UpdateMenus ();

	if (editable and ShouldEnablePasteCommand ()) {
		gBartender->EnableCmd (cmdPaste);
	}
}












#if		qLed_UseCharOffsetsForTCLScrollAPI
/*
 ********************************************************************************
 *********************************** Led_ScrollPane *****************************
 ********************************************************************************
 */


Led_ScrollPane::Led_ScrollPane (CView *anEnclosure, CBureaucrat *aSupervisor,
					short aWidth, short aHeight,
					short aHEncl, short aVEncl,
					SizingOption aHSizing,
					SizingOption aVSizing,
					Boolean hasHoriz, Boolean hasVert, Boolean hasSizeBox):
	CScrollPane (anEnclosure, aSupervisor, aWidth, aHeight, aHEncl, aVEncl, aHSizing, aVSizing, hasHoriz, hasVert, hasSizeBox)
{
	UseLongCoordinates (true);
}

void	Led_ScrollPane::DoHorizScroll (short whichPart)
{
	Led_TCL_*	editor	=	(Led_TCL_*)itsPanorama;
	Led_Assert (member (editor, Led_TCL_));
	editor->DoHorizScroll (whichPart);
}

void	Led_ScrollPane::DoVertScroll (short	whichPart)
{
	Led_TCL_*	editor	=	(Led_TCL_*)itsPanorama;
	Led_Assert (member (editor, Led_TCL_));
	editor->DoVertScroll (whichPart);
}

void	Led_ScrollPane::DoThumbDrag (short hDelta, short /*vDelta*/)
{
	Led_TCL_*	editor	=	(Led_TCL_*)itsPanorama;
	Led_Assert (member (editor, Led_TCL_));
	Led_AssertNotNil (itsVertSBar);
	editor->DoThumbDrag (hDelta, itsVertSBar->GetValue ());
}
#endif



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
