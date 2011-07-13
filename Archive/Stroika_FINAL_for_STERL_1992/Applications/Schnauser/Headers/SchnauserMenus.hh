/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SchnauserMenus__
#define	__SchnauserMenus__

/*
 * $Header: /fuji/lewis/RCS/SchnauserMenus.hh,v 1.2 1992/09/01 17:40:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *
 *
 *
 */

#include	"DebugMenu.hh"
#include	"Menu.hh"
#include	"StandardMenus.hh"

#include	"SchnauserConfig.hh"


class SchnauserCommandNamesBuilder : public FrameworkCommandNamesBuilder {
	public:
		SchnauserCommandNamesBuilder ();
};


class	SchnauserFileMenu	: public DefaultFileMenu {
	public:
		SchnauserFileMenu ();
};


class	SchnauserEditMenu	: public DefaultEditMenu {
	public:
		SchnauserEditMenu ();
};


class	SchnauserFindMenu	: public Menu {
	public:
		SchnauserFindMenu ();

		static	String	DefaultName ();
};


class	SchnauserProjectMenu	: public Menu {
	public:
		SchnauserProjectMenu ();

		static	String	DefaultName ();
};


class	SchnauserViewsMenu	: public Menu {
	public:
		SchnauserViewsMenu ();

		static	String	DefaultName ();
};


class	SchnauserWindowsMenu	: public Menu {
	public:
		SchnauserWindowsMenu ();

		static	String	DefaultName ();
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__SchnauserMenus__*/

