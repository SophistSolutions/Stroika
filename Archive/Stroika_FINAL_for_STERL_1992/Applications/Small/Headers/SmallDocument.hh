/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SmallDocument__
#define	__SmallDocument__
/*
 * $Header: /fuji/lewis/RCS/SmallDocument.hh,v 1.1 1992/06/20 18:23:41 lewis Exp $
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
 *	$Log: SmallDocument.hh,v $
 *		Revision 1.1  1992/06/20  18:23:41  lewis
 *		Initial revision
 *
 *		Revision 1.4  1992/04/20  14:05:06  lewis
 *		*** empty log message ***
 *
 *		Revision 1.3  92/03/26  18:10:54  18:10:54  sterling (Sterling Wight)
 *		*** empty log message ***
 *		
 *
 *
 */

#include	"Document.hh"

class	SmallWindow;
class	SmallDocument : public Document {
	public:
		SmallDocument ();
		virtual ~SmallDocument ();

		override	void	TryToClose ();
		override	String	GetPrintName ();

		nonvirtual	void	DoNewState ();

	private:
		SmallWindow*	fWindow;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__SmallDocument__*/

