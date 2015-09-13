/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItDocument_h__
#define	__LedItDocument_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItDocument.h,v 1.12 2003/12/09 21:04:58 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItDocument.h,v $
 *	Revision 1.12  2003/12/09 21:04:58  lewis
 *	use new SpellCheckEngine_Basic_Simple class and call fSpellCheckEngine.SetUserDictionary to specify UD name
 *	
 *	Revision 1.11  2003/06/02 16:15:41  lewis
 *	SPR#1513: Simple spellcheck engine and spell check dialog support (windows only so far)
 *	
 *	Revision 1.10  2003/04/10 19:35:41  lewis
 *	lose unused OnEditClearAll method
 *	
 *	Revision 1.9  2002/05/06 21:30:58  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.8  2001/11/27 00:28:09  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.7  2001/09/26 15:41:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.6  2001/09/18 19:13:42  lewis
 *	SPR#0791- added support for toggle (use smart cut&paste, wrap-to-window,show hidden text)
 *	commands for MacOS
 *	
 *	Revision 1.5  2001/08/29 22:59:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.4  2001/07/19 02:22:38  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support.
 *	
 *	Revision 1.3  2001/06/05 13:35:38  lewis
 *	SPR#0949- Support popup in XWindows file picker to select file type, and adjust file
 *	suffixes (and for default filetype(autoguess) use file suffixes to guess).
 *	
 *	Revision 1.2  2001/05/16 16:03:18  lewis
 *	more massive changes. Make Options () stuff portable (though fake impl om mac/X). Got
 *	rid of AppWindow class - and now X-WIndows fully using LedDoc/LedView. Much more common
 *	code between implementations - but still a lot todo
 *	
 *	Revision 1.1  2001/05/14 20:54:43  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC/LedItPP project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */

#if		qMacOS
	#include	<LSingleDoc.h>
#elif	defined (WIN32)
	#include	<afxwin.h>
	#include	<afxole.h>
#endif

#include	"Command.h"
#include	"ChunkedArrayTextStore.h"
#include	"StyledTextImager.h"
#include	"StyledTextIO.h"
#include	"StyledTextIO_HTML.h"
#include	"StyledTextIO_RTF.h"
#include	"WordProcessor.h"

#include	"LedItConfig.h"




class	LedItView;

#if		qWindows
class	LedItServerItem;
#endif

class	LedItDocument :
	#if		qMacOS
		public LSingleDoc,
	#elif	qWindows
		public COleServerDoc,
	#endif
		public MarkerOwner
{
#if		qMacOS
	public:
		LedItDocument (LCommander *inSuper, FileFormat format);
#elif	qWindows
	protected: // create from serialization only
		LedItDocument ();
		DECLARE_DYNCREATE(LedItDocument)
#elif	qXWindows
	public:
		LedItDocument ();
#endif
	
	public:
		virtual ~LedItDocument();

	public:
		override	void		DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ();
		override	TextStore*	PeekAtTextStore () const;

#if		qMacOS
	// Call exactly once (inFileSpec==NULL for new window)
	public:
		nonvirtual	void	BuildDocWindow (const FSSpec* inFileSpec);

	public:
		static	const vector<LWindow*>&	GetDocumentWindows ();

	public:
		override	Boolean	ObeyCommand (CommandT	inCommand, void* ioParam);
		override	void	FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark,
												UInt16& outMark, Str255 outName
											);

		nonvirtual	void	OnSaveACopyAsCommand ();

	public:
		override	Boolean		IsModified ();

		override	Boolean	AskSaveAs (FSSpec& outFSSpec, Boolean inRecordIt);

		override	void		DoAESave (FSSpec &inFileSpec, OSType inFileType);
		override	void		DoSave ();
		override	void		DoRevert ();
		override	void		DoPrint ();

	public:
		nonvirtual	void	PurgeUnneededMemory ();

	private:
		nonvirtual	void	DoReadCode ();
#elif	qWindows
	public:
		nonvirtual	LedItServerItem* GetEmbeddedItem ();

	public:
		override	BOOL	OnNewDocument ();
		override	void	Serialize (CArchive& ar);

	public:
		static	FileFormat	sHiddenDocOpenArg;
		override	BOOL	OnOpenDocument (LPCTSTR lpszPathName);

	protected:
		override	COleServerItem*	OnGetEmbeddedItem ();

	public:
		override	BOOL DoSave (LPCTSTR lpszPathName, BOOL bReplace);

	public:
		afx_msg		void	OnUpdateFileSave (CCmdUI* pCmdUI);
		afx_msg		void	OnFileSaveCopyAs ();
		override	void	DeleteContents ();
#endif

	public:
		nonvirtual	TextStore&										GetTextStore ();
		nonvirtual	StandardStyledTextImager::StyleDatabasePtr&		GetStyleDatabase ();
		nonvirtual	WordProcessor::ParagraphDatabasePtr&			GetParagraphDatabase ();
		nonvirtual	WordProcessor::HidableTextDatabasePtr&			GetHidableTextDatabase ();
		nonvirtual	CommandHandler&									GetCommandHandler ();
	protected:
	public:
		ChunkedArrayTextStore						fTextStore;
		RTFInfo										fRTFInfo;
		StandardStyledTextImager::StyleDatabasePtr	fStyleDatabase;
		WordProcessor::ParagraphDatabasePtr			fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr		fHidableTextDatabase;
		MultiLevelUndoCommandHandler				fCommandHandler;

#if		qXWindows
	public:
		nonvirtual	void	LoadFromFile (const string& fileName, FileFormat fileFormat);
		nonvirtual	void	Save ();
#endif


#if		qWindows
	// utilities to pick save/open file names
	public:
		static	bool	DoPromptSaveAsFileName (CString& fileName, FileFormat* fileFormat);
		static	bool	DoPromptSaveCopyAsFileName (CString& fileName, FileFormat* fileFormat);
		static	bool	DoPromptOpenFileName (CString& fileName, FileFormat* fileFormat);
	private:
		static	bool	DoPromptFileName (CString& fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, FileFormat* fileFormat);
#endif

#if		qXWindows
	private:
	public://tmphack to allow easier change of old code...
		string		fPathName;
#endif
	private:
	public:
		FileFormat	fFileFormat;
		HTMLInfo	fHTMLInfo;

#if		qMacOS
	protected:
		nonvirtual	void	DoSaveHelper ();

	public:
		nonvirtual	LedItView*	GetTextView () const { return fTextView; }

	private:
		LedItView*		fTextView;

		nonvirtual	void		NameNewDoc ();
		nonvirtual	void		OpenFile (const FSSpec &inFileSpec);
#endif
#if		qWindows
	#if		qDebug
	public:
		override	void	AssertValid () const;
	#endif

	// Generated message map functions
	protected:
		DECLARE_MESSAGE_MAP()
	
		DECLARE_DISPATCH_MAP()
		DECLARE_INTERFACE_MAP()
#endif
};

Led_SDK_String	ExtractFileSuffix (const Led_SDK_String& from);



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#if		qWindows
	inline	LedItServerItem* LedItDocument::GetEmbeddedItem ()
		{
			return (LedItServerItem*)COleServerDoc::GetEmbeddedItem();
		}
#endif
	inline	TextStore&										LedItDocument::GetTextStore ()
		{
			return fTextStore;
		}
	inline	StandardStyledTextImager::StyleDatabasePtr&	LedItDocument::GetStyleDatabase ()
		{
			return fStyleDatabase;
		}
	inline	WordProcessor::ParagraphDatabasePtr&	LedItDocument::GetParagraphDatabase ()
		{
			return fParagraphDatabase;
		}
	inline	WordProcessor::HidableTextDatabasePtr&	LedItDocument::GetHidableTextDatabase ()
		{
			return fHidableTextDatabase;
		}
	inline	CommandHandler&	LedItDocument::GetCommandHandler ()
		{
			return fCommandHandler;
		}

#endif	/*__LedItDocument_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

