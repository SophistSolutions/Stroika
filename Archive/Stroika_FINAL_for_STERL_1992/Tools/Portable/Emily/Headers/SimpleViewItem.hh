/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__SimpleViewItem__
#define	__SimpleViewItem__

/*
 * $Header: /fuji/lewis/RCS/SimpleViewItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SimpleViewItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */

#include	"ViewItem.hh"

class	SimpleViewItem : public ViewItem {
	public:
		SimpleViewItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		override	void	WriteBuilder (class ostream& to, int tabCount);
		
		override	String	GetHeaderFileName ();
	
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
};

class	SimpleViewItemType : public ItemType {
	public:
		SimpleViewItemType ();
		
		static	SimpleViewItemType&	Get ();
		
	private:		
		static	ViewItem*	SimpleViewItemBuilder ();
		static	SimpleViewItemType*	sThis;
};

#endif	/* __SimpleViewItem__ */
