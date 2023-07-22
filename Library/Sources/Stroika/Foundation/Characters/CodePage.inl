/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodePage_inl_
#define _Stroika_Foundation_Characters_CodePage_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <algorithm>
#include <cstring>

#include "../Debug/Assertions.h"

#if qPlatform_Windows
#include "Platform/Windows/CodePage.h"
#endif

#include "UTFConvert.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ****************************** GetDefaultSDKCodePage ***************************
     ********************************************************************************
     */
    inline CodePage GetDefaultSDKCodePage ()
    {
#if qPlatform_Windows
//http://blogs.msdn.com/b/michkap/archive/2005/01/06/347834.aspx
#if defined(CP_ACP)
        Assert (CP_ACP == 0);
        return CP_ACP; // special windows define which maps to the current OS code page
#else
        return 0;
#endif
//GetACP()              // means essentially the same thing but supposedly (even if we cahced GetACP() - CP_ACP is faster)
#else
        // MAYBE should use the LOCALE stuff - and get the current code-page from the locale? If such a thing?
        return kCodePage_UTF8; // So far - this is meaningless on other systems - but this would be the best guess I think
#endif
    }

    /*
     ********************************************************************************
     *********************************** UTFConvert *********************************
     ********************************************************************************
     */

    namespace UTFConvert {

        enum [[deprecated ("Since Stroika v3.0d1, use class UTFConverter")]] ConversionResult{
            conversionOK,    /* conversion successful */
            sourceExhausted, /* partial character in source, but hit end */
            targetExhausted, /* insuff. room in target for conversion */
            sourceIllegal    /* source sequence is illegal/malformed */
        };
        enum [[deprecated ("Since Stroika v3.0d1, use class UTFConverter")]] ConversionFlags{strictConversion = 0, lenientConversion};

        /**
         */
        [[deprecated ("Since Stroika v3.0d1, could support, but not clearly any reason")]] bool IsLegalUTF8Sequence (const char* source,
                                                                                                                     const char* sourceEnd);
        [[deprecated ("Since Stroika v3.0d1, could support, but not clearly any reason")]] bool IsLegalUTF8Sequence (const char8_t* source,
                                                                                                                     const char8_t* sourceEnd);

        template <typename FROM, typename TO>
        [[deprecated ("Since Stroika v3.0d1, use class UTFConverter")]] inline size_t
        QuickComputeConversionOutputBufferSize (const FROM* sourceStart, const FROM* sourceEnd)
        {
            return UTFConverter::ComputeTargetBufferSize<TO> (span<const FROM>{sourceStart, sourceEnd});
        }
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        template <typename FROM, typename TO>
        [[deprecated ("Since Stroika v3.0d1, use class UTFConverter::kThe")]] ConversionResult
        ConvertQuietly (const FROM** sourceStart, const FROM* sourceEnd, TO** targetStart, TO* targetEnd, ConversionFlags flags)
        {
            auto r = UTFConverter::kThe.ConvertQuietly (span{*sourceStart, sourceEnd}, span{*targetStart, targetEnd});
            *sourceStart += get<1> (r);
            *targetStart += get<2> (r);
            switch (get<0> (r)) {
                case UTFConverter::ConversionStatusFlag::ok:
                    return ConversionResult::conversionOK;
                case UTFConverter::ConversionStatusFlag::sourceExhausted:
                    return ConversionResult::sourceExhausted;
                case UTFConverter::ConversionStatusFlag::sourceIllegal:
                    return ConversionResult::sourceIllegal;
                default:
                    AssertNotReached ();
                    return ConversionResult::sourceIllegal;
            }
        }
        template <typename FROM, typename TO>
        [[deprecated ("Since Stroika v3.0d1, use class UTFConverter::kThe")]] inline void
        Convert (const FROM** sourceStart, const FROM* sourceEnd, TO** targetStart, TO* targetEnd, ConversionFlags /*flags*/)
        {
            RequireNotNull (sourceStart);
            RequireNotNull (targetStart);
            Require ((static_cast<size_t> (targetEnd - *targetStart) >= QuickComputeConversionOutputBufferSize<FROM, TO> (*sourceStart, sourceEnd)));

            auto r = UTFConverter::kThe.Convert (span{*sourceStart, sourceEnd}, span{*targetStart, targetEnd});
            *sourceStart += get<0> (r);
            *targetStart += get<1> (r);
        }
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    }

    /*
     ********************************************************************************
     ******************************* CodePageConverter ******************************
     ********************************************************************************
     */
    inline CodePageConverter::CodePageConverter (CodePage codePage)
        : fHandleBOM{false}
        , fCodePage{codePage}
    {
    }
    inline CodePageConverter::CodePageConverter (CodePage codePage, [[maybe_unused]] HandleBOMFlag h)
        : fHandleBOM{true}
        , fCodePage{codePage}
    {
        Require (h == eHandleBOM);
    }
    inline bool   CodePageConverter::GetHandleBOM () const { return fHandleBOM; }
    inline void   CodePageConverter::SetHandleBOM (bool handleBOM) { fHandleBOM = handleBOM; }
    inline size_t CodePageConverter::MapToUNICODE_QuickComputeOutBufSize (const char* /*inMBChars*/, size_t inMBCharCnt) const
    {
        size_t resultSize = inMBCharCnt;
        return resultSize;
    }
    inline void CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
    {
        static_assert ((sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t)),
                       "(sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t))");
        if constexpr (sizeof (wchar_t) == sizeof (char16_t)) {
            MapToUNICODE (inMBChars, inMBCharCnt, reinterpret_cast<char16_t*> (outChars), outCharCnt);
        }
        else if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
            MapToUNICODE (inMBChars, inMBCharCnt, reinterpret_cast<char32_t*> (outChars), outCharCnt);
        }
    }
    inline void CodePageConverter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
    {
        static_assert ((sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t)),
                       "(sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t))");
        if constexpr (sizeof (wchar_t) == sizeof (char16_t)) {
            MapFromUNICODE (reinterpret_cast<const char16_t*> (inChars), inCharCnt, outChars, outCharCnt);
        }
        else if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
            MapFromUNICODE (reinterpret_cast<const char32_t*> (inChars), inCharCnt, outChars, outCharCnt);
        }
    }

    /*
     ********************************************************************************
     ************ CodePageConverter::CodePageNotSupportedException ******************
     ********************************************************************************
     */
    inline CodePage CodePageConverter::CodePageNotSupportedException::GetCodePage () const { return fCodePage_; }

    /*
     ********************************************************************************
     ****************************** CodePagesInstalled ******************************
     ********************************************************************************
     */
    inline vector<CodePage> CodePagesInstalled::GetAll () { return fCodePages_; }
    inline bool             CodePagesInstalled::IsCodePageAvailable (CodePage cp)
    {
        return (find (fCodePages_.begin (), fCodePages_.end (), cp) == fCodePages_.end ());
    }
    inline void WideStringToNarrow (const wstring& ws, CodePage codePage, string* intoResult)
    {
        RequireNotNull (intoResult);
        const wchar_t* wsp = ws.c_str ();
        WideStringToNarrow (wsp, wsp + ws.length (), codePage, intoResult);
    }
    inline string WideStringToNarrow (const wstring& ws, CodePage codePage)
    {
        string result;
        WideStringToNarrow (ws, codePage, &result);
        return result;
    }
    inline void NarrowStringToWide (const string& s, CodePage codePage, wstring* intoResult)
    {
        RequireNotNull (intoResult);
        const char* sp = s.c_str ();
        NarrowStringToWide (sp, sp + s.length (), codePage, intoResult);
    }
    inline wstring NarrowStringToWide (const string& s, CodePage codePage)
    {
        wstring result;
        NarrowStringToWide (s, codePage, &result);
        return result;
    }
    inline wstring ASCIIStringToWide (const string& s)
    {
        if constexpr (qDebug) {
            for (string::const_iterator i = s.begin (); i != s.end (); ++i) {
                Assert (isascii (*i));
            }
        }
        return wstring (s.begin (), s.end ());
    }
    inline string WideStringToASCII (const wstring& s)
    {
        if constexpr (qDebug) {
            for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
                Assert (isascii (*i));
            }
        }
        DISABLE_COMPILER_MSC_WARNING_START (4244) // 'argument': conversion from 'const wchar_t' to 'const _Elem', possible loss of data
        return string{s.begin (), s.end ()};
        DISABLE_COMPILER_MSC_WARNING_END (4244)
    }
    inline string WideStringToUTF8 (const wstring& ws) { return WideStringToNarrow (ws, kCodePage_UTF8); }
    inline void   UTF8StringToWide (const char* s, wstring* intoStr)
    {
        RequireNotNull (s);
        NarrowStringToWide (s, s + ::strlen (s), kCodePage_UTF8, intoStr);
    }
    inline void    UTF8StringToWide (const string& s, wstring* intoStr) { NarrowStringToWide (s, kCodePage_UTF8, intoStr); }
    inline wstring UTF8StringToWide (const char* s)
    {
        RequireNotNull (s);
        wstring result;
        NarrowStringToWide (s, s + ::strlen (s), kCodePage_UTF8, &result);
        return result;
    }
    inline wstring UTF8StringToWide (const string& s) { return NarrowStringToWide (s, kCodePage_UTF8); }

}

#endif /*_Stroika_Foundation_Characters_CodePage_inl_*/
