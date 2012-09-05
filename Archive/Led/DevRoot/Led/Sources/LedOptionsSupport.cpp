/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/LedOptionsSupport.cpp,v 2.6 2003/12/31 03:36:18 lewis Exp $
 *
 * Changes:
 *	$Log: LedOptionsSupport.cpp,v $
 *	Revision 2.6  2003/12/31 03:36:18  lewis
 *	SPR#1602: major improvements. Added lots of overloaded set/get methods, including arrays of elts. Added method to compute new key with a path - construcing elts along the way. Support UNICODE/non-UNICODE strings much better
 *	
 *	Revision 2.5  2002/05/06 21:33:44  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.4  2001/11/27 00:29:50  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.3  2001/08/28 18:43:34  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.2  2001/07/31 15:14:36  lewis
 *	use &* trick in front of iterator to get underlying array address (needed for MSVC70B2 STL -
 *	but should work for any STL)
 *	
 *	Revision 2.1  2001/04/12 18:57:19  lewis
 *	*** empty log message ***
 *	
 *	
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	"LedOptionsSupport.h"




#if		qLedUsesNamespaces
	namespace	Led {
#endif

/*
 ********************************************************************************
 ********************************* OptionsFileHelper ****************************
 ********************************************************************************
 */
/*
@METHOD:		OptionsFileHelper::OptionsFileHelper
@DESCRIPTION:	CTOR on Mac takes open resource file handle as argument, and on Windows - takes open HKEY registry
	handle. On both cases - this class takes over ownership - and closes the handle upon descruction.
*/
#if		qMacOS
OptionsFileHelper::OptionsFileHelper (int resFile):
	fResFile (resFile)
{
}
#elif	qWindows
OptionsFileHelper::OptionsFileHelper (HKEY hkey):
	fKey (hkey)
{
}
#endif

OptionsFileHelper::~OptionsFileHelper ()
{
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		::RegCloseKey (fKey);
	#endif
}

#if		qWindows
/*
@METHOD:		OptionsFileHelper::OpenWithCreateAlongPath
@DESCRIPTION:	<p>Walk the given path (in segments) - and make sure each exists, and create each segment if it doesn't
			exist. Finally - do a regular registry open with access permissions 'samDesired'.</p>
*/
HKEY	OptionsFileHelper::OpenWithCreateAlongPath (HKEY parentKey, const Led_SDK_String& path, REGSAM samDesired)
{
	size_t	prevPos	=	0;
	HKEY	curPar	=	parentKey;
	for (;;) {
		size_t			endPos		=	path.find ('\\', prevPos);
		Led_SDK_String	segName		=	path.substr (prevPos, endPos==Led_SDK_String::npos? endPos: (endPos-prevPos));
		REGSAM			createSAM	=	(endPos == Led_SDK_String::npos)?  samDesired: KEY_READ;
		HKEY			newKey		=	NULL;

		Led_ThrowIfNotERROR_SUCCESS (::RegCreateKeyEx (curPar, segName.c_str (),
															0, REG_NONE, REG_OPTION_NON_VOLATILE, createSAM, NULL, &newKey, NULL
															)
										);
		if (curPar != parentKey) {
			::RegCloseKey (curPar);
		}
		curPar = newKey;
		if (endPos == Led_SDK_String::npos) {
			return newKey;
		}
		prevPos = endPos + 1;
	}
}
#endif

/*
@METHOD:		OptionsFileHelper::LookupPref
@DESCRIPTION:	<p>This function is overloaded to return (through pointer parameters) a number of
			different data types, including string, wstring, int, and vector<Byte>). If the given
			named preference (and type) is available, it is returned through that parameter, and
			true is returned. Otherwise, false is returned.</p>
*/
bool	OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, string* value)
{
	Led_RequireNotNil (prefName);
	Led_RequireNotNil (value);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		string	strValue;
		DWORD	dwType;
		DWORD	dwCount = 0;
		LONG	lResult	=	::RegQueryValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), NULL, &dwType, NULL, &dwCount);
		if (lResult == ERROR_SUCCESS) {
			if (dwType == REG_SZ) {
				strValue.resize (dwCount);
				lResult = ::RegQueryValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), NULL, &dwType, (LPBYTE)&(*strValue.begin ()), &dwCount);
			}
			else {
				lResult = !ERROR_SUCCESS;
			}
		}
		if (lResult == ERROR_SUCCESS) {
			if (dwType == REG_SZ) {
				*value = strValue.c_str ();	// copying like this loses xtra NUL-byte if there is one from read...
				return true;
			}
		}
		return false;
	#endif
}

bool	OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, wstring* value)
{
	Led_RequireNotNil (prefName);
	Led_RequireNotNil (value);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		wstring	strValue;
		DWORD	dwType;
		DWORD	dwCount = 0;
		LONG	lResult	=	::RegQueryValueExW (fKey, Led_SDKString2Wide (prefName).c_str (), NULL, &dwType, NULL, &dwCount);
		if (lResult == ERROR_SUCCESS) {
			if (dwType == REG_SZ) {
				strValue.resize (dwCount);
				lResult = ::RegQueryValueExW (fKey, Led_SDKString2Wide (prefName).c_str (), NULL, &dwType, (LPBYTE)&(*strValue.begin ()), &dwCount);
			}
			else {
				lResult = !ERROR_SUCCESS;
			}
		}
		if (lResult == ERROR_SUCCESS) {
			if (dwType == REG_SZ) {
				*value = strValue.c_str ();	// copying like this loses xtra NUL-byte if there is one from read...
				return true;
			}
		}
		// If not success - then maybe cuz non-UNICODE functions not available - so try to read a non-UNICODE string
		{
			string	tmp;
			if (LookupPref (prefName, &tmp)) {
				*value = ACP2WideString (tmp);
				return true;
			}
		}
		return false;
	#endif
}

bool	OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, bool* value)
{
	Led_RequireNotNil (prefName);
	Led_RequireNotNil (value);
	int	tmp		=	0;
	if (LookupPref (prefName, &tmp)) {
		*value = tmp;
		return true;
	}
	return false;
}

bool	OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, int* value)
{
	Led_RequireNotNil (prefName);
	Led_RequireNotNil (value);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		DWORD	dwValue;
		DWORD	dwType;
		DWORD	dwCount = sizeof (DWORD);
		LONG	lResult	=	::RegQueryValueEx (fKey, prefName, NULL, &dwType, (LPBYTE)&dwValue, &dwCount);
		if (lResult == ERROR_SUCCESS) {
			if (dwType == REG_DWORD) {
				*value = dwValue;
				return true;
			}
		}
		return false;
	#endif
}

bool	OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, vector<Byte>* value)
{
	Led_RequireNotNil (prefName);
	Led_RequireNotNil (value);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		DWORD	dwType	=	0;
		DWORD	dwCount =	0;
		LONG	lResult	=	::RegQueryValueEx (fKey, prefName, NULL, &dwType, NULL, &dwCount);
		if (lResult == ERROR_SUCCESS) {
			if (dwType == REG_BINARY) {
				value->resize (dwCount);
				lResult = ::RegQueryValueEx (fKey, prefName, NULL, &dwType, &(*value->begin ()), &dwCount);
			}
			else {
				lResult = !ERROR_SUCCESS;
			}
		}
		return lResult == ERROR_SUCCESS;
	#endif
	return false;
}

bool	OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, vector<string>* value)
{
	Led_RequireNotNil (value);
	value->clear ();
	string	tmp;
	while (LookupPref (Format (Led_SDK_TCHAROF ("%s_%d"), prefName, value->size ()).c_str (), &tmp)) {
		value->push_back (tmp);
	}
	return not value->empty ();
}

bool	OptionsFileHelper::LookupPref (const Led_SDK_Char* prefName, vector<wstring>* value)
{
	Led_RequireNotNil (value);
	value->clear ();
	wstring	tmp;
	while (LookupPref (Format (Led_SDK_TCHAROF ("%s_%d"), prefName, value->size ()).c_str (), &tmp)) {
		value->push_back (tmp);
	}
	return not value->empty ();
}

void	OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const string& value)
{
	Led_RequireNotNil (prefName);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		(void)::RegSetValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), NULL, REG_SZ, (LPBYTE)value.c_str (), (value.length ()+1)*sizeof(TCHAR));
	#endif
}

void	OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const wstring& value)
{
	Led_RequireNotNil (prefName);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		if (::RegSetValueExW (fKey, Led_SDKString2Wide (prefName).c_str (), NULL, REG_SZ, reinterpret_cast<const Byte*> (value.c_str ()), (value.length ()+1)*sizeof(wchar_t)) != ERROR_SUCCESS) {
			// failure to write a UNICODE string could be because the OS doesn't support it - so try writing it as ANSI
			string	tmp	=	Wide2ACPString (value);
			(void)::RegSetValueExA (fKey, Led_SDKString2ANSI (prefName).c_str (), NULL, REG_SZ, reinterpret_cast<const Byte*> (tmp.c_str ()), tmp.length ()+1);
		}
	#endif
}

void	OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, bool value)
{
	Led_RequireNotNil (prefName);
	StorePref (prefName, static_cast<int> (value));
}

void	OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, int value)
{
	Led_RequireNotNil (prefName);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		DWORD	dwValue	=	value;
		(void)::RegSetValueEx (fKey, prefName, NULL, REG_DWORD, (LPBYTE)&dwValue, sizeof (dwValue));
	#endif
}

void	OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, size_t nBytes, const Byte* data)
{
	Led_RequireNotNil (prefName);
	Led_RequireNotNil (data);
	#if		qMacOS
		Led_Assert (false);	// NYI
	#elif	qWindows
		(void)::RegSetValueEx (fKey, prefName, NULL, REG_BINARY, (LPBYTE)data, nBytes);
	#endif
}

void	OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const vector<string>& value)
{
	Led_RequireNotNil (prefName);
	for (vector<string>::const_iterator i = value.begin (); i != value.end (); ++i) {
		StorePref (Format (Led_SDK_TCHAROF ("%s_%d"), prefName, i-value.begin ()).c_str (), *i);
	}
}

void	OptionsFileHelper::StorePref (const Led_SDK_Char* prefName, const vector<wstring>& value)
{
	Led_RequireNotNil (prefName);
	for (vector<wstring>::const_iterator i = value.begin (); i != value.end (); ++i) {
		StorePref (Format (Led_SDK_TCHAROF ("%s_%d"), prefName, i-value.begin ()).c_str (), *i);
	}
}



#if		qLedUsesNamespaces
}
#endif


// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

