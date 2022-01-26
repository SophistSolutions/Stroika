/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Scroller__
#define	__Scroller__

/*
 * $Header: /fuji/lewis/RCS/Scroller.hh,v 1.4 1992/09/08 15:34:00 lewis Exp $
 *
 * Description:
 *
 *		The class AbstractScroller provides just the basic scrollable functionality. The subclass
 *		Scroller provides the more common user interface on top of that.
 *
 * TODO:
 *
 * Changes:
 *	$Log: Scroller.hh,v $
 *		Revision 1.4  1992/09/08  15:34:00  lewis
 *		Renamed NULL -> Nil.
 *
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/06/25  06:34:43  sterling
 *		React to changes in slider class. Be more careful about destroying owned guys - reacting
 *		to new defintiion for views deleting subviews - sterl made changes and lewis checked in,
 *		so not sure what else.
 *
 *		Revision 1.1  1992/06/20  17:27:41  lewis
 *		Initial revision
 *
 *		Revision 1.12  1992/05/19  10:14:08  sterling
 *		added MakeScrollRect
 *
 *		Revision 1.11  92/05/13  12:38:56  12:38:56  lewis (Lewis Pringle)
 *		STERL - added new CTOR to Scroller().
 *		
 *		Revision 1.10  92/04/17  17:05:31  17:05:31  lewis (Lewis Pringle)
 *		Add hack to ScrollerMagicView to own a widget, so scrollers do better clipping out
 *		owned widgets (important case is text widgets).
 *		
 *		Revision 1.8  1992/03/09  23:57:56  lewis
 *		Use new KeyStroke interface, and call DispatchKeyEvent instead of HandleKey.
 *
 *		Revision 1.7  1992/03/05  20:07:26  sterling
 *		allowed mofifying scrollbars on the fly
 *
 *		Revision 1.4  1991/12/05  15:05:01  lewis
 *		Added ScrollerMagicView class so we could make more stuff protected in View.
 */


#include	"GroupView.hh"
#include	"KeyHandler.hh"
#include	"MenuCommandHandler.hh"
#include	"ScrollBar.hh"
#include	"View.hh"



/*
 * NB:
 *		Once a view is placed in a scroller, do NOT call SetOrigin() on that view, as the results
 *		will be bad (it interferes with our GetWhereViewing () strategy).
 */


// SHOULD BE SCOPED CLASS!!
#if		qXGDI
struct	osWidget;
#endif

class	ScrollerMagicView : public GroupView {
	public:
		ScrollerMagicView ();
		~ScrollerMagicView ();

#if		qXtToolkit
		override	void		Layout ();
		override	void		Realize (osWidget* parent);
		override	void		UnRealize ();
		override	osWidget*	GetWidget () const;
		override	void		Draw (const Region& update);
	private:
		nonvirtual	void		AdjustWidgetExtent ();
		
		osWidget*	fMagicWidget;
#endif
};


// What is the rationale for AbstractScroller?? Seems like a weird amalgamation of functionality.
// Probably should be a pure interface class, not even a view. SSW 6/19/92
class	AbstractScroller : public View {
	protected:
		AbstractScroller (View* scrolledView);
		~AbstractScroller ();

	public:
		nonvirtual	Point	GetWhereViewing () const;
		virtual		void	SetWhereViewing (const Point& where, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	void	ScrollSoShowing (const Rect& what, Boolean forceFullyShown, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	View*	GetScrolledView () const;
		virtual		void	SetScrolledView (View* scrolledView, UpdateMode updateMode = eDelayedUpdate);

		nonvirtual	Boolean	GetRePaintWhileScrolling () const;
		nonvirtual	void	SetRePaintWhileScrolling (Boolean rePaintWhileScrolling);
		nonvirtual	Boolean	GetScrollBitsWhileScrolling () const;
		nonvirtual	void	SetScrollBitsWhileScrolling (Boolean scrollBitsWhileScrolling);

		nonvirtual	Region	GetContentArea () const;
		

	protected:
		nonvirtual	ScrollerMagicView&	GetScrollerMagicView () const;

		override	void	Layout ();

	private:
		ScrollerMagicView*	fScrollerMagic;
		Boolean				fRePaintWhileScrolling;
		Boolean				fScrollBitsWhileScrolling;
};




class	Scroller : public AbstractScroller, public MenuCommandHandler, public KeyHandler, public SliderController  {
	public:
		static	AbstractScrollBar*	const	kBuildDefaultSlider;
		static	AbstractScrollBar*	const	kBuildNoSlider;

		Scroller (View* scrolledView = Nil, 
			MenuCommandHandler* menuTarget = Nil, 
			KeyHandler* keyTarget = Nil,
			AbstractScrollBar* verticalScrollBar = kBuildDefaultSlider,
			AbstractScrollBar* horizontalScrollBar = kBuildDefaultSlider);

		Scroller (AbstractScrollBar* verticalScrollBar, AbstractScrollBar* horizontalScrollBar);
		~Scroller ();

		nonvirtual	MenuCommandHandler*	GetMenuTarget () const;
		virtual		void				SetMenuTarget (MenuCommandHandler* menuTarget);
		nonvirtual	KeyHandler*			GetKeyTarget () const;
		virtual		void				SetKeyTarget (KeyHandler* keyTarget);

		nonvirtual	void	SetScrolledViewAndTargets (View* scrolledView, MenuCommandHandler* menuTarget, KeyHandler* keyTarget, UpdateMode updateMode = eDelayedUpdate);

		/*
		 * Give owned handler a crack at commands first, and then call inherited.
		 */
		override	void	DoSetupMenus ();
		override	Boolean	DoCommand (const CommandSelection& selection);
		override	Boolean	DispatchKeyEvent (KeyBoard::KeyCode code, Boolean isUp, const KeyBoard& keyBoardState,
											  KeyComposeState& composeState);

		override	void	SetWhereViewing (const Point& where, UpdateMode updateMode = eDelayedUpdate);

		virtual		void	ScrolledViewChangedSize ();		// call to force recalc of scrollbars

		override	Boolean	SliderChanging (SliderBase& theSlider, Tracker::TrackPhase phase, Real oldValue, Real newValue);	// called while tracking
		override	void	SliderChanged (SliderBase& theSlider, Real oldValue, Real newValue);

		nonvirtual	AbstractScrollBar*	GetVerticalScrollBar () const;
		nonvirtual	void				SetVerticalScrollBar (AbstractScrollBar* slider);
		
		nonvirtual	AbstractScrollBar*	GetHorizontalScrollBar () const;
		nonvirtual	void				SetHorizontalScrollBar (AbstractScrollBar* slider);
		
		override	void	Draw (const Region& update);

	protected:
		// get the rectangle in which the scrolled view is displayed (our area minus borders and scrollbars)
		nonvirtual	Rect	GetScrollRect () const;

		override	void	Layout ();
		override	void	SetBorder_ (const Point& border, UpdateMode updateMode);
		override	void	SetMargin_ (const Point& margin, UpdateMode updateMode);

		virtual		void	AdjustStepAndPageSizes ();

		virtual	void	SetVerticalScrollBar_ (AbstractScrollBar* slider);
		virtual	void	SetHorizontalScrollBar_ (AbstractScrollBar* slider);

	private:
		nonvirtual	AbstractScrollBar*	BuildScrollBar (AbstractScrollBar::Orientation orientation, AbstractScrollBar* slider);
		
		MenuCommandHandler*			fMenuTarget;
		KeyHandler* 				fKeyTarget;
		AbstractScrollBar*			fVerticalScrollBar;
		AbstractScrollBar*			fHorizontalScrollBar;
		Boolean						fBuiltVerticalScrollBar;
		Boolean						fBuiltHorizontalScrollBar;
};



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__Scroller__*/

