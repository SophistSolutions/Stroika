/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__LineGraphItem__
#define	__LineGraphItem__

/*
 * $Header: /fuji/lewis/RCS/LineGraphItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: LineGraphItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
*		
 *
 *
 *
 */

#include	"ViewItem.hh"

class	LineGraph;
class	LineGraphItem : public ViewItem {
	public:
		LineGraphItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		nonvirtual	LineGraph&	GetLineGraph () const;
	
		override	String	GetHeaderFileName ();

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);

	private:
		LineGraph*	fLineGraph;
};

class	LineGraphItemType : public ItemType {
	public:
		LineGraphItemType ();
		
		static	LineGraphItemType&	Get ();
		
	private:		
		static	ViewItem*	LineGraphItemBuilder ();
		static	LineGraphItemType*	sThis;
};

#endif	/* __LineGraphItem__ */
