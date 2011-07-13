/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TicTacToeMenus.cc,v 1.3 1992/09/01 17:58:36 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TicTacToeMenus.cc,v $
 *		Revision 1.3  1992/09/01  17:58:36  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */




#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"MenuOwner.hh"

#include	"CommandNumbers.hh"

#include	"TicTacToeMenus.hh"





/*
 ********************************************************************************
 ******************************* TicTacToeNameBuilder ***************************
 ********************************************************************************
 */
TicTacToeNameBuilder::TicTacToeNameBuilder ():
	UserCommandNamesBuilder ()
{
	/*
	 * Add any command name to the command name table (database) that we define for
	 * our application.
	 */

#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About TicTacToe"),
		CmdNameTablePair (eSuggestMove, "Suggest Move"),
		CmdNameTablePair (eAutoPilot, "Finish Game"),
		CmdNameTablePair (eSound, "Use Sound"),
		CmdNameTablePair (eComputerOpponent, "Play Computer"),
	};
	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));

	MenuOwner::GetAccelerators ().Enter (eSuggestMove, String ("%S"));
}






/*
 ********************************************************************************
 ******************************* TicTacToeFileMenu ******************************
 ********************************************************************************
 */
TicTacToeFileMenu::TicTacToeFileMenu ():
	DefaultFileMenu ()
{
	RemoveAllMenuItems ();		// does this delete 'em???
	AddStringMenuItem (CommandHandler::eNew);
	AddSeparatorMenuItem ();
	AddStringMenuItem (CommandHandler::eQuit);
}




/*
 ********************************************************************************
 ******************************* TicTacToeEditMenu ******************************
 ********************************************************************************
 */
TicTacToeEditMenu::TicTacToeEditMenu ():
	DefaultEditMenu ()
{
}





/*
 ********************************************************************************
 ******************************* TicTacToeComputerMenu **************************
 ********************************************************************************
 */
TicTacToeComputerMenu::TicTacToeComputerMenu ():
	Menu ()
{
	AddCheckBoxMenuItem (eSound);
	AddSeparatorMenuItem ();
	AddStringMenuItem (eSuggestMove);
	AddCheckBoxMenuItem (eComputerOpponent);
	AddStringMenuItem (eAutoPilot);
}

String	TicTacToeComputerMenu::DefaultName ()
{
	return ("Computer");
}





// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

