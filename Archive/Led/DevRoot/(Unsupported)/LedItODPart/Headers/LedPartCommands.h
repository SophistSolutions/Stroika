/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Headers/LedPartCommands.h,v 1.5 1996/12/13 18:09:57 lewis Exp $
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
 *	$Log: LedPartCommands.h,v $
 *	Revision 1.5  1996/12/13 18:09:57  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 1.4  1996/12/05  21:11:06  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/09/01  15:44:28  lewis
 *	***Led 20 Released***
 *
 *	Revision 1.2  1996/02/05  05:04:47  lewis
 *	CDropAsCommand - based on looking at DR4 1.0.1 DrawShapes example
 *
 *	Revision 1.1  1996/01/22  05:49:53  lewis
 *	Initial revision
 *
 *	Revision 2.2  1995/12/13  06:06:31  lewis
 *	Misc clenaups - mostly losing Environment* in a few places
 *
 *	Revision 2.1  1995/12/06  02:00:23  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *
 *
 *
 */
#ifndef __LedPartCommands_hh__
#define __LedPartCommands_hh__	1

// -- LedPart Includes --


#include	"LedPartDef.h"






#include	"LedPartDef.h"

#include	<ODTypesM.xh>


//=============================================================================
// Forward Declarations
//=============================================================================
class LedPart;
class	ODUndo;

//=============================================================================
// Constants
//=============================================================================

//=============================================================================
// Enumerations
//=============================================================================


//=============================================================================
// CCommand
//
// Assumptions: 
// The command object assumes that for all Begin/End action pairs, the SAME
// command object will be used. In this case, null is written out for the begin
// action, and the this pointer for the command is written out for all other 
// action types. This also means that commit will only be called once on such
// commands.
//
//=============================================================================
class CCommand
{
public:
	
	// -- Init --
	CCommand(LedPart* theEditor, 
				ODBoolean canUndo = false,
				ODBoolean changesContent = false,
				ODID undoTextIndex = kUndoCommandIndex,
				ODID redoTextIndex = kRedoCommandIndex);
	
	virtual ~CCommand();

public:

	// -- Accessors --
	void 			SetChangesContent(ODBoolean changesContent);
	
	void 			SetMenuTextIDs(ODID undoTextIndex, ODID redoTextIndex);
		
	ODBoolean		CanUndo() const;
	ODBoolean		ChangesContent() const;
	ODCommandID		GetCommandID() const;
	
	// -- Actions --
	void			SetActionType(ODActionType actionType);
	ODActionType	GetActionType() const;
	void			WriteAction(ODActionType actionType);
	void			AbortAction();
	
	// Undo-able commands
	// Should call inherited in derivative classes
	virtual void	UndoCommand();
	virtual void 	RedoCommand();
	
	// Is called from DoCommand. Used to store information
	// about the command ( shapelists, etc. ).
	virtual void 	CaptureCommandState();
	
	// Called just before the command is deleted.
	virtual void 	Commit(ODDoneState state);
	
	// -- Must Override this method --
	// This method should only be called once per instance
	// Should call inherited in derivative classes
	virtual void 	DoCommand();
	

//----------------------------------------------------------------------------------------
// Data Members
//
protected:
	ODBoolean		fCanUndo;			// Default True
	ODBoolean		fChangesContent;	// Default True
	ODActionType	fActionType;		// Default = kODSingleAction
	LedPart*		fLedPart;
	ODFrame*		fFrame;				// Can be NULL

private:
	ODUndo*		fUndo;
	ODDoneState	fDoneState;					// doneState values are kODDone, kODUndone, or kODRedone (ODTypes.h)

	ODID		fUndoTextIndex;				// For Undo menu item. Default: kUndoCommandIndex
	ODID		fRedoTextIndex;				// For Redo menu item. Default: kRedoCommandIndex
};








//=============================================================================
// Constants
//=============================================================================
const ODPropertyName 	kPropFrameInfo = "SophistSolutions:Property:FrameInfo";
const ODPropertyName 	kPropMouseDownOffset = "SophistSolutions:Property:MouseDownLoc";
const ODValueType 		kODPointValue = "Sample:ValueType:ODPoint";


//=============================================================================
// Forward Declarations
//=============================================================================
class LedPart;
class Embedding;
class ODFacet;
class COrderedList;
class ODDragItemIterator;
class	ODStorageUnit;
class	ODPoint;
class	ODShape;
class	ODPart;

#if		0

//=============================================================================
// CDragShapeCommand
//=============================================================================
class CDragShapeCommand : public CCommand
{
public:
	
	// -- Init --
	CDragShapeCommand(LedPart* theEditor,
					ODFacet* sourceFacet,
					ODEventData* event,
					ODBoolean showFeedBack,
					ODBoolean canUndo = true);
	
	virtual ~CDragShapeCommand();

public:
	
	// Undo-able commands
	// Should call inherited in derivative classes
	virtual void UndoCommand();
	virtual void RedoCommand();
	
	// Is called from DoCommand. Used to store information
	// about the command ( shapelists, etc. ).
	virtual void CaptureCommandState();
	
	// Called just before the command is deleted.
	// Default does nothing
	virtual void Commit(ODDoneState state);
	
	// -- Must Override this method --
	// This method should only be called once per instance
	// Should call inherited in derivative classes
	virtual void DoCommand();
	
	// New API ( Drag Stuff )
	ODBoolean	Drag(ODEventData* event);
	void 		DragCompleted();
	ODShape* 	CreateDragShape(ODFacet* facet);
	

//----------------------------------------------------------------------------------------
// Data Members
//
private:

	ODEventData*		fEventData;
	ODBoolean			fShowDragFeedback;
	COrderedList*		fSavedShapes;
	
	ODRgnHandle			fDragRegion;
	ODDragResult		fDragResult;
	ODFacet*			fSourceFacet;
	
	ODPart*				fDestinationPart;
};

#endif


//=============================================================================
// CDropShapeCommand
//=============================================================================
class CDropShapeCommand : public CCommand
{
public:
	// -- Init --
	CDropShapeCommand(LedPart* theEditor,
					ODFacet* sourceFacet,
					ODDragItemIterator* dropInfo,
					ODPoint& dropPoint,
					ODBoolean canUndo = true);
	
	virtual ~CDropShapeCommand();
	
	// -- Accessors --
	ODDropResult GetDropResult();
	
	// Undo-able commands
	// Should call inherited in derivative classes
	virtual void UndoCommand();
	virtual void RedoCommand();
	
	// Is called from DoCommand. Used to store information
	// about the command ( shapelists, etc. ).
	virtual void CaptureCommandState();
	
	// Called just before the command is deleted.
	// Default does nothing
	virtual void Commit(ODDoneState state);
	
	// -- Must Override this method --
	// This method should only be called once per instance
	// Should call inherited in derivative classes
	virtual void DoCommand();

	// -- New API ( Drop Stuff ) --
	
	// Check the drop result, and cancel Undo if the drop failed.
	void AdjustUndo(Environment* ev);

	// Return non-zero value if the source part was moved;
	void GetDropOrigin(Environment* ev, ODPoint* originPoint);

	// Drop methods
	void ExecuteDrop(Environment* ev);

	ODBoolean DoDroppedInSameFrame(Environment* ev, 
							ODStorageUnit* dropSU, 
							ODPoint& mouseOffset, 
							ODPoint& dropPoint);


protected:
	ODFacet*			fSourceFacet;

	ODDropResult		fDropResult;
	ODBoolean			fDroppedInSameFrame;
	ODDragItemIterator* fDropInfo;
	
	ODPoint				fDropPoint;
	ODPoint				fDropOffset;
	
	COrderedList*	fSavedShapes;
};

class CDropAsCommand : public CDropShapeCommand
{
public:
	// -- Init --
	CDropAsCommand(LedPart* theEditor,
					ODFacet* sourceFacet,
					ODDragItemIterator* dropInfo,
					ODPoint& dropPoint,
					ODBoolean doEmbed);
	
	virtual ~CDropAsCommand();
	
	// Drop methods
	void ExecuteDrop(Environment* ev);


private:
	// True for embed, false for merge
	ODBoolean			fEmbedOrMerge;
	
};



class	CCutCopyCommand : public CCommand
{
public:
	
	// -- Init --
	CCutCopyCommand(LedPart* theEditor,
					ODFrame* sourceFrame,
					ODCommandID command);
	
	virtual ~CCutCopyCommand();

public:
	
	// Inherited API
	virtual void UndoCommand();
	virtual void RedoCommand();
	virtual void CaptureCommandState();
	virtual void Commit(ODDoneState state);
	virtual void DoCommand();
	
	// New API ( Cut / Copy Stuff )
	ODUpdateID	WriteShapesToClipboard();
	

//----------------------------------------------------------------------------------------
// Data Members
//
private:

	ODFrame*			fSourceFrame;
	
	ODUpdateID			fUpdateID;
  	ODUpdateID 			fActionID;  // $$$$$ (MH) not linking specific
	ODCommandID			fCommandID;
	COrderedList*		fSavedShapes;
};


//=============================================================================
// CPasteCommand
//=============================================================================
class CPasteCommand : public CCommand
{
public:
	// -- Init --
	CPasteCommand(LedPart* theEditor, ODFrame* sourceFrame);
	
	virtual ~CPasteCommand();
	
	// -- Inherited API --
	virtual void DoCommand();
	virtual void UndoCommand();
	virtual void RedoCommand();
	
	virtual void CaptureCommandState();
	virtual void Commit(ODDoneState state);

	// -- New API ( Paste Stuff ) --
	
	// Paste methods
	virtual void HandlePaste();

protected:
	ODFrame*			fSourceFrame;
	
	COrderedList*		fSavedShapes;
};

//=============================================================================
// CPasteAsCommand
// This class was created to extend the functionality of the past command
// to include forcing a merge or embed based on the result of the paste as
// dialog. This command does not include any support for translation of kinds
// since this part only supports one kind. If any support for translation on
// paste as were to be added, it could be done here as well.
//=============================================================================
class CPasteAsCommand : public CPasteCommand
{
public:
	// -- Init --
	CPasteAsCommand(LedPart* theEditor,
					ODFrame* sourceFrame,
					ODBoolean embedOrMerge);
	
	virtual ~CPasteAsCommand();
	
	// -- Inherited API --
	virtual void HandlePaste();

private:
	// True for embed, false for merge
	ODBoolean			fEmbedOrMerge;
};


#endif	/*__LedPartCommands_hh__*/



