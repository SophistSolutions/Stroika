/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__ScrollerItem__
#define	__ScrollerItem__

/*
 * $Header: /fuji/lewis/RCS/ScrollerItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: ScrollerItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

#include	"GroupItem.hh"

class	Scroller;
class	ScrollerItem : public GroupItem {
	public:
		ScrollerItem ();

		override	String	GetHeaderFileName ();
		
		nonvirtual	Boolean	GetHasVerticalSBar () const;
		nonvirtual	void	SetHasVerticalSBar (Boolean hasSBar);
		nonvirtual	Boolean	GetHasHorizontalSBar () const;
		nonvirtual	void	SetHasHorizontalSBar (Boolean hasSBar);

	protected:
		ScrollerItem (ItemType& type);

		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);
		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	SetItemInfo ();
	
	private:
		Boolean	fHasVerticalSBar;
		Boolean	fHasHorizontalSBar;
};

class	ScrollerItemType : public ItemType {
	public:
		ScrollerItemType ();
		
		static	ScrollerItemType&	Get ();
		
	private:		
		static	ViewItem*	ScrollerItemBuilder ();
		static	ScrollerItemType*	sThis;
};


#endif	/* __ScrollerItem__ */
