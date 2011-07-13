/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GridItem.cc,v 1.4 1992/09/08 16:40:43 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: GridItem.cc,v $
 *		Revision 1.4  1992/09/08  16:40:43  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  17:25:44  sterling
 *		Lots of Foundation changes.
 *
 *	 	Revision 1.2  1992/01/27  16:08:59  sterling
 * 		fixed CalcDefaultSize, added se cond column
 *
 * 		Revision 1.1  1992/01/27  04:26:28  sterling
 * 		Initial revision
 *
 *
 *
 */





#include	"StreamUtils.hh"
#include	"CheckBox.hh"
#include	"Dialog.hh"
#include	"NumberText.hh"

#include	"CommandNumbers.hh"
#include	"Grid.hh"
#include	"GridItem.hh"


#include	"ViewItemInfo.hh"
#include	"FixedGridInfo.hh"
#include	"LabeledGroupItem.hh"

/*
 ********************************************************************************
 ******************************** GridItemType **********************************
 ********************************************************************************
 */
GridItemType::GridItemType () :
	ItemType (eBuildPickList, "ViewFixedGrid", (ItemBuilderProc) &GridItemBuilder)
{
	Require (sThis == Nil);
	sThis = this;
}		
		
GridItemType&	GridItemType::Get ()
{
	RequireNotNil (sThis);
	return (*sThis);
}

ViewItem*	GridItemType::GridItemBuilder ()
{
	return (new GridItem ());
}

GridItemType*	GridItemType::sThis = Nil;


/*
 ********************************************************************************
 ****************************** GridGroupView ***********************************
 ********************************************************************************
 */
class	GridGroupView : public LabeledMagicGroup {
	public:
		GridGroupView (ViewFixedGrid& grid) :
			LabeledMagicGroup (kEmptyString),
			fGrid (grid)
		{
			View::AddSubView (&fGrid);
		}

		override	void	Layout ()
		{
			fGrid.SetSize (GetSize ());
			LabeledMagicGroup::Layout ();
		}
		
		override	void	AddView (View* subView, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate)	
		{	
			fGrid.AddView (subView);	
		}
		
		nonvirtual	void	AddView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate)
		{
			fGrid.AddView (subView);
		}
		
		override	void	RemoveView (View* subView, UpdateMode updateMode = eDelayedUpdate)
		{
			fGrid.RemoveView (subView);
		}
		
		override	void	ReorderView (View* v, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate)
		{
		}
		
		override	void	ReorderView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate)
		{
		}
	
	private:
		ViewFixedGrid& fGrid;
};

/*
 ********************************************************************************
 ******************************** GridItem **************************************
 ********************************************************************************
 */
GridItem::GridItem () :
	GroupItem (GridItemType::Get (), True),
	fGrid (Nil)
{
	ViewFixedGrid*	grid = new ViewFixedGrid ();
	fGrid = grid;
	fGrid->SetColumnCount (2);
	fGrid->SetColumnsWidth (100);
	fGrid->SetGridLinesDrawn (True);
	fGrid->SetForceSize (True);
	SetGroupView (False, new GridGroupView (*grid));
}

AbstractGrid_Portable&	GridItem::GetGrid () const
{
	RequireNotNil (fGrid);
	return (*fGrid);
}

String	GridItem::GetHeaderFileName ()
{
	static	const	String kHeaderFileName = "Grid.hh";
	return (kHeaderFileName);
}

Boolean	GridItem::SubItemsCanChangeExtent () const
{
	return (False);
}

Point	GridItem::GetMinSize () const
{
	return (Point (100, 100));
}

Point	GridItem::CalcDefaultSize_ (const Point& defaultSize) const
{
	Point	newSize = GetGrid ().CalcDefaultSize (defaultSize);
	newSize = Max (Min (newSize, GetMaxSize ()), GetMinSize ());
	
	return (newSize);
}

void	GridItem::DoRead_ (class istream& from)
{
	GroupItem::DoRead_ (from);
	
	AbstractGrid_Portable& grid = GetGrid ();
	Boolean	sortByColumn, forceSize, gridLinesDrawn;
	
	from >> sortByColumn;
	if (sortByColumn) {
		CollectionSize columnCount;
		from >> columnCount;
		grid.SetColumnCount (columnCount);
	}
	else {
		CollectionSize rowCount;
		from >> rowCount;
		grid.SetRowCount (rowCount);
	}

	Coordinate	columnGap, rowGap, columnWidth, rowHeight;
	from >> columnGap >> rowGap >> columnWidth >> rowHeight;
	grid.SetColumnGap (columnGap);
	grid.SetRowGap (rowGap);
	grid.SetColumnsWidth (columnWidth);
	grid.SetRowsHeight (rowHeight);
	
	AbstractTextView::Justification	justification;
	from >> justification;
	grid.SetColumnsJustification (justification);
	from >> forceSize >> gridLinesDrawn;
	grid.SetForceSize (forceSize);
	grid.SetGridLinesDrawn (gridLinesDrawn);
}

void	GridItem::DoWrite_ (class ostream& to, int tabCount) const
{
	GroupItem::DoWrite_ (to, tabCount);
	to << tab (tabCount);

	AbstractGrid_Portable& grid = GetGrid ();
	to << grid.GetSortByColumns ();
	if (grid.GetSortByColumns ()) {
		to << grid.GetColumnCount ();
	}
	else {
		to << grid.GetRowCount ();
	}
	to << ' ' << grid.GetColumnGap () << ' ' << grid.GetRowGap ();
	to << ' ' << grid.GetColumnWidth (1) << ' ' << grid.GetRowHeight (1);
	to << grid.GetColumnJustification (1);
	to << grid.GetForceSize () << grid.GetGridLinesDrawn () << newline;
}

void	GridItem::WriteParameters (class ostream& to, int tabCount, CommandNumber language, CommandNumber gui)
{
	GroupItem::WriteParameters (to, tabCount, language, gui);
}

void	GridItem::WriteBuilder (class ostream& to, int tabCount)
{
	GroupItem::WriteBuilder (to, tabCount);
}

void	GridItem::SetItemInfo ()
{
	FixedGridInfo info = FixedGridInfo (*this);
	Dialog d = Dialog (&info, &info, AbstractPushButton::kOKLabel, AbstractPushButton::kCancelLabel);
	d.SetDefaultButton (d.GetOKButton ());
	
	if (d.Pose ()) {
		PostCommand (new SetFixedGridInfoCommand (*this, info));
		DirtyDocument ();
	}
}


SetFixedGridInfoCommand::SetFixedGridInfoCommand (GridItem& item, class FixedGridInfo& info) :
	Command (eSetItemInfo, kUndoable),
	fItem (item),
	fItemInfoCommand (Nil),
	fNewSortByColumns (False),
	fOldSortByColumns (item.GetGrid ().GetSortByColumns ()),
	fNewCount (0),
	fOldCount (0),
	fNewColumnGap (info.GetColumnGapField ().GetValue ()),
	fOldColumnGap (item.GetGrid ().GetColumnGap ()),
	fNewRowGap (info.GetRowGapField ().GetValue ()),
	fOldRowGap (item.GetGrid ().GetRowGap ()),
	fNewColumnWidth (info.GetColumnWidthField ().GetValue ()),
	fOldColumnWidth (item.GetGrid ().GetColumnWidth (1)),
	fNewRowHeight (info.GetRowHeightField ().GetValue ()),
	fOldRowHeight (item.GetGrid ().GetRowHeight (1)),
	fNewColumnJustification (info.GetJustification ()),
	fOldColumnJustification (item.GetGrid ().GetColumnJustification (1)),
	fNewForceSize (info.GetForceSizeField ().GetOn ()),
	fOldForceSize (item.GetGrid ().GetForceSize ()),
	fNewGridLinesDrawn (info.GetGridLinesField ().GetOn ()),
	fOldGridLinesDrawn (item.GetGrid ().GetGridLinesDrawn ())
{
	fItemInfoCommand = new SetItemInfoCommand (item, info.GetViewItemInfo ());
	fNewSortByColumns = IsBadNumber (info.GetRowCountField ().GetValue ());
	if (fNewSortByColumns) {
		fNewCount = (CollectionSize) info.GetColumnCountField ().GetValue ();
	}
	else {
		fNewCount = (CollectionSize) info.GetRowCountField ().GetValue ();
	}
	
	if (fOldSortByColumns) {
		fOldCount = fItem.GetGrid ().GetColumnCount ();
	}
	else {
		fOldCount = fItem.GetGrid ().GetRowCount ();
	}
}
		
void	SetFixedGridInfoCommand::DoIt ()
{
	fItemInfoCommand->DoIt ();

	AbstractGrid_Portable& grid = fItem.GetGrid ();
	if (fNewSortByColumns) {
		grid.SetColumnCount (fNewCount);
	}
	else {
		grid.SetRowCount (fNewCount);
	}
	grid.SetColumnGap (fNewColumnGap);
	grid.SetRowGap (fNewRowGap);
	grid.SetColumnsWidth (fNewColumnWidth);
	grid.SetRowsHeight (fNewRowHeight);
	grid.SetColumnsJustification (fNewColumnJustification);
	grid.SetForceSize (fNewForceSize);
	grid.SetGridLinesDrawn (fNewGridLinesDrawn);
	
	Command::DoIt ();
}

void	SetFixedGridInfoCommand::UnDoIt ()
{
	fItemInfoCommand->UnDoIt ();

	AbstractGrid_Portable& grid = fItem.GetGrid ();
	if (fOldSortByColumns) {
		grid.SetColumnCount (fOldCount);
	}
	else {
		grid.SetRowCount (fOldCount);
	}
	grid.SetColumnGap (fOldColumnGap);
	grid.SetRowGap (fOldRowGap);
	grid.SetColumnsWidth (fOldColumnWidth);
	grid.SetRowsHeight (fOldRowHeight);
	grid.SetColumnsJustification (fOldColumnJustification);
	grid.SetForceSize (fOldForceSize);
	grid.SetGridLinesDrawn (fOldGridLinesDrawn);
	
	Command::UnDoIt ();
}
