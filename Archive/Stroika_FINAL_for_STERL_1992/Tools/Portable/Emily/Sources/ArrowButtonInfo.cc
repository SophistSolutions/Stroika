/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ArrowButtonInfo.cc,v 1.3 1992/09/01 17:25:44 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ArrowButtonInfo.cc,v $
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
// Revision 1.2  1992/07/21  18:28:39  sterling
// hi
//
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

#include "ArrowButtonInfo.hh"


ArrowButtonInfoX::ArrowButtonInfoX () :
	fTitle (),
	fDirection (kEmptyString),
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

ArrowButtonInfoX::~ArrowButtonInfoX ()
{
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fDirection);
		fDirection.RemoveSubView (&fRight);
		fDirectionRadioBank.RemoveRadioButton (&fRight);
		fDirection.RemoveSubView (&fDown);
		fDirectionRadioBank.RemoveRadioButton (&fDown);
		fDirection.RemoveSubView (&fLeft);
		fDirectionRadioBank.RemoveRadioButton (&fLeft);
		fDirection.RemoveSubView (&fUp);
		fDirectionRadioBank.RemoveRadioButton (&fUp);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	ArrowButtonInfoX::BuildForMacUI ()
{
	SetSize (Point (268, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 315, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set ArrowButton Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fDirection.SetExtent (23, 99, 48, 124, eNoUpdate);
	fDirection.SetLabel ("Direction", eNoUpdate);
		fDirectionRadioBank.SetController (this);

		fRight.SetLabel ("Right", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fRight);
		fRight.SetExtent (31, 64, 15, 53, eNoUpdate);
		fDirection.AddSubView (&fRight);

		fDown.SetLabel ("Down", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fDown);
		fDown.SetExtent (31, 4, 15, 55, eNoUpdate);
		fDirection.AddSubView (&fDown);

		fLeft.SetLabel ("Left", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fLeft);
		fLeft.SetExtent (14, 64, 15, 46, eNoUpdate);
		fDirection.AddSubView (&fLeft);

		fUp.SetLabel ("Up", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fUp);
		fUp.SetExtent (14, 4, 15, 35, eNoUpdate);
		fDirection.AddSubView (&fUp);

	AddSubView (&fDirection);

	fViewInfo.SetExtent (72, 6, 192, 316, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	ArrowButtonInfoX::BuildForMotifUI ()
{
	SetSize (Point (290, 333), eNoUpdate);

	fTitle.SetExtent (5, 5, 19, 320, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set ArrowButton Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fDirection.SetExtent (29, 108, 61, 124, eNoUpdate);
	fDirection.SetLabel ("Direction", eNoUpdate);
		fDirectionRadioBank.SetController (this);

		fRight.SetLabel ("Right", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fRight);
		fRight.SetExtent (41, 64, 20, 63, eNoUpdate);
		fDirection.AddSubView (&fRight);

		fDown.SetLabel ("Down", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fDown);
		fDown.SetExtent (41, 3, 20, 65, eNoUpdate);
		fDirection.AddSubView (&fDown);

		fLeft.SetLabel ("Left", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fLeft);
		fLeft.SetExtent (20, 64, 20, 56, eNoUpdate);
		fDirection.AddSubView (&fLeft);

		fUp.SetLabel ("Up", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fUp);
		fUp.SetExtent (20, 4, 20, 45, eNoUpdate);
		fDirection.AddSubView (&fUp);

		fDirectionTabLoop.AddFocus (&fUp);
		fDirectionTabLoop.AddFocus (&fDown);
		fDirectionTabLoop.AddFocus (&fLeft);
		fDirectionTabLoop.AddFocus (&fRight);
	AddSubView (&fDirection);

	fViewInfo.SetExtent (90, 1, 198, 328, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
	AddFocus (&fDirectionTabLoop);
}

#else

void	ArrowButtonInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (268, 325), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 315, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set ArrowButton Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fDirection.SetExtent (23, 99, 48, 124, eNoUpdate);
	fDirection.SetLabel ("Direction", eNoUpdate);
		fDirectionRadioBank.SetController (this);

		fRight.SetLabel ("Right", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fRight);
		fRight.SetExtent (31, 64, 15, 53, eNoUpdate);
		fDirection.AddSubView (&fRight);

		fDown.SetLabel ("Down", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fDown);
		fDown.SetExtent (31, 4, 15, 55, eNoUpdate);
		fDirection.AddSubView (&fDown);

		fLeft.SetLabel ("Left", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fLeft);
		fLeft.SetExtent (14, 64, 15, 46, eNoUpdate);
		fDirection.AddSubView (&fLeft);

		fUp.SetLabel ("Up", eNoUpdate);
		fDirectionRadioBank.AddRadioButton (&fUp);
		fUp.SetExtent (14, 4, 15, 35, eNoUpdate);
		fDirection.AddSubView (&fUp);

	AddSubView (&fDirection);

	fViewInfo.SetExtent (72, 6, 192, 316, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	ArrowButtonInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (268, 325));
#elif   qMotifUI
	return (Point (290, 333));
#else
	return (Point (268, 325));
#endif /* GUI */
}

void	ArrowButtonInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"ArrowButton.hh"
#include	"ArrowButtonItem.hh"

ArrowButtonInfo::ArrowButtonInfo (ArrowButtonItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	
	switch (view.GetArrowButton ().GetDirection ()) {
		case AbstractArrowButton::eUp:		fUp.SetOn (True);		break;
		case AbstractArrowButton::eDown:	fDown.SetOn (True);		break;
		case AbstractArrowButton::eLeft:	fLeft.SetOn (True);		break;
		case AbstractArrowButton::eRight:	fRight.SetOn (True);	break;
		default:							AssertNotReached ();
	}
}

AbstractArrowButton::ArrowDirection	ArrowButtonInfo::GetDirection () const
{
	if (fUp.GetOn ()) {
		return (AbstractArrowButton::eUp);
	}
	else if (fDown.GetOn ()) {
		return (AbstractArrowButton::eDown);
	}
	else if (fLeft.GetOn ()) {
		return (AbstractArrowButton::eLeft);
	}
	
	Assert (fRight.GetOn ());
	return (AbstractArrowButton::eRight);
}
