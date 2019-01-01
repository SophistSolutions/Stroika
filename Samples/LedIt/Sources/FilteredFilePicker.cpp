/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <ControlDefinitions.h>
#include <Controls.h>
#include <Icons.h>

#include <PP_Resources.h>
#include <UDesktop.h>
#include <UExtractFromAEDesc.h>
#include <UModalDialogs.h>
#include <UStandardDialogs.h>

#include "LedItResources.h"

#include "FilteredFilePicker.h"

namespace {

#if !TARGET_CARBON
    static int Append_DITL (DialogPtr dialog, int item_list_ID);
#endif

    inline MenuHandle GetPopUpMenuHandle (ControlHandle thisControl)
    {
#if TARGET_CARBON
        return ::GetControlPopupMenuHandle (thisControl);
#else
        // See LStdPopupMenu::GetMacMenuH() to see if there is a UniversalHeaders _STRICT
        // way of doing this. There isn't as of PreCW9- LGP 960529
        Led_RequireNotNil (thisControl);
        PopupPrivateData** theMenuData = (PopupPrivateData**)(*thisControl)->contrlData;
        Led_AssertNotNil (theMenuData);
        Led_EnsureNotNil ((*theMenuData)->mHandle);
        return ((*theMenuData)->mHandle);
#endif
    }

#if qUseNavServices
    inline NavTypeListHandle CreateNavTypeList (UInt16 inNumTypes, const OSType* inSFTypeList)
    {
        NavTypeListHandle list = (NavTypeListHandle)::NewHandleClear (
            (sizeof (NavTypeList) - sizeof (OSType)) +
            (inNumTypes * sizeof (OSType)));
        Led_ThrowIfNull (list);
        (*list)->componentSignature = kNavGenericSignature;
        (*list)->osTypeCount        = inNumTypes;
        memcpy ((*list)->osType, inSFTypeList, inNumTypes * sizeof (OSType));
        return list;
    }
#endif

    // Consider moving to LedSupport?
    FSSpec CombineDirAndFileName (const FSSpec& dir, ConstStr255Param fileName)
    {
        CInfoPBRec pb;
        memset (&pb, 0, sizeof (pb));
        FSSpec tmpFSSpec     = dir;
        pb.dirInfo.ioNamePtr = tmpFSSpec.name;
        pb.dirInfo.ioVRefNum = tmpFSSpec.vRefNum;
        pb.dirInfo.ioDrDirID = tmpFSSpec.parID;
        OSErr err            = ::PBGetCatInfoSync (&pb);

        FSSpec result;

        err = FSMakeFSSpec (pb.dirInfo.ioVRefNum, pb.dirInfo.ioDrDirID, fileName, &result);
        return result;
    }
}

/*
 ********************************************************************************
 ******************************** FilteredSFGetDLog *****************************
 ********************************************************************************
 */
FilteredSFGetDLog::FilteredSFGetDLog (const TypeSpec* typeNameList, size_t nTypes)
    :
#if qUseNavServices
    fCurDialog (NULL)
    ,
#endif
    fTypeSpecs (typeNameList)
    , fTypeSpecCount (nTypes)
    , fCurPopupIdx (kBadIndex)
    , fPopupDLGItemNumber (0)
#if !qUseNavServices && !TARGET_CARBON
    , fMainDialog (NULL)
#endif
{
}

bool FilteredSFGetDLog::PickFile (FSSpec* result, bool* typeSpecified, size_t* typeIndex)
{
    Led_RequireNotNil (result);

    bool userPickGood = false;

    UDesktop::Deactivate ();

#if qUseNavServices
    NavDialogCreationOptions options;
    (void)::memset (&options, 0, sizeof (options));
    Led_ThrowIfOSStatus (::NavGetDefaultDialogCreationOptions (&options));
    // Cannot figure out how to tell it to show ALL items - things like .sit files are disabled and we don't get the chance to say otherwise in our FilterProc
    options.optionFlags |= kNavNoTypePopup;
    options.optionFlags &= ~kNavAllowMultipleFiles;
    options.optionFlags &= ~kNavAllowStationery;
    options.optionFlags &= ~kNavAllowPreviews;

    options.modality = kWindowModalityAppModal;

    Led_SmallStackBuffer<OSType> osTypeBuf (fTypeSpecCount);
    {
        for (size_t i = 0; i < fTypeSpecCount; ++i) {
            osTypeBuf[i] = fTypeSpecs[i].fOSType;
        }
    }

    NavTypeListHandle  typeList   = NULL;
    NavObjectFilterUPP filterProc = NULL;
    NavEventUPP        eventProc  = NULL;
    NavReplyRecord     replyRec;
    memset (&replyRec, 0, sizeof (replyRec));
    try {
        typeList = CreateNavTypeList (fTypeSpecCount, osTypeBuf);

        eventProc = ::NewNavEventUPP (StaticNavEventProc);
        Led_AssertNotNil (eventProc);
        filterProc = ::NewNavObjectFilterUPP (StaticNavObjectFilterProc);
        Led_AssertNotNil (filterProc);

        Led_Assert (fCurDialog == NULL);
        Led_ThrowIfOSStatus (::NavCreateGetFileDialog (&options, typeList, eventProc, NULL, filterProc, this, &fCurDialog));

        Led_ThrowIfOSStatus (::NavDialogRun (fCurDialog));

        Led_ThrowIfOSStatus (::NavDialogGetReply (fCurDialog, &replyRec));

        userPickGood = replyRec.validRecord;
        if (userPickGood) {
            if (typeSpecified != NULL) {
                *typeSpecified = (fCurPopupIdx != kBadIndex) and (fCurPopupIdx >= 4);
            }
            if (typeSpecified != NULL and *typeSpecified and typeIndex != NULL) {
                *typeIndex = fCurPopupIdx - 4 + 0;
            }
            UExtractFromAEDesc::TheFSSpec (replyRec.selection, *result);
        }
    }
    catch (...) {
        userPickGood = false;
    }
    if (replyRec.validRecord) {
        ::NavDisposeReply (&replyRec);
    }
    if (fCurDialog != NULL) {
        ::NavDialogDispose (fCurDialog);
        fCurDialog = NULL;
    }
    if (typeList != NULL) {
        ::DisposeHandle (reinterpret_cast<Handle> (typeList));
    }
    if (eventProc != NULL) {
        ::DisposeNavEventUPP (eventProc);
    }
    if (filterProc != NULL) {
        ::DisposeNavObjectFilterUPP (filterProc);
    }

#elif !TARGET_CARBON

    fMainDialog = NULL; // in case PickFile () called multiple times

    Point where = {-1, -1};
#if defined(powerc)
    static RoutineDescriptor dlgProcHook_ = BUILD_ROUTINE_DESCRIPTOR (uppDlgHookYDProcInfo, SFGetDlgHook);
    RoutineDescriptor* dlgProcHook = &dlgProcHook_;
    static RoutineDescriptor fileFilter_ = BUILD_ROUTINE_DESCRIPTOR (uppFileFilterYDProcInfo, SFFilterProc);
    RoutineDescriptor* fileFilter = &fileFilter_;
#else
    DlgHookYDProcPtr    dlgProcHook = &SFGetDlgHook;
    FileFilterYDProcPtr fileFilter  = &SFFilterProc;
#endif
    StandardFileReply sfResult;
    ::CustomGetFile (fileFilter, -1, NULL, &sfResult, 0, where, dlgProcHook, NULL, NULL, NULL, this);

    if (typeSpecified != NULL) {
        *typeSpecified = (fCurPopupIdx != kBadIndex) and (fCurPopupIdx >= 4);
    }
    if (typeSpecified != NULL and *typeSpecified and typeIndex != NULL) {
        *typeIndex = fCurPopupIdx - 4 + 0;
    }

    userPickGood = sfResult.sfGood;
    if (userPickGood) {
        *result = sfResult.sfFile;
    }

#endif

    UDesktop::Activate ();
    return userPickGood;
}

#if qUseNavServices
pascal void FilteredSFGetDLog::StaticNavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams, NavCallBackUserData ioUserData)
{
    Led_RequireNotNil (ioParams);
    Led_RequireNotNil (ioUserData);
    FilteredSFGetDLog* dlog = reinterpret_cast<FilteredSFGetDLog*> (ioUserData);
    dlog->NavEventProc (inSelector, ioParams);
}

void FilteredSFGetDLog::NavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams)
{
    Led_RequireNotNil (ioParams);
    try {
        switch (inSelector) {
            case kNavCBCustomize: {
                /*
                     *  Make room for the custom controls.
                     */
                if (ioParams->customRect.right == 0 and ioParams->customRect.bottom == 0) {
                    ioParams->customRect.right  = ioParams->customRect.left + 200;
                    ioParams->customRect.bottom = ioParams->customRect.top + 20;
                }
            } break;
            case kNavCBStart: {
                /*
                     *  Add the custom dialog items (popup).
                     */
                {
                    Handle dlogItems = ::GetResource ('DITL', kOpenDLOGAdditionItems_DialogID);
                    Led_ThrowIfOSErr (::NavCustomControl (fCurDialog, kNavCtlAddControlList, dlogItems));
                    ::ReleaseResource (dlogItems);
                }

                {
                    SInt16 numItems = 0;
                    Led_ThrowIfOSErr (::NavCustomControl (fCurDialog, kNavCtlGetFirstControlID, &numItems));
                    fPopupDLGItemNumber = numItems + 1;
                }

                DialogRef dialog = ::GetDialogFromWindow (ioParams->window);

                Handle h;
                short  i;
                Rect   r;
                ::GetDialogItem (dialog, fPopupDLGItemNumber, &i, &h, &r);
                MenuHandle mm = GetPopUpMenuHandle ((ControlHandle)h);
                for (size_t i = 0; i < fTypeSpecCount; i++) {
                    Str255 tmp;
                    tmp[0] = strlen (fTypeSpecs[i].fName);
                    memcpy (&tmp[1], fTypeSpecs[i].fName, tmp[0]);
                    AppendMenu (mm, "\pHiMom");
                    SetMenuItemText (mm, ::CountMenuItems (mm), tmp);
                }
                ::SetControlMinimum ((ControlHandle)h, 1);
                ::SetControlMaximum ((ControlHandle)h, i);
                ::SetControlValue ((ControlHandle)h, 1);
                fCurPopupIdx = 1;
            } break;
            case kNavCBEvent: {
                if (ioParams->eventData.eventDataParms.event->what == mouseDown) {
                    DialogRef dialog = ::GetDialogFromWindow (ioParams->window);
                    Handle    h;
                    short     i;
                    Rect      r;
                    ::GetDialogItem (dialog, fPopupDLGItemNumber, &i, &h, &r);
                    int newValue = ::GetControlValue ((ControlHandle)h);
                    if (newValue != fCurPopupIdx) {
                        fCurPopupIdx = newValue;
                        // Force a refresh of the listbox/explorer area based on newly chosen filter
                        Led_ThrowIfOSErr (::NavCustomControl (fCurDialog, kNavCtlBrowserRedraw, NULL));
                    }
                }
            } break;
        }
    }
    catch (...) {
    } // Can't throw back through the Toolbox
}

pascal Boolean FilteredSFGetDLog::StaticNavObjectFilterProc (AEDesc* theItem, void* info, void* callBackUD, NavFilterModes filterMode)
{
    Led_RequireNotNil (theItem);
    Led_RequireNotNil (info);
    Led_RequireNotNil (callBackUD);
    FilteredSFGetDLog* dlog = reinterpret_cast<FilteredSFGetDLog*> (callBackUD);
    return dlog->NavObjectFilterProc (theItem, info, filterMode);
}

bool FilteredSFGetDLog::NavObjectFilterProc (AEDesc* theItem, void* info, NavFilterModes /*filterMode*/)
{
    Led_RequireNotNil (theItem);
    Led_RequireNotNil (info);
    bool allow = true;
    if ((theItem->descriptorType == typeFSS) or (theItem->descriptorType == typeFSRef)) {
        NavFileOrFolderInfo* fileOrFolderInfo = reinterpret_cast<NavFileOrFolderInfo*> (info);
        if (fileOrFolderInfo->isFolder) {
            allow = true; // not sure if this should be TRUE or FALSE???
        }
        else {
            allow               = false;
            OSType thisFileType = fileOrFolderInfo->fileAndFolder.fileInfo.finderInfo.fdType;
            switch (fCurPopupIdx) {
                case 1: {
                    // TRUE iff in list of types...
                    for (size_t i = 0; i < fTypeSpecCount; i++) {
                        if (thisFileType == fTypeSpecs[i].fOSType) {
                            allow = true;
                            break;
                        }
                    }
                } break;

                case 2: {
                    allow = true; // ALL
                } break;

                default: {
                    // true only if type matches THIS entry
                    Led_Assert (fCurPopupIdx >= 4);
                    if (thisFileType == fTypeSpecs[fCurPopupIdx - 4].fOSType) {
                        allow = true;
                    }
                } break;
            }
        }
    }
    return allow;
}
#endif

#if !qUseNavServices && !TARGET_CARBON
pascal short FilteredSFGetDLog::SFGetDlgHook (short item, DialogPtr dialog, void* myData)
{
    FilteredSFGetDLog* sThis = (FilteredSFGetDLog*)myData;

    // Must check sThis->fMainDialog==NULL cuz this same hook function gets called when SFGetFile
    // launches other SUB-Dialogs - like file is locked warning - and we don't want the popup added
    // to that dialog as well.
    if (item == sfHookFirstCall and sThis->fMainDialog == NULL) {
        sThis->fMainDialog         = dialog;
        sThis->fPopupDLGItemNumber = Append_DITL (dialog, kOpenDLOGAdditionItems_DialogID);
        Handle h;
        short  i;
        Rect   r;
        ::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
        MenuHandle mm = GetPopUpMenuHandle ((ControlHandle)h);
        for (size_t i = 0; i < sThis->fTypeSpecCount; i++) {
            Str255 tmp;
            tmp[0] = strlen (sThis->fTypeSpecs[i].fName);
            memcpy (&tmp[1], sThis->fTypeSpecs[i].fName, tmp[0]);
            AppendMenu (mm, "\pHiMom");
            SetMenuItemText (mm, ::CountMenuItems (mm), tmp);
        }
        ::SetControlMinimum ((ControlHandle)h, 1);
        ::SetControlMaximum ((ControlHandle)h, i);
        ::SetControlValue ((ControlHandle)h, 1);
        sThis->fCurPopupIdx = 1;
    }
    else if (sThis->fMainDialog == dialog) {
        Handle h;
        short  i;
        Rect   r;
        ::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
        int newValue = ::GetControlValue ((ControlHandle)h);
        if (newValue != sThis->fCurPopupIdx) {
            sThis->fCurPopupIdx = newValue;
            return sfHookRebuildList;
        }
    }

    return item;
}

pascal Boolean FilteredSFGetDLog::SFFilterProc (CInfoPBPtr pb, void* myData)
{
    FilteredSFGetDLog* sThis = (FilteredSFGetDLog*)myData;

    Led_AssertNotNil (pb);

    // Never filter directories
    if (pb->dirInfo.ioFlAttrib & ioDirMask) {
        return false;
    }

    OSType thisFileType = pb->hFileInfo.ioFlFndrInfo.fdType;
    bool   filtered     = true;
    switch (sThis->fCurPopupIdx) {
        case 1: {
            // TRUE iff in list of types...
            for (size_t i = 0; i < sThis->fTypeSpecCount; i++) {
                if (thisFileType == sThis->fTypeSpecs[i].fOSType) {
                    filtered = false;
                    break;
                }
            }
        } break;

        case 2: {
            filtered = false; // ALL
        } break;

        default: {
            // true only if type matches THIS entry
            if (thisFileType == sThis->fTypeSpecs[sThis->fCurPopupIdx - 4].fOSType) {
                filtered = false;
            }
        } break;
    }

    return filtered;
}
#endif

/*
 ********************************************************************************
 ******************************** FilteredSFPutDLog *****************************
 ********************************************************************************
 */
FilteredSFPutDLog::FilteredSFPutDLog (const TypeSpec* typeNameList, size_t nTypes)
    :
#if qUseNavServices
    fCurDialog (NULL)
    ,
#endif
    fTypeSpecs (typeNameList)
    , fTypeSpecCount (nTypes)
    , fCurPopupIdx (kBadIndex)
    , fPopupDLGItemNumber (0)
#if !qUseNavServices && !TARGET_CARBON
    , fMainDialog (NULL)
#endif
{
}

bool FilteredSFPutDLog::PickFile (ConstStr255Param defaultName, FSSpec* result, bool* replacing, size_t* typeIndex)
{
    Led_RequireNotNil (result);
    Led_RequireNotNil (replacing);

    bool userPickGood = false;

    if (typeIndex != NULL) {
        fCurPopupIdx = *typeIndex - 0 + 1;
    }

    UDesktop::Deactivate ();

#if qUseNavServices

    NavDialogCreationOptions options;
    (void)::memset (&options, 0, sizeof (options));
    Led_ThrowIfOSStatus (::NavGetDefaultDialogCreationOptions (&options));
    // Cannot figure out how to tell it to show ALL items - things like .sit files are disabled and we don't get the chance to say otherwise in our FilterProc
    options.optionFlags |= kNavNoTypePopup;
    options.optionFlags &= ~kNavAllowMultipleFiles;
    options.optionFlags &= ~kNavAllowStationery;
    options.optionFlags &= ~kNavAllowPreviews;

    options.modality = kWindowModalityAppModal;

    Led_SmallStackBuffer<OSType> osTypeBuf (fTypeSpecCount);
    {
        for (size_t i = 0; i < fTypeSpecCount; ++i) {
            osTypeBuf[i] = fTypeSpecs[i].fOSType;
        }
    }

    NavEventUPP    eventProc = NULL;
    NavReplyRecord replyRec;
    try {
        eventProc = ::NewNavEventUPP (StaticNavEventProc);
        Led_AssertNotNil (eventProc);

        options.saveFileName = ::CFStringCreateWithPascalString (NULL, defaultName, ::CFStringGetSystemEncoding ());

        Led_Assert (fCurDialog == NULL);
        Led_ThrowIfOSStatus (::NavCreatePutFileDialog (&options, 'TEXT', kApplicationSignature, eventProc, this, &fCurDialog));

        Led_ThrowIfOSStatus (::NavDialogRun (fCurDialog));

        Led_ThrowIfOSStatus (::NavDialogGetReply (fCurDialog, &replyRec));

        userPickGood = replyRec.validRecord;
        if (userPickGood) {
            *replacing = replyRec.replacing;

            FSSpec folderSpec;
            UExtractFromAEDesc::TheFSSpec (replyRec.selection, folderSpec);

            /*
             *  Wierd thing - but for NavCreatePutFileDialog - NavDialogGetReply appears to just return the parent
             *  directory for the chosen file. I saw some reference to this in the MWERKS/Apple header files, but nowhere
             *  in the actual docs on Apples website. Sigh... LGP 2003-03-12
             */
            Str255 tmpFileName;
            tmpFileName[0] = 0; // in case below fails...
            ::CFStringGetPascalString (replyRec.saveFileName, tmpFileName, sizeof (tmpFileName), ::CFStringGetSystemEncoding ());
            *result = CombineDirAndFileName (folderSpec, tmpFileName);
        }
    }
    catch (...) {
        userPickGood = false;
    }
    if (replyRec.validRecord) {
        ::NavDisposeReply (&replyRec);
    }
    if (fCurDialog != NULL) {
        ::NavDialogDispose (fCurDialog);
        fCurDialog = NULL;
    }
    if (eventProc != NULL) {
        ::DisposeNavEventUPP (eventProc);
    }

#elif !TARGET_CARBON

    fMainDialog = NULL; // in case PickFile () called multiple times

    Str255 saveAsPrompt;
    ::GetIndString (saveAsPrompt, STRx_Standards, str_SaveAs);

    Point where = {-1, -1};
#if defined(powerc)
    static RoutineDescriptor dlgProcHook_ = BUILD_ROUTINE_DESCRIPTOR (uppDlgHookYDProcInfo, SFPutDlgHook);
    RoutineDescriptor* dlgProcHook = &dlgProcHook_;
#else
    DlgHookYDProcPtr    dlgProcHook = &SFPutDlgHook;
#endif
    StandardFileReply sfResult;
    ::CustomPutFile (saveAsPrompt, defaultName, &sfResult, 0, where, dlgProcHook, NULL, NULL, NULL, this);

    userPickGood = sfResult.sfGood;
    if (userPickGood) {
        *result = sfResult.sfFile;
        *replacing = sfResult.sfReplacing;
    }

#endif

    UDesktop::Activate ();

    if (typeIndex != NULL) {
        *typeIndex = fCurPopupIdx - 1;
    }

    return userPickGood;
}

#if qUseNavServices
pascal void FilteredSFPutDLog::StaticNavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams, NavCallBackUserData ioUserData)
{
    Led_RequireNotNil (ioParams);
    Led_RequireNotNil (ioUserData);
    FilteredSFPutDLog* dlog = reinterpret_cast<FilteredSFPutDLog*> (ioUserData);
    dlog->NavEventProc (inSelector, ioParams);
}

void FilteredSFPutDLog::NavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams)
{
    Led_RequireNotNil (ioParams);
    try {
        switch (inSelector) {
            case kNavCBCustomize: {
                /*
                     *  Make room for the custom controls.
                     */
                if (ioParams->customRect.right == 0 and ioParams->customRect.bottom == 0) {
                    ioParams->customRect.right  = ioParams->customRect.left + 200;
                    ioParams->customRect.bottom = ioParams->customRect.top + 20;
                }
            } break;
            case kNavCBStart: {
                /*
                     *  Add the custom dialog items (popup).
                     */
                {
                    Handle dlogItems = ::GetResource ('DITL', kSaveDLOGAdditionItems_DialogID);
                    Led_ThrowIfOSErr (::NavCustomControl (fCurDialog, kNavCtlAddControlList, dlogItems));
                    ::ReleaseResource (dlogItems);
                }

                {
                    SInt16 numItems = 0;
                    Led_ThrowIfOSErr (::NavCustomControl (fCurDialog, kNavCtlGetFirstControlID, &numItems));
                    fPopupDLGItemNumber = numItems + 1;
                }

                DialogRef dialog = ::GetDialogFromWindow (ioParams->window);

                Handle h;
                short  i;
                Rect   r;
                ::GetDialogItem (dialog, fPopupDLGItemNumber, &i, &h, &r);
                MenuHandle mm = GetPopUpMenuHandle ((ControlHandle)h);
                for (size_t i = 0; i < fTypeSpecCount; i++) {
                    Str255 tmp;
                    tmp[0] = strlen (fTypeSpecs[i].fName);
                    memcpy (&tmp[1], fTypeSpecs[i].fName, tmp[0]);
                    AppendMenu (mm, "\pHiMom");
                    SetMenuItemText (mm, ::CountMenuItems (mm), tmp);
                }
                ::SetControlMinimum ((ControlHandle)h, 1);
                ::SetControlMaximum ((ControlHandle)h, i);
                ::SetControlValue ((ControlHandle)h, 1);
                ::SetControlValue ((ControlHandle)h, (fCurPopupIdx == kBadIndex) ? 1 : fCurPopupIdx);
            } break;
            case kNavCBEvent: {
                if (ioParams->eventData.eventDataParms.event->what == mouseDown) {
                    DialogRef dialog = ::GetDialogFromWindow (ioParams->window);
                    Handle    h;
                    short     i;
                    Rect      r;
                    ::GetDialogItem (dialog, fPopupDLGItemNumber, &i, &h, &r);
                    int newValue = ::GetControlValue ((ControlHandle)h);
                    if (newValue != fCurPopupIdx) {
                        fCurPopupIdx = newValue;
                        // Force a refresh of the listbox/explorer area based on newly chosen filter
                        Led_ThrowIfOSErr (::NavCustomControl (fCurDialog, kNavCtlBrowserRedraw, NULL));
                    }
                }
            } break;
        }
    }
    catch (...) {
    } // Can't throw back through the Toolbox
}
#endif

#if !qUseNavServices && !TARGET_CARBON
pascal short FilteredSFPutDLog::SFPutDlgHook (short item, DialogPtr dialog, void* myData)
{
    FilteredSFPutDLog* sThis = (FilteredSFPutDLog*)myData;

    // Must check sThis->fMainDialog==NULL cuz this same hook function gets called when SFGetFile
    // launches other SUB-Dialogs - like file is locked warning - and we don't want the popup added
    // to that dialog as well.
    if (item == sfHookFirstCall and sThis->fMainDialog == NULL) {
        sThis->fMainDialog         = dialog;
        sThis->fPopupDLGItemNumber = Append_DITL (dialog, kSaveDLOGAdditionItems_DialogID);
        Handle h;
        short  i;
        Rect   r;
        ::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
        MenuHandle mm = GetPopUpMenuHandle ((ControlHandle)h);
        for (size_t i = 0; i < sThis->fTypeSpecCount; i++) {
            Str255 tmp;
            tmp[0] = strlen (sThis->fTypeSpecs[i].fName);
            memcpy (&tmp[1], sThis->fTypeSpecs[i].fName, tmp[0]);
            ::AppendMenu (mm, "\pHiMom");
            ::SetMenuItemText (mm, ::CountMenuItems (mm), tmp);
        }
        ::SetControlMinimum ((ControlHandle)h, 1);
        ::SetControlMaximum ((ControlHandle)h, i);
        ::SetControlValue ((ControlHandle)h, (sThis->fCurPopupIdx == kBadIndex) ? 1 : sThis->fCurPopupIdx);
    }
    else if (sThis->fMainDialog == dialog) {
        Handle h;
        short  i;
        Rect   r;
        ::GetDialogItem (dialog, sThis->fPopupDLGItemNumber, &i, &h, &r);
        sThis->fCurPopupIdx = ::GetControlValue ((ControlHandle)h);
    }
    return item;
}
#endif

namespace {

#if !TARGET_CARBON
    /*
     ********************************************************************************
     ********************************** Append_DITL *********************************
     ********************************************************************************
     */
    static int Append_DITL (DialogPtr dialog, int item_list_ID)
    {
#if PRAGMA_ALIGN_SUPPORTED
#pragma options align = mac68k
#endif
        struct DialogItem {
            Handle handle;  //  handle or procedure pointer for this item
            Rect   bounds;  //  display rectangle for this item
            char   type;    //  item type - 1
            char   data[1]; //  length byte of data
        };
        struct ItemList {
            short      max_index; //  number of items - 1
            DialogItem items[1];  //  first item in the array
        };
        union ByteAccess {
            short integer;
            char  bytes[2];
        };
        Point       offset;
        Rect        max_rect;
        ItemList**  append_item_list; /* handle to DITL being appended */
        DialogItem* item;             /* pointer to item being appended */
        ItemList**  dlg_item_list;    /* handle to DLOG's item list */
        int         first_item;
        int         new_items, data_size, i;
        ByteAccess  usb;
        OSErr       err;

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
        first_item    = (**dlg_item_list).max_index + 2;

        append_item_list = (ItemList**)GetResource ('DITL', item_list_ID);
        if (append_item_list == NULL)
            return first_item;

        HLock ((Handle)append_item_list);
        new_items = (**append_item_list).max_index + 1;

        /*
             For each item,
              1. Offset the rectangle to follow the original window.
              2. Make the original window larger if necessary.
              3. fill in item handle according to type.
        */
        item = (**append_item_list).items;
        for (i = 0; i < new_items; i++) {
            OffsetRect (&item->bounds, offset.h, offset.v);
            UnionRect (&item->bounds, &max_rect, &max_rect);
            usb.integer  = 0;
            usb.bytes[1] = item->data[0];

            switch (item->type & 0x7F) {
                case ctrlItem + btnCtrl:
                case ctrlItem + chkCtrl:
                case ctrlItem + radCtrl:
                    item->handle = (Handle)NewControl ((DialogPtr)dialog,
                                                       &item->bounds,
                                                       (StringPtr)item->data,
                                                       true,
                                                       0, 0, 1,
                                                       item->type & 0x03,
                                                       0);
                    break;

                case ctrlItem + resCtrl: {
                    item->handle                               = (Handle)GetNewControl (*(short*)(item->data + 1), (DialogPtr)dialog);
                    (**(ControlHandle)item->handle).contrlRect = item->bounds;
                } break;

                case statText:
                case editText:
                    err = PtrToHand (item->data + 1, &item->handle, usb.integer);
                    break;

                case iconItem:
                    item->handle = GetIcon (*(short*)(item->data + 1));
                    break;

                case picItem:
                    item->handle = (Handle)GetPicture (*(short*)(item->data + 1));
                    break;

                default:
                    item->handle = NULL;
            }

            data_size = (usb.integer + 1) & 0xFFFE;
            item      = (DialogItem*)((char*)item + data_size + sizeof (DialogItem));
        }

        err = PtrAndHand ((**append_item_list).items,
                          (Handle)dlg_item_list,
                          GetHandleSize ((Handle)append_item_list));
        (**dlg_item_list).max_index += new_items;
        HUnlock ((Handle)append_item_list);
        ReleaseResource ((Handle)append_item_list);

        max_rect.bottom += 5;
        max_rect.right += 5;
        SizeWindow ((WindowPtr)dialog, max_rect.right, max_rect.bottom, true);

        return first_item;
#if PRAGMA_ALIGN_SUPPORTED
#pragma options align = reset
#endif
    }
#endif
}
