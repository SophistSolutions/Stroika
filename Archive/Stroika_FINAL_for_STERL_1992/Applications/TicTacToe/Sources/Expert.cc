/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Expert.cc,v 1.2 1992/09/01 17:58:36 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Expert.cc,v $
 *		Revision 1.2  1992/09/01  17:58:36  sterling
 *		*** empty log message ***
 *
 *		Revision 1.7  92/04/15  14:40:37  14:40:37  lewis (Lewis Pringle)
 *		Use new Random () function from Math.hh
 *		
 *		Revision 1.5  1992/02/11  06:00:00  lewis
 *		Minor cleanups.
 *
 *
 */




#include	"Debug.hh"
#include	"Random.hh"

#include	"Expert.hh"





static	void	FindBestMove (const Board& whichBoard, Board::CellOccupant whoseTurn, UInt32 maxSearchDepth, Cell& bestMove, PositionValue& howGood);
static	void	FindBestMoveNoPick (const Board& whichBoard, Board::CellOccupant whoseTurn, UInt32 maxSearchDepth, Cell& bestMove, PositionValue& howGood);


/*
 ********************************************************************************
 ******************************** EvaluatePosition ******************************
 ********************************************************************************
 */
int	CountPotentialWins (const Board& board, Board::CellOccupant us)
{
	int	 wins = 0;
	for (LegalMoveIterator it (board, us); not it.Done (); it.Next ()) {
		Board	b		=	board;
		Cell	current	=	it.Current ();

		Assert (b.GetCell (current) == Board::eEmpty);
		b.SetCell (current, us);

		PositionValue	newPosValue	=	EvaluatePosition (b, us, 0);
		if (newPosValue == kMaxPositionValue) {
			wins++;
		}
	}
	return (wins);
}

/*
 ********************************************************************************
 ******************************** EvaluatePosition ******************************
 ********************************************************************************
 */

PositionValue	EvaluatePosition (const Board& board, Board::CellOccupant us, UInt32 maxSearchDepth)
{
	Board::CellOccupant	opponent	=	(us == Board::eX)? Board::eO: Board::eX;

	Assert (us != Board::eEmpty);
	Assert (opponent != Board::eEmpty);

	/*
	 * If the position is one where we win, thats great, otherwise find opponents best move and
	 * minimize that.
	 */
	Board::CellOccupant	winner;
	if (board.GameDone (winner)) {
		if (winner == us) {
			return (kMaxPositionValue);
		}
		else if (winner == Board::eEmpty) {
			return (kEvenPositionValue);
		}
		else {
			return (kMinPositionValue);
		}
	}
	else if (maxSearchDepth == 0) {
		return (kEvenPositionValue);
	
	}
	else {
		/*
		 * If the board is not done, then find the opponents best move, and return the negative
		 * of that (ie bad for him is good for us, and vice-versa).
		 */
		Cell			whichCell;
		PositionValue	howGood;

		FindBestMoveNoPick (board, opponent, maxSearchDepth, whichCell, howGood);
		if (howGood >= (kMaxPositionValue-1)) {
			return (kMinPositionValue + 1);	// we would lose!!! (a little better than immediate death)
		}
		else if (howGood <= (kMinPositionValue+1)) {
			return (kMaxPositionValue-1);	// we win!!! (gratification delayed worth slightly less than immediate)
		}
		else {
			// best opponent can achieve is a tie, see if we can do better
			int potentialWins = CountPotentialWins (board, us);
			if (potentialWins > 0) {
				return (kGoodPositionValue);
			}
			else {
				return (kEvenPositionValue);
			}
		}
	}
	AssertNotReached (); return (kEvenPositionValue);
}



/*
 ********************************************************************************
 ****************************** LegalMoveIterator *******************************
 ********************************************************************************
 */

LegalMoveIterator::LegalMoveIterator (const Board& board, Board::CellOccupant /*whoseTurn*/):
	fBoard (board),
	fNextPosGood (False),
	fLastPosShown (kMinCell)
{
	FindNextPos (True);
}

Boolean	LegalMoveIterator::Done () const
{
	return (not fNextPosGood);
}

void	LegalMoveIterator::Next ()
{
	FindNextPos (False);
}

Cell	LegalMoveIterator::Current () const
{
	Require (fNextPosGood);
	Ensure (fBoard.GetCell (fLastPosShown) == Board::eEmpty);
	return (fLastPosShown);
}

void	LegalMoveIterator::FindNextPos (Boolean starting)
{
	if (starting and (fBoard.GetCell (fLastPosShown) == Board::eEmpty)) {
		fNextPosGood = True;
		return;
	}

Again:
	if (fLastPosShown.GetCol () >= kMaxCellPosition) {
		if (fLastPosShown.GetRow () >= kMaxCellPosition) {
			fNextPosGood = False;
			return;
		}
		fLastPosShown = Cell (fLastPosShown.GetRow () + 1, kMinCellPosition);
	}
	else {
		fLastPosShown = Cell (fLastPosShown.GetRow (), fLastPosShown.GetCol () + 1);
	}
	if (fBoard.GetCell (fLastPosShown) == Board::eEmpty) {
		fNextPosGood = True;
		return;
	}
	goto Again;
}





/*
 ********************************************************************************
 ********************************* FindBestMove *********************************
 ********************************************************************************
 */

Cell	FindBestMove (const Board& whichBoard, Board::CellOccupant whoseTurn, UInt32 maxSearchDepth)
{
	Cell			whichCell;
	PositionValue	howGood;

	FindBestMove (whichBoard, whoseTurn, maxSearchDepth, whichCell, howGood);
	Ensure (whichBoard.GetCell (whichCell) == Board::eEmpty);
	return (whichCell);
}

static	void	FindBestMove (const Board& whichBoard, Board::CellOccupant whoseTurn, UInt32 maxSearchDepth, Cell& bestMove, PositionValue& howGood)
{
	/*
	 * This alogorithm evaluates every available move, and picks randomly from the best available.
	 */

	howGood		=	kMinPositionValue;
	bestMove	=	kMinCell;
	
	Sequence_Array(Cell)	bestCells;
	
	static	Boolean	firstRun = True;
	if (firstRun) {
		firstRun = False;
	}
	
	for (LegalMoveIterator it (whichBoard, whoseTurn); not it.Done (); it.Next ()) {
		Board	b		=	whichBoard;
		Cell	current	=	it.Current ();
		Assert (b.GetCell (current) == Board::eEmpty);
		b.SetCell (current, whoseTurn);

		PositionValue	newPosValue	=	EvaluatePosition (b, whoseTurn, maxSearchDepth-1);
		if ((newPosValue >= howGood) or (bestCells.GetLength () == 0)) {
			Assert (b.GetCell (current) == whoseTurn);
			Assert (whichBoard.GetCell (current) == Board::eEmpty);
			if (newPosValue > howGood) {
				bestCells.RemoveAll ();
			}
			howGood = newPosValue;
			bestCells.Append (current);
		}
	}
	
	Assert (bestCells.GetLength () > 0);
	CollectionSize myRandom = Random (1, bestCells.GetLength ());
	Assert ((myRandom >= 1) and (myRandom <= bestCells.GetLength ()));
	bestMove = bestCells[myRandom];
	Ensure (whichBoard.GetCell (bestMove) == Board::eEmpty);
}

static	void	FindBestMoveNoPick (const Board& whichBoard, Board::CellOccupant whoseTurn, UInt32 maxSearchDepth, Cell& bestMove, PositionValue& howGood)
{
	/*
	 * This alogorithm evaluates every available move, and picks last good one.
	 * Cheaper than above, but makes for dull play
	 * Used internally for tree evaluation, where exciting play is unimportant.
	 */

	howGood		=	kMinPositionValue;
	bestMove	=	kMinCell;
	int	bestCount = 0;	
	
	for (LegalMoveIterator it (whichBoard, whoseTurn); not it.Done (); it.Next ()) {
		Board	b		=	whichBoard;
		Cell	current	=	it.Current ();
		Assert (b.GetCell (current) == Board::eEmpty);
		b.SetCell (current, whoseTurn);

		PositionValue	newPosValue	=	EvaluatePosition (b, whoseTurn, maxSearchDepth-1);
		if ((newPosValue >= howGood) or (bestCount == 0)) {
			Assert (b.GetCell (current) == whoseTurn);
			Assert (whichBoard.GetCell (current) == Board::eEmpty);
			if (newPosValue > howGood) {
				bestCount = 0;
			}
			howGood = newPosValue;
			bestMove = current;
			bestCount++;
		}
	}
	
	Assert (bestCount > 0);
	Ensure (whichBoard.GetCell (bestMove) == Board::eEmpty);
}




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

