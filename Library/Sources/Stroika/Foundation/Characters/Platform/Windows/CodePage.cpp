/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#include	"../../../Debug/Trace.h"

#include	"CodePage.h"




using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Characters;
using	namespace	Stroika::Foundation::Characters::Platform::Windows;





string	Characters::Platform::Windows::BSTRStringToUTF8 (const BSTR bstr)
{
	if (bstr == nullptr) {
		return string ();
	}
	else {
		int	srcStrLen	=	::SysStringLen (bstr);
		int stringLength = ::WideCharToMultiByte (kCodePage_UTF8, 0, bstr, srcStrLen, nullptr, 0, nullptr, nullptr);
		string	result;
		result.resize (stringLength);
		Verify (::WideCharToMultiByte (kCodePage_UTF8, 0, bstr, srcStrLen, Containers::Start (result), stringLength, nullptr, nullptr) == stringLength);
		return result;
	}
}

BSTR	Characters::Platform::Windows::UTF8StringToBSTR (const char* ws)
{
	RequireNotNull (ws);
	size_t	wsLen	=	::strlen (ws);
	int stringLength = ::MultiByteToWideChar (CP_UTF8, 0, ws, static_cast<int> (wsLen), nullptr, 0);
	BSTR result	= ::SysAllocStringLen (nullptr, stringLength);
	if (result == nullptr) {
		DbgTrace ("UTF8StringToBSTR () out of memory - throwing bad_alloc");
		throw bad_alloc ();
	}
	Verify (::MultiByteToWideChar (kCodePage_UTF8, 0, ws, static_cast<int> (wsLen), result, stringLength) == stringLength);
	return result;
}

wstring	Characters::Platform::Windows::BSTR2wstring (VARIANT b)
{
	if (b.vt == VT_BSTR) {
		return BSTR2wstring (b.bstrVal);
	}
	else {
		return wstring ();
	}
}
