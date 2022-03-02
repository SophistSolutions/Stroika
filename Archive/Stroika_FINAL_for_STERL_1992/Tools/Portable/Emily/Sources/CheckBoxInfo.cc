/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CheckBoxInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: CheckBoxInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.18  1992/05/19  11:35:41  sterling
 *		hi
 *
 *		Revision 1.17  92/05/13  18:46:58  18:46:58  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.16  92/04/08  17:22:36  17:22:36  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "CheckBoxInfo.hh"


CheckBoxInfoX::CheckBoxInfoX () :
	fLabel (),
	fOn (),
	fViewInfo (),
	fLabelLabel (),
	fTitle ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

CheckBoxInfoX::~CheckBoxInfoX ()
{
	RemoveFocus (&fLabel);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fLabel);
	RemoveSubView (&fOn);
	RemoveSubView (&fViewInfo);
	RemoveSubView (&fLabelLabel);
	RemoveSubView (&fTitle);
}

#if   qMacUI

void	CheckBoxInfoX::BuildForMacUI ()
{
	SetSize (Point (260, 314), eNoUpdate);

	fLabel.SetExtent (26, 46, 15, 194, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	fLabel.SetMultiLine (False);
	AddSubView (&fLabel);

	fOn.SetLabel ("On", eNoUpdate);
	fOn.SetExtent (26, 246, 15, 35, eNoUpdate);
	fOn.SetController (this);
	AddSubView (&fOn);

	fViewInfo.SetExtent (45, 4, 209, 302, eNoUpdate);
	AddSubView (&fViewInfo);

	fLabelLabel.SetExtent (26, 5, 17, 72, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fTitle.SetExtent (5, 5, 16, 302, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Check Box Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	CheckBoxInfoX::BuildForMotifUI ()
{
	SetSize (Point (269, 349), eNoUpdate);

	fLabel.SetExtent (26, 46, 26, 194, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	fLabel.SetMultiLine (False);
	AddSubView (&fLabel);

	fOn.SetLabel ("On", eNoUpdate);
	fOn.SetExtent (26, 246, 25, 50, eNoUpdate);
	fOn.SetController (this);
	AddSubView (&fOn);

	fViewInfo.SetExtent (51, 4, 214, 338, eNoUpdate);
	AddSubView (&fViewInfo);

	fLabelLabel.SetExtent (29, 1, 21, 42, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fTitle.SetExtent (5, 5, 21, 339, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Check Box Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	AddFocus (&fLabel);
	AddFocus (&fOn);
	AddFocus (&fViewInfo);
}

#else

void	CheckBoxInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (260, 314), eNoUpdate);

	fLabel.SetExtent (26, 46, 15, 194, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	fLabel.SetMultiLine (False);
	AddSubView (&fLabel);

	fOn.SetLabel ("On", eNoUpdate);
	fOn.SetExtent (26, 246, 15, 35, eNoUpdate);
	fOn.SetController (this);
	AddSubView (&fOn);

	fViewInfo.SetExtent (45, 4, 209, 302, eNoUpdate);
	AddSubView (&fViewInfo);

	fLabelLabel.SetExtent (26, 5, 17, 72, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fTitle.SetExtent (5, 5, 16, 302, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Check Box Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	CheckBoxInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (260, 314));
#elif   qMotifUI
	return (Point (269, 349));
#else
	return (Point (260, 314));
#endif /* GUI */
}

void	CheckBoxInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfLabelSize = fLabel.GetSize ();
		fLabel.SetSize (kOriginalfLabelSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfOnOrigin = fOn.GetOrigin ();
		fOn.SetOrigin (kOriginalfOnOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"CheckBoxItem.hh"
#include	"EmilyWindow.hh"

CheckBoxInfo::CheckBoxInfo (CheckBoxItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	GetLabelField ().SetText (view.GetCheckBox ().GetLabel (), eNoUpdate);
	GetOnField ().SetOn (view.GetCheckBox ().GetOn (), eNoUpdate);
	GetOnField ().SetEnabled (not EmilyWindow::GetCustomizeOnly ());
}
