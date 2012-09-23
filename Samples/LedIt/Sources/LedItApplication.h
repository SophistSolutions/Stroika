/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */

#ifndef	__LedItApplication_h__
#define	__LedItApplication_h__	1

#include    "Stroika/Foundation/StroikaPreComp.h"

#include	<vector>
#include	<string>


#if		qPlatform_MacOS
	#include	<Dialogs.h>

	#include	<LDocApplication.h>
#elif	qPlatform_Windows
	#include	<afxole.h>
#elif	qXWindows
	#include	<gtk/gtk.h>
#endif

#include	"Stroika/Frameworks/Led/GDI.h"
#include	"Stroika/Frameworks/Led/Support.h"
#include	"Stroika/Frameworks/Led/TextStore.h"

#include	"LedItConfig.h"

#if		qIncludeBasicSpellcheckEngine
	#include	"Stroika/Frameworks/Led/SpellCheckEngine_Basic.h"
#endif

#if		qPlatform_MacOS
	#include	"Stroika/Frameworks/Led/Platform/Led_PP.h"
#elif	qPlatform_Windows
	#include	"Stroika/Frameworks/Led/Platform/MFC.h"
#elif	qXWindows
	#include	"Stroika/Frameworks/Led/Platform/Gtk.h"
#endif


#if		qPlatform_Windows
class	CMenu;
#endif

class	LedItDocument;
class	LedItView;


#if		qPlatform_MacOS
	typedef	Platform::Led_PP_TmpCmdUpdater	CMD_ENABLER;
#elif	qPlatform_Windows
	typedef	Platform::Led_MFC_TmpCmdUpdater	CMD_ENABLER;
#elif	qXWindows
	typedef	Platform::Led_Gtk_TmpCmdUpdater	CMD_ENABLER;
#endif




class	LedItApplication
#if		qPlatform_MacOS
	: public LDocApplication
#elif	qPlatform_Windows
	: public CWinApp
#endif
{
	private:
		#if		qPlatform_MacOS
			typedef	LDocApplication	inherited;
		#elif	qPlatform_Windows
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

#if		qPlatform_Windows
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

#if		qPlatform_Windows
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

#if		qPlatform_MacOS
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
#if		qPlatform_Windows || qXWindows
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

