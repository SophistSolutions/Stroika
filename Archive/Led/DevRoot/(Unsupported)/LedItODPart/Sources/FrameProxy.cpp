/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/FrameProxy.cpp,v 2.8 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: FrameProxy.cpp,v $
 *	Revision 2.8  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.7  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.5  1996/02/05  05:07:42  lewis
 *	A number of changes to proxy frame stuff based on looking at DR4final DrawShapes diffs.
 *	And afew other clenaups.
 *
 *	Revision 2.4  1996/01/22  05:53:36  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1995/12/15  04:08:03  lewis
 *	In EmbeddedFrameProxy::Purge check for fFrame != NULL instead of
 *	(this->IsFrameInMemory()) and change (this->IsFrameInMemory call to
 *	also check for IsValid (frameID), based on advice from net from Jens - I think.
 *	Probably should rethink this. And maybe redo all this code. At least I must
 *	understnad it to fix my problems with embedding.
 *
 *	Revision 2.2  1995/12/13  06:16:14  lewis
 *	Cleanups, including the Environent* stuff, and using TmpIterators.
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *
 *
 *
 */


// so long as we use this, it must be included first!!!
#include "AltPoint.h"



#ifndef SOM_ODFrameFacetIterator_xh
#include <FrFaItr.xh>
#endif

#ifndef SOM_ODDraft_xh
#include <Draft.xh>
#endif

#ifndef SOM_ODTransform_xh
#include <Trnsform.xh>
#endif

#ifndef SOM_ODFrame_xh
#include <Frame.xh>
#endif

#ifndef SOM_ODFacet_xh
#include <Facet.xh>
#endif

#ifndef SOM_ODFacetIterator_xh
#include <FacetItr.xh>
#endif

// -- OpenDoc Utilities --

#ifndef _ODUTILS_
#include "ODUtils.h"
#endif

#ifndef _ODDEBUG_
#include "ODDebug.h"
#endif

#ifndef _STORUTIL_
#include <StorUtil.h>
#endif

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#include	"LedContent.h"


#include	"Embedding.h"
#include	"Except.h"
#include	"StorageU.xh"


// -- LedPart Includes --

#include	"LedPartGlobals.h"


#include "LedPart.h"

#include	"SampleCollections.h"


#include "FrameProxy.h"






inline	Environment*	SafeGetEnvironment ()
	{
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_AssertNotNil (ev);
		return ev;
	}




const ODBoolean kAddSequenceNumber		= true;
const ODBoolean kRemoveSequenceNumber	= false;




// Notes:
// For the embedded frame proxies, need some code to disambiguate frame 
// groups and sequences upon internalization of embedded frames. For example,
// when the 3rd frame in a sequence is cut/copied and pasted into another document
// which may or may not have frames in the ( pasted ) frames group/sequence.





/*
 ********************************************************************************
 ************************************ CFrameProxy *******************************
 ********************************************************************************
 */
CFrameProxy::CFrameProxy():
	fLedPart (NULL),
	fFrameID (kODNULLID),
	fFrame (NULL),
	fWeAcquiredFrame (false),
	fIsInited (false)
{
}

// Description:	Put the CFrameProxy into a usable state.
void CFrameProxy::InitializeFrameProxy( Environment* ev, 
									LedPart* editor, 
									ODID frameID, 
									ODFrame* frame )
{
	ASSERT(editor != NULL, kODErrInvalidParameter);
	
	fLedPart = editor;
	
	fFrame = kODNULL;
	
	// We MUST have either a frame ID or a frame pointer
	if (frame != kODNULL)
	{
		fFrameID = frame->GetID(ev);
		fFrame = frame;
	// $$$$$
	//	ODAcquireObject(ev,frame);
	}
	else
	{
		fFrameID = frameID;
	}

	fIsInited = kODTrue;
}

CFrameProxy::~CFrameProxy()
{
}

ODID CFrameProxy::GetFrameID() const
{
	return fFrameID;
}

void CFrameProxy::SetFrame(ODFrame* frame)
{
	Environment* ev = somGetGlobalEnvironment();
	
	fFrame = frame;
	if (fFrame)
	{
		ODAcquireObject(ev, fFrame);
		fFrameID = frame->GetID(ev);
	}
}

// Description:	GetFrame attempts to return a frame which is in a usable state.
// It does this by either returning the frame reference it has ( if non null ) 
// or, internalizing a frame from the given frame ID ( given in the constructor )
ODFrame* CFrameProxy::GetFrame(Environment* ev)
{
	if (fFrame)
		return fFrame;
		
	if (fFrameID == kODNULLID)
		return kODNULL;
	
	// We have an ID, acquire it from the draft
	ODDraft* draft = fLedPart->GetDraft ();
	fFrame = draft->AcquireFrame(ev, fFrameID);
	
	// Ensure the frame is not in limbo
	fFrame->SetInLimbo(ev, kODFalse);
	
	return fFrame;
}

ODBoolean CFrameProxy::IsFrameInMemory() const
{
#if 1
	return fFrame != NULL;
#else
	Led_AssertNotNil (fLedPart);
	ODDraft* draft = fLedPart->GetDraft ();
	Led_AssertNotNil (draft);
	return (fFrame != NULL) or (draft->IsValidID (SafeGetEnvironment (), fFrameID));
#endif
}







/*
 ********************************************************************************
 ******************************* CDisplayFrameProxy *****************************
 ********************************************************************************
 */
CDisplayFrameProxy::CDisplayFrameProxy()
{
}

// Description:	Put the display frame proxy into a usable state.
void CDisplayFrameProxy::InitializeDisplayFrameProxy( Environment* ev, 
													LedPart* editor, 
													ODID frameID, 
													ODFrame* frame )
{
	TRY
		// Sets base fields, and marks as initialized
		CFrameProxy::InitializeFrameProxy(ev, editor, frameID, frame);
	
		if (frame)
			ODAcquireObject(ev, frame);
	CATCH_ALL
	ENDTRY
	
}

CDisplayFrameProxy::~CDisplayFrameProxy()
{
	Environment* ev = somGetGlobalEnvironment();
	
	if (this->IsFrameInMemory())
	{
		ODSafeReleaseObject(fFrame);
	}
	
}

ODBoolean CDisplayFrameProxy::Read(Environment* ev, ODStorageUnit* storage, 
											CloneInfo* cloneInfo)
{
	TRY
		ODStorageUnitRef aSURef;
				
		StorageUnitGetValue(storage, ev, sizeof(ODStorageUnitRef), aSURef);
		
		if (!storage->IsValidStorageUnitRef(ev, aSURef))
			return kODFalse;
		
		ODStorageUnitID fromFrameID = storage->GetIDFromStorageUnitRef(ev, aSURef);
	
		if (cloneInfo != NULL)
		{
			ODDraft* tDraft = storage->GetDraft(ev);
			
			// ----- We are cloning -----
			fFrameID = cloneInfo->fFromDraft->Clone(ev, cloneInfo->fKey, fromFrameID, kODNULLID, kODNULLID);
		}
		else
		{
			// ----- We are just reading -----
			fFrameID = fromFrameID;
		}
	
	CATCH_ALL
		return kODFalse;
	ENDTRY
		
	return kODTrue;
}												

void CDisplayFrameProxy::Write(Environment* ev, ODStorageUnit* storage, CloneInfo* cloneInfo)
{
	ODStorageUnitRef aSURef;
	
	if (cloneInfo != NULL) {
		ODStorageUnitID frameID;
		ODID scopeFrameID = kODNULLID;
		
		// Scope frame may be null as in the case where clone into is passed a null frame.
		if (cloneInfo->fScopeFrame)
		{
			scopeFrameID = cloneInfo->fScopeFrame->GetID(ev);
		}
		
		frameID = cloneInfo->fFromDraft->WeakClone(ev, cloneInfo->fKey, 
								fFrameID, kODNULLID, scopeFrameID);
	 		 	
	 	storage->GetWeakStorageUnitRef(ev, frameID, aSURef);
	}
	else {
		// Its not a clone
	 	storage->GetWeakStorageUnitRef(ev, fFrameID, aSURef);
	}

	// ----- Write out the embedded frame reference -----
 	StorageUnitSetValue(storage, ev, sizeof(ODStorageUnitRef), aSURef);
}









/*
 ********************************************************************************
 **************************** CEmbeddedFrameProxy *******************************
 ********************************************************************************
 */

CEmbeddedFrameProxy::CEmbeddedFrameProxy()
{
	fEmbeddingShape = kODNULL;
	fContainingFrameID = kODNULLID;
	fSavedFrameSequence = 0L;
	fPart = kODNULL;
	fPresentation = kODNullTypeToken;
	fViewType = kODNullTypeToken;
	fAttached = kODFalse;
}

// Description:	Put the proxy into a usable state.
//
// Use this constructor when the part is known but there is no ODFrame or 
// ODFrameID
void	CEmbeddedFrameProxy::InitializeEmbeddedFrameProxy( Environment* ev, 
							LedPart* editor, 
							Embedding* shape, 
							ODFrame* containingFrame, 
							ODPart* part,
							ODTypeToken viewType,
							ODTypeToken presentation) 
{
	ASSERT(editor != NULL, kODErrInvalidParameter);
	ASSERT(shape != NULL, kODErrInvalidParameter);
	ASSERT(part != NULL, kODErrInvalidParameter);
	
	TRY
		fContainingFrameID = containingFrame->GetID(ev);
		
		// OD Bug see FrameProxy.h
		// fPartID = part->GetID(ev);
		ODAcquireObject(ev, part);
		fPart = part;
		
		fPresentation = presentation;
		fViewType = viewType;
		
		fEmbeddingShape = shape;
		
		// Sets base fields, and marks as initialized
		CFrameProxy::InitializeFrameProxy(ev, editor, kODNULLID, kODNULL);
		
	CATCH_ALL
	ENDTRY
}

// Description: Put the proxy into a usable state.
//
// Use this constructor for reading in a FrameProxy
void CEmbeddedFrameProxy::InitializeEmbeddedFrameProxy( Environment* ev, 
							LedPart* editor, 
							Embedding* shape)
{
	ASSERT(editor != NULL, kODErrInvalidParameter);
	ASSERT(shape != NULL, kODErrInvalidParameter);
	
	TRY
		fContainingFrameID = kODNULLID;
		
		// OD Bug see FrameProxy.h
		// fPartID = kODNULLID;
		fPart = kODNULL;
		
		fEmbeddingShape = shape;
		
		// Sets base fields, and marks as initialized
		CFrameProxy::InitializeFrameProxy(ev, editor, kODNULLID, kODNULL);
	CATCH_ALL
	ENDTRY
}

// Description:	Put the proxy into a usable state.	
void CEmbeddedFrameProxy::InitializeEmbeddedFrameProxy(	Environment* ev, 
							LedPart* editor, 
							Embedding* shape, 
							ODID frameID, 
							ODFrame* containingFrame ) 
{
	ASSERT(editor != NULL, kODErrInvalidParameter);
	ASSERT(shape != NULL, kODErrInvalidParameter);
	ASSERT(containingFrame != NULL, kODErrInvalidParameter);
	ASSERT(frameID != kODNULLID, kODErrInvalidParameter);

	TRY
		fContainingFrameID = containingFrame->GetID(ev);
	
		fEmbeddingShape = shape;
		
		// Sets base fields, and marks as initialized
		CFrameProxy::InitializeFrameProxy(ev, editor, frameID, kODNULL);
	CATCH_ALL	
	ENDTRY
}

CEmbeddedFrameProxy::~CEmbeddedFrameProxy()
{
	// The following code should only get execute in the case that a frame
	// was removed from the document temporarily and the remove was never 
	// commited.
	Environment* ev = somGetGlobalEnvironment();
	
	if (this->IsFrameInMemory())
	{
		// Get frame without attaching it
		ODFrame* tFrame = CFrameProxy::GetFrame(ev);
		tFrame->Remove(ev);
		
		// The frame is no longer in memory, 
		// protect the base class from release too many times
		this->SetFrame(kODNULL);
	}
	
	ODSafeReleaseObject(fPart);
}

ODBoolean CEmbeddedFrameProxy::IsAttached() const
{
	return fAttached;
}

ODBoolean CEmbeddedFrameProxy::IsOrphaned() const
{
	return fContainingFrameID==kODNULLID;
}

// Description:	Set the containing frame ID for this frame proxy. Do not call
// this if this proxy is already attached.
void CEmbeddedFrameProxy::SetContainingFrame(ODID id)
{
	ASSERT(!IsAttached(), kODErrInvalidParameter);

	fContainingFrameID = id;
}
		
ODID CEmbeddedFrameProxy::GetContainingFrameID(Environment* /*ev*/) const
{
	return fContainingFrameID;
}

// Description:	GetFrame attempts to return a frame which is in a usable state.
// If only an ID exists in the proxy, it will try to acquire the frame. If there 
// isn't an ID, it will create an embedded frame ( if it has a part reference ).
// If the frame already exists, but is unattached, it will attach it. This could 
// cause problems if you call GetFrame after you have detached the proxy and are 
// planning to permanently remove it, for example.
ODFrame* CEmbeddedFrameProxy::GetFrame(Environment* ev)
{
	ODFrame* tFrame;
	
	TRY
	if (this->IsFrameInMemory())
	{
		// We have a valid frame reference, get it into our temp
		tFrame = CFrameProxy::GetFrame(ev);
		
		// but it may not be attached
		// ( Because it was removed temporarily like from a cut or undone paste )
		// If not, then make it so
		if (!this->IsAttached())
		{
			this->Attach(ev);
		}
	}
	else
	{
		tFrame = CFrameProxy::GetFrame(ev);
		// InternalizingFrame $$$$$ DCS
		if (tFrame != kODNULL)
		{
			
			// Add frame to list of embedded frames
			fLedPart->GetEmbeddedFrames()->AddLast(tFrame);
			
			// If being internalized by ID we need to be attached
			this->Attach(ev);
		}
		else
		if (this->GetFrameID()==kODNULLID)
		{
			// We don't have a frame ID, embed a new frame
			// Create an embedded frame
			ODFrame* containingFrame = this->AcquireContainingFrame(ev);
			
			// Make sure we have a valid part ref
			THROW_IF_NULL(fPart);
				
			// Frame shapes are zero based, make the region zero based,
			// so make a zero based region based on the shapes bounds.
			ODRgnHandle tempShapeRegion = ODNewRgn ();
			THROW_IF_NULL(tempShapeRegion);
			Rect tRect;
			fEmbeddingShape->GetBoundingBox(&tRect);
			SetRectRgn(tempShapeRegion, 0, 0, tRect.right - tRect.left, 
												tRect.bottom - tRect.top);
			
			ODTypeToken presentation = fPresentation ? fPresentation : gGlobals->fUndefinedPresentation;
			ODTypeToken viewType = fViewType ? fViewType : gGlobals->fFrameView;
			
			ODShape* newShape = containingFrame->CreateShape(ev);
			THROW_IF_NULL(newShape);
			newShape->SetQDRegion(ev, tempShapeRegion);
			
			tFrame = fLedPart->GetDraft ()->CreateFrame(ev, 
													kODFrameObject, 
													containingFrame, 
													newShape, 
													kODNULL, // BiasCanvas (ODCanvas*)
													fPart, 
													viewType, 
													presentation,
													kODFalse, // IsSubframe is FALSE
													kODFalse); //isOverlaid
			
			// Save the internalized frame reference immediately
			// Otherwise attaching later will cause stack overflow.
			fFrame = tFrame;
			
			// We assume that if we are here that this frame is not part of any group
			// and is the first in its sequence
			tFrame->SetFrameGroup(ev, 0L);
			tFrame->ChangeSequenceNumber(ev, 0L);
			
			// Assign our local frame ID field as well
			fFrameID = tFrame->GetID(ev);
			
			// Atach this frame, Create facets for it
			this->Attach(ev);	
			
			// Release acquired containing frame, shape, part
			ODReleaseObject(ev, newShape);
			ODReleaseObject(ev, containingFrame);
			
			// Add frame to list of embedded frames
			fLedPart->GetEmbeddedFrames()->AddLast(tFrame);
		}
		else
		{
			// We have a frame ID, yet we could not internalize the frame
			// Something must be seriously wrong
			THROW_IF_NULL(tFrame);
		}
		
	}
	CATCH_ALL
		Led_Assert (false);	//	Trouble getting the frame!
	ENDTRY

	return tFrame;
}

// Description:	You MUST release the frame that is returned by this method.
ODFrame* CEmbeddedFrameProxy::AcquireContainingFrame(Environment* ev)
{			
			
	// If we haven't filled in the containing frame field, do it now
	if (fContainingFrameID==kODNULLID)
	{
		ODFrame* frame = this->GetFrame(ev);
		if (frame)
		{
			ODFrame* tFrame = frame->AcquireContainingFrame(ev);
			fContainingFrameID = tFrame->GetID(ev);
			
			// Return tFrame as containing frame
			return tFrame;
		}
		else {
			Led_Assert (false);	//	Calling  AcquireContainingFrame too early!
		}
	}
	
	ODDraft* draft =  fLedPart->GetDraft ();
	ODFrame* containingFrame = draft->AcquireFrame(ev, fContainingFrameID);
	THROW_IF_NULL(containingFrame);
	
	return containingFrame;
}

ODBoolean CEmbeddedFrameProxy::Read(Environment* ev, ODStorageUnit* storage, 
												CloneInfo* cloneInfo)
{
	TRY
		ODStorageUnitRef aSURef;
				
		StorageUnitGetValue(storage, ev, sizeof(ODStorageUnitRef), aSURef);
		
		if (!storage->IsValidStorageUnitRef(ev, aSURef))
			return kODFalse;
		
		ODStorageUnitID fromFrameID = storage->GetIDFromStorageUnitRef(ev, aSURef);
	
		if (cloneInfo != NULL)
		{
			ODDraft* tDraft = storage->GetDraft(ev);
			
			// ----- We are cloning -----
			fFrameID = cloneInfo->fFromDraft->Clone(ev, cloneInfo->fKey, fromFrameID, kODNULLID, kODNULLID);
			fContainingFrameID = cloneInfo->fScopeFrame->GetID(ev);
		}
		else
		{
			// ----- We are just reading -----
			fFrameID = fromFrameID;
		}
	
	CATCH_ALL
		return kODFalse;
	ENDTRY
		
	return kODTrue;
}												

void CEmbeddedFrameProxy::Write(Environment* ev, ODStorageUnit* storage, 
												CloneInfo* cloneInfo)
{
	ODStorageUnitRef aSURef;
	
	if (cloneInfo != NULL)
	{
		// Assert that we have a destination frame and that the source container
		// isn't the destination container
		ASSERT(cloneInfo->fScopeFrame == NULL || cloneInfo->fScopeFrame->GetID(ev) == 
						fContainingFrameID, kODErrInvalidParameter);

		ODStorageUnitID frameID;
		frameID = cloneInfo->fFromDraft->Clone(ev, cloneInfo->fKey, fFrameID, 0, fFrameID);
	 		 	
	 	storage->GetStrongStorageUnitRef(ev, frameID, aSURef);
	}
	else
	{
		// Its not a clone
	 	storage->GetStrongStorageUnitRef(ev, fFrameID, aSURef);
	}
	
	// ----- Write out the embedded frame reference -----
 	StorageUnitSetValue(storage, ev, sizeof(ODStorageUnitRef), aSURef);
}

// Description:	This method simply nulls the fFrame field after removing the frame
// from the Editor's list. The frame itself should have already been completely
// released before this method is called.
void 	CEmbeddedFrameProxy::Purge(Environment* ev)
{
	if (this->IsFrameInMemory()) {
		// Remove from list of embedded frames
		ODFrame* tFrame = CFrameProxy::GetFrame(ev);
		fLedPart->GetEmbeddedFrames()->Remove(tFrame);
		
		// Eliminate our reference
		this->SetFrame(kODNULL);
	}
}

// Description:	This method calls frame::close on fFrame, then calls purge.
void	CEmbeddedFrameProxy::CloseAndPurge()
{
	if (this->IsFrameInMemory())
	{
		Environment*	ev	=	::SafeGetEnvironment ();

		ODFrame* tFrame = CFrameProxy::GetFrame(ev);
				
		#ifdef ODDebug
			// Useful debugging information
		
			// The IsInLimbo call will throw if the frame has already
			// been removed. We should assume false in this case.
			ODBoolean isInLimbo = kODFalse;
			TRY
				isInLimbo = tFrame->IsInLimbo(ev);
			CATCH_ALL
				isInLimbo = kODFalse;
			ENDTRY
		
			ODULong tRef = fFrame->GetRefCount(ev);
		#endif
		
		tFrame->Close(ev);
			
		this->Purge(ev);
	}
}

// Description:	This method calls frame::remove on fFrame, then calls purge.
void CEmbeddedFrameProxy::RemoveAndPurge()
{
	if (this->IsFrameInMemory())
	{
		Environment*	ev	=	::SafeGetEnvironment ();
		ODFrame* tFrame = CFrameProxy::GetFrame(ev);
		
		// The IsInLimbo call will throw if the frame has already
		// been removed. We should assume false in this case.
		ODBoolean isInLimbo = kODFalse;
		TRY
			isInLimbo = tFrame->IsInLimbo(ev);
		CATCH_ALL
			isInLimbo = kODFalse;
		ENDTRY
		
		#ifdef ODDebug
			ODULong tRef = fFrame->GetRefCount(ev);
		#endif
	
		if (isInLimbo)
		{
			tFrame->Remove(ev);
		}
		else
			tFrame->Release(ev);
			
		this->Purge(ev);
	}
}

ODULong CEmbeddedFrameProxy::FindNextSequence(Environment* ev, ODULong group)
{
	ODULong sequenceCount = 0L;
	
	// Iterate over sibling embedded frames to find the current number of 
	// frames in the base frame's group
	COrdListIterator embeddedFrames(fLedPart->GetEmbeddedFrames());
	
	for (ODFrame* embeddedFrame = (ODFrame*)embeddedFrames.First(); 
			embeddedFrames.IsNotComplete();
			embeddedFrame = (ODFrame*)embeddedFrames.Next())
	{
		if (group == embeddedFrame->GetFrameGroup(ev))
		{
			// Don't assume the frame hasn't been created yet,
			// only tally those frames with sequences assigned.
			if (embeddedFrame->GetSequenceNumber(ev))
				sequenceCount++;
		}
	}
	return sequenceCount;
}

// Description:	This method will resequence all frames starting with the given 
// sequence until the end of the sequence. In the case of ADD this method should 
// be called BEFORE the new frame is given a sequence number. Likewise, in the 
// case of REMOVE, the frame should have its sequence set to zero BEFORE calling 
// renumber. Pass true to ADD a sequence or FALSE to remove a sequence. 
void CEmbeddedFrameProxy::RenumberFrameSequences(Environment* ev, ODULong sequence, ODBoolean addOrRemove)
{
	// Don't assume the frames are sequenced in the order of the OrderedList
	// Find all frames in the specified range
	COrderedList tCollection;
	ODFrame* embeddedFrame = kODNULL;
	
	// Iterate over the embedded frames
	COrdListIterator embeddedFrames(fLedPart->GetEmbeddedFrames());
	for (embeddedFrame = (ODFrame*)embeddedFrames.First(); 
			embeddedFrames.IsNotComplete();
			embeddedFrame = (ODFrame*)embeddedFrames.Next())
	{
		if (embeddedFrame->GetSequenceNumber(ev)>=sequence)
			tCollection.AddLast(embeddedFrame);
	}
	
	// Renumber
	COrdListIterator savedFrames(&tCollection);
	for (embeddedFrame = (ODFrame*)savedFrames.First(); 
			savedFrames.IsNotComplete();
			embeddedFrame = (ODFrame*)savedFrames.Next())
	{
		ODULong tSequence = embeddedFrame->GetSequenceNumber(ev);
		
		if (addOrRemove)
		{
			embeddedFrame->ChangeSequenceNumber(ev, tSequence+1);
		}
		else
		{
			embeddedFrame->ChangeSequenceNumber(ev, tSequence-1);
		}	
	}
}

ODBoolean CEmbeddedFrameProxy::IsInLimbo(Environment* ev) const
{
	if (this->IsFrameInMemory())
	{
		ODBoolean limbo = kODFalse;
		TRY
		limbo = fFrame->IsInLimbo(ev);
		CATCH_ALL
		ENDTRY
		return limbo;		
	}
	else
		// Assume that the frame is still in the document even though
		// it has not been internalized yet. 
		return kODFalse;
}

void CEmbeddedFrameProxy::SetInLimbo(Environment* ev, ODBoolean isInLimbo)
{
	if (this->IsFrameInMemory())
	{
		#ifdef ODDebug
			ODULong tRef = fFrame->GetRefCount(ev);
		#endif
	
		fFrame->SetInLimbo(ev, isInLimbo);
	}
}

void CEmbeddedFrameProxy::Attach(Environment* ev)
{
	// Do not allow attaching more than once
	// as this will cause too manyfacets to be added
	// and subsequent update problems which are hard to track
	if (fAttached)
	{
		Led_Assert (false);	//	Already attached!!
	}
	
	// ----- Assume we will succeed -----
	fAttached = TRUE;
	
	ODFrame* tContainingFrame = kODNULL;
	ODFrame* containingFrame = this->AcquireContainingFrame(ev);
	ODFrame* frame = kODNULL;
	THROW_IF_NULL(containingFrame);

	TRY	
		// This causes the frame to be loaded, if it isn't
		frame = GetFrame(ev);
		THROW_IF_NULL(frame);

#if 0
		// ----- Ensure that the frame is not in limbo -----
		frame->SetInLimbo(ev, kODFalse);
#endif
		// ----- Set the containing frame -----
		ODFrame* tContainingFrame = frame->AcquireContainingFrame(ev);
		if (tContainingFrame != containingFrame) {
			frame->SetContainingFrame (ev, containingFrame);
		}
		
		// Release acquired frame, though it is more likely that it will
		// be null since we will normally attach an unattached frame
		// so there will be no containing frame
		ODReleaseObject(ev, tContainingFrame);
	
	CATCH_ALL
		// Release acquired frame
		ODSafeReleaseObject(tContainingFrame);
		
		// Failed
		fAttached = kODFalse;
	ENDTRY
	
	// If the frame is in memory, and has a frame group and the
	// sequence is zero, then give it a valid sequence number ( iterate over frames )
	if (frame)
	{
		ODULong tGroup = frame->GetFrameGroup(ev);
		// If this frame is a part of a group
		if (tGroup!=0L)
		{
			// If this frame WAS a part of a sequence
			// Put it back there and renumber
			if (fSavedFrameSequence!=0L)
			{
				this->RenumberFrameSequences(ev, fSavedFrameSequence, kAddSequenceNumber);
			}
			else
			// Otherwise, Find new sequence number
			{
				ODULong tSequence = this->FindNextSequence(ev, tGroup);
				
				// Shouldn't need to renumber here, since we are appending
			}
		}
	}
	
	// Create facets for this frame, must wait to do this until after
	// frame is attached to containing frame, or OpenDoc will crash.
	this->CreateFacets(ev);	

	// Release acquired frame
	ODReleaseObject(ev, containingFrame);
}

void	CEmbeddedFrameProxy::Detach()
{
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	// Throw if we are not attached
	ASSERT(fAttached, kODErrInvalidParameter);

	ODFrame* containingFrame = AcquireContainingFrame(ev);
	THROW_IF_NULL(containingFrame);
	
	ODFrame* tContainingFrame = kODNULL;
	ODFrame* frame = kODNULL;
	
	// Delete our facets, muist do this before frame is removed from
	// its container, or OpenDoc will crash
	this->RemoveFacets(ev);
	
	TRY	
		
		// This causes the frame to be loaded, if it isn't
		frame = this->GetFrame(ev);
		THROW_IF_NULL(frame);

#if 0
		// ----- The frame is now in limbo -----
		frame->SetInLimbo(ev, kODTrue);
#endif
		// ----- Set the containing frame to null -----
		tContainingFrame = frame->AcquireContainingFrame(ev);
		
		if (tContainingFrame == containingFrame) {
			frame->SetContainingFrame (ev, NULL);
		}
			
		// Release acquired frame
		ODReleaseObject(ev, tContainingFrame);
	
	CATCH_ALL
		// Release acquired frame
		ODSafeReleaseObject(tContainingFrame);
		
	ENDTRY
	
	// If the frame is in memory, and has a frame group 
	// make the sequence zero, and renumber
	if (frame)
	{
		// If this frame is a part of a group
		if (frame->GetFrameGroup(ev)!=0L)
		{
			ODULong tSequence = frame->GetSequenceNumber(ev);
			frame->ChangeSequenceNumber(ev, 0L);
			this->RenumberFrameSequences(ev, tSequence, kRemoveSequenceNumber);
		}
	}
	
	// Release acquired frame
	ODReleaseObject(ev, containingFrame);
	
	// ----- It is now detached -----
	fAttached = FALSE;
}

void CEmbeddedFrameProxy::CreateFacetsForContainer(Environment* ev, ODFacet* facet)
{
	ODRect frameBounds;
	fEmbeddingShape->GetBoundingBox(&frameBounds);
	ODFrame* tFrame = this->GetFrame(ev);
	
	ODTransform* tTransform = kODNULL;
	
	TRY
		tTransform = tFrame->CreateTransform(ev);
		tTransform->SetOffset(ev, &frameBounds.TopLeft());
		
		ODFacet* embeddedFacet = facet->CreateEmbeddedFacet(ev,
															tFrame,
															kODNULL, 
															tTransform,
															kODNULL,
															kODNULL,
															kODNULL,
															kODFrameInFront);
		
		embeddedFacet->SetSelected(ev, fEmbeddingShape->IsSelected());
	CATCH_ALL
		ODSafeReleaseObject(tTransform);
	ENDTRY
	
	ODReleaseObject(ev, tTransform);
	
}

void CEmbeddedFrameProxy::CreateFacets(Environment* ev)
{
	ODFrame* containingFrame = AcquireContainingFrame(ev);
	THROW_IF_NULL(containingFrame);
	
	ODFrameFacetIterator* iter = kODNULL;
	
	TRY
		
		ODFrameFacetIterator* iter = containingFrame->CreateFacetIterator(ev);
		for (ODFacet* embeddingFacet = iter->First(ev); iter->IsNotComplete(ev); embeddingFacet = iter->Next(ev))
		{
			this->CreateFacetsForContainer(ev, embeddingFacet);
		}
		delete iter;
		
	CATCH_ALL
		delete iter;
			
		Led_Assert (false);		//	Exception thrown creating facets!
	ENDTRY
	
	// Release acquired frame
	ODReleaseObject(ev, containingFrame);
}

void CEmbeddedFrameProxy::RemoveFacetsForContainer(Environment* ev, ODFacet* facet)
{
	COrderedList tCollection;
	
	ODFacetIterator* iter = kODNULL;
	
	TRY
		// Find all the embedded facets of the frame, then store them in a 
		// temp collection For later removal
		ODFacetIterator* iter = facet->CreateFacetIterator(ev, kODChildrenOnly, kODFrontToBack);
		for (ODFacet* facet = iter->First(ev); iter->IsNotComplete(ev); facet = iter->Next(ev))
		{
			if (facet->GetFrame(ev) == this->GetFrame(ev))
				tCollection.AddLast(facet);
		}
		
		// Now, remove the facets.
		COrdListIterator ite2(&tCollection);
		for (facet = (ODFacet*)ite2.First(); ite2.IsNotComplete(); facet = (ODFacet*)ite2.Next())
		{
			facet->GetContainingFacet(ev)->RemoveFacet(ev, facet);
			delete facet;
		}
		delete iter;
	
	CATCH_ALL
		delete iter;
		Led_Assert (false);		//	Exception thrown removing facets!
	ENDTRY
}

void CEmbeddedFrameProxy::RemoveFacets(Environment* ev)
{
	// If the frame isn't loaded then there are no facets
	if (!IsFrameInMemory())
		return;
		
	ODFrame* containingFrame = AcquireContainingFrame(ev);
	THROW_IF_NULL(containingFrame);
		
	ODFrameFacetIterator* iter = kODNULL;	
	TRY
		ODFrameFacetIterator* iter = containingFrame->CreateFacetIterator(ev);
		for (ODFacet* embeddingFacet = iter->First(ev); iter->IsNotComplete(ev); embeddingFacet = iter->Next(ev))
		{
			this->RemoveFacetsForContainer(ev, embeddingFacet);
		}
		
		delete iter;
		
	CATCH_ALL
		delete iter;
		Led_Assert (false);		//	Exception thrown removing facets!"
	ENDTRY
		
	// Release acquired frame
	ODReleaseObject(ev, containingFrame);
	
}

void CEmbeddedFrameProxy::OffsetFrame(Environment* ev, ODPoint offset)
{
	ODFrame* frame = GetFrame(ev);
	THROW_IF_NULL(frame);
		
	// Should call RepositionFacets here instead of below code
	
	if (offset.x != 0 || offset.y!=0)
	{
		ODFrameFacetIterator* iter = frame->CreateFacetIterator(ev);
		for (ODFacet* odFacet = iter->First(ev); iter->IsNotComplete(ev); odFacet = iter->Next(ev))
		{
			ODTransform* tTransform  = ODCopyAndRelease(ev, odFacet->AcquireExternalTransform(ev, kODNULL));			
					
			tTransform->MoveBy(ev, &offset);
			odFacet->ChangeGeometry(ev, kODNULL, tTransform, kODNULL);
			
			// Release acquired geometry
			ODReleaseObject(ev, tTransform);
		}
		delete iter;
	}
}

void CEmbeddedFrameProxy::ResizeFrame(Environment* ev, ODRect resizeRect)
{
	// If there is no frame to resize, then return
	if (!this->IsFrameInMemory())
		return;
		
	// Setup resize geometry
	ODFrame*	frame = GetFrame(ev);
	ODShape*	newShape = frame->CreateShape(ev);
	
	newShape->SetRectangle(ev, &resizeRect);
	
	// Make the shape zero based since ODFrames are.
	ODPoint offset(-resizeRect.left, -resizeRect.top);
	ODTransform* tTransform = frame->CreateTransform(ev);
	tTransform->MoveBy(ev, &offset);
	
	// The new shape is ready
	newShape->Transform(ev, tTransform);
	
	// Reposition the facets of the frame we just resized
	ODPoint topLeft (resizeRect.left, resizeRect.top);
	RePositionFrameFacets (ev, topLeft);
	
	frame->ChangeFrameShape(ev, newShape, NULL);

	// Release acquired geometry
	ODReleaseObject(ev, newShape);
	ODReleaseObject(ev, tTransform);
}

void CEmbeddedFrameProxy::RePositionFrameFacets(Environment* ev, ODPoint& topLeft)
{
	ODFrame* frame = this->GetFrame(ev);
	
	ODFrameFacetIterator* iter = frame->CreateFacetIterator(ev);			
	for (ODFacet* odFacet = iter->First(ev); iter->IsNotComplete(ev); odFacet = iter->Next(ev))
	{
		ODTransform* tTransform = odFacet->AcquireExternalTransform(ev, NULL);			
		
		tTransform->Reset(ev);
		tTransform->MoveBy(ev, &topLeft);
		
		odFacet->ChangeGeometry(ev, NULL, tTransform, NULL);
		
		// Release acquired geometry
		ODReleaseObject(ev, tTransform);
	}
	delete iter;
}





