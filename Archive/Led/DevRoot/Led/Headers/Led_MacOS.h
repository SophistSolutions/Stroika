/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_MacOS_h__
#define	__Led_MacOS_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_MacOS.h,v 2.43 2003/04/16 14:04:10 lewis Exp $
 */


/*
@MODULE:	Led_MacOS
@DESCRIPTION:
		<p>Templated MacOS SDK wrapper support. This code handles mapping between MacOS SDK concepts
	like a Rect and its InvalidateRect () model and that of Led, and to handle basic event processing.</p>
 */



/*
 * Changes:
 *	$Log: Led_MacOS.h,v $
 *	Revision 2.43  2003/04/16 14:04:10  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext instances, and use new TextInteractor::BreakInGroupedCommands () wrapper (and other small cleanups)
 *	
 *	Revision 2.42  2003/04/10 14:02:21  lewis
 *	(vaguely related to work on SPR#1329)- cleanup doccomments and org of TextInteractor::OnCopy/Paste_Before/After. Moved mac/Win specific code exclusely to _MacOS/_Win32 subclasses(tempaltes). Improved docs
 *	
 *	Revision 2.41  2003/04/09 16:25:21  lewis
 *	SPR#1419: lose Refresh_ and Update_ overloaded virtual overrides
 *	
 *	Revision 2.40  2003/04/08 23:34:52  lewis
 *	SPR#1404: added TextInteractor::GetCaretShownSituation () method to wrap selStart==selEnd
 *	check (so we can override that for table code blinking caret. Updated each Led_XXX classlib
 *	wrapper to call that instead of selStart==selEnd
 *	
 *	Revision 2.39  2003/04/08 22:37:08  lewis
 *	SPR#1404: move WhileTrackingConstrainSelection () override from Led_Gtk/Win32/MacOS
 *	templates to default impl for TextInteractor (so its used in Embedded WP class so
 *	cells handle double clicks properly
 *	
 *	Revision 2.38  2003/04/04 14:41:41  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.37  2003/03/21 13:59:36  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to
 *	TextInteractor::ProcessSimpleClick (adding extra arg). Also lots of other small code cleanups
 *	
 *	Revision 2.36  2003/03/20 22:05:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2003/03/20 15:50:00  lewis
 *	SPR#1360 - eliminate CallEnterIdleCallback calls and start proving basic new IdleManager support
 *	
 *	Revision 2.34  2003/03/17 21:37:40  lewis
 *	SPR#1350 - qSupportEnterIdleCallback is now automatically true - and not a conditional varaible anymore
 *	
 *	Revision 2.33  2003/03/14 15:32:27  lewis
 *	SPR#1335 - Must call Update () in ::StillDown () loop.
 *	
 *	Revision 2.32  2003/03/14 14:38:49  lewis
 *	SPR#1343 - Get rid of funny code for setting anchor, and override StandardStyledTextInteractor::WhileSimpleMouseTracking
 *	to properly handle tracking over embeddings and fix the WordProcessor::WhileSimpleMouseTracking
 *	to do likewise (it was already correct except for cosmetics). Net effect is that clicking on
 *	embedding works correctly again on Win32 (was OK on ohter platforms) - and is now clearer on ALL
 *	
 *	Revision 2.31  2003/01/29 19:15:07  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.30  2002/09/19 14:14:03  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.29  2002/05/06 21:33:29  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.28  2001/11/27 00:29:41  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.27  2001/10/17 21:46:40  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.26  2001/10/11 13:54:28  lewis
 *	SPR#1057- add SetHasScrollBar backward compat routine for qSupportLed23CompatAPI
 *	
 *	Revision 2.25  2001/09/26 15:41:15  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.24  2001/09/24 16:53:35  lewis
 *	check that the result of ::FindControl() returns one of OUR controls - and not
 *	just another one from the same window
 *	
 *	Revision 2.23  2001/09/24 16:29:29  lewis
 *	SPR#0993- still working on fixing quirky scrollbar behavior - still no working right yet
 *	
 *	Revision 2.22  2001/09/24 14:22:49  lewis
 *	SPR#1042- cleaned up region code. Prep work to fix SPR#0993 (quirky scrollbar behavior).
 *	SPR#0989- cleanups related to removing flicker (though they probably weren't really needed -
 *	lost a refresh which WAS added to fix SPR#0872 but not problems seem to be created
 *	by getting rid of it again
 *	
 *	Revision 2.21  2001/09/12 14:53:30  lewis
 *	SPR#0987- Added conditional UseSecondaryHilight support (macos&win32 only so far). Redo
 *	Led_Region support for MacOS so now more like with PC (no operator conversion to macregion
 *	and auto constructs OSRegion except when called with region arg - in whcih case we dont OWN
 *	region and dont delete it on DTOR
 *	
 *	Revision 2.20  2001/09/12 03:25:13  lewis
 *	SPR#1018- support new CWPro7 (build with Carbon on required a few more changes)
 *	
 *	Revision 2.19  2001/08/28 18:43:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.18  2001/07/20 00:58:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.17  2001/07/20 00:17:59  lewis
 *	wrap non-CARBON access to CARBON accessors with #if ACCESSOR_CALLS_ARE_FUNCTIONS
 *	
 *	Revision 2.16  2001/07/19 02:21:45  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.15  2001/05/03 22:06:25  lewis
 *	vaguely related to SPR#0887- made fTickCountAtLastBlink; a float and used
 *	Led_GetTickCount instead of ::TickCount()
 *	
 *	Revision 2.14  2001/05/01 16:07:44  lewis
 *	fixed bad names - Get/SetCurClickClickCount to just one CLICK
 *	
 *	Revision 2.13  2001/04/17 23:05:30  lewis
 *	Cleanups related to SPR#0871
 *	
 *	Revision 2.12  2001/04/17 22:48:21  lewis
 *	SPR#0871- Moved a bunch of code from Led_MacOS and Led_Win32 to TextInteractor. Code
 *	was all relating to TextInteractor::GetCurClickClickCount () etc. Also - improved logic
 *	for double click detection (now shared across platforms).
 *	
 *	Revision 2.11  2001/04/17 21:37:53  lewis
 *	SPR#0872 - workaround Mac-specific selection update/cursor tracking bug with kludge
 *	
 *	Revision 2.9  2000/10/06 03:34:11  lewis
 *	moved code/datatypes for controlling show/hide display of scrollbars for Led_Win32/Led_MacOS to -
 *	mostly - TextInteractor. Still some logic in the OS-specific wrappers. Unified their APIs -
 *	supporting autohide of sbars on both (though mac maybe still slightly buggy)
 *	
 *	Revision 2.8  2000/10/05 17:26:47  lewis
 *	Changed API for SetHasScrollBar to take enum rather than bool - and supported auto-hide/show scrollbars
 *	
 *	Revision 2.7  2000/09/29 18:03:41  lewis
 *	Added DOCCOMMENTS
 *	
 *	Revision 2.6  2000/09/29 00:17:18  lewis
 *	rewrite the AlignTextEditRects() virtual to call parameterized AlignTextEditRects_ helper.
 *	Lose public SBARSIZE1 and put all local to AlignTextEditRects_ (). Make so controllable
 *	if you assume growbox and if oyu want the sbar to overlap the right edge (for when in windows -
 *	but no good otherwise - like embedded in text box in web page in NetLEdIt)
 *	
 *	Revision 2.5  2000/09/26 13:56:46  lewis
 *	Added UpdateClickCount () support. sDragAnchor becomes fDragAnechor. HandleAdjustCursor()
 *	support. Fix scrollbar mouse click support
 *	
 *	Revision 2.4  2000/09/25 21:57:43  lewis
 *	Moved most of the draw/layout/scrollbar and (except D&D) mouse handling code from
 *	Led_PP to Led_MacOS
 *	
 *	Revision 2.3  2000/09/25 00:43:33  lewis
 *	Move alot of code from Led_PPView to Led_MacOS_Helper<> template (key handling,
 *	tablet handling). Soon will move alot more
 *	
 *	Revision 2.2  2000/09/23 20:59:06  lewis
 *	Added GetUseGrafPort/Set support
 *	
 *	Revision 2.1  2000/09/22 21:35:34  lewis
 *	*** empty log message ***
 *	
 *	
 *
 *
 *
 */


#if		qMacOS
#include	<ControlDefinitions.h>
#endif

#include	"TextInteractor.h"





#if		qLedUsesNamespaces
namespace	Led {
#endif






/*
@CLASS:			Led_MacOS_Helper<BASE_INTERACTOR>
@BASES:			BASE_INTERACTOR = @'TextInteractor'
@DESCRIPTION:	<p>This helper template can be used to integrate a Led @'TextInteractor' class (such as @'WordProcessor') with the
			Mac OS. If you are using PowerPlant, you may want to use @'Led_PPView' directly.
				<p>Subclassers must override the pure virtual method:<br>
			<ul>
				<li>@'Led_MacOS_Helper<BASE_INTERACTOR>::SetupCurrentGrafPort'</li>
			</ul>
				</p>
				<p>And they will have to hook themselves into the Macintosh event-handling mechanism, and arrange to call<br>
			<ul>
				<li>@'Led_MacOS_Helper<BASE_INTERACTOR>::HandleActivateEvent'</li>
				<li>@'Led_MacOS_Helper<BASE_INTERACTOR>::HandleDeactivateEvent'</li>Led_MacOS_Helper<BASE_INTERACTOR>::HandleKeyEvent
				<li>@'Led_MacOS_Helper<BASE_INTERACTOR>::HandleKeyEvent'</li>
			</ul>
			and many more. Document later, as time permits.
				</p>
*/
template	<typename	BASE_INTERACTOR = TextInteractor>	class	Led_MacOS_Helper :
	public virtual BASE_INTERACTOR
{
	public:
		typedef	TextInteractor::UpdateMode	UpdateMode;
		typedef	MarkerOwner::UpdateInfo		UpdateInfo;
	private:
		typedef	BASE_INTERACTOR	inherited;

	public:
		Led_MacOS_Helper ();
		virtual ~Led_MacOS_Helper ();

	public:
		nonvirtual	GrafPtr	GetUseGrafPort () const;
		nonvirtual	void	SetUseGrafPort (GrafPtr grafPort);
	private:
		GrafPtr	fUseGrafPort;



	public:
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::SetupCurrentGrafPort
		@DESCRIPTION:	<p>This pure-virtual method must be overriden in subclasses.</p>
		*/
		virtual	void	SetupCurrentGrafPort () const = 0;



	//TextImager overrides
	public:
		override	Led_Tablet	AcquireTablet () const;
		override	void		ReleaseTablet (Led_Tablet tablet) const;
	protected:
		nonvirtual	size_t	GetTabletAcquireCount () const;
	private:
		mutable	size_t			fAcquireCount;
		mutable	Led_Tablet_*	fUseTablet;
	

	//TextInteractor overrides
	protected:
		override	void	RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const;
		override	bool	QueryInputKeyStrokesPending () const;
		override	bool	OnCopyCommand_Before ();


	// Window/scrollbar layout
	public:
		nonvirtual	Rect	GetWindowMargin () const;
		nonvirtual	void	SetWindowMargin (const Rect& windowMargin);
	private:
		Rect	fWindowMargin;
		static	const	Rect	kDefaultMargin;
	public:
		nonvirtual	Led_Rect	GetWindowFrameRect () const;
		nonvirtual	void		SetWindowFrameRect (const Led_Rect& windowFrameRect);
	private:
		Led_Rect	fWindowFrameRect;
	protected:
		virtual 	void	AlignTextEditRects ();
		nonvirtual	void	AlignTextEditRects_ (bool alwaysLeaveSpaceForSizeBox, bool scrollBarsOverlapOwningFrame, bool vSBarShown, bool hSBarShown);

	protected:
		nonvirtual	bool	GetAlwaysLeaveSpaceForSizeBox () const;
		nonvirtual	void	SetAlwaysLeaveSpaceForSizeBox (bool alwaysLeaveSpaceForSizeBox);
		nonvirtual	bool	GetScrollBarsOverlapOwningFrame () const;
		nonvirtual	void	SetScrollBarsOverlapOwningFrame (bool scrollBarsOverlapOwningFrame);
	private:
		bool	fAlwaysLeaveSpaceForSizeBox;
		bool	fScrollBarsOverlapOwningFrame;

	public:
		virtual	void	HandleActivateEvent ();
		virtual	void	HandleDeactivateEvent ();

	public:
		virtual	void	HandleIdleEvent ();


	// Key handling
	public:
		static	const UInt8		char_LeftArrow		 = 0x1C;
		static	const UInt8		char_RightArrow		 = 0x1D;
		static	const UInt8		char_UpArrow		 = 0x1E;
		static	const UInt8		char_DownArrow		 = 0x1F;
		static	const UInt8		char_Home			 = 0x01;
		static	const UInt8		char_End			 = 0x04;
		static	const UInt8		char_PageUp			 = 0x0B;
		static	const UInt8		char_PageDown		 = 0x0C;
		static	const UInt8		char_Enter			 = 0x03;
		static	const UInt8		char_Tab			 = 0x09;
		static	const UInt8		char_Return			 = 0x0D;
		static	const UInt8		char_FirstPrinting	 = 0x20;	// Nonprinting if less than
		static	const UInt8		char_Backspace		 = 0x08;
		static	const UInt8		char_FwdDelete		 = 0x7F;	// Only nonprinting above $20
		static	const UInt8		char_Clear			 = 0x1B;	// Same as Escape
		static	const UInt32	vkey_Clear			 = 0x00004700;

		nonvirtual	bool	IsTEDeleteKey (unsigned short	inKey);
		nonvirtual	bool	IsTECursorKey (unsigned short	inKey);
		nonvirtual	bool	IsExtraEditKey (unsigned short	inKey);
		nonvirtual	bool	IsPrintingChar (unsigned short	inChar);

	public:
		virtual	bool	HandleKeyEvent (const EventRecord& inKeyEvent);
	protected:
		virtual	void	HandleCursorKeyPress (char theChar, bool optionPressed, bool shiftPressed, bool commandPressed);


	public:
		virtual	void	Handle_Draw (Led_Region updateRgn);


	public:
		nonvirtual	void	BlinkCaretIfNeeded ();	// just allow public access
													// via idle task mechanism or whatever...
	protected:
		nonvirtual	void	DrawCaret_ ();
		nonvirtual	void	DrawCaret_ (bool on);
	protected:
		bool		fLastDrawnOn;
	private:
		float		fTickCountAtLastBlink;

	// Scrollbar support
	public:
		typedef	TextInteractor::ScrollBarType	ScrollBarType;	// redundant typedef to keep compiler happy...LGP 2000-10-05-
		typedef	TextInteractor::VHSelect		VHSelect;		// redundant typedef to keep compiler happy...LGP 2000-10-05?Needed on mac?

		override	void	SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType);

	protected:
		virtual		bool	HandleIfScrollBarClick (const EventRecord& inMouseDownEvent);
		virtual		void	HandleScrollBarClick (ControlRef scrollBar, short partCode, Led_Point where);
	protected:
		nonvirtual	void	ActivateScrollBars (bool activate);
		override	void	UpdateScrollBars ();
	private:
		nonvirtual	void	UpdateScrollBarsParameters (ControlRef scrollBar, int value, int maxValue);
	protected:
		nonvirtual	void	DoVertScroll (short whichPart);
		nonvirtual	void	DoHorzScroll (short whichPart);
		static	pascal	void	MyScrollProc (ControlHandle /*scrollBar*/, short ctlPart);
		
	private:
		static	bool	BarShown (ControlRef scrollBar);
	private:
		static	Led_MacOS_Helper<BASE_INTERACTOR>*	sCurrentlyTrackingThisFrame;
	private:
		ControlRef		fScrollBars[2];		// maybe we can lose flag??? Just keep this?
		int				fScrollBarScale_V;


	public:
		override	void	HandleAdjustCursorEvent (const EventRecord& inEvent);


	/*
	 *	Mouse clicks.
	 */
	public:
		virtual		void	HandleMouseDownEvent (const EventRecord& inMouseDownEvent);

	protected:
		size_t		fDragAnchor;		// only used while dragging mouse
	protected:
		virtual		void	HandleTrackDragSelectInText (const EventRecord& inMouseDownEvent);
};









/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


//	class	Led_MacOS_Helper<BASE_INTERACTOR>
	template	<typename	BASE_INTERACTOR>
			inline	Led_MacOS_Helper<BASE_INTERACTOR>::Led_MacOS_Helper ():
				fUseGrafPort (NULL),
				fAcquireCount (0),
				fUseTablet (NULL),
				fWindowMargin (kDefaultMargin),
				fWindowFrameRect (Led_Rect (0, 0, 0, 0)),
				fAlwaysLeaveSpaceForSizeBox (true),
				fScrollBarsOverlapOwningFrame (true),
				fLastDrawnOn (false),
				fTickCountAtLastBlink (0.0f),
				//fScrollBars (),
				fScrollBarScale_V (1),
				fDragAnchor (0)
			{
				fScrollBars[v] = NULL;
				fScrollBars[h] = NULL;
			}
	template	<typename	BASE_INTERACTOR>
		Led_MacOS_Helper<BASE_INTERACTOR>::~Led_MacOS_Helper ()
			{
				Led_Assert (fAcquireCount == 0);
				Led_Assert (fUseTablet == NULL);
				if (fScrollBars[v] != NULL) {
					::DisposeControl (fScrollBars[v]);
				}
				if (fScrollBars[h] != NULL) {
					::DisposeControl (fScrollBars[h]);
				}
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::GetUseGrafPort
		@DESCRIPTION:	<p>Get the grafport associated with this editor instance. This must be specified (@'Led_MacOS_Helper<BASE_INTERACTOR>::SetUseGrafPort')
					early on, before any drawing, or sizing etc, can be performed.</p>
		*/
			inline	GrafPtr	Led_MacOS_Helper<BASE_INTERACTOR>::GetUseGrafPort () const
				{
					return fUseGrafPort;
				}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::SetUseGrafPort
		@DESCRIPTION:	<p>See @'Led_MacOS_Helper<BASE_INTERACTOR>::GetUseGrafPort'.</p>
		*/
			inline	void	Led_MacOS_Helper<BASE_INTERACTOR>::SetUseGrafPort (GrafPtr grafPort)
				{
					if (fUseGrafPort != grafPort) {
						fUseGrafPort = grafPort;
					}
				}
	template	<typename	BASE_INTERACTOR>
		Led_Tablet	Led_MacOS_Helper<BASE_INTERACTOR>::AcquireTablet () const
			{
				if (fAcquireCount == 0) {
					if (GetUseGrafPort () == NULL) {
						throw NoTabletAvailable ();
					}
				}
				fAcquireCount++;
				Led_Assert (fAcquireCount < 100);	// not really a requirement - but hard to see how this could happen in LEGIT usage...
													// almost certainly a bug...

				Led_Assert ((fUseTablet == NULL) == (fAcquireCount == 1));	// each should be true iff the other is true
				if (fUseTablet == NULL) {
					fUseTablet = new Led_Tablet_ (GetUseGrafPort ());
				}
				if (GetTabletAcquireCount () == 1) {
					const_cast<Led_MacOS_Helper<BASE_INTERACTOR>*>(this)->SetupCurrentGrafPort ();
				}

				Led_EnsureNotNil (fUseTablet);
				return (fUseTablet);
			}
	template	<typename	BASE_INTERACTOR>
		void		Led_MacOS_Helper<BASE_INTERACTOR>::ReleaseTablet (Led_Tablet /*tablet*/) const
			{
				Led_Assert (fAcquireCount > 0);
				fAcquireCount--;
				if (fAcquireCount == 0) {
					delete fUseTablet; fUseTablet = NULL;
				}
			}
	template	<typename	BASE_INTERACTOR>
		inline	size_t	Led_MacOS_Helper<BASE_INTERACTOR>::GetTabletAcquireCount () const
			{
				return fAcquireCount;
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const
			{
				switch (RealUpdateMode (updateMode)) {
					case	eDelayedUpdate: {
						if (not windowRectArea.IsEmpty () and GetUseGrafPort () != NULL) {
							Rect	tmp	=	AsQDRect (windowRectArea);
							#if		TARGET_CARBON
								::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &tmp);
							#else
								SetupCurrentGrafPort ();
								::InvalRect (&tmp);
							#endif
						}
					}
					break;
					case	eImmediateUpdate: {
						if (not windowRectArea.IsEmpty () and GetUseGrafPort () != NULL) {
							Rect	tmp	=	AsQDRect (windowRectArea);
							#if		TARGET_CARBON
								::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &tmp);
							#else
								SetupCurrentGrafPort ();
								::InvalRect (&tmp);
							#endif
							UpdateWindowRect_ (windowRectArea);
						}
					}
					break;
				}
			}
	template	<typename	BASE_INTERACTOR>
		bool	Led_MacOS_Helper<BASE_INTERACTOR>::QueryInputKeyStrokesPending () const
			{
				EventRecord	eventRecord;
				#if		TARGET_CARBON
					return (::EventAvail (keyDownMask, &eventRecord));
				#else
					return (::OSEventAvail (keyDownMask, &eventRecord));
				#endif
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::OnCopyCommand_Before
		@ACCESS:		protected
		@DESCRIPTION:	<p>Hook @'TextInteractor::OnCopyCommand_Before' to zero out scrap.</p>
		*/
		bool	Led_MacOS_Helper<BASE_INTERACTOR>::OnCopyCommand_Before ()
			{
				#if		TARGET_CARBON
					Led_ThrowIfOSStatus (::ClearCurrentScrap ());
				#else
					Led_ThrowOSErr (::ZeroScrap ());
				#endif
				return inherited::OnCopyCommand_Before ();
			}
	template	<typename	BASE_INTERACTOR>
		inline	Rect	Led_MacOS_Helper<BASE_INTERACTOR>::GetWindowMargin () const
			{
				return fWindowMargin;
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::SetWindowMargin
		@DESCRIPTION:	<p>Sets the margins around the window frame rect (see @'Led_MacOS_Helper<BASE_INTERACTOR>::GetWindowFrameRect')
					inside which the actual window-rect (@'TextImager::GetWindowRect') will go.</p>
						<p>NB: This rect is not interpretted as a normal rectable. Its top/left/bottom/right are just interpreted as offets
					in from the edge of the window frame rect to place the window rect in @'Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects'.</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::SetWindowMargin (const Rect& windowMargin)
			{
				if (fWindowMargin != windowMargin) {
					fWindowMargin = windowMargin;
					AlignTextEditRects ();
				}
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::GetWindowFrameRect
		@DESCRIPTION:	<p>The window frame rect is the bounds on the current grafport (@'Led_MacOS_Helper<BASE_INTERACTOR>::GetUseGrafPort')
					in which we place the scrollbars, and windowrect (@'TextImager::GetWindowRect'). These are layed out within the frame rect
					each time the frame rect changes by the @'Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects' method.</p>
		*/
		inline	Led_Rect	Led_MacOS_Helper<BASE_INTERACTOR>::GetWindowFrameRect () const
			{
				return fWindowFrameRect;
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::SetWindowFrameRect
		@DESCRIPTION:	<p>See @'Led_MacOS_Helper<BASE_INTERACTOR>::GetWindowFrameRect'.</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::SetWindowFrameRect (const Led_Rect& windowFrameRect)
			{
				if (fWindowFrameRect != windowFrameRect) {
					fWindowFrameRect = windowFrameRect;
					AlignTextEditRects ();
				}
			}
	template	<typename	BASE_INTERACTOR>
		static	const	Rect	Led_MacOS_Helper<BASE_INTERACTOR>::kDefaultMargin	=	{ 2, 4, 0, 0 };
//		static	const	Rect	Led_MacOS_Helper<BASE_INTERACTOR>::kDefaultMargin	=	{50, 50, 50, 50  };	// tmp hack to debug
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects
		@DESCRIPTION:	<p>Lays out the scrollbars, and specifies the window-rect (@'TextImager::GetWindowRect') based on the
					window frame rect (@'Led_MacOS_Helper<BASE_INTERACTOR>::GetWindowFrameRect), and its own internal algorithm.
					A default algorithm is supplied by @'Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects_' with enough parameters
					that you can probably get what you want by just subclassing this routine and
					calling @'Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects_'
					with different arguements.</p>
						<p>This routine should generally <em>not</em> be overridden. Instead, call
					@'Led_MacOS_Helper<BASE_INTERACTOR>::SetAlwaysLeaveSpaceForSizeBox' or
					@'Led_MacOS_Helper<BASE_INTERACTOR>::SetScrollBarsOverlapOwningFrame' to control the behavior of the scrollbar layout.</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects ()
			{
				AlignTextEditRects_ (GetAlwaysLeaveSpaceForSizeBox (), GetScrollBarsOverlapOwningFrame (), BarShown (fScrollBars[v]), BarShown (fScrollBars[h]));
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects_
		@DESCRIPTION:	<p>Helper to implement the @'Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects_' algorithm.
					This method takes a 'alwaysLeaveSpaceForSizeBox' parameter which you can use to force the editor to leave room for
					a size box when laying out the scrollbar. This is useful if you are putting the editor into a window that has a grow-box,
					and where the window takes up the entire content area.</p>
						<p>The 'scrollBarsOverlapOwningFrame' is for a similar situation. The Mac window layout has a quirk - where scrollbars
					are supposed to be layed out so that one pixel of their edge overlaps with the edge of the window - sharing a single line.
					Actaully - the Mac's UI has changed since then - but they still define things and lay them out based on this hisorical quirk.
					So if your editor is going to take over a window entirely - you probably wnat to specify true for this, and false otherwise..</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects_ (bool alwaysLeaveSpaceForSizeBox, bool scrollBarsOverlapOwningFrame, bool vSBarShown, bool hSBarShown)
			{
				Rect	textFrame	= AsQDRect (GetWindowFrameRect ());
				bool	leaveSpaceForSizeBox	=	alwaysLeaveSpaceForSizeBox;
				if (not leaveSpaceForSizeBox) {
					// regardless of external flag, if both sbars, then leave space.
					leaveSpaceForSizeBox = (vSBarShown and hSBarShown);
				}
				const	Led_Distance	SBARSIZE	=	16;
				const	Led_Distance	SBARSIZE1	=	SBARSIZE-1;
				if (vSBarShown) {
					Rect	scrollBarRect = textFrame;
					scrollBarRect.left = scrollBarRect.right - SBARSIZE;
					if (scrollBarsOverlapOwningFrame) {
						scrollBarRect.left += 1;
						scrollBarRect.right += 1;
						scrollBarRect.top -= 1;
					}
					if (leaveSpaceForSizeBox) {
						scrollBarRect.bottom -= (SBARSIZE1-1);
					}
					//  a bit of trickery to avoid this move redrawing the scrollbars before I'm ready...
					Led_MacPortAndClipRegionEtcSaver	saver;
					SetupCurrentGrafPort ();
					#if		TARGET_CARBON
						{
							Rect	controlRect;
							::GetControlBounds (fScrollBars[v], &controlRect);
							::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &controlRect);
						}
					#else
						::InvalRect (&(*fScrollBars[v])->contrlRect);
					#endif
					#if		TARGET_CARBON
						Boolean oldContrlVis = IsControlVisible (fScrollBars[v]);
						::SetControlVisibility (fScrollBars[v], false, false);
					#else
						UInt8 oldContrlVis = (*fScrollBars[v])->contrlVis;
						(*fScrollBars[v])->contrlVis = 0;
					#endif
						::MoveControl (fScrollBars[v], scrollBarRect.left, scrollBarRect.top);
						::SizeControl (fScrollBars[v], scrollBarRect.right - scrollBarRect.left, scrollBarRect.bottom - scrollBarRect.top);
					#if		TARGET_CARBON
						::SetControlVisibility (fScrollBars[v], oldContrlVis, false);
					#else
						(*fScrollBars[v])->contrlVis = oldContrlVis;
					#endif
					#if		TARGET_CARBON
						{
							Rect	controlRect;
							::GetControlBounds (fScrollBars[v], &controlRect);
							::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &controlRect);
						}
					#else
						::InvalRect (&(*fScrollBars[v])->contrlRect);
					#endif
				}
				if (hSBarShown) {
					Rect	scrollBarRect = textFrame;
					scrollBarRect.top = scrollBarRect.bottom - SBARSIZE;
					if (scrollBarsOverlapOwningFrame) {
						scrollBarRect.top += 1;
						scrollBarRect.bottom += 1;
						scrollBarRect.left -= 1;
					}
					if (leaveSpaceForSizeBox) {
						scrollBarRect.right -= (SBARSIZE1-1);
					}
					//  a bit of trickery to avoid this move redrawing the scrollbars before I'm ready...
					Led_MacPortAndClipRegionEtcSaver	saver;
					SetupCurrentGrafPort ();
					#if		TARGET_CARBON
						{
							Rect	controlRect;
							::GetControlBounds (fScrollBars[h], &controlRect);
							::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &controlRect);
						}
					#else
						::InvalRect (&(*fScrollBars[h])->contrlRect);
					#endif
					#if		TARGET_CARBON
						Boolean oldContrlVis = IsControlVisible (fScrollBars[h]);
						::SetControlVisibility (fScrollBars[h], false, false);
					#else
						UInt8 oldContrlVis = (*fScrollBars[h])->contrlVis;
						(*fScrollBars[h])->contrlVis = 0;
					#endif
						::MoveControl (fScrollBars[h], scrollBarRect.left, scrollBarRect.top);
						::SizeControl (fScrollBars[h], scrollBarRect.right - scrollBarRect.left, scrollBarRect.bottom - scrollBarRect.top);
					#if		TARGET_CARBON
						::SetControlVisibility (fScrollBars[h], oldContrlVis, false);
					#else
						(*fScrollBars[h])->contrlVis = oldContrlVis;
					#endif
					#if		TARGET_CARBON
						{
							Rect	controlRect;
							::GetControlBounds (fScrollBars[h], &controlRect);
							::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &controlRect);
						}
					#else
						::InvalRect (&(*fScrollBars[h])->contrlRect);
					#endif
				}

				if (vSBarShown) {
					textFrame.right -= SBARSIZE;
					if (scrollBarsOverlapOwningFrame) {
						textFrame.right += 1;
					}
				}
				if (hSBarShown) {
					textFrame.bottom -= SBARSIZE;
					if (scrollBarsOverlapOwningFrame) {
						textFrame.bottom += 1;
					}
				}
			
				if (textFrame.right < textFrame.left) {
					textFrame.right = textFrame.left;
				}
				if (textFrame.bottom < textFrame.top) {
					textFrame.bottom = textFrame.top;
				}

				textFrame.top += fWindowMargin.top;
				textFrame.left += fWindowMargin.left;
				textFrame.bottom -= fWindowMargin.bottom;
				textFrame.right -= fWindowMargin.right;

				// Don't make rect size negative.
				textFrame.bottom = Led_Max (textFrame.bottom, textFrame.top);
				textFrame.right = Led_Max (textFrame.right, textFrame.left);

				SetWindowRect (AsLedRect (textFrame));
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::GetAlwaysLeaveSpaceForSizeBox
		@DESCRIPTION:	<p>The 'alwaysLeaveSpaceForSizeBox' property allows you to force the editor to leave room for
					a size box when laying out the scrollbar. This is useful if you are putting the editor into a window that has a grow-box,
					and where the window takes up the entire content area.</p>
						<p>The property is used from @'Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects'.</p>
						<p>This value defaults to true (which works best for when a Led view is embedded in a window).</p>
		*/
		inline	bool	Led_MacOS_Helper<BASE_INTERACTOR>::GetAlwaysLeaveSpaceForSizeBox () const
			{
				return fAlwaysLeaveSpaceForSizeBox;
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::SetAlwaysLeaveSpaceForSizeBox
		@DESCRIPTION:	<p>See also @'Led_MacOS_Helper<BASE_INTERACTOR>::GetAlwaysLeaveSpaceForSizeBox'</p>
		*/
		inline	void	Led_MacOS_Helper<BASE_INTERACTOR>::SetAlwaysLeaveSpaceForSizeBox (bool alwaysLeaveSpaceForSizeBox)
			{
				fAlwaysLeaveSpaceForSizeBox = alwaysLeaveSpaceForSizeBox;
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::GetScrollBarsOverlapOwningFrame
		@DESCRIPTION:	<p>The 'scrollBarsOverlapOwningFrame' is similar to @'Led_MacOS_Helper<BASE_INTERACTOR>::GetAlwaysLeaveSpaceForSizeBox'.
					The Mac window layout has a quirk - where scrollbars
					are supposed to be layed out so that one pixel of their edge overlaps with the edge of the window - sharing a single line.
					Actaully - the Mac's UI has changed since then - but they still define things and lay them out based on this hisorical quirk.
					So if your editor is going to take over a window entirely - you probably wnat to specify true for this, and false otherwise.</p>
						<p>The property is used from @'Led_MacOS_Helper<BASE_INTERACTOR>::AlignTextEditRects'.</p>
						<p>This value defaults to true (which works best for when a Led view is embedded in a window).</p>
		*/
		inline	bool	Led_MacOS_Helper<BASE_INTERACTOR>::GetScrollBarsOverlapOwningFrame () const
			{
				return fScrollBarsOverlapOwningFrame;
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::SetScrollBarsOverlapOwningFrame
		@DESCRIPTION:	<p>See also @'Led_MacOS_Helper<BASE_INTERACTOR>::GetScrollBarsOverlapOwningFrame'</p>
		*/
		inline	void	Led_MacOS_Helper<BASE_INTERACTOR>::SetScrollBarsOverlapOwningFrame (bool scrollBarsOverlapOwningFrame)
			{
				fScrollBarsOverlapOwningFrame = scrollBarsOverlapOwningFrame;
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::HandleActivateEvent
		@DESCRIPTION:	<p>Call this method when this edit view becomes the input focus (so it will start blinking the caret, etc)
					See also @'Led_MacOS_Helper<BASE_INTERACTOR>::HandleDeactivateEvent'.</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleActivateEvent ()
			{
				ActivateScrollBars (true);
				SetCaretShown (true);
				SetSelectionShown (true);
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::HandleDeactivateEvent
		@DESCRIPTION:	<p>Call this method when this edit view loese the input focus (so it will stop blinking the caret, etc).
					See also @'Led_MacOS_Helper<BASE_INTERACTOR>::HandleActivateEvent'.</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleDeactivateEvent ()
			{
				ActivateScrollBars (false);
				SetCaretShown (false);
				SetSelectionShown (false);
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::HandleIdleEvent
		@DESCRIPTION:	<p>Call this method when your applicaiton recieves idle events. This only needs to be called if you are the input
			focus, but can be safely called otherwise.</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleIdleEvent ()
			{
// NB: THIS CODE SHOULD BE REPLACED WITH USE OF THE LED 'IdleManager' support - SPR#1367
				BlinkCaretIfNeeded ();
			}
	template	<typename	BASE_INTERACTOR>
		inline	bool	Led_MacOS_Helper<BASE_INTERACTOR>::IsTEDeleteKey (unsigned short	inKey)
			{
				return ((inKey & charCodeMask) == char_Backspace);
			}
	template	<typename	BASE_INTERACTOR>
		inline	bool	Led_MacOS_Helper<BASE_INTERACTOR>::IsTECursorKey (unsigned short	inKey)
			{
				Boolean	isCursor = false;

				switch (inKey & charCodeMask) {
				
					case char_LeftArrow:
					case char_RightArrow:
					case char_UpArrow:
					case char_DownArrow:
						isCursor = true;
						break;
				}
				
				return isCursor;
			}
	template	<typename	BASE_INTERACTOR>
		inline	bool	Led_MacOS_Helper<BASE_INTERACTOR>::IsExtraEditKey (unsigned short	inKey)
			{
				bool	isExtraEdit = false;
				
				switch (inKey & charCodeMask) {
				
					case char_Home:
					case char_End:
					case char_PageUp:
					case char_PageDown:
					case char_FwdDelete:
						isExtraEdit = true;
						break;
						
					case char_Clear:
						isExtraEdit = (inKey & keyCodeMask) == vkey_Clear;
						break;
				}
				
				return isExtraEdit;
			}
	template	<typename	BASE_INTERACTOR>
		inline	bool	Led_MacOS_Helper<BASE_INTERACTOR>::IsPrintingChar (unsigned short	inChar)
			{
				return (inChar >= char_FirstPrinting) and (inChar != char_FwdDelete);
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::HandleKeyEvent
		@DESCRIPTION:	<p>Call this method when in response to Macintosh Key Event.</p>
		*/
		bool	Led_MacOS_Helper<BASE_INTERACTOR>::HandleKeyEvent (const EventRecord& inKeyEvent)
		{
			bool	handleHere	=	false;
			short	theKey = inKeyEvent.message & charCodeMask;
			
			if (not (inKeyEvent.modifiers & cmdKey)) {	// Pass up when the command key is down				
				unsigned short		theKey		 = (unsigned short) inKeyEvent.message;
				unsigned short		theChar		 = (unsigned short) (theKey & charCodeMask);
				if (IsTEDeleteKey(theKey)) {
					handleHere = true;
				}
				else if (IsTECursorKey(theKey)) {
					handleHere = true;
				}
				else if (IsExtraEditKey(theKey)) {
					handleHere = true;			
				}
				else if (IsPrintingChar(theChar)) {
					handleHere = true;
				}
			}

			if (not handleHere) {
				// for alot of cases the PP code seems to return pass-up. When this is not
				// what I want. Not sure if I'm using their code wrong, or if its just
				// wrong. Anyhow - work around for now...
				// oops - alot of it is the if (cmdKey test above :-) - well - this is good enuf for now...
				// LGP 960316
				if (theKey == char_Return or theKey == char_Tab) {
					handleHere = true;
				}
				if (inKeyEvent.modifiers & cmdKey) {
					switch (theKey) {
						case	char_LeftArrow:
						case	char_RightArrow:
						case	char_UpArrow:
						case	char_DownArrow:
						case	char_Home:
						case	char_End:
						case	char_PageUp:
						case	char_PageDown: {
							handleHere = true;
						}
						break;
					}
				}
			}

			if (handleHere) {
				// Bob Swerdlow says you are supposed todo this - seems silly to me... LGP 950212...
				::ObscureCursor ();
			
				char	theChar = theKey;
				Led_Assert (GetSelectionEnd () <= GetEnd ());
			
				/*
				 *	Behavior here as listed in Mac. Human Interface Guidelines - See Led SPR#0122
				 */
				bool	optionPressed	=	!!(inKeyEvent.modifiers&optionKey);
				bool	shiftPressed	=	!!(inKeyEvent.modifiers&shiftKey);
				bool	commandPressed	=	!!(inKeyEvent.modifiers&cmdKey);
				bool	controlPressed	=	!!(inKeyEvent.modifiers&controlKey);
			
				/*
				 *	Be sure codes for arrow keys don't conflict with second bytes. If so - we might
				 *	get confused on users typing dbcs characters! Check no conlfict with Led_BYTE too.
				 *	Any overlap could cause confusion.
				 */
				#if		qMultiByteCharacters
					Led_Assert (not Led_IsLeadByte (char_LeftArrow));
					Led_Assert (not Led_IsValidSecondByte (char_LeftArrow));
			
					Led_Assert (not Led_IsLeadByte (char_RightArrow));
					Led_Assert (not Led_IsValidSecondByte (char_RightArrow));
			
					Led_Assert (not Led_IsLeadByte (char_UpArrow));
					Led_Assert (not Led_IsValidSecondByte (char_UpArrow));
			
					Led_Assert (not Led_IsLeadByte (char_DownArrow));
					Led_Assert (not Led_IsValidSecondByte (char_DownArrow));

					Led_Assert (not Led_IsLeadByte (char_Home));
					Led_Assert (not Led_IsValidSecondByte (char_Home));
			
					Led_Assert (not Led_IsLeadByte (char_End));
					Led_Assert (not Led_IsValidSecondByte (char_End));
			
					Led_Assert (not Led_IsLeadByte (char_PageUp));
					Led_Assert (not Led_IsValidSecondByte (char_PageUp));
			
					Led_Assert (not Led_IsLeadByte (char_PageDown));
					Led_Assert (not Led_IsValidSecondByte (char_PageDown));
			
					Led_Assert (not Led_IsLeadByte (char_FwdDelete));
					Led_Assert (not Led_IsValidSecondByte (char_FwdDelete));
				#endif

				switch (theChar) {
					case	char_LeftArrow:
					case	char_RightArrow:
					case	char_UpArrow:
					case	char_DownArrow:
					case	char_Home:
					case	char_End:
					case	char_PageUp:
					case	char_PageDown: {
						HandleCursorKeyPress (theChar, optionPressed, shiftPressed, commandPressed);
					}
					break;

					case	char_FwdDelete: {
						DoSingleCharCursorEdit (eCursorForward, eCursorByChar, eCursorDestroying, qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate);
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
					}
					break;

					case	'\b':
					default: {
						if (theChar == '\r') {
							theChar = '\n';
						}
						OnTypedNormalCharacter (theChar, optionPressed, shiftPressed, commandPressed, controlPressed, false);
					}
					break;
				}
			}
			return handleHere;
		}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::HandleCursorKeyPress
		@DESCRIPTION:	<p>Helper for handling key events.</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleCursorKeyPress (char theChar, bool optionPressed, bool shiftPressed, bool commandPressed)
			{
				BreakInGroupedCommands ();
				
				/*
				 *	For any of these cursoring keys, find the appropriate action/unit/dirction, and then
				 *	apply it.
				 */
				CursorMovementDirection	dir;
				CursorMovementUnit		unit;
				CursorMovementAction	action	=	shiftPressed? eCursorExtendingSelection: eCursorMoving;
				switch (theChar) {
					case	char_LeftArrow: {
						dir		=	commandPressed? eCursorToStart: eCursorBack;
						unit	=	optionPressed? eCursorByWord: eCursorByChar;
						if (commandPressed) {
							unit = eCursorByRow;	// NB: ambiguity in case both optionPressed AND commandPressed
						}
					}
					break;
					case	char_RightArrow: {
						dir		=	commandPressed? eCursorToEnd: eCursorForward;
						unit	=	optionPressed? eCursorByWord: eCursorByChar;
						if (commandPressed) {
							unit = eCursorByRow;	// NB: ambiguity in case both optionPressed AND commandPressed
						}
					}
					break;
					case	char_UpArrow: {
						dir		=	(commandPressed or optionPressed)? eCursorToStart: eCursorBack;
						unit	=	optionPressed? eCursorByLine: eCursorByRow;
						if (commandPressed) {
							unit = eCursorByWindow;	// NB: ambiguity in case both optionPressed AND commandPressed
						}
					}
					break;
					case	char_DownArrow: {
						dir		=	(commandPressed or optionPressed)? eCursorToEnd: eCursorForward;
						unit	=	optionPressed? eCursorByLine: eCursorByRow;
						if (commandPressed) {
							unit = eCursorByWindow;	// NB: ambiguity in case both optionPressed AND commandPressed
						}
					}
					break;
					case	char_Home: {
						dir		=	eCursorToStart;
						unit	=	commandPressed? eCursorByBuffer: eCursorByRow;
					}
					break;
					case	char_End: {
						dir		=	eCursorToEnd;
						unit	=	commandPressed? eCursorByBuffer: eCursorByRow;
					}
					break;
					case	char_PageUp: {
			// NB: this isn't QUITE right for pageup - with differing height rows!!!!
						ScrollByIfRoom (-(int)GetTotalRowsInWindow ());
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return;
					}
					break;
					case	char_PageDown: {
						ScrollByIfRoom (GetTotalRowsInWindow ());
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return;
					}
					break;
					default: {
						Led_Assert (false);
					}
					break;
				}
				DoSingleCharCursorEdit (dir, unit, action, qPeekForMoreCharsOnUserTyping? eDelayedUpdate: eImmediateUpdate);
				#if		qPeekForMoreCharsOnUserTyping
					UpdateIfNoKeysPending ();
				#endif
			}
	template	<typename	BASE_INTERACTOR>
		/*
		@METHOD:		Led_MacOS_Helper<BASE_INTERACTOR>::Handle_Draw
		@DESCRIPTION:	<p>Call this method when in response to Macintosh update event. Be sure to pass in the updateRegion (in local / port coordinates).</p>
		*/
		void	Led_MacOS_Helper<BASE_INTERACTOR>::Handle_Draw (Led_Region updateRgn)
			{
				Led_RequireNotNil (updateRgn.GetOSRep ());

				// Don't throw out of a draw - simply eat it. Not sure how BEST to deal with running out of
				// memory on a draw. There is no GOOD way. For now, we just beep and forget it.
				// LGP 960523
				try {
					Led_Rect	windowRect	=	GetWindowRect ();
					Led_Rect	updateRect	=	updateRgn.GetBoundingRect ();

					Tablet_Acquirer	tablet_ (this);
					Led_Tablet		tablet	=	tablet_;
					
					tablet->SetPort ();

					// Draw sbar if needed
					#if		TARGET_CARBON
						{
							Rect	controlRect;
							::GetControlBounds (fScrollBars[v], &controlRect);
							if (BarShown (fScrollBars[v]) and Intersect (AsLedRect (controlRect), updateRgn)) {
								::Draw1Control (fScrollBars[v]);
							}
							::GetControlBounds (fScrollBars[h], &controlRect);
							if (BarShown (fScrollBars[h]) and Intersect (AsLedRect (controlRect), updateRgn)) {
								::Draw1Control (fScrollBars[h]);
							}
						}
					#else
						if (BarShown (fScrollBars[v]) and Intersect (AsLedRect ((*fScrollBars[v])->contrlRect), updateRgn)) {
							::Draw1Control (fScrollBars[v]);
						}
						if (BarShown (fScrollBars[h]) and Intersect (AsLedRect ((*fScrollBars[h])->contrlRect), updateRgn)) {
							::Draw1Control (fScrollBars[h]);
						}
					#endif


					// Compute intersection of update and window rects, and clip to that, as well as passing it as
					// the logical clipping rect (cuz sometimes otherwise text draws can overwrite the sbar??
					// Not sure why?? Maybe our calcs off some place???? Perhaps worth investigating?
					// LGP 960226
					{
						static	RgnHandle	wndRegion			=	::NewRgn ();
						static	RgnHandle	reallyUpdateRegion	=	::NewRgn ();
						static	RgnHandle	oldClip				=	::NewRgn ();
						static	RgnHandle	newClip				=	::NewRgn ();

						Led_ThrowIfNull (wndRegion);
						Led_ThrowIfNull (reallyUpdateRegion);
						Led_ThrowIfNull (oldClip);
						Led_ThrowIfNull (newClip);

						::SetRectRgn (wndRegion, (short)windowRect.left, (short)windowRect.top, (short)windowRect.right, (short)windowRect.bottom);
						::SectRgn (updateRgn.GetOSRep (), wndRegion, reallyUpdateRegion);
						if (not ::EmptyRgn (reallyUpdateRegion)) {
							try {
								::GetClip (oldClip);
								::SetClip (reallyUpdateRegion);
								::GetClip (newClip);
								::SectRgn (oldClip, newClip, newClip);
								::SetClip (newClip);
								TextImager*	imager	=	this;
								imager->Draw (Led_Region (reallyUpdateRegion).GetBoundingRect (), false);

								// Do logical clipping, and optionally draw margins...
								{
									Led_Rect	leftMargin		=	Led_Rect (windowRect.top-fWindowMargin.top, windowRect.left-fWindowMargin.left, windowRect.GetHeight () + fWindowMargin.top + fWindowMargin.bottom, fWindowMargin.left);
									Led_Rect	rightMargin		=	Led_Rect (windowRect.top-fWindowMargin.top, windowRect.right, windowRect.GetHeight () + fWindowMargin.top + fWindowMargin.bottom, fWindowMargin.right);
									Led_Rect	topMargin		=	Led_Rect (windowRect.top-fWindowMargin.top, windowRect.left, fWindowMargin.top, windowRect.GetWidth ());
									Led_Rect	bottomMargin	=	Led_Rect (windowRect.bottom, windowRect.left, fWindowMargin.bottom, windowRect.GetWidth ());
									if (Intersect (leftMargin, updateRect)) {
										Rect	wndRct	=	AsQDRect (leftMargin);
										::EraseRect (&wndRct);
									}
									if (Intersect (rightMargin, updateRect)) {
										Rect	wndRct	=	AsQDRect (rightMargin);
										::EraseRect (&wndRct);
									}
									if (Intersect (topMargin, updateRect)) {
										Rect	wndRct	=	AsQDRect (topMargin);
										::EraseRect (&wndRct);
									}
									if (Intersect (bottomMargin, updateRect)) {
										Rect	wndRct	=	AsQDRect (bottomMargin);
										::EraseRect (&wndRct);
									}
								}

								if (GetCaretShown () and fLastDrawnOn) {
									DrawCaret_ ();
								}

								::SetClip (oldClip);
							}
							catch (...) {
								::SetClip (oldClip);
								throw;
							}
						}
					}
				}
				catch (...) {
					Led_BeepNotify ();	// NOW EAT IT - so we don't let lots of annoying messages
										// which repeat themselves. Bad enuf we start beeping!
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::BlinkCaretIfNeeded ()
			{
				if (GetCaretShown () and GetCaretShownSituation ()) {
					float	now	=	Led_GetTickCount ();
					if (now > fTickCountAtLastBlink + GetTickCountBetweenBlinks ()) {
						Led_Rect	cr	=	CalculateCaretRect ();
						RefreshWindowRect (cr);
						fLastDrawnOn = not fLastDrawnOn;
						fTickCountAtLastBlink = now;
					}
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::DrawCaret_ ()
			{
				DrawCaret_ (fLastDrawnOn);
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::DrawCaret_ (bool on)
			{
				Tablet_Acquirer	tablet (this);
				Led_AssertNotNil (tablet);
				Led_Assert (*tablet == Led_GetCurrentGDIPort ());
				// Note: the reason we don't just use the GetCaretShown () flag here, and
				// instead we check that the selection is empty is because we want to reserve that
				// flag for users (client programmers) use. If we used it internally (like in SetSelection()
				// then the users values would get overwritten...
				Led_Assert (GetCaretShown ());
				if (GetCaretShownSituation ()) {
					Led_Rect	caretRect	=	CalculateCaretRect ();
					fLastDrawnOn = on;
					Rect	qdCaretRect	=	AsQDRect (caretRect);
					if (on) {
						::FillRect (&qdCaretRect, &Led_Pen::kBlackPattern);
					}
					else {
						::EraseRect (&qdCaretRect);
					}
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType)
			{
				if (GetScrollBarType (vh) != scrollBarType or (scrollBarType != eScrollBarNever and fScrollBars[vh] == NULL)) {
					inherited::SetScrollBarType (vh, scrollBarType);
					InvalidateScrollBarParameters ();
					if (GetUseGrafPort () == NULL) {
						// Not to worry. If our GetMacPort() is NULL now, we'll do the creation in our FinishCreate() method.
						// This could happen if SetHasScrollBar is - for example - called from a subclasses CTOR, or any time
						// before the FinishCreate().
// MUST FIX ABOVE COMMENT/MAKE SURE THIS IS OK!!!! AFTER MOIVING THIS CODE FROM PP to HERE!!! - LGP 2000-09-25
					}
					else {
						if (scrollBarType == eScrollBarNever) {
							if (fScrollBars[vh] != NULL) {
								Led_MacPortAndClipRegionEtcSaver	saver;
								#if		TARGET_CARBON
								{
									Rect	controlRect;
									::GetControlBounds (fScrollBars[vh], &controlRect);
									::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &controlRect);
								}
								#else
									SetupCurrentGrafPort ();
									::InvalRect (&(*fScrollBars[vh])->contrlRect);
								#endif
								::DisposeControl (fScrollBars[vh]);
								fScrollBars[vh] = NULL;
							}
						}
						else {
							Rect	bogus_scrollBarRect = {0, 0, 0, 0};
							if (fScrollBars[vh] == NULL) {
								#if		ACCESSOR_CALLS_ARE_FUNCTIONS
									fScrollBars[vh] = ::NewControl (::GetWindowFromPort (GetUseGrafPort ()), &bogus_scrollBarRect, "\p", scrollBarType == eScrollBarAlways, 0/*val*/, 0/*min*/, 0/*max*/, scrollBarProc, 0);
								#else
									fScrollBars[vh] = ::NewControl (GetUseGrafPort (), &bogus_scrollBarRect, "\p", scrollBarType == eScrollBarAlways, 0/*val*/, 0/*min*/, 0/*max*/, scrollBarProc, 0);
								#endif
							}
						}
						AlignTextEditRects ();
					}
				}
			}
	template	<typename	BASE_INTERACTOR>
		bool	Led_MacOS_Helper<BASE_INTERACTOR>::HandleIfScrollBarClick (const EventRecord& inMouseDownEvent)
			{
				if (BarShown (fScrollBars[v]) or BarShown (fScrollBars[h])) {
					SetupCurrentGrafPort ();
					Point	whereLocal	=	inMouseDownEvent.where;
					::GlobalToLocal (&whereLocal);
					ControlHandle	scrollBar	=	NULL;
					#if		ACCESSOR_CALLS_ARE_FUNCTIONS
						short	partCode = ::FindControl (whereLocal, ::GetWindowFromPort (GetUseGrafPort ()), &scrollBar);
					#else
						short	partCode = ::FindControl (whereLocal, GetUseGrafPort (), &scrollBar);
					#endif
					/*
					 * Remember - our widget can be embedded in a window with other controls, so check that its OUR control
					 * we've found.
					 */
					if (partCode != kControlNoPart and ((scrollBar == fScrollBars[v]) or (scrollBar == fScrollBars[h]))) {
						Led_Require (scrollBar == fScrollBars[v] or scrollBar == fScrollBars[h]);
						HandleScrollBarClick (scrollBar, partCode, AsLedPoint (whereLocal));
						return true;
					}
				}
				return false;
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleScrollBarClick (ControlRef scrollBar, short partCode, Led_Point where)
			{
				// clicked in sbar, so handle scrolling
				Led_AssertNotNil (scrollBar);
				Led_Require (scrollBar == fScrollBars[v] or scrollBar == fScrollBars[h]);
				switch (partCode) {
					case	kControlIndicatorPart: {	// thumb
						SetupCurrentGrafPort ();
						if (::TrackControl (scrollBar, AsQDPoint (where), NULL) == kControlIndicatorPart) {
							// User dragged thumb, so adjust text
							if (scrollBar == fScrollBars[v]) {
								size_t	newPos			=	::GetControlValue (scrollBar) * fScrollBarScale_V;
								newPos = Led_Max (newPos, 0);
								newPos = Led_Min (newPos, GetLength ());
								/*
								 *	Be careful of special case where vUnit > 1 and so we get truncation of values,
								 *	and may not be able to get a value of the end of buffer. So we check for that
								 *	special case and correct.
								 */
								int	aMax	=	::GetControlMaximum (scrollBar);
								Led_Assert (aMax >= 0);
								if (::GetControlValue (scrollBar)  == size_t (aMax)) {
									newPos = GetLength ();
								}
								SetTopRowInWindowByMarkerPosition (newPos);
							}
							else if (scrollBar == fScrollBars[h]) {
								size_t	newPos			=	::GetControlValue (scrollBar);
								newPos = Led_Min (newPos, ComputeMaxHScrollPos ());
								SetHScrollPos (newPos, TextInteractor::eDelayedUpdate);
							}
							else {
								Led_Assert (false);	// not reached
							}
						}
					}
					break;

					case	kControlUpButtonPart:
					case	kControlDownButtonPart:
					case	kControlPageUpPart:
					case	kControlPageDownPart: {
						static	ControlActionUPP scrollProcUPP = ::NewControlActionUPP (MyScrollProc);
						Led_Assert (sCurrentlyTrackingThisFrame == NULL);
						sCurrentlyTrackingThisFrame = this;	
						Led_Require (scrollBar == fScrollBars[v] or scrollBar == fScrollBars[h]);
						(void)::TrackControl (scrollBar, AsQDPoint (where), scrollProcUPP);
						Led_Assert (sCurrentlyTrackingThisFrame == this);
						sCurrentlyTrackingThisFrame = NULL;
					}
					break;

					default: {
						// what part!
						Led_Assert (false);	// not reached
					}
					break;
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::ActivateScrollBars (bool activate)
			{
				if (fScrollBars[v] != NULL or fScrollBars[h] != NULL) {
					UpdateScrollBars ();
					SetupCurrentGrafPort ();
					if (fScrollBars[v] != NULL) {
						::HiliteControl (fScrollBars[v], activate? 0: 255);
					}
					if (fScrollBars[h] != NULL) {
						::HiliteControl (fScrollBars[h], activate? 0: 255);
					}
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::UpdateScrollBars ()
			{
				UpdateScrollBars_ ();

				int	vValue	=	0;
				int	vMax	=	0;
				int	hValue	=	0;
				int	hMax	=	0;

				unsigned int	retryCount	=	0;
Again:
				if (fScrollBars[v] != NULL) {
					size_t	startOfWindow		=	GetMarkerPositionOfStartOfWindow ();
					size_t	endOfWindow			=	GetMarkerPositionOfEndOfWindow ();
					size_t	verticalWindowSpan	=	endOfWindow - startOfWindow;
					size_t	available			=	Led_Max (int (GetLength ()) - int (verticalWindowSpan), 0);

					// Because mac&windows only support values for sbar up to 32K - we must scale the values...
					const	int	kMaxSBarValue	=	32000;	// roughly... its really a bit more...
					fScrollBarScale_V = available/kMaxSBarValue + 1;	// always at least factor of ONE
					Led_Assert (fScrollBarScale_V >= 1);

					available /= fScrollBarScale_V;
					Led_Assert (available < kMaxSBarValue);

					Led_Assert (startOfWindow >= 0);

					startOfWindow /= fScrollBarScale_V;
					Led_Assert (startOfWindow < kMaxSBarValue);
					Led_Assert (startOfWindow <= available);

					vValue = startOfWindow;
					vMax = available;
				}
				if (fScrollBars[h] != NULL) {
					hValue = GetHScrollPos ();
					hMax = ComputeMaxHScrollPos ();
				}

////&&& NEEDS MORE WORK - FIX STUFF SO WE CAN CALL ABOUT
	// AlignTextEditRects_ WITH RIGHT ARGS....
				{
					bool	needExtraPreCall	=	false;
					if (GetScrollBarType (v) == eScrollBarAsNeeded) {
						if (BarShown (fScrollBars[v]) and vMax == 0) {
							needExtraPreCall = true;
						}
						else if (not BarShown (fScrollBars[v]) and vMax != 0) {
							needExtraPreCall = true;
						}
					}
					if (GetScrollBarType (h) == eScrollBarAsNeeded) {
						if (BarShown (fScrollBars[h]) and hMax == 0) {
							needExtraPreCall = true;
						}
						else if (not BarShown (fScrollBars[h]) and hMax != 0) {
							needExtraPreCall = true;
						}
					}
					if (needExtraPreCall and retryCount == 0) {
						AlignTextEditRects_ (GetAlwaysLeaveSpaceForSizeBox (), GetScrollBarsOverlapOwningFrame (), vMax != 0, hMax != 0);
						retryCount++;
						goto Again;
					}
				}

				if (fScrollBars[v] != NULL) {
					UpdateScrollBarsParameters (fScrollBars[v], vValue, vMax);
				}
				if (fScrollBars[h] != NULL) {
					UpdateScrollBarsParameters (fScrollBars[h], hValue, hMax);
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::UpdateScrollBarsParameters (ControlRef scrollBar, int value, int maxValue)
			{
				Led_Require (maxValue >= 0);
				Led_Require (value <= maxValue);
				Led_Require (value >= 0);
				Led_RequireNotNil (scrollBar);
				bool	anyChange				=	false;
				bool	needToRecomputeTERect	=	false;
				Led_AssertNotNil (scrollBar);
				if (::GetControlMinimum (scrollBar) != 0) {
					::SetControlMinimum (scrollBar, 0);
				}
				if (::GetControlMaximum (scrollBar) != maxValue) {
					#if		TARGET_CARBON
						Boolean oldContrlVis = IsControlVisible (scrollBar);
						::SetControlVisibility (scrollBar, false, false);
					#else
						UInt8 oldContrlVis = (*scrollBar)->contrlVis;	//  a bit of trickery to avoid this move redrawing the scrollbars before I'm ready...
						(*scrollBar)->contrlVis = 0;
					#endif
					::SetControlMaximum (scrollBar, maxValue);
					#if		TARGET_CARBON
						::SetControlVisibility (scrollBar, oldContrlVis, false);
					#else
						(*scrollBar)->contrlVis = oldContrlVis;
					#endif
					anyChange = true;
				}
				{
					VHSelect	vh	=	(scrollBar==fScrollBars[v])? v: h;
					if (GetScrollBarType (vh) == eScrollBarAsNeeded and maxValue == 0) {
						if (BarShown (scrollBar)) {
							Led_MacPortAndClipRegionEtcSaver	saver;
							SetupCurrentGrafPort ();
							::HideControl (scrollBar);
							needToRecomputeTERect = true;
						}
					}
					else {
						if (not BarShown (scrollBar)) {
							Led_MacPortAndClipRegionEtcSaver	saver;
							SetupCurrentGrafPort ();
							::ShowControl (scrollBar);
							needToRecomputeTERect = true;
						}
					}
				}

				if (value != ::GetControlValue (scrollBar)) {
					#if		TARGET_CARBON
						Boolean oldContrlVis = IsControlVisible (scrollBar);
						::SetControlVisibility (scrollBar, false, false);
					#else
						UInt8 oldContrlVis = (*scrollBar)->contrlVis;	//  a bit of trickery to avoid this move redrawing the scrollbars before I'm ready...
						(*scrollBar)->contrlVis = 0;
					#endif
					::SetControlValue (scrollBar, value);
					#if		TARGET_CARBON
						::SetControlVisibility (scrollBar, oldContrlVis, false);
					#else
						(*scrollBar)->contrlVis = oldContrlVis;
					#endif
					anyChange = true;
				}
				if (needToRecomputeTERect) {
					AlignTextEditRects ();
				}
#if 0
&&& MUST FIX THIS CODE FOR CASE WHERE CHANGE I HWIDTH AFFECTS IF WE HAVE V SBAR AND VICE VERSA - CHANGE
&&& TO AlignTextEditRects to force always leaving space worked fine, but looked awkward, and defeated
&&& purpose of NOT having sbars (space used anyhow).
&&& FIX PROPERLY!!!
#endif
				if (anyChange or needToRecomputeTERect) {
					Led_MacPortAndClipRegionEtcSaver	saver;
					#if		TARGET_CARBON
						{
							Rect	controlRect;
							::GetControlBounds (scrollBar, &controlRect);
							#if		ACCESSOR_CALLS_ARE_FUNCTIONS
								::InvalWindowRect (::GetWindowFromPort (GetUseGrafPort ()), &controlRect);
							#else
								::InvalWindowRect (GetUseGrafPort (), &controlRect);
							#endif
						}
					#else
						SetupCurrentGrafPort ();
						::InvalRect (&(*scrollBar)->contrlRect);
					#endif
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::DoHorzScroll (short whichPart)
			{
				if (whichPart == kControlUpButtonPart or whichPart == kControlDownButtonPart or
					whichPart == kControlPageUpPart or whichPart == kControlPageDownPart) {
					if (not DelaySomeForScrollBarClick ()) {
						return;
					}
				}

				#if		qDynamiclyChooseAutoScrollIncrement
					float	now	=	Led_GetTickCount ();
					static	float	sLastTimeThrough	=	0.0f;
					const	float	kClickThreshold		=	Led_GetDoubleClickTime ();
					bool	firstClick	=	(now - sLastTimeThrough > kClickThreshold);

					int		increment	=	firstClick? 1: 10;
				#else
					const	int		increment	=	1;
				#endif

				switch (whichPart) {
					case	kControlNoPart:
					break;
					case	kControlUpButtonPart: {
						if (GetHScrollPos () > 0) {
							SetHScrollPos (Led_Max (0, int (GetHScrollPos ()) - increment));
						}
					}
					break;
					case	kControlDownButtonPart: {
						SetHScrollPos (Led_Min (GetHScrollPos () + increment, ComputeMaxHScrollPos ()));
					}
					break;
					case	kControlPageUpPart: {
						const int	kPixelsAtATime	=	GetWindowRect ().GetWidth () /2;
						if (GetHScrollPos () > kPixelsAtATime) {
							SetHScrollPos (GetHScrollPos () - kPixelsAtATime);
						}
						else {
							SetHScrollPos (0);
						}
					}
					break;
					case	kControlPageDownPart: {
						const int	kPixelsAtATime	=	GetWindowRect ().GetWidth () /2;
						SetHScrollPos (Led_Min (GetHScrollPos () + kPixelsAtATime, ComputeMaxHScrollPos ()));
					}
					break;
					default:
					Led_Assert (false);	// cannot think of any other parts we should support
				}

				#if		qDynamiclyChooseAutoScrollIncrement
					sLastTimeThrough = now;
				#endif

				// Call this NOW so sbar position updated correctly IMMEDIATELY. Otherwise, might not get
				// updated til next update event (maybe when we let go of mouse button).
				Update ();
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::DoVertScroll (short whichPart)
			{
				if (whichPart == kControlUpButtonPart or whichPart == kControlDownButtonPart or
					whichPart == kControlPageUpPart or whichPart == kControlPageDownPart) {
					if (not DelaySomeForScrollBarClick ()) {
						return;
					}
				}

				#if		qDynamiclyChooseAutoScrollIncrement
					float	now	=	Led_GetTickCount ();
					static	float	sLastTimeThrough	=	0.0f;
					const	float	kClickThreshold		=	Led_GetDoubleClickTime ()/2;
					bool	firstClick	=	(now - sLastTimeThrough > kClickThreshold);

					int		increment	=	firstClick? 1: 2;
				#else
					const	int		increment	=	1;
				#endif

				switch (whichPart) {
					case	kControlNoPart:			break;
					case	kControlUpButtonPart:	ScrollByIfRoom (-increment, TextInteractor::eImmediateUpdate);	break;
					case	kControlDownButtonPart:	ScrollByIfRoom (increment, TextInteractor::eImmediateUpdate);	break;
					case	kControlPageUpPart:		ScrollByIfRoom (- long (GetTotalRowsInWindow ()), TextInteractor::eImmediateUpdate);	break;
					case	kControlPageDownPart:	ScrollByIfRoom (GetTotalRowsInWindow (), TextInteractor::eImmediateUpdate);	break;
					default:						Led_Assert (false);	// cannot think of any other parts we should support
				}

				#if		qDynamiclyChooseAutoScrollIncrement
					sLastTimeThrough = now;
				#endif

				// Call this NOW so sbar position updated correctly IMMEDIATELY. Otherwise, might not get
				// updated til next update event (maybe when we let go of mouse button).
				Update ();
			}
	template	<typename	BASE_INTERACTOR>
		pascal	void	Led_MacOS_Helper<BASE_INTERACTOR>::MyScrollProc (ControlHandle scrollBar, short ctlPart)
			{
				Led_RequireNotNil (scrollBar);

				Led_MacPortAndClipRegionEtcSaver	saver;

				Led_AssertNotNil (sCurrentlyTrackingThisFrame);
				Led_Require (scrollBar == sCurrentlyTrackingThisFrame->fScrollBars[v] or scrollBar == sCurrentlyTrackingThisFrame->fScrollBars[h]);
				if (scrollBar == sCurrentlyTrackingThisFrame->fScrollBars[v]) {
					sCurrentlyTrackingThisFrame->DoVertScroll (ctlPart);
				}
				else if (scrollBar == sCurrentlyTrackingThisFrame->fScrollBars[h]) {
					sCurrentlyTrackingThisFrame->DoHorzScroll (ctlPart);
				}
				else {
					Led_Assert (false);	// not reached
				}
			}
	template	<typename	BASE_INTERACTOR>
		bool	Led_MacOS_Helper<BASE_INTERACTOR>::BarShown (ControlRef scrollBar)
			{
				#if		TARGET_CARBON
					return scrollBar != NULL and ::IsControlVisible (scrollBar);
				#else
					return scrollBar != NULL and (*scrollBar)->contrlVis;
				#endif
			}
	template	<typename	BASE_INTERACTOR>
		Led_MacOS_Helper<BASE_INTERACTOR>*	Led_MacOS_Helper<BASE_INTERACTOR>::sCurrentlyTrackingThisFrame	=	NULL;
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleAdjustCursorEvent (const EventRecord& inEvent)
			{
				SetupCurrentGrafPort ();
				Point	whereLocal	=	inEvent.where;
				::GlobalToLocal (&whereLocal);
				
				if (whereLocal.h > GetWindowRect ().right or whereLocal.v > GetWindowRect ().bottom) {
					// in scrollbar
					::InitCursor ();
					return;
				}

				// If cursor is over draggable text, then change cursor to an arrow.
				// Doug Stein says is according to Apple HIG guidelines - LGP 960804
				// SPR#0371
				if (GetSelectionStart () != GetSelectionEnd ()) {
					Led_Region	r;
					GetSelectionWindowRegion (&r, GetSelectionStart (), GetSelectionEnd ());
					bool	result	=	::PtInRgn (whereLocal, r.GetOSRep ());
					if (result) {
						::InitCursor ();
						return;
					}
				}

				{
					Cursor**					iBeam		=	::GetCursor (iBeamCursor);
					Led_StackBasedHandleLocker	locker (Handle (iBeam));
					::SetCursor (*iBeam);
				}
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleMouseDownEvent (const EventRecord& inMouseDownEvent)
			{
				if (HandleIfScrollBarClick (inMouseDownEvent)) {
					return;
				}
				BreakInGroupedCommands ();

				UpdateClickCount (inMouseDownEvent.when / 60.0f, AsLedPoint (inMouseDownEvent.where));
				HandleTrackDragSelectInText (inMouseDownEvent);

				BreakInGroupedCommands ();
			}
	template	<typename	BASE_INTERACTOR>
		void	Led_MacOS_Helper<BASE_INTERACTOR>::HandleTrackDragSelectInText (const EventRecord& inMouseDownEvent)
			{
				SetupCurrentGrafPort ();

				Point	whereLocal	=	inMouseDownEvent.where;
				::GlobalToLocal (&whereLocal);

				bool	extendSelection	=	bool (inMouseDownEvent.modifiers & shiftKey);
				if (not ProcessSimpleClick (AsLedPoint (whereLocal), GetCurClickCount (), extendSelection, &fDragAnchor)) {
					return;
				}

				/*
				 *	Note that we do this just AFTER setting the first selection. This is to avoid
				 *	(if we had done it earlier) displaying the OLD selection and then quickly
				 *	erasing it (flicker). We do an Update () rather than simply use eImmediateUpdate
				 *	mode on the SetSelection () because at this point we want to redisplay the entire
				 *	window if part of it needed it beyond just the part within the selection.
				 */
				Update ();

				while (::StillDown ()) {
					Point	newPt;
					SetupCurrentGrafPort ();
					::GetMouse (&newPt);
					WhileSimpleMouseTracking (AsLedPoint (newPt), fDragAnchor);
					/*
					 *	On MacOS - we don't get update events while were in this stilldown loop, so we must
					 *	handle the updating here -- LGP SPR#1335.
					 */
					Update ();
				}
			}




#if		qLedUsesNamespaces
}
#endif


#endif	/*__Led_MacOS_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
