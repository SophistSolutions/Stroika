/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedLineItView_h__
#define	__LedLineItView_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Headers/LedLineItView.h,v 2.32 2004/02/26 03:43:05 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineItView.h,v $
 *	Revision 2.32  2004/02/26 03:43:05  lewis
 *	added qSupportGenRandomCombosCommand hack my dad can more easily generate a bunch of potential namess
 *	
 *	Revision 2.31  2003/09/22 15:55:06  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.30  2003/05/07 18:29:52  lewis
 *	SPR#1408: override SummarizeStyleMarkers (both overloads) to use StyleMarkerSummarySinkForSingleOwner
 *	so we avoid (innocuous) assert in StyleMarkerSummarySink::CombineElements
 *	
 *	Revision 2.29  2003/04/04 14:42:44  lewis
 *	SPR#1407: More work cleaning up new template-free command update/dispatch code
 *	
 *	Revision 2.28  2003/04/03 16:54:26  lewis
 *	SPR#1407: First cut at major change in command-processing classes. Instead of using templated command classes,
 *	just builtin to TextInteractor/WordProcessor (and instead of template params use new TextInteractor/WordProcessor::DialogSupport etc)
 *	
 *	Revision 2.27  2002/12/02 14:22:36  lewis
 *	use struct subclass instead of typedef for non supportsyntaxcoloring case cuz of wierd compile error (not worth debugging)
 *	
 *	Revision 2.26  2002/05/06 21:31:15  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.25  2001/11/27 00:28:20  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.24  2001/09/17 14:43:06  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 2.23  2001/08/30 01:01:59  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.22  2000/10/18 21:00:36  lewis
 *	Lose FindDialog code - and inherit new sahred implm in Led_StdDialogHelper_FindDialog etc
 *	
 *	Revision 2.21  2000/10/04 13:20:21  lewis
 *	lose some code (undo handlers etc) obsoleted by recent code sharing changes
 *	
 *	Revision 2.20  2000/03/28 04:11:54  lewis
 *	fix SynaxColoring code to reacto to changes for SPR#0718
 *	
 *	Revision 2.19  1999/12/25 04:15:51  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it worked
 *	properly turning on/off and having multiple windows onto the same doc at the same
 *	time (scrolled to same or differnet) places
 *	
 *	Revision 2.18  1999/12/24 23:13:19  lewis
 *	Add (conditioanl on qSupportSyntaxColoring) new Sytnax coloring support. Based on old
 *	tutorial code - but greatly cleaned up and integrated into Led
 *	
 *	Revision 2.17  1999/12/21 21:04:34  lewis
 *	adjust about box for UNICODE string. And react to cahnge in API for OnTypedNormalCharacter() method
 *	
 *	Revision 2.16  1999/07/19 19:04:25  lewis
 *	use name LedLineItMFCBaseClass instead of BASECLASS to avoid conflicts with templates
 *	in Led_MFC - under rare circumstances - probably compiler bug - but no matter - better name
 *	
 *	Revision 2.15  1999/03/01 15:30:52  lewis
 *	misc cleanups and use new SetHScrollbarType ETC instead of setting WS_VSCROLL stypes in PreCreateWindow
 *	
 *	Revision 2.14  1999/02/12 14:09:06  lewis
 *	cleanup
 *	
 *	Revision 2.13  1998/10/30 15:06:27  lewis
 *	new msvc60 warning code, and use mutable, rather than const cast.
 *	
 *	Revision 2.12  1998/03/04  20:29:14  lewis
 *	DIdUpdateText() override
 *
 *	Revision 2.11  1997/12/24  04:50:02  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/12/24  04:26:58  lewis
 *	*** empty log message ***
 *
 *	Revision 2.9  1997/07/27  16:02:50  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.8  1997/07/23  23:18:40  lewis
 *	OnSize() override not needed
 *
 *	Revision 2.7  1997/06/28  17:33:50  lewis
 *	Add OnInitMenuPopup () override to delete disabled commands from context menu.
 *
 *	Revision 2.6  1997/06/24  03:45:43  lewis
 *	Lose DynamiclyAdjustLayoutWidth and now override ComputeMaxHScrollPos as part
 *	of spr#0450.
 *
 *	Revision 2.5  1997/06/23  16:37:00  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1997/06/18  03:45:35  lewis
 *	Support shiftleft and shiftright
 *
 *	Revision 2.3  1997/03/23  01:06:59  lewis
 *	DynamiclyAdjustLayoutWidth () and lose DidUpdateText override
 *
 *	Revision 2.2  1997/03/04  20:18:06  lewis
 *	auto-indnet, new commands, context menu, goto line support etc.
 *
 *	Revision 2.1  1997/01/20  05:39:16  lewis
 *	Delete font style support, and added support for fixing up hscroll when we vscroll, and
 *	setting tab width to 4 characters.
 *
 *	Revision 2.0  1997/01/10  03:35:47  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 *
 *
 *
 */

#include	"Led_MFC.h"
#include	"SimpleTextInteractor.h"

#include	"LedLineItConfig.h"
#include	"SyntaxColoring.h"

#include	"Resource.h"


#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (push)
	#pragma	warning (disable : 4250)
#endif




#if		qSupportSyntaxColoring
	struct	LedLineItMFCBaseClass : public Led_MFC_X<SimpleTextInteractor>, public StyledTextImager {
		protected:
			override	Led_Distance	MeasureSegmentHeight (size_t from, size_t to) const
				{
					return Led_MFC_X<SimpleTextInteractor>::MeasureSegmentHeight (from, to);
				}
			override	Led_Distance	MeasureSegmentBaseLine (size_t from, size_t to) const
				{
					return Led_MFC_X<SimpleTextInteractor>::MeasureSegmentBaseLine (from, to);
				}
	};
#else
	struct	LedLineItMFCBaseClass : public Led_MFC_X<SimpleTextInteractor> {
	};
#endif









class	LedLineItDocument;

class	LedLineItView : public LedLineItMFCBaseClass {
	private:
		typedef	LedLineItMFCBaseClass	inherited;

	protected: // create from serialization only
		LedLineItView ();
		DECLARE_DYNCREATE(LedLineItView)

	public:
		virtual ~LedLineItView ();

	protected:
		override	void	OnInitialUpdate ();

#if		qSupportSyntaxColoring
	public:
		nonvirtual	void	ResetSyntaxColoringTable ();
	protected:
		override	void	HookLosingTextStore ();
		override	void	HookGainedNewTextStore ();
#endif


#if		qSupportSyntaxColoring
	protected:
		override	vector<RunElement>	SummarizeStyleMarkers (size_t from, size_t to) const;
		override	vector<RunElement>	SummarizeStyleMarkers (size_t from, size_t to, const TextLayoutBlock& text) const;
#endif

	public:
		nonvirtual	size_t	GetCurUserLine () const;
		nonvirtual	void	SetCurUserLine (size_t newCurLine);	// OK if bad line given

	public:
		override	void	UpdateScrollBars ();

	public:
		override	Led_Distance	ComputeMaxHScrollPos () const;
	private:
		mutable	Led_Distance	fCachedLayoutWidth;

	public:
		override	void	OnTypedNormalCharacter (Led_tChar theChar, bool optionPressed, bool shiftPressed, bool commandPressed, bool controlPressed, bool altKeyPressed);

	protected:
		override	const TabStopList&	GetTabStopList (size_t /*containingPos*/) const;
	protected:
		override	void	TabletChangedMetrics ();
	private:
		SimpleTabStopList	fTabStopList;

	public:
		override	void	DidUpdateText (const UpdateInfo& updateInfo) throw ();

	public:
		nonvirtual	LedLineItDocument&		GetDocument () const;

	public:
		afx_msg		void	OnContextMenu (CWnd* /*pWnd*/, CPoint /*point*/);

	protected:
		override	BOOL	IsSelected (const CObject* pDocItem) const;	// support for CView/OLE

	public:
		afx_msg	void	OnUpdateFontNameChangeCommand (CCmdUI* pCmdUI);
		afx_msg	void	OnFontNameChangeCommand (UINT cmdNum);
	private:
		Led_Distance	PickOtherFontHeight (Led_Distance origHeight);

	public:
		afx_msg		void	OnUpdateFontSizeChangeCommand (CCmdUI* pCmdUI);
		afx_msg		void	OnFontSizeChangeCommand (UINT cmdNum);

	public:
		afx_msg		void	OnGotoLineCommand ();
		afx_msg		void	OnShiftLeftCommand ();
		afx_msg		void	OnShiftRightCommand ();
		nonvirtual	void	OnShiftNCommand (bool shiftRight);

	#if		qSupportGenRandomCombosCommand
	public:
		afx_msg		void	OnGenRandomCombosCommand ();
	#endif

	protected:
		override	SearchParameters	GetSearchParameters () const;
		override	void				SetSearchParameters (const SearchParameters& sp);

	protected:
		afx_msg void	OnSetFocus (CWnd* pOldWnd);
		afx_msg void	OnCancelEditCntr ();
		afx_msg void	OnCancelEditSrvr ();
		afx_msg	void	OnSelectAllCommand ();
		afx_msg	void	OnChooseFontCommand ();
		DECLARE_MESSAGE_MAP()	

#if		qSupportSyntaxColoring
	private:
		#if		qSupportOnlyMarkersWhichOverlapVisibleRegion
			WindowedSyntaxColoringMarkerOwner*	fSyntaxColoringMarkerOwner;
		#else
			SimpleSyntaxColoringMarkerOwner*	fSyntaxColoringMarkerOwner;
		#endif
#endif

	#ifdef _DEBUG
	public:
		override	void	AssertValid () const;
		override	void	Dump (CDumpContext& dc) const;
	#endif
};






class	FontDlgWithNoColorNoStyles : public CFontDialog {
	public:
		FontDlgWithNoColorNoStyles (LOGFONT* lf);
		override	BOOL	OnInitDialog ();
};







/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
	#if		!qDebug
		inline	LedLineItDocument&	LedLineItView::GetDocument () const
	   		{
	   			return *(LedLineItDocument*)m_pDocument;
	   		}
	#endif


#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (pop)
#endif

#endif	/*__LedLineItView_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

