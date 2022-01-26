/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PalletMenu.cc,v 1.5 1992/09/08 16:00:29 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PalletMenu.cc,v $
 *		Revision 1.5  1992/09/08  16:00:29  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:58:05  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/03  04:06:15  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.14  1992/02/28  15:53:48  lewis
 *		Renamed from PalletView to PalletMenu.
 *
 *		Revision 1.13  1992/02/14  03:41:44  lewis
 *		Lots of changes to keep in sync with reworking of base class Menu, genrally to be more portable.
 *
 *		Revision 1.12  1992/02/12  07:26:43  lewis
 *		Subclass from Menu, not CustomMenu.
 *
 *		Revision 1.11  1992/02/11  03:03:51  lewis
 *		Comment out call to SelectItem on X, til we fully support in base class Menu.
 *
 *		Revision 1.10  1992/02/11  01:31:28  lewis
 *		Updated for changes to Menu stuff (1 class, no more CustomMenuView).
 *
 *
 */

#include	"Shape.hh"

#include	"MenuItem.hh"

#include	"PalletMenu.hh"	







/*
 ********************************************************************************
 ************************************** PalletMenu ******************************
 ********************************************************************************
 */
PalletMenu::PalletMenu ():
	Menu (),
	fSelectedItem (Nil)
{
}

void	PalletMenu::SelectItem (MenuItem* item, Panel::UpdateMode updateMode)
{
	if (item == Nil) {
		// pallets always have some item selected
		item = GetSelectedItem ();
	}

	fSelectedItem = item;
	Menu::SelectItem (item, updateMode);
}

MenuItem*	PalletMenu::GetSelectedItem () const
{
	return (fSelectedItem);
}

void	PalletMenu::SetSelectedItem (MenuItem* b, Panel::UpdateMode updateMode)
{
	SelectItem (b, updateMode);	
}






/*
 ********************************************************************************
 ******************************** GridedPalletMenu ******************************
 ********************************************************************************
 */



CollectionSize	GridedPalletMenu::kMagicCount	=	kBadSequenceIndex;

// parameterize this...
const Coordinate kGridThickness	=	1;

GridedPalletMenu::GridedPalletMenu (Boolean useGrid, CollectionSize nColumns):
	PalletMenu (),
	fUseGrid (useGrid),
	fColCount (nColumns),
	fRowCount (kMagicCount),
	fCellSize (kZeroPoint)
{
}

Point	GridedPalletMenu::CalcDefaultSize_ (const Point& /*defaultSize*/) const
{
	Point	itemSize	 = kZeroPoint;

	ForEach (MenuItemPtr, it, MakeMenuItemIterator ()) {
		register MenuItem* button	=	it.Current ();
		AssertNotNil (button);
		itemSize = Max (itemSize, button->CalcDefaultSize (button->GetSize ()));
	}

	CollectionSize	rowCount	=	GetRowCount ();
	CollectionSize	colCount	=	GetColumnCount ();
	Point	size		=	Point (itemSize.GetV () * rowCount, itemSize.GetH () * colCount);

	if (GetUseGrid ()) {
		/*
		 * Lines only used as seperators, not as borders, so need one for each cell but botright.
		 */
		size += kGridThickness * Point (rowCount-1, colCount-1);
	}
	return (size);
}

void	GridedPalletMenu::Layout ()
{
	CollectionSize	rowCount	=	GetRowCount ();
	CollectionSize	colCount	=	GetColumnCount ();

	/*
	 * Compute the item size - max of all items...
	 */
	fCellSize	 = kZeroPoint;
	ForEach (MenuItemPtr, it, MakeMenuItemIterator ()) {
		register MenuItem* button	=	it.Current ();
		AssertNotNil (button);
		fCellSize = Max (fCellSize, button->CalcDefaultSize (button->GetSize ()));
	}


	Point	cellSize	=	GetCellSize ();		// call virtual method rather than peek at fCellSize...
	/*
	 * Set each item to the cell size.
	 */
	{
		ForEach (MenuItemPtr, it, MakeMenuItemIterator ()) {
			register MenuItem* button	=	it.Current ();
			AssertNotNil (button);
			button->SetSize (cellSize);
		}
	}

	/*
	 * Now place each item in its proper place, going thru rows/cols (which first depends
	 * on which user specified).  Take into account grid lines.
	 */
	{
		CollectionSize	row	=	1;
		CollectionSize	col	=	1;
		Point	itemLoc	=	kZeroPoint;
		ForEach (MenuItemPtr, it, MakeMenuItemIterator ()) {
			register MenuItem* button	=	it.Current ();
			AssertNotNil (button);
			button->SetOrigin (itemLoc);

			/*
			 * Calc position of next item
			 */
			if (fColCount == kMagicCount) {
				/*
				 * Then we have a fixed # of rows, and we index over them as our primary index.
				 */
				Assert (fRowCount != kMagicCount);
AssertNotReached (); // symetric with below, but NYI
			}
			else {
				/*
				 * Then we have a fixed # of cols, and we index over them as our primary index.
				 */
				Assert (fRowCount == kMagicCount);
				if (col == colCount) {
					/*
					 * Wrap around
					 */
					itemLoc = Point (itemLoc.GetV (), 0);		// back to first col
					itemLoc += Point (cellSize.GetV (), 0);		// start new row
					if (GetUseGrid ()) {
						itemLoc += Point (kGridThickness, 0);
					}
					col = 1;
					row++;
				}
				else {
					/*
					 * next in this row.
					 */
					itemLoc += Point (0, cellSize.GetH ());
					if (GetUseGrid ()) {
						itemLoc += Point (0, kGridThickness);
					}
					col++;
				}
			}
		}
	}
	View::Layout ();			// Dont call inherited layout since Menu lays out items differently that we wish...
}

void	GridedPalletMenu::Draw (const Region& update)
{
	/*
	 * Draw the grid.  Items are subviews, and draw themselves.
	 */
	if (GetUseGrid ()) {
		Point			cellSize	=	GetCellSize ();
		CollectionSize	rowCount	=	GetRowCount ();
		CollectionSize	colCount	=	GetColumnCount ();

		for (CollectionSize row = 2; row <= rowCount; row++) {
			Coordinate	vPos	=	(row - 1) * (cellSize.GetV () + kGridThickness) - kGridThickness;
			OutLine (Line (Point (vPos, 0), Point (vPos, GetSize ().GetH ())));
		}
		for (CollectionSize col = 2; col <= colCount; col++) {
			Coordinate	hPos	=	(col - 1) * (cellSize.GetH () + kGridThickness) - kGridThickness;
			OutLine (Line (Point (0, hPos), Point (GetSize ().GetV (), hPos)));
		}
	}
	PalletMenu::Draw (update);
}

Boolean	GridedPalletMenu::GetUseGrid () const
{
	return (fUseGrid);
}

void	GridedPalletMenu::SetUseGrid (Boolean useGrid)
{
	if (fUseGrid != useGrid) {
		fUseGrid = useGrid;
		InvalidateLayout ();
	}
}

Point	GridedPalletMenu::GetCellSize () const
{
	return (fCellSize);
}

CollectionSize	GridedPalletMenu::GetColumnCount () const
{
	if (fColCount == kMagicCount) {
		/*
		 * User specified row count, and we infer column count from total item count.
		 */
		CollectionSize	count	=	GetLength ();
		return ((count + GetRowCount () - 1) / GetRowCount ());
	}
	else {
		Ensure (fColCount >= 1);
		return (fColCount);
	}
}

void	GridedPalletMenu::SetColumnCount (CollectionSize columnCount)
{
	Require (columnCount >= 1);
	if (fColCount != columnCount) {
		fRowCount = kMagicCount;
		fColCount = columnCount;
		InvalidateLayout ();
	}
}

CollectionSize	GridedPalletMenu::GetRowCount () const
{
	if (fRowCount == kMagicCount) {
		/*
		 * User specified row count, and we infer column count from total item count.
		 */
		CollectionSize	count	=	GetLength ();
		return ((count + GetColumnCount () - 1) / GetColumnCount ());
	}
	else {
		Ensure (fRowCount >= 1);
		return (fRowCount);
	}
}

void	GridedPalletMenu::SetRowCount (CollectionSize rowCount)
{
	Require (rowCount >= 1);
	if (fRowCount != rowCount) {
		fColCount = kMagicCount;
		fRowCount = rowCount;
		InvalidateLayout ();
	}
}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

