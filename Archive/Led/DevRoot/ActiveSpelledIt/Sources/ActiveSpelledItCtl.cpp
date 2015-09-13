/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Sources/ActiveSpelledItCtl.cpp,v 1.9 2003/12/17 21:01:33 lewis Exp $
 *
 * Changes:
 *	$Log: ActiveSpelledItCtl.cpp,v $
 *	Revision 1.9  2003/12/17 21:01:33  lewis
 *	SPR#1600: Change FindWordBreaks method to return an Object (_WordInfo) - and created and supported that new object.
 *	
 *	Revision 1.8  2003/12/17 01:59:53  lewis
 *	SPR#1585: also added propget AddWordToUserDictionarySupported
 *	
 *	Revision 1.7  2003/12/17 01:19:21  lewis
 *	SPR#1585: Added several new UD/SD loading APIs to ActiveSpelledIt. Minimal testing of the
 *	new APIs - but at least saving to UDs works (AddWordToUD UI/API).
 *	
 *	Revision 1.6  2003/06/12 17:33:55  lewis
 *	SPR#1425: fix last checkin message: really did a few small cheanges like passing in
 *	CURSOR arg to CreateScanContext API (with IDL defaultValue(0)) and added LookupWord ()
 *	method
 *	
 *	Revision 1.5  2003/06/12 17:30:39  lewis
 *	SPR#1526: significantly embellished test suiteTests/ActiveSpelledItCtl.htm
 *	
 *	Revision 1.4  2003/06/10 16:30:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.3  2003/06/10 15:14:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2003/06/10 14:30:04  lewis
 *	SPR#1526: more work - improved test suite - added basic DEMO-MODE support, added
 *	version# OLE AUTOMATION API, exception safety in OLEAUT code, and maybe a bit more...
 *	
 *	Revision 1.1  2003/06/10 02:06:18  lewis
 *	first cut at ActiveSpelledIt control
 *	
 *
 *
 *
 */
#include	<shlobj.h>

#include	"ActiveSpelledItConfig.h"

#include	"Led_ATL.h"

#include	"ActiveSpelledIt_ScanContext.h"
#include	"ActiveSpelledIt_WordInfo.h"

#include	"ActiveSpelledItCtl.h"





namespace {

	// List of available system dictionaries
	struct	SystemDict {
		wchar_t*											fDictName;
		const	SpellCheckEngine_Basic::CompiledDictionary&	fDictionary;
	}
	sSystemDictionaries [] = {
		#if		qIncludeBakedInDictionaries
		{	L"US-English",				SpellCheckEngine_Basic::kDictionary_US_English	}
		#endif
	};

}





namespace {
	wstring	GetDefUDName ()
		{
			// Place the dictionary in a reasonable - but hardwired place. Later - allow for editing that location,
			// and other spellchecking options (see SPR#1591)
			TCHAR	defaultPath[MAX_PATH+1];
			Led_Verify (::SHGetSpecialFolderPath (NULL, defaultPath, CSIDL_FLAG_CREATE | CSIDL_PERSONAL, true));
			wstring	udName	=	Led_SDKString2Wide (Led_SDK_String (defaultPath) + Led_SDK_TCHAROF ("\\My ActiveSpelledIt Dictionary.txt"));
			return udName;
		}

}





/*
 ********************************************************************************
 ***************************** ActiveSpelledItCtl *******************************
 ********************************************************************************
 */
ActiveSpelledItCtl::ActiveSpelledItCtl ():
	fSpellCheckEngine ()
{
	fSpellCheckEngine.SetMainDictionary (&SpellCheckEngine_Basic::kDictionary_US_English);
	try {
		Led_ThrowIfErrorHRESULT (put_UserDictionary (CComBSTR (GetDefUDName ().c_str ())));
	}
	catch (...) {
	}
}

ActiveSpelledItCtl::~ActiveSpelledItCtl ()
{
}

STDMETHODIMP	ActiveSpelledItCtl::InterfaceSupportsErrorInfo (REFIID riid)
{
	static const IID* arr[] = {
		&IID_IActiveSpelledItCtl,
	};
	for (size_t i = 0; i < Led_NEltsOf (arr); i++) {
		if (::InlineIsEqualGUID (*arr[i], riid)) {
			return S_OK;
		}
	}
	return S_FALSE;
}

HRESULT		ActiveSpelledItCtl::OnDraw (ATL_DRAWINFO& di)
{
	RECT& rc = *(RECT*)di.prcBounds;
	// Set Clip region to the rectangle specified by di.prcBounds
	HRGN hRgnOld = NULL;
	if (::GetClipRgn (di.hdcDraw, hRgnOld) != 1)
		hRgnOld = NULL;
	bool bSelectOldRgn = false;

	HRGN hRgnNew = ::CreateRectRgn (rc.left, rc.top, rc.right, rc.bottom);

	if (hRgnNew != NULL) {
		bSelectOldRgn = (SelectClipRgn(di.hdcDraw, hRgnNew) != ERROR);
	}

	::Rectangle (di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);
	::SetTextAlign (di.hdcDraw, TA_CENTER|TA_BASELINE);
	LPCTSTR pszText = _T("ActiveSpelledIt!");
	::TextOut (di.hdcDraw, 
		(rc.left + rc.right) / 2, 
		(rc.top + rc.bottom) / 2, 
		pszText, 
		lstrlen(pszText));

	if (bSelectOldRgn)
		::SelectClipRgn (di.hdcDraw, hRgnOld);

	return S_OK;
}

HRESULT		ActiveSpelledItCtl::FinalConstruct ()
{
	return S_OK;
}

void		ActiveSpelledItCtl::FinalRelease () 
{
}

STDMETHODIMP	ActiveSpelledItCtl::GenerateSuggestions (BSTR missingWord, VARIANT* results)
{
	if (missingWord == NULL or results == NULL) {
		return E_INVALIDARG;
	}

	try {
		Led_tString	mw	=	Led_tString (reinterpret_cast<Led_tChar*> (missingWord), ::SysStringLen (missingWord));
		::VariantClear (results);

		vector<Led_tString>	res	=	fSpellCheckEngine.GenerateSuggestions (mw);

		*results = CreateSafeArrayOfBSTR (res);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();

	return S_OK;
}

STDMETHODIMP	ActiveSpelledItCtl::CreateScanContext (BSTR text, UINT cursor, IDispatch** scanContext)
{
	static	CComBSTR	kEmptyStr;
	if (scanContext == NULL) {
		return E_INVALIDARG;
	}
	if (cursor > ::SysStringLen (text)) {
		return E_INVALIDARG;
	}
	try {
		typedef	ActiveSpelledIt_ScanContext::CTOR_ARGS	CTA;
		*scanContext = new CComObjectWithARGS<ActiveSpelledIt_ScanContext, CTA> (CTA (this, text, cursor));
		(*scanContext)->AddRef ();
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return S_OK;
}

STDMETHODIMP	ActiveSpelledItCtl::get_VersionNumber (LONG* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	*pVal = kActiveSpelledItDWORDVersion;
	return S_OK;
}

STDMETHODIMP	ActiveSpelledItCtl::get_ShortVersionString (BSTR* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	try {
		string	result	=	qLed_ShortVersionString + string (kDemoString);
		#if		qDebug
			result += "-(debug)";
		#endif
		*pVal = ::SysAllocString (ACP2WideString (result).c_str ());
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return S_OK;
}

STDMETHODIMP	ActiveSpelledItCtl::LookupWord (BSTR word, BSTR* matchedWord, VARIANT_BOOL* found)
{
	if (word == NULL or found == NULL) {
		return E_INVALIDARG;
	}
	Led_tString	mw	=	Led_tString (reinterpret_cast<Led_tChar*> (word), ::SysStringLen (word));

	Led_tString	matchedWordResult;
	*found = fSpellCheckEngine.LookupWord (mw, &matchedWordResult);
	if (*found and matchedWord != NULL) {
		*matchedWord = ::SysAllocString (matchedWordResult.c_str ());
	}

	return S_OK;
}

STDMETHODIMP	ActiveSpelledItCtl::FindWordBreaks (BSTR srcText, UINT textOffsetToStartLookingForWord, IDispatch** wordInfo)
{
	try {
		Led_RefCntPtr<TextBreaks>	textBreaks	=	fSpellCheckEngine.GetTextBreaker ();
		Led_Assert (not textBreaks.IsNull ());

		Led_tString	tText	=	Led_WideString2tString (srcText);
		size_t	wStartRes	=	0;
		size_t	wEndRes		=	0;
		bool	wReal		=	false;
		textBreaks->FindWordBreaks (tText.c_str (), tText.length (), textOffsetToStartLookingForWord, &wStartRes, &wEndRes, &wReal);
		typedef	ActiveSpelledIt_WordInfo::CTOR_ARGS	CTA;
		*wordInfo = new CComObjectWithARGS<ActiveSpelledIt_WordInfo, CTA> (CTA (wStartRes, wEndRes, wReal));
		(*wordInfo)->AddRef ();
		return S_OK;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP	ActiveSpelledItCtl::get_AvailableSystemDictionaries (VARIANT* pVal)
{
	try {
		::VariantClear (pVal);
		vector<wstring>	list;
		for (size_t i = 0; i < Led_NEltsOf (sSystemDictionaries); ++i) {
			list.push_back (sSystemDictionaries[i].fDictName);
		}
		*pVal = CreateSafeArrayOfBSTR (list);
		return S_OK;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP	ActiveSpelledItCtl::get_SystemDictionaryList (VARIANT* pVal)
{
	try {
		::VariantClear (pVal);
		typedef	SpellCheckEngine_Basic::Dictionary	Dictionary;
		vector<const Dictionary*>	usedDicts	=	fSpellCheckEngine.GetDictionaries ();
		vector<wstring>				list;
		for (size_t i = 0; i < Led_NEltsOf (sSystemDictionaries); ++i) {
			for (vector<const Dictionary*>::const_iterator j = usedDicts.begin (); j != usedDicts.end (); ++j) {
				if (*j == &sSystemDictionaries[i].fDictionary) {
					list.push_back (sSystemDictionaries[i].fDictName);
				}
			}
		}
		*pVal = CreateSafeArrayOfBSTR (list);
		return S_OK;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP	ActiveSpelledItCtl::put_SystemDictionaryList (VARIANT val)
{
	// Written to assume underlying dict API supports only a single sysDict (for now)
	try {
		vector<wstring>	sysDictNames	=	UnpackVectorOfStringsFromVariantArray (val);
		for (vector<wstring>::const_iterator i = sysDictNames.begin (); i != sysDictNames.end (); ++i) {
			bool	found	=	false;
			for (size_t j= 0; j < Led_NEltsOf (sSystemDictionaries); ++j) {
				if (*i == sSystemDictionaries[j].fDictName) {
					fSpellCheckEngine.SetMainDictionary (&sSystemDictionaries[j].fDictionary);
					found = true;
				}
			}
			if (not found) {
				return E_INVALIDARG;
			}
		}
		return S_OK;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP	ActiveSpelledItCtl::get_DictionaryList (VARIANT* pVal)
{
	try {
		::VariantClear (pVal);

		typedef	SpellCheckEngine_Basic::Dictionary	Dictionary;
		vector<const Dictionary*>	usedDicts	=	fSpellCheckEngine.GetDictionaries ();
		vector<wstring>				list;
		for (size_t i = 0; i < Led_NEltsOf (sSystemDictionaries); ++i) {
			for (vector<const Dictionary*>::const_iterator j = usedDicts.begin (); j != usedDicts.end (); ++j) {
				if (*j == &sSystemDictionaries[i].fDictionary) {
					list.push_back (sSystemDictionaries[i].fDictName);
				}
			}
		}

		typedef	SpellCheckEngine_Basic_Simple::UDDictionaryName	UDDictionaryName;
		UDDictionaryName	udName	=	fSpellCheckEngine.GetUserDictionary ();
		if (not udName.empty ()) {
			list.push_back (Led_SDKString2Wide (udName));
		}

		*pVal = CreateSafeArrayOfBSTR (list);
		return S_OK;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP	ActiveSpelledItCtl::get_UserDictionary (BSTR* pUDName)
{
	try {
		Led_SDK_String	usingUDName	=	fSpellCheckEngine.GetUserDictionary ();
		wstring			udWName		=	Led_SDKString2Wide (usingUDName);
		*pUDName = ::SysAllocStringLen (udWName.c_str (), udWName.length ());
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return S_OK;
}

STDMETHODIMP	ActiveSpelledItCtl::put_UserDictionary (BSTR UDName)
{
	try {
		Led_SDK_String	useUDName	=	(UDName==NULL)? Led_SDK_String (): Led_Wide2SDKString (UDName);
		if (not useUDName.empty ()) {
			if (useUDName != Led_Wide2SDKString (GetDefUDName ())) {
				// Require only this UD name - for security reasons (avoid allowing peek at other files)
				return E_INVALIDARG;
			}
		}
		fSpellCheckEngine.SetUserDictionary (useUDName.c_str ());
		return S_OK;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP	ActiveSpelledItCtl::get_DefaultUserDictionaryName (BSTR* pUDName)
{
	try {
		*pUDName = ::SysAllocString (GetDefUDName ().c_str ());
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return S_OK;
}

STDMETHODIMP	ActiveSpelledItCtl::get_AddWordToUserDictionarySupported (VARIANT_BOOL* supported)
{
	if (supported == NULL) {
		return E_INVALIDARG;
	}
	try {
		SpellCheckEngine::UDInterface*	udInterface	=	fSpellCheckEngine.GetUDInterface ();
		Led_AssertNotNil (udInterface);
		*supported = udInterface->AddWordToUserDictionarySupported ();
		return S_OK;
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP	ActiveSpelledItCtl::AddWordToUserDictionary (BSTR word)
{
	try {
		SpellCheckEngine::UDInterface*	udInterface	=	fSpellCheckEngine.GetUDInterface ();
		Led_AssertNotNil (udInterface);
		udInterface->AddWordToUserDictionary (word);
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return S_OK;
}

