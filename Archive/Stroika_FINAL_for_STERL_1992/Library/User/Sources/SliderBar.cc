/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SliderBar.cc,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SliderBar.cc,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/04  14:34:01  lewis
 *		Shapes now pass by value using new letter / env stuff. As side effect of that
 *		I have (at least temporarily gotten rid of Setter methods for fields of shapes
 *		So, I changed updates of line with SetFrom/SetTo to line = Line (from,to).
 *		I think thats better anyhow, but I could add back the setter methods?
 *
 *		Revision 1.3  1992/07/03  04:10:02  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.9  1992/03/05  21:53:59  sterling
 *		support for borders
 *
 *		Revision 1.8  1992/02/13  23:19:36  lewis
 *		Work around hp iostream lib bug (cruft at end of strstream) by not using it. Better
 *		to use our new dtostring rouines anyway (from Format.hh).
 *
 *		Revision 1.7  1992/02/03  21:53:33  sterling
 *		massive (but trivial) interface changes inherited from Slider
 *
 *		Revision 1.5  1992/01/08  07:17:12  lewis
 *		Sterl- added conditional compilation flag to decide if we do the picture optiimization, or not.
 *
 */

#include	<string.h>

#include	"Debug.hh"
#include	"Format.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"SliderBar.hh"



const	Coordinate	kHInset = 10;



/*
 ********************************************************************************
 ************************************ SliderBar *********************************
 ********************************************************************************
 */
SliderBar::SliderBar (SliderController* sliderController):
	Slider (sliderController),
	fMin (0),
	fMax (100),
	fValue (0),
	fSubTickSize (2),
	fTickSize (10),
#if		qUsePictureOptimization
	fPicture (Nil),
#endif	
	fFillTile (kBlackTile)
{
	SetFont (&kApplicationFont);
	SetBackground (&kWhiteTile);
}

SliderBar::~SliderBar ()
{
#if		qUsePictureOptimization
	delete fPicture;
#endif
}

Point	SliderBar::CalcDefaultSize_ (const Point& defaultSize) const
{
	Coordinate	h = (defaultSize.GetH () == 0) ? 200 : defaultSize.GetH ();
	Point p = Point (20 + GetEffectiveFont ().GetLineHeight (), h);
	return (p + GetBorder ()*2 + GetMargin ()*2);
}

void	SliderBar::DrawSliderBar (Rect barExtent)
{
	DrawBorder ();
	OutLine (GetShape (), barExtent);

	const	Coordinate	whereV = 20;
	const	Coordinate	whereV1 = whereV - 1;
	const	Coordinate	shortV = whereV / 10;
	const	Coordinate	longV = shortV*2;
	const	Coordinate	textV = GetSize ().GetV () - GetEffectiveFont ().GetFontHeight ();
	Line	line = Line (kZeroPoint, kZeroPoint);

	String	text	=	dtostring (GetMin ());
	DrawText (text, Point (textV, kHInset - TextWidth (text)/2));
	for (Real pageValue = GetMin (); pageValue < GetMax (); pageValue += GetTickSize ()) {
		Real	nextPage = pageValue + GetTickSize ();
		for (Real stepValue = pageValue; stepValue < nextPage; stepValue += GetSubTickSize ()) {
			Coordinate	whereH = kHInset + barExtent.GetWidth () * stepValue / GetMax ();
			line = Line (Point (whereV1, whereH), Point (whereV1 - shortV, whereH));
			OutLine (line);
		}
		Coordinate	whereH = kHInset + barExtent.GetWidth () * nextPage / GetMax ();
		if (nextPage < GetMax ()) {
			line = Line (Point (whereV1, whereH), Point (whereV1 - longV, whereH));
			OutLine (line);
		}
		
		text = dtostring (nextPage);
		DrawText (text, Point (textV, whereH - TextWidth (text)/2));
	}
}

void	SliderBar::Draw (const Region& /*update*/)
{
	const	Point	startP = Point (0, kHInset) + GetBorder () + GetMargin ();
	const	Coordinate	sizeH = GetSize ().GetH () - startP.GetH ()*2;
	
#if		qUsePictureOptimization
	if ((fPicture != Nil) and (fPicture->GetFrame ().GetSize () != GetSize ())) {
		delete fPicture;
		fPicture = Nil;
	}
	if (fPicture == Nil) {
		Tablet*	oldTablet = GetTablet ();
		PictureTablet*	pc	=	new PictureTablet (GetSize ());
		SetTablet (pc);

		DrawSliderBar (Rect (startP, Point (20, sizeH)));

		fPicture = new Picture (pc->GetResult ());
		SetTablet (oldTablet);
		delete pc;
	}
	AssertNotNil (fPicture);
	DrawPicture (*fPicture);
#else
	DrawSliderBar (Rect (startP, Point (20, sizeH)));
#endif

	Coordinate	whereH = sizeH * GetValue () / GetMax ();
	Fill (GetShape (), Rect (startP + Point (0, 1), Point (20, whereH)), GetFillTile ());
}

Real	SliderBar::GetMin_ () const
{
	return (fMin);
}

Real	SliderBar::GetMax_ () const
{
	return (fMax);
}

Real	SliderBar::GetValue_ () const
{
	return (fValue);
}

void	SliderBar::SetValue_ (Real newValue, Panel::UpdateMode updateMode)
{
	Require (newValue >= GetMin ());
	Require (newValue <= GetMax ());

#if		qSunCFront_OverloadingConfusionBug
	Real	smallValue	=	Min ((BigReal)fValue, (BigReal)newValue);
	Real	bigValue	=	Max ((BigReal)fValue, (BigReal)newValue);
#else
	Real	smallValue	=	Min (fValue, newValue);
	Real	bigValue	=	Max (fValue, newValue);
#endif

	fValue = newValue;

	const	Point	startP = Point (0, kHInset) + GetBorder () + GetMargin ();
	const	Coordinate	sizeH = GetSize ().GetH () - startP.GetH ()*2;

	Coordinate	leftH = startP.GetH () + sizeH * smallValue / GetMax ();
	Coordinate	rightH = startP.GetH () + sizeH * bigValue / GetMax ();

const	Coordinate	kRoundoffSluf	=	0;		// add a pixel to each side cuz of roundoff - only update region so OK

	Rect	r = Rect (Point (startP.GetV () + 1, leftH-kRoundoffSluf), Point (18, rightH-leftH+2*kRoundoffSluf));
	Refresh (r, updateMode);
}

void	SliderBar::SetBounds_ (Real newMin, Real newMax, Real newValue, Panel::UpdateMode updateMode)
{
	fMin = newMin;
	fMax = newMax;
	SetValue_ (newValue, eNoUpdate);
#if		qUsePictureOptimization
	InvalPicture ();
#endif	
	Refresh (updateMode);
}

Real	SliderBar::GetTickSize () const
{
	return (fTickSize);
}

void	SliderBar::SetTickSize (Real tickSize, Panel::UpdateMode update)
{
	if (tickSize != fTickSize) {
		fTickSize = tickSize;
#if		qUsePictureOptimization
		InvalPicture ();
#endif	
		Refresh (update);
	}
	Ensure (tickSize == fTickSize);
}
		
Real	SliderBar::GetSubTickSize () const
{
	return (fSubTickSize);
}

void	SliderBar::SetSubTickSize (Real tickSize, Panel::UpdateMode update)
{
	if (tickSize != fSubTickSize) {
		fSubTickSize = tickSize;
#if		qUsePictureOptimization
		InvalPicture ();
#endif	
		Refresh (update);
	}
	Ensure (tickSize == fSubTickSize);
}



#if		qUsePictureOptimization
void	SliderBar::InvalPicture ()
{
	if (fPicture != Nil) {
		delete fPicture;
		fPicture = Nil;
	}
}
#endif

Tile	SliderBar::GetFillTile () const
{
	return (fFillTile);
}

void	SliderBar::SetFillTile (const Tile& newFillTile, Panel::UpdateMode updateMode)
{
	fFillTile = newFillTile;
	Refresh (updateMode);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

