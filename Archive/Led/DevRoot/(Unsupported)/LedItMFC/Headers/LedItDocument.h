/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItDocument_h__
#define	__LedItDocument_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItDocument.h,v 2.19 2000/03/31 00:51:37 lewis Exp $
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
 *	Revision 2.19  2000/03/31 00:51:37  lewis
 *	SPR#0717 - more changes to adapt to new HiddenText support. Rip out some of the code I had added to LedItView - and instead keep a HiddenTextDatabase with the document (like the paragraphdatabase). Now we support REAADING RTF files with hidden text. Next todo is writing RTF with hidden text
 *	
 *	Revision 2.18  1999/12/19 15:09:27  lewis
 *	broken StyledTextIO into parts- SPR#0662
 *	
 *	Revision 2.17  1999/03/08 23:17:00  lewis
 *	Keep track of RTFInfo object
 *	
 *	Revision 2.16  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.15  1997/09/29  15:57:45  lewis
 *	New StandardStyledTextImager::StyleDatabasePtr and WordProcessor::ParagraphDatabasePtr
 *	support.
 *
 *	Revision 2.14  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.13  1997/07/14  14:49:00  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.12  1997/01/10  03:28:40  lewis
 *	throw specifiers
 *
 *	Revision 2.11  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.10  1996/12/05  21:16:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1996/10/31  00:30:59  lewis
 *	Add fHTMLInfo member so we can keep track of hidden HTML stuff read, for later writting.
 *
 *	Revision 2.8  1996/09/30  14:54:25  lewis
 *	Move Led_FileFOrmat to LedItConfig.hh
 *
 *	Revision 2.7  1996/09/03  15:11:03  lewis
 *	Add enum for eRTFFormat.
 *
 *	Revision 2.6  1996/09/01  15:43:38  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.5  1996/07/03  01:51:14  lewis
 *	Add OnUpdateFileSave() method, so we can disable file save command when saved.
 *
 *	Revision 2.4  1996/06/01  02:35:02  lewis
 *	Cleanups.
 *	Redo CTemplate stuff - and redo open file dialog/save file dialog code.
 *	subclass from MarkerOwner so we get didupdate hook to update dirty flag.
 *
 *	Revision 2.3  1996/05/05  14:40:03  lewis
 *	Code cleanups and OnFileSaveCopyAs ().
 *
 *	Revision 2.2  1996/03/16  19:08:09  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1996/01/22  05:37:24  lewis
 *	Support UNDO/CommandHandler.
 *
 *	Revision 2.0  1996/01/04  00:55:44  lewis
 *	*** empty log message ***
 *
 *	Revision 1.3  1996/01/04  00:55:22  lewis
 *	*** empty log message ***
 *
 *	Revision 1.2  1995/12/08  07:46:23  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	<afxwin.h>
#include	<afxole.h>

#include	"Command.h"
#include	"ChunkedArrayTextStore.h"
#include	"StyledTextImager.h"
#include	"StyledTextIO.h"
#include	"StyledTextIO_HTML.h"
#include	"StyledTextIO_RTF.h"
#include	"WordProcessor.h"

#include	"LedItConfig.h"







class LedItServerItem;

class	LedItDocument :	public COleServerDoc, public MarkerOwner {
	protected: // create from serialization only
		LedItDocument();
		DECLARE_DYNCREATE(LedItDocument)
	
	public:
		virtual ~LedItDocument();

	public:
		override	void		DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ();
		override	TextStore*	PeekAtTextStore () const;

	public:
		nonvirtual	LedItServerItem* GetEmbeddedItem ();

	public:
		override	BOOL	OnNewDocument ();
		override	void	Serialize (CArchive& ar);

	public:
		static	Led_FileFormat	sHiddenDocOpenArg;
		override	BOOL	OnOpenDocument (LPCTSTR lpszPathName);

	protected:
		override	COleServerItem*	OnGetEmbeddedItem ();

	public:
		override	BOOL DoSave (LPCTSTR lpszPathName, BOOL bReplace);

	public:
		afx_msg		void	OnEditClearAll ();
		afx_msg		void	OnUpdateFileSave (CCmdUI* pCmdUI);
		afx_msg		void	OnFileSaveCopyAs ();
		override	void	DeleteContents ();

	public:
		nonvirtual	TextStore&										GetTextStore ();
		nonvirtual	StandardStyledTextImager::StyleDatabasePtr&		GetStyleDatabase ();
		nonvirtual	WordProcessor::ParagraphDatabasePtr&			GetParagraphDatabase ();
		nonvirtual	WordProcessor::HidableTextDatabasePtr&			GetHidableTextDatabase ();
		nonvirtual	CommandHandler&									GetCommandHandler ();
	protected:
		ChunkedArrayTextStore						fTextStore;
		RTFInfo										fRTFInfo;
		StandardStyledTextImager::StyleDatabasePtr	fStyleDatabase;
		WordProcessor::ParagraphDatabasePtr			fParagraphDatabase;
		WordProcessor::HidableTextDatabasePtr		fHidableTextDatabase;
		MultiLevelUndoCommandHandler				fCommandHandler;


	// utilities to pick save/open file names
	public:
		static	bool	DoPromptSaveAsFileName (CString& fileName, Led_FileFormat* fileFormat);
		static	bool	DoPromptSaveCopyAsFileName (CString& fileName, Led_FileFormat* fileFormat);
		static	bool	DoPromptOpenFileName (CString& fileName, Led_FileFormat* fileFormat);
	private:
		static	bool	DoPromptFileName (CString& fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, Led_FileFormat* fileFormat);

	private:
		Led_FileFormat	fFileFormat;
		HTMLInfo		fHTMLInfo;


	#if		qDebug
	public:
		override	void	AssertValid () const;
	#endif

	// Generated message map functions
	protected:
		DECLARE_MESSAGE_MAP()
	
		DECLARE_DISPATCH_MAP()
		DECLARE_INTERFACE_MAP()
};



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	inline	LedItServerItem* LedItDocument::GetEmbeddedItem ()
		{
			return (LedItServerItem*)COleServerDoc::GetEmbeddedItem();
		}
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

