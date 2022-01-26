// text before here will be retained: Do not remove or modify this line!!!


#include "Language.hh"
#include "Shape.hh"

#include "ColorPicker.hh"


ColorPickerX::ColorPickerX () :
	fNamedColors (),
	fColorDisplay (),
	fRed (),
	fBlue (),
	fGreen (),
	fBlueLabel (),
	fRedLabel (),
	fGreenLabel ()
{
#if   qMacUI
	BuildForMacUI ();
#elif qMotifUI
	BuildForMotifUI ();
#else
	BuildForUnknownGUI ();
#endif /* GUI */
}

ColorPickerX::~ColorPickerX ()
{
#if		qMotifUI
	RemoveFocus (&fNamedColors);
#endif
	RemoveFocus (&fRed);
	RemoveFocus (&fGreen);
	RemoveFocus (&fBlue);

	RemoveSubView (&fNamedColors);
	RemoveSubView (&fColorDisplay);
	RemoveSubView (&fRed);
	RemoveSubView (&fBlue);
	RemoveSubView (&fGreen);
	RemoveSubView (&fBlueLabel);
	RemoveSubView (&fRedLabel);
	RemoveSubView (&fGreenLabel);
}

#if   qMacUI

void	ColorPickerX::BuildForMacUI ()
{
	SetSize (Point (224, 340), eNoUpdate);

	fNamedColors.SetExtent (12, 10, 209, 189, eNoUpdate);
	fNamedColors.SetController (this);
	AddSubView (&fNamedColors);

	fColorDisplay.SetExtent (157, 243, 55, 67, eNoUpdate);
	fColorDisplay.SetBorder (1, 1, eNoUpdate);
	fColorDisplay.SetMargin (1, 1, eNoUpdate);
	AddSubView (&fColorDisplay);

	fRed.SetExtent (41, 271, 26, 62, eNoUpdate);
	fRed.SetBorder (2, 2, eNoUpdate);
	fRed.SetMargin (2, 2, eNoUpdate);
	fRed.SetController (this);
	fRed.SetMaxValue (65536);
	fRed.SetMinValue (0);
	AddSubView (&fRed);

	fBlue.SetExtent (113, 271, 26, 62, eNoUpdate);
	fBlue.SetBorder (2, 2, eNoUpdate);
	fBlue.SetMargin (2, 2, eNoUpdate);
	fBlue.SetController (this);
	fBlue.SetMaxValue (65536);
	fBlue.SetMinValue (0);
	AddSubView (&fBlue);

	fGreen.SetExtent (77, 271, 26, 62, eNoUpdate);
	fGreen.SetBorder (2, 2, eNoUpdate);
	fGreen.SetMargin (2, 2, eNoUpdate);
	fGreen.SetController (this);
	fGreen.SetMaxValue (65536);
	fGreen.SetMinValue (0);
	AddSubView (&fGreen);

	fBlueLabel.SetExtent (112, 223, 25, 46, eNoUpdate);
	Font	font0 = Font ("new century schoolbook", 18);
	fBlueLabel.SetFont (&font0);
	fBlueLabel.SetText ("Blue");
	AddSubView (&fBlueLabel);

	fRedLabel.SetExtent (42, 227, 25, 42, eNoUpdate);
	Font	font1 = Font ("new century schoolbook", 18);
	fRedLabel.SetFont (&font1);
	fRedLabel.SetText ("Red");
	AddSubView (&fRedLabel);

	fGreenLabel.SetExtent (77, 209, 25, 60, eNoUpdate);
	Font	font2 = Font ("new century schoolbook", 18);
	fGreenLabel.SetFont (&font2);
	fGreenLabel.SetText ("Green");
	AddSubView (&fGreenLabel);

	AddFocus (&fRed);
	AddFocus (&fGreen);
	AddFocus (&fBlue);
}

#elif qMotifUI

void	ColorPickerX::BuildForMotifUI ()
{
	SetSize (Point (224, 340), eNoUpdate);

	fNamedColors.SetExtent (12, 10, 209, 189, eNoUpdate);
	fNamedColors.SetController (this);
	AddSubView (&fNamedColors);

	fColorDisplay.SetExtent (157, 243, 55, 67, eNoUpdate);
	fColorDisplay.SetBorder (1, 1, eNoUpdate);
	fColorDisplay.SetMargin (1, 1, eNoUpdate);
	AddSubView (&fColorDisplay);

	fRed.SetExtent (41, 271, 26, 62, eNoUpdate);
	fRed.SetController (this);
	fRed.SetMaxValue (65536);
	fRed.SetMinValue (0);
	AddSubView (&fRed);

	fBlue.SetExtent (113, 271, 26, 62, eNoUpdate);
	fBlue.SetController (this);
	fBlue.SetMaxValue (65536);
	fBlue.SetMinValue (0);
	AddSubView (&fBlue);

	fGreen.SetExtent (77, 271, 26, 62, eNoUpdate);
	fGreen.SetController (this);
	fGreen.SetMaxValue (65536);
	fGreen.SetMinValue (0);
	AddSubView (&fGreen);

	fBlueLabel.SetExtent (112, 223, 25, 46, eNoUpdate);
	Font	font3 = Font ("new century schoolbook", 18);
	fBlueLabel.SetFont (&font3);
	fBlueLabel.SetText ("Blue");
	AddSubView (&fBlueLabel);

	fRedLabel.SetExtent (42, 227, 25, 42, eNoUpdate);
	Font	font4 = Font ("new century schoolbook", 18);
	fRedLabel.SetFont (&font4);
	fRedLabel.SetText ("Red");
	AddSubView (&fRedLabel);

	fGreenLabel.SetExtent (77, 209, 25, 60, eNoUpdate);
	Font	font5 = Font ("new century schoolbook", 18);
	fGreenLabel.SetFont (&font5);
	fGreenLabel.SetText ("Green");
	AddSubView (&fGreenLabel);

	AddFocus (&fNamedColors);
	AddFocus (&fRed);
	AddFocus (&fGreen);
	AddFocus (&fBlue);
}

#else

void	ColorPickerX::BuildForUnknownGUI ();
{
	SetSize (Point (224, 340), eNoUpdate);

	fNamedColors.SetExtent (12, 10, 209, 189, eNoUpdate);
	fNamedColors.SetController (this);
	AddSubView (&fNamedColors);

	fColorDisplay.SetExtent (157, 243, 55, 67, eNoUpdate);
	fColorDisplay.SetBorder (1, 1, eNoUpdate);
	fColorDisplay.SetMargin (1, 1, eNoUpdate);
	AddSubView (&fColorDisplay);

	fRed.SetExtent (41, 271, 26, 62, eNoUpdate);
	fRed.SetController (this);
	fRed.SetMaxValue (65536);
	fRed.SetMinValue (0);
	AddSubView (&fRed);

	fBlue.SetExtent (113, 271, 26, 62, eNoUpdate);
	fBlue.SetController (this);
	fBlue.SetMaxValue (65536);
	fBlue.SetMinValue (0);
	AddSubView (&fBlue);

	fGreen.SetExtent (77, 271, 26, 62, eNoUpdate);
	fGreen.SetController (this);
	fGreen.SetMaxValue (65536);
	fGreen.SetMinValue (0);
	AddSubView (&fGreen);

	fBlueLabel.SetExtent (112, 223, 25, 46, eNoUpdate);
	Font	font6 = Font ("new century schoolbook", 18);
	fBlueLabel.SetFont (&font6);
	fBlueLabel.SetText ("Blue");
	AddSubView (&fBlueLabel);

	fRedLabel.SetExtent (42, 227, 25, 42, eNoUpdate);
	Font	font7 = Font ("new century schoolbook", 18);
	fRedLabel.SetFont (&font7);
	fRedLabel.SetText ("Red");
	AddSubView (&fRedLabel);

	fGreenLabel.SetExtent (77, 209, 25, 60, eNoUpdate);
	Font	font8 = Font ("new century schoolbook", 18);
	fGreenLabel.SetFont (&font8);
	fGreenLabel.SetText ("Green");
	AddSubView (&fGreenLabel);

	AddFocus (&fNamedColors);
	AddFocus (&fRed);
	AddFocus (&fGreen);
	AddFocus (&fBlue);
}

#endif /* GUI */

Point	ColorPickerX::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
#if   qMacUI
	return (Point (224, 340));
#elif   qMotifUI
	return (Point (224, 340));
#else
	return (Point (224, 340));
#endif /* GUI */
}

void	ColorPickerX::Layout ()
{
	const Point kSizeDelta = CalcDefaultSize () - GetSize ();
		static const Point	kOriginalfNamedColorsSize = fNamedColors.GetSize ();
		fNamedColors.SetSize (kOriginalfNamedColorsSize - Point (kSizeDelta.GetV (), kSizeDelta.GetH ()));
		static const Point	kOriginalfRedOrigin = fRed.GetOrigin ();
		fRed.SetOrigin (kOriginalfRedOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfBlueOrigin = fBlue.GetOrigin ();
		fBlue.SetOrigin (kOriginalfBlueOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfGreenOrigin = fGreen.GetOrigin ();
		fGreen.SetOrigin (kOriginalfGreenOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfBlueLabelOrigin = fBlueLabel.GetOrigin ();
		fBlueLabel.SetOrigin (kOriginalfBlueLabelOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfRedLabelOrigin = fRedLabel.GetOrigin ();
		fRedLabel.SetOrigin (kOriginalfRedLabelOrigin - Point (0, kSizeDelta.GetH ()));
		static const Point	kOriginalfGreenLabelOrigin = fGreenLabel.GetOrigin ();
		fGreenLabel.SetOrigin (kOriginalfGreenLabelOrigin - Point (0, kSizeDelta.GetH ()));
	View::Layout ();
}


// text past here will be retained: Do not remove or modify this line!!!

#include "Pallet.hh"
#include "NamedColors.hh"



ColorPicker::ColorPicker (const Color& c) :
	fStartColor (c)
{
	StringPickListItem* foo = Nil;
#if		qCFront_ConversionOpOnFunctionResultBroken
	ForEach (MapElement(String, Color), it, GetNamedColors ().operator Iterator(MapElement(String, Color))* ()) {
#else
	ForEach (MapElement(String, Color), it, GetNamedColors ()) {
#endif
		foo = fNamedColors.AddItem (it.Current ().fKey, ePrepend);
	}
	SetColor (c, eNoUpdate);
}
		
Color	ColorPicker::GetStartColor () const
{
	return (fStartColor);
}

Color	ColorPicker::GetColor () const
{
	Color c;
// hack to avoid crashes on HP - LGP may 20, 1992 - debug later...
	if (not IsBadNumber (fRed.GetValue ())) {
		c.SetRed ((ColorComponent)fRed.GetValue ());
	}
	if (not IsBadNumber (fGreen.GetValue ())) {
		c.SetGreen ((ColorComponent)fGreen.GetValue ());
	}
	if (not IsBadNumber (fBlue.GetValue ())) {
		c.SetBlue ((ColorComponent)fBlue.GetValue ());
	}
	return (c);
}

void	ColorPicker::SetColor (const Color& c, UpdateMode update)
{
	fRed.SetValue (c.GetRed (), update);
	fGreen.SetValue (c.GetGreen (), update);
	fBlue.SetValue (c.GetBlue (), update);
	
	Tile t = PalletManager::Get ().MakeTileFromColor (c);
	fColorDisplay.SetBackground (&t, eNoUpdate);
	fColorDisplay.Refresh (update);
}

void	ColorPicker::ButtonPressed (AbstractButton* b)
{
	if (b == &fNamedColors) {
		String s = fNamedColors.GetSelectedString ();
		if (s != kEmptyString) {
			SetColor (StringToColor (s));
			Update ();
		}
	}
	else {
		ColorPickerX::ButtonPressed (b);
	}
}

void 	ColorPicker::SetCurrentFocus_ (FocusItem* newFocus, UpdateMode update, Boolean validate)
{
	ColorPickerX::SetCurrentFocus_ (newFocus, update, validate);
	SetColor (GetColor());
	Update ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

