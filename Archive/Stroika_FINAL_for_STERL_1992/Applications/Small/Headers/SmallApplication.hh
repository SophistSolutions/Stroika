/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__SmallApplication__
#define	__SmallApplication__

/*
 * $Header: /fuji/lewis/RCS/SmallApplication.hh,v 1.1 1992/06/20 18:23:41 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: SmallApplication.hh,v $
 *		Revision 1.1  1992/06/20  18:23:41  lewis
 *		Initial revision
 *
 *		Revision 1.9  1992/04/20  14:05:05  lewis
 *		*** empty log message ***
 *
 *		Revision 1.8  92/03/26  18:10:54  18:10:54  sterling (Sterling Wight)
 *		*** empty log message ***
 *		
 *		Revision 1.7  1992/03/05  20:52:24  lewis
 *		Convert to new Menubar scheme.
 *
 *		Revision 1.6  1992/02/29  02:24:46  lewis
 *		Override app dtor to remove menus from menubar.
 *
 *		Revision 1.5  1992/02/12  08:26:55  lewis
 *		Use Menu rather than obsolete StandardMenu.
 *
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"Menu.hh"
#include	"MenuBar.hh"
#include	"StandardMenus.hh"


/*
 * should be nested within SmallApplication.
 * This class adds any bindings between command numbers and menu item names.
 * A subclass of FrameworkCommandNamesBuilder should always be added as the
 * first field of an application, before the menubar or any of the menus. It 
 * is important that this field be declared before the declaration of any menus,
 * because declaration order specifies construction order, and the menus depend
 * on having the name bindings already set up.
 */
class SmallCommandNamesBuilder : public FrameworkCommandNamesBuilder
{
	public:
		SmallCommandNamesBuilder ();
};

class	SmallApplication : public Application {
	public:
		SmallApplication (int argc, const char* argv []);
		~SmallApplication ();

		static	SmallApplication&	Get ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

	private:
		SmallCommandNamesBuilder	fNamesBuilder;
		MenuBar						fMenuBar;
		SystemMenu					fSystemMenu;
		DefaultFileMenu				fFileMenu;
		DefaultEditMenu				fEditMenu;
#if		qDebug
		DebugMenu					fDebugMenu;
#endif

		static	SmallApplication*	sThe;
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__SmallApplication__*/

