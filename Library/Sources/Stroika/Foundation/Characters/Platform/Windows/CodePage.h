/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_Platform_Windows_CodePage_h_
#define _Stroika_Foundation_Characters_Platform_Windows_CodePage_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#if qPlatform_Windows
#include <Windows.h>

#include <OAIdl.h>
#include <wtypes.h>
#else
#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika::Foundation::Characters::Platform::Windows {

    using Characters::CodePage;

    void WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult);
    void NarrowStringToWide (const char* sStart, const char* sEnd, CodePage codePage, wstring* intoResult);

    string  BSTRStringToUTF8 (const BSTR bstr);
    BSTR    UTF8StringToBSTR (const char* ws);
    wstring BSTR2wstring (BSTR b);
    wstring BSTR2wstring (VARIANT b);

    /*
    @METHOD:        Win32CharSetToCodePage
    @DESCRIPTION:   <p>Map from a Win32 lfCharset enumeration (as appears in RTF font specifications and LOGFONT specifications)
                This routine is made portable to other platforms BECAUSE it is used in RTF files.
                This function returns the magic code page # zero (CP_ACP on windows) if there is no good mapping.</p>
                
                From https://msdn.microsoft.com/en-us/library/windows/desktop/dd145037(v=vs.85).aspx - we have that lfCharset is a BYTE (8 bits unsigned)
    */
    CodePage Win32CharSetToCodePage (uint8_t lfCharSet);

    /*
    @METHOD:        Win32PrimaryLangIDToCodePage
    @DESCRIPTION:   <p>Map from a Win32 language identifier to a code page.</p>
    */
    CodePage Win32PrimaryLangIDToCodePage (USHORT languageIdenifier);

    class PlatformCodePageConverter {
    public:
        PlatformCodePageConverter (CodePage codePage);

    public:
        nonvirtual void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;
        nonvirtual void MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;

    private:
        CodePage fCodePage_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodePage.inl"

#endif /*_Stroika_Foundation_Characters_Platform_Windows_CodePage_h_*/
