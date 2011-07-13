/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollBarInfo.cc,v 1.2 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollBarInfo.cc,v $
 *		Revision 1.2  1992/07/21  18:28:39  sterling
 *		hi
 *
// Revision 1.1  1992/07/16  15:24:40  sterling
// hi
//
 *		
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "ScrollBarInfo.hh"


ScrollBarInfoX::ScrollBarInfoX () :
	fTitle (),
	fMaximumLabel (),
	fField1 (kEmptyString),
	fMaximum (),
	fValue (),
	fMinLabel (),
	fValueLabel (),
	fStepSizeLabel (),
	fPageSizeLabel (),
	fStepSize (),
	fPageSize (),
	fMinimum (),
	fViewInfo ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

ScrollBarInfoX::~ScrollBarInfoX ()
{
	RemoveFocus (&fValue);
	RemoveFocus (&fMinimum);
	RemoveFocus (&fMaximum);
	RemoveFocus (&fStepSize);
	RemoveFocus (&fPageSize);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fMaximumLabel);
	RemoveSubView (&fField1);
		fField1.RemoveSubView (&fVertical);
		fField1RadioBank.RemoveRadioButton (&fVertical);
		fField1.RemoveSubView (&fHorizontal);
		fField1RadioBank.RemoveRadioButton (&fHorizontal);
	RemoveSubView (&fMaximum);
	RemoveSubView (&fValue);
	RemoveSubView (&fMinLabel);
	RemoveSubView (&fValueLabel);
	RemoveSubView (&fStepSizeLabel);
	RemoveSubView (&fPageSizeLabel);
	RemoveSubView (&fStepSize);
	RemoveSubView (&fPageSize);
	RemoveSubView (&fMinimum);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	ScrollBarInfoX::BuildForMacUI ()
{
	SetSize (Point (300, 350), eNoUpdate);

	fTitle.SetExtent (3, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set ScrollBar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fMaximumLabel.SetExtent (67, 5, 15, 64, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum");
	AddSubView (&fMaximumLabel);

	fField1.SetExtent (22, 229, 50, 116, eNoUpdate);
	fField1.SetLabel ("Orientation", eNoUpdate);
		fField1RadioBank.SetController (this);

		fVertical.SetLabel ("Vertical", eNoUpdate);
		fField1RadioBank.AddRadioButton (&fVertical);
		fVertical.SetExtent (13, 5, 15, 70, eNoUpdate);
		fField1.AddSubView (&fVertical);

		fHorizontal.SetLabel ("Horizontal", eNoUpdate);
		fField1RadioBank.AddRadioButton (&fHorizontal);
		fHorizontal.SetExtent (30, 5, 15, 87, eNoUpdate);
		fField1.AddSubView (&fHorizontal);

	AddSubView (&fField1);

	fMaximum.SetExtent (67, 72, 15, 40, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fValue.SetExtent (28, 72, 15, 40, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fMinLabel.SetExtent (47, 9, 15, 60, eNoUpdate);
	fMinLabel.SetFont (&kSystemFont);
	fMinLabel.SetText ("Minimum");
	AddSubView (&fMinLabel);

	fValueLabel.SetExtent (28, 33, 15, 36, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fStepSizeLabel.SetExtent (28, 118, 15, 60, eNoUpdate);
	fStepSizeLabel.SetFont (&kSystemFont);
	fStepSizeLabel.SetText ("Step Size");
	AddSubView (&fStepSizeLabel);

	fPageSizeLabel.SetExtent (47, 115, 15, 63, eNoUpdate);
	fPageSizeLabel.SetFont (&kSystemFont);
	fPageSizeLabel.SetText ("Page Size");
	AddSubView (&fPageSizeLabel);

	fStepSize.SetExtent (28, 181, 15, 40, eNoUpdate);
	fStepSize.SetFont (&kApplicationFont);
	fStepSize.SetController (this);
	fStepSize.SetMultiLine (False);
	AddSubView (&fStepSize);

	fPageSize.SetExtent (47, 181, 15, 40, eNoUpdate);
	fPageSize.SetFont (&kApplicationFont);
	fPageSize.SetController (this);
	fPageSize.SetMultiLine (False);
	AddSubView (&fPageSize);

	fMinimum.SetExtent (47, 72, 15, 40, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fViewInfo.SetExtent (91, 4, 206, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fStepSize);
	AddFocus (&fPageSize);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	ScrollBarInfoX::BuildForMotifUI ()
{
	SetSize (Point (309, 349), eNoUpdate);

	fTitle.SetExtent (5, 5, 20, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set ScrollBar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fMaximumLabel.SetExtent (73, 5, 21, 65, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum");
	AddSubView (&fMaximumLabel);

	fField1.SetExtent (27, 227, 50, 116, eNoUpdate);
	fField1.SetLabel ("Orientation", eNoUpdate);
		fField1RadioBank.SetController (this);

		fVertical.SetLabel ("Vertical", eNoUpdate);
		fField1RadioBank.AddRadioButton (&fVertical);
		fVertical.SetExtent (13, 5, 20, 80, eNoUpdate);
		fField1.AddSubView (&fVertical);

		fHorizontal.SetLabel ("Horizontal", eNoUpdate);
		fField1RadioBank.AddRadioButton (&fHorizontal);
		fHorizontal.SetExtent (30, 5, 20, 97, eNoUpdate);
		fField1.AddSubView (&fHorizontal);

		fField1TabLoop.AddFocus (&fVertical);
		fField1TabLoop.AddFocus (&fHorizontal);
	AddSubView (&fField1);

	fMaximum.SetExtent (71, 72, 21, 40, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fValue.SetExtent (27, 72, 21, 40, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fMinLabel.SetExtent (50, 9, 21, 61, eNoUpdate);
	fMinLabel.SetFont (&kSystemFont);
	fMinLabel.SetText ("Minimum");
	AddSubView (&fMinLabel);

	fValueLabel.SetExtent (27, 25, 21, 44, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fStepSizeLabel.SetExtent (28, 115, 21, 64, eNoUpdate);
	fStepSizeLabel.SetFont (&kSystemFont);
	fStepSizeLabel.SetText ("Step Size");
	AddSubView (&fStepSizeLabel);

	fPageSizeLabel.SetExtent (51, 115, 21, 64, eNoUpdate);
	fPageSizeLabel.SetFont (&kSystemFont);
	fPageSizeLabel.SetText ("Page Size");
	AddSubView (&fPageSizeLabel);

	fStepSize.SetExtent (28, 181, 21, 40, eNoUpdate);
	fStepSize.SetFont (&kApplicationFont);
	fStepSize.SetController (this);
	fStepSize.SetMultiLine (False);
	AddSubView (&fStepSize);

	fPageSize.SetExtent (50, 181, 21, 40, eNoUpdate);
	fPageSize.SetFont (&kApplicationFont);
	fPageSize.SetController (this);
	fPageSize.SetMultiLine (False);
	AddSubView (&fPageSize);

	fMinimum.SetExtent (49, 72, 21, 40, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fViewInfo.SetExtent (100, 5, 206, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fStepSize);
	AddFocus (&fPageSize);
	AddFocus (&fViewInfo);
	AddFocus (&fField1TabLoop);
}

#else

void	ScrollBarInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (300, 350), eNoUpdate);

	fTitle.SetExtent (3, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set ScrollBar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fMaximumLabel.SetExtent (67, 5, 15, 64, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum");
	AddSubView (&fMaximumLabel);

	fField1.SetExtent (22, 229, 50, 116, eNoUpdate);
	fField1.SetLabel ("Orientation", eNoUpdate);
		fField1RadioBank.SetController (this);

		fVertical.SetLabel ("Vertical", eNoUpdate);
		fField1RadioBank.AddRadioButton (&fVertical);
		fVertical.SetExtent (13, 5, 15, 70, eNoUpdate);
		fField1.AddSubView (&fVertical);

		fHorizontal.SetLabel ("Horizontal", eNoUpdate);
		fField1RadioBank.AddRadioButton (&fHorizontal);
		fHorizontal.SetExtent (30, 5, 15, 87, eNoUpdate);
		fField1.AddSubView (&fHorizontal);

	AddSubView (&fField1);

	fMaximum.SetExtent (67, 72, 15, 40, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fValue.SetExtent (28, 72, 15, 40, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fMinLabel.SetExtent (47, 9, 15, 60, eNoUpdate);
	fMinLabel.SetFont (&kSystemFont);
	fMinLabel.SetText ("Minimum");
	AddSubView (&fMinLabel);

	fValueLabel.SetExtent (28, 33, 15, 36, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fStepSizeLabel.SetExtent (28, 118, 15, 60, eNoUpdate);
	fStepSizeLabel.SetFont (&kSystemFont);
	fStepSizeLabel.SetText ("Step Size");
	AddSubView (&fStepSizeLabel);

	fPageSizeLabel.SetExtent (47, 115, 15, 63, eNoUpdate);
	fPageSizeLabel.SetFont (&kSystemFont);
	fPageSizeLabel.SetText ("Page Size");
	AddSubView (&fPageSizeLabel);

	fStepSize.SetExtent (28, 181, 15, 40, eNoUpdate);
	fStepSize.SetFont (&kApplicationFont);
	fStepSize.SetController (this);
	fStepSize.SetMultiLine (False);
	AddSubView (&fStepSize);

	fPageSize.SetExtent (47, 181, 15, 40, eNoUpdate);
	fPageSize.SetFont (&kApplicationFont);
	fPageSize.SetController (this);
	fPageSize.SetMultiLine (False);
	AddSubView (&fPageSize);

	fMinimum.SetExtent (47, 72, 15, 40, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fViewInfo.SetExtent (91, 4, 206, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fStepSize);
	AddFocus (&fPageSize);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	ScrollBarInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (300, 350));
#elif   qMotifUI
	return (Point (309, 349));
#else
	return (Point (300, 350));
#endif /* GUI */
}

void	ScrollBarInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfStepSizeLabelOrigin = fStepSizeLabel.GetOrigin ();
		fStepSizeLabel.SetOrigin (kOriginalfStepSizeLabelOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfPageSizeLabelOrigin = fPageSizeLabel.GetOrigin ();
		fPageSizeLabel.SetOrigin (kOriginalfPageSizeLabelOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfStepSizeOrigin = fStepSize.GetOrigin ();
		fStepSize.SetOrigin (kOriginalfStepSizeOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfPageSizeOrigin = fPageSize.GetOrigin ();
		fPageSize.SetOrigin (kOriginalfPageSizeOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include "Slider.hh"
#include "ScrollBarItem.hh"
#include "EmilyWindow.hh"

ScrollBarInfo::ScrollBarInfo (ScrollBarItem& view)
{
	fViewInfo.SetUpFromView (view);

	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	GetMaxValueField ().SetValue (view.GetScrollBar ().GetMax ());
	GetMinValueField ().SetValue (view.GetScrollBar ().GetMin ());
	GetValueField ().SetValue (view.GetScrollBar ().GetValue ());
	GetStepSizeField ().SetValue (view.GetScrollBar ().GetStepSize ());
	GetPageSizeField ().SetValue (view.GetScrollBar ().GetPageSize ());

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetMaxValueField ().SetEnabled (fullEditing);
	GetMinValueField ().SetEnabled (fullEditing);
	GetValueField ().SetEnabled (fullEditing);
	GetStepSizeField ().SetEnabled (fullEditing);
	GetPageSizeField ().SetEnabled (fullEditing);
	fVertical.SetEnabled (fullEditing);
	fHorizontal.SetEnabled (fullEditing);
	
	if (view.GetScrollBar ().GetOrientation () == AbstractScrollBar::eVertical) {
		fVertical.SetOn (True);
	}
	else {
		fHorizontal.SetOn (True);
	}
}

