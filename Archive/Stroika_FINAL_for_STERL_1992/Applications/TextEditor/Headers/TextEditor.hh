/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TextEditor__
#define	__TextEditor__

/*
 * $Header: /fuji/lewis/RCS/TextEditor.hh,v 1.1 1992/06/20 18:37:23 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditor.hh,v $
 *		Revision 1.1  1992/06/20  18:37:23  lewis
 *		Initial revision
 *
 *		Revision 1.10  1992/04/02  17:45:18  lewis
 *		*** empty log message ***
 *
 *		Revision 1.9  92/03/26  17:56:59  17:56:59  sterling (Sterling Wight)
 *		*** empty log message ***
 *		
 *		Revision 1.8  1992/03/06  03:14:45  sterling
 *		used new menu stuff, a changed to TextScroller
 *
 *		Revision 1.7  1992/02/29  02:24:18  lewis
 *		Override app dtor to remove menus from menubar.
 *
 *		Revision 1.5  1992/02/12  08:23:26  lewis
 *		Use Menu rather than obsolete StanardMenu.
 *
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"Menu.hh"
#include	"MenuBar.hh"

#include	"UserMenus.hh"



// should be scoped within TextEditorApplication
class TextEditorCommandNamesBuilder : public UserCommandNamesBuilder {
	public:
		TextEditorCommandNamesBuilder ();
};


class	TextEditorApplication : public Application {
	public:
		TextEditorApplication (int argc, const char* argv[]);
		~TextEditorApplication ();

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);

		static	TextEditorApplication&	Get ();

	private:
		TextEditorCommandNamesBuilder	fBuilder;
		MenuBar							fMenuBar;
		SystemMenu						fSystemMenu;
		DefaultFileMenu					fFileMenu;
		DefaultEditMenu					fEditMenu;
		DefaultFormatMenu				fFormatMenu;
#if		qDebug
		DebugMenu						fDebugMenu;
#endif
		static	TextEditorApplication*	sThe;
};




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__TextEditor__*/

