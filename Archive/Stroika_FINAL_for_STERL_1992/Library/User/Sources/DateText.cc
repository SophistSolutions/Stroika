/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DateText.cc,v 1.1 1992/06/20 17:35:17 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: DateText.cc,v $
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/05/13  17:49:50  lewis
 *		STERL - funny change - made kBadDate be 1,1,1700 instead of zero?\
 *
 *		Revision 1.9  92/03/11  23:10:01  23:10:01  lewis (Lewis Pringle)
 *		Got rid of old qOldStreamLibOnly support.
 *		
 *		Revision 1.8  1992/03/10  00:37:19  lewis
 *		Also, change interface to ValidateKey () to take KeyStroke instead of Character, and KeyBoard.
 *
 *		Revision 1.7  1992/03/05  21:50:55  sterling
 *		changed validation code
 *
 *
 *
 */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DateText.hh"



/*
 ********************************************************************************
 ********************************** DateText ************************************
 ********************************************************************************
 */
const	Date	DateText::kBadDate	= Date (1, 1, 1700);	// hack, no good way to make bad date
DateText::DateText (TextController* controller):
	TextEdit (controller),
	fMinDate (kBadDate),
	fMaxDate (kBadDate)
{
}

DateText::DateText (const Date& date, TextController* controller):
	TextEdit (controller),
	fMinDate (kBadDate),
	fMaxDate (kBadDate)
{
	SetDate (date);
}

Boolean	DateText::ValidateKey (const KeyStroke& keyStroke)
{
	char	c	=	keyStroke.GetCharacter ();		// not really very safe...Needs more thought how to do this safely..

	if ((c != '/') and (c > 31) and ((c < '0') or (c > '9'))) {
		return (TextEdit::ValidateKey (keyStroke));
	}
	return (False);
}

Boolean	DateText::DateValid (const Date& date)
{
	if (date == kBadDate) {
		return (False);
	}
	else if ((fMinDate != kBadDate) and (date < fMinDate)) {
		return (False);
	}
	else if ((fMaxDate != kBadDate) and (date > fMaxDate)) {
		return (False);
	}
	return (True);
}

void	DateText::Validate ()
{
	if (GetLength () > 0) {
		Date	d = GetDate ();
		if (d == kBadDate) {
			FocusItem::sFocusFailed.Raise ("Current value is not a valid date.");
		}
		else if ((fMinDate != kBadDate) and (d < fMinDate)) {
			FocusItem::sFocusFailed.Raise ("Current date is too old.");
		}
		else if ((fMaxDate != kBadDate) and (d > fMaxDate)) {
			FocusItem::sFocusFailed.Raise ("Current date is too recent.");
		}
	}
	TextEdit::Validate ();
}

Date	DateText::GetMinDate () const
{
	return (fMinDate);
}

void	DateText::SetMinDate (const Date& date)
{
	fMinDate = date;
}

Date	DateText::GetMaxDate () const
{
	return (fMaxDate);
}

void	DateText::SetMaxDate (const Date& date)
{
	fMaxDate = date;
}
		
Date	DateText::StringToDate (const String& s)
{
	Date	date	= kBadDate;
	
	if (s != kEmptyString) {			
		StringStream buf;
		buf << s;
		buf.seekg (0);
		buf >> date;
	}
	
	return (date);
}

Date	DateText::GetDate () const
{
	return (StringToDate (GetText ()));
}

void	DateText::SetDate (const Date& date)
{
	StringStream buf;
	buf << date;
	SetText (String (buf));
}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

