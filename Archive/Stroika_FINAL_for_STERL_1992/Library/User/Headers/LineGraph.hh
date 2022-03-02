/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__LineGraph__
#define	__LineGraph__

/*
 * $Header: /fuji/lewis/RCS/LineGraph.hh,v 1.7 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *		Simple LineGraph classes.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LineGraph.hh,v $
 *		Revision 1.7  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  21:02:57  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/04  14:25:27  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *
 *		Revision 1.3  1992/07/03  02:23:34  lewis
 *		Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.1  1992/04/15  13:14:54  sterling
 *		Initial revision
 *
 *
 */

#include	"Sequence.hh"

#include	"Shape.hh"

#include	"Axis.hh"
#include	"SeriesGraph.hh"



class	LineGraphSeries : public GraphSeries {
	public:
		LineGraphSeries (const Tile& tile, const Shape& shape, const String& title);
		
		nonvirtual	const	Shape&	GetShape () const;
		nonvirtual	void			SetShape (const Shape& shape);
		
		nonvirtual	Point	GetShapeSize () const;
		nonvirtual	void	SetShapeSize (const Point& shapeSize);
		
	private:
		Shape	fShape;
		Point	fShapeSize;
};

#if		!qRealTemplatesAvailable
	typedef LineGraphSeries* LinGrphSPtr;
	#define	LineGraphSeriesPtr	LinGrphSPtr
	
	Declare (Iterator, LinGrphSPtr);
	Declare (Collection, LinGrphSPtr);
	Declare (AbSequence, LinGrphSPtr);
	Declare (Array, LinGrphSPtr);
	Declare (Sequence_Array, LinGrphSPtr);
	Declare (Sequence, LinGrphSPtr);
#endif

class	LineGraphLegend;				// should be scoped within LineGraph
class	LineGraph : public SeriesGraph {
	public:
		LineGraph (Axis* xAxis = Nil, Axis* yAxis = Nil);
		~LineGraph ();
			
		nonvirtual	void	AddSeries (LineGraphSeries& series);
		nonvirtual	void	RemoveSeries (LineGraphSeries& series);
		
		nonvirtual	SequenceIterator(LineGraphSeriesPtr)*	MakeLineSeriesIterator (SequenceDirection d = eSequenceForward) const;

		virtual	void	RecalculateAxis ();

	protected:
		override	void	Layout ();
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
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
		nonvirtual	LineGraphLegend&	GetLegend_ () const;
		virtual		void				SetLegend_ (LineGraphLegend& legend, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	GraphTitle&		GetGraphTitle_ () const;
		virtual		void			SetGraphTitle_ (GraphTitle& title, UpdateMode update = eDelayedUpdate);

	private:
		Axis*				fXAxis;
		Axis*				fYAxis;
		GraphTitle*			fTitle;
		LineGraphLegend*	fLegend;
		
		Sequence(LineGraphSeriesPtr)	fSeries;
};

/* The following classes should be scoped within LineGraph except for compiler bugs */
class	LineGraphLegend : public View {
	public:
		LineGraphLegend (const LineGraph& lineGraph);
		
		nonvirtual	const LineGraph&	GetGraph () const;
		nonvirtual	void				SetGraph (const LineGraph& lineGraph);
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);
		
		virtual		Coordinate	GetDefaultHeight () const;
	
	private:
		const LineGraph* fLineGraph;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/* __LineGraph__ */

