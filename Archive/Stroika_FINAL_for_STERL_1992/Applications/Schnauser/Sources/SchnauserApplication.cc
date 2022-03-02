/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SchnauserApplication.cc,v 1.4 1992/09/08 17:08:42 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SchnauserApplication.cc,v $
 *		Revision 1.4  1992/09/08  17:08:42  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"SimpleAboutBox.hh"

#include	"CommandNumbers.hh"
#include	"SchnauserDocument.hh"
#include	"UserPreferences.hh"

#include	"SchnauserApplication.hh"






/*
 ********************************************************************************
 *************************** SchnauserApplication **************************
 ********************************************************************************
 */
SchnauserApplication*	SchnauserApplication::sThe	=	Nil;

SchnauserApplication::SchnauserApplication (int argc, const char* argv[]):
	Application (argc, argv),
	fNamesBuilder (),
	fMenuBar (),
	fSystemMenu (),
	fFileMenu (),
	fEditMenu ()
#if		qDebug
	,fDebugMenu ()
#endif
{
	Assert (sThe == Nil);
	sThe = this;

	/*
	 * Add all the appropriate menus to our menu bar. Use default names. Only
	 * add the system menu to the menu bar, on the macintosh, and only
	 * add the Debug menu if debug is turned on.
	 */
#if		qMacToolkit 
	fMenuBar.AddMenu (&fSystemMenu, fSystemMenu.DefaultName ());
#endif
	fMenuBar.AddMenu (&fFileMenu, fFileMenu.DefaultName ());
	fMenuBar.AddMenu (&fEditMenu, fEditMenu.DefaultName ());
	fMenuBar.AddMenu (&fFindMenu, fFindMenu.DefaultName ());
	fMenuBar.AddMenu (&fProjectMenu, fProjectMenu.DefaultName ());
	fMenuBar.AddMenu (&fViewsMenu, fViewsMenu.DefaultName ());
	fMenuBar.AddMenu (&fWindowsMenu, fWindowsMenu.DefaultName ());
#if		qDebug 
	fMenuBar.AddMenu (&fDebugMenu, fDebugMenu.DefaultName ());
#endif

	SetMenuBar (&fMenuBar);
}

SchnauserApplication::~SchnauserApplication ()
{
	SetMenuBar (Nil);	// we dont want our base class to hold a reference to an object destroyed
	  					// while the Application sub-object still exists!

	/*
	 * Remove the menus from the menu bar, since the menus are about to be destroyed, and
	 * it is bad to have bogus pointers in our menu list!!!
	 * (An astute student of C++ might note that in the header we declare the menubar before
	 * the Menus themselves, and so menus will be destroyed first. As a practical matter
	 * we could reverse their order, but that would be too subtle. Further, we might note
	 * that in destroying a menubar, it probably does not refer to any of its menus, and
	 * so their being bogus pointers during this process might lead to no ills, it is clearly
	 * inadvisable practice, and this code is meant to illustrate safe practices as much
	 * as it is meant to work).
	 */
#if		qMacToolkit 
	fMenuBar.RemoveMenu (&fSystemMenu);
#endif
	fMenuBar.RemoveMenu (&fFileMenu);
	fMenuBar.RemoveMenu (&fEditMenu);
	fMenuBar.RemoveMenu (&fFindMenu);
	fMenuBar.RemoveMenu (&fProjectMenu);
	fMenuBar.RemoveMenu (&fViewsMenu);
	fMenuBar.RemoveMenu (&fWindowsMenu);
#if		qDebug 
	fMenuBar.RemoveMenu (&fDebugMenu);
#endif
}

void	SchnauserApplication::DoSetupMenus ()
{
	/*
	 * Call the inherited DoSetupMenus to let the base class enable whatever items it is prepared
	 * to deal with, and then specifically enable the ones that we deal with here.
	 */
	Application::DoSetupMenus ();

	EnableCommand (eAboutApplication);
	EnableCommand (eNew);
	EnableCommand (eOpen);
	EnableCommand (eEditUserPreferences);
}

Boolean		SchnauserApplication::DoCommand (const CommandSelection& selection)
{
	/*
	 * Check which command was selected. If its one that we handle, then do so, and return True
	 * indicating that we handled it.
	 * Otherwise call our inherited method to deal with the command.
	 */
	switch (selection.GetCommandNumber ()) {
		case eAboutApplication: {
			DisplaySimpleAboutBox ();
			return (True);
		}
		break;

		case eNew: {
			SchnauserDocument*	theDoc	=	 new SchnauserDocument ();
			theDoc->DoNewState ();
			return (True);
		}
		break;

		case eOpen: {
			SchnauserDocument*	theDoc	=	 new SchnauserDocument ();
			Try {
				theDoc->Read ();
			}
			Catch () {
				// failed to open document, so delete it.
				theDoc->Close ();		// deletes it.
				_this_catch_->Raise ();		// propogate error so it gets displayed...
			}
			return (True);
		}
		break;

		case	eEditUserPreferences: {
			if (UserPreferences::DisplayDialog ()) {
				UserPreferences::Save ();
			}
			return (True);
		}
		break;

		default: {
			return (Application::DoCommand (selection));
		}
		break;
	}
	AssertNotReached (); return (True);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

