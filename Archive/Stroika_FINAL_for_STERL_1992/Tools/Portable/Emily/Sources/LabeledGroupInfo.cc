/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/LabeledGroupInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: LabeledGroupInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.12  1992/05/19  11:35:52  sterling
 *		hi
 *
 *		Revision 1.11  92/05/13  18:47:10  18:47:10  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.10  92/04/08  17:22:56  17:22:56  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "LabeledGroupInfo.hh"


LabeledGroupInfoX::LabeledGroupInfoX () :
	fLabel (),
	fLabelLabel (),
	fTitle (),
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

LabeledGroupInfoX::~LabeledGroupInfoX ()
{
	RemoveFocus (&fLabel);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fLabel);
	RemoveSubView (&fLabelLabel);
	RemoveSubView (&fTitle);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	LabeledGroupInfoX::BuildForMacUI ()
{
	SetSize (Point (250, 324), eNoUpdate);

	fLabel.SetExtent (23, 70, 19, 232, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fLabelLabel.SetExtent (24, 26, 15, 39, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fTitle.SetExtent (5, 5, 16, 316, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Labeled Group Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (44, 2, 203, 318, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	LabeledGroupInfoX::BuildForMotifUI ()
{
	SetSize (Point (265, 324), eNoUpdate);

	fLabel.SetExtent (31, 101, 26, 146, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fLabelLabel.SetExtent (33, 49, 21, 46, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fTitle.SetExtent (5, 5, 21, 311, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Labeled Group Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (59, 2, 203, 318, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#else

void	LabeledGroupInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (250, 324), eNoUpdate);

	fLabel.SetExtent (23, 70, 19, 232, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fLabelLabel.SetExtent (24, 26, 15, 39, eNoUpdate);
	fLabelLabel.SetFont (&kSystemFont);
	fLabelLabel.SetText ("Label:");
	AddSubView (&fLabelLabel);

	fTitle.SetExtent (5, 5, 16, 316, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Labeled Group Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (44, 2, 203, 318, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	LabeledGroupInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (250, 324));
#elif   qMotifUI
	return (Point (265, 324));
#else
	return (Point (250, 324));
#endif /* GUI */
}

void	LabeledGroupInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfLabelSize = fLabel.GetSize ();
		fLabel.SetSize (kOriginalfLabelSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	GroupView::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"LabeledGroupItem.hh"
#include	"Dialog.hh"

LabeledGroupInfo::LabeledGroupInfo (LabeledGroupItem& view)
{
	GetViewItemInfo ().SetUpFromView (view);
	GetLabelField ().SetText (view.GetLabel ());
}


