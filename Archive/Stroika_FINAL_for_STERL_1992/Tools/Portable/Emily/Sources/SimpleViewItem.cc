/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SimpleViewItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SimpleViewItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  1992/02/03  17:39:17  sterling
 *		Initial revision
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include	"TextView.hh"

#include	"SimpleViewItem.hh"
#include	"CommandNumbers.hh"





/*
 ********************************************************************************
 ****************************** SimpleViewItemType ****************************
 ********************************************************************************
 */
SimpleViewItemType::SimpleViewItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildUser1, "View", (ItemBuilderProc)&SimpleViewItemBuilder)
#else
	ItemType (eBuildUser1, "View", &SimpleViewItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
SimpleViewItemType&	SimpleViewItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	SimpleViewItemType::SimpleViewItemBuilder ()
{
	return (new SimpleViewItem (Get ()));
}

SimpleViewItemType*	SimpleViewItemType::sThis = Nil;





/*
 ********************************************************************************
 ****************************** SimpleViewItem ********************************
 ********************************************************************************
 */
SimpleViewItem::SimpleViewItem (ItemType& type) :
	ViewItem (type)
{
	TextView*	view = new TextView ("Simple View");
	view->SetWordWrap (True);
	SetOwnedView (view);
}

String	SimpleViewItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "View.hh";
	return (kHeaderFileName);
}

Point	SimpleViewItem::CalcDefaultSize_ (const Point& defaultSize) const
{
	if (defaultSize == kZeroPoint) {
		return (ViewItem::CalcDefaultSize_ (Point (20, 60)));
	}
	return (ViewItem::CalcDefaultSize_ (defaultSize));
}

Boolean	SimpleViewItem::ItemCanBeEnabled ()
{
	return (False);
}

Boolean	SimpleViewItem::IsButton ()
{
	return (False);
}

Boolean	SimpleViewItem::IsSlider ()
{
	return (False);
}

Boolean	SimpleViewItem::IsText ()
{
	return (False);
}

Boolean	SimpleViewItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

void	SimpleViewItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	to << ViewItem::kEmptyInitializer << newline;
}
