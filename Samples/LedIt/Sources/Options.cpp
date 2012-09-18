/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */

#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#if		defined (WIN32)
	#include	<afxadv.h>
#endif

#include	"Stroika/Frameworks/Led/OptionsSupport.h"

#include	"Options.h"



#if		qSilenceAnnoyingCompilerWarnings && _MSC_VER
	#pragma	warning (4 : 4800)		//qUsePragmaWarningToSilenceNeedlessBoolConversionWarnings
#endif


#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




namespace {
	#if		qWindows
		class	MyPrefs : public OptionsFileHelper {
			private:
				typedef	OptionsFileHelper	inherited;
			public:
				MyPrefs ():
					inherited (OpenWithCreateAlongPath (HKEY_CURRENT_USER, _T("Software\\Sophist Solutions, Inc.\\LedIt!\\Settings")))
					{
					}
		};
		MyPrefs	gMyPrefsFile;
	#endif
}





typedef	TextInteractor::SearchParameters	SearchParameters;


#if		qWindows
	static	const	TCHAR kDockBarStateEntry[]					=	_T ("DockBarState");
	static	const	TCHAR kSearchParamsMatchString[]			=	_T ("MatchString");
	static	const	TCHAR kSearchParamsRecentMatchStrings[]		=	_T ("RecentMatchStrings");
	static	const	TCHAR kSearchParamsWrapSearch[]				=	_T ("WrapSearch");
	static	const	TCHAR kSearchParamsWholeWordSearch[]		=	_T ("WholeWordSearch");
	static	const	TCHAR kSearchParamsCaseSensativeSearch[]	=	_T ("CaseSensativeSearch");
	static	const	TCHAR kSmartCutAndPaste[]					=	_T ("SmartCutAndPaste");
	static	const	TCHAR kWrapToWindow[]						=	_T ("WrapToWindow");
	static	const	TCHAR kShowHiddenText[]						=	_T ("ShowHiddenText");
	static	const	TCHAR kShowParagraphGlyphs[]				=	_T ("ShowParagraphGlyphs");
	static	const	TCHAR kShowTabGlyphs[]						=	_T ("ShowTabGlyphs");
	static	const	TCHAR kShowSpaceGlyphs[]					=	_T ("ShowSpaceGlyphs");
	static	const	TCHAR kCheckFileAssocAtStartup[]			=	_T ("CheckFileAssocAtStartup");
	static	const	TCHAR kDefaultNewDocFont[]					=	_T ("DefaultNewDocFont");
#endif



	const	bool	kSmartCutAndPasteDefault	=	true;
	const	bool	kWrapToWindowDefault		=	false;
	const	bool	kShowHiddenTextDefault		=	true;
	const	bool	kShowParagraphGlyphsDefault	=	false;
	const	bool	kShowTabGlyphsDefault		=	false;
	const	bool	kShowSpaceGlyphsDefault		=	false;
#if		qWindows
	const	bool	kCheckFileAssocAtStartupDefault	=	true;
#endif

#if		!qWindows
	//tmphack - non-persistant implementation - but good enough for users to see more or less what the different options do...
	//and we can now easily plug in a 'resource manager' implementation of this for the different platforms when time allows
	static	SearchParameters	sOptVal_SearchParameters;
	static	bool				sOptVal_CutAndPaste			=	kSmartCutAndPasteDefault;
	static	bool				sOptVal_WrapToWindow		=	kWrapToWindowDefault;
	static	bool				sOptVal_ShowHiddenText		=	kShowHiddenTextDefault;
	static	bool				sOptVal_ShowParagraphGlyphs	=	kShowParagraphGlyphsDefault;
	static	bool				sOptVal_ShowTabGlyphs		=	kShowTabGlyphsDefault;
	static	bool				sOptVal_ShowSpaceGlyphs		=	kShowSpaceGlyphsDefault;
#endif







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
	#if		qWindows
		SearchParameters	sp;
		sp.fMatchString = gMyPrefsFile.GetPref (kSearchParamsMatchString, sp.fMatchString);
		sp.fRecentFindStrings = gMyPrefsFile.GetPref (kSearchParamsRecentMatchStrings, sp.fRecentFindStrings);
		sp.fWrapSearch = gMyPrefsFile.GetPref (kSearchParamsWrapSearch, sp.fWrapSearch);
		sp.fWholeWordSearch = gMyPrefsFile.GetPref (kSearchParamsWholeWordSearch, sp.fWholeWordSearch);
		sp.fCaseSensativeSearch = gMyPrefsFile.GetPref (kSearchParamsCaseSensativeSearch, sp.fCaseSensativeSearch);
		return sp;
	#else
		return sOptVal_SearchParameters;
	#endif
}

void	Options::SetSearchParameters (const SearchParameters& searchParameters)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kSearchParamsMatchString, searchParameters.fMatchString);
		gMyPrefsFile.StorePref (kSearchParamsRecentMatchStrings, searchParameters.fRecentFindStrings);
		gMyPrefsFile.StorePref (kSearchParamsWrapSearch, searchParameters.fWrapSearch);
		gMyPrefsFile.StorePref (kSearchParamsWholeWordSearch, searchParameters.fWholeWordSearch);
		gMyPrefsFile.StorePref (kSearchParamsCaseSensativeSearch, searchParameters.fCaseSensativeSearch);
	#else
		sOptVal_SearchParameters = searchParameters;
	#endif
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
	#if		qWindows
		return gMyPrefsFile.GetPref (kSmartCutAndPaste, kSmartCutAndPasteDefault);
	#else
		return sOptVal_CutAndPaste;
	#endif
}

void	Options::SetSmartCutAndPaste (bool smartCutAndPaste)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kSmartCutAndPaste, smartCutAndPaste);
	#else
		sOptVal_CutAndPaste = smartCutAndPaste;
	#endif
}

bool	Options::GetWrapToWindow () const
{
	#if		qWindows
		return gMyPrefsFile.GetPref (kWrapToWindow, kWrapToWindowDefault);
	#else
		return sOptVal_WrapToWindow;
	#endif
}

void	Options::SetWrapToWindow (bool wrapToWindow)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kWrapToWindow, wrapToWindow);
	#else
		sOptVal_WrapToWindow = wrapToWindow;
	#endif
}

bool	Options::GetShowHiddenText () const
{
	#if		qWindows
		return gMyPrefsFile.GetPref (kShowHiddenText, kShowHiddenTextDefault);
	#else
		return sOptVal_ShowHiddenText;
	#endif
}

void	Options::SetShowHiddenText (bool showHiddenText)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kShowHiddenText, showHiddenText);
	#else
		sOptVal_ShowHiddenText = showHiddenText;
	#endif
}

bool	Options::GetShowParagraphGlyphs () const
{
	#if		qWindows
		return gMyPrefsFile.GetPref (kShowParagraphGlyphs, kShowParagraphGlyphsDefault);
	#else
		return sOptVal_ShowParagraphGlyphs;
	#endif
}

void	Options::SetShowParagraphGlyphs (bool showParagraphGlyphs)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kShowParagraphGlyphs, showParagraphGlyphs);
	#else
		sOptVal_ShowParagraphGlyphs = showParagraphGlyphs;
	#endif
}

bool	Options::GetShowTabGlyphs () const
{
	#if		qWindows
		return gMyPrefsFile.GetPref (kShowTabGlyphs, kShowTabGlyphsDefault);
	#else
		return sOptVal_ShowTabGlyphs;
	#endif
}

void	Options::SetShowTabGlyphs (bool showTabGlyphs)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kShowTabGlyphs, showTabGlyphs);
	#else
		sOptVal_ShowTabGlyphs = showTabGlyphs;
	#endif
}

bool	Options::GetShowSpaceGlyphs () const
{
	#if		qWindows
		return gMyPrefsFile.GetPref (kShowSpaceGlyphs, kShowSpaceGlyphsDefault);
	#else
		return sOptVal_ShowSpaceGlyphs;
	#endif
}

void	Options::SetShowSpaceGlyphs (bool showSpaceGlyphs)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kShowSpaceGlyphs, showSpaceGlyphs);
	#else
		sOptVal_ShowSpaceGlyphs = showSpaceGlyphs;
	#endif
}

#if		qWindows
bool	Options::GetCheckFileAssocsAtStartup () const
{
	return gMyPrefsFile.GetPref (kCheckFileAssocAtStartup, kCheckFileAssocAtStartupDefault);
}

void	Options::SetCheckFileAssocsAtStartup (bool checkFileAssocsAtStartup)
{
	gMyPrefsFile.StorePref (kCheckFileAssocAtStartup, checkFileAssocsAtStartup);
}
#endif

Led_FontSpecification	Options::GetDefaultNewDocFont () const
{
	#if		qWindows
		vector<Byte>	bytes;
		if (gMyPrefsFile.LookupPref (kDefaultNewDocFont, &bytes)) {
			if (bytes.size () == sizeof (LOGFONT)) {
				Led_FontSpecification	fsp;
				fsp.SetOSRep (*reinterpret_cast<LOGFONT*> (&*bytes.begin ()));
				return fsp;
			}
		}
	#endif
	Led_FontSpecification	defFont	=	TextImager::GetStaticDefaultFont ();
	return defFont;
}

void	Options::SetDefaultNewDocFont (const Led_FontSpecification& defaultNewDocFont)
{
	#if		qWindows
		gMyPrefsFile.StorePref (kDefaultNewDocFont, sizeof (LOGFONT), reinterpret_cast<const Byte*> (&defaultNewDocFont.GetOSRep ()));
	#else
		Led_Arg_Unused (defaultNewDocFont);
	#endif
}



// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
