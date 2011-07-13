/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEditorWindow.cc,v 1.3 1992/09/08 18:02:17 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditorWindow.cc,v $
 *		Revision 1.3  1992/09/08  18:02:17  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.2  1992/09/01  17:55:46  sterling
 *		*** empty log message ***
 *
 *
 *
 */


#include "Debug.hh"
#include "Format.hh"
#include "StreamUtils.hh"

#include "Application.hh"
#include "TextEdit.hh"


#include "FontPicker.hh"
#include "ScrollableText.hh"
#include "UserCommand.hh"


#include "TextEditorDocument.hh"
#include "TextEditorWindow.hh"

/*
 ********************************************************************************
 ********************************* TextEditorWindow *****************************
 ********************************************************************************
 */
TextEditorWindow::TextEditorWindow (TextEditorDocument& myDocument):
	Window (),
	fTE (Nil)
{
	fScroller = new TextScroller (Scroller::kBuildDefaultSlider, Scroller::kBuildDefaultSlider);	
	SetMainViewAndTargets (fScroller, fScroller, fScroller);

	fTE = new TextEdit ();
	fTE->SetWordWrap (not TextEdit::kWordWrapOn);
	fTE->SetBorder (kZeroPoint);
	fTE->SetFocused (FocusItem::kFocused);
	fScroller->SetTextEdit (fTE);
	SetWindowController (&myDocument);
}		

TextEditorWindow::~TextEditorWindow ()
{
	SetMainViewAndTargets (Nil, Nil, Nil);
	fScroller->SetTextEdit (Nil);
	
	delete fScroller;
	delete fTE;
}


void		TextEditorWindow::DoSetupMenus ()
{
	Window::DoSetupMenus ();

	EnableCommand (eFont);
	EnableCommand (eFontSize);
//	EnableCommand (eUserSpecifyFontSize);

	EnableCommand (ePlainFontCommand);
	EnableCommand (eBoldFontCommand);
	EnableCommand (eItalicFontCommand);
	EnableCommand (eUnderlineFontCommand);
	EnableCommand (eOutlineFontCommand);
	EnableCommand (eShadowFontCommand);
}

Boolean		TextEditorWindow::DoCommand (const CommandSelection& selection)
{
	switch (selection.GetCommandNumber ()) {
		case eFontSize: 
			Application::Get ().PostCommand (new SetFontCommand (GetTE (), FontSize (strtol (selection.GetName ()))));
			return (True);
			
		case eFont:
			Application::Get ().PostCommand (new SetFontCommand (GetTE (), selection.GetName ()));
			return (True);

		case ePlainFontCommand:
		case eBoldFontCommand:
		case eItalicFontCommand:
		case eUnderlineFontCommand:
		case eOutlineFontCommand:
		case eShadowFontCommand:
			Application::Get ().PostCommand (new SetFontCommand (GetTE (), selection.GetCommandNumber ()));
			return (True);
		
		default:
			return (Window::DoCommand (selection));
	}
	AssertNotReached ();	return (False);
}

AbstractTextEdit&	TextEditorWindow::GetTE () const
{
	EnsureNotNil (fTE);
	return (*fTE);
}
