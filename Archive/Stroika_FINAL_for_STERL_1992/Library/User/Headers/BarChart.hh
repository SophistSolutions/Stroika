/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__BarGraph__
#define	__BarGraph__

/*
 * $Header: /fuji/lewis/RCS/BarChart.hh,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *		BarGraph class.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: BarChart.hh,v $
 *		Revision 1.6  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  21:02:57  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.3  1992/07/02  05:09:34  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/06/20  17:33:49  lewis
 *		Initial revision
 *
 *		Revision 1.2  1992/04/15  13:14:47  sterling
 *		graph support
 *
 *
 */

#include	"View.hh"

#include	"SeriesGraph.hh"


class BarGraphLegend;		// should be scoped class

class	BarGraph : public SeriesGraph {
	public:
		BarGraph (LabelledAxis* xAxis = Nil, Axis* yAxis = Nil);
		~BarGraph ();
		
		// shadow to get correct type
		nonvirtual	LabelledAxis&	GetXAxis () const;

		nonvirtual	void	AddSeries (GraphSeries& series);
		nonvirtual	void	RemoveSeries (GraphSeries& series);

		virtual	void	RecalculateAxis ();

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		override	void	Draw (const Region& update);

		override	Axis&	GetXAxis_ () const;
		override	Axis&	GetYAxis_ () const;
		
		override	SequenceIterator(GraphSeriesPtr)*	MakeSeriesIterator_ (SequenceDirection d) const;

		override	String	GetTitle_ () const;
		override	void	SetTitle_ (const String& title, UpdateMode update);
		override	Boolean	GetTitleShown_ () const;
		override	void	SetTitleShown_ (Boolean shown, UpdateMode update);
		
		override	Boolean	GetLegendShown_ () const;
		override	void	SetLegendShown_ (Boolean shown, UpdateMode update);

		/* 
			We allow protected access to our subviews, to facilitate subclassing. Calling a "Set"
			method for an owned view will delete any existing instance. This interface allows, through
			shadowing, ownership of subclasses of the given views.
		*/
		nonvirtual	BarGraphLegend&	GetLegend_ () const;
		virtual		void			SetLegend_ (BarGraphLegend& legend, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	GraphTitle&		GetGraphTitle_ () const;
		virtual		void			SetGraphTitle_ (GraphTitle& title, UpdateMode update = eDelayedUpdate);

	private:
		LabelledAxis*	fXAxis;
		Axis*			fYAxis;
		GraphTitle*		fTitle;
		BarGraphLegend*	fLegend;
		
		Sequence(GraphSeriesPtr)	fSeries;
};

/* The following classes should be scoped within BarGraph except for compiler bugs */
class	BarGraphLegend : public View {
	public:
		BarGraphLegend (const BarGraph& BarGraph);
		
		nonvirtual	const BarGraph&	GetGraph () const;
		nonvirtual	void			SetGraph (const BarGraph& BarGraph);
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);
		
		virtual		Coordinate	GetDefaultHeight () const;
		virtual		Point		GetBoxSize () const;
	
	private:
		const BarGraph* fBarGraph;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/* __BarGraph__ */

