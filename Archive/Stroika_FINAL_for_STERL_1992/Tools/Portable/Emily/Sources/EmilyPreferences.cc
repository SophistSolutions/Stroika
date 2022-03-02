/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/EmilyPreferences.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: EmilyPreferences.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.19  1992/05/19  11:35:45  sterling
 *		hi
 *
 *		Revision 1.18  92/05/13  18:47:03  18:47:03  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.17  92/04/08  17:22:49  17:22:49  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 */

#include <ctype.h>

#include "KeyBoard.hh"
#include "PushButton.hh"
#include "PickList.hh"
#include "CSymbolText.hh"
#include "ScrollableText.hh"

#include "EmilyPreferences.hh"




FileSuffixText::FileSuffixText (TextController* c) :
	CSymbolText (c)
{
}


void	FileSuffixText::Validate ()
{
	Boolean	valid = True;
	String	text = GetText ();
	if (text.GetLength () == 0) {
		// let edittext worry about whether this is correct or not
	}
	else if (text[1] != '.') {
		valid = False;
	}
	else {
		for (register CollectionSize c = 2; (valid) and (c <= text.GetLength ()); c++) {
			if ((not (isalnum (text[2].GetAsciiCode ()))) and (text[2] != '_')) {
				valid = False;
			}
		}
	}
	
	if (not valid) {
		FocusItem::sFocusFailed.Raise ("Current text is not a valid file suffix.");
	}
	TextEdit::Validate ();
}

Boolean	FileSuffixText::ValidateKey (const KeyStroke& keyStroke)
{
    if ((keyStroke == KeyStroke::kDelete) or
	    (keyStroke == KeyStroke::kClear) or
	    (keyStroke == KeyStroke::kBackspace) or
	    (keyStroke == KeyStroke::kLeftArrow) or
	    (keyStroke == KeyStroke::kRightArrow)) {
		return (TextEdit::ValidateKey (keyStroke));
	}

	char c = keyStroke.GetCharacter ();
	if ((c == '_') or (c == ' ')) {
		return (TextEdit::ValidateKey (keyStroke));
	}
	else if (c == '.') {
		return (TextEdit::ValidateKey (keyStroke));
	}
	else {
		if ((GetSelection ().GetFrom () == 1) and (not (isalpha (c)))) {
			return (False);
		}
		else if (not (isalnum (c))) {
			return (False);
		}
	}
	return (TextEdit::ValidateKey (keyStroke));
}

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "EmilyPreferences.hh"


PreferencesInfoX::PreferencesInfoX () :
	fCompileOnce (),
	fFileHeaderGroup (kEmptyString),
	fGridSizeGroup (kEmptyString),
	fDataSuffixLabel (),
	fHeaderSuffixLabel (),
	fSourceSuffixLabel (),
	fSourceSuffix (),
	fHeaderSuffix (),
	fDataSuffix (),
	fSpacingGroup (kEmptyString)
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

PreferencesInfoX::~PreferencesInfoX ()
{
	RemoveFocus (&fGridSizeGroupTabLoop);
	RemoveFocus (&fSpacingGroupTabLoop);
	RemoveFocus (&fSourceSuffix);
	RemoveFocus (&fHeaderSuffix);
	RemoveFocus (&fDataSuffix);
	RemoveFocus (&fFileHeaderGroupTabLoop);

	RemoveSubView (&fCompileOnce);
	RemoveSubView (&fFileHeaderGroup);
		fFileHeaderGroupTabLoop.RemoveFocus (&fDefaultPrepend);

		fFileHeaderGroup.RemoveSubView (&fDefaultPrependScroller);
			fDefaultPrependScroller.SetTextEdit (Nil);
	RemoveSubView (&fGridSizeGroup);
		fGridSizeGroupTabLoop.RemoveFocus (&fVertical);
		fGridSizeGroupTabLoop.RemoveFocus (&fHorizontal);

		fGridSizeGroup.RemoveSubView (&fShowGrid);
		fGridSizeGroup.RemoveSubView (&fConstrainTracking);
		fGridSizeGroup.RemoveSubView (&fVertical);
		fGridSizeGroup.RemoveSubView (&fHorizontal);
		fGridSizeGroup.RemoveSubView (&fHGridLabel);
		fGridSizeGroup.RemoveSubView (&fVGridLabel);
	RemoveSubView (&fDataSuffixLabel);
	RemoveSubView (&fHeaderSuffixLabel);
	RemoveSubView (&fSourceSuffixLabel);
	RemoveSubView (&fSourceSuffix);
	RemoveSubView (&fHeaderSuffix);
	RemoveSubView (&fDataSuffix);
	RemoveSubView (&fSpacingGroup);
		fSpacingGroupTabLoop.RemoveFocus (&fSpace);

		fSpacingGroup.RemoveSubView (&fSpace);
		fSpacingGroup.RemoveSubView (&fSpacingLabel);
		fSpacingGroup.RemoveSubView (&fSpacings);
		fSpacingGroup.RemoveSubView (&fInsert);
		fSpacingGroup.RemoveSubView (&fDelete);
}

#if   qMacUI

void	PreferencesInfoX::BuildForMacUI ()
{
	SetSize (Point (300, 348), eNoUpdate);

	fCompileOnce.SetLabel ("Compile Once", eNoUpdate);
	fCompileOnce.SetExtent (173, 176, 14, 104, eNoUpdate);
	fCompileOnce.SetController (this);
	AddSubView (&fCompileOnce);

	fFileHeaderGroup.SetExtent (188, 4, 102, 339, eNoUpdate);
	fFileHeaderGroup.SetBorder (1, 1, eNoUpdate);
	fFileHeaderGroup.SetMargin (1, 1, eNoUpdate);
	fFileHeaderGroup.SetFont (&kSystemFont);
	fFileHeaderGroup.SetLabel ("File Header", eNoUpdate);
		fDefaultPrependScroller.SetExtent (14, 1, 87, 337, eNoUpdate);
		fDefaultPrependScroller.SetBorder (0, 0, eNoUpdate);
		fDefaultPrepend.SetController (this);
		fDefaultPrepend.SetWordWrap (False);
		fDefaultPrepend.SetBorder (kZeroPoint, eNoUpdate);
		fDefaultPrepend.SetMargin (kZeroPoint, eNoUpdate);
			fDefaultPrependScroller.SetTextEdit (&fDefaultPrepend);
		fFileHeaderGroup.AddSubView (&fDefaultPrependScroller);

		fFileHeaderGroupTabLoop.AddFocus (&fDefaultPrepend);
	AddSubView (&fFileHeaderGroup);

	fGridSizeGroup.SetExtent (2, 10, 104, 150, eNoUpdate);
	fGridSizeGroup.SetHelp ("Specifies the granularity of the grid used to help align items.");
	fGridSizeGroup.SetBorder (1, 1, eNoUpdate);
	fGridSizeGroup.SetMargin (1, 1, eNoUpdate);
	fGridSizeGroup.SetFont (&kSystemFont);
	fGridSizeGroup.SetLabel ("Grid Size", eNoUpdate);
		fShowGrid.SetLabel ("Show Grid", eNoUpdate);
		fShowGrid.SetExtent (49, 2, 16, 86, eNoUpdate);
		fShowGrid.SetHelp ("Show the alignment grid in main view editing window.");
		fShowGrid.SetController (this);
		fGridSizeGroup.AddSubView (&fShowGrid);

		fConstrainTracking.SetLabel ("Constrain Tracking", eNoUpdate);
		fConstrainTracking.SetExtent (76, 2, 16, 143, eNoUpdate);
		fConstrainTracking.SetHelp ("Use alignment grid to constrain item construction and dragging.");
		fConstrainTracking.SetController (this);
		fGridSizeGroup.AddSubView (&fConstrainTracking);

		fVertical.SetExtent (15, 21, 20, 50, eNoUpdate);
		fVertical.SetFont (&kApplicationFont);
		fVertical.SetController (this);
		fVertical.SetMultiLine (False);
		fVertical.SetMaxValue (32);
		fVertical.SetMinValue (1);
		fGridSizeGroup.AddSubView (&fVertical);

		fHorizontal.SetExtent (15, 96, 20, 50, eNoUpdate);
		fHorizontal.SetFont (&kApplicationFont);
		fHorizontal.SetController (this);
		fHorizontal.SetMultiLine (False);
		fHorizontal.SetMaxValue (32);
		fHorizontal.SetMinValue (1);
		fGridSizeGroup.AddSubView (&fHorizontal);

		fHGridLabel.SetExtent (16, 79, 18, 16, eNoUpdate);
		fHGridLabel.SetFont (&kSystemFont);
		fHGridLabel.SetText ("H:");
		fGridSizeGroup.AddSubView (&fHGridLabel);

		fVGridLabel.SetExtent (16, 4, 18, 16, eNoUpdate);
		fVGridLabel.SetFont (&kSystemFont);
		fVGridLabel.SetText ("V:");
		fGridSizeGroup.AddSubView (&fVGridLabel);

		fGridSizeGroupTabLoop.AddFocus (&fVertical);
		fGridSizeGroupTabLoop.AddFocus (&fHorizontal);
	AddSubView (&fGridSizeGroup);

	fDataSuffixLabel.SetExtent (113, 19, 15, 77, eNoUpdate);
	fDataSuffixLabel.SetFont (&kSystemFont);
	fDataSuffixLabel.SetText ("Data Suffix:");
	AddSubView (&fDataSuffixLabel);

	fHeaderSuffixLabel.SetExtent (140, 5, 15, 93, eNoUpdate);
	fHeaderSuffixLabel.SetFont (&kSystemFont);
	fHeaderSuffixLabel.SetText ("Header Suffix:");
	AddSubView (&fHeaderSuffixLabel);

	fSourceSuffixLabel.SetExtent (164, 6, 15, 91, eNoUpdate);
	fSourceSuffixLabel.SetFont (&kSystemFont);
	fSourceSuffixLabel.SetText ("Source Suffix:");
	AddSubView (&fSourceSuffixLabel);

	fSourceSuffix.SetExtent (164, 98, 17, 64, eNoUpdate);
	fSourceSuffix.SetFont (&kApplicationFont);
	fSourceSuffix.SetController (this);
	fSourceSuffix.SetWordWrap (False);
	AddSubView (&fSourceSuffix);

	fHeaderSuffix.SetExtent (137, 98, 17, 64, eNoUpdate);
	fHeaderSuffix.SetFont (&kApplicationFont);
	fHeaderSuffix.SetController (this);
	fHeaderSuffix.SetWordWrap (False);
	AddSubView (&fHeaderSuffix);

	fDataSuffix.SetExtent (110, 98, 17, 65, eNoUpdate);
	fDataSuffix.SetFont (&kApplicationFont);
	fDataSuffix.SetController (this);
	fDataSuffix.SetWordWrap (False);
	AddSubView (&fDataSuffix);

	fSpacingGroup.SetExtent (8, 178, 123, 161, eNoUpdate);
	fSpacingGroup.SetBorder (1, 1, eNoUpdate);
	fSpacingGroup.SetMargin (1, 1, eNoUpdate);
	fSpacingGroup.SetFont (&kSystemFont);
	fSpacingGroup.SetLabel ("Spacing", eNoUpdate);
		fSpace.SetExtent (100, 130, 20, 25, eNoUpdate);
		fSpace.SetFont (&kApplicationFont);
		fSpace.SetController (this);
		fSpace.SetMultiLine (False);
		fSpace.SetMaxValue (1000);
		fSpace.SetMinValue (0);
		fSpacingGroup.AddSubView (&fSpace);

		fSpacingLabel.SetExtent (102, 75, 15, 54, eNoUpdate);
		fSpacingLabel.SetFont (&kSystemFont);
		fSpacingLabel.SetText ("Spacing:");
		fSpacingGroup.AddSubView (&fSpacingLabel);

		fSpacings.SetExtent (17, 7, 103, 67, eNoUpdate);
		fSpacings.SetItemsFloat (True);
		fSpacings.SetController (this);
		fSpacingGroup.AddSubView (&fSpacings);

		fInsert.SetExtent (16, 95, 20, 55, eNoUpdate);
		fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
		fInsert.SetLabel ("Insert", eNoUpdate);
		fInsert.SetController (this);
		fSpacingGroup.AddSubView (&fInsert);

		fDelete.SetExtent (41, 95, 20, 55, eNoUpdate);
		fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
		fDelete.SetLabel ("Delete", eNoUpdate);
		fDelete.SetController (this);
		fSpacingGroup.AddSubView (&fDelete);

		fSpacingGroupTabLoop.AddFocus (&fSpace);
	AddSubView (&fSpacingGroup);

	AddFocus (&fGridSizeGroupTabLoop);
	AddFocus (&fSpacingGroupTabLoop);
	AddFocus (&fSourceSuffix);
	AddFocus (&fHeaderSuffix);
	AddFocus (&fDataSuffix);
	AddFocus (&fFileHeaderGroupTabLoop);
}

#elif qMotifUI

void	PreferencesInfoX::BuildForMotifUI ()
{
	SetSize (Point (315, 348), eNoUpdate);

	fCompileOnce.SetLabel ("Compile Once", eNoUpdate);
	fCompileOnce.SetExtent (173, 176, 25, 114, eNoUpdate);
	fCompileOnce.SetController (this);
	AddSubView (&fCompileOnce);

	fFileHeaderGroup.SetExtent (195, 2, 118, 343, eNoUpdate);
	fFileHeaderGroup.SetBorder (1, 1, eNoUpdate);
	fFileHeaderGroup.SetMargin (1, 1, eNoUpdate);
	fFileHeaderGroup.SetFont (&kSystemFont);
	fFileHeaderGroup.SetLabel ("File Header", eNoUpdate);
		fDefaultPrependScroller.SetExtent (14, 1, 102, 337, eNoUpdate);
		fDefaultPrependScroller.SetBorder (0, 0, eNoUpdate);
		fDefaultPrependScroller.SetMargin (0, 0, eNoUpdate);
		fDefaultPrepend.SetController (this);
		fDefaultPrepend.SetWordWrap (False);
		fDefaultPrepend.SetBorder (kZeroPoint, eNoUpdate);
		fDefaultPrepend.SetMargin (kZeroPoint, eNoUpdate);
			fDefaultPrependScroller.SetTextEdit (&fDefaultPrepend);
		fFileHeaderGroup.AddSubView (&fDefaultPrependScroller);

		fFileHeaderGroupTabLoop.AddFocus (&fDefaultPrepend);
	AddSubView (&fFileHeaderGroup);

	fGridSizeGroup.SetExtent (2, 10, 104, 150, eNoUpdate);
	fGridSizeGroup.SetHelp ("Specifies the granularity of the grid used to help align items.");
	fGridSizeGroup.SetBorder (1, 1, eNoUpdate);
	fGridSizeGroup.SetMargin (1, 1, eNoUpdate);
	fGridSizeGroup.SetFont (&kSystemFont);
	fGridSizeGroup.SetLabel ("Grid Size", eNoUpdate);
		fShowGrid.SetLabel ("Show Grid", eNoUpdate);
		fShowGrid.SetExtent (49, 2, 25, 94, eNoUpdate);
		fShowGrid.SetHelp ("Show the alignment grid in main view editing window.");
		fShowGrid.SetController (this);
		fGridSizeGroup.AddSubView (&fShowGrid);

		fConstrainTracking.SetLabel ("Constrain Tracking", eNoUpdate);
		fConstrainTracking.SetExtent (76, 2, 25, 145, eNoUpdate);
		fConstrainTracking.SetHelp ("Use alignment grid to constrain item construction and dragging.");
		fConstrainTracking.SetController (this);
		fGridSizeGroup.AddSubView (&fConstrainTracking);

		fVertical.SetExtent (15, 21, 26, 50, eNoUpdate);
		fVertical.SetFont (&kApplicationFont);
		fVertical.SetController (this);
		fVertical.SetMultiLine (False);
		fVertical.SetMaxValue (32);
		fVertical.SetMinValue (1);
		fGridSizeGroup.AddSubView (&fVertical);

		fHorizontal.SetExtent (15, 96, 26, 50, eNoUpdate);
		fHorizontal.SetFont (&kApplicationFont);
		fHorizontal.SetController (this);
		fHorizontal.SetMultiLine (False);
		fHorizontal.SetMaxValue (32);
		fHorizontal.SetMinValue (1);
		fGridSizeGroup.AddSubView (&fHorizontal);

		fHGridLabel.SetExtent (16, 76, 21, 19, eNoUpdate);
		fHGridLabel.SetFont (&kSystemFont);
		fHGridLabel.SetText ("H:");
		fGridSizeGroup.AddSubView (&fHGridLabel);

		fVGridLabel.SetExtent (16, 2, 21, 18, eNoUpdate);
		fVGridLabel.SetFont (&kSystemFont);
		fVGridLabel.SetText ("V:");
		fGridSizeGroup.AddSubView (&fVGridLabel);

		fGridSizeGroupTabLoop.AddFocus (&fVertical);
		fGridSizeGroupTabLoop.AddFocus (&fHorizontal);
		fGridSizeGroupTabLoop.AddFocus (&fShowGrid);
		fGridSizeGroupTabLoop.AddFocus (&fConstrainTracking);
	AddSubView (&fGridSizeGroup);

	fDataSuffixLabel.SetExtent (113, 19, 21, 74, eNoUpdate);
	fDataSuffixLabel.SetFont (&kSystemFont);
	fDataSuffixLabel.SetText ("Data Suffix:");
	AddSubView (&fDataSuffixLabel);

	fHeaderSuffixLabel.SetExtent (140, 5, 21, 88, eNoUpdate);
	fHeaderSuffixLabel.SetFont (&kSystemFont);
	fHeaderSuffixLabel.SetText ("Header Suffix:");
	AddSubView (&fHeaderSuffixLabel);

	fSourceSuffixLabel.SetExtent (164, 6, 21, 87, eNoUpdate);
	fSourceSuffixLabel.SetFont (&kSystemFont);
	fSourceSuffixLabel.SetText ("Source Suffix:");
	AddSubView (&fSourceSuffixLabel);

	fSourceSuffix.SetExtent (164, 98, 26, 64, eNoUpdate);
	fSourceSuffix.SetFont (&kApplicationFont);
	fSourceSuffix.SetController (this);
	fSourceSuffix.SetWordWrap (False);
	AddSubView (&fSourceSuffix);

	fHeaderSuffix.SetExtent (137, 98, 26, 64, eNoUpdate);
	fHeaderSuffix.SetFont (&kApplicationFont);
	fHeaderSuffix.SetController (this);
	fHeaderSuffix.SetWordWrap (False);
	AddSubView (&fHeaderSuffix);

	fDataSuffix.SetExtent (110, 98, 26, 65, eNoUpdate);
	fDataSuffix.SetFont (&kApplicationFont);
	fDataSuffix.SetController (this);
	fDataSuffix.SetWordWrap (False);
	AddSubView (&fDataSuffix);

	fSpacingGroup.SetExtent (8, 178, 153, 161, eNoUpdate);
	fSpacingGroup.SetBorder (1, 1, eNoUpdate);
	fSpacingGroup.SetMargin (1, 1, eNoUpdate);
	fSpacingGroup.SetFont (&kSystemFont);
	fSpacingGroup.SetLabel ("Spacing", eNoUpdate);
		fSpace.SetExtent (120, 131, 26, 25, eNoUpdate);
		fSpace.SetFont (&kApplicationFont);
		fSpace.SetController (this);
		fSpace.SetMultiLine (False);
		fSpace.SetMaxValue (1000);
		fSpace.SetMinValue (0);
		fSpacingGroup.AddSubView (&fSpace);

		fSpacingLabel.SetExtent (122, 76, 21, 53, eNoUpdate);
		fSpacingLabel.SetFont (&kSystemFont);
		fSpacingLabel.SetText ("Spacing:");
		fSpacingGroup.AddSubView (&fSpacingLabel);

		fSpacings.SetExtent (17, 7, 133, 67, eNoUpdate);
		fSpacings.SetItemsFloat (True);
		fSpacings.SetController (this);
		fSpacingGroup.AddSubView (&fSpacings);

		fInsert.SetExtent (16, 85, 24, 70, eNoUpdate);
		fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
		fInsert.SetLabel ("Insert", eNoUpdate);
		fInsert.SetController (this);
		fSpacingGroup.AddSubView (&fInsert);

		fDelete.SetExtent (41, 85, 24, 70, eNoUpdate);
		fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
		fDelete.SetLabel ("Delete", eNoUpdate);
		fDelete.SetController (this);
		fSpacingGroup.AddSubView (&fDelete);

		fSpacingGroupTabLoop.AddFocus (&fSpacings);
		fSpacingGroupTabLoop.AddFocus (&fSpace);
		fSpacingGroupTabLoop.AddFocus (&fInsert);
		fSpacingGroupTabLoop.AddFocus (&fDelete);
	AddSubView (&fSpacingGroup);

	AddFocus (&fCompileOnce);
	AddFocus (&fGridSizeGroupTabLoop);
	AddFocus (&fSpacingGroupTabLoop);
	AddFocus (&fSourceSuffix);
	AddFocus (&fHeaderSuffix);
	AddFocus (&fDataSuffix);
	AddFocus (&fFileHeaderGroupTabLoop);
}

#else

void	PreferencesInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (300, 348), eNoUpdate);

	fCompileOnce.SetLabel ("Compile Once", eNoUpdate);
	fCompileOnce.SetExtent (173, 176, 14, 104, eNoUpdate);
	fCompileOnce.SetController (this);
	AddSubView (&fCompileOnce);

	fFileHeaderGroup.SetExtent (188, 4, 102, 339, eNoUpdate);
	fFileHeaderGroup.SetBorder (1, 1, eNoUpdate);
	fFileHeaderGroup.SetMargin (1, 1, eNoUpdate);
	fFileHeaderGroup.SetFont (&kSystemFont);
	fFileHeaderGroup.SetLabel ("File Header", eNoUpdate);
		fDefaultPrependScroller.SetExtent (14, 1, 87, 337, eNoUpdate);
		fDefaultPrependScroller.SetBorder (0, 0, eNoUpdate);
		fDefaultPrepend.SetController (this);
		fDefaultPrepend.SetWordWrap (False);
		fDefaultPrepend.SetBorder (kZeroPoint, eNoUpdate);
		fDefaultPrepend.SetMargin (kZeroPoint, eNoUpdate);
			fDefaultPrependScroller.SetTextEdit (&fDefaultPrepend);
		fFileHeaderGroup.AddSubView (&fDefaultPrependScroller);

		fFileHeaderGroupTabLoop.AddFocus (&fDefaultPrepend);
	AddSubView (&fFileHeaderGroup);

	fGridSizeGroup.SetExtent (2, 10, 104, 150, eNoUpdate);
	fGridSizeGroup.SetHelp ("Specifies the granularity of the grid used to help align items.");
	fGridSizeGroup.SetBorder (1, 1, eNoUpdate);
	fGridSizeGroup.SetMargin (1, 1, eNoUpdate);
	fGridSizeGroup.SetFont (&kSystemFont);
	fGridSizeGroup.SetLabel ("Grid Size", eNoUpdate);
		fShowGrid.SetLabel ("Show Grid", eNoUpdate);
		fShowGrid.SetExtent (49, 2, 16, 86, eNoUpdate);
		fShowGrid.SetHelp ("Show the alignment grid in main view editing window.");
		fShowGrid.SetController (this);
		fGridSizeGroup.AddSubView (&fShowGrid);

		fConstrainTracking.SetLabel ("Constrain Tracking", eNoUpdate);
		fConstrainTracking.SetExtent (76, 2, 16, 143, eNoUpdate);
		fConstrainTracking.SetHelp ("Use alignment grid to constrain item construction and dragging.");
		fConstrainTracking.SetController (this);
		fGridSizeGroup.AddSubView (&fConstrainTracking);

		fVertical.SetExtent (15, 21, 20, 50, eNoUpdate);
		fVertical.SetFont (&kApplicationFont);
		fVertical.SetController (this);
		fVertical.SetMultiLine (False);
		fVertical.SetMaxValue (32);
		fVertical.SetMinValue (1);
		fGridSizeGroup.AddSubView (&fVertical);

		fHorizontal.SetExtent (15, 96, 20, 50, eNoUpdate);
		fHorizontal.SetFont (&kApplicationFont);
		fHorizontal.SetController (this);
		fHorizontal.SetMultiLine (False);
		fHorizontal.SetMaxValue (32);
		fHorizontal.SetMinValue (1);
		fGridSizeGroup.AddSubView (&fHorizontal);

		fHGridLabel.SetExtent (16, 79, 18, 16, eNoUpdate);
		fHGridLabel.SetFont (&kSystemFont);
		fHGridLabel.SetText ("H:");
		fGridSizeGroup.AddSubView (&fHGridLabel);

		fVGridLabel.SetExtent (16, 4, 18, 16, eNoUpdate);
		fVGridLabel.SetFont (&kSystemFont);
		fVGridLabel.SetText ("V:");
		fGridSizeGroup.AddSubView (&fVGridLabel);

		fGridSizeGroupTabLoop.AddFocus (&fVertical);
		fGridSizeGroupTabLoop.AddFocus (&fHorizontal);
	AddSubView (&fGridSizeGroup);

	fDataSuffixLabel.SetExtent (113, 19, 15, 77, eNoUpdate);
	fDataSuffixLabel.SetFont (&kSystemFont);
	fDataSuffixLabel.SetText ("Data Suffix:");
	AddSubView (&fDataSuffixLabel);

	fHeaderSuffixLabel.SetExtent (140, 5, 15, 93, eNoUpdate);
	fHeaderSuffixLabel.SetFont (&kSystemFont);
	fHeaderSuffixLabel.SetText ("Header Suffix:");
	AddSubView (&fHeaderSuffixLabel);

	fSourceSuffixLabel.SetExtent (164, 6, 15, 91, eNoUpdate);
	fSourceSuffixLabel.SetFont (&kSystemFont);
	fSourceSuffixLabel.SetText ("Source Suffix:");
	AddSubView (&fSourceSuffixLabel);

	fSourceSuffix.SetExtent (164, 98, 17, 64, eNoUpdate);
	fSourceSuffix.SetFont (&kApplicationFont);
	fSourceSuffix.SetController (this);
	fSourceSuffix.SetWordWrap (False);
	AddSubView (&fSourceSuffix);

	fHeaderSuffix.SetExtent (137, 98, 17, 64, eNoUpdate);
	fHeaderSuffix.SetFont (&kApplicationFont);
	fHeaderSuffix.SetController (this);
	fHeaderSuffix.SetWordWrap (False);
	AddSubView (&fHeaderSuffix);

	fDataSuffix.SetExtent (110, 98, 17, 65, eNoUpdate);
	fDataSuffix.SetFont (&kApplicationFont);
	fDataSuffix.SetController (this);
	fDataSuffix.SetWordWrap (False);
	AddSubView (&fDataSuffix);

	fSpacingGroup.SetExtent (8, 178, 123, 161, eNoUpdate);
	fSpacingGroup.SetBorder (1, 1, eNoUpdate);
	fSpacingGroup.SetMargin (1, 1, eNoUpdate);
	fSpacingGroup.SetFont (&kSystemFont);
	fSpacingGroup.SetLabel ("Spacing", eNoUpdate);
		fSpace.SetExtent (100, 130, 20, 25, eNoUpdate);
		fSpace.SetFont (&kApplicationFont);
		fSpace.SetController (this);
		fSpace.SetMultiLine (False);
		fSpace.SetMaxValue (1000);
		fSpace.SetMinValue (0);
		fSpacingGroup.AddSubView (&fSpace);

		fSpacingLabel.SetExtent (102, 75, 15, 54, eNoUpdate);
		fSpacingLabel.SetFont (&kSystemFont);
		fSpacingLabel.SetText ("Spacing:");
		fSpacingGroup.AddSubView (&fSpacingLabel);

		fSpacings.SetExtent (17, 7, 103, 67, eNoUpdate);
		fSpacings.SetItemsFloat (True);
		fSpacings.SetController (this);
		fSpacingGroup.AddSubView (&fSpacings);

		fInsert.SetExtent (16, 95, 20, 55, eNoUpdate);
		fInsert.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
		fInsert.SetLabel ("Insert", eNoUpdate);
		fInsert.SetController (this);
		fSpacingGroup.AddSubView (&fInsert);

		fDelete.SetExtent (41, 95, 20, 55, eNoUpdate);
		fDelete.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
		fDelete.SetLabel ("Delete", eNoUpdate);
		fDelete.SetController (this);
		fSpacingGroup.AddSubView (&fDelete);

		fSpacingGroupTabLoop.AddFocus (&fSpace);
	AddSubView (&fSpacingGroup);

	AddFocus (&fGridSizeGroupTabLoop);
	AddFocus (&fSpacingGroupTabLoop);
	AddFocus (&fSourceSuffix);
	AddFocus (&fHeaderSuffix);
	AddFocus (&fDataSuffix);
	AddFocus (&fFileHeaderGroupTabLoop);
}

#endif /* GUI */

Point	PreferencesInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (300, 348));
#elif   qMotifUI
	return (Point (315, 348));
#else
	return (Point (300, 348));
#endif /* GUI */
}

void	PreferencesInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfCompileOnceOrigin = fCompileOnce.GetOrigin ();
		fCompileOnce.SetOrigin (kOriginalfCompileOnceOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfFileHeaderGroupSize = fFileHeaderGroup.GetSize ();
		fFileHeaderGroup.SetSize (kOriginalfFileHeaderGroupSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
#if   qMacUI
		const Point fFileHeaderGroupSizeDelta = Point (102,339) - fFileHeaderGroup.GetSize ();
#elif   qMotifUI
		const Point fFileHeaderGroupSizeDelta = Point (118,343) - fFileHeaderGroup.GetSize ();
#else
		const Point fFileHeaderGroupSizeDelta = Point (102,339) - fFileHeaderGroup.GetSize ();
#endif /* GUI */
			static const Point	kOriginalfDefaultPrependSize = fDefaultPrependScroller.GetSize ();
			fDefaultPrependScroller.SetSize (kOriginalfDefaultPrependSize - Point (fFileHeaderGroupSizeDelta.GetV (), fFileHeaderGroupSizeDelta.GetH ()));
		static const Point	kOriginalfSourceSuffixSize = fSourceSuffix.GetSize ();
		fSourceSuffix.SetSize (kOriginalfSourceSuffixSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfHeaderSuffixSize = fHeaderSuffix.GetSize ();
		fHeaderSuffix.SetSize (kOriginalfHeaderSuffixSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfDataSuffixSize = fDataSuffix.GetSize ();
		fDataSuffix.SetSize (kOriginalfDataSuffixSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfSpacingGroupOrigin = fSpacingGroup.GetOrigin ();
		fSpacingGroup.SetOrigin (kOriginalfSpacingGroupOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfSpacingGroupSize = fSpacingGroup.GetSize ();
		fSpacingGroup.SetSize (kOriginalfSpacingGroupSize - Point (kSizeDelta.GetV (), 0));
#if   qMacUI
		const Point fSpacingGroupSizeDelta = Point (123,161) - fSpacingGroup.GetSize ();
#elif   qMotifUI
		const Point fSpacingGroupSizeDelta = Point (153,161) - fSpacingGroup.GetSize ();
#else
		const Point fSpacingGroupSizeDelta = Point (123,161) - fSpacingGroup.GetSize ();
#endif /* GUI */
			static const Point	kOriginalfSpaceOrigin = fSpace.GetOrigin ();
			fSpace.SetOrigin (kOriginalfSpaceOrigin - Point (fSpacingGroupSizeDelta.GetV (), 0));
			static const Point	kOriginalfSpacingLabelOrigin = fSpacingLabel.GetOrigin ();
			fSpacingLabel.SetOrigin (kOriginalfSpacingLabelOrigin - Point (fSpacingGroupSizeDelta.GetV (), 0));
			static const Point	kOriginalfSpacingsSize = fSpacings.GetSize ();
			fSpacings.SetSize (kOriginalfSpacingsSize - Point (fSpacingGroupSizeDelta.GetV (), 0));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
PreferencesInfo::PreferencesInfo ()
{
	UpdateContext ();
}
		
void	PreferencesInfo::ButtonPressed (AbstractButton* button)
{
	if (button == &fSpacings) {
		fSpace.SetText (fSpacings.GetSelectedString ());
		fSpace.SetSelection ();
	}
	else if (button == &fInsert) {
		String	foo = fSpace.GetText ();
		Assert (foo != kEmptyString);
		AbstractPickListItem*	item = fSpacings.GetSelectedItem ();
		if (item == Nil) {
			fSpacings.AddItem (foo);
		}
		else {
			((StringPickListItem*)item)->SetText (foo);
		}
		fSpace.SetText (kEmptyString);
		fSpacings.SelectAll (not PickList::kSelected);
	}
	else if (button == &fDelete) {
		fSpacings.Delete (fSpacings.GetSelectedItem ());
		fSpace.SetText (kEmptyString);
	}
	else {
		PreferencesInfoX::ButtonPressed (button);
	}

	UpdateContext ();
}		

void	PreferencesInfo::TextChanged (TextEditBase* item)
{
	TextController::TextChanged (item);
	UpdateContext ();
}

void	PreferencesInfo::UpdateContext ()
{
	if (fSpacings.GetSelectedItem () != Nil) {
		fInsert.SetLabel ("Change");
		fDelete.SetEnabled (True);
	}
	else {
		fInsert.SetLabel ("Insert");
		fDelete.SetEnabled (False);
	}
	fInsert.SetEnabled (Boolean (fSpace.GetLength () > 0));
	GetVerticalField ().SetEnabled (GetGridOnField ().GetOn ());
	GetHorizontalField ().SetEnabled (GetGridOnField ().GetOn ());
	GetConstrainTrackingField ().SetEnabled (fShowGrid.GetOn ());
	if (not GetGridOnField ().GetOn ()) {
		GetConstrainTrackingField ().SetOn (not Toggle::kOn);
	}
}

