/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PixelMapButtonInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PixelMapButtonInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  1992/05/19  11:36:01  sterling
 *		hi
 *
 *		Revision 1.16  92/05/13  18:47:17  18:47:17  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.13  92/04/08  17:23:05  17:23:05  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "PixelMapButtonInfo.hh"


PixelMapButtonInfoX::PixelMapButtonInfoX () :
	fTitle (),
	fEditPixelMap (),
	fPixelMapDisplay (),
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

PixelMapButtonInfoX::~PixelMapButtonInfoX ()
{
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fEditPixelMap);
	RemoveSubView (&fPixelMapDisplay);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	PixelMapButtonInfoX::BuildForMacUI ()
{
	SetSize (Point (285, 324), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 315, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set PixelMap Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEditPixelMap.SetExtent (36, 102, 19, 99, eNoUpdate);
	fEditPixelMap.SetLabel ("Edit PixelMap", eNoUpdate);
	fEditPixelMap.SetController (this);
	AddSubView (&fEditPixelMap);

	fPixelMapDisplay.SetExtent (29, 47, 32, 32, eNoUpdate);
	fPixelMapDisplay.SetController (this);
	AddSubView (&fPixelMapDisplay);

	fViewInfo.SetExtent (69, 1, 214, 321, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	PixelMapButtonInfoX::BuildForMotifUI ()
{
	SetSize (Point (285, 324), eNoUpdate);

	fTitle.SetExtent (5, 5, 22, 313, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set PixelMap Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEditPixelMap.SetExtent (36, 102, 23, 109, eNoUpdate);
	fEditPixelMap.SetLabel ("Edit PixelMap", eNoUpdate);
	fEditPixelMap.SetController (this);
	AddSubView (&fEditPixelMap);

	fPixelMapDisplay.SetExtent (29, 47, 32, 32, eNoUpdate);
	fPixelMapDisplay.SetController (this);
	AddSubView (&fPixelMapDisplay);

	fViewInfo.SetExtent (69, 1, 214, 321, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
	AddFocus (&fEditPixelMap);
	AddFocus (&fPixelMapDisplay);
}

#else

void	PixelMapButtonInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (285, 324), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 315, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set PixelMap Button Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fEditPixelMap.SetExtent (36, 102, 19, 99, eNoUpdate);
	fEditPixelMap.SetLabel ("Edit PixelMap", eNoUpdate);
	fEditPixelMap.SetController (this);
	AddSubView (&fEditPixelMap);

	fPixelMapDisplay.SetExtent (29, 47, 32, 32, eNoUpdate);
	fPixelMapDisplay.SetController (this);
	AddSubView (&fPixelMapDisplay);

	fViewInfo.SetExtent (69, 1, 214, 321, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	PixelMapButtonInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (285, 324));
#elif   qMotifUI
	return (Point (285, 324));
#else
	return (Point (285, 324));
#endif /* GUI */
}

void	PixelMapButtonInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"PixelMapButton.hh"
#include	"PixelMapEditor.hh"
#include	"PixelMapButtonItem.hh"
#include	"Dialog.hh"

PixelMapButtonInfo::PixelMapButtonInfo (PixelMapButtonItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	fPixelMapDisplay.SetPixelMap (view.GetPixelMapButton ().GetPixelMap ());
//	fPixelMapDisplay.SetCanStayOn (False);
}

PixelMap	PixelMapButtonInfo::GetPixelMap () const
{
	return (fPixelMapDisplay.GetPixelMap ());
}

void	PixelMapButtonInfo::ButtonPressed (AbstractButton* button)
{
	if ((button == &fEditPixelMap) or (button == &fPixelMapDisplay)) {
		PixelMapEditor	editor = PixelMapEditor ();
		editor.SetPixelMap (fPixelMapDisplay.GetPixelMap ());
		
		Dialog d = Dialog (&editor, &editor, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
		d.SetDefaultButton (d.GetOKButton ());
		
		if (d.Pose ()) {
			fPixelMapDisplay.SetPixelMap (editor.GetPixelMap ());
		}
	}
	PixelMapButtonInfoX::ButtonPressed (button);
}
