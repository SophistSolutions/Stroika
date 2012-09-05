/*
	File:		FrameProxy.h

	Contains:	CEmbeddedFrameProxy class definition

	Written by:	Dave Stafford
	
	Copyright:	© 1995 by Apple Computer, Inc., all rights reserved.
	
	This file contains the definition for the class used to act as a place holder
	for an ODFrame. A mock frame will contain a frame ID if it is NOT in memory. 
	Otherwise, it will have a pointer to an ODFrame if it IS in memory.
*/

#ifndef _FRAMEPROXY_
#define _FRAMEPROXY_

// -- LedPart --

#include	<StdDefs.h>

#include "LedPartDef.h"


// -- OpenDoc --

#ifndef SOM_Module_OpenDoc_Global_Types_defined // For kODPresDefault
#include "ODTypesM.xh"
#endif

//=============================================================================
// Forward Declarations
//=============================================================================
class LedPart;
class Embedding;
class	CloneInfo;


//=============================================================================
// CFrameProxy
//
// Notions about the CFrameProxy class
// 
// Frames (embedded & display ) are lazily internalized through these objects. 
// Frame proxies store a frame ID and/or a ODFrame pointer. A frame proxy can be 
// constructed in 3 useful ways:
//
// 1. with a frame ID and a containing frame. This allows the frame to be 
// loaded at a later time. 
// 2. A frame proxy can be created with just a part reference and a containing
// frame. In this case the frame will not be acquired from the draft, but created
// anew. When a frame proxy is created in this manner it is assumed it has YET 
// to be embedded.
// 3. A frame proxy can be loaded in from storage. In most cases this works with
// no trouble. Sometimes (in 1 case, actually ), however, an EMBEDDED frame proxy can 
// be loaded with no containing frame available which designates it as an ORPHAN.
//
// Orphaned embedded frame proxies. An orphan frame proxy comes into being when a 
// document is saved and then dragged into a container part without cloning the root 
// frame. In this case the proxies will not have a containing frame and are considered
// orphans. There is special case code in Embedding to deal with this 
// situation.
//
//
// Also, in an attempt to be exception safe ( finally ) I have made the proxy
// objects require a 2 phase init. First you construct the object, then you
// MUST call one of the init methods for a particular derivative of CFrameProxy.
// Due to the facet that methods that need to be called to init
// frame proxies can fail, and since our exception handling mechanism does not
// support constructors failing, a 2 phase init is required.
//
// There are other instances in this part where constructors can fail, a similar
// solution is necessary in those instances as well.
//
//=============================================================================
class	ODStorageUnit;


class CFrameProxy
{
public:

	// -- Init --
	CFrameProxy();
	
	void InitializeFrameProxy( Environment* ev, 
								LedPart* editor, 
								ODID frameID, 
								ODFrame* frame );
	
	virtual ~CFrameProxy();
	
	// -- Accessors --
	ODBoolean			IsFrameInMemory() const;
	ODBoolean			IsFrameProxyInited() const;

	ODID				GetFrameID() const;
	
	// Get Frame tries to internalize the frame, if it isn't already
	virtual ODFrame*	GetFrame(Environment* ev);
	
	// SetFrame can be an alternative way to construct a proxy in which case
	// the frame needs to be acquired.
	void				SetFrame(ODFrame* frame);

	// Storage
	virtual ODBoolean	Read(Environment* ev, 
								ODStorageUnit* storage, 
								CloneInfo* cloneInfo) = 0;
	virtual void		Write(Environment* ev, 
								ODStorageUnit* storage, 
								CloneInfo* cloneInfo) = 0;

protected:

	// Back pointer to the part for necessary interaction
	LedPart*			fLedPart;
	ODID			 	fFrameID;
	ODFrame*		 	fFrame;	

private:

	// The frame we are acting as proxy for.
	ODBoolean			fWeAcquiredFrame;
	ODBoolean		 	fIsInited;	

};


//=============================================================================
// CDisplayFrameProxy
//
//=============================================================================
class CDisplayFrameProxy : public CFrameProxy
{
public:
	// -- Init --
	CDisplayFrameProxy();
	void InitializeDisplayFrameProxy( Environment* ev, 
										LedPart* editor, 
										ODID frameID = kODNULLID, 
										ODFrame* frame = kODNULL );
	
	virtual ~CDisplayFrameProxy();
	
	// Storage
	virtual ODBoolean	Read(Environment* ev, 
								ODStorageUnit* storage, 
								CloneInfo* cloneInfo);
	virtual void		Write(Environment* ev, 
								ODStorageUnit* storage, 
								CloneInfo* cloneInfo);

};


//=============================================================================
// CEmbeddedFrameProxy
//
//=============================================================================
class CEmbeddedFrameProxy : public CFrameProxy
{
public:
	// -- Init --
	CEmbeddedFrameProxy();
							
	void InitializeEmbeddedFrameProxy( Environment* ev, 
							LedPart* editor, 
							Embedding* shape);
							
	void InitializeEmbeddedFrameProxy( Environment* ev, 
						LedPart* editor, 
						Embedding* shape, 
						ODFrame* containingFrame, 
						ODPart* part,
						ODTypeToken viewType = kODNullTypeToken,
						ODTypeToken presentation = kODNullTypeToken);
						
	void InitializeEmbeddedFrameProxy( Environment* ev, 
						LedPart* editor, 
						Embedding* shape, 
						ODID frameID, 
						ODFrame* containingFrame );
	
	virtual ~CEmbeddedFrameProxy();
	
	// -- Accessors --
	
	virtual ODFrame*	GetFrame(Environment* ev);
	ODBoolean			IsOrphaned() const;
	
	ODID				GetContainingFrameID(Environment* ev) const;
	void				SetContainingFrame(ODID id);
	ODFrame*			AcquireContainingFrame(Environment* ev);
	
	// -- Embedded Frames --
	void				OffsetFrame(Environment* ev, ODPoint offset);		
	void				ResizeFrame(Environment* ev, ODRect resizeRect);
	void				RePositionFrameFacets(Environment* ev, ODPoint& topLeft);
			
	ODBoolean			IsAttached() const;
	void				Attach(Environment* ev);	
	void				Detach();	
		
	// Remove add frames from the document
	ODBoolean			IsInLimbo(Environment* ev) const;
	void				SetInLimbo(Environment* ev, ODBoolean isInLimbo);	
		
	void				Purge(Environment* ev);	
	void				CloseAndPurge();
	void				RemoveAndPurge();
	
	void				RenumberFrameSequences(Environment* ev, ODULong sequence, ODBoolean addOrRemove);
	ODULong				FindNextSequence(Environment* ev, ODULong group);
	
	// Storage
	ODBoolean	Read(Environment* ev, 
								ODStorageUnit* storage, 
								CloneInfo* cloneInfo);
	void		Write(Environment* ev, 
								ODStorageUnit* storage, 
								CloneInfo* cloneInfo);
	
	// -- Facets --
	void				CreateFacets(Environment* ev);
	void 				RemoveFacets(Environment* ev);
	
	void 				CreateFacetsForContainer(Environment* ev, ODFacet* facet);
	void				RemoveFacetsForContainer(Environment* ev, ODFacet* facet);

private:	
	
	// Back pointer to the embedding shape for necessary interaction
	Embedding*	fEmbeddingShape;
			
	// ID of container frame
	ODID				fContainingFrameID;
	
	// Frame sequenceing ( remove / undo / redo )
	ODULong				fSavedFrameSequence;
	
	// We store a reference to the ID of the part which owns the
	// frame for we which we are acting as proxy. This is necessary
	// to support lazy internalization and is used only in the case
	// where an embedded shape is created ( or a frame is created 
	// causing a new proxy to be added ). In this case we don't create
	// a frame and, consequently, do not have a frame ID either. In 
	// this case when the frame is called for we must create the embedded
	// frame ourselves via the CreateEmbeddedFrame call which takes the part
	// as a parameter.
	// ODID				fPartID;
	
	// OpenDoc Bug: Can't store the PartID, must store the actual part reference.
	// It appears that you can't create the part, then release it, then re-acquire
	// it later
	ODPart* 			fPart;
	
	// Also, in the case where we are constructed with a part reference, we must also
	// store a presentation and view type since we will be creating the frame 
	// in proxy code.
	ODTypeToken			fPresentation;
	ODTypeToken			fViewType;
	
	// Used primarily do determine how to release the 
	// ODFrame we are proxying
	ODBoolean			fAttached;
};



#endif