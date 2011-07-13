/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Grid__
#define	__Grid__

/*
 * $Header: /fuji/lewis/RCS/Grid.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *		Grid: Class for organizing a array of owned views. The user can designate either the number of rows or the number of columns,
 *		with the other dimension being automatically calculated. The user can also specify the gap between rows and the gap between 
 *		columns, and whether the gap should be filled in with gridlines.
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Grid.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  05:36:31  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.6  1992/03/24  02:28:50  lewis
 *		Made CalcDefaultSize () const.
 *
 *		Revision 1.3  1992/01/27  15:58:42  sterling
 *		changed TextViewGrid to ViewGrid
 *
 *		Revision 1.2  1992/01/27  05:03:16  sterling
 *		fixed includes
 *
 *		Revision 1.1  1992/01/27  04:24:26  sterling
 *		Initial revision
 *
 *
 *
 */

#include	"FocusItem.hh"
#include	"TextView.hh"
#include	"View.hh"

#if		!qRealTemplatesAvailable
	Declare (Iterator, Coordinate);
	Declare (Collection, Coordinate);
	Declare (AbSequence, Coordinate);
	Declare (Array, Coordinate);
	Declare (Sequence_Array, Coordinate);
	Declare (Sequence, Coordinate);
#endif	/*!qRealTemplatesAvailable*/



class	AbstractGrid : public View {
	protected:
		AbstractGrid ();
		
	public:
		~AbstractGrid ();
		
		nonvirtual	CollectionSize	GetCellCount () const;
		
		nonvirtual	CollectionSize	GetColumnCount () const;
		nonvirtual	CollectionSize	GetRowCount () const;
	
		nonvirtual	Coordinate	GetRowGap () const;
		nonvirtual	Coordinate	GetColumnGap () const;
		
		nonvirtual	Boolean		GetForceSize () const;
		
		nonvirtual	Boolean		GetGridLinesDrawn () const;

		nonvirtual	Coordinate	GetColumnWidth (CollectionSize column) const;
		nonvirtual	Coordinate	GetRowHeight (CollectionSize row) const;
		
		nonvirtual	Rect	GetGridCell (CollectionSize index) const;

		nonvirtual	AbstractTextView::Justification	GetColumnJustification (CollectionSize column);

		nonvirtual	Boolean		GetSortByColumns () const;
				
		nonvirtual	void	Invariant () const;
		

	protected:
		virtual	CollectionSize	GetCellCount_ () const 		= Nil;
		virtual	CollectionSize	GetColumnCount_ () const	= Nil;
		virtual	CollectionSize	GetRowCount_ () const		= Nil;
		virtual	Coordinate		GetRowGap_ () const			= Nil;

		virtual	Boolean		GetForceSize_ () const			= Nil;
		virtual	Coordinate	GetColumnGap_ () const			= Nil;
		virtual	Boolean		GetGridLinesDrawn_ () const 	= Nil;

		virtual	Coordinate	GetColumnWidth_ (CollectionSize column) const 	= Nil;
		virtual	Coordinate	GetRowHeight_ (CollectionSize row) const			= Nil;

		virtual	AbstractTextView::Justification	GetColumnJustification_ (CollectionSize column) = Nil;
		
		virtual	Boolean		GetSortByColumns_ () const 	= Nil;
		virtual	Coordinate	CalcColumnsWidth () const 	= Nil;
		virtual	Coordinate	CalcRowsHeight () const 	= Nil;

#if qDebug
		virtual	void	Invariant_ () const;
#endif
};

class	AbstractGrid_Portable : public AbstractGrid {
	protected:
		AbstractGrid_Portable ();
		
	public:
		~AbstractGrid_Portable ();

		nonvirtual	void	SetColumnCount (CollectionSize columnCount, UpdateMode update = eDelayedUpdate);	// Adjust row count to fit # items
		nonvirtual	void	SetRowCount (CollectionSize rowCount, UpdateMode update = eDelayedUpdate);		// Adjust col count to fit # items
		nonvirtual	void	SetRowGap (Coordinate height, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	SetColumnGap (Coordinate gap, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	SetForceSize (Boolean forceSize, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	SetGridLinesDrawn (Boolean linesDrawn, UpdateMode update = eDelayedUpdate);

		virtual	void	SetColumnsWidth (Coordinate width, UpdateMode update = eDelayedUpdate) = Nil;
		virtual	void	SetRowsHeight (Coordinate height, UpdateMode update = eDelayedUpdate) = Nil;
		virtual	void	SetColumnsJustification (AbstractTextView::Justification justification, UpdateMode update = eDelayedUpdate) = Nil;
		
		override	void	Layout ();
		override	void	Draw (const Region& update);
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		virtual	void	SetColumnCount_ (CollectionSize columnCount, UpdateMode update)	= Nil;
		virtual	void	SetRowCount_ (CollectionSize rowCount, UpdateMode update)		= Nil;
		virtual	void	SetRowGap_ (Coordinate gap, UpdateMode update)					= Nil;
		virtual	void	SetColumnGap_ (Coordinate gap, UpdateMode update)				= Nil;
		virtual	void	SetForceSize_ (Boolean forceSize, UpdateMode update)			= Nil;
		virtual	void	SetGridLinesDrawn_ (Boolean linesDrawn, UpdateMode update)		= Nil;

		override	CollectionSize	GetCellCount_ () const;
		virtual		SequenceIterator(ViewPtr)*	MakeCellIterator (SequenceDirection d = eSequenceForward) const;
};


class	AbstractFixedGrid_Portable : public AbstractGrid_Portable {
	protected:
		AbstractFixedGrid_Portable ();
		
	public:
		~AbstractFixedGrid_Portable ();

		override	void	SetColumnsWidth (Coordinate width, UpdateMode update = eDelayedUpdate);
		override	void	SetRowsHeight (Coordinate height, UpdateMode update = eDelayedUpdate);
		override	void	SetColumnsJustification (AbstractTextView::Justification justification, UpdateMode update = eDelayedUpdate);

	protected:		
		override	CollectionSize	GetColumnCount_ () const;
		override	void			SetColumnCount_ (CollectionSize columnCount, UpdateMode update);
	
		override	CollectionSize	GetRowCount_ () const;
		override	void			SetRowCount_ (CollectionSize rowCount, UpdateMode update);
		
		override	Coordinate	GetRowGap_ () const;
		override	void		SetRowGap_ (Coordinate gap, UpdateMode update);

		override	Coordinate	GetColumnGap_ () const;
		override	void		SetColumnGap_ (Coordinate gap, UpdateMode update);

		override	Boolean	GetForceSize_ () const;
		override	void	SetForceSize_ (Boolean forceSize, UpdateMode update);

		override	Boolean	GetGridLinesDrawn_ () const;
		override	void	SetGridLinesDrawn_ (Boolean linesDrawn, UpdateMode update);

		override	Coordinate	GetColumnWidth_ (CollectionSize column) const;
		override	Coordinate	GetRowHeight_ (CollectionSize row) const;

		override	AbstractTextView::Justification	GetColumnJustification_ (CollectionSize column);
		
		override	Boolean		GetSortByColumns_ () const;
		override	Coordinate	CalcColumnsWidth () const;
		override	Coordinate	CalcRowsHeight () const;
	
		virtual	void	SetColumnsWidth_ (Coordinate width, UpdateMode update);
		virtual	void	SetRowsHeight_ (Coordinate height, UpdateMode update);
		virtual	void	SetColumnsJustification_ (AbstractTextView::Justification justification, UpdateMode update);
		
	private:
		Boolean			fSortByColumns;
		CollectionSize	fRows;
		CollectionSize	fColumns;
		Coordinate		fRowGap;
		Coordinate		fColumnGap;
		Boolean			fDrawGridLines;
		Boolean			fForceSize;
		Coordinate		fColumnWidth;
		Coordinate		fRowHeight;
		AbstractTextView::Justification	fJustification;
};


class	AbstractDynamicGrid_Portable : public AbstractGrid_Portable {
	protected:
		AbstractDynamicGrid_Portable ();
	
	public:
		~AbstractDynamicGrid_Portable ();
		
		override	void	SetColumnsWidth (Coordinate width, UpdateMode update = eDelayedUpdate);
		override	void	SetRowsHeight (Coordinate height, UpdateMode update = eDelayedUpdate);
		override	void	SetColumnsJustification (AbstractTextView::Justification justification, UpdateMode update = eDelayedUpdate);

		nonvirtual	void	SetColumnWidth (CollectionSize columnCount, Coordinate width, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	SetRowHeight (CollectionSize columnCount, Coordinate height, UpdateMode update = eDelayedUpdate);
		nonvirtual	void	SetColumnJustification (CollectionSize columnCount, AbstractTextView::Justification, UpdateMode update = eDelayedUpdate);
		
	protected:
		override	CollectionSize	GetColumnCount_ () const;
		override	void			SetColumnCount_ (CollectionSize columnCount, UpdateMode update);
	
		override	CollectionSize	GetRowCount_ () const;
		override	void			SetRowCount_ (CollectionSize rowCount, UpdateMode update);
		
		override	Coordinate	GetRowGap_ () const;
		override	void		SetRowGap_ (Coordinate gap, UpdateMode update);

		override	Coordinate	GetColumnGap_ () const;
		override	void		SetColumnGap_ (Coordinate gap, UpdateMode update);

		override	Boolean	GetForceSize_ () const;
		override	void	SetForceSize_ (Boolean forceSize, UpdateMode update);

		override	Boolean	GetGridLinesDrawn_ () const;
		override	void	SetGridLinesDrawn_ (Boolean linesDrawn, UpdateMode update);

		override	Coordinate	GetColumnWidth_ (CollectionSize column) const;
		override	Coordinate	GetRowHeight_ (CollectionSize row) const;

		override	AbstractTextView::Justification	GetColumnJustification_ (CollectionSize column);
	
		override	Boolean		GetSortByColumns_ () const;
		override	Coordinate	CalcColumnsWidth () const;
		override	Coordinate	CalcRowsHeight () const;

	private:
		Boolean			fSortByColumns;
		CollectionSize	fRows;
		CollectionSize	fColumns;
		Coordinate		fRowGap;
		Coordinate		fColumnGap;
		Boolean			fForceSize;
		Boolean			fDrawGridLines;
		Coordinate		fCalculatedDimension;		// either height or width depending on value of fSortByColumns
		Sequence(Coordinate)	fDimensions;		// either height or width depending on value of fSortByColumns
		Sequence(Coordinate)	fJustifications;	// length always 1 if not fSortByColumns
};


// example use
// LGP Thursday, June 25, 1992 1:33:30 AM - IF THIS IS AN EXAMPLE - WHY IS IT HERE????
class	ViewDynamicGrid : public AbstractDynamicGrid_Portable {
	public:
		ViewDynamicGrid () :
			AbstractDynamicGrid_Portable ()
		{
		}
		
		nonvirtual	void	AddView (View* tv)
		{
			Require (GetSubViewCount () == (GetRowCount () * GetColumnCount ()));
			RequireNotNil (tv);
			
			AddSubView (tv);
			
			Ensure (GetSubViewCount () == (GetRowCount () * GetColumnCount ()));
		}
		
		nonvirtual	void	RemoveView (View* tv)
		{
			Require (GetSubViewCount () == (GetRowCount () * GetColumnCount ()));
			RequireNotNil (tv);
			
			RemoveSubView (tv);
			
			Ensure (GetSubViewCount () == (GetRowCount () * GetColumnCount ()));
		}

		nonvirtual	void	DeleteView (View* tv)
		{
			Require (GetSubViewCount () == (GetRowCount () * GetColumnCount ()));
			RequireNotNil (tv);
			
			delete tv;
			
			Ensure (GetSubViewCount () == (GetRowCount () * GetColumnCount ()));
		}
		
		nonvirtual	View*	GetView (CollectionSize column, CollectionSize row)
		{
			Require (GetSubViewCount () == (GetRowCount () * GetColumnCount ()));
			
			CollectionSize index = ((GetSortByColumns ()) 
				? (row * GetColumnCount () + column)
				: (column * GetRowCount () + row));
			
			Require (index <= GetSubViewCount ());
			
			return (GetSubViewByIndex (index));
		}
};

class	ViewFixedGrid : public AbstractFixedGrid_Portable {
	public:
		ViewFixedGrid () :
			AbstractFixedGrid_Portable ()
		{
		}
		
		nonvirtual	void	AddView (View* tv)
		{
			RequireNotNil (tv);
			
			AddSubView (tv);
		}
		
		nonvirtual	void	RemoveView (View* tv)
		{
			RequireNotNil (tv);
			
			RemoveSubView (tv);
		}

		nonvirtual	void	DeleteView (View* tv)
		{
			RequireNotNil (tv);
			
			delete tv;
		}
		
		nonvirtual	View*	GetView (CollectionSize column, CollectionSize row)
		{
			CollectionSize index = ((GetSortByColumns ()) 
				? (row * GetColumnCount () + column)
				: (column * GetRowCount () + row));
			
			Require (index <= GetSubViewCount ());
			
			return (GetSubViewByIndex (index));
		}
};


/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	void	AbstractGrid::Invariant () const
{
#if qDebug
	Invariant_ ();
#endif
}

// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/* __Grid__ */
