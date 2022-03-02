/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/TextEditInfo.cc,v 1.4 1992/07/21 18:28:39 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: TextEditInfo.cc,v $
 *		Revision 1.4  1992/07/21  18:28:39  sterling
 *		hi
 *
 *		Revision 1.3  1992/07/16  15:24:40  sterling
 *		hi
 *
 *		Revision 1.2  1992/06/25  10:15:58  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  1992/05/19  11:36:13  sterling
 *		hi
 *
 *		Revision 1.16  92/05/13  18:47:31  18:47:31  lewis (Lewis Pringle)
 *		STERL.
 *		
 *		Revision 1.15  92/04/08  17:23:19  17:23:19  sterling (Sterling Wight)
 *		Cleaned up dialogs for motif
 *		
 *
 *
 */

#include "FontPicker.hh"

// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "TextEditInfo.hh"


TextEditInfoX::TextEditInfoX () :
	fMaxLength (),
	fText (),
	fHorizontalSBar (),
	fRequireText (),
	fTitle (),
	fField1 (),
	fField2 (),
	fMultiLine (),
	fVerticalSBar (),
	fWordWrap (),
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

TextEditInfoX::~TextEditInfoX ()
{
	RemoveFocus (&fMaxLength);
	RemoveFocus (&fText);
	RemoveFocus (&fViewInfo);

	RemoveSubView (&fMaxLength);
	RemoveSubView (&fText);
	RemoveSubView (&fHorizontalSBar);
	RemoveSubView (&fRequireText);
	RemoveSubView (&fTitle);
	RemoveSubView (&fField1);
	RemoveSubView (&fField2);
	RemoveSubView (&fMultiLine);
	RemoveSubView (&fVerticalSBar);
	RemoveSubView (&fWordWrap);
	RemoveSubView (&fViewInfo);
}

#if   qMacUI

void	TextEditInfoX::BuildForMacUI ()
{
	SetSize (Point (310, 352), eNoUpdate);

	fMaxLength.SetExtent (70, 88, 19, 67, eNoUpdate);
	fMaxLength.SetFont (&kApplicationFont);
	fMaxLength.SetController (this);
	fMaxLength.SetMultiLine (False);
	fMaxLength.SetMinValue (0);
	AddSubView (&fMaxLength);

	fText.SetExtent (54, 200, 50, 147, eNoUpdate);
	fText.SetFont (&kApplicationFont);
	fText.SetController (this);
	fText.SetWordWrap (False);
	AddSubView (&fText);

	fHorizontalSBar.SetLabel ("Horizontal Scrollbar", eNoUpdate);
	fHorizontalSBar.SetExtent (38, 101, 15, 147, eNoUpdate);
	fHorizontalSBar.SetController (this);
	AddSubView (&fHorizontalSBar);

	fRequireText.SetLabel ("Require Text", eNoUpdate);
	fRequireText.SetExtent (24, 248, 14, 99, eNoUpdate);
	fRequireText.SetController (this);
	AddSubView (&fRequireText);

	fTitle.SetExtent (5, 5, 19, 342, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Text Edit Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fField1.SetExtent (72, 4, 15, 81, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Max Length:");
	AddSubView (&fField1);

	fField2.SetExtent (71, 163, 15, 32, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Text:");
	AddSubView (&fField2);

	fMultiLine.SetLabel ("Multiline", eNoUpdate);
	fMultiLine.SetExtent (23, 6, 15, 75, eNoUpdate);
	fMultiLine.SetController (this);
	AddSubView (&fMultiLine);

	fVerticalSBar.SetLabel ("Vertical Scrollbar", eNoUpdate);
	fVerticalSBar.SetExtent (23, 101, 15, 130, eNoUpdate);
	fVerticalSBar.SetController (this);
	AddSubView (&fVerticalSBar);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (37, 6, 15, 89, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fViewInfo.SetExtent (107, 4, 199, 345, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fMaxLength);
	AddFocus (&fText);
	AddFocus (&fViewInfo);
}

#elif qMotifUI

void	TextEditInfoX::BuildForMotifUI ()
{
	SetSize (Point (330, 374), eNoUpdate);

	fMaxLength.SetExtent (91, 85, 26, 67, eNoUpdate);
	fMaxLength.SetFont (&kApplicationFont);
	fMaxLength.SetController (this);
	fMaxLength.SetMultiLine (False);
	fMaxLength.SetMinValue (0);
	AddSubView (&fMaxLength);

	fText.SetExtent (91, 215, 26, 147, eNoUpdate);
	fText.SetFont (&kApplicationFont);
	fText.SetController (this);
	fText.SetWordWrap (False);
	AddSubView (&fText);

	fHorizontalSBar.SetLabel ("Horizontal Scrollbar", eNoUpdate);
	fHorizontalSBar.SetExtent (55, 112, 20, 159, eNoUpdate);
	fHorizontalSBar.SetController (this);
	AddSubView (&fHorizontalSBar);

	fRequireText.SetLabel ("Require Text", eNoUpdate);
	fRequireText.SetExtent (30, 262, 25, 109, eNoUpdate);
	fRequireText.SetController (this);
	AddSubView (&fRequireText);

	fTitle.SetExtent (5, 5, 19, 364, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Text Edit Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fField1.SetExtent (93, 1, 21, 81, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Max Length:");
	AddSubView (&fField1);

	fField2.SetExtent (93, 175, 21, 39, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Text:");
	AddSubView (&fField2);

	fMultiLine.SetLabel ("Multiline", eNoUpdate);
	fMultiLine.SetExtent (30, 4, 25, 84, eNoUpdate);
	fMultiLine.SetController (this);
	AddSubView (&fMultiLine);

	fVerticalSBar.SetLabel ("Vertical Scrollbar", eNoUpdate);
	fVerticalSBar.SetExtent (30, 112, 25, 136, eNoUpdate);
	fVerticalSBar.SetController (this);
	AddSubView (&fVerticalSBar);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (55, 4, 25, 105, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fViewInfo.SetExtent (123, 4, 204, 367, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fMultiLine);
	AddFocus (&fWordWrap);
	AddFocus (&fVerticalSBar);
	AddFocus (&fHorizontalSBar);
	AddFocus (&fRequireText);
	AddFocus (&fMaxLength);
	AddFocus (&fText);
	AddFocus (&fViewInfo);
}

#else

void	TextEditInfoX::BuildForUnknownGUI ();
{
	SetSize (Point (310, 352), eNoUpdate);

	fMaxLength.SetExtent (70, 88, 19, 67, eNoUpdate);
	fMaxLength.SetFont (&kApplicationFont);
	fMaxLength.SetController (this);
	fMaxLength.SetMultiLine (False);
	fMaxLength.SetMinValue (0);
	AddSubView (&fMaxLength);

	fText.SetExtent (54, 200, 50, 147, eNoUpdate);
	fText.SetFont (&kApplicationFont);
	fText.SetController (this);
	fText.SetWordWrap (False);
	AddSubView (&fText);

	fHorizontalSBar.SetLabel ("Horizontal Scrollbar", eNoUpdate);
	fHorizontalSBar.SetExtent (38, 101, 15, 147, eNoUpdate);
	fHorizontalSBar.SetController (this);
	AddSubView (&fHorizontalSBar);

	fRequireText.SetLabel ("Require Text", eNoUpdate);
	fRequireText.SetExtent (24, 248, 14, 99, eNoUpdate);
	fRequireText.SetController (this);
	AddSubView (&fRequireText);

	fTitle.SetExtent (5, 5, 19, 342, eNoUpdate);
	fTitle.SetFont (&kSystemFont);
	fTitle.SetText (" Set Text Edit Info");
	fTitle.SetJustification (AbstractTextView::eJustCenter);
	AddSubView (&fTitle);

	fField1.SetExtent (72, 4, 15, 81, eNoUpdate);
	fField1.SetFont (&kSystemFont);
	fField1.SetText ("Max Length:");
	AddSubView (&fField1);

	fField2.SetExtent (71, 163, 15, 32, eNoUpdate);
	fField2.SetFont (&kSystemFont);
	fField2.SetText ("Text:");
	AddSubView (&fField2);

	fMultiLine.SetLabel ("Multiline", eNoUpdate);
	fMultiLine.SetExtent (23, 6, 15, 75, eNoUpdate);
	fMultiLine.SetController (this);
	AddSubView (&fMultiLine);

	fVerticalSBar.SetLabel ("Vertical Scrollbar", eNoUpdate);
	fVerticalSBar.SetExtent (23, 101, 15, 130, eNoUpdate);
	fVerticalSBar.SetController (this);
	AddSubView (&fVerticalSBar);

	fWordWrap.SetLabel ("Word Wrap", eNoUpdate);
	fWordWrap.SetExtent (37, 6, 15, 89, eNoUpdate);
	fWordWrap.SetController (this);
	AddSubView (&fWordWrap);

	fViewInfo.SetExtent (107, 4, 199, 345, eNoUpdate);
	AddSubView (&fViewInfo);

	AddFocus (&fMaxLength);
	AddFocus (&fText);
	AddFocus (&fViewInfo);
}

#endif /* GUI */

Point	TextEditInfoX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (310, 352));
#elif   qMotifUI
	return (Point (330, 374));
#else
	return (Point (310, 352));
#endif /* GUI */
}

void	TextEditInfoX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfTextOrigin = fText.GetOrigin ();
		fText.SetOrigin (kOriginalfTextOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfTitleSize = fTitle.GetSize ();
		fTitle.SetSize (kOriginalfTitleSize - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfField2Origin = fField2.GetOrigin ();
		fField2.SetOrigin (kOriginalfField2Origin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfViewInfoSize = fViewInfo.GetSize ();
		fViewInfo.SetSize (kOriginalfViewInfoSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!
#include	"Dialog.hh"
#include	"TextEditItem.hh"
#include	"EmilyWindow.hh"

TextEditInfo::TextEditInfo (TextEditItem& view)
{
	fViewInfo.SetUpFromView (view);
	fTitle.SetText ("Edit " + view.GetFieldClass () + " Info", eNoUpdate);
	
	GetTextField ().SetText (view.GetText (), eNoUpdate);
	GetMaxLengthField ().SetValue (view.GetMaxLength (), eNoUpdate);
	GetWordWrapField ().SetOn (view.GetWordWrap (), eNoUpdate);
	GetRequireTextField ().SetOn (view.GetTextRequired (), eNoUpdate);
	GetMultiLineField ().SetOn (view.GetMultiLine (), eNoUpdate);
	GetVSBarField ().SetOn (view.GetHasVerticalSBar ());
	GetHSBarField ().SetOn (view.GetHasHorizontalSBar ());

	Boolean	fullEditing = Boolean (not EmilyWindow::GetCustomizeOnly ());
	GetVSBarField ().SetEnabled (fullEditing);
	GetHSBarField ().SetEnabled (fullEditing);
}


