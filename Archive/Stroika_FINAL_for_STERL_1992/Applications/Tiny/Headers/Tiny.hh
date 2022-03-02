/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Tiny__
#define	__Tiny__

/*
 * $Header: /fuji/lewis/RCS/Tiny.hh,v 1.1 1992/06/20 18:40:09 lewis Exp $
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
 *
 * Changes:
 *	$Log: Tiny.hh,v $
 *		Revision 1.1  1992/06/20  18:40:09  lewis
 *		Initial revision
 *
 *		Revision 1.1  1992/05/01  23:14:48  lewis
 *		Initial revision
 *
 *		Revision 1.9  92/03/26  17:40:40  17:40:40  sterling (Sterling Wight)
 *		use new namebuilder mechanism
 *		
 *		Revision 1.7  1992/02/29  02:23:05  lewis
 *		Override app dtor to remove menus from menubar.
 *
 *		Revision 1.6  1992/02/12  08:18:10  lewis
 *		Use Menu rather than old StandardMenu.
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"Menu.hh"
#include	"MenuBar.hh"
#include	"StandardMenus.hh"

/*
 * should be nested within TinyApplication.
 * This class adds any bindings between command numbers and menu item names.
 * A subclass of FrameworkCommandNamesBuilder should always be added as the
 * first field of an application, before the menubar or any of the menus. It 
 * is important that this field be declared before the declaration of any menus,
 * because declaration order specifies construction order, and the menus depend
 * on having the name bindings already set up.
 */
class TinyCommandNamesBuilder : public FrameworkCommandNamesBuilder
{
	public:
		TinyCommandNamesBuilder ();
};

class	TinyApplication : public Application {
	public:
		TinyApplication (int argc, const char* argv []);
		~TinyApplication ();

		static	TinyApplication&	Get ();

	private:
		TinyCommandNamesBuilder	fNamesBuilder;
		MenuBar					fMenuBar;
		SystemMenu				fSystemMenu;
		DefaultFileMenu			fFileMenu;
		DefaultEditMenu			fEditMenu;
#if		qDebug
		DebugMenu				fDebugMenu;
#endif

		static	TinyApplication*	sThis;
};

// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/*__Tiny__*/
