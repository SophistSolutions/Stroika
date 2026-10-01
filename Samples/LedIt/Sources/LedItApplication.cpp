/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if defined(WIN32)

#include <afx.h>

#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/StroikaVersion.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "Stroika/Frameworks/Led/Config.h"
#include "Stroika/Frameworks/Led/StdDialogs.h"
#if qStroika_Platform_Windows
#include "Stroika/Frameworks/Led/Platform/Windows_FileRegistration.h"
#endif
#include "Stroika/Frameworks/Led/StyledTextEmbeddedObjects.h"

#if qStroika_Platform_Windows
#include "LedItControlItem.h"
#include "LedItInPlaceFrame.h"
#include "LedItMainFrame.h"
#endif

#include "LedItDocument.h"
#include "LedItResources.h"
#include "LedItView.h"
#include "Options.h"

#include "LedItApplication.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

using Memory::MakeSharedPtr;

#if qStroika_Platform_Windows && defined(__SANITIZE_ADDRESS__)
/*
 *  The common file dialog (File Open, Save As) has the Windows shell enumerate the folder on a worker thread, and shell
 *  code there (ntshrui.dll, called via windows.storage and propsys) does a memcmp that reads past the end of its own heap
 *  block. With ASan's default strict_memcmp=1 - "memcmp always reads all n bytes" - that is reported as a heap-buffer-overflow
 *  and the app dies; no LedIt code is on the stack. strict_memcmp=0 checks only up to the first difference.
 *  ASAN_OPTIONS from the environment still applies on top of this.
 */
extern "C" const char* __asan_default_options ()
{
    return "strict_memcmp=0";
}
#endif

const char kAppName[] = "LedIt";

#if qStroika_Platform_Windows

#define STD_EXCEPT_CATCHER(APP)                                                                                                            \
    catch (CMemoryException * e)                                                                                                           \
    {                                                                                                                                      \
        (APP).HandleBadAllocException ();                                                                                                  \
        e->Delete ();                                                                                                                      \
    }                                                                                                                                      \
    catch (CException * e)                                                                                                                 \
    {                                                                                                                                      \
        (APP).HandleMFCException (e);                                                                                                      \
        e->Delete ();                                                                                                                      \
    }                                                                                                                                      \
    catch (bad_alloc)                                                                                                                      \
    {                                                                                                                                      \
        (APP).HandleBadAllocException ();                                                                                                  \
    }                                                                                                                                      \
    catch (HRESULT hr)                                                                                                                     \
    {                                                                                                                                      \
        (APP).HandleHRESULTException (hr);                                                                                                 \
    }                                                                                                                                      \
    catch (TextInteractor::BadUserInput&)                                                                                                  \
    {                                                                                                                                      \
        (APP).HandleBadUserInputException ();                                                                                              \
    }                                                                                                                                      \
    catch (...)                                                                                                                            \
    {                                                                                                                                      \
        (APP).HandleUnknownException ();                                                                                                   \
    }

#endif

#if qStroika_Platform_Windows
class SimpleLedTemplate : public CSingleDocTemplate {
public:
    SimpleLedTemplate (const char* daStr);

public:
    virtual void LoadTemplate () override;

public:
    virtual CDocument* OpenDocumentFile (LPCTSTR lpszPathName, BOOL bMakeVisible) override
    {
        // Based on MFC CSingleDocTemplate::OpenDocumentFile () from MSVC.Net 2k3 (2003-11-29)
        // But changed to cope with exceptions being thrown during OpenDoc (SPR#1572)
        CDocument* pDocument    = NULL;
        CFrameWnd* pFrame       = NULL;
        BOOL       bCreated     = FALSE; // => doc and frame created
        BOOL       bWasModified = FALSE;

        if (m_pOnlyDoc != NULL) {
            // already have a document - reinit it
            pDocument = m_pOnlyDoc;
            if (!pDocument->SaveModified ())
                return NULL; // leave the original one

            pFrame = (CFrameWnd*)AfxGetMainWnd ();
            ASSERT (pFrame != NULL);
            ASSERT_KINDOF (CFrameWnd, pFrame);
            ASSERT_VALID (pFrame);
        }
        else {
            // create a new document
            pDocument = CreateNewDocument ();
            ASSERT (pFrame == NULL); // will be created below
            bCreated = TRUE;
        }

        if (pDocument == NULL) {
            AfxMessageBox (AFX_IDP_FAILED_TO_CREATE_DOC);
            return NULL;
        }
        ASSERT (pDocument == m_pOnlyDoc);

        if (pFrame == NULL) {
            ASSERT (bCreated);

            // create frame - set as main document frame
            BOOL bAutoDelete         = pDocument->m_bAutoDelete;
            pDocument->m_bAutoDelete = FALSE;
            // don't destroy if something goes wrong
            pFrame                   = CreateNewFrame (pDocument, NULL);
            pDocument->m_bAutoDelete = bAutoDelete;
            if (pFrame == NULL) {
                AfxMessageBox (AFX_IDP_FAILED_TO_CREATE_DOC);
                delete pDocument; // explicit delete on error
                return NULL;
            }
        }

        if (lpszPathName == NULL) {
            // create a new document
            SetDefaultTitle (pDocument);

            // avoid creating temporary compound file when starting up invisible
            if (!bMakeVisible)
                pDocument->m_bEmbedded = TRUE;

            if (!pDocument->OnNewDocument ()) {
                // user has been alerted to what failed in OnNewDocument
                if (bCreated)
                    pFrame->DestroyWindow (); // will destroy document
                return NULL;
            }
        }
        else {
            CWaitCursor wait;

            // open an existing document
            bWasModified = pDocument->IsModified ();
            pDocument->SetModifiedFlag (FALSE); // not dirty for open

            BOOL docOpenDocResult = false;
            try {
                docOpenDocResult = pDocument->OnOpenDocument (lpszPathName);
            }
            catch (...) {
                if (bCreated) {
                    pDocument->OnNewDocument ();
                    CWinThread* pThread = AfxGetThread ();
                    ASSERT (pThread);
                    if (bCreated && pThread->m_pMainWnd == NULL) {
                        // set as main frame (InitialUpdateFrame will show the window)
                        pThread->m_pMainWnd = pFrame;
                    }
                    InitialUpdateFrame (pFrame, pDocument, bMakeVisible);
                }
                throw;
            }

            if (!docOpenDocResult) {
                // user has been alerted to what failed in OnOpenDocument
                if (bCreated) {
                    pFrame->DestroyWindow (); // will destroy document
                }
                else if (!pDocument->IsModified ()) {
                    // original document is untouched
                    pDocument->SetModifiedFlag (bWasModified);
                }
                else {
                    // we corrupted the original document
                    SetDefaultTitle (pDocument);

                    if (!pDocument->OnNewDocument ()) {
                        // assume we can continue
                    }
                }
                return NULL; // open failed
            }
            pDocument->SetPathName (lpszPathName);
        }

        CWinThread* pThread = AfxGetThread ();
        ASSERT (pThread);
        if (bCreated && pThread->m_pMainWnd == NULL) {
            // set as main frame (InitialUpdateFrame will show the window)
            pThread->m_pMainWnd = pFrame;
        }
        InitialUpdateFrame (pFrame, pDocument, bMakeVisible);

        return pDocument;
    }
};

class LedItDocManager : public CDocManager {
public:
    LedItDocManager ();
    virtual void          OnFileNew () override;
    virtual CDocument*    OpenDocumentFile (LPCTSTR lpszFileName) override;
    nonvirtual CDocument* OpenDocumentFile (LPCTSTR lpszFileName, FileFormat format);

public:
    virtual void RegisterShellFileTypes (BOOL bWin95) override;

private:
    nonvirtual void RegisterShellFileType (bool bWin95, CString strPathName, int iconIndexInFile, CString strFilterExt,
                                           CString strFileTypeId, CString strFileTypeName);

public:
    virtual BOOL DoPromptFileName (CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate) override;

    virtual void OnFileOpen () override;
};

inline const void* LoadAppResource (long resID, LPCTSTR resType)
{
    HRSRC hrsrc = ::FindResource (::AfxGetResourceHandle (), MAKEINTRESOURCE (resID), resType);
    AssertNotNull (hrsrc);
    HGLOBAL     hglobal    = ::LoadResource (::AfxGetResourceHandle (), hrsrc);
    const void* lockedData = ::LockResource (hglobal);
    EnsureNotNull (lockedData);
    return (lockedData);
}
static BOOL AFXAPI SetRegKey (LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL)
{
    if (lpszValueName == NULL) {
        if (::RegSetValue (HKEY_CLASSES_ROOT, lpszKey, REG_SZ, lpszValue, static_cast<DWORD> (::_tcslen (lpszValue))) != ERROR_SUCCESS) {
            TRACE1 ("Warning: registration database update failed for key '%s'.\n", lpszKey);
            return FALSE;
        }
        return TRUE;
    }
    else {
        HKEY hKey;
        if (::RegCreateKey (HKEY_CLASSES_ROOT, lpszKey, &hKey) == ERROR_SUCCESS) {
            LONG lResult = ::RegSetValueEx (hKey, lpszValueName, 0, REG_SZ, (CONST BYTE*)lpszValue,
                                            static_cast<DWORD> (::_tcslen (lpszValue) + sizeof (TCHAR)));
            if (::RegCloseKey (hKey) == ERROR_SUCCESS && lResult == ERROR_SUCCESS) {
                return TRUE;
            }
        }
        TRACE1 ("Warning: registration database update failed for key '%s'.\n", lpszKey);
        return FALSE;
    }
}
#endif

class MyAboutBox : public Led_StdDialogHelper_AboutBox {
private:
    using inherited = Led_StdDialogHelper_AboutBox;
#if qStroika_Platform_Windows
public:
    MyAboutBox (HINSTANCE hInstance, HWND parentWnd)
        : inherited (hInstance, parentWnd)
    {
    }
#endif

public:
    virtual void PreDoModalHook () override
    {
        inherited::PreDoModalHook ();
#if _UNICODE
#define kUNICODE_NAME_ADORNER L" [UNICODE]"
#else
#define kUNICODE_NAME_ADORNER " [Internal UNICODE]"
#endif

#if qStroika_Platform_Windows
        // Cuz of fact that dlog sizes specified in dlog units, and that doesn't work well for bitmaps
        // we must resize our dlog on the fly based on pict resource size...
        const int kPictWidth  = 437; // must agree with ACTUAL bitmap size
        const int kPictHeight = 273;
        const int kButHSluff  = 17;
        const int kButVSluff  = 19;
        {
            RECT windowRect;
            ::GetWindowRect (GetHWND (), &windowRect);
            // figure size of non-client area...
            int ncWidth  = 0;
            int ncHeight = 0;
            {
                RECT clientRect;
                ::GetClientRect (GetHWND (), &clientRect);
                ncWidth  = AsLedRect (windowRect).GetWidth () - AsLedRect (clientRect).GetWidth ();
                ncHeight = AsLedRect (windowRect).GetHeight () - AsLedRect (clientRect).GetHeight ();
            }
            ::MoveWindow (GetHWND (), windowRect.left, windowRect.top, kPictWidth + ncWidth, kPictHeight + ncHeight, false);
        }

        // Place and fill in version information
        {
            HWND w = ::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_VersionFieldID);
            AssertNotNull (w);
            const int kVERWidth = 230;
            ::MoveWindow (w, kPictWidth / 2 - kVERWidth / 2, 32, kVERWidth, 16, false);
            ::SetWindowText (w, _T ("Stroika ") _T (qStroika_Version_ShortVersionString) kUNICODE_NAME_ADORNER _T (" (") _T (__DATE__) _T (")"));
        }

        // Place hidden buttons which map to URLs
        {
            HWND w = ::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_InfoLedFieldID);
            AssertNotNull (w);
            ::MoveWindow (w, 15, 159, 142, 17, false);
            w = ::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_LedWebPageFieldID);
            AssertNotNull (w);
            ::MoveWindow (w, 227, 159, 179, 17, false);
        }

        // Place OK button
        {
            HWND w = ::GetDlgItem (GetHWND (), IDOK);
            AssertNotNull (w);
            RECT tmp;
            ::GetWindowRect (w, &tmp);
            ::MoveWindow (w, kButHSluff, kPictHeight - AsLedRect (tmp).GetHeight () - kButVSluff, AsLedRect (tmp).GetWidth (),
                          AsLedRect (tmp).GetHeight (), false); // width/height we should presevere
        }

        ::SetWindowText (GetHWND (), _T ("About LedIt!"));
#endif
    }
    virtual void OnClickInInfoField () override
    {
        try {
            Led_URLManager::Get ().Open ("mailto:info-led@sophists.com");
        }
        catch (...) {
            // ignore for now - since errors here prent dialog from dismissing (on MacOSX)
        }
        inherited::OnClickInInfoField ();
    }

    virtual void OnClickInLedWebPageField () override
    {
        try {
            Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/LedIt/", kAppName));
        }
        catch (...) {
            // ignore for now - since errors here prent dialog from dismissing (on MacOSX)
        }
        inherited::OnClickInLedWebPageField ();
    }
};

/*
 ********************************************************************************
 ******************************** LedItApplication ******************************
 ********************************************************************************
 */
#if qStroika_Platform_Windows
LedItApplication theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
// {0FC00620-28BD-11CF-899C-00AA00580324}
static const CLSID clsid = {0xfc00620, 0x28bd, 0x11cf, {0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x24}};

BEGIN_MESSAGE_MAP (LedItApplication, CWinApp)
ON_COMMAND (ID_APP_ABOUT, OnAppAbout)
ON_COMMAND (ID_FILE_NEW, OnFileNew)
ON_COMMAND (ID_FILE_OPEN, OnFileOpen)
ON_COMMAND (ID_FILE_PRINT_SETUP, OnFilePrintSetup)
ON_COMMAND (kToggleUseSmartCutNPasteCmd, OnToggleSmartCutNPasteOptionCommand)
ON_UPDATE_COMMAND_UI (kToggleUseSmartCutNPasteCmd, OnToggleSmartCutNPasteOptionUpdateCommandUI)
ON_COMMAND (kToggleWrapToWindowCmd, OnToggleWrapToWindowOptionCommand)
ON_UPDATE_COMMAND_UI (kToggleWrapToWindowCmd, OnToggleWrapToWindowOptionUpdateCommandUI)
ON_COMMAND (kToggleShowHiddenTextCmd, OnToggleShowHiddenTextOptionCommand)
ON_UPDATE_COMMAND_UI (kToggleShowHiddenTextCmd, OnToggleShowHiddenTextOptionUpdateCommandUI)
ON_COMMAND (cmdChooseDefaultFontDialog, OnChooseDefaultFontCommand)
ON_COMMAND (kGotoLedItWebPageCmd, OnGotoLedItWebPageCommand)
ON_COMMAND (kGotoSophistsWebPageCmd, OnGotoSophistsWebPageCommand)
ON_COMMAND (kCheckForUpdatesWebPageCmdID, OnCheckForUpdatesWebPageCommand)
END_MESSAGE_MAP ()
#endif

LedItApplication* LedItApplication::sThe = NULL;

LedItApplication::LedItApplication ()
    :
#if qStroika_Platform_Windows
    inherited ()
    ,
#endif
#if qStroika_Platform_Windows
    fOleTemplateServer ()
    ,
#endif
#if qStroika_Platform_Windows
    fInstalledFonts ()
#endif
{
    Require (sThe == NULL);
    sThe = this;

/*
     *  It would be nice to be able to add all the standard types in one place, but due to the quirkly requirement (or is it
     *  just a convention) with MFC of having the application object staticly constructed, we run into a 'race condition'
     *  of sorts. Its undefined by C++ the order of static constructors across modules. As a result - some of the static
     *  consts used in EmbeddedObjectCreatorRegistry aren't yet initialized. Sigh... Perhaps I could/should find
     *  a better way to make this work, but lets KISS (keep it simple stupid) for now, as we are very close to
     *  release -- LGP 2001-10-06.
     */
#if !qStroika_Platform_Windows
    // Tell Led about the kinds of embeddings we will allow
    EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();

#if qStroika_Platform_Windows
    // Support OLE embeddings (both created from clip, and from RTF-format files)
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (LedItControlItem::kClipFormat, LedItControlItem::kEmbeddingTag,
                                                    LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (kBadClipFormat, RTFIO::RTFOLEEmbedding::kEmbeddingTag,
                                                    LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);
#endif
#endif
}

LedItApplication::~LedItApplication ()
{
    Require (sThe == this);
    sThe = NULL;
}

LedItApplication& LedItApplication::Get ()
{
    EnsureNotNull (sThe);
    return *sThe;
}

void LedItApplication::DoAboutBox ()
{
#if qStroika_Platform_Windows
    MyAboutBox dlg (m_hInstance, AfxGetMainWnd ()->m_hWnd);
#endif
    dlg.DoModal ();
}

void LedItApplication::OnGotoLedItWebPageCommand ()
{
    try {
        Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/LedIt/", kAppName));
    }
    catch (...) {
    }
}

void LedItApplication::OnGotoSophistsWebPageCommand ()
{
    try {
        Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/", kAppName));
    }
    catch (...) {
    }
}

void LedItApplication::OnCheckForUpdatesWebPageCommand ()
{
    try {
        Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName));
    }
    catch (...) {
    }
}

void LedItApplication::OnToggleSmartCutNPasteOptionCommand ()
{
    Options o;
    o.SetSmartCutAndPaste (not o.GetSmartCutAndPaste ());
    UpdateViewsForPrefsChange ();
}

void LedItApplication::OnToggleSmartCutNPasteOption_UpdateCommandUI (CMD_ENABLER* enabler)
{
    RequireNotNull (enabler);
    enabler->SetEnabled (true);
    enabler->SetChecked (Options{}.GetSmartCutAndPaste ());
}

void LedItApplication::OnToggleWrapToWindowOptionCommand ()
{
    Options o;
    o.SetWrapToWindow (not o.GetWrapToWindow ());
    UpdateViewsForPrefsChange ();
}

void LedItApplication::OnToggleWrapToWindowOption_UpdateCommandUI (CMD_ENABLER* enabler)
{
    RequireNotNull (enabler);
    enabler->SetEnabled (true);
    enabler->SetChecked (Options{}.GetWrapToWindow ());
}

void LedItApplication::OnToggleShowHiddenTextOptionCommand ()
{
    Options o;
    o.SetShowHiddenText (not o.GetShowHiddenText ());
    UpdateViewsForPrefsChange ();
}

void LedItApplication::OnToggleShowHiddenTextOption_UpdateCommandUI (CMD_ENABLER* enabler)
{
    RequireNotNull (enabler);
    enabler->SetEnabled (true);
    enabler->SetChecked (Options{}.GetShowHiddenText ());
}

void LedItApplication::UpdateViewsForPrefsChange ()
{
    bool wrapToWindow   = Options{}.GetWrapToWindow ();
    bool smartCutNPaste = Options{}.GetSmartCutAndPaste ();
    bool showHiddenText = Options{}.GetShowHiddenText ();

#if qStroika_Platform_Windows
    // Update each open view
    POSITION tp = GetFirstDocTemplatePosition ();
    while (tp != NULL) {
        CDocTemplate* t = GetNextDocTemplate (tp);
        AssertNotNull (t);
        POSITION dp = t->GetFirstDocPosition ();
        while (dp != NULL) {
            CDocument* doc = t->GetNextDoc (dp);
            AssertNotNull (doc);
            POSITION vp = doc->GetFirstViewPosition ();
            while (vp != NULL) {
                CView* v = doc->GetNextView (vp);
                AssertNotNull (v);
                LedItView* lv = dynamic_cast<LedItView*> (v);
                if (lv != NULL) {
                    lv->SetSmartCutAndPasteMode (smartCutNPaste);
                    lv->SetWrapToWindow (wrapToWindow);
                    lv->SetShowHiddenText (showHiddenText);
                }
            }
        }
    }
#endif
}

#if qStroika_Platform_Windows
BOOL LedItApplication::InitInstance ()
{
    SetRegistryKey (_T ("Sophist Solutions, Inc."));

    // Initialize OLE libraries
    if (!AfxOleInit ()) {
        AfxMessageBox (IDP_OLE_INIT_FAILED);
        return false;
    }

#if qIncludeBasicSpellcheckEngine_
#if qStroika_Foundation_Debug_AssertionsChecked
    SpellCheckEngine_Basic::RegressionTest ();
#endif

    fSpellCheckEngine = MakeSharedPtr<SpellCheckEngine_Basic_Simple> ();

#if qStroika_Platform_Windows
    {
        // Place the dictionary in a reasonable - but hardwired place. Later - allow for editing that location,
        // and other spellchecking options (see SPR#1591)
        TCHAR defaultPath[MAX_PATH + 1];
        Verify (::SHGetSpecialFolderPath (nullptr, defaultPath, CSIDL_FLAG_CREATE | CSIDL_PERSONAL, true));
        fSpellCheckEngine->SetUserDictionary (SDKString{defaultPath} + Led_SDK_TCHAROF ("\\My LedIt Dictionary.txt"));
    }
#endif

#endif

    // Tell Led about the kinds of embeddings we will allow
    EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();

    // Support OLE embeddings (both created from clip, and from RTF-format files)
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (LedItControlItem::kClipFormat, LedItControlItem::kEmbeddingTag,
                                                    LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (kBadClipFormat, RTFIO::RTFOLEEmbedding::kEmbeddingTag,
                                                    LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine (cmdInfo);

    AfxEnableControlContainer ();

#if 0
    // LGP 960509 - doesn't appear to have any effect if present or not...
    // and prevents linking on Mac (CodeWarrior x-compiler).
    Enable3dControlsStatic ();  // Call this when linking to MFC statically
#endif

    //LoadStdProfileSettings (5);  // Load standard INI file options (including MRU)
    LoadStdProfileSettings (9); // Load standard INI file options (including MRU)

    Assert (m_pDocManager == NULL);
    m_pDocManager = new LedItDocManager ();

    AddDocTemplateForString ("LedIt\n\nLedIt\nLed Rich Text Format (*.led)\n.led\nLedIt.Document\nLedIt Document", true);

    // Enable DDE Open, and register icons / file types with the explorer/shell
    EnableShellOpen ();
    RegisterShellFileTypes (true); // ARG???

    // When a server application is launched stand-alone, it is a good idea
    //  to update the system registry in case it has been damaged.
    fOleTemplateServer.UpdateRegistry (OAT_INPLACE_SERVER);
    COleObjectFactory::UpdateRegistryAll ();

    // Tell Led about the picture resources it needs to render some special embedding markers
    StandardUnknownTypeStyleMarker::sUnknownPict          = (const Led_DIB*)::LoadAppResource (kUnknownEmbeddingPictID, RT_BITMAP);
    StandardMacPictureStyleMarker::sUnsupportedFormatPict = (const Led_DIB*)::LoadAppResource (kUnsupportedPICTFormatPictID, RT_BITMAP);

#if qStroika_Platform_Windows
    {
        class MyRegistrationHelper : public Win32UIFileAssociationRegistrationHelper {
        private:
            using inherited = Win32UIFileAssociationRegistrationHelper;

        public:
            MyRegistrationHelper ()
                : inherited (::AfxGetResourceHandle ())
            {
            }

        public:
            virtual bool CheckUserSaysOKToUpdate () const override
            {
                Options o;
                if (o.GetCheckFileAssocsAtStartup ()) {
                    Led_StdDialogHelper_UpdateWin32FileAssocsDialog dlg (::AfxGetResourceHandle (), ::GetActiveWindow ());
                    dlg.fAppName      = Led_SDK_TCHAROF ("LedIt!");
                    dlg.fTypeList     = Led_SDK_TCHAROF (".rtf");
                    dlg.fKeepChecking = true;
                    bool result       = dlg.DoModal ();
                    o.SetCheckFileAssocsAtStartup (dlg.fKeepChecking);
                    return result;
                }
                else {
                    return false;
                }
            }
        };
        MyRegistrationHelper fileAssocHelper;
        SDKString            rtfDocIcon = Characters::CString::Format (Led_SDK_TCHAROF ("$EXE$,%d"), -kLedItRTFDocumentIconID);
        fileAssocHelper.Add (Win32UIFileAssociationInfo (Led_SDK_TCHAROF (".rtf"), Led_SDK_TCHAROF ("rtffile"),
                                                         Led_SDK_TCHAROF ("Rich Text Document"), rtfDocIcon, Led_SDK_TCHAROF ("$EXE$ \"%1\"")));
        fileAssocHelper.DoIt ();
    }
#endif

    // Check to see if launched as OLE server
    if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated) {
        // Register all OLE server (factories) as running.  This enables the
        //  OLE libraries to create objects from other applications.
        COleTemplateServer::RegisterAll ();

        // Application was run with /Embedding or /Automation.  Don't show the
        //  main window in this case.
        return true;
    }

    // Dispatch commands specified on the command line
    if (not ProcessShellCommand (cmdInfo)) {
        return false;
    }

    return true;
}

#if _MFC_VER >= 0x0700
void LedItApplication::WinHelpInternal ([[maybe_unused]] DWORD_PTR dwData, [[maybe_unused]] UINT nCmd)
#else
void LedItApplication::WinHelp ([[maybe_unused]] DWORD dwData, [[maybe_unused]] UINT nCmd)
#endif
{
    // get path of executable
    TCHAR directoryName[_MAX_PATH];
    Verify (::GetModuleFileName (m_hInstance, directoryName, _MAX_PATH));

    {
        LPTSTR lpszExt = _tcsrchr (directoryName, '\\');
        ASSERT (lpszExt != NULL);
        ASSERT (*lpszExt == '\\');
        *(lpszExt + 1) = '\0';
    }
    Characters::CString::Cat (directoryName, std::size (directoryName), _T ("LedItDocs\\"));

    // wrap in try/catch, and display error if no open???
    // (NB: we use .htm instead of .html cuz some old systems - I think maybe
    //  Win95 with only Netscape 2.0 installed - only have .htm registered - not
    //  .html).
    (void)::ShellExecute (NULL, _T ("open"), _T ("index.htm"), NULL, directoryName, SW_SHOWNORMAL);
}

BOOL LedItApplication::PumpMessage ()
{
    try {
        return inherited::PumpMessage ();
    }
    STD_EXCEPT_CATCHER (*this);
    return true;
}

void LedItApplication::HandleMFCException ([[maybe_unused]] CException* e) noexcept
{
    // tmp hack for now...
    HandleUnknownException ();
}

void LedItApplication::HandleHRESULTException ([[maybe_unused]] HRESULT hr) noexcept
{
    // tmp hack for now...
    HandleUnknownException ();
}

#if 0
BOOL    LedItApplication::OnIdle (LONG lCount)
{
    POSITION    tp  =   GetFirstDocTemplatePosition ();
    while (tp != NULL) {
        CDocTemplate*       t   =   GetNextDocTemplate (tp);
        AssertNotNull (t);
        POSITION    dp  =   t->GetFirstDocPosition ();
        while (dp != NULL) {
            CDocument*      doc =   t->GetNextDoc (dp);
            AssertNotNull (doc);
            POSITION    vp  =   doc->GetFirstViewPosition ();
            while (vp != NULL) {
                CView*  v   =   doc->GetNextView (vp);
                AssertNotNull (v);
                LedItView*  lv  =   dynamic_cast<LedItView*> (v);
                if (lv != NULL) {
                    lv->CallEnterIdleCallback ();
                }
            }
        }
    }
    return inherited::OnIdle (lCount);
}
#endif

BOOL LedItApplication::ProcessShellCommand (CCommandLineInfo& rCmdInfo)
{
    try {
        switch (rCmdInfo.m_nShellCommand) {
            case CCommandLineInfo::FileOpen: {
                try {
                    if (not OpenDocumentFile (rCmdInfo.m_strFileName)) {
                        throw "";
                    }
                    return true;
                }
                catch (...) {
                    // see if file just doesn't exist. If so - then create NEW DOC with that file name (preset) - but not saved
                    HANDLE h = ::CreateFile (rCmdInfo.m_strFileName, 0, 0, NULL, OPEN_EXISTING, 0, NULL);
                    if (h == INVALID_HANDLE_VALUE) {
                        OnFileNew ();

                        CDocument* newDoc = NULL;
                        {
                            POSITION tp = GetFirstDocTemplatePosition ();
                            if (tp != NULL) {
                                CDocTemplate* t = GetNextDocTemplate (tp);
                                AssertNotNull (t);
                                POSITION dp = t->GetFirstDocPosition ();
                                if (dp != NULL) {
                                    newDoc = t->GetNextDoc (dp);
                                }
                            }
                        }

                        if (newDoc != NULL) {
                            // Set path and title so if you just hit save - you are promted to save in the name you specified, but
                            // there are visual clues that this isn't the orig file
                            newDoc->SetPathName (rCmdInfo.m_strFileName, false);
                            newDoc->SetTitle (newDoc->GetTitle () + Led_SDK_TCHAROF (" {new}"));
                            return true;
                        }
                    }
                    else {
                        ::CloseHandle (h);
                    }
                    throw;
                }
            } break;
            default: {
                return inherited::ProcessShellCommand (rCmdInfo);
            }
        }
    }
    STD_EXCEPT_CATCHER (*this);
    return false;
}

void LedItApplication::AddDocTemplateForString (const char* tmplStr, bool connectToServer)
{
    CSingleDocTemplate* pDocTemplate = new SimpleLedTemplate (tmplStr);
    pDocTemplate->SetContainerInfo (IDR_CNTR_INPLACE);
    pDocTemplate->SetServerInfo (IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE, RUNTIME_CLASS (LedItInPlaceFrame));
    AddDocTemplate (pDocTemplate);
    if (connectToServer) {
        // Connect the COleTemplateServer to the document template.
        //  The COleTemplateServer creates new documents on behalf
        //  of requesting OLE containers by using information
        //  specified in the document template.
        fOleTemplateServer.ConnectTemplate (clsid, pDocTemplate, true);
        // Note: SDI applications register server objects only if /Embedding
        //   or /Automation is present on the command line.
    }
}

void LedItApplication::OnAppAbout ()
{
    DoAboutBox ();
}

void LedItApplication::OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
    OnToggleSmartCutNPasteOption_UpdateCommandUI (CMD_ENABLER (pCmdUI));
}

void LedItApplication::OnToggleWrapToWindowOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
    OnToggleWrapToWindowOption_UpdateCommandUI (CMD_ENABLER (pCmdUI));
}

void LedItApplication::OnToggleShowHiddenTextOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
    OnToggleShowHiddenTextOption_UpdateCommandUI (CMD_ENABLER (pCmdUI));
}
#endif

void LedItApplication::OnChooseDefaultFontCommand ()
{
#if qStroika_Platform_Windows
    FontSpecification fsp = Options{}.GetDefaultNewDocFont ();

    LOGFONT lf;
    (void)::memset (&lf, 0, sizeof (lf));
    {
        Characters::CString::Copy (lf.lfFaceName, std::size (lf.lfFaceName), fsp.GetFontNameSpecifier ().fName);
        Assert (::_tcslen (lf.lfFaceName) < sizeof (lf.lfFaceName)); // cuz our cached entry - if valid - always short enuf...
    }
    lf.lfWeight    = (fsp.GetStyle_Bold ()) ? FW_BOLD : FW_NORMAL;
    lf.lfItalic    = (fsp.GetStyle_Italic ());
    lf.lfUnderline = (fsp.GetStyle_Underline ());
    lf.lfStrikeOut = (fsp.GetStyle_Strikeout ());

    lf.lfHeight = fsp.PeekAtTMHeight ();

    CFontDialog dlog (&lf);
    if (dlog.DoModal () == IDOK) {
        Options{}.SetDefaultNewDocFont (FontSpecification (*dlog.m_cf.lpLogFont));
    }
#else
    Led_Assert (false); // NYI
#endif
}

void LedItApplication::HandleBadAllocException () noexcept
{
    try {
#if qStroika_Platform_Windows
        CDialog errorDialog (kBadAllocExceptionOnCmdDialogID);
        errorDialog.DoModal ();
#else
        HandleUnknownException ();
#endif
    }
    catch (...) {
        Led_BeepNotify ();
    }
}

void LedItApplication::HandleBadUserInputException () noexcept
{
    try {
#if qStroika_Platform_Windows
        CDialog errorDialog (kBadUserInputExceptionOnCmdDialogID);
        errorDialog.DoModal ();
#else
        HandleUnknownException ();
#endif
    }
    catch (...) {
        Led_BeepNotify ();
    }
}

void LedItApplication::HandleUnknownException () noexcept
{
    try {
#if qStroika_Platform_Windows
        CDialog errorDialog (kUnknownExceptionOnCmdDialogID);
        errorDialog.DoModal ();
#endif
    }
    catch (...) {
        Led_BeepNotify ();
    }
}

#if qStroika_Platform_Windows
const vector<SDKString>& LedItApplication::GetUsableFontNames ()
{
    return fInstalledFonts.GetUsableFontNames ();
}

void LedItApplication::FixupFontMenu (CMenu* fontMenu)
{
    AssertMember (fontMenu, CMenu);
    const vector<SDKString>& fontNames = GetUsableFontNames ();

    // delete all menu items
    while (fontMenu->DeleteMenu (0, MF_BYPOSITION) != 0) {
        ;
    }

    for (UINT i = 0; i < fontNames.size (); ++i) {
        UINT cmdNum = kBaseFontNameCmd + i;
        fontMenu->AppendMenu (MF_STRING, cmdNum, fontNames[i].c_str ());
    }
}

SDKString LedItApplication::CmdNumToFontName (UINT cmdNum)
{
    const vector<SDKString>& fontNames = GetUsableFontNames ();
    return (fontNames[cmdNum - kBaseFontNameCmd]);
}
#endif

#if qStroika_Platform_Windows
/*
 ********************************************************************************
 ******************************** LedItDocManager *******************************
 ********************************************************************************
 */
SimpleLedTemplate::SimpleLedTemplate (const char* daStr)
    : CSingleDocTemplate (IDR_MAINFRAME, RUNTIME_CLASS (LedItDocument), RUNTIME_CLASS (LedItMainFrame), RUNTIME_CLASS (LedItView))
{
    m_strDocStrings = daStr;
}

void SimpleLedTemplate::LoadTemplate ()
{
    bool doMenuEmbedding     = (m_hMenuEmbedding == NULL);
    bool doMenuInPlaceServer = (m_hMenuInPlaceServer == NULL);
    bool doMenuInPlace       = (m_hMenuInPlace == NULL);
    CSingleDocTemplate::LoadTemplate ();

    // Now go and fixup the font menu...
    if (doMenuEmbedding and m_hMenuEmbedding != NULL) {
        // Not understood well just what each of these are, but I think this
        // contains a format menu.
        // LGP 960101
        CMenu tmp;
        tmp.Attach (m_hMenuEmbedding);
        LedItApplication::Get ().FixupFontMenu (tmp.GetSubMenu (4)->GetSubMenu (0));
        tmp.Detach ();
    }
    if (doMenuInPlaceServer and m_hMenuInPlaceServer != NULL) {
        // Not understood well just what each of these are, but I think this
        // contains a format menu.
        // LGP 960101
        CMenu tmp;
        tmp.Attach (m_hMenuInPlaceServer);
        LedItApplication::Get ().FixupFontMenu (tmp.GetSubMenu (3)->GetSubMenu (0));
        tmp.Detach ();
    }
    if (doMenuInPlace and m_hMenuInPlace != NULL) {
        // Not understood well just what each of these are, but I don't think this
        // contains a format menu
    }
}
#endif

#if qStroika_Platform_Windows
/*
 ********************************************************************************
 ******************************** LedItDocManager *******************************
 ********************************************************************************
 */
// Some private, but externed, MFC 4.0 routines needed to subclass CDocManager... See below...
// LGP 960222
extern BOOL AFXAPI AfxFullPath (LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
extern BOOL AFXAPI AfxResolveShortcut (CWnd* pWnd, LPCTSTR pszShortcutFile, LPTSTR pszPath, int cchPath);
extern void AFXAPI AfxGetModuleShortFileName (HINSTANCE hInst, CString& strShortName);

LedItDocManager::LedItDocManager ()
    : CDocManager ()
{
}

void LedItDocManager::OnFileNew ()
{
    // tmp hack - for now always pick the first - later this guy will own all refs
    // and just use RIGHT (namely LedDoc) one...
    CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead ();
    ASSERT (pTemplate != NULL);
    ASSERT_KINDOF (CDocTemplate, pTemplate);
    (void)pTemplate->OpenDocumentFile (NULL);
}

CDocument* LedItDocManager::OpenDocumentFile (LPCTSTR lpszFileName)
{
    return OpenDocumentFile (lpszFileName, eUnknownFormat);
}

inline SDKString GetLongPathName (const SDKString& pathName)
{
    TCHAR szPath[_MAX_PATH];
    Require (pathName.length () < _MAX_PATH);
    Characters::CString::Copy (szPath, std::size (szPath), pathName.c_str ());
    WIN32_FIND_DATA fileData;
    HANDLE          hFind = ::FindFirstFile (szPath, &fileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        TCHAR* lastSlash = ::_tcsrchr (szPath, '\\');
        if (lastSlash != NULL) {
            *lastSlash = '\0';
        }
        Characters::CString::Cat (szPath, std::size (szPath), _T ("\\"));
        Characters::CString::Cat (szPath, std::size (szPath), fileData.cFileName);
        szPath[_MAX_PATH - 1] = '\0';
        VERIFY (::FindClose (hFind));
    }
    return szPath;
}
CDocument* LedItDocManager::OpenDocumentFile (LPCTSTR lpszFileName, FileFormat format)
{
    // Lifted from CDocManager::OpenDocumentFile() with a few changes to not keep
    // separate lists of templates. Only reason I create multiple templates is to
    // get the popup in the open/save dialogs to appear.
    // Sigh!
    // LGP 960222
    // PLUS SEE BELOW - LGP 960522
    CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead ();
    ASSERT (pTemplate != NULL);
    ASSERT_KINDOF (CDocTemplate, pTemplate);

    TCHAR szPath[_MAX_PATH];
    ASSERT (::_tcslen (lpszFileName) < _MAX_PATH);
    TCHAR szTemp[_MAX_PATH];
    if (lpszFileName[0] == '\"')
        ++lpszFileName;
    Characters::CString::Copy (szTemp, std::size (szTemp), lpszFileName);
    LPTSTR lpszLast = _tcsrchr (szTemp, '\"');
    if (lpszLast != NULL)
        *lpszLast = 0;
    AfxFullPath (szPath, szTemp);
    TCHAR szLinkName[_MAX_PATH];
    if (AfxResolveShortcut (AfxGetMainWnd (), szPath, szLinkName, _MAX_PATH))
        Characters::CString::Copy (szPath, std::size (szPath), szLinkName);

    // Also, to fix SPR#0345, we must use this (or SHGetFileInfo) hack
    // to get the long-file-name version of the file name.
    Characters::CString::Copy (szPath, std::size (szPath), GetLongPathName (szPath).c_str ());

    LedItDocument::sHiddenDocOpenArg = format;
    return (pTemplate->OpenDocumentFile (szPath));
}

void LedItDocManager::RegisterShellFileTypes (BOOL bWin95)
{
    // Cloned from base CWinApp version, but we don't use Doc templates

    CString strPathName, strTemp;

    AfxGetModuleShortFileName (AfxGetInstanceHandle (), strPathName);

    // Only do for .led file now???
    // In future maybe ask user if I should do .txt, etc??? And any others I support?
    RegisterShellFileType (bWin95, strPathName, 1, ".led", "LedIt.Document", "LedIt Document");
}

void LedItDocManager::RegisterShellFileType (bool bWin95, CString strPathName, int iconIndexInFile, CString strFilterExt,
                                             CString strFileTypeId, CString strFileTypeName)
{
    static const TCHAR szShellOpenFmt[]    = _T("%s\\shell\\open\\%s");
    static const TCHAR szShellPrintFmt[]   = _T("%s\\shell\\print\\%s");
    static const TCHAR szShellPrintToFmt[] = _T("%s\\shell\\printto\\%s");
    static const TCHAR szDefaultIconFmt[]  = _T("%s\\DefaultIcon");
    static const TCHAR szShellNewFmt[]     = _T("%s\\ShellNew");
    static const TCHAR szIconIndexFmt[]    = _T(",%d");
    static const TCHAR szCommand[]         = _T("command");
    static const TCHAR szOpenArg[]         = _T(" \"%1\"");
    static const TCHAR szPrintArg[]        = _T(" /p \"%1\"");
    static const TCHAR szPrintToArg[]      = _T(" /pt \"%1\" \"%2\" \"%3\" \"%4\"");

    static const TCHAR szShellNewValueName[] = _T("NullFile");
    static const TCHAR szShellNewValue[]     = _T("");

    const int DEFAULT_ICON_INDEX = 0;

    CString strOpenCommandLine        = strPathName;
    CString strPrintCommandLine       = strPathName;
    CString strPrintToCommandLine     = strPathName;
    CString strDefaultIconCommandLine = strPathName;

    if (bWin95) {
        CString strIconIndex;
        HICON   hIcon = ::ExtractIcon (AfxGetInstanceHandle (), strPathName, iconIndexInFile);
        if (hIcon != NULL) {
            strIconIndex.Format (szIconIndexFmt, iconIndexInFile);
            ::DestroyIcon (hIcon);
        }
        else {
            strIconIndex.Format (szIconIndexFmt, DEFAULT_ICON_INDEX);
        }
        strDefaultIconCommandLine += strIconIndex;
    }

    if (!strFileTypeId.IsEmpty ()) {
        CString strTemp;

        // enough info to register it
        if (strFileTypeName.IsEmpty ()) {
            strFileTypeName = strFileTypeId; // use id name
        }

        ASSERT (strFileTypeId.Find (' ') == -1); // no spaces allowed

        // first register the type ID with our server
        if (!SetRegKey (strFileTypeId, strFileTypeName)) {
            return; // just skip it
        }

        if (bWin95) {
            // path\DefaultIcon = path,1
            strTemp.Format (szDefaultIconFmt, (LPCTSTR)strFileTypeId);
            if (!SetRegKey (strTemp, strDefaultIconCommandLine)) {
                return; // just skip it
            }
        }

        // We are an SDI application...
        // path\shell\open\command = path filename
        // path\shell\print\command = path /p filename
        // path\shell\printto\command = path /pt filename printer driver port
        strOpenCommandLine += szOpenArg;
        if (bWin95) {
            strPrintCommandLine += szPrintArg;
            strPrintToCommandLine += szPrintToArg;
        }

        // path\shell\open\command = path filename
        strTemp.Format (szShellOpenFmt, (LPCTSTR)strFileTypeId, (LPCTSTR)szCommand);
        if (!SetRegKey (strTemp, strOpenCommandLine)) {
            return; // just skip it
        }

        if (bWin95) {
            // path\shell\print\command = path /p filename
            strTemp.Format (szShellPrintFmt, (LPCTSTR)strFileTypeId, (LPCTSTR)szCommand);
            if (!SetRegKey (strTemp, strPrintCommandLine)) {
                return; // just skip it
            }

            // path\shell\printto\command = path /pt filename printer driver port
            strTemp.Format (szShellPrintToFmt, (LPCTSTR)strFileTypeId, (LPCTSTR)szCommand);
            if (!SetRegKey (strTemp, strPrintToCommandLine)) {
                return; // just skip it
            }
        }

        if (!strFilterExt.IsEmpty ()) {
            ASSERT (strFilterExt[0] == '.');
            LONG lSize   = _MAX_PATH * 2;
            LONG lResult = ::RegQueryValue (HKEY_CLASSES_ROOT, strFilterExt, strTemp.GetBuffer (lSize), &lSize);
            strTemp.ReleaseBuffer ();
            if (lResult != ERROR_SUCCESS || strTemp.IsEmpty () || strTemp == strFileTypeId) {
                // no association for that suffix
                if (!SetRegKey (strFilterExt, strFileTypeId)) {
                    return; // just skip it
                }
                if (bWin95) {
                    strTemp.Format (szShellNewFmt, (LPCTSTR)strFilterExt);
                    (void)SetRegKey (strTemp, szShellNewValue, szShellNewValueName);
                }
            }
        }
    }
}

BOOL LedItDocManager::DoPromptFileName (CString& /*fileName*/, UINT /*nIDSTitle*/, DWORD /*lFlags*/, BOOL /*bOpenFileDialog*/, CDocTemplate* /*pTemplate*/)
{
    // don't call this directly...
    Assert (false);
    return false;
}

void LedItDocManager::OnFileOpen ()
{
    CString    fileName;
    FileFormat format = eUnknownFormat;
    if (LedItDocument::DoPromptOpenFileName (fileName, &format)) {
        OpenDocumentFile (fileName, format);
    }
}
#endif
