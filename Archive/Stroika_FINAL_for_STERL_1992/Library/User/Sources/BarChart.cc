/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
/*
 * $Header: /fuji/lewis/RCS/BarChart.cc,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BarChart.cc,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/03  02:26:15  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *		
 *
 */

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"BarChart.hh"





/*
 ********************************************************************************
 ************************************* BarGraph ********************************
 ********************************************************************************
 */

BarGraph::BarGraph (LabelledAxis* xAxis, Axis* yAxis) :
	fXAxis (xAxis),
	fYAxis (yAxis),
	fSeries (),
	fLegend (Nil),
	fTitle (Nil)
{
	if (fXAxis == Nil) {
		fXAxis = new LabelledAxis (kEmptyString, Axis::eHorizontal, 0, 10);
	}
	if (fYAxis == Nil) {
		fYAxis = new Axis (kEmptyString, Axis::eVertical, 0, 100);
	}
	
	EnsureNotNil (fXAxis);
	EnsureNotNil (fYAxis);
	
	AddSubView (fXAxis);
	AddSubView (fYAxis);

	AddSubView (fTitle = new GraphTitle (kEmptyString));
	AddSubView (fLegend = new BarGraphLegend (*this));
}
		
BarGraph::~BarGraph ()
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

void	BarGraph::Layout ()
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

Point	BarGraph::CalcDefaultSize_ (const Point& defaultSize) const
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

LabelledAxis&	BarGraph::GetXAxis () const
{
	EnsureNotNil (fXAxis);
	return (*fXAxis);
}

Axis&	BarGraph::GetXAxis_ () const
{
	EnsureNotNil (fXAxis);
	return (*fXAxis);
}

Axis&	BarGraph::GetYAxis_ () const
{
	EnsureNotNil (fYAxis);
	return (*fYAxis);
}
		
void	BarGraph::AddSeries (GraphSeries& series)
{
	RequireNotNil (&series);
	
	fSeries.Append (&series);
	RecalculateAxis ();
}

void	BarGraph::RemoveSeries (GraphSeries& series)
{
	RequireNotNil (&series);
	
	fSeries.Remove (&series);
	RecalculateAxis ();
}
		
void	BarGraph::RecalculateAxis ()
{
	GetXAxis ().SetSubTicks (Max (fSeries.GetLength ()-1, (CollectionSize)0));
}

void	BarGraph::Draw (const Region& /*update*/)
{
	Rectangle	rectangle;
	
	Axis&	x = GetXAxis ();
	Axis&	y = GetYAxis ();
	
	Point	startP = Point (y.GetOrigin ().GetV (), x.GetOrigin ().GetH ());
	Coordinate	barWidth = x.GetSize ().GetH () / (fSeries.GetLength () * (x.GetMax () - x.GetMin ())) - 2;
	ForEachT (SequenceIterator, GraphSeriesPtr, it, MakeSeriesIterator ()) {
		GraphSeries&	current = *it.Current ();

		CollectionSize index = 0;
		ForEachT (SequenceIterator, Real, it1, current.MakeSequenceIterator (eSequenceForward)) {
			Real	indexValue = it1.CurrentIndex ();

			indexValue += Real (it.CurrentIndex ()-1) / (x.GetSubTicks () + 1.0);
			Point to = startP + 
				Point (y.ValueToCoordinate (it1.Current ()), 
					   x.ValueToCoordinate (indexValue));
			
			Rect	r = Rect (
				Point (to.GetV (), to.GetH () + 1), 
				Point (x.GetOrigin ().GetV () - to.GetV (), barWidth));
			
			Paint (rectangle, r, current.GetTile ());
			OutLine (rectangle, r);
		}
	}
	DrawBorder ();
}

String	BarGraph::GetTitle_ () const
{
	return (GetGraphTitle_ ().GetTitle ());
}

void	BarGraph::SetTitle_ (const String& title, Panel::UpdateMode update)
{
	GetGraphTitle_ ().SetTitle (title, update);
}

Boolean	BarGraph::GetTitleShown_ () const
{
	return (GetGraphTitle_ ().GetVisible ());
}

void	BarGraph::SetTitleShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetGraphTitle_ ().SetVisible (shown, update);
}

Boolean	BarGraph::GetLegendShown_ () const
{
	return (GetLegend_ ().GetVisible ());
}

void	BarGraph::SetLegendShown_ (Boolean shown, Panel::UpdateMode update)
{
	GetLegend_ ().SetVisible (shown, update);
}

BarGraphLegend&	BarGraph::GetLegend_ () const
{
	EnsureNotNil (fLegend);
	return (*fLegend);
}

void	BarGraph::SetLegend_ (BarGraphLegend& legend, Panel::UpdateMode update)
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

GraphTitle&	BarGraph::GetGraphTitle_ () const
{
	EnsureNotNil (fTitle);
	return (*fTitle);
}

void	BarGraph::SetGraphTitle_ (GraphTitle& title, Panel::UpdateMode update)
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

SequenceIterator(GraphSeriesPtr)*	BarGraph::MakeSeriesIterator_ (SequenceDirection d) const
{
	return (fSeries.MakeSequenceIterator (d));
}




/*
 ********************************************************************************
 ******************************* BarGraphLegend *********************************
 ********************************************************************************
 */

BarGraphLegend::BarGraphLegend (const BarGraph& BarGraph) :
	fBarGraph (&BarGraph)
{
	RequireNotNil (&BarGraph);
}

Coordinate	BarGraphLegend::GetDefaultHeight () const
{
	return (25);
}

Point	BarGraphLegend::GetBoxSize () const
{
	return (Point (10, 10));
}


Point	BarGraphLegend::CalcDefaultSize_ (const Point& /*defaultSize*/) const
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

const BarGraph&	BarGraphLegend::GetGraph () const
{
	EnsureNotNil (fBarGraph);
	return (*fBarGraph);
}

void	BarGraphLegend::SetGraph (const BarGraph& BarGraph)
{
	RequireNotNil (&BarGraph);
	fBarGraph = &BarGraph;
}
	
void	BarGraphLegend::Draw (const Region& /*update*/)
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

