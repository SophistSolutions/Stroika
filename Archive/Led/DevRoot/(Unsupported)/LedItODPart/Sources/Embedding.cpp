/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/Embedding.cpp,v 2.13 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: Embedding.cpp,v $
 *	Revision 2.13  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.12  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1996/09/30  14:45:19  lewis
 *	Played around with a bunch of different hacks to get qUseOffscreenBitmapsToReduceFlicker working.
 *
 *	Revision 2.10  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.9  1996/05/23  20:35:50  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/05/14  20:59:56  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1996/04/18  16:16:09  lewis
 *	react in a small way to some led base class embedding changes.
 *
 *	Revision 2.6  1996/03/16  19:33:56  lewis
 *	Fixed the RequestShape() stuff - so when an embedding requests a new shape
 *	we update our textimager cache so the text flows around the new size
 *	properly.
 *	React to some tab-code changes made in led - some apis changed.
 *	Plus fixed embeddings to give right baseline, draw entire area, and draw
 *	from the baseline, not the top - so we get embeddings all aligned along
 *	the baseline.
 *
 *	Revision 2.5  1996/03/05  18:47:36  lewis
 *	Removed comment char from header comment - oops.
 *	Renamed Led_ClipboardObjectAcquire::ClipFormat -> Led_ClipFormat.
 *
 *	Revision 2.4  1996/03/04  08:12:47  lewis
 *	Added needed overrides for new methods in base class -GetTag/GetWriteToClipAsFormat, Write (SinkStream_& sink).
 *
 *	Revision 2.3  1996/02/26  23:12:18  lewis
 *	kBadIndex instead of size_t (-1)
 *
 *	Revision 2.2  1996/02/05  05:07:03  lewis
 *	A number of changes to proxy frame stuff based on looking at DR4final DrawShapes diffs.
 *
 *	Revision 2.1  1996/01/04  00:25:53  lewis
 *	Lose G/SetExternalizationIndex () stuff - unused.
 *	DrawHandles () call if we are singly selected embedding.
 *
 *	Revision 2.0  1995/12/15  04:59:25  lewis
 *	*** empty log message ***
 *
 *	Revision 1.5  1995/12/15  04:59:00  lewis
 *	*** empty log message ***
 *
 *	Revision 1.4  1995/12/15  04:04:59  lewis
 *	Massive diffs.
 *	Added logical clipping on display of embeddings.
 *	fFrameProxies now a Led_Array instead of COrdList (most of the diffs from this)
 *	Some change to purge/InMemory for frame proxies based on email from
 *	the net (Jens I think?). Anyhow, its all highly questionable. All that crap.
 *	New NoteThatEmbeddingHasBeenDrawn () etc hack to speed up checking for
 *	embeddings that have scrolled out of view, and thus need to be
 *	uninitialized (removed. or whatever - maybe the fact I don't know what
 *	to call this is why its so flakey :-)
 *
 *	Revision 1.3  1995/12/13  06:15:18  lewis
 *	Massive cleanups/changes.
 *	Noteworthy onesL
 *	lost subscribe/linking support.
 *	Lose most Environment* args.
 *	Mostly got working deleing embedded parts, and scrolling them out of view.
 *
 *	Revision 1.2  1995/12/09  05:52:14  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  1995/12/06  02:03:11  lewis
 *	Initial revision
 *	
 *
 *
 *
 */

#include	<GXMath.h>


// so long as we use this, it must be included first!!!
#include	"AltPoint.h"


#include	<Canvas.xh>
#include	<Shape.xh>
#include	<Facet.xh>
#include	<FacetItr.xh>
#include	<Frame.xh>
#include	<Trnsform.xh>
#include	<FrFaItr.xh>
#include	<Window.xh>
#include	<StorageU.xh>
#include	<ODSessn.xh>
#include	<Part.xh>

#include	<TempObj.h>
#include	<TempIter.h>
#include	<ODUtils.h>
#include	<UseRsrcM.h>			// allow access to our library's resource fork
#include	<ODMemory.h>			// ODDisposePtr
#include	<FocusLib.h>
#include	<StorUtil.h>
#include	<OrdColl.h>
#include	<ODDebug.h>
#include	<UtilErrs.h>

#include	"SampleCollections.h"
#include	"FrameProxy.h"
#include	"LedContent.h"
#include	"LedPart.h"
#include	"LedPartGlobals.h"

#include	"Embedding.h"





const short kNumberOfHandles = 4;

// ----- handles -----
const short kInTopLeftCorner = 1;
const short kInBottomRightCorner = kInTopLeftCorner + 1;
const short kInTopRightCorner = kInBottomRightCorner + 1;
const short kInBottomLeftCorner = kInTopRightCorner + 1;



inline	Environment*	SafeGetEnvironment ()
	{
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_AssertNotNil (ev);
		return ev;
	}



static	void 		MapRect( Rect& src1, Rect& src2, Rect* dest );
static	void 		SortRect( Rect* rect );





const	Rect	kZeroRect	=	{0, 0, 0, 0};


const	Led_Distance	kEmbeddingVMargin	=	4;
const	Led_Distance	kEmbeddingHMargin	=	4;






// Attempts at making qUseOffscreenBitmapsToReduceFlicker work with OPENDOC. Sigh...
// What a totally fucking guess-ridden mess. I really don't understand at all the various
// points of reference for these transforms. And getting them just right so that
// the embeddings display is something I just don't have patience for right now - LGP 960913

#ifndef	qTryOffscreenCanvasHack1
	#define	qTryOffscreenCanvasHack1	0
#endif
#ifndef	qTryOffscreenCanvasHack2
	#define	qTryOffscreenCanvasHack2	qUseOffscreenBitmapsToReduceFlicker
#endif




/*
 ********************************************************************************
 ********************************** Embedding ***********************************
 ********************************************************************************
 */
Embedding::Embedding (LedPart* ledPart)
{
	fRect = kZeroRect;
	fSelected = kODFalse;
	
	fLedPart = ledPart;
}

Embedding::~Embedding()
{
	for (size_t i = 0; i < fFrameProxies.GetLength (); i++) {
		delete fFrameProxies[i+qLedFirstIndex];
	}
}

void	Embedding::GetUpdateShape(Environment* ev, ODShape* updateShape) const
{
	ODRect bounds(fRect);
	bounds.left -= fl(2.5); 
	bounds.top -= fl(2.5);
	bounds.right += fl(2.5);
	bounds.bottom += fl(2.5);
	updateShape->SetRectangle(ev, &bounds);
}

void	Embedding::GetUpdateBox (ODRect* updateBox) const
{
	ODRect bounds(fRect);
	updateBox->left -= fl(2.5);
	updateBox->top -= fl(2.5);
	updateBox->right += fl(2.5);
	updateBox->bottom += fl(2.5);
}

extern	ODFacet*	gImDoingFacet;	// hack set in LedPart::DrawFrameView
void	Embedding::DrawSegment (const StyledTextImager* imager, Led_Tablet tablet,
											size_t from, size_t to, const Led_tChar* text,
											const Led_Rect& drawInto, Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										)
{
	Led_Require (to-from == 1);
	Led_Require (text[0] == kEmbeddingSentinalChar);

	Led_Color	foreColor	=	Led_GetTextColor ();
	Led_Color	backColor	=	Led_GetTextBackgroundColor ();
	#if		qMacOS
		::RGBForeColor (&foreColor);
		::RGBBackColor (&backColor);
	#elif	qWindows && 0
		Led_Win_Obj_Selector	pen (tablet, ::GetStockObject (WHITE_PEN));
		tablet->SetTextColor (foreColor);
		tablet->SetBkColor (backColor);
	#endif

	Environment*	ev = ::somGetGlobalEnvironment();
	Led_AssertNotNil (ev);


	Led_Coordinate	embedBottom	=	useBaseLine;
	Led_Coordinate	embedTop	=	embedBottom - GetRectHeight (fRect);
	Led_Assert (embedTop >= drawInto.top);
	Led_Assert (embedBottom <= drawInto.bottom);
	Led_Rect	ourBoundsRect	=	Led_Rect (Led_Point (embedTop, drawInto.left + kEmbeddingHMargin), AsLedSize (::GetRectSize (fRect)));
	if (GetRectOrigin (ourBoundsRect) != GetRectOrigin (AsLedRect (fRect))) {
		SetBoundingBox (ev, AsQDRect (ourBoundsRect));
	}
	
	if (pixelsDrawn != NULL) {
		*pixelsDrawn = GetRectWidth (fRect) + 2*kEmbeddingVMargin;
	}

	Led_Rect	realUpdateRect	=	drawInto;
	realUpdateRect.right += GetRectWidth (fRect) + 2*kEmbeddingVMargin;

	{
		#if		qDebug
			GrafPort*	xx	=	qd.thePort;
			int savedTop = xx->portRect.top;
			int savedLeft = xx->portRect.left;
		#endif

		// Draw the embedded part
	
		Led_AssertNotNil (gImDoingFacet);


		fLedPart->NoteThatEmbeddingHasBeenDrawn (this);

	
	/*
	 *	Try to erase the bounds, minus the used-shape of the embedding.
	 */
	#if 1
	{
		RgnHandle	eraseRgn = ODNewRgn ();
		Led_AssertNotNil (eraseRgn);
		Rect	rrr	=	AsQDRect (drawInto);
		::RectRgn (eraseRgn, &rrr);
	
		CEmbeddedFrameProxy* frameProxy = fFrameProxies[qLedFirstIndex];
	Led_AssertNotNil (frameProxy);
	TempODShape	frameUsedShape	=	frameProxy->GetFrame (ev)->AcquireUsedShape (ev, NULL);
	Led_AssertNotNil (frameUsedShape);


RgnHandle	rh	=	frameUsedShape->GetQDRegion (ev);
RgnHandle	xxxx = ODNewRgn ();
::CopyRgn (rh, xxxx);

::OffsetRgn (xxxx, fRect.left, fRect.top);


		::DiffRgn (eraseRgn, xxxx, eraseRgn);
::DisposeRgn (xxxx);
		::EraseRgn (eraseRgn);
		::DisposeRgn (eraseRgn);
	}
	#endif
	
		// Temp Shape
		TempODShape tShape = gImDoingFacet->CreateShape(ev);
		
		// Get the clipping region & convert to ODShape
		TempODShape tClippingShape = gImDoingFacet->CreateShape(ev);
		{
			RgnHandle tRegion = ODNewRgn ();		//	SetQDRegion () takes over ownership of the emory
			GetClip(tRegion);
			tClippingShape->SetQDRegion(ev, tRegion);	// SetQDRegion consumes the region
		}
	
		for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
			CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
			// Getting the frame will also ensure that there is one ( a frame, that is )
			ODFrame* embeddedFrame = frameProxy->GetFrame(ev);
			Led_AssertNotNil (embeddedFrame);
	
			for (TempODFrameFacetIterator iter (ev, embeddedFrame); iter; ++iter ) {
				ODFacet* embeddedFacet	=	iter;


#if qTryOffscreenCanvasHack1
				ODPlatformCanvas	oldCanvas	=	embeddedFacet->GetCanvas (ev)->GetPlatformCanvas (ev, kODQuickDraw);
				embeddedFacet->GetCanvas (ev)->SetPlatformCanvas (ev, kODQuickDraw, tablet);	
				embeddedFacet->ChangeCanvas (ev, embeddedFacet->GetCanvas (ev));	
				try {
#endif
#if qTryOffscreenCanvasHack2
				ODCanvas*	offCanv		=	embeddedFacet->CreateCanvas (ev, kODQuickDraw, tablet, false, true);
//		fLedPart->GetODPart ()->Acquire (ev);	// up ref count for setowner - now 100% sure why? But get error quitting without it...
				offCanv->SetOwner (ev, fLedPart->GetODPart ());

// Create a biasTransform to adjust for setorigin...
ODTransform* t	=	embeddedFacet->CreateTransform(ev);
Point	foo	=	*(Point*)&tablet->portRect;
t->SetQDOffset (ev, &foo);
offCanv->SetBiasTransform(ev, t);
ODReleaseObject(ev, t);
				ODCanvas*	oldCanvas	=	embeddedFacet->HasCanvas (ev)? embeddedFacet->GetCanvas (ev): NULL;
				embeddedFacet->ChangeCanvas (ev, offCanv);	
				try {
#endif
				// Set up the clip for the embedded facet
				// DCS $$$$$ ? Need this ??
				CFocus embeddedDraw(ev, embeddedFacet);
	
				tShape->CopyFrom(ev, tClippingShape);
				
				// Convert shape's transform from parent frame to embedded frame
#if		qTryOffscreenCanvasHack2
				TempODTransform tTransform = embeddedFacet->AcquireExternalTransform(ev, offCanv);
#else
				TempODTransform tTransform = embeddedFacet->AcquireExternalTransform(ev, kODNULL);
#endif
				tShape->InverseTransform(ev, tTransform);
						
				// Draw the facet
#if		qTryOffscreenCanvasHack2
				embeddedFacet->Draw (ev, tShape, kODNULL);
				embeddedFacet->DrawChildren (ev, tShape, kODNULL);
#else
				embeddedFacet->Draw (ev, tShape, kODNULL);
				embeddedFacet->DrawChildren (ev, tShape, kODNULL);
#endif
				
				// Get the embeded facet's clip shape so we can subtract it from our working clip
#if		qTryOffscreenCanvasHack2 && 0
				TempODShape tEmbeddedClipShape = embeddedFacet->AcquireClipShape(ev, offCanv);
#else
				TempODShape tEmbeddedClipShape = embeddedFacet->AcquireClipShape(ev, kODNULL);
#endif
				tShape->CopyFrom(ev, tEmbeddedClipShape);

				tShape->Transform(ev, tTransform);	// back to OUR facet coords
#if 1
				// Remove the drawn area from our clip shape
				tClippingShape->Subtract(ev, tShape);
#endif
#if qTryOffscreenCanvasHack1
					embeddedFacet->GetCanvas (ev)->SetPlatformCanvas (ev, kODQuickDraw, oldCanvas);	
					embeddedFacet->ChangeCanvas (ev, embeddedFacet->GetCanvas (ev));	
				}
				catch (...) {
					embeddedFacet->GetCanvas (ev)->SetPlatformCanvas (ev, kODQuickDraw, oldCanvas);
					embeddedFacet->ChangeCanvas (ev, embeddedFacet->GetCanvas (ev));	
					throw;
				}
#endif
#if qTryOffscreenCanvasHack2
					embeddedFacet->ChangeCanvas (ev, oldCanvas);
					offCanv->SetOwner (ev, NULL);
					delete offCanv;
				}
				catch (...) {
					embeddedFacet->ChangeCanvas (ev, oldCanvas);
					offCanv->SetOwner (ev, NULL);
					delete offCanv;
					throw;
				}
#endif
			}
		}

	// used to check if (segmentHilighted) too - should maybe check here if printing, or active, or other
	// tests???
		if (imager->GetSelectionStart () == from and imager->GetSelectionEnd () == to) {
			DrawHandles ();
		}

		#if		qDebug
			Led_Assert (xx	==	qd.thePort);
			Led_Assert (savedTop == xx->portRect.top);
			Led_Assert (savedLeft == xx->portRect.left);
		#endif
	}
}

void	Embedding::MeasureSegmentWidth (const StyledTextImager* /*imager*/, size_t from, size_t to,
														const Led_tChar* text,
														Led_Distance* distanceResults
													) const
{
	Led_Assert (from + 1 == to);
	Led_Assert (text[0] == kEmbeddingSentinalChar);
	distanceResults[0]	=	::GetRectWidth (fRect) + 2*kEmbeddingHMargin;
}

Led_Distance	Embedding::MeasureSegmentBaseLine (const StyledTextImager* /*imager*/, size_t from, size_t to) const
{
	// Baseline for embeddings should be very bottom of the embedding - including the top, but NOT the bottom margin
	Led_Assert (from + 1 == to);
	return ::GetRectHeight (fRect) +  kEmbeddingVMargin;
}

Led_Distance	Embedding::MeasureSegmentHeight (const StyledTextImager* /*imager*/, size_t from, size_t to) const
{
	Led_Assert (from + 1 == to);
	return ::GetRectHeight (fRect) +  2*kEmbeddingVMargin;
}

void	Embedding::DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo)
{
	if (GetLength () == 0) {
		Led_AssertNotNil (GetOwner ()->PeekAtTextStore ());
		GetOwner ()->PeekAtTextStore ()->RemoveMarker (this);

		fLedPart->RemoveShape (this);
		for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
			CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
			frameProxy->RemoveAndPurge();
		}
		delete this;
	}
	else {
		SimpleEmbeddedObjectStyleMarker::DidUpdateText (updateInfo);
	}	
}

void	Embedding::Write (SinkStream_& /*sink*/)
{
	Led_Assert (false);		// cuz we (currently) need fInfo, and fStorageUnit parameters.
							// NativeFormatWriterWithODEmebddings overrides ExternalizeEmbeddings
							// to provide these parameters...
							// This shouldn't be called directly (til we find a way to get along with
							// those params..
}

void	Embedding::ExternalizeFlavors (TextInteractor_::WriterFlavorPackage& flavorPackage)
{
	Led_Assert (false);	// See Embedding::Write ();
}

const char*	Embedding::GetTag () const
{
	return kODEmbedTag;
}

void	Embedding::GetBoundingBox(Rect* bounds) const
{
	*bounds = fRect;
}

void	Embedding::GetBoundingBox(ODRect* bounds) const
{
	*bounds = fRect;
}

void	Embedding::GetHandleCenter (short whichHandle, Point* center) const
{	
	switch (whichHandle) {
		case kInTopLeftCorner:
			center->h = fRect.left;
			center->v = fRect.top;
			break;
		case kInTopRightCorner:
			center->h = fRect.right;
			center->v = fRect.top;
			break;
		case kInBottomLeftCorner:
			center->h = fRect.left;
			center->v = fRect.bottom;
			break;
		case kInBottomRightCorner:
			center->h = fRect.right;
			center->v = fRect.bottom;
			break;
	}
}

void	Embedding::CalcHandle (short whichHandle, Rect* bounds) const
{
	Point pt;
	GetHandleCenter(whichHandle, &pt);
	
	bounds->top = pt.v - 2;
	bounds->left = pt.h - 2;
	bounds->bottom = pt.v + 2;
	bounds->right = pt.h + 2;
}

void	Embedding::DrawHandles()
{
	// Reset the Pen
	PenNormal();
	
	// Set the color
	RGBForeColor (&kBlackColor);
	for (short i=1; i<=kNumberOfHandles; i++) {
		Rect handleRect;
		CalcHandle(i, &handleRect);	
		PaintRect(&handleRect);
	}
}

ODBoolean	Embedding::ShapeInRectangle(const Rect& rect) const
{
	Rect bounds;
	GetBoundingBox(&bounds);
	
	Rect intersection;
	SectRect(&bounds, &rect, &intersection);
	
	// If the Intersection is equal to the bounds
	// then we are fully contained by the given rectangle.
	return EqualRect(&bounds,&intersection);
}

ODSShort	Embedding::WhichHandle(const Point& mouse) const
{
	Rect handleRect;
	for (short i=1; i<=kNumberOfHandles; i++)
	{
		CalcHandle( i, &handleRect );
		if (::PtInRect(mouse, &handleRect))
			return i;
	}
		
	return 0;
}

void	Embedding::GetDragRect(Rect* dragRect) const
{
	GetBoundingBox(dragRect);
}

ODShape*	Embedding::CreateDragShape(Environment *ev, ODFacet* facet)
{
	ODRect bounds(fRect);
		
	ODShape* dragRgn = facet->CreateShape(ev);	
	dragRgn->SetRectangle(ev, &bounds);
	
	bounds.Inset(ff(1), ff(1));
	
	ODShape* donutHole = facet->CreateShape(ev);	
	donutHole->SetRectangle(ev, &bounds);
	
	dragRgn->Subtract(ev, donutHole);
	ODReleaseObject(ev, donutHole);
	
	return dragRgn;
}

void	Embedding::ResizeFeedback(ODFacet* facet, short whichHandle, Point& mouseLoc)
{
	Rect srcRect, dstRect;
	GetResizeRect(whichHandle, mouseLoc, &srcRect, &dstRect);
	SortRect(&dstRect);
	::FrameRect(&dstRect);			
}

void	Embedding::GetResizeRect(ODSShort whichHandle, Point& mouseLoc, Rect* srcRect, Rect* dstRect)
{
	*srcRect = fRect;
	*dstRect = *srcRect;
	
	switch (whichHandle)
	{
		case kInTopLeftCorner:
			dstRect->left = mouseLoc.h;
			dstRect->top = mouseLoc.v;
			break;
		case kInTopRightCorner:
			dstRect->right = mouseLoc.h;
			dstRect->top = mouseLoc.v;
			break;
		case kInBottomLeftCorner:
			dstRect->left = mouseLoc.h;
			dstRect->bottom = mouseLoc.v;
			break;
		case kInBottomRightCorner:
			dstRect->right = mouseLoc.h;
			dstRect->bottom = mouseLoc.v;
			break;
	}
}

void	Embedding::SetBoundingBox (Environment* ev, const Rect& bounds)
{
	fRect = bounds;
	
	// Update frame shapes for frames we are for which we are proxying
	SetProxyBounds (ev, fRect);
}

void	Embedding::SetBoundingBox (Environment* ev, ODShape* bounds)
{
	THROW_IF_NULL(bounds);
	
	ODRgnHandle tRegion = bounds->GetQDRegion(ev);
	Rect tRect = (*tRegion)->rgnBBox;
	
	SetBoundingBox(ev, tRect);
	
	// Update frame shapes for frames we are for which we are proxying
	SetProxyBounds(ev, fRect);
}

void	Embedding::SetBoundingBox(Environment* ev, const Point& anchorPoint,  const Point& currentPoint)
{
	if (anchorPoint.h < currentPoint.v)
	{
		fRect.left = anchorPoint.h;
		fRect.right = currentPoint.h;
	}
	else
	{
		fRect.left = currentPoint.h;
		fRect.right = anchorPoint.h;
	}
	
	if (anchorPoint.v < currentPoint.v)
	{
		fRect.top = anchorPoint.v;
		fRect.bottom = currentPoint.v;
	}
	else
	{
		fRect.top = currentPoint.v;
		fRect.bottom = anchorPoint.v;
	}
	
	// Update frame shapes for frames we are for which we are proxying
	SetProxyBounds(ev, fRect);
}

void	Embedding::SetProxyBounds(Environment* ev, const Rect& bounds)
{
	ODRect tRect(bounds);
	
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		frameProxy->ResizeFrame(ev, tRect);
	}
}

void	Embedding::AddFrameProxy (CEmbeddedFrameProxy* frameProxy)
{
	Led_RequireNotNil (frameProxy);
	Led_Require (IndexOf (fFrameProxies, frameProxy) == kBadIndex);
	fFrameProxies.Append (frameProxy);
}

void	Embedding::Embed (Environment* ev,  ODPart* part, ODFrame* containingFrame, ODID embeddedFrameID)
{
	ODDraft*	draft = fLedPart->GetDraft ();

	// Frames are represented by proxies
	CEmbeddedFrameProxy* frameProxy = new CEmbeddedFrameProxy ();
	
	if (embeddedFrameID == kODNULL) {
		// Make a proxy
		frameProxy->InitializeEmbeddedFrameProxy (ev, fLedPart, this, containingFrame, part);
	}
	else {
		// Make a proxy for it, using the frame passed in
		frameProxy->InitializeEmbeddedFrameProxy(ev, fLedPart,  this, embeddedFrameID, containingFrame);	
	
		// Get the shape of the frame and set the size of the Embedding
		// we should delay this until later. Causing frame to be loaded to early.
		// DCS $$$$$
		ODFrame* tFrame = frameProxy->GetFrame(ev);
		ODShape* tShape = tFrame->AcquireFrameShape(ev, kODNULL);
		ODRgnHandle tODRegion = tShape->GetQDRegion(ev);
		SetBoundingBox(ev, (*tODRegion)->rgnBBox);
		
		// Release acquired geometry
		ODReleaseObject(ev, tShape);
	}												

	// Add to our list of frameProxies
	AddFrameProxy(frameProxy);
}

#if 0
// An (de)activate, suspend, or resume event has occured. Change the highlight state of
// corresponding facets' highlight state to reflect the change in active state.
void	Embedding::Activate (Environment* ev, ODBoolean activating, ODFrame* frame)
{
	ODFacet* tFacet = GetEmbeddedFacet(frame);

	ODHighlight highlight;
	if ( IsSelected() && activating ) highlight = kODFullHighlight;
	else if ( IsSelected() && !activating ) highlight = kODDimHighlight;
	else highlight = kODNoHighlight;
	
	tFacet->ChangeHighlight (ev, highlight);
}
#endif

void	Embedding::Resize(Environment *ev, Rect& baseRect, Rect& resizeRect)
{
	MapRect (baseRect, resizeRect, &fRect);
	ODRect		odResizeRect(fRect);
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		frameProxy->ResizeFrame(ev, odResizeRect);
	}
}

ODFacet* Embedding::GetEmbeddedFacet (ODFacet* containingFacet)
{	
	Environment*	ev	=	SafeGetEnvironment ();
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		// Check all facets 
		for (TempODFrameFacetIterator	iter2 (ev, frameProxy->GetFrame(ev)); iter2; ++iter2) {
			ODFacet* facet = iter2;
			if (facet->GetContainingFacet(ev)->GetFrame(ev) == containingFacet->GetFrame(ev)) {
				return (facet);
			}
		}
	}
	return NULL;
}

ODFacet*	Embedding::GetEmbeddedFacet (ODFrame* containingFrame)
{	
	Environment*	ev	=	SafeGetEnvironment ();
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		// Check all facets 
		for (TempODFrameFacetIterator	iter2 (ev, frameProxy->GetFrame(ev)); iter2; ++iter2) {
			ODFacet* facet = iter2;
			if (facet->GetContainingFacet(ev)->GetFrame(ev) == containingFrame) {
				return (facet);
			}
		}
	}
	return NULL;
}

// Sometimes OpenDoc just wants a frame. So, this method will return the first
// embedded frame ( in memory ) found for "this" shape. kODNULL is returned
// if no frames are in memory.
//------------------------------------------------------------------------------
ODFrame*	Embedding::GetAnyFrame(Environment *ev)
{	
	// Iterate over all the proxies to locate the one belonging to the given parent
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		// If frameProxy's frame is loaded, return it
		if (frameProxy->IsFrameInMemory()) {
			return frameProxy->GetFrame(ev);
		}
	}
	return kODNULL;
}

// This method should return kODTrue if the given frame is embedded within this
// item of content. 
//
// NOTE: Does not cause load of frame.
ODBoolean	Embedding::ContainsProxyForFrame(Environment* ev, ODFrame* frame)
{
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		if (frame->GetID(ev) == frameProxy->GetFrameID())
			return kODTrue;
	}
	
	// Did not find a match
	return kODFalse;
}

void	Embedding::SetInLimbo (ODBoolean isInLimbo)
{
	// Notify all of our proxies of limbo status. See limbo discussion
	// in the programmers guide for more details.
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		frameProxy->SetInLimbo (::SafeGetEnvironment (), isInLimbo);
	}
}


//------------------------------------------------------------------------------
// CEmbeddingShape::SetShapeRectangle
//
// Set the rectangle for the shape without adjusting the embedded frames bounds.
// This is necessary in cases where OpenDoc is gonig to change the embedded frame
// shape on us and we need to adjust the intrinsic shape's bounds without changing
// the frame shape ( causing stack overflow ). 
void	Embedding::SetShapeRectangle (ODShape* bounds)
{
	// Change the bounds rect of the shape without affecting the embedded frame(s)
	ODRgnHandle tRegion = bounds->GetQDRegion(SafeGetEnvironment ());
	Rect tRect = (*tRegion)->rgnBBox;

	// if changed, we must invalidate our cached size from the MultiRowTextImager...
	if (not ::EqualRect (&fRect, &tRect)) {
		fRect = tRect;
		// make sure all the markersowners (imagers mainly) get updated so they invalidate their
		// cached sizes...
		UpdateInfo	updateInfo (GetStart (), GetEnd (), "", 0, false);
		TextStore_&	textStore	=	fLedPart->GetTextStore ();
		size_t	from	=	GetStart ();
		size_t	to		=	GetEnd ();
		Led_Array<Marker*>	allMarkersInRange	=	textStore.CollectAllMarkersInRange ((from>1)? (from-1): 1, (to <= textStore.GetLength ()+1)?(to+1): to);
		textStore.DoAboutToUpdateCalls (updateInfo, allMarkersInRange);
		textStore.DoDidUpdateCalls (updateInfo, allMarkersInRange);
	}
}

// This method is called to remove a frame from the document. If the commit flag is false
// then the shape will merely be detached with the assumption that the shape will be later
// re-added. If the commit flag is true, then the shape will PERMANENTLY be removed. That
// is to say, the frame will be removed from the parts embedded frame list, the Remove
// method will be called on the frame ( notifiying OD that the frame should be dismantled.
void	Embedding::Removed (ODBoolean commit)
{
	// Iterate over all the proxies to locate the one belonging to the given parent
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		if (frameProxy->IsAttached())
			frameProxy->Detach();	
		
		if (commit)
		{
			frameProxy->RemoveAndPurge();
		}	
	}
}

void	Embedding::Added ()
{
	// Add the shape ( frame ) to the document
	SetInLimbo (kODFalse);
}

// This method is called by the part when the user has selected "Open" for a selection.
// This will probably be only overridden by the proxy derivatives of shape.
//----------------------------------------------------------------------------------------
void Embedding::Open (Environment *ev, ODFrame* container)
{
	// Use the embedded frame for the provided container 
	ODFrame* tFrame = GetEmbeddedFacet (container)->GetFrame(ev);
	
	// tell the part to open
	ODPart*	 tPart = tFrame ? tFrame->AcquirePart(ev) : kODNULL;
	if (tPart)
	{
		tPart->Open(ev, tFrame);
		
		ODReleaseObject(ev, tPart);
	}
}

// This method is called by the part when a document is loaded and one of its display 
// frames has been internalized from storage and "connected" into the frame hierarchy.
// When this happens, we must execute the below special case code to make sure that our
// embedded content is re-internalized correctly.
void	Embedding::DisplayFrameConnected(Environment *ev, ODFrame* frame)
{
fLedPart->NoteThatEmbeddingHasBeenDrawn (this);	// hack!!! LGP 951213
	// SPECIAL CASE: In the case where a document was saved with embedded content AND then
	// dragged into a container which only clones the root part and does not use the root
	// frame for the document. In this case we must assume that any orphaned frame proxies
	// belong to the frame being added, and therefore attach the given frame to the orphaned
	// proxy frames. It is possible that there may have been multiple saved ( persistent )
	// display frames. That being the case, we will return as soon as we find 1 orphaned
	// proxy ( after having re-attached it to the passed in dispaly frame ). This way, if there
	// were multiple display frames saved, each one will get a proxy as it is internalized.
	// I can't guarantee correct restoration of ordering ( proxy to frame ), but that shouldn't
	// be important ( famous last words ).
	
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		if (frameProxy->IsOrphaned()) {
			frameProxy->SetContainingFrame (frame->GetID(ev));
			return;
		}
	}
}

// This method is called by the part whenever a new display frame is added to the 
// container part. We must create a new embedded frame for this new container frame so
// that this shape will be displayed correctly in that new frame.
void	Embedding::DisplayFrameAdded(Environment *ev, ODFrame* frame)
{
	// Find the part that this shape is 'wrapping'
	ODPart* embeddedPart = kODNULL;

	Led_Require (fFrameProxies.GetLength () != 0);	// Can't add frame to shape, there are no proxies
	
	// Scan proxies. If we have an orphaned proxy, attach it to the given frame, otherwise
	// Try to access a part reference with which we can create a new proxy for the given
	// frame.	
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		if (frameProxy->IsOrphaned())
		{
			// See special case comment in CEmbeddingShape::DisplayFrameConnected above.
			frameProxy->SetContainingFrame (frame->GetID(ev));
			return;
		}
		else
		if (frameProxy->IsFrameInMemory()) {
			embeddedPart = frameProxy->GetFrame(ev)->AcquirePart(ev);
			if (embeddedPart != NULL) {
				break;
			}
		}
	}

	if (embeddedPart)
	{
		// Make a proxy for the frame
		CEmbeddedFrameProxy*	frameProxy = new CEmbeddedFrameProxy();
		frameProxy->InitializeEmbeddedFrameProxy(ev, 
									fLedPart, 
									this, 
									frame, 
									embeddedPart);
	
		// Add to our list of frameProxies
		AddFrameProxy(frameProxy);
		
		// Release acquired part
		ODReleaseObject(ev, embeddedPart);
	}
fLedPart->NoteThatEmbeddingHasBeenDrawn (this);	// hack!!! LGP 951213
}

// This method is called by the part when a display frame is removed.
// This allows us to add a facet to the embedded frame wrapped by this shape.
void	Embedding::DisplayFrameRemoved(Environment *ev, ODFrame* frame)
{	
	// I am assuming that there will be only one frame proxy for a given frame
	CEmbeddedFrameProxy* tProxy = kODNULL;
	
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		// ----- Remove the frame & proxy -----
		ODFrame* proxyContainer = frameProxy->AcquireContainingFrame(ev);
		if (proxyContainer==frame)
		{
			tProxy = frameProxy;
			ODFrame* embeddedFrame = tProxy->GetFrame(ev);
		
			// Remove abandoned frame proxy from our list
			// Shouldn't call GetFrame after this.		
			tProxy->Detach();
		
			/// ODFrame::Remove calls release
			tProxy->RemoveAndPurge();
		}
		
		// Release acquired container frame
		ODReleaseObject(ev, proxyContainer);
	}
	// Remove proxy from this shape's list then delete it.
	//fFrameProxies->Remove(tProxy);
	fFrameProxies.RemoveAt (IndexOf (fFrameProxies, tProxy));
	delete tProxy;
}

// This method is called by the part when a display frame is removed.
// This allows us to add a facet to the embedded frame wrapped by this shape.
void	Embedding::DisplayFrameClosed(Environment *ev, ODFrame* frame)
{	
	// I am assuming that there will be only one frame proxy for a given frame
	CEmbeddedFrameProxy* tProxy = kODNULL;
	
	// Iterate over all the proxies to locate the one belonging to the given parent
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		// ----- Remove the frame & proxy -----
		ODFrame* proxyContainer = frameProxy->AcquireContainingFrame(ev);
		if (proxyContainer==frame)
		{
			tProxy = frameProxy;
			ODFrame*  embeddedFrame = tProxy->GetFrame(ev);
		
			// Remove abandoned frame proxy from our list	
			// Don't call GetFrame on the proxy after this.	
			tProxy->Detach();
		
			// Tell OD to remove the frame, clear the proxy's structure.
			tProxy->CloseAndPurge();
		}
		
		// Release acquired container frame
		ODReleaseObject(ev, proxyContainer);
	}
	// Remove proxy from this shape's list, then delete it
	// Must remove & delete outside of iteration to avoid confusing iterator
	fFrameProxies.RemoveAt (IndexOf (fFrameProxies, tProxy));
	delete tProxy;
}

// This method is called by the part when a the user is or was dragging this shape.
// Notify the embedded frame of the state of dragging.
void	Embedding::Dragging(Environment *ev, ODBoolean dragging)
{
	// We don't know exactly *which* frame was envolved in the drag
	// so notify all of our proxies.
	// Iterate over all the proxies to locate the one belonging to the given parent
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		ODFrame* frame = frameProxy->GetFrame(ev);
		frame->SetDragging(ev, dragging);
	}
}

// This method is called by the part when a new containing facet is added.
// This allows us to add a facet to the embedded frame wrapped by this shape.
void	Embedding::FacetAdded(Environment *ev, ODFacet* facet)
{
fLedPart->NoteThatEmbeddingHasBeenDrawn (this);	// hack!!! LGP 951213

	// Iterate over all the proxies to locate the one belonging to the given parent
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		// ----- Add facets to the frame being proxy, IF the frame is in memory -----
		
		if (frameProxy->IsFrameInMemory())
		{
			ODFrame* facetContainer = facet->GetFrame(ev);
			ODFrame* proxyContainer = frameProxy->AcquireContainingFrame(ev);
			if (facetContainer==proxyContainer)
			{
				frameProxy->CreateFacetsForContainer(ev, facet);
			}
			
			// Set the hilight state of the embedded facet
			ODFacet* tFacet = GetEmbeddedFacet (facet);
			
			ODHighlight highlight;
			ODBoolean active = facet->GetWindow(ev)->IsActive(ev);
		
			if ( IsSelected() && active ) highlight = kODFullHighlight;
			else if ( IsSelected() && !active ) highlight = kODDimHighlight;
			else highlight = kODNoHighlight;
			
			tFacet->SetSelected(ev, IsSelected());
			tFacet->ChangeHighlight(ev, highlight);
			
			// Release acquired container frame
			ODReleaseObject(ev, proxyContainer);
		}
	}
}

// This method is called by the part when a containing facet is removed.
// This allows us to remove the facet contained by the facet that is removed.
void	Embedding::FacetRemoved(Environment *ev, ODFacet* facet)
{
	// Iterate over all the proxies to locate the one belonging to the given parent
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
		// ----- Remove the frame & proxy -----
		ODFrame* facetContainer = facet->GetFrame(ev);
		ODFrame* proxyContainer = frameProxy->AcquireContainingFrame(ev);
		if (facetContainer==proxyContainer)
		{
			frameProxy->RemoveFacetsForContainer(ev, facet);
		}
		
		// Release acquired container frame
		ODReleaseObject(ev, proxyContainer);
	}
}

// We assume that the storage unit is focused to a property and value and
// that the current offset is the beginning of a shape.
void	Embedding::Read(Environment* ev, ODStorageUnitView* view, CloneInfo* cloneInfo)
{
	Embedding::Read (ev, view->GetStorageUnit(ev), cloneInfo);
fLedPart->NoteThatEmbeddingHasBeenDrawn (this);	// hack!!! LGP 951213
}

// We assume that the storage unit is focused to a property and value and
// that the current offset is the beginning of a shape.
void	Embedding::Read(Environment* ev, ODStorageUnit* storage, CloneInfo* cloneInfo)
{
	// tmp hack...
	struct CRGBColor {
		unsigned short	red;						/*magnitude of red component*/
		unsigned short	green;						/*magnitude of green component*/
		unsigned short	blue;						/*magnitude of blue component*/
	};
	TRY
		// Get the rectangle
		StorageUnitGetValue(storage, ev, sizeof(Rect), &fRect);
		
		// Get the shape type
		ODSShort	oldUnused;
		StorageUnitGetValue(storage, ev, sizeof(ODSShort), &oldUnused);
		
		// Get the number of handles
		
		StorageUnitGetValue(storage, ev, sizeof(ODSShort), &oldUnused);
	
		// Get the shape color
		CRGBColor	tmpHack;
		StorageUnitGetValue(storage, ev, sizeof(CRGBColor), &tmpHack);
	CATCH_ALL
	ENDTRY
	
	/// Read the number of proxies
	ODULong proxyCount;
	StorageUnitGetValue(storage, ev, sizeof(ODULong), &proxyCount);
	
	// Read in the proxies
	for (ODULong index = 0; index < proxyCount; index++)
	{
		// Make a proxy
		CEmbeddedFrameProxy* frameProxy = new CEmbeddedFrameProxy();
		frameProxy->InitializeEmbeddedFrameProxy(ev, fLedPart, this);
			
		if (frameProxy->Read(ev, storage, cloneInfo))
		{
			AddFrameProxy(frameProxy);
		}
		else
			THROW(kODErrInvalidStorageUnitRef);
	}
fLedPart->NoteThatEmbeddingHasBeenDrawn (this);	// hack!!! LGP 951213
}

// We assume that the storage unit is focused to a property and value and
// that the current offset is a good place to write out a shape.
void Embedding::Write(Environment* ev, ODStorageUnitView* view, CloneInfo* cloneInfo)
{
	Write(ev, view->GetStorageUnit(ev), cloneInfo);
}

// We assume that the storage unit is focused to a property and value and
// that the current offset is a good place to write out a shape.
void Embedding::Write(Environment* ev, ODStorageUnit* storage, CloneInfo* cloneInfo)
{
	// tmp hack...
	struct CRGBColor {
		unsigned short	red;						/*magnitude of red component*/
		unsigned short	green;						/*magnitude of green component*/
		unsigned short	blue;						/*magnitude of blue component*/
	};

	TRY
		// Write the rectangle
		StorageUnitSetValue(storage, ev, sizeof(Rect), &fRect);
		
		// Write the shape type
		ODSShort	oldUnused	=	0;
		StorageUnitSetValue(storage, ev, sizeof(ODSShort), &oldUnused);
		
		// Write the number of handles
		StorageUnitSetValue(storage, ev, sizeof(ODSShort), &oldUnused);
	
		// Write the shape color
		CRGBColor	tmpHack;
		StorageUnitSetValue(storage, ev, sizeof(CRGBColor), &tmpHack);
	CATCH_ALL
	ENDTRY
	
	/// Write the number of proxies
	ODULong proxyCount = fFrameProxies.GetLength ();
	StorageUnitSetValue(storage, ev, sizeof(ODULong), &proxyCount);
	
	// Write out the proxies contained by the scope ( source of copy ) frame
	// specified in the cloneInfo
	
	
	// Iterate over all the proxies to locate the one belonging to the given parent
#if 1
	for (size_t iter = 0; iter < fFrameProxies.GetLength (); iter++) {
		CEmbeddedFrameProxy* frameProxy	=	fFrameProxies[iter+qLedFirstIndex];
#else
	COrdListIterator iter(fFrameProxies);
	for ( CEmbeddedFrameProxy* frameProxy = (CEmbeddedFrameProxy*)iter.First();
			iter.IsNotComplete(); frameProxy = (CEmbeddedFrameProxy*)iter.Next())
	{
#endif
		ODFrame* containingFrame = frameProxy->AcquireContainingFrame(ev);
		
		// If we are cloning, then write only in the case where there is
		// no scope frame or the proxy falls within the scope frame. 
		// Write if we are not doing a clone, as well.
		if (cloneInfo && cloneInfo->fScopeFrame==kODNULL)
			frameProxy->Write(ev, storage, cloneInfo);
		else
		if (cloneInfo && containingFrame==cloneInfo->fScopeFrame)
			frameProxy->Write(ev, storage, cloneInfo);
		else
		if (cloneInfo==kODNULL)
			frameProxy->Write(ev, storage, cloneInfo);
			
		// Release acquired frame
		ODReleaseObject(ev, containingFrame);
	}
}












/*
 ********************************************************************************
 ********************************** Private Utilities ***************************
 ********************************************************************************
 */


static	void MapRect( Rect& baseRect, Rect& resizeRect, Rect* result )
{
	
	ODSShort ratio = (result->right - result->left) / (baseRect.right - baseRect.left);
	
	// Ration can't be 0
	if (ratio==0)
		ratio = 1;
	
	result->left += (resizeRect.left - baseRect.left) * ratio;
	result->right += (resizeRect.right - baseRect.right) * ratio;
	
	ratio = (result->bottom - result->top) / (baseRect.bottom - baseRect.top);
	
	// Ration can't be 0
	if (ratio==0)
		ratio = 1;
	
	result->top += (resizeRect.top - baseRect.top) * ratio;
	result->bottom += (resizeRect.bottom - baseRect.bottom) * ratio;
}

static	void	SortRect( Rect* rect )
{
	if (rect->left > rect->right)
	{
		ODSShort temp = rect->left;
		rect->left = rect->right;
		rect->right = temp;
	}
	if (rect->top > rect->bottom)
	{
		ODSShort temp = rect->top;
		rect->top = rect->bottom;
		rect->bottom = temp;
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
