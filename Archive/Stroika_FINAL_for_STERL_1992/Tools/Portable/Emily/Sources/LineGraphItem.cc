/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/LineGraphItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LineGraphItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		
 *
 *
 */





#include	"Random.hh"
#include	"Shape.hh"

#include	"StreamUtils.hh"
#include	"Dialog.hh"

#include	"LineGraph.hh"

#include	"CommandNumbers.hh"
#include	"LineGraphItem.hh"

/*
 ********************************************************************************
 ****************************** LineGraphItemType ********************************
 ********************************************************************************
 */
LineGraphItemType::LineGraphItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildUser2, "LineGraph", (ItemBuilderProc)&LineGraphItemBuilder)
#else
	ItemType (eBuildUser2, "LineGraph", &LineGraphItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
LineGraphItemType&	LineGraphItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	LineGraphItemType::LineGraphItemBuilder ()
{
	return (new LineGraphItem (Get ()));
}

LineGraphItemType*	LineGraphItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** LineGraphItem ************************************
 ********************************************************************************
 */
LineGraphItem::LineGraphItem (ItemType& type) :
	ViewItem (type),
	fLineGraph (Nil)
{
	fLineGraph = new LineGraph ();
	fLineGraph->SetTitle ("Line Graph");
	fLineGraph->GetXAxis ().SetMin (1, eNoUpdate);
	fLineGraph->GetXAxis ().SetMax (5, eNoUpdate);
	fLineGraph->GetXAxis ().SetTicks (4, eNoUpdate);
	fLineGraph->GetYAxis ().SetTicks (5, eNoUpdate);
	
	LineGraphSeries* series1  = new LineGraphSeries (kBlackTile, *new Rectangle (), "Alpha");
	CollectionSize index = 0;
	for (index = 1; index <= 5; index++) {
		Real myRandom =  Random (1, 100);
		series1->Append (myRandom);
	}
	fLineGraph->AddSeries (*series1);
	LineGraphSeries* series2  = new LineGraphSeries (kGrayTile, *new Oval (), "Beta");
	for (index = 1; index <= 5; index++) {
		Real myRandom = Random (1, 100);
		series2->Append (myRandom);
	}
	fLineGraph->AddSeries (*series2);

	SetOwnedView (fLineGraph);
}

Boolean	LineGraphItem::IsButton ()
{
	return (False);
}

Boolean	LineGraphItem::IsSlider ()
{
	return (False);
}

Boolean	LineGraphItem::IsText ()
{
	return (False);
}

Boolean	LineGraphItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

Boolean	LineGraphItem::ItemCanBeEnabled ()
{
	return (False);
}

LineGraph&	LineGraphItem::GetLineGraph () const
{
	EnsureNotNil (fLineGraph);
	return (*fLineGraph);
}
	
String	LineGraphItem::GetHeaderFileName ()
{
	static	const	String kFileName = "LineGraph.hh";
	return (kFileName);
}

void	LineGraphItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
}

void	LineGraphItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
}

void	LineGraphItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	LineGraphItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
}
