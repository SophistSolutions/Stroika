/******************************************************************************
	CEditPane.c
	
	Methods for a text editing pane.
		
	Copyright © 1989 Symantec Corporation. All rights reserved.

 ******************************************************************************/

#include	<string>

#include "CEditPane.h"
#include "Commands.h"
#include "CDocument.h"
#include "CBartender.h"
#include "Constants.h"

#if		!qUseBuiltinTE
	#include	"ChunkedArrayTextStore.hh"
	#include	"SimpleTextStore.hh"
	#include	"StyledTextEmbeddedObjects.hh"
#endif

extern	CBartender	*gBartender;



#define	cmdFind			1001
#define	cmdFindAgain	1002




CEditPane::CEditPane (CView *anEnclosure, CBureaucrat *aSupervisor):
	qEditPaneBaseClass (anEnclosure, aSupervisor, 1, 1, 0, 0, sizELASTIC, sizELASTIC, 432, false)
#if		qUseLedUndo
	,fCommandHandler (1)
#endif
	,fLastSearchText (NULL),
	fWrapSearch (true),
	fWholeWordSearch (false),
	fCaseSensativeSearch (false)
{
	Rect	margin;

#if		!qUseBuiltinTE
	SpecifyTextStore (new ChunkedArrayTextStore ());
#endif

	FitToEnclosure(TRUE, TRUE);

		/**
		 **	Give the edit pane a little margin.
		 **	Each element of the margin rectangle
		 **	specifies by how much to change that
		 **	edge. Positive values are down and to
		 **	right, negative values are up and to
		 **	the left.
		 **
		 **/

	SetRect(&margin, 2, 2, -2, -2);
	ChangeSize(&margin, FALSE);

#if		qUseLedUndo
	SetCommandHandler (&fCommandHandler);
#endif
}

CEditPane::~CEditPane ()
{
#if		qUseLedUndo
	SetCommandHandler (NULL);
#endif
#if		!qUseBuiltinTE
	TextStore_*	ts	=	PeekAtTextStore ();
	SpecifyTextStore (NULL);
	delete ts;
#endif
}

void	CEditPane::UpdateMenus()
{
	inherited::UpdateMenus();
#if		qUseLedUndo
	Str255	undoStr;
	undoStr[0] = fCommandHandler.GetUndoRedoWhatMessageText ((char*)&undoStr[1], sizeof (undoStr));
	gBartender->SetCmdText(cmdUndo, undoStr);
	if (fCommandHandler.CanUndo () or fCommandHandler.CanRedo ()) {
		gBartender->EnableCmd (cmdUndo);
	}
	else {
		gBartender->DisableCmd (cmdUndo);
	}
#endif

	gBartender->EnableCmd (cmdFind);
	if (fLastSearchText == NULL) {
		gBartender->DisableCmd (cmdFindAgain);
	}
	else {
		gBartender->EnableCmd (cmdFindAgain);
	}
}

void	CEditPane::DoCommand (long theCommand)
{
     if (((theCommand == cmdPaste) || (theCommand == cmdCut)) && 
        !((CDocument *)itsSupervisor)->dirty) {
        
		((CDocument *)itsSupervisor)->dirty = TRUE;
		gBartender->EnableCmd(cmdSave);
		gBartender->EnableCmd(cmdSaveAs);
	}

#if		qUseLedUndo
	if (theCommand == cmdUndo) {
		if (fCommandHandler.CanUndo ()) {
			fCommandHandler.DoUndo ();
		}
		else if (fCommandHandler.CanRedo ()) {
			fCommandHandler.DoRedo ();
		}
		return;
	}
#endif
	if ((theCommand == cmdFind) or (theCommand == cmdFindAgain)) {
		OnFindEtcCommand (theCommand);
	}
	inherited::DoCommand(theCommand);
}
#define	kFind_DialogID	1001

void	CEditPane::OnFindEtcCommand (long theMenuCommand)
{
	if (theMenuCommand == cmdFind) {
		DialogPtr	dialog = GetNewDialog (kFind_DialogID, NULL, (WindowPtr)-1);
		if ( dialog ) {
			Handle	itemHandle;
			Rect	itemRect;
			short	itemType;
			Str255	textPStr;

			SetDialogDefaultItem(dialog, ok);

			// GROSS HACK - we really should encapsualte this separately, and have subclasses for each dialog!!!
			GetDialogItem (dialog, 4, &itemType, &itemHandle, &itemRect);
			textPStr[0] = (fLastSearchText==NULL)? 0: strlen (fLastSearchText);
			memcpy (&textPStr[1], fLastSearchText, textPStr[0]);
			SetDialogItemText (itemHandle, textPStr);
			SelectDialogItemText (dialog, 4, 0, 1000);

			GetDialogItem (dialog, 5, &itemType, &itemHandle, &itemRect);
			SetControlMinimum (ControlRef (itemHandle), 0);
			SetControlMaximum (ControlRef (itemHandle), 1);
			SetControlValue (ControlRef (itemHandle), fWrapSearch);

			GetDialogItem (dialog, 6, &itemType, &itemHandle, &itemRect);
			SetControlMinimum (ControlRef (itemHandle), 0);
			SetControlMaximum (ControlRef (itemHandle), 1);
			SetControlValue (ControlRef (itemHandle), fWholeWordSearch);

			GetDialogItem (dialog, 7, &itemType, &itemHandle, &itemRect);
			SetControlMinimum (ControlRef (itemHandle), 0);
			SetControlMaximum (ControlRef (itemHandle), 1);
			SetControlValue (ControlRef (itemHandle), fCaseSensativeSearch);

			InitCursor ();
			ShowWindow(dialog);
			
			short	itemHit	=	cancel;
			do {
				ModalDialog (NULL, &itemHit);
				if (itemHit >= 5 and itemHit <= 7) {
					GetDialogItem (dialog, itemHit, &itemType, &itemHandle, &itemRect);
					SetControlValue (ControlRef (itemHandle), !GetControlValue (ControlRef (itemHandle)));
				}
			}
			while ((itemHit != ok) and (itemHit != cancel));

			GetDialogItem (dialog, 4, &itemType, &itemHandle, &itemRect);
			delete[] fLastSearchText;
			fLastSearchText = NULL;
			GetDialogItemText (itemHandle, textPStr);
			fLastSearchText = new char[textPStr[0] + 1];
			memcpy (fLastSearchText, &textPStr[1], textPStr[0]);
			fLastSearchText[textPStr[0]] = '\0';

			GetDialogItem (dialog, 5, &itemType, &itemHandle, &itemRect);
			fWrapSearch = GetControlValue (ControlRef (itemHandle));

			GetDialogItem (dialog, 6, &itemType, &itemHandle, &itemRect);
			fWholeWordSearch = GetControlValue (ControlRef (itemHandle));

			GetDialogItem (dialog, 7, &itemType, &itemHandle, &itemRect);
			fCaseSensativeSearch = GetControlValue (ControlRef (itemHandle));

			DisposeDialog(dialog);
			if (itemHit != ok) {
				return;
			}
		}
	}

	// search for last text entered into find dialog (could have been a find again).
	size_t	origSelStart	=	GetSelectionStart ();
	size_t	origSelEnd		=	GetSelectionEnd ();
	size_t	whereTo	=	GetTextStore ().Find (fLastSearchText, origSelEnd, fWrapSearch? TextStore_::eWrapAtEnd: TextStore_::eSearchToEnd, fCaseSensativeSearch, fWholeWordSearch);
	if ((whereTo == kBadIndex) or (whereTo == origSelStart)) {
		Led_BeepNotify ();
	}
	else {
		SetSelection (whereTo, whereTo + strlen (fLastSearchText));
		ScrollToSelection ();
	}
}

void	CEditPane::DoKeyDown (char theChar, Byte keyCode, EventRecord *macEvent)
{
    inherited::DoKeyDown(theChar, keyCode, macEvent);

    switch (keyCode) {
        case KeyHome:
        case KeyEnd:
        case KeyPageUp:
        case KeyPageDown:
            break;
            
        default:    
            if (!((CDocument *)itsSupervisor)->dirty) {
                ((CDocument *)itsSupervisor)->dirty = TRUE;
                gBartender->EnableCmd(cmdSave);
                gBartender->EnableCmd(cmdSaveAs);
            }
            break;
    }
}

void	CEditPane::DoAutoKey(char theChar, Byte keyCode, EventRecord *macEvent)
{
	inherited::DoAutoKey(theChar, keyCode, macEvent);

    switch (keyCode) {
    
        case KeyHome:
        case KeyEnd:
        case KeyPageUp:
        case KeyPageDown:
            break;
            
        default:    
            if (!((CDocument *)itsSupervisor)->dirty) {
                ((CDocument *)itsSupervisor)->dirty = TRUE;
                gBartender->EnableCmd(cmdSave);
                gBartender->EnableCmd(cmdSaveAs);
            }
            break;
    }
}

