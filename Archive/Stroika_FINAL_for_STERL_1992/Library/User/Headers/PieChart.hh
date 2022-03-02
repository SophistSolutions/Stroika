/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PieChart__
#define	__PieChart__

/*
 * $Header: /fuji/lewis/RCS/PieChart.hh,v 1.6 1992/09/08 16:00:29 lewis Exp $
 *
 * Description:
 *		Pie chart classes. Like all graph classes, pie charts support a title and a legend.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: PieChart.hh,v $
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
 *		Revision 1.2  1992/04/15  19:00:43  lewis
 *		Sterls implementation.
 *
 *
 */

#include	"Shape.hh"
#include	"View.hh"

#include	"Axis.hh"


class	AbstractPieChart : public View {
	protected:
		AbstractPieChart ();
		
	public:
		nonvirtual	String	GetTitle () const;
		nonvirtual	void	SetTitle (const String& title, UpdateMode update = eDelayedUpdate);
		nonvirtual	Boolean	GetTitleShown () const;
		nonvirtual	void	SetTitleShown (Boolean shown, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Boolean	GetLegendShown () const;
		nonvirtual	void	SetLegendShown (Boolean shown, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	void	AddSlice (Real value, const Tile& tile, const String& title);
		
		nonvirtual	Real 	GetSliceValue (CollectionSize slice) const;
		nonvirtual	void	SetSliceValue (CollectionSize slice, Real value, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Tile	GetSliceTile (CollectionSize slice) const;
		nonvirtual	void	SetSliceTile (CollectionSize slice, const Tile& tile, UpdateMode update = eDelayedUpdate);

		nonvirtual	String	GetSliceTitle (CollectionSize slice) const;
		nonvirtual	void	SetSliceTitle (CollectionSize slice, const String& title, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Real	GetSlicePercent (CollectionSize slice) const;
		
		nonvirtual	CollectionSize	CountSlices () const;
		
		nonvirtual	Real	GetTotalValue () const;
		
	protected:
		virtual	String	GetTitle_ () const 														 = Nil;
		virtual	void	SetTitle_ (const String& title, UpdateMode update) 						 = Nil;
		virtual	Boolean	GetTitleShown_ () const 												 = Nil;
		virtual	void	SetTitleShown_ (Boolean shown, UpdateMode update) 						 = Nil;
		
		virtual	Boolean	GetLegendShown_ () const 												 = Nil;
		virtual	void	SetLegendShown_ (Boolean shown, UpdateMode update) 						 = Nil;
		
		virtual	void	AddSlice_ (Real value, const Tile& tile, const String& title) 			 = Nil;
		
		virtual	Real 	GetSliceValue_ (CollectionSize slice) const 								 = Nil;
		virtual	void	SetSliceValue_ (CollectionSize slice, Real value, UpdateMode update) 	 = Nil;
		
		virtual	Tile	GetSliceTile_ (CollectionSize slice) const 							 = Nil;
		virtual	void	SetSliceTile_ (CollectionSize slice, const Tile& tile, UpdateMode update) = Nil;

		virtual	String	GetSliceTitle_ (CollectionSize slice) const 								 = Nil;
		virtual	void	SetSliceTitle_ (CollectionSize slice, const String& title, UpdateMode update) = Nil;
		
		virtual	CollectionSize	CountSlices_ () const 											 = Nil;
		
		virtual	Real	GetTotalValue_ () const 												 = Nil;
};



/*
 * Class PieChart is designed for maximal flexibility, not maximal efficiency. As a result, it
 * uses Views for all crucial concepts (Legend, Title, Slice). This allows for flexible formatting
 * capabilities (such as setting the background color of the legend or Title, and arbitrary layouts)
 * as well as features like selectable (or even stretchable) slices. Users who cannot afford this
 * overhead can subclass AbstractPieChart for cheaper but less powerful charting capabilities.
 */

class	PieChartLegend;		// should be scoped within PieChart
class	Pie;				// should be scoped within PieChart
class	Slice;				// should be scoped within PieChart
class	PieChart : public AbstractPieChart {
	public:
		PieChart ();
		~PieChart ();
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);
		override	void	Layout ();

		override	String	GetTitle_ () const;
		override	void	SetTitle_ (const String& title, UpdateMode update);
		override	Boolean	GetTitleShown_ () const;
		override	void	SetTitleShown_ (Boolean shown, UpdateMode update);
		
		override	Boolean	GetLegendShown_ () const;
		override	void	SetLegendShown_ (Boolean shown, UpdateMode update);
		
		override	void	AddSlice_ (Real value, const Tile& tile, const String& title);
		
		override	Real 	GetSliceValue_ (CollectionSize slice) const;
		override	void	SetSliceValue_ (CollectionSize slice, Real value, UpdateMode update);
		
		override	Tile	GetSliceTile_ (CollectionSize slice) const;
		override	void	SetSliceTile_ (CollectionSize slice, const Tile& tile, UpdateMode update);

		override	String	GetSliceTitle_ (CollectionSize slice) const;
		override	void	SetSliceTitle_ (CollectionSize slice, const String& title, UpdateMode update);
		
		override	CollectionSize	CountSlices_ () const;
		
		override	Real	GetTotalValue_ () const;
		
		/* 
			We allow protected access to our subviews, to facilitate subclassing. Calling a "Set"
			method for an owned view will delete any existing instance. This interface allows, through
			shadowing, ownership of subclasses of the given views.
		*/
		nonvirtual	PieChartLegend&	GetLegend_ () const;
		virtual		void			SetLegend_ (PieChartLegend& legend, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	GraphTitle&		GetChartTitle_ () const;
		virtual		void			SetChartTitle_ (GraphTitle& title, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Pie&			GetPie_ () const;
		virtual		void			SetPie_ (Pie& pie, UpdateMode update = eDelayedUpdate);
	
	private:
		PieChartLegend*	fLegend;
		GraphTitle*		fTitle;
		Pie*			fPie;
};

/* The following classes should be scoped within PieChart except for compiler bugs */
class	PieChartLegend : public View {
	public:
		PieChartLegend (const Pie& pie);
		
		nonvirtual	const Pie&	GetPie () const;
		nonvirtual	void		SetPie (const Pie& pie);
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Draw (const Region& update);
		
		virtual	Coordinate	GetDefaultHeight () const;
	
	private:
		const Pie* fPie;
};

#if		!qRealTemplatesAvailable
	typedef	class	Slice*	SlicePtr;
	Declare (Iterator, SlicePtr);
	Declare (Collection, SlicePtr);
	Declare (AbSequence, SlicePtr);
	Declare (Array, SlicePtr);
	Declare (Sequence_Array, SlicePtr);
	Declare (Sequence, SlicePtr);
#endif

class	Pie : public View {
	public:
		Pie (PieChart& owner);
		~Pie ();

		nonvirtual	SequenceIterator(SlicePtr)*	MakeSliceIterator (SequenceDirection d = eSequenceForward) const;
		nonvirtual	CollectionSize				CountSlices () const;
		nonvirtual	Slice&						GetSlice (CollectionSize index) const;
		nonvirtual	void						AddSlice (Slice& slice);
		
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		override	void	Draw (const Region& update);

	private:
		PieChart& 							fOwner;
		Sequence(SlicePtr)	fSlices;
};

class	Slice : public View {
	public:
		Slice (Pie& owner, Real value, const Tile& tile, const String& title);
		
		override	Boolean	Opaque () const;

		nonvirtual	Real	GetValue () const;
		nonvirtual	void	SetValue (Real value, UpdateMode update = eDelayedUpdate);
		
		nonvirtual	Tile	GetTile () const;
		nonvirtual	void	SetTile (const Tile& tile, UpdateMode update = eDelayedUpdate);

		nonvirtual	String	GetTitle () const;
		nonvirtual	void	SetTitle (const String& title, UpdateMode update = eDelayedUpdate);
				
		nonvirtual	void	SetArc (const Arc& arc, UpdateMode update = eDelayedUpdate);
		nonvirtual	Arc		GetArc () const;
		
	protected:
		override	void	Draw (const Region& update);

	private:
		Pie& 	fOwner;
		Real	fValue;
		Arc		fArc;
		Tile	fTile;
		String	fTitle;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/* __PieChart__ */
