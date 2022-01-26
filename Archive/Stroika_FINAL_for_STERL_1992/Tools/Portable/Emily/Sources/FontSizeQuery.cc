/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FontSizeQuery.cc,v 1.3 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * To Do:
 *
 * $Log: FontSizeQuery.cc,v $
 *		Revision 1.3  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  1992/05/19  11:35:48  sterling
 *		hi
 *
 *		Revision 1.16  92/05/13  18:47:05  18:47:05  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.15  92/04/08  17:22:51  17:22:51  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 */


// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "FontSizeQuery.hh"


FontSizeQueryX::FontSizeQueryX () :
	fTitle (),
	fFontSize (),
	fFontSizeLabel ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

FontSizeQueryX::~FontSizeQueryX ()
{
	RemoveFocus (&fFontSize);

	RemoveSubView (&fTitle);
	RemoveSubView (&fFontSize);
	RemoveSubView (&fFontSizeLabel);
}

#if   qMacUI

void	FontSizeQueryX::BuildForMacUI ()
{
	SetSize (Point (110, 193), eNoUpdate);

	fTitle.SetExtent (5, 5, 70, 186, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("What size font would you like to set the selected items to?");
	AddSubView (&fTitle);

	fFontSize.SetExtent (79, 92, 22, 70, eNoUpdate);
	fFontSize.SetFont (&kApplicationFont);
	fFontSize.SetController (this);
	fFontSize.SetMultiLine (False);
	fFontSize.SetMaxValue (255);
	fFontSize.SetMinValue (1);
	AddSubView (&fFontSize);

	fFontSizeLabel.SetExtent (82, 27, 15, 64, eNoUpdate);
	fFontSizeLabel.SetFont (&kSystemFont);
	fFontSizeLabel.SetText ("Font Size:");
	AddSubView (&fFontSizeLabel);

	AddFocus (&fFontSize);
}

#elif qMotifUI

void	FontSizeQueryX::BuildForMotifUI ()
{
	SetSize (Point (110, 193), eNoUpdate);

	fTitle.SetExtent (5, 5, 70, 186, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("What size font would you like to set the selected items to?");
	AddSubView (&fTitle);

	fFontSize.SetExtent (81, 83, 26, 70, eNoUpdate);
	fFontSize.SetFont (&kApplicationFont);
	fFontSize.SetController (this);
	fFontSize.SetMultiLine (False);
	fFontSize.SetMaxValue (255);
	fFontSize.SetMinValue (1);
	AddSubView (&fFontSize);

	fFontSizeLabel.SetExtent (82, 16, 21, 69, eNoUpdate);
	fFontSizeLabel.SetFont (&kSystemFont);
	fFontSizeLabel.SetText ("Font Size:");
	AddSubView (&fFontSizeLabel);

	AddFocus (&fFontSize);
}

#else

void	FontSizeQueryX::BuildForUnknownGUI ();
{
	SetSize (Point (110, 193), eNoUpdate);

	fTitle.SetExtent (5, 5, 70, 186, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetEditMode (AbstractTextEdit::eDisplayOnly);
	fTitle.SetBorder (0, 0, eNoUpdate);
	fTitle.SetMargin (0, 0, eNoUpdate);
	fTitle.SetText ("What size font would you like to set the selected items to?");
	AddSubView (&fTitle);

	fFontSize.SetExtent (79, 92, 22, 70, eNoUpdate);
	fFontSize.SetFont (&kApplicationFont);
	fFontSize.SetController (this);
	fFontSize.SetMultiLine (False);
	fFontSize.SetMaxValue (255);
	fFontSize.SetMinValue (1);
	AddSubView (&fFontSize);

	fFontSizeLabel.SetExtent (82, 27, 15, 64, eNoUpdate);
	fFontSizeLabel.SetFont (&kSystemFont);
	fFontSizeLabel.SetText ("Font Size:");
	AddSubView (&fFontSizeLabel);

	AddFocus (&fFontSize);
}

#endif /* GUI */

Point	FontSizeQueryX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (110, 193));
#elif   qMotifUI
	return (Point (110, 193));
#else
	return (Point (110, 193));
#endif /* GUI */
}

void	FontSizeQueryX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfFontSizeSize = fFontSize.GetSize ();
		fFontSize.SetSize (kOriginalfFontSizeSize - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
FontSizeQuery::FontSizeQuery () :
	FontSizeQueryX ()
{
}

UInt16	FontSizeQuery::GetFontSize () const
{
	return ((UInt16) fFontSize.GetValue ());
}
