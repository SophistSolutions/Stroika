/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Sources/ActiveSpelledIt_ScanContext.cpp,v 1.7 2003/11/03 20:16:54 lewis Exp $
 *
 * Changes:
 *	$Log: ActiveSpelledIt_ScanContext.cpp,v $
 *	Revision 1.7  2003/11/03 20:16:54  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.6  2003/06/12 17:33:55  lewis
 *	SPR#1425: fix last checkin message: really did a few small cheanges like passing in CURSOR arg
 *	to CreateScanContext API (with IDL defaultValue(0)) and added LookupWord () method
 *	
 *	Revision 1.5  2003/06/12 17:30:40  lewis
 *	SPR#1526: significantly embellished test suiteTests/ActiveSpelledItCtl.htm
 *	
 *	Revision 1.4  2003/06/10 16:30:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.3  2003/06/10 15:14:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2003/06/10 14:30:04  lewis
 *	SPR#1526: more work - improved test suite - added basic DEMO-MODE support,
 *	added version# OLE AUTOMATION API, exception safety in OLEAUT code, and maybe
 *	a bit more...
 *	
 *	Revision 1.1  2003/06/10 02:06:19  lewis
 *	first cut at ActiveSpelledIt control
 *	
 *
 *
 *
 */
#include	"ActiveSpelledItConfig.h"

#include	"Led_ATL.h"

#include	"ActiveSpelledItCtl.h"

#include	"ActiveSpelledIt_ScanContext.h"






/*
 ********************************************************************************
 **************************** ActiveSpelledIt_ScanContext ***********************
 ********************************************************************************
 */
ActiveSpelledIt_ScanContext::ActiveSpelledIt_ScanContext (const CTOR_ARGS& args):
	fOwner (args.fOwner),
	fText (args.fText),
	fTextStart (NULL),
	fTextEnd (NULL),
	fCursor (NULL),
	fLastFound (false),
	fWordStart (NULL),
	fWordEnd (NULL)
{
	Led_RequireNotNil (fOwner);
	fOwner->AddRef ();
	fTextStart	=	fText;
	fTextEnd	=	fTextStart + fText.Length ();
	Led_Require (args.fOrigCursor <= fText.Length ());
	fCursor = fTextStart + args.fOrigCursor;

	if (fTextStart != NULL) {
		fLastFound = fOwner->fSpellCheckEngine.ScanForUndefinedWord (fTextStart, fTextEnd, &fCursor, &fWordStart, &fWordEnd);
	}
}

ActiveSpelledIt_ScanContext::~ActiveSpelledIt_ScanContext ()
{
	Led_AssertNotNil (fOwner);
	fOwner->Release ();
}

HRESULT ActiveSpelledIt_ScanContext::FinalConstruct ()
{
	return S_OK;
}

void	ActiveSpelledIt_ScanContext::FinalRelease () 
{
}

STDMETHODIMP ActiveSpelledIt_ScanContext::get_WordFound (VARIANT_BOOL* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	*pVal = fLastFound;
	return S_OK;
}

STDMETHODIMP ActiveSpelledIt_ScanContext::get_WordStart (ULONG* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	if (fLastFound) {
		*pVal = fWordStart - fTextStart;
		return S_OK;
	}
	else {
		return E_INVALIDARG;
	}
}

STDMETHODIMP ActiveSpelledIt_ScanContext::get_WordEnd (ULONG* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	if (fLastFound) {
		*pVal = fWordEnd - fTextStart;
		return S_OK;
	}
	else {
		return E_INVALIDARG;
	}
}

STDMETHODIMP ActiveSpelledIt_ScanContext::get_Word (BSTR* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	try {
		if (fLastFound) {
			*pVal = ::SysAllocStringLen (fWordStart, fWordEnd-fWordStart);
			return S_OK;
		}
		else {
			return E_INVALIDARG;
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
	return S_OK;
}

STDMETHODIMP ActiveSpelledIt_ScanContext::Next ()
{
	try {
		if (fLastFound) {
			fLastFound = fOwner->fSpellCheckEngine.ScanForUndefinedWord (fTextStart, fTextEnd, &fCursor, &fWordStart, &fWordEnd);
			return S_OK;
		}
		else {
			return E_INVALIDARG;
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();
}

STDMETHODIMP ActiveSpelledIt_ScanContext::get_Suggestions (VARIANT* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	Led_AssertNotNil (fOwner);
	try {
		if (fLastFound) {
			Led_tString			mw	=	Led_tString (fWordStart, fWordEnd);
			vector<Led_tString>	res	=	fOwner->fSpellCheckEngine.GenerateSuggestions (mw);
			*pVal = CreateSafeArrayOfBSTR (res);
		}
		else {
			return E_INVALIDARG;
		}
	}
	CATCH_AND_HANDLE_EXCEPTIONS();

	return S_OK;
}

STDMETHODIMP	ActiveSpelledIt_ScanContext::get_Cursor (ULONG* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	*pVal = fCursor-fTextStart;
	return S_OK;
}
