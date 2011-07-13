/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEditor.cc,v 1.3 1992/09/08 18:02:17 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditor.cc,v $
 *		Revision 1.3  1992/09/08  18:02:17  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  17:55:46  sterling
 *		*** empty log message ***
 *
 *		Revision 1.11  1992/03/16  16:16:21  sterling
 *		got rid of border on scrolled text
 *
 *		Revision 1.9  1992/03/06  04:01:05  lewis
 *		Sterl changed FileBasedDocuement to be a Saveable, and at the same time did the conversion
 *		of DoRead being virtual to DoRead calling virtual protected DoRead_ (same for write).
 *		We update this code to reflect those changes. (Questionable arg to FileBasedDocument DoWrite_ - tabCount???).
 *
 *		Revision 1.8  1992/03/06  03:15:07  sterling
 *		used new menu stuff, changed to new TextScroller
 *
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"MenuBar.hh"
#include	"SimpleAboutBox.hh"

#include	"TextEditor.hh"
#include	"TextEditorDocument.hh"








/*
 ********************************************************************************
 ************************** TextEditorCommandNamesBuilder ***********************
 ********************************************************************************
 */
TextEditorCommandNamesBuilder::TextEditorCommandNamesBuilder ()
{
#if		!qMPW_STATIC_ARRAY_WITH_GEN_INIT_CODE_GEN_BUG
	static	const
#endif
	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About TextEditor"),
	};

	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));
}







/*
 ********************************************************************************
 ****************************** TextEditorApplication ***************************
 ********************************************************************************
 */
TextEditorApplication*	TextEditorApplication::sThe	=	Nil;



TextEditorApplication::TextEditorApplication (int argc, const char* argv[]):
	Application (argc, argv),
	fMenuBar (),
	fSystemMenu (),
	fFileMenu (),
	fEditMenu (),
	fFormatMenu ()
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
	fMenuBar.AddMenu (&fFormatMenu, fFormatMenu.DefaultName ());
#if		qDebug 
	fMenuBar.AddMenu (&fDebugMenu, fDebugMenu.DefaultName ());
#endif
	SetMenuBar (&fMenuBar);
}

TextEditorApplication::~TextEditorApplication ()
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
	fMenuBar.RemoveMenu (&fFormatMenu);
#if		qDebug 
	fMenuBar.RemoveMenu (&fDebugMenu);
#endif
}

void	TextEditorApplication::DoSetupMenus ()
{
	Application::DoSetupMenus ();

	EnableCommand (eAboutApplication, True);
	EnableCommand (eNew, True);
	EnableCommand (eOpen, True);
}

Boolean		TextEditorApplication::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eAboutApplication: {
			DisplaySimpleAboutBox ();
			return (True);
		}
		break;
		case eNew:	{
			TextEditorDocument*	theDoc	=	 new TextEditorDocument ();
			theDoc->DoNewState ();
			return (True);
		}
		break;
		case	eOpen: {
			TextEditorDocument*	theDoc	=	 new TextEditorDocument ();
			Try {
				theDoc->Read ();
			}
			Catch () {
				// failed to open document, so delete it.
				theDoc->Close ();		// deletes it.
			}
			return (True);
		}
		break;
		default: {
			return (Application::DoCommand (selection));
		}
	}
	AssertNotReached (); return (True);
}









// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
