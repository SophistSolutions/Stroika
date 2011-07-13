/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Grid.cc,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Grid.cc,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/08  03:17:28  lewis
 *		Use new function RoundUpTo () instead of same calculation for getting # of rows/cols from cols/rows/cellcount.
 *
 *		Revision 1.4  1992/07/03  00:59:03  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *		And, shorten a few lines.
 *
 *		Revision 1.3  1992/06/28  02:31:44  lewis
 *		Use (GetCellCount () + GetColumnCount () - 1) / GetColumnCount () instead of GetCellCount () / GetColumnCount () + 1
 *		in a bunch of places - I think that is what Sterl meant. If so, maybe we should add RoundUp()
 *		function to Math.hh to make this sort of thing less error-prone?
 *
 *		Revision 1.2  1992/06/25  08:07:24  sterling
 *		Change CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.10  1992/04/24  08:59:58  lewis
 *		Makde CalcDefauitlSize() fix for sterling.
 *
 */



#include "StreamUtils.hh"

#include "Shape.hh"

#include "Grid.hh"




#if		!qRealTemplatesAvailable
	Implement (Iterator, Coordinate);
	Implement (Collection, Coordinate);
	Implement (AbSequence, Coordinate);
	Implement (Array, Coordinate);
	Implement (Sequence_Array, Coordinate);
	Implement (Sequence, Coordinate);
#endif	/*!qRealTemplatesAvailable*/





/*
 ********************************************************************************
 ********************************** AbstractGrid ********************************
 ********************************************************************************
 */
AbstractGrid::AbstractGrid ()
{
}

AbstractGrid::~AbstractGrid ()
{
}
		
CollectionSize	AbstractGrid::GetCellCount () const
{
	Invariant ();	
	return (GetCellCount_ ());
}

CollectionSize	AbstractGrid::GetColumnCount () const
{
	Invariant ();
	return (GetColumnCount_ ());
}

CollectionSize	AbstractGrid::GetRowCount () const
{
	Invariant ();
	return (GetRowCount_ ());
}
	
Coordinate	AbstractGrid::GetRowGap () const
{
	Invariant ();
	return (GetRowGap_ ());
}

Coordinate	AbstractGrid::GetColumnGap () const
{
	Invariant ();
	return (GetColumnGap_ ());
}
		
Boolean		AbstractGrid::GetGridLinesDrawn () const
{
	Invariant ();
	return (GetGridLinesDrawn_ ());
}

Coordinate	AbstractGrid::GetColumnWidth (CollectionSize column) const
{
	Invariant ();
	return (GetColumnWidth_ (column));
}

Coordinate	AbstractGrid::GetRowHeight (CollectionSize row) const
{
	Invariant ();
	return (GetRowHeight_ (row));
}
		
Boolean		AbstractGrid::GetForceSize () const
{
	Invariant ();
	return (GetForceSize_ ());
}

AbstractTextView::Justification	AbstractGrid::GetColumnJustification (CollectionSize column)
{
	Invariant ();
	return (GetColumnJustification_ (column));
}

Boolean		AbstractGrid::GetSortByColumns () const
{
	Invariant ();
	return (GetSortByColumns_ ());
}

Rect	AbstractGrid::GetGridCell (CollectionSize index) const
{
	Require (index >= 1);

	CollectionSize currentRow = 0;
	CollectionSize currentColumn = 0;
	if (GetSortByColumns ()) {
		currentColumn = (index -1) % GetColumnCount () + 1;
		currentRow	  = (index -1) / GetColumnCount () + 1;
	}
	else {
		currentColumn  	= (index -1) / GetRowCount () + 1;
		currentRow 		= (index -1) % GetRowCount () + 1;
	}
	Point	itemSize = Point (GetRowHeight (currentRow), GetColumnWidth (currentColumn));
	Point	itemOrigin = kZeroPoint;

	for (CollectionSize col = 1; col <= (currentColumn-1); col++) {
		Coordinate	offset = GetColumnWidth (col) + GetColumnGap ();
		itemOrigin.SetH (itemOrigin.GetH () + offset);
	}
	for (CollectionSize row = 1; row <= (currentRow-1); row++) {
		Coordinate	offset = GetRowHeight (row) + GetRowGap ();
		itemOrigin.SetV (itemOrigin.GetV () + offset);
	}

	return (Rect (itemOrigin, itemSize));
}

#if qDebug
void	AbstractGrid::Invariant_ () const
{
}
#endif








/*
 ********************************************************************************
 *************************** AbstractGrid_Portable ******************************
 ********************************************************************************
 */
AbstractGrid_Portable::AbstractGrid_Portable ()
{
}

AbstractGrid_Portable::~AbstractGrid_Portable ()
{
}

void	AbstractGrid_Portable::SetColumnCount (CollectionSize columnCount, Panel::UpdateMode update)
{
	Invariant ();
	if (GetColumnCount () != columnCount) {
		SetColumnCount_ (columnCount, update);
	}
	Ensure (GetColumnCount () == columnCount);
}

void	AbstractGrid_Portable::SetRowCount (CollectionSize rowCount, Panel::UpdateMode update)
{
	Invariant ();
	if (GetRowCount () != rowCount) {
		SetRowCount_ (rowCount, update);
	}
	Ensure (GetRowCount () == rowCount);
}

void	AbstractGrid_Portable::SetRowGap (Coordinate gap, Panel::UpdateMode update)
{
	Invariant ();
	if (GetRowGap () != gap) {
		SetRowGap_ (gap, update);
	}
	Ensure (GetRowGap () == gap);
}

void	AbstractGrid_Portable::SetColumnGap (Coordinate gap, Panel::UpdateMode update)
{
	Invariant ();
	if (GetColumnGap () != gap) {
		SetColumnGap_ (gap, update);
	}
	Ensure (GetColumnGap () == gap);
}

void	AbstractGrid_Portable::SetForceSize (Boolean forceSize, Panel::UpdateMode update)
{
	Invariant ();
	if (GetForceSize () != forceSize) {
		SetForceSize_ (forceSize, update);
	}
	Ensure (GetForceSize () == forceSize);
}

void	AbstractGrid_Portable::SetGridLinesDrawn (Boolean linesDrawn, Panel::UpdateMode update)
{
	Invariant ();
	if (GetGridLinesDrawn () != linesDrawn) {
		SetGridLinesDrawn_ (linesDrawn, update);
	}
	Ensure (GetGridLinesDrawn () == linesDrawn);
}

Point	AbstractGrid_Portable::CalcDefaultSize_ (const Point& defaultSize) const
{
#if 1
	return (Point (CalcRowsHeight () + (GetRowCount () -1) * GetRowGap (), CalcColumnsWidth () + (GetColumnCount () -1) * GetColumnGap ()));
#else
	Point size = defaultSize;
	if (defaultSize == kZeroPoint) {
		size = GetSize ();
	}
	
	if (GetSortByColumns ()) {
		size.SetH (CalcColumnsWidth () + (GetColumnCount () -1) * GetColumnGap ());
	}
	else {
		size.SetV (CalcRowsHeight () + (GetRowCount () -1) * GetRowGap ());
	}
	return (size);
#endif
}

void	AbstractGrid_Portable::Layout ()
{
	CollectionSize index = 0;
	ForEach (ViewPtr, it, MakeCellIterator (eSequenceBackward)) {
		RequireNotNil (it.Current ());
		
		View&	current = *it.Current ();
		Rect	extent = GetGridCell (++index);
		if (not GetForceSize ()) {
			extent = CenterRectAroundRect (current.GetLocalExtent (), extent);
		}
		current.SetOrigin (extent.GetOrigin ());
		current.SetSize (extent.GetSize ());
	}
	
	View::Layout ();
}

void	AbstractGrid_Portable::Draw (const Region& update)
{
	if (GetGridLinesDrawn ()) {
		/*
		 * Draw the grid.  Items are subviews, and draw themselves.
		 */

		CollectionSize	rowCount	=	GetRowCount ();
		CollectionSize	colCount	=	GetColumnCount ();

		Coordinate	height = GetSize ().GetV ();
		Coordinate	width  = CalcColumnsWidth ();
		Pen	rowPen = kDefaultPen;
		Pen	colPen = kDefaultPen;
		
		rowPen.SetPenSize (Point (1 + GetRowGap (), 1));
		colPen.SetPenSize (Point (1, 1 + GetColumnGap ()));
		
		Coordinate	vPos = 0;
Coordinate hackWidth = Min (width, GetSize ().GetH ());	// hack for Motif till we get real clipping
		for (CollectionSize row = 2; (row <= (rowCount+1)); row++) {
			vPos += GetRowHeight (row-1);
			if (vPos > GetSize ().GetV ()) {
				break;
			}
			OutLine (Line (Point (vPos, 0), Point (vPos, hackWidth)), rowPen);
			vPos += GetRowGap ();
		}
		Coordinate	hPos = 0;
		for (CollectionSize col = 2; (col <= (colCount +1)); col++) {
			hPos += GetColumnWidth (col-1);
			if (hPos > GetSize ().GetH ()) {
				break;
			}
			OutLine (Line (Point (0, hPos), Point (GetSize ().GetV (), hPos)), colPen);
			hPos += GetColumnGap ();
		}
	}

	AbstractGrid::Draw (update);
}

CollectionSize	AbstractGrid_Portable::GetCellCount_ () const
{
	return (GetSubViewCount ());
}

SequenceIterator(ViewPtr)*	AbstractGrid_Portable::MakeCellIterator (SequenceDirection d) const
{
	return (MakeSubViewIterator (d));
}










/*
 ********************************************************************************
 ************************* AbstractFixedGrid_Portable ***************************
 ********************************************************************************
 */
AbstractFixedGrid_Portable::AbstractFixedGrid_Portable ():
	fSortByColumns (True),
	fRows (0),
	fColumns (1),
	fRowGap (0),
	fColumnGap (0),
	fDrawGridLines (False),
	fForceSize (False),
	fColumnWidth (50),
	fRowHeight (20),
	fJustification (AbstractTextView::eJustLeft)
{
}
		
AbstractFixedGrid_Portable::~AbstractFixedGrid_Portable ()
{
}

void	AbstractFixedGrid_Portable::SetColumnsWidth (Coordinate width, Panel::UpdateMode update)
{
	if (fColumnWidth != width) {
		SetColumnsWidth_ (width, update);
	}
	Ensure (fColumnWidth == width);
}

void	AbstractFixedGrid_Portable::SetRowsHeight (Coordinate height, Panel::UpdateMode update)
{
	if (fRowHeight != height) {
		SetRowsHeight_ (height, update);
	}
	Ensure (fRowHeight == height);
}

void	AbstractFixedGrid_Portable::SetColumnsJustification (
					AbstractTextView::Justification justification,
					Panel::UpdateMode update)
{
	if (fJustification != justification) {
		SetColumnsJustification_ (justification, update);
	}
	Ensure (fJustification == justification);
}

CollectionSize	AbstractFixedGrid_Portable::GetColumnCount_ () const
{
	if (GetSortByColumns ()) {
		return (fColumns);
	}

	return (RoundUpTo (GetCellCount (), GetRowCount ()));
}

void	AbstractFixedGrid_Portable::SetColumnCount_ (CollectionSize columnCount, Panel::UpdateMode update)
{
	fColumns = columnCount;
	fSortByColumns = True;
	InvalidateLayout ();
	Refresh (update);
}
	
CollectionSize	AbstractFixedGrid_Portable::GetRowCount_ () const
{
	if (GetSortByColumns ()) {
		return (RoundUpTo (GetCellCount (), GetColumnCount ()));
	}
	return (fRows);
}

void	AbstractFixedGrid_Portable::SetRowCount_ (CollectionSize rowCount, Panel::UpdateMode update)
{
	fRows = rowCount;
	fSortByColumns = False;
	InvalidateLayout ();
	Refresh (update);
}
		
Coordinate	AbstractFixedGrid_Portable::GetRowGap_ () const
{
	return (fRowGap);
}

void	AbstractFixedGrid_Portable::SetRowGap_ (Coordinate gap, Panel::UpdateMode update)
{
	fRowGap = gap;
	InvalidateLayout ();
	Refresh (update);
}

Coordinate	AbstractFixedGrid_Portable::GetColumnGap_ () const
{
	return (fColumnGap);
}

void	AbstractFixedGrid_Portable::SetColumnGap_ (Coordinate gap, Panel::UpdateMode update)
{
	fColumnGap = gap;
	InvalidateLayout ();
	Refresh (update);
}

Boolean	AbstractFixedGrid_Portable::GetForceSize_ () const
{
	return (fForceSize);
}

void	AbstractFixedGrid_Portable::SetForceSize_ (Boolean forceSize, Panel::UpdateMode update)
{
	fForceSize = forceSize;
	InvalidateLayout ();
	Refresh (update);
}

Boolean	AbstractFixedGrid_Portable::GetGridLinesDrawn_ () const
{
	return (fDrawGridLines);
}

void	AbstractFixedGrid_Portable::SetGridLinesDrawn_ (Boolean linesDrawn, Panel::UpdateMode update)
{
	fDrawGridLines = linesDrawn;
	Refresh (update);
}

Coordinate	AbstractFixedGrid_Portable::GetColumnWidth_ (CollectionSize /* column */) const
{
	return (fColumnWidth);
}

Coordinate	AbstractFixedGrid_Portable::GetRowHeight_ (CollectionSize /* row */) const
{
	return (fRowHeight);
}

AbstractTextView::Justification	AbstractFixedGrid_Portable::GetColumnJustification_ (CollectionSize /* column */)
{
	return (fJustification);
}
		
Boolean	AbstractFixedGrid_Portable::GetSortByColumns_ () const
{
	return (fSortByColumns);
}

Coordinate	AbstractFixedGrid_Portable::CalcColumnsWidth () const
{
	return (GetColumnCount () * fColumnWidth);
}

Coordinate	AbstractFixedGrid_Portable::CalcRowsHeight () const
{
	return (GetRowCount () * fRowHeight);
}

void	AbstractFixedGrid_Portable::SetColumnsWidth_ (Coordinate width, Panel::UpdateMode update)
{
	fColumnWidth = width;
	InvalidateLayout ();
	Refresh (update);
}

void	AbstractFixedGrid_Portable::SetRowsHeight_ (Coordinate height, Panel::UpdateMode update)
{
	fRowHeight = height;
	InvalidateLayout ();
	Refresh (update);
}

void	AbstractFixedGrid_Portable::SetColumnsJustification_ (AbstractTextView::Justification justification,
																Panel::UpdateMode update)
{
	fJustification = justification;
	InvalidateLayout ();
	Refresh (update);
}





/*
 ********************************************************************************
 ************************ AbstractDynamicGrid_Portable **************************
 ********************************************************************************
 */
AbstractDynamicGrid_Portable::AbstractDynamicGrid_Portable ():
	fSortByColumns (True),
	fRows (0),
	fColumns (1),
	fRowGap (0),
	fColumnGap (0),
	fForceSize (False),
	fDrawGridLines (False),
	fCalculatedDimension (20),
	fDimensions (),
	fJustifications ()
{
	fDimensions.Append (50);
}
	
AbstractDynamicGrid_Portable::~AbstractDynamicGrid_Portable ()
{
}
		
void	AbstractDynamicGrid_Portable::SetColumnsWidth (Coordinate width, Panel::UpdateMode update)
{
	CollectionSize	totalColumns = GetColumnCount ();
	for (CollectionSize col = 1; col <= totalColumns; col++) {
		if (GetColumnWidth (col) != width) {
			SetColumnWidth (col, width, update);
		}
	}
}

void	AbstractDynamicGrid_Portable::SetRowsHeight (Coordinate height, Panel::UpdateMode update)
{
	CollectionSize	totalRows = GetRowCount ();
	for (CollectionSize row = 1; row <= totalRows; row++) {
		if (GetRowHeight (row) != height) {
			SetRowHeight (row, height, update);
		}
	}
}

void	AbstractDynamicGrid_Portable::SetColumnsJustification (AbstractTextView::Justification justification, Panel::UpdateMode update)
{
	CollectionSize	totalColumns = GetColumnCount ();
	for (CollectionSize col = 1; col <= totalColumns; col++) {
		if (GetColumnJustification (col) != justification) {
			SetColumnJustification (col, justification, update);
		}
	}
}

void	AbstractDynamicGrid_Portable::SetColumnWidth (CollectionSize columnCount, Coordinate width, Panel::UpdateMode update)
{
	if (GetSortByColumns ()) {
		fDimensions.SetAt (width, columnCount);
	}
	else {
		fCalculatedDimension = width;
	}
	Refresh (update);
}

void	AbstractDynamicGrid_Portable::SetRowHeight (CollectionSize columnCount, Coordinate height, Panel::UpdateMode update)
{
	if (not GetSortByColumns ()) {
		fDimensions.SetAt (height, columnCount);
	}
	else {
		fCalculatedDimension = height;
	}
	Refresh (update);
}

void	AbstractDynamicGrid_Portable::SetColumnJustification (CollectionSize columnCount, AbstractTextView::Justification justification, Panel::UpdateMode update)
{
	fJustifications.SetAt (justification, columnCount);
	Refresh (update);
}

CollectionSize	AbstractDynamicGrid_Portable::GetColumnCount_ () const
{
	if (GetSortByColumns ()) {
		return (fColumns);
	}
	return (RoundUpTo (GetCellCount (), GetRowCount ()));
}

void	AbstractDynamicGrid_Portable::SetColumnCount_ (CollectionSize columnCount, Panel::UpdateMode update)
{
	Coordinate	defaultWidth = 50;		
	
	if (not fSortByColumns) {
		// oh boy, just changed major dimension, have to trash most of our old data
		defaultWidth = fCalculatedDimension;		
		fCalculatedDimension = 20;
		fDimensions.RemoveAll ();
	}
	
	fColumns = columnCount;
	fSortByColumns = True;
	
	CollectionSize oldLength = fDimensions.GetLength ();
	for (int i = oldLength; i > fColumns; i--) {
		fDimensions.RemoveAt (i);
	}
	oldLength = fDimensions.GetLength ();
	Assert (oldLength <= fColumns);
	for (i = oldLength; i < fColumns; i++) {
		fDimensions.Append (defaultWidth);
	}
	Assert (fDimensions.GetLength () == fColumns);

	InvalidateLayout ();
	Refresh (update);
}
	
	
CollectionSize	AbstractDynamicGrid_Portable::GetRowCount_ () const
{
	if (GetSortByColumns ()) {
		return (RoundUpTo (GetCellCount (), GetColumnCount ()));
	}
	return (fRows);
}

void	AbstractDynamicGrid_Portable::SetRowCount_ (CollectionSize rowCount, Panel::UpdateMode update)
{
	Coordinate	defaultHeight = 20;		
	
	if (fSortByColumns) {
		// oh boy, just changed major dimension, have to trash most of our old data
		defaultHeight = fCalculatedDimension;		
		fCalculatedDimension = 50;
		fDimensions.RemoveAll ();
	}

	fRows = rowCount;
	fSortByColumns = False;

	CollectionSize oldLength = fDimensions.GetLength ();
	for (int i = oldLength; i > fRows; i--) {
		fDimensions.RemoveAt (i);
	}
	oldLength = fDimensions.GetLength ();
	Assert (oldLength <= fRows);
	for (i = oldLength; i < fRows; i++) {
		fDimensions.Append (defaultHeight);
	}
	Assert (fDimensions.GetLength () == fRows);

	InvalidateLayout ();
	Refresh (update);
}
		
Coordinate	AbstractDynamicGrid_Portable::GetRowGap_ () const
{
	return (fRowGap);
}

void	AbstractDynamicGrid_Portable::SetRowGap_ (Coordinate gap, Panel::UpdateMode update)
{
	fRowGap = gap;
	InvalidateLayout ();
	Refresh (update);
}

Coordinate	AbstractDynamicGrid_Portable::GetColumnGap_ () const
{
	return (fColumnGap);
}

void	AbstractDynamicGrid_Portable::SetColumnGap_ (Coordinate gap, Panel::UpdateMode update)
{
	fColumnGap = gap;
	InvalidateLayout ();
	Refresh (update);
}

Boolean	AbstractDynamicGrid_Portable::GetForceSize_ () const
{
	return (fForceSize);
}

void	AbstractDynamicGrid_Portable::SetForceSize_ (Boolean forceSize, Panel::UpdateMode update)
{
	fForceSize = forceSize;
	InvalidateLayout ();
	Refresh (update);
}

Boolean	AbstractDynamicGrid_Portable::GetGridLinesDrawn_ () const
{
	return (fDrawGridLines);
}

void	AbstractDynamicGrid_Portable::SetGridLinesDrawn_ (Boolean linesDrawn, Panel::UpdateMode update)
{
	fDrawGridLines = linesDrawn;
	Refresh (update);
}

Coordinate	AbstractDynamicGrid_Portable::GetColumnWidth_ (CollectionSize column) const
{
	if (GetSortByColumns ()) {
		return (fDimensions[column]);
	}
	else {
		return (fCalculatedDimension);
	}
}

Coordinate	AbstractDynamicGrid_Portable::GetRowHeight_ (CollectionSize row) const
{
	if (GetSortByColumns ()) {
		return (fCalculatedDimension);
	}
	else {
		return (fDimensions[row]);
	}
}

AbstractTextView::Justification	AbstractDynamicGrid_Portable::GetColumnJustification_ (CollectionSize column)
{
	return ((AbstractTextView::Justification) fJustifications[column]);
}
	
Boolean	AbstractDynamicGrid_Portable::GetSortByColumns_ () const
{
	return (fSortByColumns);
}

Coordinate	AbstractDynamicGrid_Portable::CalcColumnsWidth () const
{
	Coordinate	width = 0;
	CollectionSize	totalColumns = GetColumnCount ();
	for (CollectionSize col = 1; col <= totalColumns; col++) {
		width += GetColumnWidth (col);
	}
	return (width);
}

Coordinate	AbstractDynamicGrid_Portable::CalcRowsHeight () const
{
	Coordinate	height = 0;
	CollectionSize	totalRows = GetRowCount ();
	for (CollectionSize row = 1; row <= totalRows; row++) {
		height += GetRowHeight (row);
	}
	return (height);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***
