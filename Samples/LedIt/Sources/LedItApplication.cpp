/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_MacOS
#include <Balloons.h>
#include <Gestalt.h>
#include <ToolUtils.h>

#include <LCaption.h>
#include <LDialogBox.h>
#include <LEditField.h>
#include <LGrowZone.h>
#include <LMenu.h>
#include <LMenuBar.h>
#include <LPicture.h>
#include <LPlaceHolder.h>
#include <LPrintout.h>
#include <LStdControl.h>
#include <LTabGroup.h>
#include <LTextButton.h>
#include <LWindow.h>
#include <PP_Messages.h>
#include <PP_Resources.h>
#include <TArrayIterator.h>
#include <UAppleEventsMgr.h>
#include <UDesktop.h>
#include <UMemoryMgr.h>
#include <UModalDialogs.h>
#include <URegistrar.h>
#elif defined(WIN32)

#include <afx.h>

#elif qXWindows
#include <gtk/gtkoptionmenu.h>
#endif

#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String.h"

#include "Stroika/Frameworks/Led/Config.h"
#include "Stroika/Frameworks/Led/StdDialogs.h"
#if qPlatform_Windows
#include "Stroika/Frameworks/Led/Platform/Windows_FileRegistration.h"
#endif
#include "Stroika/Frameworks/Led/StyledTextEmbeddedObjects.h"

#if qPlatform_Windows
#include "LedItControlItem.h"
#include "LedItInPlaceFrame.h"
#include "LedItMainFrame.h"
#elif qPlatform_MacOS
#include "FilteredFilePicker.h"
#endif

#include "LedItDocument.h"
#include "LedItResources.h"
#include "LedItView.h"
#include "Options.h"

#include "LedItApplication.h"

#if qXWindows
#include "AboutBoxImage.xpm"
#endif

using namespace Stroika::Foundation;
using namespace Stroika::Frameworks::Led;
using namespace Stroika::Frameworks::Led::Platform;
using namespace Stroika::Frameworks::Led::StyledTextIO;

#if qPlatform_MacOS
static Handle sDeepShitCheeseBuf = NULL; // so no mem alerts don't crash...

inline void DoStringyAlert (short alertID, const ConstStr255Param p0 = NULL, const ConstStr255Param p1 = NULL, const ConstStr255Param p2 = NULL, const ConstStr255Param p3 = NULL)
{
    if (sDeepShitCheeseBuf != NULL) {
        ::DisposeHandle (sDeepShitCheeseBuf);
        sDeepShitCheeseBuf = NULL;
    }
    if (::GetResource ('ALRT', alertID) == nil) {
        Led_BeepNotify ();
    }
    else {
        try {
            Led_CheckSomeLocalHeapRAMAvailable (4 * 1024); // empiricly arrived at how much needed to avoid crash
        }
        catch (...) {
            Led_BeepNotify ();
            return;
        }
        ::ParamText (p0 == NULL ? "\p" : p0, p1 == NULL ? "\p" : p1, p2 == NULL ? "\p" : p2, p3 == NULL ? "\p" : p3);
        ::InitCursor ();
        ::CautionAlert (alertID, nil);
    }
}
#endif

const char kAppName[] = "LedIt";

#if qPlatform_MacOS

#define STANDARD_LEDITAPPLICATION_MACOS_CATCHERS()              \
    catch (OSErr err)                                           \
    {                                                           \
        HandleMacOSException (err);                             \
    }                                                           \
    catch (bad_alloc)                                           \
    {                                                           \
        HandleBadAllocException ();                             \
    }                                                           \
    catch (TextInteractor::BadUserInput&)                       \
    {                                                           \
        HandleBadUserInputException ();                         \
    }                                                           \
    catch (const LException& err)                               \
    {                                                           \
        HandlePowerPlantException ((OSErr)err.GetErrorCode ()); \
    }                                                           \
    catch (ExceptionCode err)                                   \
    {                                                           \
        HandlePowerPlantException (err);                        \
    }                                                           \
    catch (...)                                                 \
    {                                                           \
        HandleUnknownException ();                              \
    }

#endif

#if qPlatform_Windows

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
    catch (HRESULT hr)                        \
    {                                         \
        (APP).HandleHRESULTException (hr);    \
    }                                         \
    catch (TextInteractor::BadUserInput&)     \
    {                                         \
        (APP).HandleBadUserInputException (); \
    }                                         \
    catch (...)                               \
    {                                         \
        (APP).HandleUnknownException ();      \
    }

#endif

#if qPlatform_Windows
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
    virtual void       OnFileNew () override;
    virtual CDocument* OpenDocumentFile (LPCTSTR lpszFileName) override;
    nonvirtual CDocument* OpenDocumentFile (LPCTSTR lpszFileName, FileFormat format);

public:
    virtual void RegisterShellFileTypes (BOOL bWin95) override;

private:
    nonvirtual void RegisterShellFileType (bool bWin95, CString strPathName, int iconIndexInFile, CString strFilterExt, CString strFileTypeId, CString strFileTypeName);

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
        if (::RegSetValue (HKEY_CLASSES_ROOT, lpszKey, REG_SZ, lpszValue, ::_tcslen (lpszValue)) != ERROR_SUCCESS) {
            TRACE1 ("Warning: registration database update failed for key '%s'.\n", lpszKey);
            return FALSE;
        }
        return TRUE;
    }
    else {
        HKEY hKey;
        if (::RegCreateKey (HKEY_CLASSES_ROOT, lpszKey, &hKey) == ERROR_SUCCESS) {
            LONG lResult = ::RegSetValueEx (hKey, lpszValueName, 0, REG_SZ, (CONST BYTE*)lpszValue, ::_tcslen (lpszValue) + sizeof (TCHAR));
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
#if qPlatform_Windows
public:
    MyAboutBox (HINSTANCE hInstance, HWND parentWnd)
        : inherited (hInstance, parentWnd)
    {
    }
#elif qXWindows
public:
    MyAboutBox (GtkWindow* modalParentWindow)
        : inherited (modalParentWindow)
    {
    }
#endif

public:
    virtual void PreDoModalHook () override
    {
        inherited::PreDoModalHook ();
#if _UNICODE
#define kUNICODE_NAME_ADORNER L" [UNICODE]"
#elif qWideCharacters
#define kUNICODE_NAME_ADORNER " [Internal UNICODE]"
#else
#define kUNICODE_NAME_ADORNER
#endif

#if qPlatform_MacOS
        const short    kPictHeight = 273;
        const short    kPictWidth  = 437;
        Led_SDK_String verStr      = Led_SDK_String (qLed_ShortVersionString) + kUNICODE_NAME_ADORNER " (" + __DATE__ + ")";
        const int      kVERWidth   = 230;
        SimpleLayoutHelper (kPictHeight, kPictWidth, Led_Rect (159, 15, 17, 142), Led_Rect (159, 227, 17, 179), verStr);
#elif qPlatform_Windows
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

        ::SetWindowText (GetHWND (), _T ("About LedIt!"));
#elif qXWindows
        GtkWidget* window = GetWindow ();
        gtk_container_set_border_width (GTK_CONTAINER (window), 10);

        /* now for the pixmap from gdk */
        GtkStyle*  style  = gtk_widget_get_style (window);
        GdkBitmap* mask   = NULL;
        GdkPixmap* pixmap = gdk_pixmap_create_from_xpm_d (window->window, &mask, &style->bg[GTK_STATE_NORMAL], (gchar**)AboutBoxImage_xpm);

        /* Create box for xpm and label */
        GtkWidget* box1 = gtk_hbox_new (FALSE, 0);
        gtk_container_set_border_width (GTK_CONTAINER (box1), 2);

        /* a pixmap widget to contain the pixmap */
        GtkWidget*     pixmapwid = gtk_pixmap_new (pixmap, mask);
        Led_SDK_String verStr    = Led_SDK_String (qLed_ShortVersionString) + " (" + __DATE__ + ")";

        GtkWidget* label = gtk_label_new (verStr.c_str ());

        gtk_box_pack_start (GTK_BOX (box1), label, FALSE, FALSE, 3);
        gtk_box_pack_start (GTK_BOX (box1), pixmapwid, FALSE, FALSE, 3);

        gtk_widget_show (pixmapwid);
        gtk_widget_show (label);
        gtk_widget_show (box1);

        /* a button to contain the pixmap widget */
        GtkWidget* button = gtk_button_new ();
        gtk_container_add (GTK_CONTAINER (button), box1);
        gtk_container_add (GTK_CONTAINER (window), button);
        gtk_widget_show (button);
        SetOKButton (button);

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

#if qXWindows
class LedItFilePickBox : public StdFilePickBox {
private:
    using inherited = StdFilePickBox;

public:
    LedItFilePickBox (GtkWindow* modalParentWindow, const Led_SDK_String& title, bool saveDialog, const Led_SDK_String& fileName, FileFormat fileFormat)
        : inherited (modalParentWindow, title, saveDialog, fileName)
        , fFileFormat (fileFormat)
        , fFileTypeChoice (NULL)
    {
    }

protected:
    virtual void PreDoModalHook () override
    {
        inherited::PreDoModalHook ();

        {
            GtkBox*    mainVBox     = GTK_BOX (GTK_FILE_SELECTION (GetWindow ())->main_vbox);
            GtkWidget* fileTypeHBox = gtk_hbox_new (TRUE, 10);
            gtk_box_pack_start (mainVBox, fileTypeHBox, FALSE, FALSE, 0);
            gtk_widget_show (fileTypeHBox);

            // pulldown label
            GtkWidget* filetypesLabel = gtk_label_new ("File Type:");
            gtk_label_set_justify (GTK_LABEL (filetypesLabel), GTK_JUSTIFY_RIGHT);
            gtk_misc_set_alignment (GTK_MISC (filetypesLabel), 1.0, 0.5);
            gtk_widget_show (filetypesLabel);
            gtk_box_pack_start (GTK_BOX (fileTypeHBox), filetypesLabel, FALSE, TRUE, 0);

            // pulldown menu
            fFileTypeChoice = GTK_OPTION_MENU (gtk_option_menu_new ());
            gtk_widget_show (GTK_WIDGET (fFileTypeChoice));
            gtk_box_pack_end (GTK_BOX (fileTypeHBox), GTK_WIDGET (fFileTypeChoice), FALSE, TRUE, 0);

            /*
             *  Put the fileType box in the visually pleasing spot. Of course the number 3 is based on the current, gtk 1.2.10 behavior, and could
             *  become wrong as the gtk code it depends on changes.
             */
            gtk_box_reorder_child (mainVBox, fileTypeHBox, 5);

            // Add the file types list (and autodetect for OPEN)
            {
                GtkWidget* menu = gtk_menu_new ();
                if (not fSaveDialog) {
                    GtkWidget* m = gtk_menu_item_new_with_label ("Automatically recognize type");
                    gtk_object_set_user_data (GTK_OBJECT (m), reinterpret_cast<void*> (eUnknownFormat));
                    gtk_widget_show (m);
                    gtk_menu_append (GTK_MENU (menu), m);
                    // Should add a separator...TOO!
                }
                struct FileFilterDesc {
                    Led_SDK_String fDescription;
                    FileFormat     fFormat;
                };
                static FileFilterDesc typeList[] = {
                    {"HTML file", eHTMLFormat},
                    {"Led Rich Text Format", eLedPrivateFormat},
                    {"Microsoft Rich Text Format (RTF)", eRTFFormat},
                    {"Text file", eTextFormat},
                };
                size_t defaultItem = 0;
                for (size_t i = 0; i < Led_NEltsOf (typeList); ++i) {
                    GtkWidget* m = gtk_menu_item_new_with_label (typeList[i].fDescription.c_str ());
                    gtk_object_set_user_data (GTK_OBJECT (m), reinterpret_cast<void*> (typeList[i].fFormat));
                    gtk_widget_show (m);
                    gtk_menu_append (GTK_MENU (menu), m);
                    if (fSaveDialog) {
                        // set initial item selected to reflect original type
                        if (typeList[i].fFormat == fFileFormat) {
                            defaultItem = i;
                        }
                    }
                }
                gtk_widget_show (menu);
                gtk_option_menu_set_menu (fFileTypeChoice, menu);
                gtk_menu_set_active (GTK_MENU (menu), defaultItem);
                gtk_option_menu_set_history (fFileTypeChoice, defaultItem);
            }
        }
    }
    virtual void OnOK () override
    {
        inherited::OnOK ();
        // what a long ugly line of code
        GtkWidget* activeItem = gtk_menu_get_active (GTK_MENU (gtk_option_menu_get_menu (fFileTypeChoice)));
        AssertNotNull (activeItem);
        fFileFormat = (FileFormat) (reinterpret_cast<int> (gtk_object_get_user_data (GTK_OBJECT (activeItem))));
    }

public:
    nonvirtual FileFormat GetFileFormat () const
    {
        return fFileFormat;
    }

private:
    FileFormat     fFileFormat;
    GtkOptionMenu* fFileTypeChoice;
};
#endif

/*
 ********************************************************************************
 ******************************** LedItApplication ******************************
 ********************************************************************************
 */
#if qPlatform_Windows
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
#if qPlatform_MacOS || qPlatform_Windows
    inherited ()
    ,
#endif
#if qIncludeBasicSpellcheckEngine
    fSpellCheckEngine ()
    ,
#endif
#if qPlatform_MacOS
    fHelpMenuItem (0)
    , fGotoLedItWebPageMenuItem (0)
    , fGotoSophistsWebPageMenuItem (0)
    , fCheckForUpdatesWebPageMenuItem (0)
    , fLastLowMemWarnAt (0.0f)
#endif
#if qPlatform_Windows
          fOleTemplateServer ()
    ,
#endif
#if qPlatform_Windows
    fInstalledFonts ()
#elif qXWindows
    fInstalledFonts (GDK_DISPLAY ())
    ,
#endif
#if qXWindows
        fAppWindow (NULL)
    , fDocument (NULL)
#endif
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
#ifndef CSIDL_FLAG_CREATE
#define CSIDL_FLAG_CREATE 0x8000 // new for Win2K, or this in to force creation of folder
#endif
        Verify (::SHGetSpecialFolderPath (NULL, defaultPath, CSIDL_FLAG_CREATE | CSIDL_PERSONAL, true));
        fSpellCheckEngine.SetUserDictionary (Led_SDK_String (defaultPath) + Led_SDK_TCHAROF ("\\My LedIt Dictionary.txt"));
    }
#endif

/*
     *  It would be nice to be able to add all the standard types in one place, but due to the quirkly requirement (or is it
     *  just a convention) with MFC of having the application object staticly constructed, we run into a 'race condition'
     *  of sorts. Its undefined by C++ the order of static constructors across modules. As a result - some of the static
     *  consts used in EmbeddedObjectCreatorRegistry aren't yet initialized. Sigh... Perhaps I could/should find
     *  a better way to make this work, but lets KISS (keep it simple stupid) for now, as we are very close to
     *  release -- LGP 2001-10-06.
     */
#if !qPlatform_Windows
    // Tell Led about the kinds of embeddings we will allow
    EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();

#if qPlatform_Windows
    // Support OLE embeddings (both created from clip, and from RTF-format files)
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (LedItControlItem::kClipFormat, LedItControlItem::kEmbeddingTag, LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (kBadClipFormat, RTFIO::RTFOLEEmbedding::kEmbeddingTag, LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);
#endif
#endif

#if qPlatform_MacOS
    // Register classes for objects created from 'PPob' resources
    TRegistrar<LPlaceHolder>::Register ();
    TRegistrar<LPrintout>::Register ();
    TRegistrar<LDialogBox>::Register ();
    TRegistrar<LPicture>::Register ();
    TRegistrar<LCaption>::Register ();
    TRegistrar<LPane>::Register ();
    TRegistrar<LTextButton>::Register ();
    TRegistrar<LStdCheckBox>::Register ();
    TRegistrar<LStdButton>::Register ();
    TRegistrar<LEditField>::Register ();
    TRegistrar<LTabGroup>::Register ();
    TRegistrar<LStdPopupMenu>::Register ();

// Tell Led about the picture resources it needs to render some special embedding markers
#if !qURLStyleMarkerNewDisplayMode
    StandardURLStyleMarker::sURLPict = (Picture**)::GetResource ('PICT', kURLPictID);
#endif
    StandardUnknownTypeStyleMarker::sUnknownPict   = (Picture**)::GetResource ('PICT', kUnknownEmbeddingPictID);
    StandardDIBStyleMarker::sUnsupportedFormatPict = (Picture**)::GetResource ('PICT', kUnsupportedDIBFormatPictID);

    // Always make sure sleep time no longer than the caret blink time.
    // But default to 6 ticks (PP's default) - 0.1 seconds.
    SetSleepTime (Led_Min (6, GetCaretTime ()));
#elif qXWindows
    fDocument = new LedItDocument ();

    fAppWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_signal_connect (GTK_OBJECT (fAppWindow), "destroy", GTK_SIGNAL_FUNC (xdestroy), this);
    //      gtk_window_set_title (GTK_WINDOW(fAppWindow), "LedIt!");
    UpdateFrameWindowTitle ();
    gtk_widget_set_usize (GTK_WIDGET (fAppWindow), 300, 200);
    gtk_widget_set_usize (fAppWindow, 600, 500);
    gtk_window_set_policy (GTK_WINDOW (fAppWindow), TRUE, TRUE, FALSE);
    gtk_container_set_border_width (GTK_CONTAINER (fAppWindow), 0);

    GtkWidget* main_vbox = gtk_vbox_new (FALSE, 1);
    gtk_container_border_width (GTK_CONTAINER (main_vbox), 1);
    gtk_container_add (GTK_CONTAINER (fAppWindow), main_vbox);
    gtk_widget_show (main_vbox);

    GtkWidget* menubar = get_main_menu (fAppWindow);

    gtk_box_pack_start (GTK_BOX (main_vbox), menubar, FALSE, TRUE, 0);
    gtk_widget_show (menubar);

    {
        GtkWidget* box1 = gtk_vbox_new (FALSE, 0);
        gtk_container_add (GTK_CONTAINER (main_vbox), box1);
        gtk_widget_show (box1);

        GtkWidget* box2 = gtk_vbox_new (FALSE, 10);
        gtk_container_set_border_width (GTK_CONTAINER (box2), 10);
        gtk_box_pack_start (GTK_BOX (box1), box2, TRUE, TRUE, 0);
        gtk_widget_show (box2);

        GtkWidget* table = gtk_table_new (2, 2, false);
        gtk_table_set_row_spacing (GTK_TABLE (table), 0, 2);
        gtk_table_set_col_spacing (GTK_TABLE (table), 0, 2);
        gtk_box_pack_start (GTK_BOX (box2), table, TRUE, TRUE, 0);
        gtk_widget_show (table);

        /* Create the GtkText widget */
        fTextEditor = new LedItView (fDocument);

        gtk_table_attach (GTK_TABLE (table), fTextEditor->Get_GtkWidget (), 0, 1, 0, 1,
                          static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                          static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL),
                          0, 0);
        gtk_widget_show (fTextEditor->Get_GtkWidget ());

#if 1
        /* Add a vertical scrollbar to the text widget */
        GtkAdjustment* vAdj = fTextEditor->GetAdjustmentObject (TextInteractor::v);
        GtkWidget* vscrollbar = gtk_vscrollbar_new (vAdj);
        gtk_table_attach (GTK_TABLE (table), vscrollbar, 1, 2, 0, 1, static_cast<GtkAttachOptions> (GTK_FILL), static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL), 0, 0);
        gtk_widget_show (vscrollbar);

        /* Add a horizontal scrollbar to the text widget */
        GtkAdjustment* hAdj = fTextEditor->GetAdjustmentObject (TextInteractor::h);
        GtkWidget* hscrollbar = gtk_hscrollbar_new (hAdj);
        gtk_table_attach (GTK_TABLE (table), hscrollbar, 0, 1, 1, 2, static_cast<GtkAttachOptions> (GTK_EXPAND | GTK_SHRINK | GTK_FILL), static_cast<GtkAttachOptions> (GTK_FILL), 0, 0);
        gtk_widget_show (hscrollbar);
#endif
        gtk_widget_show (fAppWindow);

        // Initially set focus to the text editor widget
        gtk_widget_grab_focus (fTextEditor->Get_GtkWidget ());
    }
#endif
}

LedItApplication::~LedItApplication ()
{
    Require (sThe == this);
    sThe = NULL;
#if qXWindows
    delete fDocument;
#endif
}

LedItApplication& LedItApplication::Get ()
{
    EnsureNotNull (sThe);
    return *sThe;
}

void LedItApplication::DoAboutBox ()
{
#if qPlatform_MacOS
    MyAboutBox dlg;
#elif qPlatform_Windows
    MyAboutBox dlg (m_hInstance, AfxGetMainWnd ()->m_hWnd);
#elif qXWindows
    MyAboutBox dlg (GTK_WINDOW (fAppWindow));
#endif
    dlg.DoModal ();
}

void LedItApplication::OnGotoLedItWebPageCommand ()
{
    try {
        Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/LedIt/", kAppName));
    }
    catch (...) {
#if qPlatform_MacOS
        DoStringyAlert (kCannotOpenWebPageAlertID);
#endif
    }
}

void LedItApplication::OnGotoSophistsWebPageCommand ()
{
    try {
        Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/", kAppName));
    }
    catch (...) {
#if qPlatform_MacOS
        DoStringyAlert (kCannotOpenWebPageAlertID);
#endif
    }
}

void LedItApplication::OnCheckForUpdatesWebPageCommand ()
{
    try {
        Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName));
    }
    catch (...) {
#if qPlatform_MacOS
        DoStringyAlert (kCannotOpenWebPageAlertID);
#endif
    }
}

#if qPlatform_MacOS
void LedItApplication::StartUp ()
{
    try {
        ObeyCommand (cmd_New, nil); // create a new window
    }
    STANDARD_LEDITAPPLICATION_MACOS_CATCHERS ();
}

void LedItApplication::MakeMenuBar ()
{
    inherited::MakeMenuBar ();

    // Base class has already constructed the menu bar, by this point...
    // So we can add font menus, and associate command numbers...
    {
        MenuRef fontMenuHandle = ::GetMenuHandle (cmd_FontMenu);
        AssertNotNull (fontMenuHandle);
        ::AppendResMenu (fontMenuHandle, 'FONT');
        AssertNotNull (LMenuBar::GetCurrentMenuBar ());
        LMenu* fontMenu = LMenuBar::GetCurrentMenuBar ()->FetchMenu (cmd_FontMenu);
        AssertNotNull (fontMenu);
        size_t nMenuItems = ::CountMenuItems (fontMenu->GetMacMenuH ());
        for (size_t i = 1; i <= nMenuItems; i++) {
            fontMenu->SetCommand (i, i - 1 + kBaseFontNameCmd); // make first cmd = kBaseFontNameCmd
        }
    }

    bool aquaUI = false;
    {
        SInt32 gestaltResponse = 0;
        aquaUI                 = (::Gestalt (gestaltMenuMgrAttr, &gestaltResponse) == noErr) and (gestaltResponse & gestaltMenuMgrAquaLayoutMask);
    }

    // Add a special help menu item you launch our help file
    // (see NewIM Essential Macintosh Toolbox, 3-68)
    {
        MenuHandle helpMenu = NULL;
#if !TARGET_CARBON
        if (::HMGetHelpMenuHandle (&helpMenu) != noErr) {
            helpMenu = NULL;
        }
#endif
        if (helpMenu == NULL) {
            // if there is no helpMenu, then add our own...
            helpMenu = ::NewMenu (kHelpMenuID, "\pHelp");
            AssertNotNull (helpMenu);
            MenuID append = 0;
            ::InsertMenu (helpMenu, append);
        }
        if (helpMenu != NULL) {
            ::AppendMenu (helpMenu, "\pBrowse Local Help");
            fHelpMenuItem = ::CountMenuItems (helpMenu);
            ::AppendMenu (helpMenu, "\p-");
            ::AppendMenu (helpMenu, "\pxxx"); // Text in SetMenuItemText() call cuz idiodic mac toolbox interprets '!' character!
            ::SetMenuItemText (helpMenu, ::CountMenuItems (helpMenu), "\pGoto LedIt! Web Page");
            fGotoLedItWebPageMenuItem = ::CountMenuItems (helpMenu);
            ::AppendMenu (helpMenu, "\pGoto Sophist Solutions Web Page");
            fGotoSophistsWebPageMenuItem = ::CountMenuItems (helpMenu);
            ::AppendMenu (helpMenu, "\pCheck for LedIt! Web Updates");
            fCheckForUpdatesWebPageMenuItem = ::CountMenuItems (helpMenu);
        }
    }

    // For Aqua UI (OSX) - lose the Quit menu item.
    if (aquaUI) {
        MenuRef fileMenu = ::GetMenuHandle (kFileMenuID);
        AssertNotNull (fileMenu);
        // Lose the separator AND the Quit menu item.
        ::DeleteMenuItem (fileMenu, ::CountMenuItems (fileMenu));
        ::DeleteMenuItem (fileMenu, ::CountMenuItems (fileMenu));
    }
}

void LedItApplication::ProcessNextEvent ()
{
    if (sDeepShitCheeseBuf == NULL) {
        sDeepShitCheeseBuf = ::NewHandle (10 * 1024);
    }
    try {
        inherited::ProcessNextEvent ();
    }
    STANDARD_LEDITAPPLICATION_MACOS_CATCHERS ();
}

void LedItApplication::HandleAppleEvent (const AppleEvent& inAppleEvent, AppleEvent& outAEReply, AEDesc& outResult, long inAENumber)
{
    try {
        inherited::HandleAppleEvent (inAppleEvent, outAEReply, outResult, inAENumber);
    }
    STANDARD_LEDITAPPLICATION_MACOS_CATCHERS ();
}

void LedItApplication::HandleMacOSException (OSErr err)
{
    switch (err) {
        case memFullErr: {
            HandleBadAllocException ();
        } break;

        default: {
            Str255 tmp;
            NumToString (err, tmp);
            DoStringyAlert (kGenericMacOSExceptionAlertID, tmp);
        } break;
    }
}

void LedItApplication::HandlePowerPlantException (ExceptionCode err)
{
    if (err > 32767 or err < -32768) {
        Str255 tmp;
        ::NumToString (err, tmp);
        ::DoStringyAlert (kPowerPlantExceptionAlertID, tmp);
    }
    else {
        Led_Assert (err == OSErr (err));
        HandleMacOSException (OSErr (err));
    }
}

Boolean LedItApplication::ObeyCommand (CommandT inCommand, void* ioParam)
{
    Boolean cmdHandled = true;

    if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fHelpMenuItem == LoWord (-inCommand)) {
        OnHelpMenuCommand ();
        return true;
    }
    if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fGotoLedItWebPageMenuItem == LoWord (-inCommand)) {
        OnGotoLedItWebPageCommand ();
        return true;
    }
    if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fGotoSophistsWebPageMenuItem == LoWord (-inCommand)) {
        OnGotoSophistsWebPageCommand ();
        return true;
    }
    if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fCheckForUpdatesWebPageMenuItem == LoWord (-inCommand)) {
        OnCheckForUpdatesWebPageCommand ();
        return true;
    }

    if (inCommand >= kBaseWindowCmd and inCommand <= kLastWindowCmd) {
        size_t                  windowIdx = (inCommand - kBaseWindowCmd);
        const vector<LWindow*>& windows   = LedItDocument::GetDocumentWindows ();
        if (windowIdx < windows.size ()) {
            LWindow* w = windows[windowIdx];
            AssertNotNull (w);
            UDesktop::SelectDeskWindow (w);
        }
        else {
            Led_Assert (false); // we shouldn't get these!
        }
        return true;
    }

    switch (inCommand) {
        case kToggleUseSmartCutNPasteCmd:
            OnToggleSmartCutNPasteOptionCommand ();
            break;
        case kToggleWrapToWindowCmd:
            OnToggleWrapToWindowOptionCommand ();
            break;
        case kToggleShowHiddenTextCmd:
            OnToggleShowHiddenTextOptionCommand ();
            break;
        default:
            cmdHandled = inherited::ObeyCommand (inCommand, ioParam);
            break;
    }

    return cmdHandled;
}

//  Pass back status of a (menu) command
void LedItApplication::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
                                          UInt16& outMark, Str255 outName)
{
    if (inCommand >= kBaseWindowCmd and inCommand <= kLastWindowCmd) {
        size_t                  windowIdx = (inCommand - kBaseWindowCmd);
        const vector<LWindow*>& windows   = LedItDocument::GetDocumentWindows ();
        if (windowIdx < windows.size ()) {
            LWindow* w = windows[windowIdx];
            AssertNotNull (w);
            outEnabled = true;
            (void)w->GetDescriptor (outName);
            outMark = UDesktop::WindowIsSelected (w) ? checkMark : 0;
        }
        else {
            Led_Assert (false); // we shouldn't get these!
        }
        outUsesMark = true;
        return;
    }

    outUsesMark = false;
    switch (inCommand) {
        case kToggleUseSmartCutNPasteCmd:
            OnToggleSmartCutNPasteOption_UpdateCommandUI (&Led_PP_TmpCmdUpdater (inCommand, outEnabled, outUsesMark, outMark, outName));
            break;
        case kToggleWrapToWindowCmd:
            OnToggleWrapToWindowOption_UpdateCommandUI (&Led_PP_TmpCmdUpdater (inCommand, outEnabled, outUsesMark, outMark, outName));
            break;
        case kToggleShowHiddenTextCmd:
            OnToggleShowHiddenTextOption_UpdateCommandUI (&Led_PP_TmpCmdUpdater (inCommand, outEnabled, outUsesMark, outMark, outName));
            break;
        default:
            inherited::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
            break;
    }
}

void LedItApplication::OnHelpMenuCommand ()
{
    try {
        FSSpec fsp;
        Led_ThrowOSErr (::FSMakeFSSpec (0, 0, "\p:LedItDocs:index.html", &fsp));
        string helpURL = Led_URLManager::Get ().FileSpecToURL (fsp);
        Led_URLManager::Get ().Open (helpURL);
    }
    catch (...) {
        DoStringyAlert (kCannotOpenHelpFileAlertID);
    }
}

void LedItApplication::UseIdleTime (const EventRecord& inMacEvent)
{
    inherited::UseIdleTime (inMacEvent);

    /*
     *  Check to see if we're too low on memory.
     */
    const float kIntervalBetweenWarnings = 10.0f;
    if (fLastLowMemWarnAt + kIntervalBetweenWarnings < ::Led_GetTickCount ()) {
        bool enufLocalMemory = true;
        bool enufMemory      = true;
        try {
            Led_CheckSomeLocalHeapRAMAvailable (8 * 1024);
        }
        catch (...) {
            enufLocalMemory = false;
        }
        try {
            ::DisposeHandle (Led_DoNewHandle (8 * 1024));
        }
        catch (...) {
            enufMemory = false;
        }

        if (not enufLocalMemory) {
            DoStringyAlert (kWarnLowLocalRAMAlertID);
            fLastLowMemWarnAt = ::Led_GetTickCount ();
        }
        if (not enufMemory) {
            DoStringyAlert (kWarnLowRAMAlertID);
            fLastLowMemWarnAt = ::Led_GetTickCount ();
        }
    }

#if qUseMacTmpMemForAllocs && 0
    // Didn't help - See SPR#0351
    float sLastSendMemBackTryAt = 0;
    if (sLastSendMemBackTryAt + 30.0f < ::Led_GetTickCount ()) {
        extern void TryToSendSomeTmpMemBackToOS ();
        TryToSendSomeTmpMemBackToOS ();
        sLastSendMemBackTryAt = ::Led_GetTickCount ();
    }
#endif
}

void LedItApplication::OpenDocument (FSSpec* inMacFSSpec)
{
    LedItDocument* doc = new LedItDocument (this, eUnknownFormat);
    try {
        doc->BuildDocWindow (inMacFSSpec);
    }
    catch (...) {
        delete doc;
        throw;
    }
}

void LedItApplication::OpenDocument (FSSpec* inMacFSSpec, FileFormat format)
{
    LedItDocument* doc = new LedItDocument (this, format);
    try {
        doc->BuildDocWindow (inMacFSSpec);
    }
    catch (...) {
        delete doc;
        throw;
    }
}

LModelObject* LedItApplication::MakeNewDocument ()
{
    LedItDocument* doc = new LedItDocument (this, eDefaultFormat);
    try {
        doc->BuildDocWindow (NULL);
    }
    catch (...) {
        delete doc;
        throw;
    }
    return doc;
}

//  Prompt the user to select a document to open
void LedItApplication::ChooseDocument ()
{
    static FilteredSFGetDLog::TypeSpec typeList[] = {
        {"HTML file", kTEXTFileType},
        {"Led Rich Text Format", kLedPrivateDocumentFileType},
        {"Microsoft Rich Text Format (RTF)", kTEXTFileType},
        {"Text file", kTEXTFileType},
    };
    FilteredSFGetDLog filteredPicker (typeList, (sizeof typeList) / (sizeof typeList[0]));

    bool       typeSpecified = false;
    size_t     typeIndex     = 0;
    FileFormat format        = eUnknownFormat;
    FSSpec     fileResult;
    if (filteredPicker.PickFile (&fileResult, &typeSpecified, &typeIndex)) {
        if (typeSpecified) {
            switch (typeIndex) {
                case 0:
                    format = eHTMLFormat;
                    break;
                case 1:
                    format = eLedPrivateFormat;
                    break;
                case 2:
                    format = eRTFFormat;
                    break;
                case 3:
                    format = eTextFormat;
                    break;
                default:
                    Led_Assert (false);
            }
        }
        OpenDocument (&fileResult, format);
    }
}

void LedItApplication::ShowAboutBox ()
{
    DoAboutBox ();
}
#endif

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
    enabler->SetChecked (Options ().GetSmartCutAndPaste ());
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
    enabler->SetChecked (Options ().GetWrapToWindow ());
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
    enabler->SetChecked (Options ().GetShowHiddenText ());
}

void LedItApplication::UpdateViewsForPrefsChange ()
{
    bool wrapToWindow   = Options ().GetWrapToWindow ();
    bool smartCutNPaste = Options ().GetSmartCutAndPaste ();
    bool showHiddenText = Options ().GetShowHiddenText ();

#if qPlatform_MacOS
    const TArray<LDocument*>&  docList = LDocument::GetDocumentList ();
    TArrayIterator<LDocument*> iterator (docList);
    LDocument*                 theDoc = NULL;
    while (iterator.Next (theDoc)) {
        AssertMember (theDoc, LedItDocument);
        LedItDocument* d = dynamic_cast<LedItDocument*> (theDoc);
        AssertNotNull (d->GetTextView ());
        d->GetTextView ()->SetSmartCutAndPasteMode (smartCutNPaste);
        d->GetTextView ()->SetWrapToWindow (wrapToWindow);
        d->GetTextView ()->SetShowHiddenText (showHiddenText);
    }
#elif qPlatform_Windows
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
#elif qXWindows
    AssertNotNull (fTextEditor);
    fTextEditor->SetSmartCutAndPasteMode (smartCutNPaste);
    fTextEditor->SetWrapToWindow (wrapToWindow);
    fTextEditor->SetShowHiddenText (showHiddenText);
#endif
}

#if qPlatform_Windows
BOOL LedItApplication::InitInstance ()
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

    // Tell Led about the kinds of embeddings we will allow
    EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();

    // Support OLE embeddings (both created from clip, and from RTF-format files)
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (LedItControlItem::kClipFormat, LedItControlItem::kEmbeddingTag, LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);
    EmbeddedObjectCreatorRegistry::Get ().AddAssoc (kBadClipFormat, RTFIO::RTFOLEEmbedding::kEmbeddingTag, LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);

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
#if !qURLStyleMarkerNewDisplayMode
    StandardURLStyleMarker::sURLPict = (const Led_DIB*)::LoadAppResource (kURLPictID, RT_BITMAP);
#endif
    StandardUnknownTypeStyleMarker::sUnknownPict          = (const Led_DIB*)::LoadAppResource (kUnknownEmbeddingPictID, RT_BITMAP);
    StandardMacPictureStyleMarker::sUnsupportedFormatPict = (const Led_DIB*)::LoadAppResource (kUnsupportedPICTFormatPictID, RT_BITMAP);

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
        Led_SDK_String       rtfDocIcon = Characters::CString::Format (Led_SDK_TCHAROF ("$EXE$,%d"), -kLedItRTFDocumentIconID);
        fileAssocHelper.Add (Win32UIFileAssociationInfo (Led_SDK_TCHAROF (".rtf"), Led_SDK_TCHAROF ("rtffile"), Led_SDK_TCHAROF ("Rich Text Document"), rtfDocIcon, Led_SDK_TCHAROF ("$EXE$ \"%1\"")));
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
    Characters::CString::Cat (directoryName, NEltsOf (directoryName), _T ("LedItDocs\\"));

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
#if qPlatform_Windows
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

    CFontDialog dlog (&lf);
    if (dlog.DoModal () == IDOK) {
        Options ().SetDefaultNewDocFont (Led_FontSpecification (*dlog.m_cf.lpLogFont));
    }
#else
    Led_Assert (false); // NYI
#endif
}

void LedItApplication::HandleBadAllocException () noexcept
{
    try {
#if qPlatform_Windows
        CDialog errorDialog (kBadAllocExceptionOnCmdDialogID);
        errorDialog.DoModal ();
#elif qPlatform_MacOS
        // ALSO, FREE ANY MEMORY WE CAN...
        TArray<LDocument*>& docList = LDocument::GetDocumentList ();
        TArrayIterator<LDocument*> iterator (docList);
        LDocument* theDoc = NULL;
        while (iterator.Next (theDoc)) {
            AssertMember (theDoc, LedItDocument);
            LedItDocument* d = dynamic_cast<LedItDocument*> (theDoc);
            d->PurgeUnneededMemory ();
        }

        DoStringyAlert (kMemoryExceptionAlertID);
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
#if qPlatform_MacOS
        DoStringyAlert (kBadUserInputExceptionAlertID);
#elif qPlatform_Windows
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
#if qPlatform_MacOS
        DoStringyAlert (kUnknownExceptionAlertID);
#elif qPlatform_Windows
        CDialog errorDialog (kUnknownExceptionOnCmdDialogID);
        errorDialog.DoModal ();
#endif
    }
    catch (...) {
        Led_BeepNotify ();
    }
}

#if qXWindows
gint LedItApplication::delete_event (GtkWidget* widget, gpointer data)
{
    // FIND ORIG GTK SAMPLE CODE AND SEE WHAT THIS USED TO GET HOOKED TO!!!!!!

    /* If you return FALSE in the "delete_event" signal handler,
     * GTK will emit the "destroy" signal. Returning TRUE means
     * you don't want the window to be destroyed.
     * This is useful for popping up 'are you sure you want to quit?'
     * type dialogs. */

    g_print ("delete event occurred\n");

    /* Change TRUE to FALSE and the main window will be destroyed with
     * a "delete_event". */

    return (TRUE);
}

void LedItApplication::xdestroy (GtkWidget* widget, gpointer data)
{
    LedItApplication* THIS = reinterpret_cast<LedItApplication*> (data);
    THIS->OnQuitCommand ();
}

void LedItApplication::OnNewDocumentCommand ()
{
    WordProcessor* wp = fTextEditor;
    TextStore&     ts = wp->GetTextStore ();
    ts.Replace (ts.GetStart (), ts.GetEnd (), LED_TCHAR_OF (""), 0);
    wp->SetDefaultFont (wp->GetStaticDefaultFont ());
    fDocument->fPathName = string ();
    UpdateFrameWindowTitle ();
}

void LedItApplication::OnOpenDocumentCommand ()
{
    LedItFilePickBox filePickerDlg (GTK_WINDOW (fAppWindow), Led_SDK_TCHAROF ("Open new document"), false, Led_SDK_String (), eUnknownFormat);
    if (filePickerDlg.DoModal ()) {
        try {
            LoadFromFile (filePickerDlg.GetFileName (), filePickerDlg.GetFileFormat ());
        }
        catch (...) {
            // should print error dialog on errors...
        }
    }
}

void LedItApplication::OnSaveDocumentCommand ()
{
#if qPrintGLIBTradeMessages
    g_message ("Entering OnSaveDocumentCommand\n");
#endif
    if (fDocument->fPathName.empty ()) {
        OnSaveAsDocumentCommand ();
    }
    else {
        Save ();
    }
}

void LedItApplication::OnSaveAsDocumentCommand ()
{
    LedItFilePickBox filePickerDlg (GTK_WINDOW (fAppWindow), Led_SDK_TCHAROF ("Save document as:"), true, fDocument->fPathName, fDocument->fFileFormat);
    if (filePickerDlg.DoModal ()) {
        try {
            SaveAs (filePickerDlg.GetFileName (), filePickerDlg.GetFileFormat ());
        }
        catch (...) {
            // should print error dialog on errors...
        }
    }
}

void LedItApplication::OnQuitCommand ()
{
#if qPrintGLIBTradeMessages
    g_message ("Entering OnQuitCommand\n");
#endif
    gtk_widget_destroy (fTextEditor->Get_GtkWidget ());
    fTextEditor = NULL;
    gtk_main_quit ();
}

void LedItApplication::LoadFromFile (const string& fileName, FileFormat fileFormat)
{
    WordProcessor* wp = fTextEditor;
    TextStore&     ts = wp->GetTextStore ();
    ts.Replace (ts.GetStart (), ts.GetEnd (), LED_TCHAR_OF (""), 0);
    wp->SetDefaultFont (wp->GetStaticDefaultFont ());
    fDocument->LoadFromFile (fileName, fileFormat);
    UpdateFrameWindowTitle ();
}

inline Led_SDK_String StripKnownSuffix (const Led_SDK_String& from)
{
    Led_SDK_String result = from;
    Led_SDK_String suffix = ExtractFileSuffix (from);
    if (suffix == ".rtf" or suffix == ".txt" or suffix == ".led" or suffix == ".htm" or suffix == ".html") {
        result.erase (result.length () - suffix.length ());
    }
    return result;
}
void LedItApplication::SaveAs (const string& fileName, FileFormat fileFormat)
{
    Require (not fileName.empty ());
    AssertNotNull (fDocument);

    fDocument->fPathName = fileName;

    /*
     *  Adjust file suffix - if needed.
     */
    {
        Led_SDK_String suffix = ExtractFileSuffix (fileName);
        switch (fileFormat) {
            case eHTMLFormat: {
                if (suffix != ".htm" and suffix != ".html") {
                    fDocument->fPathName = StripKnownSuffix (fDocument->fPathName);
                    fDocument->fPathName += ".htm";
                }
            } break;
            case eRTFFormat: {
                if (suffix != ".rtf") {
                    fDocument->fPathName = StripKnownSuffix (fDocument->fPathName);
                    fDocument->fPathName += ".rtf";
                }
            } break;
            case eLedPrivateFormat: {
                if (suffix != ".led") {
                    fDocument->fPathName = StripKnownSuffix (fDocument->fPathName);
                    fDocument->fPathName += ".led";
                }
            } break;
        }
    }
    fDocument->fFileFormat = fileFormat;
    Save ();
    UpdateFrameWindowTitle ();
}

void LedItApplication::Save ()
{
    AssertNotNull (fDocument);
    fDocument->Save ();
}

void LedItApplication::UpdateFrameWindowTitle ()
{
    AssertNotNull (fAppWindow);
    Led_SDK_String docName = Led_SDK_TCHAROF ("untitled");
    if (fDocument != NULL and not fDocument->fPathName.empty ()) {
        docName = fDocument->fPathName;
    }
    Led_SDK_String appTitle = Led_SDK_TCHAROF ("LedIt! [") + docName + Led_SDK_TCHAROF ("]");
    gtk_window_set_title (GTK_WINDOW (fAppWindow), appTitle.c_str ());
}

void LedItApplication::AppCmdDispatcher (gpointer   callback_data,
                                         guint      callback_action,
                                         GtkWidget* widget)
{
    LedItApplication* THIS = reinterpret_cast<LedItApplication*> (callback_data);
    GtkWidget*        w    = THIS->fTextEditor->Get_GtkWidget ();
    switch (callback_action) {
        case kGotoLedItWebPageCmd:
            THIS->OnGotoLedItWebPageCommand ();
            break;
        case kGotoSophistsWebPageCmd:
            THIS->OnGotoSophistsWebPageCommand ();
            break;
        case kCheckForUpdatesWebPageCmdID:
            THIS->OnCheckForUpdatesWebPageCommand ();
            break;
        case kAboutBoxCmd:
            THIS->DoAboutBox ();
            break;
        case kNewDocumentCmd:
            THIS->OnNewDocumentCommand ();
            break;
        case kOpenDocumentCmd:
            THIS->OnOpenDocumentCommand ();
            break;
        case kSaveDocumentCmd:
            THIS->OnSaveDocumentCommand ();
            break;
        case kSaveAsDocumentCmd:
            THIS->OnSaveAsDocumentCommand ();
            break;
        case kToggleUseSmartCutNPasteCmd:
            THIS->OnToggleSmartCutNPasteOptionCommand ();
            break;
        case kToggleWrapToWindowCmd:
            THIS->OnToggleWrapToWindowOptionCommand ();
            break;
        case kToggleShowHiddenTextCmd:
            THIS->OnToggleShowHiddenTextOptionCommand ();
            break;
        case kQuitCmd:
            THIS->OnQuitCommand ();
            break;
        default:
            LedItView::DispatchCommandCallback (LedItView::WidgetToTHIS (w), callback_action, widget);
    }
}

void LedItApplication::AppCmdOnInitMenu (GtkMenuItem* menuItem, gpointer callback_data)
{
    LedItApplication* THIS = reinterpret_cast<LedItApplication*> (callback_data);
    GtkWidget*        w    = THIS->fTextEditor->Get_GtkWidget ();
    {
        GtkWidget* subMenu = menuItem->submenu;
        // Iterate over each child...
        Led_Assert (GTK_IS_MENU_SHELL (subMenu));
        for (GList* i = GTK_MENU_SHELL (subMenu)->children; i != NULL; i = i->next) {
            GtkMenuItem* m = reinterpret_cast<GtkMenuItem*> (i->data);
            Led_Assert (GTK_IS_MENU_ITEM (m));
            bool bEnable = (GTK_MENU_ITEM (m)->submenu != NULL); //tmphack to test...
            gtk_widget_set_sensitive (GTK_WIDGET (m), bEnable);
            if (GTK_IS_CHECK_MENU_ITEM (m)) {
                GtkCheckMenuItem* cm = GTK_CHECK_MENU_ITEM (m);
                /*
                 *  NB: we just set the active field instead of calling gtk_check_menu_item_set_active ()
                 *  cuz for some weird reason - thats all that seems to work. I'm guessing its cuz calling the
                 *  method has other nasty side effects?? This applies to Gtk-1.2 - LGP 2001-05-22
                 */
                cm->active = false;
            }
            Led_Gtk_TmpCmdUpdater::CommandNumber commandNum = reinterpret_cast<guint> (gtk_object_get_user_data (GTK_OBJECT (m)));

            switch (commandNum) {
                case kGotoLedItWebPageCmd:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                case kGotoSophistsWebPageCmd:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                case kCheckForUpdatesWebPageCmdID:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                case kAboutBoxCmd:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                case kNewDocumentCmd:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                case kOpenDocumentCmd:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                //NO - MUST FIX!!!          case    kSaveDocumentCmd:               THIS->OnSaveDocumentCommand (); break;
                case kSaveAsDocumentCmd:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                case kToggleUseSmartCutNPasteCmd:
                    THIS->OnToggleSmartCutNPasteOption_UpdateCommandUI (CMD_ENABLER (m, commandNum));
                    break;
                case kToggleWrapToWindowCmd:
                    THIS->OnToggleWrapToWindowOption_UpdateCommandUI (CMD_ENABLER (m, commandNum));
                    break;
                case kToggleShowHiddenTextCmd:
                    THIS->OnToggleShowHiddenTextOption_UpdateCommandUI (CMD_ENABLER (m, commandNum));
                    break;
                case kQuitCmd:
                    gtk_widget_set_sensitive (GTK_WIDGET (m), true);
                    break;
                default:
                    THIS->fTextEditor->DispatchCommandUpdateCallback (m, commandNum);
            }
        }
    }
}

GtkWidget* LedItApplication::GetAppWindow () const
{
    return fAppWindow;
}

/* This is the GtkItemFactoryEntry structure used to generate new menus.
   Item 1: The menu path. The letter after the underscore indicates an
           accelerator key once the menu is open.
   Item 2: The accelerator key for the entry
   Item 3: The callback function.
   Item 4: The callback action.  This changes the parameters with
           which the function is called.  The default is 0.
   Item 5: The item type, used to define what kind of an item it is.
           Here are the possible values:

           NULL               -> "<Item>"
           ""                 -> "<Item>"
           "<Title>"          -> create a title item
           "<Item>"           -> create a simple item
           "<CheckItem>"      -> create a check item
           "<ToggleItem>"     -> create a toggle item
           "<RadioItem>"      -> create a radio item
           <path>             -> path of a radio item to link against
           "<Separator>"      -> create a separator
           "<Branch>"         -> create an item to hold sub items (optional)
           "<LastBranch>"     -> create a right justified branch
*/
GtkItemFactoryEntry LedItApplication::kMenuItemResources[] = {
    {"/_File", NULL, NULL, 0, "<Branch>"},
    {"/File/_New", "<control>N", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kNewDocumentCmd, NULL},
    {"/File/_Open...", "<control>O", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kOpenDocumentCmd, NULL},
    {"/File/_Save", "<control>S", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSaveDocumentCmd, NULL},
    {"/File/Sa_ve As...", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSaveAsDocumentCmd, NULL},
    {"/File/sep1", NULL, NULL, 0, "<Separator>"},
    {"/File/_Quit", "<control>Q", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kQuitCmd, NULL},

    {"/_Edit", NULL, NULL, 0, "<Branch>"},
    {"/Edit/_Undo", "<control>Z", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCmdUndo, NULL},
    {"/Edit/_Redo", "<control>Y", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCmdRedo, NULL},
    {"/Edit/", NULL, NULL, 0, "<Separator>"},
    {"/Edit/_Cut", "<control>X", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCmdCut, NULL},
    {"/Edit/C_opy", "<control>C", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCmdCopy, NULL},
    {"/Edit/_Paste", "<control>V", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCmdPaste, NULL},
    {"/Edit/C_lear", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCmdClear, NULL},

    {"/_Select", NULL, NULL, 0, "<Branch>"},
    {"/Select/Select _Word(s)", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectWordCmd, NULL},
    {"/Select/Select Te_xt Row(s)", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectTextRowCmd, NULL},
    {"/Select/Select _Paragraph(s)", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectParagraphCmd, NULL},
    {"/Select/", NULL, NULL, 0, "<Separator>"},
    {"/Select/Select A_ll in Cell", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectTableIntraCellAllCmd, NULL},
    {"/Select/Select Table _Cell", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectTableCellCmd, NULL},
    {"/Select/Select Table Ro_w(s)", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectTableRowCmd, NULL},
    {"/Select/Select Table C_olumn(s)", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectTableColumnCmd, NULL},
    {"/Select/Select _Table", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSelectTableCmd, NULL},
    {"/Select/", NULL, NULL, 0, "<Separator>"},
    {"/Select/Select _All", "<control>A", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCmdSelectAll, NULL},
    {"/Select/", NULL, NULL, 0, "<Separator>"},
    {"/Select/_Find...", "<control>F", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFindCmd, NULL},
    {"/Select/Find Aga_in", "<control>G", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFindAgainCmd, NULL},
    {"/Select/_Enter 'Find' String", "<control>E", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kEnterFindStringCmd, NULL},
    {"/Select/_Replace...", "<control>H", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kReplaceCmd, NULL},
    {"/Select/Replace Agai_n", "<control>R", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kReplaceAgainCmd, NULL},
    {"/Select/", NULL, NULL, 0, "<Separator>"},
    {"/Select/_Check Spelling...", "<control>L", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSpellCheckCmd, NULL},

    {"/_Insert", NULL, NULL, 0, "<Branch>"},
    {"/Insert/Insert _Table", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kInsertTableCmd, NULL},
    {"/Insert/Insert Table _Row Above", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kInsertTableRowAboveCmd, NULL},
    {"/Insert/Insert Table R_ow Below", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kInsertTableRowBelowCmd, NULL},
    {"/Insert/Insert Table _Column Before", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kInsertTableColBeforeCmd, NULL},
    {"/Insert/Insert Table Co_lumn After", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kInsertTableColAfterCmd, NULL},
    {"/Insert/", NULL, NULL, 0, "<Separator>"},
    {"/Insert/Insert _URL", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kInsertURLCmd, NULL},

    {"/_Format", NULL, NULL, 0, "<Branch>"},
    {"/_Format/Font _Name", NULL, NULL, 0, "<Branch>"},
    {"/_Format/Font _Style", NULL, NULL, 0, "<Branch>"},
    {"/_Format/Font _Style/_Plain", "<control>T", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontStylePlainCmd, "<CheckItem>"},
    {"/_Format/Font _Style/sep1", NULL, NULL, 0, "<Separator>"},
    {"/_Format/Font _Style/_Bold", "<control>B", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontStyleBoldCmd, "<CheckItem>"},
    {"/_Format/Font _Style/_Italic", "<control>I", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontStyleItalicCmd, "<CheckItem>"},
    {"/_Format/Font _Style/_Underline", "<control>U", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontStyleUnderlineCmd, "<CheckItem>"},
    {"/_Format/Font _Style/_Subscript", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSubScriptCmd, "<CheckItem>"},
    {"/_Format/Font _Style/Supe_rscript", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSuperScriptCmd, "<CheckItem>"},
    {"/_Format/Font Si_ze", NULL, NULL, 0, "<Branch>"},
    {"/_Format/Font Si_ze/9", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize9Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/10", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize10Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/12", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize12Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/14", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize14Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/18", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize18Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/24", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize24Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/36", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize36Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/48", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize48Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/72", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSize72Cmd, "<CheckItem>"},
    {"/_Format/Font Si_ze/sep1", NULL, NULL, 0, "<Separator>"},
    {"/_Format/Font Si_ze/_Smaller", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSizeSmallerCmd, NULL},
    {"/_Format/Font Si_ze/_Larger", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSizeLargerCmd, NULL},
    {"/_Format/Font Si_ze/sep2", NULL, NULL, 0, "<Separator>"},
    {"/_Format/Font Si_ze/_Other...", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontSizeOtherCmd, "<CheckItem>"},
    {"/_Format/Font _Color", NULL, NULL, 0, "<Branch>"},
    {"/_Format/Font _Color/_Black", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kBlackColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Maroon", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kMaroonColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Green", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kGreenColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Olive", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kOliveColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Navy", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kNavyColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Purple", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kPurpleColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Teal", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kTealColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Gray", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kGrayColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Silver", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kSilverColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Red", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kRedColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Lime", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kLimeColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Yellow", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kYellowColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/Bl_ue", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kBlueColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Fuchsia", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFuchsiaColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_Aqua", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kAquaColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/_White", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kWhiteColorCmd, "<CheckItem>"},
    {"/_Format/Font _Color/sep", NULL, NULL, 0, "<Separator>"},
    {"/_Format/Font _Color/O_ther...", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kFontColorOtherCmd, "<CheckItem>"},
    {"/_Format/_Choose Font Dialog...", "<control>D", GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kChooseFontDialogCmd, NULL},
    {"/_Format/sep", NULL, NULL, 0, "<Separator>"},
    {"/_Format/_Justification", NULL, NULL, 0, "<Branch>"},
    {"/_Format/_Justification/_Left", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kJustifyLeftCmd, "<CheckItem>"},
    {"/_Format/_Justification/_Center", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kJustifyCenterCmd, "<CheckItem>"},
    {"/_Format/_Justification/_Right", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kJustifyRightCmd, "<CheckItem>"},
    {"/_Format/_Justification/_Full", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kJustifyFullCmd, "<CheckItem>"},

    {"/_Remove/", NULL, NULL, 0, "<Separator>"},
    {"/_Remove/Remove _Rows", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kRemoveTableRowsCmd, NULL},
    {"/_Remove/Remove _Columns", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kRemoveTableColumnsCmd, NULL},
    {"/_Remove/sep", NULL, NULL, 0, "<Separator>"},
    {"/_Remove/_Hide Selection", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kHideSelectionCmd, NULL},
    {"/_Remove/_Unhide Selection", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kUnHideSelectionCmd, NULL},

    {"/_Options", NULL, NULL, 0, "<Branch>"},
    {"/Options/_Smart Cut and Paste", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kToggleUseSmartCutNPasteCmd, "<CheckItem>"},
    {"/Options/_Wrap to Window", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kToggleWrapToWindowCmd, "<CheckItem>"},
    {"/Options/Show _Hidden Text", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kToggleShowHiddenTextCmd, "<CheckItem>"},
    {"/Options/_Paragraph Markers Shown", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kShowHideParagraphGlyphsCmd, "<CheckItem>"},
    {"/Options/_Tab Markers Shown", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kShowHideTabGlyphsCmd, "<CheckItem>"},
    {"/Options/Spa_ce characters Shown", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kShowHideSpaceGlyphsCmd, "<CheckItem>"},

    {"/_Help", NULL, NULL, 0, "<LastBranch>"},
    {"/_Help/Goto _LedIt! Web Page", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kGotoLedItWebPageCmd, NULL},
    {"/_Help/Goto _Sophist Solutions Web Page", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kGotoSophistsWebPageCmd, NULL},
    {"/_Help/_Check for LedIt! Web Updates", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kCheckForUpdatesWebPageCmdID, NULL},
    {"/_Help/sep", NULL, NULL, 0, "<Separator>"},
    {"/_Help/_About", NULL, GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher), kAboutBoxCmd, NULL},
};

static void RecursivelySetCallback (GtkWidget* menu, gpointer data)
{
    if (GTK_IS_MENU_ITEM (menu)) {
        GtkMenuItem* m = GTK_MENU_ITEM (menu);
        if (m->submenu != NULL) {
            gtk_signal_connect (GTK_OBJECT (menu), "activate", GTK_SIGNAL_FUNC (LedItApplication::AppCmdOnInitMenu), data);
            RecursivelySetCallback (m->submenu, data);
        }
    }
    if (GTK_IS_CONTAINER (menu)) {
        GtkContainer* c = GTK_CONTAINER (menu);
        gtk_container_foreach (c, RecursivelySetCallback, data);
    }
}

GtkWidget* LedItApplication::get_main_menu (GtkWidget* window)
{
    GtkAccelGroup* accel_group = gtk_accel_group_new ();

    /* This function initializes the item factory.
     Param 1: The type of menu - can be GTK_TYPE_MENU_BAR, GTK_TYPE_MENU,
              or GTK_TYPE_OPTION_MENU.
     Param 2: The path of the menu.
     Param 3: A pointer to a gtk_accel_group.  The item factory sets up
              the accelerator table while generating menus.
    */

    GtkItemFactory* item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", accel_group);

    /* This function generates the menu items. Pass the item factory,
     the number of items in the array, the array itself, and any
     callback data for the the menu items. */
    gtk_item_factory_create_items (item_factory, Led_NEltsOf (kMenuItemResources), kMenuItemResources, this);

    /* Attach the new accelerator group to the window. */
    gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

    {
        const string           kFontMenuPath = "/Format/Font Name";
        GtkWidget*             fontNameMenu  = gtk_item_factory_get_widget (item_factory, kFontMenuPath.c_str ());
        vector<Led_SDK_String> fontNames     = LedItApplication::Get ().fInstalledFonts.GetUsableFontNames ();
        for (vector<Led_SDK_String>::const_iterator i = fontNames.begin (); i != fontNames.end (); ++i) {
            GtkItemFactoryEntry entry;
            memset (&entry, 0, sizeof (entry));
            string fontName       = *i; // really should munge it so its assured not to have any bad chars in it!!!
            string tmpPath        = kFontMenuPath + "/" + fontName;
            entry.path            = const_cast<char*> (tmpPath.c_str ());
            entry.callback        = GTK_SIGNAL_FUNC (LedItApplication::AppCmdDispatcher);
            entry.callback_action = (i - fontNames.begin ()) + kBaseFontNameCmd;
            entry.item_type       = "<CheckItem>";
            gtk_item_factory_create_items (item_factory, 1, &entry, this);
            {
                // Mark the newly created item with a CMDNUM so it will work with the CMD_UPDATE code...
                GtkWidget* justCreatedItem = gtk_item_factory_get_widget (item_factory, entry.path);
                AssertNotNull (justCreatedItem);
                gtk_object_set_user_data (GTK_OBJECT (justCreatedItem), reinterpret_cast<void*> (entry.callback_action));
            }
        }
    }

    // Walk through the list of GtkMenuItem* widgets created by the last gtk_item_factory, and set the widgets USERDATA field
    // to be the command# - for use later on in the UPDATE_COMMAND processing
    {
        for (size_t i = 0; i < Led_NEltsOf (kMenuItemResources); ++i) {
            string path = kMenuItemResources[i].path;
            {
                // strip out _'s
                for (size_t idx; (idx = path.find ('_')) != string::npos;) {
                    path.erase (idx, 1);
                }
            }

            GtkWidget* aMenu = gtk_item_factory_get_widget (item_factory, path.c_str ());
            AssertNotNull (aMenu);
            gtk_object_set_user_data (GTK_OBJECT (aMenu), reinterpret_cast<void*> (kMenuItemResources[i].callback_action));
        }
    }

    /* Finally, return the actual menu bar created by the item factory. */
    GtkWidget* menuBar = gtk_item_factory_get_widget (item_factory, "<main>");
    RecursivelySetCallback (menuBar, this);
    return menuBar;
}
#endif

#if qPlatform_Windows
const vector<Led_SDK_String>& LedItApplication::GetUsableFontNames ()
{
    return fInstalledFonts.GetUsableFontNames ();
}

void LedItApplication::FixupFontMenu (CMenu* fontMenu)
{
    AssertMember (fontMenu, CMenu);
    const vector<Led_SDK_String>& fontNames = GetUsableFontNames ();

    // delete all menu items
    while (fontMenu->DeleteMenu (0, MF_BYPOSITION) != 0) {
        ;
    }

    for (size_t i = 0; i < fontNames.size (); i++) {
        int cmdNum = kBaseFontNameCmd + i;
        fontMenu->AppendMenu (MF_STRING, cmdNum, fontNames[i].c_str ());
    }
}

Led_SDK_String LedItApplication::CmdNumToFontName (UINT cmdNum)
{
    const vector<Led_SDK_String>& fontNames = GetUsableFontNames ();
    return (fontNames[cmdNum - kBaseFontNameCmd]);
}
#endif

#if qPlatform_Windows
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

#if qPlatform_Windows
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

inline Led_SDK_String GetLongPathName (const Led_SDK_String& pathName)
{
    TCHAR szPath[_MAX_PATH];
    Require (pathName.length () < _MAX_PATH);
    Characters::CString::Copy (szPath, NEltsOf (szPath), pathName.c_str ());
    WIN32_FIND_DATA fileData;
    HANDLE          hFind = ::FindFirstFile (szPath, &fileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        TCHAR* lastSlash = ::_tcsrchr (szPath, '\\');
        if (lastSlash != NULL) {
            *lastSlash = '\0';
        }
        Characters::CString::Cat (szPath, NEltsOf (szPath), _T ("\\"));
        Characters::CString::Cat (szPath, NEltsOf (szPath), fileData.cFileName);
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
    Characters::CString::Copy (szTemp, NEltsOf (szTemp), lpszFileName);
    LPTSTR lpszLast = _tcsrchr (szTemp, '\"');
    if (lpszLast != NULL)
        *lpszLast = 0;
    AfxFullPath (szPath, szTemp);
    TCHAR szLinkName[_MAX_PATH];
    if (AfxResolveShortcut (AfxGetMainWnd (), szPath, szLinkName, _MAX_PATH))
        Characters::CString::Copy (szPath, NEltsOf (szPath), szLinkName);

    // Also, to fix SPR#0345, we must use this (or SHGetFileInfo) hack
    // to get the long-file-name version of the file name.
    Characters::CString::Copy (szPath, NEltsOf (szPath), GetLongPathName (szPath).c_str ());

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

void LedItDocManager::RegisterShellFileType (bool bWin95, CString strPathName, int iconIndexInFile, CString strFilterExt, CString strFileTypeId, CString strFileTypeName)
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
