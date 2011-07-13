/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/LayerGraphItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: LayerGraphItem.cc,v $
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

#include	"LayerGraph.hh"

#include	"CommandNumbers.hh"
#include	"LayerGraphItem.hh"

/*
 ********************************************************************************
 ****************************** LayerGraphItemType ********************************
 ********************************************************************************
 */
LayerGraphItemType::LayerGraphItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildUser2, "LayerGraph", (ItemBuilderProc)&LayerGraphItemBuilder)
#else
	ItemType (eBuildUser2, "LayerGraph", &LayerGraphItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
LayerGraphItemType&	LayerGraphItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	LayerGraphItemType::LayerGraphItemBuilder ()
{
	return (new LayerGraphItem (Get ()));
}

LayerGraphItemType*	LayerGraphItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** LayerGraphItem ************************************
 ********************************************************************************
 */
LayerGraphItem::LayerGraphItem (ItemType& type) :
	ViewItem (type),
	fLayerGraph (Nil)
{
	fLayerGraph = new LayerGraph ();
	fLayerGraph->SetTitle ("Layer Graph");
	fLayerGraph->GetXAxis ().SetMin (1, eNoUpdate);
	fLayerGraph->GetXAxis ().SetTicks (9, eNoUpdate);
	fLayerGraph->GetYAxis ().SetTicks (5, eNoUpdate);
	
	GraphSeries* gSeries1  = new GraphSeries (kBlackTile, "One");
	for (CollectionSize index = 1; index <= 10; index++) {
		Real myRandom = Random (1, 33);
		gSeries1->Append (myRandom);
	}
	fLayerGraph->AddSeries (*gSeries1);
	GraphSeries* gSeries2  = new GraphSeries (kGrayTile, "Two");
	for (index = 1; index <= 10; index++) {
		Real myRandom =  Random (1, 33);
		gSeries2->Append (myRandom);
	}
	fLayerGraph->AddSeries (*gSeries2);
	GraphSeries* gSeries3  = new GraphSeries (kLightGrayTile, "Three");
	for (index = 1; index <= 10; index++) {
		Real myRandom =  Random (1, 33);
		gSeries3->Append (myRandom);
	}
	fLayerGraph->AddSeries (*gSeries3);

	SetOwnedView (fLayerGraph);
}

Boolean	LayerGraphItem::IsButton ()
{
	return (False);
}

Boolean	LayerGraphItem::IsSlider ()
{
	return (False);
}

Boolean	LayerGraphItem::IsText ()
{
	return (False);
}

Boolean	LayerGraphItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

Boolean	LayerGraphItem::ItemCanBeEnabled ()
{
	return (False);
}

LayerGraph&	LayerGraphItem::GetLayerGraph () const
{
	EnsureNotNil (fLayerGraph);
	return (*fLayerGraph);
}
	
String	LayerGraphItem::GetHeaderFileName ()
{
	static	const	String kFileName = "LayerGraph.hh";
	return (kFileName);
}

void	LayerGraphItem::DoRead_ (class istream& from)
{
	ViewItem::DoRead_ (from);
}

void	LayerGraphItem::DoWrite_ (class ostream& to, int tabCount) const
{
	ViewItem::DoWrite_ (to, tabCount);
}

void	LayerGraphItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}

void	LayerGraphItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	ViewItem::WriteParameters (to, tabCount, language, gui);
}
