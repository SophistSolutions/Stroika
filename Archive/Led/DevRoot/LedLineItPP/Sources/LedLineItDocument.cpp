/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Sources/LedLineItDocument.cpp,v 2.22 2003/12/13 02:00:06 lewis Exp $
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
 *	$Log: LedLineItDocument.cpp,v $
 *	Revision 2.22  2003/12/13 02:00:06  lewis
 *	small changes so uses new spellcheck code (and stores engine in applicaiton object)
 *	
 *	Revision 2.21  2003/06/04 03:08:27  lewis
 *	SPR#1513: spellcheck engine support
 *	
 *	Revision 2.20  2003/03/27 16:38:31  lewis
 *	SPR#1384: LedLineItDocument::DidUpdateText () checks updateInfo.fRealContentUpdate and only set dirtyflag if real update
 *	
 *	Revision 2.19  2003/03/11 19:37:49  lewis
 *	SPR#1249 - get compiling with UNICODE/Carbon
 *	
 *	Revision 2.18  2002/05/06 21:31:29  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.17  2001/11/27 00:28:29  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.16  2001/09/11 22:30:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  2001/08/30 01:01:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.14  2001/08/27 21:30:37  lewis
 *	SPR#0981- SetPrintSpec (mPrintSpec) instead of SetPrintRecord (mPrintRecord)
 *	
 *	Revision 2.13  2001/07/19 21:06:20  lewis
 *	SPR#0960- CW6Pro support
 *	
 *	Revision 2.12  2000/10/06 13:10:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.11  1998/10/30 20:24:55  lewis
 *	Use vector<> instead of Led_Array<>
 *	
 *	Revision 2.10  1997/12/24  04:52:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1997/12/24  04:01:20  lewis
 *	compat with CWPRo2 compiler
 *
 *	Revision 2.8  1997/09/29  18:07:46  lewis
 *	Lose qLedFirstIndex.
 *
 *	Revision 2.7  1997/07/27  16:03:37  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.6  1997/07/15  05:30:07  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1997/07/14  14:45:32  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.4  1997/06/18  03:36:28  lewis
 *	Support PowerPlant1.7 (CWPro1).
 *
 *	Revision 2.3  1997/03/23  00:59:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1997/03/04  20:27:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/01/20  05:46:48  lewis
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
#include	<Finder.h>
#include	<StandardFile.h>

#include	<PP_Resources.h>
#include	<LFile.h>
#include	<LMenu.h>
#include	<LMenuBar.h>
#include	<LPrintout.h>
#include	<LPlaceHolder.h>
#include	<LString.h>
#include	<LWindow.h>
#include	<UDesktop.h>
#include	<UMemoryMgr.h>
#include	<UStandardDialogs.h>
#include	<UWindows.h>

#include	"CodePage.h"

#include	"LedLineItResources.h"
#include	"LedLineItView.h"

#include	"LedLineItDocument.h"

#if		qIncludeBasicSpellcheckEngine
#include	"LedLineItApplication.h"
#endif




class	LedLineItDocumentWindow : public LWindow {
	public:
		LedLineItDocumentWindow (ResIDT inWINDid, UInt32 inAttributes, LCommander* inSuper):
			LWindow (inWINDid, inAttributes, inSuper)
			{
				sWindowList.push_back (this);
				::AppendMenu (::GetMenuHandle (kWindowsMenuID), "\pREPLACEME");
				LMenu*	windowMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (kWindowsMenuID);
				Led_AssertNotNil (windowMenu);
				size_t	nMenuItems	=	::CountMenuItems (windowMenu->GetMacMenuH ());
				for (size_t i = 1; i <= nMenuItems; i++) {
					windowMenu->SetCommand (i, i-1+kBaseWindowCmdID);	// make first cmd = kBaseWindowCmdID
				}
			}

		~LedLineItDocumentWindow ()
			{
				LWindow*	w	=	this;
				sWindowList.erase (std::find (sWindowList.begin (), sWindowList.end (), w));
				LMenu*	windowMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (kWindowsMenuID);
				Led_AssertNotNil (windowMenu);
				windowMenu->RemoveItem (1);
				size_t	nMenuItems	=	::CountMenuItems (windowMenu->GetMacMenuH ());
				for (size_t i = 1; i <= nMenuItems; i++) {
					windowMenu->SetCommand (i, i-1+kBaseWindowCmdID);	// make first cmd = kBaseWindowCmdID
				}
			}

		// make zoom produce a roughly page-size window
		override	void	CalcStandardBoundsForScreen (const Rect	&inScreenBounds, Rect& outStdBounds) const
			{
				LWindow::CalcStandardBoundsForScreen (inScreenBounds, outStdBounds);
				short	winWidth		=	::GetRectWidth (outStdBounds);
				short	desiredWidth	=	8.5*72;	// 8.5 inches by 72dpi
				short	newWidth 		=	Led_Min (winWidth, desiredWidth);
				outStdBounds.right = outStdBounds.left + newWidth;
			}

	public:
		static	vector<LWindow*>	sWindowList;
};
vector<LWindow*>	LedLineItDocumentWindow::sWindowList;




/*
 *	I tried using the PowerPlant StCursor class, but it worked LOUSY. It kept getting
 *	its internal cache of what cursor was shown stuck, and stoped going busy!
 *
 *	This is extremely primitive, and lacks many features I'd like (like not showing up
 *	unless needed - like if enuf time has past and looks like its gonna take longer).
 *
 *	But it will do for now.... LGP 960702
 *
 *	Maybe consider lifting the Stroika cursor support sometime in the future? LGP 960702
 */
class	Led_BusyCursor {
	public:
		Led_BusyCursor ()
			{
				::SetCursor (*::GetCursor (watchCursor));
			}
		~Led_BusyCursor ()
			{
				::InitCursor ();
			}
};





/*
 ********************************************************************************
 ******************************** LedLineItDocument *****************************
 ********************************************************************************
 */

//	Construct a TextDoc associated with the specified file
//	If inFileSpec is nil, then create an empty, untitled document
LedLineItDocument::LedLineItDocument (LCommander* inSuper):
	LSingleDoc(inSuper),
	fTextStore (),
	fCommandHandler (kMaxNumUndoLevels),
	fTextView (NULL)
{
	fTextStore.AddMarkerOwner (this);
}

LedLineItDocument::~LedLineItDocument ()
{
	if (mWindow != NULL) {
		mWindow->PostAction (NULL);		//	Flush undo buffer
	}

	delete mWindow; mWindow = NULL;		// delete now (rather than letting base class) so
										// all links to our textstore/style dbase etc go away before our
										// DTOR is done...

	fTextStore.RemoveMarkerOwner (this);
}

const vector<LWindow*>&	LedLineItDocument::GetDocumentWindows ()
{
	return LedLineItDocumentWindow::sWindowList;
}

//	Respond to commands
Boolean	LedLineItDocument::ObeyCommand (CommandT inCommand, void* ioParam)
{
	Boolean	cmdHandled = true;

	switch (inCommand) {
		case	cmdSaveACopyAs: {
			OnSaveACopyAsCommand ();
		}
		break;

		default:
			cmdHandled = LSingleDoc::ObeyCommand (inCommand, ioParam);
			break;
	}
	
	return cmdHandled;
}

//	Pass back status of a (menu) command
void	LedLineItDocument::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
											 UInt16& outMark, Str255 outName
											)
{
	outUsesMark = false;
	switch (inCommand) {
		case	cmdSaveACopyAs: {
			outEnabled = true;
		}
		break;

		default:
			LSingleDoc::FindCommandStatus (inCommand, outEnabled, outUsesMark, outMark, outName);
		break;
	}
}

void	LedLineItDocument::OnSaveACopyAsCommand ()
{
	Str255	defaultName;
	GetDescriptor (defaultName);

	Str255	saveAsPrompt;
	::GetIndString(saveAsPrompt, STRx_Standards, str_SaveAs);

	bool	sfGood		=	false;
	bool	sfReplacing	=	false;
	FSSpec	sfFSSpec;
#if		TARGET_CARBON
	if (PP_StandardDialogs::AskSaveFile (defaultName, 'TEXT', sfFSSpec, sfReplacing)) {
		sfGood = true;
	}
#else
	StandardFileReply	reply;
	UDesktop::Deactivate();
	::StandardPutFile (saveAsPrompt, defaultName, &reply);
	UDesktop::Activate();
	sfGood = reply.sfGood;
	sfReplacing = reply.sfReplacing;
	sfFSSpec = reply.sfFile;
#endif

	if (sfGood) {
		if (sfReplacing) {	// Delete existing file
			ThrowIfOSErr_ (::FSpDelete(&sfFSSpec));
		}

		LFile*	savedFile		=	mFile;
		try {
			mFile = new LFile (sfFSSpec);		// Make a temporary file object
			// Make new file on disk
			mFile->CreateNewDataFile (kApplicationSignature, 'TEXT', 0);
			mFile->OpenDataFork (fsRdWrPerm);
			DoSaveHelper ();
		}
		catch (...) {
			delete mFile;
			mFile = savedFile;
			throw;
		}
		delete mFile;
		mFile = savedFile;
	}
}

void	LedLineItDocument::DidUpdateText (const UpdateInfo& updateInfo)
{
	if (updateInfo.fRealContentUpdate) {
		mIsModified = true;
	}
	SetUpdateCommandStatus (true);
}

TextStore*	LedLineItDocument::PeekAtTextStore () const
{
	return & const_cast<LedLineItDocument*>(this)->fTextStore;
}

void	LedLineItDocument::DoSaveHelper ()
{
	Led_BusyCursor	busyCursor;

	//	write text
	ThrowIfNULL_ (mFile);
	ThrowIfNULL_ (mWindow);
	
	mWindow->PostAction (NULL);


	ThrowIfOSErr_ ( ::SetFPos (mFile->GetDataForkRefNum (), fsFromStart, 0));

	const 	size_t	kBufSize	=	8*1024;
	Led_tChar	buf[kBufSize];
	size_t	offset		=	0;
	size_t	eob			=	fTextStore.GetLength ();
	while (offset < eob) {
		size_t	bytesToWrite	=	Led_Min (kBufSize, eob-offset);
		fTextStore.CopyOut (offset, bytesToWrite, buf);
		offset += bytesToWrite;
		#if		qMacOS
			Led_tChar	buf2[sizeof (buf)];
		#elif	qWindows
			Led_tChar	buf2[2*sizeof (buf)];
		#endif
		bytesToWrite = Led_NLToNative (buf, bytesToWrite, buf2, sizeof (buf2));
		long	count	=	bytesToWrite;
		Led_ThrowOSErr (::FSWrite (mFile->GetDataForkRefNum (), &count, (char*)buf2));
	}
	Led_ThrowOSErr (::SetEOF (mFile->GetDataForkRefNum (), eob));


	// If there WAS a STYL resource, we've invalidated it. So remove it.
	{
		short	curResFile	=	::CurResFile ();
		try {
			mFile->OpenResourceFork (fsRdWrPerm);
		}
		catch (...) {
		}
		if (mFile->GetResourceForkRefNum () != -1) {
			::UseResFile (mFile->GetResourceForkRefNum ());
			try {
				StScrpHandle	macStyleHandle		=	 (StScrpHandle)::Get1Resource ('styl', 128);
				if (macStyleHandle != NULL) {
					::RemoveResource (Handle (macStyleHandle));	// remove old styl resource
				}
			}
			catch (...) {
				::UseResFile (curResFile);
				mFile->CloseResourceFork ();
				throw;
			}
			::UseResFile (curResFile);
			mFile->CloseResourceFork ();
		}
	}
}

//	Name a new, untitled document window
//
//	Untitled windows start with "untitled", then "untitled 1",
//	"untitled 2", etc. Old numbers are reused, so there won't be
//	gaps in the numbering.
//
//	This routine uses a STR# resource to store the "untitled" string,
//	which can be localized to different languages. The first string
//	is "untitled" and the second is "untitled " (trailing space),
//	which is used when appending a number to the name.
void	LedLineItDocument::NameNewDoc ()
{
	// Start with the default name ("untitled")
	Str255	name;
	::GetIndString(name, STRx_Untitled, 1);
	
	long	num = 0;
	while (UWindows::FindNamedWindow(name) != nil) {
			// An existing window has the current name
			// Increment counter and try again
		::GetIndString(name, STRx_Untitled, 2);
		num++;
		Str15	numStr;
		::NumToString(num, numStr);
		LString::AppendPStr(name, numStr);
	}		
	Led_AssertNotNil (mWindow);
	mWindow->SetDescriptor (name);		// Finally, set window title
}

void	LedLineItDocument::OpenFile (const FSSpec& inFileSpec)
{
	// Create a new File object, read the entire File contents,
	// put the contents into the text view, and set the Window
	// title to the name of the File.
	try {
		Led_Require (mFile == NULL);
		mFile = new LFile (inFileSpec);

		FInfo	docFinderInfo;
		ThrowIfError_ (::FSpGetFInfo (&inFileSpec, &docFinderInfo));
		bool	isStationary	=	docFinderInfo.fdFlags & kIsStationery;
		bool	openedReadOnly	=	false;
		try {
			mFile->OpenDataFork (fsRdWrPerm);
		}
		catch (...) {
			openedReadOnly = true;
			mFile->OpenDataFork (fsRdPerm);
		}

		DoReadCode ();

		if (isStationary) {
			NameNewDoc ();
		}
		else {
			mWindow->SetDescriptor (inFileSpec.name);
		}

		if (openedReadOnly or isStationary) {
			delete mFile;
			mFile = NULL;
		}
		mIsSpecified = not (openedReadOnly or isStationary);	// if we opened RO or stationary, then hitting save button should bring up save-dialog
		mIsModified = isStationary;								// if we stationary, then closing should bring up save-dialog
	}
	catch (...) {
		// if an error opening the file, then delete the file object - in case its left open...
		delete mFile;
		mFile = NULL;
		throw;
	}
}

//	Return whether the Document is has changed since the last save
Boolean	LedLineItDocument::IsModified ()
{
	return mIsModified;
}

void	LedLineItDocument::DoAESave (FSSpec& inFileSpec, OSType /*inFileType*/)
{
	delete mFile;
	mFile = NULL;
	
	mFile = new LFile (inFileSpec);		// Make new file object

	// Make new file on disk
	mFile->CreateNewDataFile (kApplicationSignature, 'TEXT', 0);
	mFile->OpenDataFork (fsRdWrPerm);
	DoSave ();							// Write out data
										// Change window name
	mWindow->SetDescriptor (inFileSpec.name);
	mIsSpecified = true;				// Document now has a specified file
}

//	Save the entire Document to its associated File (which must already exist)
void	LedLineItDocument::DoSave ()
{
	DoSaveHelper ();

	FSSpec		fileSpec;
	mFile->GetSpecifier (fileSpec);
	mWindow->SetDescriptor (fileSpec.name);

	mIsModified = false;
}

//	Revert the Document to the last saved version on disk
void	LedLineItDocument::DoRevert ()
{
	fTextStore.Replace (fTextStore.GetStart (), fTextStore.GetEnd (), LED_TCHAR_OF (""), 0);
	fCommandHandler.Commit ();
	DoReadCode ();
	fCommandHandler.Commit ();
	mIsModified = false;
}

//	Print the contents of the Document
void	LedLineItDocument::DoPrint ()
{
	LPrintout*		thePrintout = LPrintout::CreatePrintout(prto_TextDoc);
	thePrintout->SetPrintSpec (mPrintSpec);
	LPlaceHolder*	textPlace = (LPlaceHolder*)thePrintout->FindPaneByID('TBox');

	LedLineItView	editorView;
//	editorView.AddAttributes (textAttr_MultiStyle|textAttr_Editable|textAttr_Selectable|textAttr_WordWrap);
	editorView.SpecifyTextStore (&fTextStore);
	editorView.SetScrollBarType (Led_PPView::v, Led_PPView::eScrollBarNever);
	editorView.SetScrollBarType (Led_PPView::h, Led_PPView::eScrollBarNever);
	textPlace->InstallOccupant (&editorView, atNone);
	editorView.FinishCreate ();
	editorView.SetForceAllRowsShowing (false);
	
	thePrintout->DoPrintJob();
	delete thePrintout;
}

void	LedLineItDocument::PurgeUnneededMemory ()
{
	// If we are running out of memory, best to commit all our existing commands, to free some up.
	fCommandHandler.Commit ();

	// Also, we can punt cached linebreak etc information
	Led_AssertNotNil (fTextView);
	fTextView->PurgeUnneededMemory ();
}

void	LedLineItDocument::BuildDocWindow (const FSSpec* inFileSpec)
{
	Led_Assert (mWindow == NULL);

	// GetNewCWindow() seems to crash when we are nearly out of RAM in our local heap (rather
	// than just returning NULL. So try to avoid that situation.
	Led_CheckSomeLocalHeapRAMAvailable ();

LedLineItDocumentWindow* w = new LedLineItDocumentWindow (WIND_TextDoc, windAttr_Regular | windAttr_CloseBox | windAttr_TitleBar | windAttr_Resizable | windAttr_SizeBox | windAttr_Zoomable | windAttr_Enabled | windAttr_Targetable, this);
mWindow = w;
//	mWindow = new LedLineItDocumentWindow (WIND_TextDoc, windAttr_Regular | windAttr_CloseBox | windAttr_TitleBar | windAttr_Resizable | windAttr_SizeBox | windAttr_Zoomable | windAttr_Enabled | windAttr_Targetable, this);
	mWindow->FinishCreate ();

	// By default window created with a silly size. Don't know how todo this well within
	// PowerPlant. Seems you have to specify size in WIND resource. But thats not what I want!
	// Well, we'll just take matters into our own hands then...
	{
		// Find GDevice containing largest portion of Window
		#if		TARGET_CARBON
			WindowPtr	theWindPtr			=	::GetWindowFromPort (mWindow->GetMacPort ());
		#else
			WindowPtr	theWindPtr			=	mWindow->GetMacPort ();
		#endif
		Rect		windowStructureRect	=	UWindows::GetWindowStructureRect (theWindPtr);
		GDHandle	dominantDevice = UWindows::FindDominantDevice (windowStructureRect);
		Rect		screenRect = (**dominantDevice).gdRect;
		const	Led_Coordinate	kSluffBetweenWindBottomAndScreenBottom	=	10;
		if (windowStructureRect.bottom + kSluffBetweenWindBottomAndScreenBottom < screenRect.bottom) {
			Rect	newWindBounds = UWindows::GetWindowContentRect (theWindPtr);
			// being careful about size of title bar and rest of structure region, we want to incremnt content region size by
			// how much we want to grow structure region.
			newWindBounds.bottom += (screenRect.bottom - windowStructureRect.bottom - kSluffBetweenWindBottomAndScreenBottom);
			mWindow->DoSetBounds (newWindBounds);
		}
	}

	SDimension16	winSize;
	mWindow->GetFrameSize (winSize);

	{
		Rect	minMax;
		minMax.top		=	72;			// minHeight
		minMax.left		=	72;			// minWidth
		minMax.bottom	=	10*1024;	// maxHeight
		minMax.right	=	10*1024;	// maxWidth;
		mWindow->SetMinMaxSize (minMax);
	}

	LedLineItView*	editorView	=	new LedLineItView ();
//	editorView->AddAttributes (textAttr_MultiStyle|textAttr_Editable|textAttr_Selectable|textAttr_WordWrap);
	editorView->AddAttributes (textAttr_Editable|textAttr_Selectable);
	editorView->SpecifyTextStore (&fTextStore);
	editorView->SetCommandHandler (&fCommandHandler);
#if		qIncludeBasicSpellcheckEngine
	editorView->SetSpellCheckEngine (&LedLineItApplication::Get ().fSpellCheckEngine);
#endif
	editorView->PutInside (mWindow);
	editorView->PlaceInSuperFrameAt (0, 0, false);
	editorView->ResizeFrameTo (winSize.width, winSize.height, false);
	SBooleanRect	editViewBindings;
	editViewBindings.top = true;
	editViewBindings.left = true;
	editViewBindings.bottom = true;
	editViewBindings.right = true;
	editorView->SetFrameBinding (editViewBindings);
	editorView->FinishCreate ();

	fTextView = editorView;
	mWindow->SetLatentSub (editorView);	// make editorView get the focus when window shown...

	if (inFileSpec == NULL) {
		NameNewDoc ();						// Set name of untitled window
	}
	else {
		OpenFile (*inFileSpec);		// Display contents of file in window
	}

	mWindow->Show();
}

void	LedLineItDocument::DoReadCode ()
{
	Led_BusyCursor	busyCursor;

	// read all at once. Requires contiguous block of memory, but is significantly faster
	// (for reasons I don't completely understand, but don't care so much about either...)
	// LGP 960515
	// Also note: this makes the dealing with CRLF strattling a buffer-bounary problem go away.
	// If we tried to read in chunks, we'd need to be more careful about the Led_NormalizeTextToNL () code.
	long	logEOF	=	0;
	Led_ThrowOSErr (::GetEOF (mFile->GetDataForkRefNum (), &logEOF));
	ThrowIfOSErr_ ( ::SetFPos (mFile->GetDataForkRefNum (), fsFromStart, 0));
	size_t	len	=	logEOF;
	Led_SmallStackBuffer<char>	buf (len);
	long	count	=	len;
	ThrowIfOSErr_ (::FSRead (mFile->GetDataForkRefNum (), &count, (char*)buf));
	count = Led_Min (count, len);
	#if		qWideCharacters
		CodePage						useCodePage	=	kCodePage_MAC;
		CodePagesGuesser::Confidence	conf	=	CodePagesGuesser::eLow;
		size_t							bytesToStrip	=	0;
		useCodePage =	CodePagesGuesser ().Guess (buf, count, &conf, &bytesToStrip);
		CodePageConverter				cpc			=	CodePageConverter (useCodePage, CodePageConverter::eHandleBOM);
		size_t							outCharCnt	=	cpc.MapToUNICODE_QuickComputeOutBufSize (static_cast<char*> (buf), count + 1);
		Led_SmallStackBuffer<Led_tChar>	result (outCharCnt);
		cpc.MapToUNICODE (static_cast<char*> (buf), count, static_cast<wchar_t*> (result), &outCharCnt);
		count = outCharCnt;
		result[count] = '\0';	// assure NUL-Term
		Led_tChar*	buffp	=	static_cast<Led_tChar*> (result);
	#else
		Led_tChar*	buffp	=	static_cast<char*> (buf);
	#endif
	size_t bytesRead = Led_NormalizeTextToNL (buffp, count, buffp, count);
	fTextStore.Replace (0, 0, buffp, bytesRead);
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

