/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CommandNumbers__
#define	__CommandNumbers__

/*
 * $Header: /fuji/lewis/RCS/CommandNumbers.hh,v 1.2 1992/09/01 17:36:41 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: CommandNumbers.hh,v $
 *		Revision 1.2  1992/09/01  17:36:41  sterling
 *		*** empty log message ***
 *
 *		Revision 1.3  1992/05/20  00:28:46  lewis
 *		Add XPM2.
 *
 *		Revision 1.1  92/04/27  23:06:28  23:06:28  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 *
 */

#include	"Command.hh"


enum {
	eWriteCPlusPlus		= CommandHandler::eFirstApplicationCommand,
	eWriteXPM,
	eWriteXPM2,
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__CommandNumbers__*/

