/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/LedItDocument.cpp,v 2.36 2000/04/03 19:07:19 lewis Exp $
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
 *	Revision 2.36  2000/04/03 19:07:19  lewis
 *	forgot call to reset fHidableTextDatabase on NEWDOC command
 *	
 *	Revision 2.35  2000/03/31 23:49:31  lewis
 *	support WRITING hidabletext
 *	
 *	Revision 2.34  2000/03/31 00:51:37  lewis
 *	SPR#0717 - more changes to adapt to new HiddenText support. Rip out some of the code I had added to LedItView - and instead keep a HiddenTextDatabase with the document (like the paragraphdatabase). Now we support REAADING RTF files with hidden text. Next todo is writing RTF with hidden text
 *	
 *	Revision 2.33  1999/12/19 15:09:27  lewis
 *	broken StyledTextIO into parts- SPR#0662
 *	
 *	Revision 2.32  1999/12/09 17:29:21  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.31  1999/11/15 21:35:26  lewis
 *	react to some small Led changes so that Led itself no longer deends on MFC (though this demo app clearly DOES)
 *	
 *	Revision 2.30  1999/04/28 13:56:14  lewis
 *	Fix OnNewDocument() to properly reset fonts/paragraph info (and cursel font) to defaults
 *	
 *	Revision 2.29  1999/03/08 23:17:00  lewis
 *	Keep track of RTFInfo object
 *	
 *	Revision 2.28  1999/02/09 16:36:05  lewis
 *	when opening zero-lenght file, we WERE not steting fFileFormat, which caused asserts when you tried to save
 *	
 *	Revision 2.27  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 2.26  1998/04/09  01:33:56  lewis
 *	*** empty log message ***
 *
 *	Revision 2.25  1998/04/08  02:17:46  lewis
 *	Use WordProcessor::WordProcessorTextIOSinkStream etc instead of StyledTextIOSinkStream_StandardStyledTextImager.
 *
 *	Revision 2.24  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 2.23  1997/12/24  04:12:38  lewis
 *	Do a better job forcing there to be a file suffix when saving file and user typed no suffix. Not 100% right
 *	but a big improvement on what we had before.
 *	And default to RTF - not Led format.
 *
 *	Revision 2.22  1997/09/29  16:01:04  lewis
 *	Revised APIs for fStyleDatabase, and added fParagraphDatabase.
 *
 *	Revision 2.21  1997/07/27  16:01:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.20  1997/07/14  14:50:48  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.19  1997/06/28  17:28:58  lewis
 *	Fix Doc/View creation to not use kludgy global variable, and use OnInitialUpdate() instead.
 *	In addition to beiung cleaner, it works better with MDI.
 *
 *	Revision 2.18  1997/06/23  16:33:54  lewis
 *	careful in displaying line# to take care of case where in print-preview mode - avoid crash.
 *
 *	Revision 2.17  1997/06/18  03:42:27  lewis
 *	qIncludePrefixFile
 *
 *	Revision 2.16  1997/01/10  03:30:53  lewis
 *	throw specifier
 *
 *	Revision 2.15  1996/12/13  18:03:20  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.14  1996/12/05  21:18:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.13  1996/10/31  00:32:01  lewis
 *	Fix initial format on first save.
 *	Add fHTMLInfo () support so we save some stripped html info across read/write.
 *
 *	Revision 2.12  1996/09/30  15:00:06  lewis
 *	Somewhat cleanedup/fixed? file type/file choser dialog handling.
 *	Support for additional types HTML/RTF.
 *	Revised/cleaned up hanlding of reading unknown file types.
 *	More with file reading?
 *
 *	Revision 2.11  1996/09/03  15:12:12  lewis
 *	Lose distintion between PC/Mac text. And add support for RTF.
 *
 *	Revision 2.10  1996/09/01  15:43:56  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.9  1996/07/03  01:58:02  lewis
 *	LedItDocument::OnUpdateFileSave () so save disabled when doc not dirty.
 *
 *	Revision 2.8  1996/06/01  02:45:13  lewis
 *	Massive changes. Mostly to CTemplate/pick file dialog stuff.
 *	But also to file IO (guess file type) code.
 *
 *	Revision 2.7  1996/05/05  14:45:03  lewis
 *	Cleanups, and added OnFileSaveCopyAs ().
 *
 *	Revision 2.6  1996/04/18  15:53:20  lewis
 *	Support new flavor/embedding stuff with use of LedItControlItem::DocContextDefiner ()
 *	and cleaned up file IO to use StyledTextIOReader_LedNativeFileFormat - no longer need special
 *	//NativeFormatReaderWithOLE2Emebddings.
 *
 *	Revision 2.5  1996/03/16  19:12:11  lewis
 *	Code cleanups.
 *	On AssertValid() call invariants on textstore and style database.
 *
 *	Revision 2.4  1996/03/04  07:59:08  lewis
 *	Use StyledTextIOWriter_LedNativeFileFormat instead of obsolete
 *	NativeFormatWriterWithOLE2Emebddings.
 *
 *	Revision 2.3  1996/02/26  22:48:40  lewis
 *	define qDocBeingCreated so we can let LedItView being created
 *	in its CTOR what the doc ptr is (cuz it needs to know its
 *	text store early in the game).
 *
 *	Revision 2.2  1996/01/22  05:41:51  lewis
 *	Use Led_Verify instead of VERIFY.
 *	fCommandHanldler (undo) support.
 *
 *	Revision 2.1  1996/01/11  08:31:55  lewis
 *	Misc cleanups?
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
	#pragma	warning (4: 4786)
#endif

#include	<afxwin.h>

#include	"StyledTextIO_LedNative.h"
#include	"StyledTextIO_PlainText.h"

#include	"LedItControlItem.h"
#include	"LedItServerItem.h"
#include	"LedItView.h"

#include	"LedItDocument.h"







// special exception handling just for MFC library implementation
// copied here so I could clone MFC code as needed - not well understood - UGH!!! - LGP 951227
#ifndef _AFX_OLD_EXCEPTIONS
	#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)
#else   //!_AFX_OLD_EXCEPTIONS
	#define DELETE_EXCEPTION(e)
#endif  //_AFX_OLD_EXCEPTIONS




static	void	AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CString strFilterExt, CString strFilterName);
static	void	AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate);






/*
 ********************************************************************************
 ******************************** LedItDocument *********************************
 ********************************************************************************
 */
Led_FileFormat	LedItDocument::sHiddenDocOpenArg	=	eUnknownFormat;	// See LedItDocument::OnOpenDocument ()

IMPLEMENT_DYNCREATE(LedItDocument, COleServerDoc)

BEGIN_MESSAGE_MAP		(LedItDocument, 		COleServerDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK,	OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT,	OnUpdateObjectVerbMenu)
	ON_COMMAND			(ID_OLE_EDIT_CONVERT,	OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS,		OnUpdateEditLinksMenu)
	ON_COMMAND			(ID_OLE_EDIT_LINKS,		OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST,		OnUpdateObjectVerbMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE,			OnUpdateFileSave)
	ON_COMMAND			(ID_FILE_SAVE_COPY_AS,	OnFileSaveCopyAs)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(LedItDocument, COleServerDoc)
END_DISPATCH_MAP()

// Note: we add support for IID_ILedIt to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {0FC00622-28BD-11CF-899C-00AA00580324}
static const IID IID_ILedIt =	{ 0xfc00622, 0x28bd, 0x11cf, { 0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x24 } };

BEGIN_INTERFACE_MAP(LedItDocument, COleServerDoc)
	INTERFACE_PART(LedItDocument, IID_ILedIt, Dispatch)
END_INTERFACE_MAP()






LedItDocument::LedItDocument ():
	COleServerDoc (),
	MarkerOwner (),
	fTextStore (),
	fRTFInfo (),
	fStyleDatabase (),
	fParagraphDatabase (),
	fHidableTextDatabase (),
	fCommandHandler (kMaxNumUndoLevels),
	fFileFormat (eDefaultFormat),
	fHTMLInfo ()
{
	EnableAutomation ();
	::AfxOleLockApp ();
	fTextStore.AddMarkerOwner (this);
	fStyleDatabase = new StandardStyledTextImager::StyleDatabaseRep (fTextStore);
	fParagraphDatabase = new WordProcessor::ParagraphDatabaseRep (fTextStore);
	fHidableTextDatabase = new UniformHidableTextMarkerOwner (fTextStore, &fCommandHandler);
}

LedItDocument::~LedItDocument ()
{
	fTextStore.RemoveMarkerOwner (this);
	::AfxOleUnlockApp ();
}

void	LedItDocument::DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ()
{
	SetModifiedFlag ();
}

TextStore*	LedItDocument::PeekAtTextStore () const
{
	return &const_cast<LedItDocument*> (this)->fTextStore;
}

BOOL	LedItDocument::OnNewDocument ()
{
	fCommandHandler.Commit ();
	if (!COleServerDoc::OnNewDocument ()) {
		return FALSE;
	}
	fFileFormat = eDefaultFormat;
	fStyleDatabase = new StandardStyledTextImager::StyleDatabaseRep (fTextStore);
	fParagraphDatabase = new WordProcessor::ParagraphDatabaseRep (fTextStore);
	fHidableTextDatabase = new UniformHidableTextMarkerOwner (fTextStore, &fCommandHandler);
	return TRUE;
}

COleServerItem*	LedItDocument::OnGetEmbeddedItem ()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.
	LedItServerItem* pItem = new LedItServerItem(this);
	ASSERT_VALID (pItem);
	return pItem;
}

BOOL	LedItDocument::DoSave (LPCTSTR lpszPathName, BOOL bReplace)
{
	Led_FileFormat	fileFormat	=	fFileFormat;

	CString newName = lpszPathName;
	if (newName.IsEmpty()) {
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT(pTemplate != NULL);
		newName = m_strPathName;
		if (bReplace && newName.IsEmpty()) {
			newName = m_strTitle;
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
			if (iBad != -1) {
				newName.ReleaseBuffer (iBad);
			}
#if 0
			// append the default suffix if there is one
			CString strExt;
			if (pTemplate->GetDocString(strExt, CDocTemplate::filterExt) &&
			  !strExt.IsEmpty())
			{
				ASSERT(strExt[0] == '.');
				newName += strExt;
			}
#endif
		}

		if (bReplace) {
			if (not DoPromptSaveAsFileName (newName, &fileFormat)) {
				return false;
			}
		}
		else {
			if (not DoPromptSaveCopyAsFileName (newName, &fileFormat)) {
				return false;
			}
		}
	}

	CWaitCursor wait;

	// During the actual save, we must temporarily reset the fFileFormat field so we
	// know what format to write. Don't make the change permanent til much later, when the
	// write has succeeded, and we know that this was a save, and not a save-a-copy call.
	Led_FileFormat	realSavedDocFormat	=	fFileFormat;
	fFileFormat = fileFormat;
	try {
		if (!OnSaveDocument (newName)) {
			if (lpszPathName == NULL) {
				// be sure to delete the file
				TRY {
					CFile::Remove (newName);
				}
				CATCH_ALL(e) {
					TRACE0 ("Warning: failed to delete file after failed SaveAs.\n");
					DELETE_EXCEPTION(e);
				}
				END_CATCH_ALL
			}
			return FALSE;
		}
	}
	catch (...) {
		fFileFormat = realSavedDocFormat;
		throw;
	}
	fFileFormat = realSavedDocFormat;

	// reset the title and change the document name
	if (bReplace) {
		SetPathName (newName);
		fFileFormat = fileFormat;
	}

	return TRUE;        // success
}

void	LedItDocument::Serialize (CArchive& ar)
{
	if (ar.IsStoring()) {
		Led_Require (fFileFormat != eUnknownFormat);	// We must have chosen a file format by now...

		WordProcessor::WordProcessorTextIOSrcStream		source (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);
		StyledTextIOWriterSinkStream_Memory				sink;

		switch (fFileFormat) {
			case	eTextFormat: {
				StyledTextIOWriter_PlainText	textWriter (&source, &sink);
				textWriter.Write ();
			}
			break;

			case	eRTFFormat: {
				StyledTextIOWriter_RTF	textWriter (&source, &sink, &fRTFInfo);
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
				Led_Assert (false);	// don't support writing that format (yet?)!
			}
			break;
		}
		ar.Write ((char*)sink.PeekAtData (), sink.GetLength ());
	}
	else {
		{
			// Peculiar performance hack. Lots of parts of Led will do 'Refresh' to mark the
			// screen as needing redisplay. If done enuf times, these can be expensive. So Led is
			// 'clever' - and notices that if the entire screen is invalid, in need not do the elaborate
			// computations about what needs further invalidating. This initial call to Refresh () call
			// won't REALLY end up doing any extra redisplay, cuz we're reading in a new file, and would be
			// redisplaying the whole window anyhow. But it MIGHT save us a bit of time not calculating what
			// little bits of the screen to redisplay.
			// LGP 970619
			POSITION pos = GetFirstViewPosition ();
			for (CView* p = GetNextView (pos); p != NULL; p = GetNextView (pos)) {
				LedItView*	v		=	dynamic_cast<LedItView*> (p);
				if (v != NULL) {
					v->Refresh ();
				}
			}
		}
		CFile*	file	=	ar.GetFile ();
		ASSERT_VALID (file);
		DWORD	nLen = file->GetLength();				// maybe should subtract current offset?
		Led_SmallStackBuffer<char>	buf (nLen);
		if (ar.Read (buf, nLen) != nLen) {
			AfxThrowArchiveException (CArchiveException::endOfFile);
		}
		StyledTextIOSrcStream_Memory					source (buf, nLen);
		WordProcessor::WordProcessorTextIOSinkStream	sink (&fTextStore, fStyleDatabase, fParagraphDatabase, fHidableTextDatabase);

ReRead:
		switch (fFileFormat) {
			case	eTextFormat: {
				StyledTextIOReader_PlainText	textReader (&source, &sink);
				textReader.Read ();
			}
			break;

			case	eLedPrivateFormat: {
				LedItControlItem::DocContextDefiner tmp (this);
				StyledTextIOReader_LedNativeFileFormat	textReader (&source, &sink);
				textReader.Read ();
			}
			break;

			case	eRTFFormat: {
				LedItControlItem::DocContextDefiner tmp (this);
				StyledTextIOReader_RTF	textReader (&source, &sink, &fRTFInfo);
				textReader.Read ();
			}
			break;

			case	eHTMLFormat: {
				StyledTextIOReader_HTML	textReader (&source, &sink, &fHTMLInfo);
				textReader.Read ();
			}
			break;

			case	eUnknownFormat: {
				/*
				 *	Should enhance this unknown/format reading code to take into account file suffix in our guess.
				 */

				// Try RTF	
				try {
					StyledTextIOReader_RTF	reader (&source, &sink, &fRTFInfo);
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

			default: {
				Led_Assert (false);	// don't support reading that format (yet?)!
			}
			break;
		}
		sink.Flush ();		// explicit Flush () call - DTOR would have done it - but there exceptions get silently eaten - this will at least show them...
	}
}

BOOL	LedItDocument::OnOpenDocument (LPCTSTR lpszPathName) 
{
	fCommandHandler.Commit ();

	/*
	 *	Because we need to vector through a bunch of layers of MFC code which doesn't pass along this format
	 *	inforamtion, we needed some hack to get it from where we knew the file type to here, where
	 *	we open the file. The application/DocMgr code made it hard to get/propagate the file type info
	 *	but we managed that. Were we dropped the ball was on the DocTemplate code. That looked too complex
	 *	to clone/redo to pass this info along, so when we get to the point of saying template->OpenDocument()
	 *	we first set this sHiddenDocOpenArg argument.
	 */
	fFileFormat = sHiddenDocOpenArg;

	BOOL	result	=	 COleDocument::OnOpenDocument (lpszPathName);

	// Work around bug in CDocument::OpenDocument() where it fails to call Serialize() if empty src-file (and so WE
	// fail to generate fFileFormat value)
	if (fFileFormat == eUnknownFormat) {
		fFileFormat = eTextFormat;
	}
	return result;
}

void	LedItDocument::OnEditClearAll ()
{
	DeleteContents ();
	UpdateAllViews (NULL);
}

void	LedItDocument::OnUpdateFileSave (CCmdUI* pCmdUI)
{
	ASSERT_VALID (this);
	Led_RequireNotNil (pCmdUI);
	// only enable save command if dirty, or no file name associated with this document
	pCmdUI->Enable (IsModified () or GetPathName ().GetLength () == 0);
}

void	LedItDocument::OnFileSaveCopyAs ()
{
	ASSERT_VALID (this);
	Led_Assert (m_bRemember);
	
	LPSTORAGE	savedStorage	=	m_lpRootStg;
	m_lpRootStg = NULL;

	Led_FileFormat	savedFileFormat	=	fFileFormat;
	
	try {
		DoSave (NULL, false);
	}
	catch (...) {
		m_lpRootStg = savedStorage;
		m_bRemember = true;
		fFileFormat = savedFileFormat;
		throw;
	}

	m_lpRootStg = savedStorage;
	m_bRemember = true;
	fFileFormat = savedFileFormat;
}

void	LedItDocument::DeleteContents ()
{
	fTextStore.Replace (fTextStore.GetStart (), fTextStore.GetEnd (), LED_TCHAR_OF (""), 0);
}

bool	LedItDocument::DoPromptSaveAsFileName (CString& fileName, Led_FileFormat* fileFormat)
{
	Led_RequireNotNil (fileFormat);
	return DoPromptFileName (fileName, AFX_IDS_SAVEFILE, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, fileFormat);
}

bool	LedItDocument::DoPromptSaveCopyAsFileName (CString& fileName, Led_FileFormat* fileFormat)
{
	Led_RequireNotNil (fileFormat);
	return DoPromptFileName (fileName, AFX_IDS_SAVEFILECOPY, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, fileFormat);
}

bool	LedItDocument::DoPromptOpenFileName (CString& fileName, Led_FileFormat* fileFormat)
{
	Led_RequireNotNil (fileFormat);
	return DoPromptFileName (fileName, AFX_IDS_OPENFILE, true, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, fileFormat);
}

bool	LedItDocument::DoPromptFileName (CString& fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, Led_FileFormat* fileFormat)
{
	Led_RequireNotNil (fileFormat);
	CFileDialog dlgFile (isOpenDialogCall);

	CString title;
	Led_Verify (title.LoadString (nIDSTitle));

	dlgFile.m_ofn.Flags |= fileDLogFlags;

	CString strFilter;

	CDocTemplate*	ledItPrivateDocFormatTemplate	=	NULL;
	{
		// do for all doc template
		POSITION pos = AfxGetApp()->m_pDocManager->GetFirstDocTemplatePosition();
		ledItPrivateDocFormatTemplate = AfxGetApp()->m_pDocManager->GetNextDocTemplate(pos);
	}
	if (nIDSTitle == AFX_IDS_OPENFILE) {
		AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.htm;*.html;*.led;*.rtf;*.txt;", "All Recognized (*.htm;*.html;*.led;*.rtf;*.txt)");
		AppendFilterSuffix (strFilter, dlgFile.m_ofn, ".*", "All Files (*.*)");
	}
	AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.htm;*.html", "HTML file (*.htm;*.html)");
	AppendFilterSuffix (strFilter, dlgFile.m_ofn, ledItPrivateDocFormatTemplate);
	AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.rtf", "Microsoft Rich Text Format (*.rtf)");
	AppendFilterSuffix (strFilter, dlgFile.m_ofn, "*.txt", "Plain Text (*.txt)");

	strFilter += (TCHAR)'\0';   // last string

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;

// MAYBE SHOULD ELIMINATE TYPE-SUFFIX???
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer (_MAX_PATH);

	if (nIDSTitle == AFX_IDS_OPENFILE) {
		dlgFile.m_ofn.nFilterIndex = 1;	// default to "All Recognized"
	}
	else {
		Led_FileFormat	initialFormat	=	*fileFormat;
		if (initialFormat == eUnknownFormat) {
			initialFormat = eDefaultFormat;
		}
		switch (initialFormat) {
			case	eHTMLFormat:		dlgFile.m_ofn.nFilterIndex = 1; break;
			case	eLedPrivateFormat:	dlgFile.m_ofn.nFilterIndex = 2; break;
			case	eRTFFormat:			dlgFile.m_ofn.nFilterIndex = 3; break;
			case	eTextFormat:		dlgFile.m_ofn.nFilterIndex = 4; break;
			default:					Led_Assert (false); break;
		}
	}
	bool bResult = (dlgFile.DoModal() == IDOK);
	fileName.ReleaseBuffer();
	if (bResult) {
		if (nIDSTitle == AFX_IDS_OPENFILE) {
			switch (dlgFile.m_ofn.nFilterIndex) {
				case	1:	*fileFormat = eUnknownFormat; break;
				case	2:	*fileFormat = eUnknownFormat; break;
				case	3:	*fileFormat = eHTMLFormat; break;
				case	4:	*fileFormat = eLedPrivateFormat; break;
				case	5:	*fileFormat = eRTFFormat; break;
				case	6:	*fileFormat = eTextFormat; break;
				default:	*fileFormat = eUnknownFormat; break;
			}
		}
		else {
			switch (dlgFile.m_ofn.nFilterIndex) {
				case	1:	*fileFormat = eHTMLFormat; break;
				case	2:	*fileFormat = eLedPrivateFormat; break;
				case	3:	*fileFormat = eRTFFormat; break;
				case	4:	*fileFormat = eTextFormat; break;
				default:	*fileFormat = eUnknownFormat; break;
			}
		}

		// If saving a file, and user specified no extension, we add one. Not 100% sure this is the right way todo this.
		// But seems more often than not to be right... LGP 971019
		if (not (fileDLogFlags & OFN_FILEMUSTEXIST)) {
			if (dlgFile.GetFileExt () == "") {
				switch (*fileFormat) {
					case	eTextFormat:		fileName += ".txt"; break;
					case	eRTFFormat:			fileName += ".rtf"; break;
					case	eHTMLFormat:		fileName += ".html"; break;
					case	eLedPrivateFormat:	fileName += ".led"; break;
				}
			}
		}
	}
	return bResult;
}

#if		qDebug
void	LedItDocument::AssertValid () const
{
	COleServerDoc::AssertValid ();
	fTextStore.Invariant ();
	//fStyleDatabase.Invariant ();	Cannot do this cuz we're sometimes called at in-opportune times, while updating
	// the styles...called from MFC...
}
#endif





/*
 ********************************************************************************
 ******************************** AppendFilterSuffix ****************************
 ********************************************************************************
 */

static	void	AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CString strFilterExt, CString strFilterName)
{
	Led_Require (not strFilterExt.IsEmpty ());
	Led_Require (not strFilterName.IsEmpty ());

	// add to filter
	filter += strFilterName;
	ASSERT(!filter.IsEmpty());  // must have a file type name
	filter += (TCHAR)'\0';  // next string please
	filter += (TCHAR)'*';
	filter += strFilterExt;
	filter += (TCHAR)'\0';  // next string please
	ofn.nMaxCustFilter++;
}

static	void	AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate)
{
	ASSERT_VALID(pTemplate);
	ASSERT_KINDOF(CDocTemplate, pTemplate);
	CString	strFilterExt;
	CString	strFilterName;
	if (pTemplate->GetDocString(strFilterExt, CDocTemplate::filterExt) && !strFilterExt.IsEmpty() &&
		pTemplate->GetDocString(strFilterName, CDocTemplate::filterName) && !strFilterName.IsEmpty()
		)
	{
		AppendFilterSuffix (filter, ofn, strFilterExt, strFilterName);
	}
}






// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
