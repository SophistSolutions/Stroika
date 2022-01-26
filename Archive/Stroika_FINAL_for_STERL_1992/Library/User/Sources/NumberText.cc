/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/NumberText.cc,v 1.4 1992/09/01 15:58:05 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: NumberText.cc,v $
 *		Revision 1.4  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/03  02:29:35  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.16  1992/06/09  16:15:17  sterling
 *		made GetValue be const
 *
 *		Revision 1.14  92/05/19  14:44:47  14:44:47  lewis (Lewis Pringle)
 *		Get rid of enum NumberBase - just make a typedef.
 *		
 *		Revision 1.13  92/04/06  14:55:31  14:55:31  sterling (Sterling Wight)
 *		fixed delete key, made single line by default
 *		
 *		Revision 1.12  92/03/10  00:38:00  00:38:00  lewis (Lewis Pringle)
 *		Also, change interface to ValidateKey () to take KeyStroke instead of Character, and KeyBoard.
 *		
 *		Revision 1.11  1992/03/05  21:50:55  sterling
 *		changed validation code
 *
 *		Revision 1.10  1992/01/29  05:35:04  sterling
 *		removed typedef inherited cuz of compiler bugs
 *
 *		Revision 1.9  1992/01/27  06:20:38  lewis
 *		Use inherited typedef, and check for kEmpyString, and return kBadNumber - cannot count on Format.cc routines to do this!!!
 *
 *		Revision 1.8  1992/01/24  23:44:07  lewis
 *		Removed unessary overides, and fValue field. GetValue no longer does validation testing. No longer
 *		provide number conversion routines - use Format.hh.
 *
 *
 */

#include	"Format.hh"
#include	"StreamUtils.hh"	

#include	"NumberText.hh"	
	





/*
 ********************************************************************************
 ************************************** NumberText ******************************
 ********************************************************************************
 */

NumberText::NumberText (TextController* controller, Real start, Real minValue, 
			Real maxValue, Int8 precision, NumberBase base):
	TextEdit (controller),
	fMinValue (minValue),
	fMaxValue (maxValue),
	fPrecision (precision),
	fBase (base)
{
	SetJustification (eJustRight);
	SetValue (start);
	SetWordWrap (False);
	SetMultiLine (False);
}

void	NumberText::Validate ()
{
	if (GetLength () > 0) {
		Real	value = StringToNumber (GetText ());
		if (IsBadNumber (value)) {
			FocusItem::sFocusFailed.Raise ("Current value is not a number.");
		}
		else if ((not IsBadNumber (GetMinValue ())) and (value < GetMinValue ())) {
			FocusItem::sFocusFailed.Raise ("Current value is too small.");
		}
		else if ((not IsBadNumber (GetMaxValue ())) and (value > GetMaxValue ())) {
			FocusItem::sFocusFailed.Raise ("Current value is too large.");
		}
	}
	
	TextEdit::Validate ();
}

Boolean	NumberText::ValidateKey (const KeyStroke& keyStroke)
{
	if ((keyStroke == KeyStroke::kDelete) or
		(keyStroke == KeyStroke::kBackspace) or
		(keyStroke == KeyStroke::kLeftArrow) or
		(keyStroke == KeyStroke::kRightArrow)) {
		return (TextEdit::ValidateKey (keyStroke));
	}
	
	char	c	=	keyStroke.GetCharacter ();		// not really very safe...Needs more thought how to do this safely..

	if (((c == '.') and (GetPrecision () > 0)) or ((c == '-') and (IsBadNumber (GetMinValue ()) or (GetMinValue () < 0)))) {
		return (TextEdit::ValidateKey (keyStroke));
	}	

	switch (GetBase ()) {
		case	8:		{
			if ((c >= '0') and (c <= '7')) {
				return (TextEdit::ValidateKey (keyStroke));
			}
		}
		break;
		case	10:	{
			if ((c >= '0') and (c <= '9')) {
				return (TextEdit::ValidateKey (keyStroke));
			}
		}
		break;
		case	16:	{
			if (((c >= '0') and (c <= '9')) or ((c >= 'a') and (c <= 'f')) or ((c >= 'A') and (c <= 'F'))) {
				return (TextEdit::ValidateKey (keyStroke));
			}
		}
		break;
	
		default:	AssertNotReached ();
	}
	return (False);
}

Real	NumberText::GetValue () const
{
	return (StringToNumber (GetText ()));	
}

void	NumberText::SetValue (Real value, Panel::UpdateMode updateMode)
{
	Require (IsBadNumber (value) or NumberValid (value));
	SetText (NumberToString (value), updateMode);
}

Real	NumberText::GetMinValue () const
{
	return (fMinValue);
}

void	NumberText::SetMinValue (Real value)
{
	fMinValue = value;
}

Real	NumberText::GetMaxValue () const
{
	return (fMaxValue);
}

void	NumberText::SetMaxValue (Real value)
{
	fMaxValue = value;
}

Int8	NumberText::GetPrecision () const
{
	return (fPrecision);
}

void	NumberText::SetPrecision (Int8 precision)
{
	fPrecision = precision;
}

NumberBase	NumberText::GetBase () const
{
	return (fBase);
}

void	NumberText::SetBase (NumberBase base)
{
	fBase = base;
}

Boolean	NumberText::NumberValid (Real number) const
{
	return (Boolean (
		(IsBadNumber (GetMinValue ()) or (number >= GetMinValue ())) and 
		(IsBadNumber (GetMaxValue ()) or (number <= GetMaxValue ())) and 
		(True /* some precision test */)
	));
}

Real	NumberText::StringToNumber (const String& s) const
{
	if (s == kEmptyString) {
		return (kBadNumber);
	}
	Try {
		/*
		 * If precision is 0, then we can use the integer routines, and therefore handle the base.
		 * If it is non-zero, we must ignore the base since we have no routines to handle that. Probably
		 * we should disallow that situation...
		 */
		if (GetPrecision () == 0) {
			return (strtol (s));
		}
		else {
			return (strtod (s, GetPrecision ()));
		}
	}
	Catch () {
		return (kBadNumber);
	}
}

String	NumberText::NumberToString (Real number) const
{
	if (IsBadNumber (number)) {
		return (kEmptyString);
	}
	if (GetPrecision () == 0) {
		return (ltostring ((long)number, GetBase ()));
	}
	else {
		return (dtostring (number, GetPrecision ()));
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

