/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ScrollerInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollerInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.12  1992/05/19  11:36:08  sterling
 *		hi
 *
 *		Revision 1.11  92/05/13  18:47:26  18:47:26  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.10  92/04/08  17:23:14  17:23:14  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 */

#include "FontPicker.hh"

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "ScrollerInfo.hh"


ScrollerInfoX::ScrollerInfoX () :
	fVerticalSBar (),
	fHorizontalSBar (),
	fTitle (),
	fScrollBoundsGroup (kEmptyString),
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

ScrollerInfoX::~ScrollerInfoX ()
{
	RemoveFocus (&fViewInfo);
	RemoveFocus (&fScrollBoundsGroupTabLoop);

	RemoveSubView (&fVerticalSBar);
	RemoveSubView (&fHorizontalSBar);
	RemoveSubView (&fTitle);
	RemoveSubView (&fScrollBoundsGroup);
		fScrollBoundsGroupTabLoop.RemoveFocus (&fScrollV);
		fScrollBoundsGroupTabLoop.RemoveFocus (&fScrollH);

		fScrollBoundsGroup.RemoveSubView (&fScrollH);
		fScrollBoundsGroup.RemoveSubView (&fScrollV);
		fScrollBoundsGroup.RemoveSubView (&fScrollHLabel);
		fScrollBoundsGroup.RemoveSubView (&fScrollVLabel);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	ScrollerInfoX::BuildForMacUI ()
{
	SetSize (Point (270, 315), eNoUpdate);

	fVerticalSBar.SetLabel ("Vertical Scrollbar", eNoUpdate);
	fVerticalSBar.SetExtent (214, 12, 15, 130, eNoUpdate);
	fVerticalSBar.SetController (this);
	AddSubView (&fVerticalSBar);

	fHorizontalSBar.SetLabel ("Horizontal Scrollbar", eNoUpdate);
	fHorizontalSBar.SetExtent (231, 12, 15, 147, eNoUpdate);
	fHorizontalSBar.SetController (this);
	AddSubView (&fHorizontalSBar);

	fTitle.SetExtent (3, 5, 16, 305, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Scroller Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fScrollBoundsGroup.SetExtent (209, 203, 50, 100, eNoUpdate);
	fScrollBoundsGroup.SetFont (&kSystemFont);
	fScrollBoundsGroup.SetLabel ("Scroll Bounds", eNoUpdate);
		fScrollH.SetExtent (14, 54, 20, 40, eNoUpdate);
		fScrollH.SetFont (&kApplicationFont);
		fScrollH.SetController (this);
		fScrollH.SetMultiLine (False);
		fScrollBoundsGroup.AddSubView (&fScrollH);

		fScrollV.SetExtent (14, 8, 20, 40, eNoUpdate);
		fScrollV.SetFont (&kApplicationFont);
		fScrollV.SetController (this);
		fScrollV.SetMultiLine (False);
		fScrollBoundsGroup.AddSubView (&fScrollV);

		fScrollHLabel.SetExtent (35, 55, 12, 38, eNoUpdate);
		fScrollHLabel.SetFont (&kApplicationFont);
		fScrollHLabel.SetText ("width");
		fScrollHLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBoundsGroup.AddSubView (&fScrollHLabel);

		fScrollVLabel.SetExtent (35, 9, 12, 38, eNoUpdate);
		fScrollVLabel.SetFont (&kApplicationFont);
		fScrollVLabel.SetText ("height");
		fScrollVLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBoundsGroup.AddSubView (&fScrollVLabel);

		fScrollBoundsGroupTabLoop.AddFocus (&fScrollV);
		fScrollBoundsGroupTabLoop.AddFocus (&fScrollH);
	AddSubView (&fScrollBoundsGroup);

	fViewInfo.SetExtent (21, 4, 188, 308, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
	AddFocus (&fScrollBoundsGroupTabLoop);
}

#elif qMotifUI

void	ScrollerInfoX::BuildForMotifUI ()
{
	SetSize (Point (297, 315), eNoUpdate);

	fVerticalSBar.SetLabel ("Vertical Scrollbar", eNoUpdate);
	fVerticalSBar.SetExtent (232, 10, 20, 142, eNoUpdate);
	fVerticalSBar.SetController (this);
	AddSubView (&fVerticalSBar);

	fHorizontalSBar.SetLabel ("Horizontal Scrollbar", eNoUpdate);
	fHorizontalSBar.SetExtent (257, 10, 20, 159, eNoUpdate);
	fHorizontalSBar.SetController (this);
	AddSubView (&fHorizontalSBar);

	fTitle.SetExtent (5, 5, 23, 305, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Scroller Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fScrollBoundsGroup.SetExtent (227, 201, 66, 103, eNoUpdate);
	fScrollBoundsGroup.SetFont (&kSystemFont);
	fScrollBoundsGroup.SetLabel ("Scroll Bounds", eNoUpdate);
		fScrollH.SetExtent (20, 53, 26, 40, eNoUpdate);
		fScrollH.SetFont (&kApplicationFont);
		fScrollH.SetController (this);
		fScrollH.SetMultiLine (False);
		fScrollBoundsGroup.AddSubView (&fScrollH);

		fScrollV.SetExtent (20, 8, 26, 40, eNoUpdate);
		fScrollV.SetFont (&kApplicationFont);
		fScrollV.SetController (this);
		fScrollV.SetMultiLine (False);
		fScrollBoundsGroup.AddSubView (&fScrollV);

		fScrollHLabel.SetExtent (45, 55, 18, 41, eNoUpdate);
		fScrollHLabel.SetFont (&kApplicationFont);
		fScrollHLabel.SetText ("width");
		fScrollHLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBoundsGroup.AddSubView (&fScrollHLabel);

		fScrollVLabel.SetExtent (45, 5, 18, 44, eNoUpdate);
		fScrollVLabel.SetFont (&kApplicationFont);
		fScrollVLabel.SetText ("height");
		fScrollVLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBoundsGroup.AddSubView (&fScrollVLabel);

		fScrollBoundsGroupTabLoop.AddFocus (&fScrollV);
		fScrollBoundsGroupTabLoop.AddFocus (&fScrollH);
	AddSubView (&fScrollBoundsGroup);

	fViewInfo.SetExtent (26, 4, 200, 308, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
	AddFocus (&fVerticalSBar);
	AddFocus (&fHorizontalSBar);
	AddFocus (&fScrollBoundsGroupTabLoop);
}

#else

void	ScrollerInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (270, 315), eNoUpdate);

	fVerticalSBar.SetLabel ("Vertical Scrollbar", eNoUpdate);
	fVerticalSBar.SetExtent (214, 12, 15, 130, eNoUpdate);
	fVerticalSBar.SetController (this);
	AddSubView (&fVerticalSBar);

	fHorizontalSBar.SetLabel ("Horizontal Scrollbar", eNoUpdate);
	fHorizontalSBar.SetExtent (231, 12, 15, 147, eNoUpdate);
	fHorizontalSBar.SetController (this);
	AddSubView (&fHorizontalSBar);

	fTitle.SetExtent (3, 5, 16, 305, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Scroller Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fScrollBoundsGroup.SetExtent (209, 203, 50, 100, eNoUpdate);
	fScrollBoundsGroup.SetFont (&kSystemFont);
	fScrollBoundsGroup.SetLabel ("Scroll Bounds", eNoUpdate);
		fScrollH.SetExtent (14, 54, 20, 40, eNoUpdate);
		fScrollH.SetFont (&kApplicationFont);
		fScrollH.SetController (this);
		fScrollH.SetMultiLine (False);
		fScrollBoundsGroup.AddSubView (&fScrollH);

		fScrollV.SetExtent (14, 8, 20, 40, eNoUpdate);
		fScrollV.SetFont (&kApplicationFont);
		fScrollV.SetController (this);
		fScrollV.SetMultiLine (False);
		fScrollBoundsGroup.AddSubView (&fScrollV);

		fScrollHLabel.SetExtent (35, 55, 12, 38, eNoUpdate);
		fScrollHLabel.SetFont (&kApplicationFont);
		fScrollHLabel.SetText ("width");
		fScrollHLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBoundsGroup.AddSubView (&fScrollHLabel);

		fScrollVLabel.SetExtent (35, 9, 12, 38, eNoUpdate);
		fScrollVLabel.SetFont (&kApplicationFont);
		fScrollVLabel.SetText ("height");
		fScrollVLabel.SetJustification (AbstractTextView::eJustCenter);
		fScrollBoundsGroup.AddSubView (&fScrollVLabel);

		fScrollBoundsGroupTabLoop.AddFocus (&fScrollV);
		fScrollBoundsGroupTabLoop.AddFocus (&fScrollH);
	AddSubView (&fScrollBoundsGroup);

	fViewInfo.SetExtent (21, 4, 188, 308, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
	AddFocus (&fScrollBoundsGroupTabLoop);
}

#endif /* GUI */

Point	ScrollerInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (270, 315));
#elif   qMotifUI
	return (Point (297, 315));
#else
	return (Point (270, 315));
#endif /* GUI */
}

void	ScrollerInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfVerticalSBarOrigin = fVerticalSBar.GetOrigin ();
		fVerticalSBar.SetOrigin (kOriginalfVerticalSBarOrigin - Point (kSizeDelta.GetV (), 0));
		static const Point	kOriginalfHorizontalSBarOrigin = fHorizontalSBar.GetOrigin ();
		fHorizontalSBar.SetOrigin (kOriginalfHorizontalSBarOrigin - Point (kSizeDelta.GetV (), 0));
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfScrollBoundsGroupOrigin = fScrollBoundsGroup.GetOrigin ();
		fScrollBoundsGroup.SetOrigin (kOriginalfScrollBoundsGroupOrigin - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	GroupView::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"ScrollerItem.hh"
#include	"Dialog.hh"
#include	"EmilyWindow.hh"

ScrollerInfo::ScrollerInfo (ScrollerItem& view)
{
	GetViewItemInfo ().SetUpFromView (view);

	fScrollV.SetValue (view.GetScrollSize ().GetV (), eNoUpdate);
	fScrollH.SetValue (view.GetScrollSize ().GetH (), eNoUpdate);
	GetVSBarField ().SetOn (view.GetHasVerticalSBar ());
	GetHSBarField ().SetOn (view.GetHasHorizontalSBar ());
	
	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetVSBarField ().SetEnabled (fullEditing);
	GetHSBarField ().SetEnabled (fullEditing);
}

Point	ScrollerInfo::GetScrollBounds () const
{
	return (Point ((Coordinate) fScrollV.GetValue (), (Coordinate) fScrollH.GetValue ()));
}

