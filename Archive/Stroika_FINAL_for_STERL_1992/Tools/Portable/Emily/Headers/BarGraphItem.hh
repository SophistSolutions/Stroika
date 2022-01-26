/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__BarGraphItem__
#define	__BarGraphItem__

/*
 * $Header: /fuji/lewis/RCS/BarGraphItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: BarGraphItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */

#include	"ViewItem.hh"

class	BarGraph;
class	BarGraphItem : public ViewItem {
	public:
		BarGraphItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		nonvirtual	BarGraph&	GetBarGraph () const;
	
		override	String	GetHeaderFileName ();

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);

	private:
		BarGraph*	fBarGraph;
};

class	BarGraphItemType : public ItemType {
	public:
		BarGraphItemType ();
		
		static	BarGraphItemType&	Get ();
		
	private:		
		static	ViewItem*	BarGraphItemBuilder ();
		static	BarGraphItemType*	sThis;
};

#endif	/* __BarGraphItem__ */
