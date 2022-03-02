/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/NumberTextInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: NumberTextInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.18  1992/05/19  11:35:58  sterling
 *		hi
 *
 *		Revision 1.17  92/05/13  20:18:45  20:18:45  lewis (Lewis Pringle)
 *		*** empty log message ***
 *		
 *		Revision 1.16  92/05/13  18:47:15  18:47:15  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.15  92/04/08  17:23:03  17:23:03  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 */

#include "Shape.hh"
#include "FontPicker.hh"
#include "ViewItemInfo.hh"

#include	"EmilyWindow.hh"

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "NumberTextInfo.hh"


NumberTextInfoX::NumberTextInfoX () :
	fTitle (),
	fValueLabel (),
	fMaximumLabel (),
	fPrecisionLabel (),
	fMinimumLabel (),
	fViewInfo (),
	fPrecision (),
	fMultiLine (),
	fWordWrap (),
	fRequireText (),
	fMinimum (),
	fMaximum (),
	fDefault ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

NumberTextInfoX::~NumberTextInfoX ()
{
	RemoveFocus (&fPrecision);
	RemoveFocus (&fMinimum);
	RemoveFocus (&fMaximum);
	RemoveFocus (&fDefault);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fValueLabel);
	RemoveSubView (&fMaximumLabel);
	RemoveSubView (&fPrecisionLabel);
	RemoveSubView (&fMinimumLabel);
	RemoveSubView (&fViewInfo);
	RemoveSubView (&fPrecision);
	RemoveSubView (&fMultiLine);
	RemoveSubView (&fWordWrap);
	RemoveSubView (&fRequireText);
	RemoveSubView (&fMinimum);
	RemoveSubView (&fMaximum);
	RemoveSubView (&fDefault);
}

#if   qMacUI

void	NumberTextInfoX::BuildForMacUI ()
{
	SetSize (Point (310, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 294, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Number Text Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fValueLabel.SetExtent (85, 173, 15, 52, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Default:");
	AddSubView (&fValueLabel);

	fMaximumLabel.SetExtent (64, 157, 15, 68, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum:");
	AddSubView (&fMaximumLabel);

	fPrecisionLabel.SetExtent (24, 161, 15, 64, eNoUpdate);
	fPrecisionLabel.SetFont (&kSystemFont);
	fPrecisionLabel.SetText ("Precision:");
	AddSubView (&fPrecisionLabel);

	fMinimumLabel.SetExtent (45, 161, 15, 64, eNoUpdate);
	fMinimumLabel.SetFont (&kSystemFont);
	fMinimumLabel.SetText ("Minimum:");
	AddSubView (&fMinimumLabel);

	fViewInfo.SetExtent (106, 4, 200, 317, eNoUpdate);
	AddSubView (&fViewInfo);

	fPrecision.SetExtent (24, 227, 16, 69, eNoUpdate);
	fPrecision.SetFont (&kApplicationFont);
	fPrecision.SetController (this);
	fPrecision.SetMultiLine (False);
	fPrecision.SetMaxValue (6);
	fPrecision.SetMinValue (0);
	AddSubView (&fPrecision);

	fMultiLine.SetLabel ("Multiline", eNoUpdate);
	fMultiLine.SetExtent (28, 6, 15, 75, eNoUpdate);
	fMultiLine.SetController (this);
	AddSubView (&fMultiLine);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (62, 6, 15, 89, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fRequireText.SetLabel ("Require Text", eNoUpdate);
	fRequireText.SetExtent (45, 6, 15, 99, eNoUpdate);
	fRequireText.SetController (this);
	AddSubView (&fRequireText);

	fMinimum.SetExtent (44, 227, 16, 69, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fMaximum.SetExtent (64, 227, 16, 69, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fDefault.SetExtent (84, 227, 16, 69, eNoUpdate);
	fDefault.SetFont (&kApplicationFont);
	fDefault.SetController (this);
	fDefault.SetMultiLine (False);
	AddSubView (&fDefault);

	AddFocus (&fPrecision);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fDefault);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	NumberTextInfoX::BuildForMotifUI ()
{
	SetSize (Point (362, 350), eNoUpdate);

	fTitle.SetExtent (2, 3, 22, 343, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Number Text Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fValueLabel.SetExtent (106, 169, 21, 58, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Default:");
	AddSubView (&fValueLabel);

	fMaximumLabel.SetExtent (79, 159, 21, 68, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum:");
	AddSubView (&fMaximumLabel);

	fPrecisionLabel.SetExtent (27, 161, 21, 65, eNoUpdate);
	fPrecisionLabel.SetFont (&kSystemFont);
	fPrecisionLabel.SetText ("Precision:");
	AddSubView (&fPrecisionLabel);

	fMinimumLabel.SetExtent (52, 160, 21, 66, eNoUpdate);
	fMinimumLabel.SetFont (&kSystemFont);
	fMinimumLabel.SetText ("Minimum:");
	AddSubView (&fMinimumLabel);

	fViewInfo.SetExtent (133, 1, 225, 344, eNoUpdate);
	AddSubView (&fViewInfo);

	fPrecision.SetExtent (24, 227, 26, 116, eNoUpdate);
	fPrecision.SetFont (&kApplicationFont);
	fPrecision.SetController (this);
	fPrecision.SetMultiLine (False);
	fPrecision.SetMaxValue (6);
	fPrecision.SetMinValue (0);
	AddSubView (&fPrecision);

	fMultiLine.SetLabel ("Multiline", eNoUpdate);
	fMultiLine.SetExtent (28, 6, 25, 84, eNoUpdate);
	fMultiLine.SetController (this);
	AddSubView (&fMultiLine);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (78, 6, 25, 105, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fRequireText.SetLabel ("Require Text", eNoUpdate);
	fRequireText.SetExtent (53, 6, 25, 109, eNoUpdate);
	fRequireText.SetController (this);
	AddSubView (&fRequireText);

	fMinimum.SetExtent (51, 227, 26, 116, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fMaximum.SetExtent (78, 227, 26, 116, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fDefault.SetExtent (105, 227, 26, 116, eNoUpdate);
	fDefault.SetFont (&kApplicationFont);
	fDefault.SetController (this);
	fDefault.SetMultiLine (False);
	AddSubView (&fDefault);

	AddFocus (&fMultiLine);
	AddFocus (&fWordWrap);
	AddFocus (&fRequireText);
	AddFocus (&fPrecision);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fDefault);
	AddFocus (&fViewInfo);
}

#else

void	NumberTextInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (310, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 294, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Number Text Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fValueLabel.SetExtent (85, 173, 15, 52, eNoUpdate);
	fValueLabel.SetFont (&kSystemFont);
	fValueLabel.SetText ("Default:");
	AddSubView (&fValueLabel);

	fMaximumLabel.SetExtent (64, 157, 15, 68, eNoUpdate);
	fMaximumLabel.SetFont (&kSystemFont);
	fMaximumLabel.SetText ("Maximum:");
	AddSubView (&fMaximumLabel);

	fPrecisionLabel.SetExtent (24, 161, 15, 64, eNoUpdate);
	fPrecisionLabel.SetFont (&kSystemFont);
	fPrecisionLabel.SetText ("Precision:");
	AddSubView (&fPrecisionLabel);

	fMinimumLabel.SetExtent (45, 161, 15, 64, eNoUpdate);
	fMinimumLabel.SetFont (&kSystemFont);
	fMinimumLabel.SetText ("Minimum:");
	AddSubView (&fMinimumLabel);

	fViewInfo.SetExtent (106, 4, 200, 317, eNoUpdate);
	AddSubView (&fViewInfo);

	fPrecision.SetExtent (24, 227, 16, 69, eNoUpdate);
	fPrecision.SetFont (&kApplicationFont);
	fPrecision.SetController (this);
	fPrecision.SetMultiLine (False);
	fPrecision.SetMaxValue (6);
	fPrecision.SetMinValue (0);
	AddSubView (&fPrecision);

	fMultiLine.SetLabel ("Multiline", eNoUpdate);
	fMultiLine.SetExtent (28, 6, 15, 75, eNoUpdate);
	fMultiLine.SetController (this);
	AddSubView (&fMultiLine);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (62, 6, 15, 89, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fRequireText.SetLabel ("Require Text", eNoUpdate);
	fRequireText.SetExtent (45, 6, 15, 99, eNoUpdate);
	fRequireText.SetController (this);
	AddSubView (&fRequireText);

	fMinimum.SetExtent (44, 227, 16, 69, eNoUpdate);
	fMinimum.SetFont (&kApplicationFont);
	fMinimum.SetController (this);
	fMinimum.SetMultiLine (False);
	AddSubView (&fMinimum);

	fMaximum.SetExtent (64, 227, 16, 69, eNoUpdate);
	fMaximum.SetFont (&kApplicationFont);
	fMaximum.SetController (this);
	fMaximum.SetMultiLine (False);
	AddSubView (&fMaximum);

	fDefault.SetExtent (84, 227, 16, 69, eNoUpdate);
	fDefault.SetFont (&kApplicationFont);
	fDefault.SetController (this);
	fDefault.SetMultiLine (False);
	AddSubView (&fDefault);

	AddFocus (&fPrecision);
	AddFocus (&fMinimum);
	AddFocus (&fMaximum);
	AddFocus (&fDefault);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	NumberTextInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (310, 325));
#elif   qMotifUI
	return (Point (362, 350));
#else
	return (Point (310, 325));
#endif /* GUI */
}

void	NumberTextInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfPrecisionSize = fPrecision.GetSize ();
		fPrecision.SetSize (kOriginalfPrecisionSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfMinimumSize = fMinimum.GetSize ();
		fMinimum.SetSize (kOriginalfMinimumSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfMaximumSize = fMaximum.GetSize ();
		fMaximum.SetSize (kOriginalfMaximumSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfDefaultSize = fDefault.GetSize ();
		fDefault.SetSize (kOriginalfDefaultSize - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"NumberTextItem.hh"

NumberTextInfo::NumberTextInfo (NumberTextItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	fRequireText.SetOn (view.GetTextRequired (), eNoUpdate);
	GetPrecisionField ().SetValue (view.GetNumberText ().GetPrecision (), eNoUpdate);
	GetMaxValueField ().SetValue (view.GetNumberText ().GetMaxValue (), eNoUpdate);
	GetMinValueField ().SetValue (view.GetNumberText ().GetMinValue (), eNoUpdate);
	GetDefaultField ().SetValue (view.GetNumberText ().GetValue (), eNoUpdate);

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetPrecisionField ().SetEnabled (fullEditing, eNoUpdate);
	GetMaxValueField ().SetEnabled (fullEditing, eNoUpdate);
	GetMinValueField ().SetEnabled (fullEditing, eNoUpdate);
	GetDefaultField ().SetEnabled (fullEditing, eNoUpdate);
	fRequireText.SetEnabled (fullEditing, eNoUpdate);
}



