/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Board__
#define	__Board__

/*
 * $Header: /fuji/lewis/RCS/Board.hh,v 1.2 1992/09/01 17:57:21 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Board.hh,v $
 *		Revision 1.2  1992/09/01  17:57:21  sterling
 *		*** empty log message ***
 *
 *		Revision 1.6  1992/02/11  06:00:00  lewis
 *		Minor cleanups.
 *
 *
 *
 */

#include	"Config-Foundation.hh"
#include	"Debug.hh"
#include	"Sequence.hh"



typedef	UInt8	CellPosition;
const	CellPosition	kMinCellPosition	=	1;
const	CellPosition	kMaxCellPosition	=	3;
const	CellPosition	kTotalCellPositions	=	kMaxCellPosition - kMinCellPosition + 1;

class	Cell {
	public:
		Cell ();
		Cell (CellPosition row, CellPosition col);

		nonvirtual	CellPosition	GetRow () const;
		nonvirtual	CellPosition	GetCol () const;

		nonvirtual	void	FirstRow ();
		nonvirtual	void	FirstCol ();
		nonvirtual	void	NextRow ();
		nonvirtual	void	NextCol ();

	private:
		CellPosition	fRow;
		CellPosition	fCol;
};

class	istream;
class	ostream;
extern	Boolean	operator== (const Cell& lhs, const Cell& rhs);
extern	Boolean	operator!= (const Cell& lhs, const Cell& rhs);
extern	istream&	operator >> (istream& from, Cell& cell);
extern	ostream&	operator << (ostream& to, const Cell& cell);



extern	const	Cell	kMinCell;

Declare (Iterator, Cell);
Declare (Collection, Cell);
Declare (AbSequence, Cell);
Declare (Array, Cell);
Declare (Sequence_Array, Cell);
Declare (Sequence, Cell);



class	Board {
	public:
		enum CellOccupant { eEmpty = 0, eX, eO };

		Board ();

		nonvirtual	Boolean	GetClear () const;
		nonvirtual	void	SetClear ();

		nonvirtual	UInt8	GetFilled () const;

		nonvirtual	CellOccupant	GetCell (const Cell& c) const;
		nonvirtual	void			SetCell (const Cell& c,CellOccupant occupant);

		nonvirtual	Boolean	GameDone () const;
		nonvirtual	Boolean	GameDone (CellOccupant& winner) const;

	private:
		CellOccupant	fBoard [kTotalCellPositions] [kTotalCellPositions];
		UInt8			fEntryCount;			// count of non-empty entries
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Cell::Cell ():
	fRow (kMinCellPosition),
	fCol (kMinCellPosition)
	{
	}

inline	Cell::Cell (CellPosition row, CellPosition col):
	fRow (row),
	fCol (col)
	{
		Require (row >= kMinCellPosition);
		Require (row <= kMaxCellPosition);
		Require (col >= kMinCellPosition);
		Require (col <= kMaxCellPosition);
	}

inline	CellPosition	Cell::GetRow () const	{ Ensure (fRow >= kMinCellPosition); Ensure (fRow <= kMaxCellPosition); return (fRow); }
inline	CellPosition	Cell::GetCol () const	{ Ensure (fCol >= kMinCellPosition); Ensure (fCol <= kMaxCellPosition); return (fCol); }
inline	void			Cell::FirstRow ()		{ fRow = 1; }
inline	void			Cell::FirstCol ()		{ fCol = 1; }
inline	void			Cell::NextRow ()		{ Require (fRow <= kMaxCellPosition); fRow++; }
inline	void			Cell::NextCol ()		{ Require (fCol <= kMaxCellPosition); fCol++; }



inline	Board::CellOccupant	Board::GetCell (const Cell& c) const
	{
		CellPosition	row	=	c.GetRow ();
		CellPosition	col	=	c.GetCol ();
		Require (row >= 1);
		Require (row <= kMaxCellPosition);
		Require (col >= 1);
		Require (col <= kMaxCellPosition);
		return (fBoard [row-1][col-1]);
	}

inline	void	Board::SetCell (const Cell& c, CellOccupant occupant)
	{
		CellPosition	row	=	c.GetRow ();
		CellPosition	col	=	c.GetCol ();
		Require (row >= 1);
		Require (row <= kMaxCellPosition);
		Require (col >= 1);
		Require (col <= kMaxCellPosition);
		Assert (fEntryCount >= 0);
		Assert (fEntryCount <= kMaxCellPosition*kMaxCellPosition);
		if (fBoard [row-1][col-1] != eEmpty) {
			fEntryCount--;
		}
		if (occupant != eEmpty) {
			fEntryCount++;
		}
		fBoard [row-1][col-1] = occupant;
	}

// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/*__Board__*/
