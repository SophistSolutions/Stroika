/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SliderInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  1992/05/19  11:36:11  sterling
 *		hi
 *
 *		Revision 1.16  92/05/13  18:47:29  18:47:29  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.15  92/04/08  17:23:17  17:23:17  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "SliderInfo.hh"


SliderInfoX::SliderInfoX () :
	fTitle (),
	fMaximumLabel (),
	fMinLabel (),
	fValueLabel (),
	fMinimum (),
	fMaximum (),
	fValue (),
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

SliderInfoX::~SliderInfoX ()
{
	RemoveFocus (&fValue);
	RemoveFocus (&fMinimum);
	RemoveFocus (&fMaximum);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fMaximumLabel);
	RemoveSubView (&fMinLabel);
	RemoveSubView (&fValueLabel);
	RemoveSubView (&fMinimum);
	RemoveSubView (&fMaximum);
	RemoveSubView (&fValue);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	SliderInfoX::BuildForMacUI ()
{
	SetSize (Point (300, 350), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Slider Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

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

	fMinimum.SetExtent (48, 78, 15, 50, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fMaximum.SetExtent (68, 78, 15, 50, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fValue.SetExtent (28, 78, 15, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fViewInfo.SetExtent (89, 4, 208, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	SliderInfoX::BuildForMotifUI ()
{
	SetSize (Point (309, 349), eNoUpdate);

	fTitle.SetExtent (5, 5, 20, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Slider Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

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

	fMinimum.SetExtent (55, 78, 26, 50, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fMaximum.SetExtent (82, 78, 26, 50, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fValue.SetExtent (28, 78, 26, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fViewInfo.SetExtent (100, 5, 206, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
}

#else

void	SliderInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (300, 350), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Slider Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

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

	fMinimum.SetExtent (48, 78, 15, 50, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fMaximum.SetExtent (68, 78, 15, 50, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fValue.SetExtent (28, 78, 15, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fViewInfo.SetExtent (89, 4, 208, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	SliderInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (300, 350));
#elif   qMotifUI
	return (Point (309, 349));
#else
	return (Point (300, 350));
#endif /* GUI */
}

void	SliderInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include "Slider.hh"
#include "SliderItem.hh"
#include "EmilyWindow.hh"

SliderInfo::SliderInfo (SliderItem& view)
{
	fViewInfo.SetUpFromView (view);

	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	GetMaxValueField ().SetValue (view.GetSlider ().GetMax ());
	GetMinValueField ().SetValue (view.GetSlider ().GetMin ());
	GetValueField ().SetValue (view.GetSlider ().GetValue ());

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetMaxValueField ().SetEnabled (fullEditing);
	GetMinValueField ().SetEnabled (fullEditing);
	GetValueField ().SetEnabled (fullEditing);
}

