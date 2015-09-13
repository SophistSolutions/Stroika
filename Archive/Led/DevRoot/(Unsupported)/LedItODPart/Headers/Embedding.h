/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */
#ifndef __Embedding_h__
#define __Embedding_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Headers/Embedding.h,v 2.11 1996/12/13 18:09:57 lewis Exp $
 *
 * Description:
 *
 *
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Embedding.h,v $
 *	Revision 2.11  1996/12/13 18:09:57  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.10  1996/12/05  21:11:06  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/09/30  14:42:33  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/09/01  15:44:28  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/05/23  20:34:32  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/05/14  20:58:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1996/04/18  16:14:14  lewis
 *	react (in small way) to some led class lib changes to embeddings
 *
 *	Revision 2.4  1996/03/16  19:29:29  lewis
 *	react to new tabstop code, and add MeasureSegmentbaseLine()
 *	override to make sure embeddings line up along the baseline.
 *
 *	Revision 2.3  1996/03/05  18:46:06  lewis
 *	Renamed Led_ClipboardObjectAcquire::ClipFormat --> Led_ClipFormat.
 *
 *	Revision 2.2  1996/03/04  08:11:00  lewis
 *	Added GetTag/Write/GetWriteToClipAsFormat() methods as needed overrides
 *	of new methods in StyledTextImagerEmebdding...
 *
 *	Revision 2.1  1996/02/05  05:02:00  lewis
 *	Limbo/proxy changes based on DR4CD version of DrawShapes example
 *
 *	Revision 2.0  1996/01/04  00:24:47  lewis
 *	*** empty log message ***
 *
 *	Revision 1.4  1996/01/04  00:19:31  lewis
 *	Lose ExternalizationIndex stuff - not needed.
 *
 *	Revision 1.3  1995/12/15  03:56:46  lewis
 *	Use Led_Array of proxies instead of COrderedList.
 *	Lose some Environment args.
 *	Embedding::DrawSegment() now takes an invalRect arg, for logical clipping.
 *
 *	Revision 1.2  1995/12/13  06:06:56  lewis
 *	lots of chagnes. Mostly losing linking stuff, fixing some problems with
 *	embedding, and losting Environment* args.
 *
 *	Revision 1.1  1995/12/06  02:00:23  lewis
 *	Initial revision
 *
 *
 *
 *
 */
#include	<Types.h>

#include	<StdTypes.xh>
#include	<StdDefs.xh>
#include	<ODTypes.h>
#include	<Facet.xh>
#include	<Shape.xh>
#include	<SUView.xh>
#include	<Window.xh>
#include	<DragDrp.xh>
#include	<Frame.xh>

#include	<IText.h>

#include	"StyledTextEmbeddedObjects.h"

#include	"LedPartGlobals.h"
#include	"LedPartDef.h"
#include	"LedSupport.h"


class	CEmbeddedFrameProxy;
class	COrderedList;
class	CloneInfo;
class	ODStorageUnitView;
class	ODStorageUnit;
class	ODFacet;




const	Led_PrivateEmbeddingTag	kODEmbedTag	=	"ODEmbed";



class	LedPart;
class	Embedding : public SimpleEmbeddedObjectStyleMarker {
	public:
		Embedding (LedPart* ledPart);
		~Embedding();
	
		override	void			DrawSegment (const StyledTextImager* imager, Led_Tablet tablet,
											size_t from, size_t to, const Led_tChar* text, const Led_Rect& drawInto,
											Led_Coordinate useBaseLine, Led_Distance* pixelsDrawn
										);
		override	void			MeasureSegmentWidth (const StyledTextImager* imager, size_t from, size_t to,
												const Led_tChar* text,
												Led_Distance* distanceResults
											) const;
		override	Led_Distance	MeasureSegmentBaseLine (const StyledTextImager* imager, size_t from, size_t to) const;
		override	Led_Distance	MeasureSegmentHeight (const StyledTextImager* imager, size_t from, size_t to) const;
		override	void	DidUpdateText (const MarkerOwner::UpdateInfo& updateInfo);

	public:
		override	const char*		GetTag () const;
		override	void			Write (SinkStream_& sink);
		override	void			ExternalizeFlavors (TextInteractor_::WriterFlavorPackage& flavorPackage);



		nonvirtual void		GetBoundingBox(Rect* bounds) const;	
		nonvirtual void		GetBoundingBox(ODRect* bounds) const;
	
	

		nonvirtual void		GetUpdateShape(Environment* ev, ODShape* updateShape) const;
		nonvirtual void		GetUpdateBox(ODRect* updateBox) const;
			
	
		nonvirtual void		GetDragRect(Rect* dragRect) const;
		
		// ----- Drawing -----
		void				DrawHandles();
	
		// ----- Selection / Activation -----
		ODBoolean			ShapeInRectangle(const Rect& rect) const; // InSelectionRect
		
		ODBoolean			IsSelected() const
								{return fSelected;}
		
		// ----- Handles -----
		ODSShort			WhichHandle(const Point& mouse) const;
		void 				CalcHandle(short whichHandle, Rect* bounds) const;
		nonvirtual void		GetHandleCenter(short whichHandle, Point* center) const;
	
		// ----- Resize -----
		nonvirtual void		Resize(Environment *ev, Rect& baseRect, Rect& resizeRect);
		nonvirtual void		ResizeFeedback(ODFacet* facet, short whichHandle, Point& mouseLoc);
												
		nonvirtual void		GetResizeRect(ODSShort whichHandle, 
												Point& lastLocation, 
												Rect* srcRect, 
												Rect* dstRect);
		
		// ----- OpenDoc Notifications -----
		nonvirtual void 		Open(Environment *ev, ODFrame* container);
		
		nonvirtual void 		DisplayFrameConnected(Environment *ev, ODFrame* frame);
		nonvirtual void 		DisplayFrameAdded(Environment *ev, ODFrame* frame);
		nonvirtual void 		DisplayFrameRemoved(Environment *ev, ODFrame* frame);
		nonvirtual void 		DisplayFrameClosed(Environment *ev, ODFrame* frame);
		
		nonvirtual void 		Dragging(Environment *ev, ODBoolean dragging);
		nonvirtual void 		FacetAdded(Environment *ev, ODFacet* facet);
		nonvirtual void 		FacetRemoved(Environment *ev, ODFacet* facet);
		
	
	private:
		Rect			fRect;
		ODBoolean		fSelected;

	public:
		nonvirtual				ODShape*	CreateDragShape(Environment *ev, ODFacet* facet);
	
		// ----- Inherited Methods -----
		nonvirtual void 		DrawShape(Environment* ev, ODFacet* facet);
#if 0
		nonvirtual void		OffsetShape(	Environment *ev, 
											ODCoordinate xDelta, 
											ODCoordinate yDelta);
#endif
		
		nonvirtual	void	SetShapeRectangle (ODShape* bounds);
		// ----- Embedding Protocols
		nonvirtual	ODBoolean	ContainsProxyForFrame(Environment* ev, ODFrame* frame);	
		nonvirtual	void	SetInLimbo (ODBoolean isInLimbo);
		ODFacet*	 		GetEmbeddedFacet(ODFacet* containingFacet);
		ODFacet*	 		GetEmbeddedFacet(ODFrame* containingFrame);
		ODFrame*	 		GetAnyFrame(Environment* ev);
		
		void		 		AddFrameProxy(CEmbeddedFrameProxy* frameProxy);
		
		void		 		Embed(	Environment* ev, 
									ODPart* part, 
									ODFrame* containingFrame, 
									ODID embeddedFrame);
		
		
		// ----- Shape -----
		
		nonvirtual void		SetBoundingBox(Environment* ev, const Point& anchorPoint,  const Point& currentPoint);
		nonvirtual void		SetBoundingBox(Environment* ev, const Rect& bounds);
		nonvirtual void		SetBoundingBox(Environment* ev, ODShape* bounds);
		
		nonvirtual void		SetProxyBounds(Environment* ev, const Rect& bounds);
		
#if 0
		// ----- Shape -----
		nonvirtual void		Activate(Environment* ev, ODBoolean activating, ODFrame* frame);
#endif
		
		
		// ----- LedPart Notifications -----
		nonvirtual void 		Added();
		nonvirtual void 		Removed(ODBoolean commit);
		
		// ----- Storage -----
		nonvirtual void 		Read(Environment* ev, ODStorageUnitView* view, CloneInfo* cloneInfo);
		nonvirtual void 		Read(Environment* ev, ODStorageUnit* storage, CloneInfo* cloneInfo);
		nonvirtual void 		Write(Environment* ev, ODStorageUnit* storage, CloneInfo* cloneInfo);
		nonvirtual void 		Write(Environment* ev, ODStorageUnitView* view, CloneInfo* cloneInfo);
				
	private:
		LedPart*						fLedPart;
		Led_Array<CEmbeddedFrameProxy*>	fFrameProxies;
		ODRect							fFrameRect;
		
};




#endif	/*__Embedding_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
