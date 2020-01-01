/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxctl.h>
DISABLE_COMPILER_MSC_WARNING_END (5054)

#include <comcat.h>
#include <objsafe.h>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String.h"

#include "Stroika/Frameworks/Led/StyledTextEmbeddedObjects.h"

#include "ActiveLedItControl.h"
#include "Resource.h"
#include "UserConfigCommands.h"

#include "ActiveLedIt.h"

#include "ActiveLedIt_i.c"

inline const void* LoadDLLResource (long resID, LPCTSTR resType)
{
    HRSRC hrsrc = ::FindResource (::AfxGetResourceHandle (), MAKEINTRESOURCE (resID), resType);
    AssertNotNull (hrsrc);
    HGLOBAL     hglobal    = ::LoadResource (::AfxGetResourceHandle (), hrsrc);
    const void* lockedData = ::LockResource (hglobal);
    EnsureNotNull (lockedData);
    return (lockedData);
}

//  Support for registering in the ComponentCategoryManager - that we are a SAFE control, to avoid nasty warnings
//  when running embedded in IE
static HRESULT CreateComponentCategory (CATID catid, WCHAR* catDescription)
{
    ICatRegister* pcr = NULL;
    HRESULT       hr  = S_OK;

    hr = CoCreateInstance (CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if (FAILED (hr))
        return hr;

    // Make sure the HKCR\Component Categories\{..catid...}
    // key is registered.
    CATEGORYINFO catinfo;
    catinfo.catid = catid;
    catinfo.lcid  = 0x0409; // english

    // Make sure the provided description is not too long.
    // Only copy the first 127 characters if it is.
    size_t len = wcslen (catDescription);
    if (len > 127)
        len = 127;
    Characters::CString::Copy (catinfo.szDescription, len, catDescription);

    hr = pcr->RegisterCategories (1, &catinfo);
    pcr->Release ();

    return hr;
}
static HRESULT RegisterCLSIDInCategory (REFCLSID clsid, CATID catid)
{
    // Register your component categories information.
    ICatRegister* pcr = NULL;
    HRESULT       hr  = S_OK;
    hr                = CoCreateInstance (CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if (SUCCEEDED (hr)) {
        // Register this category as being "implemented" by the class.
        CATID rgcatid[1];
        rgcatid[0] = catid;
        hr         = pcr->RegisterClassImplCategories (clsid, 1, rgcatid);
    }

    if (pcr != NULL)
        pcr->Release ();

    return hr;
}
static HRESULT UnRegisterCLSIDInCategory (REFCLSID clsid, CATID catid)
{
    ICatRegister* pcr = NULL;
    HRESULT       hr  = S_OK;

    hr = CoCreateInstance (CLSID_StdComponentCategoriesMgr, NULL, CLSCTX_INPROC_SERVER, IID_ICatRegister, (void**)&pcr);
    if (SUCCEEDED (hr)) {
        // Unregister this category as being "implemented" by the class.
        CATID rgcatid[1];
        rgcatid[0] = catid;
        hr         = pcr->UnRegisterClassImplCategories (clsid, 1, rgcatid);
    }
    if (pcr != NULL)
        pcr->Release ();
    return hr;
}

/*
 ********************************************************************************
 *********************************** ATL Support ********************************
 ********************************************************************************
 */
#include <atlbase.h>

namespace {
    class ActiveLedItModule : public CAtlMfcModule { /*CAtlDllModuleT <ActiveLedItModule>*/
    public:
        DECLARE_LIBID (LIBID_ACTIVELEDITLib)
    };
    ActiveLedItModule _AtlModule;
}

/*
 ********************************************************************************
 ******************************** ActiveLedItApp ********************************
 ********************************************************************************
 */

ActiveLedItApp theApp;

const GUID CDECL BASED_CODE _tlid      = {0x9a013a72, 0xad34, 0x11d0, {0x8d, 0x9b, 0, 0xa0, 0xc9, 0x8, 0xc, 0x73}};
const WORD                  _wVerMajor = qLed_Version_Major;
const WORD                  _wVerMinor = qLed_Version_Minor;

BOOL ActiveLedItApp::InitInstance ()
{
    SetRegistryKey (_T ("Sophist Solutions, Inc."));

    BOOL bInit = inherited::InitInstance ();

    if (bInit) {
// Tell Led about the picture resources it needs to render some special embedding markers
#if !qURLStyleMarkerNewDisplayMode
        StandardURLStyleMarker::sURLPict = (const Led_DIB*)::LoadDLLResource (kURLPictID, RT_BITMAP);
#endif
        StandardUnknownTypeStyleMarker::sUnknownPict          = (const Led_DIB*)::LoadDLLResource (kUnknownEmbeddingPictID, RT_BITMAP);
        StandardMacPictureStyleMarker::sUnsupportedFormatPict = (const Led_DIB*)::LoadDLLResource (kUnsupportedPICTFormatPictID, RT_BITMAP);

        // Tell Led about the kinds of embeddings we will allow
        EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();
    }
    return bInit;
}

int ActiveLedItApp::ExitInstance ()
{
// Detect SPR#1599 a little earlier. Basic trouble with this bug is that the
// ActiveX control objects don't get destroyed - and even though the CanUnloadDLL function
// returns false - UnloadDLL is called anyhow - and this is called indirectly.
// Must find some bugfix or better bug workaround eventually. -- LGP 2003-12-16
#if qKeepListOfALInstancesForSPR_1599BWA
    while (not ActiveLedItControl::GetAll ().empty ()) {
        ActiveLedItControl* p  = *ActiveLedItControl::GetAll ().begin ();
        IUnknown*           pi = p->GetControllingUnknown ();
        while (pi->Release () > 0)
            ;
    }
#endif
    [[maybe_unused]] _AFX_THREAD_STATE* pState = AfxGetThreadState ();
    ASSERT (pState != NULL);
    Assert (pState->m_nCtrlRef == 0);
    Assert (AfxGetModuleState ()->m_nObjectCount == 0);

    return inherited::ExitInstance ();
}

/*
 ********************************************************************************
 ******************************** DllRegisterServer *****************************
 ********************************************************************************
 */
STDAPI DllRegisterServer ()
{
    AFX_MANAGE_STATE (_afxModuleAddrThis);

    if (!AfxOleRegisterTypeLib (AfxGetInstanceHandle (), _tlid))
        return ResultFromScode (SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll (TRUE))
        return ResultFromScode (SELFREG_E_CLASS);

    // Mark the control as safe for initializing.
    HRESULT hr = CreateComponentCategory (CATID_SafeForInitializing, L"Controls safely initializable from persistent data!");
    if (FAILED (hr))
        return hr;

    hr = RegisterCLSIDInCategory (CLSID_ActiveLedIt, CATID_SafeForInitializing);
    if (FAILED (hr))
        return hr;

    // Mark the control as safe for scripting.
    hr = CreateComponentCategory (CATID_SafeForScripting, L"Controls safely scriptable!");
    if (FAILED (hr))
        return hr;

    hr = RegisterCLSIDInCategory (CLSID_ActiveLedIt, CATID_SafeForScripting);
    if (FAILED (hr))
        return hr;

    return NOERROR;
}

/*
 ********************************************************************************
 ******************************** DllUnregisterServer ***************************
 ********************************************************************************
 */
//  Removes entries from the system registry
STDAPI DllUnregisterServer ()
{
    AFX_MANAGE_STATE (_afxModuleAddrThis);

    // Remove entries from the registry.
    HRESULT hr = UnRegisterCLSIDInCategory (CLSID_ActiveLedIt, CATID_SafeForInitializing);
    if (FAILED (hr))
        return hr;

    hr = UnRegisterCLSIDInCategory (CLSID_ActiveLedIt, CATID_SafeForScripting);
    if (FAILED (hr))
        return hr;

    if (!AfxOleUnregisterTypeLib (_tlid, _wVerMajor, _wVerMinor))
        return ResultFromScode (SELFREG_E_TYPELIB);

    if (!COleObjectFactoryEx::UpdateRegistryAll (FALSE))
        return ResultFromScode (SELFREG_E_CLASS);

    return NOERROR;
}
