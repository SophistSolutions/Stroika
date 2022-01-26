/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/RadioButtonInfo.cc,v 1.5 1992/09/01 17:25:44 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: RadioButtonInfo.cc,v $
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.16  92/05/13  18:47:23  18:47:23  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.15  92/04/08  17:23:11  17:23:11  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "RadioButtonInfo.hh"


RadioButtonInfoX::RadioButtonInfoX () :
	fTitle (),
	fLabelLabel (),
	fOn (),
	fLabel (),
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

RadioButtonInfoX::~RadioButtonInfoX ()
{
	RemoveFocus (&fLabel);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fLabelLabel);
	RemoveSubView (&fOn);
	RemoveSubView (&fLabel);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	RadioButtonInfoX::BuildForMacUI ()
{
	SetSize (Point (235, 315), eNoUpdate);

	fTitle.SetExtent (5, 3, 16, 307, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Radio Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fLabelLabel.SetExtent (29, 17, 16, 36, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label");
	AddSubView (&fLabelLabel);

	fOn.SetLabel ("On", eNoUpdate);
	fOn.SetExtent (30, 211, 16, 37, eNoUpdate);
	fOn.SetController (this);
	AddSubView (&fOn);

	fLabel.SetExtent (28, 53, 18, 155, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fViewInfo.SetExtent (49, 1, 186, 311, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	RadioButtonInfoX::BuildForMotifUI ()
{
	SetSize (Point (251, 329), eNoUpdate);

	fTitle.SetExtent (5, 2, 20, 322, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Radio Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fLabelLabel.SetExtent (29, 5, 21, 43, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label");
	AddSubView (&fLabelLabel);

	fOn.SetLabel ("On", eNoUpdate);
	fOn.SetExtent (29, 243, 25, 50, eNoUpdate);
	fOn.SetController (this);
	AddSubView (&fOn);

	fLabel.SetExtent (28, 53, 26, 169, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fViewInfo.SetExtent (54, 2, 193, 326, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fLabel);
	AddFocus (&fOn);
	AddFocus (&fViewInfo);
}

#else

void	RadioButtonInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (235, 315), eNoUpdate);

	fTitle.SetExtent (5, 3, 16, 307, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Radio Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fLabelLabel.SetExtent (29, 17, 16, 36, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label");
	AddSubView (&fLabelLabel);

	fOn.SetLabel ("On", eNoUpdate);
	fOn.SetExtent (30, 211, 16, 37, eNoUpdate);
	fOn.SetController (this);
	AddSubView (&fOn);

	fLabel.SetExtent (28, 53, 18, 155, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fViewInfo.SetExtent (49, 1, 186, 311, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	RadioButtonInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (235, 315));
#elif   qMotifUI
	return (Point (251, 329));
#else
	return (Point (235, 315));
#endif /* GUI */
}

void	RadioButtonInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfOnOrigin = fOn.GetOrigin ();
		fOn.SetOrigin (kOriginalfOnOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfLabelSize = fLabel.GetSize ();
		fLabel.SetSize (kOriginalfLabelSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!

#include	"RadioButton.hh"
#include	"RadioButtonItem.hh"
#include	"EmilyWindow.hh"

RadioButtonInfo::RadioButtonInfo (RadioButtonItem& view)
{
	fViewInfo.SetUpFromView (view);

	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	GetLabelField ().SetText (view.GetRadioButton ().GetLabel (), eNoUpdate);
	GetOnField ().SetOn (view.GetRadioButton ().GetOn (), eNoUpdate);
	GetOnField ().SetEnabled (not EmilyWindow::GetCustomizeOnly ());
}

