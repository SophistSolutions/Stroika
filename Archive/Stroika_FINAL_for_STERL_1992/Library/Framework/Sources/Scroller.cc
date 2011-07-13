/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Scroller.cc,v 1.5 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: Scroller.cc,v $
 *		Revision 1.5  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.4  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.3  1992/07/03  02:14:39  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.2  1992/06/25  08:35:20  sterling
 *		Lots of changes - related to Slider defn changes and ????.
 *
 *		Revision 1.22  1992/05/19  10:37:34  sterling
 *		cleaned up Layout code, added GetScrollRect
 *
 *		Revision 1.21  92/05/13  12:40:44  12:40:44  lewis (Lewis Pringle)
 *		STERL - added new CTOR to Scroller().
 *		
 *		Revision 1.20  92/04/27  18:41:23  18:41:23  lewis (Lewis Pringle)
 *		Tried debugging scroller under X, and removing old hacks that made it work, but work badly..
 *		Using RepaintWhenScrolling (False), and no scrollbits. Only known bugs are scrollbits
 *		still must clear / erase exposed bits, and we are gettinga different callback on mac than X
 *		from scrollbars - not sure what the bug is here - must rething the callbacks that come with
 *		slider and see what makes sense....
 *		
 *		Revision 1.19  92/04/20  14:26:40  14:26:40  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		
 *		Revision 1.18  92/04/19  23:11:05  23:11:05  lewis (Lewis Pringle)
 *		Fixed flagrant display bug with hack to get scrollers to do clipping under Xt. Must do
 *		ExposeCallback and have that do Stroika refresh - not really sure why???
 *		
 *		Revision 1.17  92/04/17  17:06:16  17:06:16  lewis (Lewis Pringle)
 *		Added support to ScrollerMagicView to create a widget, which will clip our owned widgets for
 *		us. Sort of works. Better than the millions of other hacks i've tried.
 *		
 *		Revision 1.15  92/03/17  03:07:54  03:07:54  lewis (Lewis Pringle)
 *		Make mac and motif version same for layout in Scroller.
 *		
 *		Revision 1.14  1992/03/10  00:07:40  lewis
 *		Use new DispatchKeyEvent () interface instead of old HandleKey () interface.
 *
 *		Revision 1.13  1992/03/05  21:05:40  sterling
 *		support for borders
 *
 *		Revision 1.11  1992/02/04  04:58:34  sterling
 *		motif tweaks
 *
 *		Revision 1.10  1992/01/27  07:53:32  lewis
 *		For XGDI, set scrollbitswhilescrolling to false, to make less glaring some of our scrolling bugs.
 *
 *		Revision 1.7  1991/12/05  15:07:35  lewis
 *		Added scroller magic view class so we could hide more information in view.
 *
 */


#include	"OSRenamePre.hh"
#if		qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/IntrinsicP.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#if		qXmToolkit
#include	<Xm/DrawingA.h>
#endif	/*qXmToolkit*/
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"StreamUtils.hh"

#if		qXmToolkit
#include	"OSControls.hh"
#endif

#include	"ScrollBar.hh"

#include	"Scroller.hh"







/*
 ********************************************************************************
 ********************************* ScrollerMagicView ****************************
 ********************************************************************************
 */

ScrollerMagicView::ScrollerMagicView ():
	GroupView ()
#if		qXtToolkit
	,fMagicWidget (Nil)
#endif
{
}

ScrollerMagicView::~ScrollerMagicView ()
{
#if		qXtToolkit
	Require (fMagicWidget == Nil);
#endif
}

#if		qXtToolkit
static	void	ExposeCallBack (osWidget* widget, void* clientData, void* callData)
{
	RequireNotNil (widget);
	RequireNotNil (clientData);
	RequireNotNil (callData);
	XmDrawingAreaCallbackStruct*	cbs	=	(XmDrawingAreaCallbackStruct*)callData;
	RequireNotNil (cbs->event);		// Xm toolkit sometimes passes Nil, but never for expose callbacks???

	Rect	addToRegion	=	Rect (Point (cbs->event->xexpose.y, cbs->event->xexpose.x),
								  Point (cbs->event->xexpose.height, cbs->event->xexpose.width));

	// only do this for private update regions since upon getting an expose, generating a new one would be loopy!
//#if		qUsePrivateUpdateRegions
	/*
	 * Really we should find type of calldata and get rect from it.
	 */
	ScrollerMagicView* v = (ScrollerMagicView*)clientData;
	v->Refresh (addToRegion, View::eDelayedUpdate);
//#endif
}

void	ScrollerMagicView::Realize (osWidget* parent)
{
	RequireNotNil (parent);
	
	Arg args[3];
	XtSetArg (args[0], XmNmarginWidth, 0);
	XtSetArg (args[1], XmNmarginHeight, 0);
	XtSetArg (args[2], XmNshadowThickness, 0);

	fMagicWidget = ::XmCreateDrawingArea (parent, "ScrollerMagicWidget", args, 3);
	::XtRealizeWidget (fMagicWidget);
	::XtManageChild (fMagicWidget);
	AssertNotNil (XtWindow (fMagicWidget));

	::XtAddCallback (fMagicWidget, XmNexposeCallback, (XtCallbackProc)ExposeCallBack, (XtPointer)this);

	View::Realize (fMagicWidget);
}

void	ScrollerMagicView::UnRealize ()
{
	View::UnRealize ();
	if (fMagicWidget != Nil) {
		::XtDestroyWidget (fMagicWidget);
		fMagicWidget = Nil;
	}
}

osWidget*	ScrollerMagicView::GetWidget () const
{
	return (fMagicWidget);
}

void	ScrollerMagicView::Layout ()
{
	AdjustWidgetExtent ();
	GroupView::Layout ();
}

void	ScrollerMagicView::Draw (const Region& update)
{
	AdjustWidgetExtent ();
	GroupView::Draw (update);
}

void		ScrollerMagicView::AdjustWidgetExtent ()
{
	/*
	 * X generates protocol errors if we create zero sized windows - Xm blindly creates zero sized windows if we
	 * make widgets too small - nice...
	 *
	 * This isn't a very good solution, but it isn't really a very good problem either.
	 */
	Point	size	=	GetSize ();
	if (size.GetV () == 0) { size.SetV (2); }
	if (size.GetH () == 0) { size.SetH (2); }


	Point	effectiveOrigin		=	kZeroPoint;
	View*	parentWidgetView	=	GetParentWidgetView ();
	if (parentWidgetView != Nil) {
		effectiveOrigin = LocalToLocal (effectiveOrigin, parentWidgetView);	// convert to widget parent coords
	}

	// hack resize policy since if children fail to set size, I think they may screw the parent size up????
	// LGP 1/17/92
	Arg		arg;
	XtSetArg (arg, XmNresizePolicy, XmRESIZE_ANY);
	::XtSetValues (fMagicWidget, &arg, 1);
	::XtConfigureWidget (fMagicWidget, (int)effectiveOrigin.GetH (), (int)effectiveOrigin.GetV (),
						 (int)size.GetH (), (int)size.GetV (), fMagicWidget->core.border_width);
	XtSetArg (arg, XmNresizePolicy, XmRESIZE_NONE);
	::XtSetValues (fMagicWidget, &arg, 1);
}

#endif


/*
 ********************************************************************************
 ********************************* AbstractScroller *****************************
 ********************************************************************************
 */

AbstractScroller::AbstractScroller (View* scrolledView):
	View (),
	fScrollerMagic (Nil),
	fRePaintWhileScrolling (True),
	fScrollBitsWhileScrolling (True)
{
	fScrollerMagic = new ScrollerMagicView ();
	AddSubView (fScrollerMagic);
	SetScrolledView (scrolledView);

#if		qXGDI && 0
	// while the graphix stuff is still buggy, these values tend to work better...
	SetScrollBitsWhileScrolling (False);
#endif
}

AbstractScroller::~AbstractScroller ()
{
	SetScrolledView (Nil);
	RemoveSubView (fScrollerMagic);
}

Point	AbstractScroller::GetWhereViewing () const
{
	RequireNotNil (GetScrolledView ());
	return (-GetScrolledView ()->GetOrigin ());
}

void	AbstractScroller::SetWhereViewing (const Point& where, Panel::UpdateMode updateMode)
{
	RequireNotNil (GetScrolledView ());


	/*
	 * First, adjust new whereViewing so that we would not be scrolling out of range
	 * of our scrollbars.
	 *
	 * This tends to happen when called from ScrollSoShowing ().
	 * We could assert-out, but it would be redundent, and difficult to have ScrollSoShowing (),
	 * do the computations we do here.
	 */
	Point	magicViewSize		=	GetScrollerMagicView ().GetExtent ().GetSize ();
	Point	scrolledViewSize	=	GetScrolledView ()->GetSize ();
	Point	newWhereViewing		=	where;
	if (newWhereViewing.GetV () + magicViewSize.GetV () > scrolledViewSize.GetV ()) {
		newWhereViewing.SetV (scrolledViewSize.GetV () - magicViewSize.GetV ());	// pin to bottom.
	}
	if (newWhereViewing.GetH () + magicViewSize.GetH () > scrolledViewSize.GetH ()) {
		newWhereViewing.SetH (scrolledViewSize.GetH () - magicViewSize.GetH ());	// pin to right.
	}

	// pin to top-left also
	if (newWhereViewing.GetV () < 0) { newWhereViewing.SetV (0); }
	if (newWhereViewing.GetH () < 0) { newWhereViewing.SetH (0); }


	if (GetWhereViewing () != newWhereViewing) {
		Point	delta				=	newWhereViewing - GetWhereViewing ();
		Rect	magicRect			=	GetScrollerMagicView ().GetExtent ();	// its my subview so in my coords...
		Rect	newMagic			=	magicRect - delta;
		// Compute the portion of the magicRect that is still good - that is whose bits we can presever with a scrollbits...

// THIS MAGIC SB A RECTABGLE!!!!
//		Region	magicIntersection	=	magicRect * newMagic;
		Region	magicIntersection	=	magicRect * newMagic;

//gDebugStream << tab << "delta=" << delta << newline;
//gDebugStream << tab << "magicRect=" << magicRect << newline;
//gDebugStream << tab << "magicIntersection=" << magicIntersection << newline;
		/*
		 * Really do the update.
		 */
		GetScrolledView ()->SetOrigin (-newWhereViewing, eNoUpdate);

		/*
		 * Update screen.
		 *
		 *		If no optimization possible, or delayed update and no scrollbits, or
		 *		noupdate, then just refresh.
		 *		Otherwise, if immediateUpdate, or delayedWithScrollBits, then try that.
		 */
		if (magicIntersection.Empty () or 
			(updateMode == eNoUpdate) or
			((updateMode == eDelayedUpdate) and not (GetScrollBitsWhileScrolling ()))
			) {
			/*
			 * No optimization possible, so just use old approach.
			 */	
//gDebugStream << "Doing no-optimization case in setwhereviewing" << newline;
			GetScrollerMagicView ().Refresh (updateMode);
		}
		else {
			Assert ((updateMode == eImmediateUpdate) or
				((updateMode == eDelayedUpdate) and GetScrollBitsWhileScrolling ())
				);
			/*
			 * Do a scrollbits, and then update the region not fixed.
			 */
// NB: This screws up tilign patterns til we fix it somehow...
// See MacApp - at least basic idea is there, to ::setorigin().

			ScrollBits (magicRect, -delta);
			Region	magicUpdate	=	(magicRect - magicIntersection) * GetScrollerMagicView ().GetVisibleArea ();

			GetScrollerMagicView ().Refresh (magicUpdate, updateMode);
		}
	}
}

void	AbstractScroller::ScrollSoShowing (const Rect& what, Boolean forceFullyShown, Panel::UpdateMode updateMode)
{
	/*
	 * Figure out what rect of the scrolled view is showing thru, and call setwhereviewing
	 * to adjust things if its not shown.
	 */
	Point	nowViewingAt	=	GetWhereViewing ();
	Rect	viewRect		=	Rect (nowViewingAt, GetScrollerMagicView ().GetSize ());
	
	Rect	intersection = what * viewRect;
	if (((forceFullyShown) and (intersection != what)) or (intersection.Empty ())) {
		Point	scrollTo	=	GetWhereViewing ();
		if (((forceFullyShown) and (what.GetTop () <= nowViewingAt.GetV ())) or (what.GetBottom () <= nowViewingAt.GetV ())) {
			scrollTo.SetV (what.GetTop ());		// Scroll Up
		}
		else if (what.GetTop () >= viewRect.GetBottom ()) {
			Coordinate	moveUpBy	=	what.GetBottom () - viewRect.GetBottom ();
			Assert (moveUpBy > 0);
			scrollTo.SetV (viewRect.GetTop () + moveUpBy);		// Scroll Down
		}

		if (((forceFullyShown) and (what.GetLeft () <= nowViewingAt.GetH ())) or (what.GetRight () <= nowViewingAt.GetH ())) {
			scrollTo.SetH (what.GetLeft ());		// Scroll Right
		}
		else if (what.GetLeft () >= viewRect.GetRight ()) {
			Coordinate	moveLeftBy	=	what.GetRight () - viewRect.GetRight ();
			Assert (moveLeftBy > 0);
			scrollTo.SetH (viewRect.GetLeft () + moveLeftBy);		// Scroll Left
		}

		SetWhereViewing (scrollTo, updateMode);
	}
}

View*	AbstractScroller::GetScrolledView () const
{
	if (GetScrollerMagicView ().GetSubViewCount () == 0) {
		return (Nil);
	}
	else {
		Assert (GetScrollerMagicView ().GetSubViewCount () == 1);
		return (GetScrollerMagicView ().GetSubViewByIndex (1));
	}
}

void	AbstractScroller::SetScrolledView (View* scrolledView, Panel::UpdateMode updateMode)
{
	if (GetScrolledView () != scrolledView) {
		if (GetScrolledView () != Nil) {
			GetScrollerMagicView ().RemoveSubView (GetScrolledView ());
		}
		Assert (GetScrollerMagicView ().GetSubViewCount () == 0);
		if (scrolledView != Nil) {
			GetScrollerMagicView ().AddSubView (scrolledView);
		}
		GetScrollerMagicView ().Refresh (updateMode);
	}
}

Boolean	AbstractScroller::GetRePaintWhileScrolling () const
{
	return (fRePaintWhileScrolling);
}

void	AbstractScroller::SetRePaintWhileScrolling (Boolean rePaintWhileScrolling)
{
	fRePaintWhileScrolling = rePaintWhileScrolling;
}

Boolean	AbstractScroller::GetScrollBitsWhileScrolling () const
{
	return (fScrollBitsWhileScrolling);
}

void	AbstractScroller::SetScrollBitsWhileScrolling (Boolean scrollBitsWhileScrolling)
{
	fScrollBitsWhileScrolling = scrollBitsWhileScrolling;
}

Region	AbstractScroller::GetContentArea () const
{
	return (GetScrollerMagicView ().GetRegion ());
}

ScrollerMagicView&	AbstractScroller::GetScrollerMagicView () const
{
	EnsureNotNil (fScrollerMagic);
	return (*fScrollerMagic);
}

void	AbstractScroller::Layout ()
{
	GetScrollerMagicView ().SetSize (GetSize ());
	View::Layout ();
}






/*
 ********************************************************************************
 ************************************ Scroller **********************************
 ********************************************************************************
 */

AbstractScrollBar*	const	Scroller::kBuildDefaultSlider	=	(AbstractScrollBar*)1;
AbstractScrollBar*	const	Scroller::kBuildNoSlider		=	(AbstractScrollBar*)2;


Scroller::Scroller (View* scrolledView, MenuCommandHandler* menuTarget, KeyHandler* keyTarget, 
					AbstractScrollBar* verticalScrollBar, AbstractScrollBar* horizontalScrollBar):
	AbstractScroller (scrolledView),
	fMenuTarget (menuTarget),
	fKeyTarget (keyTarget),
	fVerticalScrollBar (Nil),
	fHorizontalScrollBar (Nil),
	fBuiltVerticalScrollBar (False),
	fBuiltHorizontalScrollBar (False)
{
	fVerticalScrollBar = BuildScrollBar (AbstractScrollBar::eVertical, verticalScrollBar);
	fBuiltVerticalScrollBar = Boolean (fVerticalScrollBar != verticalScrollBar);
	
	fHorizontalScrollBar = BuildScrollBar (AbstractScrollBar::eHorizontal, horizontalScrollBar);
	fBuiltHorizontalScrollBar = Boolean (fHorizontalScrollBar != horizontalScrollBar);
}

Scroller::Scroller (AbstractScrollBar* verticalScrollBar, AbstractScrollBar* horizontalScrollBar) :
	AbstractScroller (Nil),
	fMenuTarget (Nil),
	fKeyTarget (Nil),
	fVerticalScrollBar (Nil),
	fHorizontalScrollBar (Nil),
	fBuiltVerticalScrollBar (False),
	fBuiltHorizontalScrollBar (False)
{
	fVerticalScrollBar = BuildScrollBar (AbstractScrollBar::eVertical, verticalScrollBar);
	fBuiltVerticalScrollBar = Boolean (fVerticalScrollBar != verticalScrollBar);

	fHorizontalScrollBar = BuildScrollBar (AbstractScrollBar::eHorizontal, horizontalScrollBar);
	fBuiltHorizontalScrollBar = Boolean (fHorizontalScrollBar != horizontalScrollBar);
}

Scroller::~Scroller ()
{
	if ((fBuiltVerticalScrollBar) and (fVerticalScrollBar != Nil)) {
		RemoveSubView (fVerticalScrollBar);
		delete fVerticalScrollBar;
	}
	if ((fBuiltHorizontalScrollBar) and (fHorizontalScrollBar != Nil)) {
		RemoveSubView (fHorizontalScrollBar);
		delete fHorizontalScrollBar;
	}
}

MenuCommandHandler*	Scroller::GetMenuTarget () const
{
	return (fMenuTarget);
}

void	Scroller::SetMenuTarget (MenuCommandHandler* menuTarget)
{
	fMenuTarget = menuTarget;
}

KeyHandler*	Scroller::GetKeyTarget () const
{
	return (fKeyTarget);
}

void	Scroller::SetKeyTarget (KeyHandler* keyTarget)
{
	fKeyTarget = keyTarget;
}

void	Scroller::SetScrolledViewAndTargets (View* scrolledView, MenuCommandHandler* menuTarget, KeyHandler* keyTarget, Panel::UpdateMode updateMode)
{
	SetScrolledView (scrolledView, updateMode);
	SetMenuTarget (menuTarget);
	SetKeyTarget (keyTarget);
}

void	Scroller::Draw (const Region& /*update*/)
{
	DrawBorder ();
}

void	Scroller::SetBorder_ (const Point& border, Panel::UpdateMode updateMode)
{
	AbstractScroller::SetBorder_ (border, eNoUpdate);
	InvalidateLayout ();
	Refresh (updateMode);
}

void	Scroller::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode)
{
	AbstractScroller::SetMargin_ (margin, eNoUpdate);
	InvalidateLayout ();
	Refresh (updateMode);
}

void	Scroller::DoSetupMenus ()
{
	MenuCommandHandler*	menuTarget	=	GetMenuTarget ();
	if (menuTarget != Nil) {
		menuTarget->DoSetupMenus ();
	}
}

Boolean	Scroller::DoCommand (const CommandSelection& selection)
{
	MenuCommandHandler*	menuTarget	=	GetMenuTarget ();
	if (menuTarget != Nil) {
		if (menuTarget->DoCommand (selection)) {
			return (True);
		}
	}
	return (False);
}

Boolean	Scroller::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState)
{
	KeyHandler*	keyTarget	=	GetKeyTarget ();
	if (keyTarget != Nil) {
		if (keyTarget->DispatchKeyEvent (code, isUp, keyBoardState, composeState)) {
			return (True);
		}
	}
	return (False);
}

void	Scroller::SetWhereViewing (const Point& where, Panel::UpdateMode updateMode)
{
	AbstractScroller::SetWhereViewing (where, updateMode);

	Point	adjustedWhere	=	GetWhereViewing ();		// dont use where, since AbstractScroller::SetWhereViewing
														// may have adjusted it

	/*
	 * also should update scroll bars in case called from user code.
	 */
	if (fVerticalScrollBar != Nil) {
		fVerticalScrollBar->SetValue (adjustedWhere.GetV (), updateMode);
	}
	if (fHorizontalScrollBar != Nil) {
		fHorizontalScrollBar->SetValue (adjustedWhere.GetH (), updateMode);
	}
}

void	Scroller::ScrolledViewChangedSize ()
{
	InvalidateLayout ();
}

Boolean	Scroller::SliderChanging (SliderBase& theSlider, Tracker::TrackPhase /*phase*/, Real /*oldValue*/, Real newValue)
{
	Point	whereViewing	=	GetWhereViewing ();
	if (fVerticalScrollBar == &theSlider) {
		whereViewing.SetV (Coordinate (newValue));
	}
	if (fHorizontalScrollBar == &theSlider) {
		whereViewing.SetH (Coordinate (newValue));
	}
	SetWhereViewing (whereViewing, (GetRePaintWhileScrolling ()? eImmediateUpdate: eDelayedUpdate));
	return (False);
}

void	Scroller::SliderChanged (SliderBase& theSlider, Real /*oldValue*/, Real newValue)
{
	Point	whereViewing	=	GetWhereViewing ();
	if (fVerticalScrollBar == &theSlider) {
		whereViewing.SetV (Coordinate (newValue));
	}
	if (fHorizontalScrollBar == &theSlider) {
		whereViewing.SetH (Coordinate (newValue));
	}
#if		qXGDI
// temp hack for a coule of reasons - first, we must fix Tablet::Scrollbits to fill in old area with
// backround whichis why weve set GetRepaintWhileScrolliung to True for motif - amoung other things.

// but furhter, were calling thewrong callback from the motif scrollbar code.!!!! Rething names of these
// slider callbacks and their semsantics - talk about with stering????
// LGP April 27, 1992...
//
	SetWhereViewing (whereViewing, eImmediateUpdate);
#else
	SetWhereViewing (whereViewing);
#endif
}

AbstractScrollBar*	Scroller::GetVerticalScrollBar () const
{
	return (fVerticalScrollBar);
}

void	Scroller::SetVerticalScrollBar (AbstractScrollBar* slider)
{
	SetVerticalScrollBar_ (slider);
}
		
AbstractScrollBar*	Scroller::GetHorizontalScrollBar () const
{
	return (fHorizontalScrollBar);
}

void	Scroller::SetHorizontalScrollBar (AbstractScrollBar* slider)
{
	SetHorizontalScrollBar_ (slider);
}

void	Scroller::SetVerticalScrollBar_ (AbstractScrollBar* slider)
{
	if (fVerticalScrollBar != Nil) {
		if (fBuiltVerticalScrollBar) {
			RemoveSubView (fVerticalScrollBar);
			delete fVerticalScrollBar;
		}
		fVerticalScrollBar = Nil;
		Refresh ();
	}
	fVerticalScrollBar = BuildScrollBar (AbstractScrollBar::eVertical, slider);
}

void	Scroller::SetHorizontalScrollBar_ (AbstractScrollBar* slider)
{
	if (fHorizontalScrollBar != Nil) {
		if (fBuiltHorizontalScrollBar) {
			RemoveSubView (fHorizontalScrollBar);
			delete fHorizontalScrollBar;
		}
		fHorizontalScrollBar = Nil;
		Refresh ();
	}
	fHorizontalScrollBar = BuildScrollBar (AbstractScrollBar::eHorizontal, slider);
}

const	SBARSIZE		=	16;
Rect	Scroller::GetScrollRect () const
{
	Rect	scrollRect	=	GetLocalExtent ().InsetBy (GetBorder () + GetMargin ());
	if (fVerticalScrollBar != Nil) {
		scrollRect.SetSize (scrollRect.GetSize () - Point (0, SBARSIZE));
	}
	if (fHorizontalScrollBar != Nil) {
		scrollRect.SetSize (scrollRect.GetSize () - Point (SBARSIZE, 0));
	}
	// no smaller than zero in any dimension (correct for -SBARSIZE - no checks there)
	scrollRect.SetSize (Max (scrollRect.GetSize (), kZeroPoint));
	return (scrollRect);
}

void	Scroller::Layout ()
{
	/*
	 * DONT CALL AbstractScroller::Layout () since all it does is adjust the size of
	 * the magic view, and it does it incorrectly.  That would be OK, but then we get flicker,
	 * since we are resizing this view and then setting its size back, even when there is
	 * really no change.  That causes invals, and flicker.
	 */
	View::Layout ();

	/*
	 * Adjust size of sliders, if any...
	 */
	Rect	scrollRect	=	GetScrollRect ();
	if (fVerticalScrollBar != Nil) {
		fVerticalScrollBar->SetExtent (Rect (scrollRect.GetTopRight () + Point (-1, 1), Point (scrollRect.GetHeight () + 2, SBARSIZE)));
	}
	if (fHorizontalScrollBar != Nil) {
		fHorizontalScrollBar->SetExtent (Rect (scrollRect.GetBotLeft () + Point (1, -1), Point (SBARSIZE, scrollRect.GetWidth () + 2)));
	}

	GetScrollerMagicView ().SetExtent (scrollRect);

	/*
	 * Adjust min/max of sliders for amount of scrolled view shown.
	 */
	if (fVerticalScrollBar != Nil) {
		AssertNotNil (GetScrolledView ());
		Coordinate	amtToScroll	=	GetScrolledView ()->GetSize ().GetV () -
									scrollRect.GetHeight ();

// MOTIFOS IS WRONG - DO YOU WANT THIS OR NOT STERL???
#if		qMotifOS
	// we currently have to hack the Motif scrollbar to get it to calculate things correctly,
	// and those hacks conflict with the kludges scroller does in its layout (makings its size
	// one bigger than it should so it "looks pretty". This whole thing is pretty depressing,
	// god bless Motif
	amtToScroll -= 1;
#endif
		// very roughly right
		if (amtToScroll < 0) {
			amtToScroll = 0;
		}
		Assert (fVerticalScrollBar->GetValue () >= 0);
		if (fVerticalScrollBar->GetValue () > amtToScroll) {
			fVerticalScrollBar->SetValue (amtToScroll);
		}
		fVerticalScrollBar->SetBounds (0, amtToScroll, fVerticalScrollBar->GetValue ());
	}
	if (fHorizontalScrollBar != Nil) {
		AssertNotNil (GetScrolledView ());
		Coordinate	amtToScroll	=	GetScrolledView ()->GetSize ().GetH () -
									scrollRect.GetWidth ();
		// very roughly right
		if (amtToScroll < 0) {
			amtToScroll = 0;
		}
		Assert (fHorizontalScrollBar->GetValue () >= 0);
		if (fHorizontalScrollBar->GetValue () > amtToScroll) {
			fHorizontalScrollBar->SetValue (amtToScroll);
		}
		fHorizontalScrollBar->SetBounds (0, amtToScroll, fHorizontalScrollBar->GetValue ());
	}

	/*
	 * Also, must be sure layout change of slider curVal didn't affect placement of scrolled view
	 * in magic view.
	 */
	Point	whereViewing	=	GetWhereViewing ();
	if (fVerticalScrollBar != Nil) {
		if (whereViewing.GetV () > fVerticalScrollBar->GetMax ()) {
			Assert (fVerticalScrollBar->GetMax () >= 0);
			whereViewing.SetV (Coordinate (fVerticalScrollBar->GetMax ()));
		}
	}
	if (fHorizontalScrollBar != Nil) {
		if (whereViewing.GetH () > fHorizontalScrollBar->GetMax ()) {
			Assert (fHorizontalScrollBar->GetMax () >= 0);
			whereViewing.SetH (Coordinate (fHorizontalScrollBar->GetMax ()));
		}
	}
	SetWhereViewing (whereViewing);

	AdjustStepAndPageSizes ();
}

void	Scroller::AdjustStepAndPageSizes ()
{
	/*
	 * Default to scrolling by 1 pixel, and 3/4 of a page.
	 */
	if (fVerticalScrollBar != Nil) {
#if		qSunCFront_OverloadingConfusionBug
		fVerticalScrollBar->SetSteps (1, Max (BigReal (0.75 * GetScrollRect ().GetHeight ()), BigReal (1.0)));
#else
		fVerticalScrollBar->SetSteps (1, Max (0.75 * GetScrollRect ().GetHeight (), 1.0));
#endif
	}
	if (fHorizontalScrollBar != Nil) {
#if		qSunCFront_OverloadingConfusionBug
		fHorizontalScrollBar->SetSteps (1, Max (BigReal (0.75 * GetScrollRect ().GetWidth ()), BigReal (1.0)));
#else
		fHorizontalScrollBar->SetSteps (1, Max (0.75 * GetScrollRect ().GetWidth (), 1.0));
#endif
	}
}

AbstractScrollBar*	Scroller::BuildScrollBar (AbstractScrollBar::Orientation orientation, AbstractScrollBar* slider)
{
	if (slider == kBuildDefaultSlider) {
		AddSubView (slider = new ScrollBar (orientation, this));
	}
	else if (slider == kBuildNoSlider) {
		slider = Nil;
	}
	else {
	    RequireNotNil (slider);
	    AddSubView (slider);
		slider->SetSliderController (this);
	}
	
	if (slider != Nil) {
		slider->SetBounds (0, 10000, 0);	// reset in layout to something reasonable
	}
	return (slider);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


