/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1995.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/LedPartCommands.cpp,v 2.4 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: LedPartCommands.cpp,v $
 *	Revision 2.4  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.3  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/02/05  05:12:53  lewis
 *	Try to support CDropAsCommand.
 *	a few minor changes from looking at diffs between 1.0.1 and 1.0 versions
 *	of DrawShapes sample (DR4 prerelease and final).
 *
 *	Revision 2.0  1996/01/22  05:54:47  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1996/01/04  00:32:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1995/12/15  04:17:15  lewis
 *	A number of minor changes, including commenting out unused startDrag
 *	command.
 *
 *	Revision 2.3  1995/12/13  06:27:09  lewis
 *	Massive diffs. Who knows!
 *	Removed old publish / subscribe support. Lost old CSelection class.
 *	much more...
 *
 *	Revision 2.2  1995/12/08  07:28:54  lewis
 *	Misc cleanups
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *
<< FROM Command.cpp - merged in 960114>>
 *	Revision 2.3  1995/12/15  04:04:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1995/12/13  06:14:39  lewis
 *	Cleanups - mostly losing Environment* arg.
 *
 *	Revision 2.1  1995/12/06  02:03:11  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
<< END Command.cpp - merged in 960114>>
 *
 *
 *
 */






// so long as we use this, it must be included first!!!
#include "AltPoint.h"


// so long as we use this, it must be included first!!!
#include "AltPoint.h"




#include	"LedPartGlobals.h"

#include "LedPart.h"

// -- OpenDoc Includes --



// for ODName
#ifndef _ODTYPES_
#include <ODTypes.h>
#endif

#ifndef _ITEXT_
#include "IText.h"
#endif

#ifndef SOM_ODUndo_xh
#include <Undo.xh>
#endif

#ifndef SOM_ODFrame_xh
#include <Frame.xh>
#endif

#ifndef SOM_ODSession_xh
#include <ODSessn.xh>
#endif

#ifndef SOM_ODInfo_xh
#include <Info.xh>
#endif

#ifndef SOM_ODPart_xh
#include <Part.xh>
#endif

// -- OpenDoc Utilities --

#ifndef _ODDEBUG_
#include "ODDebug.h"
#endif

#ifndef _ODMEMORY_
#include "ODMemory.h"			// ODDisposePtr
#endif

#ifndef _BARRAY_
#include <BArray.h>
#endif

#ifndef _USERSRCM_
#include "UseRsrcM.h"
#endif

#ifndef _ITEXT_
#include "IText.h"
#endif

// -- Toolbox Includes --

#ifndef mathRoutinesIncludes
#include <math routines.h>
#endif

#ifndef __MEMORY__
#include <memory.h>
#endif

#include <TextUtils.h>


#include	"LedPartCommands.h"

#include	"Embedding.h"

#include	"LedPartGlobals.h"

#include "LedPart.h"


// -- OpenDoc Includes --

// for ODName  and ODByteArray
#ifndef _ODTYPES_
#include <ODTypes.h>
#endif

#ifndef SOM_ODClipboard_xh
#include <Clipbd.xh>
#endif

#ifndef SOM_ODCMDDefs_xh
#include <CMDDefs.xh>
#endif

#ifndef SOM_ODDragAndDrop_xh
#include <DragDrp.xh>
#endif
#include <TrnsForm.xh>

#ifndef SOM_ODDragItemIterator_xh
#include <DgItmIt.xh>
#endif

#ifndef SOM_ODUndo_xh
#include <Undo.xh>
#endif

#ifndef SOM_ODFrame_xh
#include <Frame.xh>
#endif
#include	<StorageU.xh>

#ifndef SOM_ODSession_xh
#include <ODSessn.xh>
#endif

#ifndef SOM_ODLinkSpec_xh
#include <LinkSpec.xh>
#endif

#ifndef SOM_ODLink_xh
#include <Link.xh>
#endif

// -- OpenDoc Utilities --

#ifndef _ODUTILS_
#include "ODUtils.h"
#endif

#ifndef _EXCEPT_
#include "Except.h"
#endif

#ifndef _STORUTIL_
#include <StorUtil.h>
#endif
#include <StdProps.xh>

#ifndef _ITEXT_
#include "IText.h"
#endif

#ifndef _ORDCOLL_
#include "OrdColl.h"
#endif

#ifndef _FOCUSLIB_
#include "FocusLib.h"
#endif

#ifndef _ODDEBUG_
#include "ODDebug.h"
#endif

#ifndef _ODMEMORY_
#include "ODMemory.h"			// ODDisposePtr
#endif

#ifndef _ISOSTR_
#include "ISOStr.h"			// ODISOStrFromCStr
#endif

// -- Toolbox Includes --

#ifndef mathRoutinesIncludes
#include <math routines.h>
#endif

#ifndef __MEMORY__
#include <memory.h>
#endif

#ifndef __OSUTILS__
#include <OSUtils.h>
#endif


#include	"LedContent.h"

#include	"LedFrameEditor.h"
















//-----------------------------------------------------------------------------
// CCommand::CCommand
//-----------------------------------------------------------------------------

CCommand::CCommand(LedPart* theEditor, 
				ODBoolean canUndo,
				ODBoolean changesContent,
				ODID undoTextIndex,
				ODID redoTextIndex)
{
	fLedPart = theEditor;
	fCanUndo = canUndo;
	
	fChangesContent = changesContent;
	fActionType = kODSingleAction;
	
	fUndoTextIndex = undoTextIndex;
	fRedoTextIndex = redoTextIndex;
	
	fDoneState = 0;
	
	fUndo = kODNULL;
}


//-----------------------------------------------------------------------------
// CCommand::~CCommand
//-----------------------------------------------------------------------------

CCommand::~CCommand()
{
	fLedPart = NULL;
}


//-----------------------------------------------------------------------------
// CCommand::SetActionType
//-----------------------------------------------------------------------------

void CCommand::SetActionType(ODActionType actionType)
{
	fActionType = actionType;
}


//-----------------------------------------------------------------------------
// CCommand::SetChangesContent
//-----------------------------------------------------------------------------

void CCommand::SetChangesContent(ODBoolean changesContent)
{
	fChangesContent = changesContent;
}


//-----------------------------------------------------------------------------
// CCommand::SetMenuTextIndexs
//-----------------------------------------------------------------------------

void CCommand::SetMenuTextIDs(ODID undoTextIndex, ODID redoTextIndex)
{
	fUndoTextIndex = undoTextIndex;
	fRedoTextIndex = redoTextIndex;
}


//-----------------------------------------------------------------------------
// CCommand::GetActionType
//-----------------------------------------------------------------------------

ODActionType CCommand::GetActionType() const
{
	return fActionType;
}


//-----------------------------------------------------------------------------
// CCommand::CanUndo
//-----------------------------------------------------------------------------

ODBoolean CCommand::CanUndo() const
{
	return fCanUndo;
}


//-----------------------------------------------------------------------------
// CCommand::ChangesContent
//-----------------------------------------------------------------------------

ODBoolean CCommand::ChangesContent() const
{
	return fChangesContent;
}


//-----------------------------------------------------------------------------
// CCommand::AbortAction
//-----------------------------------------------------------------------------

void CCommand::AbortAction()
{
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);
	// Abort this transaction
	fUndo->AbortCurrentTransaction(ev);
}


//-----------------------------------------------------------------------------
// CCommand::WriteAction
//-----------------------------------------------------------------------------

void CCommand::WriteAction(ODActionType actionType)
{
	Str255	text;
	ODName*	undoString = kODNULL;
	ODName*	redoString = kODNULL;
	
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	ODSLong tRef;
	tRef = BeginUsingLibraryResources();
	
	TRY
	
		// Get the undo string
		::GetIndString(text, kMenuStringResID, fUndoTextIndex);
		undoString = CreateIText(gGlobals->fEditorsScript, gGlobals->fEditorsLanguage, (StringPtr)&text);
		
		// Get the redo string
		GetIndString(text, kMenuStringResID, fRedoTextIndex);
		redoString = CreateIText(gGlobals->fEditorsScript, gGlobals->fEditorsLanguage, (StringPtr)&text);
		
		// Make sure we don't pass NULL menu strings to OpenDoc!
		THROW_IF_NULL(undoString);
		THROW_IF_NULL(redoString);

		// Assume that the same command is used for begin & end actions
		void* temp = kODNULL;
		ODULong	_longTmp_	=	0;
		if (actionType!=kODEndAction)
			temp = &this;
		else
			temp = &_longTmp_;
		
		ODActionData actionState;
		
		// Set up the action data
		actionState = ::CreateByteArrayStruct(temp, sizeof(temp));
		
		//actionState._maximum = sizeof(CCommand*);
		//actionState._length = sizeof(CCommand*);
		//actionState._buffer = (octet*) temp;
		
		fUndo->AddActionToHistory(ev, 
								  fLedPart->GetODPart(),		/* whichPart */
								  &actionState,					/* actionData */
								  actionType,					/* actionType */
								  undoString,					/* undoActionLabel */
								  redoString);					/* redoActionLabel */
	
		DisposeByteArrayStruct(actionState);
	
	CATCH_ALL
		ODSetSOMException(ev, ErrorCode());
	
		Led_Assert (false);	//SetMenuItemText Failed.
	ENDTRY;
	
	EndUsingLibraryResources(tRef);

	// Delete the IText data
	DisposeIText(undoString);
	DisposeIText(redoString);
}


//-----------------------------------------------------------------------------
// CCommand::DoCommand
//-----------------------------------------------------------------------------

void CCommand::DoCommand()
{
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	// Capture the command state
	this->CaptureCommandState();
	
	// for convenience, keep a reference to ODUndo
	if (this->CanUndo()) {
		fUndo = fLedPart->GetSession ()->GetUndo(ev);
	}

	if (fChangesContent)
		fLedPart->SetDirty();
		
	if (fCanUndo) {
		this->WriteAction(fActionType);
	}

	fDoneState = kODDone;
}


//-----------------------------------------------------------------------------
// CCommand::UndoCommand
//-----------------------------------------------------------------------------

void CCommand::UndoCommand()
{
	fDoneState = kODUndone;
}


//-----------------------------------------------------------------------------
// CCommand::RedoCommand
//-----------------------------------------------------------------------------

void CCommand::RedoCommand()
{
	fDoneState = kODRedone;
}


//-----------------------------------------------------------------------------
// CCommand::Commit
//
// Here you want to clean up command specific structures depnding on the value of state.
// For example, if a command creates a new piece of content and is comitted after being undone
// then it should delete the structure representing the new content. Otherwise, it should not
// deleted.
//-----------------------------------------------------------------------------

void CCommand::Commit(ODDoneState state)
{
	if (fChangesContent && (state == kODDone)||(state == kODRedone)) {
		fLedPart->SetDirty ();
	}
}


//-----------------------------------------------------------------------------
// CCommand::CaptureCommandState
//
// Here you want to save off any state info necessary for the command to be able to undo/redo.
// Example: You might make a copy of a list of references to content that is being operated on
// by the command.
//-----------------------------------------------------------------------------

void CCommand::CaptureCommandState()
{

}




#if		0



CDragShapeCommand::CDragShapeCommand(LedPart* theEditor,
										ODFacet* sourceFacet,
										ODEventData* event,
										ODBoolean showFeedBack,
										ODBoolean canUndo) :
	CCommand(theEditor,  kODTrue, kODFalse, kUndoCommandIndex, kRedoCommandIndex)
{
	fEventData = event;
	fShowDragFeedback = showFeedBack;
	fDragResult = kODDropCopy;
	fDragRegion = NULL;
	
	fSavedShapes = NULL;
	
	fSourceFacet = sourceFacet;
	
	fDestinationPart = NULL;
	fLedPart = theEditor;

	this->SetActionType(kODBeginAction);
}

CDragShapeCommand::~CDragShapeCommand()
{
	if (fDragRegion)
	{
		::DisposeRgn(fDragRegion);
		fDragRegion = NULL;
	}
	delete fSavedShapes;

}

ODShape* CDragShapeCommand::CreateDragShape(ODFacet* facet)
{
	// Led DOES provide a routine to grab the outline of the a range of text as a
	// region. Use this!!!!
	Led_Assert (false);	// NYI
	return NULL;
}

ODBoolean CDragShapeCommand::Drag(ODEventData* event)
{
	ODPart*			destination;
	ODDragAndDrop*	drag;
	ODStorageUnit*	dragSU;

	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	// Focus for drawing
	CFocus focus(ev, fSourceFacet);

	try {
		// Assume fSourceFacet is not null
		THROW_IF_NULL(fSourceFacet);
		
		// Fill the command's frame field
		fFrame = fSourceFacet->GetFrame(ev);
		
		// Get the Drag Shape & Transform it to window coordinates
		ODShape* dragShape = this->CreateDragShape(fSourceFacet);
		ODTransform* windowTransform = fSourceFacet->AcquireWindowContentTransform(ev, NULL);
		dragShape->Transform(ev, windowTransform);
			
		// Get the drag region
		TRY
			fDragRegion = (RgnHandle)::ODCopyHandle((Handle)dragShape->GetQDRegion(ev));
		CATCH_ALL
			ODSafeReleaseObject(windowTransform);
			ODSafeReleaseObject(dragShape);
			return kODFalse;
		ENDTRY
		
		// Release ref counted geometry
		ODReleaseObject(ev, windowTransform);
		ODReleaseObject(ev, dragShape);
		
		// Put the drag region into screen coordinates
		GrafPtr port;
		GetPort(&port);
		Point offset = {port->portRect.top, port->portRect.left};
		::LocalToGlobal(&offset);
		::OffsetRgn(fDragRegion, offset.h, offset.v);
		
		// Populate the drag & drop storage unit with drag data
		drag = fLedPart->GetSession ()->GetDragAndDrop(ev);
		drag->Clear(ev);
		dragSU = drag->GetContentStorageUnit(ev);
		
		ODCloneKind cloneKind = kODCloneCut;
			
		// If we are dragging a single embedded shape then externalize 
		// as per that recipe
		Embedding* tShape = fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).IsOneEmbeddedShape ();
		// Build clone information
		CloneInfo cloneInfo(0, fLedPart->GetDraft (), fSourceFacet->GetFrame(ev), kODCloneCut);
		cloneInfo.fFrom = fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).GetSelectionStart ();
		cloneInfo.fTo =  fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).GetSelectionEnd ();
		if (tShape == NULL) {
			fLedPart->fPartContent->Externalize(dragSU, &cloneInfo);
		}
		else {
			// Must find the embedded frame, so we can externalize it
			ODFrame* embeddedFrame = tShape->GetEmbeddedFacet(fSourceFacet)->GetFrame(ev);			
			// Externalize the frame
			fLedPart->fPartContent->ExternalizeSingleEmbeddedFrame(dragSU, &cloneInfo, embeddedFrame);
		}
			
		// Possible opportunity for factoring much of this and much of parallel code
		// in CCutCopyCommand into CPublishLink, but there are a few differences.
		
		// ----- Write id of source frame in case the drop occurs in the same frame.
		ODID frameID = fFrame->GetID(ev);
		dragSU->AddProperty(ev, kPropFrameInfo)->AddValue(ev, kODULong);
		StorageUnitSetValue(dragSU, ev, sizeof(ODID), &frameID);
		
		// ----- Write the offset between mousedown pt and topLeft pt of selection.
		Point mdOffset = event->where;
		Point topLeft;
			
		topLeft.v = (*fDragRegion)->rgnBBox.top;
		topLeft.h = (*fDragRegion)->rgnBBox.left;
		
		GlobalToLocal(&mdOffset);
		SubPt(topLeft,&mdOffset);
		
		dragSU->AddProperty(ev, kPropMouseDownOffset)->AddValue(ev, kODPointValue);
		StorageUnitSetValue(dragSU, ev, sizeof(Point), &mdOffset);
	
		// We must notify all frames that we are dragging that they *are* being dragged.
		// This will prevent the frame being dragged from getting called for DragEnter, etc.
	#if 1
		Led_Assert (false);	// nyi - I MUST DO THIS!!!
	#else
		fSelection->Dragging(ev, kODTrue);
	#endif
	
		// Create structures for the StartDrag call
		ODByteArray dragByteArray;
		dragByteArray._length = sizeof(RgnHandle);
		dragByteArray._maximum = sizeof(RgnHandle);
		dragByteArray._buffer = (octet*)&fDragRegion;
		
		ODByteArray eventByteArray;
		eventByteArray._length = sizeof(ODEventData*);
		eventByteArray._maximum = sizeof(ODEventData*);
		eventByteArray._buffer = (octet*)&event;
		
		fDragResult = drag->StartDrag(ev, fFrame, kODDragImageRegionHandle, &dragByteArray, &destination, &eventByteArray);
	
		fDestinationPart = destination;
		
		// Release the destination part, if there was one 
		// since it is returned to us with a bumbed ref count.
		ODReleaseObject(ev, destination);
	
	}
	catch (...) {
		return false;
	}
	return kODTrue;
}

//-----------------------------------------------------------------------------
// CDragShapeCommand::DragCompleted
//-----------------------------------------------------------------------------

void CDragShapeCommand::DragCompleted()
{
	// We must notify all frames that we are dragging that dragging has completed since
	// notified them earlier that they were being dragged. See earlier comment for 
	// explanation.
#if 1
// LGP 951210 - see other Dragging call- I MUST DO THIS!!!
#else
	fSelection->Dragging(ev, kODFalse);
#endif


	// If the destination was somewhere else AND the drag was a move
	// then get rid of the dragged shapes
	if (fDestinationPart != fLedPart->GetODPart()&&fDragResult == kODDropMove)
	{
		Environment*	ev	=	::somGetGlobalEnvironment ();
		Led_AssertNotNil (ev);
		fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).ClearSelection ();
	}

	// This is where we need to create an EndAction command, as per the
	// progammer's guide, the "dragger" is reponsible for both the begin
	// and end actions.
	if (fDragResult == kODDropUnfinished)
	{
		this->AbortAction ();
	}
	else
		this->WriteAction (kODEndAction);
}

void CDragShapeCommand::DoCommand()
{
	// Call inherited
	CCommand::DoCommand();
	
	if (fShowDragFeedback)
		this->Drag(fEventData);
	// else
	// Need to set up structures to simulate drag for scripting. $$$$$.
	
	this->DragCompleted();
	
	if (fDragRegion)
	{
		::DisposeRgn(fDragRegion);
		fDragRegion = NULL;
	}
}


//-----------------------------------------------------------------------------
// CDragShapeCommand::UndoCommand
//-----------------------------------------------------------------------------

void CDragShapeCommand::UndoCommand()
{
	// Call Inherited
	CCommand::UndoCommand();
	
	if (fDestinationPart != fLedPart->GetODPart()&&fDragResult == kODDropMove)
	{
		COrdListIterator iter(fSavedShapes);
		for (Embedding* shape = (Embedding*)iter.First();
			iter.IsNotComplete(); 
			shape = (Embedding*)iter.Next())
		{
			fLedPart->AddShape(shape);
		}

	}
}


//-----------------------------------------------------------------------------
// CDragShapeCommand::RedoCommand
//-----------------------------------------------------------------------------

void CDragShapeCommand::RedoCommand()
{
	// Call Inherited
	CCommand::RedoCommand();
	
	if (fDestinationPart != fLedPart->GetODPart()&&fDragResult == kODDropMove)
	{
	}
}


//-----------------------------------------------------------------------------
// CDragShapeCommand::Commit
//
// Here you want to clean up command specific structures depnding on the value of state.
// For example, if a command creates a new piece of content and is comitted after being undone
// then it should delete the structure representing the new content. Otherwise, it should not
// deleted.
//-----------------------------------------------------------------------------

void CDragShapeCommand::Commit(ODDoneState state)
{
	CCommand::Commit(state);
	
	if ( state != kODUndone && 
		 fDragResult == kODDropMove && 
		 fDestinationPart != fLedPart->GetODPart())
	{
		COrdListIterator iter(fSavedShapes);
		for (Embedding* shape = (Embedding*)iter.First();
			iter.IsNotComplete(); 
			shape = (Embedding*)iter.Next())
		{
			shape->Removed(true);
			delete shape;
		}
	}
}


//-----------------------------------------------------------------------------
// CDragShapeCommand::CaptureCommandState
//
// Here you want to save off any state info necessary for the command to be able to undo/redo.
// Example: You might make a copy of a list of references to content that is being operated on
// by the command.
//-----------------------------------------------------------------------------

void CDragShapeCommand::CaptureCommandState()
{
	// A move to another part is the only Undo-able drag action,
	// So save undo state here in that case.
	
	// Only a cut is undo-able, but a copy needs to make the shape and subscribe link lists
	// available to the pending publish link.
	
	fSavedShapes = new COrderedList;
}


#endif


//=============================================================================
// CDropShapeCommand::CDropShapeCommand
//=============================================================================

//-----------------------------------------------------------------------------
// CDropShapeCommand::CDropShapeCommand
//-----------------------------------------------------------------------------

CDropShapeCommand::CDropShapeCommand(LedPart* theEditor,
										ODFacet* sourceFacet,
										ODDragItemIterator* dropInfo,
										ODPoint& dropPoint,
										ODBoolean canUndo) :
	CCommand(theEditor,  canUndo, kODTrue, kUndoDropIndex, kRedoDropIndex)
{
	fDroppedInSameFrame = FALSE;
	fDropResult = kODDropFail;
	fDropPoint = dropPoint;
	fDropInfo = dropInfo;
	
	fSourceFacet = sourceFacet;
	
	fSavedShapes = NULL;
}

CDropShapeCommand::~CDropShapeCommand()
{
	delete fSavedShapes;
}

ODDropResult CDropShapeCommand::GetDropResult()
{
	return fDropResult;
}

void CDropShapeCommand::GetDropOrigin(Environment* ev, ODPoint* originPoint)
{
	// ----- Calculate originPoint (in content coordinate)
	Point dragOrigin;
	
	ODDragAndDrop* drag = fLedPart->GetSession ()->GetDragAndDrop(ev);
	
	DragReference theDrag = drag->GetDragReference(ev);
	::GetDragOrigin(theDrag, &dragOrigin);
	::GlobalToLocal(&dragOrigin);

	*originPoint = dragOrigin;
		
	ODTransform* tTransform = fSourceFacet->AcquireWindowContentTransform(ev, NULL);
	tTransform->InvertPoint(ev, originPoint); // originPoint is modified in place
	
	ODReleaseObject(ev, tTransform);
}

void CDropShapeCommand::DoCommand()
{	
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	if (!fSourceFacet->GetFrame(ev)->IsDroppable(ev))
		fDropResult = kODDropFail;
	else
	{
		this->ExecuteDrop(ev);
	}

	this->AdjustUndo(ev);
	
	// Resolve the drop before we set the action history
	CCommand::DoCommand();
	
	// if this was not a move within the same frame, then we have to add the subscribers and register them now.
	if (!fDroppedInSameFrame || (fDropResult == kODDropCopy))
	{
	}
}

void CDropShapeCommand::AdjustUndo(Environment* ev)
{
	if (fDropResult == kODDropFail)				// Drop failed--can't Undo
	{
		fCanUndo = FALSE;						// Don't call AddActionToHistory
		fChangesContent = FALSE;				// Don't call fPart->Changed()
	}
	else 
	// According to Programmer's guide Drop is always a single action. Drag
	// should specify both begin & end. 
		this->SetActionType(kODSingleAction);	
}

//-----------------------------------------------------------------------------
// CDropShapeCommand::Commit
//
// Here you want to clean up command specific structures depnding on the value of state.
// For example, if a command creates a new piece of content and is comitted after being undone
// then it should delete the structure representing the new content. Otherwise, it should not
// deleted.
//-----------------------------------------------------------------------------

void CDropShapeCommand::Commit(ODDoneState state)
{
	CCommand::Commit(state);
	
	// If the command was Done or Redone and it was not a drop move 
	// ( shapes were dragged out of document ) then we should delete the 
	// shapes in the saved shape list.
	if ( ((state!=kODDone)&&(state!=kODRedone))&&fDropResult!=kODDropMove )
	{
		COrdListIterator iter(fSavedShapes);
		for (Embedding* shape = (Embedding*)iter.First();
			iter.IsNotComplete(); 
			shape = (Embedding*)iter.Next())
		{
			shape->Removed(true);
			delete shape;
		}
	}
}

ODBoolean CDropShapeCommand::DoDroppedInSameFrame(Environment* ev,
												 ODStorageUnit* dropSU,
												 ODPoint& originPoint, 
												 ODPoint& dropPoint)
{
	return TRUE;
}


//-----------------------------------------------------------------------------
// CDropShapeCommand::CaptureCommandState
//
//-----------------------------------------------------------------------------

void CDropShapeCommand::CaptureCommandState()
{
	// Create a new list
	fSavedShapes = new COrderedList;

	// If the drop was a move, then change the menu text ids
	if (fDroppedInSameFrame && fDropResult == kODDropMove)
		this->SetMenuTextIDs(kUndoMoveIndex, kRedoMoveIndex);
	else
		this->SetMenuTextIDs(kUndoDropIndex, kRedoDropIndex);
}


//-----------------------------------------------------------------------------
// CDropShapeCommand::UndoCommand
//-----------------------------------------------------------------------------

void CDropShapeCommand::UndoCommand()
{
}

//-----------------------------------------------------------------------------
// CDropShapeCommand::RedoCommand
//-----------------------------------------------------------------------------

void CDropShapeCommand::RedoCommand()
{
}

//-----------------------------------------------------------------------------
// CDropShapeCommand::ExecuteDrop
//-----------------------------------------------------------------------------

void CDropShapeCommand::ExecuteDrop(Environment* ev)
{
	ODStorageUnit *dropSU;
	ODPoint originPoint;

	ODDragAndDrop* drag = fLedPart->GetSession ()->GetDragAndDrop(ev);
	ODULong		attributes = drag->GetDragAttributes(ev);

	// Get the drop result from D&D
	fDropResult = ((attributes & kODDropIsMove) ? kODDropMove : kODDropCopy);

	fDroppedInSameFrame = attributes & kODDropIsInSourceFrame;
	
	this->GetDropOrigin(ev, &originPoint);
	
	// ----- Iterate thru dropped items
	ODBoolean acceptedDrop = FALSE;
	for (dropSU = fDropInfo->First(ev); dropSU && !acceptedDrop;
			dropSU = fDropInfo->Next(ev))
	{		
		// The drop originated within our frame, so we can be smart
		// and just offset the selection
#if 0
		if (fDroppedInSameFrame && (fDropResult == kODDropMove))
		{
			acceptedDrop = this->DoDroppedInSameFrame(ev, dropSU, originPoint, fDropPoint);
			
			// Actually, this causes source links that are moved in their entirety to update
			// unecessarily.  We need a parameter to SelectedContentUpdated that determines
			// whether or not a change should affect fully selected source links.
		}
		else 
#endif
		{
			// Ok, the drop came from somewhere else..
			
			// So, Empty the selection
//			fSelection->ClearSelection();
			fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).ClearSelection ();
			
			// First, check to see we got a mousedown offset with the drop.
			ODPoint mouseDownOffset(0,0);
			if (dropSU->Exists(ev, kPropMouseDownOffset, kODPointValue, 0))
			{
				dropSU->Focus(ev, kPropMouseDownOffset, kODPosUndefined, kODPointValue, 0, kODPosFirstSib);
				
				StorageUnitGetValue(dropSU, ev, sizeof(ODPoint), &mouseDownOffset);
			}

			// Determine the clone kind based on the drop result
			ODCloneKind kind = (GetDropResult() == kODDropMove) ? kODCloneDropMove : kODCloneDropCopy;
			
			// D&D storage is in the from draft
			ODDraft*	fromDraft = dropSU->GetDraft(ev);
			
			ODFrame*	sourceFrame	=	fSourceFacet->GetFrame (ev);
			// Build clone info to pass to internalize
			CloneInfo info(0, fromDraft, sourceFrame, kind);
			
size_t	selStart	=	fLedPart->LookupFrameEditor (sourceFrame).GetSelectionStart ();
size_t	origSize	=	fLedPart->GetTextStore ().GetLength ();
			TRY {
				info.fFrom = selStart;
				info.fTo = fLedPart->LookupFrameEditor (sourceFrame).GetSelectionEnd ();
				fLedPart->fPartContent->HandleInternalizeContent (dropSU, &info, selStart);
			}
			CATCH_ALL
				// ----- If we failed to drop, set the drop result to kODDropFail
				fDropResult = kODDropFail;
			ENDTRY
// move the selection forward by the amount inserted
Led_Assert (fLedPart->GetTextStore ().GetLength () >= origSize);
selStart += (fLedPart->GetTextStore ().GetLength () - origSize);
fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).GetEditor ()->SetSelection (selStart, selStart);
		}
	}
	
	// ----- Notify LedPart that the drop has completed so that
	// the drag hilite will be erased.
	fLedPart->od_DropCompleted (fLedPart->GetODPart(), fDropResult);
}



CDropAsCommand::CDropAsCommand(LedPart* theEditor,
					ODFacet* sourceFacet,
					ODDragItemIterator* dropInfo,
					ODPoint& dropPoint,
					ODBoolean doEmbed) :
CDropShapeCommand(theEditor,  sourceFacet, dropInfo, dropPoint, kODTrue)
{
	fEmbedOrMerge = doEmbed;
}

CDropAsCommand::~CDropAsCommand()
{
}

void CDropAsCommand::ExecuteDrop(Environment* ev)
{
	ODStorageUnit *dropSU;
	ODPoint originPoint;

	ODDragAndDrop* drag = fLedPart->GetSession ()->GetDragAndDrop(ev);
	ODULong		attributes = drag->GetDragAttributes(ev);

	// Get the drop result from D&D
	fDropResult = ((attributes & kODDropIsMove) ? kODDropMove : kODDropCopy);

	fDroppedInSameFrame = attributes & kODDropIsInSourceFrame;
	
	this->GetDropOrigin(ev, &originPoint);
	
	// ----- Iterate thru dropped items
	ODBoolean acceptedDrop = FALSE;
	for (dropSU = fDropInfo->First(ev); dropSU && !acceptedDrop;
			dropSU = fDropInfo->Next(ev))
	{		
		// The drop originated within our frame, so we can be smart
		// and just offset the selection
#if 0
		if (fDroppedInSameFrame && (fDropResult == kODDropMove))
		{
			acceptedDrop = this->DoDroppedInSameFrame(ev, dropSU, originPoint, fDropPoint);
			
			// Actually, this causes source links that are moved in their entirety to update
			// unecessarily.  We need a parameter to SelectedContentUpdated that determines
			// whether or not a change should affect fully selected source links.
		}
		else 
#endif
		{
			// Ok, the drop came from somewhere else..
			
			// So, Empty the selection
//			fSelection->ClearSelection();
			fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).ClearSelection ();
			
			// First, check to see we got a mousedown offset with the drop.
			ODPoint mouseDownOffset(0,0);
			if (dropSU->Exists(ev, kPropMouseDownOffset, kODPointValue, 0))
			{
				dropSU->Focus(ev, kPropMouseDownOffset, kODPosUndefined, kODPointValue, 0, kODPosFirstSib);
				
				StorageUnitGetValue(dropSU, ev, sizeof(ODPoint), &mouseDownOffset);
			}

			// Determine the clone kind based on the drop result
			ODCloneKind kind = (GetDropResult() == kODDropMove) ? kODCloneDropMove : kODCloneDropCopy;
			
			// D&D storage is in the from draft
			ODDraft*	fromDraft = dropSU->GetDraft(ev);
			
			ODFrame*	sourceFrame	=	fSourceFacet->GetFrame (ev);
			// Build clone info to pass to internalize
			CloneInfo info(0, fromDraft, sourceFrame, kind);
			
size_t	selStart	=	fLedPart->LookupFrameEditor (sourceFrame).GetSelectionStart ();
size_t	origSize	=	fLedPart->GetTextStore ().GetLength ();
			TRY {
				info.fFrom = selStart;
				info.fTo = fLedPart->LookupFrameEditor (sourceFrame).GetSelectionEnd ();
	//			fLedPart->fPartContent->HandleInternalizeContent (dropSU, &info, selStart);
				fLedPart->fPartContent->HandleTranslateContent (dropSU, &info, fEmbedOrMerge);
			}
			CATCH_ALL
				// ----- If we failed to drop, set the drop result to kODDropFail
				fDropResult = kODDropFail;
			ENDTRY
// move the selection forward by the amount inserted
Led_Assert (fLedPart->GetTextStore ().GetLength () >= origSize);
selStart += (fLedPart->GetTextStore ().GetLength () - origSize);
fLedPart->LookupFrameEditor (fSourceFacet->GetFrame (ev)).GetEditor ()->SetSelection (selStart, selStart);
		}
	}
	
	// ----- Notify LedPart that the drop has completed so that
	// the drag hilite will be erased.
	fLedPart->od_DropCompleted (fLedPart->GetODPart(), fDropResult);
}








//=============================================================================
// CCutCopyCommand::CCutCopyCommand
//=============================================================================

//-----------------------------------------------------------------------------
// CCutCopyCommand::CCutCopyCommand
//-----------------------------------------------------------------------------

CCutCopyCommand::CCutCopyCommand(LedPart* theEditor,
											ODFrame* sourceFrame,
											ODCommandID command) :
CCommand(theEditor,  kODFalse, kODFalse, kUndoCopyIndex, kRedoCopyIndex)
{
	fSavedShapes = NULL;
	
	fSourceFrame = sourceFrame;
	fCommandID = command;
	
	// Cut operations are undoable and change content
	if (fCommandID==kODCommandCut)
	{
		fCanUndo = kODTrue;
		fChangesContent = kODTrue;
	}
}



//-----------------------------------------------------------------------------
// CCutCopyCommand::~CCutCopyCommand
//-----------------------------------------------------------------------------

CCutCopyCommand::~CCutCopyCommand()
{
	delete fSavedShapes;
}


//-----------------------------------------------------------------------------
// CCutCopyCommand::DoCommand
//-----------------------------------------------------------------------------

void CCutCopyCommand::DoCommand()
{
	// Call inherited
	CCommand::DoCommand();

	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	// Capture the update id, to be checked in redo, undo
	fUpdateID = this->WriteShapesToClipboard();
	
	// If this is a cut operation then cut away the shapes
	if (fCommandID==kODCommandCut)
	{
		fLedPart->LookupFrameEditor (fSourceFrame).ClearSelection ();
	}
}


//-----------------------------------------------------------------------------
// CCutCopyCommand::WriteShapesToClipboard
//-----------------------------------------------------------------------------

ODUpdateID CCutCopyCommand::WriteShapesToClipboard()
{
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	ODClipboard* clip = fLedPart->GetSession ()->GetClipboard(ev);
	
	// Remove existing clipboard data
	clip->Clear(ev);
	
	// Get the clipboard content storage unit
	ODStorageUnit* clipSU = clip->GetContentStorageUnit(ev);
		
	// Create clone info
	ODCloneKind kind;
	if (fCommandID==kODCommandCut)
		kind = kODCloneCut;
	else
		kind = kODCloneCopy;
	
	CloneInfo info(0, fLedPart->GetDraft (), fSourceFrame, kind);
	ODUpdateID updateID = clip->GetUpdateID(ev);
	ODLinkSpec* linkSpec = NULL;
	
	TRY {
		info.fFrom = fLedPart->LookupFrameEditor (fSourceFrame).GetSelectionStart ();
		info.fTo =  fLedPart->LookupFrameEditor (fSourceFrame).GetSelectionEnd ();
		fLedPart->fPartContent->Externalize(clipSU, &info);
	}
	CATCH_ALL {
		// If an exception was raised then clear the clipboard.
		// The previous contents of the clipboard are lost.
		clip->Clear(ev);
		
		delete linkSpec;
	}
	ENDTRY

	//$$$$$ (MH) New API, Not linking specific	???? does this return different from GetUpdateID?
	//or could we use the fUpdateID field?  Do we need fUPdateID in the first place?
	fActionID = clip->ActionDone(ev, kind);

	// Return the update ID for this clipboard operation
	return updateID;

}


//-----------------------------------------------------------------------------
// CCutCopyCommand::UndoCommand
//-----------------------------------------------------------------------------

void CCutCopyCommand::UndoCommand()
{
	// Call Inherited
	CCommand::UndoCommand();
	
	// We should never get here unless this is a cut operation
	COrdListIterator iter(fSavedShapes);
	for (Embedding* shape = (Embedding*)iter.First();
		iter.IsNotComplete(); 
		shape = (Embedding*)iter.Next())
	{
		fLedPart->AddShape(shape);
	}

	// (MH)  Call selection changed before adding back any publishers. That way, 
	// They won't be updated. Publishers that are entirely contained in content
	// being removed should not propagate updates, therefore don't need to when they
	// are added back in again.
	
	// fFrame->GetPresentation(ev)->Invalidate(ev, fDrawSelection->GetUpdateShape());
//	fSelection->SelectedContentUpdated(ev);
}


//-----------------------------------------------------------------------------
// CCutCopyCommand::RedoCommand
//-----------------------------------------------------------------------------

void CCutCopyCommand::RedoCommand()
{
	// Call Inherited
	CCommand::RedoCommand();
	
	this->WriteShapesToClipboard();
	
	// If this is a cut operation then cut away the shapes
	if (fCommandID==kODCommandCut)
	{
//		fSelection->ClearSelection();
		fLedPart->LookupFrameEditor (fSourceFrame).ClearSelection ();
	}
}


//-----------------------------------------------------------------------------
// CCutCopyCommand::Commit
//
// Here you want to clean up command specific structures depnding on the value of state.
// For example, if a command creates a new piece of content and is comitted after being undone
// then it should delete the structure representing the new content. Otherwise, it should not
// deleted.
//-----------------------------------------------------------------------------

void CCutCopyCommand::Commit(ODDoneState state)
{
	CCommand::Commit(state);
	
	if ( (state != kODUndone) && (fCommandID == kODCommandCut) )
	{
		COrdListIterator iter(fSavedShapes);
		for (Embedding* shape = (Embedding*)iter.First();
			iter.IsNotComplete(); 
			shape = (Embedding*)iter.Next())
		{
			shape->Removed(true);
			delete shape;
		}
	}
}


//-----------------------------------------------------------------------------
// CCutCopyCommand::CaptureCommandState
//
// Here you want to save off any state info necessary for the command to be able to undo/redo.
// Example: You might make a copy of a list of references to content that is being operated on
// by the command.
//-----------------------------------------------------------------------------

void CCutCopyCommand::CaptureCommandState()
{
	// Only a cut is undo-able, but a copy needs to make the shape and subscribe 
	// link lists available to the pending publish link.
	fSavedShapes = new COrderedList;
	// Set the menu text IDs for the right command 
	if (fCommandID == kODCommandCut)
	{
		this->SetMenuTextIDs(kUndoCutIndex, kRedoCutIndex);
	}
	else
		this->SetMenuTextIDs(kUndoCopyIndex, kRedoCopyIndex);
}


//=============================================================================
// CPasteCommand::CPasteCommand
//=============================================================================

//-----------------------------------------------------------------------------
// CPasteCommand::CPasteCommand
//-----------------------------------------------------------------------------

CPasteCommand::CPasteCommand(LedPart* theEditor,
										ODFrame* sourceFrame) :
	CCommand(theEditor,  kODTrue, kODTrue, kUndoPasteIndex, kRedoPasteIndex)
{
	fSourceFrame = sourceFrame;
	
	fSavedShapes = NULL;
}

//-----------------------------------------------------------------------------
// CPasteCommand::~CPasteCommand
//-----------------------------------------------------------------------------

CPasteCommand::~CPasteCommand()
{
	delete fSavedShapes;
}


//-----------------------------------------------------------------------------
// CPasteCommand::HandlePaste
//-----------------------------------------------------------------------------

void CPasteCommand::HandlePaste()
{
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	// Get the clipboard storage unit and internalize it
	ODClipboard* clip = fLedPart->GetSession ()->GetClipboard(ev);
	
	// Get the clipboard content storage unit
	ODStorageUnit* contentSU = clip->GetContentStorageUnit(ev);
		
	// The clone kind is paste
	ODCloneKind kind =  kODClonePaste;
	
	// D&D storage is in the from draft
	ODDraft*	fromDraft = contentSU->GetDraft(ev);
	
	// Build clone info to pass to internalize
	CloneInfo info(0, fromDraft, fSourceFrame, kind);

size_t	selStart	=	fLedPart->LookupFrameEditor (fSourceFrame).GetSelectionStart ();
size_t	origSize	=	fLedPart->GetTextStore ().GetLength ();
	TRY {
//		fSelection->HandleInternalizeContent(ev, contentSU, &info);
		info.fFrom = selStart;
		info.fTo = fLedPart->LookupFrameEditor (fSourceFrame).GetSelectionEnd ();
		fLedPart->fPartContent->HandleInternalizeContent (contentSU, &info, selStart);
	}
	CATCH_ALL {
		Led_Assert (false);	// Failure to internlalize paste!
	}
	ENDTRY

// move the selection forward by the amount inserted
Led_Assert (fLedPart->GetTextStore ().GetLength () >= origSize);
selStart += (fLedPart->GetTextStore ().GetLength () - origSize);
fLedPart->LookupFrameEditor (fSourceFrame).GetEditor ()->SetSelection (selStart, selStart);
}


//-----------------------------------------------------------------------------
// CPasteCommand::DoCommand
//-----------------------------------------------------------------------------

void CPasteCommand::DoCommand()
{	

	// Resolve the drop before we set the action history
	CCommand::DoCommand();
	
	// Empty the selection
//	fSelection->ClearSelection();
	fLedPart->LookupFrameEditor (fSourceFrame).ClearSelection ();

	// Do the paste
	this->HandlePaste();
}


//-----------------------------------------------------------------------------
// CPasteCommand::Commit
//
// Here you want to clean up command specific structures depending on the value of state.
// For example, if a command creates a new piece of content and is comitted after being undone
// then it should delete the structure representing the new content. Otherwise, it should not
// deleted.
//-----------------------------------------------------------------------------

void CPasteCommand::Commit(ODDoneState state)
{
	CCommand::Commit(state);
	
	// If the command was Done or Redone and it was not a drop move 
	// ( shapes were dragged out of document ) then we should delete the 
	// shapes in the saved shape list.
	if ((state!=kODDone && state!=kODRedone))
	{
		COrdListIterator iter(fSavedShapes);
		for (Embedding* shape = (Embedding*)iter.First();
			iter.IsNotComplete(); 
			shape = (Embedding*)iter.Next())
		{
			shape->Removed(true);
			delete shape;
		}
	}

}


//-----------------------------------------------------------------------------
// CPasteCommand::CaptureCommandState
//
//-----------------------------------------------------------------------------

void CPasteCommand::CaptureCommandState()
{
	// Create a new list
	fSavedShapes = new COrderedList;
}

void CPasteCommand::UndoCommand()
{
	// select pasted shapes and remove them from the document
//	fSelection->ClearSelection();
	fLedPart->LookupFrameEditor (fSourceFrame).ClearSelection ();
}

void CPasteCommand::RedoCommand ()
{
	// add dropped shapes back into document
	COrdListIterator ite(fSavedShapes);
	for (Embedding* shape = (Embedding*)ite.First(); ite.IsNotComplete(); shape = (Embedding*)ite.Next())
	{
		// Add the shape to the content list
		fLedPart->AddShape(shape);
	}
		
	// (MH)  Call selection changed before adding back any publishers. That way, 
	// They won't be updated. Publishers that are entirely contained in content
	// being removed/replaced don't need to propagate any updates.
	
	// fFrame->GetPresentation(ev)->Invalidate(ev, fDrawSelection->GetUpdateShape());
//	fSelection->SelectedContentUpdated(ev);
}

//=============================================================================
// CPasteAsCommand::CPasteAsCommand
//=============================================================================

//-----------------------------------------------------------------------------
// CPasteAsCommand::CPasteAsCommand
//-----------------------------------------------------------------------------

CPasteAsCommand::CPasteAsCommand(LedPart* theEditor,
										ODFrame* sourceFrame,
										ODBoolean embedOrMerge) :
CPasteCommand(theEditor, sourceFrame)
{
	fEmbedOrMerge = embedOrMerge;
}

//-----------------------------------------------------------------------------
// CPasteAsCommand::~CPasteAsCommand
//-----------------------------------------------------------------------------

CPasteAsCommand::~CPasteAsCommand()
{
}


//-----------------------------------------------------------------------------
// CPasteAsCommand::HandlePaste
//-----------------------------------------------------------------------------

void	CPasteAsCommand::HandlePaste()
{	
	Environment*	ev	=	::somGetGlobalEnvironment ();
	Led_AssertNotNil (ev);

	// Get the clipboard storage unit and internalize it
	ODClipboard* clip = fLedPart->GetSession ()->GetClipboard(ev);
	
	// Get the clipboard content storage unit
	ODStorageUnit* contentSU = clip->GetContentStorageUnit(ev);
		
	// The clone kind is paste
	ODCloneKind kind =  kODClonePaste;
	
	// D&D storage is in the from draft
	ODDraft*	fromDraft = contentSU->GetDraft(ev);
	
	// Build clone info to pass to internalize
	CloneInfo info(0, fromDraft, fSourceFrame, kind);
	
	size_t	selStart	=	fLedPart->LookupFrameEditor (fSourceFrame).GetSelectionStart ();
	size_t	origSize	=	fLedPart->GetTextStore ().GetLength ();
	TRY {
		info.fFrom = selStart;
		info.fTo = fLedPart->LookupFrameEditor (fSourceFrame).GetSelectionEnd ();
//		fLedPart->fPartContent->HandleInternalizeContent (contentSU, &info, selStart);
		fLedPart->fPartContent->HandleTranslateContent (contentSU, &info, fEmbedOrMerge);
//		fSelection->HandleTranslateContent(ev, contentSU, &info, fDoEmbed);
	}
	CATCH_ALL
		Led_Assert (false);	//	Failure to internlalize paste
	ENDTRY

	// move the selection forward by the amount inserted
	Led_Assert (fLedPart->GetTextStore ().GetLength () >= origSize);
	selStart += (fLedPart->GetTextStore ().GetLength () - origSize);
	fLedPart->LookupFrameEditor (fSourceFrame).GetEditor ()->SetSelection (selStart, selStart);
}



