/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/ActiveSpelledIt/Sources/ActiveSpelledIt_WordInfo.cpp,v 1.1 2004/01/01 23:24:48 lewis Exp $
 *
 * Changes:
 *	$Log: ActiveSpelledIt_WordInfo.cpp,v $
 *	Revision 1.1  2004/01/01 23:24:48  lewis
 *	forgot to checkin earlier
 *	
 *	
 *
 *
 *
 */
#include	"ActiveSpelledItConfig.h"

#include	"Led_ATL.h"

#include	"ActiveSpelledIt_WordInfo.h"






/*
 ********************************************************************************
 ******************************* ActiveSpelledIt_WordInfo ***********************
 ********************************************************************************
 */
ActiveSpelledIt_WordInfo::ActiveSpelledIt_WordInfo (const CTOR_ARGS& args):
	fWordStart (args.fWordStart),
	fWordEnd (args.fWordEnd),
	fWordReal (args.fWordReal)
{
	_pAtlModule->Lock ();		// Prevent module from closing while this object still exists
}

ActiveSpelledIt_WordInfo::~ActiveSpelledIt_WordInfo ()
{
	_pAtlModule->Unlock ();
}

HRESULT ActiveSpelledIt_WordInfo::FinalConstruct ()
{
	return S_OK;
}

void	ActiveSpelledIt_WordInfo::FinalRelease () 
{
}

STDMETHODIMP ActiveSpelledIt_WordInfo::get_WordStart (UINT* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	*pVal = fWordStart;
	return S_OK;
}

STDMETHODIMP ActiveSpelledIt_WordInfo::get_WordEnd (UINT* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	*pVal = fWordEnd;
	return S_OK;
}

STDMETHODIMP	ActiveSpelledIt_WordInfo::get_WordReal (VARIANT_BOOL* pVal)
{
	if (pVal == NULL) {
		return E_INVALIDARG;
	}
	*pVal = fWordReal;
	return S_OK;
}
