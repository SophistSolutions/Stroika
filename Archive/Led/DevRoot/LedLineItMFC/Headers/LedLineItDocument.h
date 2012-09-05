/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItDocument_h__
#define	__LedLineItDocument_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItDocument.h,v 2.16 2003/12/11 03:09:13 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItDocument.h,v $
 *	Revision 2.16  2003/12/11 03:09:13  lewis
 *	SPR#1585: move spellcheckengine object to application. Use qIncludeBasicSpellcheckEngine define to control its inclusion. Define default locaiton for LedLineIt spell dict.
 *	
 *	Revision 2.15  2003/09/22 15:56:57  lewis
 *	SPR#1552- override  OnOpenDocument() to replace MFC's buggy implemeantion with our
 *	own (hopefully less buggy version)
 *	
 *	Revision 2.14  2003/06/03 12:31:12  lewis
 *	SPR#1513: hook in spellcheck engine
 *	
 *	Revision 2.13  2003/04/10 19:35:55  lewis
 *	lose unused OnEditClearAll method
 *	
 *	Revision 2.12  2003/04/01 16:16:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.11  2002/05/06 21:31:14  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.10  2001/11/27 00:28:20  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.9  2001/09/15 13:29:52  lewis
 *	check if the default code page is available and pick new default if UTF8 isnt available
 *	
 *	Revision 2.8  2001/09/13 02:43:43  lewis
 *	change default encoding from UTF7 to UTF8 (cuz more widely supported, and more compact)
 *	
 *	Revision 2.7  2001/09/09 22:36:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2001/09/09 22:31:55  lewis
 *	SPR#0994, 0697- Added fCodePage flag to the LedLineItDocument class, and did alot
 *	of work reworking the calling of the CFileDialog so it could use my new replacement
 *	version (which includes an Encoding popup). Pretty ugly code for passing args through
 *	the MFC layers (like codepage from dialog to Serialize method). Most of the REAL guts
 *	code is in the CodePage module, and the stuff here is very MFC-specific.
 *	
 *	Revision 2.5  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1999/02/06 14:13:08  lewis
 *	lose unneeded OpenDocument override
 *	
 *	Revision 2.3  1997/12/24 04:50:02  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.1  1997/07/14  14:59:03  lewis
 *	Renamed TextStore_ to TextStore.
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#include	<afxwin.h>
#include	<afxole.h>

#include	"CodePage.h"
#include	"Command.h"
#include	"ChunkedArrayTextStore.h"

#include	"LedLineItConfig.h"



const	CodePage	kDefaultNewDocCodePage	=	CodePagesInstalled::IsCodePageAvailable (kCodePage_UTF8)? kCodePage_UTF8: CodePagesInstalled::GetDefaultCodePage ();


const	CodePage	kAutomaticallyGuessCodePage	=	-2;	// I HOPE this # doesn't conflict with any legit ones!!!
const	CodePage	kIGNORECodePage				=	-3;	// I HOPE this # doesn't conflict with any legit ones!!!


class	LedLineItServerItem;

class	LedLineItDocument :	public COleServerDoc, public MarkerOwner {
	protected: // create from serialization only
		LedLineItDocument ();
		DECLARE_DYNCREATE(LedLineItDocument)
	
	public:
		virtual ~LedLineItDocument();

	public:
		override	void		DidUpdateText (const UpdateInfo& /*updateInfo*/) throw ();
		override	TextStore*	PeekAtTextStore () const;

	public:
		nonvirtual	LedLineItServerItem* GetEmbeddedItem ();

	public:
		override	BOOL	OnNewDocument ();
		override	BOOL	OnOpenDocument (LPCTSTR lpszPathName);
		override	void	Serialize (CArchive& ar);

	protected:
		override	COleServerItem*	OnGetEmbeddedItem ();

	public:
		override	BOOL DoSave (LPCTSTR lpszPathName, BOOL bReplace);


	public:
		static	CodePage	LedLineItDocument::sHiddenDocOpenArg;

	public:
		afx_msg		void	OnUpdateFileSave (CCmdUI* pCmdUI);
		afx_msg		void	OnFileSaveCopyAs ();
		override	void	DeleteContents ();

	public:
		nonvirtual	TextStore&			GetTextStore ();
		nonvirtual	CommandHandler&		GetCommandHandler ();
	protected:
		ChunkedArrayTextStore			fTextStore;
		MultiLevelUndoCommandHandler	fCommandHandler;

	private:
		CodePage	fCodePage;


	// utilities to pick save/open file names
	public:
		static	bool	DoPromptSaveAsFileName (CString* fileName, CodePage* codePage);
		static	bool	DoPromptSaveCopyAsFileName (CString* fileName, CodePage* codePage);
		static	bool	DoPromptOpenFileName (CString* fileName, CodePage* codePage);
	private:
		static	bool	DoPromptFileName (CString* fileName, UINT nIDSTitle, bool isOpenDialogCall, long fileDLogFlags, CodePage* codePage);

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
	inline	LedLineItServerItem* LedLineItDocument::GetEmbeddedItem ()
		{
			return (LedLineItServerItem*)COleServerDoc::GetEmbeddedItem();
		}
	inline	TextStore&			LedLineItDocument::GetTextStore ()
		{
			return fTextStore;
		}
	inline	CommandHandler&	LedLineItDocument::GetCommandHandler ()
		{
			return fCommandHandler;
		}

#endif	/*__LedLineItDocument_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

