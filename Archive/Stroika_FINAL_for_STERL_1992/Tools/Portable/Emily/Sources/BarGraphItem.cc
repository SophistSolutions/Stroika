/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/BarGraphItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: BarGraphItem.cc,v $
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

#include	"BarChart.hh"

#include	"CommandNumbers.hh"
#include	"BarGraphItem.hh"

/*
 ********************************************************************************
 ****************************** BarGraphItemType ********************************
 ********************************************************************************
 */
BarGraphItemType::BarGraphItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildUser2, "BarGraph", (ItemBuilderProc)&BarGraphItemBuilder)
#else
	ItemType (eBuildUser2, "BarGraph", &BarGraphItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
BarGraphItemType&	BarGraphItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	BarGraphItemType::BarGraphItemBuilder ()
{
	return (new BarGraphItem (Get ()));
}

BarGraphItemType*	BarGraphItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** BarGraphItem ************************************
 ********************************************************************************
 */
BarGraphItem::BarGraphItem (ItemType& type) :
	ViewItem (type),
	fBarGraph (Nil)
{
	fBarGraph = new BarGraph ();
	fBarGraph->SetTitle ("Bar Chart");
	fBarGraph->GetXAxis ().SetMin (1, eNoUpdate);
	fBarGraph->GetXAxis ().SetMax (6, eNoUpdate);
	fBarGraph->GetXAxis ().SetTicks (5, eNoUpdate);
	fBarGraph->GetYAxis ().SetTicks (5, eNoUpdate);
	
	GraphSeries* bSeries1  = new GraphSeries (kBlackTile, "Mary");
	for (CollectionSize index = 1; index <= 5; index++) {
		Real myRandom = Random (1, 100);
		bSeries1->Append (myRandom);
	}
	fBarGraph->AddSeries (*bSeries1);
	GraphSeries* bSeries2  = new GraphSeries (kGrayTile, "Bob");
	for (index = 1; index <= 5; index++) {
		Real myRandom = Random (1, 100);
		bSeries2->Append (myRandom);
	}
	fBarGraph->AddSeries (*bSeries2);
	GraphSeries* bSeries3  = new GraphSeries (kLightGrayTile, "George");
	for (index = 1; index <= 5; index++) {
		Real myRandom = Random (1, 100);
		bSeries3->Append (myRandom);
	}
	fBarGraph->AddSeries (*bSeries3);

	fBarGraph->GetXAxis ().SetTickLabel (1, "3/4");
	fBarGraph->GetXAxis ().SetTickLabel (2, "3/11");
	fBarGraph->GetXAxis ().SetTickLabel (3, "3/18");
	fBarGraph->GetXAxis ().SetTickLabel (4, "3/25");
	fBarGraph->GetXAxis ().SetTickLabel (5, "4/2");

	SetOwnedView (fBarGraph);
}

Boolean	BarGraphItem::IsButton ()
{
	return (False);
}

Boolean	BarGraphItem::IsSlider ()
{
	return (False);
}

Boolean	BarGraphItem::IsText ()
{
	return (False);
}

Boolean	BarGraphItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

Boolean	BarGraphItem::ItemCanBeEnabled ()
{
	return (False);
}

BarGraph&	BarGraphItem::GetBarGraph () const
{
	EnsureNotNil (fBarGraph);
	return (*fBarGraph);
}
	
String	BarGraphItem::GetHeaderFileName ()
{
	static	const	String kFileName = "BarChart.hh";
	return (kFileName);
}

void	BarGraphItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
}

void	BarGraphItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
}

void	BarGraphItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	BarGraphItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
}
