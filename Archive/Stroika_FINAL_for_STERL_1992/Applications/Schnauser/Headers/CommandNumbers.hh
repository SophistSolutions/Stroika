/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__CommandNumbers__
#define	__CommandNumbers__

/*
 * $Header: /fuji/lewis/RCS/CommandNumbers.hh,v 1.2 1992/09/01 17:40:39 sterling Exp $
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
 *		Revision 1.2  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *
 *
 */

#include	"Command.hh"

#include	"SchnauserConfig.hh"


enum { kMaxWindowsMenuItems = 100 };

enum {
// Need new command #s
	eSomeCommand		= CommandHandler::eFirstApplicationCommand,

	//eEditUserPreferences
	eEditUserPreferences,


	// Find/Searching
	eQueryFind,
	eFindAgain,
	eFindSelection,
	eDisplaySelection,
	eQueryReplace,
	eReplaceAgain,
	eGotoLine,


	// Views
	eNewFilesView,
	eNewClassesView,
	eNewFunctionsView,

	eOpenView,

	eViewViewsByName,
	eViewViewsByIcon,


	// Windows Menu commands
	eWindowsMenuStart,
	eWindowsMenuEnd	=	eWindowsMenuStart + kMaxWindowsMenuItems,

};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__CommandNumbers__*/

