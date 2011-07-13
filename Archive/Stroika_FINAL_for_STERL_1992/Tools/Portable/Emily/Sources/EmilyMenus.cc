/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EmilyMenus.cc,v 1.5 1992/09/01 17:25:44 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyMenus.cc,v $
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/03  04:57:27  lewis
 *		*** empty log message ***
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.10  1992/05/19  11:35:45  sterling
 *		hi
 *
 *		Revision 1.9  92/05/13  18:47:03  18:47:03  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.2  1992/03/06  21:52:58  sterling
 *		motif
 *
 *
 *
 *
 *
 */

#include "Font.hh"
#include "String.hh"
#include "MenuOwner.hh"
#include "StringMenuItem.hh"

#include "EmilyMenus.hh"
#include "CommandNumbers.hh"



CommandNamesBuilder::CommandNamesBuilder ()
{
	const	CmdNameTablePair	kCommandNames[] = {
		CmdNameTablePair (CommandHandler::eAboutApplication, "About Emily"),
		CmdNameTablePair (eSetPreferences, "Set Preferences"),
		CmdNameTablePair (eJustifyLeft, "Left Justification"),
		CmdNameTablePair (eJustifyCenter, "Center Justification"),
		CmdNameTablePair (eJustifyRight, "Right Justification"),
		CmdNameTablePair (eJustifyFull, "Full Justification"),
		CmdNameTablePair (eSizeToFit, "Size to Fit"),
		CmdNameTablePair (eSetClassInfo, "Set Class Info"),
		CmdNameTablePair (eSetItemInfo, "Set Item Info"),
		CmdNameTablePair (eEnabled, "Enable Item"),
		CmdNameTablePair (eGroupItems, "Group"),
		CmdNameTablePair (eUngroupItems, "Ungroup"),
		CmdNameTablePair (eAlignLeft, "Align Left"),
		CmdNameTablePair (eAlignCenterH, "Align Horizontal Centers"),
		CmdNameTablePair (eAlignRight, "Align Right"),
		CmdNameTablePair (eAlignTop, "Align Top"),
		CmdNameTablePair (eAlignCenterV, "Align Vertical Centers"),
		CmdNameTablePair (eAlignBottom, "Align Bottom"),
		CmdNameTablePair (eSpaceHorizontally, "Space Horizontally"),
		CmdNameTablePair (eUniformSpacesHorizontally, "Uniform"),
		CmdNameTablePair (eUserSpecifyHorizontalSpacing, "Space Horizontally"),
		CmdNameTablePair (eSpaceVertically, "Space Vertically"),
		CmdNameTablePair (eUniformSpacesVertically, "Uniform"),
		CmdNameTablePair (eUserSpecifyVerticalSpacing, "Space Vertically"),
		CmdNameTablePair (eConstrainLeft, "Attach Left"),
		CmdNameTablePair (eConstrainTop, "Attach Top"),
		CmdNameTablePair (eConstraintWidth, "Attach Width"),
		CmdNameTablePair (eConstrainHeight, "Attach Height"),
		CmdNameTablePair (eFontSize, "Set Font Size"),
		CmdNameTablePair (eFontStyle, "Set Font Style"),
		CmdNameTablePair (eInheritFont, "Inherit Font"),
		CmdNameTablePair (eFont, "Set Font"),
		CmdNameTablePair (eUserSpecifyFontSize, "Set Font"),
		CmdNameTablePair (ePlainFontCommand, "Plain Style"),
		CmdNameTablePair (eBoldFontCommand, "Bold Style"),
		CmdNameTablePair (eItalicFontCommand, "Italic Style"),
		CmdNameTablePair (eUnderlineFontCommand, "Underline Style"),
		CmdNameTablePair (eOutlineFontCommand, "Outline Style"),
		CmdNameTablePair (eShadowFontCommand, "Shadow Style"),
		CmdNameTablePair (eMacUI, "Macintosh"),
		CmdNameTablePair (eMotifUI, "Motif"),
		CmdNameTablePair (eWindowsGUI, "Windows"),
		CmdNameTablePair (eEnglish, "English"),
		CmdNameTablePair (eFrench, "French"),
		CmdNameTablePair (eGerman, "German"),
		CmdNameTablePair (eItalian, "Italian"),
		CmdNameTablePair (eSpanish, "Spanish"),
		CmdNameTablePair (eJapanese, "Japanese"),
		CmdNameTablePair (eCustomizeOnly, "Customize Only"),
		CmdNameTablePair (eDeleteCustomization, "Delete Customization"),
		CmdNameTablePair (eAutoCustomize, "Auto Customize"),
		CmdNameTablePair (eReorderFocusItems, "Reorder Focus Items"),
	};

	CommandNameTable::Get ().AddPairs (kCommandNames, sizeof (kCommandNames)/sizeof (kCommandNames[0]));

	MenuOwner::GetAccelerators ().Enter (eGroupItems, String ("%G"));
	MenuOwner::GetAccelerators ().Enter (eSetItemInfo, String ("%I"));
	MenuOwner::GetAccelerators ().Enter (eSizeToFit, String ("%F"));
}




EmilyFileMenu::EmilyFileMenu ()
{
	AbstractStringMenuItem* setPreferences = AddStringMenuItem (eSetPreferences, True);
	ReorderMenuItem (setPreferences, GetMenuItemByCommand (CommandHandler::eRevertToSaved));
#if 0
	ForEachS (MenuItemPtr, it, MakeMenuItemIterator ()) {
		gDebugStream << " index = " << it.CurrentIndex () << " current = " << it.Current ()->GetName () << newline;
	}
	ForEachS (MenuItemPtr, it1, MakeMenuItemIterator (eSequenceBackward)) {
		gDebugStream << " index = " << it1.CurrentIndex () << " current = " << it1.Current ()->GetName () << newline;
	}
#endif
}

EmilyEditMenu::EmilyEditMenu ()
{
	AddStringMenuItem (CommandHandler::eDuplicate);
	AddSeparatorMenuItem ();
	AddStringMenuItem (eSizeToFit);
	AddStringMenuItem (eSetItemInfo, True);
	AddStringMenuItem (eSetClassInfo, True);
	AddSeparatorMenuItem ();
	AddStringMenuItem (eReorderFocusItems, True);
}

EmilyFormatMenu::EmilyFormatMenu ()
{
	AddStringMenuItem (eInheritFont);
	AddSeparatorMenuItem ();
	AddCheckBoxMenuItem (eEnabled);
	AddSeparatorMenuItem ();
	AddStringMenuItem (eGroupItems);
	AddStringMenuItem (eUngroupItems);
	AddSeparatorMenuItem ();

	fAlignmentMenu.AddStringMenuItem (eAlignLeft);
	fAlignmentMenu.AddStringMenuItem (eAlignCenterH);
	fAlignmentMenu.AddStringMenuItem (eAlignRight);
	fAlignmentMenu.AddStringMenuItem (eAlignTop);
	fAlignmentMenu.AddStringMenuItem (eAlignCenterV);
	fAlignmentMenu.AddStringMenuItem (eAlignBottom);
	AddCascadeMenuItem (&fAlignmentMenu, "Align");

	AddCascadeMenuItem (&fHorizontalSpacingMenu, "Space Horizontally");
	AddCascadeMenuItem (&fVerticalSpacingMenu, "Space Vertically");

	fAttachmentMenu.AddCheckBoxMenuItem (eConstrainLeft);
	fAttachmentMenu.AddCheckBoxMenuItem (eConstrainTop);
	fAttachmentMenu.AddCheckBoxMenuItem (eConstraintWidth);
	fAttachmentMenu.AddCheckBoxMenuItem (eConstrainHeight);
	AddCascadeMenuItem (&fAttachmentMenu, "Attachments");
}

EmilyFormatMenu::~EmilyFormatMenu ()
{
	DeleteAllMenuItems ();
}


EmilyCustomizeMenu::EmilyCustomizeMenu ()
{
	AddStringMenuItem (eDeleteCustomization, True);
	AddStringMenuItem (eAutoCustomize);
	AddCheckBoxMenuItem (eCustomizeOnly);
	AddSeparatorMenuItem ();

	fLanguagesMenu.AddCheckBoxMenuItem (eEnglish);
	fLanguagesMenu.AddCheckBoxMenuItem (eFrench);
	fLanguagesMenu.AddCheckBoxMenuItem (eGerman);
	fLanguagesMenu.AddCheckBoxMenuItem (eItalian);
	fLanguagesMenu.AddCheckBoxMenuItem (eSpanish);
	fLanguagesMenu.AddCheckBoxMenuItem (eJapanese);
	AddCascadeMenuItem (&fLanguagesMenu, "Language");

	fGUIMenu.AddCheckBoxMenuItem (eMacUI);
	fGUIMenu.AddCheckBoxMenuItem (eMotifUI);
	fGUIMenu.AddCheckBoxMenuItem (eWindowsGUI);
	AddCascadeMenuItem (&fGUIMenu, "GUI");
}

EmilyCustomizeMenu::~EmilyCustomizeMenu ()
{
	DeleteAllMenuItems ();
}

String	EmilyCustomizeMenu::DefaultName ()
{
	const String kMenuTitle	= String (String::eReadOnly, "Customize");
	return (kMenuTitle);
}

