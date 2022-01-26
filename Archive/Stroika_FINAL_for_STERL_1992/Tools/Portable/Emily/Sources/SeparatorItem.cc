/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SeparatorItem.cc,v 1.5 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SeparatorItem.cc,v $
 *		Revision 1.5  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.1  92/04/30  14:18:34  14:18:34  sterling (Sterling Wight)
 *		Initial revision
 *		
 *
 *
 *
 */





#include	"StreamUtils.hh"

#include	"Separator.hh"

#include	"SeparatorItem.hh"
#include	"CommandNumbers.hh"





/*
 ********************************************************************************
 ****************************** SeparatorItemType ****************************
 ********************************************************************************
 */
SeparatorItemType::SeparatorItemType () :
#if qHP_BadTypedefForStaticProcs
	ItemType (eBuildLine, "Separator", (ItemBuilderProc)&SeparatorItemBuilder)
#else
	ItemType (eBuildLine, "Separator", &SeparatorItemBuilder)
#endif
{
	Require (sThis == Nil);
	sThis = this;
}		
		
SeparatorItemType&	SeparatorItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	SeparatorItemType::SeparatorItemBuilder ()
{
	return (new SeparatorItem (Get ()));
}

SeparatorItemType*	SeparatorItemType::sThis = Nil;





/*
 ********************************************************************************
 ****************************** SeparatorItem ********************************
 ********************************************************************************
 */
SeparatorItem::SeparatorItem (ItemType& type) :
	ViewItem (type),
	fSeparator (Nil)
{
	fSeparator = new Separator (Point::eHorizontal);
	SetOwnedView (fSeparator);
}

String	SeparatorItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "Seperator.hh";
	return (kHeaderFileName);
}

Point	SeparatorItem::CalcDefaultSize_ (const Point& hint) const
{
	Point	defaultSize = hint;
	if (defaultSize <= Point (5, 5)) {
		defaultSize = Point (100, 100);
	}
	return (ViewItem::CalcDefaultSize_ (defaultSize));
}

Boolean	SeparatorItem::ItemCanBeEnabled ()
{
	return (False);
}

Boolean	SeparatorItem::IsButton ()
{
	return (False);
}

Boolean	SeparatorItem::IsSlider ()
{
	return (False);
}

Boolean	SeparatorItem::IsText ()
{
	return (False);
}

Boolean	SeparatorItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (False);
}

void	SeparatorItem::WriteBuilder (class ostream& to, int tabCount)
{
	ViewItem::WriteBuilder (to, tabCount);
	String orientation = "Point::eHorizontal";
	if (fSeparator->GetOrientation () == Point::eVertical) {
		orientation = "Point::eVertical";
	}
	to << "(" << orientation << ")," << newline;
}

Point	SeparatorItem::GetMinSize () const
{
	if (fSeparator->GetOrientation () == Point::eVertical) {
		return (Point (10, 1));
	}
	return (Point (1, 10));
}

void	SeparatorItem::Layout ()
{
	ViewItem::Layout ();
	if (GetSize ().GetH () >= GetSize ().GetV ()) {
		fSeparator->SetOrientation (Point::eHorizontal);
	}
	else {
		fSeparator->SetOrientation (Point::eVertical);
	}
}
