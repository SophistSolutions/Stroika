/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TicTacToeMenus__
#define	__TicTacToeMenus__

/*
 * $Header: /fuji/lewis/RCS/TicTacToeMenus.hh,v 1.2 1992/09/01 17:57:21 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: TicTacToeMenus.hh,v $
 *		Revision 1.2  1992/09/01  17:57:21  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */

#include	"Application.hh"
#include	"StandardMenus.hh"

#include	"UserMenus.hh"


class TicTacToeNameBuilder : public UserCommandNamesBuilder {
	public:	
		TicTacToeNameBuilder ();
};

class	TicTacToeFileMenu : public DefaultFileMenu {
	public:
		TicTacToeFileMenu ();
};

class	TicTacToeEditMenu : public DefaultEditMenu {
	public:
		TicTacToeEditMenu ();
};

class	TicTacToeComputerMenu : public Menu {
	public:
		TicTacToeComputerMenu ();

		static	String	DefaultName ();
};





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__TicTacToeMenus__*/
