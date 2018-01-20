/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#ifndef __FilteredFIlePicker_h__
#define __FilteredFIlePicker_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <Dialogs.h>
#include <Navigation.h>
#include <StandardFile.h>

#include <LDocApplication.h>

#include "Stroika/Frameworks/Led/Support.h"

#include "LedItConfig.h"

#ifndef qUseNavServices
#if TARGET_CARBON
#define qUseNavServices 1
#else
#define qUseNavServices 0
#endif
#endif

class FilteredSFGetDLog {
public:
    struct TypeSpec {
        const char* fName;
        OSType      fOSType;
    };
    FilteredSFGetDLog (const TypeSpec* typeNameList, size_t nTypes);

    nonvirtual bool PickFile (FSSpec* result, bool* typeSpecified, size_t* typeIndex);

#if qUseNavServices
protected:
    static pascal void StaticNavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams, NavCallBackUserData ioUserData);
    virtual void       NavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams);
    static pascal Boolean StaticNavObjectFilterProc (AEDesc* theItem, void* info, void* callBackUD, NavFilterModes filterMode);
    virtual bool          NavObjectFilterProc (AEDesc* theItem, void* info, NavFilterModes filterMode);

protected:
    NavDialogRef fCurDialog;
#endif

protected:
#if !qUseNavServices && !TARGET_CARBON
    static pascal short SFGetDlgHook (short item, DialogPtr dialog, void* myData);
    static pascal Boolean SFFilterProc (CInfoPBPtr pb, void* myData);
#endif

    const TypeSpec* fTypeSpecs;
    size_t          fTypeSpecCount;
    size_t          fCurPopupIdx;
    size_t          fPopupDLGItemNumber;
#if !qUseNavServices && !TARGET_CARBON
    DialogPtr fMainDialog;
#endif
};

class FilteredSFPutDLog {
public:
    struct TypeSpec {
        const char* fName;
        OSType      fOSType;
    };
    FilteredSFPutDLog (const TypeSpec* typeNameList, size_t nTypes);

    nonvirtual bool PickFile (ConstStr255Param defaultName, FSSpec* result, bool* replacing, size_t* typeIndex);

#if qUseNavServices
protected:
    static pascal void StaticNavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams, NavCallBackUserData ioUserData);
    virtual void       NavEventProc (NavEventCallbackMessage inSelector, NavCBRecPtr ioParams);

protected:
    NavDialogRef fCurDialog;
#endif

protected:
#if !qUseNavServices && !TARGET_CARBON
    static pascal short SFPutDlgHook (short item, DialogPtr dialog, void* myData);
#endif

    const TypeSpec* fTypeSpecs;
    size_t          fTypeSpecCount;
    size_t          fCurPopupIdx;
    size_t          fPopupDLGItemNumber;
#if !qUseNavServices && !TARGET_CARBON
    DialogPtr fMainDialog;
#endif
};

#endif /*__FilteredFIlePicker_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
