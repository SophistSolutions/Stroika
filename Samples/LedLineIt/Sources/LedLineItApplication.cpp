/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include <afx.h>

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Frameworks/Led/Platform/Windows_FileRegistration.h"
#include "Stroika/Frameworks/Led/StdDialogs.h"

#include "FontMenu.h"
#include "LedLineItDocFrame.h"
#include "LedLineItDocument.h"
#include "LedLineItInPlaceFrame.h"
#include "LedLineItMainFrame.h"
#include "LedLineItView.h"
#include "Options.h"
#include "Resource.h"

#include "LedLineItApplication.h"

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;

#define STD_EXCEPT_CATCHER(APP)               \
    catch (CMemoryException * e)              \
    {                                         \
        (APP).HandleBadAllocException ();     \
        e->Delete ();                         \
    }                                         \
    catch (CException * e)                    \
    {                                         \
        (APP).HandleMFCException (e);         \
        e->Delete ();                         \
    }                                         \
    catch (bad_alloc)                         \
    {                                         \
        (APP).HandleBadAllocException ();     \
    }                                         \
    catch (Win32ErrorException&)              \
    {                                         \
        (APP).HandleUnknownException ();      \
    }                                         \
    catch (HRESULTErrorException&)            \
    {                                         \
        (APP).HandleUnknownException ();      \
    }                                         \
    catch (TextInteractor::BadUserInput&)     \
    {                                         \
        (APP).HandleBadUserInputException (); \
    }                                         \
    catch (...)                               \
    {                                         \
        (APP).HandleUnknownException ();      \
    }

class SimpleLedTemplate : public CMultiDocTemplate {
public:
    SimpleLedTemplate (const char* daStr)
        : CMultiDocTemplate (IDR_MAINFRAME, RUNTIME_CLASS (LedLineItDocument), RUNTIME_CLASS (LedLineItDocFrame),
                             RUNTIME_CLASS (LedLineItView))
    {
        m_strDocStrings = daStr;
    }

    virtual void LoadTemplate () override
    {
        CMultiDocTemplate::LoadTemplate ();

        // Now go and fixup the font menu...
        if (m_hMenuShared != NULL) {
            CMenu tmp;
            tmp.Attach (m_hMenuShared);
#if qSupportSyntaxColoring
            FixupFontMenu (tmp.GetSubMenu (2)->GetSubMenu (7));
#else
            FixupFontMenu (tmp.GetSubMenu (2)->GetSubMenu (6));
#endif
            tmp.Detach ();
        }
    }

public:
    CDocument* OpenDocumentFile (LPCTSTR lpszPathName, BOOL bMakeVisible)
    {
        /*
         *  Based on CDocManager::OpenDocumentFile () from MFC (Version?? from MSVC.Net 2k3 - 2003-11-28)
         *  but fixed to better handle exceptions. -- LGP 2003-11-28
         */
        CDocument* pDocument = CreateNewDocument ();
        if (pDocument == NULL) {
            TRACE (traceAppMsg, 0, "CDocTemplate::CreateNewDocument returned NULL.\n");
            AfxMessageBox (AFX_IDP_FAILED_TO_CREATE_DOC);
            return NULL;
        }
        ASSERT_VALID (pDocument);

        try {
            BOOL bAutoDelete         = pDocument->m_bAutoDelete;
            pDocument->m_bAutoDelete = FALSE; // don't destroy if something goes wrong
            CFrameWnd* pFrame        = CreateNewFrame (pDocument, NULL);
            pDocument->m_bAutoDelete = bAutoDelete;
            if (pFrame == NULL) {
                AfxMessageBox (AFX_IDP_FAILED_TO_CREATE_DOC);
                delete pDocument; // explicit delete on error
                return NULL;
            }
            ASSERT_VALID (pFrame);

            if (lpszPathName == NULL) {
                // create a new document - with default document name
                SetDefaultTitle (pDocument);

                // avoid creating temporary compound file when starting up invisible
                if (!bMakeVisible)
                    pDocument->m_bEmbedded = TRUE;

                if (!pDocument->OnNewDocument ()) {
                    // user has be alerted to what failed in OnNewDocument
                    TRACE (traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
                    pFrame->DestroyWindow ();
                    return NULL;
                }

                // it worked, now bump untitled count
                m_nUntitledCount++;
            }
            else {
                // open an existing document
                CWaitCursor wait;
                if (!pDocument->OnOpenDocument (lpszPathName)) {
                    TRACE (traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
                    throw bad_alloc (); // misc random exception - not sure what TO throw here?
                }
                pDocument->SetPathName (lpszPathName);
            }

            InitialUpdateFrame (pFrame, pDocument, bMakeVisible);
            return pDocument;
        }
        catch (...) {
            pDocument->OnCloseDocument ();
            throw;
        }
    }
};

// Make sure the given name isn't a mangled 8.3 name
inline Led_SDK_String GetLongPathName (const Led_SDK_String& pathName)
{
    TCHAR szPath[_MAX_PATH];
    Require (pathName.length () < _MAX_PATH);
    Characters::CString::Copy (szPath, NEltsOf (szPath), pathName.c_str ());
    WIN32_FIND_DATA fileData;
    HANDLE          hFind = ::FindFirstFile (szPath, &fileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        TCHAR* lastSlash = ::_tcsrchr (szPath, '\\');
        if (lastSlash == NULL) {
            szPath[0] = '\0'; // just plain file name
        }
        else {
            // strip the filename part - just keeping the full directory path
            *lastSlash = '\0';
            Characters::CString::Cat (szPath, NEltsOf (szPath), _T ("\\"));
        }
        Characters::CString::Cat (szPath, NEltsOf (szPath), fileData.cFileName);
        VERIFY (::FindClose (hFind));
    }
    return szPath;
}

class LedLineItDocManager : public CDocManager {
private:
    using inherited = CDocManager;

public:
    LedLineItDocManager ()
        : inherited ()
    {
    }

public:
    virtual void OnFileOpen () override
    {
        ::CString fileName;
        int       codePage = 0;
        if (LedLineItDocument::DoPromptOpenFileName (&fileName, &codePage)) {
            OpenDocumentFile (fileName, codePage);
        }
    }

public:
    virtual BOOL DoPromptFileName (::CString& /*fileName*/, UINT /*nIDSTitle*/, DWORD /*lFlags*/, BOOL /*bOpenFileDialog*/, CDocTemplate* /*pTemplate*/) override
    {
        Assert (false); // shouldn't be called - cuz we now override OnFileOpen () to avoid it...
        return false;
    }

public:
    CDocument* OpenDocumentFile (LPCTSTR lpszFileName, CodePage codePage)
    {
        Assert (LedLineItDocument::sHiddenDocOpenArg == kIGNORECodePage);
        try {
            // If there is already an open, untitledDoc - and we open a new document - silently close the unused
            // untitled doc
            CDocument* untitledDoc = NULL;
            {
                size_t   docCount = 0;
                POSITION tp       = GetFirstDocTemplatePosition ();
                while (tp != NULL) {
                    CDocTemplate* t = GetNextDocTemplate (tp);
                    AssertNotNull (t);
                    POSITION dp = t->GetFirstDocPosition ();
                    while (dp != NULL) {
                        CDocument* doc = t->GetNextDoc (dp);
                        AssertNotNull (doc);
                        docCount++;
                        if (docCount == 1) {
                            // See if doc not dirty and has no file name and if so - set it to untitledDoc
                            if (not doc->IsModified () and doc->GetPathName ().GetLength () == 0) {
                                untitledDoc = doc;
                            }
                        }
                        else {
                            untitledDoc = NULL;
                        }
                    }
                }
            }

            LedLineItDocument::sHiddenDocOpenArg = codePage;
            CDocument* doc                       = inherited::OpenDocumentFile (GetLongPathName (lpszFileName).c_str ());
            LedLineItDocument::sHiddenDocOpenArg = kIGNORECodePage;

            if (untitledDoc != NULL) {
                untitledDoc->OnCloseDocument ();
            }
            return doc;
        }
        catch (...) {
            LedLineItDocument::sHiddenDocOpenArg = kIGNORECodePage;
            throw;
        }
    }
    virtual CDocument* OpenDocumentFile (LPCTSTR lpszFileName) override
    {
        return OpenDocumentFile (lpszFileName, kAutomaticallyGuessCodePage);
    }
};

const char kAppName[] = "LedLineIt";

/*
 ********************************************************************************
 ******************************** LedLineItApplication **************************
 ********************************************************************************
 */
LedLineItApplication theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
// {0FC00620-28BD-11CF-899C-00AA00580325}
static const CLSID clsid = {0xfc00620, 0x28bd, 0x11cf, {0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x25}};

BEGIN_MESSAGE_MAP (LedLineItApplication, CWinApp)
ON_COMMAND (ID_APP_ABOUT, OnAppAbout)
ON_COMMAND (ID_FILE_NEW, OnFileNew)
ON_COMMAND (ID_FILE_OPEN, OnFileOpen)
ON_COMMAND (ID_FILE_PRINT_SETUP, OnFilePrintSetup)
ON_COMMAND (kToggleAutoIndentOptionCmd, OnToggleAutoIndentOptionCommand)
ON_UPDATE_COMMAND_UI (kToggleAutoIndentOptionCmd, OnToggleAutoIndentOptionUpdateCommandUI)
ON_COMMAND (kToggleTreatTabAsIndentCharOptionCmd, OnToggleTreatTabAsIndentCharOptionCommand)
ON_UPDATE_COMMAND_UI (kToggleTreatTabAsIndentCharOptionCmd, OnToggleTreatTabAsIndentCharOptionUpdateCommandUI)
ON_COMMAND (kToggleUseSmartCutNPasteCmdID, OnToggleSmartCutNPasteOptionCommand)
ON_UPDATE_COMMAND_UI (kToggleUseSmartCutNPasteCmdID, OnToggleSmartCutNPasteOptionUpdateCommandUI)
#if qSupportSyntaxColoring
ON_COMMAND_RANGE (kNoSyntaxColoringCmd, kVBSyntaxColoringCmd, OnSyntaxColoringOptionCommand)
ON_UPDATE_COMMAND_UI_RANGE (kNoSyntaxColoringCmd, kVBSyntaxColoringCmd, OnSyntaxColoringOptionUpdateCommandUI)
#endif
ON_COMMAND (cmdChooseDefaultFontDialog, OnChooseDefaultFontCommand)
ON_COMMAND (kGotoLedLineItWebPageCmdID, OnGotoLedLineItWebPageCommand)
ON_COMMAND (kCheckForUpdatesWebPageCmdID, OnCheckForUpdatesWebPageCommand)
ON_COMMAND (kGotoSophistsWebPageCmdID, OnGotoSophistsWebPageCommand)
END_MESSAGE_MAP ()

LedLineItApplication* LedLineItApplication::sThe = NULL;

LedLineItApplication::LedLineItApplication ()
    :
#if qIncludeBasicSpellcheckEngine
    fSpellCheckEngine ()
    ,
#endif
    fOleTemplateServer ()
{
    Require (sThe == NULL);
    sThe = this;

#if qIncludeBasicSpellcheckEngine && qDebug
    SpellCheckEngine_Basic::RegressionTest ();
#endif

#if qIncludeBasicSpellcheckEngine && qPlatform_Windows
    {
        // Place the dictionary in a reasonable - but hardwired place. Later - allow for editing that location,
        // and other spellchecking options (see SPR#1591)
        TCHAR defaultPath[MAX_PATH + 1];
        Verify (::SHGetSpecialFolderPath (NULL, defaultPath, CSIDL_FLAG_CREATE | CSIDL_PERSONAL, true));
        fSpellCheckEngine.SetUserDictionary (Led_SDK_String (defaultPath) + Led_SDK_TCHAROF ("\\My LedLineIt Dictionary.txt"));
    }
#endif
}

LedLineItApplication::~LedLineItApplication ()
{
    Require (sThe == this);
    sThe = NULL;
}

LedLineItApplication& LedLineItApplication::Get ()
{
    EnsureNotNull (sThe);
    return *sThe;
}

BOOL LedLineItApplication::InitInstance ()
{
    SetRegistryKey (_T ("Sophist Solutions, Inc."));

    // Initialize OLE libraries
    if (!AfxOleInit ()) {
        AfxMessageBox (IDP_OLE_INIT_FAILED);
        return false;
    }

#if qUseSpyglassDDESDIToOpenURLs
    Led_URLManager::InitDDEHandler ();
#endif

    // Create MDI Frame Window
    {
        LedLineItMainFrame* pFrame = new LedLineItMainFrame ();
        if (not pFrame->LoadFrame (IDR_MAINFRAME)) {
            return false;
        }
        m_pMainWnd = pFrame;

        pFrame->DragAcceptFiles ();
    }

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine (cmdInfo);

#if 0
    // LGP 960509 - doesn't appear to have any effect if present or not...
    // and prevents linking on Mac (CodeWarrior x-compiler).
    Enable3dControlsStatic ();  // Call this when linking to MFC statically
#endif

    LoadStdProfileSettings (9); // Load standard INI file options (including MRU)

    Assert (m_pDocManager == NULL);
    m_pDocManager = new LedLineItDocManager ();

    AddDocTemplateForString ("LedLineIt\n\nLedLineIt\n\n.ledtext\nLedLineIt.Document\nLedLineIt Document", true);

    // Enable DDE Open, and register icons / file types with the explorer/shell
    EnableShellOpen ();
    RegisterShellFileTypes (true); // ARG???

    // When a server application is launched stand-alone, it is a good idea
    //  to update the system registry in case it has been damaged.
    fOleTemplateServer.UpdateRegistry (OAT_INPLACE_SERVER);
    COleObjectFactory::UpdateRegistryAll ();

#if qPlatform_Windows
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
                    dlg.fAppName      = Led_SDK_TCHAROF ("LedLineIt!");
                    dlg.fTypeList     = Led_SDK_TCHAROF (".txt,.bat");
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
        Led_SDK_String       txtDocIcon = Win32UIFileAssociationInfo::kNoChange;
        Led_SDK_String       batDocIcon = Win32UIFileAssociationInfo::kNoChange;
        fileAssocHelper.Add (Win32UIFileAssociationInfo (
            Led_SDK_TCHAROF (".txt"),
            Led_SDK_TCHAROF ("txtfile"),
            Led_SDK_TCHAROF ("Text Document"),
            Win32UIFileAssociationInfo::kNoChange,
            Led_SDK_TCHAROF ("$EXE$ \"%1\"")));
        fileAssocHelper.Add (Win32UIFileAssociationInfo (
            Led_SDK_TCHAROF (".bat"),
            Led_SDK_TCHAROF ("batfile"),
            Led_SDK_TCHAROF ("MS-DOS Batch File"),
            Win32UIFileAssociationInfo::kNoChange,
            Led_SDK_TCHAROF ("$EXE$ \"%1\""),
            Win32UIFileAssociationInfo::kNoChange));
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

    AssertNotNull (m_pMainWnd);
    m_pMainWnd->ShowWindow (m_nCmdShow); // show window NOW so will be up if any error processing cmdline args

    // Dispatch commands specified on the command line
    if (not ProcessShellCommand (cmdInfo)) {
        return false;
    }

    return true;
}

void LedLineItApplication::WinHelpInternal (DWORD_PTR dwData, UINT nCmd)
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
    Characters::CString::Cat (directoryName, NEltsOf (directoryName), _T ("LedLineItDocs\\"));

    // wrap in try/catch, and display error if no open???
    // (NB: we use .htm instead of .html cuz some old systems - I think maybe
    //  Win95 with only Netscape 2.0 installed - only have .htm registered - not
    //  .html).
    (void)::ShellExecute (NULL, _T ("open"), _T ("index.htm"), NULL, directoryName, SW_SHOWNORMAL);
}

BOOL LedLineItApplication::PumpMessage ()
{
    try {
        return inherited::PumpMessage ();
    }
    STD_EXCEPT_CATCHER (*this);
    return true;
}

void LedLineItApplication::HandleMFCException (CException* /*e*/) noexcept
{
    // tmp hack for now...
    HandleUnknownException ();
}

void LedLineItApplication::HandleBadAllocException () noexcept
{
    try {
        CDialog errorDialog (kBadAllocExceptionOnCmdDialogID);
        errorDialog.DoModal ();
    }
    catch (...) {
        Led_BeepNotify ();
    }
}

void LedLineItApplication::HandleBadUserInputException () noexcept
{
    try {
#if qPlatform_Windows
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

void LedLineItApplication::HandleUnknownException () noexcept
{
    try {
        CDialog errorDialog (kUnknownExceptionOnCmdDialogID);
        errorDialog.DoModal ();
    }
    catch (...) {
        Led_BeepNotify ();
    }
}

void LedLineItApplication::AddDocTemplateForString (const char* tmplStr, bool connectToServer)
{
    SimpleLedTemplate* pDocTemplate = new SimpleLedTemplate (tmplStr);
    pDocTemplate->SetContainerInfo (IDR_CNTR_INPLACE);
    pDocTemplate->SetServerInfo (IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE, RUNTIME_CLASS (LedLineItInPlaceFrame));
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

void LedLineItApplication::OnAppAbout ()
{
    class MyAboutBox : public Led_StdDialogHelper_AboutBox {
    private:
        using inherited = Led_StdDialogHelper_AboutBox;

    public:
        MyAboutBox (HINSTANCE hInstance, HWND parentWnd)
            : inherited (hInstance, parentWnd)
        {
        }

    public:
        virtual BOOL OnInitDialog () override
        {
            BOOL result = inherited::OnInitDialog ();

            // Cuz of fact that dlog sizes specified in dlog units, and that doesn't work well for bitmaps
            // we must resize our dlog on the fly based on pict resource size...
            const Led_Coordinate kPictWidth  = 437; // must agree with ACTUAL bitmap size
            const Led_Coordinate kPictHeight = 273;
            const Led_Coordinate kButHSluff  = 17;
            const Led_Coordinate kButVSluff  = 19;
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
#if _UNICODE
#define kUNICODE_NAME_ADORNER L" [UNICODE]"
#elif qWideCharacters
#define kUNICODE_NAME_ADORNER " [Internal UNICODE]"
#else
#define kUNICODE_NAME_ADORNER
#endif
                ::SetWindowText (w, _T (qLed_ShortVersionString) kUNICODE_NAME_ADORNER _T (" (") _T (__DATE__) _T (")"));
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
                ::MoveWindow (w, kButHSluff, kPictHeight - AsLedRect (tmp).GetHeight () - kButVSluff, AsLedRect (tmp).GetWidth (), AsLedRect (tmp).GetHeight (), false); // width/height we should presevere
            }

            ::SetWindowText (GetHWND (), _T ("About LedLineIt!"));

            return (result);
        }

    public:
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
                Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/LedLineIt/", kAppName));
            }
            catch (...) {
                // ignore for now - since errors here prent dialog from dismissing (on MacOSX)
            }
            inherited::OnClickInLedWebPageField ();
        }
    };
    MyAboutBox dlg (m_hInstance, AfxGetMainWnd ()->m_hWnd);
    dlg.DoModal ();
}

void LedLineItApplication::OnGotoLedLineItWebPageCommand ()
{
    Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/LedLineIt/", kAppName));
}

void LedLineItApplication::OnCheckForUpdatesWebPageCommand ()
{
    Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName));
}

void LedLineItApplication::OnGotoSophistsWebPageCommand ()
{
    Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/", kAppName));
}

void LedLineItApplication::OnToggleAutoIndentOptionCommand ()
{
    Options o;
    o.SetAutoIndent (not o.GetAutoIndent ());
}

void LedLineItApplication::OnToggleTreatTabAsIndentCharOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
    RequireNotNull (pCmdUI);
    pCmdUI->Enable ();
    pCmdUI->SetCheck (Options ().GetTreatTabAsIndentChar ());
}

void LedLineItApplication::OnToggleTreatTabAsIndentCharOptionCommand ()
{
    Options o;
    o.SetTreatTabAsIndentChar (not o.GetTreatTabAsIndentChar ());
}

void LedLineItApplication::OnToggleAutoIndentOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
    RequireNotNull (pCmdUI);
    pCmdUI->Enable ();
    pCmdUI->SetCheck (Options ().GetAutoIndent ());
}

void LedLineItApplication::OnToggleSmartCutNPasteOptionCommand ()
{
    Options o;
    o.SetSmartCutAndPaste (not o.GetSmartCutAndPaste ());
    UpdateViewsForPrefsChange ();
}

void LedLineItApplication::OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
    RequireNotNull (pCmdUI);
    pCmdUI->Enable ();
    pCmdUI->SetCheck (Options ().GetSmartCutAndPaste ());
}

#if qSupportSyntaxColoring
void LedLineItApplication::OnSyntaxColoringOptionCommand (UINT cmdNum)
{
    Options o;
    switch (cmdNum) {
        case kNoSyntaxColoringCmd:
            o.SetSyntaxColoringOption (Options::eSyntaxColoringNone);
            break;
        case kCPlusPlusSyntaxColoringCmd:
            o.SetSyntaxColoringOption (Options::eSyntaxColoringCPlusPlus);
            break;
        case kVBSyntaxColoringCmd:
            o.SetSyntaxColoringOption (Options::eSyntaxColoringVB);
            break;
        default:
            Assert (false);
    }
    UpdateViewsForPrefsChange ();
}

void LedLineItApplication::OnSyntaxColoringOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
    RequireNotNull (pCmdUI);
    pCmdUI->Enable ();
    switch (pCmdUI->m_nID) {
        case kNoSyntaxColoringCmd:
            pCmdUI->SetCheck (Options ().GetSyntaxColoringOption () == Options::eSyntaxColoringNone);
            break;
        case kCPlusPlusSyntaxColoringCmd:
            pCmdUI->SetCheck (Options ().GetSyntaxColoringOption () == Options::eSyntaxColoringCPlusPlus);
            break;
        case kVBSyntaxColoringCmd:
            pCmdUI->SetCheck (Options ().GetSyntaxColoringOption () == Options::eSyntaxColoringVB);
            break;
        default:
            Assert (false);
    }
}
#endif

void LedLineItApplication::OnChooseDefaultFontCommand ()
{
    Led_FontSpecification fsp = Options ().GetDefaultNewDocFont ();

    LOGFONT lf;
    (void)::memset (&lf, 0, sizeof (lf));
    {
        Characters::CString::Copy (lf.lfFaceName, NEltsOf (lf.lfFaceName), fsp.GetFontNameSpecifier ().fName);
        Assert (::_tcslen (lf.lfFaceName) < sizeof (lf.lfFaceName)); // cuz our cached entry - if valid - always short enuf...
    }
    lf.lfWeight    = (fsp.GetStyle_Bold ()) ? FW_BOLD : FW_NORMAL;
    lf.lfItalic    = (fsp.GetStyle_Italic ());
    lf.lfUnderline = (fsp.GetStyle_Underline ());
    lf.lfStrikeOut = (fsp.GetStyle_Strikeout ());

    lf.lfHeight = fsp.PeekAtTMHeight ();

    FontDlgWithNoColorNoStyles dlog (&lf);
    if (dlog.DoModal () == IDOK) {
        Options ().SetDefaultNewDocFont (Led_FontSpecification (*dlog.m_cf.lpLogFont));
    }
}

void LedLineItApplication::UpdateViewsForPrefsChange ()
{
    bool smartCutNPaste = Options ().GetSmartCutAndPaste ();

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
                LedLineItView* lv = dynamic_cast<LedLineItView*> (v);
                if (lv != NULL) {
                    lv->SetSmartCutAndPasteMode (smartCutNPaste);
#if qSupportSyntaxColoring
                    lv->ResetSyntaxColoringTable ();
#endif
                }
            }
        }
    }
}

BOOL LedLineItApplication::ProcessShellCommand (CCommandLineInfo& rCmdInfo)
{
    try {
/*
        *   SPR#0775. MFC doesnt' keep track of all the files requested to open. So walk the list of file arguments
        *   a SECOND TIME! PATHETIC!
        */
#if !qNo_argc_argv_MacrosSupported
        if (rCmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen) {
            for (int i = 1; i < __argc; i++) {
                LPCTSTR pszParam = __targv[i];
                if (pszParam[0] == '-' || pszParam[0] == '/') {
                    // skip flags
                }
                else {
                    try {
                        if (not m_pDocManager->OpenDocumentFile (pszParam)) {
                            return false;
                        }
                    }
                    catch (...) {
                        // see if file just doesn't exist. If so - then create NEW DOC with that file name (preset) - but not saved
                        HANDLE h = ::CreateFile (rCmdInfo.m_strFileName, 0, 0, NULL, OPEN_EXISTING, 0, NULL);
                        if (h == INVALID_HANDLE_VALUE) {
                            POSITION      p         = m_pDocManager->GetFirstDocTemplatePosition ();
                            CDocTemplate* pTemplate = (CDocTemplate*)m_pDocManager->GetNextDocTemplate (p);
                            AssertNotNull (pTemplate);
                            CDocument* newDoc = pTemplate->OpenDocumentFile (NULL); // new doc
                            if (newDoc != NULL) {
                                // Set path and title so if you just hit save - you are promted to save in the name you specified, but
                                // there are visual clues that this isn't the orig file
                                newDoc->SetPathName (pszParam, false);
                                newDoc->SetTitle (newDoc->GetTitle () + Led_SDK_TCHAROF (" {new}"));
                                continue; // with outer for loop - ignore exception
                            }
                        }
                        else {
                            ::CloseHandle (h);
                        }
                        throw;
                    }
                }
            }
            return true;
        }
#endif
        return inherited::ProcessShellCommand (rCmdInfo);
    }
    STD_EXCEPT_CATCHER (*this);
    return false;
}
