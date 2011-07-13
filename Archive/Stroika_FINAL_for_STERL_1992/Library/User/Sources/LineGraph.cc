/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
/*
 * $Header: /fuji/lewis/RCS/LineGraph.cc,v 1.8 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LineGraph.cc,v $
 *		Revision 1.8  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  21:36:22  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/04  14:31:55  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *		Shapes now pass by value using new letter / env stuff. As side effect of that
 *		I have (at least temporarily gotten rid of Setter methods for fields of shapes
 *		So, I changed updates of line with SetFrom/SetTo to line = Line (from,to).
 *		I think thats better anyhow, but I could add back the setter methods?
 *
 *		Revision 1.4  1992/07/03  02:28:06  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/07/02  05:12:48  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  92/04/10  00:33:12  00:33:12  lewis (Lewis Pringle)
 *		Initial revision
 *		
 *
 */

#include	"Debug.hh"

#include	"Shape.hh"

#include	"LineGraph.hh"







/*
 ********************************************************************************
 ********************************* LineGraphSeries ******************************
 ********************************************************************************
 */
 
#if		!qRealTemplatesAvailable
	Implement (Iterator, LinGrphSPtr);
	Implement (Collection, LinGrphSPtr);
	Implement (AbSequence, LinGrphSPtr);
	Implement (Array, LinGrphSPtr);
	Implement (Sequence_Array, LinGrphSPtr);
	Implement (Sequence, LinGrphSPtr);
#endif

LineGraphSeries::LineGraphSeries (const Tile& tile, const Shape& shape, const String& title):
	GraphSeries (tile, title),
	fShape (shape),
	fShapeSize (Point (7, 7))
{
}
	
const Shape&	LineGraphSeries::GetShape () const
{
	return (fShape);
}

void	LineGraphSeries::SetShape (const Shape& shape)
{
	fShape = shape;
}

Point	LineGraphSeries::GetShapeSize () const
{
	return (fShapeSize);
}

void	LineGraphSeries::SetShapeSize (const Point& shapeSize)
{
	fShapeSize = shapeSize;
}






/*
 ********************************************************************************
 ************************************* LineGraph ********************************
 ********************************************************************************
 */

LineGraph::LineGraph (Axis* xAxis, Axis* yAxis):
	fXAxis (xAxis),
	fYAxis (yAxis),
	fSeries (),
	fLegend (Nil),
	fTitle (Nil)
{
	if (fXAxis == Nil) {
		fXAxis = new Axis (kEmptyString, Axis::eHorizontal, 0, 10);
	}
	if (fYAxis == Nil) {
		fYAxis = new Axis (kEmptyString, Axis::eVertical, 0, 100);
	}
	
	EnsureNotNil (fXAxis);
	EnsureNotNil (fYAxis);
	
	AddSubView (fXAxis);
	AddSubView (fYAxis);

	AddSubView (fTitle = new GraphTitle (kEmptyString));
	AddSubView (fLegend = new LineGraphLegend (*this));
}
		
LineGraph::~LineGraph ()
{
	RemoveSubView (fXAxis);
	delete fXAxis;

	RemoveSubView (fYAxis);	
	delete fYAxis;

	RemoveSubView (fTitle);	
	delete fTitle;

	RemoveSubView (fLegend);	
	delete fLegend;
}

void	LineGraph::Layout ()
{
	Point	graphSize = GetSize ();
	Point	graphOrigin = kZeroPoint;
	
	if (GetTitleShown ()) {
		Point	titleSize = GetGraphTitle_ ().CalcDefaultSize (GetSize ());
		GetGraphTitle_ ().SetSize (titleSize);
		GetGraphTitle_ ().SetOrigin (Point (0, (GetSize ().GetH () - titleSize.GetH ())/2));
		Point	offset = Point (Coordinate (titleSize.GetV () * 1.25), 0);
		graphSize -= offset;
		graphOrigin.SetV (offset.GetV ());
	}
	if (GetLegendShown ()) {
		Point	legendSize = GetLegend_ ().CalcDefaultSize (GetSize ());
		GetLegend_ ().SetSize (legendSize);
		GetLegend_ ().SetOrigin (Point (GetSize ().GetV () - legendSize.GetV (), (GetSize ().GetH () - legendSize.GetH ())/2));
		graphSize -= Point (legendSize.GetV () + legendSize.GetV () / 4, 0);
	}

	Axis&	x = GetXAxis ();
	Axis&	y = GetYAxis ();
	
	x.SetSize (x.CalcDefaultSize (graphSize));
	y.SetSize (y.CalcDefaultSize (graphSize));
	
	x.SetSize (x.CalcDefaultSize (graphSize 
		+ Point (0, -y.GetSize ().GetH () + x.GetTickRect ().GetLeft ())));
	y.SetSize (y.CalcDefaultSize (graphSize 
		+ Point (-x.GetSize ().GetV () + y.GetTickLabelRect ().GetBottom () - y.GetTickRect ().GetBottom (),
		  0)));

	y.SetOrigin (graphOrigin);
	x.SetOrigin (graphOrigin + Point (
		graphSize.GetV () - x.GetSize ().GetV (),
		y.GetSize ().GetH () - x.GetTickRect ().GetLeft ()));

	View::Layout ();
}

Point	LineGraph::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	tempSize = defaultSize;
	if (tempSize == kZeroPoint) {
		tempSize = Point (100, 100);
	}
	
	Point	newSize = GetXAxis ().CalcDefaultSize (tempSize) + GetYAxis ().CalcDefaultSize (tempSize);

	if (GetTitleShown ()) {
		Point titleSize = GetGraphTitle_ ().CalcDefaultSize (tempSize);
		newSize += Point (Coordinate (titleSize.GetV () * 1.25), 0);
	}
	if (GetLegendShown ()) {
		Point legendSize = GetLegend_ ().CalcDefaultSize (tempSize);
		newSize += Point (Coordinate (legendSize.GetV () * 1.25), 0);
	}
	
	return (newSize);
}
	
Axis&	LineGraph::GetXAxis_ () const
{
	EnsureNotNil (fXAxis);
	return (*fXAxis);
}

Axis&	LineGraph::GetYAxis_ () const
{
	EnsureNotNil (fYAxis);
	return (*fYAxis);
}
		
void	LineGraph::AddSeries (LineGraphSeries& series)
{
	RequireNotNil (&series);
	
	fSeries.Append (&series);
	RecalculateAxis ();
}

void	LineGraph::RemoveSeries (LineGraphSeries& series)
{
	RequireNotNil (&series);
	
	fSeries.Remove (&series);
	RecalculateAxis ();
}
		
SequenceIterator(LineGraphSeriesPtr)*	LineGraph::MakeLineSeriesIterator (SequenceDirection d) const
{
	return (fSeries.MakeSequenceIterator (d));
}
		
void	LineGraph::RecalculateAxis ()
{

}

void	LineGraph::Draw (const Region& /*update*/)
{
	Line	line = Line ();
	
	ForEach (LineGraphSeriesPtr, it, MakeLineSeriesIterator ()) {
		Coordinate	startH = GetXAxis ().GetOrigin ().GetH ();
		Coordinate	startV = GetYAxis ().GetOrigin ().GetV ();
		
		LineGraphSeries&	current = *it.Current ();
		
		Point	from = kZeroPoint;
		CollectionSize index = 0;
		ForEach (Real, it1, current) {
			++index;
			Point to = Point (startV + GetYAxis ().ValueToCoordinate (it1.Current ()), 
						startH + GetXAxis ().ValueToCoordinate (Real(index)));
			
			if (index > 1) {
				line = Line (from, to);
				OutLine (line);
			}
			
			from = to;

			Rect	frame = Rect (to - current.GetShapeSize ()/2, current.GetShapeSize ());
			Paint (current.GetShape (), frame, Brush (current.GetTile (), eXorTMode));
			OutLine (current.GetShape (), frame, Pen (kBlackTile, Point (1, 1)));
		}
	}
	DrawBorder ();
}

String	LineGraph::GetTitle_ () const
{
	return (GetGraphTitle_ ().GetTitle ());
}

void	LineGraph::SetTitle_ (const String& title, Panel::UpdateMode update)
{
	GetGraphTitle_ ().SetTitle (title, update);
}

Boolean	LineGraph::GetTitleShown_ () const
{
	return (GetGraphTitle_ ().GetVisible ());
}

void	LineGraph::SetTitleShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetGraphTitle_ ().SetVisible (shown, update);
}

Boolean	LineGraph::GetLegendShown_ () const
{
	return (GetLegend_ ().GetVisible ());
}

void	LineGraph::SetLegendShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetLegend_ ().SetVisible (shown, update);
}

LineGraphLegend&	LineGraph::GetLegend_ () const
{
	EnsureNotNil (fLegend);
	return (*fLegend);
}

void	LineGraph::SetLegend_ (LineGraphLegend& legend, Panel::UpdateMode update)
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

GraphTitle&	LineGraph::GetGraphTitle_ () const
{
	EnsureNotNil (fTitle);
	return (*fTitle);
}

void	LineGraph::SetGraphTitle_ (GraphTitle& title, Panel::UpdateMode update)
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

SequenceIterator(GraphSeriesPtr)*	LineGraph::MakeSeriesIterator_ (SequenceDirection d) const
{
	return ((SequenceIterator(GraphSeriesPtr)*) MakeLineSeriesIterator (d));
}





/*
 ********************************************************************************
 ****************************** LineGraphLegend *********************************
 ********************************************************************************
 */

LineGraphLegend::LineGraphLegend (const LineGraph& lineGraph):
	fLineGraph (&lineGraph)
{
	RequireNotNil (&lineGraph);
}

Coordinate	LineGraphLegend::GetDefaultHeight () const
{
	return (20);
}

Point	LineGraphLegend::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point where = Point (1, 1);
	Coordinate maxWidth = 0;
	Font f = GetEffectiveFont ();
	Coordinate	lineStart = 0;
	ForEach (LineGraphSeriesPtr, it, GetGraph ().MakeLineSeriesIterator ()) {
		LineGraphSeries* current = it.Current ();
		maxWidth = Max (maxWidth, TextWidth (current->GetTitle ()));
		where += Point (current->GetShapeSize ().GetV () + 2, 0);
		if ((where.GetV () + current->GetShapeSize ().GetV ()) > GetDefaultHeight ()) {
			where.SetV (1);
			where.SetH (where.GetH () + current->GetShapeSize ().GetH () + 2 + maxWidth + 2);
			maxWidth = 0;
		}
	}
	if (maxWidth != 0) {
		maxWidth += 12;
	}
	return (Point (GetDefaultHeight (), where.GetH () + maxWidth));
}

const LineGraph&	LineGraphLegend::GetGraph () const
{
	EnsureNotNil (fLineGraph);
	return (*fLineGraph);
}

void	LineGraphLegend::SetGraph (const LineGraph& lineGraph)
{
	RequireNotNil (&lineGraph);
	fLineGraph = &lineGraph;
}
	
void	LineGraphLegend::Draw (const Region& /*update*/)
{
	Point where = Point (1, 1);
	Coordinate maxWidth = 0;
	Font f = GetEffectiveFont ();
	Coordinate	lineStart = 0;
	ForEach (LineGraphSeriesPtr, it, GetGraph ().MakeLineSeriesIterator ()) {
		LineGraphSeries* current = it.Current ();
		Rect shapeRect = CenterRectAroundRect (Rect (where, current->GetShapeSize ()), Rect (where, Point (lineStart + f.GetFontHeight (), current->GetShapeSize ().GetH ())));
		
		Paint (current->GetShape (), shapeRect, Brush (current->GetTile ()));
		OutLine (current->GetShape (), shapeRect);
		maxWidth = Max (maxWidth, TextWidth (current->GetTitle ()));
		DrawText (current->GetTitle (), where + Point (lineStart, current->GetShapeSize ().GetH () + 2));
		where += Point (current->GetShapeSize ().GetV () + 2, 0);
		if ((where.GetV () + current->GetShapeSize ().GetV ()) > GetSize ().GetV ()) {
			where.SetV (1);
			where.SetH (where.GetH () + current->GetShapeSize ().GetH () + 2 + maxWidth + 2);
			maxWidth = 0;
		}
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

