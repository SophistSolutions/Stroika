/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__PalletMenu__
#define	__PalletMenu__

/*
 * $Header: /fuji/lewis/RCS/PalletMenu.hh,v 1.3 1992/09/01 15:54:46 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *
 * Changes:
 *	$Log: PalletMenu.hh,v $
 *		Revision 1.3  1992/09/01  15:54:46  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.12  1992/02/28  15:52:36  lewis
 *		Renamed from PalletView to PalletMenu.
 *
 *		Revision 1.11  1992/02/14  03:33:52  lewis
 *		Made PalletView be a virtual subclass of Menu only if qMPW_VIRTUAL_BASE_CLASS_BUG not true (which alas it is on mac-
 *		probably broken on unix too since bother are 2.1 based compilers. Also, add fCellSize field, rather than
 *		using first view to keep size (cuz of how
 *		we now only set sizes in layout - not calc default size). And made GetCellSize () virtual so subclasses
 *		can use some other criteria than what we use in CalcDefaultSize.
 *
 *		Revision 1.10  1992/02/12  07:22:35  lewis
 *		Make PalletView be a virtual Menu instead of a CustomMenu. (btw, should probably rename this PalletMenu).
 *
 *
 */

#include	"Menu.hh"



class PalletMenu : public
#if		!qMPW_VIRTUAL_BASE_CLASS_BUG
		virtual
#endif
		Menu {
	public:
		PalletMenu ();

		nonvirtual	MenuItem*	GetSelectedItem () const;
		virtual		void		SetSelectedItem (MenuItem* view, UpdateMode updateMode = eDelayedUpdate);

	protected:	
		override	void	SelectItem (MenuItem* item, UpdateMode updateMode);

	private:
		MenuItem*	fSelectedItem;
};




/*
 * This class assumes the items are of homogeneous size, and lines them up into
 * columns.  It allows for an optional grid line to seperate items.
 */
class	GridedPalletMenu : public PalletMenu {
	public:
		GridedPalletMenu (Boolean useGrid, CollectionSize nColumns);

		nonvirtual	Boolean	GetUseGrid () const;
		nonvirtual	void	SetUseGrid (Boolean useGrid);

		virtual	Point	GetCellSize () const;			// we assure homogeneity

		/*
		 * Since you can specify the total # of items, the row and column count, the
		 * system is overspecified.  Therefore, we ignore either the row or column count
		 * if it was not given us last.  You should simply use one or the other, and the
		 * results will be intuitive.
		 */
		nonvirtual	CollectionSize	GetColumnCount () const;
		nonvirtual	void			SetColumnCount (CollectionSize columnCount);	// Adjust row count to fit # items

		nonvirtual	CollectionSize	GetRowCount () const;
		nonvirtual	void			SetRowCount (CollectionSize rowCount);			// Adjust col count to fit # items
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	Layout ();
		override	void	Draw (const Region& update);

	private:
		static	CollectionSize	kMagicCount;
		Boolean			fUseGrid;
		CollectionSize	fColCount;
		CollectionSize	fRowCount;
		Point			fCellSize;
};


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/* __PalletMenu__ */
