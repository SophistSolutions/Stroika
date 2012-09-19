/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */

#ifndef	__LedItDocument_h__
#define	__LedItDocument_h__	1

#include    "Stroika/Foundation/StroikaPreComp.h"

#if		qMacOS
	#include	<LSingleDoc.h>
#elif	defined (WIN32)
	#include	<afxwin.h>
	#include	<afxole.h>
#endif

#include	"Stroika/Frameworks/Led/Command.h"
#include	"Stroika/Frameworks/Led/ChunkedArrayTextStore.h"
#include	"Stroika/Frameworks/Led/StyledTextImager.h"
#include	"Stroika/Frameworks/Led/StyledTextIO/StyledTextIO.h"
#include	"Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_HTML.h"
#include	"Stroika/Frameworks/Led/StyledTextIO/StyledTextIO_RTF.h"
#include	"Stroika/Frameworks/Led/WordProcessor.h"

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
		StyledTextIO::RTFInfo						fRTFInfo;
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
		FileFormat				fFileFormat;
		StyledTextIO::HTMLInfo	fHTMLInfo;

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

