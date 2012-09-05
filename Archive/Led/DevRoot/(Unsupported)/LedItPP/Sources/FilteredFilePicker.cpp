/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Sources/FilteredFilePicker.cpp,v 2.10 1997/12/24 04:42:23 lewis Exp $
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
 *	$Log: FilteredFilePicker.cpp,v $
 *	Revision 2.10  1997/12/24 04:42:23  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  1997/09/29  17:59:09  lewis
 *	Lose qLedFirstIndex
 *
 *	Revision 2.8  1997/07/27  16:02:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.7  1996/12/13  18:11:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.6  1996/12/05  21:25:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1996/09/01  15:45:40  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.4  1996/06/01  02:55:37  lewis
 *	tried to make it STRICT_ friendly, but gave up.
 *	Added #pragma options align=mac68k.
 *
 *	Revision 2.3  1996/05/23  20:45:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1996/03/16  19:18:19  lewis
 *	Fixed bug where we added popups to subdialogs launched by custom file dialog
 *	proc - like 'file is locked' alert. Be careful to only add our
 *	pupups to the FIRST dialog.
 *
 *	Revision 2.1  1996/03/04  08:20:53  lewis
 *	Never filter out Folders in sfgetfile filterproc.
 *
 *	Revision 2.0  1996/02/26  23:29:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#include	<Icons.h>

#include	<PP_Resources.h>
#include	<UDesktop.h>

#include	"LedItResources.h"

#include	"FilteredFilePicker.h"


static	int	Append_DITL (DialogPtr dialog, int item_list_ID);

inline	MenuHandle	GetPopUpMenuHandle (ControlHandle thisControl)
	{
		// See LStdPopupMenu::GetMacMenuH() to see if there is a UniversalHeaders _STRICT
		// way of doing this. There isn't as of PreCW9- LGP 960529
		Led_RequireNotNil (thisControl);
		PopupPrivateData** theMenuData = (PopupPrivateData**)(*thisControl)->contrlData;
		Led_AssertNotNil (theMenuData);
		Led_EnsureNotNil ((*theMenuData)->mHandle);
		return((*theMenuData)->mHandle);
	}






/*
 ********************************************************************************
 ******************************** FilteredSFGetDLog *****************************
 ********************************************************************************
 */
FilteredSFGetDLog::FilteredSFGetDLog (const TypeSpec* typeNameList, size_t nTypes):
	fTypeSpecs (typeNameList),
	fTypeSpecCount (nTypes),
	fCurPopupIdx (kBadIndex),
	fPopupDLGItemNumber (0),
	fMainDialog (NULL)
{
}

bool	FilteredSFGetDLog::PickFile (StandardFileReply* result, bool* typeSpecified, size_t* typeIndex)
{
	Led_RequireNotNil (result);
	UDesktop::Deactivate ();

	fMainDialog = NULL;		// in case PickFile () called multiple times

	Point	where	=	{ -1, -1 };
	#if		defined (powerc)
		static	RoutineDescriptor dlgProcHook_ = BUILD_ROUTINE_DESCRIPTOR (uppDlgHookYDProcInfo, SFGetDlgHook);
		RoutineDescriptor* dlgProcHook = &dlgProcHook_;
		static	RoutineDescriptor fileFilter_ = BUILD_ROUTINE_DESCRIPTOR (uppFileFilterYDProcInfo, SFFilterProc);
		RoutineDescriptor* fileFilter = &fileFilter_;
	#else
		DlgHookYDProcPtr	dlgProcHook	=	&SFGetDlgHook;
		FileFilterYDProcPtr	fileFilter	=	&SFFilterProc;
	#endif
	::CustomGetFile (fileFilter, -1, NULL, result, 0, where, dlgProcHook, NULL, NULL, NULL, this);
	UDesktop::Activate ();

	if (typeSpecified != NULL) {
		*typeSpecified = (fCurPopupIdx != kBadIndex) and (fCurPopupIdx >= 4);
	}
	if (typeSpecified != NULL and *typeSpecified and typeIndex != NULL) {
		*typeIndex = fCurPopupIdx-4 + 0;
	}
	return result->sfGood;
}

pascal short	FilteredSFGetDLog::SFGetDlgHook (short item, DialogPtr dialog, void* myData)
{
	FilteredSFGetDLog*	sThis	=	(FilteredSFGetDLog*)myData;

	// Must check sThis->fMainDialog==NULL cuz this same hook function gets called when SFGetFile
	// launches other SUB-Dialogs - like file is locked warning - and we don't want the popup added
	// to that dialog as well.
	if (item == sfHookFirstCall and sThis->fMainDialog == NULL) {
		sThis->fMainDialog = dialog;
		sThis->fPopupDLGItemNumber	=	Append_DITL (dialog, kOpenDLOGAdditionItems_DialogID);
		Handle	h;
		short	i;
		Rect	r;
		::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
		MenuHandle		mm	=	GetPopUpMenuHandle ((ControlHandle)h);
		for (size_t i = 0; i < sThis->fTypeSpecCount; i++) {
			Str255	tmp;
			tmp[0] = strlen (sThis->fTypeSpecs[i].fName);
			memcpy (&tmp[1], sThis->fTypeSpecs[i].fName, tmp[0]);
			AppendMenu (mm, "\pHiMom");
			SetMenuItemText (mm, ::CountMItems (mm), tmp);
		}
		::SetControlMinimum ((ControlHandle)h, 1);
		::SetControlMaximum ((ControlHandle)h, i);
		::SetControlValue ((ControlHandle)h, 1);
		sThis->fCurPopupIdx = 1;
	}
	else if (sThis->fMainDialog == dialog) {
		Handle	h;
		short	i;
		Rect	r;
		::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
		int	newValue = ::GetControlValue ((ControlHandle)h);
		if (newValue != sThis->fCurPopupIdx) {
			sThis->fCurPopupIdx = newValue;
			return sfHookRebuildList;
		}
	}
	
	return item;
}

pascal	Boolean	FilteredSFGetDLog::SFFilterProc (CInfoPBPtr pb, void* myData)
{
	FilteredSFGetDLog*	sThis	=	(FilteredSFGetDLog*)myData;

	Led_AssertNotNil (pb);

	// Never filter directories
	if (pb->dirInfo.ioFlAttrib & ioDirMask) {
		return false;
	}

	OSType	thisFileType	=	pb->hFileInfo.ioFlFndrInfo.fdType;
	bool	filtered	=	true;
	switch (sThis->fCurPopupIdx) {
		case	1: {
			// TRUE iff in list of types...
			for (size_t i = 0; i < sThis->fTypeSpecCount; i++) {
				if (thisFileType == sThis->fTypeSpecs[i].fOSType) {
					filtered = false;
					break;
				}
			}
		}
		break;

		case	2: {
			filtered = false;	// ALL
		}
		break;

		default: {
			// true only if type matches THIS entry
			if (thisFileType == sThis->fTypeSpecs[sThis->fCurPopupIdx-4].fOSType) {
				filtered = false;
			}
		}
		break;
	}

	return filtered;
}





/*
 ********************************************************************************
 ******************************** FilteredSFPutDLog *****************************
 ********************************************************************************
 */
FilteredSFPutDLog::FilteredSFPutDLog (const TypeSpec* typeNameList, size_t nTypes):
	fTypeSpecs (typeNameList),
	fTypeSpecCount (nTypes),
	fCurPopupIdx (kBadIndex),
	fPopupDLGItemNumber (0),
	fMainDialog (NULL)
{
}

bool	FilteredSFPutDLog::PickFile (ConstStr255Param defaultName, StandardFileReply* result, size_t* typeIndex)
{
	Led_RequireNotNil (result);

	fMainDialog = NULL;		// in case PickFile () called multiple times

	if (typeIndex != NULL) {
		fCurPopupIdx = *typeIndex - 0 + 1;
	}

	Str255	saveAsPrompt;
	::GetIndString(saveAsPrompt, STRx_Standards, str_SaveAs);

	UDesktop::Deactivate();
	Point	where	=	{ -1, -1 };
	#if		defined (powerc)
		static	RoutineDescriptor dlgProcHook_ = BUILD_ROUTINE_DESCRIPTOR (uppDlgHookYDProcInfo, SFPutDlgHook);
		RoutineDescriptor* dlgProcHook = &dlgProcHook_;
	#else
		DlgHookYDProcPtr dlgProcHook	=	&SFPutDlgHook;
	#endif
	::CustomPutFile (saveAsPrompt, defaultName, result, 0, where, dlgProcHook, NULL, NULL, NULL, this);
	UDesktop::Activate();

	if (typeIndex != NULL) {
		*typeIndex = fCurPopupIdx-1;
	}
	return result->sfGood;
}

pascal short	FilteredSFPutDLog::SFPutDlgHook (short item, DialogPtr dialog, void* myData)
{
	FilteredSFPutDLog*	sThis	=	(FilteredSFPutDLog*)myData;

	// Must check sThis->fMainDialog==NULL cuz this same hook function gets called when SFGetFile
	// launches other SUB-Dialogs - like file is locked warning - and we don't want the popup added
	// to that dialog as well.
	if (item == sfHookFirstCall and sThis->fMainDialog == NULL) {
		sThis->fMainDialog = dialog;
		sThis->fPopupDLGItemNumber	=	Append_DITL (dialog, kSaveDLOGAdditionItems_DialogID);
		Handle	h;
		short	i;
		Rect	r;
		::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
		MenuHandle		mm	=	GetPopUpMenuHandle ((ControlHandle)h);
		for (size_t i = 0; i < sThis->fTypeSpecCount; i++) {
			Str255	tmp;
			tmp[0] = strlen (sThis->fTypeSpecs[i].fName);
			memcpy (&tmp[1], sThis->fTypeSpecs[i].fName, tmp[0]);
			::AppendMenu (mm, "\pHiMom");
			::SetMenuItemText (mm, ::CountMItems (mm), tmp);
		}
		::SetControlMinimum ((ControlHandle)h, 1);
		::SetControlMaximum ((ControlHandle)h, i);
		::SetControlValue ((ControlHandle)h,  (sThis->fCurPopupIdx == kBadIndex)? 1: sThis->fCurPopupIdx);
	}
	else if (sThis->fMainDialog == dialog) {
		Handle	h;
		short	i;
		Rect	r;
		::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
		sThis->fCurPopupIdx = ::GetControlValue ((ControlHandle)h);
	}
	return item;
}







/*
 ********************************************************************************
 ************************************** Append_DITL *****************************
 ********************************************************************************
 */
static	int	Append_DITL (DialogPtr dialog, int item_list_ID)
{
	#if PRAGMA_ALIGN_SUPPORTED
		#pragma options align=mac68k
	#endif
	 struct	DialogItem {
	    Handle  handle;		//	handle or procedure pointer for this item
	    Rect    bounds;		//	display rectangle for this item
	    char    type;		//	item type - 1
	    char    data[1];	//	length byte of data
	};
	struct ItemList {
	    short		max_index;	//	number of items - 1
	    DialogItem	items[1];	//	first item in the array
	};
	union ByteAccess {
	    short	integer;
	    char	bytes[2];
	};
	Point         	offset;
    Rect            max_rect;
    ItemList** 		append_item_list;    /* handle to DITL being appended */
    DialogItem* 	item;                /* pointer to item being appended */
    ItemList**		dlg_item_list;        /* handle to DLOG's item list */
    int           	first_item;
    int            	new_items, data_size, i;
    ByteAccess    	usb;
    OSErr          	err;
 
/*
    Using the original DLOG
 
    1. Remember the original window Size.
    2. Set the offset Point to be the bottom of the original window.
    3. Subtract 5 pixels from bottom and right, to be added
       back later after we have possibly expanded window.
    4. Get working Handle to original item list.
    5. Calculate our first item number to be returned to caller.
    6. Get locked Handle to DITL to be appended.
    7. Calculate count of new items.
*/
	Led_AssertNotNil (dialog);
 
    max_rect = ((DialogPeek)dialog)->window.port.portRect;
    offset.v = max_rect.bottom;
    offset.h = 0;
    max_rect.bottom -= 5;
    max_rect.right -= 5;
 
    dlg_item_list = (ItemList**)((DialogPeek)dialog)->items;
    first_item = (**dlg_item_list).max_index + 2;
 
    append_item_list = (ItemList**)GetResource('DITL', item_list_ID);
    if ( append_item_list == NULL )
        return first_item;
 
    HLock((Handle)append_item_list);
    new_items = (**append_item_list).max_index + 1;
 
/*
     For each item,
      1. Offset the rectangle to follow the original window.
      2. Make the original window larger if necessary.
      3. fill in item handle according to type.
*/
    item = (**append_item_list).items;
    for ( i = 0; i < new_items; i++ ) {
        OffsetRect(&item->bounds, offset.h, offset.v);
        UnionRect(&item->bounds, &max_rect, &max_rect);
        usb.integer = 0;
        usb.bytes[1] = item->data[0];
 
        switch ( item->type & 0x7F ) {
            case ctrlItem + btnCtrl :
            case ctrlItem + chkCtrl :
            case ctrlItem + radCtrl :
                item->handle = (Handle)NewControl((DialogPtr) dialog,
                                                  &item->bounds,
                                                  (StringPtr)item->data,
                                                  true,
                                                  0, 0, 1,
                                                  item->type & 0x03,
                                                  0);
            break;
 
            case ctrlItem + resCtrl :
            {
                item->handle = (Handle)GetNewControl(*(short*)(item->data + 1), (DialogPtr) dialog);
                (**(ControlHandle)item->handle).contrlRect = item->bounds;
            }
            break;

            case statText :
            case editText :
                err = PtrToHand(item->data + 1, &item->handle, usb.integer);
            break;

            case iconItem :
                item->handle = GetIcon(*(short*)(item->data + 1));
            break;
 
            case picItem :
                item->handle = (Handle)GetPicture(*(short*)(item->data + 1));
            break;
 
            default :
                item->handle = NULL;
        }
 
        data_size = (usb.integer + 1) & 0xFFFE;
        item = (DialogItem*)((char*)item + data_size + sizeof(DialogItem));
    }
 
    err = PtrAndHand((**append_item_list).items,
                     (Handle)dlg_item_list,
                     GetHandleSize((Handle) append_item_list));
    (**dlg_item_list).max_index += new_items;
    HUnlock((Handle) append_item_list);
    ReleaseResource((Handle) append_item_list);
 
    max_rect.bottom += 5;
    max_rect.right += 5;
    SizeWindow((WindowPtr) dialog, max_rect.right, max_rect.bottom, true);
 
    return first_item;
	#if PRAGMA_ALIGN_SUPPORTED
		#pragma options align=reset
	#endif
}

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

