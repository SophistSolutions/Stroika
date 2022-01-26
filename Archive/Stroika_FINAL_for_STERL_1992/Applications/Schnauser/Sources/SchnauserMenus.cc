/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SchnauserMenus.cc,v 1.4 1992/09/01 17:42:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SchnauserMenus.cc,v $
 *		Revision 1.4  1992/09/01  17:42:50  sterling
 *		*** empty log message ***
 *
 *
 */




#include	"Debug.hh"

#include	"StringMenuItem.hh"

#include	"CommandNumbers.hh"

#include	"SchnauserMenus.hh"







/*
 ********************************************************************************
 ************************** SchnauserCommandNamesBuilder ************************
 ********************************************************************************
 */
SchnauserCommandNamesBuilder::SchnauserCommandNamesBuilder ()
{
	/*
	 * Add any command name to the command name table (database) that we define for
	 * our application.
	 */

#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About Schnauser"),
		CmdNameTablePair (CommandHandler::eNew, "New Project"),
		CmdNameTablePair (CommandHandler::eOpen, "Open Project"),

		CmdNameTablePair (eEditUserPreferences, "Edit Preferences"),

		CmdNameTablePair (eQueryFind, "Find"),
		CmdNameTablePair (eFindAgain, "Find Again"),
		CmdNameTablePair (eFindSelection, "Find Selection"),
		CmdNameTablePair (eDisplaySelection, "Display Selection"),
		CmdNameTablePair (eQueryReplace, "Replace"),
		CmdNameTablePair (eReplaceAgain, "Replace Again"),
		CmdNameTablePair (eGotoLine, "Goto Line"),

		CmdNameTablePair (eNewFilesView, "New Files View"),
		CmdNameTablePair (eNewClassesView, "New Classes View"),
		CmdNameTablePair (eNewFunctionsView, "New Functions View"),

		CmdNameTablePair (eOpenView, "Open View"),

		CmdNameTablePair (eViewViewsByName, "View Views By Name"),
		CmdNameTablePair (eViewViewsByIcon, "View Views By Icon"),

	};
	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));
}





/*
 ********************************************************************************
 ****************************** SchnauserFileMenu *******************************
 ********************************************************************************
 */
SchnauserFileMenu::SchnauserFileMenu ():
	DefaultFileMenu ()
{
	// This seems a bizarre way of saying insert prefs/separator after eRevertToSaved???
	AbstractStringMenuItem* preferences = AddStringMenuItem (eEditUserPreferences, True);
	ReorderMenuItem (preferences, GetMenuItemByCommand (CommandHandler::eRevertToSaved));
// MUST ADD RETURN TYPE TO AddSeperatorMenuItem () TO GET THIS TO WORK!!!
//ReorderMenuItem (AddSeperatorMenuItem (), GetMenuItemByCommand (CommandHandler::eRevertToSaved));
}




/*
 ********************************************************************************
 *************************** SchnauserEditMenu **********************************
 ********************************************************************************
 */
SchnauserEditMenu::SchnauserEditMenu ():
	DefaultEditMenu ()
{
}



/*
 ********************************************************************************
 ***************************** SchnauserFindMenu ********************************
 ********************************************************************************
 */

SchnauserFindMenu::SchnauserFindMenu ():
	Menu ()
{
	AddStringMenuItem (eQueryFind, True);
	AddStringMenuItem (eFindAgain);
	AddStringMenuItem (eFindSelection);
	AddSeperatorMenuItem ();
	AddStringMenuItem (eDisplaySelection);
	AddSeperatorMenuItem ();
	AddStringMenuItem (eQueryReplace, True);
	AddStringMenuItem (eReplaceAgain);
	AddSeperatorMenuItem ();
	AddStringMenuItem (eGotoLine);
}

String	SchnauserFindMenu::DefaultName ()
{
	return ("Find");
}



/*
 ********************************************************************************
 *************************** SchnauserProjectMenu *******************************
 ********************************************************************************
 */

SchnauserProjectMenu::SchnauserProjectMenu ():
	Menu ()
{
}

String	SchnauserProjectMenu::DefaultName ()
{
	return ("Project");
}




/*
 ********************************************************************************
 ***************************** SchnauserViewsMenu *******************************
 ********************************************************************************
 */

SchnauserViewsMenu::SchnauserViewsMenu ():
	Menu ()
{
	AddStringMenuItem (eNewFilesView);
	AddStringMenuItem (eNewClassesView);
	AddStringMenuItem (eNewFunctionsView);
	AddSeperatorMenuItem ();
	AddStringMenuItem (eOpenView);
	AddSeperatorMenuItem ();
	AddCheckBoxMenuItem (eViewViewsByName, "By Name");
	AddCheckBoxMenuItem (eViewViewsByIcon, "By Icon");
}

String	SchnauserViewsMenu::DefaultName ()
{
	return ("Views");
}




/*
 ********************************************************************************
 ***************************** SchnauserWindowsMenu *****************************
 ********************************************************************************
 */

SchnauserWindowsMenu::SchnauserWindowsMenu ():
	Menu ()
{
}

String	SchnauserWindowsMenu::DefaultName ()
{
	return ("Windows");
}







// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
