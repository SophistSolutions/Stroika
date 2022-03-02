/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/OSControls.cc,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: OSControls.cc,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/16  18:00:34  lewis
 *		Set os background tile to effective background in prepare for draw in mac
 *		os controls.
 *
 *		Revision 1.3  1992/07/08  03:22:15  lewis
 *		Using AssertNotReached () instead of SwitchFallThru ().
 *
 *		Revision 1.2  1992/06/25  08:22:05  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_.
 *
 *		Revision 1.81  1992/06/03  21:18:00  lewis
 *		Add cast of tmp pascal string ptr to (const void*) since CFront under AUX (for mac os) started
 *		barfing on it without the cast. Not sure why.
 *
 *		Revision 1.79  92/05/01  01:37:01  01:37:01  lewis (Lewis Pringle)
 *		Comment out Application::Get ().SuspendCommandProcessing (); in motif trakcpress () - hack to do release.
 *		
 *		Revision 1.78  92/04/30  03:45:21  03:45:21  lewis (Lewis Pringle)
 *		Comment out suspend/resume of idletasks - prevents update event processing currently.
 *		This whole area is a mess. We must debug but low priority now.
 *		
 *		Revision 1.77  92/04/24  09:04:19  09:04:19  lewis (Lewis Pringle)
 *		Turn back on disabling of idle tasks, etc.. had some good and bad effects. Also, added gross hack
 *		for motif (ALL THIS IN MOTIF TRACKPRESS) loosing button release - peek out our last copy of the
 *		event - then we dont care if the Xt stuff works or not!.
 *		
 *		Revision 1.76  92/04/20  14:33:55  14:33:55  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix compliant.
 *		
 *		Revision 1.75  92/04/19  23:10:52  23:10:52  lewis (Lewis Pringle)
 *		Added comments.
 *		
 *		Revision 1.74  92/04/17  01:16:17  01:16:17  lewis (Lewis Pringle)
 *		Started adding code to patch GCs, but I think its hopeless, so I'm probably going to give it up.
 *		Also, in AdjustWidgetExtent(), effectiveorigin sb initailised to zeroPoint, no GetOrigin().
 *		Also, futzed with MotifOSContro::TrackPRess(). Made no real chagnes so far, but very unhappy with teh
 *		way it works - and usually doesn't - must find some safe way to fix it.
 *		
 *		Revision 1.73  92/04/16  10:42:55  10:42:55  lewis (Lewis Pringle)
 *		Assert GetOrigin ()==kZeroPoint in MotifOSControl::AdjustWidgetExtent () - I think algoritm was
 *		wrong,and only worked because this was so. Fix algorithm after we've tested, and thought it out
 *		more carefully.
 *		
 *		Revision 1.71  92/04/14  19:40:01  19:40:01  lewis (Lewis Pringle)
 *		Got rid of gdebugstreams, and commented out code.
 *		
 *		Revision 1.70  92/04/14  12:47:38  12:47:38  lewis (Lewis Pringle)
 *		Commented back in CleanupAfterDraw and PrepareForDraw() in MacOSControl::ThumbActionProc ()
 *		ReWrote MacOSControl::PrepareForDraw (UpdateMode updateMode) to use Tablet Coordinates rather than
 *		Quickdraw SetOrigin. Now we can cleanup code in Tablet. TrackPress also needed to be patched to make this work.
 *		Use new ToPStringTrunc routine.
 *		
 *		Revision 1.69  92/04/14  07:36:06  07:36:06  lewis (Lewis Pringle)
 *		Cleanup AdjustWidgetExtent().
 *		
 *		Revision 1.68  92/04/09  02:27:52  02:27:52  lewis (Lewis Pringle)
 *		Bigtime cleanup of MotifOSControl::Realize. Got rid of setting font, etc, and size there.
 *		Only do Manage if effectively visible, and comment carefully on why we dont use mappedWhenManged.
 *		Generally either got rid of magic, or commented on why it was needed.
 *		
 *		Revision 1.67  92/04/08  15:49:21  15:49:21  lewis (Lewis Pringle)
 *		Get rid of MagicView/MagicWidget for motif os controls.
 *		
 *		Revision 1.66  92/04/02  13:07:27  13:07:27  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.65  92/03/26  18:36:31  18:36:31  lewis (Lewis Pringle)
 *		Add InvalLayout of us and parent in Motif EffectiveFontChanged.
 *		
 *		Revision 1.64  1992/03/26  09:59:01  lewis
 *		Get rid of oldVisible arg to EffecitveVisibiltyChanged, and added newFont/updateMode to EffeictveFontChanged.
 *
 *		Revision 1.62  1992/03/19  16:55:05  lewis
 *		Allow nested GoReal/GoUnReal calls keeping a stack. Also, started trying to fixup TrackPress code to do right
 *		thing - needs more thought however - old hack of recursively calling event loop caused probelms cuz
 *		of idle tasks, etc... Tried just disabling them, but that caused trouble too. Much thought required to really fix
 *		right!!!
 *
 *		Revision 1.61  1992/03/17  03:06:52  lewis
 *		Tried restoring some of my previous hacks. mappedWhenManaged still broken, but some of the other resize
 *		things work somewhat. Also, by default use no magic view.
 *
 *		Revision 1.59  1992/03/16  19:37:50  lewis
 *		"Fix" for textedits appearing and then disapearing in Emily of trying to do
 *		closer to the "right thing" for AdjustWidgetExtent () cuases some other weird
 *		bugs with textedit. So disable it, and debug later.
 *
 *		Revision 1.58  1992/03/16  17:28:49  lewis
 *		Motif: Changed AdjustWidgetExtent () to not just return out if GetSize () =0,
 *		and to just use some small size(1,1) and really set that size. This is because
 *		otherwise some calls (like EffectiveFontChanged) will cause the widget to be
 *		drawn in the wrong place (this happened in Emily when TE Views were placed by
 *		a tracker).
 *
 *		Set mapped when managed to False, and override EffectiveVisibilityChanged to call XtMap/UnMap Widget -
 *		unfortunatly that screws up menubars, so tempoirarily hacked out - try again soon.
 *
 *		Revision 1.57  1992/03/13  16:37:40  lewis
 *		Wrap Try-Catch around RunOnce call in TrackPress () since it can raise exceptions.
 *		Fixed CalcDefaultSize for motif widgets - call was XtQueryGeometry - NOT XtMakeGeometryRequest???
 *		Still doesnt work well in all cases, but better.
 *
 *		Revision 1.56  1992/03/10  21:56:12  lewis
 *		Had sense reversed for isUp? operator to define if x event was KeyPress or KeyRelease in
 *		HandleKey for motif widgets.
 *
 *		Revision 1.55  1992/03/10  13:07:15  lewis
 *		Cleanup, and use a bit more of new keyboard stuff.
 *
 *		Revision 1.54  1992/03/10  00:31:49  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *		And use new HandleKeyEvent () intefact instead of old HandleKey () interface.
 *
 *		Revision 1.53  1992/03/05  20:36:52  sterling
 *		defferent (better?) margin/width defaults
 *
 *		Revision 1.52  1992/02/28  22:31:08  lewis
 *		Make Realize/UnRealize more lientient about being called when fWidget=Nil/not Nil.
 *
 *		Revision 1.50  1992/02/12  07:09:19  lewis
 *		Use GDIConfig rather than OSConfig.
 *
 *		Revision 1.49  1992/02/05  14:36:22  lewis
 *		Put back eventhandler for fWidget, but dont use pointer motion mask, since that seems to break
 *		textedit selecting. Needs more investigation.
 *
 *		Revision 1.48  1992/02/04  22:50:36  lewis
 *		Add kludge at the end of MotifOSControls::TrackPress () to wait for the buttonpress to finish before
 *		returning. This was important to get our textedit support working properly.
 *
 *		Revision 1.47  1992/02/04  18:48:23  sterling
 *		commented out pointer motion callback since we
 *		currently don't use, and currently breaks textedit
 *		probably cus we say motionmask instead of motionhint
 *
 *		Revision 1.46  1992/01/31  20:18:49  lewis
 *		Allow Nil event recored in bogus expose proc, since sometimes thats what motif gives us...
 *
 *		Revision 1.45  1992/01/31  05:18:12  lewis
 *		Added support for replacing the expose proc. VERY VERY big imporovment in how stroika graphix,
 *		and widget display works!! More work to be done, but ENOURMOUSLY better. Still a lot or work ahead
 *		to get the fine points right, and alot of worries about portablity and compatability with future releases.
 *		Also, note big comment on THUNKs - we never really implemented this, and I must think more about whether
 *		we should? Also, unclear how to deal with all the other various messages - handling expose procs was hard
 *		enuf!
 *
 *		Revision 1.44  1992/01/29  18:58:03  lewis
 *		Use DrawingArea widget instead of BullitenBoard for magic view. Got rid of bullitenboard specific resource setting.
 *
 *		Revision 1.43  1992/01/29  15:13:07  lewis
 *		Hacked MotifOSControl::CalcDefaultSize to return parameter if we fail to generate a good size (which
 *		sadly seems always to be the case - I wish I knew why).
 *
 *		Revision 1.39  1992/01/24  17:57:33  lewis
 *		Allow setting of widget values when unrelized - just ignore instead of asserting. Still
 *		Assert out on calls to GetValues when were unrealized.
 *
 *		Revision 1.38  1992/01/23  19:58:51  lewis
 *		Call AdjustWidgetExtent from realize, and in that routine, skip out if fwidget is nil (we now consider that OK),
 *		and also skip out of size is zero - THAT IS JUST A TEMP HACK - really we must find a way to set sizes where motif wont
 *		barf if we say something it doesnt like!!!!!
 *
 *		Revision 1.37  1992/01/23  08:09:24  lewis
 *		Cleanup and trash lots of rendundent implementations of things. Trash bogus widget design.
 *		Pull out lots of stuff that is now done redunently in different subclasses, but was in-appropriate for
 *		some others (not so sure that was a good idea).
 *
 *		Revision 1.36  1992/01/23  04:22:58  lewis
 *		Added ability to disable building the magic view which is needed in menus since they peek
 *		at their parent widget.
 *		Also, worked more on cloning, which is looking to be very difficult.
 *
 *		Revision 1.35  1992/01/22  21:27:18  lewis
 *		Work on widget cloning support.
 *
 *		Revision 1.34  1992/01/22  21:15:45  lewis
 *		Begin working on widget cloning support.
 *
 *		Revision 1.33  1992/01/20  12:24:56  lewis
 *		Added refresh to AdjustExtent () so in case Xm implemnets move of widget by copyarea,
 *		we wont get cruft (we were with textedit - perhaps a race?).
 *
 *		Revision 1.31  1992/01/20  06:54:25  lewis
 *		Added bogus, ignored callbacks for widgets to force us to get expose, pointer motion, and button press events.
 *
 *		Revision 1.30  1992/01/20  04:21:50  lewis
 *		Added Get/Set Unsinged Char Resoruce Value procs.
 *
 *		Revision 1.26  1992/01/17  18:44:05  lewis
 *		Put in hack to adjust the resize policy of the magic widget to avoid
 *		child resizing failure to come back and resize magic widget....
 *
 *		Revision 1.25  1992/01/17  17:51:37  lewis
 *		Added call to set margin height in addition to margin width.
 *
 *		Revision 1.23  1992/01/14  23:27:48  lewis
 *		Added MagicWidgetOwner support (though not fully utilized).
 *		Added XmNmarginWidth 0 to hopefully fix bug where bulliten boards would not place things where I
 *		asked it to.
 *
 *		Revision 1.22  1992/01/10  14:49:54  lewis
 *		Implemented bogus events for motif - not sure totally correct - only time will tell - just did ButtonPress and keyclick.
 *
 *		Revision 1.20  1992/01/10  04:02:27  lewis
 *		Try using LocalToTablet/TabletToLocal to avoid some global coord problems with X.
 *
 *		Revision 1.19  1992/01/10  03:23:40  lewis
 *		Tried to implement CalcDefaultSize () for motif widgets, but it didn't seem to work. Not sure why.
 *		Added boolean for SetSensative so that it can be adjusted before we realize. Also, moved Layout
 *		code to AdjustXXSize () so could be shared between layout, and Draw - now when a widets View parent
 *		is moved, the view does also.
 *
 *		Revision 1.16  1992/01/06  10:06:13  lewis
 *		Tried using XtConfigureWidget instead of XtSetValues (). Not really sure
 *		which is right to use. But they seem to behave slightly differently.
 *		Ugh...
 *
 *		Revision 1.15  1992/01/03  18:11:26  lewis
 *		Add nicer interface to setting resources (a bit more typesafe) and try to debug
 *		setting of size/origin for widgets with no room...
 *
 *		Revision 1.14  1991/12/27  22:39:20  lewis
 *		Implemented SetSensative.
 *
 *		Revision 1.13  1991/12/27  17:03:57  lewis
 *		Added new callback support.
 *
 *
 *
 */



#include	<string.h>

#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Controls.h>
#include	<Windows.h>
#elif 	qXtToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/IntrinsicP.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/StringDefs.h>
#if		qXmToolkit
#include	<Xm/Xm.h>
// not used anymore#include	<Xm/DrawingA.h>
#endif	/*qXmToolkit*/
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"

#include	"Debug.hh"
#include	"Mapping_HashTable.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"

#include	"DeskTop.hh"

#include	"OSControls.hh"








// temp hack include for motif...
#if		qXmToolkit
#include "Application.hh"
#include "EventManager.hh"
#endif







#if		qMacToolkit

/*
 ********************************************************************************
 ************************************ MacOSControl ******************************
 ********************************************************************************
 */
MacOSControl*	MacOSControl::sCurrentlyTracked	=	Nil;

static	osRect		kZeroMacRect = {0, 0, 0, 0};


static	osControlRecord**	sFreeControl = Nil;

MacOSControl::MacOSControl (short procID):
	View (),
	fOSRep (Nil),
	fProcID (procID)
{
	static	osWindowPtr	macWindow = ((GDIConfiguration ().ColorQDAvailable ()) ?
		(::NewCWindow (Nil, &kZeroMacRect, "\p", False, 0, Nil, False, 0)) :
		(::NewWindow  (Nil, &kZeroMacRect, "\p", False, 0, Nil, False, 0)));

	osControlRecord**	prev = Nil;
	osControlRecord**	next = sFreeControl;
	while (next != Nil) {
		if ((*next)->contrlRfCon == fProcID) {
			if (prev == Nil) {
				Assert (next == sFreeControl);
				sFreeControl = (osControlRecord**) (*next)->contrlOwner;
			}
			else {
				Assert ((osControlRecord**) (*prev)->contrlOwner == next);
				(*prev)->contrlOwner = (*next)->contrlOwner;
			}
			
			fOSRep = next;
			(*fOSRep)->contrlOwner = Nil;
			(*fOSRep)->contrlRfCon = (long) this;
			next = Nil;
		}
		else {
			prev = next;
			next = (osControlRecord**) (*next)->contrlOwner;
		}
	}

	if (fOSRep == Nil) {
		fOSRep = ::NewControl (macWindow, &kZeroMacRect, "\p", False, 0, 0, 0, (procID | useWFont), long (this));
		if (fOSRep == Nil) {
			gMemoryException.Raise ();
		}
		((osWindowPeek) macWindow)->controlList = Nil;
		(*GetOSRepresentation ())->contrlOwner = Nil;
	}
}

MacOSControl::~MacOSControl ()
{
	if (fOSRep != Nil) {
		Assert ((*fOSRep)->contrlOwner == Nil);
		Assert ((*fOSRep)->contrlVis == 0);
		(*fOSRep)->contrlRfCon = fProcID;
		(*fOSRep)->contrlValue = 0;
		(*fOSRep)->contrlMin = 0;
		(*fOSRep)->contrlMax = 0;
		(*fOSRep)->contrlOwner = (osGrafPort*) sFreeControl;
		sFreeControl = fOSRep;
	}
}

Boolean	MacOSControl::TrackPress (const MousePressInfo& mouseInfo)
{
	RequireNotNil (GetTablet ());
	PrepareForDraw (eImmediateUpdate);
		/*
		 * Patch control into window temporarily since mac control manager expects it to be that way.
		 */
		// SHOULD ASSERT SOMEHOW THAT thePort IS A WINDOW - HOW CAN YA TELL????
		Assert (osWindowPeek (qd.thePort)->controlList == Nil);
		Assert ((*GetOSRepresentation ())->nextControl == Nil);
		Assert ((*GetOSRepresentation ())->contrlOwner == qd.thePort);
		osWindowPeek (qd.thePort)->controlList = fOSRep;

		/*
		 * Use mac control manager to see if it thinks we got a click in this control  - probably unnessary, but
		 * it tells us the part code too, whick we use to decide which tracking proc to use.
		 */
		osPoint				osp;
		osControlRecord**	whichControl;
		short	partCode	=	::FindControl (os_cvt (LocalToTablet (mouseInfo.fPressAt), osp), qd.thePort, &whichControl);
		if (whichControl == Nil) {
			// could be disabled.
			osWindowPeek (qd.thePort)->controlList = Nil;
			CleanupAfterDraw ();
			return (True);
		}
		Assert (whichControl == GetOSRepresentation ());
		osWindowPeek (qd.thePort)->controlList = Nil;

#if		qMPW_PASCAL_PROC_PTR_BUG
		osProcPtr		actionProc	=	(partCode == inThumb)? osProcPtr ((void*)&ThumbActionProc): osProcPtr (DragActionProc);
#else
		osProcPtr		actionProc	=	(partCode == inThumb)? osProcPtr (ThumbActionProc): osProcPtr (DragActionProc);
#endif
		Assert (sCurrentlyTracked == Nil);
		sCurrentlyTracked = this;

		/*
		 * Invoke Mac Control manager, and have it call back to our actionProc - we regain control there, it it calls back to this.
		 */
		short	finalPartCode = ::TrackControl (GetOSRepresentation (), os_cvt (LocalToTablet (mouseInfo.fPressAt), osp), actionProc);
		Assert (sCurrentlyTracked == this);
		sCurrentlyTracked = Nil;

	CleanupAfterDraw ();

	Assert ((finalPartCode == 0) or (finalPartCode == partCode));	// I think illegal - but could be OK
	DoneProc (finalPartCode);
	return (True);
}

void	MacOSControl::Layout ()
{
	Assert ((*GetOSRepresentation ())->contrlOwner == Nil);
	Assert ((*GetOSRepresentation ())->contrlVis == 0);			// so it doesnt try to draw...
	PrepareForDraw (eNoUpdate);
		osPoint osp = os_cvt (GetSize (), osp);
		::SizeControl (GetOSRepresentation (), osp.h, osp.v);
	CleanupAfterDraw ();
	View::Layout ();
}

void	MacOSControl::Draw (const Region& update)
{
	RequireNotNil (GetTablet ());
	PrepareForDraw (eImmediateUpdate);
		::Draw1Control (GetOSRepresentation ());
	CleanupAfterDraw ();
	View::Draw (update);
}

UInt8	MacOSControl::GetHiliteState () const
{
	return ((*GetOSRepresentation ())->contrlHilite);
}

void	MacOSControl::SetHiliteState (UInt8 hiliteState, UpdateMode updateMode)
{
	if (GetHiliteState () != hiliteState) {
		PrepareForDraw (updateMode);
			::HiliteControl (GetOSRepresentation (), hiliteState);
		CleanupAfterDraw ();
		if (updateMode != eImmediateUpdate) {
			Refresh (updateMode);
		}
	}
}

Int16	MacOSControl::GetValue () const
{
	return (::GetCtlValue (GetOSRepresentation ()));
}

void	MacOSControl::SetValue (Int16 value, UpdateMode updateMode)
{
	if (GetValue () != value) {
		PrepareForDraw (updateMode);
			::SetCtlValue (GetOSRepresentation (), value);
		CleanupAfterDraw ();
		if (updateMode != eImmediateUpdate) {
			Refresh (updateMode);
		}
	}
}

Int16	MacOSControl::GetMin () const
{
	return (::GetCtlMin (GetOSRepresentation ()));
}

void	MacOSControl::SetMin (Int16 min, UpdateMode updateMode)
{
	if (GetMin () != min) {
		PrepareForDraw (updateMode);
			::SetCtlMin (GetOSRepresentation (), min);
		CleanupAfterDraw ();
		if (updateMode != eImmediateUpdate) {
			Refresh (updateMode);
		}
	}
}

Int16	MacOSControl::GetMax () const
{
	return (::GetCtlMax (GetOSRepresentation ()));
}

void	MacOSControl::SetMax (Int16 max, UpdateMode updateMode)
{
	if (GetMax () != max) {
		PrepareForDraw (updateMode);
			::SetCtlMax (GetOSRepresentation (), max);
		CleanupAfterDraw ();
		if (updateMode != eImmediateUpdate) {
			Refresh (updateMode);
		}
	}
}

String	MacOSControl::GetTitle () const
{
	osStr255	tmp;
	::GetCTitle (GetOSRepresentation (), tmp);
	return (String ((const void*)&tmp[1], tmp[0]));		// not sure why cast to (const void*) necessary...
														// Added by LGP June 2, 1992 since suddenly broke when I compiled this
														// under MacOS under AUX...
}

void	MacOSControl::SetTitle (const String& title, UpdateMode updateMode)
{
	if (GetTitle () != title) {
		PrepareForDraw (updateMode);
			osStr255	tmp;
			::SetCTitle (GetOSRepresentation (), title.ToPStringTrunc (tmp, sizeof (tmp)));
		CleanupAfterDraw ();

		if (updateMode != eImmediateUpdate) {
			Refresh (updateMode);
		}
	}
}

void	MacOSControl::ActionProc (short /*partCode*/)
{
	// Override this to be notified during tracking of mac os control - see Inside Mac
}

void	MacOSControl::DoneProc (short /*partCode*/)
{
	// Override this to be notified during tracking completed successfully
}

#if		qDebug
static	UInt32	sDepthCount = 0;
#endif	/*qDebug*/
static	UInt8	sOSSaves = 0;

void	MacOSControl::PrepareForDraw (UpdateMode updateMode)
{
	/*
	 * Setup the grafport so the control manager will be happy. For font information, and current pen, etc, if
	 * it is possible to get the control manager to use the ones we want, it is by setting it into the current
	 * port. Similarly, we setup clipping by setting the clip of the current port. Also,
	 * we set the current port to that of the control, so we can directly call control manager traps afterwards.
	 *
	 * If updateMode == eImmediateUpdate, then allow the control manager to draw directly.
	 * If updateMode == eNoUpdate, then set the control tonvisible (or if it cannot be drawn for other reasons).
	 *
	 * More controlversially, if we are in delayed update mode, then treat that as eNoUpdate. This is necessary
	 * since when the user says delayed update, he may be trying to avoid flicker, so we must not draw now - only
	 * at Update () call.
	 *
	 * Note that all coordinates for the toolbox are in Tablet coordinates.
	 */
	osControlRecord**	osc	=	GetOSRepresentation ();
#if		qDebug
	sDepthCount++;
	Assert (sDepthCount == 1);
#endif	/*qDebug*/

	if ((GetTablet () == Nil) or (updateMode != eImmediateUpdate) or (not GetEffectiveVisibility ())) {
		(*osc)->contrlVis = 0x00;
	}
	else {
		/*
		 * Do all calls for information first, and then the QD calls, since others may disturb
		 * QD environment (eg GetVisibleArea () might change the port).
		 */
		Brush		background		= Brush (GetEffectiveBackground (), eCopyTMode);
		Region		clipRegion		= GetVisibleArea ();

		sOSSaves++;
		Tablet::PrepareOSRep (GetTablet ()->GetOSGrafPtr ());
		Tablet::SetupOSRepFromFont (GetTablet ()->GetOSGrafPtr (), eOrTMode, GetEffectiveFont ());
		Tablet::SetupOSRepFromBrush (GetTablet ()->GetOSGrafPtr (), background);
		Tablet::SetupOSRepFromClip (GetTablet ()->GetOSGrafPtr (), LocalToTablet (clipRegion), kZeroPoint);
		GetTablet ()->SetOSBackground (GetEffectiveBackground ());

		(*osc)->contrlOwner = qd.thePort;

		/*
		 * Be sure its properly placed, and make it invisible when we move it to be sure its not drawn in the old or
		 * new position.
		 */
		(*osc)->contrlVis = 0x00;
		osPoint osp = os_cvt (LocalToTablet (kZeroPoint), osp);
		::MoveControl (osc, osp.h, osp.v);

		(*osc)->contrlVis = (GetVisible ()? 0xff: 0x00);
	
		Ensure (qd.thePort == GetTablet ()->GetOSGrafPtr ());
	}
}

void	MacOSControl::CleanupAfterDraw ()
{
#if		qDebug
	sDepthCount--;
	Assert (sDepthCount == 0);
#endif	/*qDebug*/
	if (sOSSaves > 0) {
		sOSSaves--;
		Tablet::RestoreOSRep ();
	}

	(*GetOSRepresentation ())->contrlVis = 0;
	(*GetOSRepresentation ())->contrlOwner = Nil;
}

pascal	void	MacOSControl::ThumbActionProc ()
{
	AssertNotNil (sCurrentlyTracked);
	sCurrentlyTracked->CleanupAfterDraw ();
	sCurrentlyTracked->ActionProc (inThumb);
	sCurrentlyTracked->PrepareForDraw (eImmediateUpdate);
}

pascal	void	MacOSControl::DragActionProc (struct osControlRecord** ctl, short partCode)
{
	AssertNotNil (sCurrentlyTracked);
	AssertNotNil (ctl);
	Assert (::GetCRefCon (ctl) == long (sCurrentlyTracked));

	sCurrentlyTracked->CleanupAfterDraw ();
	sCurrentlyTracked->ActionProc (partCode);
	sCurrentlyTracked->PrepareForDraw (eImmediateUpdate);
}



#elif	qXmToolkit






/*
 ********************************************************************************
 ********************************** MotifOSControl ******************************
 ********************************************************************************
 */

static	void	HandlePointerMotion (osWidget* /*w*/, char* /*clientData*/, osEventRecord* /*e*/, osBoolean* /*continue_to_dispatch*/)
{
}

static	void	HandleButtonPress (osWidget* /*w*/, char* /*clientData*/, osEventRecord* /*e*/, osBoolean* /*continue_to_dispatch*/)
{
}

static	Boolean	sButtonPressed = False;	
static	void	HandleButtonUpDuringTrackPressWait (osWidget* /*w*/, char* /*clientData*/, osEventRecord* /*e*/, osBoolean* /*continue_to_dispatch*/)
{
	Assert (sButtonPressed);
	sButtonPressed = False;
}








/*
 * High level goal:

// NOT YET IMPLEMENTED!!!


 *
 *		To Replace an expose proc with our own, and still be able to eventually call the widgets
 * expose proc.
 *
 *		Below is a rather ugly hack, but the next best thing I can think of would be simpler, but non-portable.
 * The issue at hand is that when we replace a callback proc for a given class (in this case exposeProc),
 * when we get called, we need to know which one we are replacing (eg are we replacing the pushbutton expose proc,
 * or the label expose proc).
 *
 *		There are a number of approaches that don't work that might leap to mind (they did for me). One, is to
 * stick a user-data style value into the widget instance and use that to figure out what we are overriding.
 * That fails for two reasons - the first is that Xt does not appear to have such a feature (though I think motif
 * does and otherwise we could probably finagle something), and secondly, and more importantly for the same reason
 * that using the class of the widget fails.
 *
 *		Why does that fail? Why cannot we save the old method associated with the given class, and when we get called
 * for a given widget, look at the class field, lookup in a Mapping the appropriate REAL expose proc, and call that?
 *
 *		!!!!!!!!!!!Sometimes people call the INHERITED proc for a given widget, and the way they do that is indirect,
 * through the class pointer for their base class!!!!!!!!!!!!!!!!!! (if they did it the way C++ does by directly
 * jumping to the right procedure based on static type analysis we would not have any trouble, and the above techique
 * WOULD work.
 *
 *		So, we are left in a situation where we must replace the widget expose proc, and when we are called, we have
 * to figure out which one is being replaced. We have three parameters, a widget, and event, and a region. The
 * later two we clearly can learn nothing from, and we've already talked about why it would be difficult to learn
 * anything from the widget parameter. So the only thing left, is to use THUNKS.
 *
 *		How do we implement thunks? The easiest, and most extensible way is just to allocate some space on the heap,
 * and patch instructions into memory. This has the difficultly that it is fairly non-portable, and on any given
 * system would take a day or so for me to port (obviously its fairly easy for someone familiar with the target system).
 * However, on some systems it may even be imposible. I don't know for sure, but some systems I suspsect look awry
 * at executing code in data space. I wouldn't want to use such a system, since its a very handy technique for optimizing
 * certain things, but ...
 *
 *		So what other choice do we have to implement these thunks. Each subclass of MotifOSControl must provide
 * a procptr in the constructor that calls an agreed upon method of MotifOSControl (with a th, and we simply use that as
 * our thunk.
 *
 *
 */

//void	MotifOSControl::ReplacementExposeProc (osWidget* w, osEventRecord* event, osRegion* r);


struct	SavedWidgetClassPtrs {
	void	(*fRealExposeProc)(osWidget* w, osEventRecord* event, osRegion*);
};
typedef	osWidgetClass*	osWidgetClassPtr;

#if		!qRealTemplatesAvailable
		AbMappingDeclare (osWidgetClassPtr, SavedWidgetClassPtrs);
		Mapping_HTDeclare(osWidgetClassPtr, SavedWidgetClassPtrs);
		
		AbMappingImplement (osWidgetClassPtr, SavedWidgetClassPtrs);
		Mapping_HTImplement1 (osWidgetClassPtr, SavedWidgetClassPtrs);
		Mapping_HTImplement2 (osWidgetClassPtr, SavedWidgetClassPtrs);
		Mapping_HTImplement3 (osWidgetClassPtr, SavedWidgetClassPtrs);
#endif

static	Mapping_HashTable (osWidgetClassPtr,SavedWidgetClassPtrs)	sWidgetClassInfo;




/*
 * Use a Dictionary to map wigets to our class pointers cuz in callbacks we need some way to find
 * out the class we are represenating. Use a hash table, cuz we want this lookup to be FAST.
 *
 * Consider looking at collisions - we could throw more memory at this!!!
 */
typedef	osWidget*		osWidgetPtr;
typedef	MotifOSControl*	MotifOSControlPtr;

#if		!qRealTemplatesAvailable
		AbMappingDeclare (osWidgetPtr,MotifOSControlPtr);
		Mapping_HTDeclare(osWidgetPtr,MotifOSControlPtr);
		
		AbMappingImplement (osWidgetPtr,MotifOSControlPtr);
		Mapping_HTImplement1 (osWidgetPtr,MotifOSControlPtr);
		Mapping_HTImplement2 (osWidgetPtr,MotifOSControlPtr);
		Mapping_HTImplement3 (osWidgetPtr,MotifOSControlPtr);
#endif


static	Mapping_HashTable (osWidgetPtr,MotifOSControlPtr)	sWidgets;










UInt8	MotifOSControl::sRealModeDepth	=	0;
void	MotifOSControl::GoReal ()
{
	Require (sRealModeDepth <= 10);	// sanity test

	// only first time thru do we bother doing the replaces...
	if (sRealModeDepth == 0) {
		ForEach (DictionaryElement(osWidgetClassPtr,SavedWidgetClassPtrs), it, sWidgetClassInfo) {
			(it.Current ().fKey)->core_class.expose = it.Current ().fElt.fRealExposeProc;
		}
	}
	sRealModeDepth++;
}
void	MotifOSControl::GoUnReal ()
{
	Require (sRealModeDepth > 0);
	sRealModeDepth--;
	// Only if totally done with realmode pushes do we go back to ReplacementExposeProc
	if (sRealModeDepth == 0) {
		ForEach (DictionaryElement(osWidgetClassPtr,SavedWidgetClassPtrs), it, sWidgetClassInfo) {
			(it.Current ().fKey)->core_class.expose = (XtExposeProc)ReplacementExposeProc;
		}
	}
}




MotifOSControl::MotifOSControl ():
	View (),
	fWidget (Nil)
{
}

MotifOSControl::~MotifOSControl ()
{
	Require (fWidget == Nil);		// must be unrealized before destroyed...
	Assert (fWidget == Nil);
}

Boolean	MotifOSControl::TrackPress (const MousePressInfo& mouseInfo)
{
	/*
	 * Generate a bogus event for the widget. The only trouble is that there may be
	 * some info from the real event that generated this that we want to keep. Thats tough, since
	 * at this point, we don't event know if there was such a beast.
	 *
	 * Ultimately, the best solution to this problem, is probably to add an eventRecord to the
	 * mouse info, so we can just use it here (must be a boolean to say if MosuePressInfo created
	 * with event or not).
	 *
	 * For now, just snag last event, and copy a few stray fields...
	 */
	RequireNotNil (fWidget);
	osEventRecord	er;

	EventManager::Get ().GetCurrentEvent (er);
	er.xbutton.type = ButtonPress;
	er.xbutton.display = XtDisplay (fWidget);
	er.xbutton.window = XtWindow (fWidget);		// IMPLIES WIDGET - NOT GADGET - CLEANUP FOR GADGET
	AssertNotNil (er.xbutton.window);
	er.xbutton.root = DefaultRootWindow (er.xbutton.display);
	er.xbutton.x = int (mouseInfo.fPressAt.GetH ());
	er.xbutton.y = int (mouseInfo.fPressAt.GetV ());
	Point	globalPoint	=	LocalToAncestor (mouseInfo.fPressAt, &DeskTop::Get ());
	er.xbutton.x_root = int (globalPoint.GetH ());
	er.xbutton.y_root = int (globalPoint.GetV ());
	er.xbutton.same_screen = True;
	if (er.xbutton.button == 0) {
		er.xbutton.button = Button1;		 // if probably bogus event...
	}


#if		0


// I would like to do something that doesnt do full sub event loop, but not sure I can?? If I dont do subevent loop
// I could miss imporant event, but if I do, I could get in deep over my head??? Guess I'll just have to do that, and
// be careful?? No - some compromise, wheere I can still break out of loop below based on some other criteria from virtual
// method...
  /*
   * New MotifOSControl::TrackPress () strategy ---
   */
   for (; not TrackPressDone ();) {
       XEvent event;
       XtAppNextEvent (app, &event);
	   TrackPressDispatchNextEvent (event);
//       XtDispatchEvent (&event);
   }


#else

//Application::Get ().SuspendIdleTasks ();
//Application::Get ().SuspendCommandProcessing ();
//GoReal ();
	::XtDispatchEvent (&er);

	/*
	 * The below hack of installing an eventhandler and then removing it after a while loop is to provide
	 * the semantics that when you call TrackPress() it doesn't return until the user is done with the event
	 * (ie has finished tracking). This is not the greatest way of solving the problem. In particular, it seems
	 * important to put our handler at the HEAD of the list (thats why we dont call XtAddEventHandler) so that
	 * we dont get called til too late.
	 *
	 * The whole business is necessary, for example, in textedit, you want to inval the menus when the tracking is
	 * done so they get updated properly (thats when we first noticed the bug).
	 */
	::XtInsertEventHandler (fWidget, ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil, XtListHead);
	sButtonPressed = True;
	Exception* caught = Nil;
	Try {
		while (sButtonPressed) {
//gDebugStream << "in oscontrol eventloop about to call runonce" << newline;
			EventManager::Get ().RunOnce ();
			osEventRecord	xxxer;
			EventManager::Get ().GetCurrentEvent (xxxer);
			if ((xxxer.xbutton.type == ButtonRelease) and sButtonPressed) {
//gDebugStream << "got bad cased in motif oscotnrols trackpress FIXED!!!" << newline;
				break;
			}
		}
	}
	Catch () {
		caught = _this_catch_;
	}
//GoUnReal ();
//Application::Get ().ResumeIdleTasks ();
//Application::Get ().ResumeCommandProcessing ();
//gDebugStream << "done with while loop and removing event handler" << newline;
	::XtRemoveEventHandler (fWidget, ButtonReleaseMask, FALSE, HandleButtonUpDuringTrackPressWait, Nil);
	if (caught != Nil) {
		caught->Raise ();
	}
#endif


	return (True);
}

Boolean	MotifOSControl::HandleKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
										KeyComposeState& composeState)
{
	/*
	 * Generate a bogus event for the widget. The only trouble is that there may be
	 * some info from the real event that generated this that we want to keep. Thats tough, since
	 * at this point, we don't event know if there was such a beast.
	 *
	 * One tough question is do we do a keyPress or release - must study problem more...
	 */
	RequireNotNil (fWidget);
	osEventRecord	er;

	EventManager::Get ().GetCurrentEvent (er);

	er.xkey.type = isUp? KeyRelease: KeyPress;
	er.xkey.display = XtDisplay (fWidget);
	er.xkey.window = XtWindow (fWidget);		// IMPLIES WIDGET - NOT GADGET - CLEANUP FOR GADGET
	AssertNotNil (er.xkey.window);
	er.xkey.root = DefaultRootWindow (er.xkey.display);
	er.xkey.same_screen = True;

	// Also, fold this into the GetEffectiveKeyboard code in EventManager...
	if (keyBoardState.GetKey (KeyBoard::eShiftKey)) {
		er.xkey.state |= ShiftMask;
	}
	er.xkey.keycode = code;

	::XtDispatchEvent (&er);

	return (True);
}

void	MotifOSControl::Layout ()
{
	AdjustWidgetExtent ();
	View::Layout ();
}

static	osEventRecord	mkBogusExposeEvent (const Region update, osWidget* w);
static	osEventRecord	mkBogusExposeEvent (const Region update, osWidget* w)
{
	osEventRecord er;
	memset (&er, 0, sizeof (er));

	EventManager::Get ().GetCurrentEvent (er);
	er.xbutton.type = Expose;
	er.xbutton.display = XtDisplay (w);
	er.xbutton.window = XtWindow (w);		// IMPLIES WIDGET - NOT GADGET - CLEANUP FOR GADGET
	AssertNotNil (er.xexpose.window);
	er.xexpose.x = int (update.GetBounds ().GetOrigin ().GetH ());
	er.xexpose.y = int (update.GetBounds ().GetOrigin ().GetV ());
	er.xexpose.width = int (update.GetBounds ().GetSize ().GetH ());
	er.xexpose.height = int (update.GetBounds ().GetSize ().GetV ());
	return (er);
}

void	MotifOSControl::Draw (const Region& update)
{
	AdjustWidgetExtent (eImmediateUpdate);		// in case its been moved
	View::Draw (update);

	GoReal ();
		SavedWidgetClassPtrs	swcp;
		if (not sWidgetClassInfo.Lookup (fWidget->core.widget_class, &swcp)) {
			if (not sWidgetClassInfo.Lookup (fWidget->core.widget_class->core_class.superclass, &swcp)) {
				AssertNotReached ();		// we lost our record???
			}
		}
		AssertNotNil (swcp.fRealExposeProc);
		osEventRecord	er	=	mkBogusExposeEvent (update, fWidget);
		(*swcp.fRealExposeProc) (fWidget, &er, update.PeekAtOSRegion ());		// probably should pass real event???
	GoUnReal ();
}

Point	MotifOSControl::CalcDefaultSize_ (const Point& defaultSize) const
{
	if (fWidget == Nil) {
		return (defaultSize);
	}
	else {
		XtWidgetGeometry	request;
		XtWidgetGeometry	reply;

		memset (&request, 0, sizeof (request));
		memset (&reply, 0, sizeof (reply));

		request.request_mode |= CWWidth; 
		request.request_mode |= CWHeight; 
		request.request_mode |= CWBorderWidth;
		request.request_mode |= XtCWQueryOnly;

		/*
		 * This code assumes sizes dont include border (but my OSCOntrol's do).
		 */
		request.width = int (defaultSize.GetH ());
		if (request.width >= 2*fWidget->core.border_width) {
			request.width -= 2*fWidget->core.border_width;
		}
		else {
			request.width = 0;
		}
		request.height = int (defaultSize.GetV ());
		if (request.height >= 2*fWidget->core.border_width) {
			request.height -= 2*fWidget->core.border_width;
		}
		else {
			request.height = 0;
		}
		switch (::XtQueryGeometry (fWidget, &request, &reply)) {
			case	XtGeometryYes:		break;	// good
#if		qDebug
			case	XtGeometryNo:		gDebugStream << "BAD???" << newline; break;		// bad??? I Only asked...
			case	XtGeometryAlmost:	gDebugStream << "ALMOST?" << newline; break;	// OK...
#endif
			default:	AssertNotReached ();
		}

		Point	result	=	Point (reply.height + 2*reply.border_width, reply.width + 2*reply.border_width);
		return (result);
	}
}

void	MotifOSControl::AdjustWidgetExtent (UpdateMode updateMode)
{
	if (GetOSRepresentation () != Nil) {
		RequireNotNil (fWidget);		// must be realized by now...

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
		::XtSetValues (fWidget, &arg, 1);
		::XtConfigureWidget (fWidget, (int)effectiveOrigin.GetH (), (int)effectiveOrigin.GetV (),
							 (int)size.GetH (), (int)size.GetV (), fWidget->core.border_width);
		XtSetArg (arg, XmNresizePolicy, XmRESIZE_NONE);
		::XtSetValues (fWidget, &arg, 1);
	}
}

osWidget*	MotifOSControl::GetWidget () const
{
	return (fWidget);
}

void	MotifOSControl::GetResourceValue (const char* resourceID, void* xtArgValue, size_t sizeVal) const
{
	RequireNotNil (resourceID);
	RequireNotNil (fWidget);
	Require (sizeVal <= sizeof (XtArgVal));
	Arg		arg;
	XtSetArg (arg, (char*)resourceID, xtArgValue);
	::XtGetValues (GetOSRepresentation (), &arg, 1);
}

void	MotifOSControl::SetResourceValue (const char* resourceID, long xtArgValue)
{
	RequireNotNil (resourceID);
	if (GetOSRepresentation () != Nil) {
		Arg		arg;
		XtSetArg (arg, (char*)resourceID, xtArgValue);
		::XtSetValues (GetOSRepresentation (), &arg, 1);
	}
}

Boolean	MotifOSControl::GetBooleanResourceValue (const char* resourceID) const
{
	osBoolean	osb = 0;
	GetResourceValue (resourceID, &osb, sizeof (osb));
	return (Boolean (!!osb));
}

void	MotifOSControl::SetBooleanResourceValue (const char* resourceID, Boolean value)
{
	SetResourceValue (resourceID, (XtArgVal)value);
}

UInt16	MotifOSControl::GetDimensionResourceValue (const char* resourceID) const
{
	osDimension anInt = 0;
	GetResourceValue (resourceID, &anInt, sizeof (anInt));
	return (anInt);
}

void	MotifOSControl::SetDimensionResourceValue (const char* resourceID, UInt16 value)
{
	SetResourceValue (resourceID, (XtArgVal)value);
}

UInt8	MotifOSControl::GetUnsignedCharResourceValue (const char* resourceID) const
{
	UInt8	anInt = 0;
	GetResourceValue (resourceID, &anInt, sizeof (anInt));
	return (anInt);
}

void	MotifOSControl::SetUnsignedCharResourceValue (const char* resourceID, UInt8 value)
{
	SetResourceValue (resourceID, value);
}

int	MotifOSControl::GetIntResourceValue (const char* resourceID) const
{
	int anInt = 0;
	GetResourceValue (resourceID, &anInt, sizeof (anInt));
	return (anInt);
}

void	MotifOSControl::SetIntResourceValue (const char* resourceID, int value)
{
	SetResourceValue (resourceID, value);
}

String	MotifOSControl::GetStringResourceValue (const char* resourceID) const
{
	AssertNotImplemented (); return (kEmptyString);
}

void	MotifOSControl::SetStringResourceValue (const char* resourceID, const String& value)
{
	if (fWidget != Nil) {
		char*	tmp = value.ToCString ();
		XmString mStrTmp = XmStringCreate (tmp, XmSTRING_DEFAULT_CHARSET);
			SetResourceValue (XmNlabelString, (XtArgVal)mStrTmp);
		::XmStringFree (mStrTmp);
		delete (tmp);
	}
}

void	MotifOSControl::EffectiveFontChanged (const Font& newFont, UpdateMode updateMode)
{
	if (fWidget != Nil) {
		Arg args[1];
		XmFontList	fontList	=	::XmFontListCreate ((XFontStruct*)newFont.GetXFontStruct (), XmSTRING_DEFAULT_CHARSET);
		XtSetArg (args[0], XmNfontList, fontList);
		::XtSetValues (fWidget, args, 1);
		::XmFontListFree (fontList);
	}
	InvalidateLayout ();		// since our layout probably depends on font info
	if (GetParentView () != Nil) {
		GetParentView ()->InvalidateLayout ();	// since our CalcDefaultSize () depends on font info, our parent will
												// probably want to re-lay things out...
	}
	View::EffectiveFontChanged (newFont, updateMode);
}

void	MotifOSControl::EffectiveVisibilityChanged (Boolean newVisible, UpdateMode updateMode)
{
	if (fWidget != Nil) {
		if (newVisible) {
			::XtManageChild (fWidget);		// Be sure were manged by this point
			::XtMapWidget (fWidget);
		}
		else {
			::XtUnmapWidget (fWidget);
		}
	}
	View::EffectiveVisibilityChanged (newVisible, updateMode);
}

void	MotifOSControl::Realize (osWidget* parent)
{
	RequireNotNil (parent);
// See why this fails - SOON - april 14, 1992	Require (fWidget == Nil or XtParent (fWidget) == parent);

	if (fWidget == Nil) {
		/*
		 * Build the actual widget. This is a virtual method call, and subclasses must override this to build the
		 * actaul widget we are wrapping.
		 */
		Assert (fWidget == Nil);
		fWidget = DoBuild_ (parent);
		AssertNotNil (fWidget);


#if		1
		{
			// This probably does not belong here - only applies to LabelWidgets (And perhaps, but
			// not sure about htings that subclass or own LabelWidgets). Leave for now, cuz
			// not doing any harm????
			Arg args[1];
			XtSetArg (args[0], XmNrecomputeSize, (XtArgVal) False);
			::XtSetValues (fWidget, args, 1);
		}
#endif


		/*
 		 * Do this before being realized, so that we are less likely to get drawing done outside of our draw method.
		 * Drawing during the realize could cause unsightly flicker.
		 */
		SavedWidgetClassPtrs	swcp;
		if (not sWidgetClassInfo.Lookup (fWidget->core.widget_class, &swcp)) {
			swcp.fRealExposeProc = fWidget->core.widget_class->core_class.expose;
			sWidgetClassInfo.Enter (fWidget->core.widget_class, swcp);

			// how could this happen???
			Assert (fWidget->core.widget_class->core_class.expose != (XtExposeProc)ReplacementExposeProc);

			// only replace this NOW if we are currently at depth 0
			if (sRealModeDepth == 0) {
				fWidget->core.widget_class->core_class.expose = (XtExposeProc)ReplacementExposeProc;
			}
		}
#if		qDebug
		{
			MotifOSControl* xxx;
			Assert (not sWidgets.Lookup (fWidget, &xxx));
		}
#endif
		sWidgets.Enter (fWidget, this);


		/*
		 * Would like to be able to say XtSetArg (args[0], XtNmappedWhenManaged, False); here
		 * and then manage the widget, right after realizeing it.
		 *
		 * (Orielly VI-231 recomends realizing before managing... (at least for manager widgets).)
		 *
		 * However, that seems to screw up motif text edit. Focus never seems to change, and te widgets
		 * dont seem to respond to clicks. Therefore, we backdown, and dont do it, but instead, delay actually
		 * managing til the widget is made visible. This means we must call xtmanage in the EffectiveVisibiltyChanged()
		 * method, and below, when we've checked if we are effectively visible.
		 */
		::XtRealizeWidget (fWidget);


		/*
		 * Add bogus event handlers so that the X window selects the given events.
		 *
		 * Not sure why we cannot select PointerMotionHintMask, but seems to cause damage to textEdit. I should think
		 * that it would only apply in the situation where all PointerMotion callbacks had selected it,
		 * but that doesnt seem to be the case - ourselecting the hint flag seems to force textEdit to only
		 * get hints - or some such problem. Would like to be able to set PointerMotionHintMask.
		 */
		::XtAddEventHandler (fWidget, PointerMotionMask, FALSE, HandlePointerMotion, Nil);
		::XtAddEventHandler (fWidget, ButtonPressMask, FALSE, HandleButtonPress, Nil);

		AdjustWidgetExtent (eNoUpdate);
		if (GetEffectiveVisibility ()) {
			::XtManageChild (fWidget);		// only manage ourselves when we become visible - see EffectiveVisibilityChanged()...
			::XtMapWidget (fWidget);
		}
	}

	EnsureNotNil (fWidget);
}

void	MotifOSControl::UnRealize ()
{
	// really should copy out old values for label etc, so if we are re-realized
	// we can set them properly. If all went thru our interfaces this saving
	// would be unncessary, but try to do right thing if people directly access
	// widget rep, and if widgets themselves do!!! (eg editing text...)

	if (fWidget != Nil) {
#if		qDebug
		{
			MotifOSControl* xxx;
			Assert (sWidgets.Lookup (fWidget, &xxx));
		}
#endif
		sWidgets.Remove (DictionaryElement(osWidgetPtr,MotifOSControlPtr) (fWidget, this));
	
		::XtDestroyWidget (fWidget);
		fWidget = Nil;
	}
}

void	MotifOSControl::PatchGCs ()
{
}

void	MotifOSControl::UnPatchGCs ()
{
}

void	MotifOSControl::PatchPrimitvesGCs ()
{
}

void	MotifOSControl::UnPatchPrimitvesGCs ()
{
}

void	MotifOSControl::ReplacementExposeProc (osWidget* w, osEventRecord* event, osRegion* r)
{
	RequireNotNil (w);
	Require (sRealModeDepth == 0);
	SavedWidgetClassPtrs	swcp;
#if		qDebug
	memset (&swcp, 0, sizeof (swcp));		// so AssertNotNil() below tells is really non-nil
#endif
	if (not sWidgetClassInfo.Lookup (w->core.widget_class, &swcp)) {
		if (not sWidgetClassInfo.Lookup (w->core.widget_class->core_class.superclass, &swcp)) {
			AssertNotReached ();		// we lost our record???
		}
	}
	AssertNotNil (swcp.fRealExposeProc);

	MotifOSControl* xxx	=	Nil;
	if (sWidgets.Lookup (w, &xxx)) {
		AssertNotNil (xxx);
		if (r == Nil) {
			xxx->Refresh ();
		}
		else {
			xxx->Refresh (Region (r));
		}
	}
	else {
		GoReal ();
		(swcp.fRealExposeProc) (w, event, r);
		GoUnReal ();
	}
}

#endif	/*Toolkit*/







// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

