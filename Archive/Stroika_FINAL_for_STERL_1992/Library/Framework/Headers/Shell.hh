/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Shell__
#define	__Shell__

/*
 * $Header: /fuji/lewis/RCS/Shell.hh,v 1.6 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		A window shell is a structure used to display a logical window to the user.
 *	A shell is something largely managed by the external windowing system, and
 *	what we call a "Window" is a token used to glue the content area of what the user
 *	percieces as a window to our own internal data structures.
 *
 *		Window Shells are largely implementation details, and can largely be ignored by the programmer - mostly
 *	the programmer will just create windows, and the shell management will be taken care of for him.
 *
 * TODO:
 *
 * Notes:
 *
 * Changes:
 *	$Log: Shell.hh,v $
 *		Revision 1.6  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.5  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.4  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.2  1992/07/01  04:05:14  lewis
 *		Renamed Strings.hh String.hh
 *
 *		Revision 1.14  1992/03/26  09:38:44  lewis
 *		Made GetVisibleArea () const method.
 *
 *		Revision 1.13  1992/01/29  07:38:33  lewis
 *		Added fields to make mainappshell work again under unix after sterls re-organization.
 *
 *		Revision 1.12  1992/01/29  04:37:46  sterling
 *		massibe changes
 *
 *		Revision 1.11  1992/01/22  15:09:02  sterling
 *		added "class Keyboard" cuz keyboard is references
 *
 *		Revision 1.10  1992/01/19  21:25:49  lewis
 *		Added StandardAlertWindowShell.
 *
 *		Revision 1.9  1992/01/18  09:11:45  lewis
 *		Moved  window hints stuff to Implementation Shell. Did inserter for WindowShellHints.
 *		Re-organized how we do window placement - still needs some work.
 *
 *		Revision 1.8  1992/01/07  00:45:13  lewis
 *		Added GetLocalContentOrigin () and GetLocalContentRegion () accessor routines.
 *
 *		Revision 1.7  1992/01/06  09:59:32  lewis
 *		Added GetReallyVisible () method to Shell and changed meaning of GetVisible
 *		to just refer to if the window is supposed to be visible. Take into account
 *		that under X it make take some time for it to actually be made visible,
 *		and that under mactoolkit we may want to wait til we get around to calling
 *		Desktop::SyncronizeWindows....
 *
 *		Revision 1.5  1992/01/05  05:57:09  lewis
 *		Added losts of support directed at moving View stuff out of window.
 *		
 *		Revision 1.4  1991/12/27  16:51:41  lewis
 *		Got most of X/Motif implematnion working, and significantly cleaned up code,
 *		and mac implementation. Still a bit to go here.
 *
 *		Revision 1.2  1991/12/18  09:04:44  lewis
 *		Original implementation - got limping along under motif.
 *
 *
 *
 */

#include	"String.hh"

#include	"PixelMap.hh"
#include	"Region.hh"



#if		qXGDI
struct	osWidget;
struct	osDisplay;
#endif

#if		qMacGDI || qXGDI
struct	osEventRecord;
#endif
#if		qMacGDI
struct osGrafPort;
#endif


class	Window;
class	WindowPlane;
class	ImplementationWindowShell;




/*
 * These are things that you can specify about how you want your window shell to look and
 * behave. They are generally SYSTEM DEPENDENT, and may will only affect behaviour
 * if they are passed in when you create the shell, and not if you try resetting them.
 *
 * This class is somewhat unusual in design, in that it is not intended to do anything, but
 * package up a request, and say what parameters have been specified.
 *
 */
class	WindowShellHints {
	public:
		WindowShellHints ();

	/*
	 * Setting Interface.
	 */
		nonvirtual	void	SetTitle (const String& title, Boolean userSet = True);

		nonvirtual	void	SetDesiredOrigin (const Point& desiredOrigin, Boolean userSet = True);

		nonvirtual	void	SetMinSize (const Point& minSize, Boolean userSet = True);
		nonvirtual	void	SetMaxSize (const Point& maxSize, Boolean userSet = True);
		nonvirtual	void	SetDesiredSize (const Point& desiredSize, Boolean userSet = True);

		nonvirtual	void	SetCloseable (Boolean closeable, Boolean userSet = True);
		nonvirtual	void	SetMoveable (Boolean moveable, Boolean userSet = True);
		nonvirtual	void	SetResizeable (Boolean resizeable, Boolean userSet = True);
		nonvirtual	void	SetZoomable (Boolean zoomable, Boolean userSet = True);

#if		qMotifUI
		nonvirtual	void	SetIcon (const PixelMap& icon, Boolean userSet = True);
#endif


	/*
	 * Query Interface.
	 */
		nonvirtual	Boolean		TitleSet () const;
		nonvirtual	String		GetTitle () const;

		nonvirtual	Boolean		DesiredOriginSet () const;
		nonvirtual	Point		GetDesiredOrigin () const;

		nonvirtual	Boolean		MinSizeSet () const;
		nonvirtual	Point		GetMinSize () const;
		nonvirtual	Boolean		MaxSizeSet () const;
		nonvirtual	Point		GetMaxSize () const;
		nonvirtual	Boolean		DesiredSizeSet () const;
		nonvirtual	Point		GetDesiredSize () const;

		nonvirtual	Boolean		CloseableSet () const;
		nonvirtual	Boolean		GetCloseable () const;
		nonvirtual	Boolean		MoveableSet () const;
		nonvirtual	Boolean		GetMoveable () const;
		nonvirtual	Boolean		ResizeableSet () const;
		nonvirtual	Boolean		GetResizeable () const;
		nonvirtual	Boolean		ZoomableSet () const;
		nonvirtual	Boolean		GetZoomable () const;

#if		qMotifUI
		nonvirtual	Boolean		IconSet () const;
		nonvirtual	PixelMap	GetIcon () const;
#endif

	private:
		Boolean		fTitleSet;
		String		fTitle;
		Boolean		fDesiredOriginSet;
		Point		fDesiredOrigin;
		Boolean		fMinSizeSet;
		Point		fMinSize;
		Boolean		fMaxSizeSet;
		Point		fMaxSize;
		Boolean		fDesiredSizeSet;
		Point		fDesiredSize;
		Boolean		fCloseableSet;
		Boolean		fCloseable;
		Boolean		fMoveableSet;
		Boolean		fMoveable;
		Boolean		fResizeableSet;
		Boolean		fResizeable;
		Boolean		fZoomableSet;
		Boolean		fZoomable;
#if		qMotifUI
		Boolean		fIconSet;
		PixelMap	fIcon;
#endif
};


/*
 * Stream inserters and extractors.
 */
class ostream&	operator<< (class ostream& out, const WindowShellHints& h);
class istream&	operator>> (class istream& in, WindowShellHints& h);



class KeyBoard;

/*
 * WindowShell is an abstract class defining the protocol for interactions between "user windows"
 * and the shell provided by the window manager (what the liveware percieves as "the window").
 */
class	WindowShell {
	protected:
		WindowShell ();

	public:
		virtual ~WindowShell ();

		virtual	Window&			GetWindow () const			= Nil;
		virtual	WindowPlane&	GetWindowPlane () const		= Nil;

		virtual	WindowShellHints	GetWindowShellHints () const							=	Nil;
		virtual	void				SetWindowShellHints (const WindowShellHints& hints)		=	Nil;		


		/*
		 *		These sizes/origins refer to the shell itself - and not the content area.
		 * (Add more methods to do that??)
		 *
		 *		NB: These setsize methods DO NOT gaurentee that the size will be given, for some
		 * it is just a requiest. Certain subclasses may guarentee the size requested, but
		 * not all.
		 *
		 *		Also, note that both the getters and setters are virtual, as the system may change these
		 * values out from under us, without notification.
		 */
		virtual	Point	GetOrigin () const								=	Nil;
		virtual	void	SetOrigin (const Point& newOrigin)				=	Nil;
		virtual	Point	GetSize () const								=	Nil;
		virtual	void	SetSize (const Point& newSize)					=	Nil;
		virtual	Rect	GetExtent () const								=	Nil;
		virtual	void	SetExtent (const Rect& extent)					=	Nil;


		/*
		 * note all coods in globals.
		 * Also, no SetContentOrigin () since thats up to the shell.
		 *
		 * NB: We have a GetLocalContentRegion () defined here so we can reliable get this value - calling
		 * GlobalToLocal (GetContentRegion () would be a race, since between when get did the getorigin () for
		 * the contentregion, we call GetOrigin () again to compute Global again.
		 */
		virtual	Point	GetLocalContentOrigin () const					=	Nil;
		virtual	Point	GetContentOrigin () const						=	Nil;
		virtual	Point	GetContentSize () const							=	Nil;
		virtual	void	SetContentSize (const Point& size)				=	Nil;
		virtual	Region	GetLocalContentRegion () const					=	Nil;
		virtual	Region	GetContentRegion () const						=	Nil;


		/*
		 * Visibility.
		 */
		virtual	Boolean	GetVisible () const								=	Nil;	// requested visibilty (except for slight time lag - should be same as GetReallyVisible ())
		virtual	Boolean	GetReallyVisible () const						=	Nil;
		virtual	void	SetVisible (Boolean visible)					=	Nil;

		virtual	Region	GetVisibleArea () const							=	Nil;

		virtual	String	GetTitle () const					=	Nil;
		virtual	void	SetTitle (const String& title)		=	Nil;

		virtual	Boolean		GetCloseable () const		=	Nil;
		virtual	Boolean		GetMoveable () const		=	Nil;
		virtual	Boolean		GetResizeable () const		=	Nil;
		virtual	Boolean		GetZoomable () const		=	Nil;

#if		qMotifUI
		virtual	PixelMap	GetIcon () const				=	Nil;
#endif
		 
		/*
		 * Retrieve OS Represenation of shell.
		 */
#if		qMacGDI
		virtual	osGrafPort*	GetOSRepresentation () const	=	Nil;
#elif	qXGDI
		virtual	osWidget*	GetOSRepresentation () const	=	Nil;
#endif
	
	private:
#if		qMacUI
		// Implementation details - on mac - user code must do some window manager support
		nonvirtual	void	DragEvent (const Point& pressAt, const KeyBoard& keyBoardState);
		nonvirtual	void	GrowEvent (const Point& pressAt);
		nonvirtual	void	GoAwayEvent (const Point& pressAt);
		nonvirtual	void	ZoomEvent (const Point& pressAt, Boolean zoomIn);

		virtual		Point	LocalToTablet (const Point& p)	const;
		virtual		Point	TabletToLocal (const Point& p)	const;
#endif	/*qMacUI*/

		virtual	void	BeInWindow (Window* window)					=	Nil;		// called only by window to attach-detach shells.
	friend	class	Window;															// so it can call BeInWindow ()
};



/*
 * Used to share code between various shell implementations.
 */
class	ImplementationWindowShell : public WindowShell {
	protected:
		ImplementationWindowShell (const WindowShellHints& hints);

	public:
		~ImplementationWindowShell ();

		override	Window&			GetWindow () const;
		override	WindowPlane&	GetWindowPlane () const;

		override	WindowShellHints	GetWindowShellHints () const;
		override	void				SetWindowShellHints (const WindowShellHints& hints);

		override	Point	GetOrigin () const;
		override	void	SetOrigin (const Point& newOrigin);
		override	Point	GetSize () const;
		override	void	SetSize (const Point& newSize);
		override	Rect	GetExtent () const;
		override	void	SetExtent (const Rect& extent);

		override	void	SetContentSize (const Point& size);
		override	Point	GetLocalContentOrigin () const;
		override	Point	GetContentOrigin () const;
		override	Point	GetContentSize () const;
		override	Region	GetLocalContentRegion () const;
		override	Region	GetContentRegion () const;

		override	Boolean	GetVisible () const;
		override	Boolean	GetReallyVisible () const;
		override	void	SetVisible (Boolean visible);

		override	Region	GetVisibleArea () const;

#if		qMacGDI
		override	osGrafPort*	GetOSRepresentation () const;
#elif	qXGDI
		override	osWidget*	GetOSRepresentation () const;
#endif

		override	String	GetTitle () const;
		override	void	SetTitle (const String& title);

		override	Boolean		GetCloseable () const;
		override	Boolean		GetMoveable () const;
		override	Boolean		GetResizeable () const;
		override	Boolean		GetZoomable () const;

#if		qMotifUI
		override	PixelMap	GetIcon () const;
#endif

		static	WindowShellHints	GetDefaultHints ();
		
	protected:
#if		qMacToolkit
		nonvirtual	void	SetOSRepresentation (osGrafPort* theOSWindow, const Point& topLeftBorder);
#elif	qXtToolkit
		nonvirtual	void	SetOSRepresentation (osWidget* theOSWindow);
#endif	/*qGDI*/
		nonvirtual	void	SetWindowPlane (WindowPlane* plane);
		
		virtual		void	SetContentOrigin (const Point& newOrigin);
		
		virtual		String	SetTitle_ (const String& title);
		virtual		void 	ApplyWindowShellHints (WindowShellHints& hints);
		virtual		void	BuildOSRepresentation (WindowShellHints& hints) = Nil;

	private:
		override	void	BeInWindow (Window* window);

		WindowShellHints	fWindowShellHints;
		Window*				fWindow;
		WindowPlane*		fWindowPlane;
		Point				fTopLeftBorder;
		Point				fBotRightBorder;
		Boolean				fVisible;
#if		qMacToolkit
		osGrafPort*			fOSWindow;
#elif	qXtToolkit
		osWidget*			fShellWidget;
#endif	/*qGDI*/

#if		qXtToolkit
		nonvirtual	void	CheckBorders ();
#endif	/*qXtToolkit*/
};

/*
 * This is a very common sort of shell. You use this for a main document. This would be
 * a "DocumentProc" on the mac, and MDI window under MS Windows, and a "topLevelShell?" under Xt.
 */
class	StandardDocumentWindowShell : public ImplementationWindowShell {
	public:
		StandardDocumentWindowShell ();
		StandardDocumentWindowShell (const WindowShellHints& hints);
		~StandardDocumentWindowShell ();

		static	WindowShellHints	GetDefaultHints ();
	
	protected:
		override	void	BuildOSRepresentation (WindowShellHints& hints);
};



/*
 * Use this shell for Modal dialogs. These dialogs can be moved by the user, but trap other interactions
 * with the windowing system (if possible on that system) to make the interaction modal.
 */
class	StandardDialogWindowShell : public ImplementationWindowShell {
	public:
		StandardDialogWindowShell ();
		StandardDialogWindowShell (const WindowShellHints& hints);
		~StandardDialogWindowShell ();

		static	WindowShellHints	GetDefaultHints ();
	
	protected:
		override	void	BuildOSRepresentation (WindowShellHints& hints);
};


/*
 */
class	StandardAlertWindowShell : public ImplementationWindowShell {
	public:
		StandardAlertWindowShell ();
		StandardAlertWindowShell (const WindowShellHints& hints);
		~StandardAlertWindowShell ();

		static	WindowShellHints	GetDefaultHints ();
	
	protected:
		override	void	BuildOSRepresentation (WindowShellHints& hints);
};


/*
 * Use this shell to build a shell window with no adornments added by the window manager. This might
 * be used for help-balloons, popup menus, or for a pallet you want to display without a shell around it.
 *
 * <we may need to add params to tell system if its transient???>
 */
class	PlainWindowShell : public ImplementationWindowShell {
	public:
		PlainWindowShell ();
		PlainWindowShell (const WindowShellHints& hints);
		~PlainWindowShell ();

		static	WindowShellHints	GetDefaultHints ();
	
	protected:
		override	void	BuildOSRepresentation (WindowShellHints& hints);
};



class	StandardPalletWindowShell : public ImplementationWindowShell {
	public:
		StandardPalletWindowShell ();
		StandardPalletWindowShell (const WindowShellHints& hints);
		~StandardPalletWindowShell ();

		static	WindowShellHints	GetDefaultHints ();
	
	#if		qMacToolkit
		Point	StandardPalletWindowShell::LocalToTablet (const Point& p)	const;
		Point	StandardPalletWindowShell::TabletToLocal (const Point& p)	const;
	#endif	/*qMacToolkit*/

	protected:
		override	void	BuildOSRepresentation (WindowShellHints& hints);
};



#if		qMotifUI
class	MainApplicationShell : public ImplementationWindowShell {
	public:
		MainApplicationShell (const String& appName, const String& appClass, osDisplay* display);
		MainApplicationShell (const WindowShellHints& hints, const String& appName, const String& appClass, osDisplay* display);
		~MainApplicationShell ();

		static	WindowShellHints	GetDefaultHints ();

	protected:
		override	void	BuildOSRepresentation (WindowShellHints& hints);

	private:
		String		fAppName;
		String		fAppClass;
		osDisplay*	fOSDisplay;
};
#endif	/*qMotifUI*/





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Shell__*/

