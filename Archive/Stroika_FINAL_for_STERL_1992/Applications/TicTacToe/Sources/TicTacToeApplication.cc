/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TicTacToeApplication.cc,v 1.3 1992/09/08 18:04:12 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TicTacToeApplication.cc,v $
 *		Revision 1.3  1992/09/08  18:04:12  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  17:58:36  sterling
 *		*** empty log message ***
 *
 *		Revision 1.20  1992/05/19  16:14:49  lewis
 *		Get rid of Document and mvoe menus into TicTacToeWindow.
 *
 *		Revision 1.17  92/04/07  10:37:46  10:37:46  lewis (Lewis Pringle)
 *		Fix ifdefs for debugmenu so I can compile with debug off.
 *		
 *		Revision 1.12  1992/03/05  23:00:39  lewis
 *		Support new style of menus for motif/mac.
 *		Just prototype implementation for motif.
 *
 *		Revision 1.11  1992/02/29  02:24:05  lewis
 *		Override app dtor to remove menus from menubar.
 *
 *		Revision 1.9  1992/02/11  06:00:00  lewis
 *		Minor cleanups.
 *
 *		Revision 1.8  1992/02/11  02:03:15  lewis
 *		Set closable attribute in shell hints.
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Dialog.hh"
#include	"Document.hh"
#include	"MenuBarMainView.hh"
#include	"MenuBar.hh"
#include	"Shell.hh"
#include	"Window.hh"

#include	"SimpleAboutBox.hh"

#include	"CommandNumbers.hh"
#include	"TicTacToeMenus.hh"
#include	"TicTacToeView.hh"

#include	"TicTacToeApplication.hh"





class	TicTacToeWindow : public Window {
	public:
		TicTacToeWindow ();
		virtual ~TicTacToeWindow ();

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);

	public:
		TicTacToeView			fTicTacToeView;
		MenuBarMainView			fMenuBarMainView;
		MenuBar					fMenuBar;
		SystemMenu				fSystemMenu;
		TicTacToeFileMenu		fFileMenu;
		TicTacToeEditMenu		fEditMenu;
		TicTacToeComputerMenu	fComputerMenu;
#if		qDebug
		DebugMenu				fDebugMenu;
#endif
};









/*
 ********************************************************************************
 ******************************* TicTacToeApplication ***************************
 ********************************************************************************
 */
TicTacToeApplication*	TicTacToeApplication::sTicTacToeApplication	=	Nil;


TicTacToeApplication::TicTacToeApplication (int argc, const char* argv []):
	Application (argc, argv),
	WindowController (),
	fNameBuilder (),
	fWindow (Nil)
{
	Assert (sTicTacToeApplication == Nil);
	sTicTacToeApplication = this;

	fWindow = new TicTacToeWindow ();
	fWindow->SetWindowController (this);

#if		qMacGUI
	SetMenuBar (&fWindow->fMenuBar);
#endif

	fWindow->Select ();
}

TicTacToeApplication::~TicTacToeApplication ()
{
	// maybe we should override shutdown and do this here...
	if (fWindow != Nil) {
		fWindow->SetVisible (False);
		fWindow->SetActive (False);			// I wonder if this should happen automatically???? LGP May 19, 1992 - but how???
	}

#if		qMacGUI
	SetMenuBar (Nil);
#endif

	delete (fWindow);
}

TicTacToeApplication&	TicTacToeApplication::Get ()
{
	EnsureNotNil (sTicTacToeApplication);
	return (*sTicTacToeApplication);
}

void	TicTacToeApplication::DoSetupMenus ()
{
	Application::DoSetupMenus ();

	EnableCommand (eAboutApplication, True);
}

Boolean		TicTacToeApplication::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eAboutApplication:
			DisplaySimpleAboutBox ();
			return (True);

		default:
			return (Application::DoCommand (selection));
	}
	AssertNotReached (); return (True);
}

void	TicTacToeApplication::HandleClose (Window& ofWindow)
{
	if (&ofWindow == fWindow) {
		TryToShutDown ();
	}
}





/*
 ********************************************************************************
 *********************************** TicTacToeWindow ****************************
 ********************************************************************************
 */
TicTacToeWindow::TicTacToeWindow ():
	Window (),
	fTicTacToeView (),
	fMenuBar (),
	fFileMenu (),
	fEditMenu (),
	fComputerMenu ()
#if		qDebug
	,fDebugMenu ()
#endif
	,fMenuBarMainView ()
{
#if		qMacGUI
	fMenuBar.AddMenu (&fSystemMenu, fSystemMenu.DefaultName ());
#endif	/*qMacGUI*/
	fMenuBar.AddMenu (&fFileMenu, fFileMenu.DefaultName ());
	fMenuBar.AddMenu (&fEditMenu, fEditMenu.DefaultName ());
	fMenuBar.AddMenu (&fComputerMenu, fComputerMenu.DefaultName ());
#if		qDebug
	fMenuBar.AddMenu (&fDebugMenu, fDebugMenu.DefaultName ());
#endif

#if		qMotifGUI
	fMenuBarMainView.SetMenuBar (&fMenuBar);
#endif
	fMenuBarMainView.SetMainView (&fTicTacToeView);


// Fix MenuBarmainView toave  a setmainview and targets and auto dispatch like window does...
	SetMainViewAndTargets (&fMenuBarMainView, &fTicTacToeView, &fTicTacToeView);

	// not sure we need to do this?
	WindowShellHints	hints	=	GetShell ().GetWindowShellHints ();
	hints.SetCloseable (False);
	GetShell ().SetWindowShellHints (hints);
	SetTitle ("Tic Tac Toe");
}

TicTacToeWindow::~TicTacToeWindow ()
{
#if		qMacGUI
	fMenuBar.RemoveMenu (&fSystemMenu);
#endif	/*qMacGUI*/
	fMenuBar.RemoveMenu (&fFileMenu);
	fMenuBar.RemoveMenu (&fEditMenu);
	fMenuBar.RemoveMenu (&fComputerMenu);
#if		qDebug
	fMenuBar.RemoveMenu (&fDebugMenu);
#endif

	fMenuBarMainView.SetMainView (Nil);
#if		qMotifGUI
	fMenuBarMainView.SetMenuBar (N);
#endif
	SetMainViewAndTargets (Nil, Nil, Nil);
}

void	TicTacToeWindow::DoSetupMenus ()
{
	Window::DoSetupMenus ();

	EnableCommand (eNew, True);
}

Boolean		TicTacToeWindow::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eNew:
			fTicTacToeView.ClearBoard ();
			return (True);

		default:
			return (Window::DoCommand (selection));
	}
	AssertNotReached (); return (True);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

