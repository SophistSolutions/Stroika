/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */
#ifndef	__LedFrameEditor__
#define	__LedFrameEditor__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Headers/LedFrameEditor.h,v 2.11 1996/12/13 18:09:57 lewis Exp $
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
 *	$Log: LedFrameEditor.h,v $
 *	Revision 2.11  1996/12/13 18:09:57  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.10  1996/12/05  21:11:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/09/30  14:43:24  lewis
 *	React to change in AdjustBounds() code in Led.
 *
 *	Revision 2.8  1996/09/01  15:44:28  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/02/26  23:06:23  lewis
 *	Moved most of the Led_Rect conversion ops etc here from Led class lib.
 *	Interacter --> Interactor.
 *
 *	Revision 2.6  1996/02/05  05:03:08  lewis
 *	Merged Led_OpenDoc and the little subclass we had here together.
 *
 *
 *
 *	<<< OLD MATERIAL FROM Led_OpenDoc.hh - merged in here 960202 >>>
 *
 *	Revision 2.1  1996/01/22  05:50:30  lewis
 *	Moved DidUpdateText() updating hack into Led itself so other libraries
 *	trying todo split views will work right.
 *
 *	Revision 2.0  1996/01/04  00:24:30  lewis
 *	*** empty log message ***
 *
 *	Revision 1.9  1995/12/15  04:02:38  lewis
 *	*** empty log message ***
 *
 *	Revision 1.8  1995/12/13  06:11:40  lewis
 *	Now have BlinkCaretIfNeeded/DrawCaret_ support here, from
 *	TextInteractor. Now things work better with facets, and more shared
 *	code in TextInteractor.
 *
 *	Revision 1.7  1995/12/09  05:51:01  lewis
 *	Lose SimpleLed_OpenDoc class - no point in it.
 *
 *	Revision 1.6  1995/11/25  00:17:48  lewis
 *	Add keyCode arg to TypeChar()
 *
 *	Revision 1.5  1995/11/04  23:06:49  lewis
 *	No longer need WholeTextMarker and instead override
 *	DidUpdateText in Led_OpenDoc_ itself using new (a couple releases old)
 *	TextStore callback support.
 *
 *	Revision 1.4  1995/11/02  21:48:09  lewis
 *	Support SharedStyleDatabase*
 *
 *	Revision 1.3  1995/10/19  22:04:50  lewis
 *	Support new StandardStyledWordWrappedLed_OpenDoc.
 *
 *	Revision 1.2  1995/10/09  22:15:07  lewis
 *	no more need for Replace override.
 *	(in SimpleLed - but had to add one in Led_OpenDoc for support
 *	of new WholeTextMarker keeping track of any changes to text.
 *	(maybe even this no longer needed since we now have callbacks from
 *	textstore for whole imager?).
 *
 *	Revision 1.1  1995/09/06  20:52:38  lewis
 *	Initial revision
 *
 *
 *
 */


#include <Types.h>


#include <DragDrp.xh>
#include <ODTypes.h>
#include <Facet.xh>
#include <Shape.xh>
#include <SUView.xh>
#include <Window.xh>

#include <IText.h>


//#include	"Led.h"
//#include	"Led.h"
#include	"WordWrappedTextInteractor.h"

#include	"SampleCollections.h"




class	LedPart;
class	ODWindow;
class	ODFrame;
class	ODFacet;
class	ODShape;
class	Embedding;


Point		AsQDPoint (ODPoint p);
ODPoint		AsODPoint (Point pt);
Led_Point	AsLedPoint (ODPoint odp);
Rect		AsQDRect (ODRect odr);
ODRect		AsODRect (Rect r);
Led_Rect	AsLedRect (ODRect odr);
ODRect		AsODRect (Led_Rect r);



class	FrameEditor {
	public:
		FrameEditor (LedPart* partInfo, StandardStyledTextImager::SharedStyleDatabase* styleDatabase, TextStore_* useTextStore, ODFrame* useODFrame);
		virtual ~FrameEditor ();


	public:
		nonvirtual	ODFrame*	GetODFrame ( ) const					{return fODFrame;}
	private:
		ODFrame*		fODFrame;




	/*
	 *	Commands
	 */
	public:
		nonvirtual	void	OnSelectAllCommand ();


	public:

		nonvirtual	void	FrameShapeChanged ();
		nonvirtual	void 	FacetAdded (ODFacet* facet);
		nonvirtual	void 	FacetRemoved(ODFacet* facet);
		nonvirtual	void	DrawFacet (ODFacet* facet, ODShape* invalidShape);


	public:
		nonvirtual	bool	GetHasScrollBar () const;
		nonvirtual	void	SetHasScrollBar (bool hasScrollBar);
	private:
		bool		fHasVerticalScrollBar;

	public:
		nonvirtual	void	BlinkCaretIfNeeded ();
		nonvirtual	void	TypeChar (Led_tChar theChar, unsigned char keyCode, short theModifers);

	private:
		LedPart*	fPartInfo;

	public:
		nonvirtual	ODWindow*		GetPartWindow() const;
		nonvirtual	ODBoolean		HasPartWindow() const;
					void			SetPartWindow(ODWindow* window);
	private:
		ODWindow*		fPartWindow;


	public:
		nonvirtual	ODBoolean		HasSourceFrame () const;
		nonvirtual	ODFrame*		GetSourceFrame () const;
		nonvirtual	void			SetSourceFrame (ODFrame* sourceFrame);
	private:
		ODFrame*		fSourceFrame;


	public:
		bool							HasAttachedFrames() const;
		const	Led_Array<ODFrame*>&	GetAttachedFrames() const;
	private:
		Led_Array<ODFrame*>	fAttachedFrames;	// frames where are attached to us (ie have us as their source)



	public:
		nonvirtual	void	HandleMouseDownInFrame (ODEventData* event, ODFrame* frame, ODFacet* facet);
		nonvirtual	bool	HandleDragWithin (ODFacet* facet, Led_Point where);	// return true iff OK to drop here

	protected:
		nonvirtual	bool	IsADragSelect (ODEventData* event, ODFacet* facet);
		nonvirtual	void	HandleDragSelect (ODEventData* event, ODFacet* facet);
		nonvirtual	void	HandleScrollBarClick (ODPlatformWindow window, ControlHandle scrollBar, short partCode, Led_Point where);

	public:
		nonvirtual	void	AdjustCursor (Led_Point where);

	public:
		nonvirtual	void	SetSelectionAndCaretShown (bool shown);

	public:
		nonvirtual	void	ActivateScrollBars (bool activate);

	private:
		nonvirtual	void	DoVertScroll (short whichPart);


	public:
		nonvirtual	void	ClearSelection ();

	private:
		nonvirtual	void	RepositionScrollBar (ODFacet* facet = NULL);	// NULL => all
		nonvirtual	void	UpdateScrollBars (ODFacet* facet = NULL);		// NULL => all

	private:
#define	qMergeLedStuff	1
		class	LedODInteractor : public 
		#if qMergeLedStuff
			StandardStyledWordWrappedTextInteractor
		#else
			Led 
		#endif
			{
			public:
				LedODInteractor (FrameEditor* frameEditor, SharedStyleDatabase* styleDatabase, ODFrame* frame);
				~LedODInteractor ();

			protected:
				typedef	StandardStyledWordWrappedTextInteractor	inherited;

			public:
				nonvirtual	void	DrawCaret ();

			// Recieve notification of change, and handle it (updating sbars, xlating to TCL messages, etc)
			protected:
				override	void	InvalidateScrollBarParameters ();
				override	void	UpdateScrollBars ();

			public:
				nonvirtual	void	FixupScrolledOffEmbeddings ();
				nonvirtual	void	ForceCaretShownOn ();

#if qMergeLedStuff
	public:
		class	TmpUseFacetObj {
			public:
				TmpUseFacetObj (LedODInteractor* doc, ODFacet* facet);
				~TmpUseFacetObj ();
			private:
				LedODInteractor*	fDoc;
				ODFacet*		fOldFacet;
		};
		nonvirtual	ODFacet*	TemporarilyUseFacet (ODFacet* facet);	// can be NULL - never leave using facet that might be destroyed (returns old value)
	private:
		ODFacet*	fCurrentlyUpdatingFacet;


	protected:
		override	Led_Tablet	AcquireTablet () const;
		override	void		ReleaseTablet (Led_Tablet tablet) const;
	private:
		size_t	fAcquireCount;
	private:
		size_t	fDragAnchor;


	public:
		virtual		void	TypeChar (Led_tChar theChar, unsigned char keyCode, short theModifers);
		virtual		void	DoClick (Led_Point hitPt, short modifierKeys, long /*when*/);


	#if		qMacOS
	public:
		nonvirtual	void	BlinkCaretIfNeeded ();	// just allow public access
													// via idle task mechanism or whatever...
	protected:
		nonvirtual	void	DrawCaret_ ();
		nonvirtual	void	DrawCaret_ (bool on);
	protected:
		bool		fLastDrawnOn;
		int			fTickCountAtLastBlink;
	#endif

	public:
		override	bool	QueryInputKeyStrokesPending () const;

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

	private:	
		ODFrame*		fFrame;
		FocusState*		fAcquiredTablet;
#endif

			private:
				FrameEditor*	fFrameEditor;
		};
		LedODInteractor	fEditor;
		int				fScrollBarScale;


	public:
		nonvirtual	LedODInteractor*		GetEditor ( )					{return &fEditor;}


	friend	class	LedODInteractor;
	#if		qMacOS
		static	pascal	void	MyScrollProc (ControlHandle scrollBar, short ctlPart);
	#endif


	public:
		void	Externalize(ODStorageUnitView* storageUnitView);
		void	CleanseFrameInfoProperty(ODStorageUnit* storageUnit);
		void	ExternalizeFrameInfo(ODStorageUnit* storageUnit,
										ODDraftKey key, ODFrame* scopeFrame);
		void	CloneInto(ODDraftKey key,
										ODStorageUnitView* storageUnitView,
										ODFrame* scopeFrame);
		void	InitFromStorage(ODStorageUnitView* storageUnitView);
					
		ODBoolean		IsFrameActive();
		void			SetFrameActive(ODBoolean active);
		ODBoolean		FrameNeedsReactivating();
		void			SetFrameReactivate(ODBoolean reactivate);
		ODFacet*		GetActiveFacet();
		void			SetActiveFacet(ODFacet* facet);
		void			ReleaseSourceFrame();
		void			AttachFrame(ODFrame* frame);
		void			DetachFrame();
		ODFrame*		GetAttachedFrame();
		ODBoolean		HasAttachedFrame();
		void			SetShouldDisposeWindow(ODBoolean should);
		ODBoolean		ShouldDisposeWindow();



	public:
		nonvirtual	size_t	GetSelectionStart () const;
		nonvirtual	size_t	GetSelectionEnd () const;
		nonvirtual	void	SetSelection (size_t from, size_t to);

	public:
		nonvirtual	Led_Array<Embedding*>	GetEmbeddings () const;		// all embeddings
		nonvirtual	Led_Array<Embedding*>	GetEmbeddings (size_t from, size_t to) const;
	public:
		nonvirtual	Embedding*	IsOneEmbeddedShape () const;	// current selection
		nonvirtual	Embedding*	IsOneEmbeddedShape (size_t from, size_t to) const;


	private:
		nonvirtual	Led_Point	GlobalToLocal (ODFacet* facet, Led_Point p);

	private:
		ODBoolean		fFrameActive;
		ODBoolean		fFrameReactivate;
		ODBoolean		fShouldDisposeWindow;
		ODFacet*		fActiveFacet;
		ODFrame*		fAttachedFrame;
};




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	Point	AsQDPoint (ODPoint p)
		{
			Point pt;
			pt.h = FixedToInt(p.x);
			pt.v = FixedToInt(p.y);
			return pt;
		}
	inline	ODPoint	AsODPoint (Point pt)
		{
			ODPoint	p;
			p.x = IntToFixed(pt.h);
			p.y = IntToFixed(pt.v);
			return (p);
		}
	inline	Led_Point	AsLedPoint (ODPoint p)
		{
			Led_Point pt;
			pt.h = FixedToInt(p.x);
			pt.v = FixedToInt(p.y);
			return pt;
		}
	inline	Rect	AsQDRect (ODRect odr)
		{
			Rect r;
			r.top = FixedToInt (odr.top);
			r.left = FixedToInt (odr.left);
			r.bottom = FixedToInt (odr.bottom);
			r.right = FixedToInt (odr.right);
			return r;
		}
	inline	ODRect	AsODRect (Rect r)
		{
			ODRect	odr;
			odr.top = IntToFixed (r.top);
			odr.left = IntToFixed (r.left);
			odr.bottom = IntToFixed (r.bottom);
			odr.right = IntToFixed (r.right);
			return (odr);
		}
	inline	Led_Rect	AsLedRect (ODRect odr)
		{
			Led_Rect r;
			r.top = FixedToInt (odr.top);
			r.left = FixedToInt (odr.left);
			r.bottom = FixedToInt (odr.bottom);
			r.right = FixedToInt (odr.right);
			return r;
		}
	inline	ODRect	AsODRect (Led_Rect r)
		{
			ODRect	odr;
			odr.top = IntToFixed (r.top);
			odr.left = IntToFixed (r.left);
			odr.bottom = IntToFixed (r.bottom);
			odr.right = IntToFixed (r.right);
			return (odr);
		}

	inline	FrameEditor::LedODInteractor::TmpUseFacetObj::TmpUseFacetObj (FrameEditor::LedODInteractor* doc, ODFacet* facet):
		fDoc (doc),
		fOldFacet (doc->TemporarilyUseFacet (facet))
			{
			}
	inline	FrameEditor::LedODInteractor::TmpUseFacetObj::~TmpUseFacetObj ()
			{
				Led_AssertNotNil (fDoc);
				(void)fDoc->TemporarilyUseFacet (fOldFacet);
			}


inline	bool	FrameEditor::GetHasScrollBar () const
	{
		return (fHasVerticalScrollBar);
	}
inline ODBoolean FrameEditor::IsFrameActive()
{
	return fFrameActive;
}

inline void FrameEditor::SetFrameActive(ODBoolean active)
{
	fFrameActive = active;
}

inline ODBoolean FrameEditor::FrameNeedsReactivating()
{
	return fFrameReactivate;
}

inline void FrameEditor::SetFrameReactivate(ODBoolean reactivate)
{
	fFrameReactivate = reactivate;
}

inline ODFacet* FrameEditor::GetActiveFacet()
{
	return fActiveFacet;
}

inline void FrameEditor::SetActiveFacet(ODFacet* facet)
{
	fActiveFacet = facet;
}

inline ODFrame* FrameEditor::GetAttachedFrame()
{
	return fAttachedFrame;
}

inline ODBoolean FrameEditor::HasAttachedFrame()
{
	return (fAttachedFrame != kODNULL);
}

inline void FrameEditor::SetShouldDisposeWindow(ODBoolean should)
{
	fShouldDisposeWindow = should;
}

inline ODBoolean FrameEditor::ShouldDisposeWindow()
{
	return fShouldDisposeWindow;
}
inline ODWindow* FrameEditor::GetPartWindow() const
{
	return fPartWindow;
}

inline ODBoolean FrameEditor::HasPartWindow() const
{
	return (fPartWindow != kODNULL);
}

inline void FrameEditor::SetPartWindow(ODWindow* window)
{
	fPartWindow = window;
}
		inline	ODBoolean	FrameEditor::HasSourceFrame () const
{
	return (fSourceFrame != NULL);
}
		inline	ODFrame*	FrameEditor::GetSourceFrame () const
{
	return fSourceFrame;
}

#endif

