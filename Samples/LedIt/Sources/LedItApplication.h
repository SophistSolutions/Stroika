/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */

#ifndef __LedItApplication_h__
#define __LedItApplication_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <string>
#include <vector>

#if qPlatform_MacOS
#include <Dialogs.h>

#include <LDocApplication.h>
#elif qPlatform_Windows
#pragma warning(push)
#pragma warning(disable : 5054)
#include <afxole.h>
#pragma warning(pop)
#elif qXWindows
#include <gtk/gtk.h>
#endif

#include "Stroika/Frameworks/Led/GDI.h"
#include "Stroika/Frameworks/Led/Support.h"
#include "Stroika/Frameworks/Led/TextStore.h"

#include "LedItConfig.h"

#if qIncludeBasicSpellcheckEngine
#include "Stroika/Frameworks/Led/SpellCheckEngine_Basic.h"
#endif

#if qPlatform_MacOS
#include "Stroika/Frameworks/Led/Platform/Led_PP.h"
#elif qPlatform_Windows
#include "Stroika/Frameworks/Led/Platform/MFC.h"
#elif qXWindows
#include "Stroika/Frameworks/Led/Platform/Gtk.h"
#endif

#if qPlatform_Windows
class CMenu;
#endif

class LedItDocument;
class LedItView;

#if qPlatform_MacOS
using CMD_ENABLER = Platform::Led_PP_TmpCmdUpdater;
#elif qPlatform_Windows
using CMD_ENABLER = Platform::Led_MFC_TmpCmdUpdater;
#elif qXWindows
using CMD_ENABLER = Platform::Led_Gtk_TmpCmdUpdater;
#endif

class LedItApplication
#if qPlatform_MacOS
    : public LDocApplication
#elif qPlatform_Windows
    : public CWinApp
#endif
{
private:
#if qPlatform_MacOS
    using inherited = LDocApplication;
#elif qPlatform_Windows
    using inherited = CWinApp;
#endif

public:
    LedItApplication ();
    virtual ~LedItApplication ();

public:
    static LedItApplication& Get ();

private:
    static LedItApplication* sThe;

#if qIncludeBasicSpellcheckEngine
public:
    SpellCheckEngine_Basic_Simple fSpellCheckEngine;
#endif

#if qPlatform_Windows
public:
    nonvirtual void FixupFontMenu (CMenu* fontMenu);
    nonvirtual Led_SDK_String CmdNumToFontName (UINT cmdNum);
    nonvirtual const vector<Led_SDK_String>& GetUsableFontNames (); // perform whatever filtering will be done on sys installed fonts and return the names
#endif

protected:
    nonvirtual void OnToggleSmartCutNPasteOptionCommand ();
    nonvirtual void OnToggleSmartCutNPasteOption_UpdateCommandUI (CMD_ENABLER* enabler);
    nonvirtual void OnToggleWrapToWindowOptionCommand ();
    nonvirtual void OnToggleWrapToWindowOption_UpdateCommandUI (CMD_ENABLER* enabler);
    nonvirtual void OnToggleShowHiddenTextOptionCommand ();
    nonvirtual void OnToggleShowHiddenTextOption_UpdateCommandUI (CMD_ENABLER* enabler);

private:
    nonvirtual void UpdateViewsForPrefsChange ();

#if qPlatform_Windows
public:
    virtual BOOL InitInstance () override;

public:
#if _MFC_VER >= 0x0700
    virtual void WinHelpInternal (DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT) override;
#else
    vitual void WinHelp (DWORD dwData, UINT nCmd = HELP_CONTEXT) override;
#endif

    // handle exceptions....
public:
    virtual BOOL PumpMessage () override;

    nonvirtual void HandleMFCException (CException* e) noexcept;
    nonvirtual void HandleHRESULTException (HRESULT hr) noexcept;
#if 0
public:
    virtual    BOOL    OnIdle (LONG lCount) override;
#endif

private:
    nonvirtual void AddDocTemplateForString (const char* tmplStr, bool connectToServer);

    COleTemplateServer fOleTemplateServer;

public:
    virtual BOOL ProcessShellCommand (CCommandLineInfo& rCmdInfo);

protected:
    afx_msg void OnAppAbout ();
    afx_msg void OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI);
    afx_msg void OnToggleWrapToWindowOptionUpdateCommandUI (CCmdUI* pCmdUI);
    afx_msg void OnToggleShowHiddenTextOptionUpdateCommandUI (CCmdUI* pCmdUI);

private:
    DECLARE_MESSAGE_MAP ()
#endif

protected:
    nonvirtual void OnChooseDefaultFontCommand ();

    // handle exceptions....
public:
    nonvirtual void HandleBadAllocException () noexcept;
    nonvirtual void HandleBadUserInputException () noexcept;
    nonvirtual void HandleUnknownException () noexcept;

public:
    nonvirtual void DoAboutBox ();
    nonvirtual void OnGotoLedItWebPageCommand ();
    nonvirtual void OnGotoSophistsWebPageCommand ();
    nonvirtual void OnCheckForUpdatesWebPageCommand ();

#if qPlatform_MacOS
public:
    virtual void MakeMenuBar () override;
    virtual void StartUp () override;
    virtual void ProcessNextEvent () override;
    virtual void HandleAppleEvent (const AppleEvent& inAppleEvent, AppleEvent& outAEReply, AEDesc& outResult, long inAENumber) override;

protected:
    nonvirtual void HandleMacOSException (OSErr err);
    nonvirtual void HandlePowerPlantException (ExceptionCode err);

public:
    virtual void ShowAboutBox () override;

    virtual Boolean ObeyCommand (CommandT inCommand, void* ioParam = nil) override;
    virtual void    FindCommandStatus (CommandT inCommand,
                                       Boolean& outEnabled, Boolean& outUsesMark,
                                       UInt16& outMark, Str255 outName) override;

public:
    nonvirtual void OnHelpMenuCommand ();

private:
    short fHelpMenuItem;

private:
    short fGotoLedItWebPageMenuItem;

private:
    short fGotoSophistsWebPageMenuItem;

private:
    short fCheckForUpdatesWebPageMenuItem;

public:
    virtual void UseIdleTime (const EventRecord& inMacEvent) override;

private:
    float fLastLowMemWarnAt;

public:
    virtual void          OpenDocument (FSSpec* inMacFSSpec) override;
    virtual void          OpenDocument (FSSpec* inMacFSSpec, FileFormat format) override;
    virtual LModelObject* MakeNewDocument () override;

public:
    virtual void ChooseDocument () override;

private:
    static pascal Boolean SFGetDlgModalFilter (DialogPtr dialog, EventRecord* theEvent, short* itemHit, void* myData);
    static pascal short   SFGetDlgHook (short item, DialogPtr dialog, void* myData);
#endif

#if qXWindows
private:
    nonvirtual GtkWidget* get_main_menu (GtkWidget* window);

public:
    static gint delete_event (GtkWidget* widget, gpointer data);
    static void xdestroy (GtkWidget* widget, gpointer data);

private:
    LedItDocument* fDocument;

public:
    nonvirtual void OnNewDocumentCommand ();
    nonvirtual void OnOpenDocumentCommand ();
    nonvirtual void OnSaveDocumentCommand ();
    nonvirtual void OnSaveAsDocumentCommand ();
    nonvirtual void OnQuitCommand ();

public:
    nonvirtual void LoadFromFile (const string& fileName, FileFormat fileFormat);
    nonvirtual void SaveAs (const string& fileName, FileFormat fileFormat);
    nonvirtual void Save ();

private:
    nonvirtual void UpdateFrameWindowTitle ();

public:
    static void AppCmdDispatcher (gpointer   callback_data,
                                  guint      callback_action,
                                  GtkWidget* widget);
    static void AppCmdOnInitMenu (GtkMenuItem* menuItem, gpointer callback_data);

public:
    nonvirtual GtkWidget* GetAppWindow () const;

private:
    GtkWidget* fAppWindow;
    LedItView* fTextEditor;

private:
    static GtkItemFactoryEntry kMenuItemResources[];
#endif
#if qPlatform_Windows || qXWindows
public:
    Led_InstalledFonts fInstalledFonts; // Keep a static copy for speed, and so font#s are static throughout the life of the applet
#endif
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#endif /*__LedItApplication_h__*/
