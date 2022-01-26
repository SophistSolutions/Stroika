/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__SeparatorItem__
#define	__SeparatorItem__

/*
 * $Header: /fuji/lewis/RCS/SeparatorItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: SeparatorItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  92/04/30  14:15:19  14:15:19  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 */

#include	"ViewItem.hh"

class	Separator;
class	SeparatorItem : public ViewItem {
	public:
		SeparatorItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		override	void	WriteBuilder (class ostream& to, int tabCount);
		
		override	String	GetHeaderFileName ();

		override	Point	GetMinSize () const;
		override	void	Layout ();
	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;

	private:
		Separator*	fSeparator;
};

class	SeparatorItemType : public ItemType {
	public:
		SeparatorItemType ();
		
		static	SeparatorItemType&	Get ();
		
	private:		
		static	ViewItem*	SeparatorItemBuilder ();
		static	SeparatorItemType*	sThis;
};

#endif	/* __SeparatorItem__ */
