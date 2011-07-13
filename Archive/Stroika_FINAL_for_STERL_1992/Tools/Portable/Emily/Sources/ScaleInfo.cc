/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScaleInfo.cc,v 1.2 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScaleInfo.cc,v $
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

#include "ScaleInfo.hh"


ScaleInfoX::ScaleInfoX () :
	fSensitive (),
	fThumbLength (),
	fThumbLengthLabel (),
	fValueLabel (),
	fValue (),
	fTitle (),
	fMaximumLabel (),
	fMinLabel (),
	fMinimum (),
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

ScaleInfoX::~ScaleInfoX ()
{
	RemoveFocus (&fValue);
	RemoveFocus (&fMinimum);
	RemoveFocus (&fMaximum);
	RemoveFocus (&fViewInfo);
	RemoveFocus (&fThumbLength);

	RemoveSubView (&fSensitive);
	RemoveSubView (&fThumbLength);
	RemoveSubView (&fThumbLengthLabel);
	RemoveSubView (&fValueLabel);
	RemoveSubView (&fValue);
	RemoveSubView (&fTitle);
	RemoveSubView (&fMaximumLabel);
	RemoveSubView (&fMinLabel);
	RemoveSubView (&fMinimum);
	RemoveSubView (&fMaximum);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	ScaleInfoX::BuildForMacUI ()
{
	SetSize (Point (300, 350), eNoUpdate);

	fSensitive.SetLabel ("Sensitive", eNoUpdate);
	fSensitive.SetExtent (48, 176, 15, 79, eNoUpdate);
	fSensitive.SetController (this);
	AddSubView (&fSensitive);

	fThumbLength.SetExtent (28, 273, 15, 50, eNoUpdate);
	fThumbLength.SetFont (&kApplicationFont);
	fThumbLength.SetController (this);
	fThumbLength.SetMultiLine (False);
	AddSubView (&fThumbLength);

	fThumbLengthLabel.SetExtent (28, 176, 16, 92, eNoUpdate);
	fThumbLengthLabel.SetFont (&kSystemFont);
	fThumbLengthLabel.SetText ("Thumb Length");
	AddSubView (&fThumbLengthLabel);

	fValueLabel.SetExtent (28, 39, 15, 36, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fValue.SetExtent (28, 78, 15, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fTitle.SetExtent (5, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Scale Info");
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

	fViewInfo.SetExtent (88, 4, 209, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
	AddFocus (&fThumbLength);
}

#elif qMotifUI

void	ScaleInfoX::BuildForMotifUI ()
{
	SetSize (Point (309, 349), eNoUpdate);

	fSensitive.SetLabel ("Sensitive", eNoUpdate);
	fSensitive.SetExtent (54, 176, 20, 89, eNoUpdate);
	fSensitive.SetController (this);
	AddSubView (&fSensitive);

	fThumbLength.SetExtent (28, 273, 21, 50, eNoUpdate);
	fThumbLength.SetFont (&kApplicationFont);
	fThumbLength.SetController (this);
	fThumbLength.SetMultiLine (False);
	AddSubView (&fThumbLength);

	fThumbLengthLabel.SetExtent (28, 176, 16, 92, eNoUpdate);
	fThumbLengthLabel.SetFont (&kSystemFont);
	fThumbLengthLabel.SetText ("Thumb Length");
	AddSubView (&fThumbLengthLabel);

	fValueLabel.SetExtent (28, 34, 16, 37, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fValue.SetExtent (28, 78, 21, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fTitle.SetExtent (5, 5, 20, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Scale Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fMaximumLabel.SetExtent (83, 13, 16, 65, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum");
	AddSubView (&fMaximumLabel);

	fMinLabel.SetExtent (56, 16, 16, 61, eNoUpdate);
	fMinLabel.SetFont (&kSystemFont);
	fMinLabel.SetText ("Minimum");
	AddSubView (&fMinLabel);

	fMinimum.SetExtent (55, 78, 21, 50, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fMaximum.SetExtent (82, 78, 21, 50, eNoUpdate);
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
	AddFocus (&fThumbLength);
	AddFocus (&fSensitive);
}

#else

void	ScaleInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (300, 350), eNoUpdate);

	fSensitive.SetLabel ("Sensitive", eNoUpdate);
	fSensitive.SetExtent (48, 176, 15, 79, eNoUpdate);
	fSensitive.SetController (this);
	AddSubView (&fSensitive);

	fThumbLength.SetExtent (28, 273, 15, 50, eNoUpdate);
	fThumbLength.SetFont (&kApplicationFont);
	fThumbLength.SetController (this);
	fThumbLength.SetMultiLine (False);
	AddSubView (&fThumbLength);

	fThumbLengthLabel.SetExtent (28, 176, 16, 92, eNoUpdate);
	fThumbLengthLabel.SetFont (&kSystemFont);
	fThumbLengthLabel.SetText ("Thumb Length");
	AddSubView (&fThumbLengthLabel);

	fValueLabel.SetExtent (28, 39, 15, 36, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Value");
	AddSubView (&fValueLabel);

	fValue.SetExtent (28, 78, 15, 50, eNoUpdate);
	fValue.SetFont (&kApplicationFont);
	fValue.SetController (this);
	fValue.SetMultiLine (False);
	AddSubView (&fValue);

	fTitle.SetExtent (5, 5, 16, 340, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText ("Set Scale Info");
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

	fViewInfo.SetExtent (88, 4, 209, 342, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fValue);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fViewInfo);
	AddFocus (&fThumbLength);
}

#endif /* GUI */

Point	ScaleInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (300, 350));
#elif   qMotifUI
	return (Point (309, 349));
#else
	return (Point (300, 350));
#endif /* GUI */
}

void	ScaleInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include "Scale.hh"
#include "ScaleItem.hh"
#include "EmilyWindow.hh"

ScaleInfo::ScaleInfo (ScaleItem& view)
{
	fViewInfo.SetUpFromView (view);

	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	GetMaxValueField ().SetValue (view.GetScale ().GetMax ());
	GetMinValueField ().SetValue (view.GetScale ().GetMin ());
	GetValueField ().SetValue (view.GetScale ().GetValue ());
	fThumbLength.SetValue (view.GetScale ().GetThumbLength ());
	fSensitive.SetOn (view.GetScale ().GetSensitive ());

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetMaxValueField ().SetEnabled (fullEditing);
	GetMinValueField ().SetEnabled (fullEditing);
	GetValueField ().SetEnabled (fullEditing);
	fThumbLength.SetEnabled (fullEditing);
	fSensitive.SetEnabled (fullEditing);
}

