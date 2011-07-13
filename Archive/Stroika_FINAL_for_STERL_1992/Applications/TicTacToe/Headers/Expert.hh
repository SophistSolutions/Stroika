/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Expert__
#define	__Expert__

/*
 * $Header: /fuji/lewis/RCS/Expert.hh,v 1.2 1992/09/01 17:57:21 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Expert.hh,v $
 *		Revision 1.2  1992/09/01  17:57:21  sterling
 *		*** empty log message ***
 *
 *		Revision 1.4  1992/02/11  06:00:00  lewis
 *		Minor cleanups.
 *
 *
 *
 *
 */

#include	"Board.hh"



typedef	int	PositionValue;			// larger means better for our side...
const	kMinPositionValue	=	-10;
const	kPoorPositionValue	=	-5;
const	kEvenPositionValue	=	0;
const	kGoodPositionValue	=	5;
const	kMaxPositionValue	=	10;
extern	PositionValue	EvaluatePosition (const Board& board, Board::CellOccupant us, UInt32 maxSearchDepth = kMaxUInt32);


class	LegalMoveIterator : public Iterator (Cell) {
	public:
		LegalMoveIterator (const Board& board, Board::CellOccupant whoseTurn);

		override	Boolean	Done () const;
		override	void	Next ();
		override	Cell	Current () const;

	private:
		Board	fBoard;
		Boolean	fNextPosGood;
		Cell	fLastPosShown;

		nonvirtual	void	FindNextPos (Boolean starting);
};


// we require that some move be available, and, in fact, that the game not be done...
extern	Cell	FindBestMove (const Board& whichBoard, Board::CellOccupant whoseTurn, UInt32 maxSearchDepth = 3);




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

#endif	/*__Expert__*/

