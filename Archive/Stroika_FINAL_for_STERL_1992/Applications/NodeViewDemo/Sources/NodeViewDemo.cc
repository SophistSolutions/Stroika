/*
 * $Header: /fuji/lewis/RCS/NodeViewDemo.cc,v 1.4 1992/09/08 16:48:46 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: NodeViewDemo.cc,v $
 *		Revision 1.4  1992/09/08  16:48:46  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:31:57  sterling
 *		*** empty log message ***
 *
 *
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Document.hh"
#include	"MenuBar.hh"

#include	"SimpleAboutBox.hh"

#include	"NodeViewCommands.hh"
#include	"NodeViewDemo.hh"
#include	"NodeViewDocument.hh"






NodeViewCommandNamesBuilder::NodeViewCommandNamesBuilder ()
{
	/*
	 * Add any command name to the command name table (database) that we define for
	 * our application.
	 */

#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About NodeViewDemo"),
		CmdNameTablePair (eConfigureNodes, "Configure Nodes"),
		CmdNameTablePair (eAddNode, "Add Node"),
		CmdNameTablePair (eRemoveNode, "Remove Nodes"),
		CmdNameTablePair (eShiftLeft, "Shift Left"),
		CmdNameTablePair (eShiftRight, "Shift Right"),
	};
	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));
}


/*
 ********************************************************************************
 *************************************** NodeViewDemo ***************************
 ********************************************************************************
 */
NodeViewDemo*	NodeViewDemo::sThe	=	Nil;

NodeViewDemo::NodeViewDemo (int argc, const char* argv []) :
	Application (argc, argv),
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

#if		qMacToolkit 
	fMenuBar.AddMenu (&fSystemMenu, fSystemMenu.DefaultName ());
#endif
	fMenuBar.AddMenu (&fFileMenu, fFileMenu.DefaultName ());
	
	fEditMenu.AddSeparatorMenuItem ();
	fEditMenu.AddStringMenuItem (eConfigureNodes);
	fEditMenu.AddStringMenuItem (eAddNode);
	fEditMenu.AddStringMenuItem (eRemoveNode);
	fEditMenu.AddStringMenuItem (eShiftLeft);
	fEditMenu.AddStringMenuItem (eShiftRight);
	
	fMenuBar.AddMenu (&fEditMenu, fEditMenu.DefaultName ());
#if		qDebug 
	fMenuBar.AddMenu (&fDebugMenu, fDebugMenu.DefaultName ());
#endif

	SetMenuBar (&fMenuBar);
}

NodeViewDemo::~NodeViewDemo ()
{
	SetMenuBar (Nil);
	
#if		qMacToolkit 
	fMenuBar.RemoveMenu (&fSystemMenu);
#endif
	fMenuBar.RemoveMenu (&fFileMenu);
	fMenuBar.RemoveMenu (&fEditMenu);
#if		qDebug 
	fMenuBar.RemoveMenu (&fDebugMenu);
#endif
}

void	NodeViewDemo::DoSetupMenus ()
{
	Application::DoSetupMenus ();

	EnableCommand (eAboutApplication, True);
	EnableCommand (eNew, True);
	EnableCommand (eOpen, True);
}

Boolean		NodeViewDemo::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eAboutApplication:
			DisplaySimpleAboutBox ();
			return (True);

		case eNew:
			{
				NodeViewDocument*	theDoc	=	 new NodeViewDocument ();
				theDoc->DoNewState ();
			}
			return (True);

		case	eOpen: 
			{
				NodeViewDocument*	theDoc	=	 new NodeViewDocument ();
				Try {
					theDoc->Read ();
				}
				Catch () {
					// failed to open document, so delete it.
					theDoc->Close ();		// deletes it.
				}
			}
			return (True);
			
		default:
			return (Application::DoCommand (selection));
	}
	AssertNotReached (); return (True);
}





// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***
