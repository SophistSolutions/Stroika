/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/LedLineItApplication.cpp,v 2.49 2003/12/31 16:05:15 lewis Exp $
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
 *	$Log: LedLineItApplication.cpp,v $
 *	Revision 2.49  2003/12/31 16:05:15  lewis
 *	SPR#1610: replace call to Win32 ::OpenFile with ::CreateFile (so works with UNICODE apps)
 *	
 *	Revision 2.48  2003/12/11 03:09:15  lewis
 *	SPR#1585: move spellcheckengine object to application. Use qIncludeBasicSpellcheckEngine define
 *	to control its inclusion. Define default locaiton for LedLineIt spell dict.
 *	
 *	Revision 2.47  2003/11/29 22:31:08  lewis
 *		SPR#1572. Override OpenDocumentFile () in SimpleLedTemplate()
 *	to work around MFC's buggy handling of failure from
 *	CDocument::OpenDOcument() (before I did this - it would
 *	have crashed if I'd opened a second doc).
 *		Then - also - add code to LedLineItApplication::ProcessShellCmd()
 *	code so if the open fails due to file not existing, then
 *	create a NEW doc - and preset that as the default file name
 *	and append " {new}" to the doc title.
 *	
 *	Revision 2.46  2003/09/22 15:55:59  lewis
 *	use STD_EXCEPT_CATCHER(APP) cleanup - as part of SPR#1552
 *	
 *	Revision 2.45  2003/05/08 17:11:01  lewis
 *	SPR#1473: fixed GetLongPathName to handle case of plain filename without leading path part
 *	
 *	Revision 2.44  2003/04/16 17:25:52  lewis
 *	SPR#1440: added .bat support (and new optional edit/open facility to Win32UIFileAssociationInfo)
 *	
 *	Revision 2.43  2003/04/01 16:17:57  lewis
 *	SPR#1395: LedLineItDocManager::OpenDocumentFile () now looks for unmodified untitled documents
 *	(a single one) and closes it on successful open of a NEW document.
 *	
 *	Revision 2.42  2003/03/20 22:31:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.41  2003/03/20 17:07:16  lewis
 *	SPR#1360 - use new IdleManager instead of old OnEnterIdle
 *	
 *	Revision 2.40  2003/03/17 21:41:09  lewis
 *	SPR#1350 - qSupportEnterIdleCallback is now automatically true - and not a conditional varaible anymore
 *	
 *	Revision 2.39  2003/03/13 18:08:29  lewis
 *	fix aboutbox code to use qWideCharacters instead of qLed_CharacterSet
 *	
 *	Revision 2.38  2003/03/11 02:32:11  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 2.37  2003/01/23 01:26:40  lewis
 *	SPR#1257 - try/catch for Led_URLManager::Open call in about box
 *	
 *	Revision 2.36  2003/01/20 16:31:34  lewis
 *	minor tweeks noted by MWERKS CW8.3 compiler PLUS qNo_argc_argv_MacrosSupported workaround
 *	
 *	Revision 2.35  2002/09/09 02:57:42  lewis
 *	change Led/Sophists URL to say version# 31
 *	
 *	Revision 2.34  2002/09/04 02:36:11  lewis
 *	if _MFC_VER>0x700(VC.NET), then override WinHelpInternal() instead of WinHelp(). AddDocTemplateForString()
 *	call change from .txt to .ledtxt as main declared file type since MFC writes stuff (EXE name) that conflicts
 *	with what we write for that suffix
 *	
 *	Revision 2.33  2002/05/06 21:31:19  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.32  2001/11/27 00:28:22  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.31  2001/09/17 14:43:06  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 2.30  2001/09/16 21:14:04  lewis
 *	fix so builds on Win32 FULL_UNICODE
 *	
 *	Revision 2.29  2001/09/16 18:35:21  lewis
 *	SPR#1033- Added support to check/fix file assocs (depending on user prefs / dialog choices) to
 *	this app (Win32UIFileAssociationRegistrationHelper)
 *	
 *	Revision 2.28  2001/09/12 00:22:32  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 2.27  2001/09/09 22:36:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.26  2001/09/09 22:31:56  lewis
 *	SPR#0994, 0697- Added fCodePage flag to the LedLineItDocument class, and did alot
 *	of work reworking the calling of the CFileDialog so it could use my new replacement
 *	version (which includes an Encoding popup). Pretty ugly code for passing args through
 *	the MFC layers (like codepage from dialog to Serialize method). Most of the REAL guts
 *	code is in the CodePage module, and the stuff here is very MFC-specific.
 *	
 *	Revision 2.25  2001/09/06 20:27:15  lewis
 *	SPR#1003- cleanup about box
 *	
 *	Revision 2.24  2001/08/30 01:02:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.23  2000/10/16 22:50:09  lewis
 *	use new Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.22  2000/06/08 22:24:07  lewis
 *	SPR#0775. Override ProcessShellCommand() to properly handle OpenFile args - when more than one
 *	
 *	Revision 2.21  2000/06/08 21:49:49  lewis
 *	SPR#0774- added option to treat TAB as indent-selection command - as MSDEV does
 *	
 *	Revision 2.20  2000/06/08 21:22:28  lewis
 *	SPR#0773- call DragAcceptFiles
 *	
 *	Revision 2.19  1999/12/25 04:15:51  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it worked properly
 *	turning on/off and having multiple windows onto the same doc at the same time (scrolled to
 *	same or differnet) places
 *	
 *	Revision 2.18  1999/12/21 21:04:34  lewis
 *	adjust about box for UNICODE string. And react to cahnge in API for OnTypedNormalCharacter() method
 *	
 *	Revision 2.17  1999/12/09 17:35:27  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.16  1999/12/09 03:29:07  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.15  1999/08/28 18:21:43  lewis
 *	up version# of DOCFILE to navigate to on web to 2.3
 *	
 *	Revision 2.14  1999/02/22 13:12:03  lewis
 *	add Options module so we can preserve in registry flags like autoindent and smartcutandpaste
 *	
 *	Revision 2.13  1999/02/06 14:14:09  lewis
 *	subclass the DocManager, to override its OpenDocument () method to fix pathname it passes to be
 *	a LONGPATHNAME. Seems like MFC should do this!
 *	
 *	Revision 2.12  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable instaed of const cast,
 *	new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.11  1998/04/25  01:48:05  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.8  1997/07/24  00:33:23  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1997/07/23  23:19:36  lewis
 *	Lose unneedd LedLineItDocManager (vestige of LedIt).
 *	Support MDI changing smart cutandpaste mode flags.
 *	OnIdle notify Led of idle time.
 *
 *	Revision 2.6  1997/06/28  17:38:47  lewis
 *	Support MDI.
 *	Fixup template string (no more .led - use .txt).
 *
 *	Revision 2.5  1997/06/23  16:40:15  lewis
 *	fix URL we open in aboutbox. And on open of URLs from aboutbox, close aboutbox.
 *
 *	Revision 2.4  1997/06/18  03:47:06  lewis
 *	qIncludePrefixFile and cleanups
 *
 *	Revision 2.3  1997/03/23  03:04:53  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1997/03/23  01:10:33  lewis
 *	Fixed menu bar support for toggling SmartCutNPasteCommand
 *	HandleBadAllocException ()
 *
 *	Revision 2.1  1997/03/04  20:20:54  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:38:30  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedLineIt! 2.1 ==========>
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afx.h>

#include	"LedStdDialogs.h"
#include	"Led_Win32_FileRegistration.h"

#include	"FontMenu.h"
#include	"LedLineItDocFrame.h"
#include	"LedLineItMainFrame.h"
#include	"LedLineItInPlaceFrame.h"
#include	"LedLineItDocument.h"
#include	"LedLineItView.h"
#include	"Options.h"
#include	"Resource.h"

#include	"LedLineItApplication.h"








#define	STD_EXCEPT_CATCHER(APP)\
	catch (CMemoryException* e) {\
		(APP).HandleBadAllocException ();\
		e->Delete ();\
	}\
	catch (CException* e) {\
		(APP).HandleMFCException (e);\
		e->Delete ();\
	}\
	catch (bad_alloc) {\
		(APP).HandleBadAllocException ();\
	}\
	catch (Win32ErrorException&) {\
		(APP).HandleUnknownException ();\
	}\
	catch (HRESULTErrorException&) {\
		(APP).HandleUnknownException ();\
	}\
	catch (TextInteractor::BadUserInput&) {\
		(APP).HandleBadUserInputException ();\
	}\
	catch (...) {\
		(APP).HandleUnknownException ();\
	}







class	SimpleLedTemplate : public CMultiDocTemplate {
	public:
		SimpleLedTemplate (const char* daStr):
			CMultiDocTemplate (IDR_MAINFRAME, RUNTIME_CLASS(LedLineItDocument), RUNTIME_CLASS(LedLineItDocFrame),
						RUNTIME_CLASS(LedLineItView)
				)
			{
				m_strDocStrings = daStr;
			}

		override	void	LoadTemplate ()
			{
				CMultiDocTemplate::LoadTemplate ();
				
				// Now go and fixup the font menu...
				if (m_hMenuShared != NULL) {
					CMenu	tmp;
					tmp.Attach (m_hMenuShared);
					#if		qSupportSyntaxColoring
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
				 *	Based on CDocManager::OpenDocumentFile () from MFC (Version?? from MSVC.Net 2k3 - 2003-11-28)
				 *	but fixed to better handle exceptions. -- LGP 2003-11-28
				 */
				CDocument* pDocument = CreateNewDocument();
				if (pDocument == NULL)	{
					TRACE(traceAppMsg, 0, "CDocTemplate::CreateNewDocument returned NULL.\n");
					AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
					return NULL;
				}
				ASSERT_VALID(pDocument);

				try {
					BOOL bAutoDelete = pDocument->m_bAutoDelete;
					pDocument->m_bAutoDelete = FALSE;   // don't destroy if something goes wrong
					CFrameWnd* pFrame = CreateNewFrame(pDocument, NULL);
					pDocument->m_bAutoDelete = bAutoDelete;
					if (pFrame == NULL)	{
						AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
						delete pDocument;       // explicit delete on error
						return NULL;
					}
					ASSERT_VALID(pFrame);

					if (lpszPathName == NULL) {
						// create a new document - with default document name
						SetDefaultTitle(pDocument);

						// avoid creating temporary compound file when starting up invisible
						if (!bMakeVisible)
							pDocument->m_bEmbedded = TRUE;

						if (!pDocument->OnNewDocument()) {
							// user has be alerted to what failed in OnNewDocument
							TRACE(traceAppMsg, 0, "CDocument::OnNewDocument returned FALSE.\n");
							pFrame->DestroyWindow();
							return NULL;
						}

						// it worked, now bump untitled count
						m_nUntitledCount++;
					}
					else {
						// open an existing document
						CWaitCursor wait;
						if (!pDocument->OnOpenDocument (lpszPathName)) {
							TRACE(traceAppMsg, 0, "CDocument::OnOpenDocument returned FALSE.\n");
							throw bad_alloc ();		// misc random exception - not sure what TO throw here?
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
	inline	Led_SDK_String	GetLongPathName (const Led_SDK_String& pathName)
		{
			TCHAR szPath[_MAX_PATH];
			Led_Require (pathName.length () < _MAX_PATH);
			::_tcscpy (szPath, pathName.c_str ());
			WIN32_FIND_DATA fileData;
			HANDLE hFind = ::FindFirstFile (szPath, &fileData);
			if (hFind != INVALID_HANDLE_VALUE) {
				TCHAR* lastSlash = ::_tcsrchr (szPath, '\\');
				if (lastSlash == NULL) {
					szPath[0] = '\0';	// just plain file name
				}
				else {
					// strip the filename part - just keeping the full directory path
					*lastSlash = '\0';
					::_tcscat (szPath, _T ("\\"));
				}
				::_tcsncat (szPath, fileData.cFileName, _MAX_PATH);
				szPath[_MAX_PATH-1] = '\0';
				VERIFY (::FindClose (hFind));
			}
			return szPath;
		}


class	LedLineItDocManager : public CDocManager {
	private:
		typedef	CDocManager	inherited;
	public:
		LedLineItDocManager ():
			inherited ()
			{
			}

	public:
		override	void	OnFileOpen ()
			{
				CString fileName;
				int	codePage	=	0;	
				if (LedLineItDocument::DoPromptOpenFileName (&fileName, &codePage)) {
					OpenDocumentFile (fileName, codePage);
				}
			}

	public:
		override	BOOL DoPromptFileName(CString& /*fileName*/, UINT /*nIDSTitle*/, DWORD /*lFlags*/, BOOL /*bOpenFileDialog*/, CDocTemplate* /*pTemplate*/)
			{
				Led_Assert (false); // shouldn't be called - cuz we now override OnFileOpen () to avoid it...
				return false;
			}
	public:
		CDocument*	OpenDocumentFile (LPCTSTR lpszFileName, CodePage codePage)
			{
				Led_Assert (LedLineItDocument::sHiddenDocOpenArg == kIGNORECodePage);
				try {
					// If there is already an open, untitledDoc - and we open a new document - silently close the unused
					// untitled doc
					CDocument*	untitledDoc	=	NULL;
					{
						size_t		docCount	=	0;
						POSITION	tp			=	GetFirstDocTemplatePosition ();
						while (tp != NULL) {
							CDocTemplate*		t	=	GetNextDocTemplate (tp);
							Led_AssertNotNil (t);
							POSITION	dp	=	t->GetFirstDocPosition ();
							while (dp != NULL) {
								CDocument*		doc	=	t->GetNextDoc (dp);
								Led_AssertNotNil (doc);
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
					CDocument*	doc	= inherited::OpenDocumentFile (GetLongPathName (lpszFileName).c_str ());
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
		override	CDocument*	OpenDocumentFile (LPCTSTR lpszFileName)
			{
				return OpenDocumentFile (lpszFileName, kAutomaticallyGuessCodePage);
			}

};




const	char	kAppName[]	=	"LedLineIt";








/*
 ********************************************************************************
 ******************************** LedLineItApplication **************************
 ********************************************************************************
 */
LedLineItApplication		theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.
// {0FC00620-28BD-11CF-899C-00AA00580325}
static const CLSID clsid =	{ 0xfc00620, 0x28bd, 0x11cf, { 0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x25 } };


BEGIN_MESSAGE_MAP(LedLineItApplication, CWinApp)
	ON_COMMAND					(ID_APP_ABOUT,									OnAppAbout)
	ON_COMMAND					(ID_FILE_NEW,									OnFileNew)
	ON_COMMAND					(ID_FILE_OPEN,									OnFileOpen)
	ON_COMMAND					(ID_FILE_PRINT_SETUP,							OnFilePrintSetup)
	ON_COMMAND					(kToggleAutoIndentOptionCmd,					OnToggleAutoIndentOptionCommand)
	ON_UPDATE_COMMAND_UI		(kToggleAutoIndentOptionCmd,					OnToggleAutoIndentOptionUpdateCommandUI)
	ON_COMMAND					(kToggleTreatTabAsIndentCharOptionCmd,			OnToggleTreatTabAsIndentCharOptionCommand)
	ON_UPDATE_COMMAND_UI		(kToggleTreatTabAsIndentCharOptionCmd,			OnToggleTreatTabAsIndentCharOptionUpdateCommandUI)
	ON_COMMAND					(kToggleUseSmartCutNPasteCmdID,					OnToggleSmartCutNPasteOptionCommand)
	ON_UPDATE_COMMAND_UI		(kToggleUseSmartCutNPasteCmdID,					OnToggleSmartCutNPasteOptionUpdateCommandUI)
#if		qSupportSyntaxColoring
	ON_COMMAND_RANGE			(kNoSyntaxColoringCmd, kVBSyntaxColoringCmd,	OnSyntaxColoringOptionCommand)
	ON_UPDATE_COMMAND_UI_RANGE	(kNoSyntaxColoringCmd, kVBSyntaxColoringCmd,	OnSyntaxColoringOptionUpdateCommandUI)
#endif
	ON_COMMAND					(cmdChooseDefaultFontDialog,					OnChooseDefaultFontCommand)
	ON_COMMAND					(kGotoLedLineItWebPageCmdID,					OnGotoLedLineItWebPageCommand)
	ON_COMMAND					(kCheckForUpdatesWebPageCmdID,					OnCheckForUpdatesWebPageCommand)
	ON_COMMAND					(kGotoSophistsWebPageCmdID,						OnGotoSophistsWebPageCommand)
END_MESSAGE_MAP()



LedLineItApplication*	LedLineItApplication::sThe	=	NULL;

LedLineItApplication::LedLineItApplication ():
#if		qIncludeBasicSpellcheckEngine
	fSpellCheckEngine (),
#endif
	fOleTemplateServer ()
{
	Led_Require (sThe == NULL);
	sThe = this;

	#if		qIncludeBasicSpellcheckEngine && qDebug
		SpellCheckEngine_Basic::RegressionTest ();
	#endif

	#if		qIncludeBasicSpellcheckEngine && qWindows
		{
			// Place the dictionary in a reasonable - but hardwired place. Later - allow for editing that location,
			// and other spellchecking options (see SPR#1591)
			TCHAR	defaultPath[MAX_PATH+1];
			Led_Verify (::SHGetSpecialFolderPath (NULL, defaultPath, CSIDL_FLAG_CREATE | CSIDL_PERSONAL, true));
			fSpellCheckEngine.SetUserDictionary (Led_SDK_String (defaultPath) + Led_SDK_TCHAROF ("\\My LedLineIt Dictionary.txt"));
		}
	#endif
}

LedLineItApplication::~LedLineItApplication ()
{
	Led_Require (sThe == this);
	sThe = NULL;
}

LedLineItApplication&	LedLineItApplication::Get ()
{
	Led_EnsureNotNil (sThe);
	return *sThe;
}

BOOL LedLineItApplication::InitInstance ()
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


	// Create MDI Frame Window
	{
		LedLineItMainFrame*	pFrame	=	new LedLineItMainFrame ();
		if (not pFrame->LoadFrame (IDR_MAINFRAME)) {
			return false;
		}
		m_pMainWnd = pFrame;

		pFrame->DragAcceptFiles ();
	}
	


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo	cmdInfo;
	ParseCommandLine (cmdInfo);


	#if		0
	// LGP 960509 - doesn't appear to have any effect if present or not...
	// and prevents linking on Mac (CodeWarrior x-compiler).
	Enable3dControlsStatic ();	// Call this when linking to MFC statically
	#endif

	LoadStdProfileSettings (9);  // Load standard INI file options (including MRU)

	Led_Assert (m_pDocManager == NULL);
	m_pDocManager = new LedLineItDocManager ();

	AddDocTemplateForString ("LedLineIt\n\nLedLineIt\n\n.ledtext\nLedLineIt.Document\nLedLineIt Document", true);

	// Enable DDE Open, and register icons / file types with the explorer/shell
	EnableShellOpen ();
	RegisterShellFileTypes (true);	// ARG???

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	fOleTemplateServer.UpdateRegistry (OAT_INPLACE_SERVER);
	COleObjectFactory::UpdateRegistryAll ();

	#if		qWindows
		{
			class	MyRegistrationHelper : public Win32UIFileAssociationRegistrationHelper {
				private:
					typedef	Win32UIFileAssociationRegistrationHelper	inherited;
				public:
					MyRegistrationHelper ():
						inherited (::AfxGetResourceHandle ())
						{
						}

				public:
					override	bool	CheckUserSaysOKToUpdate () const
						{
							Options	o;
							if (o.GetCheckFileAssocsAtStartup ()) {
								Led_StdDialogHelper_UpdateWin32FileAssocsDialog		dlg(::AfxGetResourceHandle (), ::GetActiveWindow ());
								dlg.fAppName = Led_SDK_TCHAROF ("LedLineIt!");
								dlg.fTypeList = Led_SDK_TCHAROF (".txt,.bat");
								dlg.fKeepChecking = true;
								bool	result	= dlg.DoModal ();
								o.SetCheckFileAssocsAtStartup (dlg.fKeepChecking);
								return result;
							}
							else {
								return false;
							}
						}

			};
			MyRegistrationHelper	fileAssocHelper;
			Led_SDK_String	txtDocIcon	=	Win32UIFileAssociationInfo::kNoChange;
			Led_SDK_String	batDocIcon	=	Win32UIFileAssociationInfo::kNoChange;
			fileAssocHelper.Add (Win32UIFileAssociationInfo (
									Led_SDK_TCHAROF (".txt"),
									Led_SDK_TCHAROF ("txtfile"),
									Led_SDK_TCHAROF ("Text Document"),
									Win32UIFileAssociationInfo::kNoChange,
									Led_SDK_TCHAROF ("$EXE$ \"%1\"")
								));
			fileAssocHelper.Add (Win32UIFileAssociationInfo (
									Led_SDK_TCHAROF (".bat"),
									Led_SDK_TCHAROF ("batfile"),
									Led_SDK_TCHAROF ("MS-DOS Batch File"),
									Win32UIFileAssociationInfo::kNoChange,
									Led_SDK_TCHAROF ("$EXE$ \"%1\""),
									Win32UIFileAssociationInfo::kNoChange
								));
			fileAssocHelper.DoIt ();
		}
	#endif

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated) {
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return true;
	}

	Led_AssertNotNil (m_pMainWnd);
	m_pMainWnd->ShowWindow (m_nCmdShow);	// show window NOW so will be up if any error processing cmdline args

	// Dispatch commands specified on the command line
	if (not ProcessShellCommand (cmdInfo)) {
		return false;
	}

	return true;
}

#if		_MFC_VER >= 0x0700
void	LedLineItApplication::WinHelpInternal (DWORD_PTR dwData, UINT nCmd)
#else
void	LedLineItApplication::WinHelp (DWORD dwData, UINT nCmd)
#endif
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
	::_tcscat (directoryName, _T ("LedLineItDocs\\"));

	// wrap in try/catch, and display error if no open???
	// (NB: we use .htm instead of .html cuz some old systems - I think maybe
	//	Win95 with only Netscape 2.0 installed - only have .htm registered - not
	//  .html).
	(void)::ShellExecute (NULL, _T ("open"), _T ("index.htm"), NULL, directoryName, SW_SHOWNORMAL);
}

BOOL	LedLineItApplication::PumpMessage ()
{
	try {
		return inherited::PumpMessage ();
	}
	STD_EXCEPT_CATCHER (*this);
	return true;
}

void	LedLineItApplication::HandleMFCException (CException* /*e*/) throw ()
{
	// tmp hack for now...
	HandleUnknownException ();
}

void	LedLineItApplication::HandleBadAllocException () throw ()
{
	try {
		CDialog	errorDialog (kBadAllocExceptionOnCmdDialogID);
		errorDialog.DoModal ();
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

void	LedLineItApplication::HandleBadUserInputException () throw ()
{
	try {
		#if		qWindows
			CDialog	errorDialog (kBadUserInputExceptionOnCmdDialogID);
			errorDialog.DoModal ();
		#else
			HandleUnknownException ();
		#endif
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

void	LedLineItApplication::HandleUnknownException () throw ()
{
	try {
		CDialog	errorDialog (kUnknownExceptionOnCmdDialogID);
		errorDialog.DoModal ();
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

void	LedLineItApplication::AddDocTemplateForString (const char* tmplStr, bool connectToServer)
{
	SimpleLedTemplate* pDocTemplate = new SimpleLedTemplate (tmplStr);
	pDocTemplate->SetContainerInfo (IDR_CNTR_INPLACE);
	pDocTemplate->SetServerInfo (IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE, RUNTIME_CLASS(LedLineItInPlaceFrame));
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

void	LedLineItApplication::OnAppAbout ()
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
					const	Led_Coordinate	kPictWidth	=	437;	// must agree with ACTUAL bitmap size
					const	Led_Coordinate	kPictHeight	=	273;
					const	Led_Coordinate	kButHSluff	=	17;
					const	Led_Coordinate	kButVSluff	=	19;
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
						const	int	kVERWidth	=	230;
						::MoveWindow (w, kPictWidth/2 - kVERWidth/2, 32, kVERWidth, 16, false);
						#if		_UNICODE
							#define	kUNICODE_NAME_ADORNER	L" [UNICODE]"
						#elif	qWideCharacters
							#define	kUNICODE_NAME_ADORNER	" [Internal UNICODE]"
						#else
							#define	kUNICODE_NAME_ADORNER
						#endif
						::SetWindowText (w, _T (qLed_ShortVersionString) kUNICODE_NAME_ADORNER _T (" (") _T (__DATE__) _T (")"));
					}

					// Place hidden buttons which map to URLs
					{
						HWND	w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_InfoLedFieldID);
						Led_AssertNotNil (w);
						::MoveWindow (w, 15, 159, 142, 17, false);
						w	=	::GetDlgItem (GetHWND (), kLedStdDlg_AboutBox_LedWebPageFieldID);
						Led_AssertNotNil (w);
						::MoveWindow (w, 227, 159, 179, 17, false);
					}

					// Place OK button
					{
						HWND	w	=	::GetDlgItem (GetHWND (), IDOK);
						Led_AssertNotNil (w);
						RECT	tmp;
						::GetWindowRect (w, &tmp);
						::MoveWindow (w, kButHSluff, kPictHeight - AsLedRect (tmp).GetHeight ()-kButVSluff, AsLedRect (tmp).GetWidth (), AsLedRect (tmp).GetHeight (), false);	// width/height we should presevere
					}

					::SetWindowText (GetHWND (), _T ("About LedLineIt!"));

					return (result);
				}
		public:
			override	void	OnClickInInfoField ()
				{
					try {
						Led_URLManager::Get ().Open ("mailto:info-led@sophists.com");
					}
					catch (...) {
						// ignore for now - since errors here prent dialog from dismissing (on MacOSX)
					}
					inherited::OnClickInInfoField ();
				}

			override	void	OnClickInLedWebPageField ()
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
	MyAboutBox	dlg (m_hInstance, AfxGetMainWnd ()->m_hWnd);
	dlg.DoModal ();
}

void	LedLineItApplication::OnGotoLedLineItWebPageCommand ()
{
	Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/LedLineIt/", kAppName));
}

void	LedLineItApplication::OnCheckForUpdatesWebPageCommand ()
{
	Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName));
}

void	LedLineItApplication::OnGotoSophistsWebPageCommand ()
{
	Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/", kAppName));
}

void	LedLineItApplication::OnToggleAutoIndentOptionCommand ()
{
	Options	o;
	o.SetAutoIndent (not o.GetAutoIndent ());
}

void	LedLineItApplication::OnToggleTreatTabAsIndentCharOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	pCmdUI->Enable ();
	pCmdUI->SetCheck (Options ().GetTreatTabAsIndentChar ());
}

void	LedLineItApplication::OnToggleTreatTabAsIndentCharOptionCommand ()
{
	Options	o;
	o.SetTreatTabAsIndentChar (not o.GetTreatTabAsIndentChar ());
}

void	LedLineItApplication::OnToggleAutoIndentOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	pCmdUI->Enable ();
	pCmdUI->SetCheck (Options ().GetAutoIndent ());
}

void	LedLineItApplication::OnToggleSmartCutNPasteOptionCommand ()
{
	Options	o;
	o.SetSmartCutAndPaste (not o.GetSmartCutAndPaste ());
	UpdateViewsForPrefsChange ();
}

void	LedLineItApplication::OnToggleSmartCutNPasteOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	pCmdUI->Enable ();
	pCmdUI->SetCheck (Options ().GetSmartCutAndPaste ());
}

#if		qSupportSyntaxColoring
void	LedLineItApplication::OnSyntaxColoringOptionCommand (UINT cmdNum)
{
	Options	o;
	switch (cmdNum) {
		case	kNoSyntaxColoringCmd:			o.SetSyntaxColoringOption (Options::eSyntaxColoringNone);	break;
		case	kCPlusPlusSyntaxColoringCmd:	o.SetSyntaxColoringOption (Options::eSyntaxColoringCPlusPlus);	break;
		case	kVBSyntaxColoringCmd:			o.SetSyntaxColoringOption (Options::eSyntaxColoringVB);	break;
		default:	Led_Assert (false);
	}
	UpdateViewsForPrefsChange ();
}

void	LedLineItApplication::OnSyntaxColoringOptionUpdateCommandUI (CCmdUI* pCmdUI)
{
	Led_RequireNotNil (pCmdUI);
	pCmdUI->Enable ();
	switch (pCmdUI->m_nID) {
		case	kNoSyntaxColoringCmd:			pCmdUI->SetCheck (Options ().GetSyntaxColoringOption () == Options::eSyntaxColoringNone);	break;
		case	kCPlusPlusSyntaxColoringCmd:	pCmdUI->SetCheck (Options ().GetSyntaxColoringOption () == Options::eSyntaxColoringCPlusPlus);	break;
		case	kVBSyntaxColoringCmd:			pCmdUI->SetCheck (Options ().GetSyntaxColoringOption () == Options::eSyntaxColoringVB);	break;
		default:	Led_Assert (false);
	}
}
#endif

void	LedLineItApplication::OnChooseDefaultFontCommand ()
{
	Led_FontSpecification	fsp	=	Options ().GetDefaultNewDocFont ();

	LOGFONT	lf;
	(void)::memset (&lf, 0, sizeof (lf));
	{
		(void)::_tcscpy (lf.lfFaceName, fsp.GetFontNameSpecifier ().fName);
		Led_Assert (::_tcslen (lf.lfFaceName) < sizeof (lf.lfFaceName));	// cuz our cached entry - if valid - always short enuf...
	}
	lf.lfWeight = (fsp.GetStyle_Bold ())? FW_BOLD: FW_NORMAL;
	lf.lfItalic = (fsp.GetStyle_Italic ());
	lf.lfUnderline = (fsp.GetStyle_Underline ());
	lf.lfStrikeOut = (fsp.GetStyle_Strikeout ());

	lf.lfHeight = fsp.PeekAtTMHeight ();

	FontDlgWithNoColorNoStyles	dlog (&lf);
	if (dlog.DoModal () == IDOK) {
		Options ().SetDefaultNewDocFont (Led_FontSpecification (*dlog.m_cf.lpLogFont));
	}
}

void	LedLineItApplication::UpdateViewsForPrefsChange ()
{
	bool	smartCutNPaste	=	Options ().GetSmartCutAndPaste ();

	// Update each open view
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
				LedLineItView*	lv	=	dynamic_cast<LedLineItView*> (v);
				if (lv != NULL) {
					lv->SetSmartCutAndPasteMode (smartCutNPaste);
					#if		qSupportSyntaxColoring
						lv->ResetSyntaxColoringTable ();
					#endif
				}
			}
		}
	}
}

BOOL	LedLineItApplication::ProcessShellCommand (CCommandLineInfo& rCmdInfo)
{
	try {
		/*
		*	SPR#0775. MFC doesnt' keep track of all the files requested to open. So walk the list of file arguments
		*	a SECOND TIME! PATHETIC!
		*/
		#if		!qNo_argc_argv_MacrosSupported
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
						HANDLE	h	=	::CreateFile (rCmdInfo.m_strFileName, 0, 0, NULL, OPEN_EXISTING, 0, NULL);
						if (h == INVALID_HANDLE_VALUE) {
							POSITION		p			=	m_pDocManager->GetFirstDocTemplatePosition ();
							CDocTemplate*	pTemplate	=	(CDocTemplate*)m_pDocManager->GetNextDocTemplate (p);
							Led_AssertNotNil (pTemplate);
							CDocument*		newDoc		=	pTemplate->OpenDocumentFile (NULL);		// new doc
							if (newDoc != NULL) {
								// Set path and title so if you just hit save - you are promted to save in the name you specified, but
								// there are visual clues that this isn't the orig file
								newDoc->SetPathName (pszParam, false);
								newDoc->SetTitle (newDoc->GetTitle () + Led_SDK_TCHAROF (" {new}"));
								continue;	// with outer for loop - ignore exception
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




// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
