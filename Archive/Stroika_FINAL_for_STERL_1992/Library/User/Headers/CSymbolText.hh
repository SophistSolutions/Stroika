/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CSymbolText__
#define	__CSymbolText__	1

/*
 * $Header: /fuji/lewis/RCS/CSymbolText.hh,v 1.3 1992/09/08 16:00:29 lewis Exp $
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
 *	$Log: CSymbolText.hh,v $
 *		Revision 1.3  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.7  1992/03/09  23:53:24  lewis
 *		Use new HandleKeyStroke () interface.
 *		Also, change interface to ValidateKey () to take KeyStroke instead of Character, and KeyBoard.
 *
 *		Revision 1.6  1992/03/05  21:46:06  sterling
 *		changed validateon code
 *
 *
 *
 */

#include	"TextEdit.hh"


class	CSymbolText : public TextEdit {
	public:
		CSymbolText (TextController* c = Nil);

		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);

	protected:
		override	void	Validate ();
		override	Boolean	ValidateKey (const KeyStroke& keyStroke);
};


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif /* __CSymbolText__ */
