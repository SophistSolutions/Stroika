/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Tracker__
#define	__Tracker__

/*
 * $Header: /fuji/lewis/RCS/Tracker.hh,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		<STERL - THIS COULD USE A GOOD EXPLANATION- AND YOUR THE ONLY ONE WHO CAN DO THIS SECTION!!>
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Tracker.hh,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/04  14:11:38  lewis
 *		Keep the line in LineBuilder by value - not ptr. Taking advantage of new
 *		letter/env support in Shape.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/04/30  14:48:13  sterling
 *		added Get and SetBounds to a bunch of trackers
 *
 *		Revision 1.10  92/03/05  20:07:26  20:07:26  sterling (Sterling Wight)
 *		scroller aware
 *		
 *		Revision 1.9  1992/01/15  10:06:08  lewis
 *		Inherit from Panel, for now, but later change to inerinting from Enclosure.
 *
 *		Revision 1.8  1992/01/14  06:05:14  lewis
 *		Made Tracker be a TabletOwner, and change ctors to take seperate Tablet/origin when constructed from
 *		a Panel. Leave same sttyle "short" constructor when we know the type of the Panel is View.
 *
 *		Revision 1.7  1992/01/08  05:37:34  lewis
 *		Inherit from EventHandler instead of EventManager after change in how these
 *		work. Also, make kDefaultTimeout scoped.
 *
 *
 *
 *
 */

#include	"Time.hh"

#include	"Shape.hh"
#include	"Tablet.hh"

#include	"Command.hh"
#include	"EventManager.hh"
#include	"MouseHandler.hh"




class	Scroller;

class	Tracker : public MouseHandler, public Panel, public EventHandler, public TabletOwner {
	public:
		static	const	Time	kDefaultTimeOut;
		enum	TrackPhase {
			eTrackPress,
			eTrackMove,
			eTrackRelease,
		};

		/*
		 * As tracking pens, both Pen (kGrayTile, tmNotXor), and
		 * Pen (kGrayTile, eXorTMode); are good, but tmNotXor is better on light
		 * backgrounds (sounds right - at least thats what apple usually uses).
		 */
		static	const	Pen	kDefaultFeedbackPen;

	protected:
		Tracker (Panel& enclosure, Tablet& tablet, const Point& localToTabletOffset);

	public:
		override	Tablet*	GetTablet () const;
		override	Panel*	GetParentPanel () const;

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState);
		
		nonvirtual	Boolean	GetTrackNonMovement ()	const;
		virtual		void	SetTrackNonMovement (Boolean trackNonMovement);
		nonvirtual	Point	GetHysteresis ()	const;
		virtual		void	SetHysteresis (const Point& hysteresis);
		nonvirtual	Time	GetTimeOut ()	const;
		virtual		void	SetTimeOut (Time timeOut);
		nonvirtual	Pen		GetFeedBackPen ()	const;
		virtual		void	SetFeedBackPen (const Pen& feedBackPen);
		nonvirtual	Time	GetFeedBackDelay () const;
		nonvirtual	void	SetFeedBackDelay (const Time& delay);

		// draggers are "scroller aware" so that they can support autoscrolling while tracking
		nonvirtual	Scroller*	GetScroller () const;
		nonvirtual	void		SetScroller (Scroller* scroller);

	protected:
		virtual		void	TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next)			= Nil;
		virtual		void	TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next)		= Nil;
		virtual		Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next)		= Nil;

		virtual	Boolean		BeginTracking (const Point& anchor, const Point& current);		
		nonvirtual	Boolean	TrackOnce (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);

		override	void	ProcessMouseMoved (const osEventRecord& eventRecord, const Point& newLocation, const KeyBoard& keyBoardState, Region& newMouseRegion);

		nonvirtual	Region	GetFeedbackClip () const;
		nonvirtual	void	SetFeedbackClip (const Region& clipTo);

		override	Point	LocalToTablet_ (const Point& p)	const;
		override	Point	TabletToLocal_ (const Point& p)	const;

		virtual		void	ScrollSoShowing (const Rect& whatToShow, Boolean forceFullyShown);

	private:
		Panel&		fEnclosure;
		Boolean		fTrackNonMovement;
		Point		fHysteresis;
		Time		fTimeOut;
		Time		fStartingTime;
		Pen			fFeedBackPen;
		Time		fFeedBackDelay;
		Region		fFeedbackClip;
		Tablet&		fTablet;
		Point		fLocalToTabletOffset;
		Scroller*	fScroller;
};



class	View;
class	ShapeSizer : public Tracker {
	public:
		enum	SizeDirection {
			eDefault,
			eBotRight,
			eBottom,
			eBotLeft,
			eLeft,
			eTopLeft,
			eTop,
			eTopRight,
			eRight,
		};
		enum	SizeConstraint {
			eNoConstraint,
			eVertical,
			eHorizontal,
		};
		static	const	Point	kDefaultHandleSize;



		ShapeSizer (const Shape& shape, const Rect& shapeExtent, View& enclosure, 
					const Point& minSize, const Point& maxSize, SizeDirection sizeDirection = eDefault);
		ShapeSizer (const Shape& shape, const Rect& shapeExtent, Panel& enclosure,
					Tablet& tablet, const Point& localToTabletOffset,
					const Point& minSize, const Point& maxSize, SizeDirection sizeDirection = eDefault);

		nonvirtual	SizeDirection	GetSizeDirection () const;
		virtual		void			SetSizeDirection (SizeDirection sizeDirection);

		nonvirtual	Rect	GetOldShapeExtent () const;
		nonvirtual	Rect	GetNewShapeExtent () const;
		
		nonvirtual	Point	GetHandleSize () const;
		virtual		void	SetHandleSize (const Point& handleSize);
		
		nonvirtual	SizeConstraint	GetSizeConstraint () const;
		virtual		void			SetSizeConstraint (SizeConstraint);

	protected:
		override	void	TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);
		override	void	TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
	
		virtual		Rect	CalcNewShapeExtent (const Point& anchor, const Point& next);
		nonvirtual	void	SetNewShapeExtent (const Rect& newExtent);

		virtual		SizeDirection	CalcSizeDirection (const Point& anchor)	const;
		nonvirtual	Rect			GetHandle (SizeDirection sizeDirection)	const;
	private:
		Point			fMinSize;
		Point			fMaxSize;
		Rect			fOldExtent;
		Rect			fNewExtent;
		SizeDirection	fSizeDirection;
		Point			fHandleSize;
		SizeConstraint	fSizeConstraint;
};





class	ShapeDragger : public Tracker {
	public:
		enum	DragDirection {
			eDefault,
			eAny,
			eVertical,
			eHorizontal
		};
		static	const	Point	kNoSlop;
		static	const	Point	kHugeSlop;
		static	const	Point	kNoGrid;

		ShapeDragger (const Shape& shape, const Rect& shapeExtent, View& enclosure, 
					  DragDirection dragDirection = eDefault);
		ShapeDragger (const Shape& shape, const Rect& shapeExtent, Panel& enclosure, 
					  Tablet& tablet, const Point& localToTabletOffset,
					  DragDirection dragDirection = eDefault);

		nonvirtual	DragDirection	GetDragDirection () const;
		virtual		void			SetDragDirection (DragDirection dragDirection);
		
		nonvirtual	Point	GetGrid () const;
		virtual		void	SetGrid (const Point& grid);

		nonvirtual	Rect	GetOldShapeExtent () const;
		nonvirtual	Rect	GetNewShapeExtent () const;
		
		nonvirtual	Point	GetSlop () const;
		virtual		void	SetSlop (const Point& slop);

		nonvirtual	Region	GetBounds () const;
		nonvirtual	void	SetBounds (const Region& bounds);

	protected:
		override	void	TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);
		override	void	TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

		virtual		Rect	CalcNewShapeExtent (const Point& anchor, const Point& next);
		nonvirtual	void	SetNewShapeExtent (const Rect& newExtent);

		virtual		Boolean	WithinSlop (const Point& p);

	private:
		Region			fBounds;
		Rect			fOldExtent;
		Rect			fNewExtent;
		DragDirection	fDragDirection;
		Point			fSlop;
		Point			fGrid;
};


class	View;
#if		qCanFreelyUseVirtualBaseClasses
class	ViewSizer : public ShapeSizer {
#else
class	ViewSizer : public ShapeSizer, public CommandHandler {
#endif
	public:
		ViewSizer (View& view, const Point& minSize, const Point& maxSize, SizeDirection sizeDirection = ShapeSizer::eDefault);

	protected:
		override	Boolean		TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

	private:
		View&			fView;
};

#if		qCanFreelyUseVirtualBaseClasses
class	ViewDragger : public ShapeDragger {
#else
class	ViewDragger : public ShapeDragger, public CommandHandler {
#endif
	public:
		ViewDragger (View& view, DragDirection dragDirection = ShapeDragger::eDefault);

	protected:
		override	Boolean		TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

	private:
		View&	fView;
};

class	Selector : public Tracker {
	public:
		Selector (View& enclosure, const Shape& shape, const Region& bounds, Boolean animate = False);
		Selector (Panel& enclosure, Tablet& tablet, const Point& localToTabletOffset, const Shape& shape, const Region& bounds, Boolean animate = False);
		
		nonvirtual	Rect	GetSelection () const;

		nonvirtual	Region	GetBounds () const;
		nonvirtual	void	SetBounds (const Region& bounds);

	protected:
		override	void	TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);
		override	void	TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

	private:
		Region	fBounds;
		Boolean	fAnimate;
		Int8	fPatternIndex;
		Pen*	fCurrentPen;
		Rect	fSelection;

#if		qMPW_CLASS_SCOPED_ARRAYOFOBJS_WITH_NO_NOARG_CTOR_BUG
		static	const	Tile*	kMarqueArray;
#else
		static	const	Tile	kMarqueArray [4];
#endif
};


class	Line;
class	LineBuilder : public Tracker {
	public:
		LineBuilder (View& enclosure, const Region& bounds);
		LineBuilder (Panel& enclosure, Tablet& tablet, const Point& localToTabletOffset, const Region& bounds);

		nonvirtual	Point	GetStartPoint () const;
		nonvirtual	Point	GetEndPoint () const;

		nonvirtual	Region	GetBounds () const;
		nonvirtual	void	SetBounds (const Region& bounds);

	protected:
		override	void	TrackConstrain (TrackPhase phase, const Point& anchor, const Point& previous, Point& next);
		override	void	TrackFeedback (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);
		override	Boolean	TrackResult (TrackPhase phase, const Point& anchor, const Point& previous, const Point& next);

	private:
		Point	fStartPoint;
		Point	fEndPoint;
		Region	fBounds;
		Line	fLine;
};

class	SizeViewCommand : public Command {
	public:
		SizeViewCommand (View& view, const String& name, const Point& newOrigin, const Point& newSize);
		
		override	void	DoIt ();
		override	void	UnDoIt () ;

	private:
		View&		fView;
		Point		fNewOrigin;
		Point		fOldOrigin;
		Point		fNewSize;
		Point		fOldSize;
};



/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Boolean	Tracker::GetTrackNonMovement ()	const			{ return (fTrackNonMovement);	}
inline	Point	Tracker::GetHysteresis ()	const				{ return (fHysteresis);			}
inline	Time	Tracker::GetTimeOut ()	const					{ return (fTimeOut);			}
inline	Pen		Tracker::GetFeedBackPen ()	const				{ return (fFeedBackPen);		}
inline	Time	Tracker::GetFeedBackDelay () const				{ return (fFeedBackDelay);		}
inline	void	Tracker::SetFeedBackDelay (const Time& delay)	{ fFeedBackDelay = delay;		}
inline	Region	Tracker::GetFeedbackClip () const				{ return (fFeedbackClip);		}
inline	void	Tracker::SetFeedbackClip (const Region& clipTo)	{ fFeedbackClip = clipTo;		}




// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__Tracker__*/

