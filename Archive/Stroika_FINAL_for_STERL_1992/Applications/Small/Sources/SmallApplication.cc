/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SmallApplication.cc,v 1.3 1992/09/08 17:38:33 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SmallApplication.cc,v $
 *		Revision 1.3  1992/09/08  17:38:33  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  17:44:49  sterling
 *		*** empty log message ***
 *
 *		Revision 1.11  1992/03/05  20:52:43  lewis
 *		Convert to new Menubar scheme.
 *
 *		Revision 1.10  1992/02/29  02:24:56  lewis
 *		Override app dtor to remove menus from menubar.
 *
 *
 *
 */

#include	"Debug.hh"

#include	"SimpleAboutBox.hh"

#include	"SmallDocument.hh"

#include	"SmallApplication.hh"







/*
 ********************************************************************************
 ************************ SmallCommandNamesBuilder ******************************
 ********************************************************************************
 */
SmallCommandNamesBuilder::SmallCommandNamesBuilder ()
{
	/*
	 * Add any command name to the command name table (database) that we define for
	 * our application.
	 */

#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About Small"),
	};
	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));
}






/*
 ********************************************************************************
 ****************************** SmallApplication ********************************
 ********************************************************************************
 */
SmallApplication*	SmallApplication::sThe	=	Nil;

SmallApplication::SmallApplication (int argc, const char* argv[]):
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

SmallApplication::~SmallApplication ()
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

void	SmallApplication::DoSetupMenus ()
{
	/*
	 * Call the inherited DoSetupMenus to let the base class enable whatever items it is prepared
	 * to deal with, and then specifically enable the ones that we deal with here.
	 */
	Application::DoSetupMenus ();

	EnableCommand (eAboutApplication, True);
	EnableCommand (eNew, True);
}

extern	void	DumpHeap (size_t cutOff);
Boolean		SmallApplication::DoCommand (const CommandSelection& selection)
{
	/*
	 * Check which command was selected. If its one that we handle, then do so, and return True
	 * indicating that we handled it.
	 * Otherwise call our inherited method to deal with the command.
	 */
	switch (selection.GetCommandNumber ()) {
		case eAboutApplication:
			DisplaySimpleAboutBox ();
			return (True);

		case eNew:	
			{
				SmallDocument*	theDoc	=	 new SmallDocument ();
				theDoc->DoNewState ();
			}
			return (True);

		default:
			return (Application::DoCommand (selection));
	}
	AssertNotReached (); return (True);
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

