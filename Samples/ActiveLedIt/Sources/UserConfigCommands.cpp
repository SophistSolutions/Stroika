/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#pragma warning(push)
#pragma warning(disable : 5054)
#include <afxctl.h>
#pragma warning(pop)

#include <atlbase.h>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/Format.h"

#include "FontMenu.h"

#include "UserConfigCommands.h"

#define CATCH_AND_HANDLE_EXCEPTIONS()     \
    catch (HRESULT hr)                    \
    {                                     \
        return hr;                        \
    }                                     \
    catch (Win32ErrorException & we)      \
    {                                     \
        return (HRESULT_FROM_WIN32 (we)); \
    }                                     \
    catch (HRESULTErrorException & h)     \
    {                                     \
        return static_cast<HRESULT> (h);  \
    }                                     \
    catch (...)                           \
    {                                     \
        return DISP_E_EXCEPTION;          \
    }

namespace {
    template <class EnumType, class CollType>
    HRESULT CreateSTLEnumerator (IUnknown** ppUnk, IUnknown* pUnkForRelease, CollType& collection)
    {
        if (ppUnk == NULL)
            return E_POINTER;
        *ppUnk = NULL;

        CComObject<EnumType>* pEnum = NULL;
        HRESULT               hr    = CComObject<EnumType>::CreateInstance (&pEnum);

        if (FAILED (hr))
            return hr;

        hr = pEnum->Init (pUnkForRelease, collection);

        if (SUCCEEDED (hr))
            hr = pEnum->QueryInterface (ppUnk);

        if (FAILED (hr))
            delete pEnum;

        return hr;
    }
}

namespace {
    template <typename INT_NAME_GRABBER>
    void DoRemove (vector<CComPtr<IDispatch>>* list, VARIANT eltIntNameOrIndex)
    {
        CComVariant e2r (eltIntNameOrIndex);
        if (SUCCEEDED (e2r.ChangeType (VT_UI4))) {
            size_t idx = e2r.ulVal;
            if (idx >= list->size ()) {
                Led_ThrowIfErrorHRESULT (DISP_E_MEMBERNOTFOUND);
            }
            list->erase (list->begin () + idx, list->begin () + idx + 1);
        }
        else if (SUCCEEDED (e2r.ChangeType (VT_BSTR))) {
            for (vector<CComPtr<IDispatch>>::iterator i = list->begin (); i != list->end (); ++i) {
                if (wstring (INT_NAME_GRABBER (*i)) == wstring (e2r.bstrVal)) {
                    list->erase (i, i + 1);
                    break;
                }
            }
        }
        else if (SUCCEEDED (e2r.ChangeType (VT_DISPATCH))) {
            for (vector<CComPtr<IDispatch>>::iterator i = list->begin (); i != list->end (); ++i) {
                if ((*i).p == e2r.pdispVal) {
                    list->erase (i, i + 1);
                    break;
                }
            }
        }
    }
    template <typename INT_NAME_GRABBER>
    size_t DoFindIndex (vector<CComPtr<IDispatch>>* list, VARIANT eltIntNameOrObj)
    {
        for (vector<CComPtr<IDispatch>>::iterator i = list->begin (); i != list->end (); ++i) {
            CComVariant nameV (eltIntNameOrObj);
            if (SUCCEEDED (nameV.ChangeType (VT_BSTR))) {
                if (wstring (INT_NAME_GRABBER (*i)) == wstring (nameV.bstrVal)) {
                    return i - list->begin ();
                }
            }
            else if (SUCCEEDED (nameV.ChangeType (VT_DISPATCH))) {
                if ((*i).p == nameV.pdispVal) {
                    return i - list->begin ();
                }
            }
        }
        return kBadIndex;
    }
}

namespace {
    struct CMD_NAME_GRABBER {
        CMD_NAME_GRABBER (IDispatch* p)
            : fDisp (p)
        {
        }
        operator wstring ()
        {
            CComQIPtr<IALCommand> c = fDisp;
            CComBSTR              name;
            Led_ThrowIfErrorHRESULT (c->get_InternalName (&name));
            return wstring (name);
        }
        CComPtr<IDispatch> fDisp;
    };

    struct ACCEL_NAME_GRABBER {
        ACCEL_NAME_GRABBER (IDispatch* p)
            : fDisp (p)
        {
        }
        operator wstring ()
        {
            CComQIPtr<IALAcceleratorElement> c = fDisp;
            CComBSTR                         name;
            Led_ThrowIfErrorHRESULT (c->get_CommandInternalName (&name));
            return wstring (name);
        }
        CComPtr<IDispatch> fDisp;
    };
}

namespace {
    // Used as an implemenation detail for CComEnumOnSTL<> invocation
    struct STL_ATL_COPY_VARIANT_IDISPATCH {
        static HRESULT copy (VARIANT* p1, const ATL::CComPtr<IDispatch>* p2)
        {
            if (p2 != NULL and *p2 != NULL) {
                p1->vt         = VT_DISPATCH;
                p1->pdispVal   = *p2;
                IDispatch* pp2 = *p2;
                pp2->AddRef ();
                return S_OK;
            }
            else {
                p1->vt = VT_EMPTY;
                return E_FAIL;
            }
        }
        static void init (VARIANT* p)
        {
            p->vt = VT_EMPTY;
        }
        static void destroy (VARIANT* p)
        {
            ::VariantClear (p);
        }
    };
}

namespace {
    wstring NormalizeCmdNameToInternal (const wstring& cmdName)
    {
        wstring result = cmdName;
    ReStart:
        for (wstring::iterator i = result.begin (); i != result.end (); ++i) {
            // erase spaces, and punctuation (except dash/underbar)
            if (iswspace (*i) or (iswpunct (*i) and *i != '-' and *i != '_')) {
                result.erase (i, i + 1);
                goto ReStart;
            }
        }
        return result;
    }
}

namespace {
    const string kFontNameCMDPrefix = "FontName";
}

/*
 ********************************************************************************
 ****************************** AL_CommandListHelper ****************************
 ********************************************************************************
 */
UserCommandNameNumberRegistry* UserCommandNameNumberRegistry::sThis;

UserCommandNameNumberRegistry& UserCommandNameNumberRegistry::Get ()
{
    if (sThis == NULL) {
        sThis = new UserCommandNameNumberRegistry ();
    }
    return *sThis;
}

UserCommandNameNumberRegistry::UserCommandNameNumberRegistry ()
    : fName2Num ()
    , fNum2Name ()
    , fNextUserCmdNum (kFirstOLEUserCmdCmdID)
{
}

UINT UserCommandNameNumberRegistry::Enter (const wstring& internalName)
{
    UINT cmdNum = 0;
    if (not Lookup (internalName, &cmdNum)) {
        fName2Num.insert (map<wstring, UINT>::value_type (internalName, fNextUserCmdNum));
        fNum2Name.insert (map<UINT, wstring>::value_type (fNextUserCmdNum, internalName));
        cmdNum = fNextUserCmdNum;
        fNextUserCmdNum++;
        if (fNextUserCmdNum > kLastOLEUserCmdCmdID) {
            Assert (false); // not a good thing - should deal with this more gracefully... probably wont ever happen - LGP 2004-01-21
            fNextUserCmdNum = kFirstOLEUserCmdCmdID;
        }
    }
    return cmdNum;
}

bool UserCommandNameNumberRegistry::Lookup (UINT cmdNum, wstring* internalName)
{
    map<UINT, wstring>::const_iterator i = fNum2Name.find (cmdNum);
    if (i == fNum2Name.end ()) {
        return false;
    }
    else {
        *internalName = (*i).second;
        return true;
    }
}

bool UserCommandNameNumberRegistry::Lookup (const wstring& internalName, UINT* cmdNum)
{
    map<wstring, UINT>::const_iterator i = fName2Num.find (internalName);
    if (i == fName2Num.end ()) {
        return false;
    }
    else {
        *cmdNum = (*i).second;
        return true;
    }
}

wstring UserCommandNameNumberRegistry::Lookup (UINT cmdNum)
{
    wstring name;
    [[maybe_unused]] bool r = Lookup (cmdNum, &name);
    Assert (r);
    return name;
}

/*
 ********************************************************************************
 ************************* ActiveLedIt_CurrentEventArguments ********************
 ********************************************************************************
 */
ActiveLedIt_CurrentEventArguments::ActiveLedIt_CurrentEventArguments ()
    : fInternalName ()
    , fEnabled (false)
    , fChecked (false)
    , fName ()
{
}

ActiveLedIt_CurrentEventArguments::~ActiveLedIt_CurrentEventArguments ()
{
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::get_InternalCommandName (BSTR* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = CComBSTR (fInternalName.c_str ()).Detach ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::put_InternalCommandName (BSTR val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fInternalName = val;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::get_Enabled (VARIANT_BOOL* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fEnabled;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::put_Enabled (VARIANT_BOOL val)
{
    fEnabled = !!val;
    return S_OK;
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::get_Checked (VARIANT_BOOL* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fChecked;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::put_Checked (VARIANT_BOOL val)
{
    fChecked = !!val;
    return S_OK;
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::get_Name (BSTR* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = CComBSTR (fName.c_str ()).Detach ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_CurrentEventArguments::put_Name (BSTR val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fName = val;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

HRESULT ActiveLedIt_CurrentEventArguments::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_CurrentEventArguments::FinalRelease ()
{
}

/*
 ********************************************************************************
 ****************************** AL_CommandListHelper ****************************
 ********************************************************************************
 */
AL_CommandListHelper::AL_CommandListHelper ()
    : fOwnedItems ()
{
}

AL_CommandListHelper::~AL_CommandListHelper ()
{
}

STDMETHODIMP AL_CommandListHelper::GeneratePopupMenu (IDispatch* acceleratorTable, HMENU* val)
{
    try {
        if (val == NULL) {
            return E_INVALIDARG;
        }
        HMENU hMenu = ::CreatePopupMenu ();
        for (vector<CComPtr<IDispatch>>::iterator i = fOwnedItems.begin (); i != fOwnedItems.end (); ++i) {
            CComQIPtr<IALCommand> alc = *i;
            CComBSTR              cmdName;
            Led_ThrowIfErrorHRESULT (alc->get_Name (&cmdName));
            Led_ThrowIfErrorHRESULT (alc->AppendSelfToMenu (hMenu, acceleratorTable));
        }
        *val = hMenu;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP AL_CommandListHelper::LookupCommand (BSTR internalName, IDispatch** val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        *val = NULL;
        for (vector<CComPtr<IDispatch>>::iterator i = fOwnedItems.begin (); i != fOwnedItems.end (); ++i) {
            CComQIPtr<IALCommand> alc = *i;
            CComBSTR              iName;
            Led_ThrowIfErrorHRESULT (alc->get_InternalName (&iName));
            if (iName == internalName) {
                *val = *i;
                AssertNotNull (*val);
                (*val)->AddRef ();
                return S_OK;
            }
            CComQIPtr<IALCommandList> alcl = *i;
            if (alcl.p != NULL) {
                // If its a popup menu - recurse
                Led_ThrowIfErrorHRESULT (alcl->LookupCommand (internalName, val));
                if (*val != NULL) {
                    // then were done - found in a submenu - so return at this point
                    return S_OK;
                }
            }
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP AL_CommandListHelper::IndexOf (VARIANT internalNameOrObject, UINT* val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        *val = DoFindIndex<CMD_NAME_GRABBER> (&fOwnedItems, internalNameOrObject);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP AL_CommandListHelper::get__NewEnum (IUnknown** ppUnk)
{
    using VarVarEnum = CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, STL_ATL_COPY_VARIANT_IDISPATCH, std::vector<CComPtr<IDispatch>>>;
    if (ppUnk == NULL) {
        return E_INVALIDARG;
    }
    try {
        return CreateSTLEnumerator<VarVarEnum> (ppUnk, this, fOwnedItems);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP AL_CommandListHelper::get_Item (long Index, IDispatch** pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    if (Index < 0 or static_cast<size_t> (Index) >= fOwnedItems.size ()) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fOwnedItems[Index];
        (*pVal)->AddRef ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP AL_CommandListHelper::get_Count (long* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fOwnedItems.size ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

void AL_CommandListHelper::AppendBuiltinCmd (const BuiltinCmdSpec& cmdSpec)
{
    fOwnedItems.push_back (ActiveLedIt_BuiltinCommand::mk (cmdSpec));
}

void AL_CommandListHelper::AppendBuiltinCmds (const BuiltinCmdSpec* cmdSpecsStart, const BuiltinCmdSpec* cmdSpecsEnd)
{
    for (const BuiltinCmdSpec* i = cmdSpecsStart; i != cmdSpecsEnd; ++i) {
        AppendBuiltinCmd (*i);
    }
}

/*
 ********************************************************************************
 *************************** AL_UserCommandListHelper ***************************
 ********************************************************************************
 */
AL_UserCommandListHelper::AL_UserCommandListHelper ()
{
}

AL_UserCommandListHelper::~AL_UserCommandListHelper ()
{
}

STDMETHODIMP AL_UserCommandListHelper::Insert (IDispatch* newElt, UINT afterElt)
{
    try {
        if (newElt == NULL) {
            return E_INVALIDARG;
        }
        size_t idx = min (static_cast<size_t> (afterElt), fOwnedItems.size ());
        fOwnedItems.insert (fOwnedItems.begin () + idx, newElt);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP AL_UserCommandListHelper::Remove (VARIANT eltIntNameOrIndex)
{
    try {
        DoRemove<CMD_NAME_GRABBER> (&fOwnedItems, eltIntNameOrIndex);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP AL_UserCommandListHelper::Clear ()
{
    try {
        fOwnedItems.clear ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

/*
 ********************************************************************************
 ***************************** ActiveLedIt_UserCommand **************************
 ********************************************************************************
 */
ActiveLedIt_UserCommand::ActiveLedIt_UserCommand ()
{
    fCommandNumber = UserCommandNameNumberRegistry::Get ().Enter (fInternalName);
}

ActiveLedIt_UserCommand::~ActiveLedIt_UserCommand ()
{
}

HRESULT ActiveLedIt_UserCommand::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_UserCommand::FinalRelease ()
{
}

STDMETHODIMP ActiveLedIt_UserCommand::put_Name (BSTR val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fName = val;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_UserCommand::put_InternalName (BSTR val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fInternalName  = val;
        fCommandNumber = UserCommandNameNumberRegistry::Get ().Enter (fInternalName);
        ;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

/*
 ********************************************************************************
 ************************ ActiveLedIt_AcceleratorElement ************************
 ********************************************************************************
 */
ActiveLedIt_AcceleratorElement::ActiveLedIt_AcceleratorElement ()
    : fCommandInternalName ()
    , fAcceleratorModifierFlag (static_cast<AcceleratorModifierFlag> (0))
    , fKey (0)
{
}

HRESULT ActiveLedIt_AcceleratorElement::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_AcceleratorElement::FinalRelease ()
{
}

STDMETHODIMP ActiveLedIt_AcceleratorElement::get_CommandInternalName (BSTR* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = CComBSTR (fCommandInternalName.c_str ()).Detach ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_AcceleratorElement::put_CommandInternalName (BSTR val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fCommandInternalName = val;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_AcceleratorElement::get_ModifierFlag (AcceleratorModifierFlag* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fAcceleratorModifierFlag;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_AcceleratorElement::put_ModifierFlag (AcceleratorModifierFlag val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fAcceleratorModifierFlag = val;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_AcceleratorElement::get_Key (WORD* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fKey;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_AcceleratorElement::put_Key (WORD val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fKey = val;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

/*
 ********************************************************************************
 ************************ ActiveLedIt_AcceleratorTable **************************
 ********************************************************************************
 */
ActiveLedIt_AcceleratorTable::ActiveLedIt_AcceleratorTable ()
    : fAccelerators ()
{
}

ActiveLedIt_AcceleratorTable::~ActiveLedIt_AcceleratorTable ()
{
}

HRESULT ActiveLedIt_AcceleratorTable::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_AcceleratorTable::FinalRelease ()
{
}

void ActiveLedIt_AcceleratorTable::AppendACCEL (const char* internalCmdName, AcceleratorModifierFlag modifierFlag, WORD key)
{
    CComObject<ActiveLedIt_AcceleratorElement>* ae = NULL;
    Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_AcceleratorElement>::CreateInstance (&ae));
    CComQIPtr<IDispatch> result = ae->GetUnknown ();
    ae->put_CommandInternalName (CComBSTR (internalCmdName));
    ae->put_ModifierFlag (modifierFlag);
    ae->put_Key (key);
    Add (ae);
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::get__NewEnum (IUnknown** ppUnk)
{
    using VarVarEnum = CComEnumOnSTL<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, STL_ATL_COPY_VARIANT_IDISPATCH, std::vector<CComPtr<IDispatch>>>;
    if (ppUnk == NULL) {
        return E_INVALIDARG;
    }
    try {
        return CreateSTLEnumerator<VarVarEnum> (ppUnk, this, fAccelerators);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::get_Item (long Index, IDispatch** pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    if (Index < 0 or static_cast<size_t> (Index) >= fAccelerators.size ()) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fAccelerators[Index];
        (*pVal)->AddRef ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::get_Count (long* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = fAccelerators.size ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::Lookup (BSTR cmdInternalName, IDispatch** pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    if (cmdInternalName == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = NULL;
        for (vector<CComPtr<IDispatch>>::const_iterator i = fAccelerators.begin (); i != fAccelerators.end (); ++i) {
            CComQIPtr<IALAcceleratorElement> ae = *i;
            CComBSTR                         itsInternalCmdName;
            Led_ThrowIfErrorHRESULT (ae->get_CommandInternalName (&itsInternalCmdName));
            if (CComBSTR (cmdInternalName) == itsInternalCmdName) {
                *pVal = *i;
                (*pVal)->AddRef ();
                return S_OK;
            }
        }
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::Add (IDispatch* newElt, UINT atIndex)
{
    if (newElt == NULL) {
        return E_INVALIDARG;
    }
    try {
        size_t idx = min (static_cast<size_t> (atIndex), fAccelerators.size ());
        fAccelerators.insert (fAccelerators.begin () + idx, newElt);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::Remove (VARIANT eltIntNameOrIndex)
{
    try {
        DoRemove<ACCEL_NAME_GRABBER> (&fAccelerators, eltIntNameOrIndex);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::Clear ()
{
    try {
        fAccelerators.clear ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_AcceleratorTable::GenerateWin32AcceleratorTable (HACCEL* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        CComQIPtr<IALCommandList> builtins = GenerateBuiltinCommandsObject ();

        Memory::SmallStackBuffer<ACCEL> accels (fAccelerators.size ());
        size_t                          goodKeysFound = 0;
        for (vector<CComPtr<IDispatch>>::const_iterator i = fAccelerators.begin (); i != fAccelerators.end (); ++i) {
            CComQIPtr<IALAcceleratorElement> ae = *i;
            CComBSTR                         internalCmdName;
            Led_ThrowIfErrorHRESULT (ae->get_CommandInternalName (&internalCmdName));
            UINT cmdNum = 0;
            if (cmdNum == 0) {
                // See if its in the builtin command list
                long bicc = 0;
                builtins->get_Count (&bicc);
                for (size_t ii = 0; ii < static_cast<size_t> (bicc); ++ii) {
                    CComPtr<IDispatch> e;
                    Led_ThrowIfErrorHRESULT (builtins->get_Item (ii, &e));
                    CComQIPtr<IALCommand> alc = e;
                    CComBSTR              bicCmdName;
                    Led_ThrowIfErrorHRESULT (alc->get_InternalName (&bicCmdName));
                    if (bicCmdName == internalCmdName) {
                        // trick to extract CMD# from the element
                        HMENU hMenu = ::CreatePopupMenu ();
                        Led_ThrowIfErrorHRESULT (alc->AppendSelfToMenu (hMenu, NULL));
                        cmdNum = ::GetMenuItemID (hMenu, 0);
                        ::DestroyMenu (hMenu);
                    }
                }
            }
            if (cmdNum == 0) {
                // See if its a user-command...
                (void)UserCommandNameNumberRegistry::Get ().Lookup (wstring (internalCmdName), &cmdNum);
            }

            // If we have a cmd# - then add it....
            if (cmdNum != 0) {
                AcceleratorModifierFlag modfierFlag;
                Led_ThrowIfErrorHRESULT (ae->get_ModifierFlag (&modfierFlag));
                WORD key;
                Led_ThrowIfErrorHRESULT (ae->get_Key (&key));
                accels[goodKeysFound].fVirt = static_cast<BYTE> (modfierFlag);
                accels[goodKeysFound].key   = key;
                Assert (cmdNum <= numeric_limits<WORD>::max ());
                accels[goodKeysFound].cmd = static_cast<WORD> (cmdNum);
                goodKeysFound++;
            }
            *pVal = ::CreateAcceleratorTable (accels, goodKeysFound);
        }
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

/*
 ********************************************************************************
 ************************* ActiveLedIt_StaticCommandList ************************
 ********************************************************************************
 */
ActiveLedIt_StaticCommandList::ActiveLedIt_StaticCommandList ()
{
}

ActiveLedIt_StaticCommandList::~ActiveLedIt_StaticCommandList ()
{
}

void ActiveLedIt_StaticCommandList::Append (IDispatch* p)
{
    fOwnedItems.push_back (p);
}

HRESULT ActiveLedIt_StaticCommandList::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_StaticCommandList::FinalRelease ()
{
}

/*
 ********************************************************************************
 *************************** ActiveLedIt_UserCommandList ************************
 ********************************************************************************
 */
ActiveLedIt_UserCommandList::ActiveLedIt_UserCommandList ()
{
}

ActiveLedIt_UserCommandList::~ActiveLedIt_UserCommandList ()
{
}

HRESULT ActiveLedIt_UserCommandList::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_UserCommandList::FinalRelease ()
{
}

/*
 ********************************************************************************
 *************************** ActiveLedIt_MenuItemPopup **************************
 ********************************************************************************
 */
ActiveLedIt_MenuItemPopup::ActiveLedIt_MenuItemPopup ()
{
}

ActiveLedIt_MenuItemPopup::~ActiveLedIt_MenuItemPopup ()
{
}

HRESULT ActiveLedIt_MenuItemPopup::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_MenuItemPopup::FinalRelease ()
{
}

STDMETHODIMP ActiveLedIt_MenuItemPopup::Insert (IDispatch* newElt, UINT afterElt)
{
    if (newElt == NULL) {
        return E_INVALIDARG;
    }
    try {
        size_t idx = min (static_cast<size_t> (afterElt), fOwnedItems.size ());
        fOwnedItems.insert (fOwnedItems.begin () + idx, newElt);
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_MenuItemPopup::Remove (VARIANT eltIntNameOrIndex)
{
    try {
        DoRemove<CMD_NAME_GRABBER> (&fOwnedItems, eltIntNameOrIndex);
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP ActiveLedIt_MenuItemPopup::Clear ()
{
    try {
        fOwnedItems.clear ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP ActiveLedIt_MenuItemPopup::AppendSelfToMenu (HMENU menu, IDispatch* acceleratorTable)
{
    try {
        HMENU subMenu = NULL;
        Led_ThrowIfErrorHRESULT (GeneratePopupMenu (acceleratorTable, &subMenu));
        ::AppendMenu (menu, MF_POPUP, reinterpret_cast<UINT_PTR> (subMenu), Led_Wide2SDKString (L"&" + fName).c_str ());
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

/*
 ********************************************************************************
 ********************************* AL_CommandHelper *****************************
 ********************************************************************************
 */
AL_CommandHelper::AL_CommandHelper ()
    : fName ()
    , fInternalName ()
    , fCommandNumber (0)
{
}

AL_CommandHelper::~AL_CommandHelper ()
{
}

STDMETHODIMP AL_CommandHelper::get_Name (BSTR* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = CComBSTR (fName.c_str ()).Detach ();
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
    return S_OK;
}

STDMETHODIMP AL_CommandHelper::get_InternalName (BSTR* pVal)
{
    if (pVal == NULL) {
        return E_INVALIDARG;
    }
    try {
        *pVal = CComBSTR (fInternalName.c_str ()).Detach ();
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP AL_CommandHelper::AppendSelfToMenu (HMENU menu, IDispatch* acceleratorTable)
{
    // MUST CALL SOME NEW - AS YET UNDEFEIND - MEMBER OF ACCELTABLE TO GET STRING FROM PRIVATE CMD NAME
    try {
        wstring suffix;
        if (acceleratorTable) {
            CComQIPtr<IALAcceleratorTable> at = acceleratorTable;
            if (at.p != NULL) {
                CComPtr<IDispatch> accelElt;
                Led_ThrowIfErrorHRESULT (at->Lookup (CComBSTR (fInternalName.c_str ()), &accelElt));
                if (accelElt.p != NULL) {
                    CComQIPtr<IALAcceleratorElement> accelerator = accelElt;

                    AcceleratorModifierFlag modFlag = static_cast<AcceleratorModifierFlag> (0);
                    Led_ThrowIfErrorHRESULT (accelerator->get_ModifierFlag (&modFlag));
                    WORD key = 0;
                    Led_ThrowIfErrorHRESULT (accelerator->get_Key (&key));

                    if (modFlag & eVIRTKEY) {
                        suffix += L"\t";
                    }
                    if (modFlag & eCONTROL) {
                        suffix += L"Ctrl+";
                    }
                    if (modFlag & eALT) {
                        suffix += L"Alt+";
                    }
                    if (modFlag & eSHIFT) {
                        suffix += L"Shift+";
                    }
                    if (modFlag & eVIRTKEY) {
                        if (VK_F1 <= key and key <= VK_F12) {
                            suffix += Led_SDKString2Wide (Characters::CString::Format (Led_SDK_TCHAROF ("F%d"), key + 1 - VK_F1));
                        }
                        else if (key == VK_SUBTRACT) {
                            suffix += L"Num-";
                        }
                        else if (key == VK_ADD) {
                            suffix += L"Num+";
                        }
                        else if (key == VK_RETURN) {
                            suffix += L"Enter";
                        }
                        else if (key == VK_BACK) {
                            suffix += L"Backspace";
                        }
                        else if (key == VK_TAB) {
                            suffix += L"Tab";
                        }
                        else if (key == VK_DELETE) {
                            suffix += L"Delete";
                        }
                        else if (key == VK_INSERT) {
                            suffix += L"Insert";
                        }
                        else if ('A' <= key and key <= 'Z') {
                            suffix.push_back (key);
                        }
                        else {
                            suffix += L"(Unknown key)";
                        }
                    }
                }
            }
        }
        ::AppendMenu (menu, fCommandNumber == 0 ? MF_SEPARATOR : MF_STRING, fCommandNumber, Led_Wide2SDKString ((fCommandNumber == 0 ? L"" : L"&") + fName + suffix).c_str ());
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

/*
 ********************************************************************************
 ******************************* AL_UserCommandHelper ***************************
 ********************************************************************************
 */
AL_UserCommandHelper::AL_UserCommandHelper ()
{
}

AL_UserCommandHelper::~AL_UserCommandHelper ()
{
}

STDMETHODIMP AL_UserCommandHelper::put_Name (BSTR val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fName = val;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

STDMETHODIMP AL_UserCommandHelper::put_InternalName (BSTR val)
{
    if (val == NULL) {
        return E_INVALIDARG;
    }
    try {
        fInternalName = val;
        return S_OK;
    }
    CATCH_AND_HANDLE_EXCEPTIONS ()
}

/*
 ********************************************************************************
 **************************** ActiveLedIt_BuiltinCommand ************************
 ********************************************************************************
 */
ActiveLedIt_BuiltinCommand::ActiveLedIt_BuiltinCommand ()
{
}

ActiveLedIt_BuiltinCommand::~ActiveLedIt_BuiltinCommand ()
{
}

ActiveLedIt_BuiltinCommand* ActiveLedIt_BuiltinCommand::mk (const BuiltinCmdSpec& cmdSpec)
{
    // Careful to return object with ZERO ref count (like CreateInstance) - so person calling this guy - will
    // be in balance when he first grabs a ref! So - be careful about catching excpetions while filling in detail
    // on object.
    CComObject<ActiveLedIt_BuiltinCommand>* o = NULL;
    Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_BuiltinCommand>::CreateInstance (&o));
    try {
        o->fName          = ACP2WideString (cmdSpec.fCmdName);
        o->fInternalName  = NormalizeCmdNameToInternal (ACP2WideString (cmdSpec.fInternalCmdName));
        o->fCommandNumber = cmdSpec.fCmdNum;
        return o;
    }
    catch (...) {
        delete o;
        throw;
    }
}

HRESULT ActiveLedIt_BuiltinCommand::FinalConstruct ()
{
    return S_OK;
}

void ActiveLedIt_BuiltinCommand::FinalRelease ()
{
}

/*
 ********************************************************************************
 ********************************** mkFontNameCMDName ***************************
 ********************************************************************************
 */
string mkFontNameCMDName (const Led_SDK_String& fontName)
{
    return kFontNameCMDPrefix + Led_SDKString2ANSI (fontName);
}

/*
 ********************************************************************************
 **************************** GenerateBuiltinCommandsObject *********************
 ********************************************************************************
 */
CComPtr<IDispatch> GenerateBuiltinCommandsObject ()
{
    CComPtr<IDispatch>                         builtinCmds;
    CComObject<ActiveLedIt_StaticCommandList>* o = NULL;
    Led_ThrowIfErrorHRESULT (CComObject<ActiveLedIt_StaticCommandList>::CreateInstance (&o));
    builtinCmds = o;

    o->AppendBuiltinCmds (kAllCmds, kAllCmds + NEltsOf (kAllCmds));
    {
        const vector<Led_SDK_String>& fontNames = GetUsableFontNames ();
        Assert (fontNames.size () <= kLastFontNameCmd - kBaseFontNameCmd + 1);
        for (size_t i = 0; i < fontNames.size (); i++) {
            int cmdNum = kBaseFontNameCmd + i;
            if (cmdNum > kLastFontNameCmd) {
                break; // asserted out before above - now just ignore extra font names...
            }
            ActiveLedIt_BuiltinCommand* c = ActiveLedIt_BuiltinCommand::mk (BuiltinCmdSpec (cmdNum, mkFontNameCMDName (fontNames[i]).c_str ()));
            c->SetName (Led_SDKString2Wide (fontNames[i]));
            o->Append (c);
        }
    }
    return builtinCmds;
}

/*
 ********************************************************************************
 *********************************** CmdObjOrName2Num ***************************
 ********************************************************************************
 */
UINT CmdObjOrName2Num (const VARIANT& cmdObjOrName)
{
    CComVariant c (cmdObjOrName);
    if (SUCCEEDED (c.ChangeType (VT_BSTR))) {
        wstring lookForCmdName = c.bstrVal == NULL ? wstring () : wstring (c.bstrVal);
        for (const BuiltinCmdSpec* i = kAllCmds; i != kAllCmds + NEltsOf (kAllCmds); ++i) {
            if (NormalizeCmdNameToInternal (ACP2WideString ((*i).fInternalCmdName)) == lookForCmdName) {
                return (*i).fCmdNum;
            }
        }

        if (lookForCmdName.length () > kFontNameCMDPrefix.length () and lookForCmdName.substr (0, kFontNameCMDPrefix.length ()) == ACP2WideString (kFontNameCMDPrefix)) {
            const vector<Led_SDK_String>& fontNames = GetUsableFontNames ();
            Assert (fontNames.size () <= kLastFontNameCmd - kBaseFontNameCmd + 1);
            for (size_t i = 0; i < fontNames.size (); i++) {
                int cmdNum = kBaseFontNameCmd + i;
                if (cmdNum > kLastFontNameCmd) {
                    break; // asserted out before above - now just ignore extra font names...
                }
                if (lookForCmdName == NormalizeCmdNameToInternal (ACP2WideString (mkFontNameCMDName (fontNames[i])))) {
                    return cmdNum;
                }
            }
        }

        // If not found in builtin cmd list - maybe a user command?
        UINT uCmdNum = 0;
        if (UserCommandNameNumberRegistry::Get ().Lookup (c.bstrVal, &uCmdNum)) {
            return uCmdNum;
        }
    }
    else if (SUCCEEDED (c.ChangeType (VT_DISPATCH))) {
        CComQIPtr<IALCommand> bicc = c.pdispVal;
        if (bicc.p == 0) {
            return 0;
        }
        else {
            HMENU hMenu = ::CreatePopupMenu ();
            try {
                Led_ThrowIfErrorHRESULT (bicc->AppendSelfToMenu (hMenu, NULL));
                UINT cmdNum = ::GetMenuItemID (hMenu, 0);
                ::DestroyMenu (hMenu);
                return static_cast<WORD> (cmdNum);
            }
            catch (...) {
                ::DestroyMenu (hMenu);
                throw;
            }
        }
    }
    return 0;
}

/*
 ********************************************************************************
 *********************************** CmdNum2Name ********************************
 ********************************************************************************
 */
wstring CmdNum2Name (UINT cmdNum)
{
    // regular builtin CMD
    for (const BuiltinCmdSpec* i = kAllCmds; i != kAllCmds + NEltsOf (kAllCmds); ++i) {
        if ((*i).fCmdNum == cmdNum) {
            return NormalizeCmdNameToInternal (ACP2WideString ((*i).fInternalCmdName));
        }
    }

    // Dynamic font name command
    if (kBaseFontNameCmd <= cmdNum and cmdNum <= kLastFontNameCmd) {
        const vector<Led_SDK_String>& fontNames = GetUsableFontNames ();
        size_t                        i         = cmdNum - kBaseFontNameCmd;
        i                                       = max (fontNames.size () - 1, i);
        i                                       = max (fontNames.size (), i);
        return NormalizeCmdNameToInternal (ACP2WideString (mkFontNameCMDName (fontNames[i])));
    }

    // USER command
    wstring userCmdIntName;
    if (UserCommandNameNumberRegistry::Get ().Lookup (cmdNum, &userCmdIntName)) {
        return userCmdIntName;
    }

    return wstring (); // no such CMD found...
}
