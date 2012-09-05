/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/LedLineItDocument.cpp,v 2.40 2004/01/28 17:39:08 lewis Exp $
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
 *	Revision 2.40  2004/01/28 17:39:08  lewis
 *	fix small bug with startup linebreak code - break if curLineSize >= fBreakWidths(not just >)
 *	
 *	Revision 2.39  2004/01/27 20:36:14  lewis
 *	SPR#1625: added a dialog and code to catch the case of reading a file with long lines, and offer to break it at (user specifyable  in the dialog) character count breakpoints.
 *	
 *	Revision 2.38  2003/12/11 03:09:16  lewis
 *	SPR#1585: move spellcheckengine object to application. Use qIncludeBasicSpellcheckEngine define to control its inclusion. Define default locaiton for LedLineIt spell dict.
 *	
 *	Revision 2.37  2003/11/25 23:00:56  lewis
 *	SPR#1563: use TextBreaks_Basic_TextEditor in LedLineIt
 *	
 *	Revision 2.36  2003/09/22 22:08:07  lewis
 *	SPR#1552: change args to internalizer.InternalizeFlavor_FILEData
 *	
 *	Revision 2.35  2003/09/22 15:56:58  lewis
 *	SPR#1552- override  OnOpenDocument() to replace MFC's buggy implemeantion with our
 *	own (hopefully less buggy version)
 *	
 *	Revision 2.34  2003/06/03 12:31:13  lewis
 *	SPR#1513: hook in spellcheck engine
 *	
 *	Revision 2.33  2003/04/16 16:54:41  lewis
 *	SPR#1433: for default save file name - dont grab from doctemplate, but use .txt instead
 *	
 *	Revision 2.32  2003/04/10 19:35:56  lewis
 *	lose unused OnEditClearAll method
 *	
 *	Revision 2.31  2003/04/01 16:15:50  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.30  2003/03/27 16:38:18  lewis
 *	SPR#1384: LedLineItDocument::DidUpdateText () checks updateInfo.fRealContentUpdate and only set
 *	dirtyflag if real update
 *	
 *	Revision 2.29  2003/01/20 16:31:10  lewis
 *	minor tweeks noted by MWERKS CW8.3 compiler
 *	
 *	Revision 2.28  2002/09/09 02:56:59  lewis
 *	fix MSVC warning
 *	
 *	Revision 2.27  2002/05/06 21:31:19  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.26  2001/11/27 00:28:22  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.25  2001/09/18 15:05:37  lewis
 *	fix last fix (BOM only first chunk on write of text)
 *	
 *	Revision 2.24  2001/09/18 14:30:09  lewis
 *	when writing docs (serialize method) - only write BOM on first chunk
 *	
 *	Revision 2.23  2001/09/13 02:50:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.22  2001/09/13 02:49:30  lewis
 *	we had code to add existing (sometimes default) code page to list of code pages in popup -
 *	LOSE that code. Works badly - eg on Win98 - where we dont even SUPPORT that code page
 *	
 *	Revision 2.21  2001/09/11 12:27:14  lewis
 *	SPR#1022- add BOM flag (&CTOR overload) to CodePageConverter class to fix problems with BOM handling - esp UTF7/8
 *	
 *	Revision 2.20  2001/09/09 22:36:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.19  2001/09/09 22:31:56  lewis
 *	SPR#0994, 0697- Added fCodePage flag to the LedLineItDocument class, and did alot
 *	of work reworking the calling of the CFileDialog so it could use my new replacement
 *	version (which includes an Encoding popup). Pretty ugly code for passing args through
 *	the MFC layers (like codepage from dialog to Serialize method). Most of the REAL guts
 *	code is in the CodePage module, and the stuff here is very MFC-specific.
 *	
 *	Revision 2.18  2001/08/30 01:02:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.17  2001/07/31 15:52:14  lewis
 *	warnings
 *	
 *	Revision 2.16  2000/06/23 21:52:23  lewis
 *	SPR#0805- handle opening of wide-char UNICODE files
 *	
 *	Revision 2.15  2000/01/17 23:07:11  lewis
 *	SPR#0692 - fix so we can save text files at all (CP_ACP) - later do even better and
 *	let user pick code page (or UNICODE)
 *	
 *	Revision 2.14  1999/12/14 18:54:17  lewis
 *	fix so can open files when building for UNICODE (though this code is still far from UNICODe
 *	friendly - need to support UNICODE format files still - and writing code maybe wrong for UNICODE
 *	
 *	Revision 2.13  1999/12/09 17:35:27  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.12  1999/12/09 03:29:07  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new
 *	Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.11  1999/02/06 14:13:08  lewis
 *	lose unneeded OpenDocument override
 *	
 *	Revision 2.10  1998/10/30 15:08:09  lewis
 *	lots of little cleanups - using vector instead of Led_Array, mutable instaed of
 *	const cast, new msvc60 warning
 *	stuff, etc.
 *	
 *	Revision 2.9  1997/12/24  04:51:19  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1997/09/29  18:04:02  lewis
 *	Lose qLedFirstIndex
 *
 *	Revision 2.7  1997/07/27  16:03:03  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.6  1997/07/23  23:21:31  lewis
 *	lose some old LedIt! remnants
 *
 *	Revision 2.5  1997/07/14  14:59:39  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.4  1997/06/28  17:38:07  lewis
 *	Lose gDocBeingCreated hack - didn't work for MDI, and OnInitialUpdate() code much cleaner anyhow.
 *
 *	Revision 2.3  1997/06/23  16:41:34  lewis
 *	*** empty log message ***
 *
 *	Revision 2.2  1997/06/18  03:47:26  lewis
 *	qIncludePrefixFile and cleanups
 *
 *	Revision 2.1  1997/03/04  20:21:01  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1997/01/10  03:38:30  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxwin.h>

#include	"FlavorPackage.h"

#include	"LedLineItServerItem.h"
#include	"Resource.h"

#include	"LedLineItDocument.h"







// special exception handling just for MFC library implementation
// copied here so I could clone MFC code as needed - not well understood - UGH!!! - LGP 951227
#ifndef _AFX_OLD_EXCEPTIONS
	#define DELETE_EXCEPTION(e) do { e->Delete(); } while (0)
#else   //!_AFX_OLD_EXCEPTIONS
	#define DELETE_EXCEPTION(e)
#endif  //_AFX_OLD_EXCEPTIONS




static	void	AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CString strFilterExt, CString strFilterName);
static	void	AppendFilterSuffix (CString& filter, OPENFILENAME& ofn, CDocTemplate* pTemplate);






	static	Led_SDK_String	MapCodePageToPrettyName (CodePage cp)
		{
			switch (cp) {
				case	kAutomaticallyGuessCodePage:		return Led_SDK_TCHAROF ("Automaticly Detect");
				default:									return CodePagePrettyNameMapper::GetName (cp);
			}
		}

	static	bool	ShuffleToFront (vector<CodePage>* codePages, CodePage cp)
		{
			vector<CodePage>::iterator	i	=	std::find (codePages->begin (), codePages->end (), cp);
			if (i != codePages->end ()) {
				codePages->erase (i);
				codePages->insert (codePages->begin (), cp);
				return true;
			}
			return false;
		}


class	FileDialogWithCodePage : public CFileDialog {
	private:
		typedef	CFileDialog	inherited;
	public:
		FileDialogWithCodePage (bool asOpenDialog, const vector<CodePage>& codePages, CodePage initialCodePage) :
			CFileDialog (asOpenDialog),
			fCodePages (codePages),
			fCodePage (initialCodePage)
		{
			m_ofn.Flags |= OFN_ENABLETEMPLATE | OFN_EXPLORER;
			m_ofn.lpTemplateName = m_lpszTemplateName = MAKEINTRESOURCE (kFileDialogAddOnID);
		}

		override	BOOL OnInitDialog ()
			{
				inherited::OnInitDialog ();
				fCodePageComboBox.SubclassWindow (::GetDlgItem (GetSafeHwnd (), kFileDialog_EncodingComboBox));
				for (vector<CodePage>::const_iterator i = fCodePages.begin (); i != fCodePages.end (); ++i) {
					fCodePageComboBox.AddString (MapCodePageToPrettyName (*i).c_str ());
					if (*i == fCodePage) {
						fCodePageComboBox.SetCurSel (i-fCodePages.begin ());
					}
				}
				return (true);
			}

		CodePage	fCodePage;

	protected:
		afx_msg	void	OnCodePageSelChange ()
		{
			int	curSel	=	fCodePageComboBox.GetCurSel ();
			if (curSel != CB_ERR) {
				Led_Assert (curSel < static_cast<int> (fCodePages.size ()));
				fCodePage = fCodePages[curSel];
			}
		}

	private:
		DECLARE_MESSAGE_MAP()

	private:
		vector<CodePage>	fCodePages;
		CComboBox			fCodePageComboBox;
};

BEGIN_MESSAGE_MAP(FileDialogWithCodePage, CFileDialog)
	ON_CBN_SELCHANGE  (kFileDialog_EncodingComboBox, OnCodePageSelChange)
END_MESSAGE_MAP()



namespace	{
	class	LineTooLongOnReadDialog : public CDialog {
		public:
			LineTooLongOnReadDialog (const Led_SDK_String& message, size_t breakCount) :
				CDialog (kLineTooLongOnRead_DialogID),
				fMessage (message),
				fBreakCount (breakCount)
				{
				}
			override	BOOL OnInitDialog ()
				{
					BOOL	result	=	CDialog::OnInitDialog();
					Led_CenterWindowInParent (m_hWnd);
					SetDlgItemText (kLineTooLongOnRead_Dialog_MessageFieldID, fMessage.c_str ());
					SetDlgItemInt (kLineTooLongOnRead_Dialog_BreakNumFieldID, fBreakCount);
					return (result);
				}
		override	void	OnOK ()
			{
				size_t	origBreakCount	=	fBreakCount;
				BOOL	trans	=	false;
				fBreakCount = GetDlgItemInt (kLineTooLongOnRead_Dialog_BreakNumFieldID, &trans);
				if (not trans) {
					fBreakCount = origBreakCount;
				}
				CDialog::OnOK ();
			}
		private:
			Led_SDK_String	fMessage;
		public:
			size_t			fBreakCount;
		protected:
			DECLARE_MESSAGE_MAP()
	};
	BEGIN_MESSAGE_MAP(LineTooLongOnReadDialog, CDialog)
	END_MESSAGE_MAP()
}







/*
 ********************************************************************************
 **************************** LedLineItDocument *********************************
 ********************************************************************************
 */
CodePage	LedLineItDocument::sHiddenDocOpenArg	=	kIGNORECodePage;

IMPLEMENT_DYNCREATE(LedLineItDocument, COleServerDoc)

BEGIN_MESSAGE_MAP		(LedLineItDocument, 		COleServerDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK,	OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT,	OnUpdateObjectVerbMenu)
	ON_COMMAND			(ID_OLE_EDIT_CONVERT,	OnEditConvert)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS,		OnUpdateEditLinksMenu)
	ON_COMMAND			(ID_OLE_EDIT_LINKS,		OnEditLinks)
	ON_UPDATE_COMMAND_UI(ID_OLE_VERB_FIRST,		OnUpdateObjectVerbMenu)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE,			OnUpdateFileSave)
	ON_COMMAND			(ID_FILE_SAVE_COPY_AS,	OnFileSaveCopyAs)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(LedLineItDocument, COleServerDoc)
END_DISPATCH_MAP()

// Note: we add support for IID_ILedLineIt to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {0FC00622-28BD-11CF-899C-00AA00580324}
static const IID IID_ILedLineIt =	{ 0xfc00622, 0x28bd, 0x11cf, { 0x89, 0x9c, 0x0, 0xaa, 0x0, 0x58, 0x3, 0x24 } };

BEGIN_INTERFACE_MAP(LedLineItDocument, COleServerDoc)
	INTERFACE_PART(LedLineItDocument, IID_ILedLineIt, Dispatch)
END_INTERFACE_MAP()




LedLineItDocument::LedLineItDocument ():
	COleServerDoc (),
	MarkerOwner (),
	fTextStore (),
	fCommandHandler (kMaxNumUndoLevels),
	fCodePage (kDefaultNewDocCodePage)
{
	EnableAutomation ();
	AfxOleLockApp ();

	fTextStore.SetTextBreaker (Led_RefCntPtr<TextBreaks> (new TextBreaks_Basic_TextEditor ())); 
	fTextStore.AddMarkerOwner (this);
}

LedLineItDocument::~LedLineItDocument ()
{
	fTextStore.RemoveMarkerOwner (this);
	AfxOleUnlockApp ();
}

void	LedLineItDocument::DidUpdateText (const UpdateInfo& updateInfo) throw ()
{
	if (updateInfo.fRealContentUpdate) {
		SetModifiedFlag ();
	}
}

TextStore*	LedLineItDocument::PeekAtTextStore () const
{
	return &const_cast<LedLineItDocument*> (this)->fTextStore;
}

BOOL	LedLineItDocument::OnNewDocument ()
{
	fCommandHandler.Commit ();
	if (!COleServerDoc::OnNewDocument ()) {
		return FALSE;
	}
	return TRUE;
}

COleServerItem*	LedLineItDocument::OnGetEmbeddedItem ()
{
	// OnGetEmbeddedItem is called by the framework to get the COleServerItem
	//  that is associated with the document.  It is only called when necessary.
	LedLineItServerItem* pItem = new LedLineItServerItem(this);
	ASSERT_VALID (pItem);
	return pItem;
}

BOOL	LedLineItDocument::DoSave (LPCTSTR lpszPathName, BOOL bReplace)
{
	CodePage	useCodePage	=	fCodePage;
	CString		newName = lpszPathName;
	if (newName.IsEmpty()) {
		CDocTemplate* pTemplate = GetDocTemplate ();
		ASSERT(pTemplate != NULL);
		newName = m_strPathName;
		if (bReplace && newName.IsEmpty()) {
			newName = m_strTitle;
			// check for dubious filename
			int iBad = newName.FindOneOf(_T(" #%;/\\"));
			if (iBad != -1) {
				newName.ReleaseBuffer (iBad);
			}

			// append .txt by default (SPR#1433) instead of grabbing from DocTemplate
			if (not newName.IsEmpty ()) {
				newName += _T (".txt");
			}
		}

		if (bReplace) {
			if (not DoPromptSaveAsFileName (&newName, &useCodePage)) {
				return false;
			}
		}
		else {
			if (not DoPromptSaveCopyAsFileName (&newName, &useCodePage)) {
				return false;
			}
		}
	}

	CWaitCursor wait;

	CodePage	savedCodePage	=	fCodePage;
	fCodePage = useCodePage;
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
			if (not bReplace) {
				fCodePage = savedCodePage;
			}
			return FALSE;
		}
		if (not bReplace) {
			fCodePage = savedCodePage;
		}
	}
	catch (...) {
		if (not bReplace) {
			fCodePage = savedCodePage;
		}
		throw;
	}

	// reset the title and change the document name
	if (bReplace) {
		SetPathName (newName);
	}

	return TRUE;        // success
}

BOOL	LedLineItDocument::OnOpenDocument (LPCTSTR lpszPathName)
{
	/*
	 *	Override this - instead of counting on default implementation - because the MFC version with 
	 *	Visual Studio.Net 2003 is buggy when opening read-only files (cuz they are already opened
	 *	someplace else, or because they are on a CD). Do this simple override to
	 *	get good/better reading files functionality.
	 *
	 *	Note that this code being here probably means the READING side of the Serialize code is never
	 *	executed (but leave it in there in case someone clones the code to use in another app, and there
	 *	they use the serialize code).
	 *
	 *			-- LGP 2003-09-22 - for SPR#1552.
	 *
	 *	Also - later added further changes - such as using MyFlavorPackageInternalizer to breakup
	 *	long lines, etc...
	 *			-- LGP 2004-01-27
	 */
	fCommandHandler.Commit ();

	class	MyFlavorPackageInternalizer : public FlavorPackageInternalizer {
		private:
			typedef	FlavorPackageInternalizer	inherited;
		public:
			enum	{kMaxLineSize	=	1024};
		public:
			MyFlavorPackageInternalizer (TextStore& ts):
				inherited (ts),
				fBreakLongLines (false),
				fBreakWidths (kMaxLineSize)
				{
				}
		public:
			override	void	InternalizeFlavor_FILEGuessFormatsFromStartOfData (
											Led_ClipFormat* suggestedClipFormat,
											CodePage* suggestedCodePage,
											const Byte* fileStart, const Byte* fileEnd
									)
				{
					size_t	curLineSize	=	0;
					size_t	maxLineSize	=	0;
					for (const Byte* p = fileStart; p != fileEnd; ++p) {
						if (*p == '\n' or *p == '\r') {
							curLineSize = 0;
						}
						else {
							curLineSize++;
						}
						maxLineSize = max (maxLineSize, curLineSize);
					}
					if (suggestedCodePage != NULL and (*suggestedCodePage == kCodePage_UNICODE_WIDE or *suggestedCodePage == kCodePage_UNICODE_WIDE_BIGENDIAN)) {
						maxLineSize /= 2;	// because we'd be counting null-bytes between chars.
											// Note this whole computation is VERY approximate - because its counting raw bytes of what could be
											// encoded text. For example - if the text was SJIS or UTF-7 encoding of far-east text - this would
											// greatly exagerate the estimated line size...
					}

					if (maxLineSize > kMaxLineSize) {
						LineTooLongOnReadDialog	dlg (Format (Led_SDK_TCHAROF ("This file contains at least one very long line (approximately %d characters). This may reduce editor performance, and make viewing the file awkward. Long lines can optionally be automatically broken up if they exceed the 'Break at characer count' value below."), maxLineSize/100*100), kMaxLineSize);
						fBreakLongLines = (dlg.DoModal () == IDOK);
						fBreakWidths = dlg.fBreakCount;
					}
				}

			override	bool	InternalizeFlavor_FILEDataRawBytes (
										Led_ClipFormat* suggestedClipFormat,
										CodePage* suggestedCodePage,
										size_t from, size_t to,
										const void* rawBytes, size_t nRawBytes
								)
				{
					Led_ClipFormat	cf	=	(suggestedClipFormat == NULL or *suggestedClipFormat == kBadClipFormat)? kTEXTClipFormat: *suggestedClipFormat;
					Led_Require (cf == kTEXTClipFormat);

					fBreakWidths = max (fBreakWidths, 1);	// assure a decent value given...

					if (fBreakLongLines) {
						#if		qWideCharacters
							CodePage						useCodePage =	(suggestedCodePage == NULL or *suggestedCodePage == kCodePage_INVALID)?
																						CodePagesGuesser ().Guess (rawBytes, nRawBytes):
																						*suggestedCodePage
																					;
							if (suggestedCodePage != NULL) {
								*suggestedCodePage = useCodePage;
							}
							CodePageConverter				cpc			=	CodePageConverter (useCodePage, CodePageConverter::eHandleBOM);
							size_t							outCharCnt	=	cpc.MapToUNICODE_QuickComputeOutBufSize (reinterpret_cast<const char*> (rawBytes), nRawBytes + 1);
							Led_SmallStackBuffer<Led_tChar>	fileData2 (outCharCnt);
							cpc.MapToUNICODE (reinterpret_cast<const char*> (rawBytes), nRawBytes, static_cast<wchar_t*> (fileData2), &outCharCnt);
							size_t	charsRead = outCharCnt;
							Led_Assert (charsRead <= nRawBytes);
							charsRead = Led_NormalizeTextToNL (fileData2, charsRead, fileData2, charsRead);

							{
								Led_SmallStackBuffer<Led_tChar>	patchedData (charsRead + charsRead/fBreakWidths);
								size_t	curLineSize	=	0;
								size_t	ourIdx		=	0;
								for (const Led_tChar* p = fileData2; p != fileData2 + charsRead; ++p) {
									if (*p == '\n' or *p == '\r') {
										curLineSize = 0;
									}
									else {
										curLineSize++;
									}
									patchedData[ourIdx++] = *p;
									if (curLineSize >= fBreakWidths) {
										curLineSize = 0;
										patchedData[ourIdx++] = '\n';
									}
								}
								charsRead = ourIdx;
								GetTextStore ().Replace (from, to, patchedData, charsRead);
							}
							return true;
						#else
							// Copy byte by byte to a new buffer, and break lines that are too long - as I go....
							Led_SmallStackBuffer<Byte>	patchedBytes (nRawBytes + nRawBytes/fBreakWidths);
							size_t	curLineSize	=	0;
							size_t	ourIdx		=	0;
							for (const Byte* p = reinterpret_cast<const Byte*> (rawBytes); p != reinterpret_cast<const Byte*> (rawBytes) + nRawBytes; ++p) {
								if (*p == '\n' or *p == '\r') {
									curLineSize = 0;
								}
								else {
									curLineSize++;
								}
								patchedBytes[ourIdx++] = *p;
								if (curLineSize >= fBreakWidths) {
									curLineSize = 0;
									patchedBytes[ourIdx++] = '\n';
								}
							}
							return inherited::InternalizeFlavor_FILEDataRawBytes (suggestedClipFormat, suggestedCodePage, from, to, patchedBytes, ourIdx);
						#endif
					}
					else {
						return inherited::InternalizeFlavor_FILEDataRawBytes (suggestedClipFormat, suggestedCodePage, from, to, rawBytes, nRawBytes);
					}
				}

		public:
			bool	fBreakLongLines;
			size_t	fBreakWidths;
	};

	MyFlavorPackageInternalizer	internalizer (fTextStore);

	CodePage	useCodePage	=	fCodePage;
	if (LedLineItDocument::sHiddenDocOpenArg != kIGNORECodePage) {
		useCodePage	=	sHiddenDocOpenArg;
		if (useCodePage == kAutomaticallyGuessCodePage) {
			useCodePage =	kCodePage_INVALID;	// implies automatically guess
		}
	}
	Led_ClipFormat	cf	=	kTEXTClipFormat;
	internalizer.InternalizeFlavor_FILEData (lpszPathName, &cf, &useCodePage, 0, fTextStore.GetEnd ());
	fCodePage = useCodePage;	// use whatever codePage file was opened with... by default... for future saves

	if (not internalizer.fBreakLongLines) {
		SetModifiedFlag (FALSE);     // start off with doc unmodified
	}
	return true;
}

void	LedLineItDocument::Serialize (CArchive& ar)
{
	if (ar.IsStoring ()) {
		const size_t	kBufSize	=	8*1024;
		Led_tChar	buf[kBufSize];
		size_t	offset		=	0;
		size_t	eob			=	fTextStore.GetLength ();
		bool	firstTime	=	true;
		while (offset < eob) {
			size_t	charsToWrite	=	Led_Min (kBufSize, eob-offset);
			fTextStore.CopyOut (offset, charsToWrite, buf);
			offset += charsToWrite;
			#if		qMacOS
				Led_tChar	buf2[sizeof (buf)];
			#elif	qWindows
				Led_tChar	buf2[2*sizeof (buf)];
			#endif
			charsToWrite = Led_NLToNative (buf, charsToWrite, buf2, sizeof (buf2));
			#if		qWideCharacters
				CodePageConverter			cpc			=	CodePageConverter (fCodePage);
				cpc.SetHandleBOM (firstTime);		// only for the first block of text do we write a byte-order mark
				firstTime = false;
				size_t						outCharCnt	=	cpc.MapFromUNICODE_QuickComputeOutBufSize (static_cast<Led_tChar*> (buf2), charsToWrite + 1);
				Led_SmallStackBuffer<char>	buf3_ (outCharCnt);
				size_t	nBytesToWrite	=	0;
				cpc.MapFromUNICODE (static_cast<Led_tChar*> (buf2), charsToWrite, static_cast<char*> (buf3_), &outCharCnt);
				nBytesToWrite = outCharCnt;
				char*	buffp	=	static_cast<char*> (buf3_);
			#else
				char*	buffp	=	static_cast<char*> (buf2);
				size_t	nBytesToWrite	=	charsToWrite;
			#endif
			ar.Write (buffp, nBytesToWrite);
		}
	}
	else {
		CFile*	file	=	ar.GetFile ();
		ASSERT_VALID (file);
		DWORD	nLen = static_cast<DWORD> (file->GetLength ());				// maybe should subtract current offset?
		Led_SmallStackBuffer<char>	buf (nLen);
		if (ar.Read (buf, nLen) != nLen) {
			AfxThrowArchiveException (CArchiveException::endOfFile);
		}

		CodePage	useCodePage	=	fCodePage;
		if (LedLineItDocument::sHiddenDocOpenArg != kIGNORECodePage) {
			useCodePage	=	sHiddenDocOpenArg;
			if (useCodePage == kAutomaticallyGuessCodePage) {
				CodePagesGuesser::Confidence	conf	=	CodePagesGuesser::eLow;
				size_t			bytesToStrip	=	0;
				useCodePage =	CodePagesGuesser ().Guess (buf, nLen, &conf, &bytesToStrip);
			}
		}

		#if		qWideCharacters
			CodePageConverter				cpc			=	CodePageConverter (useCodePage, CodePageConverter::eHandleBOM);
			size_t							outCharCnt	=	cpc.MapToUNICODE_QuickComputeOutBufSize (static_cast<char*> (buf), nLen + 1);
			Led_SmallStackBuffer<Led_tChar>	result (outCharCnt);
			cpc.MapToUNICODE (static_cast<char*> (buf), nLen, static_cast<wchar_t*> (result), &outCharCnt);
			nLen = outCharCnt;
			result[nLen] = '\0';	// assure NUL-Term
			Led_tChar*	buffp	=	static_cast<Led_tChar*> (result);
		#else
			Led_tChar*	buffp	=	static_cast<char*> (buf);
		#endif

		nLen = Led_NormalizeTextToNL (buffp, nLen, buffp, nLen);
		fTextStore.Replace (0, 0, buffp, nLen);

		fCodePage = useCodePage;	// whatever codepage I just used to read the doc should be the new codepage...
	}
}

void	LedLineItDocument::OnUpdateFileSave (CCmdUI* pCmdUI)
{
	ASSERT_VALID (this);
	Led_RequireNotNil (pCmdUI);
	// only enable save command if dirty, or no file name associated with this document
	pCmdUI->Enable (IsModified () or GetPathName ().GetLength () == 0);
}

void	LedLineItDocument::OnFileSaveCopyAs ()
{
	ASSERT_VALID (this);
	Led_Assert (m_bRemember);
	
	LPSTORAGE	savedStorage	=	m_lpRootStg;
	m_lpRootStg = NULL;

	try {
		DoSave (NULL, false);
	}
	catch (...) {
		m_lpRootStg = savedStorage;
		m_bRemember = true;
		throw;
	}

	m_lpRootStg = savedStorage;
	m_bRemember = true;
}

void	LedLineItDocument::DeleteContents ()
{
	fTextStore.Replace (fTextStore.GetStart (), fTextStore.GetEnd (), LED_TCHAR_OF (""), 0);
}

bool	LedLineItDocument::DoPromptSaveAsFileName (CString* fileName, CodePage* codePage)
{
	return DoPromptFileName (fileName, AFX_IDS_SAVEFILE, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, codePage);
}

bool	LedLineItDocument::DoPromptSaveCopyAsFileName (CString* fileName, CodePage* codePage)
{
	return DoPromptFileName (fileName, AFX_IDS_SAVEFILECOPY, false, OFN_HIDEREADONLY | OFN_PATHMUSTEXIST, codePage);
}

bool	LedLineItDocument::DoPromptOpenFileName (CString* fileName, CodePage* codePage)
{
	return DoPromptFileName (fileName, AFX_IDS_OPENFILE, true, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, codePage);
}

bool	LedLineItDocument::DoPromptFileName (CString* fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, CodePage* codePage)
{
	vector<CodePage>	codePages	=	CodePagesInstalled::GetAll ();
	#if		qDebug
		{
			// We use these magic numbers internally here - just assure they don't conflict...
			Led_Assert (std::find (codePages.begin (), codePages.end (), kAutomaticallyGuessCodePage) == codePages.end ());
			Led_Assert (std::find (codePages.begin (), codePages.end (), kIGNORECodePage) == codePages.end ());
		}
	#endif

#if 0
	if (not isOpenDialogCall) {
		/*
		 *	Assure the given (argument) code page is in the list.
		 */
		vector<CodePage>::iterator	i	=	std::find (codePages.begin (), codePages.end (), *codePage);
		if (i == codePages.end ()) {
			codePages.push_back (*codePage);
		}
	}
#endif
	sort (codePages.begin (), codePages.end ());

	/*
	 *	Bring certain special code pages to the head of the list.
	 */
	(void)ShuffleToFront (&codePages, kCodePage_UNICODE_WIDE_BIGENDIAN);
	(void)ShuffleToFront (&codePages, kCodePage_UNICODE_WIDE);
	(void)ShuffleToFront (&codePages, kCodePage_UTF8);
	(void)ShuffleToFront (&codePages, kCodePage_UTF7);
	if (isOpenDialogCall) {
		codePages.insert (codePages.begin (), kAutomaticallyGuessCodePage);
	}


	FileDialogWithCodePage dlgFile (isOpenDialogCall, codePages, isOpenDialogCall? kAutomaticallyGuessCodePage: *codePage);

	CString title;
	Led_Verify (title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= fileDLogFlags;

	CString strFilter;
	AppendFilterSuffix (strFilter, dlgFile.m_ofn, ".txt", "Text Files (*.txt)");
	AppendFilterSuffix (strFilter, dlgFile.m_ofn, ".*", "All Files (*.*)");

	strFilter += (TCHAR)'\0';   // last string

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;

	dlgFile.m_ofn.lpstrFile = fileName->GetBuffer (_MAX_PATH);

	dlgFile.m_ofn.nFilterIndex = 2;	// default to "All"
	bool bResult = (dlgFile.DoModal() == IDOK);
	fileName->ReleaseBuffer ();
	*codePage = dlgFile.fCodePage;
	return bResult;
}

#if		qDebug
void	LedLineItDocument::AssertValid () const
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
