/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TicTacToeApplication__
#define	__TicTacToeApplication__

/*
 * $Header: /fuji/lewis/RCS/TicTacToeApplication.hh,v 1.2 1992/09/01 17:57:21 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: TicTacToeApplication.hh,v $
 *		Revision 1.2  1992/09/01  17:57:21  sterling
 *		*** empty log message ***
 *
 *		Revision 1.12  1992/05/19  16:12:37  lewis
 *		Get rid of Document and mvoe menus into TicTacToeWindow.
 *
 *		Revision 1.8  1992/03/05  23:00:20  lewis
 *		Support new style of menus for motif/mac.
 *
 *		Revision 1.7  1992/02/29  02:23:53  lewis
 *		Override app dtor to remove menus from menubar.
 *
 *		Revision 1.5  1992/02/12  08:19:44  lewis
 *		Use Menu rather than obsolete CStandardMenu.
 *
 *
 *
 *
 */

#include	"Application.hh"
#include	"DebugMenu.hh"
#include	"Menu.hh"
#include	"MenuBar.hh"
#include	"Window.hh"

#include	"TicTacToeMenus.hh"



class	TicTacToeApplication : public Application, public WindowController {
	public:
		TicTacToeApplication (int argc, const char* argv []);
		~TicTacToeApplication ();

		override	void		DoSetupMenus ();
		override	Boolean		DoCommand (const CommandSelection& selection);

		override	void		HandleClose (Window& ofWindow);

		static	TicTacToeApplication&	Get ();

	private:
		TicTacToeNameBuilder	fNameBuilder;
		class	TicTacToeWindow;
		TicTacToeWindow*		fWindow;

		static	TicTacToeApplication*	sTicTacToeApplication;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__TicTacToeApplication__*/
