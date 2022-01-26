/* Copyright(c) Sophist Solutions Inc. 1991-1992.  All rights reserved */
#ifndef	__PieChartItem__
#define	__PieChartItem__

/*
 * $Header: /fuji/lewis/RCS/PieGraphItem.hh,v 1.3 1992/09/01 16:25:27 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: PieGraphItem.hh,v $
 *		Revision 1.3  1992/09/01  16:25:27  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */

#include	"ViewItem.hh"

class	PieChart;
class	PieChartItem : public ViewItem {
	public:
		PieChartItem (ItemType& type);

		override	Boolean	IsButton ();
		override	Boolean	IsSlider ();
		override	Boolean	IsText ();
		override	Boolean	IsFocusItem (CommandNumber gui);
		override	Boolean	ItemCanBeEnabled ();
		
		nonvirtual	PieChart&	GetPieChart () const;
	
		override	String	GetHeaderFileName ();

	protected:
		override	void	DoRead_ (class istream& from);
		override	void	DoWrite_ (class ostream& to, int tabCount) const;

		override	void	WriteBuilder (class ostream& to, int tabCount);
		override	void	WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui);

	private:
		PieChart*	fPieChart;
};

class	PieChartItemType : public ItemType {
	public:
		PieChartItemType ();
		
		static	PieChartItemType&	Get ();
		
	private:		
		static	ViewItem*	PieChartItemBuilder ();
		static	PieChartItemType*	sThis;
};

#endif	/* __PieChartItem__ */
