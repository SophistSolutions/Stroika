/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Tiny.cc,v 1.3 1992/09/08 18:05:28 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Tiny.cc,v $
 *		Revision 1.3  1992/09/08  18:05:28  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  18:00:14  sterling
 *		*** empty log message ***
 *
 *		Revision 1.11  1992/02/29  02:23:27  lewis
 *		Override app dtor to remove menus from menubar.
 *
 *
 */

#include	"Debug.hh"

#include	"Tiny.hh"



/*
 ********************************************************************************
 ************************* TinyCommandNamesBuilder ******************************
 ********************************************************************************
 */
TinyCommandNamesBuilder::TinyCommandNamesBuilder ()
{
	/*
	 * Add any command name to the command name table (database) that we define for
	 * our application.
	 */

#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About Tiny"),
	};
	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));
}


/*
 ********************************************************************************
 ********************************** TinyApplication *****************************
 ********************************************************************************
 */
TinyApplication*	TinyApplication::sThis		=	Nil;

TinyApplication::TinyApplication (int argc, const char* argv []):
	Application (argc, argv),
	fMenuBar (),
	fSystemMenu (),
	fFileMenu (),
	fEditMenu ()
#if		qDebug
	,fDebugMenu ()
#endif
{
	Assert (sThis == Nil);
	sThis = this;

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

TinyApplication::~TinyApplication ()
{
	SetMenuBar (Nil);

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

TinyApplication&	TinyApplication::Get ()
{
	EnsureNotNil (sThis);
	return (*sThis);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
