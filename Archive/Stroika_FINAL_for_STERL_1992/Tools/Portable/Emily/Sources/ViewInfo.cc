/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ViewInfo.cc,v 1.5 1992/09/01 17:25:44 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ViewInfo.cc,v $
 *		Revision 1.5  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.15  92/04/08  17:23:22  17:23:22  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 *
 */


// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "ViewInfo.hh"


ViewInfoX::ViewInfoX () :
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

ViewInfoX::~ViewInfoX ()
{
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	ViewInfoX::BuildForMacUI ()
{
	SetSize (Point (220, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 316, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set View Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (24, 4, 195, 318, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	ViewInfoX::BuildForMotifUI ()
{
	SetSize (Point (220, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 22, 316, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set View Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (27, 4, 192, 318, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#else

void	ViewInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (220, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 316, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set View Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (24, 4, 195, 318, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	ViewInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (220, 325));
#elif   qMotifUI
	return (Point (220, 325));
#else
	return (Point (220, 325));
#endif /* GUI */
}

void	ViewInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"ViewItem.hh"

ViewInfo::ViewInfo (ViewItem& view)
{
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	if (view.fFieldNameTemporary) {
		view.SetFieldName (kEmptyString);	// flush any names set for code generation
	}
	fViewInfo.SetUpFromView (view);
}

