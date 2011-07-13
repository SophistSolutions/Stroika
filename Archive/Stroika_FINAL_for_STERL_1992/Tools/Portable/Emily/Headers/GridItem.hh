/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__GridItem__
#define	__GridItem__

/*
 * $Header: /fuji/lewis/RCS/GridItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: GridItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/03/06  21:53:47  sterling
 *		motif
 *
 * 		Revision 1.1  1992/01/27  04:24:26  sterling
 * 		Initial revision
 *
 *
 */

#include	"GroupItem.hh"

class	AbstractGrid_Portable;
class	GridItem : public GroupItem {
	public:
		GridItem ();
		
		override	Point	GetMinSize () const;
		override	String	GetHeaderFileName ();
		
		nonvirtual	AbstractGrid_Portable&	GetGrid () const;

		override	Boolean	SubItemsCanChangeExtent () const;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	SetItemInfo ();
	
	private:
		AbstractGrid_Portable*	fGrid;
};

class	GridItemType : public ItemType {
	public:
		GridItemType ();
		
		static	GridItemType&	Get ();
		
	private:		
		static	ViewItem*	GridItemBuilder ();
		static	GridItemType*	sThis;
};

class	SetFixedGridInfoCommand : public Command {
	public:
		SetFixedGridInfoCommand (GridItem& item, class FixedGridInfo& info);
		
		override	void	DoIt ();		
		override	void	UnDoIt ();
	
	private:
		GridItem&		fItem;
		class Command*	fItemInfoCommand;
	
		Boolean			fNewSortByColumns;
		Boolean			fOldSortByColumns;
		CollectionSize	fNewCount;
		CollectionSize	fOldCount;
		Coordinate		fNewColumnGap;
		Coordinate		fOldColumnGap;
		Coordinate		fNewRowGap;
		Coordinate		fOldRowGap;
		Coordinate		fNewColumnWidth;
		Coordinate		fOldColumnWidth;
		Coordinate		fNewRowHeight;
		Coordinate		fOldRowHeight;
		AbstractTextView::Justification	fNewColumnJustification;
		AbstractTextView::Justification	fOldColumnJustification;
		Boolean			fNewForceSize;
		Boolean			fOldForceSize;
		Boolean			fNewGridLinesDrawn;
		Boolean			fOldGridLinesDrawn;
};

#endif	/* __GridItem__ */
