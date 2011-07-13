/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextViewInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextViewInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  1992/05/19  11:36:14  sterling
 *		hi
 *
 *		Revision 1.16  92/05/13  18:47:32  18:47:32  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.15  92/04/08  17:23:21  17:23:21  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 */

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "TextViewInfo.hh"


TextViewInfoX::TextViewInfoX () :
	fTitle (),
	fText (),
	fWordWrap (),
	fTextLabel (),
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

TextViewInfoX::~TextViewInfoX ()
{
	RemoveFocus (&fText);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fTitle);
	RemoveSubView (&fText);
	RemoveSubView (&fWordWrap);
	RemoveSubView (&fTextLabel);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	TextViewInfoX::BuildForMacUI ()
{
	SetSize (Point (295, 320), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 309, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set TextView Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fText.SetExtent (27, 51, 61, 150, eNoUpdate);
	fText.SetFont (&kApplicationFont);
	fText.SetController (this);
	fText.SetWordWrap (False);
	AddSubView (&fText);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (26, 213, 16, 89, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fTextLabel.SetExtent (50, 18, 15, 32, eNoUpdate);
	fTextLabel.SetFont (&kSystemFont);
	fTextLabel.SetText ("Text:");
	AddSubView (&fTextLabel);

	fViewInfo.SetExtent (93, 4, 198, 307, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fText);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	TextViewInfoX::BuildForMotifUI ()
{
	SetSize (Point (295, 320), eNoUpdate);

	fTitle.SetExtent (5, 5, 21, 311, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set TextView Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fText.SetExtent (35, 51, 55, 150, eNoUpdate);
	fText.SetFont (&kApplicationFont);
	fText.SetController (this);
	fText.SetWordWrap (False);
	AddSubView (&fText);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (33, 210, 25, 105, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fTextLabel.SetExtent (51, 10, 21, 38, eNoUpdate);
	fTextLabel.SetFont (&kSystemFont);
	fTextLabel.SetText ("Text:");
	AddSubView (&fTextLabel);

	fViewInfo.SetExtent (93, 4, 200, 313, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fText);
	AddFocus (&fWordWrap);
	AddFocus (&fViewInfo);
}

#else

void	TextViewInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (295, 320), eNoUpdate);

	fTitle.SetExtent (5, 5, 16, 309, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set TextView Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fText.SetExtent (27, 51, 61, 150, eNoUpdate);
	fText.SetFont (&kApplicationFont);
	fText.SetController (this);
	fText.SetWordWrap (False);
	AddSubView (&fText);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (26, 213, 16, 89, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fTextLabel.SetExtent (50, 18, 15, 32, eNoUpdate);
	fTextLabel.SetFont (&kSystemFont);
	fTextLabel.SetText ("Text:");
	AddSubView (&fTextLabel);

	fViewInfo.SetExtent (93, 4, 198, 307, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fText);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	TextViewInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (295, 320));
#elif   qMotifUI
	return (Point (295, 320));
#else
	return (Point (295, 320));
#endif /* GUI */
}

void	TextViewInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfTextSize = fText.GetSize ();
		fText.SetSize (kOriginalfTextSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfWordWrapOrigin = fWordWrap.GetOrigin ();
		fWordWrap.SetOrigin (kOriginalfWordWrapOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"TextViewItem.hh"
#include	"EmilyWindow.hh"

TextViewInfo::TextViewInfo (TextViewItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	fText.SetText (view.GetText (), eNoUpdate);
	fWordWrap.SetOn (view.GetWordWrap (), eNoUpdate);
}

