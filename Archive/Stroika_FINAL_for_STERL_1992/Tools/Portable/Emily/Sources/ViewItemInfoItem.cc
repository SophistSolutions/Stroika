/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/ViewItemInfoItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: ViewItemInfoItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 */

#include	"StreamUtils.hh"
#include	"Dialog.hh"

#include	"CommandNumbers.hh"
#include	"ViewItemInfo.hh"
#include	"ViewItemInfoItem.hh"
#include	"LabeledGroupItem.hh"




/*
 ********************************************************************************
 **************************** ViewItemInfoItemType ******************************
 ********************************************************************************
 */
ViewItemInfoItemType::ViewItemInfoItemType () :
	ItemType (eBuildUser1, "ViewItemInfo", (ItemBuilderProc) &ViewItemInfoItemBuilder)
{
	Require (sThis == Nil);
	sThis = this;
}		
		
ViewItemInfoItemType&	ViewItemInfoItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	ViewItemInfoItemType::ViewItemInfoItemBuilder ()
{
	return (new ViewItemInfoItem ());
}

ViewItemInfoItemType*	ViewItemInfoItemType::sThis = Nil;



/*
 ********************************************************************************
 ****************************** GridGroupView ***********************************
 ********************************************************************************
 */
class	ViewItemMagicGroup : public LabeledMagicGroup {
	public:
		ViewItemMagicGroup (ViewItemInfo& info) :
			LabeledMagicGroup (kEmptyString),
			fInfo (info)
		{
			View::AddSubView (&fInfo);
		}

		override	void	Layout ()
		{
			fInfo.SetSize (GetSize ());
			LabeledMagicGroup::Layout ();
		}
		
		override	void	AddView (View* subView, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate)	
		{	
			fInfo.AddSubView (subView, index, updateMode);	
		}
		
		nonvirtual	void	AddView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate)
		{
			fInfo.AddSubView (subView, neighborView, addMode, updateMode);
		}
		
		override	void	RemoveView (View* subView, UpdateMode updateMode = eDelayedUpdate)
		{
			fInfo.RemoveSubView (subView, updateMode);
		}
		
		override	void	ReorderView (View* v, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate)
		{
			fInfo.ReorderSubView (v, index, updateMode);
		}
	
		override	void	ReorderView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate)
		{
			fInfo.ReorderSubView (subView, neighborView, addMode, updateMode);
		}
	
	private:
		ViewItemInfo& fInfo;
};

/*
 ********************************************************************************
 **************************** ViewItemInfoItem **********************************
 ********************************************************************************
 */
ViewItemInfoItem::ViewItemInfoItem () :
	GroupItem (ViewItemInfoItemType::Get (), True)
{
	SetGroupView (False, new ViewItemMagicGroup (*new ViewItemInfo ()));
}

String	ViewItemInfoItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "ViewItemInfo.hh";
	return (kHeaderFileName);
}


Point	ViewItemInfoItem::GetMinSize () const
{
	return (Point (180, 300));
}

Point	ViewItemInfoItem::CalcDefaultSize_ (const Point& defaultSize) const
{
	if (defaultSize == kZeroPoint) {
		return (Point (200, 300));
	}
	return (defaultSize);
}

void	ViewItemInfoItem::DoRead_ (class istream& from)
{
	GroupItem::DoRead_ (from);
}

void	ViewItemInfoItem::DoWrite_ (class ostream& to, int tabCount) const
{
	GroupItem::DoWrite_ (to, tabCount);
}

void	ViewItemInfoItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	GroupItem::WriteParameters (to, tabCount, language, gui);
}

Boolean	ViewItemInfoItem::IsFocusItem (CommandNumber /*gui*/)
{
	return (True);
}

String	ViewItemInfoItem::GetFocusItemFieldName () const
{
	return (GetFieldName ());
}
