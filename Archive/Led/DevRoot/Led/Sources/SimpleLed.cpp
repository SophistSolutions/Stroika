/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/SimpleLed.cpp,v 2.7 2003/02/28 19:05:58 lewis Exp $
 *
 * Changes:
 *	$Log: SimpleLed.cpp,v $
 *	Revision 2.7  2003/02/28 19:05:58  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.6  2002/09/19 16:47:44  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.5  2002/09/19 12:55:05  lewis
 *	SPR#1099- fix args to OnCreate_Msg() to be JSUT the LPCREATESTRUCT. Override OnCreate()
 *	in MFCHELPER wrapper. Use OnCreate () msg hook to check incoming default window style -
 *	and to set the default value for SetScrollBarType () accordingly.
 *	
 *	Revision 2.4  2002/09/19 01:58:53  lewis
 *	SPR#1103- qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR workaround AND
 *	added _WIN32 support AND added WP HSCROLLER code and command-handler.
 *	
 *	Revision 2.3  2002/05/06 21:33:49  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.2  2001/11/27 00:29:55  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.1  2001/10/16 15:36:28  lewis
 *	SPR#1062- Renamed Led.h/cpp to SimpleLed.h/cpp. Renamed SimpleLed
 *	to SimpleLedWordProcessor. Added new SimpleLedLineEditor.
 *	
 *
 *
 *	<<<< BASED ON Led.cpp module - just renamed to SimpleLed.cpp -- LGP 2001-10-16 >>>>
 *
 *
 *	Revision 2.20  2001/08/28 18:43:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.19  2001/06/27 13:24:47  lewis
 *	qSilenceAnnoyingCompilerWarnings
 *	
 *	Revision 2.18  2000/07/25 14:10:48  lewis
 *	rename Led class to SimpleLed so as to not conflict with Led namespace -
 *	and adjusted Led tutorial accordinly
 *	
 *	Revision 2.17  2000/04/14 22:40:24  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.16  1999/11/13 16:32:21  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.15  1999/05/03 22:05:07  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.14  1999/02/23 16:03:32  lewis
 *	fix Led::GetLayoutMargins () override
 *	
 *	Revision 2.13  1998/10/30 14:29:28  lewis
 *	tmphack support for GetLayoutMargins instead of GetLayoutWidth
 *	
 *	Revision 2.12  1998/07/24  01:06:59  lewis
 *	*** empty log message ***
 *
 *	Revision 2.11  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.10  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.9  1997/07/23  23:08:01  lewis
 *	Fixed for Led 2.2.
 *	Support PowerPlant.
 *	Desupport TCL.
 *
 *	Revision 2.8  1997/06/18  20:08:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.7  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.6  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.5  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.4  1996/05/14  20:25:27  lewis
 *	fixed use of __MFC_VER - sb _MFC_VER.
 *
 *	Revision 2.3  1996/02/26  18:55:00  lewis
 *	Lots of changes. Lose all the version number string crap. And now
 *	we own a text store - always. These aren't really deisnged to be
 *	subclassed.
 *
 *	Revision 2.2  1996/02/05  04:17:48  lewis
 *	new owned text store code handled in class Led (autogen of textstore).
 *	For other led classes, you must supply the texstore.
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.7  1995/06/19  13:48:34  lewis
 *	Fix display of non-zero version substage for release buil.ds
 *
 *	Revision 1.6  1995/06/04  04:26:23  lewis
 *	Support qLed_Version_Stage_ReleaseCandidate - and comment back in assert
 *	that version substage == 0 for released version - SPR#0310
 *
 *	Revision 1.5  1995/06/02  07:39:22  lewis
 *	Comment out assert til I decide how to deal with release candidates.
 *
 *	Revision 1.4  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only
 *	relevant to emacs users. SPR 0301.
 *
 *	Revision 1.3  1995/03/03  15:52:54  lewis
 *	New support for computing kVersion string from #defines now kept in
 *	LedConfig.hh
 *
 *	Revision 1.2  1995/03/02  05:46:10  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:46  lewis
 *	Initial revision
 *
 *
 *	<<<<From Led_DialogText - merged in here 960215>>>
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.2  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'.
 *	Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.1  1995/05/21  17:09:01  lewis
 *	Initial revision
 *	<<<<End Led_DialogText - merged in here 960215>>>
 *
 */

#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<stdlib.h>
#include	<string.h>

#include	"SimpleLed.h"




#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4786)		//qQuiteAnnoyingDebugSymbolTruncationWarnings
#endif



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	using	namespace	Led;
#endif







/*
 ********************************************************************************
 ***************************** SimpleLedWordProcessor ***************************
 ********************************************************************************
 */
SimpleLedWordProcessor::SimpleLedWordProcessor ():
	inherited (),
	fCommandHandler (kMaxUndoLevels),
	fTextStore ()
{
	#if		!qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
		SpecifyTextStore (&fTextStore);
		SetCommandHandler (&fCommandHandler);
	#endif
}

SimpleLedWordProcessor::~SimpleLedWordProcessor ()
{
	#if		!qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
		SetCommandHandler (NULL);
		SpecifyTextStore (NULL);
	#endif
}

#if		qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
#if		defined (_MFC_VER)
void	SimpleLedWordProcessor::OnInitialUpdate()
{
	SpecifyTextStore (&fTextStore);
	SetCommandHandler (&fCommandHandler);
	inherited::OnInitialUpdate ();
}

void	SimpleLedWordProcessor::PostNcDestroy ()
{
	SetCommandHandler (NULL);
	SpecifyTextStore (NULL);
	inherited::PostNcDestroy ();
}
#elif	defined (_WIN32)
LRESULT	SimpleLedWordProcessor::OnCreate_Msg (LPCREATESTRUCT createStruct)
{
	SpecifyTextStore (&fTextStore);
	SetCommandHandler (&fCommandHandler);
	return inherited::OnCreate_Msg (createStruct);
}

void	SimpleLedWordProcessor::OnNCDestroy_Msg ()
{
	SetCommandHandler (NULL);
	SpecifyTextStore (NULL);
	inherited::OnNCDestroy_Msg ();
}
#endif
#endif


#if		defined (_MFC_VER)
	IMPLEMENT_DYNCREATE		(SimpleLedWordProcessor,		CView)
	BEGIN_MESSAGE_MAP		(SimpleLedWordProcessor,		SimpleLedWordProcessor::inherited)
	END_MESSAGE_MAP()
#endif












/*
 ********************************************************************************
 ******************************** SimpleLedLineEditor ***************************
 ********************************************************************************
 */
SimpleLedLineEditor::SimpleLedLineEditor ():
	inherited (),
	fCommandHandler (kMaxUndoLevels),
	fTextStore ()
{
	#if		!qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
		SpecifyTextStore (&fTextStore);
		SetCommandHandler (&fCommandHandler);
		SetScrollBarType (h, eScrollBarAlways);
		SetScrollBarType (v, eScrollBarAlways);
	#endif
}

SimpleLedLineEditor::~SimpleLedLineEditor ()
{
	#if		!qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
		SpecifyTextStore (NULL);
	#endif
}

#if		qCannotSafelyCallLotsOfComplexVirtMethodCallsInsideCTORDTOR
#if		defined (_MFC_VER)
void	SimpleLedLineEditor::OnInitialUpdate()
{
	SpecifyTextStore (&fTextStore);
	SetCommandHandler (&fCommandHandler);
	inherited::OnInitialUpdate ();
}

void	SimpleLedLineEditor::PostNcDestroy ()
{
	SetCommandHandler (NULL);
	SpecifyTextStore (NULL);
	inherited::PostNcDestroy ();
}
#elif	defined (_WIN32)
LRESULT	SimpleLedLineEditor::OnCreate_Msg (LPCREATESTRUCT createStruct)
{
	SpecifyTextStore (&fTextStore);
	SetCommandHandler (&fCommandHandler);
	return inherited::OnCreate_Msg (createStruct);
}

void	SimpleLedLineEditor::OnNCDestroy_Msg ()
{
	SetCommandHandler (NULL);
	SpecifyTextStore (NULL);
	inherited::OnNCDestroy_Msg ();
}
#endif
#endif

#if		defined (_MFC_VER)
	IMPLEMENT_DYNCREATE		(SimpleLedLineEditor,		CView)
	BEGIN_MESSAGE_MAP		(SimpleLedLineEditor,		SimpleLedLineEditor::inherited)
	END_MESSAGE_MAP()
#endif










/*
 ********************************************************************************
 ********************************* LedDialogText ********************************
 ********************************************************************************
 */

LedDialogText::LedDialogText ():
	inherited ()
{
}


#if		qWindows && defined (_MFC_VER)
void	LedDialogText::PostNcDestroy ()
{
	// Don't auto-delete ourselves!
	CWnd::PostNcDestroy ();
}
int		LedDialogText::OnMouseActivate (CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// Don't do CView::OnMouseActiveate() cuz that assumes our parent is a frame window,
	// and tries to make us the current view in the document...
	int nResult = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	return nResult;
}
#endif



#if		qWindows && defined (_MFC_VER)
IMPLEMENT_DYNCREATE		(LedDialogText,						CView)
BEGIN_MESSAGE_MAP		(LedDialogText,						LedDialogText::inherited)
	ON_WM_MOUSEACTIVATE		()
END_MESSAGE_MAP()
#endif





// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
