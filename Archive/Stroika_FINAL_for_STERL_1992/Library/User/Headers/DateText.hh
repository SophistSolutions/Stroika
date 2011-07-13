/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__DateText__
#define	__DateText__

/*
 * $Header: /fuji/lewis/RCS/DateText.hh,v 1.3 1992/09/08 16:00:29 lewis Exp $
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
 *	$Log: DateText.hh,v $
 *		Revision 1.3  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/03/09  23:55:28  lewis
 *		Change interface to ValidateKey () to take KeyStroke instead of Character, and KeyBoard.
 *
 *		Revision 1.6  1992/03/05  21:46:06  sterling
 *		changed validateon code
 *
 *
 *
 */

#include "Date.hh"

#include "TextEdit.hh"


class	DateText : public TextEdit {
	public:
		static	const	Date	kBadDate;
		
		DateText (TextController* controller = Nil);
		DateText (const Date& date, TextController* controller = Nil);	
		
		nonvirtual	Date	GetMinDate () const;
		virtual		void	SetMinDate (const Date& date);
		
		nonvirtual	Date	GetMaxDate () const;
		virtual		void	SetMaxDate (const Date& date);
				
		nonvirtual	Date	GetDate () const;
		virtual		void	SetDate (const Date& date);
		
		virtual		Boolean	DateValid (const Date& date);

		static		Date	StringToDate (const String& s);

	protected:
		override	void	Validate ();
		override	Boolean	ValidateKey (const KeyStroke& keyStroke);

	private:
		Date	fMinDate;
		Date	fMaxDate;
};




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/* __DateText__ */
