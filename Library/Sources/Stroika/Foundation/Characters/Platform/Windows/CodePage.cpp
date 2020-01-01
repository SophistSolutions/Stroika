/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../../../StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "../../../Debug/Trace.h"
#include "../../../Execution/Throw.h"

#include "CodePage.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Characters::Platform::Windows;

/*
 ********************************************************************************
 *************** Characters::Platform::Windows::BSTRStringToUTF8 ****************
 ********************************************************************************
 */
string Characters::Platform::Windows::BSTRStringToUTF8 (const BSTR bstr)
{
    if (bstr == nullptr) {
        return string ();
    }
    else {
        int    srcStrLen    = ::SysStringLen (bstr);
        int    stringLength = ::WideCharToMultiByte (kCodePage_UTF8, 0, bstr, srcStrLen, nullptr, 0, nullptr, nullptr);
        string result;
        result.resize (stringLength);
        Verify (::WideCharToMultiByte (kCodePage_UTF8, 0, bstr, srcStrLen, Containers::Start (result), stringLength, nullptr, nullptr) == stringLength);
        return result;
    }
}

/*
 ********************************************************************************
 *************** Characters::Platform::Windows::UTF8StringToBSTR ****************
 ********************************************************************************
 */
BSTR Characters::Platform::Windows::UTF8StringToBSTR (const char* ws)
{
    RequireNotNull (ws);
    size_t wsLen        = ::strlen (ws);
    int    stringLength = ::MultiByteToWideChar (CP_UTF8, 0, ws, static_cast<int> (wsLen), nullptr, 0);
    BSTR   result       = ::SysAllocStringLen (nullptr, stringLength);
    if (result == nullptr)
        [[UNLIKELY_ATTR]]
        {
            Execution::Throw (bad_alloc ());
        }
    Verify (::MultiByteToWideChar (kCodePage_UTF8, 0, ws, static_cast<int> (wsLen), result, stringLength) == stringLength);
    return result;
}

/*
 ********************************************************************************
 ******************* Characters::Platform::Windows::BSTR2wstring ****************
 ********************************************************************************
 */
wstring Characters::Platform::Windows::BSTR2wstring (VARIANT b)
{
    if (b.vt == VT_BSTR) {
        return BSTR2wstring (b.bstrVal);
    }
    else {
        return wstring ();
    }
}

/*
 ********************************************************************************
 **************************** PlatformCodePageConverter *************************
 ********************************************************************************
 */
void PlatformCodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
{
    Require (inMBCharCnt == 0 or inMBChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);
    //  *outCharCnt = ::MultiByteToWideChar (fCodePage, MB_ERR_INVALID_CHARS, inMBChars, inMBCharCnt, outChars, *outCharCnt);
    *outCharCnt = ::MultiByteToWideChar (fCodePage, 0, inMBChars, static_cast<int> (inMBCharCnt), outChars, static_cast<int> (*outCharCnt));
#if 0
// enable to debug cases (e.g. caused when you read a CRLF file with fstream
// in text mode, and get - somehow - stuff that triggers this ??? - with convert to
// xml???). Anyhow - get error#102 - BUF_NOT_BIG-ENUF or osmeting like that...
//
// Debug when its happening again -- LGP 2008-09-02
    if (*outCharCnt == 0) {
        DWORD x = GetLastError ();
        int breaker = 1;
    }
#endif
}

void PlatformCodePageConverter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
    Require (inCharCnt == 0 or inChars != nullptr);
    RequireNotNull (outCharCnt);
    Require (*outCharCnt == 0 or outChars != nullptr);
    *outCharCnt = ::WideCharToMultiByte (fCodePage, 0, inChars, static_cast<int> (inCharCnt), outChars, static_cast<int> (*outCharCnt), nullptr, nullptr);
}
