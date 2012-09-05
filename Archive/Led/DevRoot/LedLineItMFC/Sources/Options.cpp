/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItMFC/Sources/Options.cpp,v 2.14 2003/12/31 03:47:45 lewis Exp $
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
 *	Revision 2.14  2003/12/31 03:47:45  lewis
 *	SPR#1580: as small part of implementing combobox support - had to change TextStore::SearchParameters saving to TextInteractor::SearchParameters saving (small change). BIGGER CHANGE: use (mostly new) OptionsFileHelper support to handle options saving.
 *	
 *	Revision 2.13  2003/03/27 00:55:14  lewis
 *	SPR# 1369 - use 'Courier New' instead of 'Courier' since the later is not scalable(truetype)
 *	
 *	Revision 2.12  2003/01/30 21:07:59  lewis
 *	fix reading/writing search string to/from registry for Partial UNICODE case (unicode
 *	led but non-UNICODE Win32 SDK API)
 *	
 *	Revision 2.11  2002/05/06 21:31:21  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.10  2001/11/27 00:28:23  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.9  2001/09/17 14:43:07  lewis
 *	SPR#1032- added SetDefaultFont dialog and saved preferences (pref for new docs)
 *	
 *	Revision 2.8  2001/09/16 18:35:21  lewis
 *	SPR#1033- Added support to check/fix file assocs (depending on user prefs / dialog
 *	choices) to this app (Win32UIFileAssociationRegistrationHelper)
 *	
 *	Revision 2.7  2001/08/30 01:02:01  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2001/07/31 15:52:30  lewis
 *	static_cast<> for warnings
 *	
 *	Revision 2.5  2000/06/08 21:49:49  lewis
 *	SPR#0774- added option to treat TAB as indent-selection command - as MSDEV does
 *	
 *	Revision 2.4  1999/12/25 04:15:52  lewis
 *	Add MENU COMMANDS for the qSupportSyntaxColoring option. And debug it so it worked
 *	properly turning on/off and having multiple windows onto the same doc at the same
 *	time (scrolled to same or differnet) places
 *	
 *	Revision 2.3  1999/12/09 17:35:27  lewis
 *	lots of cleanups/fixes for new (partial/full) UNICODE support - SPR#0645
 *	
 *	Revision 2.2  1999/12/09 03:29:08  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new
 *	Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.1  1999/02/22 13:12:04  lewis
 *	add Options module so we can preserve in registry flags like autoindent
 *	and smartcutandpaste
 *	
 *
 *
 *	<< Based on code from LedItMFC- 990222 >>
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<afxadv.h>

#include	"LedOptionsSupport.h"

#include	"Options.h"



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif




namespace {
	#if		qWindows
		class	MyPrefs : public OptionsFileHelper {
			private:
				typedef	OptionsFileHelper	inherited;
			public:
				MyPrefs ():
					inherited (OpenWithCreateAlongPath (HKEY_CURRENT_USER, _T("Software\\Sophist Solutions, Inc.\\LedLineIt!\\Settings")))
					{
					}
		};
		MyPrefs	gMyPrefsFile;
	#endif
}



typedef	TextInteractor::SearchParameters	SearchParameters;



static	const	TCHAR	kDockBarStateEntry[]					=	_T ("DockBarState");
static	const	TCHAR	kSearchParamsMatchString[]				=	_T ("MatchString");
static	const	TCHAR	kSearchParamsRecentMatchStrings[]		=	_T ("RecentMatchStrings");
static	const	TCHAR	kSearchParamsWrapSearch[]				=	_T ("WrapSearch");
static	const	TCHAR	kSearchParamsWholeWordSearch[]			=	_T ("WholeWordSearch");
static	const	TCHAR	kSearchParamsCaseSensativeSearch[]		=	_T ("CaseSensativeSearch");
static	const	TCHAR	kSmartCutAndPaste[]						=	_T ("SmartCutAndPaste");
static	const	TCHAR	kAutoIndent[]							=	_T ("AutoIndent");
static	const	TCHAR	kTabAutoShiftsText[]					=	_T ("TabAutoShiftsText");
static	const	TCHAR	kSyntaxColoring[]						=	_T ("kSyntaxColoring");
static	const	TCHAR	kCheckFileAssocAtStartup[]				=	_T ("CheckFileAssocAtStartup");
static	const	TCHAR	kDefaultNewDocFont[]					=	_T ("DefaultNewDocFont");




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
	sp.fMatchString = gMyPrefsFile.GetPref (kSearchParamsMatchString, sp.fMatchString);
	sp.fRecentFindStrings = gMyPrefsFile.GetPref (kSearchParamsRecentMatchStrings, sp.fRecentFindStrings);
	sp.fWrapSearch = gMyPrefsFile.GetPref (kSearchParamsWrapSearch, sp.fWrapSearch);
	sp.fWholeWordSearch = gMyPrefsFile.GetPref (kSearchParamsWholeWordSearch, sp.fWholeWordSearch);
	sp.fCaseSensativeSearch = gMyPrefsFile.GetPref (kSearchParamsCaseSensativeSearch, sp.fCaseSensativeSearch);
	return sp;
}

void	Options::SetSearchParameters (const SearchParameters& searchParameters)
{
	gMyPrefsFile.StorePref (kSearchParamsMatchString, searchParameters.fMatchString);
	gMyPrefsFile.StorePref (kSearchParamsRecentMatchStrings, searchParameters.fRecentFindStrings);
	gMyPrefsFile.StorePref (kSearchParamsWrapSearch, searchParameters.fWrapSearch);
	gMyPrefsFile.StorePref (kSearchParamsWholeWordSearch, searchParameters.fWholeWordSearch);
	gMyPrefsFile.StorePref (kSearchParamsCaseSensativeSearch, searchParameters.fCaseSensativeSearch);
}

#if		qWindows
const CDockState&	Options::GetDocBarState () const
{
	static	CDockState	dockState;	// keep static copy and clear each time cuz CDocState doesn't support copy CTOR - LGP971214
	dockState.Clear ();
	vector<Byte>	bytes;
	if (gMyPrefsFile.LookupPref (kDockBarStateEntry, &bytes)) {
		CMemFile file;
		file.Write(&*bytes.begin (), bytes.size ());
		file.SeekToBegin ();
		CArchive ar (&file, CArchive::load);
		dockState.Serialize (ar);
		ar.Close();
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
	ULONG	nSize = static_cast<ULONG> (file.GetLength ());
	ASSERT (nSize < 4096);
	BYTE* p = new BYTE[nSize];
	file.SeekToBegin ();
	file.Read (p, nSize);
	gMyPrefsFile.StorePref (kDockBarStateEntry, nSize, p);
	delete [] p;
}
#endif

bool	Options::GetSmartCutAndPaste () const
{
	return gMyPrefsFile.GetPref (kSmartCutAndPaste, true);
}

void	Options::SetSmartCutAndPaste (bool smartCutAndPaste)
{
	gMyPrefsFile.StorePref (kSmartCutAndPaste, smartCutAndPaste);
}

bool	Options::GetAutoIndent () const
{
	return gMyPrefsFile.GetPref (kAutoIndent, true);
}

void	Options::SetAutoIndent (bool autoIndent)
{
	gMyPrefsFile.StorePref (kAutoIndent, autoIndent);
}

bool	Options::GetTreatTabAsIndentChar () const
{
	return gMyPrefsFile.GetPref (kTabAutoShiftsText, true);
}

void	Options::SetTreatTabAsIndentChar (bool tabAsIndentChar)
{
	gMyPrefsFile.StorePref (kTabAutoShiftsText, tabAsIndentChar);
}

#if		qSupportSyntaxColoring
Options::SyntaxColoringOption	Options::GetSyntaxColoringOption () const
{
	const SyntaxColoringOption kDefault = eSyntaxColoringNone;
	int	result	=	gMyPrefsFile.GetPref<int> (kSyntaxColoring, kDefault);
	if (result >= 1 and result <= 3) {
		return static_cast<Options::SyntaxColoringOption> (result);
	}
	else {
		return kDefault;
	}
}

void	Options::SetSyntaxColoringOption (SyntaxColoringOption syntaxColoringOption)
{
	gMyPrefsFile.StorePref (kSyntaxColoring, syntaxColoringOption);
}
#endif

#if		qWindows
bool	Options::GetCheckFileAssocsAtStartup () const
{
	return gMyPrefsFile.GetPref (kCheckFileAssocAtStartup, true);
}

void	Options::SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup)
{
	gMyPrefsFile.StorePref (kCheckFileAssocAtStartup, checkFileAssocsAtStartup);
}
#endif

Led_FontSpecification	Options::GetDefaultNewDocFont () const
{
	vector<Byte>	bytes;
	if (gMyPrefsFile.LookupPref (kDefaultNewDocFont, &bytes)) {
		if (bytes.size () == sizeof (LOGFONT)) {
			Led_FontSpecification	fsp;
			fsp.SetOSRep (*reinterpret_cast<LOGFONT*> (&*bytes.begin ()));
			return fsp;
		}
	}

	// A good default font for LedLineIt - really just want something monospace,
	// but don't know how better to choose...
	// Not TOO important what we do here. Really we should get/save a user-chosen default in the
	// prefs file!
	Led_FontSpecification	defFont	=	TextImager::GetStaticDefaultFont ();
	defFont.SetFontName (_T ("Courier New"));
	defFont.SetPointSize (10);
	return defFont;
}

void	Options::SetDefaultNewDocFont (const Led_FontSpecification& defaultNewDocFont)
{
	gMyPrefsFile.StorePref (kDefaultNewDocFont, sizeof (LOGFONT), reinterpret_cast<const Byte*> (&defaultNewDocFont.GetOSRep ()));
}


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
