/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef __LedLineItApplication_h__
#define __LedLineItApplication_h__  1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include    <afxole.h>

#include    "Stroika/Frameworks/Led/Support.h"
#include    "Stroika/Frameworks/Led/TextStore.h"

#include    "LedLineItConfig.h"

#if     qIncludeBasicSpellcheckEngine
#include    "Stroika/Frameworks/Led/SpellCheckEngine_Basic.h"
#endif


// Later put into prefs object!
extern  bool    gSupportAutoIdent;

class   LedLineItApplication : public CWinApp {
private:
    typedef CWinApp inherited;

public:
    LedLineItApplication ();
    ~LedLineItApplication ();

public:
    static  LedLineItApplication&   Get ();
private:
    static  LedLineItApplication*   sThe;

public:
    override    BOOL    InitInstance ();

#if     qIncludeBasicSpellcheckEngine
public:
    SpellCheckEngine_Basic_Simple           fSpellCheckEngine;
#endif

public:
#if     _MFC_VER >= 0x0700
    override    void    WinHelpInternal (DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
#else
    override    void    WinHelp (DWORD dwData, UINT nCmd = HELP_CONTEXT);
#endif

    // handle exceptions....
public:
    override    BOOL    PumpMessage ();

    nonvirtual  void    HandleMFCException (CException* e) throw ();
    nonvirtual  void    HandleBadAllocException () throw ();
    nonvirtual  void    HandleBadUserInputException () throw ();
    nonvirtual  void    HandleUnknownException () throw ();


private:
    nonvirtual  void    AddDocTemplateForString (const char* tmplStr, bool connectToServer);

    COleTemplateServer fOleTemplateServer;


private:
    nonvirtual  BOOL    ProcessShellCommand (CCommandLineInfo& rCmdInfo);

private:
    afx_msg void    OnAppAbout ();
    afx_msg void    OnGotoLedLineItWebPageCommand ();
    afx_msg void    OnGotoSophistsWebPageCommand ();
    afx_msg void    OnCheckForUpdatesWebPageCommand ();
    afx_msg void    OnToggleAutoIndentOptionCommand ();
    afx_msg void    OnToggleAutoIndentOptionUpdateCommandUI (CCmdUI* pCmdUI);
    afx_msg void    OnToggleTreatTabAsIndentCharOptionCommand ();
    afx_msg void    OnToggleTreatTabAsIndentCharOptionUpdateCommandUI (CCmdUI* pCmdUI);
    afx_msg void    OnToggleSmartCutNPasteOptionCommand ();
    afx_msg void    OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI);
#if     qSupportSyntaxColoring
    afx_msg void    OnSyntaxColoringOptionCommand (UINT cmdNum);
    afx_msg void    OnSyntaxColoringOptionUpdateCommandUI (CCmdUI* pCmdUI);
#endif
    afx_msg void    OnChooseDefaultFontCommand ();

private:
    nonvirtual  void    UpdateViewsForPrefsChange ();

private:
    DECLARE_MESSAGE_MAP()
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif  /*__LedLineItApplication_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

