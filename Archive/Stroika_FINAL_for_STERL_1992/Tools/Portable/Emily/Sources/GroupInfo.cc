/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GroupInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: GroupInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  1992/05/19  11:35:49  sterling
 *		hi
 *
 *		Revision 1.16  92/05/13  18:47:06  18:47:06  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.15  92/04/08  17:22:53  17:22:53  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "GroupInfo.hh"


GroupInfoX::GroupInfoX () :
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

GroupInfoX::~GroupInfoX ()
{
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	GroupInfoX::BuildForMacUI ()
{
	SetSize (Point (220, 323), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 305, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Group Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (29, 4, 188, 316, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	GroupInfoX::BuildForMotifUI ()
{
	SetSize (Point (234, 323), eNoUpdate);

	fTitle.SetExtent (5, 5, 20, 313, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Group Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (29, 4, 188, 316, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#else

void	GroupInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (220, 323), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 305, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Group Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fViewInfo.SetExtent (29, 4, 188, 316, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	GroupInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (220, 323));
#elif   qMotifUI
	return (Point (234, 323));
#else
	return (Point (220, 323));
#endif /* GUI */
}

void	GroupInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"GroupItem.hh"
#include	"Dialog.hh"

GroupInfo::GroupInfo (GroupItem& view)
{
	GetViewItemInfo ().SetUpFromView (view);
}

