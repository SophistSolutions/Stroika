/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorApplication.cc,v 1.4 1992/09/08 16:52:13 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditorApplication.cc,v $
 *		Revision 1.4  1992/09/08  16:52:13  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:37:42  sterling
 *		*** empty log message ***
 *
 *		Revision 1.3  92/04/29  12:37:25  12:37:25  lewis (Lewis Pringle)
 *		Add open/ document.
 *
 *		Revision 1.1  92/04/20  11:06:02  11:06:02  lewis (Lewis Pringle)
 *		Initial revision
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"SimpleAboutBox.hh"

#include	"CommandNumbers.hh"
#include	"PixelMapEditorDocument.hh"

#include	"PixelMapEditorApplication.hh"






/*
 ********************************************************************************
 *************************** PixelMapEditorApplication **************************
 ********************************************************************************
 */
PixelMapEditorApplication*	PixelMapEditorApplication::sThe	=	Nil;

PixelMapEditorApplication::PixelMapEditorApplication (int argc, const char* argv[]):
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
#if		qDebug 
	fMenuBar.AddMenu (&fDebugMenu, fDebugMenu.DefaultName ());
#endif

	SetMenuBar (&fMenuBar);
}

PixelMapEditorApplication::~PixelMapEditorApplication ()
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
#if		qDebug 
	fMenuBar.RemoveMenu (&fDebugMenu);
#endif
}

void	PixelMapEditorApplication::DoSetupMenus ()
{
	/*
	 * Call the inherited DoSetupMenus to let the base class enable whatever items it is prepared
	 * to deal with, and then specifically enable the ones that we deal with here.
	 */
	Application::DoSetupMenus ();

	EnableCommand (eAboutApplication, True);
	EnableCommand (eNew, True);
	EnableCommand (eOpen, True);
}

Boolean		PixelMapEditorApplication::DoCommand (const CommandSelection& selection)
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
			PixelMapEditorDocument*	theDoc	=	 new PixelMapEditorDocument ();
			theDoc->DoNewState ();
			return (True);
		}
		break;

		case eOpen: {
			PixelMapEditorDocument*	theDoc	=	 new PixelMapEditorDocument ();
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

