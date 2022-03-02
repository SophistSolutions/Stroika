/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Schnauser__
#define	__Schnauser__

/*
 * $Header: /fuji/lewis/RCS/SchnauserApplication.hh,v 1.2 1992/09/01 17:40:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: SchnauserApplication.hh,v $
 *		Revision 1.2  1992/09/01  17:40:39  sterling
 *		*** empty log message ***
 *
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"MenuBar.hh"

#include	"SchnauserConfig.hh"
#include	"SchnauserMenus.hh"



class	SchnauserApplication : public Application {
	public:
		SchnauserApplication (int argc, const char* argv []);
		~SchnauserApplication ();

		static	SchnauserApplication&	Get ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

	private:
		SchnauserCommandNamesBuilder	fNamesBuilder;
		MenuBar							fMenuBar;
		SystemMenu						fSystemMenu;
		SchnauserFileMenu				fFileMenu;
		SchnauserEditMenu				fEditMenu;
		SchnauserFindMenu				fFindMenu;
		SchnauserProjectMenu			fProjectMenu;
		SchnauserViewsMenu				fViewsMenu;
		SchnauserWindowsMenu			fWindowsMenu;

#if		qDebug
		DebugMenu						fDebugMenu;
#endif

		static	SchnauserApplication*	sThe;
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Schnauser__*/

