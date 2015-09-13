/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__Led_Gtk_h__
#define	__Led_Gtk_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Headers/Led_Gtk.h,v 2.73 2004/02/11 03:18:03 lewis Exp $
 */


/*
@MODULE:	Led_Gtk
@DESCRIPTION:
		<p>Templated Gtk SDK wrapper support (X-Windows/Linux). This code handles mapping between Gtk SDK concepts
	and that of Led, and to handle basic event processing.</p>
 */



/*
 * Changes:
 *	$Log: Led_Gtk.h,v $
 *	Revision 2.73  2004/02/11 03:18:03  lewis
 *	add IdleManager.h include
 *	
 *	Revision 2.72  2004/02/10 03:05:52  lewis
 *	SPR#1634: Added EnterIdler class to get notified on OnEnterIdle messages. Also - added
 *	IdleManager::NonIdleContext class to notify idle-manager of non-idle (eg executing a command)
 *	contexts (lots of these usages).
 *	
 *	Revision 2.71  2003/05/15 12:50:27  lewis
 *	SPR#1487: Add a 'GetEnabled' method to TextInteractor::CommandUpdater and subclasses (to help with SPR)
 *	
 *	Revision 2.70  2003/04/30 15:07:19  lewis
 *	static_cast<> to avoid gcc 3.2 warning
 *	
 *	Revision 2.69  2003/04/30 15:06:02  lewis
 *	static_cast<> to avoid gcc 3.2 warning
 *	
 *	Revision 2.68  2003/04/27 19:14:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.67  2003/04/18 21:02:57  lewis
 *	misc cleanups to get things compiling with gcc 2.96 on Linux RedHat 7.3
 *	
 *	Revision 2.66  2003/04/18 17:19:16  lewis
 *	small code cleanups to things complained about by new gcc 2.96
 *	
 *	Revision 2.65  2003/04/16 14:04:09  lewis
 *	SPR#1437: cosmetic cleanups - nest code and scope in in braces inside of UndoContext instances, and use new TextInteractor::BreakInGroupedCommands () wrapper (and other small cleanups)
 *	
 *	Revision 2.64  2003/04/15 23:16:07  lewis
 *	use kClear_CmdID isntead of manually doing the clear for a DELETE key (so works better with tables)
 *	
 *	Revision 2.63  2003/04/15 22:58:23  lewis
 *	Update_ -> Update ()
 *	
 *	Revision 2.62  2003/04/09 16:24:47  lewis
 *	SPR#1419: lose Refresh_ and Update_ overloaded virtual overrides
 *	
 *	Revision 2.61  2003/04/08 23:34:51  lewis
 *	SPR#1404: added TextInteractor::GetCaretShownSituation () method to wrap selStart==selEnd
 *	check (so we can override that for table code blinking caret. Updated each Led_XXX classlib
 *	wrapper to call that instead of selStart==selEnd
 *	
 *	Revision 2.60  2003/04/08 22:37:08  lewis
 *	SPR#1404: move WhileTrackingConstrainSelection () override from Led_Gtk/Win32/MacOS
 *	templates to default impl for TextInteractor (so its used in Embedded WP class so cells
 *	handle double clicks properly
 *	
 *	Revision 2.59  2003/04/04 16:26:00  lewis
 *	SPR#1407- getting new command processing code working on X-Windows
 *	
 *	Revision 2.58  2003/04/04 14:41:40  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.57  2003/04/04 00:55:20  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 2.56  2003/04/03 22:49:08  lewis
 *	SPR#1407: more work on getting this covnersion to new command handling working (mostly MacOS/Linux now)
 *	
 *	Revision 2.55  2003/04/03 16:41:21  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command
 *	classes, just builtin to TextInteractor/WordProcessor (and instead of template params use new
 *	TextInteractor/WordProcessor::DialogSupport etc
 *	
 *	Revision 2.54  2003/03/21 13:59:35  lewis
 *	SPR#1364- fix setting of drag anchor. Moved it from Led_XXX(Win32/MacOS/Gtk) to TextInteractor::ProcessSimpleClick
 *	(adding extra arg). Also lots of other small code cleanups
 *	
 *	Revision 2.53  2003/03/20 15:49:59  lewis
 *	SPR#1360 - eliminate CallEnterIdleCallback calls and start proving basic new IdleManager support
 *	
 *	Revision 2.52  2003/03/17 21:37:40  lewis
 *	SPR#1350 - qSupportEnterIdleCallback is now automatically true - and not a conditional varaible anymore
 *	
 *	Revision 2.51  2003/03/14 14:38:48  lewis
 *	SPR#1343 - Get rid of funny code for setting anchor, and override StandardStyledTextInteractor::WhileSimpleMouseTracking
 *	to properly handle tracking over embeddings and fix the WordProcessor::WhileSimpleMouseTracking to do likewise
 *	(it was already correct except for cosmetics). Net effect is that clicking on embedding works correctly again
 *	on Win32 (was OK on ohter platforms) - and is now clearer on ALL
 *	
 *	Revision 2.50  2003/01/29 19:15:05  lewis
 *	SPR#1265- use the keyword typename instead of class in template declarations
 *	
 *	Revision 2.49  2002/11/21 01:57:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2002/11/20 20:05:58  lewis
 *	got rid of use of NotificationOf_SelectionChanged and use SetSelction override instead(SPR#1172)
 *	
 *	Revision 2.47  2002/05/06 21:33:27  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.46  2001/11/27 00:29:40  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.45  2001/10/17 21:46:40  lewis
 *	massive DocComment cleanups (mostly <p>/</p> fixups)
 *	
 *	Revision 2.44  2001/09/24 14:17:16  lewis
 *	SPR#1042- new cleaned up region code
 *	
 *	Revision 2.43  2001/09/08 20:25:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.42  2001/09/08 16:42:54  lewis
 *	cleaned up g_message calls - lost needless trailing NL (g_message automatically adds it). Added
 *	more debugging stuff for CLIPBOARD (SPR#1019)
 *	
 *	Revision 2.41  2001/09/05 16:22:56  lewis
 *	SPR#1016- cleanup semantics of ReaderFlavorPackage::GetFlavorSize/ReaderFlavorPackage::ReadFlavorData
 *	
 *	Revision 2.40  2001/08/30 20:34:56  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.39  2001/08/30 20:33:16  lewis
 *	SPR#1004- catch command exceptions TextInteractorCommonCommandHelper_Gtk<>
 *	
 *	Revision 2.38  2001/08/28 18:43:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.37  2001/08/27 19:50:48  lewis
 *	SPR#0975- disable SetUseBitmapScrollingOptimization () cuz its broken
 *	
 *	Revision 2.36  2001/06/04 16:49:01  lewis
 *	comment out a few more qPrintGLIBTraceMessages messages (add && 0) so we get less spurrious
 *	output when debugging. Use new Led_Tablet_::SetClipRectangle () so I can get make private
 *	a few more Led_Tablet_ members
 *	
 *	Revision 2.35  2001/05/27 02:21:39  lewis
 *	SPR#0936- call XSetClipRectangles/XSetClipMask to get clipping right.
 *	
 *	Revision 2.34  2001/05/25 21:09:21  lewis
 *	SPR#0899- Support RTF clipboard type, and much more (related). Significant reorg of clipboard (Gtk-specific)
 *	support. Now uses more subeventloops (now that I discovered that I can).
 *	
 *	Revision 2.33  2001/05/24 13:42:44  lewis
 *	SPR#0938- hack to get Led_BeepNotify() working for X-Windows- gBeepNotifyCallBackProc
 *	
 *	Revision 2.32  2001/05/23 23:01:55  lewis
 *	SPR#0926- fix X-Windows Paste code to handle case where no pastable data available
 *	
 *	Revision 2.31  2001/05/23 21:56:11  lewis
 *	support (untested) building Led_Gtk_Helper<> templates off a Gtkclass that doesn't inherit from GtkEditable
 *	
 *	Revision 2.30  2001/05/23 16:29:23  lewis
 *	SPR#0935- add asserts clipboard_atom returned correctly (under debugger for a while it looked like
 *	it wasnot? Maybe I was confused?). And added debugging messages if gtk_selection_owner_set returned false.
 *	REAL bug with clipboard code wasn't here - but in the LedTickCount2XTime () routine, but had to examine it
 *	from here, and left some of the debugging staging
 *	
 *	Revision 2.29  2001/05/23 15:24:26  lewis
 *	SPR#0937- old code for GDK_Clear was REALLY supposed to be for GDK_DELETE. GDK_CLEAR code now just
 *	calls OnCommandClear()(not sure how to generate a CLEAR key cmd - so I couldn't test this)
 *	
 *	Revision 2.28  2001/05/22 21:41:40  lewis
 *	implement SPR#0924- Led_Gtk_TmpCmdUpdater. Also a number of small cleanups to g_messsage trace calls
 *	
 *	Revision 2.27  2001/05/20 21:49:14  lewis
 *	onidle code - and cleanup a few messages, and fix code for various timers (now I read up that timer
 *	procs just return true - so they dont need to get recreated)
 *	
 *	Revision 2.26  2001/05/18 21:04:07  lewis
 *	lots of attempts (all failed) at implementing Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Update_ ().
 *	Check for pending Expose/GraphicsExpose events - and avoid autoscroll tracking, and a few other similer
 *	things (speed back - prevents us from falling too far behind). Lots of small fixups to TraceMessage stuff.
 *	
 *	Revision 2.25  2001/05/15 16:40:31  lewis
 *	cleanup - g_message tags sprintf() style args directly
 *	
 *	Revision 2.24  2001/05/08 21:38:54  lewis
 *	Hook destroy() message, and delete the Led-class as well. Added gtk_GetParentClass ()
 *	
 *	Revision 2.23  2001/05/04 20:42:31  lewis
 *	kill some old hack font code now that we are well on the way to having new good font code
 *	
 *	Revision 2.22  2001/05/04 17:41:07  lewis
 *	SPR#0901- fix bug with buttonclick handling - crash on non-leftmost click
 *	
 *	Revision 2.21  2001/05/04 17:07:43  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.20  2001/05/03 22:08:08  lewis
 *	SPR#0887- blinking caret support.
 *	
 *	Revision 2.19  2001/05/03 20:58:01  lewis
 *	minor tweeks - like updating the GtkEditable's selection start/end on sel changed events. Big
 *	change was SPR#0894- X-Windows clipboard support. Handle_selection_received/Static_selection_get/OnCopy/Paste_* methods.
 *	
 *	Revision 2.18  2001/05/02 18:32:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.17  2001/05/02 18:21:08  lewis
 *	SPR#0895 - call SyncronizeLedXTickCount()
 *	
 *	Revision 2.16  2001/05/02 15:26:42  lewis
 *	only call gtk_signal_emit_by_name on scrollbar update if there was a real change
 *	
 *	Revision 2.15  2001/05/02 14:54:46  lewis
 *	SPR#0882- implemented scrolling/scrollbar support (GtkAdjustment API)
 *	
 *	Revision 2.14  2001/05/01 19:28:45  lewis
 *	SPR#0892- Start/StopAutoscrollTimer support. Now we handle autoscrolling properly while tracking mouse downs
 *	
 *	Revision 2.13  2001/05/01 16:31:56  lewis
 *	most of support for SPR#0892 - mouse click/tracking. Still no support for D&D and no support for
 *	autoscrolling text (esp while mouse outside of window - so need timer)
 *	
 *	Revision 2.12  2001/05/01 13:18:53  lewis
 *	breakup commands OnKillFocus
 *	
 *	Revision 2.11  2001/04/30 22:37:45  lewis
 *	lose USE_XIM support (since needs lots of work to get right - and cannot test right now - wait til
 *	testable to try out). And SPR#0886 - add PRELIM support for FocusIn/FocusOut events
 *	
 *	Revision 2.10  2001/04/27 15:24:50  lewis
 *	SPR#0885- most support for Led_Gtk command support - TextInteractorCommonCommandHelper_Gtk<> etc.
 *	
 *	Revision 2.9  2001/04/26 20:56:56  lewis
 *	SPR#0884- (arrow etc) key handling. And various code cleanups
 *	
 *	Revision 2.8  2001/04/26 17:01:26  lewis
 *	cleanups
 *	
 *	Revision 2.7  2001/04/26 00:43:36  lewis
 *	SPR#0881 - added Layout method - and there call SetWindowrect - and called that from onchangesize and realize
 *	
 *	Revision 2.6  2001/04/25 22:52:35  lewis
 *	Major cleanups. Now using GtkEditable (SPR#0880). Got rid of background flicker.Got rid of most of
 *	the egregions background flicker by using NO_WINDOW flag like gtktext.c widget. Had to hook a few more
 *	widget messages - like realize / draw (as opposed to expose). Still kludged together - but Had to hook a
 *	few more widget messages - like realize / draw (as opposed to expose). Still kludged together - but
 *	
 *	Revision 2.5  2001/04/23 19:33:51  lewis
 *	SPR#0877- check if windowRect.IsEmpty() before calling GetWidth/GetHeight()
 *	
 *	Revision 2.4  2001/04/23 17:19:55  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.3  2001/04/20 21:46:21  lewis
 *	better support for handling keystrokes (CR/LF and multiple chars from IME)
 *	
 *	Revision 2.2  2000/09/05 23:24:53  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.1  2000/09/05 22:35:49  lewis
 *	new preliminary support for the X-Windows GTK class library (for Gnone)
 *	
 *	
 *
 *
 *
 */

#include	<set>

#include	<gtk/gtk.h>
#include	<gtk/gtkeditable.h>
#include	<gdk/gdkkeysyms.h>


#include	"IdleManager.h"
#include	"TextInteractor.h"


typedef	struct	_GtkMenuItem	GtkMenuItem;


/*
@CONFIGVAR:		qPrintGLIBTraceMessages
@DESCRIPTION:	<p>Moderately useful trace messages output. Defaults to true only when qDebug true.</p>
 */
#ifndef	qPrintGLIBTraceMessages
	#define	qPrintGLIBTraceMessages	qDebug
#endif






#if		qLedUsesNamespaces
namespace	Led {
#endif




struct	GtkBaseInfo_GtkEditable {
	typedef	GtkEditable			BaseClass;
	typedef	GtkEditableClass	BaseMetaClass;
	static	GtkType				GetBaseTypeId ()	{	return GTK_TYPE_EDITABLE; }
};



/*
@CLASS:			Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>
@BASES:			BASE_INTERACTOR = @'TextInteractor'
@DESCRIPTION:	<p>DOCUMENT SOON - BASED ON @'Led_MFC_Helper<MFC_BASE_CLASS,BASE_INTERACTOR>'</p>
*/
template	<typename	BASE_INTERACTOR = TextInteractor, typename GTKBASEINFO = GtkBaseInfo_GtkEditable>	class	Led_Gtk_Helper :
	public virtual BASE_INTERACTOR
{
	public:
		typedef	TextInteractor::UpdateMode	UpdateMode;
		typedef	MarkerOwner::UpdateInfo		UpdateInfo;
	private:
		typedef	BASE_INTERACTOR	inherited;

	public:
		Led_Gtk_Helper ();

	public:
		virtual ~Led_Gtk_Helper ();

	protected:
		struct	Led2GTKMapper {
			typename GTKBASEINFO::BaseClass					fGTKBase;
			Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*	fRealClass;
		};
		Led2GTKMapper*	fLed2GTKMapper;
	public:
		nonvirtual	GtkWidget*										Get_GtkWidget () const;
		nonvirtual	typename GTKBASEINFO::BaseClass*				Get_GTKBASE ();
		static		Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*	WidgetToTHIS (GtkWidget* widget);

	// Command dispatch support
	public:
		nonvirtual	bool	DispatchCommandUpdateCallback (GtkMenuItem* m, TextInteractor::CommandNumber c);
		static		void	DispatchCommandCallback (gpointer callback_data, guint callback_action, GtkWidget* widget);

	// Standard GTK static widget routines
	public:
		static	void	Static_Destroy (GtkObject* o);

		static	gint	Static_expose (GtkWidget* widget, GdkEventExpose* event);
		virtual	gint	Handle_Expose (const GdkEventExpose& event);
		virtual	void	Handle_DrawingHelper (const Led_Rect& updateRect);

		static	gint	Static_key_press (GtkWidget* widget, GdkEventKey* event);
		virtual	gint	Handle_key_press (const GdkEventKey& event);

		static	void	Static_Draw (GtkWidget* widget, GdkRectangle* area);
		virtual	void	Handle_Draw (const GdkRectangle& area);

		static	void	Static_Realize (GtkWidget* widget);
		virtual	void	Handle_Realize ();

		static	void	Static_size_allocate (GtkWidget* widget, GtkAllocation* allocation);
		virtual	void	Handle_size_allocate (const GtkAllocation& allocation);

		static	gint	Static_FocusIn (GtkWidget* widget, GdkEventFocus* event);
		virtual	gint	Handle_FocusIn (const GdkEventFocus& event);

		static	gint	Static_FocusOut (GtkWidget* widget, GdkEventFocus* event);
		virtual	gint	Handle_FocusOut (const GdkEventFocus& event);

		static	gint	Static_ButtonPress (GtkWidget* widget, GdkEventButton* event);
		virtual	gint	Handle_ButtonPress (const GdkEventButton& event);

		static	gint	Static_MouseMoved (GtkWidget* widget, GdkEventMotion* event);
		virtual	gint	Handle_MouseMoved (const GdkEventMotion& event);

		static	gint	Static_ButtonRelease (GtkWidget* widget, GdkEventButton* event);
		virtual	gint	Handle_ButtonRelease (const GdkEventButton& event);

		static	gint	Static_ScrollTimeout (gpointer data);
		virtual	gint	Handle_ScrollTimeout ();

		static	gint	Static_CaretBlinkTimer (gpointer data);
		virtual	gint	Handle_CaretBlinkTimer ();

	// Blinking caret:
	public:
		nonvirtual	void	BlinkCaretIfNeeded ();	// just allow public access
													// via idle task mechanism or whatever...
	protected:
		nonvirtual	void	DrawCaret_ ();
		nonvirtual	void	DrawCaret_ (bool on);
	private:
		nonvirtual	void	StartBlinkCaretTimer ();
		nonvirtual	void	StopBlinkCaretTimer ();
	protected:
		bool		fLastDrawnOn;
	private:
		float		fTickCountAtLastBlink;
		gint		fBlinkCaretTimerID;

	// Keyboard Processing:
	private:
		nonvirtual	bool	CheckIfDraggingBeepAndReturn ();

	// Mouse Processing
	protected:
		Led_Point	fMouseTrackingLastPoint;
	protected:
		size_t		fDragAnchor;		// only used while dragging mouse
	private:
		nonvirtual	void	StartAutoscrollTimer ();
		nonvirtual	void	StopAutoscrollTimer ();
	private:
		gint	fAutoScrollTimerID;

	// Scrollbar support
	public:
		typedef	TextInteractor::ScrollBarType	ScrollBarType;	// redundant typedef to keep compiler happy...LGP 2000-10-05-MSVC60
		typedef	TextInteractor::VHSelect		VHSelect;		// redundant typedef to keep compiler happy...LGP 2000-10-05-MSVC60

	private:
		static	bool	ChangeNSetHelper_ (gfloat* value, gfloat newValue);
	public:
		override	void		SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType);
		override	void		UpdateScrollBars ();
	protected:
		virtual	bool		ShouldUpdateHScrollBar () const;
		virtual	bool		ShouldUpdateVScrollBar () const;
	public:
		nonvirtual	GtkAdjustment*	GetAdjustmentObject (VHSelect vh) const;
	private:
		GtkAdjustment*	fAdjustmentObj[2];
		gfloat			fAdjustmentLastVal[2];
	private:
		static		void	Static_AdjustmentDisconnect (GtkAdjustment *adjustment, Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>* THIS);
		nonvirtual	void	SetAdjustmentObjects (GtkAdjustment* hadj, GtkAdjustment* vadj);
		static		void	Static_AdjustmentChangeCallback (GtkAdjustment *adjustment, Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>* THIS);
		static		void	Static_AdjustmentValueChangeCallback (GtkAdjustment *adjustment, Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>* THIS);
		virtual		void	OnScrollMessage (GtkAdjustment *adjustment);

	protected:
		virtual	void	Layout ();
	
	//TEXTIMAGER
	public:
		override	Led_Tablet	AcquireTablet () const;
		override	void		ReleaseTablet (Led_Tablet tablet) const;
		override	void		SetSelection (size_t start, size_t end);
		#if		qUsingMemberNameToOverloadInTemplateClassBug
			void	SetSelection (size_t start, size_t end, TextInteractor::UpdateMode updateMode)
				{
					BASE_INTERACTOR::SetSelection (start, end, updateMode);
				}
		#else
			using	BASE_INTERACTOR::SetSelection;
		#endif

	//TEXTINTERACTOR
	protected:
		override	void	RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const;
		override	void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const;
		override	bool	QueryInputKeyStrokesPending () const;

	// Clipboard Support
	protected:
		override	bool	OnCopyCommand_Before ();
		override	void	OnCopyCommand_After ();
		override	bool	OnPasteCommand_Before ();
		override	void	OnPasteCommand_After ();
		static		void	Static_selection_get (GtkWidget* widget, GtkSelectionData *selection_data, guint info, guint time);

	protected:
		static	void	Static_selection_received  (GtkWidget* widget, GtkSelectionData* selection_data, guint time);
		virtual	void	Handle_selection_received  (const GtkSelectionData* selection_data, guint time);
	private:
		set<GdkAtom>	fCurServerClipFormats;

	// METACLASS INFO FOR GTK...
	public:
		struct	GtkLedEditorClass;
		static	GtkType			gtk_LedEditor_get_type ();
		static	void			gtk_LedEditor_class_init (GtkLedEditorClass *widgetClass);
		static	void			gtk_LedEditor_init (Led2GTKMapper *image);
		static	GtkWidgetClass*	gtk_GetParentClass ();


	public:
		static	GdkAtom	kAtom_TARGETS;
		static	GdkAtom	kAtom_CLIPBOARD;
		static	GdkAtom	kAtom_TIMESTAMP;
		static	GdkAtom	kAtom_MULTIPLE;

	private:
		mutable	Led_Tablet	fTablet;
};


template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
	inline	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*	GET_LED_GTK_HELPER (GtkWidget* w)
		{
			return Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::WidgetToTHIS (w);
		}






/*
@CLASS:			Gtk_CommandNumberMapping
@DESCRIPTION:	<p></p>
*/
class	Gtk_CommandNumberMapping : public CommandNumberMapping<int> {
	protected:
		Gtk_CommandNumberMapping ();
};








/*
@CLASS:			Led_Gtk_TmpCmdUpdater
@DESCRIPTION:
				<p>Helper class, used with @'Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>' template.</p>
				<p>For now - this is just a empty unused class, as I have not yet seen how to implement this style of updating
			within Gtk. It may not be supported for the first Gtk release of Led (Led 3.0).</p>
*/
class	Led_Gtk_TmpCmdUpdater : public TextInteractor::CommandUpdater {
	public:
		Led_Gtk_TmpCmdUpdater (GtkMenuItem* m, CommandNumber cmdNum);
	public:
		nonvirtual	operator Led_Gtk_TmpCmdUpdater* ();

	public:
		override	CommandNumber	GetCmdID () const;
		override	bool			GetEnabled () const;
		override	void			SetEnabled (bool enabled);
		override	void			SetChecked (bool checked);
		override	void			SetText (const Led_SDK_Char* text);

	private:
		GtkMenuItem*	fMenuItem;
		CommandNumber 	fCommand;
};












/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

	#include	<gdk/gdkx.h>

 //	class	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		inline	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Led_Gtk_Helper ():
				fLed2GTKMapper (NULL),
				fLastDrawnOn (false),
				fTickCountAtLastBlink (0.0f),
				fBlinkCaretTimerID (0),
				fMouseTrackingLastPoint (),
				fDragAnchor (0),
				fAutoScrollTimerID (0),
				//fAdjustmentObj (),
				//fAdjustmentLastVal (),
				fCurServerClipFormats (),
				fTablet (NULL)
			{
				SetUseBitmapScrollingOptimization (false);	// unfortunately - due to SPR#0976

				if (gBeepNotifyCallBackProc == NULL) {
					gBeepNotifyCallBackProc = &gdk_beep;
				}

				/*
				 *	Assign values to Atom 'constants'. Name them constants to encourage code not to change them. But they cannot be properly INITIALIZED
				 *	constants, because they need to have the XDisplay object (and its gtk global) set, and this comes as an arg to main (really 
				 *	even later when we open arg display object).
				 */
				if (kAtom_TARGETS == None) {
					kAtom_TARGETS = gdk_atom_intern ("TARGETS", false);
					Led_Assert (kAtom_TARGETS != None);
				}
				if (kAtom_CLIPBOARD == None) {
					kAtom_CLIPBOARD = gdk_atom_intern ("CLIPBOARD", false);
					Led_Assert (kAtom_CLIPBOARD != None);
				}
				if (kAtom_TIMESTAMP == None) {
					kAtom_TIMESTAMP = gdk_atom_intern ("TIMESTAMP", false);
					Led_Assert (kAtom_TIMESTAMP != None);
				}
				if (kAtom_MULTIPLE == None) {
					kAtom_MULTIPLE = gdk_atom_intern ("MULTIPLE", false);
					Led_Assert (kAtom_MULTIPLE != None);
				}

				fLed2GTKMapper = reinterpret_cast<Led2GTKMapper*> (gtk_type_new (gtk_LedEditor_get_type ()));
				fAdjustmentObj[v] = NULL;
				fAdjustmentObj[h] = NULL;
				fAdjustmentLastVal[v] = 0.0f;
				fAdjustmentLastVal[h] = 0.0f;
				GTK_WIDGET (fLed2GTKMapper)->requisition.width = 400;
				GTK_WIDGET (fLed2GTKMapper)->requisition.height = 400;
				fLed2GTKMapper->fRealClass = this;
				SetAdjustmentObjects (NULL, NULL);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		inline	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::~Led_Gtk_Helper ()
			{
				Led_Assert (fAutoScrollTimerID == 0);			// I don't see how we can get destroyed while tracking?
				delete fTablet;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		inline	GtkWidget*	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Get_GtkWidget () const
			{
				Led_RequireNotNil (fLed2GTKMapper);
				Led_EnsureNotNil (&fLed2GTKMapper->fGTKBase);
				Led_Ensure (GTK_CHECK_TYPE ((&fLed2GTKMapper->fGTKBase), gtk_LedEditor_get_type ()));
				return GTK_WIDGET (&fLed2GTKMapper->fGTKBase);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		inline	typename GTKBASEINFO::BaseClass*	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Get_GTKBASE ()
			{
				Led_RequireNotNil (fLed2GTKMapper);
				Led_EnsureNotNil (&fLed2GTKMapper->fGTKBase);
				Led_Ensure (GTK_CHECK_TYPE ((&fLed2GTKMapper->fGTKBase), gtk_LedEditor_get_type ()));
				return &fLed2GTKMapper->fGTKBase;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		inline	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::WidgetToTHIS (GtkWidget* widget)
			{
				Led_RequireNotNil (widget);
				typedef	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>		THIS_TYPE;	// typedef needed cuz of macro (comma confused macro processing)
				THIS_TYPE*	answer	=	reinterpret_cast <Led2GTKMapper*> (widget)->fRealClass;
				Led_Ensure (GTK_WIDGET (answer->Get_GTKBASE ()) == widget);
				Led_AssertMember (answer, THIS_TYPE);
				return answer;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		bool	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::DispatchCommandUpdateCallback (GtkMenuItem* m, TextInteractor::CommandNumber c)
			{
				// NB: As of Led 3.1a6 - I don't believe this is ever called - and so all commands always enabled.
				// That should be addressed in some future version of the Led_Gtk support. Perhaps use
				// an OnIdle handler mechanism? And some dirtying flag (so I know when to do the update)?
				Led_Gtk_TmpCmdUpdater	enabler (m, Gtk_CommandNumberMapping::Get ().Lookup (c));
				return OnUpdateCommand (&enabler);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::DispatchCommandCallback (gpointer callback_data, guint callback_action, GtkWidget* widget)
			{
				typedef	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>	THIS_TYPE;
				THIS_TYPE*	THIS	=	reinterpret_cast<THIS_TYPE*> (callback_data);
				try {
					IdleManager::NonIdleContext	nonIdleContext;
					(void)THIS->OnPerformCommand (Gtk_CommandNumberMapping::Get ().Lookup (callback_action));
				}
				catch (...) {
					Led_BeepNotify ();		// Not sure this is the best thing to call / do here - but
											// better than allowing the throw to propagate (since Gtk doesn't handle  it)
											// SPR#1004 - LGP 2001-08-30
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_Destroy (GtkObject* o)
			{
				Led_RequireNotNil (o);
				typedef	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>		THIS_TYPE;	// typedef needed cuz of macro (comma confused macro processing)
				THIS_TYPE*	answer	=	reinterpret_cast <Led2GTKMapper*> (o)->fRealClass;
				Led_Assert (answer == WidgetToTHIS (GTK_WIDGET (o)));
				delete answer;
				reinterpret_cast <Led2GTKMapper*> (o)->fRealClass = NULL;
				GTK_OBJECT_CLASS(gtk_GetParentClass ())->destroy (o);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_expose (GtkWidget* widget, GdkEventExpose* event)
			{
				return WidgetToTHIS (widget)->Handle_Expose (*event);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_Expose (const GdkEventExpose& event)
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Led_Gtk_Helper<>::Handle_Expose (%d,%d,%d,%d)", event.area.y, event.area.x, event.area.height, event.area.width);
				#endif
				Handle_DrawingHelper (Led_Rect (event.area.y, event.area.x, event.area.height, event.area.width));
				return FALSE;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_DrawingHelper (const Led_Rect& updateRect)
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Entering Led_Gtk_Helper<>::Handle_DrawingHelper (%d,%d,%d,%d)", updateRect.GetTop (), updateRect.GetLeft (), updateRect.GetHeight (), updateRect.GetWidth ());
				#endif

				GtkWidget*	widget	=	Get_GtkWidget ();
				if (GTK_WIDGET_VISIBLE (widget) and GTK_WIDGET_MAPPED (widget)) {
					bool		printing	=	false;
// MAYBE SHOULD USE INTERSECTION OF WINDOW(widget) rect and given update rect???
					typename BASE_INTERACTOR::Tablet_Acquirer	tablet_ (this);
					Led_Tablet	tablet	=	tablet_;
					Led_AssertNotNil (tablet);
					tablet->SetClip (updateRect);
					Draw (updateRect, printing);
					if (GetCaretShown () and fLastDrawnOn) {
						DrawCaret_ ();
					}
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		inline	GtkAdjustment*	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::GetAdjustmentObject (VHSelect vh) const
			{
				Led_Require (vh == v or vh == h);
				Led_Require (vh <= 1);
				return fAdjustmentObj[vh];
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		bool	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ChangeNSetHelper_ (gfloat* value, gfloat newValue)
			{
				if (*value != newValue) {
					*value = newValue;
					return true;
				}
				else {
					return false;
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::SetScrollBarType (VHSelect vh, ScrollBarType scrollBarType)
			{
				if (GetScrollBarType (vh) != scrollBarType) {
					inherited::SetScrollBarType (vh, scrollBarType);
//NOT SURE WE CAN SUPPORT THIS API/SHOULD - Not excatly how samples I've seen with Gtk work...???
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		/*
		@METHOD:		Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::UpdateScrollBars
		@DESCRIPTION:	<p>Hook the @'TextInteractor::UpdateScrollBars' () notification routine to update the values in
			our scrollbars.</p>
		*/
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::UpdateScrollBars ()
			{
				BASE_INTERACTOR::UpdateScrollBars_ ();

				GtkWidget*	widget	=	Get_GtkWidget ();
				if (GTK_WIDGET_REALIZED (widget)) {
					if (ShouldUpdateVScrollBar ()) {
						size_t	startOfWindow		=	GetMarkerPositionOfStartOfWindow ();
						size_t	endOfWindow			=	GetMarkerPositionOfEndOfWindow ();
						size_t	verticalWindowSpan	=	endOfWindow - startOfWindow;

						Led_RequireNotNil (fAdjustmentObj[v]);

						bool	changed	=	false;
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[v]->lower, 0);
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[v]->upper, GetLength ());
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[v]->step_increment, 1);		// hack - don't really step by this amount...
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[v]->page_increment, verticalWindowSpan);
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[v]->page_size, verticalWindowSpan);
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[v]->value, startOfWindow);
						fAdjustmentLastVal[v] = fAdjustmentObj[v]->value;

						if (changed) {
							#if		qPrintGLIBTraceMessages && 0
								g_message ("CHANGED: about to emit adjustmentchange (V) signal from inside of Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::UpdateScrollBars () with\r\n\tupper=%f,value=%f", fAdjustmentObj[v]->upper, fAdjustmentObj[v]->value);
							#endif
							gtk_signal_emit_by_name (GTK_OBJECT (fAdjustmentObj[v]), "changed");
						}
					}

					if (ShouldUpdateHScrollBar ()) {
						Led_RequireNotNil (fAdjustmentObj[h]);
						bool	changed	=	false;
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[h]->lower, 0);
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[h]->step_increment, 1);		// hack - don't really step by this amount...
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[h]->page_increment, GetWindowRect ().GetWidth ());
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[h]->page_size, fAdjustmentObj[h]->page_increment);
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[h]->upper, fAdjustmentObj[h]->page_size + ComputeMaxHScrollPos ());
						changed |= ChangeNSetHelper_ (&fAdjustmentObj[h]->value, GetHScrollPos ());
						changed |= ChangeNSetHelper_ (&fAdjustmentLastVal[h], fAdjustmentObj[h]->value);

						if (changed) {
							#if		qPrintGLIBTraceMessages && 0
								g_message ("CHANGED: about to emit adjustmentchange (H) signal from inside of Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::UpdateScrollBars () with\r\n\tupper=%f,value=%f", fAdjustmentObj[h]->upper, fAdjustmentObj[h]->value);
							#endif
							gtk_signal_emit_by_name (GTK_OBJECT (fAdjustmentObj[h]), "changed");
						}
					}
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		/*
		@METHOD:		Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ShouldUpdateHScrollBar
		@DESCRIPTION:	<p>Return true if Led_Win32_Helper<BASE_INTERACTOR> should automaticly generate
					@'Led_Win32_Helper<BASE_INTERACTOR>::SetHScrollInfo' calls.</p>
						<p>See also @'Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ShouldUpdateVScrollBar'.</p>
		*/
		bool		Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ShouldUpdateHScrollBar () const
			{
				return true;	//tmphack
				//NB: we must update sbar even if NEVER - when the style is ON, cuz we may need to HIDE the sbar
			//	return GetScrollBarType (h) != eScrollBarNever or (GetStyle () & WS_HSCROLL);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		/*
		@METHOD:		Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ShouldUpdateHScrollBar
		@DESCRIPTION:	<p>See  @'Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ShouldUpdateHScrollBar'.</p>
		*/
		bool		Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ShouldUpdateVScrollBar () const
			{
				return true;	//tmphack
				//NB: we must update sbar even if NEVER - when the style is ON, cuz we may need to HIDE the sbar
			//	return GetScrollBarType (v) != eScrollBarNever or (GetStyle () & WS_VSCROLL);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_AdjustmentDisconnect (GtkAdjustment *adjustment, Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>* THIS)
			{
				if (adjustment == THIS->fAdjustmentObj[h]) {
					THIS->SetAdjustmentObjects (NULL, THIS->fAdjustmentObj[v]);
				}
				if (adjustment == THIS->fAdjustmentObj[v]) {
					THIS->SetAdjustmentObjects (THIS->fAdjustmentObj[h], NULL);
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::SetAdjustmentObjects (GtkAdjustment* hadj, GtkAdjustment* vadj)
			{
				if (hadj)
					g_return_if_fail (GTK_IS_ADJUSTMENT (hadj));
				else
					hadj = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
				if (vadj)
					g_return_if_fail (GTK_IS_ADJUSTMENT (vadj));
				else
					vadj = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));

				if (fAdjustmentObj[h] && (fAdjustmentObj[h] != hadj)) {
					gtk_signal_disconnect_by_data (GTK_OBJECT (fAdjustmentObj[h]), this);
					gtk_object_unref (GTK_OBJECT (fAdjustmentObj[h]));
				}

				if (fAdjustmentObj[v] && (fAdjustmentObj[v] != vadj)) {
					gtk_signal_disconnect_by_data (GTK_OBJECT (fAdjustmentObj[v]), this);
					gtk_object_unref (GTK_OBJECT (fAdjustmentObj[v]));
				}

				if (fAdjustmentObj[h] != hadj) {
					fAdjustmentObj[h] = hadj;
					gtk_object_ref (GTK_OBJECT (fAdjustmentObj[h]));
					gtk_object_sink (GTK_OBJECT (fAdjustmentObj[h]));
					gtk_signal_connect (GTK_OBJECT (fAdjustmentObj[h]), "changed", (GtkSignalFunc) Static_AdjustmentChangeCallback, this);
					gtk_signal_connect (GTK_OBJECT (fAdjustmentObj[h]), "value_changed", (GtkSignalFunc) Static_AdjustmentValueChangeCallback, this);
					gtk_signal_connect (GTK_OBJECT (fAdjustmentObj[h]), "disconnect", (GtkSignalFunc) Static_AdjustmentDisconnect, this);
					Static_AdjustmentChangeCallback (hadj, this);
				}
				if (fAdjustmentObj[v] != vadj) {
					fAdjustmentObj[v] = vadj;
					gtk_object_ref (GTK_OBJECT (fAdjustmentObj[v]));
					gtk_object_sink (GTK_OBJECT (fAdjustmentObj[v]));
					gtk_signal_connect (GTK_OBJECT (fAdjustmentObj[v]), "changed", (GtkSignalFunc) Static_AdjustmentChangeCallback, this);
					gtk_signal_connect (GTK_OBJECT (fAdjustmentObj[v]), "value_changed", (GtkSignalFunc) Static_AdjustmentValueChangeCallback, this);
					gtk_signal_connect (GTK_OBJECT (fAdjustmentObj[v]), "disconnect", (GtkSignalFunc) Static_AdjustmentDisconnect, this);
					Static_AdjustmentChangeCallback (vadj, this);
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_AdjustmentChangeCallback (GtkAdjustment *adjustment, Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>* THIS)
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Entering Led_Gtk_Helper<>::Static_AdjustmentChangeCallback (BAR=%s,upper=%f,value=%f)", (adjustment==THIS->fAdjustmentObj[v])? "v": "h", adjustment->upper, adjustment->value);
				#endif
				THIS->OnScrollMessage (adjustment);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_AdjustmentValueChangeCallback (GtkAdjustment *adjustment, Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>* THIS)
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Entering Led_Gtk_Helper<>::Static_AdjustmentValueChangeCallback (BAR=%s,upper=%f,value=%f)", (adjustment==THIS->fAdjustmentObj[v])? "v": "h", adjustment->upper, adjustment->value);
				#endif
				THIS->OnScrollMessage (adjustment);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::OnScrollMessage (GtkAdjustment *adjustment)
			{
				/*
				 *	Clamp the value here, because we'll get really confused
				 * if someone tries to move the adjusment outside of the
				 * allowed bounds
				 */
				gfloat	old_val = adjustment->value;
				adjustment->value = MIN (adjustment->value, adjustment->upper - adjustment->page_size);
				adjustment->value = MAX (adjustment->value, 0.0);

				if (adjustment->value != old_val) {
					gtk_signal_handler_block_by_func (GTK_OBJECT (adjustment),	GTK_SIGNAL_FUNC (Static_AdjustmentChangeCallback), this);
					gtk_adjustment_changed (adjustment);
					gtk_signal_handler_unblock_by_func (GTK_OBJECT (adjustment), GTK_SIGNAL_FUNC (Static_AdjustmentChangeCallback),	this);
				}

				VHSelect	vh	=	(adjustment==fAdjustmentObj[v])? v: h;
				Led_Assert (adjustment==fAdjustmentObj[vh]);	// effectively assures its either the V or H scrollbar.

				if (fAdjustmentLastVal[vh] != fAdjustmentObj[vh]->value) {
					/*
					 *	Alas - instead of sending us scrollup/pagedown etc messages, they send us the final resulting value.
					 *	Attempt to intuit/guess if it was really a page up or down or whatever. If we cannot guess properly - then treat
					 *	it as a thumb move (which it probably was).
					 */
					gfloat	diff	=	fAdjustmentObj[vh]->value - fAdjustmentLastVal[vh];
					#if		qDynamiclyChooseAutoScrollIncrement
						float	now	=	Led_GetTickCount ();
						static	float	sLastTimeThrough	=	0.0f;
						const	float	kClickThreshold		=	Led_GetDoubleClickTime ()/3;
						bool	firstClick	=	(now - sLastTimeThrough > kClickThreshold);
					#endif
					switch (vh) {
						case	v: {
							#if		qDynamiclyChooseAutoScrollIncrement
								int		increment	=	firstClick? 1: 2;
							#else
								const	int		increment	=	1;
							#endif
							if (diff == -1) {
								ScrollByIfRoom (-increment, eImmediateUpdate);
							}
							else if (diff == 1) {
								ScrollByIfRoom (increment, eImmediateUpdate);
								}
							else if (diff == fAdjustmentObj[vh]->page_size) {
								ScrollByIfRoom (GetTotalRowsInWindow ());
							}
							else if (diff == -fAdjustmentObj[vh]->page_size) {
								ScrollByIfRoom (-(int)GetTotalRowsInWindow ());
							}
							else {
								size_t	newPos = static_cast<size_t> (Led_Min (fAdjustmentObj[vh]->value, GetLength ()));
								SetTopRowInWindowByMarkerPosition (newPos);
							}
						}
						break;
						case	h: {
							#if		qDynamiclyChooseAutoScrollIncrement
								int		increment	=	firstClick? 1: 10;
							#else
								const	int		increment	=	1;
							#endif
							if (diff == -1) {
								if (GetHScrollPos () > 0) {
									SetHScrollPos (Led_Max (0, int (GetHScrollPos ()) - increment));
								}
							}
							else if (diff == 1) {
								SetHScrollPos (Led_Min (GetHScrollPos () + increment, ComputeMaxHScrollPos ()));
							}
							else if (diff == -fAdjustmentObj[vh]->page_size) {
								const int	kPixelsAtATime	=	GetWindowRect ().GetWidth () /2;
								if (GetHScrollPos () > kPixelsAtATime) {
									SetHScrollPos (GetHScrollPos () - kPixelsAtATime);
								}
								else {
									SetHScrollPos (0);
								}
							}
							else if (diff == fAdjustmentObj[vh]->page_size) {
								const int	kPixelsAtATime	=	GetWindowRect ().GetWidth () /2;
								SetHScrollPos (Led_Min (GetHScrollPos () + kPixelsAtATime, ComputeMaxHScrollPos ()));
							}
							else {
								size_t	newPos = static_cast<size_t> (Led_Min (fAdjustmentObj[vh]->value, ComputeMaxHScrollPos ()));
								SetHScrollPos (Led_Min (newPos, ComputeMaxHScrollPos ()));
							}
						}
						break;
					}
					#if		qDynamiclyChooseAutoScrollIncrement
						sLastTimeThrough = now;
					#endif
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Layout ()
			{
				GtkWidget*	widget	=	Get_GtkWidget ();
				gint		height	=	0;
				gint		width	=	0;
				Led_AssertNotNil (widget);
				Led_AssertNotNil (widget->window);
				gdk_window_get_size  (widget->window, &width, &height);

				Led_Rect	r	=	Led_Rect (0, 0, height, width);
				static_cast<TextImager*>(this)->SetWindowRect (r);
				InvalidateScrollBarParameters ();
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		Led_Tablet	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::AcquireTablet () const
			{
				if (fTablet == NULL) {
					GtkWidget*	widget		=	Get_GtkWidget ();

					if (not GTK_WIDGET_REALIZED (widget)) {
						throw typename BASE_INTERACTOR::NoTabletAvailable ();
					}

					Display*	display		=	GDK_WINDOW_XDISPLAY (widget->window);
					Drawable	drawable	=	GDK_WINDOW_XWINDOW (widget->window);
					Led_AssertNotNil (display);
					Led_Assert (drawable != 0);
					int			screen		=	DefaultScreen (display);

					fTablet = new Led_Tablet_ (display, drawable);
				}
				return fTablet;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::ReleaseTablet (Led_Tablet tablet) const
			{
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::SetSelection (size_t start, size_t end)
			{
				BASE_INTERACTOR::SetSelection (start, end);
				GtkWidget*		widget			=	Get_GtkWidget ();
				if (GTK_IS_EDITABLE (widget)) {
					// By default this is true- but someone may want to base Led_Gtk_Helper<> on a Gtk class that isn't a subclass of GtkEditable
					GtkEditable*	editable		=	GTK_EDITABLE (widget);	
					size_t	selStart;
					size_t	selEnd;
					GetSelection (&selStart, &selEnd);
					editable->selection_start_pos = selStart;
					editable->selection_end_pos = selEnd;
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const
			{
				GtkWidget*	widget		=	Get_GtkWidget ();
				if (not windowRectArea.IsEmpty () and GTK_WIDGET_REALIZED (widget)) {
					Display*	display		=	GDK_WINDOW_XDISPLAY (widget->window);
					Drawable	drawable	=	GDK_WINDOW_XWINDOW (widget->window);
					#if		qPrintGLIBTraceMessages && 0
						g_message ("Entering Led_Gtk_Helper<>::RefreshWindowRect_ (%d,%d,%d,%d)", windowRectArea.GetTop (), windowRectArea.GetLeft (), windowRectArea.GetHeight (), windowRectArea.GetWidth ());
					#endif
					switch (RealUpdateMode (updateMode)) {
						case	eDelayedUpdate: {
							#if 1
								/*
								 *	Tried posting event rather than XClearArea () to avoid extra erase. So far - doesn't seem
								 *	to have fixed my problems - but lets investigate a bit further... LGP 2001-04-24.
								 */
								XEvent	event;
								(void)::memset (&event, 0, sizeof (event));
								event.type = Expose;
								event.xexpose.send_event = true;
								event.xexpose.display = display;
								event.xexpose.window = drawable;
								event.xexpose.x = (int)windowRectArea.GetLeft ();
								event.xexpose.y = (int)windowRectArea.GetTop ();
								event.xexpose.width = (int)windowRectArea.GetWidth ();
								event.xexpose.height = (int)windowRectArea.GetHeight ();
								event.xexpose.count = 0;
								Led_Verify (::XSendEvent (display, drawable, false, ExposureMask, &event) != 0);
							#else
								::XClearArea (display, drawable, (int)windowRectArea.GetLeft (), (int)windowRectArea.GetTop (),
									  (unsigned int)windowRectArea.GetWidth (), (unsigned int)windowRectArea.GetHeight (), true
									);
							#endif
						}
						break;
						case	eImmediateUpdate: {
							// Just draw directly. Would be NICE to be able to clear the updateRect as well - but I cannot see any good way todo that
							// with X/GDK
							const_cast<Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*> (this)->Handle_DrawingHelper (windowRectArea);
						}
						break;
					}
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::UpdateWindowRect_ (const Led_Rect& windowRectArea) const
			{
				/*
				 *	Gtk/Gdk event processing really deeply sucks here. All I want to do is process all
				 *	Expose/etc events. But NO.....
				 *
				 *	They don't give you this ability. There is a partially nearly halfway useful hack called
				 *	gdk_event_get_graphics_expose () - but it hangs waiting for such events if they are not already arrived.
				 *	Thats useless. Also - it doesn't grab REAL expose events - just graphics and noexpose events.
				 *
				 *				GtkWidget*      widget          =       Get_GtkWidget ();
				 *				GdkEvent*		e				=		NULL;
				 *				while ((e = gdk_event_get_graphics_expose (widget->window)) != NULL) {
				 *					gtk_widget_event (widget, e);
				 *					bool	done	=	(e->expose.count == 0);
				 *					gdk_event_free (e);
				 *					if (done) {
				 *						break;
				 *					}
				 *				}
				 *
				 *
				 *	Also - I tried grabbing ALL the events, handling the expose events, and then pushing hack the saved events:
				 *
				 *
				 *		vector<GdkEvent*>	savedEvents;
				 *		while (gdk_events_pending ()) {
				 *			GdkEvent*	e	=	gdk_event_get ();
				 *			if (e->any.type == GDK_EXPOSE || e->any.type == GDK_NO_EXPOSE) {
				 *				gtk_widget_event (widget, e);
				 *				bool breakNow	=	e->expose.count == 0;
				 *				gdk_event_free (e);
				 *				if (breakNow) {
				 *					break;
				 *				}
				 *			}
				 *			else {
				 *				savedEvents.push_back (e);
				 *			}
				 *		}
				 *		//	Now we must return the events back to the Gdk Q. SUCKY!!!
				 *		for (vector<GdkEvent*>::reverse_iterator i = savedEvents.rbegin (); i != savedEvents.rend (); ++i) {
				 *			gdk_event_put (*i);
				 *			gdk_event_free (*i);
				 *		}
				 *
				 *
				 *		Then - I tried a while (::XCheckIfEvent (display, &xevent, Static_ExposePredicate, reinterpret_cast<XPointer> (this))
				 *	loop. That didn't work either. Its very hard to debug why. I just gave up. There was no pressing need to get this
				 *	working properly. Maybe a future version of Gtk will address this.
				 */
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		bool	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::QueryInputKeyStrokesPending () const
			{
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		bool	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::OnCopyCommand_Before ()
			{
				size_t			selStart;
				size_t			selEnd;
				GetSelection (&selStart, &selEnd);
				if (selStart != selEnd)	{
					ReaderClipboardFlavorPackage::sPrivateClipData.clear ();	// clear the private clipboard...
					GtkWidget*		widget			=	Get_GtkWidget ();
					#if		qPrintGLIBTraceMessages
						Window		oldSelOwner	=	::XGetSelectionOwner (GDK_WINDOW_XDISPLAY (widget->window), kAtom_CLIPBOARD);
					#endif
					/*
					 *	Not sure why using the Led_GetTickCount () value tends to fail on some (X-server that comes with RedHat 6.1)
					 *	X-servers. Anyhow - GDK_CURRENT_TIME seems to work fine all the time (I've seen so far).
					 */
					#if		1
						guint32	time	=	GDK_CURRENT_TIME;
					#else
						guint32	time	=	LedTickCount2XTime (Led_GetTickCount ());
					#endif
					bool	result	=	 (gtk_selection_owner_set (widget, kAtom_CLIPBOARD, time));
					if (not result) {
						Led_BeepNotify ();
					}
					#if		qPrintGLIBTraceMessages
						if (result) {
							g_message ("Led_Gtk_Helper<>::OnCopyCommand_Before ()- gtk_selection_owner_set () SUCCEEDED");
						}
						else {
							Window		newSelOwner	=	::XGetSelectionOwner (GDK_WINDOW_XDISPLAY (widget->window), kAtom_CLIPBOARD);
							g_message ("Led_Gtk_Helper<>::OnCopyCommand_Before ()- FAILED to set_owner_selection- oldSelOwner=%d, newSelOwner=%d, myWnd=%d", oldSelOwner, newSelOwner, GDK_WINDOW_XWINDOW (widget->window));
						}
					#endif
					return result;
				}
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::OnCopyCommand_After ()
			{
				/*
				 *	At this point - the selection data (X-terminology - or clipboard data in my jargon)
				 *	has been stored in ReaderClipboardFlavorPackage::sPrivateClipData. If we get a SelectionRequest message,
				 *	then we just copy right out of that global variable.
				 */
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		bool	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::OnPasteCommand_Before ()
			{
				/*
				 *	First see if we already have the selection object. If YES - then our sPrivateClip data is already correct.
				 *	If not - then fetch the clipboard data from the true selection owner and stick it into our private clip.
				 */
				GtkWidget*		widget			=	Get_GtkWidget ();
				if (gdk_selection_owner_get (kAtom_CLIPBOARD) != widget->window) {
					guint32			time			=	LedTickCount2XTime (Led_GetTickCount ());

					/*
					 *	Get available formats.
					 */
					fCurServerClipFormats.clear ();
					gtk_selection_convert (widget, kAtom_CLIPBOARD, kAtom_TARGETS, time);
					gtk_main ();	// subevent loop - popped out of (via a gtk_main_quit () call) in Handle_selection_received ()

					if (fCurServerClipFormats.size () == 0) {
						// always at least try plain text... some old X-clients may not support "TARGETS" type....
						fCurServerClipFormats.insert (kTEXTClipFormat);
					}

					/*
					 *	Clear the private clipboard cuz Handle_selection_received () will append data to private clipboard.
					 *
					 *	Then - loop through all the fCurServerClipFormats we just retrieved a second ago, and ask for each type, and use that
					 *	data to fill in our privateclipdata buffer.
					 */
					ReaderClipboardFlavorPackage::sPrivateClipData.clear ();	// 
					for (set<GdkAtom>::const_iterator i = fCurServerClipFormats.begin (); i != fCurServerClipFormats.end (); ++i) {
						gtk_selection_convert (widget, kAtom_CLIPBOARD, *i, time);
						gtk_main ();	// subevent loop - popped out of (via a gtk_main_quit () call) in Handle_selection_received ()
					}
					return true;
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::OnPasteCommand_After ()
			{
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_selection_get (GtkWidget* widget, GtkSelectionData *selection_data, guint info, guint time)
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Led_Gtk_Helper<>::Static_selection_get (selection=%s,target=%s)", gdk_atom_name (selection_data->selection), gdk_atom_name (selection_data->target));
				#endif
				if (selection_data->selection == kAtom_CLIPBOARD) {
					/*
					 *	Callback called from gtk - indirectly as the result of a SelectionRequest event (from somebody watching/wanting
					 *	the clipboard data). Grab the latest private clipboard data.
					 */
					ReaderClipboardFlavorPackage	internalClip;
					if (internalClip.GetFlavorAvailable (selection_data->target)) {
						size_t						bufSize	=	internalClip.GetFlavorSize (selection_data->target);
						Led_SmallStackBuffer<char>	buf (bufSize);
						bufSize = internalClip.ReadFlavorData (selection_data->target, bufSize, buf);
						gtk_selection_data_set (selection_data, selection_data->target, 8*sizeof(gchar), (guchar*)(char *)buf, bufSize);
						#if		qPrintGLIBTraceMessages
							g_message ("\tHad the data - and returend it");
						#endif
					}
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_selection_received  (GtkWidget* widget, GtkSelectionData* selection_data, guint time)
			{
				WidgetToTHIS (widget)->Handle_selection_received (selection_data, time);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_selection_received  (const GtkSelectionData* selection_data, guint time)
			{
				/*
				 *	This is a callback from the X-Windows system in response to our having called gtk_selection_convert () - requuested
				 *	clipboard data in a particular format. If we get a good response - we add the data of that type to our private
				 *	clipboard.
				 */
				Led_RequireNotNil (selection_data);
				#if		qPrintGLIBTraceMessages
					g_message ("Led_Gtk_Helper<>::Handle_selection_received (selection=%s, target=%s, dataLen=%d)", gdk_atom_name (selection_data->selection), gdk_atom_name (selection_data->target), selection_data->length);
				#endif
				if (selection_data->target == kAtom_TARGETS) {
					// we are receiving the answer to the TARGETS request (in _getFormats())
					fCurServerClipFormats.clear ();
					#if		qPrintGLIBTraceMessages
						g_message ("Led_Gtk_Helper<>::Handle_selection_received: TARGETS:");
					#endif
					if (selection_data->data != NULL and selection_data->length > 0 and selection_data->type == GDK_SELECTION_TYPE_ATOM) {
						GdkAtom*	atomP	=	reinterpret_cast<GdkAtom *> (selection_data->data);
						size_t		count	=	selection_data->length / sizeof(GdkAtom);
						for (size_t i = 0; i < count; ++i) {
							/*
							 *	Ignore a few atom types often returned, which just confuse matters.
							 */
							if (atomP[i] == kAtom_TARGETS) { continue; }
							if (atomP[i] == kAtom_TIMESTAMP) { continue; }
							if (atomP[i] == kAtom_MULTIPLE) { continue; }

							fCurServerClipFormats.insert (atomP[i]);
							#if		qPrintGLIBTraceMessages
								g_message ("\t\t%s", gdk_atom_name (atomP[i]));
							#endif
						}
					}
				}
				else if (selection_data->data != NULL and selection_data->length > 0) {
					WriterClipboardFlavorPackage	privateClipWriter;
					privateClipWriter.AddFlavorData (selection_data->target, selection_data->length, selection_data->data);
				}
				gtk_main_quit ();	// matching 'gtk_main ()' loop from Led_Gtk_Helper<>::OnPasteCommand_Before
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_Draw (GtkWidget* widget, GdkRectangle* area)
			{
				WidgetToTHIS (widget)->Handle_Draw (*area);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_Draw (const GdkRectangle& area)
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Entering Led_Gtk_Helper<>::Handle_Draw (%d,%d,%d,%d)", area.y, area.x, area.height, area.width);
				#endif
				Handle_DrawingHelper (Led_Rect (area.y, area.x, area.height, area.width));
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_Realize (GtkWidget* widget)
			{
				WidgetToTHIS (widget)->Handle_Realize ();
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_Realize ()
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Entering Led_Gtk_Helper<>::Handle_Realize");
				#endif
				GtkWidget*	widget		=	Get_GtkWidget ();

				GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

				GdkWindowAttr	attributes;
				attributes.window_type = GDK_WINDOW_CHILD;
				attributes.x = widget->allocation.x;
				attributes.y = widget->allocation.y;
				attributes.width = widget->allocation.width;
				attributes.height = widget->allocation.height;
				attributes.wclass = GDK_INPUT_OUTPUT;
				attributes.visual = gtk_widget_get_visual (widget);
				attributes.colormap = gtk_widget_get_colormap (widget);
				attributes.event_mask = gtk_widget_get_events (widget);
				attributes.event_mask |= (GDK_EXPOSURE_MASK |
											GDK_BUTTON_PRESS_MASK |
											GDK_BUTTON_RELEASE_MASK |
											GDK_BUTTON_MOTION_MASK |
											GDK_ENTER_NOTIFY_MASK |
											GDK_LEAVE_NOTIFY_MASK |
											GDK_KEY_PRESS_MASK
										);
				gint	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

				attributes.cursor = gdk_cursor_new (GDK_XTERM);
				attributes_mask |= GDK_WA_CURSOR;

				widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, attributes_mask);
				gdk_window_set_user_data (widget->window, widget);
				gdk_cursor_destroy (attributes.cursor); /* The X server will keep it around as long as necessary */

				widget->style = gtk_style_attach (widget->style, widget->window);

				gdk_window_set_background (widget->window, &widget->style->base[GTK_WIDGET_STATE (widget)]);

				Layout ();
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_size_allocate (GtkWidget* widget, GtkAllocation* allocation)
			{
				WidgetToTHIS (widget)->Handle_size_allocate (*allocation);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_size_allocate (const GtkAllocation& allocation)
			{
				GtkWidget*		widget		=	Get_GtkWidget ();
				widget->allocation = allocation;
				if (GTK_WIDGET_REALIZED (widget)) {
					gdk_window_move_resize (widget->window, allocation.x, allocation.y, allocation.width, allocation.height);
					Layout ();
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_FocusIn (GtkWidget* widget, GdkEventFocus* event)
			{
				return WidgetToTHIS (widget)->Handle_FocusIn (*event);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_FocusIn (const GdkEventFocus& event)
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Entering Led_Gtk_Helper<>::Handle_FocusIn");
				#endif
// MUST ADD CODE a FOCUS_RECT drawing???
				GtkWidget*		widget		=	Get_GtkWidget ();
				GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);
				SetCaretShown (true);
				SetSelectionShown (true);
				StartBlinkCaretTimer ();
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_FocusOut (GtkWidget* widget, GdkEventFocus* event)
			{
				return WidgetToTHIS (widget)->Handle_FocusOut (*event);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_FocusOut (const GdkEventFocus& event)
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Entering Led_Gtk_Helper<>::Handle_FocusOut");
				#endif
// MUST ADD CODE a FOCUS_RECT drawing???
				BreakInGroupedCommands ();

				GtkWidget*		widget		=	Get_GtkWidget ();
				GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);
				SetCaretShown (false);
				SetSelectionShown (false);
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_ButtonPress (GtkWidget* widget, GdkEventButton* event)
			{
				return WidgetToTHIS (widget)->Handle_ButtonPress (*event);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_ButtonPress (const GdkEventButton& event)
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Entering Led_Gtk_Helper<>::Handle_ButtonPress");
				#endif
				SyncronizeLedXTickCount (event.time);
				BreakInGroupedCommands ();

				/*
				 *	Could have used Led's TextInteractor::UpdateClickCount () - but seems easier and probably more appropriate to use clickcount computed by gtk.
				 *		//	UpdateClickCount (event.time/1000.0f, Led_Point (event.y, event.x));
				 */
				if (event.button == 1) {
					switch (event.type) {
						case	GDK_BUTTON_PRESS:		SetCurClickCount (1, event.time/1000.0f); break;
						case	GDK_2BUTTON_PRESS:		SetCurClickCount (2, event.time/1000.0f); break;
						case	GDK_3BUTTON_PRESS:		SetCurClickCount (3, event.time/1000.0f); break;
						default:	Led_Assert (false);	SetCurClickCount (1, event.time/1000.0f); break;
					}

					Led_Point	at			=	Led_Point (static_cast<Led_Coordinate> (event.y), static_cast<Led_Coordinate> (event.x));
					GtkWidget*	widget		=	Get_GtkWidget ();

					fMouseTrackingLastPoint = at;
					if (not GTK_WIDGET_HAS_FOCUS (widget)) {
						gtk_widget_grab_focus (widget);
					}

					bool	extendSelection	=	event.state & GDK_SHIFT_MASK;

					if (not ProcessSimpleClick (at, GetCurClickCount (), extendSelection, &fDragAnchor)) {
						return false;
					}

					/*
					 *	Note that we do this just AFTER setting the first selection. This is to avoid
					 *	(if we had done it earlier) displaying the OLD selection and then quickly
					 *	erasing it (flicker). We do an Update () rather than simply use eImmediateUpdate
					 *	mode on the SetSelection () because at this point we want to redisplay the entire
					 *	window if part of it needed it beyond just the part within the selection.
					 */
					Update ();

					gtk_grab_add (widget);
					StartAutoscrollTimer ();

					Led_Assert (GetCurClickCount () >= 1);
					Led_Assert (fDragAnchor <= GetEnd ());	// Subtle point. fDragAnchor is passed as a hidden variable to
															// other routines - but ONLY when we have the CAPTURE. If the capture
															// is not set, then 'fDragAnchor' is assumed to have an invalid value,
															// This implies that text shouldn't be changed while we have the mouse
															// captured.

				}
				else {
					SetCurClickCount (0, event.time/1000.0f);
				}
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_MouseMoved (GtkWidget* widget, GdkEventMotion* event)
			{
				return WidgetToTHIS (widget)->Handle_MouseMoved (*event);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_MouseMoved (const GdkEventMotion& event)
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Entering Led_Gtk_Helper<>::Handle_MouseMoved");
				#endif
				SyncronizeLedXTickCount (event.time);
				GtkWidget*	widget		=	Get_GtkWidget ();
				Led_Point	at			=	Led_Point (static_cast<Led_Coordinate> (event.y), static_cast<Led_Coordinate> (event.x));
				if (event.is_hint or (widget->window != event.window)) {
					gint x, y;
					GdkModifierType mask;
					gdk_window_get_pointer (widget->window, &x, &y, &mask);
					at = Led_Point (y, x);
				}
				fMouseTrackingLastPoint =	at;		// save for OnTimer_Msg ()
				if (GTK_WIDGET_HAS_GRAB (widget)) {
						// Skip doing autotracking if pending expose events still not handled - to prevent us from falling behind...
						{
							XEvent e;
							Display*	display		=	GDK_WINDOW_XDISPLAY (widget->window);
							if (::XCheckTypedEvent (display, Expose, &e) or ::XCheckTypedEvent (display, GraphicsExpose, &e)) {
								::XPutBackEvent (display, &e);
								return false;
							}
						}
					WhileSimpleMouseTracking (at, fDragAnchor);
				}
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_ButtonRelease (GtkWidget* widget, GdkEventButton* event)
			{
				return WidgetToTHIS (widget)->Handle_ButtonRelease (*event);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_ButtonRelease (const GdkEventButton& event)
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Entering Led_Gtk_Helper<>::Handle_ButtonRelease");
				#endif
				SyncronizeLedXTickCount (event.time);
				GtkWidget*	widget		=	Get_GtkWidget ();
				if (GTK_WIDGET_HAS_GRAB (widget)) {
					gtk_grab_remove (widget);
					StopAutoscrollTimer ();
					WhileSimpleMouseTracking (Led_Point (static_cast<Led_Coordinate> (event.y), static_cast<Led_Coordinate> (event.x)), fDragAnchor);
				}
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_ScrollTimeout (gpointer data)
			{
				Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*	widget	=	reinterpret_cast<Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*> (data);
				return widget->Handle_ScrollTimeout ();
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_ScrollTimeout ()
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Handle_ScrollTimeout");
				#endif
				GtkWidget*	widget		=	Get_GtkWidget ();
				GDK_THREADS_ENTER ();
				{
					if (GTK_WIDGET_HAS_GRAB (widget)) {
						// Skip doing autotracking if pending expose events still not handled - to prevent us from falling behind...
						{
							XEvent e;
							Display*	display		=	GDK_WINDOW_XDISPLAY (widget->window);
							if (::XCheckTypedEvent (display, Expose, &e) or ::XCheckTypedEvent (display, GraphicsExpose, &e)) {
								::XPutBackEvent (display, &e);
								goto End;
							}
						}
						// Since we have the capture, we got the last mouse moved event, and so we have a VALID value stored in
						// fMouseTrackingLastPoint for the mouse location. Use that.
						WhileSimpleMouseTracking (fMouseTrackingLastPoint, fDragAnchor);
						StartAutoscrollTimer ();
					}
				}
			End:
				GDK_THREADS_LEAVE ();
 				return true;	// keep calling
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_CaretBlinkTimer (gpointer data)
			{
				Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*	widget	=	reinterpret_cast<Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>*> (data);
				return widget->Handle_CaretBlinkTimer ();
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_CaretBlinkTimer ()
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Entering Handle_CaretBlinkTimer (): selShown=%s", GetSelectionShown ()?"yes":"no");
				#endif
				GDK_THREADS_ENTER ();
				if (GetSelectionShown ()) {
					BlinkCaretIfNeeded ();
				}
				GDK_THREADS_LEAVE ();
 				return true;	// keep calling
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::BlinkCaretIfNeeded ()
			{
				#if		qPrintGLIBTraceMessages && 0
					g_message ("Entering BlinkCaretIfNeeded (): selShownAndNeeded=%s, now=%f, fTickCountAtLastBlink=%f", (GetCaretShown () and (GetSelectionStart () == GetSelectionEnd ()))?"yes":"no", Led_GetTickCount (), fTickCountAtLastBlink);
				#endif
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
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::DrawCaret_ ()
			{
				DrawCaret_ (fLastDrawnOn);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::DrawCaret_ (bool on)
			{
				typename BASE_INTERACTOR::Tablet_Acquirer	tablet (this);
				Led_AssertNotNil (static_cast<Led_Tablet> (tablet));
				// Note: the reason we don't just use the GetCaretShown () flag here, and
				// instead we check that the selection is empty is because we want to reserve that
				// flag for users (client programmers) use. If we used it internally (like in SetSelection()
				// then the users values would get overwritten...
				Led_Assert (GetCaretShown ());
				if (GetCaretShownSituation ()) {
					Led_Rect	caretRect	=	CalculateCaretRect ();
					fLastDrawnOn = on;
					if (on) {
						tablet->EraseBackground_SolidHelper (caretRect, Led_Color::kBlack);
					}
					else {
						EraseBackground (tablet, caretRect, false);
					}
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::StartBlinkCaretTimer ()
			{
				if (fBlinkCaretTimerID == 0) {
					const	int	kTimeout	=	static_cast<int> (GetTickCountBetweenBlinks () * 1000);
					Led_Verify (fBlinkCaretTimerID = ::gtk_timeout_add (kTimeout, Static_CaretBlinkTimer, this));
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::StopBlinkCaretTimer ()
			{
				if (fBlinkCaretTimerID != 0) {
					::gtk_timeout_remove (fBlinkCaretTimerID);
					fBlinkCaretTimerID = 0;
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		bool	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::CheckIfDraggingBeepAndReturn ()
			{
				GtkWidget*	widget		=	Get_GtkWidget ();
				if (GTK_WIDGET_HAS_GRAB (widget)) {
					// we must be tracking - drop characters typed at that point on the floor
					// send a beep message as well to indicate that the characters are being dropped!
					OnBadUserInput ();
					return true;
				}
				return false;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::StartAutoscrollTimer ()
			{
				/*
				 *	Not sure about this - just for debug sake??? - Did LButonUp get dropped on the floor somehow? - LGP 960530
				 */
	// disable assert for now - LGP 2001-05-02			Led_Assert (fAutoScrollTimerID == 0);
				if (fAutoScrollTimerID == 0) {
					const	int	kTimeout	=	20;	// 20 milliseconds - update autoscroll every 1/50
													// second. (nb: equiv to SCROLL_TIME which gkttext.c declares as 100).
					Led_Verify (fAutoScrollTimerID = ::gtk_timeout_add (kTimeout, Static_ScrollTimeout, this));
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::StopAutoscrollTimer ()
			{
				if (fAutoScrollTimerID != 0) {
					::gtk_timeout_remove (fAutoScrollTimerID);
					fAutoScrollTimerID = 0;
				}
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Static_key_press (GtkWidget* widget, GdkEventKey* event)
			{
				return WidgetToTHIS (widget)->Handle_key_press (*event);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		gint	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::Handle_key_press (const GdkEventKey& event)
			{
				#if		qPrintGLIBTraceMessages
					g_message ("Handle_key_press");
				#endif
				SyncronizeLedXTickCount (event.time);

				bool	shiftPressed	=	event.state & GDK_SHIFT_MASK;
				bool	controlPressed	=	event.state & GDK_CONTROL_MASK;
				bool	altKeyPressed	=	event.state & GDK_MOD1_MASK;

				switch (event.keyval) {
					case	GDK_Page_Up: {		// page UP
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();

						if (controlPressed) {
							SetSelection (GetMarkerPositionOfStartOfWindow (), GetMarkerPositionOfStartOfWindow ());
						}
						else {
							// NB: this isn't QUITE right for pageup - with differing height rows!!!!

							// Do the actual scrolling - this is the only part that makes any sense!
							ScrollByIfRoom (-(int)GetTotalRowsInWindow ());
						}
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_Page_Down: {		// page DOWN
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();

						if (controlPressed) {
							SetSelection (GetMarkerPositionOfEndOfWindow (), GetMarkerPositionOfEndOfWindow ());
						}
						else {
							ScrollByIfRoom (GetTotalRowsInWindow ());
						}
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_End: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();
						DoSingleCharCursorEdit (eCursorToEnd, controlPressed? eCursorByBuffer: eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving,
								qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
							);
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_Home: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();
						DoSingleCharCursorEdit (eCursorToStart, controlPressed? eCursorByBuffer: eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving,
								qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
							);
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_Left: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();
						DoSingleCharCursorEdit (eCursorBack, controlPressed? eCursorByWord: eCursorByChar, shiftPressed? eCursorExtendingSelection: eCursorMoving,
								qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
							);
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_Up: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();
						if (controlPressed) {
							DoSingleCharCursorEdit (eCursorToStart, eCursorByLine, shiftPressed? eCursorExtendingSelection: eCursorMoving,
									qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
								);
						}
						else {
							DoSingleCharCursorEdit (eCursorBack, eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving,
									qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
								);
						}
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_Right: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();
						DoSingleCharCursorEdit (eCursorForward, controlPressed? eCursorByWord: eCursorByChar, shiftPressed? eCursorExtendingSelection: eCursorMoving,
								qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
							);
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_Down: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}
						BreakInGroupedCommands ();
						if (controlPressed) {
							DoSingleCharCursorEdit (eCursorToEnd, eCursorByLine, shiftPressed? eCursorExtendingSelection: eCursorMoving,
								qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
							);
						}
						else {
							DoSingleCharCursorEdit (eCursorForward, eCursorByRow, shiftPressed? eCursorExtendingSelection: eCursorMoving,
								qPeekForMoreCharsOnUserTyping? eDefaultUpdate: eImmediateUpdate
							);
						}
						#if		qPeekForMoreCharsOnUserTyping
							UpdateIfNoKeysPending ();
						#endif
						return true;
					}
					break;

					case	GDK_Clear: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}

						BreakInGroupedCommands ();
						OnClearCommand ();
						return true;
					}
					break;

					case	GDK_Delete: {
						if (CheckIfDraggingBeepAndReturn ()) {	return true;	}

						BreakInGroupedCommands ();

						/*
						 *	If the selection is empty, then delete the following character (if any)
						 *	and if it is non-empty - simply delete its contents.
						 */
						if (GetSelectionStart () == GetSelectionEnd ()) {
							// note this doesn't change the selection - since we only delete following
							// the selection...
							//
							// Also note that we count on the fact that it is safe toocall FindNextCharacter ()
							// at the buffers end and it will pin to that end...
							SetSelection (GetSelectionStart (), FindNextCharacter (GetSelectionStart ()), eDefaultUpdate);
							InteractiveReplace (LED_TCHAR_OF (""), 0);
						}
						else {
#if 1
							OnPerformCommand (kClear_CmdID);
#else
							size_t	newSelection	=	GetSelectionStart ();
							InteractiveReplace (LED_TCHAR_OF (""), 0);
							SetSelection (newSelection, newSelection);
#endif
						}
						ScrollToSelection ();
						Update ();
						return true;
					}
					break;

					default: {
						/*
						 *	See gtktext.c - and handle control/alt keys similarly - mapping them to forward/back etc. cmds
						 *
						 */
						if (controlPressed) {
							return false;	// ret TRUE iff handled... - as a recongized cmd-char...
						}
						else if (altKeyPressed) {
							return false;	// ret TRUE iff handled... - as a recongized altKey-char...
						}
						else if (event.length > 0) {
							struct	GdkEventKey_BWA {
								GdkEventType	type;
								GdkWindow*		window;
								gint8			send_event;
								guint32			time;
								guint			state;
								guint			keyval;
								gint			length;
								gchar*			xstring;
							};
							Led_Assert (sizeof (GdkEventKey_BWA) == sizeof (GdkEventKey));	// Hack/BWA cuz accessing .string field of this structure barfed on GCC...
							const GdkEventKey_BWA& e	=	*reinterpret_cast<const GdkEventKey_BWA*> (&event);
							/*
							 * it appears an GdkEventKey may contain an entire string typed in all at once - if coming from an IME.
							 *	So - just pass along each char successively.
							 */
							for (size_t i = 0; i < e.length; ++i) {
								Led_tChar	c	=	e.xstring[i];
								if (c == '\r') {
									c = '\n';
								}
								OnTypedNormalCharacter (c, false, false, false, false, false);
							}
							return true;
						}
						else {
							return false;
						}
					}
					break;
				}
				Led_Assert (false);	/*NOTREACHED*/
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		struct Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::GtkLedEditorClass {
			typename GTKBASEINFO::BaseMetaClass parent_class;
		};

	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		GtkType	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::gtk_LedEditor_get_type ()
			{
				static	GtkType LedEditor_type = 0;

				if (!LedEditor_type) {
					static const GtkTypeInfo kLedEditor_info = {
						"GtkLedEditor",
						sizeof (Led2GTKMapper),
						sizeof (GtkLedEditorClass),
						(GtkClassInitFunc) gtk_LedEditor_class_init,
						(GtkObjectInitFunc) gtk_LedEditor_init,
						/* reserved_1 */ NULL,
						/* reserved_2 */ NULL,
						(GtkClassInitFunc) NULL,
					};
					LedEditor_type = gtk_type_unique (GTKBASEINFO::GetBaseTypeId (), &kLedEditor_info);
				}

				return LedEditor_type;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::gtk_LedEditor_class_init (GtkLedEditorClass *widgetClass)
			{
				GtkObjectClass*		object_class		=	reinterpret_cast<GtkObjectClass*> (widgetClass);
				GtkWidgetClass*		widget_class		=	reinterpret_cast<GtkWidgetClass*> (widgetClass);
				object_class->destroy = Static_Destroy;
				widget_class->expose_event = Static_expose;
				widget_class->key_press_event = Static_key_press;
				widget_class->draw = Static_Draw;
				widget_class->realize = Static_Realize;
				widget_class->size_allocate = Static_size_allocate;
				widget_class->focus_in_event = Static_FocusIn;
				widget_class->focus_out_event = Static_FocusOut;
				widget_class->button_press_event = Static_ButtonPress;
				widget_class->motion_notify_event = Static_MouseMoved;
				widget_class->button_release_event = Static_ButtonRelease;
				widget_class->selection_get = Static_selection_get;
				widget_class->selection_received = Static_selection_received;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		void	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::gtk_LedEditor_init (Led2GTKMapper *image)
			{
				/*
				 *	Not entirely sure why this is better to use NO_WINDOW approach. But when I used a window - I got flicker -
				 *	from the background classes drawing. Instead - I mimiced what the gtktext class did to avoid the flicker - 
				 *	creating a separate window and saying GTK_NO_WINDOW).
				 *
				 *		GTK_WIDGET_SET_FLAGS (image, GTK_NO_WINDOW);
				 */
				GTK_WIDGET_SET_FLAGS (image, GTK_CAN_FOCUS);
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
		GtkWidgetClass*	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::gtk_GetParentClass ()
			{
				GtkWidgetClass*	parentClass	=	reinterpret_cast<GtkWidgetClass*> (gtk_type_class (GTKBASEINFO::GetBaseTypeId ()));
				return parentClass;
			}
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
			GdkAtom	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::kAtom_TARGETS		=	None;
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
			GdkAtom	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::kAtom_CLIPBOARD	=	None;
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
			GdkAtom	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::kAtom_TIMESTAMP	=	None;
	template	<typename	BASE_INTERACTOR, typename GTKBASEINFO>
			GdkAtom	Led_Gtk_Helper<BASE_INTERACTOR,GTKBASEINFO>::kAtom_MULTIPLE		=	None;





//	class	Gtk_CommandNumberMapping
	inline	Gtk_CommandNumberMapping::Gtk_CommandNumberMapping ()
		{
		}




//	class	Led_Gtk_TmpCmdUpdater
	inline	Led_Gtk_TmpCmdUpdater::operator Led_Gtk_TmpCmdUpdater* ()
		{
			return this;
		}
	inline	Led_Gtk_TmpCmdUpdater::CommandNumber	Led_Gtk_TmpCmdUpdater::GetCmdID () const
		{
			return fCommand;
		}






#if		qLedUsesNamespaces
}
#endif




#endif	/*__Led_Gtk_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
