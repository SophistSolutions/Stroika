/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/KeyBoard.cc,v 1.3 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: KeyBoard.cc,v $
 *		Revision 1.3  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.16  1992/04/20  14:24:17  lewis
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *
 *		Revision 1.15  92/04/15  13:44:10  13:44:10  sterling (Sterling Wight)
 *		fixed a few key constants for the Mac (arrow keys)
 *		
 *		Revision 1.14  92/03/16  16:06:22  16:06:22  sterling (Sterling Wight)
 *		defined keycodes for Mac
 *		
 *		Revision 1.13  1992/03/11  08:13:53  lewis
 *		Declare different names static member keystrokes explicity - had forgotten scope resoultion - not
 *		sure why it ever linked? Also, implemented stream inserter.
 *
 *		Revision 1.12  1992/03/10  13:10:45  lewis
 *		Added kDelete to KeyStrokes, implemented KeyComposeState::ReferenceCompose ()
 *
 *		Revision 1.11  1992/03/10  00:06:09  lewis
 *		Did implementation of KeyStroke, and related classes. Incomplete.
 *
 *		Revision 1.7  1992/01/03  21:34:15  lewis
 *		Use same stuff for XGDI as for qMacGDI.
 *
 *
 */



#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Events.h>
#elif	qXToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/keysym.h>
#include	<X11/Xlib.h>
#include	<X11/Xutil.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"


#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"KeyBoard.hh"






/*
 ********************************************************************************
 ********************************* KeyComposeState ******************************
 ********************************************************************************
 */

KeyComposeState::KeyComposeState ()
{
}

#if		qXToolkit
_XComposeStatus&	KeyComposeState::ReferenceCompose ()
{
	Assert (sizeof (_XComposeStatus) == sizeof (fCompose));
	return (*(_XComposeStatus*)(char*)fCompose);
}
#endif




/*
 ********************************************************************************
 *************************************** KeyStroke ******************************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
	Implement (Iterator, KeyStrokeModifiers);
	Implement (Collection, KeyStrokeModifiers);
	Implement (AbSet, KeyStrokeModifiers);
	//Implement (Set_BitString, KeyStrokeModifiers);
	Set_BitStringImplement1 (KeyStrokeModifiers);
	Set_BitStringImplement2 (KeyStrokeModifiers);
#endif


// MUST FIX THESE!!!
#if		qMacToolkit
const	KeyStroke	KeyStroke::kTab			=	KeyStroke ('\t');
const	KeyStroke	KeyStroke::kSpace		=	KeyStroke (' ');
const	KeyStroke	KeyStroke::kBackspace	=	KeyStroke (8);
const	KeyStroke	KeyStroke::kDelete		=	KeyStroke (127);	// forward delete
const	KeyStroke	KeyStroke::kClear		=	KeyStroke (27);		// note same as escape
const	KeyStroke	KeyStroke::kEnter		=	KeyStroke ('\n');		//???
const	KeyStroke	KeyStroke::kReturn		=	KeyStroke ('\r');		//???
const	KeyStroke	KeyStroke::kEscape		=	KeyStroke (27);
const	KeyStroke	KeyStroke::kHelp		=	KeyStroke (5);
const	KeyStroke	KeyStroke::kLeftArrow	=	KeyStroke (28);
const	KeyStroke	KeyStroke::kRightArrow	=	KeyStroke (29);
const	KeyStroke	KeyStroke::kUpArrow		=	KeyStroke (30);
const	KeyStroke	KeyStroke::kDownArrow	=	KeyStroke (31);
#elif	qXtToolkit
const	KeyStroke	KeyStroke::kTab			=	KeyStroke (XK_Tab);
const	KeyStroke	KeyStroke::kSpace		=	KeyStroke (XK_space);
const	KeyStroke	KeyStroke::kBackspace	=	KeyStroke (XK_BackSpace);
const	KeyStroke	KeyStroke::kDelete		=	KeyStroke (XK_Delete);
const	KeyStroke	KeyStroke::kClear		=	KeyStroke (XK_Clear);
const	KeyStroke	KeyStroke::kEnter		=	KeyStroke (XK_KP_Enter);	//???
const	KeyStroke	KeyStroke::kReturn		=	KeyStroke (XK_Return);		//???/OR ENTER>>>
const	KeyStroke	KeyStroke::kEscape		=	KeyStroke (XK_Escape);
const	KeyStroke	KeyStroke::kHelp		=	KeyStroke (XK_Help);
const	KeyStroke	KeyStroke::kLeftArrow	=	KeyStroke (XK_Left);
const	KeyStroke	KeyStroke::kRightArrow	=	KeyStroke (XK_Right);
const	KeyStroke	KeyStroke::kDownArrow	=	KeyStroke (XK_Down);
const	KeyStroke	KeyStroke::kUpArrow		=	KeyStroke (XK_Up);
#endif	/*Toolkit*/

KeyStroke::KeyStroke ():
	fCharacter (0),
	fModifers (Nil)
{
}

KeyStroke::KeyStroke (UInt16 character):
	fCharacter (character),
	fModifers (Nil)
{
}

KeyStroke::KeyStroke (const KeyStroke& from):
	fCharacter (from.GetCharacter ()),
	fModifers (Nil)
{
	if (from.fModifers != Nil) {
		SetModifiers (from.GetModifiers ());
	}
}

KeyStroke::~KeyStroke ()
{
	delete (fModifers);
}

const KeyStroke& KeyStroke::operator= (const KeyStroke& rhs)
{
	fCharacter = rhs.GetCharacter ();
	delete (fModifers); fModifers = Nil;
	if (rhs.fModifers != Nil) {
		SetModifiers (rhs.GetModifiers ());
	}
	return (*this);
}

UInt16	KeyStroke::GetCharacter () const
{
	return (fCharacter);
}

String	KeyStroke::GetPrintRep () const
{
	AssertNotImplemented ();
}

const	AbSet (KeyStrokeModifiers)&	KeyStroke::GetModifiers () const
{
	static	Set_BitString (KeyStrokeModifiers)	kNoModifiers;

	if (fModifers == Nil) {
		return (kNoModifiers);
	}
	else {
		EnsureNotNil (fModifers);
		return (*fModifers);
	}
}

void	KeyStroke::SetModifiers (const	AbSet (KeyStrokeModifiers)& modifiers)
{
	if (fModifers == Nil) {
		fModifers = new Set_BitString (KeyStrokeModifiers) (modifiers);
	}
}






/*
 ********************************************************************************
 ******************************* Boolean comparisons ****************************
 ********************************************************************************
 */
Boolean	operator==	(const KeyStroke& lhs, const KeyStroke& rhs)
{
	return Boolean ((lhs.GetCharacter () == rhs.GetCharacter ()) and (lhs.GetModifiers () == rhs.GetModifiers ()));
}

Boolean	operator!=	(const KeyStroke& lhs, const KeyStroke& rhs)
{
	return (not (lhs == rhs));
}







/*
 ********************************************************************************
 ************************************* KeyBoard *********************************
 ********************************************************************************
 */

ostream&	operator<< (ostream& out, const KeyStroke& keyStroke)
{
	out << lparen << keyStroke.GetCharacter ();
	out << lparen;

#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (KeyStrokeModifiers, it, keyStroke.GetModifiers ().operator Iterator(KeyStrokeModifiers)* ()) {
#else
	ForEach (KeyStrokeModifiers, it, keyStroke.GetModifiers ()) {
#endif
		KeyStrokeModifiers m = it.Current ();
		switch (m) {
			case	KeyStroke::eAltKeyModifier: out << "A "; break;
			case	KeyStroke::eComposeKeyModifier: out << "D "; break;
			case	KeyStroke::eControlKeyModifier: out << "C "; break;
			case	KeyStroke::eCommandKeyModifier: out << "E "; break;
			case	KeyStroke::eMetaKeyModifier: out << "M "; break;
			case	KeyStroke::eOptionKeyModifier: out << "O "; break;
			case	KeyStroke::eShiftKeyModifier: out << "S "; break;
			default:	AssertNotImplemented(); break;
		}
	}
	out << rparen;
	out << rparen;
	return (out);
}

istream&	operator>> (istream& in, KeyStroke& keyStroke)
{
	AssertNotImplemented ();		// cuz of modifiers
	return (in);
}







/*
 ********************************************************************************
 ************************************* KeyBoard *********************************
 ********************************************************************************
 */

KeyBoard::KeyBoard ()
{
	ClearKeys ();
}

KeyBoard::KeyBoard (const KeyBoard& keyBoard)
{
	Assert (&keyBoard != this);
#if		qMacGDI || qXGDI
	memcpy (&fKeyMap, &keyBoard.fKeyMap, sizeof (fKeyMap));
#endif	/*GDI*/
}

const KeyBoard&	KeyBoard::operator= (const KeyBoard& rhs)
{
	Assert (&rhs != this);
#if		qMacGDI || qXGDI
	memcpy (&fKeyMap, &rhs.fKeyMap, sizeof (fKeyMap));
#endif	/*GDI*/
	return (*this);
}

void	KeyBoard::ClearKeys ()
{
#if		qMacGDI || qXGDI
	memset (&fKeyMap, 0, sizeof (fKeyMap));
#endif	/*GDI*/
}

Boolean	KeyBoard::GetKey (KeyCode keyCode) const
{
#if		qMacGDI || qXGDI
	Require ((keyCode >= eMinKeyCode) and (keyCode <= eMaxKeyCode));
	UInt8	index		=	keyCode / 32;
	UInt8	byteOffet	=	(3 - ((keyCode % 32) / 8)) * 8;
	UInt32	bitOffset	=	byteOffet + (keyCode % 8);

	return Boolean (!! (fKeyMap [index] & (1 << bitOffset)) );
#else   /* !qMacOS */
//	AssertNotImplemented ();
#if		qDebug
gDebugStream << "GetKey not implemented" << newline;
#endif /*qDebug*/
	return (False);
#endif	/* qMacOS */
}

void	KeyBoard::SetKey (KeyCode keyCode, Boolean pressed)
{
#if		qMacGDI || qXGDI
	Require ((keyCode >= eMinKeyCode) and (keyCode <= eMaxKeyCode));
	UInt8	index		=	keyCode / 32;
	UInt8	byteOffet	=	(3 - ((keyCode % 32) / 8)) * 8;
	UInt32	bitOffset	=	byteOffet + (keyCode % 8);
	if (pressed) {
		fKeyMap [index] |= (1 << bitOffset);
	}
	else {
		fKeyMap [index] &= ~(1 << bitOffset);
	}
	Ensure (GetKey (keyCode) == pressed);
#endif	/*GDI*/
}





/*
 ********************************************************************************
 ********************************** GetKeyBoard *********************************
 ********************************************************************************
 */
KeyBoard	GetKeyBoard ()
{
	KeyBoard	kbrd;
#if		qMacGDI
	::GetKeys (kbrd.GetOSRepresentation ());
#endif	/*GDI*/
	return (kbrd);
}







/*
 ********************************************************************************
 *********************************** operator== *********************************
 ********************************************************************************
 */
Boolean	operator== (const KeyBoard& lhs, const KeyBoard& rhs)
{
#if		qMacGDI || qXGDI
	return Boolean (memcmp (lhs.PeekAtOSRepresentation (), rhs.PeekAtOSRepresentation (), sizeof (long [4])) == 0);
#else   /* !qMacOS */
	AssertNotImplemented ();
	return (False);
#endif	/* qMacOS */
}




/*
 ********************************************************************************
 *********************************** operator!= *********************************
 ********************************************************************************
 */
Boolean	operator!= (const KeyBoard& lhs, const KeyBoard& rhs)
{
#if		qMacGDI || qXGDI
	return Boolean (memcmp (lhs.PeekAtOSRepresentation (), rhs.PeekAtOSRepresentation (), sizeof (long [4])) != 0);
#else   /* !qMacOS */
	AssertNotImplemented ();
        return (False);
#endif	/* qMacOS */
}



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

