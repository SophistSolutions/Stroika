/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/View.cc,v 1.12 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *		Rewrite TabletToLocal_ (const Point& p), and LocalToTablet_() to use cached values. Also, cleanup code
 *		in those two functions - lots of old hacks abound and must be expunged.
 *		<<DONE, EXCEPT FOR THE TESTING/EXPUNGING - LGP April 1, 1992>>
 *
 *
 * Changes:
 *	$Log: View.cc,v $
 *		Revision 1.12  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.11  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.10  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.9  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.8  1992/07/16  18:04:37  lewis
 *		Got rid of includes for BackPixPat call under macos, and got rid of
 *		the calls - instead call Tablet::SetOSBackground ().
 *
 *		Revision 1.7  1992/07/16  17:20:12  sterling
 *		made render reset window backpixpat to views background
 *
 *		Revision 1.6  1992/07/08  03:47:26  lewis
 *		Renamed PointInside->Contains.
 *		Also, sterl commented on definition of GetEffectiveLive.
 *
 *		Revision 1.5  1992/07/04  14:24:11  lewis
 *		Got rid of class ScaledShape - and added GetLogicalSize method to Shape.
 *
 *		Revision 1.4  1992/07/03  02:20:55  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.3  1992/07/02  05:02:02  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  09:04:26  sterling
 *		Lots of changes - use PalletManager to make gray brushes instead of kGrayTile and eBicTMode -
 *		(LGP comment - really neither approach quite right - probably get rid of Gray() method in View).
 *		Mix in Bordered, and put in util methods to draw differnt kinds of borders
 *		Assert in View::DTOR that all subviews deleted.
 *
 *		Revision 1.1  1992/06/20  17:30:04  lewis
 *		Initial revision
 *
 *		Revision 1.51  1992/06/09  15:14:57  sterling
 *		eliminated pen and brush inheritance, eliminated drawn region cache, rewrote visible area cache
 *
 *		Revision 1.50  92/05/23  01:00:03  01:00:03  lewis (Lewis Pringle)
 *		Include "BlockAllocated.hh".
 *		
 *		Revision 1.49  92/05/13  13:12:48  13:12:48  lewis (Lewis Pringle)
 *		STERL: changed RemoveSubView () to check if arg Nil, or has no parent as part of overall effort to rationalize memory
 *		management, and make clear who owns what and get rid of hacks where view owns subview and deletes them - Soon will be up to subclasses.
 *		Sterls has now converted Emily tomake this possible.
 *		
 *		Revision 1.48  92/05/12  23:47:51  23:47:51  lewis (Lewis Pringle)
 *		Speed tweeks done based on Mac profiling, and misc code cleanups. Got rid of
 *		old #ifdefed out code. Added assertions. Added checks for if (fSubViews!=Nil) before
 *		building iterators in a small number (apx 4) places where mac profiling indicated that building iterators was a substantail
 *		expense.
 *		
 *		Revision 1.47  92/04/24  09:10:33  09:10:33  lewis (Lewis Pringle)
 *		Better comments.
 *		
 *		Revision 1.46  92/04/20  14:23:30  14:23:30  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *		
 *		Revision 1.44  92/04/17  01:24:45  01:24:45  lewis (Lewis Pringle)
 *		Added some comments about View::GetVisibleArea() and tried to add some assertions to it, but they
 *		fialed - I need to do major cleanups, or at the very least documention on this code.
 *		
 *		Revision 1.43  92/04/16  11:20:56  11:20:56  lewis (Lewis Pringle)
 *		Attempt to add expose proc to see if that fixed display problems in Emily (dialogs work fine) with
 *		qUseOurHackWidget stuff. Expore other alternatives - this one has sooo many negatives.
 *		
 *		Revision 1.42  92/04/15  00:31:33  00:31:33  lewis (Lewis Pringle)
 *		Added AdjustOurHackWidget() routine for adjusting extent of hack widget, and called as appropriate.
 *		Adjust Realize code for qBuildOurHackWidget.
 *		Gor rid of XmIsRowColumn hack - instead say XmIsGadget(w) before adding to RestackWindows list - works much better now.
 *		Also, in View::EffectiveVisibilityChanged(), call XtMap/UnMapWidgets.
 *		
 *		Revision 1.41  92/04/14  19:42:43  19:42:43  lewis (Lewis Pringle)
 *		Noted that View::Reparent () was buggy - called with view hierarchy in inconsistent state - bad idea anyway.
 *		Got rid of it entirely - expanded its code inline in AddSubView and RemoveSubView () removed unneeded portions,
 *		and commented on more that could be cleaned up. In the case of AddSunView (), I moved the SetTablet()
 *		call to the bottom so that the EffectiveFontChanged() method would be called after realization. All of this
 *		should be cleaned up.
 *		Got rid of some debug stuff.
 *		Net effect is that the restacking seems to work OK now for all cases - still must retest the case where we get rid
 *		of XmRowColumn hack, and when we build Hack Widgets.
 *		
 *		Revision 1.39  92/04/14  07:13:42  07:13:42  lewis (Lewis Pringle)
 *		Fixed GetParentWidgetView () to make sure Widget changed, for case when parent view has same widget as
 *		child - then it should not count as ParentWidgetView (eg cases like Motif MenuBar where we own a MotifOSControl).
 *		
 *		Revision 1.38  92/04/13  01:00:34  01:00:34  lewis (Lewis Pringle)
 *		Got rid of gdebugmessages, and fixedup SyncMotifWidget stuff slightly - works a bit better, but still cannot make work
 *		stuff to always build widgets- screws up menus.
 *		
 *		Revision 1.37  92/04/10  00:46:07  00:46:07  lewis (Lewis Pringle)
 *		Worked on restack stuff, and MakeOurHackWidget, to further imrpove clipping of widgets, but not really working.
 *		Probably incompatabilities with the motif menu stuff. Need some way to disable this functionality...
 *		
 *		Revision 1.36  92/04/09  14:55:21  14:55:21  lewis (Lewis Pringle)
 *		Fixed serious bug in View Nil Iterator stuff. Hack to only keep one iterator around ever failed, since
 *		other code I wrote causes invocation of two Nil view iterators. Fixed using blockallocator from mem.h
 *		More imporatantly, added support to View for XtToolkit reordering of X window list based on our view
 *		widgets, and subview lists. Soon to do support so we always create views, for clipping of Xt Widgets.
 *		
 *		Revision 1.35  92/04/06  15:33:08  15:33:08  sterling (Sterling Wight)
 *		invalidate cached origin on reparent
 *		
 *		Revision 1.34  92/04/02  11:39:31  11:39:31  lewis (Lewis Pringle)
 *		Got rid of Get/SetClip methods. As more careful about overrides of TabletOwner routines to do
 *		SetClips before calling inherited method to indrect to tablet.
 *		Got rid of GetEffectiveChacedOrigin () - instead do all the magic in LocalToTablet, TabletToLocal_.
 *		Then tabletowner routines work fine using these cahced values. Mode BltBlti overrides to reflect
 *		chagners in TabletOwner.
 *		Got rid of Focus stuff - instead just call SetClip (GetVisibleArea()) before each call Views overrides
 *		of TabletOwner drawing indirection primimates (like Outline).
 *		
 *		Revision 1.33  92/03/26  18:36:16  18:36:16  lewis (Lewis Pringle)
 *		Cleanup comments.
 *		
 *		Revision 1.32  1992/03/26  09:29:23  lewis
 *		Moved GroupView, and LabledGroupView to GroupView.hh/cc.
 *		Made GetBrush/GetEffectiveBrush, and Pen/Font/Background routines simple oneliner inlines.
 *		Got rid of CalcXX version of said attributes.
 *		Added updateMode args to most related rouintes, Reparent (), Add/Remove/ReorderSubview, etc...
 *		Made AddSubView/RemoveSubView/Reparent all non-virtual.
 *		Got rid of extra arg to EffectiveLiveChanged.
 *		Initialize fEffectiveXXX with real default - &kDefaultXXX. Then, add a little code to InvalCurXXX, and
 *		SetXXX so that GetXXX can be trivial inline.
 *		Get rid of a bunch of InvalLayouts done - mostly relating to font changes - let that be done
 *		in subclasses thru their EffectiveFontChanged message.
 *
 *		Revision 1.30  1992/03/23  18:43:56  lewis
 *		More minor speed tweeks, and better comments in a few places.
 *
 *		Revision 1.29  1992/03/22  21:58:28  lewis
 *		Speed tweeks based on mac profiling data. Only controversial one being to get rid of Refresh () call in EffectiveFontChanged.
 *		Should seriously reconsider when - in general - it is approproiate to do these refreshes!!!
 *
 *		Revision 1.27  1992/03/13  16:39:22  lewis
 *		Put in debugmessage (for unix only) when we destruct a view with subviews to help track down those cases.
 *
 *		Revision 1.26  1992/03/10  00:08:23  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.25  1992/03/05  21:31:25  sterling
 *		changed groupview, no longer auto remove from parent,
 *		added labeledgroupview, minor tweaks
 *
 *		Revision 1.23  1992/02/28  22:14:46  lewis
 *		Cleanup realize/unrealize support. Recognize that we were too strict before. Allow for
 *		Realize () when already realized, and UnRealize () when were unrealized. This allows for
 *		case where top level guy has no widget but subthings do, and all realiuzed
 *		with respect to some top level widget that is not a view.
 *
 *		Revision 1.22  1992/02/28  16:27:07  lewis
 *		Add CurrentIndex () override to NilViewIterator since we added it to abstract interface for
 *		SequenceIterators.
 *
 *		Revision 1.21  1992/02/28  15:35:48  lewis
 *		Minor cleanups, and added ensure to GetEffectiveCachedOrigin () as a prelude
 *		to getting rid of it and putting the cache check code directly into
 *		localtotablet, et al.
 *
 *		Revision 1.20  1992/02/15  05:52:24  sterling
 *		serveral changes, chiefly using EffectiveLiveChanged
 *
 *		Revision 1.19  1992/02/14  03:30:58  lewis
 *		Added RequireNotNil (fCurrentTablet) in a few places before dereferencing it.
 *
 *		Revision 1.17  1992/01/21  21:02:18  lewis
 *		Get rid of reference to it.More ().
 *
 *		Revision 1.14  1992/01/14  06:08:01  lewis
 *		Have view inherit from TabletOwner.
 *
 *		Revision 1.13  1992/01/08  06:49:34  lewis
 *		Sterl - merged in his changes having todo with being active/live.
 *
 *
 *
 */


#include	"OSRenamePre.hh"

#if	qXtToolkit
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
		#include	<Xm/RowColumn.h>	// so we can special case check for menus...
	#endif
#endif	/* Toolkit */

#include	"OSRenamePost.hh"



#include	"BlockAllocated.hh"
#include	"Debug.hh"
#include	"OSConfiguration.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"
#include	"Pallet.hh"
#include	"Shape.hh"

#include	"Adornment.hh"
#include	"DeskTop.hh"

#include	"View.hh"




#if		!qRealTemplatesAvailable
	Implement (Iterator, ViewPtr);
	Implement (Collection, ViewPtr);
	Implement (AbSequence, ViewPtr);
	Implement (Array, ViewPtr);
	Implement (Sequence_Array, ViewPtr);
	Implement (Sequence, ViewPtr);
#endif



/*
 * Currently this class is implemented in such a way as to only allow for one instantiation at a time. That could
 * easily be fixed, however.
 */
class	NilViewPtrSequenceIterator : public SequenceIterator(ViewPtr) {
	public:
		NilViewPtrSequenceIterator ():
			SequenceIterator(ViewPtr) ()
			{
			}

		override	Boolean			Done () const;
		override	void			Next ();
		override	ViewPtr			Current () const;
		override	CollectionSize	CurrentIndex () const;
		override	SequenceDirection	GetDirection () const;

		nonvirtual	void*	operator new (size_t n);
		nonvirtual	void	operator delete (void* p);
};



#define	qBuildOurHackWidget		0




#if		qXtToolkit && !qRealTemplatesAvailable
typedef	osWidget*	osWidgetPtr;
Declare (Collection, osWidgetPtr);
Implement (Collection, osWidgetPtr);
Declare (Sequence, osWidgetPtr);
Implement (Sequence, osWidgetPtr);
Declare (DoubleLinkList, osWidgetPtr);
Implement (DoubleLinkList, osWidgetPtr);
Declare (Sequence, osWidgetPtr);
Implement (Sequence, osWidgetPtr);
#endif





/*
 ********************************************************************************
 **************************************** View **********************************
 ********************************************************************************
 */

View::View ():
	fParentView (Nil),
	fSubViews (Nil),
	fCurrentTablet (Nil),
	fCachedOrigin (kZeroPoint),
	fNeedsLayout (True),
	fChildNeedsLayout (False),
	fEffectiveFont ((Font*)&kDefaultFont),
	fEffectiveBackground ((Tile*)&kDefaultBackground),
	fVisibleArea (Nil),
	fCachedOriginValid (False),
	fHasFont (False),
	fHasBackground (False),
#if		qXtToolkit && qBuildOurHackWidget
	fOurHackWidget (Nil),
#endif
	fAdornment (Nil)
{
}

View::~View ()
{
	Require (GetParentView () == Nil);		// Must remove views before destroying them!

#if		qXtToolkit
	Require (GetWidget () == Nil);		// assume we are already unrealized - therefore so are our children
#endif
	Require (GetSubViewCount () == 0);
	delete fSubViews;

	delete (fAdornment);
	if (fHasFont) {
		Assert (fEffectiveFont != Nil);
		if ((fEffectiveFont != &kSystemFont) and (fEffectiveFont != &kApplicationFont)) {
			delete (fEffectiveFont);
		}
	}
	if (fHasBackground) {
		Assert (fEffectiveBackground != Nil);
		if (fEffectiveBackground != &kDefaultBackground) {
			delete (fEffectiveBackground);
		}
	}
	if (fVisibleArea != &kEmptyRegion) {
		delete (fVisibleArea);
	}
}

#if		qXtToolkit
void	View::Realize (osWidget* parent)
{
	RequireNotNil (parent);

#if		qBuildOurHackWidget
	// try forcing creation of a widget at every level, if we have subviews.. For clipping of widgets..
	Require (fOurHackWidget == Nil);
	if (GetSubViewCount () != 0 and GetWidget () == Nil) {
		MakeOurHackWidget ();
	}
#endif

	osWidget*	childrensParent	=	parent;
#if		qBuildOurHackWidget
	if (fOurHackWidget != Nil) {
		childrensParent = fOurHackWidget;
	}

// disable assert - debug later--april 16, 1992
//	Assert (childrensParent == GetWidget ());		// either we did a hack widget, or someone else created a real one, and that
													// should have been passed here...
#endif
	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		it.Current ()->Realize (childrensParent);
	}

	/*
	 * When we are realized, and after we realize our children, then we must make sure the stacking
	 * order of the varous created windows is setup correctly. If we have no window, or have no
	 * widget, count on the parent view of ours that does to do this. Or, if we are being realized
	 * due to a Reparent () call, then do the Sync call there.
	 */
	if (GetWidget () != Nil and XtWindow (GetWidget ()) != Nil) {
		SyncronizeXWindowsWithChildWidgets ();
	}
}

void	View::UnRealize ()
{
	/*
	 * Note that this can be called with a Nil parent widget view since the parent widget might be
	 * in some parent panel or enclosure, not neccesarily a view???
	 */
	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		AssertNotNil (it.Current ());
		it.Current ()->UnRealize ();
	}

#if		qBuildOurHackWidget
	if (fOurHackWidget != Nil) {
		UnMakeOurHackWidget ();
	}
	Ensure (fOurHackWidget == Nil);
#endif
}

osWidget*	View::GetWidget () const
{
#if		qBuildOurHackWidget
	return (fOurHackWidget);
#endif
	return (Nil);		// most views do not have an associated widget - also, this is necessary
	  					// for portability - dont want people forced to override this method...
}



// copied from window.cc - not sure I need to be so elaborate/??
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
	// Not sure anything else could work for X, however???
//#if		qUsePrivateUpdateRegions
	/*
	 * Really we should find type of calldata and get rect from it.
	 */
	View* v = (View*)clientData;
	v->Refresh (addToRegion, View::eDelayedUpdate);
//#endif
}

void	View::MakeOurHackWidget ()
{
#if		qBuildOurHackWidget
	Require (fOurHackWidget == Nil);

#if		qXmToolkit
	/*
	 * For some other Xt based toolkit, we'll need another widget...
	 */
	Arg args[3];
	XtSetArg (args[0], XmNmarginWidth, 0);
	XtSetArg (args[1], XmNmarginHeight, 0);
	XtSetArg (args[2], XmNshadowThickness, 0);

	fOurHackWidget = ::XmCreateDrawingArea (GetParentWidget (), "OurHackWidget", args, 3);
//gDebugStream << "building hack widget " << fOurHackWidget << "for view=" << this << " and parentView=" << GetParentView () << newline;

	::XtAddCallback (fOurHackWidget, XmNexposeCallback, (XtCallbackProc)ExposeCallBack, (XtPointer)this);

	AdjustOurHackWidget ();
#endif

	::XtRealizeWidget (fOurHackWidget);

	if (GetEffectiveVisibility ()) {
		::XtManageChild (fOurHackWidget);		// only manage ourselves when we become visible - see EffectiveVisibilityChanged()...
		::XtMapWidget (fOurHackWidget);
	}
	AssertNotNil (XtWindow (fOurHackWidget));
#else
	AssertNotReached ();
#endif
}

void	View::UnMakeOurHackWidget ()
{
#if		qBuildOurHackWidget
	RequireNotNil (fOurHackWidget);
	::XtDestroyWidget (fOurHackWidget);
	fOurHackWidget = Nil;
#else
	AssertNotReached ();
#endif
}

void	View::AdjustOurHackWidget ()
{
#if		qBuildOurHackWidget
// not sure this is a good place ???? Not garuenteed called??? Really must override setsize_!!!
	if (fOurHackWidget != Nil) {
		// X generates protocol errors if we create zero sized windows - nice...
		Point	size	=	GetSize ();
		if (size.GetV () == 0) { size.SetV (1); }
		if (size.GetH () == 0) { size.SetH (1); }

		// size the magic view.. then size our little view - no need to place our little view since its always zero-zero based in our
		// magic view
		Point	effectiveOrigin		=	GetOrigin ();
effectiveOrigin = kZeroPoint;
// hack - not sure which this should be -thinkout more...
		View*	parentWidgetView	=	GetParentWidgetView ();
		if (parentWidgetView != Nil) {
			effectiveOrigin = LocalToLocal (effectiveOrigin, parentWidgetView);	// convert to widget parent coords
		}
//gDebugStream << "Adjusting OurHackWidget: this=" << this << "extent to " << Rect (effectiveOrigin, size) << newline;
//gDebugStream << tab << "AND our parentView = " << GetParentView () << " and our parentWidgetView = " << GetParentWidgetView () << newline;
		::XtConfigureWidget (fOurHackWidget, (int)effectiveOrigin.GetH (), (int)effectiveOrigin.GetV (),
							 (int)size.GetH (), (int)size.GetV (), fOurHackWidget->core.border_width);
//gDebugStream << tab << "AFTER XTCONFIGURE+ widgtet origin=" << Point (fOurHackWidget->core.y, fOurHackWidget->core.x) <<
//  " and widget =" << fOurHackWidget << " and parentWidget= " << XtParent (fOurHackWidget) << newline;
	}
#else
	AssertNotReached ();
#endif
}

View*	View::GetParentWidgetView () const
{
	/*
	 * Search up list of parents, for one with a widget, and whose widget differs from ours. Sometimes (eg with
	 * MotifOSControl - maybe only this case) we have an owner of one of these classes overriding GetWidget ()
	 * and returning GetWidget () for that owner class too. In that case, we dont want to consider that to be
	 * a Parent Widget View.
	 */
	osWidget*	startWidget	=	GetWidget ();
	for (register View* v = GetParentView ();
		 v != Nil and (v->GetWidget () == Nil or v->GetWidget () == startWidget);
		 v = v->GetParentView ())
		;
	Ensure (v == Nil or (v->GetWidget () != Nil and v->GetWidget () != startWidget));
	return (v);
}

osWidget*	View::GetParentWidget () const
{
	View*	v	=	GetParentWidgetView ();
	Assert ((v==Nil) or (v->GetWidget () != Nil));
	return ((v==Nil)? Nil: v->GetWidget ());
}

void	View::SyncronizeXWindowsWithChildWidgets ()
{
	/*
	 * Recursively go thru all children, with this as their GetParentWidgetView (), and if they
	 * have a window (that is are not a gadget, and are realized), then be sure X has their
	 * windows appropriately stacked (call XRestackWindows). This is used to assure that widgets
	 * appear to have the appropriate front-to-back ordering.
	 *
	 * Only call this routine for a view with a widget, and a window for that widget.
	 */
	Require (GetWidget () != Nil and XtWindow (GetWidget ()) != Nil);

	if (GetSubViewCount () != 0) {
		/*
		 * Form a list of subwindows of THIS window. (really we keep widget ptrs, and they ALL MUST have
		 * an associated window (that is gadgets need not apply).
		 *
		 * This process returns a list front-to-back ordered.
		 */
		Sequence(osWidgetPtr)	listOfSubWidgets;
		ForEach (ViewPtr, it, MakeSubViewIterator ()) {
			View*	sv	=	it.Current ();
			sv->SyncChildWidgetsHelper (listOfSubWidgets);
		}

		/*
		 * At this point, we can formulate a call to XRestackWindows. Hopefully it will be generous, and cause
		 * no flicker if this does not consitute a change in the window hierarchy. If it does, we could modify
		 * the algoritm here to check first, but that would be more expensive, since all the relevant information
		 * resides on the Z server. We'll be optimistic - they did everything else right in X :-).
		 */
		extern	osDisplay*	gDisplay;
		int			nWindows	=	listOfSubWidgets.GetLength ();
		osWindow*	subWindows	=	new osWindow[nWindows];
		int			i			=	0;

		ForEach (osWidgetPtr, itW, listOfSubWidgets) {
			osWidget*	w	=	itW.Current ();
			AssertNotNil (w);
			AssertNotNil (XtWindow (w));

			if (XtParent (w) != GetWidget ()) {
				nWindows--;
			}
			else if (XmIsGadget (w)) {
				nWindows--;
			}
			else {
				subWindows[i++] = XtWindow (w);
			}
		}
		if (nWindows != 0) {
			::XRestackWindows (gDisplay, subWindows, nWindows);
		}
		delete (subWindows);
	}
}

void	View::SyncChildWidgetsHelper (class Sequence(osWidgetPtr)& orderedList)
{
	/*
	 * Append myself if I have a window, otherwise, my subviews get a crack.
<slight revisiion - in case of gadget - append nothing>, and just return
	 */
	if (GetWidget () != Nil) {
		if (XtWindow (GetWidget ()) != Nil) {
			orderedList.Append (GetWidget ());
		}
	}
	else {
		ForEach (ViewPtr, it, MakeSubViewIterator ()) {
			View*	sv	=	it.Current ();
			AssertNotNil (sv);
			osWidget*	w	=	sv->GetWidget ();
			if (w == Nil) {
				/*
				 * If our subview has no widget, then try its subviews. Note that we do a pre-order
				 * traversal here, ordering things from back to front visually. This is the same as
				 * for our draw algorithm - a critical relationship.
			 <<< Check accuracy - we may need to do post-order - or correct at the end>>>
			     *
				 * Just continue adding to our list of window siblings.
				 */
				sv->SyncChildWidgetsHelper (orderedList);
			}
			else {
				/*
				 * If we actually have a widget at this level, then there are two cases we must worry about.
				 * If there is a window associated with that widget, and if there is not. If there is, then
				 * we are done with this leg of the traversal, and can just add this widget to our list, and
				 * continue iterating (it might make sense to recurse here and call SyncronizeXWindowsWithChildWidgets ()
				 * but that would make this method a bludgeon, rather than a scalpel, and we can get the
				 * recursion we need elsewhere << Not totally thought out yet>>>
				 *
			     * If there is no window associated with this widget (ie we have a gadget) from the X reorder windows
				 * point of view, we must simple continue recursing as above. Later, if in our high level algorithm
				 * we came up with a way of fixing the ordering of gadgets, we might treat both cases likewise here,
				 * and just add them to our list, but for now, to avoid having to do furhter searching, and flattening
				 * in our calling routine (SyncronizeXWindowsWithChildWidgets ()) we just keep recursing.
<< NOW, dont recurse past gadgets, ignore them>>
				 */
				AssertNotNil (w);
				if (XtWindow (w) != Nil) {
					orderedList.Append (w);
				}
			}
		}
	}
}

#endif	/*qXtToolkit*/

void	View::SetOrigin_ (const Point& newOrigin, Panel::UpdateMode updateMode)
{
	Require  (newOrigin != GetOrigin ());
	
	Boolean	mustUpdate = Boolean ((updateMode != eNoUpdate) and (GetParentView () != Nil));
	if (mustUpdate) {
		Refresh (GetVisibleArea ());
	}
	
	Panel::SetOrigin_ (newOrigin, eNoUpdate);
	InvalCachedOrigin ();
	DoInvalVisibleArea ();
	
	if (mustUpdate) {
		Refresh (GetVisibleArea ());
		if (updateMode == eImmediateUpdate) {
			AssertNotNil (GetParentView ());
			GetParentView ()->Update ();
		}
	}
}

void	View::SetSize_ (const Point& newSize, Panel::UpdateMode updateMode)
{
	Require (newSize != GetSize ());

	Boolean	mustUpdate = Boolean ((updateMode != eNoUpdate) and (GetParentView () != Nil));
	if (mustUpdate) {
		Refresh (GetVisibleArea ());
	}
	
	InvalidateLayout ();
	Panel::SetSize_ (newSize, eNoUpdate);
	DoInvalVisibleArea ();
	
	if (mustUpdate) {
		AssertNotNil (GetParentView ());
		Refresh (GetVisibleArea ());
		if (updateMode == eImmediateUpdate) {
			GetParentView ()->Update ();
		}
	}
}

void	View::SetExtent_ (const Rect& newExtent, Panel::UpdateMode updateMode)
{
	Panel::SetExtent_ (newExtent, (updateMode == eImmediateUpdate)? eDelayedUpdate: updateMode);
	DoInvalVisibleArea ();
	if (updateMode == eImmediateUpdate) {
		if (GetParentView ()) {
			GetParentView ()->Update ();
		}
		else {
			Update ();
		}
	}
}

Point	View::CalcDefaultSize (const Point& hint) const
{
	return (CalcDefaultSize_ (hint));
}

Point	View::CalcDefaultSize_ (const Point& hint) const
{
	return (hint);
}

Boolean	View::TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState)
{
	Require (GetEffectiveLive ());
	
	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		register View* v	=	it.Current ();
		AssertNotNil (v);
		if (v->GetLive () and v->Contains (cursorAt)) {
			if (v->TrackMovement (v->EnclosureToLocal (cursorAt), mouseRgn, keyBoardState)) {
				// OK to use GetLocalExtent instead of GetVisibleArea (which is much more expensive)
				// since we keep doing this as we wind up the call chain, effectively reducing the
				// region to the visible area...
				mouseRgn = v->LocalToEnclosure (mouseRgn) * GetLocalExtent ();
				return (True);	
			}
			else {
				return (False);	
			}
		}
	}
	return (False);	
}

Boolean	View::TrackPress (const MousePressInfo& mouseInfo)
{
	Require (GetEffectiveLive ());
	
	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		register View* v	=	it.Current ();
		AssertNotNil (v);
		if (v->GetLive () and v->Contains (mouseInfo.fPressAt)) {
			return (v->TrackPress (MousePressInfo (mouseInfo, v->EnclosureToLocal (mouseInfo.fPressAt))));
		}
	}
	return (False);	
}

Boolean	View::TrackHelp (const Point& cursorAt, Region& helpRegion)
{
	Require (GetEffectiveLive ());
	
	if (GetHelpEnabled ()) {
		if (fSubViews != Nil) {
			ForEach (ViewPtr, it, MakeSubViewIterator ()) {
				register View* v	=	it.Current ();
				AssertNotNil (v);
				if (v->GetLive () and v->Contains (cursorAt)) {
					if (v->TrackHelp (v->EnclosureToLocal (cursorAt), helpRegion)) {
						helpRegion = v->LocalToEnclosure (helpRegion) * GetLocalExtent ();
						return (True);	
					}
					else {
						break;	
					}
				}
			}
			if (GetHelp () != kEmptyString) {
				//ShowHelp (LocalToGlobal (cursorAt), LocalToGlobal (helpRegion));
				ShowHelp (LocalToAncestor (cursorAt, &DeskTop::Get ()), LocalToAncestor (helpRegion, &DeskTop::Get ()));
				helpRegion = GetVisibleArea ();
				return (True);
			}
		}
	}
	return (False);
}

void	View::Draw (const Region& /*update*/)
{
#if		qXtToolkit && qBuildOurHackWidget
	/* Must do this here since its the only place we get notified that our view moved */
	/* Note that this draw happens before our subviews draw, so we should be all set */
	AdjustOurHackWidget ();
#endif
}

void	View::DrawBorder ()
{
	DrawBorder_ (GetLocalExtent (), GetBorder ());
}

void	View::DrawBorder_ (const Rect& box, const Point& border)
{
#if		qMacUI
	DrawMacBorder (box, border);
#elif	qMotifUI
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawMotifBorder (box, border, c, GetPlane ());
#elif	qWinUI
	Color c = PalletManager::Get ().MakeColorFromTile (GetEffectiveBackground ());
	DrawWinBorder (box, border, c, GetPlane ());
#endif	/*	GUI	*/
}

void	View::CalcColorSet (Color background, Color& selectColor, Color& topShadow, Color& bottomShadow)
{
	// see Kobara, chapter on Color Sets
	selectColor 	= background * .85;
	topShadow 		= background * 1.5;
	bottomShadow 	= background * .50;
}

void	View::DrawMacBorder (const Rect& box, const Point& border)
{
	Pen p = kDefaultPen;
	if (not GetEffectiveLive ()) {
		p.SetTile (PalletManager::Get ().MakeTileFromColor (kGrayColor));
	}
	p.SetPenSize (border);
	OutLine (GetShape (), box, p);
}

void	View::DrawMotifBorder (const Rect& box, const Point& border, Color backGroundColor, Bordered::Plane plane)
{
	if (border != kZeroPoint) {
		Color	selectColor;
		Color	topShadow;
		Color	bottomShadow;
		
		CalcColorSet (backGroundColor, selectColor, topShadow, bottomShadow);
		
		if (plane == Bordered::eIn) {
			Color temp = topShadow;
			topShadow = bottomShadow;
			bottomShadow = temp;
		}
		
		Brush	topBrush	=	Brush (PalletManager::Get ().MakeTileFromColor (topShadow));
		Brush	botBrush	=	Brush (PalletManager::Get ().MakeTileFromColor (bottomShadow));
		Coordinate	height = box.GetSize ().GetV ();
		Coordinate	width = box.GetSize ().GetH ();
		Rectangle	s;
		Rect		r;
		
		for (register int i = 0; i < border.GetH (); i++) {
			r.SetOrigin (Point (box.GetTop (), box.GetLeft () + i));
			r.SetSize (Point (height-i, 1));
			Paint (s, r, topBrush);
		}
		for (i = 0; i < border.GetV (); i++) {
			r.SetOrigin (Point (box.GetTop () + i, box.GetLeft ()));
			r.SetSize (Point (1, width-i));
			Paint (s, r, topBrush);
		}
		for (i = 0; i < border.GetH (); i++) {
			r.SetOrigin (Point (box.GetTop ()+i, box.GetRight ()-i-1));
			r.SetSize (Point (height-i, 1));
			Paint (s, r, botBrush);
		}
		for (i = 0; i < border.GetV (); i++) {
			r.SetOrigin (Point (box.GetBottom ()-i-1, box.GetLeft ()+i));
			r.SetSize (Point (1, width-i));
			Paint (s, r, botBrush);
		}
	}
}

void	View::DrawWinBorder (const Rect& box, const Point& border, Color backGroundColor, Bordered::Plane plane)
{
	DrawMotifBorder (box, border, backGroundColor, plane);
}

void	View::SetBorder_ (const Point& border, Panel::UpdateMode updateMode)
{
	Bordered::SetBorder_ (border, updateMode);
	Refresh (updateMode);
}

void	View::SetMargin_ (const Point& margin, Panel::UpdateMode updateMode)
{
	Bordered::SetMargin_ (margin, updateMode);
	Refresh (updateMode);
}

void	View::SetPlane_ (Bordered::Plane plane, Panel::UpdateMode updateMode)
{
	Bordered::SetPlane_ (plane, updateMode);
	Refresh (updateMode);
}

Tablet*	View::GetTablet () const
{
	return (fCurrentTablet);
}

void	View::SetTablet (Tablet* newTablet, Panel::UpdateMode updateMode)
{
	if (fCurrentTablet != newTablet) {
		InvalCachedOrigin ();
		DoInvalVisibleArea ();

		fCurrentTablet = newTablet;

// should change this to call EffectiveTabletChanged instead
		EffectiveFontChanged (GetEffectiveFont (), updateMode);		// cuz font metrics may change on new tablet

		ForEach (ViewPtr, it, MakeSubViewIterator ()) {
			AssertNotNil (it.Current ());
			it.Current ()->SetTablet (newTablet, updateMode);
		}
	}
}

Panel*	View::GetParentPanel () const
{
	return (fParentView);
}

void	View::SetVisible_ (Boolean visible, Panel::UpdateMode updateMode)
{
	Require (visible != GetVisible ());

	/*
	 * Note that Panel::SetVisible_ (visible, updateMode); calls EffectiveVisibilityChanged, so we dont have to.
	 */

	Boolean	wasLive = GetEffectiveLive ();
	if (visible) {
		/*
		 * By default, we do not lay out invisible views so must reinform parent that we need a layout
		 * and ensure that our children our correctly laid out.
		 */
		fChildNeedsLayout = True;
		NotifyEnclosureOfInvalidLayout ();

		/*
		 * optimize by laying everyone out when becoming visible. Lay everything out
		 * while still invisible to minimize region updating.
		 *
		 * LGP March 26, 1992 - Not sure this is a good hack to do at this level - maybe elsewhere??? Explain why
		 * you think otherwise sterl????
		 */
		if ((fParentView != Nil) and (updateMode != eNoUpdate)) {
			Panel::SetVisible_ (visible, eNoUpdate);
			fParentView->ProcessLayout ();	// want to have a legitimate size and origin before we are layed out
			Panel::SetVisible_ (not visible, eNoUpdate);
		}
		ProcessLayout (True);
	}
	DoInvalVisibleArea ();
	Panel::SetVisible_ (visible, updateMode);
	DoInvalVisibleArea (True);		// force siblings to have visible area adjusted if becoming visible
	
	if (wasLive != GetEffectiveLive ()) {
		EffectiveLiveChanged (not wasLive, updateMode);
	}
}

void	View::EffectiveVisibilityChanged (Boolean newVisible, Panel::UpdateMode updateMode)
{
#if		qXtToolkit && qBuildOurHackWidget
	if (fOurHackWidget != Nil and GetEffectiveVisibility ()) {
		if (newVisible) {
			::XtManageChild (fOurHackWidget);		// only manage ourselves when we become visible - see EffectiveVisibilityChanged()...
			::XtMapWidget (fOurHackWidget);
		}
		else {
			::XtUnmapWidget (fOurHackWidget);
		}
	}
#endif
	if (fSubViews != Nil) {
		ForEach (ViewPtr, it, MakeSubViewIterator ()) {
			if (it.Current ()->GetVisible ()) {
				it.Current ()->EffectiveVisibilityChanged (newVisible, updateMode);
			}
		}
	}
}

Region	View::GetVisibleArea () const
{
	/*
	 	Definitions:

		VisibleArea is with respect to a Tablet, so if a view has no tablet, its visible area is empty.
		An invisible view has an empty VisibleArea.
		Otherwise, a view's VisibleArea is its DrawnRegion, clipped by its parent's visible area, and
		opaque siblings if the same tablet that are in front of it. 
		
		The visible area is expressed in local coordinates.
		
		As an optimization, we do not cache the visible area when our tablet is Nil, or when we
		are not visible. This allows us to avoid needless invalidations of our subview's caches. It
		is safe because our children will also be invisible/have no tablet if we do, except when
		the child has explicitely set its Tablet, in which case we have no effect on its VisibleArea
		anyhow.
	*/
	
	if ((GetTablet () == Nil) or (not GetEffectiveVisibility ())) {
		Assert (fVisibleArea == Nil);
		return (kEmptyRegion);
	}
	
	if (fVisibleArea == Nil) {
		/*
		 * We must take into account being eclipsed by
		 * other windows.  Use of panel stuff instead of view should fix that.
		 */
		 
		View*	parent	=	GetParentView ();
		Region	visibleArea	=	GetDrawnRegion ();
		if ((parent != Nil) and (parent->GetTablet () == GetTablet ())) {
			visibleArea	*=	parent->GetVisibleArea ();	// we temporarily keep visibleArea is parent's coordinates
	
			if (not visibleArea.Empty ()) {
				/*
				 * Subtract out siblings which obscure us.
				 */
				AssertNotNil (parent);
				AssertNotNil (parent->fSubViews);	// after all, we are a subview!
				Rect	visibleBounds = visibleArea.GetBounds ();
				ForEach (ViewPtr, it, *parent->fSubViews) {
					register	View*	sibling	=	it.Current ();
					AssertNotNil (sibling);
		
					if (sibling == this) {
						break;		// Only go as far as this entry, since anybody else is 'behind' us.
					}
		
					/*
					 * Subtract out of the visible area that of any siblings who are in front of us.
					 * If they are not whole they may adjust thier shape to take that into account.
					 */
					if (sibling->GetVisible () and sibling->Opaque () and (sibling->GetTablet () == GetTablet ()) 
						and Intersects (visibleBounds, sibling->GetExtent ())) {

						visibleArea -= sibling->LocalToEnclosure (sibling->GetVisibleArea ());
					}
				}
			}
		}
		if (visibleArea.Empty ()) {
			((View*)this)->fVisibleArea = (Region*)&kEmptyRegion;
		}
		else {
			((View*)this)->fVisibleArea = new Region (EnclosureToLocal (visibleArea));
		}
	}
	
	EnsureNotNil (fVisibleArea);
	return (*fVisibleArea);
}

void	View::SetFont (const Font* font, Panel::UpdateMode updateMode)
{
	/*
	 * Note that we avoid deleting the saved font if it is one of the two constants
	 * that we also avoid copying below. This could be extended to more constants, but
	 * we only really know of these two...
	 */
	if (fHasFont) {
		Assert (fEffectiveFont != Nil);
		if ((fEffectiveFont != &kSystemFont) and (fEffectiveFont != &kApplicationFont)) {
			delete fEffectiveFont;
			fEffectiveFont = Nil;
		}
		fHasFont = False;
	}
	Assert (not fHasFont);

	if (font == Nil) {
		View*	parent	=	GetParentView ();
		if (parent == Nil) {
			fEffectiveFont = (Font*) &kDefaultFont;
		}
		else {
			fEffectiveFont = parent->fEffectiveFont;
		}
	}
	else {
		if ((font == &kSystemFont) or (font == &kApplicationFont)) {
			fEffectiveFont = (Font*) font;
		}
		else {
			fEffectiveFont = new Font (*font);
		}
		fHasFont = True;
	}
	AssertNotNil (fEffectiveFont);

	/*
	 * Important to have fEffectiveFont in good shape before we call this method of our children,
	 * since they will in turn, look back at this value.
	 */
	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		AssertNotNil (it.Current ());
		it.Current ()->InvalCurEffectiveFont (updateMode);
	}

	/*
	 * Call EffectiveChange method last since it might plausibly call Refresh (eImmediateUpdate), and
	 * that would be unfortunate if our children hadn't been updated yet.
	 */
	AssertNotNil (fEffectiveFont);
	EffectiveFontChanged (*fEffectiveFont, updateMode);
}

void	View::InvalCurEffectiveFont (Panel::UpdateMode updateMode)
{
	/*
		This routine gets called by Add/Remove SubView and by SetFont. This should cover every case
		where our "inherited" font changes. We don't bother to inval if we own a font, as our parent's
		value has no effect on us. Also, in most cases, invaling our font has no effect, cuz our parent
		used the default font, and our new parent does too.
		We are careful to call Inval on our children, and to call EffectiveFontChanged, only in the
		cases where our effective font really changed.
	*/

	if (fHasFont) {
		return;
	}
	else {
		const Font*	oldFont = fEffectiveFont;
		View*	parent	=	GetParentView ();
		if (parent == Nil) {
			fEffectiveFont = (Font*) &kDefaultFont;
		}
		else {
			fEffectiveFont = parent->fEffectiveFont;
		}
		AssertNotNil (fEffectiveFont);

		if (oldFont != fEffectiveFont) {
			if (fSubViews != Nil) {
				ForEach (ViewPtr, it, MakeSubViewIterator ()) {
					AssertNotNil (it.Current ());
					it.Current ()->InvalCurEffectiveFont (updateMode);
				}
			}
			EffectiveFontChanged (*fEffectiveFont, updateMode);
		}
	}
}

void	View::EffectiveFontChanged (const Font& /*newFont*/, UpdateMode /*updateMode*/)
{
	/*
	 * By default we do nothing here. If your subclass of view has its visual appearance depending
	 * on the current font, do a Refresh (updateMode) here. If its layout depends on the font, then
	 * do an InvalidateLayout (). If its CalcDefaultSize () depends on the background, then to a
	 * if (GetParentView()!=Nil) { GetParentView()->InvalidateLayout (); }.
	 */
}

Boolean		View::Opaque () const
{
	return (fHasBackground);
}

void	View::SetBackground (const Tile* tile, Panel::UpdateMode updateMode)
{
	if (fHasBackground) {
		Assert (fEffectiveBackground != Nil);
		if (fEffectiveBackground != &kDefaultBackground) {
			delete (fEffectiveBackground);
		}
		fHasBackground = False;
	}
	
	if (tile == Nil) {
		View*	parent	=	GetParentView ();
		if (parent == Nil) {
			fEffectiveBackground = (Tile*) &kDefaultBackground;
		}
		else {
			fEffectiveBackground = parent->fEffectiveBackground;
		}
	}
	else {
		if (tile == &kDefaultBackground) {
			fEffectiveBackground = (Tile*) &kDefaultBackground;
		}
		else {
			/*
			 * NB: Because of this usage, Tile cannot be safely subclassed.
			 */
			fEffectiveBackground = new Tile (*tile);
		}
		fHasBackground = True;
	}
	AssertNotNil (fEffectiveBackground);

	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		AssertNotNil (it.Current ());
		it.Current ()->InvalCurEffectiveBackground (updateMode);
	}

	/*
	 * Call EffectiveChange method last since it might plausibly call Refresh (eImmediateUpdate), and
	 * that would be unfortunate if our children hadn't been updated yet.
	 */
	EffectiveBackgroundChanged (*fEffectiveBackground, updateMode);
}

void	View::InvalCurEffectiveBackground (Panel::UpdateMode updateMode)
{
	/*
		This routine gets called by Add/Remove SubView and by SetBackground. This should cover every case
		where our "inherited" background changes. We don't bother to inval if we own a background, as our parent's
		value has no effect on us. Also, in most cases, invaling our background has no effect, cuz our parent
		used the default background, and our new parent does too.
		We are careful to call Inval on our children, and to call EffectiveBackgroundChanged, only in the
		cases where our effective background really changed.
	*/

	if (fHasBackground) {
		return;
	}
	else {
		const Tile*	oldTile = fEffectiveBackground;
		View*	parent	=	GetParentView ();
		if (parent == Nil) {
			fEffectiveBackground = (Tile*) &kDefaultBackground;
		}
		else {
			fEffectiveBackground = parent->fEffectiveBackground;
		}

		if (oldTile != fEffectiveBackground) {
			if (fSubViews != Nil) {
				ForEach (ViewPtr, it, MakeSubViewIterator ()) {
					AssertNotNil (it.Current ());
					it.Current ()->InvalCurEffectiveBackground (updateMode);
				}
			}
			AssertNotNil (fEffectiveBackground);
			EffectiveBackgroundChanged (*fEffectiveBackground, updateMode);
		}
	}
}

void	View::EffectiveBackgroundChanged (const Tile& /*newBackground*/, UpdateMode /*updateMode*/)
{
	/*
	 * By default we do nothing here. If your subclass of view has its visual appearance depending
	 * on the current background, do a Refresh (updateMode) here. If its layout depends on the background, then
	 * do an InvalidateLayout (). If its CalcDefaultSize () depends on the background, then to a
	 * if (GetParentView()!=Nil) { GetParentView()->InvalidateLayout (); }.
	 */
}

void	View::AddAdornment (Adornment* adornment, Panel::UpdateMode updateMode)
{
	Require (fAdornment != adornment);
	RequireNotNil (adornment);

	if ((updateMode == eNoUpdate) or (GetParentView () == Nil) or (GetTablet () == Nil)) {
		Assert (adornment->fOwner == Nil);		// better not have two
		adornment->fOwner = this;
		
		fAdornment = new CompositeAdornment (adornment, fAdornment);
		fAdornment->fOwner = this;
		DoInvalVisibleArea ();
	}
	else {
		Region	updateRegion = GetVisibleArea ();
		AddAdornment (adornment, eNoUpdate);
		updateRegion += GetVisibleArea ();
		updateRegion -= GetLocalRegion ();	// this assumes that adornments are outside of our bounds, reduces flicker, but is it correct???
		Refresh (updateRegion, updateMode);
	}
}

void	View::RemoveAdornment (Adornment* adornment, Panel::UpdateMode updateMode)
{
	RequireNotNil (adornment);
	RequireNotNil (fAdornment);
	
	if ((updateMode == eNoUpdate) or (GetParentView () == Nil) or (GetTablet () == Nil)) {
		fAdornment = fAdornment->RemoveAdornment (adornment);
		DoInvalVisibleArea ();
	}
	else {
		Region	updateRegion = GetVisibleArea ();
		fAdornment = fAdornment->RemoveAdornment (adornment);
		updateRegion -= GetLocalRegion ();	// this assumes that adornments are outside of our bounds, reduces flicker, but is it correct???
		Refresh (updateRegion, updateMode);
	}
}

void	View::OutLine (const Shape& s, const Rect& shapeBounds)
{
	if (GetEffectiveLive ()) {
		OutLine (s, shapeBounds, kDefaultPen);
	}
	else {
		static	const	Pen*	kGrayPen = Nil;
		if (kGrayPen == Nil) {
			kGrayPen = new Pen (PalletManager::Get ().MakeTileFromColor (kGrayColor), eCopyTMode, kDefaultPen.GetPenSize ());
		}
		AssertNotNil (kGrayPen);
		OutLine (s, shapeBounds, *kGrayPen);
	}
}

void	View::OutLine (const Shape& s, const Rect& shapeBounds, const Pen& pen)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	t->OutLine (s, LocalToTablet (shapeBounds), pen);
}

void	View::OutLine (const Shape& s, const Pen& pen)
{
	OutLine (s, Rect (kZeroPoint, s.GetLogicalSize ()), pen);
}

void	View::OutLine (const Shape& s)
{
	OutLine (s, Rect (kZeroPoint, s.GetLogicalSize ()));
}

void	View::Paint (const Shape& s, const Rect& shapeBounds)
{
	Paint (s, shapeBounds, kDefaultBrush);
}

void	View::Paint (const Shape& s, const Rect& shapeBounds, const Brush& brush)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	t->Paint (s, LocalToTablet (shapeBounds), brush);
}

void	View::Paint (const Shape& s, const Brush& brush)
{
	Paint (s, Rect (kZeroPoint, s.GetLogicalSize ()), brush);
}

void	View::Paint (const Shape& s)
{
	Paint (s, Rect (kZeroPoint, s.GetLogicalSize ()));
}

void	View::Fill (const Shape& s, const Rect& shapeBounds, const Tile& tile)
{
	Paint (s, shapeBounds, Brush (tile, eCopyTMode));
}

void	View::Fill (const Shape& s, const Tile& tile)
{
	Fill (s, Rect (kZeroPoint, s.GetLogicalSize ()), tile);
}

void	View::Fill (const Tile& tile)
{
	Fill (GetShape (), GetLocalExtent (), tile);
}

void	View::Erase (const Shape& s, const Rect& shapeBounds)
{
	Fill (s, shapeBounds, GetEffectiveBackground ());
}

void	View::Erase (const Shape& s)
{
	Erase (s, Rect (kZeroPoint, s.GetLogicalSize ()));
}

void	View::Erase ()
{
	Erase (GetShape (), GetLocalExtent ());
}

void	View::Gray (const Shape& s, const Rect& shapeBounds)
{
#if		0
	/*
	 * I think that with ColorQD 2.0 there is a new mode to gray things out.  Probably should
	 * check availability, and use that.
	 */
	if (False and OSConfiguration ().ColorQD32Available ()) {
		// not sure tile matters here???
		// doesn't seem to work
		static	const	Brush	kGrayBrush = Brush (kBlackTile, eGrayisTextOrTMode);
		Paint (s, shapeBounds, kGrayBrush);
	}
#endif
	static	const	Brush*	kGrayBrush = Nil;
	if (kGrayBrush == Nil) {
		kGrayBrush = new Brush (PalletManager::Get ().MakeTileFromColor (kGrayColor), eBicTMode);
	}
	AssertNotNil (kGrayBrush);
	Paint (s, shapeBounds, *kGrayBrush);
}

void	View::Gray (const Shape& s)
{
	Gray (s, Rect (kZeroPoint, s.GetLogicalSize ()));
}

void	View::Gray ()
{
	Gray (GetShape (), GetLocalExtent ());
}

void	View::Hilight (const Shape& s, const Rect& shapeBounds)
{
	static	const	Brush*	kHiliteBrush = Nil;
	
	if (kHiliteBrush == Nil) {
		#if		qMacGDI
			kHiliteBrush = new Brush (kBlackTile, eHilightTMode);
		#else	/*qMacGDI*/
			kHiliteBrush = new Brush (kBlackTile, eXorTMode);
		#endif	/*qMacGDI*/
	}
	AssertNotNil (kHiliteBrush);
	Paint (s, shapeBounds, *kHiliteBrush);
}

void	View::Hilight (const Shape& s)
{
	Hilight (s, Rect (kZeroPoint, s.GetLogicalSize ()));
}

void	View::Hilight ()
{
	Hilight (GetShape (), GetLocalExtent ());
}

void	View::Invert (const Shape& s, const Rect& shapeBounds)
{
	static	const	Brush*	kInvertBrush = Nil;
	if (kInvertBrush == Nil) {
		kInvertBrush = new Brush (kBlackTile, eXorTMode);
	}
	AssertNotNil (kInvertBrush);
	Paint (s, shapeBounds, *kInvertBrush);
}

void	View::Invert (const Shape& s)
{
	Invert (s, Rect (kZeroPoint, s.GetLogicalSize ()));
}

void	View::Invert ()
{
	Invert (GetShape (), GetLocalExtent ());
}

Coordinate	View::TextWidth (const String& text, const Point& at) const
{
	return (TextWidth (text, GetEffectiveFont (), at));
}

Coordinate	View::TextWidth (const String& text, const Font& font, const Point& at) const
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);					// should we just ignore???  FAMOUS TEXTWIDTH PROBLEM
										// if we ever get, try just using desktop (silently???)
	Coordinate result = t->TextWidth (text, font, LocalToTablet (at));
	return (result);
}

void	View::DrawText (const String& text, const Point& at)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::DrawText (text, GetEffectiveFont (), at);
}

void	View::DrawText (const String& text, const Font& font, const Point& at)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::DrawText (text, font, at);
}

void	View::DrawPicture (const Picture& picture, const Rect& intoRect)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::DrawPicture (picture, intoRect);
}

void	View::DrawPicture (const Picture& picture, const Point& at)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::DrawPicture (picture, at);
}

void	View::ScrollBits (const Rect& r, const Point& delta, const Tile& background)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::ScrollBits (r, delta, background);
}

void	View::ScrollBits (const Rect& r, const Point& delta)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::ScrollBits (r, delta, GetEffectiveBackground ());
}

void	View::BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::BitBlit (from, fromRect, toRect, tMode);
}

void	View::BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode, const Region& mask)
{
	Tablet*	t	=	GetTablet ();
	RequireNotNil (t);
	t->SetClip (LocalToTablet (GetVisibleArea ()));
	TabletOwner::BitBlit (from, fromRect, toRect, tMode, mask);
}

void	View::BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode)
{
	TabletOwner::BitBlit (fromRect, to, toRect, tMode);
}

void	View::BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode, const Region& mask)
{
	TabletOwner::BitBlit (fromRect, to, toRect, tMode, mask);
}

void	View::Render (const Region& updateRegion)
{
	Require (GetVisible ());
	Assert (not fNeedsLayout);
	Assert (not fChildNeedsLayout);

#if		qMacGDI
	// not sure always safe to do this if-test...
	if (GetBackground () != Nil) {
		GetTablet ()->SetOSBackground (*GetBackground ());
	}
#endif	/* qMacGDI */

	if (fHasBackground) {
		Erase ();
	}
	Draw (updateRegion);
	if (fAdornment != Nil) {
		fAdornment->Draw ();
	}

	if (fSubViews != Nil) {
		Rect	updateBounds = updateRegion.GetBounds ();

		ForEach (ViewPtr, it, MakeSubViewIterator (eSequenceBackward)) {
			register	View*	v	=	it.Current ();
			AssertNotNil (v);
			if (v->GetVisible () and Intersects (updateBounds, v->GetExtent ())) {
				Region	localUpdate = v->EnclosureToLocal (updateRegion) * v->GetVisibleArea ();
				if (not localUpdate.Empty ()) {
					v->Render (localUpdate);
				}
			}
		}
	}
}

void	View::AddSubView (View* subView, CollectionSize index, Panel::UpdateMode updateMode)
{
	// Consider having a childrenlistchanged method called with an updateMode to have that call
	// InvalidateLayout () if need be...

	RequireNotNil (subView);
	Require (subView->GetParentView () == Nil);
	
	if (index == eAppend) {
		index = GetSubViewCount () + 1;
	}

	Boolean	oldLive = subView->GetEffectiveLive ();
	Boolean	oldVisibility = subView->GetEffectiveVisibility ();

	subView->fParentView = this;

	subView->InvalCachedOrigin ();
	subView->DoInvalVisibleArea ();
	
	/* 
		These invalidations are efficient as they check to see whether their cached values really
		change or not, so we can consider them cheap to call with a clear conscience.
	*/
	subView->InvalCurEffectiveFont (updateMode);
	subView->InvalCurEffectiveBackground (updateMode);

	if (oldVisibility != subView->GetEffectiveVisibility ()) {
		subView->EffectiveVisibilityChanged (not oldVisibility, updateMode);
	}
	if (oldLive != subView->GetEffectiveLive ()) {
		subView->EffectiveLiveChanged (not oldLive, updateMode);
	}

	if (fSubViews == Nil) {
		fSubViews = new Sequence(ViewPtr);
	}
	AssertNotNil (fSubViews);
	fSubViews->InsertAt (subView, index);
	fChildNeedsLayout = True;
	InvalidateLayout ();
	subView->DoInvalVisibleArea (True);				// force appropriate siblings to have visible area adjusted

#if		qXtToolkit
	/*
	 * Only realize if we have a parent to realize with. We can be otherwise realized, but then thats
	 * someone elses responsability.
	 */
	View*	pwv	=	subView->GetParentWidgetView ();
	if (pwv != Nil) {
		AssertNotNil (pwv);
		osWidget*	pw	=	pwv->GetWidget ();
		AssertNotNil (pw);
		AssertNotNil (XtWindow (pw));	// otherwise we need to search for parent with a window...

		subView->Realize (pw);

		/*
		 * When we are realized we may have introduced new windows that must be restacked with respect
		 * to our X sibling windows. This is already taken care of for us, by code in View::Realize()
		 * except in the case where we have a widget at this level, but not when we have no widget, and
		 * it is our ancestor who must re-sync its list. So only in that case to we notify it, to resync.
		 */
		pwv->SyncronizeXWindowsWithChildWidgets ();
	}
#endif	/*qXtToolkit*/


// hack to put at end - to see if fixed font probelms..

	subView->SetTablet (GetTablet (), updateMode);
}

void	View::AddSubView (View* subView, View* neighborView, AddMode addMode, Panel::UpdateMode updateMode)
{
	RequireNotNil (neighborView);
	CollectionSize	index = fSubViews->IndexOf (neighborView);
	Require (index != kBadSequenceIndex);
	AddSubView (subView, (addMode == eAppend) ? index + 1 : index, updateMode);
}

void	View::RemoveSubView (View* subView, Panel::UpdateMode updateMode)
{
	// In order to be "exception friendly" we must allow RemoveSubView to be called with a Nil
	// pointer, and with a Nil parent. The first case happens if we built subviews by pointer
	// and had an exception raised early in our constructor (before we alloced the view), and
	// the second case happens when we build views in static memory, but get an exception
	// raised before we call AddSubView.
	// Of course, we won't trap some programmer bugs this way, but there seems to be no
	// reasonable alternative.
	
	if ((subView == Nil) or (subView->GetParentView () == Nil)) {
		return;
	}

	RequireNotNil (subView);
	Require (subView->GetParentView () == this);

	AssertNotNil (fSubViews);
	fSubViews->Remove (subView);

#if		qXtToolkit
	/*
	 * Always unrealize, even if we have no parent widget since we may be the top level view, and
	 * still have someone above us being the widget that the whole view/widget hierarchy is in.
<< NOT TRUE IN THIS CONTEXT- SB able to SAY IF GETPARENTWIDGET != Nil>>
	 */
	subView->UnRealize ();
#endif	/*qXtToolkit*/

// some of this could be optimized since we know about new effectiveVis, etc... dont need to call??
	Boolean	oldLive = subView->GetEffectiveLive ();
	Boolean	oldVisibility = subView->GetEffectiveVisibility ();

	Require (subView->fParentView == this);
	subView->fParentView = Nil;

	subView->SetTablet (Nil);
	subView->InvalCachedOrigin ();
	subView->DoInvalVisibleArea ();

	/* 
		These invalidations are efficient as they check to see whether their cached values really
		change or not, so we can consider them cheap to call with a clear conscience.
	*/
	subView->InvalCurEffectiveFont (updateMode);
	subView->InvalCurEffectiveBackground (updateMode);

	if (oldVisibility != subView->GetEffectiveVisibility ()) {
		subView->EffectiveVisibilityChanged (not oldVisibility, updateMode);
	}
	if (oldLive != subView->GetEffectiveLive ()) {
		subView->EffectiveLiveChanged (not oldLive, updateMode);
	}


	/*
	 * At this point in might make sense to delete the fSubViews list if we have a zero length, but
	 * that would be bad if somebody was currently iterating... Actually, this is quite likely,
	 * since it is common to iterate and remove subviews!
	 */
}

void	View::ReorderSubView (View* v, CollectionSize index, Panel::UpdateMode updateMode)
{
	RequireNotNil (v);
	if (index == eAppend) {
		index = GetSubViewCount ();	// not count + 1 cuz we will remove view first, shrinking list temporarily
	}
	Region	oldVis = v->GetVisibleArea ();
	v->DoInvalVisibleArea ();
	AssertNotNil (fSubViews);
	fSubViews->Remove (v);
	fSubViews->InsertAt (v, index);
	v->DoInvalVisibleArea (True);		// force siblings to have visible area adjusted

#if		qXtToolkit
	/*
	 * If we reorder our subviews, some of them may have widgets, which may in turn have X windows, and
	 * so we must reorder them. But we may not have a widget, and so all our subviews are and siblings
	 * X windows are reconend as really siblings of our ParentWidget View.
	 *
	 * Note this isnt quite right - we really want the first parent with a widget that has an X window...
	 */
	View*	pwv	=	GetParentWidgetView ();
	if (pwv != Nil) {
		RequireNotNil (pwv->GetWidget ());
		RequireNotNil (XtWindow (pwv->GetWidget ()));	// really should have searched parents for first widget that
														// had an X window!!!
		pwv->SyncronizeXWindowsWithChildWidgets ();
	}
#endif

	v->Refresh (v->GetVisibleArea () - oldVis, updateMode);
	Ensure (fSubViews->IndexOf (v) == index);
}

void	View::ReorderSubView (View* v, View* neighborView, AddMode addMode, Panel::UpdateMode updateMode)
{
	RequireNotNil (neighborView);
	Require (v != neighborView);
	
	CollectionSize	index = fSubViews->IndexOf (neighborView);
	CollectionSize	oldIndex = fSubViews->IndexOf (v);

	Require (index != kBadSequenceIndex);
	Require (oldIndex != kBadSequenceIndex);
	
	if (oldIndex < index) {
		index--;
	}
	ReorderSubView (v, (addMode == eAppend) ? index + 1 : index, updateMode);
}

CollectionSize	View::GetSubViewCount () const
{
	return ((fSubViews == Nil)? 0: fSubViews->GetLength ());
}

CollectionSize	View::GetSubViewIndex (View* v)
{
	return ((fSubViews == Nil)? kBadSequenceIndex: fSubViews->IndexOf (v));
}

View*	View::GetSubViewByIndex (CollectionSize index)
{
	Require (index >= 1);
	RequireNotNil (fSubViews);
	Require (index <= fSubViews->GetLength ());
	return ((*fSubViews)[index]);
}

SequenceIterator(ViewPtr)*	View::MakeSubViewIterator (SequenceDirection d) const
{
	if (fSubViews == Nil) {
		return (new NilViewPtrSequenceIterator ());
	}
	else {
		return (fSubViews->MakeSequenceIterator (d));
	}
}

/*
 * This routine invalidates our own visible area, but also invalidates the area of
 * any siblings we might obscure. It takes the alwaysCheckSiblings flag because they
 * are a few special circumstances where our fVisibleArea might be Nil, but where we
 * still need to be taken into account by our siblings. These times our when being added
 * as a subview, being reordered as a subview, and when being made visible. Even then,
 * we only invalidate our siblings if we are visible and opaque, then minimum conditions
 * needed to clip out a sibling.
 */
void	View::DoInvalVisibleArea (Boolean alwaysCheckSiblings)
{
	if (((fVisibleArea != Nil) and (fVisibleArea != &kEmptyRegion)) or alwaysCheckSiblings) {
		View*	parent	=	GetParentView ();
		if ((parent != Nil) and (parent->fVisibleArea != Nil) and Opaque () and (GetEffectiveVisibility ())) {
			// can invalidate siblings too (those who are visually behind us)
			AssertNotNil (parent);
			AssertNotNil (parent->fSubViews);
			ForEach (ViewPtr, it, parent->MakeSubViewIterator (eSequenceBackward)) {
				AssertNotNil (it.Current ());
				Assert (not it.Done ());
				if (it.Current () == this) {
					break;
				}
				it.Current ()->InvalVisibleArea ();
			}
		}
	}
	InvalVisibleArea ();
	Ensure (fVisibleArea == Nil);
}


/*
 * This routine is only called by DoInvalVisibleArea
 */
void	View::InvalVisibleArea ()
{
	if (fVisibleArea != Nil) {
		if (fVisibleArea != &kEmptyRegion) {
			delete (fVisibleArea);
		}
		fVisibleArea = Nil;

		/*
		 * If we had a non-nil VisibleArea, then some subview could have relied on it, and so we must
		 * inval them too.
		 */
		if (fSubViews != Nil) {
			ForEach (ViewPtr, it, MakeSubViewIterator ()) {
				AssertNotNil (it.Current ());
				it.Current ()->InvalVisibleArea ();
			}
		}
	}

#if qDebug
	/*
	 * Here we assume that if our visiblearea is nil, our childrens must be too. Do one
	 * Level check - too hard to do recursive check without changing headers, and that would be
	 * overkill (recursive routine).
	 */
	 
	ForEach (ViewPtr, it, MakeSubViewIterator ()) {
		AssertNotNil (it.Current ());
		Assert (it.Current ()->fVisibleArea == Nil);
	}
#endif	/* qDebug */

	Ensure (fVisibleArea == Nil);
}

void	View::InvalCachedOrigin ()
{
	if (fCachedOriginValid) {
		fCachedOriginValid = False;
		
		/*
		 * If we had a good fCachedOriginValid, then some subview could have one too,
		 * and so we must inval them too.
		 */
		if (fSubViews != Nil) {
			ForEach (ViewPtr, it, MakeSubViewIterator ()) {
				AssertNotNil (it.Current ());
				it.Current ()->InvalCachedOrigin ();
			}
		}
	}
	Ensure (not fCachedOriginValid);
}

Region	View::GetDrawnRegion () const
{
	if (fAdornment == Nil) {
		return (GetRegion ());
	}
	else {
#if 	qMPW_CFront_AccessShouldBeGenerousBug
		((Adornment*) fAdornment)->Invalidate (eNoUpdate);
#else
		fAdornment->Invalidate (eNoUpdate);
#endif
		return (GetRegion () + fAdornment->GetAdornRegion ());
	}
}

void	View::Layout ()
{
	fNeedsLayout = False;

#if		qXtToolkit && qBuildOurHackWidget
	AdjustOurHackWidget ();
#endif
}

void	View::InvalidateLayout ()
{
	if (not fNeedsLayout) {
		fNeedsLayout = True;
		NotifyEnclosureOfInvalidLayout ();
	}
}

Boolean	View::ProcessLayout (Boolean layoutInvisible)
{
	Boolean	didLayouts = False;
	if (fNeedsLayout) {
		didLayouts = True;
		Layout ();
	}

	/*
	 * Subtle point here - we not only check if our children need a layout, but also
	 * whether or not we do. If our layout call indirectly invalidated our layout, no
	 * point in laying out our children now - we will get another crack when we get layed
	 * out again.
	 *
	 * Not totally sure we want to do this, but it may be a slight optimization in some cases.???
	 * Sterl says they may not be that uncommon - what are examples???
	 */
	if (fChildNeedsLayout and (not fNeedsLayout)) {
		/*
		 * Very important to set fChildNeedsLayout false before iterating, since one of those
		 * might invalidate us again!!!.
		 */
		fChildNeedsLayout = False;
		didLayouts = True;
		ForEach (ViewPtr, it, MakeSubViewIterator ()) {
			AssertNotNil (it.Current ());
			if (it.Current ()->GetVisible () or layoutInvisible) {
				(void) it.Current ()->ProcessLayout (layoutInvisible);
			}
		}
	}
	return (didLayouts);
}

void	View::NotifyEnclosureOfInvalidLayout ()
{
	for (register View*	v = GetParentView (); (v != Nil) and not v->fChildNeedsLayout; v = v->GetParentView ()) {
		v->fChildNeedsLayout = True;
	}
}

Boolean	View::GetLive () const
{
	return (GetVisible ());
}

Boolean	View::GetEffectiveLive () const
{
	/*
	 * Slightly controversial definition... 
	 * We are only considered live if we have a non-live parent (by default). This is valuable,
	 * because it lets subclasses of view rely on on EffectiveLiveChanged being called whenever
	 * a view loses its parent (through a removesubview). This is a good place to add/remove idle
	 * tasks, for example. Not that some view somewhere (the "magic view" in window) must override
	 * GetEffectiveLive to be live even though it has no parent.
	 */
	return (Boolean (GetLive () and (fParentView != Nil) and (fParentView->GetEffectiveLive ())));
}

void	View::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	// This routine is a performance bottleneck, so check for nil fSubViews before we try construcing an
	// iterator...
	if (fSubViews != Nil) {
		ForEach (ViewPtr, it, MakeSubViewIterator ()) {
			/*
			 * Subtle Point: note that we simple call the GetLive method of It.Current, and do not compare this
			 * result with our oldLive. This is correct behavior. The difference between GetLive and GetEffectiveLive
			 * is the GetEffectiveLive takes your parent's liveness into account: GetEffectiveLive .=. (GetLive () and parent.GetEffectiveLive ())
			 * If a view is not live, than the parent's live state is irrelevant, since the view will never be GetEffectiveLive.
			 * So, we only pass on EffectiveLiveChanged calls to subviews who are logically live, since their EffectiveLive will
			 * be the same as our own. The point is the child could only have been Effective live in one case - it was live and all its
			 * parents, and so only if it is live, its effectivelive == ours, otherwise its effectivelive is false.
			 */
			View*	subView		=	it.Current ();
			if (subView->GetLive ()) {
				subView->EffectiveLiveChanged (newLive, updateMode);
			}
		}
	}
}

Point	View::LocalToTablet_ (const Point& p)	const
{
	if (not fCachedOriginValid) {
		if (GetParentView () == Nil) {
			((View*)this)->fCachedOrigin = LocalToEnclosure (kZeroPoint);
		}
		else {
			if (GetParentView ()->GetTablet () == GetTablet ()) {
				((View*)this)->fCachedOrigin = GetParentView ()->LocalToTablet (LocalToEnclosure (kZeroPoint));
			}
			else {
				((View*)this)->fCachedOrigin = kZeroPoint;	// If our parent changes tablet, then don't add in our origin
															// not necessarily the most self-evident definition????
			}
		}
		((View*)this)->fCachedOriginValid = True;
	}
	return (fCachedOrigin + p);
}

Point	View::TabletToLocal_ (const Point& p)	const
{
	if (not fCachedOriginValid) {
		if (GetParentView () == Nil) {
			((View*)this)->fCachedOrigin = LocalToEnclosure (kZeroPoint);
		}
		else {
			if (GetParentView ()->GetTablet () == GetTablet ()) {
				((View*)this)->fCachedOrigin = GetParentView ()->LocalToTablet (LocalToEnclosure (kZeroPoint));
			}
			else {
				((View*)this)->fCachedOrigin = kZeroPoint;	// If our parent changes tablet, then don't add in our origin
															// not necessarily the most self-evident definition????
			}
		}
		((View*)this)->fCachedOriginValid = True;
	}
	return (p - fCachedOrigin);
}








/*
 ********************************************************************************
 ***************************** NilViewPtrSequenceIterator **********************
 ********************************************************************************
 */

#if		!qRealTemplatesAvailable
Declare (BlockAllocated, NilViewPtrSequenceIterator);
Implement (BlockAllocated, NilViewPtrSequenceIterator);
#endif

Boolean		NilViewPtrSequenceIterator::Done () const
{
	return (True);
}

void		NilViewPtrSequenceIterator::Next ()
{
}

ViewPtr		NilViewPtrSequenceIterator::Current () const
{
	AssertNotReached ();	return (Nil);
}

CollectionSize	NilViewPtrSequenceIterator::CurrentIndex () const
{
	AssertNotReached ();	return (1);
}

SequenceDirection	NilViewPtrSequenceIterator::GetDirection () const
{
	return (eSequenceForward);
}

void*	NilViewPtrSequenceIterator::operator new (size_t n)
{
	RequireNotNil (n == sizeof (NilViewPtrSequenceIterator));
	return (BlockAllocated(NilViewPtrSequenceIterator)::operator new (n));
}

void	NilViewPtrSequenceIterator::operator delete (void* p)
{
	RequireNotNil (p);
	BlockAllocated(NilViewPtrSequenceIterator)::operator delete (p);
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

