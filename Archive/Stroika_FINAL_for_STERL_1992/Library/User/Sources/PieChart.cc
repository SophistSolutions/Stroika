/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
/*
 *
 * $Header: /fuji/lewis/RCS/PieChart.cc,v 1.8 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PieChart.cc,v $
 *		Revision 1.8  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  21:36:22  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/04  14:33:17  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *
 *		Revision 1.4  1992/07/03  04:07:39  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/07/02  05:12:48  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *
 */



#include	"Debug.hh"
#include	"Format.hh"

#include	"Shape.hh"

#include	"PieChart.hh"




/*
 ********************************************************************************
 ******************************* AbstractPieChart *******************************
 ********************************************************************************
 */

AbstractPieChart::AbstractPieChart () :
	View ()
{
}
		
String	AbstractPieChart::GetTitle () const
{
	return (GetTitle_ ());
}
		
void	AbstractPieChart::SetTitle (const String& title, Panel::UpdateMode update)
{
	if (GetTitle () != title) {
		SetTitle_ (title, update);
	}
	Ensure (GetTitle () == title);
}
		
Boolean	AbstractPieChart::GetTitleShown () const
{
	return (GetTitleShown_ ());
}
		
void	AbstractPieChart::SetTitleShown (Boolean shown, Panel::UpdateMode update)
{
	if (GetTitleShown () != shown) {
		SetTitleShown_ (shown, update);
	}
	Ensure (GetTitleShown () == shown);
}
		
Boolean	AbstractPieChart::GetLegendShown () const
{
	return (GetLegendShown_ ());
}
		
void	AbstractPieChart::SetLegendShown (Boolean shown, Panel::UpdateMode update)
{
	if (GetLegendShown () != shown) {
		SetLegendShown_ (shown, update);
	}
	Ensure (GetLegendShown () == shown);
}
		
void	AbstractPieChart::AddSlice (Real value, const Tile& tile, const String& title)
{
#if qDebug
	CollectionSize oldSliceTotal = CountSlices ();
#endif
	AddSlice_ (value, tile, title);

	Ensure (CountSlices () == (oldSliceTotal+1));
}
		
Real 	AbstractPieChart::GetSliceValue (CollectionSize slice) const
{
	Require (slice <= CountSlices ());
	
	return (GetSliceValue_ (slice));
}
		
void	AbstractPieChart::SetSliceValue (CollectionSize slice, Real value, Panel::UpdateMode update)
{
	Require (slice <= CountSlices ());
	
	if (GetSliceValue (slice) != value) {
		SetSliceValue_ (slice, value, update);
	}
	Ensure (GetSliceValue (slice) == value);
}
		
Tile	AbstractPieChart::GetSliceTile (CollectionSize slice) const
{
	Require (slice <= CountSlices ());
	
	return (GetSliceTile_ (slice));
}
		
void	AbstractPieChart::SetSliceTile (CollectionSize slice, const Tile& tile, Panel::UpdateMode update)
{
	Require (slice <= CountSlices ());
	
//	if (GetSliceTile (slice) != tile) {
	if (True) {
		SetSliceTile_ (slice, tile, update);
	}
//	Ensure (GetSliceTile (slice) == tile);
}
		
String	AbstractPieChart::GetSliceTitle (CollectionSize slice) const
{
	Require (slice <= CountSlices ());
	
	return (GetSliceTitle_ (slice));
}
		
void	AbstractPieChart::SetSliceTitle (CollectionSize slice, const String& title, Panel::UpdateMode update)
{
	Require (slice <= CountSlices ());
	
	if (GetSliceTitle (slice) != title) {
		SetSliceTitle_ (slice, title, update);
	}
	Ensure (GetSliceTitle (slice) == title);
}
		
Real	AbstractPieChart::GetSlicePercent (CollectionSize slice) const
{
	Require (slice <= CountSlices ());
	
	return (GetSliceValue (slice) / GetTotalValue ());
}
				
CollectionSize	AbstractPieChart::CountSlices () const
{
	return (CountSlices_ ());
}	
		
Real	AbstractPieChart::GetTotalValue () const
{
	return (GetTotalValue_ ());
}
		

/*
 ********************************************************************************
 *********************************** PieChart ***********************************
 ********************************************************************************
 */
#if		!qRealTemplatesAvailable
	Implement (Iterator, SlicePtr);
	Implement (Collection, SlicePtr);
	Implement (AbSequence, SlicePtr);
	Implement (Array, SlicePtr);
	Implement (Sequence_Array, SlicePtr);
	Implement (Sequence, SlicePtr);
#endif

PieChart::PieChart () :
	AbstractPieChart (),
	fLegend (Nil),
	fTitle (Nil),
	fPie (Nil)
{
	AddSubView (fPie = new Pie (*this));
	AddSubView (fTitle = new GraphTitle (kEmptyString));
	AddSubView (fLegend = new PieChartLegend (*fPie));
}

PieChart::~PieChart ()
{
	RemoveSubView (fPie);
	delete fPie;

	RemoveSubView (fTitle);
	delete fTitle;

	RemoveSubView (fLegend);
	delete fLegend;
}
		
Point	PieChart::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	newSize = GetPie_ ().CalcDefaultSize (defaultSize);
	if (GetTitleShown ()) {
		Point chartSize = GetChartTitle_ ().CalcDefaultSize (defaultSize);
		newSize += Point (Coordinate (chartSize.GetV () * 1.25), 0);
	}
	if (GetLegendShown ()) {
		Point legendSize = GetLegend_ ().CalcDefaultSize (defaultSize);
		newSize += Point (Coordinate (legendSize.GetV () * 1.25), 0);
	}
	
	return (newSize);
}

void	PieChart::Draw (const Region& /*update*/)
{
	DrawBorder ();
}

void	PieChart::Layout ()
{
	Point	pieSize = GetSize ();
	Point	pieOrigin = kZeroPoint;
	
	if (GetTitleShown ()) {
		Point	chartSize = GetChartTitle_ ().CalcDefaultSize (GetSize ());
		GetChartTitle_ ().SetSize (chartSize);
		GetChartTitle_ ().SetOrigin (Point (0, (GetSize ().GetH () - chartSize.GetH ())/2));
		Point	offset = Point (Coordinate (chartSize.GetV () * 1.25), 0);
		pieOrigin += offset;
		pieSize -= offset;
	}
	if (GetLegendShown ()) {
		Point	legendSize = GetLegend_ ().CalcDefaultSize (GetSize ());
		GetLegend_ ().SetSize (legendSize);
		GetLegend_ ().SetOrigin (Point (GetSize ().GetV () - legendSize.GetV (), (GetSize ().GetH () - legendSize.GetH ())/2));
		pieSize -= Point (legendSize.GetV () + legendSize.GetV () / 4, 0);
	}
	
	Coordinate delta = pieSize.GetV () - pieSize.GetH ();
	if (delta > 0) {
		pieSize.SetV (pieSize.GetH ());
		pieOrigin.SetV (pieOrigin.GetV () + delta/2);
	}
	else {
		pieSize.SetH (pieSize.GetV ());
		pieOrigin.SetH (pieOrigin.GetH () - delta/2);
	}
	
	GetPie_ ().SetExtent (Rect (pieOrigin, pieSize));

	View::Layout ();
}

String	PieChart::GetTitle_ () const
{
	return (GetChartTitle_ ().GetTitle ());
}

void	PieChart::SetTitle_ (const String& title, Panel::UpdateMode update)
{
	GetChartTitle_ ().SetTitle (title, update);
}

Boolean	PieChart::GetTitleShown_ () const
{
	return (GetChartTitle_ ().GetVisible ());
}

void	PieChart::SetTitleShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetChartTitle_ ().SetVisible (shown, update);
}
		
Boolean	PieChart::GetLegendShown_ () const
{
	return (GetLegend_ ().GetVisible ());
}

void	PieChart::SetLegendShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetLegend_ ().SetVisible (shown, update);
}
		
void	PieChart::AddSlice_ (Real value, const Tile& tile, const String& title)
{
	Slice* newSlice = new Slice (GetPie_ (), value, tile, title);
	GetPie_ ().AddSlice (*newSlice);
}

Real 	PieChart::GetSliceValue_ (CollectionSize slice) const
{
	return (GetPie_ ().GetSlice (slice).GetValue ());
}

void	PieChart::SetSliceValue_ (CollectionSize slice, Real value, Panel::UpdateMode update)
{
	GetPie_ ().GetSlice (slice).SetValue (value, update);
}
		
Tile	PieChart::GetSliceTile_ (CollectionSize slice) const
{
	return (GetPie_ ().GetSlice (slice).GetTile ());
}

void	PieChart::SetSliceTile_ (CollectionSize slice, const Tile& tile, Panel::UpdateMode update)
{
	GetPie_ ().GetSlice (slice).SetTile (tile, update);
}

String	PieChart::GetSliceTitle_ (CollectionSize slice) const
{
	return (GetPie_ ().GetSlice (slice).GetTitle ());
}

void	PieChart::SetSliceTitle_ (CollectionSize slice, const String& title, Panel::UpdateMode update)
{
	GetPie_ ().GetSlice (slice).SetTitle (title, update);
}
		
CollectionSize	PieChart::CountSlices_ () const
{
	return (GetPie_ ().CountSlices ());
}
		
Real	PieChart::GetTotalValue_ () const
{
	Real total = 0;
	ForEach (SlicePtr, it, GetPie_ ().MakeSliceIterator ()) {
		total += it.Current ()->GetValue ();
	}
	return (total);
}

PieChartLegend&	PieChart::GetLegend_ () const
{
	EnsureNotNil (fLegend);
	return (*fLegend);
}

void	PieChart::SetLegend_ (PieChartLegend& legend, Panel::UpdateMode update)
{
	RequireNotNil (&legend);
	if (&legend != fLegend) {
		RequireNotNil (fLegend);
		RemoveSubView (fLegend, update);
		delete fLegend;
		fLegend = &legend;
		AddSubView (fLegend, update);
	}
	EnsureNotNil (fLegend);
}

GraphTitle&	PieChart::GetChartTitle_ () const
{
	EnsureNotNil (fTitle);
	return (*fTitle);
}

void	PieChart::SetChartTitle_ (GraphTitle& title, Panel::UpdateMode update)
{
	RequireNotNil (&title);
	if (&title != fTitle) {
		RequireNotNil (fTitle);
		RemoveSubView (fTitle, update);
		delete fTitle;
		fTitle = &title;
		AddSubView (fTitle, update);
	}
	EnsureNotNil (fTitle);
}
		
Pie&	PieChart::GetPie_ () const
{
	EnsureNotNil (fPie);
	return (*fPie);
}

void	PieChart::SetPie_ (Pie& pie, Panel::UpdateMode update)
{
	RequireNotNil (&pie);
	if (&pie != fPie) {
		RequireNotNil (fPie);
		RemoveSubView (fPie, update);
		delete fPie;
		fPie = &pie;
		AddSubView (fPie, update);
	}
	EnsureNotNil (fPie);
}






/*
 ********************************************************************************
 ******************************* PieChartLegend *********************************
 ********************************************************************************
 */
PieChartLegend::PieChartLegend (const Pie& pie):
	fPie (&pie)
{
	RequireNotNil (fPie);
}
		
const Pie&	PieChartLegend::GetPie () const
{
	RequireNotNil (fPie);
	return (*fPie);
}

void	PieChartLegend::SetPie (const Pie& pie)
{
	fPie = &pie;
	RequireNotNil (fPie);
}

Coordinate	PieChartLegend::GetDefaultHeight () const
{
	return (25);
}
	
Point	PieChartLegend::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point where = Point (1, 1);
	Point rSize = Point (10, 10);
	Coordinate maxWidth = 0;
	Font f = GetEffectiveFont ();
	Coordinate	lineStart = 0;
	ForEach (SlicePtr, it, GetPie ().MakeSliceIterator ()) {
		Slice* current = it.Current ();
		maxWidth = Max (maxWidth, TextWidth (current->GetTitle ()));
		where += Point (rSize.GetV () + 2, 0);
		if ((where.GetV () + rSize.GetV ()) > GetDefaultHeight ()) {
			where.SetV (1);
			where.SetH (where.GetH () + rSize.GetH () + 2 + maxWidth + 2);
			maxWidth = 0;
		}
	}
	if (maxWidth != 0) {
		maxWidth += 12;
	}
	
	return (Point (GetDefaultHeight (), where.GetH () + maxWidth));
}

void	PieChartLegend::Draw (const Region& /*update*/)
{
	Point where = Point (1, 1);
	Point rSize = Point (10, 10);
	Coordinate maxWidth = 0;
	Font f = GetEffectiveFont ();
	Coordinate	lineStart = 0;
	ForEach (SlicePtr, it, GetPie ().MakeSliceIterator ()) {
		Slice* current = it.Current ();
		Rect shapeRect = CenterRectAroundRect (Rect (where, rSize), Rect (where, Point (lineStart + f.GetFontHeight (), rSize.GetH ())));
		
		Paint (Rectangle (), shapeRect, Brush (current->GetTile ()));
		OutLine (Rectangle (), shapeRect);
		maxWidth = Max (maxWidth, TextWidth (current->GetTitle ()));
		DrawText (current->GetTitle (), where + Point (lineStart, rSize.GetH () + 2));
		where += Point (rSize.GetV () + 2, 0);
		if ((where.GetV () + rSize.GetV ()) > GetSize ().GetV ()) {
			where.SetV (1);
			where.SetH (where.GetH () + rSize.GetH () + 2 + maxWidth + 2);
			maxWidth = 0;
		}
	}
}





/*
 ********************************************************************************
 ************************************ Pie ***************************************
 ********************************************************************************
 */
Pie::Pie (PieChart& owner):
	fOwner (owner),
	fSlices ()
{
	SetShape (Oval ());
}

Pie::~Pie ()
{
	ForEach (SlicePtr, it, MakeSliceIterator ()) {
		Slice* current = it.Current ();
		RemoveSubView (current);
		delete current;
	}
}
	
SequenceIterator(SlicePtr)*	Pie::MakeSliceIterator (SequenceDirection d) const
{
	return (fSlices.MakeSequenceIterator (d));
}

CollectionSize	Pie::CountSlices () const
{
	return (fSlices.GetLength ());
}

void	Pie::AddSlice (Slice& slice)
{
	RequireNotNil (&slice);
	fSlices.Append (&slice);
	AddSubView (&slice);
}
	
Slice&	Pie::GetSlice (CollectionSize index) const
{
	Require (index <= CountSlices ());
	return (*fSlices[index]);
}
		
Point	Pie::CalcDefaultSize_ (const Point& defaultSize) const
{
	Coordinate c = (defaultSize.GetH () == 0) ? 100 : defaultSize.GetH ();
	return (Point (c, c));
}

void	Pie::Layout ()
{
	View::Layout ();
	
	Angle startAngle = 0;
	Real	totalValue = fOwner.GetTotalValue ();
	ForEach (SlicePtr, it, MakeSliceIterator ()) {
		Slice* current = it.Current ();
		Angle width = Angle (360 * current->GetValue () / totalValue, Angle::eDegrees);

		Assert (width >= 0);
		Assert (width <= 360);
		current->SetSize (GetSize ());
		current->SetArc (Arc (startAngle, width));
		startAngle = startAngle + width;
	}
}

void	Pie::Draw (const Region& update)
{
	OutLine (GetShape (), GetLocalExtent ());
	View::Draw (update);
}





/*
 ********************************************************************************
 ********************************** Slice ***************************************
 ********************************************************************************
 */
Slice::Slice (Pie& owner, Real value, const Tile& tile, const String& title):
	fOwner (owner),
	fValue (value),
	fTile (tile),
	fTitle (title),
	fArc (0, 0)
{
}

Real	Slice::GetValue () const
{
	return (fValue);
}

void	Slice::SetValue (Real value, Panel::UpdateMode updateMode)
{
	if (GetValue () != value) {
		fValue = value;
	}
	Ensure (GetValue () == value);
}

Tile	Slice::GetTile () const
{
	return (fTile);
}

void	Slice::SetTile (const Tile& tile, Panel::UpdateMode updateMode)
{
	fTile = tile;
	Refresh (updateMode);
}
		
String	Slice::GetTitle () const
{
	return (fTitle);
}
		
void	Slice::SetTitle (const String& title, Panel::UpdateMode updateMode)
{
	if (GetTitle () != title) {
		fTitle = title;
	}
	Ensure (GetTitle () == title);
}
		
void	Slice::SetArc (const Arc& arc, Panel::UpdateMode updateMode)
{
	fArc = arc;
	Refresh (updateMode);
}

Arc		Slice::GetArc () const
{
	return (fArc);
}

void	Slice::Draw (const Region& /* update */)
{
	Paint (fArc, GetLocalExtent (), Brush (GetTile ()));
}

Boolean	Slice::Opaque () const
{
	return (False);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

