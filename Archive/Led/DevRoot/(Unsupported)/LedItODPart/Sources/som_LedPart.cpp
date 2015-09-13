/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/som_LedPart.cpp,v 2.5 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: som_LedPart.cpp,v $
 *	Revision 2.5  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.4  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.2  1996/01/04  00:33:36  lewis
 *	Massive changes. No longer pass along Environemt* pointer. And now Led
 *	OD method callbacks prefixed with od_ for clarity as to which are
 *	really callbacks.
 *
 *	Revision 2.1  1995/12/13  06:29:07  lewis
 *	Massive cleanups. New try/catch support, so misc c++ exceptions caught
 *	and not propagated through SOM - causing crash.
 *	Move towards new convention of calling od_ methods in c++ SOM wrapper object
 *	and not passing Enviroemnt*. Instead we assert here ev is same
 *	as ::somGetGlobalEnvironment(), and then just grab that in our
 *	actual code as needed.
 *
 *	
 *
 *
 *
 */


// Notification that this is a SOM source file
#define SophistSolutions_som_LedPart_Class_Source


#include	<ODTypes.h>
#include	<Part.xh>

#include	<Except.h>
#include	<ODUtils.h>

#include	"LedPart.h"
#include	"som_FrameIterator.xh"

#include	"som_LedPart.xih"





/*
 *	It really obfuscates the code, and probably even slows it down slightly always passing
 *	along these silly environment objects. It really annoys me. And appears completely avoidable.
 *	This assertion just checks that it is safe to use ::somGetGlobalEnvironment () instead of
 *	passing along the global, in case this assumption ever changes.
 */
#define	Require_EnvironmentCanBeHadFromGlobal(ev)\
	Led_Require (ev == ::somGetGlobalEnvironment ())





inline	LedPart*	GetLedPart (SophistSolutions_som_LedPart* somSelf)
	{
		Led_AssertNotNil (somSelf);
		SophistSolutions_som_LedPartData*	somThis = SophistSolutions_som_LedPartGetData(somSelf);
		Led_AssertNotNil (somSelf);
		Led_AssertNotNil (somThis->fPart);
		Led_AssertMember (somThis->fPart, LedPart);
		return (somThis->fPart);
	}



#define	StartSOMUnwindProtect\
	try {


	// Should also catch things like bad_alloc, and map them to kODErrOutOfMemory etc...
#define	EndSOMUnwindProtect\
	}\
	catch (ODException _exception) {\
		ODSetSOMException(ev,_exception);\
	}\
	catch (...) {\
		ODSetSOMException(ev,kODErrNotImplemented);\
	}






/*
 ********************************************************************************
 *********************** SOM LedPart class wrappers *****************************
 ********************************************************************************
 */
SOM_Scope	void 	SOMLINK	som_LedPart__somInit (SophistSolutions_som_LedPart* somSelf)
{
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__somInit");

	SophistSolutions_som_LedPart_parent_ODPart_somInit (somSelf);
}

SOM_Scope	void 	SOMLINK	som_LedPart__somUninit (SophistSolutions_som_LedPart* somSelf)
{
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__somUninit");

	delete GetLedPart (somSelf);

	SophistSolutions_som_LedPart_parent_ODPart_somUninit(somSelf);
}

SOM_Scope	void 	SOMLINK	som_LedPart__InitPart (
			SophistSolutions_som_LedPart* somSelf, Environment* ev,
			ODStorageUnit* storageUnit, ODPart* partWrapper
	)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);

	// We must call the initialize method of our parent
	// class to allow OpenDoc to annotate our part's storageUnit, to 
	// set our refcount, and to change our "initialized" flag to true.
	SophistSolutions_som_LedPart_parent_ODPart_InitPart (somSelf, ev, storageUnit, partWrapper);

	Led_AssertNotNil (somSelf);
	SophistSolutions_som_LedPartData *somThis = SophistSolutions_som_LedPartGetData(somSelf);

	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__InitPart");

	Led_Assert (somThis->fPart == NULL);

	StartSOMUnwindProtect {
		somThis->fPart = new LedPart ();	// Create the "real" C++ LedPart
		GetLedPart (somSelf)->od_InitPart (storageUnit,partWrapper);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__InitPartFromStorage (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODStorageUnit* storageUnit, ODPart* partWrapper
						)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);

	// Call the initialize method of our parent PersistentObject class to generate
	// the appropriate time/date stamp on our ODPart's storage unit.
	SophistSolutions_som_LedPart_parent_ODPart_InitPartFromStorage(somSelf, ev, storageUnit, partWrapper);
	
	SophistSolutions_som_LedPartData *somThis = SophistSolutions_som_LedPartGetData(somSelf);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__InitPartFromStorage");

	Led_AssertNotNil (somSelf);
	Led_Assert (somThis->fPart == NULL);

	StartSOMUnwindProtect {
		// Create the "real" C++ LedPart.		
		somThis->fPart = new LedPart;
		
		GetLedPart (somSelf)->od_InitPartFromStorage (storageUnit,partWrapper);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODSize	SOMLINK	som_LedPart__Purge(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODSize size)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__Purge");

	ODSize tSize = 0;
	StartSOMUnwindProtect {
		tSize = GetLedPart (somSelf)->od_Purge (size);
	}
	EndSOMUnwindProtect;
	return tSize;
}

SOM_Scope	void 	SOMLINK	som_LedPart__Externalize (SophistSolutions_som_LedPart* somSelf, Environment* ev)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__Externalize");
	SophistSolutions_som_LedPart_parent_ODPart_Externalize(somSelf,ev);
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_Externalize ();
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__Release (SophistSolutions_som_LedPart* somSelf, Environment* ev)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);

	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__Release");

	SophistSolutions_som_LedPart_parent_ODPart_Release(somSelf,ev);
	
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_Release ();
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__ReleaseAll (SophistSolutions_som_LedPart* somSelf, Environment* ev)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ReleaseAll");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_ReleaseAll ();
	}
	EndSOMUnwindProtect;
	SophistSolutions_som_LedPart_parent_ODPart_ReleaseAll (somSelf,ev);
}

SOM_Scope	ODBoolean	SOMLINK	som_LedPart__HasExtension (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODType extensionName)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__HasExtension");
	ODBoolean result	=	SophistSolutions_som_LedPart_parent_ODPart_HasExtension (somSelf, ev, extensionName);
	if (not result) {
		StartSOMUnwindProtect {
			// check if we support this extention
			result = GetLedPart (somSelf)->od_HasExtension (extensionName);
		}
		EndSOMUnwindProtect;
	}
	return result;
}

SOM_Scope	void 	SOMLINK	som_LedPart__ReleaseExtension (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODExtension* extension)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ReleaseExtension");
	SophistSolutions_som_LedPart_parent_ODPart_ReleaseExtension(somSelf,ev,extension);
	StartSOMUnwindProtect {	
		GetLedPart (somSelf)->od_ReleaseExtension (extension);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODExtension*	SOMLINK	som_LedPart__AcquireExtension (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODType extensionName)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
    SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__AcquireExtension");
	ODExtension* result	=	SophistSolutions_som_LedPart_parent_ODPart_AcquireExtension (somSelf,ev,extensionName);
	if (result == NULL) {
		StartSOMUnwindProtect {
			result = GetLedPart (somSelf)->od_AcquireExtension (extensionName);
		}
		EndSOMUnwindProtect;
	}
	return result;
}

SOM_Scope	void 	SOMLINK	som_LedPart__FulfillPromise (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODStorageUnitView* promiseSUView)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__FulfillPromise");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_FulfillPromise (promiseSUView);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__DropCompleted(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODPart* part, ODDropResult dropResult)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DropCompleted");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_DropCompleted (part, dropResult);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODDragResult	SOMLINK	som_LedPart__DragEnter (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODDragItemIterator* dragInfo, ODFacet* facet, ODPoint* where
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DragEnter");
	ODDragResult tDragResult = false;
	try {
		tDragResult = GetLedPart (somSelf)->od_DragEnter (dragInfo, facet, where);
	}
	catch (ODException _exception) {
		ODSetSOMException (ev, _exception);
	}
	catch (...) {
		tDragResult = kODDropFail;
	}
	return tDragResult;
}

SOM_Scope	ODDragResult	SOMLINK	som_LedPart__DragWithin (
									SophistSolutions_som_LedPart* somSelf, Environment* ev,
									ODDragItemIterator* dragInfo, ODFacet* facet, ODPoint* where
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DragWithin");
	ODDragResult tDragResult = kODFalse;
	StartSOMUnwindProtect {
		tDragResult = GetLedPart (somSelf)->od_DragWithin (dragInfo, facet, where);
	}
	EndSOMUnwindProtect;
	return tDragResult;
}

SOM_Scope	void 	SOMLINK	som_LedPart__DragLeave(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* facet, ODPoint* where)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DragLeav");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_DragLeave (facet, where);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODDropResult	SOMLINK	som_LedPart__Drop (
									SophistSolutions_som_LedPart* somSelf, Environment* ev,
									ODDragItemIterator* dropInfo, ODFacet* facet, ODPoint* where
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__Drop");
	ODDropResult tDropResult = kODDropFail;
	StartSOMUnwindProtect {
		tDropResult = GetLedPart (somSelf)->od_Drop (dropInfo, facet, where);
	}
	EndSOMUnwindProtect;
	return tDropResult;
}

SOM_Scope	void 	SOMLINK	som_LedPart__ContainingPartPropertiesUpdated (
							SophistSolutions_som_LedPart* somSelf, Environment* ev,
							ODFrame* frame, ODStorageUnit* propertyUnit
					)
{
 	Require_EnvironmentCanBeHadFromGlobal (ev);
    SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ContainingPartPropertiesUpdated");
    SophistSolutions_som_LedPart_parent_ODPart_ContainingPartPropertiesUpdated (somSelf,ev,frame,propertyUnit);
}

SOM_Scope	ODStorageUnit*	SOMLINK	som_LedPart__AcquireContainingPartProperties (
										SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
    SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__AcquireContainingPartProperties");
    return (SophistSolutions_som_LedPart_parent_ODPart_AcquireContainingPartProperties(somSelf,ev,frame));
}

SOM_Scope	ODBoolean	SOMLINK	som_LedPart__RevealFrame (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODFrame* embeddedFrame, ODShape* revealShape
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__RevealFrame");
	StartSOMUnwindProtect {
		return GetLedPart (somSelf)->od_RevealFrame (embeddedFrame, revealShape);
	}
	EndSOMUnwindProtect;
	return kODFalse;
}

SOM_Scope	void 	SOMLINK	som_LedPart__EmbeddedFrameSpec (
									SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* embeddedFrame, ODObjectSpec* spec
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__EmbeddedFrameSpec");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_EmbeddedFrameSpec (embeddedFrame, spec);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODEmbeddedFramesIterator*	SOMLINK	som_LedPart__CreateEmbeddedFramesIterator (
													SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* /*frame*/
												)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__CreateEmbeddedFramesIterator");
	som_EmbeddedFramesIterator* tIter = kODNULL;
	SOM_TRY {
		tIter = new som_EmbeddedFramesIterator ();
		tIter->InitEmbeddedFramesIterator (ev, somSelf);
	}
	SOM_CATCH_ALL {
		ODDeleteObject (tIter);
	}
	SOM_ENDTRY
	return tIter;
}

SOM_Scope	void 	SOMLINK	som_LedPart__DisplayFrameAdded (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DisplayFrameAdded");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_DisplayFrameAdded (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__DisplayFrameRemoved(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DisplayFrameRemoved");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_DisplayFrameRemoved (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__DisplayFrameConnected(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DisplayFrameConnected");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_DisplayFrameConnected (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__DisplayFrameClosed(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DisplayFrameClosed");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_DisplayFrameClosed (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__AttachSourceFrame (
									SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame, ODFrame* sourceFrame
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__AttachSourceFrame");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_AttachSourceFrame (frame,sourceFrame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__FrameShapeChanged (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__FrameShapeChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_FrameShapeChanged (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__ViewTypeChanged (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ViewTypeChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_ViewTypeChanged (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__PresentationChanged (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__PresentationChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_PresentationChanged (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__SequenceChanged (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__SequenceChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_SequenceChanged (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__WritePartInfo (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODInfoType partInfo, ODStorageUnitView* storageUnitView
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__WritePartInfo");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_WritePartInfo (partInfo, storageUnitView);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODInfoType	SOMLINK	som_LedPart__ReadPartInfo (
									SophistSolutions_som_LedPart* somSelf, Environment* ev,
									ODFrame* frame, ODStorageUnitView* storageUnitView
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ReadPartInfo");
	ODInfoType tInfo = kODNULL;
	StartSOMUnwindProtect {
		tInfo = GetLedPart (somSelf)->od_ReadPartInfo (frame,storageUnitView);
	}
	EndSOMUnwindProtect;
	return tInfo;
}

SOM_Scope	ODID	SOMLINK	som_LedPart__Open (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__Open");
	ODID tID = kODNULLID;
	StartSOMUnwindProtect {
		tID = GetLedPart (somSelf)->od_Open (frame);
	}
	EndSOMUnwindProtect;
	return tID;
}

SOM_Scope	ODFrame*	SOMLINK	som_LedPart__RequestEmbeddedFrame (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODFrame* containingFrame, ODFrame* baseFrame, ODShape* frameShape,
								ODPart* embedPart, ODTypeToken viewType, ODTypeToken presentation, ODBoolean isOverlaid
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__RequestEmbeddedFrame");
	ODFrame* tFrame = kODNULL;
	StartSOMUnwindProtect {
		tFrame = GetLedPart (somSelf)->od_RequestEmbeddedFrame (containingFrame, baseFrame, frameShape, embedPart, viewType, presentation, isOverlaid);
	}
	EndSOMUnwindProtect;
	return tFrame;
}

SOM_Scope	void 	SOMLINK	som_LedPart__RemoveEmbeddedFrame (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* embeddedFrame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__RemoveEmbeddedFrame");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_RemoveEmbeddedFrame (embeddedFrame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODShape*	SOMLINK	som_LedPart__RequestFrameShape(SophistSolutions_som_LedPart* somSelf, Environment* ev,
		ODFrame* embeddedFrame,
		ODShape* frameShape)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__RequestFrameShape");
	ODShape* tShape = kODNULL;
	StartSOMUnwindProtect {
		tShape = GetLedPart (somSelf)->od_RequestFrameShape (embeddedFrame, frameShape);
	}
	EndSOMUnwindProtect;
	return tShape;
}

SOM_Scope	void 	SOMLINK	som_LedPart__UsedShapeChanged (
									SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* embeddedFrame
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__UsedShapeChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_UsedShapeChanged (embeddedFrame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODShape*	SOMLINK	som_LedPart__AdjustBorderShape (
									SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* embeddedFacet, ODShape* shape
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__AdjustBorderShape");
	ODShape* tShape = kODNULL;
	StartSOMUnwindProtect {
		tShape = GetLedPart (somSelf)->od_AdjustBorderShape (embeddedFacet, shape);
	}
	EndSOMUnwindProtect;
	return tShape;
}

SOM_Scope	void 	SOMLINK	som_LedPart__FacetAdded(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* facet)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__FacetAdded");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_FacetAdded (facet);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__FacetRemoved (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* facet)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__FacetRemoved");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_FacetRemoved (facet);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__CanvasChanged (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* facet)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__CanvasChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_CanvasChanged (facet);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__GeometryChanged (
								SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* facet,
								ODBoolean clipShapeChanged, ODBoolean externalTransformChanged
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__GeometryChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_GeometryChanged (facet, clipShapeChanged, externalTransformChanged);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__Draw (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* facet, ODShape* invalidShape)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__Draw");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_Draw (facet, invalidShape);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__CanvasUpdated (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODCanvas* canvas)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__CanvasUpdated");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_CanvasUpdated (canvas);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__HighlightChanged(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFacet* facet)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__HighlightChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_HighlightChanged (facet);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODULong	SOMLINK	som_LedPart__GetPrintResolution (SophistSolutions_som_LedPart* /*somSelf*/, Environment* ev, ODFrame* /*frame*/)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__GetPrintResolution");
	return 72;		//	72 is the default Quickdraw resolution and also OpenDoc's default canvas resolution.
}

SOM_Scope	ODLinkSource*	SOMLINK	som_LedPart__CreateLink(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODByteArray* data)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__CreateLink");
	ODLinkSource *linkSource = kODNULL;
	StartSOMUnwindProtect {
		linkSource = GetLedPart (somSelf)->od_CreateLink (data);
	}
	EndSOMUnwindProtect;
	return linkSource;
}

SOM_Scope	void	SOMLINK	som_LedPart__LinkUpdated (
							SophistSolutions_som_LedPart* somSelf, Environment* ev, ODLink* odLink, ODUpdateID change
						)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__LinkUpdated");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_LinkUpdated (odLink, change);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__RevealLink (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODLinkSource* linkSource)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__RevealLink");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_RevealLink (linkSource);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODBoolean 	SOMLINK	som_LedPart__EditInLinkAttempted(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__EditInLinkAttempted");
	ODBoolean result = kODFalse;
	StartSOMUnwindProtect {
		result = GetLedPart (somSelf)->od_EditInLinkAttempted (frame);
	}
	EndSOMUnwindProtect;
	return result;
}

SOM_Scope	void 	SOMLINK	som_LedPart__EmbeddedFrameUpdated (
								SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame, ODUpdateID change
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
    SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__EmbeddedFrameUpdated");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_EmbeddedFrameUpdated (frame, change);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__LinkStatusChanged (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__LinkStatusChanged");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_LinkStatusChanged (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODBoolean 	SOMLINK	som_LedPart__BeginRelinquishFocus (
									SophistSolutions_som_LedPart* somSelf, Environment* ev,
									ODTypeToken focus, ODFrame* ownerFrame, ODFrame* proposedFrame
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__BeginRelinquishFocus");
	ODBoolean tBoolean = kODFalse;
	StartSOMUnwindProtect {
		tBoolean = GetLedPart (somSelf)->od_BeginRelinquishFocus (focus, ownerFrame, proposedFrame);
	}
	EndSOMUnwindProtect;
	return tBoolean;
}

SOM_Scope	void 	SOMLINK	som_LedPart__CommitRelinquishFocus (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODTypeToken focus, ODFrame* ownerFrame, ODFrame* proposedFrame
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__CommitRelinquishFocus");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_CommitRelinquishFocus (focus, ownerFrame, proposedFrame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__AbortRelinquishFocus (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODTypeToken focus, ODFrame* ownerFrame, ODFrame* proposedFrame
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__AbortRelinquishFocus");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_AbortRelinquishFocus (focus, ownerFrame, proposedFrame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__FocusAcquired (
									SophistSolutions_som_LedPart* somSelf, Environment* ev, ODTypeToken focus, ODFrame* ownerFrame
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__FocusAcquired");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_FocusAcquired (focus, ownerFrame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__FocusLost (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODTypeToken focus, ODFrame* ownerFrame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__FocusLost");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_FocusLost (focus,ownerFrame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__CloneInto (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODDraftKey key, ODStorageUnit* toSU, ODFrame* scope
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__CloneInto");
	SophistSolutions_som_LedPart_parent_ODPart_CloneInto (somSelf, ev, key, toSU, scope);
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_CloneInto (key,toSU,scope);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__ExternalizeKinds(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODTypeList* kindset)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ExternalizeKinds");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_ExternalizeKinds (kindset);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__ChangeKind (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODType kind)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ChangeKind");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_ChangeKind (kind);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__ClonePartInfo (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODDraftKey key, ODInfoType partInfo, ODStorageUnitView* storageUnitView, ODFrame* scope
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
    SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ClonePartInfo");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_ClonePartInfo (key, partInfo, storageUnitView, scope);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	ODBoolean	SOMLINK	som_LedPart__HandleEvent (
									SophistSolutions_som_LedPart* somSelf, Environment* ev, ODEventData* event,
									ODFrame* frame, ODFacet* facet, ODEventInfo* eventInfo
								)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__HandleEvent");
	StartSOMUnwindProtect {
		return GetLedPart (somSelf)->od_HandleEvent (event, frame, facet, eventInfo);
	}
	EndSOMUnwindProtect;
	return false;
}

SOM_Scope	void 	SOMLINK	som_LedPart__AdjustMenus (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODFrame* frame)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__AdjustMenus");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_AdjustMenus (frame);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__UndoAction (SophistSolutions_som_LedPart* somSelf, Environment* ev, ODActionData* actionState)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__UndoAction");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_UndoAction (actionState);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__RedoAction(SophistSolutions_som_LedPart* somSelf, Environment* ev, ODActionData* actionState)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__RedoAction");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_RedoAction (actionState);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__DisposeActionState (
								SophistSolutions_som_LedPart* somSelf, Environment* ev,
								ODActionData* actionState, ODDoneState doneState
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__DisposeActionState");
	StartSOMUnwindProtect {
		GetLedPart (somSelf)->od_DisposeActionState (actionState, doneState);
	}
	EndSOMUnwindProtect;
}

SOM_Scope	void 	SOMLINK	som_LedPart__WriteActionState (
								SophistSolutions_som_LedPart* /*somSelf*/, Environment* ev,
								ODActionData* /*actionState*/, ODStorageUnitView* /*storageUnitView*/
							)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	// This is for persistent undo which we is not currently supported in OpenDoc 1.0.
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__WriteActionState");
}

SOM_Scope	ODActionData 	SOMLINK	som_LedPart__ReadActionState (SophistSolutions_som_LedPart* /*somSelf*/, Environment* ev, ODStorageUnitView* /*storageUnitView*/)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
	SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__ReadActionState");
	ODActionData data;
	data._maximum = 0;
	data._length = 0;
	data._buffer = kODNULL;
	return ((ODActionData)data);
}

SOM_Scope	COrderedList* 	SOMLINK	som_LedPart__GetEmbeddedFrames (SophistSolutions_som_LedPart* somSelf, Environment* ev)
{
	Require_EnvironmentCanBeHadFromGlobal (ev);
    SOMMethodDebug ("SophistSolutions_som_LedPart","som_LedPart__GetEmbeddedFrames");
	StartSOMUnwindProtect {
		return GetLedPart (somSelf)->GetEmbeddedFrames();
	}
	EndSOMUnwindProtect;
	return NULL;
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

