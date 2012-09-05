/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Sources/LedItDocument.cpp,v 2.27 2000/10/06 13:09:59 lewis Exp $
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
 *	$Log: LedItDocument.cpp,v $
 *	Revision 2.27  2000/10/06 13:09:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.26  2000/10/05 17:28:09  lewis
 *	react to API change for SetHasScrollBar- now takes ENUM rather than bool
 *	
 *	Revision 2.25  2000/04/03 19:09:27  lewis
 *	fHidableTextDatabase support
 *	
 *	Revision 2.24  1999/12/21 21:05:26  lewis
 *	Split StyledTextIO into separate files
 *	
 *	Revision 2.23  1998/10/30 15:04:43  lewis
 *	Use vector - not Led_Array
 *	
 *	Revision 2.22  1998/04/08  02:35:56  lewis
 *	WordProcessor::WordProcessorTextIOSinkStream etc instead of StyledTextIOSinkStream_StandardStyledTextImager.
 *
 *	Revision 2.21  1997/12/24  04:42:23  lewis
 *	*** empty log message ***
 *
 *	Revision 2.20  1997/12/24  03:57:01  lewis
 *	new mwerks compiler, and fix default file format
 *
 *	Revision 2.19  1997/09/29  17:59:52  lewis
 *	Add SetParagraphDatabase and replace old SpecifyStyleDatabase with new SetStyleDatabase API.
 *	Lose qLedFIrstIndex.
 *
 *	Revision 2.18  1997/07/27  16:02:25  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.17  1997/07/14  01:24:27  lewis
 *	Renamed TextStore_ to TextStore, and Led_PPView_ to Led_PPView.
 *
 *	Revision 2.16  1997/06/18  03:32:10  lewis
 *	Changes for PowerPlant 1.7 (CWPro1)
 *
 *	Revision 2.15  1997/01/10  03:45:52  lewis
 *	new scrolbar interface support.
 *
 *	Revision 2.14  1996/12/13  18:11:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.13  1996/12/05  21:25:04  lewis
 *	*** empty log message ***
 *
 *	Revision 2.12  1996/10/31  01:08:34  lewis
 *	Add fHTMLInfo to keep track of stuff read from HTML file but hidden so can be written back out.
 *	A few name changes/cleanups
 *	Fix save as over already open file bug.
 *
 *	Revision 2.11  1996/10/15  19:30:07  lewis
 *	Fixed code for saveas/saveacopyas to set right type.
 *
 *	Revision 2.10  1996/09/30  15:13:01  lewis
 *	Window menu.
 *	Cleanedup doc type support (not perfect, but better)
 *	Added support for html files.
 *	Cleaned up excpetion hanlding and unknown file type reading code.
 *	more...
 *
 *	Revision 2.9  1996/09/03  15:15:58  lewis
 *	Added RTF support.
 *
 *	Revision 2.8  1996/09/01  15:45:40  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/07/03  01:44:45  lewis
 *	Use Led_BusyCursor isntead of StCursor, cuz later doesn't work well (lose
 *	track of changes made elsewhere via SetCursor, etc).
 *	And do on read of files.
 *
 *	Revision 2.6  1996/06/01  02:56:49  lewis
 *	min doc size.
 *	Better default and zoom doc sizes.
 *
 *	Revision 2.5  1996/05/05  14:57:03  lewis
 *	Added SaveCopyAs command (and reworked things to cshare code).
 *
 *	Revision 2.4  1996/04/18  16:06:14  lewis
 *	Added LedItDocument::PurgeUnneededMemory ().
 *	Dont set commandhandler for new view created just for printing...
 *
 *	Revision 2.3  1996/03/16  19:20:51  lewis
 *	Use Led_DoNewHandle().
 *	Fixed trouble with opening locked files (mIsSpecified).
 *
 *	Revision 2.2  1996/03/05  18:59:26  lewis
 *	Fixed crasher bug reading corrupt file under low memory circumstances. Ran
 *	out of mem reading as Native format doc left reader_src / textstore etc
 *	in bad (half read) state. Reset them now before attemtpting to read as text.
 *	Now support reading stationarty files and READ-ONLY files - very similarly.
 *	Close the file when done reading, and set mSpecified to false so we are
 *	forced to pick a new file on save. Only diff is for stationary we call
 *	it untitled, and for READONLY we use the same name (for window name).
 *
 *	Revision 2.1  1996/03/04  08:23:01  lewis
 *	Cleanup read code to fix bug where we ignored STYL resouce for reading text when
 *	we didn't know the type up-front (ie in case file we type unknown).
 *
 *	Revision 2.0  1996/02/26  23:29:57  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */
#include	<Finder.h>

#include	<LFile.h>
#include	<LMenu.h>
#include	<LMenuBar.h>
#include	<LPrintout.h>
#include	<LPlaceHolder.h>
#include	<LString.h>
#include	<LWindow.h>
#include	<UMemoryMgr.h>
#include	<UWindows.h>

#include	"StyledTextIO.h"
#include	"StyledTextIO_PlainText.h"
#include	"StyledTextIO_LedNative.h"
#include	"StyledTextIO_STYLText.h"

#include	"FilteredFilePicker.h"
#include	"LedItResources.h"
#include	"LedItView.h"

#include	"LedItDocument.h"





class	LedItDocumentWindow : public LWindow {
	public:
		LedItDocumentWindow (ResIDT inWINDid, Uint32 inAttributes, LCommander* inSuper):
			LWindow (inWINDid, inAttributes, inSuper)
			{
				sWindowList.push_back (this);
				::AppendMenu (::GetMenuHandle (kWindowsMenuID), "\pREPLACEME");
				LMenu*	windowMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (kWindowsMenuID);
				Led_AssertNotNil (windowMenu);
				size_t	nMenuItems	=	::CountMItems (windowMenu->GetMacMenuH ());
				for (size_t i = 1; i <= nMenuItems; i++) {
					windowMenu->SetCommand (i, i-1+kBaseWindowCmdID);	// make first cmd = kBaseWindowCmdID
				}
			}

		~LedItDocumentWindow ()
			{
				LWindow*	w	=	this;
				sWindowList.erase (std::find (sWindowList.begin (), sWindowList.end (), w));
				LMenu*	windowMenu	=	LMenuBar::GetCurrentMenuBar()->FetchMenu (kWindowsMenuID);
				Led_AssertNotNil (windowMenu);
				windowMenu->RemoveItem (1);
				size_t	nMenuItems	=	::CountMItems (windowMenu->GetMacMenuH ());
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
vector<LWindow*>	LedItDocumentWindow::sWindowList;









static	FilteredSFPutDLog::TypeSpec	sPutFileTypeList[]	=	{
	{	"HTML file",						kTEXTFileType				},
	{	"Led Rich Text Format",				kLedPrivateDocumentFileType	},
	{	"Microsoft Rich Text Format (RTF)",	kTEXTFileType				},
	{	"Text file",						kTEXTFileType				},
};

inline	Led_FileFormat	MapPutFileTypeListIdxToFormat (size_t typeIndex)
	{
		switch (typeIndex) {
			case	0:	return (eHTMLFormat);
			case	1:	return (eLedPrivateFormat);
			case	2:	return (eRTFFormat);
			case	3:	return (eTextFormat);
			default:	Led_Assert (false);	return (eTextFormat);
		}
	}

inline	size_t	MapPutFileFormatToTypeListIdx (Led_FileFormat format)
	{
		switch (format) {
			case	eHTMLFormat:			return (0);
			case	eLedPrivateFormat:		return (1);
			case	eRTFFormat:				return (2);
			case	eTextFormat:			return (3);
			default:	Led_Assert (false);	return (0);
		}
	}

inline	OSType	MapFormatToOSType (Led_FileFormat fileFormat)
	{
		switch (fileFormat) {
			case	eTextFormat:		return (kTEXTFileType);
			case	eLedPrivateFormat:	return (kLedPrivateDocumentFileType);
			case	eRTFFormat:			return (kTEXTFileType);
			case	eHTMLFormat:		return (kTEXTFileType);
			case	eUnknownFormat:		return (kLedPrivateDocumentFileType);
			default:					return (kLedPrivateDocumentFileType);
		}
	}



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
 ************************************ LedItDocument *****************************
 ********************************************************************************
 */


//	Construct a TextDoc associated with the specified file
//	If inFileSpec is nil, then create an empty, untitled document
LedItDocument::LedItDocument (LCommander* inSuper, Led_FileFormat format):
	LSingleDoc (inSuper),
	fTextStore (),
	fStyleDatabase (),
	fParagraphDatabase (),
	fHidableTextDatabase (),
	fCommandHandler (kMaxNumUndoLevels),
	fFileFormat (format),
	fHTMLInfo (),
	fTextView (NULL)
{
	fTextStore.AddMarkerOwner (this);
	fStyleDatabase = new StandardStyledTextImager::StyleDatabaseRep (fTextStore);
	fParagraphDatabase = new WordProcessor::ParagraphDatabaseRep (fTextStore);
	fHidableTextDatabase = new UniformHidableTextMarkerOwner (fTextStore, &fCommandHandler);
}

LedItDocument::~LedItDocument ()
{
	if (mWindow != NULL) {
		mWindow->PostAction (NULL);		//	Flush undo buffer
	}

	delete mWindow; mWindow = NULL;		// delete now (rather than letting base class) so
										// all links to our textstore/style dbase etc go away before our
										// DTOR is done...

	fTextStore.RemoveMarkerOwner (this);
}

const vector<LWindow*>&	LedItDocument::GetDocumentWindows ()
{
	return LedItDocumentWindow::sWindowList;
}

//	Respond to commands
Boolean	LedItDocument::ObeyCommand (CommandT inCommand, void* ioParam)
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
void	LedItDocument::FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
											 Char16& outMark, Str255 outName
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

void	LedItDocument::OnSaveACopyAsCommand ()
{
	StandardFileReply	reply;
	FilteredSFPutDLog	filteredPicker (sPutFileTypeList, (sizeof sPutFileTypeList)/(sizeof sPutFileTypeList[0]));

	Str255	defaultName;
	GetDescriptor (defaultName);
	size_t 	typeIndex	=	MapPutFileFormatToTypeListIdx (fFileFormat);
	if (filteredPicker.PickFile (defaultName, &reply, &typeIndex)) {
		if (reply.sfReplacing) {	// Delete existing file
			ThrowIfOSErr_ (::FSpDelete(&reply.sfFile));
		}

		Led_FileFormat	savedFileFormat	=	fFileFormat;
		LFile*			savedFile		=	mFile;
	
		fFileFormat	=	MapPutFileTypeListIdxToFormat (typeIndex);
		try {
			mFile = new LFile (reply.sfFile);		// Make a temporary file object

			OSType	fileType = MapFormatToOSType (fFileFormat);			// Find proper file type

			// Make new file on disk
			mFile->CreateNewDataFile (kApplicationSignature, fileType, 0);
			mFile->OpenDataFork (fsRdWrPerm);
			DoSaveHelper ();
		}
		catch (...) {
			delete mFile;
			mFile = savedFile;
			fFileFormat = savedFileFormat;
			throw;
		}
		fFileFormat = savedFileFormat;
		delete mFile;
		mFile = savedFile;
	}
}

void	LedItDocument::DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ()
{
	mIsModified = true;
	SetUpdateCommandStatus (true);
}

TextStore*	LedItDocument::PeekAtTextStore () const
{
	return & const_cast<LedItDocument*>(this)->fTextStore;
}

void	LedItDocument::DoSaveHelper ()
{
	Led_BusyCursor	busyCursor;

	//	write text
	ThrowIfNULL_ (mFile);
	ThrowIfNULL_ (mWindow);
	
	mWindow->PostAction (NULL);

	WordProcessor::WordProcessorTextIOSrcStream				source (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

	ThrowIfOSErr_ ( ::SetFPos (mFile->GetDataForkRefNum (), fsFromStart, 0));
	StyledTextIOWriterSinkStream_FileDescriptor				sink (mFile->GetDataForkRefNum ());

	// Setup output buffer, but be sure there is plenty of RAM before doing so
	{
		char*	cheeseBuf	=	NULL;
		try {
			cheeseBuf =	new char [64*1024];
			sink.SetBufferSize (64*1024);
			delete[] cheeseBuf;
		}
		catch (...) {
			delete[] cheeseBuf;
		}
	}

	// Now actually write the file
	switch (fFileFormat) {
		case	eTextFormat: {
			StyledTextIOWriter_PlainText	textWriter (&source, &sink);
			textWriter.Write ();

			{
				short	curResFile	=	::CurResFile ();
				try {
					mFile->CreateNewFile (kApplicationSignature, kTEXTFileType, 0);
				}
				catch (...) {
				}
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
		
						vector<StandardStyledTextImager::InfoSummaryRecord>
													ledStyleRuns	=	fStyleDatabase->GetStyleInfo (0, fTextStore.GetLength ());
						size_t						nStyleRuns		=	ledStyleRuns.size ();
						Led_Assert (offsetof (StScrpRec, scrpStyleTab) == sizeof (short));	// thats why we add sizeof (short)
						macStyleHandle	=	(StScrpHandle)::Led_DoNewHandle (sizeof (short) + nStyleRuns*sizeof (ScrpSTElement));
						HLock (Handle (macStyleHandle));
						(*macStyleHandle)->scrpNStyles = nStyleRuns;
						StandardStyledTextImager::Convert (ledStyleRuns, (*macStyleHandle)->scrpStyleTab);
						HUnlock (Handle (macStyleHandle));

						::AddResource (Handle (macStyleHandle), 'styl', 128, "\p");
						ThrowIfResError_();
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
		break;

		case	eRTFFormat: {
			StyledTextIOWriter_RTF	textWriter (&source, &sink);
			textWriter.Write ();
		}
		break;

		case	eHTMLFormat: {
			StyledTextIOWriter_HTML	textWriter (&source, &sink, &fHTMLInfo);
			textWriter.Write ();
		}
		break;

		case	eLedPrivateFormat: {
			StyledTextIOWriter_LedNativeFileFormat	textWriter (&source, &sink);
			textWriter.Write ();
		}
		break;
		
		default: {
			Led_Assert (false);
		}
		break;
	}

	sink.UpdateEOF ();
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
void	LedItDocument::NameNewDoc ()
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

void	LedItDocument::OpenFile (const FSSpec& inFileSpec)
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
Boolean	LedItDocument::IsModified ()
{
	return mIsModified;
}

//	Save Document in the specified file with the specified file type
//
//	If file type is fileType_Default, use the normal file type for
//	this document
void	LedItDocument::DoAESave (FSSpec& inFileSpec, OSType inFileType)
{
	delete mFile;
	mFile = NULL;
	
	mFile = new LFile (inFileSpec);		// Make new file object

	OSType	fileType = MapFormatToOSType (fFileFormat);			// Find proper file type
	if (inFileType != fileType_Default) {
		fileType = inFileType;
	}

	// Make new file on disk
	mFile->CreateNewDataFile (kApplicationSignature, fileType, 0);
	mFile->OpenDataFork (fsRdWrPerm);
	DoSave ();							// Write out data
										// Change window name
	mWindow->SetDescriptor(inFileSpec.name);
	mIsSpecified = true;				// Document now has a specified file
}

//	Ask the user to save a Document and give it a name
//
//	Returns false if the user cancels the operation
Boolean	LedItDocument::AskSaveAs (FSSpec& outFSSpec, Boolean inRecordIt)
{
	StandardFileReply	reply;
	FilteredSFPutDLog	filteredPicker (sPutFileTypeList, (sizeof sPutFileTypeList)/(sizeof sPutFileTypeList[0]));

	Str255	defaultName;
	GetDescriptor (defaultName);
	size_t 	typeIndex	=	MapPutFileFormatToTypeListIdx (fFileFormat);
	if (filteredPicker.PickFile (defaultName, &reply, &typeIndex)) {
		fFileFormat = MapPutFileTypeListIdxToFormat (typeIndex);
		
		/*
		 *	Close the file (in case were overwriting our original, so the delete below doesn't fail.
		 *	Any error writing NEW file out, mark us as UNTITLED, and NOT associated with any file...
		 */
		delete mFile;
		mFile = NULL;		
		mIsSpecified = false;
		try {
			if (inRecordIt) {
				SendAESaveAs (reply.sfFile, fileType_Default, false);
			}
			if (reply.sfReplacing) {	// Delete existing file
				ThrowIfOSErr_ (::FSpDelete (&reply.sfFile));
			}
			DoAESave (reply.sfFile, fileType_Default);
			outFSSpec = reply.sfFile;
		}
		catch (...) {
			delete mFile;
			mFile = NULL;		
			mIsSpecified = false;
			NameNewDoc ();
			throw;
		}
	}
	return reply.sfGood;
}

//	Save the entire Document to its associated File (which must already exist)
void	LedItDocument::DoSave ()
{
	DoSaveHelper ();

	FSSpec		fileSpec;
	mFile->GetSpecifier (fileSpec);
	mWindow->SetDescriptor (fileSpec.name);

	mIsModified = false;
}

//	Revert the Document to the last saved version on disk
void	LedItDocument::DoRevert ()
{
	fTextStore.Replace (fTextStore.GetStart (), fTextStore.GetEnd (), "", 0);
	fCommandHandler.Commit ();
	DoReadCode ();
	fCommandHandler.Commit ();
	mIsModified = false;
}

//	Print the contents of the Document
void	LedItDocument::DoPrint ()
{
	LPrintout*		thePrintout = LPrintout::CreatePrintout(prto_TextDoc);
	thePrintout->SetPrintRecord(mPrintRecordH);
	LPlaceHolder*	textPlace = (LPlaceHolder*)thePrintout->FindPaneByID('TBox');

	LedItView	editorView;
	editorView.AddAttributes (textAttr_MultiStyle|textAttr_Editable|textAttr_Selectable|textAttr_WordWrap);
	editorView.SpecifyTextStore (&fTextStore);
	editorView.SetStyleDatabase (fStyleDatabase);
	editorView.SetParagraphDatabase (fParagraphDatabase);
	editorView.SetHidableTextDatabase (fHidableTextDatabase);
	editorView.SetScrollBarType (Led_PPView::v, Led_PPView::eScrollBarNever);
	editorView.SetScrollBarType (Led_PPView::h, Led_PPView::eScrollBarNever);
	textPlace->InstallOccupant (&editorView, atNone);
	editorView.FinishCreate ();
	editorView.SetForceAllRowsShowing (false);
	
	thePrintout->DoPrintJob();
	delete thePrintout;
}

void	LedItDocument::PurgeUnneededMemory ()
{
	// If we are running out of memory, best to commit all our existing commands, to free some up.
	fCommandHandler.Commit ();

	// Also, we can punt cached linebreak etc information
	Led_AssertNotNil (fTextView);
	fTextView->PurgeUnneededMemory ();
}

void	LedItDocument::BuildDocWindow (const FSSpec* inFileSpec)
{
	Led_Assert (mWindow == NULL);

	// GetNewCWindow() seems to crash when we are nearly out of RAM in our local heap (rather
	// than just returning NULL. So try to avoid that situation.
	Led_CheckSomeLocalHeapRAMAvailable ();

	mWindow = new LedItDocumentWindow (WIND_TextDoc, windAttr_Regular | windAttr_CloseBox | windAttr_TitleBar | windAttr_Resizable | windAttr_SizeBox | windAttr_Zoomable | windAttr_Enabled | windAttr_Targetable, this);
	mWindow->FinishCreate ();

	// By default window created with a silly size. Don't know how todo this well within
	// PowerPlant. Seems you have to specify size in WIND resource. But thats not what I want!
	// Well, we'll just take matters into our own hands then...
	{
		// Find GDevice containing largest portion of Window
		WindowPtr	theWindPtr			=	mWindow->GetMacPort ();
		Rect		windowStructureRect	=	UWindows::GetWindowStructureRect (theWindPtr);
		GDHandle	dominantDevice = UWindows::FindDominantDevice (windowStructureRect);
		Rect		screenRect = (**dominantDevice).gdRect;
		const	kSluffBetweenWindBottomAndScreenBottom	=	10;
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

	LedItView*	editorView	=	new LedItView ();

	editorView->AddAttributes (textAttr_MultiStyle|textAttr_Editable|textAttr_Selectable|textAttr_WordWrap);
	editorView->SpecifyTextStore (&fTextStore);
	editorView->SetStyleDatabase (fStyleDatabase);
	editorView->SetParagraphDatabase (fParagraphDatabase);
	editorView->SetHidableTextDatabase (fHidableTextDatabase);
	editorView->SetCommandHandler (&fCommandHandler);
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
//		fFileFormat = eLedPrivateFormat;	// for new documents, default to native format
		NameNewDoc ();						// Set name of untitled window
	}
	else {
		OpenFile (*inFileSpec);		// Display contents of file in window
	}

	mWindow->Show();
}

void	LedItDocument::DoReadCode ()
{
	Led_BusyCursor	busyCursor;

	StScrpHandle	styleInfo	=	NULL;
	try {
		short	curResFile	=	::CurResFile ();
		mFile->OpenResourceFork (fsRdPerm);
		::UseResFile (mFile->GetResourceForkRefNum ());
		styleInfo = (StScrpHandle)::Get1Resource ('styl', 128);
		if (styleInfo != NULL) {
			::DetachResource ((Handle)styleInfo);
		}
		::UseResFile (curResFile);
		mFile->CloseResourceFork ();
	}
	catch (...) {
	}
	try {
		ThrowIfOSErr_ ( ::SetFPos (mFile->GetDataForkRefNum (), fsFromStart, 0));
		StyledTextIOSrcStream_FileDescriptor			source (mFile->GetDataForkRefNum (), Handle (styleInfo));
		WordProcessor::WordProcessorTextIOSinkStream	sink (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

ReRead:
		switch (fFileFormat) {
			case	eTextFormat: {
				source.SetBufferSize (0);	// no need for buffering with these readers
				if (styleInfo == NULL) {
					StyledTextIOReader_PlainText	textReader (&source, &sink);
					textReader.Read ();
				}
				else {
					StyledTextIOReader_STYLText	textReader (&source, &sink);
					textReader.Read ();
				}
			}
			break;
	
			case	eRTFFormat: {
				source.SetBufferSize (16*1024);		// reads a byte at a time - buffering helps ALOT
				StyledTextIOReader_RTF	textReader (&source, &sink);
				textReader.Read ();
			}
			break;

			case	eHTMLFormat: {
				source.SetBufferSize (16*1024);		// reads a byte at a time - buffering helps ALOT
				StyledTextIOReader_HTML	textReader (&source, &sink, &fHTMLInfo);
				textReader.Read ();
			}
			break;

			case	eLedPrivateFormat: {
// unsure if buffering would help here or not - try both ways - LGP 960902
				StyledTextIOReader_LedNativeFileFormat	textReader (&source, &sink);
				textReader.Read ();
			}
			break;
			
			default: {
				/*
				 *	Should enhance this unknown/format reading code to take into
				 *	account file suffix in our guess. Also should take into account
				 *	FINDER File-Type info.
				 */

				// for unknown file format, 
				//		first try RTF
				//		then Led private format
				//		first try HTML
				//		then try to read as plain text
				source.SetBufferSize (1*1024);		// most/all of these quicklook routines only examine the first 1K or so
				
				// Try RTF	
				try {
					StyledTextIOReader_RTF	reader (&source, &sink);
					if (reader.QuickLookAppearsToBeRightFormat ()) {
						fFileFormat = eRTFFormat;
						goto ReRead;
					}
				}
				catch (...) {
					// ignore any errors, and proceed to next file type
				}

				// Try LedNativeFileFormat	
				try {
					StyledTextIOReader_LedNativeFileFormat	reader (&source, &sink);
					if (reader.QuickLookAppearsToBeRightFormat ()) {
						fFileFormat = eLedPrivateFormat;
						goto ReRead;
					}
				}
				catch (...) {
					// ignore any errors, and proceed to next file type
				}

				// Try HTML	
				try {
					StyledTextIOReader_HTML	reader (&source, &sink);
					if (reader.QuickLookAppearsToBeRightFormat ()) {
						fFileFormat = eHTMLFormat;
						goto ReRead;
					}
				}
				catch (...) {
					// ignore any errors, and proceed to next file type
				}

				// Nothing left todo but to read the text file as plain text, as best we can...
				fFileFormat = eTextFormat;
				goto ReRead;
			}
			break;
		}
	}
	catch (...) {
		if (styleInfo != NULL) {
			::DisposeHandle((Handle) styleInfo);
		}
		throw;
	}
	if (styleInfo != NULL) {
		::DisposeHandle((Handle) styleInfo);
	}
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

