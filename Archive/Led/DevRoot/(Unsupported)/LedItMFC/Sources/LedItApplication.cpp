/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/LedItApplication.cpp,v 2.36 2003/03/17 21:42:21 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItApplication.cpp,v $
 *	Revision 2.36  2003/03/17 21:42:21  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.35  2000/10/16 22:49:57  lewis
 *	use new Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.34  2000/10/14 13:55:31  lewis
 *	change URL-goto-address
 *	
 *	Revision 2.33  2000/03/17 22:40:14  lewis
 *	SPR#0717- preliminary support - for showhiddentext testing and UI
 *	
 *	Revision 2.32  1999/12/21 21:03:59  lewis
 *	adjust about box for UNICODE string
 *	
 *	Revision 2.31  1999/12/09 17:29:21  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.30  1999/11/15 21:35:26  lewis
 *	react to some small Led changes so that Led itself no longer deends on MFC (though this demo
 *	app clearly DOES)
 *	
 *	Revision 2.29  1999/03/08 22:45:02  lewis
 *	Break out RTFInfo stuff which was really just a repository for type declarations, and store
 *	that into new type RTFIO
 *	
 *	Revision 2.28  1999/02/22 13:12:42  lewis
 *	lose (I believe) unneeded extra calls to UpdateViewsForprfefsChange from updateui routine
 *	
 *	Revision 2.27  1999/02/08 22:31:35  lewis
 *	Support new optional 'wrap-to-window' feature. Used to be we ALWAYS did that. Now can be on/off.
 *	Next todo is to fix the OFF case to properly use ruler
 *	
 *	Revision 2.26  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 2.25  1998/04/25  01:51:23  lewis
 *	abstract some logic into routine - GetLongPathName ()
 *
 *	Revision 2.24  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1997/12/24  04:15:39  lewis
 *	New Options class - use this to replace use of global variables for a coupel options.
 *	And add new RTFOLEEmbedding::kEmbeddingTag support.
 *
 *	Revision 2.22  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.21  1997/07/23  23:16:24  lewis
 *	Fixed MDI support for toggle smart paste (even though this app not mdi, makes it easier for people
 *	who clone this code).
 *
 *	Revision 2.20  1997/06/23  16:33:27  lewis
 *	when we click on URLs in about box, also close the window.
 *
 *	Revision 2.19  1997/06/18  03:42:05  lewis
 *	qIncludePrefixFile, and other cleanups
 *
 *	Revision 2.18  1997/03/23  03:32:01  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1997/03/23  01:02:11  lewis
 *	HandleBadAllocException ()
 *
 *	Revision 2.16  1997/03/04  20:15:58  lewis
 *	*** empty log message ***
 *
 *	Revision 2.15  1997/01/10  03:30:08  lewis
 *	No longer call Enable3dcontrols (dont think I was using them, and makes link errors in mwerks windows compiler).
 *	9 std profile settigns - 9 files in MRU list.
 *
 *	Revision 2.14  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.13  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/09/30  15:01:36  lewis
 *	Minor code cleanups, and added fSearchParameters here.
 *	Changed name of some string in AddDocTemplateForString().
 *
 *	Revision 2.11  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.10  1996/07/19  20:56:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/07/19  16:47:29  lewis
 *	changed name of helpfile doc file to open.
 *
 *	Revision 2.8  1996/07/03  01:56:38  lewis
 *	Support new about box with pict and live URLs jumping to my web page.
 *	Override LedItApplication::WinHelp() to ShellExectute .html docs file.
 *	Do LedItApp::Get() method.
 *	otehrs...
 *
 *	Revision 2.7  1996/06/01  02:43:47  lewis
 *	Very substnatial changes.
 *	new about box code.
 *	totally new code for registataion of icons/etc and open/save pick file
 *	dialogs and CTempalte stuff.
 *
 *	Revision 2.6  1996/05/23  20:17:42  lewis
 *	fix SPR#0345 by doing FindFirstFile hack to get long file name.
 *	Call EnableShellOpen/RegisterShellFileTypes (true - sb false???).
 *
 *	Revision 2.5  1996/05/14  20:42:46  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/05/05  14:41:39  lewis
 *	Cleanup app startup code.
 *	Register embedding types.
 *	Assign PICT resources for special embeddings.
 *	InitDDE for talking to web browsers with Spyglass DDE/URL embeddings.
 *
 *	Revision 2.3  1996/04/18  15:51:16  lewis
 *	Cleanups, and now that we use new flavor stuff, we must
 *	register with EmbeddedObjectCreatorRegistry our types.
 *
 *	Revision 2.2  1996/02/26  22:46:36  lewis
 *	FIx computation of help file name to be based on our app directory, but
 *	not name (cuz I name files Led 2.0b3, etc... and don't name help files
 *	that way).
 *	Get led version string for LedConfig.hh instead of Led::lVersion.
 *	Override LedItDocManager::OpenDocumentFile () to work around bug
 *	where we got multiple main windows openened.
 *
 *	Revision 2.1  1996/02/05  04:48:26  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1996/01/04  00:58:05  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:57:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:47:54  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#if		_MSC_VER && (qSilenceAnnoyingCompilerWarnings || !defined (qSilenceAnnoyingCompilerWarnings))
	#pragma	warning (4 : 4800)
	#pragma	warning (4 : 4786)
#endif

#include	<afx.h>

#include	"LedConfig.h"
#include	"LedStdDialogs.h"

#include	"FontMenu.h"
#include	"LedItControlItem.h"
#include	"LedItMainFrame.h"
#include	"LedItInPlaceFrame.h"
#include	"LedItDocument.h"
#include	"LedItView.h"
#include	"Resource.h"
#include	"Options.h"

#include	"LedItApplication.h"






const	char	kLedItUserPageURL[]		=	"http://www.sophists.com/Led/LedIt/Default.asp?From=LedIt30W";
const	char	kSophistSolutionsURL[]	=	"http://www.sophists.com/Default.asp?From=LedIt30W";






// Some private, but externed, MFC 4.0 routines needed to subclass CDocManager... See below...
// LGP 960222
extern	BOOL	AFXAPI	AfxFullPath (LPTSTR lpszPathOut, LPCTSTR lpszFileIn);
extern	BOOL	AFXAPI	AfxResolveShortcut(CWnd* pWnd, LPCTSTR pszShortcutFile, LPTSTR pszPath, int cchPath);
extern	void	AFXAPI	AfxGetModuleShortFileName (HINSTANCE hInst, CString& strShortName);


class	SimpleLedTemplate : public CSingleDocTemplate {
	public:
		SimpleLedTemplate (const char* daStr):
			CSingleDocTemplate (IDR_MAINFRAME, RUNTIME_CLASS(LedItDocument), RUNTIME_CLASS(LedItMainFrame),
						RUNTIME_CLASS(LedItView)
				)
			{
				m_strDocStrings = daStr;
			}

		override	void	LoadTemplate ()
			{
				bool	doMenuEmbedding		=	(m_hMenuEmbedding == NULL);
				bool	doMenuInPlaceServer	=	(m_hMenuInPlaceServer == NULL);
				bool	doMenuInPlace		=	(m_hMenuInPlace == NULL);
				CSingleDocTemplate::LoadTemplate ();
				
				// Now go and fixup the font menu...
				if (doMenuEmbedding and m_hMenuEmbedding != NULL) {
					// Not understood well just what each of these are, but I think this
					// contains a format menu.
					// LGP 960101
					CMenu	tmp;
					tmp.Attach (m_hMenuEmbedding);
					FixupFontMenu (tmp.GetSubMenu (2)->GetSubMenu (0));
					tmp.Detach ();
				}
				if (doMenuInPlaceServer and m_hMenuInPlaceServer != NULL) {
					// Not understood well just what each of these are, but I think this
					// contains a format menu.
					// LGP 960101
					CMenu	tmp;
					tmp.Attach (m_hMenuInPlaceServer);
					FixupFontMenu (tmp.GetSubMenu (1)->GetSubMenu (0));
					tmp.Detach ();
				}
				if (doMenuInPlace and m_hMenuInPlace != NULL) {
					// Not understood well just what each of these are, but I don't think this
					// contains a format menu
				}
			}
};

class	LedItDocManager : public CDocManager {
	public:
		LedItDocManager ():
			CDocManager ()
			{
			}
		override	void		OnFileNew ();
		override	CDocument*	OpenDocumentFile (LPCTSTR lpszFileName);
		nonvirtual	CDocument*	OpenDocumentFile (LPCTSTR lpszFileName, Led_FileFormat format);

	public:
		override	void	RegisterShellFileTypes (BOOL bWin95);
	private:
		nonvirtual	void	RegisterShellFileType (bool bWin95, CString strPathName, int iconIndexInFile, CString strFilterExt, CString strFileTypeId, CString strFileTypeName);

	public:
		override	BOOL	DoPromptFileName (CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);


		override	void	OnFileOpen ();
};

	inline	const	void*	LoadAppResource (long resID, LPCTSTR resType)
		{
			HRSRC	hrsrc	=	::FindResource (::AfxGetResourceHandle (), MAKEINTRESOURCE (resID), resType);
			Led_AssertNotNil (hrsrc);
			HGLOBAL	hglobal	=	::LoadResource (::AfxGetResourceHandle (), hrsrc);
			const void*	lockedData	=	::LockResource (hglobal);
			Led_EnsureNotNil (lockedData);
			return (lockedData);
		}






static	BOOL	AFXAPI	SetRegKey (LPCTSTR lpszKey, LPCTSTR lpszValue, LPCTSTR lpszValueName = NULL)
{
	if (lpszValueName == NULL) {
		if (::RegSetValue(HKEY_CLASSES_ROOT, lpszKey, REG_SZ, lpszValue, ::_tcslen (lpszValue)) != ERROR_SUCCESS) {
			TRACE1("Warning: registration database update failed for key '%s'.\n", lpszKey);
			return FALSE;
		}
		return TRUE;
	}
	else {
		HKEY hKey;
		if(::RegCreateKey(HKEY_CLASSES_ROOT, lpszKey, &hKey) == ERROR_SUCCESS) {
			LONG lResult = ::RegSetValueEx(hKey, lpszValueName, 0, REG_SZ, (CONST BYTE*)lpszValue, ::_tcslen (lpszValue) + sizeof(TCHAR) );
			if(::RegCloseKey(hKey) == ERROR_SUCCESS && lResult == ERROR_SUCCESS) {
				return TRUE;
			}
		}
		TRACE1("Warning: registration database update failed for key '%s'.\n", lpszKey);
		return FALSE;
	}
}









/*
 ********************************************************************************
 ******************************** LedItApplication ******************************
 ********************************************************************************
 */
LedItApplication		theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
// {0FC00620-28BD-11CF-899C-00AA00580324}
static const CLSID clsid =	{ 0xfc00620, 0x28bd, 0x11cf, { 0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x24 } };


BEGIN_MESSAGE_MAP(LedItApplication, CWinApp)
	ON_COMMAND				(ID_APP_ABOUT,					OnAppAbout)
	ON_COMMAND				(ID_FILE_NEW,					OnFileNew)
	ON_COMMAND				(ID_FILE_OPEN,					OnFileOpen)
	ON_COMMAND				(ID_FILE_PRINT_SETUP,			OnFilePrintSetup)
	ON_COMMAND				(kToggleUseSmartCutNPasteCmdID,	OnToggleSmartCutNPasteOptionCommand)
	ON_UPDATE_COMMAND_UI	(kToggleUseSmartCutNPasteCmdID,	OnToggleSmartCutNPasteOptionUpdateCommandUI)
	ON_COMMAND				(kToggleWrapToWindowCmdID,		OnToggleWrapToWindowOptionCommand)
	ON_UPDATE_COMMAND_UI	(kToggleWrapToWindowCmdID,		OnToggleWrapToWindowOptionUpdateCommandUI)
	ON_COMMAND				(kToggleShowHiddenTextCmdID,	OnToggleShowHiddenTextOptionCommand)
	ON_UPDATE_COMMAND_UI	(kToggleShowHiddenTextCmdID,	OnToggleShowHiddenTextOptionUpdateCommandUI)
	ON_COMMAND				(kGotoLedItWebPageCmdID,		OnGotoLedItWebPageCommand)
	ON_COMMAND				(kGotoSophistsWebPageCmdID,		OnGotoSophistsWebPageCommand)
END_MESSAGE_MAP()

LedItApplication*	LedItApplication::sThe	=	NULL;

LedItApplication::LedItApplication ():
	fOleTemplateServer ()
{
	Led_Require (sThe == NULL);
	sThe = this;
}

LedItApplication::~LedItApplication ()
{
	Led_Require (sThe == this);
	sThe = NULL;
}

LedItApplication&	LedItApplication::Get ()
{
	Led_EnsureNotNil (sThe);
	return *sThe;
}

BOOL	LedItApplication::InitInstance ()
{
	SetRegistryKey (_T ("Sophist Solutions, Inc."));

	// Initialize OLE libraries
	if (!AfxOleInit ()) {
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return false;
	}

	#if		qUseSpyglassDDESDIToOpenURLs
	Led_URLManager::InitDDEHandler ();
	#endif


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo	cmdInfo;
	ParseCommandLine (cmdInfo);

	AfxEnableControlContainer ();

	#if		0
	// LGP 960509 - doesn't appear to have any effect if present or not...
	// and prevents linking on Mac (CodeWarrior x-compiler).
	Enable3dControlsStatic ();	// Call this when linking to MFC statically
	#endif

	//LoadStdProfileSettings (5);  // Load standard INI file options (including MRU)
	LoadStdProfileSettings (9);  // Load standard INI file options (including MRU)

	Led_Assert (m_pDocManager == NULL);
	m_pDocManager = new LedItDocManager ();

	AddDocTemplateForString ("LedIt\n\nLedIt\nLed Rich Text Format (*.led)\n.led\nLedIt.Document\nLedIt Document", true);

	// Enable DDE Open, and register icons / file types with the explorer/shell
	EnableShellOpen ();
	RegisterShellFileTypes (true);	// ARG???


	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	fOleTemplateServer.UpdateRegistry (OAT_INPLACE_SERVER);
	COleObjectFactory::UpdateRegistryAll ();


	// Tell Led about the picture resources it needs to render some special embedding markers
	StandardURLStyleMarker::sURLPict = (const Led_DIB*)::LoadAppResource (kURLPictID, RT_BITMAP);
	StandardUnknownTypeStyleMarker::sUnknownPict = (const Led_DIB*)::LoadAppResource (kUnknownEmbeddingPictID, RT_BITMAP);
	StandardMacPictureStyleMarker::sUnsupportedFormatPict = (const Led_DIB*)::LoadAppResource (kUnsupportedPICTFormatPictID, RT_BITMAP);

	// Tell Led about the kinds of embeddings we will allow
	EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();

	// Support OLE embeddings (both created from clip, and from RTF-format files)
	EmbeddedObjectCreatorRegistry::Get ().AddAssoc (LedItControlItem::kClipFormat, LedItControlItem::kEmbeddingTag, LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);
	EmbeddedObjectCreatorRegistry::Get ().AddAssoc (kBadClipFormat, RTFIO::RTFOLEEmbedding::kEmbeddingTag, LedItControlItem::mkLedItControlItemStyleMarker, LedItControlItem::mkLedItControlItemStyleMarker);


	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated) {
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

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

void	LedItApplication::WinHelp (DWORD dwData, UINT nCmd)
{
	// get path of executable
	TCHAR directoryName[_MAX_PATH];
	Led_Verify (::GetModuleFileName (m_hInstance, directoryName, _MAX_PATH));

	{
		LPTSTR lpszExt = _tcsrchr (directoryName, '\\');
		ASSERT(lpszExt != NULL);
		ASSERT(*lpszExt == '\\');
		*(lpszExt+1) = '\0';
	}
	(void)::_tcscat (directoryName, _T ("LedItDocs\\"));

	// wrap in try/catch, and display error if no open???
	// (NB: we use .htm instead of .html cuz some old systems - I think maybe
	//	Win95 with only Netscape 2.0 installed - only have .htm registered - not
	//  .html).
	(void)::ShellExecute (NULL, _T ("open"), _T ("index.htm"), NULL, directoryName, SW_SHOWNORMAL);
}

BOOL	LedItApplication::PumpMessage ()
{
	try {
		return inherited::PumpMessage ();
	}
	catch (CMemoryException* e) {
		HandleBadAllocException ();
		e->Delete ();
	}
	catch (CException* e) {
		HandleMFCException (e);
		e->Delete ();
	}
	catch (bad_alloc) {
		HandleBadAllocException ();
	}
	catch (HRESULT hr) {
		HandleHRESULTException (hr);
	}
	catch (...) {
		HandleUnknownException ();
	}
	return true;
}

void	LedItApplication::HandleMFCException (CException* /*e*/) throw ()
{
	// tmp hack for now...
	HandleUnknownException ();
}

void	LedItApplication::HandleHRESULTException (HRESULT hr) throw ()
{
	// tmp hack for now...
	HandleUnknownException ();
}

void	LedItApplication::HandleBadAllocException () throw ()
{
	try {
		CDialog	errorDialog (kBadAllocExceptionOnCmdDialogID);
		errorDialog.DoModal ();
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

void	LedItApplication::HandleUnknownException () throw ()
{
	try {
		CDialog	errorDialog (kUnknownExceptionOnCmdDialogID);
		errorDialog.DoModal ();
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

BOOL	LedItApplication::OnIdle (LONG lCount)
{
	#if		qSupportEnterIdleCallback
		POSITION	tp	=	GetFirstDocTemplatePosition ();
		while (tp != NULL) {
			CDocTemplate*		t	=	GetNextDocTemplate (tp);
			Led_AssertNotNil (t);
			POSITION	dp	=	t->GetFirstDocPosition ();
			while (dp != NULL) {
				CDocument*		doc	=	t->GetNextDoc (dp);
				Led_AssertNotNil (doc);
				POSITION	vp	=	doc->GetFirstViewPosition ();
				while (vp != NULL) {
					CView*	v	=	doc->GetNextView (vp);
					Led_AssertNotNil (v);
					LedItView*	lv	=	dynamic_cast<LedItView*> (v);
					if (lv != NULL) {
						lv->CallEnterIdleCallback ();
					}
				}
			}
		}
	#endif
	return inherited::OnIdle (lCount);
}

void	LedItApplication::AddDocTemplateForString (const char* tmplStr, bool connectToServer)
{
	CSingleDocTemplate* pDocTemplate = new SimpleLedTemplate (tmplStr);
	pDocTemplate->SetContainerInfo (IDR_CNTR_INPLACE);
	pDocTemplate->SetServerInfo (IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE, RUNTIME_CLASS(LedItInPlaceFrame));
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

void	LedItApplication::OnAppAbout ()
{
	class	MyAboutBox : public Led_StdDialogHelper_AboutBox {
		private:
			typedef	Led_StdDialogHelper_AboutBox	inherited;
		public:
				MyAboutBox (HINSTANCE hInstance, HWND parentWnd):
					inherited (hInstance, parentWnd)
				{
				}
		public:
			override	BOOL	OnInitDialog ()
				{
					BOOL	result	=	inherited::OnInitDialog ();

					// Cuz of fact that dlog sizes specified in dlog units, and that doesn't work well for bitmaps
					// we must resize our dlog on the fly based on pict resource size...
					const	kPictWidth	=	380;	// must agree with ACTUAL bitmap size
					const	kPictHeight	=	283;
					const	kButVSluff	=	12;
					{
						RECT	windowRect;
						::GetWindowRect (GetHWND (), &windowRect);
						// figure size of non-client area...
						int	ncWidth		=	0;
						int	ncHeight	=	0;
						{
							RECT	clientRect;
							::GetClientRect (GetHWND (), &clientRect);
							ncWidth = AsLedRect (windowRect).GetWidth () - AsLedRect (clientRect).GetWidth ();
							ncHeight = AsLedRect (windowRect).GetHeight () - AsLedRect (clientRect).GetHeight ();
						}
						::MoveWindow (GetHWND (), windowRect.left, windowRect.top, kPictWidth+ncWidth, kPictHeight+ncHeight, false);
					}

					// Place and fill in version information
					{
						HWND	w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_VersionFieldID);
						Led_AssertNotNil (w);
						::MoveWindow (w, 92, 37, 230, 16, false);
						#if		_UNICODE
							#define	kUNICODE_NAME_ADORNER	L" [UNICODE]"
						#elif	qLed_CharacterSet == qUNICODE_CharacterSet
							#define	kUNICODE_NAME_ADORNER	" [Internal UNICODE]"
						#else
							#define	kUNICODE_NAME_ADORNER
						#endif
						::SetWindowText (w, _T (qLed_ShortVersionString) kUNICODE_NAME_ADORNER _T (" (") _T (__DATE__) _T (")."));
					}

					// Place hidden buttons which map to URLs
					{
						HWND	w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_InfoLedFieldID);
						Led_AssertNotNil (w);
						MoveWindow (w, 112, 183, 154, 14, false);
						w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_LedWebPageFieldID);
						Led_AssertNotNil (w);
						MoveWindow (w, 10, 197, 156, 14, false);
					}

					// Place OK button
					{
						HWND	w	=	::GetDlgItem (GetHWND (), IDOK);
						Led_AssertNotNil (w);
						RECT	tmp;
						::GetWindowRect (w, &tmp);
						::MoveWindow (w, 10, kPictHeight - AsLedRect (tmp).GetHeight ()-kButVSluff, AsLedRect (tmp).GetWidth (), AsLedRect (tmp).GetHeight (), false);	// width/height we should presevere
					}

					::SetWindowText (GetHWND (), _T ("About LedIt!"));

					return (result);
				}
		public:
			override	void	OnClickInInfoField ()
				{
					Led_URLManager::Get ().Open ("mailto:info-led@sophists.com");
					inherited::OnClickInInfoField ();
				}

			override	void	OnClickInLedWebPageField ()
				{
					Led_URLManager::Get ().Open (kLedItUserPageURL);
					inherited::OnClickInLedWebPageField ();
				}
	};
	MyAboutBox	dlg (m_hInstance, AfxGetMainWnd ()->m_hWnd);
	dlg.DoModal ();
}

void	LedItApplication::OnToggleSmartCutNPasteOptionCommand ()
{
	Options	o;
	o.SetSmartCutAndPaste (not o.GetSmartCutAndPaste ());
	UpdateViewsForPrefsChange ();
}

void	LedItApplication::OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	pCmdUI->Enable ();
	pCmdUI->SetCheck (Options ().GetSmartCutAndPaste ());
}

void	LedItApplication::OnToggleWrapToWindowOptionCommand ()
{
	Options	o;
	o.SetWrapToWindow (not o.GetWrapToWindow ());
	UpdateViewsForPrefsChange ();
}

void	LedItApplication::OnToggleWrapToWindowOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	pCmdUI->Enable ();
	pCmdUI->SetCheck (Options ().GetWrapToWindow ());
}

void	LedItApplication::OnToggleShowHiddenTextOptionCommand ()
{
	Options	o;
	o.SetShowHiddenText (not o.GetShowHiddenText ());
	UpdateViewsForPrefsChange ();
}

void	LedItApplication::OnToggleShowHiddenTextOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	pCmdUI->Enable ();
	pCmdUI->SetCheck (Options ().GetShowHiddenText ());
}

void	LedItApplication::OnGotoLedItWebPageCommand ()
{
	Led_URLManager::Get ().Open (kLedItUserPageURL);
}

void	LedItApplication::OnGotoSophistsWebPageCommand ()
{
	Led_URLManager::Get ().Open (kSophistSolutionsURL);
}

void	LedItApplication::UpdateViewsForPrefsChange ()
{
	bool	wrapToWindow	=	Options ().GetWrapToWindow ();
	bool	smartCutNPaste	=	Options ().GetSmartCutAndPaste ();
	bool	showHiddenText	=	Options ().GetShowHiddenText ();

	// Update each open view
	#if		qSupportEnterIdleCallback
		POSITION	tp	=	GetFirstDocTemplatePosition ();
		while (tp != NULL) {
			CDocTemplate*		t	=	GetNextDocTemplate (tp);
			Led_AssertNotNil (t);
			POSITION	dp	=	t->GetFirstDocPosition ();
			while (dp != NULL) {
				CDocument*		doc	=	t->GetNextDoc (dp);
				Led_AssertNotNil (doc);
				POSITION	vp	=	doc->GetFirstViewPosition ();
				while (vp != NULL) {
					CView*	v	=	doc->GetNextView (vp);
					Led_AssertNotNil (v);
					LedItView*	lv	=	dynamic_cast<LedItView*> (v);
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







/*
 ********************************************************************************
 ******************************** LedItDocManager *******************************
 ********************************************************************************
 */
void	LedItDocManager::OnFileNew ()
{
	// tmp hack - for now always pick the first - later this guy will own all refs
	// and just use RIGHT (namely LedDoc) one...
	CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead();
	ASSERT(pTemplate != NULL);
	ASSERT_KINDOF(CDocTemplate, pTemplate);
	(void)pTemplate->OpenDocumentFile (NULL);
}

CDocument*	LedItDocManager::OpenDocumentFile (LPCTSTR lpszFileName)
{
	return OpenDocumentFile (lpszFileName, eUnknownFormat);
}

	inline	Led_SDK_String	GetLongPathName (const Led_SDK_String& pathName)
		{
			TCHAR szPath[_MAX_PATH];
			Led_Require (pathName.length () < _MAX_PATH);
			_tcscpy (szPath, pathName.c_str ());
			WIN32_FIND_DATA fileData;
			HANDLE hFind = ::FindFirstFile (szPath, &fileData);
			if (hFind != INVALID_HANDLE_VALUE) {
				TCHAR* lastSlash = ::_tcsrchr (szPath, '\\');
				if (lastSlash != NULL) {
					*lastSlash = '\0';
				}
				::_tcscat (szPath, _T ("\\"));
				::_tcsncat (szPath, fileData.cFileName, _MAX_PATH);
				szPath[_MAX_PATH-1] = '\0';
				VERIFY (::FindClose (hFind));
			}
			return szPath;
		}
CDocument*	LedItDocManager::OpenDocumentFile (LPCTSTR lpszFileName, Led_FileFormat format)
{
	// Lifted from CDocManager::OpenDocumentFile() with a few changes to not keep
	// separate lists of templates. Only reason I create multiple templates is to
	// get the popup in the open/save dialogs to appear.
	// Sigh!
	// LGP 960222
	// PLUS SEE BELOW - LGP 960522
	CDocTemplate* pTemplate = (CDocTemplate*)m_templateList.GetHead();
	ASSERT(pTemplate != NULL);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	TCHAR szPath[_MAX_PATH];
	ASSERT(::_tcslen (lpszFileName) < _MAX_PATH);
	TCHAR szTemp[_MAX_PATH];
	if (lpszFileName[0] == '\"')
		++lpszFileName;
	_tcsncpy(szTemp, lpszFileName, _MAX_PATH);
	LPTSTR lpszLast = _tcsrchr(szTemp, '\"');
	if (lpszLast != NULL)
		*lpszLast = 0;
	AfxFullPath(szPath, szTemp);
	TCHAR szLinkName[_MAX_PATH];
	if (AfxResolveShortcut(AfxGetMainWnd(), szPath, szLinkName, _MAX_PATH))
		_tcscpy (szPath, szLinkName);

	// Also, to fix SPR#0345, we must use this (or SHGetFileInfo) hack
	// to get the long-file-name version of the file name.
	(void)::_tcscpy (szPath, GetLongPathName (szPath).c_str ());

	LedItDocument::sHiddenDocOpenArg = format;
	return (pTemplate->OpenDocumentFile (szPath));
}

void	LedItDocManager::RegisterShellFileTypes (BOOL bWin95)
{
	// Cloned from base CWinApp version, but we don't use Doc templates

	CString strPathName, strTemp;

	AfxGetModuleShortFileName (AfxGetInstanceHandle(), strPathName);

// Only do for .led file now???
// In future maybe ask user if I should do .txt, etc??? And any others I support?
	RegisterShellFileType (bWin95, strPathName, 1, ".led", "LedIt.Document", "LedIt Document");
}

void	LedItDocManager::RegisterShellFileType (bool bWin95, CString strPathName, int iconIndexInFile, CString strFilterExt, CString strFileTypeId, CString strFileTypeName)
{
	static const TCHAR szShellOpenFmt[] = _T("%s\\shell\\open\\%s");
	static const TCHAR szShellPrintFmt[] = _T("%s\\shell\\print\\%s");
	static const TCHAR szShellPrintToFmt[] = _T("%s\\shell\\printto\\%s");
	static const TCHAR szDefaultIconFmt[] = _T("%s\\DefaultIcon");
	static const TCHAR szShellNewFmt[] = _T("%s\\ShellNew");
	static const TCHAR szIconIndexFmt[] = _T(",%d");
	static const TCHAR szCommand[] = _T("command");
	static const TCHAR szOpenArg[] = _T(" \"%1\"");
	static const TCHAR szPrintArg[] = _T(" /p \"%1\"");
	static const TCHAR szPrintToArg[] = _T(" /pt \"%1\" \"%2\" \"%3\" \"%4\"");

	static const TCHAR szShellNewValueName[] = _T("NullFile");
	static const TCHAR szShellNewValue[] = _T("");

	const	DEFAULT_ICON_INDEX 	=	0;

	CString strOpenCommandLine = strPathName;
	CString strPrintCommandLine = strPathName;
	CString strPrintToCommandLine = strPathName;
	CString strDefaultIconCommandLine = strPathName;

	if (bWin95) {
		CString strIconIndex;
		HICON hIcon = ::ExtractIcon (AfxGetInstanceHandle(), strPathName, iconIndexInFile);
		if (hIcon != NULL) {
			strIconIndex.Format (szIconIndexFmt, iconIndexInFile);
			::DestroyIcon(hIcon);
		}
		else {
			strIconIndex.Format(szIconIndexFmt, DEFAULT_ICON_INDEX);
		}
		strDefaultIconCommandLine += strIconIndex;
	}

	if (!strFileTypeId.IsEmpty()) {
		CString	strTemp;

		// enough info to register it
		if (strFileTypeName.IsEmpty ()) {
			strFileTypeName = strFileTypeId;    // use id name
		}

		ASSERT(strFileTypeId.Find (' ') == -1);  // no spaces allowed

		// first register the type ID with our server
		if (!SetRegKey (strFileTypeId, strFileTypeName)) {
			return;		// just skip it
		}

		if (bWin95) {
			// path\DefaultIcon = path,1
			strTemp.Format (szDefaultIconFmt, (LPCTSTR)strFileTypeId);
			if (!SetRegKey (strTemp, strDefaultIconCommandLine)) {
				return;		// just skip it
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
			return;		// just skip it
		}

		if (bWin95) {
			// path\shell\print\command = path /p filename
			strTemp.Format (szShellPrintFmt, (LPCTSTR)strFileTypeId, (LPCTSTR)szCommand);
			if (!SetRegKey (strTemp, strPrintCommandLine)) {
				return;		// just skip it
			}

			// path\shell\printto\command = path /pt filename printer driver port
			strTemp.Format(szShellPrintToFmt, (LPCTSTR)strFileTypeId, (LPCTSTR)szCommand);
			if (!SetRegKey(strTemp, strPrintToCommandLine)) {
				return;		// just skip it
			}
		}

		if (!strFilterExt.IsEmpty ()) {
			ASSERT(strFilterExt[0] == '.');
			LONG lSize = _MAX_PATH * 2;
			LONG lResult = ::RegQueryValue (HKEY_CLASSES_ROOT, strFilterExt, strTemp.GetBuffer(lSize), &lSize);
			strTemp.ReleaseBuffer ();
			if (lResult != ERROR_SUCCESS || strTemp.IsEmpty() || strTemp == strFileTypeId) {
				// no association for that suffix
				if (!SetRegKey (strFilterExt, strFileTypeId)) {
					return;		// just skip it
				}
				if (bWin95) {
					strTemp.Format (szShellNewFmt, (LPCTSTR)strFilterExt);
					(void)SetRegKey (strTemp, szShellNewValue, szShellNewValueName);
				}
			}
		}
	}
}

BOOL	LedItDocManager::DoPromptFileName (CString& /*fileName*/, UINT /*nIDSTitle*/, DWORD /*lFlags*/, BOOL /*bOpenFileDialog*/, CDocTemplate* /*pTemplate*/)
{
	// don't call this directly...
	Led_Assert (false);
	return false;
}

void	LedItDocManager::OnFileOpen ()
{
	CString fileName;
	Led_FileFormat	format	=	eUnknownFormat;	
	if (LedItDocument::DoPromptOpenFileName (fileName, &format)) {
		OpenDocumentFile (fileName, format);
	}
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
