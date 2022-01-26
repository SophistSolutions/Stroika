/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/Window.cc,v 1.14 1992/09/08 15:34:00 lewis Exp $
 *
 * TODO:
 *
 *		(1)		Make WindowShell a panel, and return as enclosure, and also put all the
 *				windowplane stuff into WindowShell...
 *
 *		(2)		Not too sure we dealt with the visibilty issue properly - consider getting useing VisibiltyNotify event
 *
 *
 * Changes:
 *	$Log: Window.cc,v $
 *		Revision 1.14  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.13  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.12  1992/07/21  21:29:31  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.11  1992/07/21  20:12:43  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.10  1992/07/16  17:59:37  lewis
 *		Change SetBackground () code to not use old code to set backpixpat - use
 *		new tablet routine - but that commented out since may no longer be necessary.
 *
 *		Revision 1.9  1992/07/16  17:04:29  sterling
 *		fixed AdjustWindowShellHints to take shell size into account
 *
 *		Revision 1.8  1992/07/16  05:33:39  lewis
 *		Say if (GDIConfiguration ().ColorQDAvailable ()) instead of
 *		if (tile->IsOldQDPattern ()) - method went away.
 *
 *		Revision 1.7  1992/07/15  22:03:13  lewis
 *		Use 0 instead of kZeroSeconds.
 *
 *		Revision 1.6  1992/07/08  03:49:54  lewis
 *		Renamed PointInside->Contains, and replaced calls to SwitchFallThru with
 *		Require?Assert NotReached ().
 *
 *		Revision 1.5  1992/07/03  02:21:39  lewis
 *		Use Panel:: to scope access to nested UpdateMode enum.
 *
 *		Revision 1.4  1992/07/02  05:03:25  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.3  1992/06/30  22:56:43  lewis
 *		Disabled BackPixPat () call in macGDI part of Window::SetBackground () cuz I rediscovered what
 *		it breaks - native pushbuttons dont display properly when depressed - re-enable and debug some
 *		other time... Not too important right now...
 *
 *		Revision 1.2  1992/06/25  09:07:37  sterling
 *		Put main view at end of trickview view list (LGP comment - not sure why relevant?) and
 *		LGP turned on SetBackground code in Window - not really sure its safe.
 *
 *		Revision 1.79  92/04/20  14:23:50  14:23:50  lewis (Lewis Pringle)
 *		Added typedef char* caddr_t; in #if qSnake && _POSIX_SOURCE since xutils.h the distribute not posix complaint.
 *		
 *		Revision 1.77  92/04/08  15:54:42  15:54:42  lewis (Lewis Pringle)
 *		On X, set the tablet visible area to the extent of the trickview when the
 *		window resizes, and on updates (Window::Update()) temporarily set it to the
 *		update region so we get more flicker free drawing - clipping is automatically done to the updateRegion (idea from the mac).
 *		
 *		Revision 1.76  92/04/02  13:07:41  13:07:41  lewis (Lewis Pringle)
 *		Instead of manually re-#defining symbols undefs in Intrinsic.h, we just re-include osrenamepre/post after that
 *		file. This is better because as we add more renames, its painfull having to update all these other places. Hopefully
 *		the entire hack can go away at some point.
 *		
 *		Revision 1.75  92/04/02  11:43:53  11:43:53  lewis (Lewis Pringle)
 *		Got rid of gDisplay param to constructing Tablet - no longer needed.
 *		
 *		Revision 1.74  92/03/26  09:42:19  09:42:19  lewis (Lewis Pringle)
 *		Got rid of oldLive arg to EffectiveLiveChanged, and made GetVisibleArea const. Also got rid
 *		of oldVisible arg to EffeitveVisibiltyChanged.
 *		
 *		Revision 1.72  1992/03/16  20:42:37  lewis
 *		Put back XClearArea calls since they were really right in Window::Refresh()
 *		if we are not using private update regions (at least close to right).
 *		Use qXmToolkit - not qMotifToolkit in TrickView::Render override.
 *
 *		Revision 1.71  1992/03/16  17:29:21  lewis
 *		For X, in IdleTask where we look for fUpdateRegion non-empty, before we do the update, first check that
 *		there are not any more expose extents  in the X Q. This was very difficult, and bizare, and I doubt that
 *		I got it right. Seems OK, for now, but I have little confidence.
 *		Get rid of XClearArea calls in Window::Refresh () - I dont quite remember why they were there, but there
 *		are wrong for now - maybe it was part of an attempt to use the REAL X update mechanism???
 *
 *		Revision 1.70  1992/03/10  00:09:33  lewis
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *		Use new DispatchKeyEvent () interface instead of old HandleKey () interface.
 *
 *		Revision 1.69  1992/03/05  21:34:58  sterling
 *		made trickview be a groupview, so could freely add subviews
 *
 *		Revision 1.68  1992/02/24  06:50:14  lewis
 *		Be more careful in expose callback to only add to region the exposed amount, rather than
 *		the entire extent. That should have fixed our last flicker bug, but its still there!!!!
 *
 *		Revision 1.67  1992/02/21  00:52:23  lewis
 *		compile on mac.
 *
 *		Revision 1.66  1992/02/20  09:55:07  lewis
 *		Doing stuff in callback caused core dump in Emily when we closed window cuz aparently we
 *		got callback called after widget destroyed. Being more explicit about when it got
 *		destroyed in destructor for window seemed to solve the problem.
 *
 *		Revision 1.65  1992/02/20  09:34:06  lewis
 *		Add call to w->Refresh () in ExposeCallback- forgot last time. Must be more careful here
 *		and snag real region from arg to callback. Same with ProcessUpdate () in EventHandler-
 *		use Refresh rather than peeking at field.
 *
 *		Revision 1.64  1992/02/20  08:53:27  lewis
 *		Fixed bug in WindowEventHandler where it forget to pass along event (update) if it didnt
 *		handle it.
 *
 *		Revision 1.62  1992/02/20  06:12:22  lewis
 *		On window effectivevisiblitychange, we must add theh extent to the update region. Really should propbably
 *		do this on visibility notify, from X.
 *
 *		Revision 1.61  1992/02/19  22:28:19  lewis
 *		Got rid of using override of EventHandler to get client message for WM_DELETE_)WINDOW. Instead
 *		use XmAddProtocolHandleCallback (or whatever) to define the callback, and let Xt normally dispatch client
 *		messages.
 *		Deleted some debug messages, and no longer override ProcessUpdate for Xt - instead count on our idletas
 *		 and private update region implementation to handle updates.
 *		This got rid of a lot of flicker, and is more Xt freindly, though I have soem fears about it breaking
 *		things. Seemed to work fine the first time, but needs more testing.
 *
 *		Revision 1.60  1992/02/18  03:53:15  lewis
 *		Do XtDispatch for clientmessage so our callbacks get called. We must do that more often and use the Xt mechanism?
 *		I think??
 *
 *		Revision 1.59  1992/02/15  05:41:41  sterling
 *		merged ActiveItem in , used EffectiveLiveChanged
 *
 *		Revision 1.58  1992/02/12  07:19:40  lewis
 *		Use GDIConfig instead of OSConfig.
 *
 *		Revision 1.57  1992/02/11  01:24:50  lewis
 *		Deleted unused backwards compatability routines.
 *
 *		Revision 1.56  1992/02/06  21:30:17  lewis
 *		Had to comment back in some backward compat hacks til I get get rid of
 *		dependencies (trouble on mac).
 *
 *		Revision 1.55  1992/02/04  22:54:29  lewis
 *		Put in more assertnotreached calls in obsolete methods to clean things up. Next pass, delete them.
 *
 *		Revision 1.54  1992/02/02  05:50:37  lewis
 *		Did even bigger hack, onto of TrickView::Render hack to get it from erasing the menbubar in
 *		the mainviewwindow. Did this cuz they arenet views yet. That will change very soon, cuz Carl
 *		has made it a high-priority (being able to build pulldown menus anywhere, and install them in
 *		views).
 *		Also, hopefully I can now get back to fixing the graphix layer, making this ENTIRE hack unnessary.
 *
 *		Revision 1.53  1992/02/02  05:25:16  lewis
 *		Did hack in TrickView::Render so that erase would work. The next step is to start working on the
 *		graphix layer, and make it really work right (probably wont get to that for another week or so).
 *
 *		Revision 1.52  1992/01/31  22:50:43  lewis
 *		Deleted some backward compatility stuff, and added XtDispatchEvent
 *		to HandleConfigureNotify() method to fix bug where menus did not popup
 *		in the right position when you moved the main shell.
 *
 *		Revision 1.51  1992/01/31  17:04:58  sterling
 *		fixed memory leak by deleteing fTrickView
 *
 *		Revision 1.50  1992/01/31  05:34:31  lewis
 *		Got rid of clear-area calls in the invalidate code (for case with private regions). I think that
 *		was wrong, or just a quick hack I did to deal with backround problems. Also, tried small hack
 *		in TrickView::Render that didnt work too well. High on priority list now is dealing with window
 *		backgrounds.
 *
 *		Revision 1.49  1992/01/29  20:02:51  lewis
 *		Changed from bullitenboard to DrawingArea for mainviewwidget.
 *
 *		Revision 1.48  1992/01/29  16:46:16  lewis
 *		Realize bullitenbaord before managing, as it says on page 231, of new motif book (Orielly VI).
 *		Should be irrelevent for us, but lets not look for trouble. Also, delete addcallback since
 *		apparently not supported for this widget (exposecallback for bullienboard).
 *
 *		Revision 1.46  1992/01/29  05:23:09  sterling
 *		chaned WIndowHints
 *
 *		Revision 1.45  1992/01/27  23:38:23  lewis
 *		Added override of WindowEventHandler for ProcessInterapplicationCommunications for XToolkit so we get ICCM message
 *		saying window manager wants to close our top level shell window. Treat that as if user did a close menu command,
 *		and so if doc dirty, it will get saved, and can be cancelled out of.  X Docs, Volume 1, 10.2.3.2.2 WM_DELETE_WINDOW
 *		say that it is our option to ignore the message if the user changes his mind (cancels the save changes dialog),
 *		or to manually delete it ourselves.
 *
 *		Revision 1.44  1992/01/22  08:05:02  lewis
 *		Fixed privateUpdateRegion off, with X (slightly) so it works as well as with - which isn't very well.
 *		Must fix soon. Also, call fTablet->Sync () at end of Window::Update ().
 *
 *		Revision 1.43  1992/01/20  05:43:16  lewis
 *		Moved expose callback here from EventManager.
 *
 *		Revision 1.42  1992/01/19  21:42:27  lewis
 *		In call to set hints desired size from mainView calcdefaultsize, pass in current desired size
 *		as a default, in case the view doesn't set a size.
 *
 *		Revision 1.41  1992/01/18  09:17:47  lewis
 *		Added new ApplyWindowShellHints to Window:: to handle placement of windows on first set-visible.
 *
 *		Revision 1.40  1992/01/17  17:53:20  lewis
 *		Added set of marginHeight to 0, for mainViewBullitenBoard.
 *
 *		Revision 1.38  1992/01/14  23:30:36  lewis
 *		deleted set of origin to zero/zero for bulliten board main view widget, and added
 *		call to set marginWidth to zero. That margin width thing I think was the last
 *		trouble haunting me with placement of widgets - I hope. Eventaully, I will get rid
 *		of bulliten board and do my own custom widget here anyhow, since bulliten boards have
 *		much too much excess baggage.
 *
 *		Revision 1.37  1992/01/10  14:52:58  lewis
 *		A couple hacks, and a kludge or three to get update events limping along -- I need some time to get clipping
 *		working to really do this right.
 *
 *		Revision 1.35  1992/01/08  23:58:01  lewis
 *		Added portable window update mechanism - though still a little buggy on motif - trouble
 *		may be with regions...
 *
 *		Revision 1.34  1992/01/08  06:53:07  lewis
 *		Sterl - re-ordered window setting invisble and deactivation in WindowController.
 *
 *		Revision 1.33  1992/01/08  05:46:43  lewis
 *		Add WindowEventHandler to process Update events.
 *
 *		Revision 1.31  1992/01/07  00:52:30  lewis
 *		Set mainview extent using GetLocalContentRegion () rather than GlobalToLocal (GetContentRegion ()) since
 *		the former makes two calls on Shell::GetOrigin () which CAN - AND SOMETIMES DO - return different values
 *		under X.
 *
 *		Revision 1.30  1992/01/06  10:09:12  lewis
 *		Got rid of alot of crap no longer used. Fixed bug where TrickView didnt
 *		return mainViewWidget. Used XtResizeWidget on fMainView - only way
 *		I could managet to get it to resize - XtSetValues seems to have been
 *		ignored  - very curious. Also, played with the resources passed to this
 *		mainWindow widget. Changed the visibilty stuff and supported RealyVisible().
 *		Had to move processlayout checker elsewhere and always run it since we
 *		dont get a hook when were made visible (maybe we should).
 *
 *		Revision 1.28  1992/01/04  07:29:52  lewis
 *		Got window to not subclass from View, and got backwards compat working
 *		to a great degree. Still quite alot of work todo here in cleanups.
 *		Also, haven't completely thought how how to handle PlaceNewWindow business.
 *		Also, must maintain backwards compat til sterl gets new dialogeditor working
 *		and we bootstrap all the dialogs.
 *
 *		Revision 1.27  1991/12/27  16:59:16  lewis
 *		worked on new window architecture with shells as seperate objects, and got working
 *		(mostly) on both mac and unix. Still needs a fair amount of work to be "right".
 *
 *		Revision 1.25  1991/12/18  09:06:39  lewis
 *		Started adding support for new window architecture and Window Shells.
 *
 *
 */


#include	"OSRenamePre.hh"
#if		qMacToolkit
#include	<Memory.h>
#include	<QuickDraw.h>
#include	<OSUtils.h>				// for trap caching
#include	<SysEqu.h>
#include	<Resources.h>			// to snag default mdef
#include	<ToolUtils.h>
#include	<Traps.h>				// for trap caching
#include	<Windows.h>
#elif	qXmToolkit
#if		qSnake && _POSIX_SOURCE
typedef	char*	caddr_t;		// work around hp POSIX headers bug
#endif
#include	<X11/Xlib.h>
// not really right - really motif toolkit?
#include	<X11/Intrinsic.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/IntrinsicP.h>
#include	"OSRenamePost.hh"		// they undef a bunch of things we define
#include	"OSRenamePre.hh"		// so blast back!!!
#include	<X11/StringDefs.h>
#include	<Xm/Xm.h>
#include	<Xm/DrawingA.h>
#include	<Xm/Protocols.h>
#endif	/*Toolkit*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"
#include	"Memory.hh"
#include	"StreamUtils.hh"

#include	"GDIConfiguration.hh"
#include	"Region.hh"
#include	"Shape.hh"
#include	"Sound.hh"

#include	"Application.hh"
#include	"DeskTop.hh"
#include	"GroupView.hh"
#include	"MenuOwner.hh"
#include	"Shell.hh"
#include	"WindowPlane.hh"

#include	"Window.hh"


#if		!qRealTemplatesAvailable
	Implement (Iterator, WindowPtr);
	Implement (Collection, WindowPtr);
	Implement (AbSequence, WindowPtr);
	Implement (Array, WindowPtr);
	Implement (Sequence_Array, WindowPtr);
	Implement (Sequence, WindowPtr);
#endif

const	Boolean	Window::kActive = True;

#if		qMacGDI
#if		qCacheTraps

pascal	void	(*kCopyRgnPtr) (osRegion**, osRegion**) =
			(pascal void (*) (osRegion**, osRegion**))::NGetTrapAddress (_CopyRgn, ToolTrap);
pascal	Boolean	(*kEmptyRgnPtr) (osRegion**) =
			(pascal Boolean (*) (osRegion**))::NGetTrapAddress (_EmptyRgn, ToolTrap);
pascal	void	(*kInvalRgnPtr) (osRegion**) =
			(pascal void (*) (osRegion**))::NGetTrapAddress (_InvalRgn, ToolTrap);
pascal	void	(*kBeginUpdatePtr) (osGrafPort*) =
			(pascal void (*) (osGrafPort*))::NGetTrapAddress (_BeginUpDate, ToolTrap);
pascal	void	(*kEndUpdatePtr) (osGrafPort*) =
			(pascal void (*) (osGrafPort*))::NGetTrapAddress (_EndUpDate, ToolTrap);

#endif	/*qCacheTraps*/


static	void	DoSetOrigin (short newH, short newV)
	{
		RequireNotNil (qd.thePort);
		if ((qd.thePort->portRect.top != newV) or (qd.thePort->portRect.left != newH)) {
#if		qCacheTraps
			static	pascal	void	(*kSetOriginPtr) (short,short) =
						(pascal void (*) (short,short))::NGetTrapAddress (_SetOrigin, ToolTrap);
			(*kSetOriginPtr) (newH, newV);
#else
			::SetOrigin (newH, newV);
#endif
		}
	}

static	void	DoSectRgn (const osRegion** r1, const osRegion** r2, osRegion** r3)
	{
#if		qCacheTraps
		static	pascal	void	(*kSectRgnPtr) (osRegion**, osRegion**, osRegion**) =
					(pascal void (*) (osRegion**, osRegion**, osRegion**))::NGetTrapAddress (_SectRgn, ToolTrap);
		(*kSectRgnPtr) ((osRegion**)r1, (osRegion**)r2, r3);
#else
		::SectRgn ((osRegion**)r1, (osRegion**)r2, r3);
#endif
	}

inline	void	DoCopyRgn (const osRegion** r1, osRegion** r2)
	{
#if		qCacheTraps
		(*kCopyRgnPtr) ((osRegion**)r1, r2);
#else
		::CopyRgn ((osRegion**)r1, r2);
#endif
	}

inline	Boolean	DoEmptyRgn (const osRegion** r)
	{
#if		qCacheTraps
		return Boolean ((*kEmptyRgnPtr) ((osRegion**)r));
#else
		return Boolean (::EmptyRgn ((osRegion**)r));
#endif
	}

inline	void	DoBeginUpdate (osGrafPort* osgp)
	{
		AssertNotNil (osgp);
#if		qCacheTraps
		(*kBeginUpdatePtr) (osgp);
#else
		::BeginUpdate (osgp);
#endif
	}

inline	void	DoEndUpdate (osGrafPort* osgp)
	{
		AssertNotNil (osgp);
#if		qCacheTraps
		(*kEndUpdatePtr) (osgp);
#else
		::EndUpdate (osgp);
#endif
	}
#endif	/*qMacGDI*/





#if		qXGDI
extern	osDisplay*      gDisplay;
#endif	/*qXGDI*/







// these guys only running for visible windows, and they run very fast, so should not be much
// of a drain...
class	ProcessLayoutChecker : public PeriodicTask {
	public:
		ProcessLayoutChecker (Window& aWindow):
			fWindow (aWindow)
			{
				SetPeriodicity (0);			// soonest available idle time...
			}
		override	void	RunABit ()
			{
				if (fWindow.GetVisible ()) {		// don't bother if invisible - may want to allow this anyhow???
					fWindow.ProcessLayout ();
#if		qUsePrivateUpdateRegions
#if		qXtToolkit
					/*
					 * If more X expose events pending, then we may as well let xt process them so we dont
					 * get flicker here...
					 */
					XEvent e;
					if (::XCheckTypedEvent (gDisplay, Expose, &e)) {
						::XPutBackEvent (gDisplay, &e);
						return;
					}
#endif
					if (not fWindow.fUpdateRegion.Empty ()) {
						fWindow.Update (fWindow.fUpdateRegion);
					}
#endif	/*qUsePrivateUpdateRegions*/
				}
			}
	private:
		Window&	fWindow;
};




#if 	1 && qMotifUI
// Stolen from X11R4/mit/lib/X/region.h Must be some better way to do this, but what???
typedef struct {
    short x1, x2, y1, y2;
} Box, BOX, BoxRec, *BoxPtr;
typedef struct {
    short x, y, width, height;
}RECTANGLE, RectangleRec, *RectanglePtr;

typedef struct osRegion /*_XRegion*/ {
    long size;
    long numRects;
    BOX *rects;
    BOX extents;
} REGION;
#endif

class	TrickView : public GroupView {
	public:
		TrickView (Window& w);

#if 	qXmToolkit
virtual		void	Render (const Region& updateRegion)
{
	if (not updateRegion.Empty ()) {
		osDisplay*	theDisplay	=	XtDisplay (fWindow.GetOSRepresentation ());

		Arg		args[1];
		unsigned long bg = 0;
		XtSetArg (args[0], XtNbackground, &bg);
		XtGetValues (fWindow.GetOSRepresentation (), args, 1);

		Rect	bounds 	= updateRegion.GetBounds ();
		_XGC*			theGC;
		XGCValues		gcv;

		gcv.fill_style = FillSolid;
		gcv.foreground = bg;
		gcv.subwindow_mode = IncludeInferiors;
		if (! (theGC = ::XCreateGC (theDisplay, XtWindow (fWindow.GetOSRepresentation ()),
								  GCSubwindowMode | GCFillStyle | GCForeground, &gcv))) {
			AssertNotReached (); // must raise exception!!
		}

		{
			// code stolen and warmed over from X11R4 region implemention of ::XSetRegion()
			register	osRegion*	theRegion	=	updateRegion.PeekAtOSRegion ();
			register int i;
			register osRect *xr, *pr;
			register BOX *pb;
			xr = new osRect [theRegion->numRects];
			for (pr = xr, pb = theRegion->rects, i = theRegion->numRects; --i >= 0; pr++, pb++) {
				pr->x = pb->x1;
				pr->y = pb->y1;
				pr->width = pb->x2 - pb->x1;
				pr->height = pb->y2 - pb->y1;
			}
			::XFillRectangles (theDisplay, XtWindow (fWindow.GetOSRepresentation ()), theGC, xr, theRegion->numRects);
			delete (xr);
		}
		::XFreeGC (theDisplay, theGC);
	}
	View::Render (updateRegion);
}
#endif
		override	void	Layout ();
		override	Panel*	GetParentPanel () const;
		override	Point	LocalToTablet_ (const Point& p)	const;
		override	Point	TabletToLocal_ (const Point& p)	const;
		override	void	Refresh_ (const Region& updateRegion, UpdateMode updateMode);
		override	void	Update_ (const Region& updateRegion);
		override	Boolean	GetEffectiveLive () const;
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);	// overridden to provide public scope
#if		qXGDI
		override	osWidget*	GetWidget () const;
#endif
	private:
		Window&	fWindow;
};






class	WindowEventHandler : public EventHandler {
	public:
		WindowEventHandler (Window& w);
		~WindowEventHandler ();

		override	void	ProcessUpdate (const osEventRecord& eventRecord, Window& window, const Region& updateRegion);

	private:
//		typedef	EventHandler	inherited;
		Window&	fWindow;
};







/*
 ********************************************************************************
 ******************************* WindowController *******************************
 ********************************************************************************
 */

WindowController::WindowController ()
{
}

WindowController::~WindowController ()
{
	/*
	 * Wouldn't be safe to delete the windows here, since we didn't create them, and don't
	 * know that they were created on the heap.  It is upto the creator of the windows to remove
	 * them before destroying the window controller.
	 */
	Require (fWindows.IsEmpty ());
}

void	WindowController::HandleClose (Window& ofWindow)
{
	Assert (fWindows.Contains (&ofWindow));

	/*
	 * Deactivate here since may be deleted after this, before we get next event, and process
	 * next event (ie activate event).
	 */
	ofWindow.SetActive (not Window::kActive, View::eNoUpdate);
	ofWindow.SetVisible (False);
}

SequenceIterator(WindowPtr)*	WindowController::MakeWindowIterator (SequenceDirection d)	const 
{
	return (fWindows.MakeSequenceIterator (d));
}










/*
 ********************************************************************************
 ************************************* Window ***********************************
 ********************************************************************************
 */


Sequence(WindowPtr)	Window::sWindows;

Window::Window (WindowShell* windowShell):
	fTablet (Nil),
	fMainView (Nil),
	fMenuTarget (Nil),
	fKeyTarget (Nil),
	fWindowController (Nil),
	fProcessFirstClick (False),
	fProcessLayoutChecker (Nil),
	fShell (Nil),
	fTrickView (Nil),
	fWindowEventHandler (Nil),
#if		qUsePrivateUpdateRegions
	fUpdateRegion (kEmptyRegion),
#endif
	fActive (False)
{
	if (windowShell != Nil) {
		SetShell (windowShell);
	}
}

Window::Window ():
	fTablet (Nil),
	fMainView (Nil),
	fMenuTarget (Nil),
	fKeyTarget (Nil),
	fWindowController (Nil),
	fProcessFirstClick (False),
	fProcessLayoutChecker (Nil),
	fShell (Nil),
	fTrickView (Nil),
	fWindowEventHandler (Nil),
#if		qUsePrivateUpdateRegions
	fUpdateRegion (kEmptyRegion),
#endif
	fActive (False)
{
	SetShell (new StandardDocumentWindowShell ());
}

Window::~Window ()
{
	if (fProcessLayoutChecker != Nil) {
		Application::Get ().RemoveIdleTask (fProcessLayoutChecker);
		delete (fProcessLayoutChecker);
		fProcessLayoutChecker = Nil;
	}
	sWindows.Remove (this);
	Assert (not GetActive ());
	Assert (not GetVisible ());		// close windows before deleting them...
	GetPlane ().RemoveWindow (this);
	SetWindowController (Nil);


	delete (fTrickView);
	delete (fWindowEventHandler);
	delete (fTablet);

// Must come after killing trickview, and before shell so deleted after subwingets, and before parent...
#if		qXGDI
	if (fMainViewWidget != Nil) {
		::XtDestroyWidget (fMainViewWidget);
		fMainViewWidget = Nil;
	}
#endif	/*qXGDI*/
	delete (fShell);
}

WindowController*	Window::GetWindowController () const
{
	return (fWindowController);
}

void	Window::SetWindowController (WindowController* windowController)
{
	if (fWindowController != Nil) {
		fWindowController->fWindows.Remove (this);
	}
	fWindowController = windowController;
	if (fWindowController != Nil) {
		fWindowController->fWindows.Append (this);
	}
}

void	Window::SetMainView (View* v)
{
	if (fMainView != Nil) {
		fTrickView->RemoveSubView (fMainView);
	}
	fMainView = v;
	if (v != Nil) {
		Require (fTrickView->GetSubViewCount () == 0);
		fTrickView->AddSubView (v);
	}
}

void	Window::SetMenuTarget (MenuCommandHandler* menuTarget)
{
	fMenuTarget = menuTarget;
}

void	Window::SetKeyTarget (KeyHandler* keyTarget)
{
	fKeyTarget = keyTarget;
}

void	Window::SetMainViewAndTargets (View* v, MenuCommandHandler* menuTarget, KeyHandler* keyTarget)
{
	SetMainView (v);
	SetMenuTarget (menuTarget);
	SetKeyTarget (keyTarget);
}

Region	Window::GetVisibleArea () const
{
#if 1
	return (fTrickView->GetVisibleArea ());		// maybe should be fshell->???
#else
#if 0
(void) View::GetVisibleArea ();	// just get something into fVisibleArea or optimizations will break
		DoSetOrigin (0, 0);						// in case someone else has munnged it
		return (GetOSRepresentation ()->visRgn);		// its in local coordinates
#else
		return (*fVisibleArea);
#endif

#endif
}

void	Window::DoSetupMenus ()
{
	/*
	 * Handler more specific, so call it last (so it can override windowcontroller changes).
	 */
	if (GetWindowController () != Nil) {
		GetWindowController ()->DoSetupMenus ();
	}

	EnableCommand (eClose, True);

	if (GetMenuTarget () != Nil) {
		GetMenuTarget ()->DoSetupMenus ();
	}
}


//	q_MPW_CFRONT_21_BUG_CRASHES_IN_REALLY_REALLY_PRINT
			class	CloseCommand : public Command {
				public:
					CloseCommand (Window& w) : Command (CommandHandler::eClose), fWindow (w) {}
					override	void	DoIt ()
						{
							fWindow.Close ();
						}
					override	Boolean	CommitsLastCommand () const
						{
							// Also, we almost certainly make former
							// undoable commands unsafe, so we flush out the undo buffer
							return (True);
						}
				private:
					Window&	fWindow;
			};
			

Boolean	Window::DoCommand (const CommandSelection& selection)
{
	/*
	 * Handler more specific, so give it first crack at commands.
	 */
	if (GetMenuTarget () != Nil) {
		if (GetMenuTarget ()->DoCommand (selection)) {
			return (True);
		}
	}

	switch (selection.GetCommandNumber ()) {
		case	eClose: {
			/*
			 * we will sometime be destroyed by a close, so we build a command to do the dirty work
			 * after we have relinquished control of execution.
			 */
			Application::Get ().PostCommand (new CloseCommand (*this));
			return (True);
		}
		break;
		default: {
			return (Boolean ((GetWindowController () == Nil)? False: GetWindowController ()->DoCommand (selection)));
		}
		break;
	}
	AssertNotReached (); return (False);
}

Boolean	Window::DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState, KeyComposeState& composeState)
{
	/*
	 * Handler more specific, so give it first crack at commands.
	 */
	if (GetKeyTarget () != Nil) {
		if (GetKeyTarget ()->DispatchKeyEvent (code, isUp, keyBoardState, composeState)) {
			return (True);
		}
	}

	return (Boolean ((GetWindowController () == Nil)?
				     False:
					 GetWindowController ()->DispatchKeyEvent (code, isUp, keyBoardState, composeState)));
}

void	Window::Select ()
{
	GetPlane ().ReorderWindow (this);
	SetVisible (True);
}

void	Window::Close ()
{
	/*
	 * This could fail.  It could also, cause this to be disposed, so lookout...
	 */
	if (fWindowController == Nil) {

// LGP - not sure we should allow this - perhaps do assert - or ignore - instead - force people to do controlers or dont enable close???
		SetVisible (not View::kVisible);
		/*
		 * Deactivate here since may be deleted after this, before we get next event, and process
		 * next event (ie activate event).
		 */
		SetActive (not kActive, View::eNoUpdate);
	}
	else {
		fWindowController->HandleClose (*this);
	}
}

#if		qMacGDI
Window*	Window::OSLookup (osGrafPort* osWin)
{
#if 	qDebug
	Window*	theWindow = Nil;
	ForEach (WindowPtr, it, sWindows) {
		if (it.Current ()->GetOSRepresentation () == osWin) {
			theWindow = it.Current ();
			break;
		}
	}
	AssertNotNil (theWindow);
	Assert (theWindow->GetOSRepresentation () == osWin);
	Assert (((Window*) ((osWindowPeek) osWin)->refCon) == theWindow);
#endif	/*qDebug*/

	return ((Window*) ((osWindowPeek) osWin)->refCon);
}
#elif	qXGDI
Window*		Window::OSLookup (osWidget* osWin)
{
	ForEach (WindowPtr, it, sWindows) {
		if (it.Current ()->fMainViewWidget == osWin or it.Current ()->GetOSRepresentation () == osWin) {
			return (it.Current ());
		}
	}
	return (Nil);
}

Window*		Window::OSLookup (unsigned long osWin)
{
	ForEach (WindowPtr, it, sWindows) {
		// treat either the shell, or the content window as our window, for now...
		Window*	cur = it.Current ();
		if (XtWindow (cur->fMainViewWidget) == osWin or XtWindow (cur->GetOSRepresentation ()) == osWin) {
			return (it.Current ());
		}
	}
	return (Nil);
}
#endif	/*GDI*/

#if		qXGDI
void	Window::HandleConfigureNotify (const osEventRecord& eventRecord)
{
	AssertNotNil (fTrickView);
	fTrickView->SetExtent (fShell->GetExtent ());

	/*
	 * Some subwidgets need to be notified of change - they need to be replaced/resized?
	 */
	osEventRecord	er	=	eventRecord;
	::XtDispatchEvent (&er);
}
#endif	/*qXGDI*/

void	Window::SetContentSize (const Point& newSize)
{
	AssertNotNil (fShell);
	fShell->SetContentSize (newSize);
	AssertNotNil (fTrickView);
	fTrickView->SetExtent (fShell->GetExtent ());
}

void	Window::AdjustWindowShellHints (WindowShellHints& hints)
{
	if (not hints.DesiredSizeSet ()) {
		AssertNotNil (fMainView);
		Point	hintSize = fMainView->CalcDefaultSize (hints.GetDesiredSize ());
		if (fShell != Nil) {
			hintSize += fShell->GetLocalContentOrigin ();
		}
		hints.SetDesiredSize (hintSize, False);
	}
}

void	Window::Render (const Region& updateRegion)
{
	AssertNotNil (fTrickView);
	fTrickView->Render (updateRegion);
#if		qMacToolkit
	AssertNotNil (fShell);
	if (fShell->GetResizeable ()) {
		Region	clip	=	fTablet->GetClip ();
		fTablet->SetClip (GetSizeBox ());
		::DrawGrowIcon (GetOSRepresentation ());
		fTablet->SetClip (clip);
	}
#endif	/*qMacToolkit*/
}

#if		qMacToolkit
Rect	Window::GetSizeBox () const
{
	static	const	Point	kSizeBoxSize	=	Point (15, 15);
	return (Rect (fShell->GetContentSize () - kSizeBoxSize, kSizeBoxSize));
}
#endif	/*qMacToolkit*/

Boolean	Window::TrackPress (const MousePressInfo& mouseInfo)
{
	AssertNotNil (fShell);
#if		qMacGDI
	osPoint	thePoint;
	osWindowPtr	whichWin;
	short	windowPart = ::FindWindow (os_cvt (LocalToGlobal (mouseInfo.fPressAt), thePoint), &whichWin);
	switch (windowPart) {
		case	inContent:
			goto doInContent;

		case	inDrag:
			fShell->DragEvent (mouseInfo.fPressAt, mouseInfo.fKeyBoard);
			break;	

		case	inGrow:
			fShell->GrowEvent (mouseInfo.fPressAt);
			break;

		case	inGoAway:
			fShell->GoAwayEvent (mouseInfo.fPressAt);
			break;

		case	inZoomIn:
		case	inZoomOut:
			fShell->ZoomEvent (mouseInfo.fPressAt, Boolean (windowPart == inZoomIn));
			break;

		default:	AssertNotReached ();
	}
	return (True);

doInContent:
#endif	/*qMacGDI*/

	if (GetPlane ().GetFrontWindow () != this) {
		if (DeskTop::Get ().GetModal ()) {
			Beep ();
			return (True);
		}
		
		GetPlane ().ReorderWindow (this);
		Assert (GetPlane ().GetFrontWindow () == this);
		if (not GetProcessFirstClick ()) {
			return (True);
		}
	}
	return (fTrickView->TrackPress (mouseInfo));
}

WindowPlane&	Window::GetPlane ()	const
{
	AssertNotNil (fShell);
	EnsureNotNil (&fShell->GetWindowPlane ());
	return (fShell->GetWindowPlane ());
}

void	Window::SetProcessFirstClick (Boolean processFirstClick)
{
	fProcessFirstClick = processFirstClick;
}

String	Window::GetTitle ()	const
{
	AssertNotNil (fShell);
	return (fShell->GetTitle ());
}

void	Window::SetTitle (const String& title/*, UpdateMode update*/)
{
	AssertNotNil (fShell);
	fShell->SetTitle (title);
}

void	Window::SetBackground (const Tile* tile)
{
	/*
	 * Also, set the backpat, since when wmgr draws window it will use this (like when
	 * window gets resized).
	 */
	if (tile == Nil) {
		tile = &kWhiteTile;		// force some background tile for windows
	}
	fTrickView->SetBackground (tile);

	// not sure there is any point in this...
	// maybe??? we always reset soon after - maybe prevent first display ??? before we do any draws, when
	// window DEF first draws window???
#if 0
	AssertNotNil (GetTablet ());
	GetTablet ()->SetOSBackground (*tile);
#endif
}

#if		qMacToolkit
struct osGrafPort*	Window::GetOSRepresentation () const
{
	AssertNotNil (fTablet);
	EnsureNotNil (fTablet->GetOSGrafPtr ());
	return (fTablet->GetOSGrafPtr ());
}
#elif	qXtToolkit
osWidget*	Window::GetOSRepresentation () const
{
	EnsureNotNil (fShell->GetOSRepresentation ());
	return (fShell->GetOSRepresentation ());
}
#endif	/*Toolkit*/

Boolean	Window::GetActive () const					
{		
	return (GetActive_ ());			
}

void	Window::SetActive (Boolean active, Panel::UpdateMode update)
{
	if (active != GetActive ()) {
		SetActive_ (active, update);
	}
	Ensure (GetActive () == active);
}

void	Window::SetActive_ (Boolean active, Panel::UpdateMode updateMode)
{
	Boolean	oldLive = GetEffectiveLive ();	
	fActive = active;
	if (GetEffectiveLive () != oldLive) {
		EffectiveLiveChanged (not oldLive, updateMode);
	}
}

Boolean	Window::GetActive_ () const
{
	return (fActive);
}

Boolean	Window::GetLive () const
{
	return (fActive);
}

void	Window::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode)
{
	MenuOwner::SetMenusOutOfDate ();
	AssertNotNil (fTrickView);
	fTrickView->EffectiveLiveChanged (newLive, updateMode);
}

Region	Window::GetContentRegion () const
{
	AssertNotNil (fShell);
	return (GlobalToLocal (fShell->GetContentRegion ()));
}

#if		qXGDI
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
#if		qUsePrivateUpdateRegions
	/*
	 * Really we should find type of calldata and get rect from it.
	 */
	Window* w = (Window*)clientData;
	w->Refresh (addToRegion, View::eDelayedUpdate);
#endif
}
#endif	/*qXGDI*/

#if		qXGDI
static	void	WMDeleteWindowCallback (osWidget* w, char* client_data, char* cbs)
{
	Try {
		AssertNotNil (client_data);
		Application::Get ().PostCommand (new CloseCommand (*(Window*)client_data));
	}
	Catch () {
		// ignore exception... Thats OK
	}
}
#endif	/*qXGDI*/

void	Window::SetShell (WindowShell* windowShell)
{
	RequireNotNil (windowShell);

	fTrickView = new TrickView (*this);

	fShell = windowShell;
	fShell->BeInWindow (this);

#if		qMacGDI
	::SetWRefCon (fShell->GetOSRepresentation (), long (this));
	fTablet = new Tablet (fShell->GetOSRepresentation ());
#elif	qXGDI
	Arg args[3];
	XtSetArg (args[0], XmNmarginWidth, 0);
	XtSetArg (args[1], XmNmarginHeight, 0);
	XtSetArg (args[2], XmNshadowThickness, 0);

	fMainViewWidget = ::XmCreateDrawingArea (GetOSRepresentation (), "MainViewWidget", args, 3);
	::XtRealizeWidget (fMainViewWidget);
	::XtManageChild (fMainViewWidget);
	AssertNotNil (XtWindow (fMainViewWidget));

	::XtAddCallback (fMainViewWidget, XmNexposeCallback, (XtCallbackProc)ExposeCallBack, (XtPointer)this);

	fTablet = new Tablet (XtWindow (fMainViewWidget));
#endif	/*qXGDI*/

	fTrickView->SetTablet (fTablet);
#if		qMacUI
// not really right under X, and currently buggy - so lets not try...
// In fact, it may not even really be right on the mac to always make it white. It might be better at this
// point to say setbackground of (peek at windows current background) for the mac - at least...
// probably cannot get away with just doing nothing on mac cuz our View's notion of the background must agree with
// the window notion occationally, like to do erases in subviews...
	SetBackground (&kWhiteTile);
#endif
	GetPlane ().AddWindow (this);
	SetVisible (fShell->GetVisible ());
	sWindows.Append (this);

	fWindowEventHandler = new WindowEventHandler (*this);

	fProcessLayoutChecker = new ProcessLayoutChecker (*this);
	Application::Get ().AddIdleTask (fProcessLayoutChecker);

	if (GetEffectiveLive ()) {
		fTrickView->EffectiveLiveChanged (True, View::eNoUpdate);
	}

#if		qXmToolkit
	// See Orielly VI-592-595
	Atom	WM_DELETE_WINDOW	= ::XInternAtom (XtDisplay (GetOSRepresentation ()), "WM_DELETE_WINDOW", FALSE);
	::XmAddWMProtocolCallback (GetOSRepresentation (), WM_DELETE_WINDOW, WMDeleteWindowCallback, (XtPointer)this);
#endif
}

Point	Window::LocalToGlobal (const Point& p) const
{
	return (p + GetOrigin ());
}

Point	Window::GlobalToLocal (const Point& p) const
{
	return (p - GetOrigin ());
}

Rect	Window::LocalToGlobal (const Rect& r) const
{
	return (Rect (LocalToGlobal (r.GetOrigin ()), r.GetSize ()));
}

Rect	Window::GlobalToLocal (const Rect& r) const
{
	return (Rect (GlobalToLocal (r.GetOrigin ()), r.GetSize ()));
}

Region	Window::LocalToGlobal (const Region& r) const
{
	Point	origin = r.GetBounds ().GetOrigin ();
	return (r + (LocalToGlobal (origin) - origin));
}

Region	Window::GlobalToLocal (const Region& r) const
{
	Point	origin = r.GetBounds ().GetOrigin ();
	return (r + (GlobalToLocal (origin) - origin));
}

Point	Window::LocalToTablet (const Point& p) const
{
//AssertNotReached ();
#if		qMacToolkit
	AssertNotNil (fShell);
	return (fShell->LocalToTablet (p));
#else
	return (p);
#endif
}

Point	Window::TabletToLocal (const Point& p) const
{
//AssertNotReached ();
#if		qMacToolkit
	AssertNotNil (fShell);
	return (fShell->TabletToLocal (p));
#else
	return (p);
#endif
}

Region	Window::LocalToTablet (const Region& r) const
{
//AssertNotReached ();
	Point	origin = r.GetBounds ().GetOrigin ();
	return (r + (LocalToTablet (origin) - origin));
}

Region	Window::TabletToLocal (const Region& r) const
{
//AssertNotReached ();
	Point	origin = r.GetBounds ().GetOrigin ();
	return (r + (TabletToLocal (origin) - origin));
}

Rect	Window::GetExtent () const
{
// ALLOW FOR NOW...AssertNotReached ();
	//	Assert (Rect (GetOrigin (), GetSize ()) == fShell->GetExtent ());	// not quite gaurenteed since under X this could change asyncronously - but not likely...
																			// actually under X quite likely...
	return (fShell->GetExtent ());
}

Rect	Window::GetLocalExtent () const
{
	return (Rect (kZeroPoint, GetSize ()));
}

Boolean	Window::Contains (const Point& p) const
{
	return (Rectangle ().Contains (p, GetExtent ()));
}

Boolean	Window::ProcessLayout (Boolean layoutInvisible)
{
	AssertNotNil (fTrickView);
	return (fTrickView->ProcessLayout (layoutInvisible));
}

Boolean	Window::GetVisible () const
{
	AssertNotNil (fShell);
	return (fShell->GetVisible ());
}

Boolean	Window::GetReallyVisible () const
{
	AssertNotNil (fShell);
	return (fShell->GetReallyVisible ());
}

void	Window::SetVisible (Boolean visible)
{
	AssertNotNil (fShell);
	if (visible != GetVisible ()) {
		fShell->SetVisible (visible);
		fTrickView->SetExtent (fShell->GetExtent ());
		EffectiveVisibilityChanged (visible, View::eNoUpdate);
	}
}

void	Window::EffectiveVisibilityChanged (Boolean newVisible, Panel::UpdateMode updateMode)
{
	AssertNotNil (fTrickView);
	fTrickView->SetVisible (newVisible, updateMode);		// update trick views visibilty too so GetEffectiveVisibilty retruns right answer...
#if		qUsePrivateUpdateRegions
	if (newVisible) {
		fUpdateRegion += GetLocalExtent ();
	}
#endif
}

Point	Window::GetOrigin () const
{
//AssertNotReached ();
	AssertNotNil (fShell);
	return (fShell->GetOrigin ());
}

Point	Window::GetSize () const
{
//AssertNotReached ();
	AssertNotNil (fShell);
	return (fShell->GetSize ());
}

void	Window::SetOrigin (const Point& newOrigin)
{
//AssertNotReached ();
	AssertNotNil (fShell);
	fShell->SetOrigin (newOrigin);
	AssertNotNil (fTrickView);
	fTrickView->SetExtent (GetExtent ());
}

void	Window::Refresh (const Region& r, Panel::UpdateMode update)
{
	switch (update) {
		case View::eNoUpdate: {
		}
		break;

		case View::eDelayedUpdate:	{
#if		qUsePrivateUpdateRegions
			fUpdateRegion += r;
#elif	qXToolkit
			/*
			 * Not exactly the right thing to do in this case, but at least it will generate an Expose
			 * event, and the erase should be harmless. Better to use private update regions with X.
			 */
			Rect rr = r.GetBounds ();
			::XClearArea (gDisplay, XtWindow (fMainViewWidget), (int)rr.GetLeft (), (int)rr.GetTop (),
						  (unsigned int)rr.GetWidth (), (unsigned int)rr.GetHeight (), True);
#elif	qMacToolkit
			Region	invalRegion	=	LocalToTablet (r);
			RequireNotNil (GetOSRepresentation ());
			Tablet::xDoSetPort (GetOSRepresentation ());
			DoSetOrigin (0, 0);				// in case someone else has munnged it
#if		qCacheTraps
			(*kInvalRgnPtr) ((osRegion**)invalRegion.PeekAtOSRegion ());
#else
			::InvalRgn ((osRegion**)invalRegion.PeekAtOSRegion ());
#endif
#endif	/*Toolkit*/
		}
		break;
	
		case View::eImmediateUpdate:	{
#if		qUsePrivateUpdateRegions
			fUpdateRegion += r;
#elif	qXToolkit
			/*
			 * Not exactly the right thing to do in this case, but at least it will generate an Expose
			 * event, and the erase should be harmless. Better to use private update regions with X.
			 */
			Rect rr = r.GetBounds ();
			::XClearArea (gDisplay, XtWindow (fMainViewWidget), (int)rr.GetLeft (), (int)rr.GetTop (),
						  (unsigned int)rr.GetWidth (), (unsigned int)rr.GetHeight (), True);
#elif	qMacToolkit
			Region	invalRegion	=	LocalToTablet (r);
			RequireNotNil (GetOSRepresentation ());
			Tablet::xDoSetPort (GetOSRepresentation ());
			DoSetOrigin (0, 0);				// in case someone else has munnged it
#if		qCacheTraps
			(*kInvalRgnPtr) ((osRegion**)invalRegion.PeekAtOSRegion ());
#else
			::InvalRgn ((osRegion**)invalRegion.PeekAtOSRegion ());
#endif
#endif	/*PRIVATE/Toolkit*/
			Update (r);
		}
		break;
		
		default:	RequireNotReached ();
	}
}

void	Window::Update (const Region& updateRegion)
{
#if		qDebug
	const	UInt32	kMaxDepth	=	5;				// Really we assure no such thing, but could be a bad sign if we go too deep...
	static	UInt32	updateLevel =	0;				// prevent deep recursive calls
#endif	/*qDebug*/
	Assert (++updateLevel <= kMaxDepth);			// allow only two levels of recursion...

	if (ProcessLayout ()) {
		Assert (updateLevel <= kMaxDepth-1);
		Update ();
		Ensure (--updateLevel <= kMaxDepth-2);
	}
	else {
#if		qUsePrivateUpdateRegions
		/* This case is not quite right if using mactoolkit AND privateUpdateRegions - though I cannot see why you would...*/

		Region	oldUpdateRegion	=	fUpdateRegion;

		fUpdateRegion *= updateRegion;		// temporarily set the update region to be just the subpart we are updating...

		if (not fUpdateRegion.Empty ()) {
			/*
			 * To be like the mac, we should somehow set the visible region of the tablet to be just the update region.
			 * the only way I know of doing this would be to temporarily create a bunch of windows to hide things - that
			 * would seem to be overkill. (we can handle our own drawing with clipping - trouble is motif widgets - the have their
			 * own gc's. Any other ideas???
			 */
// Set the visiblearea for the tablet to the updateregion temporarily so that we only update that which
// we wanted to
  			Tablet*	t	=	fTrickView->GetTablet ();
  			AssertNotNil (t);
			t->SetVisibleArea (fUpdateRegion);
			Render (fUpdateRegion);
			t->SetVisibleArea (fTrickView->LocalToTablet (fTrickView->GetLocalExtent ()));
		}

		fUpdateRegion = oldUpdateRegion - updateRegion;	// subtract of what we just updated.
#elif	qMacToolkit
		AssertNotNil (GetOSRepresentation ());

		/*
		 * On the mac, the update region field of the window is in global coordinates.
		 */
		Region	globalUpdateRegion	=	LocalToGlobal (updateRegion);

		/*
		 * We want to only update the updateRegion, even if this is smaller than our osWindow
		 * updateRgn field.  To do this, we munge the osWindow field before the BeginUpdate call.
		 * This is kinda dangerous, and assumes that the update region of a window is never freed
		 * but is only made empty with SetEmptyRgn calls). See IM I-278 for the gory details
		 */
		osRegion**		windowUpdateRgn = osWindowPeek (GetOSRepresentation ())->updateRgn;
		AssertNotNil (windowUpdateRgn);

		/*
		 * Save the update region to resture un-updated parts later, and
		 * temporarily set the update region to be just the subpart we are updating...
		 */
		Region			oldUpdateRgn = windowUpdateRgn;
		DoSectRgn (windowUpdateRgn, globalUpdateRegion.PeekAtOSRegion (), windowUpdateRgn);
		if (not DoEmptyRgn (windowUpdateRgn)) {
			DoBeginUpdate (GetOSRepresentation ());
				/*
				 * At this point the visRgn has been intersected with the updateRgn.  We pass this
				 * along for logical clipping purposes.
				 */
				Tablet::xDoSetPort (GetOSRepresentation ());
				DoSetOrigin (0, 0);
				Render (TabletToLocal (GetOSRepresentation ()->visRgn));
			DoEndUpdate (GetOSRepresentation ());
		}
		Assert (windowUpdateRgn == osWindowPeek (GetOSRepresentation ())->updateRgn);
		DoCopyRgn ((oldUpdateRgn - globalUpdateRegion).PeekAtOSRegion (), windowUpdateRgn);
#elif	qXToolkit
		Render (updateRegion);
#endif	/*Toolkit*/
		Assert (--updateLevel <= kMaxDepth-1);
	}
	AssertNotNil (fTablet);
	fTablet->Sync ();
}

void	Window::Update ()
{
//	Update (GetLocalExtent ());
	Update (Rect (kZeroPoint, fShell->GetSize ()));		// SB CONTENTSIZE???
}

void	Window::SetFont (const Font* font)
{
	AssertNotNil (fTrickView);
	fTrickView->SetFont (font);
}

Boolean	Window::TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState)
{
	AssertNotNil (fTrickView);
	if (fTrickView->GetEffectiveLive ()) {
		return (fTrickView->TrackMovement (cursorAt, mouseRgn, keyBoardState));
	}
	return (False);
}

Boolean	Window::TrackHelp (const Point& cursorAt, Region& helpRegion)
{
	AssertNotNil (fTrickView);
	if (fTrickView->GetEffectiveLive ()) {
		return (fTrickView->TrackHelp (cursorAt, helpRegion));
	}
	return (False);
}










/*
 ********************************************************************************
 ************************************** TrickView *******************************
 ********************************************************************************
 */

TrickView::TrickView (Window& w) :
	fWindow (w)
{
}

void	TrickView::Layout ()
{
#if		qXtToolkit
// not sure why we must call XtResizeWidget instead of XtSetValues - I think both should work but XtSetValues
// is better - it just doesnt work!!! LGP Jan 6, 1992

// maybe it will again now that (I think) I got rid of the race conditions with
// getting extnts and converting coords... Try again or understand why it doesnt work???

	Point size = GetSize ();
	::XtResizeWidget (GetWidget (), (int)size.GetH (), (int)size.GetV (), 0);

	// be sure the tablet has the right visible area...
	AssertNotNil (GetTablet ());
	GetTablet ()->SetVisibleArea (LocalToTablet (GetLocalExtent ()));
#endif	/*qXtToolkit*/
	if (fWindow.fMainView != Nil) {
		fWindow.fMainView->SetExtent (fWindow.GetShell ().GetLocalContentRegion ().GetBounds ());
	}
	GroupView::Layout ();
}

Panel*	TrickView::GetParentPanel () const
{
	return (&fWindow.GetPlane ());
}

Point	TrickView::LocalToTablet_ (const Point& p)	const
{
	return (fWindow.LocalToTablet (p));
}

Point	TrickView::TabletToLocal_ (const Point& p)	const
{
	return (fWindow.TabletToLocal (p));
}

void	TrickView::Refresh_ (const Region& updateRegion, Panel::UpdateMode updateMode)
{
	fWindow.Refresh (updateRegion, updateMode);
}

void	TrickView::Update_ (const Region& updateRegion)
{
	fWindow.Update (updateRegion);
}

Boolean	TrickView::GetEffectiveLive () const
{
	return (fWindow.GetLive ());
}

// overridden to provide public scope
void	TrickView::EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode update)
{
	GroupView::EffectiveLiveChanged (newLive, update);
}

#if		qXGDI
osWidget*	TrickView::GetWidget () const
{
	EnsureNotNil (fWindow.fMainViewWidget);
	return (fWindow.fMainViewWidget);
}
#endif	/*qXGDI*/










/*
 ********************************************************************************
 ********************************** WindowEventHandler **************************
 ********************************************************************************
 */
WindowEventHandler::WindowEventHandler (Window& w):
	EventHandler (),
	fWindow (w)
{
	Activate ();
}

WindowEventHandler::~WindowEventHandler ()
{
	Deactivate ();
}

void	WindowEventHandler::ProcessUpdate (const osEventRecord& eventRecord, Window& window, const Region& updateRegion)
{
	/*
	 * For Xt, we just let Xt dispatch the event, assuming it will dispatch it to a widget, or set of widgets whose
	 * expose proc we've replaced. Then, we'll call fWindow.Refresh () appropriately from those patches. Then, at idle time,
	 * we'll process the update events.
	 */
#if		qMacGDI
	if (&window == &fWindow) {
		Assert (fWindow.GetVisible ());
#if		qUsePrivateUpdateRegions
		// when the update event comes from the operating system, we must add it to our update region
		fWindow.fUpdateRegion += updateRegion;
#endif
		fWindow.Update (updateRegion);
		return;
	}
#endif
	EventHandler::ProcessUpdate (eventRecord, window, updateRegion);
}





// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

