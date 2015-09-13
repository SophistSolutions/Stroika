/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItView_h__
#define	__LedItView_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItView.h,v 1.32 2004/01/09 18:50:29 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedItView.h,v $
 *	Revision 1.32  2004/01/09 18:50:29  lewis
 *	SPR#1614: since we changed WordProcessor::CalculateFarthestRightMarginInWindow () to call GetLayoutWidth () - we no longer need the override in LedItView
 *	
 *	Revision 1.31  2003/11/06 16:37:57  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.30  2003/11/06 16:08:46  lewis
 *	SPR#1559: change CalculateFarthestRightMarginInWindow to CalculateFarthestRightMargin and return TWIPS.
 *	SPR#1560: lose WordProcessorHScrollbarHelper<> template.
 *	
 *	Revision 1.29  2003/05/05 19:30:29  lewis
 *	SPR#1462: Must override LedItView::OnUpdateCommand () to handle toolbar CMD#s and add them to command# list
 *	
 *	Revision 1.28  2003/04/04 16:26:22  lewis
 *	SPR#1407- getting new command processing code working on X-Windows
 *	
 *	Revision 1.27  2003/04/04 14:42:23  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 1.26  2003/04/03 16:54:11  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated
 *	command classes, just builtin to TextInteractor/WordProcessor (and instead of template params
 *	use new TextInteractor/WordProcessor::DialogSupport etc)
 *	
 *	Revision 1.25  2003/01/22 01:08:23  lewis
 *	SPR#1256 - OtherSize&ParagraphSpacing dialogs moved to LedStdDialogs(untested on Mac).
 *	
 *	Revision 1.24  2003/01/21 16:49:04  lewis
 *	use qSupportParagraphIndentsDlg to decide if we define cmd# in LedViewCmdxxx
 *	
 *	Revision 1.23  2003/01/21 13:25:53  lewis
 *	SPR#1186 - first cut at Indents dialog support
 *	
 *	Revision 1.22  2002/11/19 19:56:36  lewis
 *	SPR#1176 - Added insert row/col before/after commands
 *	
 *	Revision 1.21  2002/10/25 17:57:02  lewis
 *	SPR#1146 - Added 'Insert Symbol' menu item
 *	
 *	Revision 1.20  2002/10/22 00:43:19  lewis
 *	SPR#1136- Add URL support
 *	
 *	Revision 1.19  2002/09/11 04:15:51  lewis
 *	SPR#1094- VERY preliminary Table support - an insert menu and InsertTable command
 *	
 *	Revision 1.18  2002/05/06 21:30:59  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.17  2001/11/27 00:28:09  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.16  2001/09/26 15:41:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.15  2001/09/18 19:13:42  lewis
 *	SPR#0791- added support for toggle (use smart cut&paste, wrap-to-window,show hidden text)
 *	commands for MacOS
 *	
 *	Revision 1.14  2001/08/29 22:59:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.13  2001/08/17 16:29:24  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.12  2001/08/16 18:53:54  lewis
 *	SPR#0959- more work on 'GetInfo' support for embeddings.
 *	
 *	Revision 1.11  2001/07/31 15:45:28  lewis
 *	qTypedefConfusedAccessCheckingCompilerBug
 *	
 *	Revision 1.10  2001/07/19 02:22:38  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support.
 *	
 *	Revision 1.9  2001/07/17 19:08:04  lewis
 *	lose GetSoleSelectedEmbedding () implemenation (now inherited) - and SPR#0959- added
 *	preliminary RightClick-to-show-edit-embedding-properties support
 *	
 *	Revision 1.8  2001/07/11 21:51:21  lewis
 *	SPR#0906- Partly implemented bullet/list support. Just did basic support (bullet lists, no
 *	other style lists). Did support indent levels (including new Increase/Decrease indent level commands).
 *	Still no RTF/HTML support, and a small display bug on following lines when I add bullet attribute.
 *	
 *	Revision 1.7  2001/06/05 15:44:06  lewis
 *	SPR#0950- enable Find dialog for X-Windows/Gtk. Also fixed some small find bugs on Mac/Win that were
 *	introduced by merging of code, and some cleanups enabled by merging
 *	
 *	Revision 1.6  2001/05/22 21:44:32  lewis
 *	MAJOR changes - now supporting checked menu items on X-Windows and enabling (SPR#0924). Also, much
 *	improved dialog code - including dialog for aboutbox, font/color pickers etc (SPR#0923)
 *	
 *	Revision 1.5  2001/05/18 23:00:11  lewis
 *	small cleanups - and gotoWebPage(help menu) support for XWindows
 *	
 *	Revision 1.4  2001/05/18 21:08:30  lewis
 *	About box code cleanups/sharing and support for Linux
 *	
 *	Revision 1.3  2001/05/16 16:03:18  lewis
 *	more massive changes. Make Options () stuff portable (though fake impl om mac/X). Got rid of AppWindow
 *	class - and now X-WIndows fully using LedDoc/LedView. Much more common code between implementations -
 *	but still a lot todo
 *	
 *	Revision 1.2  2001/05/15 16:43:16  lewis
 *	SPR#0920- lots more misc cleanups to share more code across platforms and standardize cmd names etc
 *	
 *	Revision 1.1  2001/05/14 20:54:44  lewis
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
	#include	"Led_PP_WordProcessor.h"
#elif	defined (WIN32)
	#include	"Led_MFC_WordProcessor.h"
#elif	qXWindows
	#include	"Led_Gtk_WordProcessor.h"
#endif


#include	"LedItConfig.h"
#include	"LedItResources.h"






#if		qWindows
class	LedItControlItem;
class	LedItDocument;
#endif
#if		qXWindows
class	LedItDocument;
#endif



#if		qWindows
	#if		qTypedefConfusedAccessCheckingCompilerBug
		class	LedItViewAlmostBASE : public Led_MFC_X <WordProcessor> {
		};
	#else
		typedef	Led_MFC_X <WordProcessor> LedItViewAlmostBASE;
	#endif
#endif

#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (disable : 4250)
#endif
class	LedItView : 
#if		qMacOS
	public WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor> >
#elif	qWindows
	public WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE>
#elif	qXWindows
	public WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor> >
#endif
{
	private:
		#if		qMacOS
			typedef	WordProcessorCommonCommandHelper_PP<Led_PPView_X<WordProcessor> >		inherited;
		#elif	qWindows
			typedef	WordProcessorCommonCommandHelper_MFC <LedItViewAlmostBASE>				inherited;
		#elif	qXWindows
			typedef	WordProcessorCommonCommandHelper_Gtk<Led_Gtk_Helper<WordProcessor> >	inherited;
		#endif

#if		qMacOS
	public:
	 	LedItView ();
#elif	qWindows
	protected: // create from serialization only
		LedItView ();
		DECLARE_DYNCREATE (LedItView)
#elif	qXWindows
	public:
		LedItView (LedItDocument* owningDoc);
#endif

	public:
		virtual ~LedItView ();

#if		qWindows
	protected:
		override	void	OnInitialUpdate ();
#endif

#if		qWindows
	public:
		override	bool	OnUpdateCommand (CommandUpdater* enabler);
#endif

	public:
		nonvirtual	void	SetWrapToWindow (bool wrapToWindow);
	private:
		bool	fWrapToWindow;

	public:
		override	void	GetLayoutMargins (RowReference row, Led_Coordinate* lhs, Led_Coordinate* rhs) const;
		override	void	SetWindowRect (const Led_Rect& windowRect);

#if		qWindows
	public:
		nonvirtual	LedItDocument&		GetDocument () const;
#endif

#if		qWindows
		nonvirtual	LedItControlItem*					GetSoleSelectedOLEEmbedding () const;
#endif

#if		qMacOS
	public:
		override	void	FindCommandStatus (CommandT inCommand, Boolean& outEnabled, Boolean& outUsesMark, UInt16& outMark, Str255 outName);
#endif

#if		qWindows
	public:
		afx_msg		void	OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

	protected:
		override	BOOL	IsSelected (const CObject* pDocItem) const;	// support for CView/OLE
#endif

	public:
		nonvirtual		IncrementalParagraphInfo	GetParaFormatSelection ();
		nonvirtual		void						SetParaFormatSelection (const IncrementalParagraphInfo& pf);

	public:
		override	void	OnShowHideGlyphCommand (CommandNumber cmdNum);

	protected:
		override	SearchParameters	GetSearchParameters () const;
		override	void				SetSearchParameters (const SearchParameters& sp);

	public:
		nonvirtual	void	SetShowHiddenText (bool showHiddenText);

#if		qWindows
	protected:
		afx_msg void	OnSetFocus (CWnd* pOldWnd);
		afx_msg void	OnSize (UINT nType, int cx, int cy);
		afx_msg void	OnInsertObject ();
		afx_msg void	OnCancelEditCntr ();
		afx_msg void	OnCancelEditSrvr ();
		afx_msg	void	OnBarReturn (NMHDR*, LRESULT*);
		DECLARE_MESSAGE_MAP()


	#ifdef _DEBUG
	public:
		override	void	AssertValid () const;
		override	void	Dump (CDumpContext& dc) const;
	#endif
#endif
};
#if		qQuiteAnnoyingDominanceWarnings
	#pragma warning (default : 4250)
#endif







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	#if		!qDebug && qWindows
		inline	LedItDocument&	LedItView::GetDocument () const
	   		{
	   			return *(LedItDocument*)m_pDocument;
	   		}
	#endif

#endif	/*__LedItView_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

