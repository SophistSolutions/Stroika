/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Window__
#define	__Window__

/*
 * $Header: /fuji/lewis/RCS/Window.hh,v 1.7 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *		The nomenclature for such things is sadly inconsitent across window systems.
 *		What we refer to as a 'Window' here is equivilent to an 'MDI window' in MS windows,
 *		and a 'Shell Window' in Motif.
 *
 * Changes:
 *	$Log: Window.hh,v $
 *		Revision 1.7  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.6  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.5  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.3  1992/07/08  02:27:38  lewis
 *		Renamed PointInside -> Contains ().
 *
 *		Revision 1.2  1992/07/02  04:46:23  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr, and used Panel::
 *		scope resulution operator for nested enum UpdateMode.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.28  92/03/26  09:33:08  09:33:08  lewis (Lewis Pringle)
 *		Got rid of extra arg oldLive to EffectiveLiveChanged, and EffecitveVisibilityChanged(). Also, made
 *		GetVisibleArea () const method.
 *		
 *		Revision 1.26  1992/03/10  00:00:25  lewis
 *		Use new DispatchKeyEvent () interface instead of old HandleKey () interface.
 *
 *		Revision 1.25  1992/02/21  20:08:01  lewis
 *		Got rid of qMPW_SymbolTableOverflowProblem workaround.
 *
 *		Revision 1.24  1992/02/15  04:58:23  sterling
 *		put ActiveItem code here
 *
 *		Revision 1.23  1992/02/11  01:03:41  lewis
 *		Got rid of unused backward compat calls.
 *
 *		Revision 1.22  1992/01/31  22:48:58  lewis
 *		Got rid of more backward compatability shit now that Sterl has bootstrapped.
 *
 *		Revision 1.21  1992/01/31  05:33:19  lewis
 *		Use private region stuff now with X. It is needed, and will be more as we seguey into using XtExposeCallbacks
 *		rather than directly using the X events.
 *
 *		Revision 1.20  1992/01/29  04:37:57  sterling
 *		changed window hints support
 *
 *		Revision 1.19  1992/01/22  08:04:25  lewis
 *		Tried turning privateUpdateRegion stuff off, and made it work(unix). This stuff needs lots of
 *		work.
 *
 *		Revision 1.18  1992/01/18  09:16:00  lewis
 *		Tried being a virtual base of MenuCommandHandler and KeyHandler in Window so we could mix Window together
 *		with WindowController (which already had these) but it agravated the usual virtual base class bugs with the
 *		2.1 based HP compiler. Leave the vbase in - delete it if it causes trouble.
 *		Also, add new method AppleWindowShellHints () as part of new stragegy for layout of windows in the window
 *		plane before being made visible.
 *
 *		Revision 1.15  1992/01/08  23:55:10  lewis
 *		Added support for Private update regions (portable implementation of update regions for motif).
 *
 *		Revision 1.14  1992/01/08  05:38:38  lewis
 *		Add WindowEventHandler.
 *
 *		Revision 1.13  1992/01/06  10:01:26  lewis
 *		Lots of cleanups of cruft left around for backward compatablity. Also,
 *		got rid of AdjustMainViewExtent.
 *		Supported GetReallyVisible () and stopped keeping our own boolean for visibilty.
 *
 *		Revision 1.12  1992/01/05  05:43:45  lewis
 *		Made window no longer subclass from View.
 *
 *		Revision 1.11  1991/12/27  16:53:12  lewis
 *		Converted to usage of new Shell support and moved alot of code into shell.
 *
 *		Revision 1.10  1991/12/18  09:05:19  lewis
 *		Started adding Shell support.
 *
 *		Revision 1.8  1991/12/12  02:59:29  lewis
 *		Event related cleanups.
 *
 *
 */



#include	"Sequence.hh"

#include	"Tablet.hh"

#include	"KeyHandler.hh"
#include	"MenuCommandHandler.hh"
#include	"View.hh"



/*
 * Cannot figure out how to do the eqivilent of invalrgn in motif, so fake it ourselves for now.
 * Use qUsePrivateUpdateRegions.
 */
#if		qMacToolkit
#define	qUsePrivateUpdateRegions	0
#else
#define	qUsePrivateUpdateRegions	1
#endif


#if		qXGDI
struct	osWidget;
#endif

#if		qMacToolkit || qXToolkit
struct	osEventRecord;
#endif



#if		!qRealTemplatesAvailable
	typedef	class Window*	WindowPtr;
	Declare (Iterator, WindowPtr);
	Declare (Collection, WindowPtr);
	Declare (AbSequence, WindowPtr);
	Declare (Array, WindowPtr);
	Declare (Sequence_Array, WindowPtr);
	Declare (Sequence, WindowPtr);
#endif


class	WindowShell;
class	WindowShellHints;





/*
 * Use to pass on menu commands, in target chain.
 */
class	WindowController : public virtual MenuCommandHandler, public virtual KeyHandler {
	public:
		WindowController ();
		virtual ~WindowController ();

		virtual	void	HandleClose (Window& ofWindow);

		nonvirtual	SequenceIterator(WindowPtr)*	MakeWindowIterator (SequenceDirection d = eSequenceForward) const;

	protected:
		Sequence(WindowPtr)	fWindows;

	friend	class	Window;
};

// To ease the pain of changing window design...	LGP Jan 3
#define		qWindowBackwardCompatabiltyMode		1




class	WindowPlane;
class	ProcessLayoutChecker;
class	TrickView;
class	WindowEventHandler;
#if 	qCanFreelyUseVirtualBaseClasses
class	Window : public virtual LiveItem, public virtual MenuCommandHandler, public virtual KeyHandler
#else
class	Window : public LiveItem, public virtual MenuCommandHandler, public virtual KeyHandler
#endif
	{
	public:
		static	const	Boolean	kActive;

		Window (WindowShell* windowShell);
		Window ();

		~Window ();		

		nonvirtual	WindowController*	GetWindowController () const;
		virtual		void				SetWindowController (WindowController* windowController);


		/*
		 * Main View can be Nil.  Setting MainView does not delete the old main view.
		 * A reference is kept to the view: it is not copied.  It is automatically
		 * resized to fit the size of the window.
		 */
		nonvirtual	View*				GetMainView () const;
		virtual		void				SetMainView (View* v);
		nonvirtual	MenuCommandHandler*	GetMenuTarget () const;
		virtual		void				SetMenuTarget (MenuCommandHandler* menuTarget);
		nonvirtual	KeyHandler*			GetKeyTarget () const;
		virtual		void				SetKeyTarget (KeyHandler* keyTarget);

		nonvirtual	void	SetMainViewAndTargets (View* v, MenuCommandHandler* menuTarget, KeyHandler* keyTarget);


		nonvirtual	void	SetContentSize (const Point& size);

		// called by shell to place the window, just before being made visible. It adjusts the size
		// of the window if the user did not.
		virtual	void	AdjustWindowShellHints (WindowShellHints& hints);

		/*
		 * Pass commands on to WindowController.
		 * Give Main View a crack at commands first, and then call inherited.
		 */
		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		override	Boolean	DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
											  KeyComposeState& composeState);


		nonvirtual	Boolean	GetVisible () const;				// has been setvisible
		nonvirtual	void	SetVisible (Boolean visible);
		nonvirtual	Boolean	GetReallyVisible () const;			// Real Visibility (according to window manager, or osrep) may lag waiting for WM or desktop::syncronize...

		override	Boolean	GetLive () const;

		nonvirtual	void	Select ();	// make visible and bring to front of plane
		
		virtual		void	Close ();					// just call WindowController::HandleClose ()

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

	private:
		// should go away - LGP Mar 14, 1992
		override	void	Render (const Region& updateRegion);
	public:

		nonvirtual	WindowPlane&	GetPlane ()	const;
		
		nonvirtual	Boolean	GetProcessFirstClick () const;
		nonvirtual	void	SetProcessFirstClick (Boolean processFirstClick);
		
		nonvirtual	WindowShell&	GetShell () const;

		nonvirtual	Boolean	GetActive () const;
		nonvirtual	void	SetActive (Boolean active, Panel::UpdateMode update = Panel::eDelayedUpdate);

#if		qMacGDI
		static	Window*		OSLookup (osGrafPort* osWin);		// can return Nil if not found
#elif	qXGDI
		static	Window*		OSLookup (osWidget* osWin);			// can return Nil if not found
		static	Window*		OSLookup (unsigned long osWin);		// can return Nil if not found
#endif

#if		qXGDI
		virtual		void	HandleConfigureNotify (const osEventRecord& eventRecord);
#endif

	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, Panel::UpdateMode updateMode);
		virtual		void	EffectiveVisibilityChanged (Boolean newVisible, Panel::UpdateMode updateMode);

		virtual	Boolean	GetActive_ () const;
		virtual	void	SetActive_ (Boolean active, Panel::UpdateMode updateMode);
		
		virtual		void	SetShell (WindowShell* windowShell);

	private:
		WindowShell*			fShell;
		Tablet*					fTablet;
		View*					fMainView;
		MenuCommandHandler* 	fMenuTarget;
		KeyHandler* 			fKeyTarget;
		Boolean					fProcessFirstClick;
		WindowController*		fWindowController;
		ProcessLayoutChecker*	fProcessLayoutChecker;
		TrickView*				fTrickView;					// implementation detail view owned by window to do magic...
		WindowEventHandler*		fWindowEventHandler;
#if		qUsePrivateUpdateRegions
		Region					fUpdateRegion;
#endif

#if		qXGDI
		osWidget*	fMainViewWidget;
#endif	/*qXGDI*/

#if		qMacUI
		Rect	GetSizeBox () const;
#endif

		static	Sequence(WindowPtr)	sWindows;

#if		qWindowBackwardCompatabiltyMode
	public:
		override	void	SetBackground (const Tile* tile);

#if		qMacGDI
		nonvirtual	struct osGrafPort*	GetOSRepresentation () const;
#elif	qXGDI
		nonvirtual	osWidget*			GetOSRepresentation () const;
#endif

		virtual	Region	GetContentRegion () const;	//	returns a region in local coords even though the windowshell version returns in global coords???

// LGP Dec 18, 1991 -
// Not very elegant to put this stuff here - in a sense it might belong in some subclass, or as methods of the shell,
// but we posit that windows have icons (for display when they are iconized), and a title, and
// probably other "Hints" about user resizing.
//
// This are basically all just trivial re-directions to the shell. The shell may choose to ignore them.
// not sure we want this here - maybe just use setwindowhints mechanism?

		nonvirtual	String		GetTitle () const;
		nonvirtual	void		SetTitle (const String& title);

public:	// temp hack so people can still think of window as a view - briefly....

		nonvirtual	Region	GetVisibleArea () const;

		nonvirtual	Point	LocalToGlobal (const Point& p) const;
		nonvirtual	Point	GlobalToLocal (const Point& p) const;
		nonvirtual	Rect	LocalToGlobal (const Rect& r) const;
		nonvirtual	Rect	GlobalToLocal (const Rect& r) const;
		nonvirtual	Region	LocalToGlobal (const Region& r) const;
		nonvirtual	Region	GlobalToLocal (const Region& r) const;

		nonvirtual	Point	LocalToTablet (const Point& p) const;
		nonvirtual	Point	TabletToLocal (const Point& p) const;
		nonvirtual	Region	LocalToTablet (const Region& r) const;
		nonvirtual	Region	TabletToLocal (const Region& r) const;

		nonvirtual	Rect	GetExtent () const;
		nonvirtual	Rect	GetLocalExtent () const;

		nonvirtual	Boolean	ProcessLayout (Boolean layoutInvisible = False);

		nonvirtual	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState);
		nonvirtual	Boolean	TrackHelp (const Point& cursorAt, Region& helpRegion);

		nonvirtual	void	Update ();
		nonvirtual	void	Update (const Region& updateRegion);


		nonvirtual	void	SetOrigin (const Point& newOrigin);

		nonvirtual	void	SetFont (const Font* font);
		nonvirtual	Point	GetOrigin () const;
		nonvirtual	Point	GetSize () const;

Boolean fActive;
Boolean fVisible;

		nonvirtual	Boolean	Contains (const Point& p) const;	// p in Enclosure coordinates

		nonvirtual	void	Refresh (const Region& r, Panel::UpdateMode updateMode);
		nonvirtual	void	Refresh (const Region& r);
		nonvirtual	void	Refresh (Panel::UpdateMode updateMode);
#endif	/*qWindowBackwardCompatabiltyMode*/



	friend	class	TrickView;
	friend	class	ProcessLayoutChecker;
	friend	class	WindowEventHandler;
};




/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

inline	Boolean				Window::GetProcessFirstClick () const	{	return (fProcessFirstClick);				}
inline	View*				Window::GetMainView () const			{	return (fMainView);							}
inline	MenuCommandHandler*	Window::GetMenuTarget () const			{	return (fMenuTarget);						}
inline	KeyHandler*			Window::GetKeyTarget () const			{	return (fKeyTarget);						}
inline	WindowShell&		Window::GetShell () const				{	EnsureNotNil (fShell); return (*fShell);	}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Window__*/

