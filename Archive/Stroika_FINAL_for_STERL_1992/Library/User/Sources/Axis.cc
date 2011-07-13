/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
/*
 * $Header: /fuji/lewis/RCS/Axis.cc,v 1.7 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Axis.cc,v $
 *		Revision 1.7  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/16  16:02:04  sterling
 *		hack to work around MPW Lib bug, removed reference to kSystemFont
 *
 *		Revision 1.4  1992/07/04  14:27:25  lewis
 *		Shapes now pass by value using new letter / env stuff. As side effect of that
 *		I have (at least temporarily gotten rid of Setter methods for fields of shapes
 *		So, I changed updates of line with SetFrom/SetTo to line = Line (from,to).
 *		I think thats better anyhow, but I could add back the setter methods?
 *
 *		Revision 1.3  1992/07/03  02:25:42  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  09:23:22  sterling
 *		Lots of changes including support for new definition of DrawText.
 *
 *		Revision 1.1  1992/06/20  17:35:17  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/05/21  17:32:38  lewis
 *		React to change in definition of String::SubString().
 *
 *		Revision 1.1  92/04/15  13:24:51  13:24:51  sterling (Sterling Wight)
 *		Initial revision
 *		
 *		Revision 1.5  1992/01/24  23:43:39  lewis
 *		Use dtostring rather than numbertext::NumbreToString.
 *
 *
 */

#include	"Debug.hh"
#include	"Format.hh"

#include	"Shape.hh"

#include	"Axis.hh"
#include	"NumberText.hh"






/*
 ********************************************************************************
 ********************************** GraphTitle **********************************
 ********************************************************************************
 */
GraphTitle::GraphTitle (const String& title):
	TextView (title)
{
// terrible quickee workaround for corrupt object file SSW 7/10
#if 0
	SetFont (&kSystemFont);
#else
	Font f = Font ("Chicago", 12);
	SetFont (&f);
#endif
}

String	GraphTitle::GetTitle () const
{
	return (GetText ());
}

void	GraphTitle::SetTitle (const String& title, Panel::UpdateMode update)
{
	SetText (title, update);
}







/*
 ********************************************************************************
 ************************************* Axis *************************************
 ********************************************************************************
 */

const	Int16	Axis::kTickLength = 4;
const	Int16	Axis::kSubTickLength = 2;

Axis::Axis (const String& label, AxisOrientation orientation, const Real& minValue, const Real& maxValue, Int16 ticks, Int16 subTicks):
	fLabel (label),
	fOrientation (orientation),
	fMin (minValue),
	fMax (maxValue),
	fTicks (ticks),
	fSubTicks (subTicks),
	fTickRect (kZeroRect),
	fTickLabelRect (kZeroRect),
	fLabelRect (kZeroRect)
{
}

String	Axis::GetLabel () const
{
	return (fLabel);
}

Real	Axis::GetMin () const
{
	return (fMin);
}

Real	Axis::GetMax () const
{
	return (fMax);
}

UInt8	Axis::GetTicks () const
{
	return (fTicks);
}

UInt8	Axis::GetSubTicks () const
{
	return (fSubTicks);
}

void	Axis::Draw (const Region& update)
{
	DrawTicks ();
	DrawTickLabels ();
	DrawLabel ();

	if (fOrientation == eVertical) {
		OutLine (Line (fTickRect.GetTopRight () - Point (0, 1), fTickRect.GetBotRight () - Point (0, 1)));
	}
	else {
		OutLine (Line (fTickRect.GetTopLeft (), fTickRect.GetTopRight ()));
	}
	
	View::Draw (update);
}

void	Axis::SetLabel (const String& label, Panel::UpdateMode updateMode)
{
	if (fLabel != label) {
		Panel::UpdateMode	tempUpdate = (updateMode == eImmediateUpdate) ? eDelayedUpdate : updateMode;
		Refresh (fLabelRect, tempUpdate);
		fLabel = label;
		CalcRects (GetSize ());
		Refresh (fLabelRect, tempUpdate);
		if (updateMode == eImmediateUpdate) {
			Update ();
		}
	}
}

void	Axis::SetMin (const Real& newMin, Panel::UpdateMode updateMode)
{
	if (fMin != newMin) {
		Panel::UpdateMode	tempUpdate = (updateMode == eImmediateUpdate) ? eDelayedUpdate : updateMode;
		Refresh (fTickRect, tempUpdate);
		Refresh (fTickLabelRect, tempUpdate);
		fMin = newMin;
		CalcRects (GetSize ());
		Refresh (fTickRect, tempUpdate);
		Refresh (fTickLabelRect, tempUpdate);
		if (updateMode == eImmediateUpdate) {
			Update ();
		}
	}
}

void	Axis::SetMax (const Real& newMax, Panel::UpdateMode updateMode)
{
	if (fMax != newMax) {
		Panel::UpdateMode	tempUpdate = (updateMode == eImmediateUpdate) ? eDelayedUpdate : updateMode;
		Refresh (fTickRect, tempUpdate);
		Refresh (fTickLabelRect, tempUpdate);
		fMax = newMax;
		CalcRects (GetSize ());
		Refresh (fTickRect, tempUpdate);
		Refresh (fTickLabelRect, tempUpdate);
		if (updateMode == eImmediateUpdate) {
			Update ();
		}
	}
}

void	Axis::SetTicks (UInt8 ticks, Panel::UpdateMode updateMode)
{
	Require (ticks >= 0);
	if (fTicks != ticks) {
		Panel::UpdateMode	tempUpdate = (updateMode == eImmediateUpdate) ? eDelayedUpdate : updateMode;
		Refresh (fTickRect, tempUpdate);
		Refresh (fTickLabelRect, tempUpdate);
		fTicks = ticks;
		CalcRects (GetSize ());
		Refresh (fTickRect, tempUpdate);
		Refresh (fTickLabelRect, tempUpdate);
		if (updateMode == eImmediateUpdate) {
			Update ();
		}
	}
}

String	Axis::ValueToString (const Real& value) const
{
	return (dtostring (value));
}

void	Axis::SetSubTicks (UInt8 subTicks, Panel::UpdateMode updateMode)
{
	Require (subTicks >= 0);
	if (fSubTicks != subTicks) {
		fSubTicks = subTicks;
		Refresh (fTickRect, updateMode);
	}
}

Coordinate	Axis::GetCoordinateOfMin ()
{
	return ((fOrientation == eVertical) ? fTickRect.GetBottom () : fTickRect.GetLeft ());
}

Coordinate	Axis::GetCoordinateOfMax ()
{
	return ((fOrientation == eVertical) ? fTickRect.GetTop () : fTickRect.GetRight ());
}

Real	Axis::CoordinateToValue (Coordinate coordinate)
{
	return (GetMin () + (coordinate - GetCoordinateOfMin ()) *
		((GetMax () - GetMin ()) / (GetCoordinateOfMax () - GetCoordinateOfMin ())));
}

Coordinate	Axis::ValueToCoordinate (const Real& value)
{
	return (GetCoordinateOfMin () + (value - GetMin ()) /
		((GetMax () - GetMin ()) / (GetCoordinateOfMax () - GetCoordinateOfMin ())));
}

void	Axis::CalcRects (const Point& size)
{
	if (GetTablet () != Nil) {
		const Font&	f = GetEffectiveFont ();
	
		if (fOrientation == eHorizontal) {
			fTickRect = Rect (kZeroPoint, Point (Axis::kTickLength + 1, size.GetH ()));
			fTickLabelRect = Rect (
				Point (fTickRect.GetHeight ()+1, 0), 
				Point (f.GetFontHeight (), fTickRect.GetWidth ()));
				
			String	minString = ValueToString (GetMin ());	
			String	maxString = ValueToString (GetMax ());
			Coordinate	padding = TextWidth (minString, f, kZeroPoint) + TextWidth (maxString, f, kZeroPoint);
			padding /= 2;
			fTickRect.SetOrigin (fTickRect.GetOrigin () + Point (0, TextWidth (minString, f, kZeroPoint)/2));
			fTickRect.SetSize (fTickRect.GetSize () - Point (0, padding));
				
			fLabelRect = fTickLabelRect;
			fLabelRect.SetSize (Point (
				f.GetFontHeight (),
				TextWidth (fLabel, f, kZeroPoint)));		
			fLabelRect.SetOrigin (Point (
				fTickLabelRect.GetBottom () + 1,
				(fTickLabelRect.GetWidth () - fLabelRect.GetWidth ()) / 2));
		}
		else {
			CollectionSize		totalChars	= fLabel.GetLength ();
			fLabelRect.SetSize (Point (
				(f.GetFontHeight ()) * totalChars,
				TextWidth ("W", f, kZeroPoint)));		
			fLabelRect.SetOrigin (Point (
				(size.GetV () - fLabelRect.GetHeight ()) / 2,
				0));

			Require (GetTicks () > 0);
			Coordinate	width = 0;
			Real	increment = (GetMax () - GetMin ()) / GetTicks ();
			Real	value = GetMin ();
			for (int tickCount = 0; tickCount <= GetTicks (); tickCount++) {
				width = Max (width, TextWidth (ValueToString (value), f, kZeroPoint));
				value += increment;
			}
			fTickLabelRect = Rect (
				Point (0, fLabelRect.GetRight () + 1), 
				Point (size.GetV (), width));
			Coordinate	charHeight = (f.GetFontHeight ());
				
			fTickRect = Rect (
				Point (charHeight/2, fTickLabelRect.GetRight () + 1), 
				Point (size.GetV () - charHeight, Axis::kTickLength + 1));
		}
	}
}

void	Axis::Layout ()
{
	View::Layout ();
	CalcRects (GetSize ());
}

Point	Axis::CalcDefaultSize_ (const Point& defaultSize) const
{
	CalcRects (defaultSize);
	if (fOrientation == eHorizontal) {
		return (Point (fLabelRect.GetBottom (), fTickLabelRect.GetRight ()));
	}
	return (Point (fTickLabelRect.GetBottom (), fTickRect.GetRight ()));
}

void	Axis::DrawTicks ()
{
	Real	increment = ((GetTicks () == 0) ? 0 : ((GetMax () - GetMin ()) / GetTicks ()));
	Real	subIncrement = increment / (GetSubTicks () + 1);
	Real	value = GetMin ();
	Line	tickLine = Line (kZeroPoint, kZeroPoint);
	Coordinate	lineStart = (fOrientation == eVertical) 
		? fTickRect.GetLeft ()
		: fTickRect.GetTop ();
	Coordinate	subTickLineStart = (fOrientation == eVertical) 
		? fTickRect.GetLeft () - Axis::kSubTickLength
		: fTickRect.GetTop ();
	for (int tickCount = 0; tickCount <= GetTicks (); tickCount++) {
		Coordinate	where = ValueToCoordinate (value);
		if (fOrientation == eVertical) {
			tickLine = Line (Point (where, lineStart), Point (where, lineStart + Axis::kTickLength));
		}
		else {
			tickLine = Line (Point (lineStart, where), Point (lineStart + Axis::kTickLength, where));
		}
		OutLine (tickLine);
		
		if (tickCount < GetTicks ()) {
			for (int subTickCount = 0; subTickCount <= GetSubTicks (); subTickCount++) {
				if (subTickCount > 0) {
					where = ValueToCoordinate (value);
					if (fOrientation == eVertical) {
						tickLine = Line (Point (where, subTickLineStart), Point (where, subTickLineStart + Axis::kSubTickLength));
					}
					else {
						tickLine = Line (Point (subTickLineStart, where), Point (subTickLineStart + Axis::kSubTickLength, where));
					}
					OutLine (tickLine);
				}
				value += subIncrement;
			}
		}
	}
}

void	Axis::DrawTickLabels ()
{
	Real	increment = ((GetTicks () == 0) ? 0 : ((GetMax () - GetMin ()) / GetTicks ()));
	Real	value = GetMin ();
	const Font&	f = GetEffectiveFont ();
	Coordinate	offset = -(f.GetFontHeight ()) /2;

	for (int tickCount = 0; tickCount <= GetTicks (); tickCount++) {
		Coordinate	where = ValueToCoordinate (value);
		String	label = ValueToString (value);
		
		if (fOrientation == eVertical) {
			DrawText (label, Point (
				where + offset,
				fTickLabelRect.GetLeft () + 
					(fTickLabelRect.GetWidth () -TextWidth (label, f, kZeroPoint)) / 2));
		}
		else {
			DrawText (label, Point (
				fTickLabelRect.GetTop (), 
				where - TextWidth (label, f, kZeroPoint) / 2));
		}
		value += increment;
	}
}

void	Axis::DrawLabel ()
{
	const Font&	f = GetEffectiveFont ();
	Coordinate	offset = -(f.GetFontHeight ()) /2;
	if (fOrientation == eVertical) {
		Coordinate		where		= fLabelRect.GetTop ();
		CollectionSize	totalChars	= fLabel.GetLength ();
		for (CollectionSize count = 1; count <= totalChars; count++) {
			String	charString = fLabel.SubString (count, 1);
			DrawText (charString, Point (
				where + offset,
				fLabelRect.GetLeft () + 
					(fLabelRect.GetWidth () - TextWidth (charString, f, kZeroPoint)) / 2));
			where += f.GetFontHeight ()/2;
		}
	}
	else {
		DrawText (fLabel, fLabelRect.GetOrigin ());
	}
}

Rect	Axis::GetTickRect () const
{
	return (fTickRect);
}

Rect	Axis::GetTickLabelRect () const
{
	return (fTickLabelRect);
}

Rect	Axis::GetLabelRect () const
{
	return (fLabelRect);
}

Axis::AxisOrientation	Axis::GetOrientation () const
{
	return (fOrientation);
}






/*
 ********************************************************************************
 ********************************* LabelledAxis *********************************
 ********************************************************************************
 */
LabelledAxis::LabelledAxis (const String& label, Axis::AxisOrientation orientation, 
			  const Real& minValue, const Real& maxValue,
			  Int16 ticks, Int16 subTicks) :
	Axis (label, orientation, minValue, maxValue, ticks, subTicks),
	fLabels ()
{
	for (CollectionSize i = 1; i <= GetTicks (); i++) {
		fLabels.Append (kEmptyString);
	}
}
		
void	LabelledAxis::DrawTickLabels ()
{
	Real	increment = ((GetTicks () == 0) ? 0 : ((GetMax () - GetMin ()) / GetTicks ()));
	Real	value = GetMin () + increment/2;
	const Font&	f = GetEffectiveFont ();
	Coordinate	offset = - (f.GetFontHeight ()) /2;

	for (int tickCount = 1; tickCount <= GetTicks (); tickCount++) {
		Coordinate	where = ValueToCoordinate (value);
		
		String	label = GetTickLabel (tickCount);
		if (GetOrientation () == eVertical) {
			DrawText (label, Point (
				where + offset,
				GetTickLabelRect ().GetLeft () + 
					(GetTickLabelRect ().GetWidth () - TextWidth (label, f, kZeroPoint)) / 2));
		}
		else {
			DrawText (label, Point (
				GetTickLabelRect ().GetTop (), 
				where - TextWidth (label, f, kZeroPoint) / 2));
		}
		value += increment;
	}
}

void	LabelledAxis::SetTicks (UInt8 ticks, Panel::UpdateMode updateMode)
{
	CollectionSize oldLength = fLabels.GetLength ();
	for (int i = oldLength; i > ticks; i--) {
		fLabels.RemoveAt (i);
	}
	oldLength = fLabels.GetLength ();
	Assert (oldLength <= ticks);
	for (i = oldLength; i < ticks; i++) {
		fLabels.Append (kEmptyString);
	}
	Assert (fLabels.GetLength () == ticks);

	Axis::SetTicks (ticks, updateMode);
}
		
void	LabelledAxis::SetTickLabel (UInt8 tick, const String& label, Panel::UpdateMode updateMode)
{
	Require (fLabels.GetLength () == GetTicks ());
	Require (tick <= GetTicks ());
	Require (tick >= 1);
	
	fLabels.SetAt (label, tick);
	Refresh (updateMode);
	
	Ensure (GetTickLabel (tick) == label);
}

String	LabelledAxis::GetTickLabel (UInt8 tick)
{
	Require (fLabels.GetLength () == GetTicks ());
	Require (tick <= GetTicks ());
	Require (tick >= 1);
	
	return (fLabels[tick]);
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

