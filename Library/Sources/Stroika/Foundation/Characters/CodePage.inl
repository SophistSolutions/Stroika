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

    class [[deprecated ("Since Stroika v3.0d2, use ReadByteOrderMark")]] CodePagesGuesser
    {
    public:
        enum class Confidence : uint8_t {
            eLow    = 0,
            eMedium = 10,
            eHigh   = 100
        };

    public:
        /*
        @METHOD:        CodePagesGuesser::Guess
        @DESCRIPTION:   <p>Guess the code page of the given snippet of text. Return that codepage.
                    Always make some guess, and return the level of quality of the guess in the
                    optional parameter 'confidence' - unless its nullptr (which it is by default),
                    and return the number of bytes of BOM (byte-order-mark) prefix to strip from
                    the source in 'bytesFromFrontToStrip' unless it is nullptr (which it is by
                    default).
                    </p>
        */
        nonvirtual CodePage Guess (const void* input, size_t nBytes, Confidence* confidence = nullptr, size_t* bytesFromFrontToStrip = nullptr);
    };

    /// <summary>
    /// DEPRECATED
    /// </summary>
    enum {
        kCodePage_INVALID [[deprecated ("Since v3.0d2 - deprecated - use optional")]] = 0xffffffff, // I hope this is always an invalid code page???

        kCodePage_ANSI [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kANSI")]] = WellKnownCodePages::kANSI,

        kCodePage_MAC [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kMAC")]]       = WellKnownCodePages::kMAC,
        kCodePage_PC [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kPC")]]         = WellKnownCodePages::kPC,
        kCodePage_PCA [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kPCA")]]       = WellKnownCodePages::kPCA,
        kCodePage_Thai [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kThai")]]     = WellKnownCodePages::kThai,
        kCodePage_SJIS [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kSJIS")]]     = WellKnownCodePages::kSJIS,
        kCodePage_GB2312 [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kGB2312")]] = WellKnownCodePages::kGB2312,
        kCodePage_Korean [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kKorean")]] = WellKnownCodePages::kKorean,
        kCodePage_BIG5 [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kBIG5")]]     = WellKnownCodePages::kBIG5,
        kCodePage_EasternEuropean [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kEasternEuropean")]] =
            WellKnownCodePages::kEasternEuropean,
        kCodePage_CYRILIC [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kCyrilic")]] = WellKnownCodePages::kCyrilic,
        kCodePage_GREEK [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kGreek")]]     = WellKnownCodePages::kGreek,
        kCodePage_Turkish [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kTurkish")]] = WellKnownCodePages::kTurkish,
        kCodePage_HEBREW [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kHebrew")]]   = WellKnownCodePages::kHebrew,
        kCodePage_ARABIC [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kArabic")]]   = WellKnownCodePages::kArabic,
        kCodePage_Baltic [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kBaltic")]]   = WellKnownCodePages::kBaltic,
        kCodePage_Vietnamese [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kVietnamese")]] = WellKnownCodePages::kVietnamese,

        kCodePage_UNICODE_WIDE [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kUNICODE_WIDE")]] = WellKnownCodePages::kUNICODE_WIDE,
        kCodePage_UNICODE_WIDE_BIGENDIAN [[deprecated ("Since v3.0d2 - deprecated - use WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN")]] =
            WellKnownCodePages::kUNICODE_WIDE_BIGENDIAN,

        kCodePage_UTF7 [[deprecated ("Since v3.0d2 - UTF-7 deprecated")]]          = 65000,
        kCodePage_UTF8 [[deprecated ("Since v3.0d2 - WellKnownCodePages::kUTF8")]] = WellKnownCodePages::kUTF8
    };

    class [[deprecated ("Since Stroika v3.0d2, use CodeCvt")]] CodePageConverter
    {
    public:
        enum class HandleBOMFlag {
            eHandleBOM
        };
        static constexpr HandleBOMFlag eHandleBOM = HandleBOMFlag::eHandleBOM;

    public:
        /**
         */
        CodePageConverter (CodePage codePage);
        CodePageConverter (CodePage codePage, HandleBOMFlag h);

    public:
        /**
         *         <p>In UNICODE, files are generally headed by a byte order mark (BOM). This
                mark is used to indicate if the file is big endian, or little-endian (if the
                characters are wide-characters). This is true for 2 and 4 byte UNICODE (UCS-2, UCS-4)
                UNICODE, as well as for UTF-X encodings (such as UTF-7 and UTF-8). It is also used
                to indicate whether or not the file is in a UTF encoding (as byte order doesn't matter
                in any (most?) of the UTF encodings.</p>
                    <p>The basic rubrick for BOM's is that they are the character 0xfeff, as it would
                be encoded in the given UTF or UCS encoding.</p>
                    <p>Because of this type of encoding - if you have a 0xfeff character (after
                decoding) at the beginning of a buffer, there is no way for this routine to know if
                that was REALLY there, or if it was byte order mark. And its not always desirable for
                the routine producing these encodings to produce the byte order mark, but sometimes
                its highly desirable. So - this class lets you get/set a flag to indicate whether or not
                to process BOMs on input, and whether or not to generate them on encoded outputs.
                </p>
                    <p>See also @'CodePageConverter::SetHandleBOM', and note that there is an
                overloaded CTOR that lets you specify CodePageConverter::eHandleBOM as a final
                argument to automatically set this BOM converter flag.</p>
        */
        nonvirtual bool GetHandleBOM () const;
        /**
         * See also @'CodePageConverter::GetHandleBOM'.</p>
         */
        nonvirtual void SetHandleBOM (bool handleBOM);

    private:
        bool fHandleBOM;

    public:
        /** 
         *  Map the given multibyte chars in the fCodePage codepage into wide UNICODE
         *  characters. Pass in a buffer 'outChars' of
         *  size large enough to accomodate those characrters.</p>
         *   
         *  'outCharCnt' is the size of the output buffer coming in, and it contains the number
         *  of UNICODE chars copied out on return.</p>
         */
        nonvirtual void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const;
        nonvirtual void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const;
        nonvirtual void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;

        /*
        @METHOD:        CodePageConverter::MapToUNICODE_QuickComputeOutBufSize
        @DESCRIPTION:   <p>Call to get an upper bound, reasonable buffer size to use to pass to
                    @'CodePageConverter::MapToUNICODE' calls.</p>
        */
        nonvirtual size_t MapToUNICODE_QuickComputeOutBufSize (const char* inMBChars, size_t inMBCharCnt) const;

        nonvirtual void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
        nonvirtual void MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
        nonvirtual void MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;

        /*
        @METHOD:        CodePageConverter::MapFromUNICODE_QuickComputeOutBufSize
        @DESCRIPTION:   <p>Call to get an upper bound, reasonable buffer size to use to pass to MapFromUNICODE calls.</p>
        */
        nonvirtual size_t MapFromUNICODE_QuickComputeOutBufSize (const char16_t* inChars, size_t inCharCnt) const;
        nonvirtual size_t MapFromUNICODE_QuickComputeOutBufSize (const char32_t* inChars, size_t inCharCnt) const;
        nonvirtual size_t MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const;

    private:
        CodePage fCodePage;
    };

    [[deprecated ("Since Stroika v3.0d2 - use CodeCvt")]] inline void MapSBUnicodeTextWithMaybeBOMToUNICODE (const char* inMBChars, size_t inMBCharCnt,
                                                                                                             wchar_t* outChars, size_t* outCharCnt)
    {
        DISABLE_COMPILER_MSC_WARNING_START (4996);
        DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        RequireNotNull (outChars);
        RequireNotNull (outCharCnt);
        [[maybe_unused]] size_t      outBufSize = *outCharCnt;
        CodePagesGuesser::Confidence confidence = CodePagesGuesser::Confidence::eLow;
        CodePage                     cp         = CodePagesGuesser{}.Guess (inMBChars, inMBCharCnt, &confidence, nullptr);
        if (confidence <= CodePagesGuesser::Confidence::eLow) {
            cp = WellKnownCodePages::kUTF8;
        }
        CodePageConverter cpCvt (cp, CodePageConverter::eHandleBOM);
        cpCvt.MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
        Ensure (*outCharCnt <= outBufSize);
        DISABLE_COMPILER_MSC_WARNING_END (4996);
        DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
        DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    }

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
        return WellKnownCodePages::kUTF8; // So far - this is meaningless on other systems - but this would be the best guess I think
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
    inline CodePage CodePageNotSupportedException::GetCodePage () const { return fCodePage_; }

    /*
     ********************************************************************************
     ****************************** CodePagesInstalled ******************************
     ********************************************************************************
     */
    inline vector<CodePage> CodePagesInstalled::GetAll () { return fCodePages_; }
    inline bool             CodePagesInstalled::IsCodePageAvailable (CodePage cp)
    {
        return find (fCodePages_.begin (), fCodePages_.end (), cp) == fCodePages_.end ();
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
    inline string WideStringToUTF8 (const wstring& ws) { return WideStringToNarrow (ws, WellKnownCodePages::kUTF8); }
    inline void   UTF8StringToWide (const char* s, wstring* intoStr)
    {
        RequireNotNull (s);
        NarrowStringToWide (s, s + ::strlen (s), WellKnownCodePages::kUTF8, intoStr);
    }
    inline void    UTF8StringToWide (const string& s, wstring* intoStr) { NarrowStringToWide (s, WellKnownCodePages::kUTF8, intoStr); }
    inline wstring UTF8StringToWide (const char* s)
    {
        RequireNotNull (s);
        wstring result;
        NarrowStringToWide (s, s + ::strlen (s), WellKnownCodePages::kUTF8, &result);
        return result;
    }
    inline wstring UTF8StringToWide (const string& s) { return NarrowStringToWide (s, WellKnownCodePages::kUTF8); }

    [[deprecated ("Since Stroika v3.0d2 - use CodeCvt")]] vector<byte>
    MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP = WellKnownCodePages::kUTF8); // suitable for files

    [[deprecated ("Since Stroika v3.0d2 - use CodeCvt")]] wstring MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end);

}

#endif /*_Stroika_Foundation_Characters_CodePage_inl_*/
