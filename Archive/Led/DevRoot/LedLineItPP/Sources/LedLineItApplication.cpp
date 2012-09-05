/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Sources/LedLineItApplication.cpp,v 2.27 2004/02/12 21:02:29 lewis Exp $
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
 *	Revision 2.27  2004/02/12 21:02:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.26  2003/12/13 02:00:06  lewis
 *	small changes so uses new spellcheck code (and stores engine in applicaiton object)
 *	
 *	Revision 2.25  2003/03/20 17:16:03  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.24  2003/03/13 18:09:02  lewis
 *	fix aboutbox code to use qWideCharacters instead of qLed_CharacterSet and to use kUNICODE_NAME_ADORNER at all (for MacOS)
 *	
 *	Revision 2.23  2003/03/11 21:45:07  lewis
 *	SPR#1287 - use Gestalt to see if Aqua UI is present and remove QUIT menu if it is. Also - if HMGetHelpMenuHandle() == NULL (as it is in OSX) - then append a Help menu and store our help menu items in that
 *	
 *	Revision 2.22  2003/03/11 19:37:49  lewis
 *	SPR#1249 - get compiling with UNICODE/Carbon
 *	
 *	Revision 2.21  2003/03/11 02:40:36  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 2.20  2003/01/23 01:27:45  lewis
 *	SPR#1257 - try/catch for Led_URLManager::Open call in about box
 *	
 *	Revision 2.19  2002/05/06 21:31:29  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.18  2001/11/27 00:28:28  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.17  2001/09/12 02:23:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.16  2001/09/12 00:22:18  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 2.15  2001/09/06 22:16:39  lewis
 *	SPR#1003- Mac about box picture
 *	
 *	Revision 2.14  2001/08/30 01:01:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.13  2001/07/19 21:06:20  lewis
 *	SPR#0960- CW6Pro support
 *	
 *	Revision 2.12  2000/10/19 15:51:54  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 2.11  2000/10/16 19:34:28  lewis
 *	prelim support for Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.10  2000/10/14 13:55:00  lewis
 *	change URL-goto-address
 *	
 *	Revision 2.9  2000/04/16 14:39:55  lewis
 *	support new PP excpetion scheme
 *	
 *	Revision 2.8  1999/08/28 18:22:55  lewis
 *	up version# of DOCFILE to navigate to on web to 2.3
 *	
 *	Revision 2.7  1998/10/30 20:24:55  lewis
 *	Use vector<> instead of Led_Array<>
 *	
 *	Revision 2.6  1997/12/24  04:52:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1997/12/24  04:01:09  lewis
 *	compat with cwpro2 compiler
 *
 *	Revision 2.4  1997/09/29  18:07:35  lewis
 *	Lose qLedFirstIndex
 *
 *	Revision 2.3  1997/07/27  16:03:37  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1997/06/18  03:36:16  lewis
 *	Support PowerPlant 1.7 (CWPro1)
 *
 *	Revision 2.1  1997/03/04  20:27:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:51:52  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */
#include	<Balloons.h>
#include	<Gestalt.h>
#include	<TextUtils.h>
#include	<ToolUtils.h>
#include	<StandardFile.h>

#include	<LCaption.h>
#include	<LDialogBox.h>
#include	<LEditField.h>
#include	<LGrowZone.h>
#include	<LMenu.h>
#include	<LMenuBar.h>
#include	<LPlaceHolder.h>
#include	<LPrintout.h>
#include	<LStdControl.h>
#include	<LPicture.h>
#include	<LTabGroup.h>
#include	<LTextButton.h>
#include	<LWindow.h>
#include	<TArrayIterator.h>
#include	<PP_Messages.h>
#include	<PP_Resources.h>
#include	<UAppleEventsMgr.h>
#include	<UDesktop.h>
#include	<URegistrar.h>
#include	<UMemoryMgr.h>
#include	<UModalDialogs.h>
#include	<UStandardDialogs.h>

#include	"LedGDI.h"
#include	"LedStdDialogs.h"
#include	"TextInteractor.h"

#include	"LedLineItDocument.h"
#include	"LedLineItResources.h"

#include	"LedLineItApplication.h"


static	Handle	sDeepShitCheeseBuf	=	NULL;	// so no mem alerts don't crash...

inline	void	DoStringyAlert (short alertID, const ConstStr255Param p0 = NULL, const ConstStr255Param p1 = NULL, const ConstStr255Param p2 = NULL, const ConstStr255Param p3 = NULL)
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
				Led_CheckSomeLocalHeapRAMAvailable (2*1024);	// empiricly arrived at how much needed to avoid crash
			}
			catch (...) {
				Led_BeepNotify ();
				return;
			}
			::ParamText (p0==NULL?"\p":p0, p1==NULL?"\p":p1, p2==NULL?"\p":p2, p3==NULL?"\p":p3);
			::InitCursor ();
			::CautionAlert (alertID, nil);
		}
	}





const	char	kAppName[]	=	"LedLineIt";


//const	char	kLedLineItUserPageURL[]	=	"http://www.sophists.com/Led/LedLineIt/Default.asp?From=LedLineIt30M";
//const	char	kSophistSolutionsURL[]	=	"http://www.sophists.com/Default.asp?From=LedLineIt30M";






/*
 ********************************************************************************
 ********************************* LedLineItApplication *************************
 ********************************************************************************
 */
LedLineItApplication*	LedLineItApplication::sThe	=	NULL;




LedLineItApplication::LedLineItApplication ():
	LDocApplication (),
#if		qIncludeBasicSpellcheckEngine
	fSpellCheckEngine (),
#endif
	fHelpMenuItem (0),
	fGotoLedLineItWebPageMenuItem (0),
	fGotoSophistsWebPageMenuItem (0),
	fCheckForUpdatesWebPageMenuItem (0),
	fSearchParameters (),
	fLastLowMemWarnAt (0.0f)
{
	Led_Require (sThe == NULL);
	sThe = this;

	#if		qIncludeBasicSpellcheckEngine && qDebug
		SpellCheckEngine_Basic::RegressionTest ();
	#endif

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

	// Always make sure sleep time no longer than the caret blink time.
	// But default to 6 ticks (PP's default) - 0.1 seconds.
	SetSleepTime (Led_Min (6, GetCaretTime ()));
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

void	LedLineItApplication::StartUp ()
{
	try {
		ObeyCommand (cmd_New, nil);		// create a new window
	}
	catch (OSErr err) {
		HandleMacOSException (err);
	}
	catch (bad_alloc) {
		HandleMacOSException (memFullErr);
	}
	catch (const LException& err) {
		HandlePowerPlantException ((OSErr)err.GetErrorCode ());
	}
	catch (ExceptionCode err) {
		HandlePowerPlantException (err);
	}
	catch (TextInteractor::BadUserInput&) {
		HandleBadUserInputException ();
	}
	catch (...) {
		HandleUnknownException ();
	}
}

void	LedLineItApplication::MakeMenuBar ()
{
	inherited::MakeMenuBar();

	// Base class has already constructed the menu bar, by this point...
	// So we can add font menus, and associate command numbers...
	{
		MenuRef fontMenuHandle	=	::GetMenuHandle (cmd_FontMenu);
		Led_AssertNotNil (fontMenuHandle);
		::AppendResMenu (fontMenuHandle, 'FONT');
		Led_AssertNotNil (LMenuBar::GetCurrentMenuBar());
		LMenu*	fontMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (cmd_FontMenu);
		Led_AssertNotNil (fontMenu);
		size_t	nMenuItems	=	::CountMenuItems (fontMenu->GetMacMenuH ());
		for (size_t i = 1; i <= nMenuItems; i++) {
			fontMenu->SetCommand (i, i-1+kBaseFontNameCmdID);	// make first cmd = kBaseFontNameCmdID
		}
	}

	bool	aquaUI	=	false;
	{
		SInt32	gestaltResponse	=	0;
		aquaUI	=	 (::Gestalt (gestaltMenuMgrAttr, &gestaltResponse) == noErr) and (gestaltResponse & gestaltMenuMgrAquaLayoutMask);
	}

	// Add a special help menu item you launch our help file
	// (see NewIM Essential Macintosh Toolbox, 3-68)
	{
		MenuHandle	helpMenu	=	NULL;
#if		!TARGET_CARBON
		if (::HMGetHelpMenuHandle (&helpMenu) != noErr) {
			helpMenu = NULL;
		}
#endif
		if (helpMenu == NULL) {
			// if there is no helpMenu, then add our own...
			helpMenu = ::NewMenu (kHelpMenuID, "\pHelp");
			Led_AssertNotNil (helpMenu);
			MenuID	append	=	0;
			::InsertMenu (helpMenu, append);
			
		}
		if (helpMenu != NULL) {
			::AppendMenu (helpMenu, "\pBrowse Local Help");
			fHelpMenuItem = ::CountMenuItems (helpMenu);
			::AppendMenu (helpMenu, "\p-");
			::AppendMenu (helpMenu, "\pxxx");	// Text in SetMenuItemText() call cuz idiodic mac toolbox interprets '!' character!
			::SetMenuItemText (helpMenu, ::CountMenuItems (helpMenu), "\pGoto LedLineIt! Web Page");
			fGotoLedLineItWebPageMenuItem = ::CountMenuItems (helpMenu);
			::AppendMenu (helpMenu, "\pGoto Sophist Solutions Web Page");
			fGotoSophistsWebPageMenuItem = ::CountMenuItems (helpMenu);
			::AppendMenu (helpMenu, "\pCheck For LedLineIt! Updates Web Page");
			fCheckForUpdatesWebPageMenuItem = ::CountMenuItems (helpMenu);
		}
	}

	// For Aqua UI (OSX) - lose the Quit menu item.
	if (aquaUI) {
		MenuRef	fileMenu	=	::GetMenuHandle (kFileMenuID);
		Led_AssertNotNil (fileMenu);
		// Lose the separator AND the Quit menu item.
		::DeleteMenuItem (fileMenu, ::CountMenuItems (fileMenu));
		::DeleteMenuItem (fileMenu, ::CountMenuItems (fileMenu));		
	}
}

void	LedLineItApplication::ProcessNextEvent ()
{
	if (sDeepShitCheeseBuf == NULL) {
		sDeepShitCheeseBuf = ::NewHandle (4*1024);
	}
	try {
		inherited::ProcessNextEvent ();
	}
	catch (OSErr err) {
		HandleMacOSException (err);
	}
	catch (bad_alloc) {
		HandleMacOSException (memFullErr);
	}
	catch (const LException& err) {
		HandlePowerPlantException ((OSErr)err.GetErrorCode ());
	}
	catch (ExceptionCode err) {
		HandlePowerPlantException (err);
	}
	catch (TextInteractor::BadUserInput&) {
		HandleBadUserInputException ();
	}
	catch (...) {
		HandleUnknownException ();
	}
}

void	LedLineItApplication::HandleAppleEvent (const AppleEvent& inAppleEvent, AppleEvent& outAEReply, AEDesc& outResult, long inAENumber)
{
	try {
		inherited::HandleAppleEvent (inAppleEvent, outAEReply, outResult, inAENumber);
	}
	catch (OSErr err) {
		HandleMacOSException (err);
	}
	catch (bad_alloc) {
		HandleMacOSException (memFullErr);
	}
	catch (const LException& err) {
		HandlePowerPlantException ((OSErr)err.GetErrorCode ());
	}
	catch (ExceptionCode err) {
		HandlePowerPlantException (err);
	}
	catch (TextInteractor::BadUserInput&) {
		HandleBadUserInputException ();
	}
	catch (...) {
		HandleUnknownException ();
	}
}

void	LedLineItApplication::HandleMacOSException (OSErr err)
{
	switch (err) {
		case	memFullErr: {
			// ALSO, FREE ANY MEMORY WE CAN...
			TArray<LDocument*>&			docList	=	LDocument::GetDocumentList ();
			TArrayIterator<LDocument*>	iterator (docList);
			LDocument*	theDoc	=	NULL;
			while (iterator.Next (theDoc)) {
				Led_AssertMember (theDoc, LedLineItDocument);
				LedLineItDocument*	d	=	dynamic_cast<LedLineItDocument*> (theDoc);
				d->PurgeUnneededMemory ();
			}

			DoStringyAlert (kMemoryExceptionAlertID);
		}
		break;

		default: {
			Str255	tmp;
			NumToString (err, tmp);
			DoStringyAlert (kGenericMacOSExceptionAlertID, tmp);
		}
		break;
	}
}

void	LedLineItApplication::HandlePowerPlantException (ExceptionCode err)
{
	if (err > 32767 or err < -32768) {
		Str255	tmp;
		::NumToString (err, tmp);
		::DoStringyAlert (kPowerPlantExceptionAlertID, tmp);
	}
	else {
		Led_Assert (err == OSErr (err));
		HandleMacOSException (OSErr (err));
	}
}

void	LedLineItApplication::HandleBadUserInputException () throw ()
{
	try {
		DoStringyAlert (kUnknownExceptionAlertID);
	}
	catch (...) {
		Led_BeepNotify ();
	}
}

void	LedLineItApplication::HandleUnknownException ()
{
	DoStringyAlert (kUnknownExceptionAlertID);
}

Boolean	LedLineItApplication::ObeyCommand (CommandT	inCommand, void* ioParam)
{
	Boolean	cmdHandled = true;

	if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fHelpMenuItem == LoWord(-inCommand)) {
		OnHelpMenuCommand ();
		return true;
	}
	if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fGotoLedLineItWebPageMenuItem == LoWord(-inCommand)) {
		OnGotoLedLineItWebPageCommand ();
		return true;
	}
	if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fGotoSophistsWebPageMenuItem == LoWord(-inCommand)) {
		OnGotoSophistsWebPageCommand ();
		return true;
	}
	if ((HiWord ((-inCommand)) == kHMHelpMenuID or HiWord ((-inCommand)) == kHelpMenuID) and fCheckForUpdatesWebPageMenuItem == LoWord(-inCommand)) {
		OnCheckForUpdatesWebPageCommand ();
		return true;
	}

	if (inCommand >= kBaseWindowCmdID and inCommand <= kLastWindowCmdID) {
		size_t						windowIdx	=	(inCommand-kBaseWindowCmdID);
		const vector<LWindow*>&	windows		=	LedLineItDocument::GetDocumentWindows ();
		if (windowIdx < windows.size ()) {
			LWindow*	w	=	windows[windowIdx];
			Led_AssertNotNil (w);
			UDesktop::SelectDeskWindow (w);
		}
		else {
			Led_Assert (false);	// we shouldn't get these!
		}
		return true;
	}

	switch (inCommand) {
		default:
			cmdHandled = inherited::ObeyCommand (inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}

//	Pass back status of a (menu) command
void	LedLineItApplication::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
											 UInt16& outMark, Str255 outName
											)
{
	if (inCommand >= kBaseWindowCmdID and inCommand <= kLastWindowCmdID) {
		size_t					windowIdx	=	(inCommand-kBaseWindowCmdID);
		const vector<LWindow*>&	windows		=	LedLineItDocument::GetDocumentWindows ();
		if (windowIdx < windows.size ()) {
			LWindow*	w	=	windows[windowIdx];
			Led_AssertNotNil (w);
			outEnabled = true;
			(void)w->GetDescriptor (outName);
			outMark = UDesktop::WindowIsSelected (w)? checkMark: 0;
		}
		else {
			Led_Assert (false);	// we shouldn't get these!
		}
		outUsesMark = true;
		return;
	}

	outUsesMark = false;
	switch (inCommand) {
		default:
			inherited::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
			break;
	}
}

void	LedLineItApplication::OnHelpMenuCommand ()
{
	try {
		FSSpec	fsp;
		Led_ThrowOSErr (::FSMakeFSSpec (0, 0, "\p:LedLineItDocs:index.html", &fsp));
		string	helpURL	=	Led_URLManager::Get ().FileSpecToURL (fsp);
		Led_URLManager::Get ().Open (helpURL);
	}
	catch (...) {
		DoStringyAlert (kCannotOpenHelpFileAlertID);
	}
}

void	LedLineItApplication::OnGotoLedLineItWebPageCommand ()
{
	try {
		Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/LedLineIt/", kAppName));
//		Led_URLManager::Get ().Open (kLedLineItUserPageURL);
	}
	catch (...) {
		DoStringyAlert (kCannotOpenWebPageAlertID);
	}
}

void	LedLineItApplication::OnGotoSophistsWebPageCommand ()
{
	try {
		Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/", kAppName));
//		Led_URLManager::Get ().Open (kSophistSolutionsURL);
	}
	catch (...) {
		DoStringyAlert (kCannotOpenWebPageAlertID);
	}
}

void	LedLineItApplication::OnCheckForUpdatesWebPageCommand ()
{
	try {
		Led_URLManager::Get ().Open (MakeSophistsAppNameVersionURL ("/Led/CheckForUpdates.asp", kAppName));
	}
	catch (...) {
		DoStringyAlert (kCannotOpenWebPageAlertID);
	}
}

void	LedLineItApplication::UseIdleTime (const EventRecord& inMacEvent)
{
	inherited::UseIdleTime (inMacEvent);
	
	/*
	 *	Check to see if we're too low on memory.
	 */
	const	float	kIntervalBetweenWarnings	=	10.0f;
	if (fLastLowMemWarnAt + kIntervalBetweenWarnings < ::Led_GetTickCount ()) {
		bool	enufLocalMemory	=	true;
		bool	enufMemory		=	true;
		try {
			Led_CheckSomeLocalHeapRAMAvailable (8*1024);
		}
		catch (...) {
			enufLocalMemory = false;
		}
		try {
			::DisposeHandle (Led_DoNewHandle (8*1024));
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
	
	#if		qUseMacTmpMemForAllocs && 0
		// Didn't help - See SPR#0351
		float	sLastSendMemBackTryAt	=	0;
		if (sLastSendMemBackTryAt + 30.0f < ::Led_GetTickCount ()) {
			extern	void	TryToSendSomeTmpMemBackToOS ();
			TryToSendSomeTmpMemBackToOS ();
			sLastSendMemBackTryAt = ::Led_GetTickCount ();
		}
	#endif
}

void	LedLineItApplication::OpenDocument (FSSpec* inMacFSSpec)
{
	LedLineItDocument*	doc	=	new LedLineItDocument (this);
	try {
		doc->BuildDocWindow (inMacFSSpec);
	}
	catch (...) {
		delete doc;
		throw;
	}
}

LModelObject*	LedLineItApplication::MakeNewDocument ()
{
	LedLineItDocument*	doc	=	new LedLineItDocument (this);
	try {
		doc->BuildDocWindow (NULL);
	}
	catch (...) {
		delete doc;
		throw;
	}
	return doc;
}

//	Prompt the user to select a document to open
void	LedLineItApplication::ChooseDocument ()
{
#if		TARGET_CARBON
	FSSpec	fileSpec;
	if (PP_StandardDialogs::AskChooseOneFile ('TEXT', fileSpec, kNavAllFilesInPopup | kNavSelectAllReadableItem)) {
		OpenDocument (&fileSpec);
	}
#else
	StandardFileReply	result;
	UDesktop::Deactivate();
	::StandardGetFile (NULL, -1, NULL, &result);
	UDesktop::Activate();
	if (result.sfGood) {
		OpenDocument (&result.sfFile);
	}
#endif
}

void	LedLineItApplication::ShowAboutBox ()
{
	class	MyAboutBox : public Led_StdDialogHelper_AboutBox {
		private:
			typedef	Led_StdDialogHelper_AboutBox	inherited;
		public:
			override	void	PreDoModalHook ()
				{
					#if		_UNICODE
						#define	kUNICODE_NAME_ADORNER	L" [UNICODE]"
					#elif	qWideCharacters
						#define	kUNICODE_NAME_ADORNER	" [Internal UNICODE]"
					#else
						#define	kUNICODE_NAME_ADORNER
					#endif
					inherited::PreDoModalHook ();
					const	short	kPictHeight	=	273;
					const	short	kPictWidth	=	437;
					Led_SDK_String	verStr	=	Led_SDK_String (qLed_ShortVersionString) + kUNICODE_NAME_ADORNER " (" + __DATE__ + ")";
					SimpleLayoutHelper (kPictHeight, kPictWidth, Led_Rect (159, 15, 17, 142), Led_Rect (159, 227, 17, 179), verStr);
				}
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
						//Led_URLManager::Get ().Open (kLedLineItUserPageURL);
					}
					catch (...) {
						// ignore for now - since errors here prent dialog from dismissing (on MacOSX)
					}
					inherited::OnClickInLedWebPageField ();
				}
	};
	MyAboutBox	dlg;
	dlg.DoModal ();
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

