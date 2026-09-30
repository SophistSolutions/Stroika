/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

#ifndef __LedItApplication_h__
#define __LedItApplication_h__ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <string>
#include <vector>

#if qStroika_Platform_Windows

#include "Stroika/Foundation/Execution/Platform/Windows/COM.h"

DISABLE_COMPILER_MSC_WARNING_START (5054)
#include <afxole.h>
DISABLE_COMPILER_MSC_WARNING_END (5054)
#endif

#include "Stroika/Foundation/Execution/Logger.h"

#include "Stroika/Frameworks/Led/GDI.h"
#include "Stroika/Frameworks/Led/Support.h"
#include "Stroika/Frameworks/Led/TextStore.h"

#include "LedItConfig.h"

#if qIncludeBasicSpellcheckEngine
#include "Stroika/Frameworks/Led/SpellCheckEngine_Basic.h"
#endif

#if qStroika_Platform_Windows
#include "Stroika/Frameworks/Led/Platform/MFC.h"
#endif

#if qStroika_Platform_Windows
class CMenu;
#endif

class LedItDocument;
class LedItView;

#if qStroika_Platform_Windows
using CMD_ENABLER = Platform::Led_MFC_TmpCmdUpdater;
#endif

class LedItApplication
#if qStroika_Platform_Windows
    : public CWinApp
#endif
{
private:
#if qStroika_Platform_Windows
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
    shared_ptr<SpellCheckEngine_Basic_Simple> fSpellCheckEngine;
#endif

#if qStroika_Platform_Windows
public:
    nonvirtual void      FixupFontMenu (CMenu* fontMenu);
    nonvirtual SDKString CmdNumToFontName (UINT cmdNum);
    nonvirtual const vector<SDKString>& GetUsableFontNames (); // perform whatever filtering will be done on sys installed fonts and return the names
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

#if qStroika_Platform_Windows
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

#if qStroika_Platform_Windows
private:
    Execution::Platform::Windows::COMInitializer fCOMInitializer_{COINIT_APARTMENTTHREADED};
#endif
private:
    Execution::Logger::Activator fLogMgrActivator_;
#if qStroika_Platform_Windows
public:
    InstalledFonts fInstalledFonts; // Keep a static copy for speed, and so font#s are static throughout the life of the applet
#endif
};

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#endif /*__LedItApplication_h__*/
