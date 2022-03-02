/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ViewItemInfo.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ViewItemInfo.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.27  92/04/08  17:23:24  17:23:24  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *		Revision 1.9  1992/01/31  18:21:58  sterling
 *		Bootstrapped
 *
 *
 *
 */




#include 	"Shape.hh"
#include	"Dialog.hh"
#include	"CheckBox.hh"
#include	"FontPicker.hh"

#include	"CommandNumbers.hh"
#include	"ViewItem.hh"

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "ViewItemInfo.hh"


ViewItemInfo_X::ViewItemInfo_X () :
	fQuickHelpGroup (kEmptyString),
	fField (),
	fClass (),
	fMarginGroup (kEmptyString),
	fBorderGroup (kEmptyString),
	fBounds (kEmptyString),
	fFieldLabel (),
	fClassLabel (),
	fFontButton (),
	fColor ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

ViewItemInfo_X::~ViewItemInfo_X ()
{
	RemoveFocus (&fField);
	RemoveFocus (&fClass);
	RemoveFocus (&fBoundsTabLoop);
	RemoveFocus (&fBorderGroupTabLoop);
	RemoveFocus (&fMarginGroupTabLoop);
	RemoveFocus (&fQuickHelpGroupTabLoop);

	RemoveSubView (&fQuickHelpGroup);
		fQuickHelpGroupTabLoop.RemoveFocus (&fQuickHelp);

		fQuickHelpGroup.RemoveSubView (&fQuickHelp);
	RemoveSubView (&fField);
	RemoveSubView (&fClass);
	RemoveSubView (&fMarginGroup);
		fMarginGroupTabLoop.RemoveFocus (&fMarginHeight);
		fMarginGroupTabLoop.RemoveFocus (&fMarginWidth);

		fMarginGroup.RemoveSubView (&fMarginHeightLabel);
		fMarginGroup.RemoveSubView (&fMarginWidthLabel);
		fMarginGroup.RemoveSubView (&fMarginHeight);
		fMarginGroup.RemoveSubView (&fMarginWidth);
	RemoveSubView (&fBorderGroup);
		fBorderGroupTabLoop.RemoveFocus (&fBorderHeight);
		fBorderGroupTabLoop.RemoveFocus (&fBorderWidth);

		fBorderGroup.RemoveSubView (&fBorderHeightLabel);
		fBorderGroup.RemoveSubView (&fBorderWidth);
		fBorderGroup.RemoveSubView (&fBorderHeight);
		fBorderGroup.RemoveSubView (&fBorderWidthLabel);
	RemoveSubView (&fBounds);
		fBoundsTabLoop.RemoveFocus (&fTop);
		fBoundsTabLoop.RemoveFocus (&fLeft);
		fBoundsTabLoop.RemoveFocus (&fHeight);
		fBoundsTabLoop.RemoveFocus (&fWidth);

		fBounds.RemoveSubView (&fBoundsWidthLabel);
		fBounds.RemoveSubView (&fBoundsHeightLabel);
		fBounds.RemoveSubView (&fBoundsLeftLabel);
		fBounds.RemoveSubView (&fWidth);
		fBounds.RemoveSubView (&fHeight);
		fBounds.RemoveSubView (&fBoundsTopLabel);
		fBounds.RemoveSubView (&fLeft);
		fBounds.RemoveSubView (&fTop);
	RemoveSubView (&fFieldLabel);
	RemoveSubView (&fClassLabel);
	RemoveSubView (&fFontButton);
	RemoveSubView (&fColor);
}

#if   qMacUI

void	ViewItemInfo_X::BuildForMacUI ()
{
	SetSize (Point (212, 309), eNoUpdate);

	fQuickHelpGroup.SetExtent (110, 2, 100, 304, eNoUpdate);
	fQuickHelpGroup.SetBorder (1, 1, eNoUpdate);
	fQuickHelpGroup.SetMargin (1, 1, eNoUpdate);
	fQuickHelpGroup.SetFont (&kSystemFont);
	fQuickHelpGroup.SetLabel ("Quick Help", eNoUpdate);
		fQuickHelp.SetExtent (13, 2, 84, 299, eNoUpdate);
		fQuickHelp.SetBorder (0, 0, eNoUpdate);
		fQuickHelp.SetFont (&kApplicationFont);
		fQuickHelp.SetController (this);
		fQuickHelpGroup.AddSubView (&fQuickHelp);

		fQuickHelpGroupTabLoop.AddFocus (&fQuickHelp);
	AddSubView (&fQuickHelpGroup);

	fField.SetExtent (8, 45, 17, 108, eNoUpdate);
	fField.SetMargin (1, 1, eNoUpdate);
	fField.SetFont (&kApplicationFont);
	fField.SetController (this);
	fField.SetWordWrap (False);
	fField.SetMultiLine (False);
	AddSubView (&fField);

	fClass.SetExtent (35, 45, 17, 109, eNoUpdate);
	fClass.SetMargin (1, 1, eNoUpdate);
	fClass.SetFont (&kApplicationFont);
	fClass.SetController (this);
	fClass.SetWordWrap (False);
	fClass.SetMultiLine (False);
	AddSubView (&fClass);

	fMarginGroup.SetExtent (55, 163, 50, 76, eNoUpdate);
	fMarginGroup.SetBorder (1, 1, eNoUpdate);
	fMarginGroup.SetMargin (1, 1, eNoUpdate);
	fMarginGroup.SetFont (&kSystemFont);
	fMarginGroup.SetLabel ("Margin", eNoUpdate);
		fMarginHeightLabel.SetExtent (34, 5, 12, 28, eNoUpdate);
		fMarginHeightLabel.SetFont (&kApplicationFont);
		fMarginHeightLabel.SetText ("height");
		fMarginGroup.AddSubView (&fMarginHeightLabel);

		fMarginWidthLabel.SetExtent (34, 40, 12, 27, eNoUpdate);
		fMarginWidthLabel.SetFont (&kApplicationFont);
		fMarginWidthLabel.SetText ("width");
		fMarginGroup.AddSubView (&fMarginWidthLabel);

		fMarginHeight.SetExtent (16, 3, 16, 32, eNoUpdate);
		fMarginHeight.SetFont (&kApplicationFont);
		fMarginHeight.SetController (this);
		fMarginHeight.SetMultiLine (False);
		fMarginHeight.SetMinValue (0);
		fMarginGroup.AddSubView (&fMarginHeight);

		fMarginWidth.SetExtent (16, 38, 16, 32, eNoUpdate);
		fMarginWidth.SetFont (&kApplicationFont);
		fMarginWidth.SetController (this);
		fMarginWidth.SetMultiLine (False);
		fMarginWidth.SetMinValue (0);
		fMarginGroup.AddSubView (&fMarginWidth);

		fMarginGroupTabLoop.AddFocus (&fMarginHeight);
		fMarginGroupTabLoop.AddFocus (&fMarginWidth);
	AddSubView (&fMarginGroup);

	fBorderGroup.SetExtent (3, 163, 49, 76, eNoUpdate);
	fBorderGroup.SetBorder (1, 1, eNoUpdate);
	fBorderGroup.SetMargin (1, 1, eNoUpdate);
	fBorderGroup.SetFont (&kSystemFont);
	fBorderGroup.SetLabel ("Border", eNoUpdate);
		fBorderHeightLabel.SetExtent (33, 5, 12, 27, eNoUpdate);
		fBorderHeightLabel.SetFont (&kApplicationFont);
		fBorderHeightLabel.SetText ("height");
		fBorderGroup.AddSubView (&fBorderHeightLabel);

		fBorderWidth.SetExtent (15, 39, 16, 32, eNoUpdate);
		fBorderWidth.SetFont (&kApplicationFont);
		fBorderWidth.SetController (this);
		fBorderWidth.SetMultiLine (False);
		fBorderWidth.SetMinValue (0);
		fBorderGroup.AddSubView (&fBorderWidth);

		fBorderHeight.SetExtent (15, 3, 16, 32, eNoUpdate);
		fBorderHeight.SetFont (&kApplicationFont);
		fBorderHeight.SetController (this);
		fBorderHeight.SetMultiLine (False);
		fBorderHeight.SetMinValue (0);
		fBorderGroup.AddSubView (&fBorderHeight);

		fBorderWidthLabel.SetExtent (33, 41, 12, 27, eNoUpdate);
		fBorderWidthLabel.SetFont (&kApplicationFont);
		fBorderWidthLabel.SetText ("width");
		fBorderGroup.AddSubView (&fBorderWidthLabel);

		fBorderGroupTabLoop.AddFocus (&fBorderHeight);
		fBorderGroupTabLoop.AddFocus (&fBorderWidth);
	AddSubView (&fBorderGroup);

	fBounds.SetExtent (57, 3, 48, 151, eNoUpdate);
	fBounds.SetBorder (1, 1, eNoUpdate);
	fBounds.SetMargin (1, 1, eNoUpdate);
	fBounds.SetFont (&kSystemFont);
	fBounds.SetLabel ("Bounds", eNoUpdate);
		fBoundsWidthLabel.SetExtent (32, 119, 12, 26, eNoUpdate);
		fBoundsWidthLabel.SetFont (&kApplicationFont);
		fBoundsWidthLabel.SetText ("width");
		fBoundsWidthLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsWidthLabel);

		fBoundsHeightLabel.SetExtent (31, 79, 12, 28, eNoUpdate);
		fBoundsHeightLabel.SetFont (&kApplicationFont);
		fBoundsHeightLabel.SetText ("height");
		fBoundsHeightLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsHeightLabel);

		fBoundsLeftLabel.SetExtent (31, 47, 12, 17, eNoUpdate);
		fBoundsLeftLabel.SetFont (&kApplicationFont);
		fBoundsLeftLabel.SetText ("left");
		fBoundsLeftLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsLeftLabel);

		fWidth.SetExtent (15, 115, 15, 32, eNoUpdate);
		fWidth.SetFont (&kApplicationFont);
		fWidth.SetController (this);
		fWidth.SetMultiLine (False);
		fWidth.SetMinValue (0);
		fBounds.AddSubView (&fWidth);

		fHeight.SetExtent (15, 78, 15, 32, eNoUpdate);
		fHeight.SetFont (&kApplicationFont);
		fHeight.SetController (this);
		fHeight.SetMultiLine (False);
		fHeight.SetMinValue (0);
		fBounds.AddSubView (&fHeight);

		fBoundsTopLabel.SetExtent (31, 13, 12, 16, eNoUpdate);
		fBoundsTopLabel.SetFont (&kApplicationFont);
		fBoundsTopLabel.SetText ("top");
		fBoundsTopLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsTopLabel);

		fLeft.SetExtent (15, 41, 15, 32, eNoUpdate);
		fLeft.SetFont (&kApplicationFont);
		fLeft.SetController (this);
		fLeft.SetMultiLine (False);
		fBounds.AddSubView (&fLeft);

		fTop.SetExtent (15, 4, 15, 32, eNoUpdate);
		fTop.SetFont (&kApplicationFont);
		fTop.SetController (this);
		fTop.SetMultiLine (False);
		fBounds.AddSubView (&fTop);

		fBoundsTabLoop.AddFocus (&fTop);
		fBoundsTabLoop.AddFocus (&fLeft);
		fBoundsTabLoop.AddFocus (&fHeight);
		fBoundsTabLoop.AddFocus (&fWidth);
	AddSubView (&fBounds);

	fFieldLabel.SetExtent (9, 7, 16, 36, eNoUpdate);
	fFieldLabel.SetFont (&kSystemFont);
	fFieldLabel.SetText ("Field:");
	AddSubView (&fFieldLabel);

	fClassLabel.SetExtent (35, 3, 16, 39, eNoUpdate);
	fClassLabel.SetFont (&kSystemFont);
	fClassLabel.SetText ("Class:");
	AddSubView (&fClassLabel);

	fFontButton.SetExtent (35, 242, 21, 62, eNoUpdate);
	fFontButton.SetMargin (2, 5, eNoUpdate);
	fFontButton.SetLabel ("Font", eNoUpdate);
	fFontButton.SetController (this);
	AddSubView (&fFontButton);

	fColor.SetExtent (9, 242, 21, 62, eNoUpdate);
	fColor.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fColor.SetMargin (2, 5, eNoUpdate);
	fColor.SetLabel ("Color", eNoUpdate);
	fColor.SetController (this);
	AddSubView (&fColor);

	AddFocus (&fField);
	AddFocus (&fClass);
	AddFocus (&fBoundsTabLoop);
	AddFocus (&fBorderGroupTabLoop);
	AddFocus (&fMarginGroupTabLoop);
	AddFocus (&fQuickHelpGroupTabLoop);
}

#elif qMotifUI

void	ViewItemInfo_X::BuildForMotifUI ()
{
	SetSize (Point (248, 330), eNoUpdate);

	fQuickHelpGroup.SetExtent (135, 2, 111, 323, eNoUpdate);
	fQuickHelpGroup.SetBorder (1, 1, eNoUpdate);
	fQuickHelpGroup.SetMargin (1, 1, eNoUpdate);
	fQuickHelpGroup.SetFont (&kSystemFont);
	fQuickHelpGroup.SetLabel ("Quick Help", eNoUpdate);
		fQuickHelp.SetExtent (13, 2, 95, 273, eNoUpdate);
		fQuickHelp.SetBorder (0, 0, eNoUpdate);
		fQuickHelp.SetMargin (0, 0, eNoUpdate);
		fQuickHelp.SetFont (&kApplicationFont);
		fQuickHelp.SetController (this);
		fQuickHelpGroup.AddSubView (&fQuickHelp);

		fQuickHelpGroupTabLoop.AddFocus (&fQuickHelp);
	AddSubView (&fQuickHelpGroup);

	fField.SetExtent (11, 42, 26, 89, eNoUpdate);
	fField.SetFont (&kApplicationFont);
	fField.SetController (this);
	fField.SetWordWrap (False);
	AddSubView (&fField);

	fClass.SetExtent (38, 42, 26, 89, eNoUpdate);
	fClass.SetFont (&kApplicationFont);
	fClass.SetController (this);
	fClass.SetWordWrap (False);
	AddSubView (&fClass);

	fMarginGroup.SetExtent (70, 160, 62, 92, eNoUpdate);
	fMarginGroup.SetBorder (1, 1, eNoUpdate);
	fMarginGroup.SetMargin (1, 1, eNoUpdate);
	fMarginGroup.SetFont (&kSystemFont);
	fMarginGroup.SetLabel ("Margin", eNoUpdate);
		fMarginHeightLabel.SetExtent (41, 2, 18, 43, eNoUpdate);
		fMarginHeightLabel.SetFont (&kApplicationFont);
		fMarginHeightLabel.SetText ("height");
		fMarginGroup.AddSubView (&fMarginHeightLabel);

		fMarginWidthLabel.SetExtent (39, 49, 20, 38, eNoUpdate);
		fMarginWidthLabel.SetFont (&kApplicationFont);
		fMarginWidthLabel.SetText ("width");
		fMarginGroup.AddSubView (&fMarginWidthLabel);

		fMarginHeight.SetExtent (16, 8, 26, 32, eNoUpdate);
		fMarginHeight.SetFont (&kApplicationFont);
		fMarginHeight.SetController (this);
		fMarginHeight.SetMultiLine (False);
		fMarginHeight.SetMinValue (0);
		fMarginGroup.AddSubView (&fMarginHeight);

		fMarginWidth.SetExtent (16, 52, 26, 32, eNoUpdate);
		fMarginWidth.SetFont (&kApplicationFont);
		fMarginWidth.SetController (this);
		fMarginWidth.SetMultiLine (False);
		fMarginWidth.SetMinValue (0);
		fMarginGroup.AddSubView (&fMarginWidth);

		fMarginGroupTabLoop.AddFocus (&fMarginHeight);
		fMarginGroupTabLoop.AddFocus (&fMarginWidth);
	AddSubView (&fMarginGroup);

	fBorderGroup.SetExtent (4, 159, 62, 92, eNoUpdate);
	fBorderGroup.SetBorder (1, 1, eNoUpdate);
	fBorderGroup.SetMargin (1, 1, eNoUpdate);
	fBorderGroup.SetFont (&kSystemFont);
	fBorderGroup.SetLabel ("Border", eNoUpdate);
		fBorderHeightLabel.SetExtent (40, 2, 19, 44, eNoUpdate);
		fBorderHeightLabel.SetFont (&kApplicationFont);
		fBorderHeightLabel.SetText ("height");
		fBorderGroup.AddSubView (&fBorderHeightLabel);

		fBorderWidth.SetExtent (16, 52, 26, 32, eNoUpdate);
		fBorderWidth.SetFont (&kApplicationFont);
		fBorderWidth.SetController (this);
		fBorderWidth.SetMultiLine (False);
		fBorderWidth.SetMinValue (0);
		fBorderGroup.AddSubView (&fBorderWidth);

		fBorderHeight.SetExtent (16, 9, 26, 32, eNoUpdate);
		fBorderHeight.SetFont (&kApplicationFont);
		fBorderHeight.SetController (this);
		fBorderHeight.SetMultiLine (False);
		fBorderHeight.SetMinValue (0);
		fBorderGroup.AddSubView (&fBorderHeight);

		fBorderWidthLabel.SetExtent (41, 49, 18, 40, eNoUpdate);
		fBorderWidthLabel.SetFont (&kApplicationFont);
		fBorderWidthLabel.SetText ("width");
		fBorderGroup.AddSubView (&fBorderWidthLabel);

		fBorderGroupTabLoop.AddFocus (&fBorderHeight);
		fBorderGroupTabLoop.AddFocus (&fBorderWidth);
	AddSubView (&fBorderGroup);

	fBounds.SetExtent (69, 3, 61, 151, eNoUpdate);
	fBounds.SetBorder (1, 1, eNoUpdate);
	fBounds.SetMargin (1, 1, eNoUpdate);
	fBounds.SetFont (&kSystemFont);
	fBounds.SetLabel ("Bounds", eNoUpdate);
		fBoundsWidthLabel.SetExtent (42, 113, 18, 38, eNoUpdate);
		fBoundsWidthLabel.SetFont (&kApplicationFont);
		fBoundsWidthLabel.SetText ("width");
		fBoundsWidthLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsWidthLabel);

		fBoundsHeightLabel.SetExtent (42, 73, 18, 42, eNoUpdate);
		fBoundsHeightLabel.SetFont (&kApplicationFont);
		fBoundsHeightLabel.SetText ("height");
		fBoundsHeightLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsHeightLabel);

		fBoundsLeftLabel.SetExtent (42, 41, 18, 32, eNoUpdate);
		fBoundsLeftLabel.SetFont (&kApplicationFont);
		fBoundsLeftLabel.SetText ("left");
		fBoundsLeftLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsLeftLabel);

		fWidth.SetExtent (15, 115, 26, 32, eNoUpdate);
		fWidth.SetFont (&kApplicationFont);
		fWidth.SetController (this);
		fWidth.SetMultiLine (False);
		fWidth.SetMinValue (0);
		fBounds.AddSubView (&fWidth);

		fHeight.SetExtent (15, 78, 26, 32, eNoUpdate);
		fHeight.SetFont (&kApplicationFont);
		fHeight.SetController (this);
		fHeight.SetMultiLine (False);
		fHeight.SetMinValue (0);
		fBounds.AddSubView (&fHeight);

		fBoundsTopLabel.SetExtent (42, 7, 18, 25, eNoUpdate);
		fBoundsTopLabel.SetFont (&kApplicationFont);
		fBoundsTopLabel.SetText ("top");
		fBoundsTopLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsTopLabel);

		fLeft.SetExtent (15, 41, 26, 32, eNoUpdate);
		fLeft.SetFont (&kApplicationFont);
		fLeft.SetController (this);
		fLeft.SetMultiLine (False);
		fBounds.AddSubView (&fLeft);

		fTop.SetExtent (15, 4, 26, 32, eNoUpdate);
		fTop.SetFont (&kApplicationFont);
		fTop.SetController (this);
		fTop.SetMultiLine (False);
		fBounds.AddSubView (&fTop);

		fBoundsTabLoop.AddFocus (&fTop);
		fBoundsTabLoop.AddFocus (&fLeft);
		fBoundsTabLoop.AddFocus (&fHeight);
		fBoundsTabLoop.AddFocus (&fWidth);
	AddSubView (&fBounds);

	fFieldLabel.SetExtent (12, 4, 21, 35, eNoUpdate);
	fFieldLabel.SetFont (&kSystemFont);
	fFieldLabel.SetText ("Field:");
	AddSubView (&fFieldLabel);

	fClassLabel.SetExtent (38, 0, 21, 41, eNoUpdate);
	fClassLabel.SetFont (&kSystemFont);
	fClassLabel.SetText ("Class:");
	AddSubView (&fClassLabel);

	fFontButton.SetExtent (32, 256, 24, 70, eNoUpdate);
	fFontButton.SetLabel ("Font", eNoUpdate);
	fFontButton.SetController (this);
	AddSubView (&fFontButton);

	fColor.SetExtent (6, 256, 24, 70, eNoUpdate);
	fColor.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fColor.SetLabel ("Color", eNoUpdate);
	fColor.SetController (this);
	AddSubView (&fColor);

	AddFocus (&fField);
	AddFocus (&fClass);
	AddFocus (&fBoundsTabLoop);
	AddFocus (&fBorderGroupTabLoop);
	AddFocus (&fMarginGroupTabLoop);
	AddFocus (&fColor);
	AddFocus (&fFontButton);
	AddFocus (&fQuickHelpGroupTabLoop);
}

#else

void	ViewItemInfo_X::BuildForUnknownGUI ();
{
	SetSize (Point (212, 309), eNoUpdate);

	fQuickHelpGroup.SetExtent (110, 2, 100, 304, eNoUpdate);
	fQuickHelpGroup.SetBorder (1, 1, eNoUpdate);
	fQuickHelpGroup.SetMargin (1, 1, eNoUpdate);
	fQuickHelpGroup.SetFont (&kSystemFont);
	fQuickHelpGroup.SetLabel ("Quick Help", eNoUpdate);
		fQuickHelp.SetExtent (13, 2, 84, 299, eNoUpdate);
		fQuickHelp.SetBorder (0, 0, eNoUpdate);
		fQuickHelp.SetFont (&kApplicationFont);
		fQuickHelp.SetController (this);
		fQuickHelpGroup.AddSubView (&fQuickHelp);

		fQuickHelpGroupTabLoop.AddFocus (&fQuickHelp);
	AddSubView (&fQuickHelpGroup);

	fField.SetExtent (8, 45, 17, 108, eNoUpdate);
	fField.SetMargin (1, 1, eNoUpdate);
	fField.SetFont (&kApplicationFont);
	fField.SetController (this);
	fField.SetWordWrap (False);
	fField.SetMultiLine (False);
	AddSubView (&fField);

	fClass.SetExtent (35, 45, 17, 109, eNoUpdate);
	fClass.SetMargin (1, 1, eNoUpdate);
	fClass.SetFont (&kApplicationFont);
	fClass.SetController (this);
	fClass.SetWordWrap (False);
	fClass.SetMultiLine (False);
	AddSubView (&fClass);

	fMarginGroup.SetExtent (55, 163, 50, 76, eNoUpdate);
	fMarginGroup.SetBorder (1, 1, eNoUpdate);
	fMarginGroup.SetMargin (1, 1, eNoUpdate);
	fMarginGroup.SetFont (&kSystemFont);
	fMarginGroup.SetLabel ("Margin", eNoUpdate);
		fMarginHeightLabel.SetExtent (34, 5, 12, 28, eNoUpdate);
		fMarginHeightLabel.SetFont (&kApplicationFont);
		fMarginHeightLabel.SetText ("height");
		fMarginGroup.AddSubView (&fMarginHeightLabel);

		fMarginWidthLabel.SetExtent (34, 40, 12, 27, eNoUpdate);
		fMarginWidthLabel.SetFont (&kApplicationFont);
		fMarginWidthLabel.SetText ("width");
		fMarginGroup.AddSubView (&fMarginWidthLabel);

		fMarginHeight.SetExtent (16, 3, 16, 32, eNoUpdate);
		fMarginHeight.SetFont (&kApplicationFont);
		fMarginHeight.SetController (this);
		fMarginHeight.SetMultiLine (False);
		fMarginHeight.SetMinValue (0);
		fMarginGroup.AddSubView (&fMarginHeight);

		fMarginWidth.SetExtent (16, 38, 16, 32, eNoUpdate);
		fMarginWidth.SetFont (&kApplicationFont);
		fMarginWidth.SetController (this);
		fMarginWidth.SetMultiLine (False);
		fMarginWidth.SetMinValue (0);
		fMarginGroup.AddSubView (&fMarginWidth);

		fMarginGroupTabLoop.AddFocus (&fMarginHeight);
		fMarginGroupTabLoop.AddFocus (&fMarginWidth);
	AddSubView (&fMarginGroup);

	fBorderGroup.SetExtent (3, 163, 49, 76, eNoUpdate);
	fBorderGroup.SetBorder (1, 1, eNoUpdate);
	fBorderGroup.SetMargin (1, 1, eNoUpdate);
	fBorderGroup.SetFont (&kSystemFont);
	fBorderGroup.SetLabel ("Border", eNoUpdate);
		fBorderHeightLabel.SetExtent (33, 5, 12, 27, eNoUpdate);
		fBorderHeightLabel.SetFont (&kApplicationFont);
		fBorderHeightLabel.SetText ("height");
		fBorderGroup.AddSubView (&fBorderHeightLabel);

		fBorderWidth.SetExtent (15, 39, 16, 32, eNoUpdate);
		fBorderWidth.SetFont (&kApplicationFont);
		fBorderWidth.SetController (this);
		fBorderWidth.SetMultiLine (False);
		fBorderWidth.SetMinValue (0);
		fBorderGroup.AddSubView (&fBorderWidth);

		fBorderHeight.SetExtent (15, 3, 16, 32, eNoUpdate);
		fBorderHeight.SetFont (&kApplicationFont);
		fBorderHeight.SetController (this);
		fBorderHeight.SetMultiLine (False);
		fBorderHeight.SetMinValue (0);
		fBorderGroup.AddSubView (&fBorderHeight);

		fBorderWidthLabel.SetExtent (33, 41, 12, 27, eNoUpdate);
		fBorderWidthLabel.SetFont (&kApplicationFont);
		fBorderWidthLabel.SetText ("width");
		fBorderGroup.AddSubView (&fBorderWidthLabel);

		fBorderGroupTabLoop.AddFocus (&fBorderHeight);
		fBorderGroupTabLoop.AddFocus (&fBorderWidth);
	AddSubView (&fBorderGroup);

	fBounds.SetExtent (57, 3, 48, 151, eNoUpdate);
	fBounds.SetBorder (1, 1, eNoUpdate);
	fBounds.SetMargin (1, 1, eNoUpdate);
	fBounds.SetFont (&kSystemFont);
	fBounds.SetLabel ("Bounds", eNoUpdate);
		fBoundsWidthLabel.SetExtent (32, 119, 12, 26, eNoUpdate);
		fBoundsWidthLabel.SetFont (&kApplicationFont);
		fBoundsWidthLabel.SetText ("width");
		fBoundsWidthLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsWidthLabel);

		fBoundsHeightLabel.SetExtent (31, 79, 12, 28, eNoUpdate);
		fBoundsHeightLabel.SetFont (&kApplicationFont);
		fBoundsHeightLabel.SetText ("height");
		fBoundsHeightLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsHeightLabel);

		fBoundsLeftLabel.SetExtent (31, 47, 12, 17, eNoUpdate);
		fBoundsLeftLabel.SetFont (&kApplicationFont);
		fBoundsLeftLabel.SetText ("left");
		fBoundsLeftLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsLeftLabel);

		fWidth.SetExtent (15, 115, 15, 32, eNoUpdate);
		fWidth.SetFont (&kApplicationFont);
		fWidth.SetController (this);
		fWidth.SetMultiLine (False);
		fWidth.SetMinValue (0);
		fBounds.AddSubView (&fWidth);

		fHeight.SetExtent (15, 78, 15, 32, eNoUpdate);
		fHeight.SetFont (&kApplicationFont);
		fHeight.SetController (this);
		fHeight.SetMultiLine (False);
		fHeight.SetMinValue (0);
		fBounds.AddSubView (&fHeight);

		fBoundsTopLabel.SetExtent (31, 13, 12, 16, eNoUpdate);
		fBoundsTopLabel.SetFont (&kApplicationFont);
		fBoundsTopLabel.SetText ("top");
		fBoundsTopLabel.SetJustification (AbstractTextView::eJustCenter);
		fBounds.AddSubView (&fBoundsTopLabel);

		fLeft.SetExtent (15, 41, 15, 32, eNoUpdate);
		fLeft.SetFont (&kApplicationFont);
		fLeft.SetController (this);
		fLeft.SetMultiLine (False);
		fBounds.AddSubView (&fLeft);

		fTop.SetExtent (15, 4, 15, 32, eNoUpdate);
		fTop.SetFont (&kApplicationFont);
		fTop.SetController (this);
		fTop.SetMultiLine (False);
		fBounds.AddSubView (&fTop);

		fBoundsTabLoop.AddFocus (&fTop);
		fBoundsTabLoop.AddFocus (&fLeft);
		fBoundsTabLoop.AddFocus (&fHeight);
		fBoundsTabLoop.AddFocus (&fWidth);
	AddSubView (&fBounds);

	fFieldLabel.SetExtent (9, 7, 16, 36, eNoUpdate);
	fFieldLabel.SetFont (&kSystemFont);
	fFieldLabel.SetText ("Field:");
	AddSubView (&fFieldLabel);

	fClassLabel.SetExtent (35, 3, 16, 39, eNoUpdate);
	fClassLabel.SetFont (&kSystemFont);
	fClassLabel.SetText ("Class:");
	AddSubView (&fClassLabel);

	fFontButton.SetExtent (35, 242, 21, 62, eNoUpdate);
	fFontButton.SetMargin (2, 5, eNoUpdate);
	fFontButton.SetLabel ("Font", eNoUpdate);
	fFontButton.SetController (this);
	AddSubView (&fFontButton);

	fColor.SetExtent (9, 242, 21, 62, eNoUpdate);
	fColor.SetEnabled (not EnableItem::kEnabled, eNoUpdate);
	fColor.SetMargin (2, 5, eNoUpdate);
	fColor.SetLabel ("Color", eNoUpdate);
	fColor.SetController (this);
	AddSubView (&fColor);

	AddFocus (&fField);
	AddFocus (&fClass);
	AddFocus (&fBoundsTabLoop);
	AddFocus (&fBorderGroupTabLoop);
	AddFocus (&fMarginGroupTabLoop);
	AddFocus (&fQuickHelpGroupTabLoop);
}

#endif /* GUI */

Point	ViewItemInfo_X::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (212, 309));
#elif   qMotifUI
	return (Point (248, 330));
#else
	return (Point (212, 309));
#endif /* GUI */
}

void	ViewItemInfo_X::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfQuickHelpGroupSize = fQuickHelpGroup.GetSize ();
		fQuickHelpGroup.SetSize (kOriginalfQuickHelpGroupSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
#if   qMacUI
		const Point fQuickHelpGroupSizeDelta = Point (100,304) - fQuickHelpGroup.GetSize ();
#elif   qMotifUI
		const Point fQuickHelpGroupSizeDelta = Point (111,323) - fQuickHelpGroup.GetSize ();
#else
		const Point fQuickHelpGroupSizeDelta = Point (100,304) - fQuickHelpGroup.GetSize ();
#endif /* GUI */
			static const Point	kOriginalfQuickHelpSize = fQuickHelp.GetSize ();
			fQuickHelp.SetSize (kOriginalfQuickHelpSize - Point (fQuickHelpGroupSizeDelta.GetV (), fQuickHelpGroupSizeDelta.GetH ()));
		static const Point	kOriginalfFieldSize = fField.GetSize ();
		fField.SetSize (kOriginalfFieldSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfClassSize = fClass.GetSize ();
		fClass.SetSize (kOriginalfClassSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfMarginGroupOrigin = fMarginGroup.GetOrigin ();
		fMarginGroup.SetOrigin (kOriginalfMarginGroupOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfBorderGroupOrigin = fBorderGroup.GetOrigin ();
		fBorderGroup.SetOrigin (kOriginalfBorderGroupOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfFontButtonOrigin = fFontButton.GetOrigin ();
		fFontButton.SetOrigin (kOriginalfFontButtonOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfColorOrigin = fColor.GetOrigin ();
		fColor.SetOrigin (kOriginalfColorOrigin - Point (0, kSizeDelta.GetH ()));
	GroupView::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"EmilyWindow.hh"


ViewItemInfo::ViewItemInfo () :
	fFont (Nil),
	fItem (Nil)
{
	GetField ().SetTextRequired (False);
	GetClass ().SetEnabled (EmilyWindow::GetFullEditing ());
	GetField ().SetEnabled (EmilyWindow::GetFullEditing ());
}

ViewItemInfo::~ViewItemInfo ()
{
	delete fFont;
}

void	ViewItemInfo::SetUpFromView (class ViewItem& item)
{
	Require (fItem == Nil);
	
	fItem = &item;
	
	Point	oldOrigin = item.LocalToEnclosure (item.GetOwnedView ().GetOrigin ());
	Point	oldSize	  = item.GetOwnedView ().GetSize ();

	GetClass ().SetText (item.GetFieldClass ());
	GetField ().SetText (item.GetFieldName ());
	GetTop ().SetValue (oldOrigin.GetV ());
	GetLeft ().SetValue (oldOrigin.GetH ());
	GetHeight ().SetValue (oldSize.GetV ());
	GetWidth ().SetValue (oldSize.GetH ());
	if (item.GetItemBorder () == Point (-100, -100)) {	// change to use constant
		GetBorderHeight ().SetValue (0);
		GetBorderWidth ().SetValue (0);
		GetMarginHeight ().SetValue (0);
		GetMarginWidth ().SetValue (0);
//		GetBorderHeight ().SetEnabled (False);
//		GetBorderWidth ().SetEnabled (False);
//		GetMarginHeight ().SetEnabled (False);
//		GetMarginWidth ().SetEnabled (False);
	}
	else {
		GetBorderHeight ().SetValue (item.GetItemBorder ().GetV ());
		GetBorderWidth ().SetValue (item.GetItemBorder ().GetH ());
		GetMarginHeight ().SetValue (item.GetItemMargin ().GetV ());
		GetMarginWidth ().SetValue (item.GetItemMargin ().GetH ());
	}
	GetQuickHelp ().SetText (item.GetHelp ());
	
	GetTop ().SetEnabled (item.CanChangeExtent ());
	GetLeft ().SetEnabled (item.CanChangeExtent ());
	GetHeight ().SetEnabled (item.CanChangeExtent ());
	GetWidth ().SetEnabled (item.CanChangeExtent ());

	if (item.GetFont () != Nil) {
		fFont = new Font (*item.GetFont ());
	}
}

void	ViewItemInfo::ButtonPressed (AbstractButton* button)
{
	if (button == &fFontButton) {
		FontPicker	picker = FontPicker (fFont);
		Dialog d = Dialog (&picker, &picker, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
		d.SetDefaultButton (d.GetOKButton ());
	
		if (d.Pose ()) {
			delete fFont;
			if (picker.GetInheritFont ().GetOn ()) {
				fFont = Nil;
			}
			else {
				fFont = new Font (picker.fFont);
			}
		}
	}
	else {
		ViewItemInfo_X::ButtonPressed (button);
	}
}



SetItemInfoCommand::SetItemInfoCommand (class ViewItem& item, ViewItemInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fNewClassName (info.GetClass ().GetText ()),
	fOldClassName (item.GetFieldClass ()),
	fNewFieldName (info.GetField ().GetText ()),
	fOldFieldName (item.GetFieldName ()),
	fNewOrigin (Point (info.GetTop ().GetValue (), info.GetLeft ().GetValue ())),
	fOldOrigin (item.GetItemOrigin ()),
	fNewSize (Point (info.GetHeight ().GetValue (), info.GetWidth ().GetValue ())),
	fOldSize (item.GetItemSize ()),
	fOldBorder (item.GetItemBorder ()),
	fNewBorder (Point (info.GetBorderHeight ().GetValue (), info.GetBorderWidth ().GetValue ())),
	fOldMargin (item.GetItemMargin ()),
	fNewMargin (Point (info.GetMarginHeight ().GetValue (), info.GetMarginWidth ().GetValue ())),
	fNewHelp (info.GetQuickHelp ().GetText ()),
	fOldHelp (item.GetHelp ()),
	fNewFont (info.fFont),
	fOldFont (item.GetItemFont ())
{
	if (fNewFont != Nil) {
		fNewFont = new Font (*fNewFont);
	}
	if (fOldFont != Nil) {
		fOldFont = new Font (*fOldFont);
	}
	
	if (not info.GetMarginHeight ().GetEnabled ()) {
		fNewMargin = fOldMargin;
	}
	if (not info.GetBorderHeight ().GetEnabled ()) {
		fNewBorder = fOldBorder;
	}
}			
		
SetItemInfoCommand::~SetItemInfoCommand ()
{
	delete fNewFont;
	delete fOldFont;
}
		
void	SetItemInfoCommand::DoIt ()
{
	fItem.SetFieldClass (fNewClassName);
	fItem.SetFieldName (fNewFieldName);
	fItem.SetHelp (fNewHelp);
	fItem.SetItemOrigin (fNewOrigin);
	fItem.SetItemSize (fNewSize);
	fItem.SetItemBorder (fNewBorder);
	fItem.SetItemMargin (fNewMargin);
	if (fNewFont != fOldFont) {
		fItem.SetItemFont (fNewFont);
	}
	fItem.ApplyCurrentParams ();
	if (fNewFont != fOldFont) {
		fItem.Refresh ();
	}
	
	Command::DoIt ();
}
		
void	SetItemInfoCommand::UnDoIt ()
{
	fItem.SetFieldClass (fOldClassName);
	fItem.SetFieldName (fOldFieldName);
	fItem.SetHelp (fOldHelp);
	fItem.SetItemOrigin (fOldOrigin);
	fItem.SetItemSize (fOldSize);
	fItem.SetItemBorder (fOldBorder);
	fItem.SetItemMargin (fOldMargin);
	if (fNewFont != fOldFont) {
		fItem.SetItemFont (fOldFont);
	}
	fItem.ApplyCurrentParams ();
	if (fNewFont != fOldFont) {
		fItem.Refresh ();
	}
	
	Command::UnDoIt ();
}
	

