/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Board.cc,v 1.2 1992/09/01 17:58:36 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Board.cc,v $
 *		Revision 1.2  1992/09/01  17:58:36  sterling
 *		*** empty log message ***
 *
 *
 *
 */




#include	<string.h>

#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Board.hh"




extern	const	Cell	kMinCell	=	Cell (kMinCellPosition, kMinCellPosition);




/*
 ********************************************************************************
 ***************************************** Cell *********************************
 ********************************************************************************
 */

Boolean	operator== (const Cell& lhs, const Cell& rhs)
{
	return (Boolean ((lhs.GetRow () == rhs.GetRow ()) and (lhs.GetCol () == rhs.GetCol ())));
}

Boolean	operator!= (const Cell& lhs, const Cell& rhs)
{
	return (Boolean ((lhs.GetRow () != rhs.GetRow ()) or (lhs.GetCol () != rhs.GetCol ())));
}

Implement (Iterator, Cell);
Implement (Collection, Cell);
Implement (AbSequence, Cell);
Implement (Array, Cell);
Implement (Sequence_Array, Cell);
Implement (Sequence, Cell);





/*
 ********************************************************************************
 *************************************** Board **********************************
 ********************************************************************************
 */

Board::Board ():
//	fBoard (),
	fEntryCount (0)
{
	SetClear ();
}

void	Board::SetClear ()
{
	Assert (eEmpty == 0);
	memset (fBoard, 0, sizeof (fBoard));
	fEntryCount = 0;
#if		qDebug
	for (CellPosition r = kMinCellPosition; r <= kMaxCellPosition; r++) {
		for (CellPosition c = kMinCellPosition; c <= kMaxCellPosition; c++) {
			Assert (GetCell (Cell (r, c)) == eEmpty);
		}
	}
#endif	/*qDebug*/
}

Boolean	Board::GetClear () const
{
	return (Boolean (fEntryCount == 0));
}

UInt8	Board::GetFilled () const
{
	return (fEntryCount);
}


Boolean	Board::GameDone () const
{
	CellOccupant winner;
	return (GameDone (winner));
}

Boolean	Board::GameDone (CellOccupant& winner) const
{
	winner = eEmpty;			// initialize in case no winner...



if (fEntryCount <= 2) {
	return (False);
}

	/*
	 * Check diagonals: left to right, top-down, then right to left, top-down
	 */
	CellOccupant	hopefulWinner	=	GetCell (kMinCell);
	if (hopefulWinner != eEmpty) {
		for (CellPosition r = kMinCellPosition; r <= kMaxCellPosition; r++) {
			if (GetCell (Cell (r, r)) != hopefulWinner) {
				break;
			}
		}
		Assert (hopefulWinner != eEmpty);
		if (r > kMaxCellPosition) {
			winner = hopefulWinner;
			return (True);
		}
	}
	hopefulWinner	=	GetCell (Cell (kMinCellPosition, kMaxCellPosition));
	if (hopefulWinner != eEmpty) {
		for (CellPosition r = kMinCellPosition; r <= kMaxCellPosition; r++) {
			if (GetCell (Cell (r, kMaxCellPosition - r + kMinCellPosition)) != hopefulWinner) {
				break;
			}
		}
		Assert (hopefulWinner != eEmpty);
		if (r > kMaxCellPosition) {
			winner = hopefulWinner;
			return (True);
		}
	}


	/*
	 * Now check for rows, and columns.
	 */
	for (CellPosition r = kMinCellPosition; r <= kMaxCellPosition; r++) {
		hopefulWinner	=	GetCell (Cell (r, kMinCellPosition));
		if (hopefulWinner == eEmpty) {
			continue;
		}
		for (CellPosition c = kMinCellPosition+1; c <= kMaxCellPosition; c++) {	// is c++ valid c++?
			if (hopefulWinner != GetCell (Cell (r, c))) {
				break;
			}
		}
		if (c <= kMaxCellPosition) {
			continue;
		}

		/* If we got here, we win!!! */
		Assert (c > kMaxCellPosition);
		Assert (hopefulWinner != eEmpty);
		winner = hopefulWinner;
		return (True);
	}
	for (CellPosition c = kMinCellPosition; c <= kMaxCellPosition; c++) {		// is c++ valid c++?
		hopefulWinner	=	GetCell (Cell (kMinCellPosition, c));
		if (hopefulWinner == eEmpty) {
			continue;
		}
		for (CellPosition r = kMinCellPosition+1; r <= kMaxCellPosition; r++) {
			if (hopefulWinner != GetCell (Cell (r, c))) {
				break;
			}
		}
		if (r <= kMaxCellPosition) {
			continue;
		}

		/* If we got here, we win!!! */
		Assert (r > kMaxCellPosition);
		Assert (hopefulWinner != eEmpty);
		winner = hopefulWinner;
		return (True);
	}

	/*
	 * Check if the board is full
	 */
	for (r = kMinCellPosition; r <= kMaxCellPosition; r++) {
		for (CellPosition c = kMinCellPosition; c <= kMaxCellPosition; c++) {
			if (GetCell (Cell (r, c)) == Board::eEmpty) {
				/*
				 * If we didn't win on the diagonals, or rows or columns, and we
				 * are not a full board, then we loose!
				 */
				return (False);
			}
		}
	}

	Ensure (winner == eEmpty);
	return (True);					// board is full
}



/*
 ********************************************************************************
 *********************************** iostream support ***************************
 ********************************************************************************
 */

class istream&	operator>> (class istream& in, Cell& cell)
{
	char	ch;
	in >> ch;
	if (ch != lparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	int vc;
	in >> vc;
	int hc;
	in >> hc;
	in >> ch;
	if (ch != rparen) {					// check for surrounding parens
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	Assert (in);

	if ((vc < kMinCellPosition) or (vc >= kMaxCellPosition)) {
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}
	if ((hc < kMinCellPosition) or (hc >= kMaxCellPosition)) {
		in.clear (ios::badbit | in.rdstate ());
		return (in);
	}

	cell = Cell (vc, hc);
	return (in);
}

ostream&	operator<< (class ostream& out, const Cell& cell)
{
	out << lparen << int (cell.GetRow ()) << ' ' << int (cell.GetCol ()) << rparen;
	return (out);
}



// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

