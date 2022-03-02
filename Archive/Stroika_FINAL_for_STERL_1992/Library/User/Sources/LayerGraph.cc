/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
/*
 * $Header: /fuji/lewis/RCS/LayerGraph.cc,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LayerGraph.cc,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/04  14:30:41  lewis
 *		Warmed over sterls MyPolygon hack so it works with my shape changes - must
 *		discuss with him and see what we need to do to Polygon, and shapes in general.
 *
 *		Revision 1.3  1992/07/03  02:27:28  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/05/13  17:54:04  lewis
 *		STERL.
 *
 *		Revision 1.1  92/04/15  13:24:53  13:24:53  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 */

#include	"Debug.hh"

#include	"Shape.hh"

#include	"LayerGraph.hh"





/*
 ********************************************************************************
 ************************************* LayerGraph ********************************
 ********************************************************************************
 */

LayerGraph::LayerGraph (Axis* xAxis, Axis* yAxis) :
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
	AddSubView (fLegend = new LayerGraphLegend (*this));
}
		
LayerGraph::~LayerGraph ()
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

void	LayerGraph::Layout ()
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

Point	LayerGraph::CalcDefaultSize_ (const Point& defaultSize) const
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
	
Axis&	LayerGraph::GetXAxis_ () const
{
	EnsureNotNil (fXAxis);
	return (*fXAxis);
}

Axis&	LayerGraph::GetYAxis_ () const
{
	EnsureNotNil (fYAxis);
	return (*fYAxis);
}
		
void	LayerGraph::AddSeries (GraphSeries& series)
{
	RequireNotNil (&series);
	
	fSeries.Append (&series);
	RecalculateAxis ();
}

void	LayerGraph::RemoveSeries (GraphSeries& series)
{
	RequireNotNil (&series);
	
	fSeries.Remove (&series);
	RecalculateAxis ();
}
		
void	LayerGraph::RecalculateAxis ()
{

}

class	MyPolygon : public ShapeRepresentation {
	public:
		MyPolygon (const Sequence(Point)& points);
#if 0
	protected:
		MyPolygon (const MyPolygon& from);
		const MyPolygon& operator= (const MyPolygon& rhs);
#endif
	public:
		override	void	OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const;
		override	void	Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const;

		override	Point	GetLogicalSize ()	const;
		nonvirtual	Point	GetTopLeft () const;
		
	private:
		Polygon	fPolygon;
		Point	fTopLeft;
};

MyPolygon::MyPolygon (const Sequence(Point)& points) :
	fPolygon (points),
	fTopLeft (kZeroPoint)
{
	if (points.GetLength () >= 1) {
		fTopLeft = points[1];
		ForEach (Point, it, points) {
			if (fTopLeft.GetV () > it.Current ().GetV ()) {
				fTopLeft.SetV (it.Current ().GetV ());
			}
			if (fTopLeft.GetH () > it.Current ().GetH ()) {
				fTopLeft.SetH (it.Current ().GetH ());
			}
		}
	}
}

void	MyPolygon::OutLine (Tablet& on, const Rect& shapeBounds, const Pen& pen) const
{
	fPolygon.OutLine (on, shapeBounds, pen);
}

void	MyPolygon::Paint (Tablet& on, const Rect& shapeBounds, const Brush& brush) const
{
	fPolygon.Paint (on, shapeBounds, brush);
}

Point	MyPolygon::GetLogicalSize ()	const
{
	return (fPolygon.GetLogicalSize ());
}

Point	MyPolygon::GetTopLeft () const
{
	return (fTopLeft);
}

void	LayerGraph::Draw (const Region& /*update*/)
{
	Sequence_Array(Point)	points;
	Real	highWaterMark = -1000;

	for (int i = 1; i <= 5; i++) {
		points.Append (kZeroPoint);
	}

	Point	startP = Point (GetYAxis ().GetOrigin ().GetV (), GetXAxis ().GetOrigin ().GetH ());

	GraphSeries* previous = Nil;
	ForEach (GraphSeriesPtr, it, MakeSeriesIterator ()) {
		GraphSeries&	current = *it.Current ();
		
		Point	from = kZeroPoint;
		CollectionSize index = 0;
		ForEach (Real, it1, current) {
			++index;
			Real oldToValue = 0;
			Real oldFromValue = GetYAxis ().GetMin ();
			ForEach (GraphSeriesPtr, it3, MakeSeriesIterator ()) {
				if (it3.Current () == &current) {
					break;
				}
				oldToValue += (*it3.Current ())[index];
				if (index > 1) {
					oldFromValue += (*it3.Current ())[index-1];
				}
			}
			
			Coordinate	xCoord = GetXAxis ().ValueToCoordinate (Real(index));
			Point oldFrom = startP + Point (GetYAxis ().ValueToCoordinate (oldFromValue), xCoord);
			Point oldTo   = startP + Point (GetYAxis ().ValueToCoordinate (oldToValue), xCoord);
			Point to 	  = startP + Point (GetYAxis ().ValueToCoordinate (it1.Current () + oldToValue), xCoord);

			if (index > 1) {
				points.SetAt (Point (oldFrom.GetV (),	from.GetH ()), 	1);
				points.SetAt (Point (oldTo.GetV (),		to.GetH ()), 	2);
				points.SetAt (Point (to.GetV (), 		to.GetH ()), 	3);
				points.SetAt (Point (from.GetV (),		from.GetH ()), 	4);
				points.SetAt (Point (oldFrom.GetV (),	from.GetH ()), 	5);

#if 1
				MyPolygon*	mp	=	new MyPolygon (points);	// DONT DELETE - DELETED AUTOMATICALLY BY SHAPE!!!
				Fill (Shape (mp), Rect (mp->GetTopLeft (), mp->GetLogicalSize ()), current.GetTile ());
#else
				MyPolygon	poly = MyPolygon (points);
				Fill (poly, Rect (poly.GetTopLeft (), poly.GetLogicalSize ()), current.GetTile ());
#endif
			}
			
			from = to;
		}
		previous = it.Current ();
	}
	DrawBorder ();
}

String	LayerGraph::GetTitle_ () const
{
	return (GetGraphTitle_ ().GetTitle ());
}

void	LayerGraph::SetTitle_ (const String& title, Panel::UpdateMode update)
{
	GetGraphTitle_ ().SetTitle (title, update);
}

Boolean	LayerGraph::GetTitleShown_ () const
{
	return (GetGraphTitle_ ().GetVisible ());
}

void	LayerGraph::SetTitleShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetGraphTitle_ ().SetVisible (shown, update);
}

Boolean	LayerGraph::GetLegendShown_ () const
{
	return (GetLegend_ ().GetVisible ());
}

void	LayerGraph::SetLegendShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetLegend_ ().SetVisible (shown, update);
}

LayerGraphLegend&	LayerGraph::GetLegend_ () const
{
	EnsureNotNil (fLegend);
	return (*fLegend);
}

void	LayerGraph::SetLegend_ (LayerGraphLegend& legend, Panel::UpdateMode update)
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

GraphTitle&	LayerGraph::GetGraphTitle_ () const
{
	EnsureNotNil (fTitle);
	return (*fTitle);
}

void	LayerGraph::SetGraphTitle_ (GraphTitle& title, Panel::UpdateMode update)
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

SequenceIterator(GraphSeriesPtr)*	LayerGraph::MakeSeriesIterator_ (SequenceDirection d) const
{
	return (fSeries.MakeSequenceIterator (d));
}





/*
 ********************************************************************************
 ****************************** LayerGraphLegend ********************************
 ********************************************************************************
 */

LayerGraphLegend::LayerGraphLegend (const LayerGraph& LayerGraph):
	fLayerGraph (&LayerGraph)
{
	RequireNotNil (&LayerGraph);
}

Coordinate	LayerGraphLegend::GetDefaultHeight () const
{
	return (25);
}

Point	LayerGraphLegend::GetBoxSize () const
{
	return (Point (10, 10));
}

Point	LayerGraphLegend::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point where = Point (1, 1);
	Coordinate maxWidth = 0;
	Font f = GetEffectiveFont ();
	Coordinate	lineStart = 0;
	ForEach (GraphSeriesPtr, it, GetGraph ().MakeSeriesIterator ()) {
		GraphSeries* current = it.Current ();
		maxWidth = Max (maxWidth, TextWidth (current->GetTitle ()));
		where += Point (GetBoxSize ().GetV () + 2, 0);
		if ((where.GetV () + GetBoxSize ().GetV ()) > GetDefaultHeight ()) {
			where.SetV (1);
			where.SetH (where.GetH () + GetBoxSize ().GetH () + 2 + maxWidth + 2);
			maxWidth = 0;
		}
	}
	if (maxWidth != 0) {
		maxWidth += 12;
	}
	return (Point (GetDefaultHeight (), where.GetH () + maxWidth));
}

const LayerGraph&	LayerGraphLegend::GetGraph () const
{
	EnsureNotNil (fLayerGraph);
	return (*fLayerGraph);
}

void	LayerGraphLegend::SetGraph (const LayerGraph& LayerGraph)
{
	RequireNotNil (&LayerGraph);
	fLayerGraph = &LayerGraph;
}
	
void	LayerGraphLegend::Draw (const Region& /*update*/)
{
	Point where = Point (1, 1);
	Coordinate maxWidth = 0;
	Font f = GetEffectiveFont ();
	Coordinate	lineStart = 0;
	ForEach (GraphSeriesPtr, it, GetGraph ().MakeSeriesIterator ()) {
		GraphSeries* current = it.Current ();
		Rect shapeRect = CenterRectAroundRect (Rect (where, GetBoxSize ()), Rect (where, Point (lineStart + f.GetFontHeight (), GetBoxSize ().GetH ())));
		
		Paint (Rectangle (), shapeRect, Brush (current->GetTile ()));
		OutLine (Rectangle (), shapeRect);
		maxWidth = Max (maxWidth, TextWidth (current->GetTitle ()));
		DrawText (current->GetTitle (), where + Point (lineStart, GetBoxSize ().GetH () + 2));
		where += Point (GetBoxSize ().GetV () + 2, 0);
		if ((where.GetV () + GetBoxSize ().GetV ()) > GetSize ().GetV ()) {
			where.SetV (1);
			where.SetH (where.GetH () + GetBoxSize ().GetH () + 2 + maxWidth + 2);
			maxWidth = 0;
		}
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

