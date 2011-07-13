/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CSymbolText.cc,v 1.2 1992/09/01 15:58:05 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 * Notes:
 *
 * Changes:
 *	$Log: CSymbolText.cc,v $
 *		Revision 1.2  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/04/06  14:53:47  sterling
 *		made delete key work properly
 *
 *		Revision 1.11  92/03/10  01:27:50  01:27:50  lewis (Lewis Pringle)
 *		Use temporary constant kUnderBar, instead of compiler temp since CFront doesnt support temps of class with dtor cannot
 *		appear in expression with ||.
 *		
 *		Revision 1.10  1992/03/10  00:36:36  lewis
 *		Use new HandleKeyStroke () interface.
 *
 *		Revision 1.9  1992/03/05  21:50:55  sterling
 *		changed validation code
 *
 *		Revision 1.8  1992/01/31  17:14:08  sterling
 *		turned of WordWrap by default
 *
 *
 *
 *
 */

#include	<ctype.h>

#include	"CSymbolText.hh"



/*
 ********************************************************************************
 ******************************** CSymbolText ***********************************
 ********************************************************************************
 */

CSymbolText::CSymbolText (TextController* c):
	TextEdit (c)
{
	SetTextRequired (False);
}

void	CSymbolText::Validate ()
{
	Boolean	valid = True;
	String	text = GetText ();
	if (text.GetLength () == 0) {
		// let edittext worry about whether this is correct or not
	}
	else if ((not (isalpha (text[1].GetAsciiCode ()))) and (text[1] != '_')) {
		valid = False;
	}
	else {
		for (register CollectionSize c = 2; (valid) and (c <= text.GetLength ()); c++) {
			if ((not (isalnum (text[2].GetAsciiCode ()))) and (text[2] != '_')) {
				valid = False;
			}
		}
	}
	
	if (not valid) {
		FocusItem::sFocusFailed.Raise ("Current text is not a valid C symbol.");
	}
	TextEdit::Validate ();
}

Boolean	CSymbolText::ValidateKey (const KeyStroke& keyStroke)
{
	const	KeyStroke	kUnderBar	=	KeyStroke ('_');
	if ((keyStroke == KeyStroke::kDelete) or
		(keyStroke == KeyStroke::kBackspace) or
		(keyStroke == KeyStroke::kLeftArrow) or
		(keyStroke == KeyStroke::kRightArrow) or
		(keyStroke == KeyStroke::kSpace) or
		(keyStroke == kUnderBar)) {
		return (TextEdit::ValidateKey (keyStroke));
	}
	else {
		// isalpha/isalnum probably not right here??? - not portable...
		char	c	=	keyStroke.GetCharacter ();
		if ((GetSelection ().GetFrom () == 1) and (not (isalpha (c)))) {
			return (False);
		}
		else if (not (isalnum (c))) {
			return (False);
		}
	}
	return (TextEdit::ValidateKey (keyStroke));
}

Boolean	CSymbolText::HandleKeyStroke (const KeyStroke& keyStroke)
{
	const	KeyStroke	kUnderBar	=	KeyStroke ('_');
	if (ValidateKey (keyStroke)) {
		if ((keyStroke == KeyStroke::kDelete) or
			(keyStroke == KeyStroke::kBackspace) or
			(keyStroke == KeyStroke::kLeftArrow) or
			(keyStroke == KeyStroke::kRightArrow)) {
			return (TextEdit::HandleKeyStroke (keyStroke));
		}

		if (keyStroke == KeyStroke::kSpace) {
			return (TextEdit::HandleKeyStroke (kUnderBar));
		}
		return (TextEdit::HandleKeyStroke (keyStroke));
	}
	return (False);
}



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***
