/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapEditorMenus.cc,v 1.2 1992/09/01 17:37:42 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapEditorMenus.cc,v $
 *		Revision 1.2  1992/09/01  17:37:42  sterling
 *		*** empty log message ***
 *
 *		Revision 1.3  1992/05/20  00:30:00  lewis
 *		Add eWriteXPM2.
 *
 *		Revision 1.1  92/04/29  12:38:43  12:38:43  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 */




#include	"Debug.hh"

#include	"StringMenuItem.hh"

#include	"CommandNumbers.hh"

#include	"PixelMapEditorMenus.hh"







/*
 ********************************************************************************
 ********************* PixelMapEditorCommandNamesBuilder ************************
 ********************************************************************************
 */
PixelMapEditorCommandNamesBuilder::PixelMapEditorCommandNamesBuilder ()
{
	/*
	 * Add any command name to the command name table (database) that we define for
	 * our application.
	 */

#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About PixelMapEditor"),
		CmdNameTablePair (eWriteCPlusPlus, "Write PixelMap as C++ Source"),
		CmdNameTablePair (eWriteXPM, "Write PixelMap as XPM File"),
		CmdNameTablePair (eWriteXPM2, "Write PixelMap as XPM2 File"),
	};
	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));
}





/*
 ********************************************************************************
 ****************************** PixelMapEditorFileMenu **************************
 ********************************************************************************
 */
PixelMapEditorFileMenu::PixelMapEditorFileMenu ():
	DefaultFileMenu ()
{
	ReorderMenuItem (AddStringMenuItem (eWriteCPlusPlus, True), GetMenuItemByCommand (CommandHandler::eSaveAs));
	ReorderMenuItem (AddStringMenuItem (eWriteXPM, True), GetMenuItemByCommand (eWriteCPlusPlus));
	ReorderMenuItem (AddStringMenuItem (eWriteXPM2, True), GetMenuItemByCommand (eWriteXPM));
}




/*
 ********************************************************************************
 *************************** PixelMapEditorEditMenu *****************************
 ********************************************************************************
 */
PixelMapEditorEditMenu::PixelMapEditorEditMenu ():
	DefaultEditMenu ()
{
}






// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
