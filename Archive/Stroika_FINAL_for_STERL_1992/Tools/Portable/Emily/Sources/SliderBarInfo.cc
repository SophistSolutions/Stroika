/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SliderBarInfo.cc,v 1.2 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderBarInfo.cc,v $
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

#include "SliderBarInfo.hh"


SliderBarInfoX::SliderBarInfoX () :
	fTitle (),
	fSubTicks (),
	fTicks (),
	fField1 (),
	fField2 (),
	fMinimum (),
	fValue (),
	fMaximumLabel (),
	fMinLabel (),
	fValueLabel (),
	fMaximum (),
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

SliderBarInfoX::~SliderBarInfoX ()
{
	RemoveFocus (&fValue);
	RemoveFocus (&fMinimum);
	RemoveFocus (&fMaximum);
	RemoveFocus (&fViewInfo);
	RemoveFocus (&fTicks);
	RemoveFocus (&fSubTicks);

	RemoveSubView (&fTitle);
	RemoveSubView (&fSubTicks);
	RemoveSubView (&fTicks);
	RemoveSubView (&fField1);
	RemoveSubView (&fField2);
	RemoveSubView (&fMinimum);
	RemoveSubView (&fValue);
	RemoveSubView (&fMaximumLabel);
	RemoveSubView (&fMinLabel);
	RemoveSubView (&fValueLabel);
	RemoveSubView (&fMaximum);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	SliderBarInfoX::BuildForMacUI ()
{
	SetSize (Point (300, 350), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Slider Bar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fSubTicks.SetExtent (46, 223, 15, 50, eNoUpdate);
	fSubTicks.SetFont (&kApplicationFont);
	fSubTicks.SetController (this);
	fSubTicks.SetMultiLine (False);
	AddSubView (&fSubTicks);

	fTicks.SetExtent (26, 223, 15, 50, eNoUpdate);
	fTicks.SetFont (&kApplicationFont);
	fTicks.SetController (this);
	fTicks.SetMultiLine (False);
	AddSubView (&fTicks);

	fField1.SetExtent (47, 165, 16, 56, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("SubTicks");
	AddSubView (&fField1);

	fField2.SetExtent (27, 185, 15, 36, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Ticks");
	AddSubView (&fField2);

	fMinimum.SetExtent (48, 78, 15, 50, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fValue.SetExtent (28, 78, 15, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fMaximumLabel.SetExtent (68, 11, 15, 64, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum");
	AddSubView (&fMaximumLabel);

	fMinLabel.SetExtent (48, 15, 15, 60, eNoUpdate);
	fMinLabel.SetFont (&kSystemFont);
	fMinLabel.SetText ("Minimum");
	AddSubView (&fMinLabel);

	fValueLabel.SetExtent (28, 39, 15, 36, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fMaximum.SetExtent (68, 78, 15, 50, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fViewInfo.SetExtent (91, 4, 206, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
	AddFocus (&fTicks);
	AddFocus (&fSubTicks);
}

#elif qMotifUI

void	SliderBarInfoX::BuildForMotifUI ()
{
	SetSize (Point (309, 349), eNoUpdate);

	fTitle.SetExtent (2, 5, 20, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Slider Bar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fSubTicks.SetExtent (46, 223, 21, 50, eNoUpdate);
	fSubTicks.SetFont (&kApplicationFont);
	fSubTicks.SetController (this);
	fSubTicks.SetMultiLine (False);
	AddSubView (&fSubTicks);

	fTicks.SetExtent (26, 223, 21, 50, eNoUpdate);
	fTicks.SetFont (&kApplicationFont);
	fTicks.SetController (this);
	fTicks.SetMultiLine (False);
	AddSubView (&fTicks);

	fField1.SetExtent (47, 156, 16, 56, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("SubTicks");
	AddSubView (&fField1);

	fField2.SetExtent (27, 176, 16, 33, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Ticks");
	AddSubView (&fField2);

	fMinimum.SetExtent (55, 78, 26, 50, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fValue.SetExtent (28, 78, 26, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fMaximumLabel.SetExtent (83, 13, 21, 65, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum");
	AddSubView (&fMaximumLabel);

	fMinLabel.SetExtent (56, 16, 21, 61, eNoUpdate);
	fMinLabel.SetFont (&kSystemFont);
	fMinLabel.SetText ("Minimum");
	AddSubView (&fMinLabel);

	fValueLabel.SetExtent (28, 34, 21, 44, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fMaximum.SetExtent (82, 78, 26, 50, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fViewInfo.SetExtent (100, 5, 206, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
	AddFocus (&fTicks);
	AddFocus (&fSubTicks);
}

#else

void	SliderBarInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (300, 350), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Slider Bar Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fSubTicks.SetExtent (46, 223, 15, 50, eNoUpdate);
	fSubTicks.SetFont (&kApplicationFont);
	fSubTicks.SetController (this);
	fSubTicks.SetMultiLine (False);
	AddSubView (&fSubTicks);

	fTicks.SetExtent (26, 223, 15, 50, eNoUpdate);
	fTicks.SetFont (&kApplicationFont);
	fTicks.SetController (this);
	fTicks.SetMultiLine (False);
	AddSubView (&fTicks);

	fField1.SetExtent (47, 165, 16, 56, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("SubTicks");
	AddSubView (&fField1);

	fField2.SetExtent (27, 185, 15, 36, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Ticks");
	AddSubView (&fField2);

	fMinimum.SetExtent (48, 78, 15, 50, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fValue.SetExtent (28, 78, 15, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fMaximumLabel.SetExtent (68, 11, 15, 64, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum");
	AddSubView (&fMaximumLabel);

	fMinLabel.SetExtent (48, 15, 15, 60, eNoUpdate);
	fMinLabel.SetFont (&kSystemFont);
	fMinLabel.SetText ("Minimum");
	AddSubView (&fMinLabel);

	fValueLabel.SetExtent (28, 39, 15, 36, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fMaximum.SetExtent (68, 78, 15, 50, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fViewInfo.SetExtent (91, 4, 206, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
	AddFocus (&fTicks);
	AddFocus (&fSubTicks);
}

#endif /* GUI */

Point	SliderBarInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (300, 350));
#elif   qMotifUI
	return (Point (309, 349));
#else
	return (Point (300, 350));
#endif /* GUI */
}

void	SliderBarInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include "SliderBar.hh"
#include "SliderBarItem.hh"
#include "EmilyWindow.hh"

SliderBarInfo::SliderBarInfo (SliderBarItem& view)
{
	fViewInfo.SetUpFromView (view);

	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	GetMaxValueField ().SetValue (view.GetSlider ().GetMax ());
	GetMinValueField ().SetValue (view.GetSlider ().GetMin ());
	GetValueField ().SetValue (view.GetSlider ().GetValue ());
	fTicks.SetValue (view.GetSliderBar ().GetTickSize ());
	fSubTicks.SetValue (view.GetSliderBar ().GetSubTickSize ());

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetMaxValueField ().SetEnabled (fullEditing);
	GetMinValueField ().SetEnabled (fullEditing);
	GetValueField ().SetEnabled (fullEditing);
	fTicks.SetEnabled (fullEditing);
	fSubTicks.SetEnabled (fullEditing);
}

