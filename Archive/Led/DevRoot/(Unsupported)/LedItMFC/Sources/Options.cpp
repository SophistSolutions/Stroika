/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Sources/Options.cpp,v 1.7 2000/03/17 22:40:14 lewis Exp $
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
 *	$Log: Options.cpp,v $
 *	Revision 1.7  2000/03/17 22:40:14  lewis
 *	SPR#0717- preliminary support - for showhiddentext testing and UI
 *	
 *	Revision 1.6  1999/12/28 17:19:01  lewis
 *	save prefrences for ShowPara/Tab/SpaceGlyphs.
 *	
 *	Revision 1.5  1999/12/09 17:29:21  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 1.4  1999/02/08 22:31:37  lewis
 *	Support new optional 'wrap-to-window' feature. Used to be we ALWAYS did that. Now can be on/off. Next todo is to fix the OFF case to properly use ruler
 *	
 *	Revision 1.3  1998/10/30 14:56:05  lewis
 *	mosstly new msvc60 pragma warnign changes
 *	
 *	Revision 1.2  1997/12/24  04:47:51  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1997/12/24  04:17:04  lewis
 *	Initial revision
 *
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxadv.h>

#include	"Options.h"



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif



typedef	TextStore::SearchParameters	SearchParameters;



static	const	TCHAR kSection[]							=	_T ("Settings");
static	const	TCHAR kDockBarStateEntry[]					=	_T ("DockBarState");
static	const	TCHAR kSearchParamsMatchString[]			=	_T ("MatchString");
static	const	TCHAR kSearchParamsWrapSearch[]				=	_T ("WrapSearch");
static	const	TCHAR kSearchParamsWholeWordSearch[]		=	_T ("WholeWordSearch");
static	const	TCHAR kSearchParamsCaseSensativeSearch[]	=	_T ("CaseSensativeSearch");
static	const	TCHAR kSmartCutAndPaste[]					=	_T ("SmartCutAndPaste");
static	const	TCHAR kWrapToWindow[]						=	_T ("WrapToWindow");
static	const	TCHAR kShowHiddenText[]						=	_T ("ShowHiddenText");
static	const	TCHAR kShowParagraphGlyphs[]				=	_T ("ShowParagraphGlyphs");
static	const	TCHAR kShowTabGlyphs[]						=	_T ("ShowTabGlyphs");
static	const	TCHAR kShowSpaceGlyphs[]					=	_T ("ShowSpaceGlyphs");





/*
 ********************************************************************************
 *********************************** Options ************************************
 ********************************************************************************
 */

Options::Options ()
{
}

Options::~Options ()
{
}

SearchParameters	Options::GetSearchParameters () const
{
	SearchParameters	sp;
	sp.fMatchString = Led_SDKString2tString ((LPCTSTR)AfxGetApp ()->GetProfileString (kSection, kSearchParamsMatchString));
	sp.fWrapSearch = AfxGetApp ()->GetProfileInt (kSection, kSearchParamsWrapSearch, sp.fWrapSearch);
	sp.fWholeWordSearch = AfxGetApp ()->GetProfileInt (kSection, kSearchParamsWholeWordSearch, sp.fWholeWordSearch);
	sp.fCaseSensativeSearch = AfxGetApp ()->GetProfileInt (kSection, kSearchParamsCaseSensativeSearch, sp.fCaseSensativeSearch);
	return sp;
}

void	Options::SetSearchParameters (const SearchParameters& searchParameters)
{
	AfxGetApp ()->WriteProfileString (kSection, kSearchParamsMatchString, Led_tString2SDKString (searchParameters.fMatchString).c_str ());
	AfxGetApp ()->WriteProfileInt (kSection, kSearchParamsWrapSearch, searchParameters.fWrapSearch);
	AfxGetApp ()->WriteProfileInt (kSection, kSearchParamsWholeWordSearch, searchParameters.fWholeWordSearch);
	AfxGetApp ()->WriteProfileInt (kSection, kSearchParamsCaseSensativeSearch, searchParameters.fCaseSensativeSearch);
}

const CDockState&	Options::GetDocBarState () const
{
	static	CDockState	dockState;	// keep static copy and clear each time cuz CDocState doesn't support copy CTOR - LGP971214
	dockState.Clear ();
	BYTE* p;
	UINT nLen = 0;
	if (AfxGetApp ()->GetProfileBinary (kSection, kDockBarStateEntry, &p, &nLen)) {
		ASSERT(nLen < 4096);
		CMemFile file;
		file.Write(p, nLen);
		file.SeekToBegin();
		CArchive ar(&file, CArchive::load);
		dockState.Serialize (ar);
		ar.Close();
		delete p;
	}
	return dockState;
}

void	Options::SetDocBarState (const CDockState& dockState)
{
	CMemFile file;
	CArchive ar (&file, CArchive::store);
	CDockState&	ds	=	const_cast<CDockState&> (dockState);	// Serialize/Write shouldn't change object!
	ds.Serialize (ar);
	ar.Close ();
	int nSize = file.GetLength ();
	ASSERT (nSize < 4096);
	BYTE* p = new BYTE[nSize];
	file.SeekToBegin ();
	file.Read (p, nSize);
	AfxGetApp ()->WriteProfileBinary (kSection, kDockBarStateEntry, p, nSize);
	delete [] p;
}

bool	Options::GetSmartCutAndPaste () const
{
	return AfxGetApp ()->GetProfileInt (kSection, kSmartCutAndPaste, true);
}

void	Options::SetSmartCutAndPaste (bool smartCutAndPaste)
{
	AfxGetApp ()->WriteProfileInt (kSection, kSmartCutAndPaste, smartCutAndPaste);
}

bool	Options::GetWrapToWindow () const
{
	return AfxGetApp ()->GetProfileInt (kSection, kWrapToWindow, true);
}

void	Options::SetWrapToWindow (bool wrapToWindow)
{
	AfxGetApp ()->WriteProfileInt (kSection, kWrapToWindow, wrapToWindow);
}

bool	Options::GetShowHiddenText () const
{
	return AfxGetApp ()->GetProfileInt (kSection, kShowHiddenText, true);
}

void	Options::SetShowHiddenText (bool showHiddenText)
{
	AfxGetApp ()->WriteProfileInt (kSection, kShowHiddenText, showHiddenText);
}

bool	Options::GetShowParagraphGlyphs () const
{
	return AfxGetApp ()->GetProfileInt (kSection, kShowParagraphGlyphs, false);
}

void	Options::SetShowParagraphGlyphs (bool showParagraphGlyphs)
{
	AfxGetApp ()->WriteProfileInt (kSection, kShowParagraphGlyphs, showParagraphGlyphs);
}

bool	Options::GetShowTabGlyphs () const
{
	return AfxGetApp ()->GetProfileInt (kSection, kShowTabGlyphs, false);
}

void	Options::SetShowTabGlyphs (bool showTabGlyphs)
{
	AfxGetApp ()->WriteProfileInt (kSection, kShowTabGlyphs, showTabGlyphs);
}

bool	Options::GetShowSpaceGlyphs () const
{
	return AfxGetApp ()->GetProfileInt (kSection, kShowSpaceGlyphs, false);
}

void	Options::SetShowSpaceGlyphs (bool showSpaceGlyphs)
{
	AfxGetApp ()->WriteProfileInt (kSection, kShowSpaceGlyphs, showSpaceGlyphs);
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
