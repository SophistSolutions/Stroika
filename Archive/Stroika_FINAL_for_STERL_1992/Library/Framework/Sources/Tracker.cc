/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Tracker.cc,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Changes:
 *	$Log: Tracker.cc,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/08  03:44:18  lewis
 *		Renamed PointInside to Contains (). Also, use AssertNotReached () etc
 *		instead of SwitchFallThru (deleted).
 *
 *		Revision 1.4  1992/07/04  14:23:08  lewis
 *		Take advantage of new shape letter/envelope support- pass by value, instead of reference.
 *		Shapes now pass by value using new letter / env stuff. As side effect of that
 *		I have (at least temporarily gotten rid of Setter methods for fields of shapes
 *		So, I changed updates of line with SetFrom/SetTo to line = Line (from,to).
 *		I think thats better anyhow, but I could add back the setter methods?
 *
 *		Revision 1.3  1992/07/01  01:38:12  lewis
 *		Got rid of qNestedTypesAvailable flag - assume always true.
 *
 *		Revision 1.2  1992/06/25  08:53:43  sterling
 *		Minor change to Tracker::TrackPress() - at end only call TrackOnce() if not done tracking.
 *
 *		Revision 1.29  1992/05/01  01:39:09  lewis
 *		Comment out Application::Get ().SuspendIdleTasks (); and Application::Get ().SuspendCommandProcessing ()
 *		since they seem to be causeing trouble in some cases. REDO this again later. Next week.
 *
 *		Revision 1.28  92/04/30  14:52:35  14:52:35  sterling (Sterling Wight)
 *		added Get and SetBounds to a bunch of trackers
 *		
 *		Revision 1.27  92/04/14  12:36:44  12:36:44  lewis (Lewis Pringle)
 *		Made default Feedback delays consistent across Selector constructors.
 *		
 *		Revision 1.26  92/04/07  12:56:28  12:56:28  sterling (Sterling Wight)
 *		tweaked feedbacks delays
 *		
 *		Revision 1.25  92/04/06  15:30:48  15:30:48  sterling (Sterling Wight)
 *		fixed memory leak in Selector (fCurrentPen not deleted)
 *		
 *		Revision 1.24  92/04/02  11:42:32  11:42:32  lewis (Lewis Pringle)
 *		Since we call SetClip, and that method has been remvoed from TabletOwner, we must do the indirection
 *		to the Tablet ourselves.
 *		
 *		Revision 1.23  92/03/26  17:28:10  17:28:10  sterling (Sterling Wight)
 *		got rid of cast on return values
 *		
 *		Revision 1.22  1992/03/19  17:03:54  lewis
 *		Tried putting in calls to SuspendIdleTasks, and CommandProcssing while tracking - code not really right, but
 *		tried to do as similar to what was there as possible - it usied to call ProcessNextEvent, and weve made the
 *		protected, so now must suspend other processing, and then call RunOnce.
 *		Code needs cleanup (Tracker::TrackPress()).
 *
 *		Revision 1.21  1992/03/05  21:21:27  sterling
 *		made "Scroller aware"
 *
 *		Revision 1.20  1992/02/24  04:08:53  lewis
 *		Do a sync after feedback, and before wait while tracking.
 *
 *		Revision 1.19  1992/02/12  07:17:28  lewis
 *		Cleanup compiler warnings.
 *
 *		Revision 1.18  1992/02/11  01:39:02  lewis
 *		Throw in a server grab while tracking in Tracker::TrackPress().
 *
 *		Revision 1.17  1992/02/03  16:39:29  sterling
 *		got rid of fault Requires in ShapeDragger
 *
 *		Revision 1.15  1992/01/15  10:06:38  lewis
 *		Inherit from Panel for now, since MouseHandler no longer does - but soon inherit from Enclosure instead.
 *
 *		Revision 1.14  1992/01/14  06:07:19  lewis
 *		Made Tracker be a TabletOwner, and change ctors to take seperate Tablet/origin
 *		when constructed from a Panel. Leave same style "short" constructor when we know the type of the Panel is View.
 *
 *		Revision 1.12  1992/01/08  23:56:12  lewis
 *		Use rectangle shapes rather than regions under Xt while things remain buggy with X
 *		region implementation. Also, fixed one thing that caused trouble on mac - using
 *		original bounds for the sizer - since its sometimes zero...
 *
 *		Revision 1.11  1992/01/08  05:45:47  lewis
 *		Support new EventHandler/Manager scheme - now we inherit from EventHanlder instead of EventManager -
 *		call Activate/Deactivate () around mouseclicks.
 *
 *		Revision 1.10  1992/01/03  07:17:26  lewis
 *		Try a couple of harmless hacks to get stuff working on X - use shape extent rather than 100x100
 * 		for regionshape since X having trouble scaling - that hack doesn't seem to be cutting it...
 *
 *		Revision 1.9  1991/12/27  19:28:22  lewis
 *		merged in mac bugfixes from sterling.
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"KeyBoard.hh"
#include	"Shape.hh"

#include	"DeskTop.hh"
#include	"View.hh"

#include	"Application.hh"
#include	"Scroller.hh"
#include	"Tracker.hh"



const	Point	kDefaultHysteresis	= Point (3, 3);











/*
 ********************************************************************************
 ************************************ Tracker ***********************************
 ********************************************************************************
 */
Tracker::Tracker (Panel& enclosure, Tablet& tablet, const Point& localToTabletOffset):
	MouseHandler (),
	EventHandler (),
	TabletOwner (),
	fEnclosure (enclosure),
	fTrackNonMovement (False),
	fHysteresis (kDefaultHysteresis),
	fTimeOut (kDefaultTimeOut),
	fStartingTime (GetCurrentTime ()),
	fFeedBackPen (kDefaultFeedbackPen),
#if		qSunCFront_OverloadingConfusionBug
	fFeedBackDelay ((BigReal)0),
#else
	fFeedBackDelay (0),
#endif
	fFeedbackClip (enclosure.GetVisibleArea ()),
	fTablet (tablet),
	fLocalToTabletOffset (localToTabletOffset),
	fScroller (Nil)
{
	SetOrigin (kZeroPoint, eNoUpdate);
	SetSize (fEnclosure.GetSize (), eNoUpdate);
}

Tablet*	Tracker::GetTablet () const
{
	return (&fTablet);
}

Panel*	Tracker::GetParentPanel () const
{
	return (&fEnclosure);
}

void	Tracker::ProcessMouseMoved (const osEventRecord& /*eventRecord*/, const Point& /*newLocation*/, const KeyBoard& /*keyBoardState*/, Region& /*newMouseRegion*/)
{
	// ignore mouse moved events - cut off processing (BUT WHY - LGP Jan 7, 1992)...
	// LGP Jan 14, 1992 - maybe we cut them off so nobody resets cursor under us??? That would make sense!!!
}

Boolean	Tracker::TrackPress (const MousePressInfo& mouseInfo)
{
	Point	anchor 		= 	mouseInfo.fPressAt;
	Point	previous 	= 	mouseInfo.fPressAt;
	Point	next 		= 	mouseInfo.fPressAt;

	Activate ();
	ReorderActiveList ();

	/*
	 * Suspend idle task processing and command processing while tracking. Not so clear this is a good idea?
	 * But code used to call ProcessNextEvent (), and all that runonce does in addition is these two things (
	 * well roughly so).
	 *
	 * Also, I cannot see any sensible use of idletask, and command processing DURING tracking. A blinking caret
	 * would clearly be a mistake while tracking... (???)
	 */
//Application::Get ().SuspendIdleTasks ();
//Application::Get ().SuspendCommandProcessing ();

		while (EventManager::Get ().MouseStillDown () and not BeginTracking (anchor, AncestorToLocal (EventManager::Get ().GetMouseLocation (), &DeskTop::Get ()))) {
			EventManager::Get ().RunOnce ();
//		 	EventManager::Get ().ProcessNextEvent ();
// WHY NOT CALL RUNONCE HERE???
		}

// Not so clear where to resume??? This seems wrong, but too many exit points to get this really right - this
// code is the most 'equivilent' to the old call to ProcessNextEvent (), however...
//Application::Get ().ResumeIdleTasks ();
//Application::Get ().ResumeCommandProcessing ();


		if (not EventManager::Get ().MouseStillDown ()) {
			Deactivate ();
			return (True);
		}

		RequireNotNil (GetTablet ());
		GetTablet ()->SetClip (LocalToTablet (GetFeedbackClip ()));
// On a trail basis, we'll do a ServerGrab() to make sure no-one screws up our graphix.
// Generally, I think this is not necessary, and probably a bad idea, since it is
// dangerous, and some would consider unfriendly, but the X docs do recomend it.???
// LGP Feb 10, 1992
GetTablet ()->ServerGrab ();
		Boolean		doneTracking = TrackOnce (eTrackPress, anchor, previous, next);
		while (not (doneTracking) and EventManager::Get ().MouseStillDown ()) {
			next = AncestorToLocal (EventManager::Get ().GetMouseLocation (), &DeskTop::Get ());
			doneTracking = TrackOnce (eTrackMove, anchor, previous, next);
			previous = next;
		}
		if (not doneTracking) {
			TrackOnce (eTrackRelease, anchor, previous, next);
		}
GetTablet ()->ServerUnGrab ();

	Deactivate ();

	return (True);
}

Boolean	Tracker::TrackMovement (const Point& /*cursorAt*/, Region& /*mouseRgn*/, const KeyBoard& /*keyBoardState*/)
{
	return (False);
}

Boolean	Tracker::BeginTracking (const Point& anchor, const Point& current)
{
	return (Boolean ((Abs (current - anchor) >= fHysteresis) or
					 ((GetCurrentTime () - fStartingTime) >= fTimeOut)
					)
			);
}

Boolean	Tracker::TrackOnce (TrackPhase phase, const Point& anchor, const Point& previous, Point& next)
{
	Boolean	abort = False;
	TrackConstrain (phase, anchor, previous, next);
	if ((phase != eTrackMove) or (fTrackNonMovement) or (previous != next)) {
		TrackFeedback (phase, anchor, previous, next);
		AssertNotNil (GetTablet ());
		GetTablet ()->Sync ();
		EventManager::Get ().WaitFor (GetFeedBackDelay ());
	}
	return (TrackResult (phase, anchor, previous, next));
}

void	Tracker::SetTrackNonMovement (Boolean trackNonMovement)
{
	fTrackNonMovement = trackNonMovement;
}

void	Tracker::SetHysteresis (const Point& hysteresis)
{
	fHysteresis = hysteresis;
}

void	Tracker::SetTimeOut (Time timeOut)
{
	fTimeOut = timeOut;
}

void	Tracker::SetFeedBackPen (const Pen& feedBackPen)
{
	fFeedBackPen = feedBackPen;
}

Point	Tracker::LocalToTablet_ (const Point& p)	const
{
	return (p + fLocalToTabletOffset);
}

Point	Tracker::TabletToLocal_ (const Point& p)	const
{
	return (p - fLocalToTabletOffset);
}

Scroller*	Tracker::GetScroller () const
{
	return (fScroller);
}

void	Tracker::SetScroller (Scroller* scroller)
{
	fScroller = scroller;
}

void	Tracker::ScrollSoShowing (const Rect& whatToShow, Boolean forceFullyShown)
{
	RequireNotNil (GetScroller ());

	Point	oldOffset = fLocalToTabletOffset;
	Point	oldWhereViewing = GetScroller ()->GetWhereViewing ();
	GetScroller ()->ScrollSoShowing (whatToShow, forceFullyShown, Panel::eImmediateUpdate);
	if (oldWhereViewing != GetScroller ()->GetWhereViewing ()) {
		fLocalToTabletOffset = oldOffset - GetScroller ()->GetWhereViewing () + oldWhereViewing;
		SetFeedbackClip (GetFeedbackClip () + (oldOffset - fLocalToTabletOffset));
		RequireNotNil (GetTablet ());
		GetTablet ()->SetClip (LocalToTablet (GetFeedbackClip ()));
	}
}





/*
 ********************************************************************************
 ********************************** ShapeSizer **********************************
 ********************************************************************************
 */
const	Point	ShapeSizer::kDefaultHandleSize	= Point (6, 6);

ShapeSizer::ShapeSizer (const Shape& shape, const Rect& shapeExtent, View& enclosure, 
					const Point& minSize, const Point& maxSize, SizeDirection sizeDirection):
	Tracker (enclosure, *enclosure.GetTablet (), enclosure.LocalToTablet (kZeroPoint)),
	fOldExtent (shapeExtent),
	fNewExtent (shapeExtent),
	fSizeDirection (eDefault),
	fMinSize (minSize),
	fMaxSize (maxSize),
	fHandleSize (kDefaultHandleSize),
	fSizeConstraint (eNoConstraint)
{
	Require (GetOldShapeExtent ().GetHeight () >= fMinSize.GetV ());
	Require (GetOldShapeExtent ().GetWidth () >= fMinSize.GetH ());
	Require (GetOldShapeExtent ().GetHeight () <= fMaxSize.GetV ());
	Require (GetOldShapeExtent ().GetWidth () <= fMaxSize.GetH ());

#if		qXToolkit
	SetShape (Rectangle ());	// since the region stuff works pretty badly...
#else
	SetShape (RegionShape (shape.ToRegion (Rect (kZeroPoint, Point (100, 100)))));
#endif
	SetSizeDirection (sizeDirection);
}

ShapeSizer::ShapeSizer (const Shape& shape, const Rect& shapeExtent, Panel& enclosure,
						Tablet& tablet, const Point& localToTabletOffset,
						const Point& minSize, const Point& maxSize, SizeDirection sizeDirection):
	Tracker (enclosure, tablet, localToTabletOffset),
	fOldExtent (shapeExtent),
	fNewExtent (shapeExtent),
	fSizeDirection (eDefault),
	fMinSize (minSize),
	fMaxSize (maxSize),
	fHandleSize (kDefaultHandleSize),
	fSizeConstraint (eNoConstraint)
{
	Require (GetOldShapeExtent ().GetHeight () >= fMinSize.GetV ());
	Require (GetOldShapeExtent ().GetWidth () >= fMinSize.GetH ());
	Require (GetOldShapeExtent ().GetHeight () <= fMaxSize.GetV ());
	Require (GetOldShapeExtent ().GetWidth () <= fMaxSize.GetH ());

#if		qXToolkit
	SetShape (Rectangle ());	// since the region stuff works pretty badly...
#else
	SetShape (RegionShape (shape.ToRegion (Rect (kZeroPoint, Point (100, 100)))));
#endif
	SetSizeDirection (sizeDirection);
}

Rect	ShapeSizer::GetOldShapeExtent () const
{
	return (fOldExtent);
}

Rect	ShapeSizer::GetNewShapeExtent () const
{
	return (fNewExtent);
}

void	ShapeSizer::SetNewShapeExtent (const Rect& newExtent)
{
	fNewExtent = newExtent;
}

Point	ShapeSizer::GetHandleSize () const
{
	return (fHandleSize);
}

void	ShapeSizer::SetHandleSize (const Point& handleSize)
{
	fHandleSize = handleSize;
}

ShapeSizer::SizeDirection		ShapeSizer::GetSizeDirection () const
{
	return (fSizeDirection);
}

void	ShapeSizer::SetSizeDirection (SizeDirection sizeDirection)
{
	fSizeDirection = sizeDirection;
	if ((fSizeDirection == eLeft) or (fSizeDirection == eRight)) {
		SetSizeConstraint (eHorizontal);
	}
	else if ((fSizeDirection == eTop) or (fSizeDirection == eBottom)) {
		SetSizeConstraint (eVertical);
	}
}

ShapeSizer::SizeConstraint	ShapeSizer::GetSizeConstraint () const
{
	return (fSizeConstraint);
}

void	ShapeSizer::SetSizeConstraint (SizeConstraint sizeConstraint)
{
	fSizeConstraint = sizeConstraint;
}

ShapeSizer::SizeDirection	ShapeSizer::CalcSizeDirection (const Point& anchor)	const
{
	for (SizeDirection direction = eBotRight; direction <= eRight; direction++) {
		if (GetHandle (direction).Contains (anchor)) {
			return (direction);
		}
	}
	AssertNotReached ();	return (eBotRight);
}

Rect	ShapeSizer::GetHandle (SizeDirection sizeDirection)	const
{
	Rect	extent	= GetOldShapeExtent ();
	Point	center	= CalcRectCenter (extent);
	Point	delta	= kZeroPoint;
	switch (sizeDirection) {
		case	eLeft:		
			delta.SetV (center.GetV () - GetHandleSize ().GetV ()/2);
			delta.SetH (extent.GetLeft ());
			break;
		case	eTopLeft:	
			delta = extent.GetTopLeft ();	
			break;
		case	eTop:
			delta.SetV (extent.GetTop ());
			delta.SetH (center.GetH () - GetHandleSize ().GetH ()/2);
			break;
		case	eTopRight:	
			delta = extent.GetTopRight () - Point (0, GetHandleSize ().GetH ());	
			break;
		case	eRight:
			delta.SetV (center.GetV () - GetHandleSize ().GetV ()/2);
			delta.SetH (extent.GetRight ());
			break;
		case	eBotRight:	
			delta = extent.GetBotRight () - GetHandleSize ();	
			break;
		case	eBottom:
			delta.SetV (extent.GetBottom ());
			delta.SetH (center.GetH () - GetHandleSize ().GetH ()/2);
			break;
		case	eBotLeft:	
			delta = extent.GetBotLeft () - Point (GetHandleSize ().GetV (), 0);	
			break;
		default:	RequireNotReached ();
	}
	return (Rect (delta, GetHandleSize ()));
}

Rect	ShapeSizer::CalcNewShapeExtent (const Point& anchor, const Point& next)
{
	Rect	newExtent = GetOldShapeExtent ();
	Point	delta = next - anchor;
	switch (GetSizeDirection ()) {
		case eTopLeft: {
			newExtent = Rect (newExtent.GetOrigin () + delta, newExtent.GetSize () - delta);
		}
		break;
		case eTop: {
			Assert (delta.GetH () == 0);
			newExtent = Rect (newExtent.GetOrigin () + delta, newExtent.GetSize () - delta);
		}
		break;
		case eTopRight:	 {
			newExtent = Rect (
				newExtent.GetOrigin () + Point (delta.GetV (), 0), 
				newExtent.GetSize ()   + Point (-delta.GetV (), delta.GetH ()));	
		}
		break;
		case eRight: {
			Assert (delta.GetV () == 0);
			newExtent = Rect (newExtent.GetOrigin (), newExtent.GetSize () + delta);
		}
		break;
		case eBotRight: {
			newExtent = Rect (newExtent.GetOrigin (), newExtent.GetSize () + delta);	
		}
		break;
		case eBottom: {
			Assert (delta.GetH () == 0);
			newExtent = Rect (newExtent.GetOrigin (), newExtent.GetSize () + delta);
		}
		break;
		case eBotLeft: {	
			newExtent = Rect (
				newExtent.GetOrigin () + Point (0, delta.GetH ()), 
				newExtent.GetSize ()   + Point (delta.GetV (), -delta.GetH ()));	
		}
		break;
		case eLeft: {
			Assert (delta.GetV () == 0);
			newExtent = Rect (newExtent.GetOrigin () + delta, newExtent.GetSize () - delta);	
		}
		break;
		default: {
			AssertNotReached ();
		}
		break;
	}
	return (newExtent);
}

void	ShapeSizer::TrackConstrain (TrackPhase /*phase*/, const Point& anchor, const Point& previous, Point& next)
{
	if (next != previous) {
		if (GetSizeDirection () == eDefault) {
			if (GetKeyBoard ().GetKey (KeyBoard::eOptionKey)) {
				Point	delta = Abs (next - previous);
				SetSizeConstraint ((delta.GetV () >= delta.GetH ()) ? eVertical : eHorizontal);
			}
			SetSizeDirection (CalcSizeDirection (anchor));
		}
		
		switch (GetSizeConstraint ()) {
			case eNoConstraint:	break;
			case eVertical:		next.SetH (previous.GetH ());	break;
			case eHorizontal:	next.SetV (previous.GetV ());	break;
			default:			AssertNotReached ();
		}

		Rect	newExtent = CalcNewShapeExtent (anchor, next);
		if (newExtent.GetWidth () < fMinSize.GetH ()) {
			if (GetSizeDirection () == eBotRight) {
				next.SetH ((fOldExtent.GetOrigin () + fMinSize).GetH ());
			}
			else {
				next.SetH (previous.GetH ());
			}	
		}
		else if (newExtent.GetWidth () > fMaxSize.GetH ()) {
			if (GetSizeDirection () == eBotRight) {
				next.SetH ((fOldExtent.GetOrigin () + fMaxSize).GetH ());
			}
			else {
				next.SetH (previous.GetH ());
			}
		}
		
		if (newExtent.GetHeight () < fMinSize.GetV ()) {
			if (GetSizeDirection () == eBotRight) {
				next.SetV ((fOldExtent.GetOrigin () + fMinSize).GetV ());
			}
			else {
				next.SetV (previous.GetV ());
			}
		}
		else if (newExtent.GetHeight () > fMaxSize.GetV ()) {
			if (GetSizeDirection () == eBotRight) {
				next.SetV ((fOldExtent.GetOrigin () + fMaxSize).GetV ());
			}
			else {
				next.SetV (previous.GetV ());
			}
		}
	}
}

void	ShapeSizer::TrackFeedback (Tracker::TrackPhase phase, const Point& anchor, const Point& /*previous*/, const Point& next)
{
	OutLine (GetShape (), GetNewShapeExtent (), GetFeedBackPen ());
	if (phase == eTrackMove) {
		SetNewShapeExtent (CalcNewShapeExtent (anchor, next));
		
		if (GetScroller () != Nil) {
			ScrollSoShowing (GetNewShapeExtent (), True);
		}
		OutLine (GetShape (), GetNewShapeExtent (), GetFeedBackPen ());
	}
}

Boolean	ShapeSizer::TrackResult (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/, const Point& /*next*/)
{
	return (False);
}







/*
 ********************************************************************************
 ************************************ ViewSizer *********************************
 ********************************************************************************
 */
ViewSizer::ViewSizer (View& view, const Point& minSize, const Point& maxSize, ShapeSizer::SizeDirection sizeDirection):
	ShapeSizer (view.GetShape (), view.GetExtent (), *view.GetParentView (), minSize, maxSize, sizeDirection),
	fView (view)
{
}

Boolean		ViewSizer::TrackResult (Tracker::TrackPhase phase, const Point& /*anchor*/, const Point& /*previous*/, const Point& /*next*/)
{
	if (phase == eTrackRelease) {
		SizeViewCommand*	command = new SizeViewCommand (fView, "Resize", 
			fView.GetOrigin () + GetNewShapeExtent ().GetOrigin () - GetOldShapeExtent ().GetOrigin (),
			fView.GetSize () + GetNewShapeExtent ().GetSize () - GetOldShapeExtent ().GetSize ());
		command->DoIt ();
		PostCommand (command);
	}
	return (False);
}





/*
 ********************************************************************************
 ********************************** ShapeDragger ********************************
 ********************************************************************************
 */
const	Point	ShapeDragger::kNoSlop	= Point (-1, -1);
const	Point	ShapeDragger::kHugeSlop = kMaxPoint;
const	Point	ShapeDragger::kNoGrid	= Point (1, 1);


ShapeDragger::ShapeDragger (const Shape& shape, const Rect& shapeExtent, View& enclosure, DragDirection dragDirection):
	Tracker (enclosure, *enclosure.GetTablet (), enclosure.LocalToTablet (kZeroPoint)),
	fOldExtent (shapeExtent),
	fNewExtent (shapeExtent),
	fDragDirection (dragDirection),
	fBounds (enclosure.GetLocalRegion ()),
	fSlop (kHugeSlop),
	fGrid (kNoGrid)
{
#if		qXToolkit
	SetShape (Rectangle ());// since region stuff broken...
#else
	SetShape (RegionShape (shape.ToRegion (Rect (kZeroPoint, shapeExtent.GetSize ()))));
#endif
}

ShapeDragger::ShapeDragger (const Shape& shape, const Rect& shapeExtent, Panel& enclosure, Tablet& tablet, const Point& localToTabletOffset, DragDirection dragDirection):
	Tracker (enclosure, tablet, localToTabletOffset),
	fOldExtent (shapeExtent),
	fNewExtent (shapeExtent),
	fDragDirection (dragDirection),
	fBounds (enclosure.GetLocalRegion ()),
	fSlop (kHugeSlop),
	fGrid (kNoGrid)
{
#if		qXToolkit
	SetShape (Rectangle ());// since region stuff broken...
#else
	SetShape (RegionShape (shape.ToRegion (Rect (kZeroPoint, shapeExtent.GetSize ()))));
#endif
}

Rect	ShapeDragger::GetOldShapeExtent () const
{
	return (fOldExtent);
}

Rect	ShapeDragger::GetNewShapeExtent () const
{
	return (fNewExtent);
}

Region	ShapeDragger::GetBounds () const
{
	return (fBounds);
}

void	ShapeDragger::SetBounds (const Region& bounds)
{
	fBounds = bounds;
}

void	ShapeDragger::SetNewShapeExtent (const Rect& newExtent)
{
	fNewExtent = newExtent;
}

ShapeDragger::DragDirection	ShapeDragger::GetDragDirection () const
{
	return (fDragDirection);
}

void	ShapeDragger::SetDragDirection (DragDirection dragDirection)
{
	fDragDirection = dragDirection;
}

Point	ShapeDragger::GetGrid () const
{
	return (fGrid);
}

void	ShapeDragger::SetGrid (const Point& grid)
{
	fGrid = grid;
}

Point	ShapeDragger::GetSlop () const
{
	return (fSlop);
}

void	ShapeDragger::SetSlop (const Point& slop)
{
	fSlop = slop;
}

Rect	ShapeDragger::CalcNewShapeExtent (const Point& anchor, const Point& next)
{
	return (GetOldShapeExtent () + next - anchor);
}

Boolean	ShapeDragger::WithinSlop (const Point& p)
{
	if (fSlop == kHugeSlop) {
		return (True);
	}
	else if (GetScroller () != Nil) {
		return (True);	// not really right, have to see if can be made viewed
	}
	else if (fSlop == kNoSlop) {
		return (fBounds.Contains (p));
	}
	else {
		Region	r = Region (fBounds);
		r.SetBounds (InsetBy (r.GetBounds (), -fSlop));
		return (r.Contains (p));
	}
}

void	ShapeDragger::TrackConstrain (Tracker::TrackPhase /*phase*/, const Point& anchor, const Point& previous, Point& next)
{	
	if (not WithinSlop (next)) {
		return;
	}
	
	if (next != previous) {
		if (GetDragDirection () == eDefault) {
			if (GetKeyBoard ().GetKey (KeyBoard::eOptionKey)) {
				Point	delta = Abs (next - previous);
				if (delta.GetV () >= delta.GetH ()) {
					SetDragDirection (eVertical);
				}
				else {
					SetDragDirection (eHorizontal);
				}
			}
			else {
				SetDragDirection (eAny);
			}
		}
	
		switch (GetDragDirection ()) {
			case eAny:			break;
			case eVertical:		next.SetH (previous.GetH ());	break;
			case eHorizontal:	next.SetV (previous.GetV ());	break;
			default:			AssertNotReached ();
		}
		
		Point	newOrigin = CalcNewShapeExtent (anchor, next).GetOrigin ();
		Point	anchorOffset = anchor - GetOldShapeExtent ().GetOrigin ();
		
		if (GetScroller () == Nil) {	// actually need fancier test if have a scroller but still out of bounds
			if (newOrigin.GetH () < fBounds.GetBounds ().GetLeft ()) {
				next.SetH (fBounds.GetBounds ().GetLeft () + anchorOffset.GetH ());
			}
			else if ((next.GetH ()) > fBounds.GetBounds ().GetRight ()) {
				next.SetH (fBounds.GetBounds ().GetRight ());
			}
		
			if (newOrigin.GetV () < fBounds.GetBounds ().GetTop ()) {
				next.SetV (fBounds.GetBounds ().GetTop () + anchorOffset.GetV ());
			}
			else if ((next.GetV ()) > fBounds.GetBounds ().GetBottom ()) {
				next.SetV (fBounds.GetBounds ().GetBottom ());
			}
		}
		
		if (next != previous) {
			next = anchor + RoundBy (next - anchor, fGrid);
		}
	}
}

void	ShapeDragger::TrackFeedback (Tracker::TrackPhase phase, const Point& anchor, const Point& previous, const Point& next)
{
	if (WithinSlop (previous)) {
		OutLine (GetShape (), GetNewShapeExtent (), GetFeedBackPen ());
	}
	if (phase == eTrackMove) {
		SetNewShapeExtent (CalcNewShapeExtent (anchor, next));
		if (GetScroller () != Nil) {
			ScrollSoShowing (GetNewShapeExtent (), True);
		}
		if (WithinSlop (next)) {
			OutLine (GetShape (), GetNewShapeExtent (), GetFeedBackPen ());
		}
	}
}

Boolean	ShapeDragger::TrackResult (Tracker::TrackPhase phase, const Point& anchor, const Point& /*previous*/, const Point& next)
{
	Assert (WithinSlop (anchor));
	if ((phase == eTrackRelease) and (not WithinSlop (next))) {
		SetNewShapeExtent (CalcNewShapeExtent (anchor, anchor));
	}
	return (False);
}









/*
 ********************************************************************************
 ********************************** ViewDragger *********************************
 ********************************************************************************
 */
ViewDragger::ViewDragger (View& view, ShapeDragger::DragDirection dragDirection):
	ShapeDragger (view.GetShape (), view.GetExtent (), *view.GetParentView (), dragDirection),
	fView (view)
{
}

Boolean		ViewDragger::TrackResult (Tracker::TrackPhase phase, const Point& anchor, const Point& previous, const Point& next)
{
	ShapeDragger::TrackResult (phase, anchor, previous, next);
	if (phase == eTrackRelease) {
		SizeViewCommand*	command = new SizeViewCommand (fView, "Move", GetNewShapeExtent ().GetOrigin (), fView.GetSize ());
		command->DoIt ();
		PostCommand (command);
	}
	return (False);
}








/*
 ********************************************************************************
 ************************************ Selector **********************************
 ********************************************************************************
 */
Selector::Selector (View& enclosure, const Shape& shape, const Region& bounds, Boolean animate):
	Tracker (enclosure, *enclosure.GetTablet (), enclosure.LocalToTablet (kZeroPoint)),
	fBounds (bounds),
	fAnimate (animate),
	fPatternIndex (0),
	fCurrentPen (Nil),
	fSelection (kZeroRect)
{
#if		qXToolkit
	SetShape (Rectangle ());
#else
	SetShape (RegionShape (shape.ToRegion (Rect (kZeroPoint, Point (100, 100))))); // really need a way to clone shapes
#endif
	if (fAnimate) {
		fCurrentPen = new Pen (kMarqueArray[fPatternIndex], eNotXorTMode, Point (1, 1));
		SetFeedBackPen (*fCurrentPen);
		SetTrackNonMovement (True);
#if		qSunCFront_OverloadingConfusionBug
		SetFeedBackDelay ((BigReal).05);
#else
		SetFeedBackDelay (.05);
#endif
	}
}

Selector::Selector (Panel& enclosure, Tablet& tablet, const Point& localToTabletOffset, const Shape& shape, const Region& bounds, Boolean animate):
	Tracker (enclosure, tablet, localToTabletOffset),
	fBounds (bounds),
	fAnimate (animate),
	fPatternIndex (0),
	fCurrentPen (Nil),
	fSelection (kZeroRect)
{
#if		qXToolkit
	SetShape (Rectangle ());
#else
	SetShape (RegionShape (shape.ToRegion (Rect (kZeroPoint, Point (100, 100))))); // really need a way to clone shapes
#endif
	if (fAnimate) {
		fCurrentPen = new Pen (kMarqueArray[fPatternIndex], eNotXorTMode, Point (1, 1));
		SetFeedBackPen (*fCurrentPen);
		SetTrackNonMovement (True);
#if		qSunCFront_OverloadingConfusionBug
		SetFeedBackDelay ((BigReal).05);
#else
		SetFeedBackDelay (.05);
#endif
	}
}

Region	Selector::GetBounds () const
{
	return (fBounds);
}

void	Selector::SetBounds (const Region& bounds)
{
	fBounds = bounds;
}

void	Selector::TrackConstrain (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& previous, Point& next)
{
	if (GetScroller () == Nil) {	// actually need fancier test if have a scroller but still out of bounds
		if (not (fBounds.Contains (next))) {
			next = previous;
		}
		Ensure (fBounds.Contains (next));
	}
}

Rect	Selector::GetSelection () const
{
	return (fSelection);
}

void	Selector::TrackFeedback (Tracker::TrackPhase phase, const Point& anchor, const Point& /*previous*/, const Point& next)
{
	if (phase == eTrackPress) {
		fSelection  = BoundingRect (anchor, next);
	}
	OutLine (GetShape (), fSelection, GetFeedBackPen ());
	if (phase == eTrackMove) {
		fSelection  = BoundingRect (anchor, next);
		if (fAnimate) {
			if (++fPatternIndex > 3) {
				fPatternIndex = 0;
			}
			delete fCurrentPen;	
			fCurrentPen = new Pen (kMarqueArray[fPatternIndex], eNotXorTMode, Point (1, 1));
			SetFeedBackPen (*fCurrentPen);
		}
		OutLine (GetShape (), fSelection, GetFeedBackPen ());
	}
}

Boolean	Selector::TrackResult (Tracker::TrackPhase phase, const Point& /*anchor*/, const Point& /*previous*/, const Point& /*next*/)
{
	if (phase == eTrackRelease) {
		delete fCurrentPen;
	}
	return (False);
}








/*
 ********************************************************************************
 ********************************** LineBuilder *********************************
 ********************************************************************************
 */
LineBuilder::LineBuilder (View& enclosure, const Region& bounds):
	Tracker (enclosure, *enclosure.GetTablet (), enclosure.LocalToTablet (kZeroPoint)),
	fStartPoint (kZeroPoint),
	fEndPoint (kZeroPoint),
	fBounds (bounds),
	fLine (kZeroPoint, kZeroPoint)
{
	SetShape (fLine);
	SetFeedBackPen (Pen (kBlackTile, eNotXorTMode, Point (1, 1)));
}

LineBuilder::LineBuilder (Panel& enclosure, Tablet& tablet, const Point& localToTabletOffset, const Region& bounds):
	Tracker (enclosure, tablet, localToTabletOffset),
	fStartPoint (kZeroPoint),
	fEndPoint (kZeroPoint),
	fBounds (bounds),
	fLine (kZeroPoint, kZeroPoint)
{
	SetShape (fLine);
	SetFeedBackPen (Pen (kBlackTile, eNotXorTMode, Point (1, 1)));
}

Region	LineBuilder::GetBounds () const
{
	return (fBounds);
}

void	LineBuilder::SetBounds (const Region& bounds)
{
	fBounds = bounds;
}

void	LineBuilder::TrackConstrain (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/, Point& next)
{
	if (GetScroller () == Nil) {	// actually need fancier test if have a scroller but still out of bounds
		if (next.GetH () < fBounds.GetBounds ().GetLeft ()) {
			next.SetH (fBounds.GetBounds ().GetLeft ());
		}
		else if (next.GetH () > fBounds.GetBounds ().GetRight ()) {
			next.SetH (fBounds.GetBounds ().GetRight ());
		}
	
		if (next.GetV () < fBounds.GetBounds ().GetTop ()) {
			next.SetV (fBounds.GetBounds ().GetTop ());
		}
		else if (next.GetV () > fBounds.GetBounds ().GetBottom ()) {
			next.SetV (fBounds.GetBounds ().GetBottom ());
		}
	}
}

void	LineBuilder::TrackFeedback (Tracker::TrackPhase phase, const Point& anchor, const Point& /*previous*/, const Point& next)
{
	if (phase == eTrackPress) {
		fStartPoint = anchor;
	}
	
	OutLine (GetShape (), GetExtent (), GetFeedBackPen ());
	if (phase == eTrackMove) {
		fEndPoint = next;
#if 1
		SetShape (fLine = Line (fStartPoint, fEndPoint));
#else
		fLine->SetTo (fEndPoint);
#endif
		if (GetScroller () != Nil) {
			ScrollSoShowing (GetLocalExtent (), True);
		}
		OutLine (GetShape (), GetLocalExtent (), GetFeedBackPen ());
	}
}

Boolean	LineBuilder::TrackResult (Tracker::TrackPhase /*phase*/, const Point& /*anchor*/, const Point& /*previous*/, const Point& /*next*/)
{
	return (False);
}

Point	LineBuilder::GetStartPoint () const
{
	return (fStartPoint);
}

Point	LineBuilder::GetEndPoint () const
{
	return (fEndPoint);
}








/*
 ********************************************************************************
 ****************************** SizeViewCommand *********************************
 ********************************************************************************
 */

SizeViewCommand::SizeViewCommand (View& view, const String& name, const Point& newOrigin, const Point& newSize):
	Command (CommandHandler::eNoCommand, Command::kUndoable),
	fView (view),
	fOldOrigin (view.GetOrigin ()),
	fNewOrigin (newOrigin),
	fOldSize (view.GetSize ()),
	fNewSize (newSize)
{	
	SetName (name);
}

void	SizeViewCommand::DoIt () 
{
	fView.SetExtent (Rect (fNewOrigin, fNewSize));
	Command::DoIt ();
}

void	SizeViewCommand::UnDoIt () 
{
	fView.SetExtent (Rect (fOldOrigin, fOldSize));
	Command::UnDoIt ();
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

