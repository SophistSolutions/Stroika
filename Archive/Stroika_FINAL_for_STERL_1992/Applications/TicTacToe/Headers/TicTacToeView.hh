/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__TicTacToeView__
#define	__TicTacToeView__

/*
 * $Header: /fuji/lewis/RCS/TicTacToeView.hh,v 1.2 1992/09/01 17:57:21 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: TicTacToeView.hh,v $
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

#include	"KeyHandler.hh"
#include	"Menu.hh"
#include	"View.hh"

#include	"Board.hh"





class	TicTacToeView : public View, public KeyHandler, public MenuCommandHandler {
	public:
		TicTacToeView ();

		override	void	Draw (const Region& update);

		override	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		override	Boolean	HandleKeyStroke (const KeyStroke& keyStroke);

		nonvirtual	void	ClearBoard (UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Board::CellOccupant	GetCell (const Cell& c) const;
		nonvirtual	void				SetCell (const Cell& c, Board::CellOccupant occupant,
												 UpdateMode updateMode = eDelayedUpdate);

	protected:
		override	void	Layout ();

		nonvirtual	Boolean	PointToCell (const Point& where, Cell& c);
		nonvirtual	Rect	GetItemRect (const Cell& c);


		virtual		void	DrawLines ();
		nonvirtual	void	DrawCells ();
		virtual		void	DrawCell (const Cell& c);

	private:
		nonvirtual	void	AnnounceWinner (Board::CellOccupant winner, Board::CellOccupant player);
		
		Point			fLineSize;		// height of horiz / width of vert
		Point			fCellSize;
		Board			fBoard;
		Board::CellOccupant	fNextOccupant;
		Boolean			fSoundOn;
		Boolean			fComputerOpponent;
};




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

#endif	/*__TicTacToeView__*/
