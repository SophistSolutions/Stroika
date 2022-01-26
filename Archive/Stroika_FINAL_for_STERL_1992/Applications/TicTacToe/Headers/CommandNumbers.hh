/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CommandNumbers__
#define	__CommandNumbers__

/*
 * $Header: /fuji/lewis/RCS/CommandNumbers.hh,v 1.2 1992/09/01 17:57:21 sterling Exp $
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
 *		Revision 1.2  1992/09/01  17:57:21  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 *
 */

#include	"Command.hh"

enum {
	/*
	 * Expert commands
	 */
	eSuggestMove = CommandHandler::eFirstApplicationCommand,
	eAutoPilot,
	eSound,
	eComputerOpponent,
};



#endif	/*__CommandNumbers__*/
