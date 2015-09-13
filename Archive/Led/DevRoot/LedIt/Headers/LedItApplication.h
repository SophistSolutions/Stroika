/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItApplication_h__
#define	__LedItApplication_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItApplication.h,v 1.20 2003/12/09 21:04:57 lewis Exp $
 *
 * Changes:
 *	$Log: LedItApplication.h,v $
 *	Revision 1.20  2003/12/09 21:04:57  lewis
 *	use new SpellCheckEngine_Basic_Simple class and call fSpellCheckEngine.SetUserDictionary to specify UD name
 *	
 *	Revision 1.19  2003/11/30 01:59:27  lewis
 *	SPR#1572: override CWinApp::ProcessShellCommand () to detect if a failed file open (on startup)
 *	was becuase the file didn't exist - and if so - create the empty doc - with that assosciated file
 *	name (so when you do a save - it goes by default to that name). Modify Title to say {new} so its
 *	clear it wasn't found. ALso - fixed CSingleDocTemplate::OpenDocumentFIle() code so it properly
 *	handles exceptions.
 *	
 *	Revision 1.18  2003/03/20 16:05:24  lewis
 *	SPR#1360 - get rid of EnterIdle/OnIdle support
 *	
 *	Revision 1.17  2003/03/11 02:31:10  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 1.16  2002/09/04 02:20:52  lewis
 *	if _MFC_VER>=0x700 then override WinHelpInternal() instead
 *	
 *	Revision 1.15  2002/05/06 21:30:57  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.14  2001/11/27 00:28:08  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.13  2001/09/17 14:42:16  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 1.12  2001/09/12 00:21:55  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 1.11  2001/08/29 22:59:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.10  2001/07/19 02:22:38  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support.
 *	
 *	Revision 1.9  2001/06/05 15:44:06  lewis
 *	SPR#0950- enable Find dialog for X-Windows/Gtk. Also fixed some small find bugs on Mac/Win
 *	that were introduced by merging of code, and some cleanups enabled by merging
 *	
 *	Revision 1.8  2001/06/05 13:35:38  lewis
 *	SPR#0949- Support popup in XWindows file picker to select file type, and adjust file suffixes
 *	(and for default filetype(autoguess) use file suffixes to guess).
 *	
 *	Revision 1.7  2001/06/04 20:55:02  lewis
 *	LedItApplication::UpdateFrameWindowTitle () to implement SPR#0932- update titlebar to
 *	say filename for XWindows
 *	
 *	Revision 1.6  2001/06/04 19:32:14  lewis
 *	SPR#0931 - use StdFilePickBox for Linux
 *	
 *	Revision 1.5  2001/05/22 21:44:32  lewis
 *	MAJOR changes - now supporting checked menu items on X-Windows and enabling (SPR#0924).
 *	Also, much improved dialog code - including dialog for aboutbox, font/color pickers etc (SPR#0923)
 *	
 *	Revision 1.4  2001/05/18 23:00:11  lewis
 *	small cleanups - and gotoWebPage(help menu) support for XWindows
 *	
 *	Revision 1.3  2001/05/18 21:08:29  lewis
 *	About box code cleanups/sharing and support for Linux
 *	
 *	Revision 1.2  2001/05/16 16:03:17  lewis
 *	more massive changes. Make Options () stuff portable (though fake impl om mac/X). Got rid of
 *	AppWindow class - and now X-WIndows fully using LedDoc/LedView. Much more common code between
 *	implementations - but still a lot todo
 *	
 *	Revision 1.1  2001/05/14 20:54:43  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *
 *
 *
 *
 *	<<<***		Based on LedItMFC project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */

#include	<vector>
#include	<string>


#if		qMacOS
	#include	<Dialogs.h>

	#include	<LDocApplication.h>
#elif	qWindows
	#include	<afxole.h>
#elif	qXWindows
	#include	<gtk/gtk.h>
#endif

#include	"LedGDI.h"
#include	"LedSupport.h"
#include	"TextStore.h"

#include	"LedItConfig.h"

#if		qIncludeBasicSpellcheckEngine
	#include	"SpellCheckEngine_Basic.h"
#endif

#if		qMacOS
	#include	"Led_PP.h"
#elif	qWindows
	#include	"Led_MFC.h"
#elif	qXWindows
	#include	"Led_Gtk.h"
#endif


#if		qWindows
class	CMenu;
#endif

class	LedItDocument;
class	LedItView;


#if		qMacOS
	typedef	Led_PP_TmpCmdUpdater	CMD_ENABLER;
#elif	qWindows
	typedef	Led_MFC_TmpCmdUpdater	CMD_ENABLER;
#elif	qXWindows
	typedef	Led_Gtk_TmpCmdUpdater	CMD_ENABLER;
#endif




class	LedItApplication
#if		qMacOS
	: public LDocApplication
#elif	qWindows
	: public CWinApp
#endif
{
	private:
		#if		qMacOS
			typedef	LDocApplication	inherited;
		#elif	qWindows
			typedef	CWinApp			inherited;
		#endif

	public:
		LedItApplication ();
		virtual ~LedItApplication ();

	public:
		static	LedItApplication&	Get ();
	private:
		static	LedItApplication*	sThe;


	#if		qIncludeBasicSpellcheckEngine
	public:
		SpellCheckEngine_Basic_Simple	fSpellCheckEngine;
	#endif

#if		qWindows
	public:
		nonvirtual	void							FixupFontMenu (CMenu* fontMenu);
		nonvirtual	Led_SDK_String					CmdNumToFontName (UINT cmdNum);
		nonvirtual	const vector<Led_SDK_String>&	GetUsableFontNames ();	// perform whatever filtering will be done on sys installed fonts and return the names
#endif

	protected:
		nonvirtual	void	OnToggleSmartCutNPasteOptionCommand ();
		nonvirtual	void	OnToggleSmartCutNPasteOption_UpdateCommandUI (CMD_ENABLER* enabler);
		nonvirtual	void	OnToggleWrapToWindowOptionCommand ();
		nonvirtual	void	OnToggleWrapToWindowOption_UpdateCommandUI (CMD_ENABLER* enabler);
		nonvirtual	void	OnToggleShowHiddenTextOptionCommand ();
		nonvirtual	void	OnToggleShowHiddenTextOption_UpdateCommandUI (CMD_ENABLER* enabler);

	private:
		nonvirtual	void	UpdateViewsForPrefsChange ();

#if		qWindows
	public:
		override	BOOL	InitInstance ();

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
		nonvirtual	void	HandleHRESULTException (HRESULT hr) throw ();
#if 0
	public:
		override	BOOL	OnIdle (LONG lCount);
#endif

	private:
		nonvirtual	void	AddDocTemplateForString (const char* tmplStr, bool connectToServer);

		COleTemplateServer fOleTemplateServer;

	public:
		override	BOOL ProcessShellCommand (CCommandLineInfo& rCmdInfo);

	protected:
		afx_msg	void	OnAppAbout ();
		afx_msg	void	OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI);
		afx_msg	void	OnToggleWrapToWindowOptionUpdateCommandUI (CCmdUI* pCmdUI);
		afx_msg	void	OnToggleShowHiddenTextOptionUpdateCommandUI (CCmdUI* pCmdUI);


	private:
		DECLARE_MESSAGE_MAP()
#endif

	protected:
		nonvirtual	void	OnChooseDefaultFontCommand ();

	// handle exceptions....
	public:
		nonvirtual	void	HandleBadAllocException () throw ();
		nonvirtual	void	HandleBadUserInputException () throw ();
		nonvirtual	void	HandleUnknownException () throw ();

	public:
		nonvirtual	void	DoAboutBox ();
		nonvirtual	void	OnGotoLedItWebPageCommand ();
		nonvirtual	void	OnGotoSophistsWebPageCommand ();
		nonvirtual	void	OnCheckForUpdatesWebPageCommand ();

#if		qMacOS
	public:
		override	void	MakeMenuBar ();
		override	void	StartUp ();
		override	void	ProcessNextEvent ();
		override	void	HandleAppleEvent (const AppleEvent& inAppleEvent, AppleEvent& outAEReply, AEDesc& outResult, long inAENumber);

	protected:
		nonvirtual	void	HandleMacOSException (OSErr err);
		nonvirtual	void	HandlePowerPlantException (ExceptionCode err);

	public:
		override	void	ShowAboutBox ();

		override	Boolean	ObeyCommand (CommandT inCommand, void *ioParam = nil);
		override	void	FindCommandStatus (CommandT inCommand,
									Boolean &outEnabled, Boolean &outUsesMark,
									UInt16& outMark, Str255 outName
								);

	public:
		nonvirtual	void	OnHelpMenuCommand ();
	private:
		short	fHelpMenuItem;

	private:
		short	fGotoLedItWebPageMenuItem;

	private:
		short	fGotoSophistsWebPageMenuItem;

	private:
		short	fCheckForUpdatesWebPageMenuItem;

	public:
		override	void	UseIdleTime (const EventRecord& inMacEvent);
	private:
		float	fLastLowMemWarnAt;

	public:
		override void			OpenDocument (FSSpec *inMacFSSpec);
		override void			OpenDocument (FSSpec *inMacFSSpec, FileFormat format);
		override LModelObject*	MakeNewDocument();

	public:
		override void	ChooseDocument ();
	private:
		static	pascal	Boolean	SFGetDlgModalFilter (DialogPtr dialog, EventRecord* theEvent, short* itemHit, void* myData);
		static	pascal	short	SFGetDlgHook (short item, DialogPtr dialog, void* myData);
#endif


#if		qXWindows
	private:
		nonvirtual	GtkWidget*	get_main_menu (GtkWidget  *window);


	public:
		static	gint	delete_event (GtkWidget *widget, gpointer   data);
		static	void	xdestroy (GtkWidget *widget, gpointer   data);

	private:
		LedItDocument*	fDocument;

	public:
		nonvirtual	void	OnNewDocumentCommand ();
		nonvirtual	void	OnOpenDocumentCommand ();
		nonvirtual	void	OnSaveDocumentCommand ();
		nonvirtual	void	OnSaveAsDocumentCommand ();
		nonvirtual	void	OnQuitCommand ();

	public:
		nonvirtual	void	LoadFromFile (const string& fileName, FileFormat fileFormat);
		nonvirtual	void	SaveAs (const string& fileName, FileFormat fileFormat);
		nonvirtual	void	Save ();

	private:
		nonvirtual	void	UpdateFrameWindowTitle ();

	public:
		static	void	AppCmdDispatcher (gpointer callback_data,
												 guint callback_action,
												 GtkWidget *widget
											);
		static	void	AppCmdOnInitMenu (GtkMenuItem* menuItem, gpointer callback_data);

	public:
		nonvirtual	GtkWidget*	GetAppWindow () const;
	private:
		GtkWidget*	fAppWindow;
		LedItView*	fTextEditor;

	private:
		static GtkItemFactoryEntry kMenuItemResources[];
#endif
#if		qWindows || qXWindows
	public:
		Led_InstalledFonts	fInstalledFonts;	// Keep a static copy for speed, and so font#s are static throughout the life of the applet
#endif
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#endif	/*__LedItApplication_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

