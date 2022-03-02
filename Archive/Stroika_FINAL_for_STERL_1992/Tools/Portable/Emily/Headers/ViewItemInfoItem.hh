/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ViewItemInfoItem__
#define	__ViewItemInfoItem__

/*
 * $Header: /fuji/lewis/RCS/ViewItemInfoItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ViewItemInfoItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

#include	"GroupItem.hh"

class	ViewItemInfo;
class	ViewItemInfoItem : public GroupItem {
	public:
		ViewItemInfoItem ();
		
		override	Point	GetMinSize () const;
		override	String	GetHeaderFileName ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	String	GetFocusItemFieldName () const;

	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
};

class	ViewItemInfoItemType : public ItemType {
	public:
		ViewItemInfoItemType ();
		
		static	ViewItemInfoItemType&	Get ();
		
	private:		
		static	ViewItem*	ViewItemInfoItemBuilder ();
		static	ViewItemInfoItemType*	sThis;
};


#endif	/* __ViewItemInfoItem__ */
