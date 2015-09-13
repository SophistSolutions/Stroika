/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItApplication_h__
#define	__LedLineItApplication_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItApplication.h,v 2.20 2003/12/11 03:09:11 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItApplication.h,v $
 *	Revision 2.20  2003/12/11 03:09:11  lewis
 *	SPR#1585: move spellcheckengine object to application. Use qIncludeBasicSpellcheckEngine define to control its inclusion. Define default locaiton for LedLineIt spell dict.
 *	
 *	Revision 2.19  2003/03/20 17:07:15  lewis
 *	SPR#1360 - use new IdleManager instead of old OnEnterIdle
 *	
 *	Revision 2.18  2003/03/11 02:31:59  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 2.17  2002/09/04 02:36:08  lewis
 *	if _MFC_VER>0x700(VC.NET), then override WinHelpInternal() instead of WinHelp().
 *	AddDocTemplateForString() call change from .txt to .ledtxt as main declared file
 *	type since MFC writes stuff (EXE name) that conflicts with what we write for that suffix
 *	
 *	Revision 2.16  2002/05/06 21:31:13  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.15  2001/11/27 00:28:19  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.14  2001/09/17 14:43:06  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 2.13  2001/09/12 00:22:32  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 2.12  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.11  2000/06/08 22:24:06  lewis
 *	SPR#0775. Override ProcessShellCommand() to properly handle OpenFile args - when more than one
 *	
 *	Revision 2.10  2000/06/08 21:49:49  lewis
 *	SPR#0774- added option to treat TAB as indent-selection command - as MSDEV does
 *	
 *	Revision 2.9  1999/12/25 04:15:51  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it
 *	worked properly turning on/off and having multiple windows onto the same doc at
 *	the same time (scrolled to same or differnet) places
 *	
 *	Revision 2.8  1999/02/22 13:12:03  lewis
 *	add Options module so we can preserve in registry flags like autoindent and smartcutandpaste
 *	
 *	Revision 2.7  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.5  1997/07/23  23:18:11  lewis
 *	override OnIdle() to notify Led.
 *
 *	Revision 2.4  1997/07/14  14:59:03  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.3  1997/03/23  01:06:52  lewis
 *	HandleBadAllocException
 *
 *	Revision 2.2  1997/03/04  20:18:25  lewis
 *	enw commands, new options
 *
 *	Revision 2.1  1997/01/20  05:39:05  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 *
 *
 *
 */

#include	<afxole.h>

#include	"LedSupport.h"
#include	"TextStore.h"

#include	"LedLineItConfig.h"

#if		qIncludeBasicSpellcheckEngine
	#include	"SpellCheckEngine_Basic.h"
#endif


// Later put into prefs object!
extern	bool	gSupportAutoIdent;

class	LedLineItApplication : public CWinApp {
	private:
		typedef	CWinApp	inherited;

	public:
		LedLineItApplication ();
		~LedLineItApplication ();

	public:
		static	LedLineItApplication&	Get ();
	private:
		static	LedLineItApplication*	sThe;

	public:
		override	BOOL	InitInstance ();

#if		qIncludeBasicSpellcheckEngine
	public:
		SpellCheckEngine_Basic_Simple			fSpellCheckEngine;
#endif

	public:
		#if		_MFC_VER >= 0x0700
			override	void	WinHelpInternal (DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
		#else
			override	void	WinHelp (DWORD dwData, UINT nCmd = HELP_CONTEXT);
		#endif

	// handle exceptions....
	public:
		override	BOOL	PumpMessage ();

		nonvirtual	void	HandleMFCException (CException* e) throw ();
		nonvirtual	void	HandleBadAllocException () throw ();
		nonvirtual	void	HandleBadUserInputException () throw ();
		nonvirtual	void	HandleUnknownException () throw ();


	private:
		nonvirtual	void	AddDocTemplateForString (const char* tmplStr, bool connectToServer);

		COleTemplateServer fOleTemplateServer;


	private:
		nonvirtual	BOOL	ProcessShellCommand (CCommandLineInfo& rCmdInfo);

	private:
		afx_msg	void	OnAppAbout ();
		afx_msg	void	OnGotoLedLineItWebPageCommand ();
		afx_msg	void	OnGotoSophistsWebPageCommand ();
		afx_msg	void	OnCheckForUpdatesWebPageCommand ();
		afx_msg	void	OnToggleAutoIndentOptionCommand ();
		afx_msg	void	OnToggleAutoIndentOptionUpdateCommandUI (CCmdUI* pCmdUI);
		afx_msg	void	OnToggleTreatTabAsIndentCharOptionCommand ();
		afx_msg	void	OnToggleTreatTabAsIndentCharOptionUpdateCommandUI (CCmdUI* pCmdUI);
		afx_msg	void	OnToggleSmartCutNPasteOptionCommand ();
		afx_msg	void	OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI);
#if		qSupportSyntaxColoring
		afx_msg	void	OnSyntaxColoringOptionCommand (UINT cmdNum);
		afx_msg	void	OnSyntaxColoringOptionUpdateCommandUI (CCmdUI* pCmdUI);
#endif
		afx_msg	void	OnChooseDefaultFontCommand ();

	private:
		nonvirtual	void	UpdateViewsForPrefsChange ();

	private:
		DECLARE_MESSAGE_MAP()
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif	/*__LedLineItApplication_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

