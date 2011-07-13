/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/PieGraphItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: PieGraphItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 */





#include	"Random.hh"
#include	"StreamUtils.hh"
#include	"Dialog.hh"

#include	"PieChart.hh"

#include	"CommandNumbers.hh"
#include	"PieGraphItem.hh"

/*
 ********************************************************************************
 ****************************** PieChartItemType ********************************
 ********************************************************************************
 */
PieChartItemType::PieChartItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildUser2, "PieChart", (ItemBuilderProc)&PieChartItemBuilder)
#else
	ItemType (eBuildUser2, "PieChart", &PieChartItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
PieChartItemType&	PieChartItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	PieChartItemType::PieChartItemBuilder ()
{
	return (new PieChartItem (Get ()));
}

PieChartItemType*	PieChartItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** PieChartItem ************************************
 ********************************************************************************
 */
PieChartItem::PieChartItem (ItemType& type) :
	ViewItem (type),
	fPieChart (Nil)
{
	fPieChart = new PieChart ();
	fPieChart->SetTitle ("Pie Chart");
	fPieChart->AddSlice (Random (1, 10), kBlackTile, "A");
	fPieChart->AddSlice (Random (1, 10), kGrayTile, "B");
	fPieChart->AddSlice (Random (1, 10), kDarkGrayTile, "C");
	fPieChart->AddSlice (Random (1, 10), kLightGrayTile, "D");

	SetOwnedView (fPieChart);
}

Boolean	PieChartItem::IsButton ()
{
	return (False);
}

Boolean	PieChartItem::IsSlider ()
{
	return (False);
}

Boolean	PieChartItem::IsText ()
{
	return (False);
}

Boolean	PieChartItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

Boolean	PieChartItem::ItemCanBeEnabled ()
{
	return (False);
}

PieChart&	PieChartItem::GetPieChart () const
{
	EnsureNotNil (fPieChart);
	return (*fPieChart);
}
	
String	PieChartItem::GetHeaderFileName ()
{
	static	const	String kFileName = "PieChart.hh";
	return (kFileName);
}

void	PieChartItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
}

void	PieChartItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
}

void	PieChartItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	PieChartItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
}
