/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../../Foundation/StroikaPreComp.h"

#include <climits>

#if qHasFeature_ATLMFC

#include <afxext.h>
#include <afxole.h>

#include "MFC.h"

#if qSupportDrawTextGetTextExtent

#include "SimpleTextImager.h"
#include "SimpleTextStore.h"
#include "WordWrappedTextImager.h"

#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;

#if qLedAssertsDefaultToMFCAsserts && qDebug
static class OneTimeLedMFCAssertionFunctionSetter {
public:
    static void MFCAssertionHandler (const char* fileName, int lineNum)
    {
        ::AfxAssertFailedLine (fileName, lineNum);
    }
    OneTimeLedMFCAssertionFunctionSetter ()
    {
        Led_SetAssertionHandler (MFCAssertionHandler);
    }
} sOneTimeLedMFCAssertionFunctionSetter;
#endif

#if qLedCheckCompilerFlagsConsistency
namespace Stroika::Frameworks::Led::Platform::LedCheckCompilerFlags_Led_MFC {
    int LedCheckCompilerFlags_ (qMFCRequiresCWndLeftmostBaseClass) = qMFCRequiresCWndLeftmostBaseClass;
}
#endif

/*
 ********************************************************************************
 *********************** LedSupport class lib support ***************************
 ********************************************************************************
 */
#if qGenerateStandardMFCExceptions
static struct MFC_MODULE_INIT {
    static void DoThrowAfxThrowMemoryException () { AfxThrowMemoryException (); }
    static void DoThrowAfxThrowArchiveException () { AfxThrowArchiveException (CArchiveException::badIndex); /* best guess/message/exception I could think of*/ }
    MFC_MODULE_INIT ()
    {
        Led_Set_OutOfMemoryException_Handler (&DoThrowAfxThrowMemoryException);
        Led_Set_BadFormatDataException_Handler (&DoThrowAfxThrowArchiveException);
    }
} sMFC_MODULE_INIT;
#endif

#if qProvideLedStubsForOLEACCDLL
/*
 ********************************************************************************
 ******************************** OLEACC.DLL ************************************
 ********************************************************************************
 */
STDAPI CreateStdAccessibleObject (HWND hwnd, LONG idObject, REFIID riid, void** ppvObject)
{
    HINSTANCE oleACCDLL = ::LoadLibrary (_T ("OLEACC.dll"));
    HRESULT   hr        = E_FAIL;
    if (oleACCDLL != NULL) {
        HRESULT (WINAPI * pCreateStdAccessibleObject)
        (HWND, LONG, REFIID, void**) =
            (HRESULT (WINAPI*) (HWND, LONG, REFIID, void**)) (::GetProcAddress (oleACCDLL, "CreateStdAccessibleObject"));
        if (pCreateStdAccessibleObject != NULL) {
            hr = (pCreateStdAccessibleObject) (hwnd, idObject, riid, ppvObject);
        }
        Verify (::FreeLibrary (oleACCDLL));
    }
    return hr;
}

STDAPI AccessibleObjectFromWindow (HWND hwnd, DWORD dwId, REFIID riid, void** ppvObject)
{
    HINSTANCE oleACCDLL = ::LoadLibrary (_T ("OLEACC.dll"));
    HRESULT   hr        = E_FAIL;
    if (oleACCDLL != NULL) {
        HRESULT (WINAPI * pAccessibleObjectFromWindow)
        (HWND, DWORD, REFIID, void**) =
            (HRESULT (WINAPI*) (HWND, DWORD, REFIID, void**)) (::GetProcAddress (oleACCDLL, "AccessibleObjectFromWindow"));
        if (pAccessibleObjectFromWindow != NULL) {
            hr = (pAccessibleObjectFromWindow) (hwnd, dwId, riid, ppvObject);
        }
        Verify (::FreeLibrary (oleACCDLL));
    }
    return hr;
}

STDAPI_ (LRESULT)
LresultFromObject (REFIID riid, WPARAM wParam, LPUNKNOWN punk)
{
    HINSTANCE oleACCDLL = ::LoadLibrary (_T ("OLEACC.dll"));
    HRESULT   hr        = E_FAIL;
    if (oleACCDLL != NULL) {
        LRESULT (WINAPI * pLresultFromObject)
        (REFIID, WPARAM, LPUNKNOWN) =
            (LRESULT (WINAPI*) (REFIID, WPARAM, LPUNKNOWN)) (::GetProcAddress (oleACCDLL, "LresultFromObject"));
        if (pLresultFromObject != NULL) {
            hr = (pLresultFromObject) (riid, wParam, punk);
        }
        Verify (::FreeLibrary (oleACCDLL));
    }
    return hr;
}
#endif

/*
 ********************************************************************************
 ******************** Led_MFCReaderDAndDFlavorPackage ***************************
 ********************************************************************************
 */
Led_MFCReaderDAndDFlavorPackage::Led_MFCReaderDAndDFlavorPackage (COleDataObject* dataObject)
    : ReaderFlavorPackage ()
    , fDataObject (dataObject)
{
    RequireNotNull (dataObject);
}

bool Led_MFCReaderDAndDFlavorPackage::GetFlavorAvailable (Led_ClipFormat clipFormat) const
{
    AssertNotNull (fDataObject);
    return fDataObject->IsDataAvailable (clipFormat);
}

size_t Led_MFCReaderDAndDFlavorPackage::GetFlavorSize (Led_ClipFormat clipFormat) const
{
    // is there some more efficeint way todo this? - LGP 960410
    HGLOBAL hObj = fDataObject->GetGlobalData (clipFormat);
    if (hObj == NULL) {
        // Some D&D sources - e.g. "Character Map" application on WinXP - return NULL for GetGlobalData()
        // UNLESS you pass along the full FORMATC. No idea why... SPR#1146
        FORMATETC formatC;
        fDataObject->BeginEnumFormats ();
        while (fDataObject->GetNextFormat (&formatC)) {
            if (formatC.cfFormat == clipFormat) {
                hObj = fDataObject->GetGlobalData (clipFormat, &formatC);
            }
        }
    }
    if (hObj == NULL) {
        return 0;
    }
    size_t result = ::GlobalSize (hObj);
    ::GlobalFree (hObj);
    return result;
}

size_t Led_MFCReaderDAndDFlavorPackage::ReadFlavorData (Led_ClipFormat clipFormat, size_t bufSize, void* buf) const
{
    HGLOBAL hObj = fDataObject->GetGlobalData (clipFormat);
    if (hObj == NULL) {
        // Some D&D sources - e.g. "Character Map" application on WinXP - return NULL for GetGlobalData()
        // UNLESS you pass along the full FORMATC. No idea why... SPR#1146
        FORMATETC formatC;
        fDataObject->BeginEnumFormats ();
        while (fDataObject->GetNextFormat (&formatC)) {
            if (formatC.cfFormat == clipFormat) {
                hObj = fDataObject->GetGlobalData (clipFormat, &formatC);
            }
        }
    }
    size_t sizeCopied = 0;
    if (hObj != NULL) {
        size_t realSize = ::GlobalSize (hObj);
        sizeCopied      = min (realSize, bufSize);
        void* p         = ::GlobalLock (hObj);
        if (p != nullptr and sizeCopied != 0) {
            (void)::memcpy (buf, p, sizeCopied);
        }
        ::GlobalUnlock (hObj);
        ::GlobalFree (hObj);
    }
    Ensure (sizeCopied <= bufSize);
    return sizeCopied;
}

COleDataObject* Led_MFCReaderDAndDFlavorPackage::GetOleDataObject () const
{
    return fDataObject;
}

/*
 ********************************************************************************
 ******************** Led_MFCWriterDAndDFlavorPackage ***************************
 ********************************************************************************
 */
Led_MFCWriterDAndDFlavorPackage::Led_MFCWriterDAndDFlavorPackage (COleDataSource* dataObject)
    : WriterFlavorPackage ()
    , fDataObject (dataObject)
{
    RequireNotNull (dataObject);
}

void Led_MFCWriterDAndDFlavorPackage::AddFlavorData (Led_ClipFormat clipFormat, size_t bufSize, const void* buf)
{
    HGLOBAL dataHandle = ::GlobalAlloc (GMEM_DDESHARE, bufSize);
    Led_ThrowIfNull (dataHandle);
    char* lockedMem = (char*)::GlobalLock (dataHandle);
    AssertNotNull (lockedMem);
    (void)::memcpy (lockedMem, buf, bufSize);
    ::GlobalUnlock (lockedMem);
    fDataObject->CacheGlobalData (clipFormat, dataHandle);

    // Note we needn't delete 'dataHandle' cuz puts the handle into a
    // stgMedium, and on desctuction of ole data source, it calls stgfree(or some such)
    // which appears to delte the global handle...
    // LGP 960412
}

COleDataSource* Led_MFCWriterDAndDFlavorPackage::GetOleDataSource () const
{
    return fDataObject;
}

/*
 ********************************************************************************
 ***************************** MFC_CommandNumberMapping *************************
 ********************************************************************************
 */
MFC_CommandNumberMapping::MFC_CommandNumberMapping ()
{
    AddAssociation (ID_EDIT_UNDO, TextInteractor::kUndo_CmdID);
    AddAssociation (ID_EDIT_REDO, TextInteractor::kRedo_CmdID);
    AddAssociation (ID_EDIT_SELECT_ALL, TextInteractor::kSelectAll_CmdID);
    AddAssociation (ID_EDIT_CUT, TextInteractor::kCut_CmdID);
    AddAssociation (ID_EDIT_COPY, TextInteractor::kCopy_CmdID);
    AddAssociation (ID_EDIT_PASTE, TextInteractor::kPaste_CmdID);
    AddAssociation (ID_EDIT_CLEAR, TextInteractor::kClear_CmdID);
    AddAssociation (ID_EDIT_FIND, TextInteractor::kFind_CmdID);
}

/*
 ********************************************************************************
 ****************************** Led_MFC_TmpCmdUpdater ***************************
 ********************************************************************************
 */
Led_MFC_TmpCmdUpdater::CommandNumber Led_MFC_TmpCmdUpdater::GetCmdID () const
{
    return fCmdNum;
}

bool Led_MFC_TmpCmdUpdater::GetEnabled () const
{
    return fEnabled;
}

void Led_MFC_TmpCmdUpdater::SetEnabled (bool enabled)
{
    EnsureNotNull (fCmdUI);
    fCmdUI->Enable (enabled);
    fEnabled = enabled;
}

void Led_MFC_TmpCmdUpdater::SetChecked (bool checked)
{
    EnsureNotNull (fCmdUI);
    fCmdUI->SetCheck (checked);
}

void Led_MFC_TmpCmdUpdater::SetText (const Led_SDK_Char* text)
{
    EnsureNotNull (fCmdUI);
    fCmdUI->SetText (text);
}

#if qSupportDrawTextGetTextExtent
/*
@METHOD:        Led_GetTextExtent
@DESCRIPTION:   <p>Instantiate @'Led_GetTextExtent<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>.
        <p>See also @'qSupportDrawTextGetTextExtent',
    @'Led_DrawText',
    @'Led_GetTextExtent<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>'.</p>
*/
CSize Led_GetTextExtent (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
{
    return Led_GetTextExtent<WordWrappedTextImager, SimpleTextImager, SimpleTextStore> (cdc, text, r, wordWrap);
}

/*
@METHOD:        Led_DrawText
@DESCRIPTION:   <p>Instantiate @'Led_DrawText<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>.</p>
        <p>See also @'qSupportDrawTextGetTextExtent',
    @'Led_GetTextExtent',
    @'Led_DrawText<WORDWRAPPEDTEXTIMAGER,SIMPLETEXTIMAGER,TEXTSTORE>'.</p>
*/
void Led_DrawText (CDC* cdc, const Led_tString& text, CRect r, bool wordWrap)
{
    Led_DrawText<WordWrappedTextImager, SimpleTextImager, SimpleTextStore> (cdc, text, r, wordWrap);
}
#endif

#endif
