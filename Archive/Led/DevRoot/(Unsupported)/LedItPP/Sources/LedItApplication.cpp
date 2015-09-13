/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Sources/LedItApplication.cpp,v 2.27 2000/10/19 15:51:35 lewis Exp $
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
 *	Revision 2.27  2000/10/19 15:51:35  lewis
 *	use new portable About code and Find code. Further cleanups/improvements.
 *	
 *	Revision 2.26  2000/10/16 19:34:43  lewis
 *	prelim support for Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.25  2000/10/14 13:56:55  lewis
 *	change URL-goto-address
 *	
 *	Revision 2.24  2000/06/15 21:57:25  lewis
 *	a little closer to having SPR#0785 working - setting spacing from Mac test app
 *	
 *	Revision 2.23  2000/04/16 14:40:12  lewis
 *	support new PP excpetion scheme
 *	
 *	Revision 2.22  1999/08/28 18:24:44  lewis
 *	up version# of DOCFILE to navigate to on web to 2.3
 *	
 *	Revision 2.21  1998/10/30 15:03:48  lewis
 *	use vector<> - not Led_Array.
 *	
 *	Revision 2.20  1998/05/05  00:35:36  lewis
 *	*** empty log message ***
 *
 *	Revision 2.19  1997/12/24  04:42:23  lewis
 *	*** empty log message ***
 *
 *	Revision 2.18  1997/12/24  03:56:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.17  1997/09/29  17:59:38  lewis
 *	Lose qLedFirstIndex
 *
 *	Revision 2.16  1997/07/27  16:02:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.15  1997/06/18  03:31:48  lewis
 *	changes for new PowerPlant 1.7 (CWPro1).
 *
 *	Revision 2.14  1997/03/04  20:36:25  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1996/12/13  18:11:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.12  1996/12/05  21:25:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1996/10/31  01:07:36  lewis
 *	Slight re-org of document creation to avoid problems with exceptions.
 *
 *	Revision 2.10  1996/09/30  15:10:46  lewis
 *	Cleaned up file format support and now support html as well.
 *	Added fSearchParameters so find string same across docs.
 *	And added window menu support.
 *
 *	Revision 2.9  1996/09/03  15:15:43  lewis
 *	Added RTF support
 *
 *	Revision 2.8  1996/09/01  15:45:40  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/07/19  16:49:52  lewis
 *	Chaned docs file name.
 *
 *	Revision 2.6  1996/07/03  01:45:42  lewis
 *	Redid dialogs using PPobs instead of DLOG/DITLs.
 *	Related: did a bunch of RegisterClass calls - some for LedItView dialogs.
 *
 *	Revision 2.5  1996/06/01  02:56:01  lewis
 *	more cleanup of low memory handling etc, idle time warning if low on mem.
 *	Also, totally new about box code (launches urls).
 *
 *	Revision 2.4  1996/05/23  20:45:39  lewis
 *	adjusted font menu ids by one.
 *	don't put up no-mem alert on excpetions more than once every 4 seconds (just beep if it happens again).
 *	??? maybe the beep is what I should have done, but didnt do?
 *
 *	Revision 2.3  1996/05/05  14:58:08  lewis
 *	Cleaned up alert handling, and now have separate alerts for a
 *	bunch of different cases (macos/no mem etc), and we catch them
 *	in a couple xtra places we'd forgetton (startup?)
 *
 *	Revision 2.2  1996/04/18  16:07:10  lewis
 *	Mostly override LedItApplication::ProcessNextEvent () to catch exceptions
 *	and do a half-decent job of reporting messages about troubles.
 *	And when out out of memory, purge uneeded memory.
 *	Call EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();, and
 *	set pict resources for a few embeddings displays.
 *
 *	Revision 2.1  1996/03/04  08:21:53  lewis
 *	Assign picts for URL embedding and UnknwonType object embedding.
 *
 *	Revision 2.0  1996/02/26  23:29:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#include	<Balloons.h>
#include	<ToolUtils.h>

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

#include	"LedStdDialogs.h"
#include	"StyledTextEmbeddedObjects.h"

#include	"FilteredFilePicker.h"
#include	"LedItDocument.h"
#include	"LedItResources.h"

#include	"LedItApplication.h"




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









const	char	kLedItUserPageURL[]		=	"http://www.sophists.com/Led/LedIt/Default.asp?From=LedIt30M";
const	char	kSophistSolutionsURL[]	=	"http://www.sophists.com/Default.asp?From=LedIt30M";





/*
 ********************************************************************************
 ********************************* LedItApplication *****************************
 ********************************************************************************
 */
LedItApplication*	LedItApplication::sThe	=	NULL;




LedItApplication::LedItApplication ():
	LDocApplication (),
	fHelpMenuItem (0),
	fGotoLedItWebPageMenuItem (0),
	fGotoSophistsWebPageMenuItem (0),
	fSearchParameters (),
	fLastLowMemWarnAt (0.0f)
{
	Led_Require (sThe == NULL);
	sThe = this;

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

	EmbeddedObjectCreatorRegistry::Get ().AddStandardTypes ();

	// Tell Led about the picture resources it needs to render some special embedding markers
	StandardURLStyleMarker::sURLPict = (Picture**)::GetResource ('PICT', kURLPictID);
	StandardUnknownTypeStyleMarker::sUnknownPict = (Picture**)::GetResource ('PICT', kUnknownEmbeddingPictID);
	StandardDIBStyleMarker::sUnsupportedFormatPict = (Picture**)::GetResource ('PICT', kUnsupportedDIBFormatPictID);

	// Always make sure sleep time no longer than the caret blink time.
	// But default to 6 ticks (PP's default) - 0.1 seconds.
	SetSleepTime (Led_Min (6, GetCaretTime ()));
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

void	LedItApplication::StartUp ()
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
	catch (...) {
		HandleUnknownException ();
	}
}

void	LedItApplication::MakeMenuBar ()
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
		size_t	nMenuItems	=	::CountMItems (fontMenu->GetMacMenuH ());
		for (size_t i = 1; i <= nMenuItems; i++) {
			fontMenu->SetCommand (i, i-1+kBaseFontNameCmdID);	// make first cmd = kBaseFontNameCmdID
		}
	}

	// Add a special help menu item you launch our help file
	// (see NewIM Essential Macintosh Toolbox, 3-68)
	{
		MenuHandle	helpMenu	=	NULL;
		if (::HMGetHelpMenuHandle (&helpMenu) == noErr and helpMenu != NULL) {
			::AppendMenu (helpMenu, "\pBrowse Local Help");
			fHelpMenuItem = ::CountMenuItems (helpMenu);
			::AppendMenu (helpMenu, "\p-");
			::AppendMenu (helpMenu, "\pxxx");	// Text in SetMenuItemText() call cuz idiodic mac toolbox interprets '!' character!
			::SetMenuItemText (helpMenu, ::CountMenuItems (helpMenu), "\pGoto LedIt! Web Page");
			fGotoLedItWebPageMenuItem = ::CountMenuItems (helpMenu);
			::AppendMenu (helpMenu, "\pGoto Sophist Solutions Web Page");
			fGotoSophistsWebPageMenuItem = ::CountMenuItems (helpMenu);
		}
	}
}

void	LedItApplication::ProcessNextEvent ()
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
	catch (...) {
		HandleUnknownException ();
	}
}

void	LedItApplication::HandleAppleEvent (const AppleEvent& inAppleEvent, AppleEvent& outAEReply, AEDesc& outResult, long inAENumber)
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
	catch (...) {
		HandleUnknownException ();
	}
}

void	LedItApplication::HandleMacOSException (OSErr err)
{
	switch (err) {
		case	memFullErr: {
			// ALSO, FREE ANY MEMORY WE CAN...
			TArray<LDocument*>&			docList	=	LDocument::GetDocumentList ();
			TArrayIterator<LDocument*>	iterator (docList);
			LedItDocument*	theDoc	=	NULL;
			while (iterator.Next (theDoc)) {
				Led_AssertMember (theDoc, LedItDocument);
				theDoc->PurgeUnneededMemory ();
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

void	LedItApplication::HandlePowerPlantException (ExceptionCode err)
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

void	LedItApplication::HandleUnknownException ()
{
	DoStringyAlert (kUnknownExceptionAlertID);
}

Boolean	LedItApplication::ObeyCommand (CommandT	inCommand, void* ioParam)
{
	Boolean	cmdHandled = true;

	if (HiWord((-inCommand)) == kHMHelpMenuID and fHelpMenuItem == LoWord(-inCommand)) {
		OnHelpMenuCommand ();
		return true;
	}
	if (HiWord((-inCommand)) == kHMHelpMenuID and fGotoLedItWebPageMenuItem == LoWord(-inCommand)) {
		OnGotoLedItWebPageCommand ();
		return true;
	}
	if (HiWord((-inCommand)) == kHMHelpMenuID and fGotoSophistsWebPageMenuItem == LoWord(-inCommand)) {
		OnGotoSophistsWebPageCommand ();
		return true;
	}

	if (inCommand >= kBaseWindowCmdID and inCommand <= kLastWindowCmdID) {
		size_t					windowIdx	=	(inCommand-kBaseWindowCmdID);
		const vector<LWindow*>&	windows		=	LedItDocument::GetDocumentWindows ();
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
void	LedItApplication::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
											 Char16& outMark, Str255 outName
											)
{
	if (inCommand >= kBaseWindowCmdID and inCommand <= kLastWindowCmdID) {
		size_t					windowIdx	=	(inCommand-kBaseWindowCmdID);
		const vector<LWindow*>&	windows		=	LedItDocument::GetDocumentWindows ();
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

void	LedItApplication::OnHelpMenuCommand ()
{
	try {
		FSSpec	fsp;
		Led_ThrowOSErr (::FSMakeFSSpec (0, 0, "\p:LedItDocs:index.html", &fsp));
		string	helpURL	=	Led_URLManager::Get ().FileSpecToURL (fsp);
		Led_URLManager::Get ().Open (helpURL);
	}
	catch (...) {
		DoStringyAlert (kCannotOpenHelpFileAlertID);
	}
}

void	LedItApplication::OnGotoLedItWebPageCommand ()
{
	try {
		Led_URLManager::Get ().Open (kLedItUserPageURL);
	}
	catch (...) {
		DoStringyAlert (kCannotOpenWebPageAlertID);
	}
}

void	LedItApplication::OnGotoSophistsWebPageCommand ()
{
	try {
		Led_URLManager::Get ().Open (kSophistSolutionsURL);
	}
	catch (...) {
		DoStringyAlert (kCannotOpenWebPageAlertID);
	}
}

void	LedItApplication::UseIdleTime (const EventRecord& inMacEvent)
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

void	LedItApplication::OpenDocument (FSSpec* inMacFSSpec)
{
	LedItDocument*	doc	=	new LedItDocument (this, eUnknownFormat);
	try {
		doc->BuildDocWindow (inMacFSSpec);
	}
	catch (...) {
		delete doc;
		throw;
	}
}

void	LedItApplication::OpenDocument (FSSpec* inMacFSSpec, Led_FileFormat format)
{
	LedItDocument*	doc	=	new LedItDocument (this, format);
	try {
		doc->BuildDocWindow (inMacFSSpec);
	}
	catch (...) {
		delete doc;
		throw;
	}
}

LModelObject*	LedItApplication::MakeNewDocument ()
{
	LedItDocument*	doc	=	new LedItDocument (this, eDefaultFormat);
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
void	LedItApplication::ChooseDocument ()
{
	static	FilteredSFGetDLog::TypeSpec	typeList[]	=	{
		{	"HTML file",						kTEXTFileType				},
		{	"Led Rich Text Format",				kLedPrivateDocumentFileType	},
		{	"Microsoft Rich Text Format (RTF)",	kTEXTFileType				},
		{	"Text file",						kTEXTFileType				},
	};
	StandardFileReply	reply;
	FilteredSFGetDLog	filteredPicker (typeList, (sizeof typeList)/(sizeof typeList[0]));

	bool 			typeSpecified	=	false;
	size_t 			typeIndex		=	0;
	Led_FileFormat	format			=	eUnknownFormat;
	if (filteredPicker.PickFile (&reply, &typeSpecified, &typeIndex)) {
		if (typeSpecified) {
			switch (typeIndex) {
				case	1:	format = eHTMLFormat; break;
				case	2:	format = eLedPrivateFormat; break;
				case	3:	format = eRTFFormat; break;
				case	4:	format = eTextFormat; break;
				default:	Led_Assert (false);
			}
		}
		OpenDocument (&reply.sfFile, format);
	}
}


#if 0

	// Really should go inside LedItApplication::ShowAboutBox (), but for
	// MWERKS CW10 and earlier don't like it when we do - gens syntax error.
	// I think this is a compiler bug, but not important enuf to get excited about
	class	AboutDlg : public LDialogBox {
		public:
			static	LDialogBox*	CreateDialogBoxStream (LStream* inStream)
				{
					return (new AboutDlg (inStream));
				}
			AboutDlg (LStream* inStream):
					LDialogBox (inStream),
					fHandler (NULL)
				{
				}
			override	Boolean	HandleKeyPress (const EventRecord& /*inKeyEvent*/)
				{
					Led_AssertNotNil (fHandler);
					fHandler->ListenToMessage (msg_OK, NULL);
					return true;
				}
			override	void	Click (SMouseDownEvent& inMouseDown)
				{
					LDialogBox::Click (inMouseDown);
					Led_AssertNotNil (fHandler);
					fHandler->ListenToMessage (msg_OK, NULL);
				}
		StDialogHandler*	fHandler;
	};
#endif

void	LedItApplication::ShowAboutBox ()
{
#if 1
	class	MyAboutBox : public Led_StdDialogHelper_AboutBox {
		private:
			typedef	Led_StdDialogHelper_AboutBox	inherited;
		public:
			override	void	PreDoModalHook ()
				{
					inherited::PreDoModalHook ();
					const	short	kPictHeight	=	283;
					const	short	kPictWidth	=	379;
					Led_SDK_String	verStr	=	Led_SDK_String (qLed_ShortVersionString) + " (" + __DATE__ + ").";
					SimpleLayoutHelper (kPictHeight, kPictWidth, Led_Rect (182, 124, 16, 139), Led_Rect (198, 18, 14, 176), verStr);
				}
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
	#if		qMacOS
		MyAboutBox	dlg;
	#elif	qWindows
		MyAboutBox	dlg (ghInstance, fhWnd);
	#endif
	dlg.DoModal ();
#else
	class	PeekerAtOldFunc : public URegistrar {
		public:
			static	ClassCreatorFunc	LookupCreatorFunct (ClassIDT inClassID)
				{
					SClassTableEntry	theEntry	=	{ inClassID, NULL };
					ArrayIndexT			index		=	sClassTable->FetchIndexOf (theEntry);
					return (index == LArray::index_Bad)? NULL: (sClassTable->FetchItemAt (index, theEntry),theEntry.creatorFunc);
				}
	};

	ClassCreatorFunc	oldCreatorFunct	=	 PeekerAtOldFunc::LookupCreatorFunct (LDialogBox::class_ID);
	URegistrar::RegisterClass (LDialogBox::class_ID,	(ClassCreatorFunc)AboutDlg::CreateDialogBoxStream);

	class	MyDialogHandler : public StDialogHandler {
		public:
			MyDialogHandler (LCommander* inSuper):
				StDialogHandler (kLedAboutBoxID, inSuper)
				{
					{
						LCaption*	versionField	=	(LCaption*)GetDialog ()->FindPaneByID ('VERS');
						Led_AssertNotNil (versionField);
						Str255	verString;
						::strcpy ((char*)verString, qLed_ShortVersionString);
						::strcat ((char*)verString, " (");
						::strcat ((char*)verString, __DATE__);
						::strcat ((char*)verString, ").");
						::c2pstr ((char*)verString);
						versionField->SetDescriptor (verString);
					}

					{
						AboutDlg*	dlg	=	(AboutDlg*)GetDialog ();
						Led_AssertNotNil (dlg);
						Led_Assert (dlg->fHandler == NULL);
						dlg->fHandler = this;
					}
				}
			override	void	FindCommandStatus (CommandT /*inCommand*/, Boolean& outEnabled, Boolean& /*outUsesMark*/, Char16& /*outMark*/, Str255 /*outName*/)
				{
					outEnabled = false;	// see base class (StDialogHandler) description in this method...
				}
			override	void	ListenToMessage (MessageT inMessage, void* ioParam)
				{
					switch (inMessage) {
						case	'leml':		Led_URLManager::Get ().Open ("mailto:info-led@sophists.com"); break;
						case	'lweb':		Led_URLManager::Get ().Open (kLedItUserPageURL); break;
					};
					StDialogHandler::ListenToMessage (inMessage, ioParam);
				}
	};
	MyDialogHandler	theAboutBox (this);

	URegistrar::RegisterClass (LDialogBox::class_ID, oldCreatorFunct);

	while (true) {
		MessageT	hitMessage = theAboutBox.DoDialog ();
		if (hitMessage != 0) {
			break;
		}
	}
#endif
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

