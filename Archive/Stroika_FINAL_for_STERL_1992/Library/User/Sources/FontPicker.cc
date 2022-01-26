/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/FontPicker.cc,v 1.8 1992/09/08 16:00:29 lewis Exp $
 *
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: FontPicker.cc,v $
 *		Revision 1.8  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:22:12  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.5  1992/07/16  15:59:47  sterling
 *		hack to work around MPW Lib bug, removed reference to kSystemFont
 *
 *		Revision 1.3  1992/07/03  02:26:46  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.32  1992/05/18  17:31:10  lewis
 *		Worked around Emily CodeGen bug - must remove Focus on all items added for - Emily currently adds
 *		properly but only removes for ones added on the mac!!!
 *
 *		Revision 1.31  92/05/13  17:52:43  17:52:43  lewis (Lewis Pringle)
 *		STERL - react to new Emily codegen.
 *		
 *		Revision 1.27  92/03/26  17:30:57  17:30:57  sterling (Sterling Wight)
 *		made CalcDefaultSize const (temp hack to Emily changes code generation)
 *		
 *		Revision 1.19  1992/02/16  16:40:56  lewis
 *		Minor include cleanups.
 *
 *		Revision 1.14  1992/01/31  18:07:39  sterling
 *		worked around bad Emily code generation of #include (added semicolor)
 *
 *		Revision 1.13  1992/01/31  17:17:05  sterling
 *		totally redone, noe bootstrapped from Intercace builder
 *
 *		Revision 1.8  1992/01/31  17:14:08  sterling
 *		turned of WordWrap by default
 *
 *
 *
 *
 */

#include 	"Menu.hh"
#include	"Dialog.hh"
#include	"UserCommand.hh"


// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "FontPicker.hh"


FontPickerX::FontPickerX () :
	fStyle (),
	fFontSizeList (),
	fFontList (),
	fInheritFont (),
	fFontSize (),
	fSampleText (),
	fSizeLabel ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

FontPickerX::~FontPickerX ()
{
	SetCurrentFocus (Nil);	// LGP Added

// LGP Bug fix - must tell sterl to fix CODEGEN!!!
#if		qMacUI
	RemoveFocus (&fSampleText);
#elif	qMotifUI
	RemoveFocus (&fSampleText);
	RemoveFocus (&fInheritFont);
	RemoveFocus (&fStyle);
	RemoveFocus (&fFontSizeList);
#endif

	RemoveSubView (&fStyle);
	RemoveSubView (&fFontSizeList);
	RemoveSubView (&fFontList);
	RemoveSubView (&fInheritFont);
	RemoveSubView (&fFontSize);
	RemoveSubView (&fSampleText);
	RemoveSubView (&fSizeLabel);
}

#if   qMacUI

void	FontPickerX::BuildForMacUI ()
{
	SetFont (&kApplicationFont);
	SetSize (Point (190, 340), eNoUpdate);

	fStyle.SetExtent (9, 139, 20, 193, eNoUpdate);
	fStyle.SetHelp ("Click here to select the font style.");
	fStyle.AppendItem ("Plain");
	fStyle.AppendItem ("Bold");
	fStyle.AppendItem ("Italic");
	fStyle.AppendItem ("Underline");
	fStyle.AppendItem ("Outline");
	fStyle.AppendItem ("Shadow");
	fStyle.SetCurrentItem (1);
	fStyle.SetLabel ("Style:");
	fStyle.SetController (this);
	AddSubView (&fStyle);

	fFontSizeList.SetExtent (33, 139, 122, 42, eNoUpdate);
	fFontSizeList.SetHelp ("Choose from among a set of predefined sizes. Enter a special size in the text box below.");
	fFontSizeList.SetBorder (1, 1, eNoUpdate);
	fFontSizeList.AddItem ("6");
	fFontSizeList.AddItem ("8");
	fFontSizeList.AddItem ("9");
	fFontSizeList.AddItem ("10");
	fFontSizeList.AddItem ("12");
	fFontSizeList.AddItem ("14");
	fFontSizeList.AddItem ("18");
	fFontSizeList.AddItem ("36");
	fFontSizeList.AddItem ("72");
	fFontSizeList.SetController (this);
	AddSubView (&fFontSizeList);

	fFontList.SetExtent (33, 9, 152, 112, eNoUpdate);
	fFontList.SetHelp ("Select a single font name.");
	fFontList.SetBorder (1, 1, eNoUpdate);
	fFontList.SetController (this);
	AddSubView (&fFontList);

	fInheritFont.SetLabel ("Inherit Font", eNoUpdate);
	fInheritFont.SetExtent (170, 222, 14, 96, eNoUpdate);
	fInheritFont.SetController (this);
	AddSubView (&fInheritFont);

	fFontSize.SetExtent (166, 163, 18, 32, eNoUpdate);
	fFontSize.SetHelp ("Choose a special font size.");
	fFontSize.SetFont (&kApplicationFont);
	fFontSize.SetController (this);
	fFontSize.SetMultiLine (False);
	fFontSize.SetMaxValue (255);
	fFontSize.SetMinValue (1);
	AddSubView (&fFontSize);

	fSampleText.SetExtent (33, 199, 123, 132, eNoUpdate);
	fSampleText.SetHelp ("Type any text here you wish, to see what the font, size, and style you have chosen look like.");
	fSampleText.SetFont (&kApplicationFont);
	fSampleText.SetController (this);
	fSampleText.SetText ("The quick brown fox jumped over the shaggy dog.");
	AddSubView (&fSampleText);

	fSizeLabel.SetExtent (168, 127, 16, 32, eNoUpdate);
// terrible quickee workaround for corrupt object file SSW 7/10
#if 0
	fSizeLabel.SetFont (&kSystemFont);
#else
	Font f = Font ("Chicago", 12);
	fSizeLabel.SetFont (&f);
#endif
	fSizeLabel.SetText ("Size:");
	AddSubView (&fSizeLabel);

	AddFocus (&fSampleText);
}

#elif qMotifUI

void	FontPickerX::BuildForMotifUI ()
{
	SetFont (&kApplicationFont);
	SetSize (Point (190, 340), eNoUpdate);

	fStyle.SetExtent (0, 139, 29, 193, eNoUpdate);
	fStyle.SetHelp ("Click here to select the font style.");
	fStyle.AppendItem ("Plain");
	fStyle.AppendItem ("Bold");
	fStyle.AppendItem ("Italic");
	fStyle.AppendItem ("Underline");
	fStyle.AppendItem ("Outline");
	fStyle.AppendItem ("Shadow");
	fStyle.SetCurrentItem (1);
	fStyle.SetLabel ("Style:");
	fStyle.SetController (this);
	AddSubView (&fStyle);

	fFontSizeList.SetExtent (33, 139, 122, 42, eNoUpdate);
	fFontSizeList.SetHelp ("Choose from among a set of predefined sizes. Enter a special size in the text box below.");
	fFontSizeList.SetBorder (1, 1, eNoUpdate);
	fFontSizeList.AddItem ("6");
	fFontSizeList.AddItem ("8");
	fFontSizeList.AddItem ("9");
	fFontSizeList.AddItem ("10");
	fFontSizeList.AddItem ("12");
	fFontSizeList.AddItem ("14");
	fFontSizeList.AddItem ("18");
	fFontSizeList.AddItem ("36");
	fFontSizeList.AddItem ("72");
	fFontSizeList.SetController (this);
	AddSubView (&fFontSizeList);

	fFontList.SetExtent (33, 9, 152, 112, eNoUpdate);
	fFontList.SetHelp ("Select a single font name.");
	fFontList.SetBorder (1, 1, eNoUpdate);
	fFontList.SetController (this);
	AddSubView (&fFontList);

	fInheritFont.SetLabel ("Inherit Font", eNoUpdate);
	fInheritFont.SetExtent (164, 219, 25, 107, eNoUpdate);
	fInheritFont.SetController (this);
	AddSubView (&fInheritFont);

	fFontSize.SetExtent (164, 162, 24, 32, eNoUpdate);
	fFontSize.SetHelp ("Choose a special font size.");
	fFontSize.SetMargin (1, 1, eNoUpdate);
	fFontSize.SetFont (&kApplicationFont);
	fFontSize.SetController (this);
	fFontSize.SetMultiLine (False);
	fFontSize.SetMaxValue (255);
	fFontSize.SetMinValue (1);
	AddSubView (&fFontSize);

	fSampleText.SetExtent (33, 199, 123, 132, eNoUpdate);
	fSampleText.SetHelp ("Type any text here you wish, to see what the font, size, and style you have chosen look like.");
	fSampleText.SetFont (&kApplicationFont);
	fSampleText.SetController (this);
	fSampleText.SetText ("The quick brown fox jumped over the shaggy dog.");
	AddSubView (&fSampleText);

	fSizeLabel.SetExtent (168, 127, 21, 36, eNoUpdate);
	fSizeLabel.SetFont (&kSystemFont);
	fSizeLabel.SetText ("Size:");
	AddSubView (&fSizeLabel);

	AddFocus (&fSampleText);
	AddFocus (&fInheritFont);
	AddFocus (&fStyle);
	AddFocus (&fFontSizeList);
}

#else

void	FontPickerX::BuildForUnknownGUI ();
{
	SetFont (&kApplicationFont);
	SetSize (Point (190, 340), eNoUpdate);

	fStyle.SetExtent (9, 139, 20, 193, eNoUpdate);
	fStyle.SetHelp ("Click here to select the font style.");
	fStyle.AppendItem ("Plain");
	fStyle.AppendItem ("Bold");
	fStyle.AppendItem ("Italic");
	fStyle.AppendItem ("Underline");
	fStyle.AppendItem ("Outline");
	fStyle.AppendItem ("Shadow");
	fStyle.SetCurrentItem (1);
	fStyle.SetLabel ("Style:");
	fStyle.SetController (this);
	AddSubView (&fStyle);

	fFontSizeList.SetExtent (33, 139, 122, 42, eNoUpdate);
	fFontSizeList.SetHelp ("Choose from among a set of predefined sizes. Enter a special size in the text box below.");
	fFontSizeList.SetBorder (1, 1, eNoUpdate);
	fFontSizeList.AddItem ("6");
	fFontSizeList.AddItem ("8");
	fFontSizeList.AddItem ("9");
	fFontSizeList.AddItem ("10");
	fFontSizeList.AddItem ("12");
	fFontSizeList.AddItem ("14");
	fFontSizeList.AddItem ("18");
	fFontSizeList.AddItem ("36");
	fFontSizeList.AddItem ("72");
	fFontSizeList.SetController (this);
	AddSubView (&fFontSizeList);

	fFontList.SetExtent (33, 9, 152, 112, eNoUpdate);
	fFontList.SetHelp ("Select a single font name.");
	fFontList.SetBorder (1, 1, eNoUpdate);
	fFontList.SetController (this);
	AddSubView (&fFontList);

	fInheritFont.SetLabel ("Inherit Font", eNoUpdate);
	fInheritFont.SetExtent (170, 222, 14, 96, eNoUpdate);
	fInheritFont.SetController (this);
	AddSubView (&fInheritFont);

	fFontSize.SetExtent (166, 163, 18, 32, eNoUpdate);
	fFontSize.SetHelp ("Choose a special font size.");
	fFontSize.SetFont (&kApplicationFont);
	fFontSize.SetController (this);
	fFontSize.SetMultiLine (False);
	fFontSize.SetMaxValue (255);
	fFontSize.SetMinValue (1);
	AddSubView (&fFontSize);

	fSampleText.SetExtent (33, 199, 123, 132, eNoUpdate);
	fSampleText.SetHelp ("Type any text here you wish, to see what the font, size, and style you have chosen look like.");
	fSampleText.SetFont (&kApplicationFont);
	fSampleText.SetController (this);
	fSampleText.SetText ("The quick brown fox jumped over the shaggy dog.");
	AddSubView (&fSampleText);

	fSizeLabel.SetExtent (168, 127, 16, 32, eNoUpdate);
	fSizeLabel.SetFont (&kSystemFont);
	fSizeLabel.SetText ("Size:");
	AddSubView (&fSizeLabel);

	AddFocus (&fSampleText);
}

#endif /* GUI */

Point	FontPickerX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (190, 340));
#elif   qMotifUI
	return (Point (190, 340));
#else
	return (Point (190, 340));
#endif /* GUI */
}

void	FontPickerX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfFontSizeListOrigin = fFontSizeList.GetOrigin ();
		fFontSizeList.SetOrigin (kOriginalfFontSizeListOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfFontListSize = fFontList.GetSize ();
		fFontList.SetSize (kOriginalfFontListSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfInheritFontOrigin = fInheritFont.GetOrigin ();
		fInheritFont.SetOrigin (kOriginalfInheritFontOrigin - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfFontSizeOrigin = fFontSize.GetOrigin ();
		fFontSize.SetOrigin (kOriginalfFontSizeOrigin - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfSampleTextOrigin = fSampleText.GetOrigin ();
		fSampleText.SetOrigin (kOriginalfSampleTextOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfSizeLabelOrigin = fSizeLabel.GetOrigin ();
		fSizeLabel.SetOrigin (kOriginalfSizeLabelOrigin - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!

FontStylePopUp::FontStylePopUp () :
	OptionMenuButton ()
{
#if 	qMacUI
	SetAutoResize (True);
#endif
}



SetFontCommand::SetFontCommand (View& view, const Font* oldFont, const Font* newFont) :
	Command (eSetFont, kUndoable),
	fView (view),
	fOldFont (Nil),
	fNewFont (Nil)
{
	if (oldFont != Nil) {
		fOldFont = new Font (*oldFont);
	}

	if (newFont != Nil) {
		fNewFont = new Font (*newFont);
	}

	SetName ("Set Font");
}

SetFontCommand::SetFontCommand (View& view, const String& fontName) :
	Command (eSetFont, kUndoable),
	fView (view),
	fOldFont (Nil),
	fNewFont (Nil)
{
	if (view.GetFont () != Nil) {
		fOldFont = new Font (*view.GetFont ());
	}

	Font f = fView.GetEffectiveFont ();
	f.SetName (fontName);
	fNewFont = new Font (f);

	SetName ("Set Font");
}

SetFontCommand::SetFontCommand (View& view, FontSize fontSize) :
	Command (eSetFont, kUndoable),
	fView (view),
	fOldFont (Nil),
	fNewFont (Nil)
{
	if (view.GetFont () != Nil) {
		fOldFont = new Font (*view.GetFont ());
	}

	Font f = fView.GetEffectiveFont ();
	f.SetSize (fontSize);
	fNewFont = new Font (f);
	
	SetName ("Set Font Size");
}

SetFontCommand::SetFontCommand (View& view, CommandNumber fontStyleCommand) :
	Command (eSetFont, kUndoable),
	fView (view),
	fOldFont (Nil),
	fNewFont (Nil)
{
	if (view.GetFont () != Nil) {
		fOldFont = new Font (*view.GetFont ());
	}

	Font f = fView.GetEffectiveFont ();
	int fontStyle = -1;
	switch (fontStyleCommand) {
		case ePlainFontCommand:
			break;
			
		case eBoldFontCommand:
			fontStyle = Font::eBoldFontStyle;
			break;
			
		case eItalicFontCommand:
			fontStyle = Font::eItalicFontStyle;
			break;
			
		case eUnderlineFontCommand:
			fontStyle = Font::eUnderlineFontStyle;
			break;
			
		case eOutlineFontCommand:
			fontStyle = Font::eOutlineFontStyle;
			break;
			
		case eShadowFontCommand:
			fontStyle = Font::eShadowFontStyle;
			break;
		
		default:
			RequireNotReached ();
	}
	
	if (fontStyle == -1) {
		f.SetStyle (kPlainFontStyle);
	}
	else {
		FontStyle	correctTypeFontStyle = FontStyle (fontStyle);
		
		Set_BitString(FontStyle)	style = f.GetStyle ();
		if (style.Contains (correctTypeFontStyle)) {
			style.Remove (correctTypeFontStyle);
		}
		else {
			style.Add (correctTypeFontStyle);
		}
		f.SetStyle (style);
	}

	fNewFont = new Font (f);

	SetName ("Set Font Style");
}

SetFontCommand::~SetFontCommand ()
{
	delete fOldFont;
	delete fNewFont;
}

void	SetFontCommand::DoIt ()
{
	fView.SetFont (fNewFont);
	fView.Refresh (View::eImmediateUpdate);
	Command::DoIt ();
}

void	SetFontCommand::UnDoIt ()
{
	fView.SetFont (fOldFont);
	fView.Refresh (View::eImmediateUpdate);
	Command::UnDoIt ();
}





FontPicker::FontPicker (const Font& font) :
	fFont (font)
{
	Initialize ();
	fInheritFont.SetVisible (not Panel::kVisible, eNoUpdate);
}

FontPicker::FontPicker (const Font* font) :
	fFont (kApplicationFont)
{
	if (font == Nil) {
		fInheritFont.SetOn (Toggle::kOn, eNoUpdate);
	}
	else {
		fFont = *font;
	}
	Initialize ();
}

void	FontPicker::Initialize ()
{
	for (FontIterator it = FontIterator (); not it.Done (); it.Next ()) {
		fFontList.AddItem (it.Current ().GetName ());
	}
	
	if (not fInheritFont.GetOn ()) {
		AbstractPickListItem*	item = fFontList.StringToItem (fFont.GetName ());
		if (item != Nil) {
			item->SetSelected (PickList::kSelected, eNoUpdate);
		}
		fFontSize.SetValue (fFont.GetSize (), eNoUpdate);
		item = fFontSizeList.StringToItem (fFontSize.GetText ());
		if (item != Nil) {
			item->SetSelected (PickList::kSelected, eNoUpdate);
		}

		UInt16	styleCount = 0;
		Set_BitString(FontStyle)	style = fFont.GetStyle ();
		if (style.Contains (Font::eBoldFontStyle)) {
			fStyle.SetCurrentItem (2, eNoUpdate);
		}
		else if (style.Contains (Font::eItalicFontStyle)) {
			fStyle.SetCurrentItem (3, eNoUpdate);
		}
		else if (style.Contains (Font::eUnderlineFontStyle)) {
			fStyle.SetCurrentItem (4, eNoUpdate);
		}
		else if (style.Contains (Font::eOutlineFontStyle)) {
			fStyle.SetCurrentItem (5, eNoUpdate);
		}
		else if (style.Contains (Font::eShadowFontStyle)) {
			fStyle.SetCurrentItem (6, eNoUpdate);
		}
		else {
			fStyle.SetCurrentItem (1, eNoUpdate);
		}
	}
}
	
void	FontPicker::ButtonPressed (AbstractButton* button)
{
	if (button == &fFontList) {
		String	fontName = fFontList.GetSelectedString ();
		if (fontName != kEmptyString) {
			fFont.SetName (fontName);
			fSampleText.SetFont (&fFont);
			fSampleText.Refresh (eImmediateUpdate);
			fInheritFont.SetOn (not Toggle::kOn);
		}
	}
	else if (button == &fFontSizeList) {
		String	fontSize = fFontSizeList.GetSelectedString ();
		if (fontSize != kEmptyString) {
			fFontSize.SetText (fontSize);
			UpdateFontSizeChange (False);
			fInheritFont.SetOn (not Toggle::kOn);
		}
	}

	else if (button == &fStyle) {
		Set_BitString(FontStyle)	style = kPlainFontStyle;
		CollectionSize current = fStyle.GetCurrentItem ();

		if (current == 2) {
			style.Add (Font::eBoldFontStyle);
		}
		else if (current == 3) {
			style.Add (Font::eItalicFontStyle);
		}
		else if (current == 4) {
			style.Add (Font::eUnderlineFontStyle);
		}
		else if (current == 5) {
			style.Add (Font::eOutlineFontStyle);
		}
		else if (current == 6) {
			style.Add (Font::eShadowFontStyle);
		}
		fFont.SetStyle (style);
		fSampleText.SetFont (&fFont);
		fSampleText.Refresh ();
		
		if (style != kPlainFontStyle) {
			fInheritFont.SetOn (not Toggle::kOn);
		}
	}
	else if (button == &fInheritFont) {
		fFontSize.SetValue (12);
		fFontList.SelectAll (not PickList::kSelected);
		fFontSizeList.SelectAll (not PickList::kSelected);
	}
	else {
		FontPickerX::ButtonPressed (button);
	}
}

void	FontPicker::SetCurrentFocus_ (FocusItem* newFocus, Panel::UpdateMode update, Boolean validate)	
{
	FocusItem*	oldFocus = GetCurrentFocus ();
	FontPickerX::SetCurrentFocus_ (newFocus, update, validate);
	if (oldFocus == &fFontSize)  {
		UpdateFontSizeChange ();
	}
}

void	FontPicker::UpdateFontSizeChange (Boolean updatePickList, Panel::UpdateMode update)
{
	AbstractPickListItem*	item = fFontSizeList.StringToItem (fFontSize.GetText ());
	if (item != Nil) {
		if (updatePickList) {
			item->SetSelected (PickList::kSelected, update);
			return;
		}
	}
	else {
		fFontSizeList.SelectAll (not PickList::kSelected, update);
	}
	Assert (fFontSize.GetValue () <= 255);
	if (fFontSize.GetValue () != fFont.GetSize ()) {
		fFont.SetSize ((UInt16) fFontSize.GetValue ());


// this seems to cause crash under motif - but all other stuff works fine...
// Seems that if you cause this to happen while during a blink of the caret, and try then to set the
// font you loose - I did reproduce this bug in an OLD (3 weeks old) Emily, and so it is not new - just
// happens much more often now for some reason... LGP May 18, 1992...
		fSampleText.SetFont (&fFont);

		fSampleText.Refresh (update);
		fInheritFont.SetOn (not Toggle::kOn);
	}
}

SetFontCommand*	FontPicker::PickFont (View* view)
{
	RequireNotNil (view);
	const Font*	oldFont = view->GetFont ();
	FontPicker	picker = FontPicker (view->GetFont ());
	Dialog d = Dialog (&picker, &picker, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());

	if (d.Pose ()) {
		if (picker.fInheritFont.GetOn ()) {
			return (new SetFontCommand (*view, oldFont, Nil));
		}
		else {
			return (new SetFontCommand (*view, oldFont, &picker.fFont));
		}
	}
	else {
		return (Nil);
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

