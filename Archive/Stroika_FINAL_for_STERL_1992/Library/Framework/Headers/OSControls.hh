/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__OSControls__
#define	__OSControls__	1

/*
 * $Header: /fuji/lewis/RCS/OSControls.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * NOTE:
 *
 *			In MotifOSControls, When we refer to REALIZED here we mean that WE'VE BUILT the widget. This is ANALOGOUS
 *			to the Xt usage, but not the same!!!
 *
 *
 * Changes:
 *	$Log: OSControls.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  06:03:26  sterling
 *		Changed CalcDefaultSize to CalcDefaultSize_ to react to View change.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.28  1992/04/17  01:15:24  lewis
 *		Added code to MotifOScontrol to try patchig GCs of owned views. Doomed to fail. Never really finished, but I think its
 *		almost hopeless.
 *
 *		Revision 1.27  92/04/08  15:48:46  15:48:46  lewis (Lewis Pringle)
 *		Get rid of MagicView/MagicWidget for motif os controls.
 *		
 *		Revision 1.26  92/03/26  09:36:15  09:36:15  lewis (Lewis Pringle)
 *		Simplified args to EffectiveLive/Visibility/Font changed - no old value, just the new value passed.
 *		
 *		Revision 1.25  1992/03/24  02:28:50  lewis
 *		Made CalcDefaultSize () const.
 *
 *		Revision 1.24  1992/03/19  16:53:29  lewis
 *		Change sRealMode from Boolean to int keeping depth, allowing nested calls to real/unreal mode.
 *
 *		Revision 1.23  1992/03/16  17:17:02  lewis
 *		Override EffectiveVisibilityChanged () to call XtMap/UnMap Widget.
 *
 *		Revision 1.22  1992/03/09  23:54:27  lewis
 *		Use new HandleKeyStroke () interface.
 *		Use new conditional compile macro qXmToolkit instead of qMotifToolkit.
 *
 *		Revision 1.21  1992/01/31  05:16:46  lewis
 *		Added support for patching the Xt expose proc.
 *		This has been a major step forward in fixing the various display problems - we still have a ways to
 *		go, but things look MUCH better.
 *
 *		Revision 1.20  1992/01/23  19:47:12  lewis
 *		Make realize/unrealize stuff public so owners can access it.
 *
 *		Revision 1.19  1992/01/23  08:03:41  lewis
 *		Got rid of bogus widget support - seems too hard. Using sterls design where each widget
 *		wrapper (eg native pushbutton) is in charge of keeping values 'til were realized.
 *		Got rid of lots of inappropriate wrappers, liek SetTitle, etc... Just do in particular widget wrapper.
 *
 *		Revision 1.18  1992/01/23  04:18:52  lewis
 *		Added ability to disable building the magic view which is needed in menus since they peek
 *		at their parent widget.
 *
 *		Revision 1.16  1992/01/20  04:22:16  lewis
 *		Added Get/Set Unsinged Char Resoruce Value procs.
 *
 *		Revision 1.13  1992/01/14  23:26:38  lewis
 *		Added magic widget owner for motif controls.
 *
 *		Revision 1.12  1992/01/10  03:08:08  lewis
 *		Added OSControls::CalcDefaultSize for motif.
 *		Added sensative boolean so we can stash it away when were unrealized - just hack til we do
 *		full cloning... Also, moved stuff from Layout to AdjustWidgetExtent so it can be shared (called from
 *		Draw).
 *
 *		Revision 1.11  1992/01/03  04:05:34  lewis
 *		Added accessors for resource values.
 *
 *		Revision 1.10  1991/12/27  16:51:04  lewis
 *		Start working on added more virtual method callbacks.
 *
 */

#include	"KeyHandler.hh"
#include	"View.hh"


#if		qMacToolkit
class	MacOSControl : public View {
	public:
		MacOSControl (short procID);
		virtual ~MacOSControl ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		override	void	Layout ();
		override	void	Draw (const Region& update);

		nonvirtual	UInt8	GetHiliteState () const;
		virtual		void	SetHiliteState (UInt8 hiliteState, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Int16	GetValue () const;
		virtual		void	SetValue (Int16 value, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Int16	GetMin () const;
		virtual		void	SetMin (Int16 min, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Int16	GetMax () const;
		virtual		void	SetMax (Int16 max, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	String	GetTitle () const;
		override	void	SetTitle (const String& title, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	struct	osControlRecord**	GetOSRepresentation () const;

	protected:
		/*
		 * override (and ususally call inherited) to provide behavior while tracking
		 */
		virtual	void	ActionProc (short partCode);
		virtual	void	DoneProc (short partCode);		// called when done with tracking

	private:
		struct	osControlRecord**	fOSRep;
		short	fProcID;

		nonvirtual	void	PrepareForDraw (UpdateMode updateMode);
		nonvirtual	void	CleanupAfterDraw ();

		// for implementing tracking
		static	MacOSControl*	sCurrentlyTracked;
		static	pascal	void	ThumbActionProc ();
		static	pascal	void	DragActionProc (struct osControlRecord** ctl, short partCode);
};

#elif	qXtToolkit

// If any of these accessor methods dont make sense, then ignore them - this class is solely
// a utility class designed for code sharing...
class	MotifOSControl : public View, public KeyHandler {
	public:
		MotifOSControl ();
		virtual ~MotifOSControl ();

		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);

		override	Boolean	HandleKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
											KeyComposeState& composeState);


		override	void	Layout ();
		override	void	Draw (const Region& update);

// Be cautious about calling these low level routines...
		nonvirtual	osWidget*	GetOSRepresentation () const;		// can be nil of unrealized
		nonvirtual	void		GetResourceValue (const char* resourceID, void* xtArgValue, size_t sizeVal) const;
		nonvirtual	void		SetResourceValue (const char* resourceID, long xtArgValue);// xtArgType must be same type as X's XtArgType!
		nonvirtual	Boolean		GetBooleanResourceValue (const char* resourceID) const;
		nonvirtual	void		SetBooleanResourceValue (const char* resourceID, Boolean value);
		nonvirtual	UInt16		GetDimensionResourceValue (const char* resourceID) const;// be sure type of value same as that of Xt Dimension
		nonvirtual	void		SetDimensionResourceValue (const char* resourceID, UInt16 value);
		nonvirtual	UInt8		GetUnsignedCharResourceValue (const char* resourceID) const;
		nonvirtual	void		SetUnsignedCharResourceValue (const char* resourceID, UInt8 value);
		nonvirtual	int			GetIntResourceValue (const char* resourceID) const;
		nonvirtual	void		SetIntResourceValue (const char* resourceID, int value);
		nonvirtual	String		GetStringResourceValue (const char* resourceID) const;
		nonvirtual	void		SetStringResourceValue (const char* resourceID, const String& value);


// make public cuz owners want to call these directly...
public:
override	osWidget*	GetWidget () const;// for view/realize support
override	void	Realize (osWidget* parent);
override	void	UnRealize ();


	protected:
		override	Point	CalcDefaultSize_ (const Point& defaultSize) const;
		override	void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);

		virtual	void	AdjustWidgetExtent (UpdateMode updateMode = eDelayedUpdate);

		virtual	osWidget*	DoBuild_ (osWidget* parent)		=	Nil;		// actually build the guy...

		override	void	EffectiveVisibilityChanged (Boolean newVisible, UpdateMode updateMode);


	/*
	 * Code to patch owned GCs. This is experimental, highly dangerous, and very unlikely to succeed,
	 * but what choice is there????? My Hack to have magic views associated with the controls is inadequate,
	 * putting those hack widgets into View doesn't deal with the problem that we sometimes have additional
	 * clipping beyond the visible area due to parameters to the Update() function (this is important because
	 * failure to handle this properly results in drawing that overlaps the widget to be obliterated SOMETIMES -
	 * when an Draw () is called that overlaps PART of the widget, but not the things that overlap the widget - eg
	 * handles for resizing of Emily PushButton).
	 *
	 * Idea is to have a hook proc that patches, and unpatches GCs for the owned widget. We provide here some
	 * common cases, but subclasses must decide how to compose them, and get the right gcs patched. Very ugly,
	 * slow, and error prone - Must move away from native widgets for motif - they TRUELY SUCK WIND!!!
	 *
	 * My hope level on this is very low - almost not worth trying - Labels call XClearArea which has no
	 * GC, and therefore no clipping, and PushButtons dont really ever use their own gc, they dynamically
	 * create one based on their parent. We could work around the later problem, but I think portable implementations
	 * would be MUCH easier.
	 */
	protected:
		virtual		void	PatchGCs ();		// by default does nothing - subclasses must handle this
		virtual		void	UnPatchGCs ();		// by default does nothing - subclasses must handle this

	protected:
		nonvirtual	void	PatchPrimitvesGCs ();
		nonvirtual	void	UnPatchPrimitvesGCs ();
	private:
		struct _XGC;
		_XGC*	fSavedHighlightGC;
		_XGC*	fSavedTopShaddowGC;
		_XGC*	fSavedBottomShaddowGC;


	private:
		osWidget*		fWidget;

		static	void	ReplacementExposeProc (osWidget* w, osEventRecord* event, osRegion*);

		static	UInt8	sRealModeDepth;
		static	void	GoReal ();
		static	void	GoUnReal ();
};

#endif	/*Toolkit*/





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */

#if		qMacToolkit
inline	osControlRecord**	MacOSControl::GetOSRepresentation () const		{ EnsureNotNil (fOSRep); return (fOSRep); }
#elif	qXmToolkit
inline	osWidget*			MotifOSControl::GetOSRepresentation () const    { return (fWidget); }
#endif	/*Toolkit*/


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__OSControls__*/
