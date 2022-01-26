/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__View__
#define	__View__

/*
 * $Header: /fuji/lewis/RCS/View.hh,v 1.8 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 *		Tablets:		Views have a notion of a current tablet (they inherit that from panel).
 *						Setting a views current
 *						tablet sets that of its subviews.  Also, we provide simple shape
 *						draw methods which have an implicit tablet.  SHOULD WE DO THIS
 *						WITH WINDOWS INSTEAD OF TABLETS?????
 *
 * Changes:
 *	$Log: View.hh,v $
 *		Revision 1.8  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.7  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.6  1992/07/21  20:55:17  sterling
 *		Use Sequence instead of obsolete SequencePtr.
 *
 *		Revision 1.5  1992/07/21  19:59:01  sterling
 *		changed qGUI to qUI, supported qWinUI
 *
 *		Revision 1.4  1992/07/04  14:12:48  lewis
 *		Got rid of ScaledShape - just put logical size into Shape. Did this mainly
 *		cuz Sterl had wanted it for a while, and I'm gonna be making lots of
 *		other changes to Shape soon - At same time did letter envelope change.
 *
 *		Revision 1.3  1992/07/02  04:45:18  lewis
 *		Renamed Sequence_DoublyLLOfPointers->SequencePtr.
 *
 *		Revision 1.2  1992/06/25  07:23:19  sterling
 *		Renamed CalcDefaultSize to CalcDefaultSize_ as virtual and did nonvirtual CalcDefaultSize to call it.
 *		Mixed in Bordered, and override SetBorder/Margin/Plane. And added a bunch of drawing utils.
 *		(LGP checked in for Sterl - ask him about why he added util drawing etc for Bordered???).
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.26  92/06/09  14:58:17  14:58:17  sterling (Sterling Wight)
 *		Eliminated pen and brush inheritance, eliminated drawn region cache.
 *		
 *		Revision 1.25  1992/04/24  06:38:54  lewis
 *		Made Realize public for easier hacking - whole thing is a hack anyhow.
 *
 *		Revision 1.24  92/04/15  00:30:23  00:30:23  lewis (Lewis Pringle)
 *		Add private nonvirtual      void    AdjustOurHackWidget ();
 *		
 *		Revision 1.23  92/04/14  19:37:07  19:37:07  lewis (Lewis Pringle)
 *		Carefully commented on how dangerous Reparent was, after wasting a day tracking down a bug in it, and
 *		then finally got rid of it entirely.
 *		Commnented on conditions you can assume about calls to Realize/UnRealize.
 *		
 *		Revision 1.22  92/04/14  07:13:09  07:13:09  lewis (Lewis Pringle)
 *		Make some Xt hacks private instead of protected.
 *		
 *		Revision 1.20  92/04/09  14:54:16  14:54:16  lewis (Lewis Pringle)
 *		Cleanup comments on XtToolkit specific hacks in View, and add to them to support clipping,
 *		and proper back-to-front ordering of widgets.
 *		
 *		Revision 1.19  92/04/02  11:35:54  11:35:54  lewis (Lewis Pringle)
 *		Got rid of Get/SetClip methods. As more careful about overrides of TabletOwner routines to do
 *		SetClips before calling inherited method to indrect to tablet.
 *		Got rid of GetEffectiveChacedOrigin () - instead do all the magic in LocalToTablet, TabletToLocal_.
 *		Then tabletowner routines work fine using these cahced values. Mode BltBlti overrides to reflect
 *		chagners in TabletOwner.
 *		Got rid of Focus stuff - instead just call SetClip (GetVisibleArea()) before each call Views overrides
 *		of TabletOwner drawing indirection primimates (like Outline).
 *		
 *		Revision 1.18  92/03/26  09:25:34  09:25:34  lewis (Lewis Pringle)
 *		Moved GroupView, and LabledGroupView to GroupView.hh/cc.
 *		Made GetBrush/GetEffectiveBrush, and Pen/Font/Background routines simple oneliner inlines.
 *		Got rid of CalcXX version of said attributes.
 *		Added updateMode args to most related rouintes, Reparent (), Add/Remove/ReorderSubview, etc...
 *		Made AddSubView/RemoveSubView/Reparent all non-virtual.
 *		Got rid of extra arg to EffectiveLiveChanged.
 *		
 *		Revision 1.16  1992/03/05  20:07:26  sterling
 *		made addsubview protected, changed groupview
 *
 *		Revision 1.15  1992/02/15  04:50:37  sterling
 *		used EffectiveLiveChanged
 *
 *		Revision 1.13  1992/01/15  10:07:21  lewis
 *		Inerhit from Panel since MouseHandler no longer does.
 *
 *		Revision 1.12  1992/01/14  06:07:47  lewis
 *		Have view inherit from TabletOwner.
 *
 *		Revision 1.8  1991/12/05  15:05:01  lewis
 *		Tried to change headers so we made more stuff protected, but too many places
 *		barfed. Will make another pass later.
 *
 *
 */

#include	"Sequence.hh"

#include	"Brush.hh"
#include	"Cursor.hh"
#include	"Font.hh"
#include	"Pen.hh"
#include	"Tablet.hh"
#include	"Tile.hh"

#include	"Bordered.hh"
#include	"Command.hh"
#include	"LiveItem.hh"
#include	"MouseHandler.hh"
#include	"QuickHelp.hh"




#if		!qRealTemplatesAvailable
	typedef	class	View*	ViewPtr;
	Declare (Iterator, ViewPtr);
	Declare (Collection, ViewPtr);
	Declare (AbSequence, ViewPtr);
	Declare (Array, ViewPtr);
	Declare (Sequence_Array, ViewPtr);
	Declare (Sequence, ViewPtr);
#endif


// These are really UI decisions and belong in the framework, not font stuff.  Is this right place?
extern	const	Font	kApplicationFont;
extern	const	Font	kSystemFont;

extern	const	Font	kDefaultFont;
extern	const	Brush	kDefaultBrush;
extern	const	Pen		kDefaultPen;
extern	const	Tile	kDefaultBackground;

extern	const	PixelMapCursor	kArrowCursor;
extern	const	PixelMapCursor	kIBeamCursor;
extern	const	PixelMapCursor	kCrossCursor;
extern	const	PixelMapCursor	kPlusCursor;
extern	const	PixelMapCursor	kWatchCursor;






class	Adornment;
class	CompositeAdornment;
class	Shape;
#if		qXGDI
struct osWidget;
#endif


#if		qCanFreelyUseVirtualBaseClasses
class	View : public virtual MouseHandler, public virtual Panel, public virtual QuickHelp, public virtual LiveItem, public virtual TabletOwner, public virtual Bordered
#else
class	View : public MouseHandler, public Panel, public QuickHelp, public LiveItem, public TabletOwner, public Bordered
#endif
	 {

	public:
		View ();
		~View ();

	public:		
		override	Boolean	TrackMovement (const Point& cursorAt, Region& mouseRgn, const KeyBoard& keyBoardState);
		override	Boolean	TrackPress (const MousePressInfo& mouseInfo);
		override	Boolean	TrackHelp (const Point& cursorAt, Region& helpRegion);

	public:		
		override	Tablet*	GetTablet () const;
		nonvirtual	void	SetTablet (Tablet* newTablet, UpdateMode updateMode = eDelayedUpdate);

		override	Panel*	GetParentPanel () const;
		override	Region	GetVisibleArea () const;

		nonvirtual	Point	CalcDefaultSize (const Point& defaultSize = kZeroPoint) const;

	protected:
		override	void	SetOrigin_ (const Point& newOrigin, UpdateMode updateMode);
		override	void	SetSize_ (const Point& newSize, UpdateMode updateMode);
		override	void	SetExtent_ (const Rect& newExtent, UpdateMode updateMode);
		override	void	SetVisible_ (Boolean visible, UpdateMode updateMode);
		override	void	EffectiveVisibilityChanged (Boolean newVisible, UpdateMode updateMode);

		virtual		Point	CalcDefaultSize_ (const Point& hint) const;

	protected:
		virtual		void	Draw (const Region& update);		// usually override this
		
		nonvirtual	void	DrawBorder ();
		virtual		void	DrawBorder_ (const Rect& box, const Point& border);

		static		void	CalcColorSet (Color background, Color& selectColor, Color& topShadow, Color& bottomShadow);
		nonvirtual	void	DrawMacBorder (const Rect& box, const Point& border);
		nonvirtual	void	DrawMotifBorder (const Rect& box, const Point& border, Color backGroundColor, Bordered::Plane plane);
		nonvirtual	void	DrawWinBorder (const Rect& box, const Point& border, Color backGroundColor, Bordered::Plane plane);

	public:
		/*
		 * Provide default graphix behavior via default background tile inheritance
		 * (from parent views).  Take these into account in outline/paint accessors.  Also, do
		 * same for font...
		 *		NB:		Nil for Font*,  means use inherited.
		 */

		nonvirtual	const Font*		GetFont () const;
		nonvirtual	void			SetFont (const Font* font, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	const Font&		GetEffectiveFont () const;					// one would draw with

		nonvirtual	const Tile*		GetBackground () const;
		virtual		void			SetBackground (const Tile* tile, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	const Tile&		GetEffectiveBackground () const;			// one would draw with

	protected:
		virtual		void	EffectiveFontChanged (const Font& newFont, UpdateMode updateMode);
		virtual		void	EffectiveBackgroundChanged (const Tile& newBackground, UpdateMode updateMode);
		
	protected:
		override	void	SetBorder_ (const Point& border, UpdateMode updateMode);
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode);
		override	void	SetPlane_ (Bordered::Plane plane, UpdateMode updateMode);
	
	public:
		virtual		void	AddAdornment (Adornment* adornment, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	RemoveAdornment (Adornment* adornment, UpdateMode updateMode = eDelayedUpdate);

		/* should the view clip out siblings? By default, only if it has a background */
		virtual		Boolean	Opaque () const;


public: // sadly we need public cuz of adornment - fix asap...

		/*
		 * Indirect to tablet methods, after doing focus, which may involve coordinate
		 * transforms, setup of default pens, etc... and aligning tiles.
		 */
		override	void		OutLine (const Shape& s, const Rect& shapeBounds, const Pen& pen);
		nonvirtual	void		OutLine (const Shape& s, const Rect& shapeBounds);
		nonvirtual	void		OutLine (const Shape& s, const Pen& pen);
		nonvirtual	void		OutLine (const Shape& s);
		override	void		Paint (const Shape& s, const Rect& shapeBounds, const Brush& brush);
		nonvirtual	void		Paint (const Shape& s, const Rect& shapeBounds);
		nonvirtual	void		Paint (const Shape& s, const Brush& brush);
		nonvirtual	void		Paint (const Shape& s);

		/*
		 * Convenient wrappers on Paint ().
		 *		-	Fill allows specification of a tile rather than a brush.
		 *		-	Erase Fills with the effective background tile.
		 *		-	Gray grays out the given shape
		 *		-	Hilight uses the standard UI mechanism to hilite a given shape.
		 *		-	Invert inverts the colors of the given shape.
		 *
		 *	These routines come in three varieties:
		 *		-	Taking a shape, and a bounds
		 *		-	A scaled shape (which implies the bounds)
		 *		-	Or, no shape in which case we use the owned (Panel) shape, and the localExtent.
		 */
		nonvirtual	void		Fill (const Shape& s, const Rect& shapeBounds, const Tile& tile);
		nonvirtual	void		Fill (const Shape& s, const Tile& tile);
		nonvirtual	void		Fill (const Tile& tile);
		nonvirtual	void		Erase (const Shape& s, const Rect& shapeBounds);
		nonvirtual	void		Erase (const Shape& s);
		nonvirtual	void		Erase ();
		nonvirtual	void		Gray (const Shape& s, const Rect& shapeBounds);
		nonvirtual	void		Gray (const Shape& s);
		nonvirtual	void		Gray ();
		nonvirtual	void		Hilight (const Shape& s, const Rect& shapeBounds);
		nonvirtual	void		Hilight (const Shape& s);
		nonvirtual	void		Hilight ();
		nonvirtual	void		Invert (const Shape& s, const Rect& shapeBounds);
		nonvirtual	void		Invert (const Shape& s);
		nonvirtual	void		Invert ();

	/*
	 * overload these TabletOwner methods to default the font to our EffectiveFont. Also, reset the
	 * clip to the current local region on draws.
	 */
	public:
		override	void		DrawText (const String& text, const Point& at = kZeroPoint);
		nonvirtual	void		DrawText (const String& text, const Font& font, const Point& at);
		nonvirtual	Coordinate	TextWidth (const String& text, const Point& at = kZeroPoint) const;
		nonvirtual	Coordinate	TextWidth (const String& text, const Font& font, const Point& at = kZeroPoint) const;

	protected:
		override	void		ScrollBits (const Rect& r, const Point& delta, const Tile& background);
		nonvirtual	void		ScrollBits (const Rect& r, const Point& delta);

	protected:
		override	void		DrawPicture (const Picture& picture, const Rect& intoRect);
		override	void		DrawPicture (const Picture& picture, const Point& at = kZeroPoint);

	protected:
		override	void		BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode = eCopyTMode);
		override	void		BitBlit (const PixelMap& from, const Rect& fromRect, const Rect& toRect, TransferMode tMode, const Region& mask);
		nonvirtual	void		BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode = eCopyTMode);
		nonvirtual	void		BitBlit (const Rect& fromRect, PixelMap& to, const Rect& toRect, TransferMode tMode, const Region& mask);

	public:
		nonvirtual	View*	GetParentView () const;

	protected:
		/*
		 * Manage subViews (similar to subpanels). Note that whatever the UpdateMode given to AddSubView, it still
		 * must invalidate visible regions, etc, and still must inval layout.
		 */		
		nonvirtual	void	AddSubView (View* subView, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	AddSubView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate);
		
		nonvirtual	void	RemoveSubView (View* subView, UpdateMode updateMode = eDelayedUpdate);
		
		nonvirtual	void	ReorderSubView (View* v, CollectionSize index = 1, UpdateMode updateMode = eDelayedUpdate);
		nonvirtual	void	ReorderSubView (View* subView, View* neighborView, AddMode addMode = ePrepend, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	CollectionSize	GetSubViewCount () const;
		nonvirtual	CollectionSize	GetSubViewIndex (View* v);
		nonvirtual	View*			GetSubViewByIndex (CollectionSize index);

		nonvirtual	SequenceIterator(ViewPtr)*	MakeSubViewIterator (SequenceDirection d = eSequenceForward) const;
	
// should some of this be protected??? at least the layout stufff???
	public:
		// Layout support
		nonvirtual	void	InvalidateLayout ();
		nonvirtual	Boolean	ProcessLayout (Boolean layoutInvisible = False);		// lays out this and all subviews

		nonvirtual	Region	GetDrawnRegion () const;	// includes any adornment, enclosure coordinates

	private:
		Point	fCachedOrigin;
		Font*	fEffectiveFont;
		Tile*	fEffectiveBackground;
		Region*	fVisibleArea;
		Boolean	fCachedOriginValid;
		Boolean	fHasFont;
		Boolean	fHasBackground;

		nonvirtual	void	InvalCurEffectiveFont (UpdateMode updateMode);
		nonvirtual	void	InvalCurEffectiveBackground (UpdateMode updateMode);

		nonvirtual	void	DoInvalVisibleArea (Boolean alwaysCheckSiblings = False);			// calls parent or our-own - or siblings as appropriate
		nonvirtual	void	InvalVisibleArea ();
		nonvirtual	void	InvalCachedOrigin ();

	public:	
		override	Boolean	GetLive () const;
		override	Boolean	GetEffectiveLive () const;
		
	protected:
		override	void	EffectiveLiveChanged (Boolean newLive, UpdateMode updateMode);

	public:
		/*
		 * This routine should rarely be overridden, but one time when it is necessary, is when
		 * a view wishes to draw after its subviews have.
		 */
		virtual		void	Render (const Region& updateRegion);

	protected:
		virtual		void	Layout ();



#if		qXtToolkit
	/*
	 * All of these methods are implementation details to support keeping the Stroika
	 * view heirarchy in-sync with the motif/X widget/window heirarchies. This is
	 * a sticky business, and when we have a solidified design, it should be documented
	 * here...
	 *
	 * NB: Our notion of realization here does not correspond preciesely to the Xt notion - here
	 * we really mean building the widget, and realizing, all at once... This is "analogous" to
	 * the Xt interpretation, but one level removed (ie were using Xt, and its using X) and so
	 * not identical usage.
	 */



	/*
	 * Realize()/UnRealize recursively call method of same name on children - be sure
	 * to call inherited if you override, and could possibly have subviews with widgets
	 * that you want automatically realized/unrealized. You can do this manually yourself
	 * if you wish. If you do override, and call inherited, do inherited::Realize () at the very
	 * end, and after having set your fWidget field to the new widget, and pass the new widget
	 * along as an arg to inherited::Realize () since we use it to realize our subwidgets, and
	 * if no widget is created for "this" and we have subviews, we will be foreced to create
	 * a widget here privately to own any subwidgets, and give them clipping.
	 *
	 * Realize () can be called when you are already realized, and UnRealize can when you
	 * are already UnRealized() - but - if Realize () is called redundently, you can assume that
	 * your parent widget has not changed.
	 */
	public:
		virtual		void		Realize (osWidget* parent);
	protected:
		virtual		void		UnRealize ();
		virtual		osWidget*	GetWidget () const;		// returns Nil if not realized, or there is no widget...


	/*
	 * It would appear that in order to get clipping to work properly with widgets, my only hope is to
	 * create shitloads of extra widgets (with corresponding X windows that really give me the clipping).
	 * I don't need to create them in all cases, and CANNOT in many. So we will need to provide some
	 * control over this feature. In particular, I think we can avoid this in classes with no subviews.
	 * This avoidance is is safe, since the only trouble we could get in is that some of our children
	 * would have the wrong parent as we created our new widget, but that cannot happen since we only
	 * avoid building a widget if either some subclass has, or if we have no children.
	 */
	private:
		osWidget*	fOurHackWidget;		// one we created here as a hack to get clipping of subwidgets...

		nonvirtual	void	MakeOurHackWidget ();
		nonvirtual	void	UnMakeOurHackWidget ();
		nonvirtual	void	AdjustOurHackWidget ();

	protected:
		nonvirtual	View*		GetParentWidgetView () const;	// wind parent list til we find a widget
		nonvirtual	osWidget*	GetParentWidget () const;		// widget from GetParentWidgetView ()

	/*
	 * Recursively go thru all children, with this as their GetParentWidgetView (), and if they
	 * have a window (that is are not a gadget, and are realized), then be sure X has their
	 * windows appropriately stacked (call XRestackWindows). This is used to assure that widgets
	 * appear to have the appropriate front-to-back ordering.
	 */
	protected:
	private:	// for now assume private - see if we need to make protected later...
		nonvirtual	void		SyncronizeXWindowsWithChildWidgets ();
	private:
		nonvirtual	void		SyncChildWidgetsHelper (class Sequence(osWidgetPtr)& orderedList);

#endif	/*qXtToolkit*/


	/*
	 * Override to keep cached values for efficiency sake
	 */
	protected:
		override	Point	LocalToTablet_ (const Point& p)	const;
		override	Point	TabletToLocal_ (const Point& p)	const;


	private:
		View*					fParentView;
		Sequence(ViewPtr)*		fSubViews;
		Tablet*					fCurrentTablet;
		Boolean					fNeedsLayout;
		Boolean					fChildNeedsLayout;
		CompositeAdornment*		fAdornment;
		
		nonvirtual	void	NotifyEnclosureOfInvalidLayout ();
		
		friend	class	Adornment;
friend class Printer;// hack to get at subviews...
};





/*
 ********************************************************************************
 ************************************ InLines ***********************************
 ********************************************************************************
 */
inline	View*	View::GetParentView () const	{ return (fParentView); }

inline	const	Font*	View::GetFont () const					{ return (fHasFont? fEffectiveFont: Nil); }
inline	const	Font&	View::GetEffectiveFont () const			{ EnsureNotNil (fEffectiveFont); return (*fEffectiveFont); }
inline	const	Tile*	View::GetBackground () const			{ return (fHasBackground? fEffectiveBackground: Nil); }
inline	const	Tile&	View::GetEffectiveBackground () const	{ EnsureNotNil (fEffectiveBackground); return (*fEffectiveBackground); }



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*__View__*/
