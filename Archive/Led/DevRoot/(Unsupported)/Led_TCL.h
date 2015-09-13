/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1997.  All rights reserved */
#ifndef	__Led_TCL_h__
#define	__Led_TCL_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/Led_TCL.h,v 2.14 1997/06/18 20:04:17 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Led_TCL.h,v $
 *	Revision 2.14  1997/06/18 20:04:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.13  1997/01/10  03:03:46  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/12/13  17:54:11  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.11  1996/12/05  19:08:17  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1996/09/30  14:15:51  lewis
 *	Lose AdjustBounds() and instead redid the UpdateScrollBars() code, etc...
 *
 *	Revision 2.9  1996/09/01  15:30:29  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.8  1996/02/26  18:38:10  lewis
 *	Moved tcl-specific code here from LedConfig.h, and LedSupport.h
 *
 *	Revision 2.7  1996/02/05  04:10:03  lewis
 *	Get rid of fWeOwnTextStore and TextStore_ arg to CTOR.
 *
 *	Revision 2.6  1995/12/13  05:53:50  lewis
 *	Moved BlinkCaretIfNeeded/DrawCaret_ support here, and unify caret support
 *	in TextInteractor.
 *
 *	Revision 2.5  1995/11/25  00:18:16  lewis
 *	another OnPasteCommand() override and UpdateMenus () - tmp hack? for
 *	paste of pict? needs more work...
 *
 *	Revision 2.4  1995/11/02  21:49:06  lewis
 *	Override GetTextStyle() to fix menu display for style runs and
 *	OnCopyCommand/OnPasteCommand() tohandle updating style runs
 *	for Style-specific TCL class
 *
 *	Revision 2.3  1995/10/19  22:05:04  lewis
 *	Support new StandardStyledWordWrappedLed_TCL.
 *
 *	Revision 2.2  1995/10/09  22:17:00  lewis
 *	No more need for override of Replace function.
 *	no more need to include headers for concrete text stores.
 *
 *	Revision 2.1  1995/09/06  20:52:38  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.14  1995/06/08  05:10:08  lewis
 *	Code cleanups
 *
 *	Revision 1.13  1995/05/29  23:45:07  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.12  1995/05/21  17:05:41  lewis
 *	Moved qLotsOfSubclassesFromMixinPointYeildTemporarilyBadVTable back
 *	here since must be done in concrete subclass of TextStore.
 *
 *	Revision 1.11  1995/05/20  04:42:39  lewis
 *	Massive changes.
 *	Mostly now using WordWrappedTextInteractor now. These should all me
 *	much simpler - SPR 0261, 0262.
 *
 *	Revision 1.10  1995/05/09  23:26:31  lewis
 *	Moved fMultiByteInputCharBuf to TextInteractor_ to share code - SPR#0242.
 *
 *	Revision 1.9  1995/05/08  03:03:40  lewis
 *	No more need to override NotificationOf_Scrolling ()
 *	And ScrollSoShowing, ScrollByIfRoom no longer return bool.
 *
 *	Revision 1.8  1995/05/06  19:23:34  lewis
 *	Renamed GetByteLength->GetLength () - SPR #0232.
 *
 *	Revision 1.7  1995/05/05  19:41:49  lewis
 *	Deleted fWeDidInit instance variable - never used.
 *
 *	Revision 1.6  1995/04/20  06:43:30  lewis
 *	Ton of workarounds for new MWERKS compiler bug qAmbiguousAccessForSomeVBaseClassCasesBug.
 *
 *	Revision 1.5  1995/04/16  19:21:51  lewis
 *	Got rid of SimpleTextImager - SPR 0205.
 *
 *	Revision 1.4  1995/03/23  03:45:27  lewis
 *	Fixed SelectWholeLineAfter args - accidentally missed when TextInteacter
 *	base class changed arg list and we stopped really overriding!
 *
 *	Revision 1.3  1995/03/17  02:58:54  lewis
 *	Added DoThumbDrag() virtual called from our scrollpane subclass.
 *
 *	Revision 1.2  1995/03/02  05:45:35  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:22  lewis
 *	Initial revision
 *
 *
 *
 */

#include	"TextInteractor.h"
#include	"TextInteractorMixins.h"


#include	"CAbstractText.h"




/*
 *	TCL-only configuration variables.
 */
#ifndef	qLed_UseCharOffsetsForTCLScrollAPI
	/*
	 *	qLed_UseCharOffsetsForTCLScrollAPI is GREATLY prefered, since it allows for
	 *	non-constant numbers of pixels per row, and not computing row #'s (and therefore
	 *	word-wrapping) text not displayed.
	 *
	 *	The other mode - line-based - is for LVEJ backwards compatability.
	 */
	#define	qLed_UseCharOffsetsForTCLScrollAPI	1
#endif



#if		qLed_UseCharOffsetsForTCLScrollAPI
	#include	"CScrollPane.h"
#endif






LongPt		AsLongPt (Led_Point p);
Led_Point	AsLedPoint (LongPt p);
LongRect	AsLongRect (Led_Rect r);
Led_Rect	AsLedRect (LongRect r);




/*
 *	Led_ is the class which introduces behavior - such as keyclicks, and mouse tracking.
 */
class	Led_TCL_ : public virtual TextInteractor_, public CAbstractText {
	protected:
		Led_TCL_ (CView *anEnclosure, CBureaucrat *aSupervisor,
					short aWidth, short aHeight,
					short aHEncl, short aVEncl,
					SizingOption aHSizing,
					SizingOption aVSizing,
					short aLineWidth = -1, Boolean aScrollHoriz = FALSE
				);
	public:
		virtual ~Led_TCL_ ();

	/*
	 * Overrides of the TCL CAbstractText API to thunk down to LED code.
	 */
	public:
		override	void		SetTextPtr (Ptr textPtr, long numChars);
		override	Handle		GetTextHandle ();
		override	Handle		CopyTextRange (long start, long end);
		override	void		InsertTextPtr (Ptr text, long length, Boolean fRedraw);
		override	void		SetFontNumber (short aFontNumber);
		override	void		SetFontStyle (short aStyle);
		override	void		SetFontSize (short aSize);
		override	void		SetTextMode (short aMode);
		override	void		SetAlignCmd (long anAlignment);
		override	long		GetAlignCmd ();
		override	void		SetSpacingCmd (long aSpacingCmd);
		override	long		GetSpacingCmd ();
		override	long		GetHeight (long startLine, long endLine);
		override	long		GetCharOffset (LongPt *aPt);
		override	void		GetCharPoint (long offset, LongPt *aPt);
		override	void		GetCharStyle (long charOffset, TextStyle *theStyle);
		override	void		GetTextStyle (short *whichAttributes, TextStyle *aStyle);
		override	long		FindLine (long charPos);
		override	long		GetLength ();				// TCL's
		override	size_t		GetLength () const;			// Led's - do both here so we get overloading right!
		override	long		GetNumLines ();
		override	void		PerformEditCommand (long theCommand);
		override	void		DoKeyDown (char theChar, Byte keyCode, EventRecord* macEvent);
		override	void		TypeChar (char theChar, short theModifiers);
		override	void		GetSelection (long *selStart, long *selEnd);
		override	void		HideSelection (Boolean hide, Boolean redraw);
		override	void		Draw (Rect* area);
		override	void		SetBounds (LongRect* aBounds);
		override	void		Dawdle (long* maxSleep);
		override	void		Activate ();
		override	void		Deactivate ();
		override	void		SetWantsClicks (Boolean aWantsClicks);
		override	void		DoClick (Point hitPt, short modifierKeys, long when);
		override	void		ResizeFrame (Rect* delta);
		override	void		GetSteps (short* hStep, short* vStep);



	/*
	 *	Overrides of the TCL CAbstractText API to thunk down to LED code, together with
	 *	overloaded Led versions.
	 */
	public:
		override	void		SetSelection (long selStart, long selEnd, Boolean fRedraw);
		override	void		SetSelection (size_t start, size_t end, UpdateMode updateMode = eDelayedUpdate);



	protected:
		override	void		NotificationOf_SelectionChanged ();


	// Called from a number of places - including InvalidateScrollBarParameters () - override this to
	// change sbar behavior.
	protected:
		override	void		UpdateScrollBars ();


	/*
	 *	Other TCL base class overrides needed to thunk to Led code.
	 */
	public:
		override	Boolean		HitSamePart (Point pointA, Point pointB);


	// TCL overrides to support using character offsets instead of line offsets
	// for sbar positions
	#if		qLed_UseCharOffsetsForTCLScrollAPI
		override	void	GetExtent (long* theHExtent, long* theVExtent);
		override	void	GetFrameSpan (short* theHSpan, short* theVSpan);
		override	void	SetScales (short aHScale, short aVScale);
		override	void	GetScales (short* theHScale, short* theVScale);
		override	void	GetPixelExtent (long* hExtent, long* vExtent);
	#endif
		override	void	Scroll (long hDelta, long vDelta, Boolean redraw);

	protected:
		nonvirtual	void	SetTCLPanoramPositionFromLedPosition ();


	// disambiguate between TCL and Our versions - these simply thunk down to Led versions...
	public:
		override	void	ScrollToSelection ();
		override	void	ScrollToSelection (UpdateMode updateMode);
		nonvirtual	void	Refresh (UpdateMode updateMode = eDelayedUpdate) const;
		nonvirtual	void	Refresh (const Led_Rect& area, UpdateMode updateMode = eDelayedUpdate) const;
		nonvirtual	void	Refresh (size_t from, size_t to, UpdateMode updateMode = eDelayedUpdate) const;
		nonvirtual	void	Refresh (const Marker* range, UpdateMode updateMode = eDelayedUpdate) const;



	protected:
		override	Led_Tablet	AcquireTablet () const;
		override	void		ReleaseTablet (Led_Tablet tablet) const;
#if		qDebug
	private:
		size_t	fAcquireCount;		// just for debugging - be sure zero on DTOR
#endif
	private:
		size_t	fDragAnchor;


	public:
		nonvirtual	void	BlinkCaretIfNeeded ();	// just allow public access
													// via idle task mechanism or whatever...
	protected:
		nonvirtual	void	DrawCaret_ ();
		nonvirtual	void	DrawCaret_ (bool on);
	private:
		bool		fLastDrawnOn;
		int			fTickCountAtLastBlink;


#if		qLed_UseCharOffsetsForTCLScrollAPI
	/*
	 *	Helper for new Led_ScrollPane...
	 */
	public:
		virtual		void	DoHorizScroll (short whichPart);
		virtual		void	DoVertScroll (short whichPart);
		virtual		void	DoThumbDrag (short newHPos, short newVPos);
#endif

	/*
	 * Overrides of the Led_ code that must thunk down to TCL calls
	 */
	protected:
		override	void	Refresh_ (UpdateMode updateMode) const;
		override	void	Refresh_ (const Led_Rect& area, UpdateMode updateMode) const;
		override	void	RefreshWindowRect_ (const Led_Rect& windowRectArea, UpdateMode updateMode) const;
		override	void	Update_ () const;
		override	void	Update_ (const Led_Rect& area) const;
		override	void	UpdateWindowRect_ (const Led_Rect& windowRectArea) const;
		override	void	OnCutCommand ();
		override	void	OnCopyCommand_After ();


	protected:
		override	bool	QueryInputKeyStrokesPending () const;


	// TCL CAbstractText overrides to work around call to GetTextHandle() and use Led char-boundary routines...
	public:
		override	void	GetCharBefore (long* aPosition, tCharBuf charBuf);
		override	void	GetCharAfter (long* aPosition, tCharBuf charBuf);


	private:
		long	fSpacingCmd;
		long	fAlignCmd;
};




/*
 *	SimpleLed_TCL is our first concrete editor class. You can directly instantiate and use this one.
 *	Or mix and match various component classes to make your own editor with the features
 *	you need.
 */
class	SimpleLed_TCL : public WordWrappedTextInteractor, public Led_TCL_ {
	public:
		SimpleLed_TCL (CView *anEnclosure, CBureaucrat *aSupervisor,
								short aWidth, short aHeight,
								short aHEncl, short aVEncl,
								SizingOption aHSizing,
								SizingOption aVSizing,
								short aLineWidth, Boolean aScrollHoriz
						);


	/*
	 *	Must combine behaviors of different mixins.
	 */
	public:
		override	void		SetSelection (long selStart, long selEnd, Boolean fRedraw);
		override	void		SetSelection (size_t start, size_t end, UpdateMode updateMode = eDelayedUpdate);
};





/*
 */
class	StandardStyledWordWrappedLed_TCL : public StandardStyledWordWrappedTextInteractor, public Led_TCL_ {
	public:
		StandardStyledWordWrappedLed_TCL (CView *anEnclosure, CBureaucrat *aSupervisor,
								short aWidth, short aHeight,
								short aHEncl, short aVEncl,
								SizingOption aHSizing,
								SizingOption aVSizing,
								short aLineWidth, Boolean aScrollHoriz
						);


	public:
// maybe only needed temporarily??? For paste of PICT etc...
override	void UpdateMenus();


	/*
	 *	Must combine behaviors of different mixins.
	 */
	public:
		override	void		SetSelection (long selStart, long selEnd, Boolean fRedraw);
		override	void		SetSelection (size_t start, size_t end, UpdateMode updateMode = eDelayedUpdate);

	public:
		override	void		GetTextStyle (short *whichAttributes, TextStyle *aStyle);

};











#if		qLed_UseCharOffsetsForTCLScrollAPI

class	Led_ScrollPane : public CScrollPane {
	public:
		Led_ScrollPane (CView *anEnclosure, CBureaucrat *aSupervisor,
							short aWidth, short aHeight,
							short aHEncl, short aVEncl,
							SizingOption aHSizing,
							SizingOption aVSizing,
							Boolean hasHoriz, Boolean hasVert, Boolean hasSizeBox);


	public:
		override	void		DoHorizScroll (short whichPart);
		override	void		DoVertScroll (short whichPart);
		override	void		DoThumbDrag (short hDelta, short vDelta);
};

#endif





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

	inline	LongPt		AsLongPt (Led_Point p)
		{
			LongPt	lp;
			lp.v = p.v;
			lp.h = p.h;
			return lp;
		}
	inline	Led_Point	AsLedPoint (LongPt p)
		{
			return Led_Point (p.v, p.h);
		}
	inline	LongRect	AsLongRect (Led_Rect r)
		{
			LongRect	lr;
			lr.top = r.top;
			lr.left = r.left;
			lr.bottom = r.bottom;
			lr.right = r.right;
			return lr;
		}
	inline	Led_Rect	AsLedRect (LongRect r)
		{
			return Led_Rect (r.top, r.left, r.bottom-r.top, r.right-r.left);
		}





	inline	void	Led_TCL_::Refresh (UpdateMode updateMode) const
		{
			TextInteractor_::Refresh (updateMode);
		}
	inline	void	Led_TCL_::Refresh (const Led_Rect& area, UpdateMode updateMode) const
		{
			TextInteractor_::Refresh (area, updateMode);
		}
	inline	void	Led_TCL_::Refresh (size_t from, size_t to, UpdateMode updateMode) const
		{
			TextInteractor_::Refresh (from, to, updateMode);
		}
	inline	void	Led_TCL_::Refresh (const Marker* range, UpdateMode updateMode) const
		{
			TextInteractor_::Refresh (range, updateMode);
		}


#endif	/*__Led_TCL_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

