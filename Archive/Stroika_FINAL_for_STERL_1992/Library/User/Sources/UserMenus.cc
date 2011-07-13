/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/UserMenus.cc,v 1.3 1992/09/01 15:58:05 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: UserMenus.cc,v $
 *		Revision 1.3  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/14  20:02:17  lewis
 *		Rename Seperator->Separator.
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *
 *
 */

#include	"StreamUtils.hh"

#include	"Font.hh"

#include	"MenuOwner.hh"
#include	"CascadeMenuItem.hh"
#include	"SeparatorMenuItem.hh"
#include	"StringMenuItem.hh"

#include	"UserMenus.hh"






/*
 ********************************************************************************
 ***************************** UserCommandNamesBuilder **************************
 ********************************************************************************
 */
UserCommandNamesBuilder::UserCommandNamesBuilder ():
	FrameworkCommandNamesBuilder ()
{
	const	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (eJustifyLeft,  String (String::eReadOnly, "Left Justification")),
		CmdNameTablePair (eJustifyCenter,  String (String::eReadOnly, "Center Justification")),
		CmdNameTablePair (eJustifyRight,  String (String::eReadOnly, "Right Justification")),
		CmdNameTablePair (eJustifyFull,  String (String::eReadOnly, "Full Justification")),
		CmdNameTablePair (eFontSize,  String (String::eReadOnly, "Set Font Size")),
		CmdNameTablePair (eFontStyle,  String (String::eReadOnly, "Set Font Style")),
		CmdNameTablePair (eFont,  String (String::eReadOnly, "Set Font")),
		CmdNameTablePair (eUserSpecifyFontSize,  String (String::eReadOnly, "Set Font")),
		CmdNameTablePair (ePlainFontCommand,  String (String::eReadOnly, "Plain Style")),
		CmdNameTablePair (eBoldFontCommand,  String (String::eReadOnly, "Bold Style")),
		CmdNameTablePair (eItalicFontCommand,  String (String::eReadOnly, "Italic Style")),
		CmdNameTablePair (eUnderlineFontCommand,  String (String::eReadOnly, "Underline Style")),
		CmdNameTablePair (eOutlineFontCommand,  String (String::eReadOnly, "Outline Style")),
		CmdNameTablePair (eShadowFontCommand,  String (String::eReadOnly, "Shadow Style")),
	};

	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));
}






/*
 ********************************************************************************
 ***************************** DefaultFontMenu **********************************
 ********************************************************************************
 */
DefaultFontMenu::DefaultFontMenu ():
	Menu ()
{
	for (FontIterator it = FontIterator (); not it.Done (); it.Next ()) {
		AddStringMenuItem (eFont, it.Current ().GetName ());
	}
}

DefaultFontMenu::~DefaultFontMenu ()
{
	DeleteAllMenuItems ();
}

String	DefaultFontMenu::DefaultName ()
{
	const String kMenuTitle	=	String (String::eReadOnly, "Font");
	return (kMenuTitle);
}







/*
 ********************************************************************************
 ***************************** DefaultFontMenu **********************************
 ********************************************************************************
 */
DefaultFontSizeMenu::DefaultFontSizeMenu ():
	Menu ()
{
	AddStringMenuItem (eFontSize, "9");
	AddStringMenuItem (eFontSize, "10");
	AddStringMenuItem (eFontSize, "12");
	AddStringMenuItem (eFontSize, "16");
	AddStringMenuItem (eFontSize, "18");
	AddStringMenuItem (eFontSize, "24");
	AddStringMenuItem (eFontSize, "36");
	AddStringMenuItem (eFontSize, "72");
	(AddStringMenuItem (eUserSpecifyFontSize, "Other"))->SetExtended (True);
}

DefaultFontSizeMenu::~DefaultFontSizeMenu ()
{
	DeleteAllMenuItems ();
}

String	DefaultFontSizeMenu::DefaultName ()
{
	const String kMenuTitle	=	String (String::eReadOnly, "Font Size");
	return (kMenuTitle);
}






/*
 ********************************************************************************
 ************************** DefaultFontStyleMenu ********************************
 ********************************************************************************
 */
DefaultFontStyleMenu::DefaultFontStyleMenu ():
	Menu ()
{
	AddStringMenuItem (ePlainFontCommand);
	AddSeparatorMenuItem ();
	AddStringMenuItem (eBoldFontCommand);
	AddStringMenuItem (eItalicFontCommand);
	AddStringMenuItem (eUnderlineFontCommand);
	AddStringMenuItem (eOutlineFontCommand);
	AddStringMenuItem (eShadowFontCommand);
}

DefaultFontStyleMenu::~DefaultFontStyleMenu ()
{
	DeleteAllMenuItems ();
}

String	DefaultFontStyleMenu::DefaultName ()
{
	const String kMenuTitle	=	String (String::eReadOnly, "Font Style");
	return (kMenuTitle);
}







/*
 ********************************************************************************
 ************************** DefaultFontJustificationMenu ************************
 ********************************************************************************
 */
DefaultFontJustificationMenu::DefaultFontJustificationMenu ():
	Menu ()
{
	AddStringMenuItem (eJustifyLeft);
	AddStringMenuItem (eJustifyCenter);
	AddStringMenuItem (eJustifyRight);
	AddStringMenuItem (eJustifyFull);
}

DefaultFontJustificationMenu::~DefaultFontJustificationMenu ()
{
	DeleteAllMenuItems ();
}

String	DefaultFontJustificationMenu::DefaultName ()
{
	const String kMenuTitle	=	String (String::eReadOnly, "Font Justification");
	return (kMenuTitle);
}





/*
 ********************************************************************************
 *************************** DefaultFormatMenu **********************************
 ********************************************************************************
 */
DefaultFormatMenu::DefaultFormatMenu ():
	Menu (),
	fFontMenu (),
	fFontSizeMenu (),
	fFontStyleMenu (),
	fFontJustificationMenu ()
{
	AddCascadeMenuItem (&fFontMenu, fFontMenu.DefaultName ());
	AddCascadeMenuItem (&fFontSizeMenu, fFontSizeMenu.DefaultName ());
	AddCascadeMenuItem (&fFontStyleMenu, fFontStyleMenu.DefaultName ());
	AddCascadeMenuItem (&fFontJustificationMenu, fFontJustificationMenu.DefaultName ());
}

DefaultFormatMenu::~DefaultFormatMenu ()
{
	DeleteAllMenuItems ();
}

String	DefaultFormatMenu::DefaultName ()
{
	const String kMenuTitle	=	String (String::eReadOnly, "Format");
	return (kMenuTitle);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

