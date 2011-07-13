/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ClassInfo.cc,v 1.6 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ClassInfo.cc,v $
 *		Revision 1.6  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */





#include	"CheckBox.hh"
#include	"GroupView.hh"

#include 	"Shape.hh"
#include	"CommandNumbers.hh"
#include	"ViewItem.hh"

#include	"FontPicker.hh"

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "ClassInfo.hh"


ClassInfoX::ClassInfoX () :
	fTitle (),
	fQuickHelpGroup (kEmptyString),
	fClass (),
	fClassLabel (),
	fField1 (),
	fBaseClass (),
	fAutoSize (),
	fFontButton (),
	fScrollBounds (kEmptyString)
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

ClassInfoX::~ClassInfoX ()
{
	RemoveFocus (&fClass);
	RemoveFocus (&fBaseClass);
	RemoveFocus (&fScrollBoundsTabLoop);
	RemoveFocus (&fQuickHelpGroupTabLoop);

	RemoveSubView (&fTitle);
	RemoveSubView (&fQuickHelpGroup);
		fQuickHelpGroupTabLoop.RemoveFocus (&fQuickHelp);

		fQuickHelpGroup.RemoveSubView (&fQuickHelp);
	RemoveSubView (&fClass);
	RemoveSubView (&fClassLabel);
	RemoveSubView (&fField1);
	RemoveSubView (&fBaseClass);
	RemoveSubView (&fAutoSize);
	RemoveSubView (&fFontButton);
	RemoveSubView (&fScrollBounds);
		fScrollBoundsTabLoop.RemoveFocus (&fScrollHeight);
		fScrollBoundsTabLoop.RemoveFocus (&fScrollWidth);

		fScrollBounds.RemoveSubView (&fScrollWidth);
		fScrollBounds.RemoveSubView (&fScrollHeight);
		fScrollBounds.RemoveSubView (&fScrollWidthLabel);
		fScrollBounds.RemoveSubView (&fScrollHeightLabel);
}

#if   qMacUI

void	ClassInfoX::BuildForMacUI ()
{
	SetSize (Point (201, 303), eNoUpdate);

	fTitle.SetExtent (3, 6, 15, 289, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Class Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fQuickHelpGroup.SetExtent (96, 0, 102, 299, eNoUpdate);
	fQuickHelpGroup.SetFont (&kSystemFont);
	fQuickHelpGroup.SetLabel ("Quick Help", eNoUpdate);
		fQuickHelp.SetExtent (14, 1, 79, 297, eNoUpdate);
		fQuickHelp.SetBorder (0, 0, eNoUpdate);
		fQuickHelp.SetFont (&kApplicationFont);
		fQuickHelp.SetController (this);
		fQuickHelpGroup.AddSubView (&fQuickHelp);

		fQuickHelpGroupTabLoop.AddFocus (&fQuickHelp);
	AddSubView (&fQuickHelpGroup);

	fClass.SetExtent (24, 83, 18, 105, eNoUpdate);
	fClass.SetFont (&kApplicationFont);
	fClass.SetController (this);
	fClass.SetWordWrap (False);
	AddSubView (&fClass);

	fClassLabel.SetExtent (26, 43, 15, 38, eNoUpdate);
	fClassLabel.SetFont (&kSystemFont);
	fClassLabel.SetText ("Class:");
	AddSubView (&fClassLabel);

	fField1.SetExtent (48, 7, 15, 73, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Base Class:");
	AddSubView (&fField1);

	fBaseClass.SetExtent (47, 82, 18, 106, eNoUpdate);
	fBaseClass.SetFont (&kApplicationFont);
	fBaseClass.SetController (this);
	fBaseClass.SetWordWrap (False);
	AddSubView (&fBaseClass);

	fAutoSize.SetLabel ("Autosize", eNoUpdate);
	fAutoSize.SetExtent (76, 167, 15, 74, eNoUpdate);
	fAutoSize.SetController (this);
	AddSubView (&fAutoSize);

	fFontButton.SetExtent (74, 87, 20, 60, eNoUpdate);
	fFontButton.SetLabel ("Font", eNoUpdate);
	fFontButton.SetController (this);
	AddSubView (&fFontButton);

	fScrollBounds.SetExtent (17, 199, 50, 96, eNoUpdate);
	fScrollBounds.SetFont (&kSystemFont);
	fScrollBounds.SetLabel ("Scroll Bounds", eNoUpdate);
		fScrollWidth.SetExtent (17, 52, 16, 32, eNoUpdate);
		fScrollWidth.SetFont (&kApplicationFont);
		fScrollWidth.SetController (this);
		fScrollWidth.SetMultiLine (False);
		fScrollWidth.SetMinValue (0);
		fScrollBounds.AddSubView (&fScrollWidth);

		fScrollHeight.SetExtent (17, 10, 16, 32, eNoUpdate);
		fScrollHeight.SetFont (&kApplicationFont);
		fScrollHeight.SetController (this);
		fScrollHeight.SetMultiLine (False);
		fScrollHeight.SetMinValue (0);
		fScrollBounds.AddSubView (&fScrollHeight);

		fScrollWidthLabel.SetExtent (34, 53, 12, 30, eNoUpdate);
		fScrollWidthLabel.SetFont (&kApplicationFont);
		fScrollWidthLabel.SetText ("width");
		fScrollWidthLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBounds.AddSubView (&fScrollWidthLabel);

		fScrollHeightLabel.SetExtent (34, 11, 12, 30, eNoUpdate);
		fScrollHeightLabel.SetFont (&kApplicationFont);
		fScrollHeightLabel.SetText ("height");
		fScrollHeightLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBounds.AddSubView (&fScrollHeightLabel);

		fScrollBoundsTabLoop.AddFocus (&fScrollHeight);
		fScrollBoundsTabLoop.AddFocus (&fScrollWidth);
	AddSubView (&fScrollBounds);

	AddFocus (&fClass);
	AddFocus (&fBaseClass);
	AddFocus (&fScrollBoundsTabLoop);
	AddFocus (&fQuickHelpGroupTabLoop);
}

#elif qMotifUI

void	ClassInfoX::BuildForMotifUI ()
{
	SetSize (Point (260, 303), eNoUpdate);

	fTitle.SetExtent (3, 3, 21, 294, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Class Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fQuickHelpGroup.SetExtent (119, 0, 136, 299, eNoUpdate);
	fQuickHelpGroup.SetFont (&kSystemFont);
	fQuickHelpGroup.SetLabel ("Quick Help", eNoUpdate);
		fQuickHelp.SetExtent (14, 1, 118, 297, eNoUpdate);
		fQuickHelp.SetBorder (0, 0, eNoUpdate);
		fQuickHelp.SetMargin (0, 0, eNoUpdate);
		fQuickHelp.SetFont (&kApplicationFont);
		fQuickHelp.SetController (this);
		fQuickHelpGroup.AddSubView (&fQuickHelp);

		fQuickHelpGroupTabLoop.AddFocus (&fQuickHelp);
	AddSubView (&fQuickHelpGroup);

	fClass.SetExtent (30, 80, 26, 105, eNoUpdate);
	fClass.SetFont (&kApplicationFont);
	fClass.SetController (this);
	fClass.SetWordWrap (False);
	AddSubView (&fClass);

	fClassLabel.SetExtent (31, 37, 21, 41, eNoUpdate);
	fClassLabel.SetFont (&kSystemFont);
	fClassLabel.SetText ("Class:");
	AddSubView (&fClassLabel);

	fField1.SetExtent (57, 6, 21, 72, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Base Class:");
	AddSubView (&fField1);

	fBaseClass.SetExtent (57, 80, 26, 106, eNoUpdate);
	fBaseClass.SetFont (&kApplicationFont);
	fBaseClass.SetController (this);
	fBaseClass.SetWordWrap (False);
	AddSubView (&fBaseClass);

	fAutoSize.SetLabel ("Autosize", eNoUpdate);
	fAutoSize.SetExtent (94, 166, 25, 86, eNoUpdate);
	fAutoSize.SetController (this);
	AddSubView (&fAutoSize);

	fFontButton.SetExtent (94, 86, 24, 70, eNoUpdate);
	fFontButton.SetLabel ("Font", eNoUpdate);
	fFontButton.SetController (this);
	AddSubView (&fFontButton);

	fScrollBounds.SetExtent (23, 197, 64, 96, eNoUpdate);
	fScrollBounds.SetFont (&kSystemFont);
	fScrollBounds.SetLabel ("Scroll Bounds", eNoUpdate);
		fScrollWidth.SetExtent (17, 55, 26, 32, eNoUpdate);
		fScrollWidth.SetFont (&kApplicationFont);
		fScrollWidth.SetController (this);
		fScrollWidth.SetMultiLine (False);
		fScrollWidth.SetMinValue (0);
		fScrollBounds.AddSubView (&fScrollWidth);

		fScrollHeight.SetExtent (17, 10, 26, 32, eNoUpdate);
		fScrollHeight.SetFont (&kApplicationFont);
		fScrollHeight.SetController (this);
		fScrollHeight.SetMultiLine (False);
		fScrollHeight.SetMinValue (0);
		fScrollBounds.AddSubView (&fScrollHeight);

		fScrollWidthLabel.SetExtent (40, 52, 20, 37, eNoUpdate);
		fScrollWidthLabel.SetFont (&kApplicationFont);
		fScrollWidthLabel.SetText ("width");
		fScrollWidthLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBounds.AddSubView (&fScrollWidthLabel);

		fScrollHeightLabel.SetExtent (40, 3, 20, 43, eNoUpdate);
		fScrollHeightLabel.SetFont (&kApplicationFont);
		fScrollHeightLabel.SetText ("height");
		fScrollHeightLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBounds.AddSubView (&fScrollHeightLabel);

		fScrollBoundsTabLoop.AddFocus (&fScrollHeight);
		fScrollBoundsTabLoop.AddFocus (&fScrollWidth);
	AddSubView (&fScrollBounds);

	AddFocus (&fClass);
	AddFocus (&fBaseClass);
	AddFocus (&fScrollBoundsTabLoop);
	AddFocus (&fFontButton);
	AddFocus (&fQuickHelpGroupTabLoop);
	AddFocus (&fAutoSize);
}

#else

void	ClassInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (201, 303), eNoUpdate);

	fTitle.SetExtent (3, 6, 15, 289, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Class Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fQuickHelpGroup.SetExtent (96, 0, 102, 299, eNoUpdate);
	fQuickHelpGroup.SetFont (&kSystemFont);
	fQuickHelpGroup.SetLabel ("Quick Help", eNoUpdate);
		fQuickHelp.SetExtent (14, 1, 79, 297, eNoUpdate);
		fQuickHelp.SetBorder (0, 0, eNoUpdate);
		fQuickHelp.SetFont (&kApplicationFont);
		fQuickHelp.SetController (this);
		fQuickHelpGroup.AddSubView (&fQuickHelp);

		fQuickHelpGroupTabLoop.AddFocus (&fQuickHelp);
	AddSubView (&fQuickHelpGroup);

	fClass.SetExtent (24, 83, 18, 105, eNoUpdate);
	fClass.SetFont (&kApplicationFont);
	fClass.SetController (this);
	fClass.SetWordWrap (False);
	AddSubView (&fClass);

	fClassLabel.SetExtent (26, 43, 15, 38, eNoUpdate);
	fClassLabel.SetFont (&kSystemFont);
	fClassLabel.SetText ("Class:");
	AddSubView (&fClassLabel);

	fField1.SetExtent (48, 7, 15, 73, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Base Class:");
	AddSubView (&fField1);

	fBaseClass.SetExtent (47, 82, 18, 106, eNoUpdate);
	fBaseClass.SetFont (&kApplicationFont);
	fBaseClass.SetController (this);
	fBaseClass.SetWordWrap (False);
	AddSubView (&fBaseClass);

	fAutoSize.SetLabel ("Autosize", eNoUpdate);
	fAutoSize.SetExtent (76, 167, 15, 74, eNoUpdate);
	fAutoSize.SetController (this);
	AddSubView (&fAutoSize);

	fFontButton.SetExtent (74, 87, 20, 60, eNoUpdate);
	fFontButton.SetLabel ("Font", eNoUpdate);
	fFontButton.SetController (this);
	AddSubView (&fFontButton);

	fScrollBounds.SetExtent (17, 199, 50, 96, eNoUpdate);
	fScrollBounds.SetFont (&kSystemFont);
	fScrollBounds.SetLabel ("Scroll Bounds", eNoUpdate);
		fScrollWidth.SetExtent (17, 52, 16, 32, eNoUpdate);
		fScrollWidth.SetFont (&kApplicationFont);
		fScrollWidth.SetController (this);
		fScrollWidth.SetMultiLine (False);
		fScrollWidth.SetMinValue (0);
		fScrollBounds.AddSubView (&fScrollWidth);

		fScrollHeight.SetExtent (17, 10, 16, 32, eNoUpdate);
		fScrollHeight.SetFont (&kApplicationFont);
		fScrollHeight.SetController (this);
		fScrollHeight.SetMultiLine (False);
		fScrollHeight.SetMinValue (0);
		fScrollBounds.AddSubView (&fScrollHeight);

		fScrollWidthLabel.SetExtent (34, 53, 12, 30, eNoUpdate);
		fScrollWidthLabel.SetFont (&kApplicationFont);
		fScrollWidthLabel.SetText ("width");
		fScrollWidthLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBounds.AddSubView (&fScrollWidthLabel);

		fScrollHeightLabel.SetExtent (34, 11, 12, 30, eNoUpdate);
		fScrollHeightLabel.SetFont (&kApplicationFont);
		fScrollHeightLabel.SetText ("height");
		fScrollHeightLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBounds.AddSubView (&fScrollHeightLabel);

		fScrollBoundsTabLoop.AddFocus (&fScrollHeight);
		fScrollBoundsTabLoop.AddFocus (&fScrollWidth);
	AddSubView (&fScrollBounds);

	AddFocus (&fClass);
	AddFocus (&fBaseClass);
	AddFocus (&fScrollBoundsTabLoop);
	AddFocus (&fQuickHelpGroupTabLoop);
}

#endif /* GUI */

Point	ClassInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (201, 303));
#elif   qMotifUI
	return (Point (260, 303));
#else
	return (Point (201, 303));
#endif /* GUI */
}

void	ClassInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfQuickHelpGroupSize = fQuickHelpGroup.GetSize ();
		fQuickHelpGroup.SetSize (kOriginalfQuickHelpGroupSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
#if   qMacUI
		const Point fQuickHelpGroupSizeDelta = Point (102,299) - fQuickHelpGroup.GetSize ();
#elif   qMotifUI
		const Point fQuickHelpGroupSizeDelta = Point (136,299) - fQuickHelpGroup.GetSize ();
#else
		const Point fQuickHelpGroupSizeDelta = Point (102,299) - fQuickHelpGroup.GetSize ();
#endif /* GUI */
			static const Point	kOriginalfQuickHelpSize = fQuickHelp.GetSize ();
			fQuickHelp.SetSize (kOriginalfQuickHelpSize - Point (fQuickHelpGroupSizeDelta.GetV (), fQuickHelpGroupSizeDelta.GetH ()));
		static const Point	kOriginalfClassSize = fClass.GetSize ();
		fClass.SetSize (kOriginalfClassSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfBaseClassSize = fBaseClass.GetSize ();
		fBaseClass.SetSize (kOriginalfBaseClassSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfScrollBoundsOrigin = fScrollBounds.GetOrigin ();
		fScrollBounds.SetOrigin (kOriginalfScrollBoundsOrigin - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"Dialog.hh"
#include	"EmilyWindow.hh"

ClassInfo::ClassInfo (View& view) :
	fView (view),
	fFont (Nil)
{
	if (fView.GetFont () != Nil) {
		fFont = new Font (*fView.GetFont ());
	}
	
	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetClassNameField ().SetEnabled (fullEditing);
	GetBaseClassNameField ().SetEnabled (fullEditing);
}
		
ClassInfo::~ClassInfo ()
{
	delete fFont;
}

void	ClassInfo::ButtonPressed (AbstractButton* button)
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
	else if (button == &fAutoSize) {
		GetSizeVField ().SetEnabled (Boolean (not GetAutoSizeField ().GetOn ()));
		GetSizeHField ().SetEnabled (Boolean (not GetAutoSizeField ().GetOn ()));
	}
	else {
		ClassInfoX::ButtonPressed (button);
	}
}
	
