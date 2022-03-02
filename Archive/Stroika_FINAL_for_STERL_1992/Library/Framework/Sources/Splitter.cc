/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Splitter.cc,v 1.10 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Splitter.cc,v $
 *		Revision 1.10  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.9  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.7  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.6  1992/07/16  16:58:56  sterling
 *		changed GUI to UI
 *
 *		Revision 1.4  1992/07/03  02:17:01  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/07/02  04:59:58  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  08:44:10  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/06/09  22:20:13  lewis
 *		Changed SequenceForEach to ForEachS.
 *
 *		Revision 1.4  92/06/09  15:19:36  15:19:36  sterling (Sterling Wight)
 *		used Erase instead of Paint
 *		
 *		Revision 1.3  1992/05/13  13:06:56  lewis
 *		STERL: added checks for zero sized views and adjusted size to 100x100.
 *
 *		Revision 1.1  92/04/30  13:13:12  13:13:12  sterling (Sterling Wight)
 *		Initial revision
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"Shape.hh"

#include	"Splitter.hh"
#include 	"Tracker.hh"





/*
 ********************************************************************************
 ***************************** AbstractSplitViewInfo ****************************
 ********************************************************************************
 */
AbstractSplitViewInfo::AbstractSplitViewInfo ()
{
}

AbstractSplitViewInfo::~AbstractSplitViewInfo ()
{
}
		
Point	AbstractSplitViewInfo::GetMinSize () const
{
	Invariant ();
	return (GetMinSize_ ());
}

void	AbstractSplitViewInfo::SetMinSize (const Point& size)
{
	Invariant ();
	if (GetMinSize () != size) {
		SetMinSize_ (size);
	}
	Invariant ();
	Ensure (GetMinSize () == size);
}
		
Point	AbstractSplitViewInfo::GetMaxSize () const
{
	Invariant ();
	return (GetMaxSize_ ());
}

void	AbstractSplitViewInfo::SetMaxSize (const Point& size)
{
	Invariant ();
	if (GetMaxSize () != size) {
		SetMaxSize_ (size);
	}
	Invariant ();
	Ensure (GetMaxSize () == size);
}

View&	AbstractSplitViewInfo::GetView () const
{
	Invariant ();
	return (GetView_ ());
}

#if 	qDebug		
void	AbstractSplitViewInfo::Invariant_ () const
{
	Assert (GetMinSize_ () <= GetMaxSize_ ());
}
#endif




/*
 ********************************************************************************
 ********************************* SplitViewInfo ********************************
 ********************************************************************************
 */
SplitViewInfo::SplitViewInfo (View& view):
	AbstractSplitViewInfo (),
	fView (&view),
	fMinSize (kZeroPoint),
	fMaxSize (kMaxPoint)
{
	RequireNotNil (&view);
}

Point	SplitViewInfo::GetMinSize_ () const
{
	return (fMinSize);
}

void	SplitViewInfo::SetMinSize_ (const Point& size)
{
	fMinSize = size;
}
		
Point	SplitViewInfo::GetMaxSize_ () const
{
	return (fMaxSize);
}

void	SplitViewInfo::SetMaxSize_ (const Point& size)
{
	fMaxSize = size;
}
		
View&	SplitViewInfo::GetView_ () const
{
	EnsureNotNil (fView);
	return (*fView);
}

class	PrivateSplitViewInfo : public AbstractSplitViewInfo {
	public:
		PrivateSplitViewInfo (const SplitViewInfo& info);
		PrivateSplitViewInfo (View& view);
	
		Real	fProportion;
		
	protected:	
		override	Point	GetMinSize_ () const;
		override	void	SetMinSize_ (const Point& size);
		
		override	Point	GetMaxSize_ () const;
		override	void	SetMaxSize_ (const Point& size);
		
		override	View&	GetView_ () const;
		
	private:
		SplitViewInfo	fInfo;
};

PrivateSplitViewInfo::PrivateSplitViewInfo (const SplitViewInfo& info) :
	fInfo (info),
	fProportion (0)
{
	RequireNotNil (&info);
}

PrivateSplitViewInfo::PrivateSplitViewInfo (View& view) :
	fInfo (SplitViewInfo (view)),
	fProportion (0)
{
	RequireNotNil (&view);
}

Point	PrivateSplitViewInfo::GetMinSize_ () const
{
	return (fInfo.GetMinSize ());
}

void	PrivateSplitViewInfo::SetMinSize_ (const Point& size)
{
	fInfo.SetMinSize (size);
}		

Point	PrivateSplitViewInfo::GetMaxSize_ () const
{
	return (fInfo.GetMaxSize ());
}

void	PrivateSplitViewInfo::SetMaxSize_ (const Point& size)
{
	fInfo.SetMaxSize (size);
}	

View&	PrivateSplitViewInfo::GetView_ () const
{
	return (fInfo.GetView ());
}


#if		!qRealTemplatesAvailable
	Implement (Iterator, PrivateSplitViewInfoPtr);
	Implement (Collection, PrivateSplitViewInfoPtr);
	Implement (AbSequence, PrivateSplitViewInfoPtr);
	Implement (Array, PrivateSplitViewInfoPtr);
	//Implement (Sequence_Array, PrivateSplitViewInfoPtr);
	Sequence_ArrayImplement1 (PrivateSplitViewInfoPtr);
	Sequence_ArrayImplement2 (PrivateSplitViewInfoPtr);
	Implement (Sequence, PrivateSplitViewInfoPtr);
#endif

/*
 ********************************************************************************
 ******************************** AbstractSplitter ******************************
 ********************************************************************************
 */
AbstractSplitter::AbstractSplitter (Point::Direction orientation) :
	fOrientation (orientation),
	fGapHeight (3),
	fSplitViews ()
{
}
		
Point	AbstractSplitter::CalcDefaultSize_ (const Point& hint) const
{
	Point	defaultSize = hint;
	Point::Direction d = GetOrientation ();
	
	defaultSize.SetVH (d, 0);
	CollectionSize visibleViews = 0;
	ForEach (PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo& current = *it.Current ();
		if (current.GetView ().GetVisible ()) {
			defaultSize.SetVH (d, defaultSize[d] + current.GetView ().GetSize ()[d]);
			visibleViews++;
		}
	}
	defaultSize.SetVH (d, defaultSize[d] + GetGapHeight () * (visibleViews-1));
	
	return (defaultSize);
}

Real	AbstractSplitter::CalcProportion (Coordinate size)
{
	Real totalProportion = 0;
	CollectionSize visibleViews = 0;
	ForEach (PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo& current = *it.Current ();
		if (current.GetView ().GetVisible ()) {
			totalProportion += current.fProportion;
			visibleViews++;
		}
	}
	Point::Direction d = GetOrientation ();
	
	Coordinate	spaceToPartition = GetSize ()[d];
	spaceToPartition -= (GetGapHeight () * (visibleViews-1));
	Require (spaceToPartition > 0);
	return (totalProportion * size / spaceToPartition);
}

void	AbstractSplitter::ResizeSplitter (const MousePressInfo& mouseInfo, CollectionSize index)
{
	Require (index < fSplitViews.GetLength ());
	
	Point::Direction d = GetOrientation ();
	PrivateSplitViewInfo&	s1 = *fSplitViews[index];
	PrivateSplitViewInfo&	s2 = *fSplitViews[index + 1];
	
	Rect	r = BoundingRect (s1.GetView ().GetExtent ().GetBotRight (), s2.GetView ().GetExtent ().GetTopLeft ());
	Rect	bounds = BoundingRect (s1.GetView ().GetExtent ().GetTopLeft (), s2.GetView ().GetExtent ().GetBotRight ());
	ShapeDragger dragger = ShapeDragger (Rectangle (), r, *this, (d == Point::eVertical) ? ShapeDragger::eVertical : Point::eHorizontal);
	dragger.SetTimeOut (0);
	dragger.SetBounds (bounds);
	dragger.TrackPress (mouseInfo);
	Point	delta = dragger.GetNewShapeExtent ().GetOrigin () - dragger.GetOldShapeExtent ().GetOrigin ();
	
	Real rDelta = CalcProportion (delta[d]);
	s1.fProportion += rDelta;
	s2.fProportion -= rDelta;
	InvalidateLayout ();
}

Rect	AbstractSplitter::CalcGapRect (const View& view) const
{
	Rect r = GetLocalExtent ();
	if (GetOrientation () == Point::eVertical) {
		r.SetOrigin (view.GetExtent ().GetBotLeft ());
		r.SetSize (Point (GetGapHeight (), GetSize ().GetH ()));
	}
	else {
		r.SetOrigin (view.GetExtent ().GetTopRight ());
		r.SetSize (Point (GetSize ().GetV (), GetGapHeight ()));
	}
	return (r);
}

Boolean	AbstractSplitter::TrackPress (const MousePressInfo& mouseInfo)
{
	if (View::TrackPress (mouseInfo)) {
		return (True);
	}
	ForEachT (SequenceIterator, PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo& current = *it.Current ();
		if (current.GetView ().GetVisible () and (it.CurrentIndex () < fSplitViews.GetLength ())) {
			Rect r = CalcGapRect (current.GetView ());
			
			if (r.Contains (mouseInfo.fPressAt)) {
				ResizeSplitter (mouseInfo, it.CurrentIndex ());
				return (True);
			}
		}
	}
	
	return (False);
}

Boolean	AbstractSplitter::TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState)
{
	if (View::TrackMovement (cursorAt, mouseRgn, keyBoardState)) {
		return (True);
	}
	
	ForEachT (SequenceIterator, PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo& current = *it.Current ();
		if (current.GetView ().GetVisible () and (it.CurrentIndex () < fSplitViews.GetLength ())) {
			Rect r = CalcGapRect (current.GetView ());
			
			if (r.Contains (cursorAt)) {
				kCrossCursor.Install ();
				mouseRgn = r;
				return (True);
			}
		}
	}

	return (False);
}

Point::Direction	AbstractSplitter::GetOrientation () const
{
	return (fOrientation);
}

void	AbstractSplitter::SetOrientation (Point::Direction orientation, Panel::UpdateMode update)
{
	if (GetOrientation () != orientation) {
		fOrientation = orientation;
		InvalidateLayout ();
		Refresh (update);
	}
	Ensure (GetOrientation () == orientation);
}

void	AbstractSplitter::AddSplitView (const SplitViewInfo& info, Panel::UpdateMode update)
{
	RequireNotNil (&info);
	AddSubView (&info.GetView (), update);
	PrivateSplitViewInfo* newInfo = new PrivateSplitViewInfo (info);
	if (newInfo->GetView ().GetSize () == kZeroPoint) {
		newInfo->GetView ().SetSize (Point (100, 100), eNoUpdate);
	}
	if (newInfo->GetView ().GetVisible ()) {
		newInfo->fProportion = newInfo->GetView ().GetSize ()[GetOrientation ()];
	}
	fSplitViews.Append (newInfo);
}

void	AbstractSplitter::AddSplitView (View& view, Panel::UpdateMode update)
{
	RequireNotNil (&view);
	AddSubView (&view, update);
	PrivateSplitViewInfo* newInfo = new PrivateSplitViewInfo (view);
	if (newInfo->GetView ().GetSize () == kZeroPoint) {
		newInfo->GetView ().SetSize (Point (100, 100), eNoUpdate);
	}
	if (newInfo->GetView ().GetVisible ()) {
		newInfo->fProportion = newInfo->GetView ().GetSize ()[GetOrientation ()];
	}
	fSplitViews.Append (newInfo);
}

void	AbstractSplitter::RemoveSplitView (const SplitViewInfo& info, Panel::UpdateMode update)
{
	RequireNotNil (&info);
	
	View*	view = &info.GetView ();
	AssertNotNil (view);
	RemoveSubView (view, update);
	ForEach (PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo*	current = it.Current ();
		if (&current->GetView () == view) {
			fSplitViews.Remove (current);
		}
	}
}

void	AbstractSplitter::RemoveSplitView (View& view, Panel::UpdateMode update)
{
	RequireNotNil (&view);
	
	RemoveSubView (&view, update);
	ForEach (PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo*	current = it.Current ();
		if (&current->GetView () == &view) {
			fSplitViews.Remove (current);
		}
	}
}

Coordinate	AbstractSplitter::GetGapHeight () const
{
	return (fGapHeight);
}

void	AbstractSplitter::SetGapHeight (Coordinate gapHeight, Panel::UpdateMode update)
{
	if (GetGapHeight () != gapHeight) {
		fGapHeight = gapHeight;
		InvalidateLayout ();
		Refresh (update);
	}
	Ensure (GetGapHeight () == gapHeight);
}
		
void	AbstractSplitter::Layout ()
{
	Real totalProportion = 0;
	CollectionSize	visibleViews = 0;
	ForEach (PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo& current = *it.Current ();
		if (current.GetView ().GetVisible ()) {
			totalProportion += current.fProportion;
			visibleViews++;
		}
	}
	Point::Direction d = GetOrientation ();
	
	Coordinate	spaceToPartition = GetSize ()[d];
	spaceToPartition -= (GetGapHeight () * (visibleViews-1));
	Point	origin = kZeroPoint;
	ForEach (PrivateSplitViewInfoPtr, it1, fSplitViews) {
		PrivateSplitViewInfo& current = *it1.Current ();
		if (current.GetView ().GetVisible ()) {
			Point newSize = GetSize ();
			newSize.SetVH (d, current.fProportion * spaceToPartition / totalProportion);
			current.GetView ().SetSize (newSize);
			current.GetView ().SetOrigin (origin);
			origin.SetVH (d, origin[d] + newSize[d] + GetGapHeight ());
		}
	}

	View::Layout ();
}

void	AbstractSplitter::Draw (const Region& /*update*/)
{
	Rect r = GetLocalExtent ();
	ForEachT (SequenceIterator, PrivateSplitViewInfoPtr, it, fSplitViews) {
		PrivateSplitViewInfo& current = *it.Current ();
		if (current.GetView ().GetVisible () and (it.CurrentIndex () < fSplitViews.GetLength ())) {
			DrawGap (CalcGapRect (current.GetView ()));
		}
	}
}






/*
 ********************************************************************************
 ******************************** Splitter_MacUI *******************************
 ********************************************************************************
 */
Splitter_MacUI::Splitter_MacUI (Point::Direction orientation):
	AbstractSplitter (orientation)
{
}

void	Splitter_MacUI::DrawGap (const Rect& where)
{
	Paint (Rectangle (), where, kBlackTile);
}





/*
 ********************************************************************************
 ******************************* Splitter_MotifUI ******************************
 ********************************************************************************
 */
Splitter_MotifUI::Splitter_MotifUI (Point::Direction orientation):
	AbstractSplitter (orientation),
	fSashLength (12)
{
	SetGapHeight (6);
}

void	Splitter_MotifUI::DrawGap (const Rect& where)
{
	const	Coordinate	kSeparatorWidth = 1;
	
	Rect halfRect1 = where;
	Rect halfRect2 = kZeroRect;
	if (GetOrientation () == Point::eVertical) {
		Point	newSize = Point (Point (kSeparatorWidth, halfRect1.GetSize ().GetH ()));
		halfRect1.SetSize (newSize);
		halfRect1.SetOrigin (halfRect1.GetOrigin () + Point (where.GetHeight ()/2-kSeparatorWidth, 0));
		halfRect2 = halfRect1 + Point (kSeparatorWidth, 0);
	}
	else {
		Point	newSize = Point (halfRect1.GetSize ().GetV (), kSeparatorWidth);
		halfRect1.SetSize (newSize);
		halfRect1.SetOrigin (halfRect1.GetOrigin () + Point (0, where.GetWidth ()/2-kSeparatorWidth));
		halfRect2 = halfRect1 + Point (0, kSeparatorWidth);
	}

	Paint (Rectangle (), halfRect1, kBlackTile);
	Paint (Rectangle (), halfRect2, kLightGrayTile);
	
	Point::Direction d = GetOrientation ();
	Point::Direction otherD = (d == Point::eVertical) ? Point::eHorizontal : Point::eVertical;
	
	Point	sashSize;
	Point	sashOrigin;
	sashSize.SetVH (otherD, GetSashLength ());
	sashSize.SetVH (d, where.GetSize ()[d]);
	sashOrigin.SetVH (d, where.GetOrigin ()[d]);
	sashOrigin.SetVH (otherD, where.GetOrigin ()[otherD] + where.GetSize ()[otherD] - sashSize[otherD] - 10);
	Rect	sashRect = Rect (sashOrigin, Point (sashSize));
	Erase (Rectangle (), sashRect);
	OutLine (Rectangle (), sashRect);
}

Coordinate	Splitter_MotifUI::GetSashLength () const
{
	return (fSashLength);
}

void	Splitter_MotifUI::SetSashLength (Coordinate sashLength, Panel::UpdateMode update)
{
	if (GetSashLength () != sashLength) {
		fSashLength = sashLength;
		Refresh (update);
	}
	Ensure (GetSashLength () == sashLength);
}



/*
 ********************************************************************************
 ******************************** Splitter_WinUI *******************************
 ********************************************************************************
 */
Splitter_WinUI::Splitter_WinUI (Point::Direction orientation):
	AbstractSplitter (orientation)
{
}

void	Splitter_WinUI::DrawGap (const Rect& where)
{
	Paint (Rectangle (), where, kBlackTile);
}








/*
 ********************************************************************************
 *********************************** Splitter ***********************************
 ********************************************************************************
 */
Splitter::Splitter (Point::Direction orientation):
#if qMacUI
	Splitter_MacUI (orientation)
#elif qMotifUI
	Splitter_MotifUI (orientation)
#elif qWinUI
	Splitter_WinUI (orientation)
#endif
{
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

