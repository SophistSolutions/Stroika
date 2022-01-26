/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PushButtonInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PushButtonInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  1992/05/19  11:36:04  sterling
 *		hi
 *
 *		Revision 1.16  92/05/13  18:47:21  18:47:21  lewis (Lewis Pringle)
 *		STERL.
 *		
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "PushButtonInfo.hh"


PushButtonInfoX::PushButtonInfoX () :
	fTitle (),
	fLabel (),
	fViewInfo (),
	fField1 ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

PushButtonInfoX::~PushButtonInfoX ()
{
	RemoveFocus (&fLabel);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fLabel);
	RemoveSubView (&fViewInfo);
	RemoveSubView (&fField1);
}

#if   qMacUI

void	PushButtonInfoX::BuildForMacUI ()
{
	SetSize (Point (250, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 315, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Push Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fLabel.SetExtent (30, 46, 16, 157, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fViewInfo.SetExtent (48, 6, 199, 316, eNoUpdate);
	AddSubView (&fViewInfo);

	fField1.SetExtent (31, 5, 15, 39, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Label:");
	AddSubView (&fField1);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	PushButtonInfoX::BuildForMotifUI ()
{
	SetSize (Point (290, 333), eNoUpdate);

	fTitle.SetExtent (5, 5, 19, 323, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Push Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fLabel.SetExtent (30, 46, 26, 157, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fViewInfo.SetExtent (58, 1, 230, 328, eNoUpdate);
	AddSubView (&fViewInfo);

	fField1.SetExtent (31, 5, 21, 39, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Label:");
	AddSubView (&fField1);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#else

void	PushButtonInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (250, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 315, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Push Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fLabel.SetExtent (30, 46, 16, 157, eNoUpdate);
	fLabel.SetFont (&kApplicationFont);
	fLabel.SetController (this);
	fLabel.SetWordWrap (False);
	AddSubView (&fLabel);

	fViewInfo.SetExtent (48, 6, 199, 316, eNoUpdate);
	AddSubView (&fViewInfo);

	fField1.SetExtent (31, 5, 15, 39, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Label:");
	AddSubView (&fField1);

	AddFocus (&fLabel);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	PushButtonInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (250, 325));
#elif   qMotifUI
	return (Point (290, 333));
#else
	return (Point (250, 325));
#endif /* GUI */
}

void	PushButtonInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"PushButton.hh"
#include	"PushButtonItem.hh"

PushButtonInfo::PushButtonInfo (PushButtonItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	GetLabelField ().SetText (view.GetPushButton ().GetLabel (), eNoUpdate);
}

