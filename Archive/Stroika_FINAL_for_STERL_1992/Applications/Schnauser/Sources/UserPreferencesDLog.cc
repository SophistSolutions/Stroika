/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/UserPreferencesDLog.cc,v 1.1 1992/06/30 04:00:31 lewis Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: UserPreferencesDLog.cc,v $
 *		Revision 1.1  1992/06/30  04:00:31  lewis
 *		Initial revision
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "UserPreferencesDLog.hh"


UserPreferencesDLogX::UserPreferencesDLogX () :
	fField1 (),
	fField2 (kEmptyString),
	fField7 (kEmptyString),
	fField14 (kEmptyString),
	fOnStartup ()
{
#if   qMacGUI
	BuildForMacGUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

UserPreferencesDLogX::~UserPreferencesDLogX ()
{
	RemoveFocus (&fField2TabLoop);
	RemoveFocus (&fField7TabLoop);

	RemoveSubView (&fField1);
	RemoveSubView (&fField2);
		fField2TabLoop.RemoveFocus (&fField4);

		fField2.RemoveSubView (&fField3);
		fField2.RemoveSubView (&fField4);
		fField2.RemoveSubView (&fField5);
		fField2.RemoveSubView (&fField6);
	RemoveSubView (&fField7);
		fField7TabLoop.RemoveFocus (&fField11TabLoop);

		fField7.RemoveSubView (&fField8);
		fField7.RemoveSubView (&fField9);
		fField7.RemoveSubView (&fField10);
		fField7.RemoveSubView (&fField11);
			fField11TabLoop.RemoveFocus (&fField13);

			fField11.RemoveSubView (&fField12);
			fField11.RemoveSubView (&fField13);
	RemoveSubView (&fField14);
		fField14.RemoveSubView (&fField15);
		fField14.RemoveSubView (&fField16);
		fField14.RemoveSubView (&fField17);
	RemoveSubView (&fOnStartup);
}

#if   qMacGUI

void	UserPreferencesDLogX::BuildForMacGUI ()
{
	SetSize (Point (359, 413), eNoUpdate);

	fField1.SetExtent (2, 2, 14, 413, eNoUpdate);
	fField1.SetFont (&kApplicationFont);
	fField1.SetText ("User Preferences");
	fField1.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fField1);

	fField2.SetExtent (71, 16, 36, 240, eNoUpdate);
	fField2.SetHelp ("Write out checkpoint files every N keystrokes as a backup in case of crashes, or inadvertant edits.");
	fField2.SetFont (&kApplicationFont);
	fField2.SetLabel ("Checkpoint", eNoUpdate);
		fField3.SetExtent (18, 173, 12, 54, eNoUpdate);
		fField3.SetText ("keystrokes.");
		fField2.AddSubView (&fField3);

		fField4.SetExtent (15, 139, 15, 26, eNoUpdate);
		fField4.SetController (this);
		fField4.SetMaxValue (10000);
		fField4.SetMinValue (1);
		fField2.AddSubView (&fField4);

		fField5.SetExtent (18, 103, 12, 28, eNoUpdate);
		fField5.SetText ("every");
		fField2.AddSubView (&fField5);

		fField6.SetLabel ("Checkpoint", eNoUpdate);
		fField6.SetExtent (17, 7, 15, 92, eNoUpdate);
		fField6.SetController (this);
		fField2.AddSubView (&fField6);

		fField2TabLoop.AddFocus (&fField4);
	AddSubView (&fField2);

	fField7.SetExtent (194, 16, 114, 351, eNoUpdate);
	fField7.SetFont (&kApplicationFont);
	fField7.SetLabel ("Syntax Directed Editing", eNoUpdate);
		fField8.SetLabel ("Bold Global Symbol Definitions", eNoUpdate);
		fField8.SetExtent (91, 47, 15, 217, eNoUpdate);
		fField8.SetHelp ("Show the defining use of a global symbol in Bold, so that it stands out more.");
		fField8.SetController (this);
		fField7.AddSubView (&fField8);

		fField9.SetLabel ("Bold Keywords", eNoUpdate);
		fField9.SetExtent (69, 47, 15, 117, eNoUpdate);
		fField9.SetHelp ("Show C++ keyords in bold - I add this option since Lightspeed Pascal did- but it is probably stupid.");
		fField9.SetController (this);
		fField7.AddSubView (&fField9);

		fField10.SetLabel ("Full Syntax Directed Editing", eNoUpdate);
		fField10.SetExtent (21, 14, 15, 199, eNoUpdate);
		fField10.SetHelp ("Schauser will automatically format your program text, and display unsyntactic portions in an ugly outline font.");
		fField10.SetController (this);
		fField7.AddSubView (&fField10);

		fField11.SetExtent (45, 47, 19, 69, eNoUpdate);
		fField11.SetHelp ("Indent each lexical level the given number of spaces.");
			fField12.SetExtent (3, 3, 12, 28, eNoUpdate);
			fField12.SetText ("Indent");
			fField11.AddSubView (&fField12);

			fField13.SetExtent (2, 45, 15, 21, eNoUpdate);
			fField13.SetController (this);
			fField13.SetMaxValue (8);
			fField13.SetMinValue (1);
			fField11.AddSubView (&fField13);

			fField11TabLoop.AddFocus (&fField13);
		fField7.AddSubView (&fField11);

		fField7TabLoop.AddFocus (&fField11TabLoop);
	AddSubView (&fField7);

	fField14.SetExtent (121, 16, 61, 333, eNoUpdate);
	fField14.SetHelp ("Allow Schanuser to reparse code in the background (otherwise it will be done under user control - on demand). You can adjust how much of the CPU time is devoted to parsing. Use less on a slower computer, and more on a faster one.");
	fField14.SetFont (&kApplicationFont);
	fField14.SetLabel ("Parsing", eNoUpdate);
		fField15.SetExtent (12, 236, 39, 78, eNoUpdate);
		fField15.SetSliderController (this);
		fField14.AddSubView (&fField15);

		fField16.SetExtent (22, 196, 12, 38, eNoUpdate);
		fField16.SetText ("Priority");
		fField14.AddSubView (&fField16);

		fField17.SetLabel ("Parse in background", eNoUpdate);
		fField17.SetExtent (19, 9, 15, 153, eNoUpdate);
		fField17.SetController (this);
		fField14.AddSubView (&fField17);

	AddSubView (&fField14);

	fOnStartup.SetExtent (46, 16, 18, 224, eNoUpdate);
	fOnStartup.SetHelp ("When Schnauser starts up, should it open a new project, prompt for an old project, or open the projects that were open last time you ran.");
	fOnStartup.SetBorder (0, 0, eNoUpdate);
	fOnStartup.SetMargin (0, 0, eNoUpdate);
	fOnStartup.SetFont (&kApplicationFont);
	fOnStartup.AppendItem ("Open Nothing");
	fOnStartup.AppendItem ("New Project");
	fOnStartup.AppendItem ("Open Old Project");
	fOnStartup.AppendItem ("Open Saved Projects");
	fOnStartup.SetCurrentItem (1);
	fOnStartup.SetLabel ("On Startup:");
	fOnStartup.SetController (this);
	AddSubView (&fOnStartup);

	AddFocus (&fField2TabLoop);
	AddFocus (&fField7TabLoop);
}

#else

void	UserPreferencesDLogX::BuildForUnknownGUI ();
{
	SetSize (Point (359, 413), eNoUpdate);

	fField1.SetExtent (2, 2, 14, 413, eNoUpdate);
	fField1.SetFont (&kApplicationFont);
	fField1.SetText ("User Preferences");
	fField1.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fField1);

	fField2.SetExtent (71, 16, 36, 240, eNoUpdate);
	fField2.SetHelp ("Write out checkpoint files every N keystrokes as a backup in case of crashes, or inadvertant edits.");
	fField2.SetFont (&kApplicationFont);
	fField2.SetLabel ("Checkpoint", eNoUpdate);
		fField3.SetExtent (18, 173, 12, 54, eNoUpdate);
		fField3.SetText ("keystrokes.");
		fField2.AddSubView (&fField3);

		fField4.SetExtent (15, 139, 15, 26, eNoUpdate);
		fField4.SetController (this);
		fField4.SetMaxValue (10000);
		fField4.SetMinValue (1);
		fField2.AddSubView (&fField4);

		fField5.SetExtent (18, 103, 12, 28, eNoUpdate);
		fField5.SetText ("every");
		fField2.AddSubView (&fField5);

		fField6.SetLabel ("Checkpoint", eNoUpdate);
		fField6.SetExtent (17, 7, 15, 92, eNoUpdate);
		fField6.SetController (this);
		fField2.AddSubView (&fField6);

		fField2TabLoop.AddFocus (&fField4);
	AddSubView (&fField2);

	fField7.SetExtent (194, 16, 114, 351, eNoUpdate);
	fField7.SetFont (&kApplicationFont);
	fField7.SetLabel ("Syntax Directed Editing", eNoUpdate);
		fField8.SetLabel ("Bold Global Symbol Definitions", eNoUpdate);
		fField8.SetExtent (91, 47, 15, 217, eNoUpdate);
		fField8.SetHelp ("Show the defining use of a global symbol in Bold, so that it stands out more.");
		fField8.SetController (this);
		fField7.AddSubView (&fField8);

		fField9.SetLabel ("Bold Keywords", eNoUpdate);
		fField9.SetExtent (69, 47, 15, 117, eNoUpdate);
		fField9.SetHelp ("Show C++ keyords in bold - I add this option since Lightspeed Pascal did- but it is probably stupid.");
		fField9.SetController (this);
		fField7.AddSubView (&fField9);

		fField10.SetLabel ("Full Syntax Directed Editing", eNoUpdate);
		fField10.SetExtent (21, 14, 15, 199, eNoUpdate);
		fField10.SetHelp ("Schauser will automatically format your program text, and display unsyntactic portions in an ugly outline font.");
		fField10.SetController (this);
		fField7.AddSubView (&fField10);

		fField11.SetExtent (45, 47, 19, 69, eNoUpdate);
		fField11.SetHelp ("Indent each lexical level the given number of spaces.");
			fField12.SetExtent (3, 3, 12, 28, eNoUpdate);
			fField12.SetText ("Indent");
			fField11.AddSubView (&fField12);

			fField13.SetExtent (2, 45, 15, 21, eNoUpdate);
			fField13.SetController (this);
			fField13.SetMaxValue (8);
			fField13.SetMinValue (1);
			fField11.AddSubView (&fField13);

			fField11TabLoop.AddFocus (&fField13);
		fField7.AddSubView (&fField11);

		fField7TabLoop.AddFocus (&fField11TabLoop);
	AddSubView (&fField7);

	fField14.SetExtent (121, 16, 61, 333, eNoUpdate);
	fField14.SetHelp ("Allow Schanuser to reparse code in the background (otherwise it will be done under user control - on demand). You can adjust how much of the CPU time is devoted to parsing. Use less on a slower computer, and more on a faster one.");
	fField14.SetFont (&kApplicationFont);
	fField14.SetLabel ("Parsing", eNoUpdate);
		fField15.SetExtent (12, 236, 39, 78, eNoUpdate);
		fField15.SetSliderController (this);
		fField14.AddSubView (&fField15);

		fField16.SetExtent (22, 196, 12, 38, eNoUpdate);
		fField16.SetText ("Priority");
		fField14.AddSubView (&fField16);

		fField17.SetLabel ("Parse in background", eNoUpdate);
		fField17.SetExtent (19, 9, 15, 153, eNoUpdate);
		fField17.SetController (this);
		fField14.AddSubView (&fField17);

	AddSubView (&fField14);

	fOnStartup.SetExtent (46, 16, 18, 224, eNoUpdate);
	fOnStartup.SetHelp ("When Schnauser starts up, should it open a new project, prompt for an old project, or open the projects that were open last time you ran.");
	fOnStartup.SetBorder (0, 0, eNoUpdate);
	fOnStartup.SetMargin (0, 0, eNoUpdate);
	fOnStartup.SetFont (&kApplicationFont);
	fOnStartup.AppendItem ("Open Nothing");
	fOnStartup.AppendItem ("New Project");
	fOnStartup.AppendItem ("Open Old Project");
	fOnStartup.AppendItem ("Open Saved Projects");
	fOnStartup.SetCurrentItem (1);
	fOnStartup.SetLabel ("On Startup:");
	fOnStartup.SetController (this);
	AddSubView (&fOnStartup);

	AddFocus (&fField2TabLoop);
	AddFocus (&fField7TabLoop);
}

#endif /* GUI */

Point	UserPreferencesDLogX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacGUI
	return (Point (359, 413));
#else
	return (Point (359, 413));
#endif /* GUI */
}

void	UserPreferencesDLogX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfField1Size = fField1.GetSize ();
		fField1.SetSize (kOriginalfField1Size - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
