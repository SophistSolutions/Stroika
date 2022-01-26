/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SmallWindow__
#define	__SmallWindow__

/*
 * $Header: /fuji/lewis/RCS/SmallWindow.hh,v 1.2 1992/06/25 09:48:08 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: SmallWindow.hh,v $
 *		Revision 1.2  1992/06/25  09:48:08  sterling
 *		Add DTor for SmallWindow.
 *
 *		Revision 1.1  1992/06/20  18:23:41  lewis
 *		Initial revision
 *
 *
 *
 *
 */

#include	"Window.hh"


class SmallDocument;
class	SmallWindow : public Window {
	public:
		SmallWindow (SmallDocument& myDocument);
		~SmallWindow ();

	private:
		class SmallView*	fView;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__SmallWindow__*/
