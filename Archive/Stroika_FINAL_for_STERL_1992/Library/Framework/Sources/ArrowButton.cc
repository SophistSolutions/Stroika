/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ArrowButton.cc,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: ArrowButton.cc,v $
// Revision 1.8  1992/09/08  15:34:00  lewis
// Renamed NULL -> Nil.
//
// Revision 1.7  1992/09/01  15:46:50  sterling
// Lots of Foundation changes.
//
// Revision 1.6  1992/07/21  20:12:43  sterling
// changed qGUI to qUI, supported qWinUI
//
 *	Revision 1.5  1992/07/16  16:31:13  sterling
 *	changed GUI to UI
 *	
 * Revision 1.3  1992/07/04  14:15:37  lewis
 * Shapes now pass by value using new letter / env stuff. As side effect of that
 * I have (at least temporarily gotten rid of Setter methods for fields of shpes.
 * So, I changed updates of lien with SetFrom/SetTo to line = Line (from,to).
 * I think thats better anyhow, but I could add back the setter methods?
 *
 * Revision 1.2  1992/07/03  00:17:11  lewis
 * General cleanups, and Use Panel:: to scope access to nested UpdateMode enum.
 *
 * Revision 1.1  1992/06/25  07:36:23  sterling
 * Initial revision
 *
 *
 *
 *
 */

#include	"Pallet.hh"

#include	"ArrowButton.hh"



/*
 ********************************************************************************
 ****************************** AbstractArrowButton *****************************
 ********************************************************************************
 */

AbstractArrowButton::AbstractArrowButton (AbstractArrowButton::ArrowDirection direction,
											ButtonController* controller) :
	Button (controller),
	fDirection (direction)
{
}

AbstractArrowButton::ArrowDirection	AbstractArrowButton::GetDirection () const
{
	return (fDirection);
}

void	AbstractArrowButton::SetDirection (AbstractArrowButton::ArrowDirection direction, Panel::UpdateMode updateMode)
{
	if (GetDirection () != direction) {
		SetDirection_ (direction, updateMode);
	}
	Ensure (GetDirection () == direction);
}

void	AbstractArrowButton::SetDirection_ (AbstractArrowButton::ArrowDirection direction, Panel::UpdateMode update)
{
	fDirection = direction;
	Refresh (update);
}





/*
 ********************************************************************************
 ************************** AbstractArrowButton_MacUI **************************
 ********************************************************************************
 */
AbstractArrowButton_MacUI::AbstractArrowButton_MacUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	AbstractArrowButton (direction, controller)
{
	SetBorder (1, 1, eNoUpdate);
}

Point	AbstractArrowButton_MacUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	return (Point (14, 14) + GetBorder ()*2 + GetMargin ()*2);
}





/*
 ********************************************************************************
 ************************* ArrowButton_MacUI_Portable **************************
 ********************************************************************************
 */
ArrowButton_MacUI_Portable::ArrowButton_MacUI_Portable (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	AbstractArrowButton_MacUI (direction, controller),
	fArrowShape (Rectangle ())
{
	SetDirection_ (GetDirection (), eNoUpdate);
}
		
Boolean	ArrowButton_MacUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if ((mouseInButton) and (phase != Tracker::eTrackRelease)) {
		Fill (fArrowShape, GetArrowBounds (), kBlackTile);
	}
	else {
		Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
		Tile t = PalletManager::Get ().MakeTileFromColor (baseColor);
		Fill (fArrowShape, GetArrowBounds (), t);
	}
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		if (GetController () != Nil) {
			GetController ()->ButtonPressed (this);
		}
	}
	
	return (AbstractArrowButton_MacUI::Track (phase, mouseInButton));
}

void	ArrowButton_MacUI_Portable::Draw (const Region& /*update*/)
{
	Rect extent = GetLocalExtent ();
	DrawMacBorder (extent, GetBorder ());
	
	Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
	
	Rect r = InsetBy (extent, GetBorder () + Point (1, 1));
	Tile t = PalletManager::Get ().MakeTileFromColor (baseColor * 1.4);
	Fill (Rectangle (), r, t);

	t = PalletManager::Get ().MakeTileFromColor (baseColor * .5);
	r.SetOrigin (extent.GetBotLeft () + Point (-2, 1));
	r.SetSize (Point (1, extent.GetSize ().GetH () - 2));
	Fill (Rectangle (), r, t);
	r.SetOrigin (extent.GetTopRight () + Point (1, -2));
	r.SetSize (Point (extent.GetSize ().GetV ()-2, 1));
	Fill (Rectangle (), r, t);

	t = PalletManager::Get ().MakeTileFromColor (baseColor);
	Fill (fArrowShape, GetArrowBounds (), t);
	
	t = PalletManager::Get ().MakeTileFromColor (baseColor * .4);
	OutLine (fArrowShape, GetArrowBounds (), t);

}

void	ArrowButton_MacUI_Portable::SetDirection_ (AbstractArrowButton::ArrowDirection direction, Panel::UpdateMode updateMode)
{
	static	Sequence_Array(Point)	fLeftArrow;
	static	Sequence_Array(Point)	fUpArrow;
	static	Sequence_Array(Point)	fRightArrow;
	static	Sequence_Array(Point)	fDownArrow;
	
	if (fLeftArrow.GetLength () == 0) {
		fLeftArrow.Append (Point (5, 0));
		fLeftArrow.Append (Point (0, 5));
		fLeftArrow.Append (Point (3, 5));
		fLeftArrow.Append (Point (3, 9));
		fLeftArrow.Append (Point (7, 9));
		fLeftArrow.Append (Point (7, 5));
		fLeftArrow.Append (Point (10, 5));
		fLeftArrow.Append (Point (5, 0));
	}
	if (fUpArrow.GetLength () == 0) {
		fUpArrow.Append (Point (0, 5));
		fUpArrow.Append (Point (5, 10));
		fUpArrow.Append (Point (5, 7));
		fUpArrow.Append (Point (9, 7));
		fUpArrow.Append (Point (9, 3));
		fUpArrow.Append (Point (5, 3));
		fUpArrow.Append (Point (5, 0));
		fUpArrow.Append (Point (0, 5));
	}
	if (fRightArrow.GetLength () == 0) {
		fRightArrow.Append (Point (0, 4));
		fRightArrow.Append (Point (5, 9));
		fRightArrow.Append (Point (10, 4));
		fRightArrow.Append (Point (7, 4));
		fRightArrow.Append (Point (7, 0));
		fRightArrow.Append (Point (3, 0));
		fRightArrow.Append (Point (3, 4));
		fRightArrow.Append (Point (0, 4));
	}
	if (fDownArrow.GetLength () == 0) {
		fDownArrow.Append (Point (0, 3));
		fDownArrow.Append (Point (0, 7));
		fDownArrow.Append (Point (4, 7));
		fDownArrow.Append (Point (4, 10));
		fDownArrow.Append (Point (9, 5));
		fDownArrow.Append (Point (4, 0));
		fDownArrow.Append (Point (4, 3));
		fDownArrow.Append (Point (0, 3));
	}
	
	switch (direction) {
		case eLeft:
			fArrowShape = Polygon (fLeftArrow);
			break;
		
		case eUp:
			fArrowShape = Polygon (fUpArrow);
			break;
		
		case eRight:
			fArrowShape = Polygon (fRightArrow);
			break;
			
		case eDown:
			fArrowShape = Polygon (fDownArrow);
			break;
		
		default:
			RequireNotReached ();
	}
	
	AbstractArrowButton_MacUI::SetDirection_ (direction, updateMode);
}

Rect	ArrowButton_MacUI_Portable::GetArrowBounds () const
{
	Rect r = InsetBy (GetLocalExtent (), GetBorder () + Point (1, 1));
	
	switch (GetDirection ()) {
		case eLeft:
		case eRight:
			r.InsetBy (0, 1);
			r.SetSize (r.GetSize () - Point (2, 1));
			break;
		
		case eUp:
		case eDown:
			r.InsetBy (1, 0);
			r.SetSize (r.GetSize () - Point (1, 2));
			break;
		
		default:
			AssertNotReached ();
	}
	return (r);
}





/*
 ********************************************************************************
 ************************* AbstractArrowButton_MotifUI *************************
 ********************************************************************************
 */
AbstractArrowButton_MotifUI::AbstractArrowButton_MotifUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	AbstractArrowButton (direction, controller)
{
	SetBorder (2, 2, eNoUpdate);
	SetPlane (Bordered::eIn, eNoUpdate);
}

Point	AbstractArrowButton_MotifUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	return (Point (11, 11) + GetBorder ()*2 + GetMargin ()*2);
}





/*
 ********************************************************************************
 ************************* ArrowButton_MotifUI_Portable ************************
 ********************************************************************************
 */
ArrowButton_MotifUI_Portable::ArrowButton_MotifUI_Portable (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	AbstractArrowButton_MotifUI (direction, controller),
	fArrowShape (Rectangle ())
{
	SetDirection_ (GetDirection (), eNoUpdate);
}
		
Boolean	ArrowButton_MotifUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	DrawArrow (Boolean ((mouseInButton) and (phase != Tracker::eTrackRelease)));	
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		if (GetController () != Nil) {
			GetController ()->ButtonPressed (this);
		}
	}

	return (AbstractArrowButton_MotifUI::Track (phase, mouseInButton));
}

void	ArrowButton_MotifUI_Portable::DrawMitredLine (Line& line, Coordinate thickness, const Point& fromOffset, const Point& toOffset, const Pen& pen)
{
	for (int i = 1; i <= thickness; i++) {
		OutLine (line, kZeroRect, pen);
#if 1
		line = Line (line.GetFrom () + fromOffset, line.GetTo () + toOffset);
#else
		line.SetFrom (line.GetFrom () + fromOffset);
		line.SetTo (line.GetTo () + toOffset);
#endif
	}
}

void	ArrowButton_MotifUI_Portable::DrawArrow (Boolean pressed)
{
	Rect r = GetLocalExtent ().InsetBy (GetBorder () + GetMargin ());
	
	Color 	backGroundColor = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	Color	selectColor;
	Color	topShadow;
	Color	bottomShadow;
	
	CalcColorSet (backGroundColor, selectColor, topShadow, bottomShadow);
	
	if (Boolean (GetPlane () == Bordered::eOut) ^ pressed) {
		Color temp = topShadow;
		topShadow = bottomShadow;
		bottomShadow = temp;
		temp = backGroundColor;
		backGroundColor = selectColor;
		selectColor = temp;
	}
	
	Tile t = PalletManager::Get ().MakeTileFromColor (selectColor);
	Fill (Rectangle (), r, t);
	t = PalletManager::Get ().MakeTileFromColor (backGroundColor);
	Fill (fArrowShape, r, t);

	Point border = GetBorder ();
	if (border == kZeroPoint) {
		border = Point (2, 2);	// sometimes want arrow with no outside border (like in a scrollbar)
	}

	Pen		topPen = Pen (PalletManager::Get ().MakeTileFromColor (topShadow));
	Pen		botPen = Pen (PalletManager::Get ().MakeTileFromColor (bottomShadow));
	Point	center = CalcRectCenter (r);
	Line	line;

	switch (GetDirection ()) {
		case eLeft:
#if 1
			line = Line (Point (r.GetTop (), r.GetRight () - 1), r.GetBotRight () - Point (1, 1));
			DrawMitredLine (line, border.GetH (), Point (1, -1), Point (-1, -1), botPen);

			line = Line (Point (center.GetV (), r.GetLeft ()), r.GetBotRight () - Point (1, border.GetH ()));
			DrawMitredLine (line, border.GetH (), Point (0, 1), Point (0, 1), botPen);

			line = Line (Point (center.GetV (), r.GetLeft ()), Point (r.GetTop (), r.GetRight () - border.GetH ()));
			DrawMitredLine (line, border.GetH (), Point (0, 1), Point (0, 1), topPen);
#else
			line.SetFrom (Point (r.GetTop (), r.GetRight () - 1));
			line.SetTo (r.GetBotRight () - Point (1, 1));
			DrawMitredLine (line, border.GetH (), Point (1, -1), Point (-1, -1), botPen);

			line.SetFrom (Point (center.GetV (), r.GetLeft ()));
			line.SetTo (r.GetBotRight () - Point (1, border.GetH ()));
			DrawMitredLine (line, border.GetH (), Point (0, 1), Point (0, 1), botPen);

			line.SetFrom (Point (center.GetV (), r.GetLeft ()));
			line.SetTo (Point (r.GetTop (), r.GetRight () - border.GetH ()));
			DrawMitredLine (line, border.GetH (), Point (0, 1), Point (0, 1), topPen);
#endif
			break;
		
		case eUp:
#if 1
			line = Line (Point (r.GetBottom ()-1, r.GetLeft ()), Point (r.GetBottom ()-1, r.GetRight () -1));
			DrawMitredLine (line, border.GetH (), Point (-1, 1), Point (-1, -1), botPen);

			line = Line (Point (r.GetTop (), center.GetH ()), Point (r.GetBottom ()-border.GetV (), r.GetRight ()-1));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), botPen);

			line = Line (Point (r.GetTop (), center.GetH ()), Point (r.GetBottom ()-border.GetV (), r.GetLeft ()));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), topPen);
#else
			line.SetFrom (Point (r.GetBottom ()-1, r.GetLeft ()));
			line.SetTo (Point (r.GetBottom ()-1, r.GetRight () -1));
			DrawMitredLine (line, border.GetH (), Point (-1, 1), Point (-1, -1), botPen);

			line.SetFrom (Point (r.GetTop (), center.GetH ()));
			line.SetTo (Point (r.GetBottom ()-border.GetV (), r.GetRight ()-1));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), botPen);

			line.SetFrom (Point (r.GetTop (), center.GetH ()));
			line.SetTo (Point (r.GetBottom ()-border.GetV (), r.GetLeft ()));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), topPen);
#endif
			break;
		
		case eRight:
#if 1
			line = Line (r.GetOrigin (), Point (r.GetBottom ()-1, r.GetLeft ()));
			DrawMitredLine (line, border.GetH (), Point (1, 1), Point (-1, 1), topPen);

			line = Line (Point (r.GetBottom ()-1, r.GetLeft () + border.GetH ()), Point (center.GetV (), r.GetRight ()-1));
			DrawMitredLine (line, border.GetH (), Point (0, -1), Point (0, -1), botPen);

			line = Line (Point (r.GetTop (), r.GetLeft () + border.GetH ()), Point (center.GetV (), r.GetRight ()-1));
			DrawMitredLine (line, border.GetH (), Point (0, -1), Point (0, -1), topPen);
#else
			line.SetFrom (r.GetOrigin ());
			line.SetTo (Point (r.GetBottom ()-1, r.GetLeft ()));
			DrawMitredLine (line, border.GetH (), Point (1, 1), Point (-1, 1), topPen);

			line.SetFrom (Point (r.GetBottom ()-1, r.GetLeft () + border.GetH ()));
			line.SetTo (Point (center.GetV (), r.GetRight ()-1));
			DrawMitredLine (line, border.GetH (), Point (0, -1), Point (0, -1), botPen);

			line.SetFrom (Point (r.GetTop (), r.GetLeft () + border.GetH ()));
			line.SetTo (Point (center.GetV (), r.GetRight ()-1));
			DrawMitredLine (line, border.GetH (), Point (0, -1), Point (0, -1), topPen);
#endif
			break;
			
		case eDown:
#if 1
			line = Line (r.GetOrigin (), Point (r.GetTop (), r.GetRight () -1));
			DrawMitredLine (line, border.GetH (), Point (1, 1), Point (1, -1), topPen);

			line = Line (r.GetOrigin (), Point (r.GetBottom () - border.GetV (), center.GetH ()));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), topPen);

			line = Line (Point (r.GetTop (), r.GetRight () -1), Point (r.GetBottom ()-border.GetV (), center.GetH ()));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), botPen);
#else
			line.SetFrom (r.GetOrigin ());
			line.SetTo (Point (r.GetTop (), r.GetRight () -1));
			DrawMitredLine (line, border.GetH (), Point (1, 1), Point (1, -1), topPen);

			line.SetFrom (r.GetOrigin ());
			line.SetTo (Point (r.GetBottom () - border.GetV (), center.GetH ()));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), topPen);

			line.SetFrom (Point (r.GetTop (), r.GetRight () -1));
			line.SetTo (Point (r.GetBottom ()-border.GetV (), center.GetH ()));
			DrawMitredLine (line, border.GetV (), Point (1, 0), Point (1, 0), botPen);
#endif
			break;
		
		default:
			AssertNotReached ();
	}
}

void	ArrowButton_MotifUI_Portable::Draw (const Region& /*update*/)
{
	Color backGroundColor = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (GetLocalExtent (), GetBorder (), backGroundColor, GetPlane ());
	DrawArrow (False);	
}

void	ArrowButton_MotifUI_Portable::SetDirection_ (AbstractArrowButton::ArrowDirection direction, Panel::UpdateMode updateMode)
{
	static	Sequence_Array(Point)	fLeftArrow;
	static	Sequence_Array(Point)	fUpArrow;
	static	Sequence_Array(Point)	fRightArrow;
	static	Sequence_Array(Point)	fDownArrow;
	
	if (fLeftArrow.GetLength () == 0) {
		fLeftArrow.Append (Point (5, 0));
		fLeftArrow.Append (Point (11, 11));
		fLeftArrow.Append (Point (0, 11));
		fLeftArrow.Append (Point (5, 0));
	}
	if (fUpArrow.GetLength () == 0) {
		fUpArrow.Append (Point (0, 5));
		fUpArrow.Append (Point (11, 11));
		fUpArrow.Append (Point (11, 0));
		fUpArrow.Append (Point (0, 5));
	}
	if (fRightArrow.GetLength () == 0) {
		fRightArrow.Append (Point (5, 11));
		fRightArrow.Append (Point (11, 0));
		fRightArrow.Append (Point (0, 0));
		fRightArrow.Append (Point (5, 11));
	}
	if (fDownArrow.GetLength () == 0) {
		fDownArrow.Append (Point (10, 5));
		fDownArrow.Append (Point (0, 0));
		fDownArrow.Append (Point (0, 10));
		fDownArrow.Append (Point (10, 5));
	}
	
	switch (direction) {
		case eLeft:
			fArrowShape = Polygon (fLeftArrow);
			break;
		
		case eUp:
			fArrowShape = Polygon (fUpArrow);
			break;
		
		case eRight:
			fArrowShape = Polygon (fRightArrow);
			break;
			
		case eDown:
			fArrowShape = Polygon (fDownArrow);
			break;
		
		default:
			AssertNotReached ();
	}
	
	AbstractArrowButton_MotifUI::SetDirection_ (direction, updateMode);
}


/*
 ********************************************************************************
 ************************** AbstractArrowButton_WinUI **************************
 ********************************************************************************
 */
AbstractArrowButton_WinUI::AbstractArrowButton_WinUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	AbstractArrowButton (direction, controller)
{
	SetBorder (1, 1, eNoUpdate);
}

Point	AbstractArrowButton_WinUI::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	return (Point (14, 14) + GetBorder ()*2 + GetMargin ()*2);
}





/*
 ********************************************************************************
 ************************* ArrowButton_WinUI_Portable **************************
 ********************************************************************************
 */
ArrowButton_WinUI_Portable::ArrowButton_WinUI_Portable (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	AbstractArrowButton_WinUI (direction, controller),
	fArrowShape (Rectangle ())
{
	SetDirection_ (GetDirection (), eNoUpdate);
}
		
Boolean	ArrowButton_WinUI_Portable::Track (Tracker::TrackPhase phase, Boolean mouseInButton)
{
	if ((mouseInButton) and (phase != Tracker::eTrackRelease)) {
		Fill (fArrowShape, GetArrowBounds (), kBlackTile);
	}
	else {
		Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
		Tile t = PalletManager::Get ().MakeTileFromColor (baseColor);
		Fill (fArrowShape, GetArrowBounds (), t);
	}
	if ((phase == Tracker::eTrackRelease) and (mouseInButton)) {
		if (GetController () != Nil) {
			GetController ()->ButtonPressed (this);
		}
	}
	
	return (AbstractArrowButton_WinUI::Track (phase, mouseInButton));
}

void	ArrowButton_WinUI_Portable::Draw (const Region& /*update*/)
{
	Rect extent = GetLocalExtent ();
	DrawMacBorder (extent, GetBorder ());
	
	Color baseColor = Color (kMaxComponent, kMaxComponent, kMaxComponent) * .65;
	
	Rect r = InsetBy (extent, GetBorder () + Point (1, 1));
	Tile t = PalletManager::Get ().MakeTileFromColor (baseColor * 1.4);
	Fill (Rectangle (), r, t);

	t = PalletManager::Get ().MakeTileFromColor (baseColor * .5);
	r.SetOrigin (extent.GetBotLeft () + Point (-2, 1));
	r.SetSize (Point (1, extent.GetSize ().GetH () - 2));
	Fill (Rectangle (), r, t);
	r.SetOrigin (extent.GetTopRight () + Point (1, -2));
	r.SetSize (Point (extent.GetSize ().GetV ()-2, 1));
	Fill (Rectangle (), r, t);

	t = PalletManager::Get ().MakeTileFromColor (baseColor);
	Fill (fArrowShape, GetArrowBounds (), t);
	
	t = PalletManager::Get ().MakeTileFromColor (baseColor * .4);
	OutLine (fArrowShape, GetArrowBounds (), t);

}

void	ArrowButton_WinUI_Portable::SetDirection_ (AbstractArrowButton::ArrowDirection direction, Panel::UpdateMode updateMode)
{
	static	Sequence_Array(Point)	fLeftArrow;
	static	Sequence_Array(Point)	fUpArrow;
	static	Sequence_Array(Point)	fRightArrow;
	static	Sequence_Array(Point)	fDownArrow;
	
	if (fLeftArrow.GetLength () == 0) {
		fLeftArrow.Append (Point (5, 0));
		fLeftArrow.Append (Point (0, 5));
		fLeftArrow.Append (Point (3, 5));
		fLeftArrow.Append (Point (3, 9));
		fLeftArrow.Append (Point (7, 9));
		fLeftArrow.Append (Point (7, 5));
		fLeftArrow.Append (Point (10, 5));
		fLeftArrow.Append (Point (5, 0));
	}
	if (fUpArrow.GetLength () == 0) {
		fUpArrow.Append (Point (0, 5));
		fUpArrow.Append (Point (5, 10));
		fUpArrow.Append (Point (5, 7));
		fUpArrow.Append (Point (9, 7));
		fUpArrow.Append (Point (9, 3));
		fUpArrow.Append (Point (5, 3));
		fUpArrow.Append (Point (5, 0));
		fUpArrow.Append (Point (0, 5));
	}
	if (fRightArrow.GetLength () == 0) {
		fRightArrow.Append (Point (0, 4));
		fRightArrow.Append (Point (5, 9));
		fRightArrow.Append (Point (10, 4));
		fRightArrow.Append (Point (7, 4));
		fRightArrow.Append (Point (7, 0));
		fRightArrow.Append (Point (3, 0));
		fRightArrow.Append (Point (3, 4));
		fRightArrow.Append (Point (0, 4));
	}
	if (fDownArrow.GetLength () == 0) {
		fDownArrow.Append (Point (0, 3));
		fDownArrow.Append (Point (0, 7));
		fDownArrow.Append (Point (4, 7));
		fDownArrow.Append (Point (4, 10));
		fDownArrow.Append (Point (9, 5));
		fDownArrow.Append (Point (4, 0));
		fDownArrow.Append (Point (4, 3));
		fDownArrow.Append (Point (0, 3));
	}
	
	switch (direction) {
		case eLeft:
			fArrowShape = Polygon (fLeftArrow);
			break;
		
		case eUp:
			fArrowShape = Polygon (fUpArrow);
			break;
		
		case eRight:
			fArrowShape = Polygon (fRightArrow);
			break;
			
		case eDown:
			fArrowShape = Polygon (fDownArrow);
			break;
		
		default:
			RequireNotReached ();
	}
	
	AbstractArrowButton_WinUI::SetDirection_ (direction, updateMode);
}

Rect	ArrowButton_WinUI_Portable::GetArrowBounds () const
{
	Rect r = InsetBy (GetLocalExtent (), GetBorder () + Point (1, 1));
	
	switch (GetDirection ()) {
		case eLeft:
		case eRight:
			r.InsetBy (0, 1);
			r.SetSize (r.GetSize () - Point (2, 1));
			break;
		
		case eUp:
		case eDown:
			r.InsetBy (1, 0);
			r.SetSize (r.GetSize () - Point (1, 2));
			break;
		
		default:
			AssertNotReached ();
	}
	return (r);
}

/*
 ********************************************************************************
 ****************************** ArrowButton_MacUI *******************************
 ********************************************************************************
 */
ArrowButton_MacUI::ArrowButton_MacUI (AbstractArrowButton::ArrowDirection direction):
	ArrowButton_MacUI_Portable (direction, Nil)
{
}

ArrowButton_MacUI::ArrowButton_MacUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	ArrowButton_MacUI_Portable (direction, controller)
{
}

#if		qSoleInlineVirtualsNotStripped
ArrowButton_MacUI::~ArrowButton_MacUI ()
{
}
#endif	/*qSoleInlineVirtualsNotStripped*/



/*
 ********************************************************************************
 **************************** ArrowButton_MotifUI *******************************
 ********************************************************************************
 */
ArrowButton_MotifUI::ArrowButton_MotifUI (AbstractArrowButton::ArrowDirection direction):
	ArrowButton_MotifUI_Portable (direction, Nil)
{
}

ArrowButton_MotifUI::ArrowButton_MotifUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	ArrowButton_MotifUI_Portable (direction, controller)
{
}

#if		qSoleInlineVirtualsNotStripped
ArrowButton_MotifUI::~ArrowButton_MotifUI ()
{
}
#endif	/*qSoleInlineVirtualsNotStripped*/



/*
 ********************************************************************************
 ****************************** ArrowButton_WinUI *******************************
 ********************************************************************************
 */
ArrowButton_WinUI::ArrowButton_WinUI (AbstractArrowButton::ArrowDirection direction):
	ArrowButton_WinUI_Portable (direction, Nil)
{
}

ArrowButton_WinUI::ArrowButton_WinUI (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
	ArrowButton_WinUI_Portable (direction, controller)
{
}

#if		qSoleInlineVirtualsNotStripped
ArrowButton_WinUI::~ArrowButton_WinUI ()
{
}
#endif	/*qSoleInlineVirtualsNotStripped*/




/*
 ********************************************************************************
 ********************************* ArrowButton **********************************
 ********************************************************************************
 */

ArrowButton::ArrowButton (AbstractArrowButton::ArrowDirection direction):
#if		qMacUI
	ArrowButton_MacUI (direction, Nil)
#elif	qMotifUI
	ArrowButton_MotifUI (direction, Nil)
#elif	qWinUI
	ArrowButton_WinUI (direction, Nil)
#endif
{
}

ArrowButton::ArrowButton (AbstractArrowButton::ArrowDirection direction, ButtonController* controller) :
#if		qMacUI
	ArrowButton_MacUI (direction, controller)
#elif		qMotifUI
	ArrowButton_MotifUI (direction, controller)
#endif
{
}

#if		qSoleInlineVirtualsNotStripped
ArrowButton::~ArrowButton ()
{
}
#endif	/*qSoleInlineVirtualsNotStripped*/
