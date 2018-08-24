/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodePage_h_
#define _Stroika_Foundation_Characters_CodePage_h_ 1

#include "../StroikaPreComp.h"

#include <codecvt>
#include <string>
#include <vector>

#include "../Configuration/Common.h"
#include "../Memory/Common.h"

/**
@MODULE:    CodePage
@DESCRIPTION:
        <p>This module is designed to provide mappings between wide UNICODE and various other code pages
    and UNICODE encodings.</p>
 *
 *
 *  TODO:
 *
 *  @todo      Consdier LOSING THIS FILE - and use <codecvt>
 *
 *  (o)     FIRST - we need to fix PortableWideStringToNarrow_ and CodePageConverter to have a MORE SPACE
 *          EFFICNET (COMPUTE OUT BUF SIZE). Run basic xlate algortihm without writing to OUTPUTR, and find
 *          use that WOUDL be used.
 *
 *  (o)     THEN - test performance, and see if windows version can get away with using portable version
 *          (where codes overlap)
 *
 *  (o)     Current support for char32_t is almost totally wrong. I don't understand exactly how char16_t
 *          and char32_t are interrelated, but I'm sure its not as simplistic as what is implemented here.
 *          Probably - its that bit about Character(t).IsSurrogate() - for combiing characters. But this
 *          should be good enuf to get us started...
 */

namespace Stroika::Foundation::Characters {

    using std::byte;

    class String;

    /*
     ********************************************************************************
     *********************************** Character **********************************
     ********************************************************************************
     */
    enum {
        kCodePage_INVALID = 0xffffffff, // I hope this is always an invalid code page???

        kCodePage_ANSI = 1252,

        kCodePage_MAC             = 2,
        kCodePage_PC              = 437, //  IBM PC code page 437
        kCodePage_PCA             = 850, //  IBM PC code page 850, used by IBM Personal System/2
        kCodePage_Thai            = 874, // From uniscribe sample code (LGP 2003-01-13)
        kCodePage_SJIS            = 932,
        kCodePage_GB2312          = 936, // Chinese (Simplified)
        kCodePage_Korean          = 949, // Korean
        kCodePage_BIG5            = 950, // Chinese (Traditional)
        kCodePage_EasternEuropean = 1250,
        kCodePage_CYRILIC         = 1251, // Russian (Cyrilic)
        kCodePage_GREEK           = 1253,
        kCodePage_Turkish         = 1254,
        kCodePage_HEBREW          = 1255,
        kCodePage_ARABIC          = 1256,
        kCodePage_Baltic          = 1257,
        kCodePage_Vietnamese      = 1258,

        kCodePage_UNICODE_WIDE           = 1200, // Standard UNICODE for MS-Windows
        kCodePage_UNICODE_WIDE_BIGENDIAN = 1201,

        kCodePage_UTF7 = 65000,
        kCodePage_UTF8 = 65001

/// NOT CURRENTLY USED - SO DONT DEFINE CUZ I DONT UNDERSTAND! kCodePage_OEM        =   1,                  // What does this mean??? PC ONLY???
#if 0
                                                // Only supported on Win2K
                                                kCodePage_SYMBOL    =   42,
#endif
    };

    /*
        *       <p>A codePage is a Win32 (really DOS) concept which describes a particular single or
        *    multibyte (narrow) character set encoding. Use Win32 CodePage numbers. Maybe someday add
        *  a layer to map to/from Mac 'ScriptIDs' - which are basicly analagous.</p>
        *      <p>Use this with @'CodePageConverter'.</p>
        */
    using CodePage = int;

    /*
        * TODO:
        *      Returns a printable (DEFINE CAREFULLY - BUT CHARSET USED IN HTTP CONTENT TYPE STRINGS)
        *
    */
    wstring GetCharsetString (CodePage cp);

    /*
    @METHOD:        GetDefaultSDKCodePage
    @DESCRIPTION:   <p>Returns the assumed code page of @'Led_SDK_Char'.</p>
    */
    CodePage GetDefaultSDKCodePage ();

    /*
    @CLASS:         CodePageConverter
    @DESCRIPTION:
            <p>Helper class to wrap conversions between code pages (on Mac known as scripts)
        and UTF-16 (WIDE UNICODE).</p>
    */
    class CodePageConverter {
    public:
        class CodePageNotSupportedException;
        enum HandleBOMFlag { eHandleBOM };

    public:
        CodePageConverter (CodePage codePage);
        CodePageConverter (CodePage codePage, HandleBOMFlag h);

    public:
        /*
        @METHOD:        CodePageConverter::GetHandleBOM
        @DESCRIPTION:   <p>In UNICODE, files are generally headed by a byte order mark (BOM). This
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
        /*
        @METHOD:        CodePageConverter::SetHandleBOM
        @DESCRIPTION:   <p>See also @'CodePageConverter::GetHandleBOM'.</p>
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

    // Coming IN, the 'outCharCnt' is the buffer size of outChars, and coming OUT it is hte actual
    // number of characters written to outChars. This function will NUL-terminate the outChars iff inMBChars
    // was NUL-terminated (and there is enough space in the buffer).
    void MapSBUnicodeTextWithMaybeBOMToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);

    class CodePageConverter::CodePageNotSupportedException {
    public:
        CodePageNotSupportedException (CodePage codePage);

    public:
        CodePage fCodePage;
    };

    /*
    @CLASS:         UTF8Converter
    @DESCRIPTION:
            <p>Helper class to wrap conversions between between UTF8 and wide-character UNICODE.</p>
    */
    class UTF8Converter {
    public:
        nonvirtual void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const;
        nonvirtual void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const;
        nonvirtual void MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;

        nonvirtual size_t MapToUNICODE_QuickComputeOutBufSize (const char* inMBChars, size_t inMBCharCnt) const;

        nonvirtual void MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
        nonvirtual void MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
        nonvirtual void MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;

        nonvirtual size_t MapFromUNICODE_QuickComputeOutBufSize (const char16_t* inChars, size_t inCharCnt) const;
        nonvirtual size_t MapFromUNICODE_QuickComputeOutBufSize (const char32_t* inChars, size_t inCharCnt) const;
        nonvirtual size_t MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const;
    };

    /*
    @CLASS:         CodePagesInstalled
    @DESCRIPTION:
                <p>Helper class to check what code pages are installed on a given machine.</p>
    */
    class CodePagesInstalled {
    public:
        CodePagesInstalled ();

    public:
        /*
        @METHOD:        CodePagesInstalled::GetAll
        @DESCRIPTION:   <p>Returns a list of all code pages installed on the system.
                    This list is returned in sorted order.</p>
        */
        vector<CodePage> GetAll ();

        /*
        @METHOD:        CodePagesInstalled::IsCodePageAvailable
        @DESCRIPTION:   <p>Checks if the given code page is installed.</p>
        */
        bool IsCodePageAvailable (CodePage cp);

    private:
        vector<CodePage> fCodePages_;
    };

    /*
    @CLASS:         CodePagesGuesser
    @DESCRIPTION:   <p>Guess the code page of the given argument text.</p>
    */
    class CodePagesGuesser {
    public:
        enum class Confidence : uint8_t { eLow    = 0,
                                          eMedium = 10,
                                          eHigh   = 100 };

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

    /*
    @CLASS:         CodePagePrettyNameMapper
    @DESCRIPTION:   <p>Code to map numeric code pages to symbolic user-interface appropriate names.</p>
    */
    class CodePagePrettyNameMapper {
    public:
        static wstring GetName (CodePage cp);

    public:
        struct CodePageNames;

        // This class builds commands with command names. The UI may wish to change these
        // names (eg. to customize for particular languages, etc)
        // Just patch these strings here, and commands will be created with these names.
        // (These names appear in text of undo menu item)
    public:
        static CodePageNames GetCodePageNames ();
        static void          SetCodePageNames (const CodePageNames& cmdNames);
        static CodePageNames MakeDefaultCodePageNames ();

    private:
        static CodePageNames sCodePageNames_;
    };
    struct CodePagePrettyNameMapper::CodePageNames {
        wstring fUNICODE_WIDE;
        wstring fUNICODE_WIDE_BIGENDIAN;
        wstring fANSI;
        wstring fMAC;
        wstring fPC;
        wstring fSJIS;
        wstring fUTF7;
        wstring fUTF8;
        wstring f850;
        wstring f851;
        wstring f866;
        wstring f936;
        wstring f949;
        wstring f950;
        wstring f1250;
        wstring f1251;
        wstring f10000;
        wstring f10001;
        wstring f50220;
    };

    void    WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult);
    void    WideStringToNarrow (const wstring& ws, CodePage codePage, string* intoResult);
    string  WideStringToNarrow (const wstring& ws, CodePage codePage);
    void    NarrowStringToWide (const char* sStart, const char* sEnd, CodePage codePage, wstring* intoResult);
    void    NarrowStringToWide (const string& s, CodePage codePage, wstring* intoResult);
    wstring NarrowStringToWide (const string& s, CodePage codePage);

    string  WideStringToUTF8 (const wstring& ws);
    void    UTF8StringToWide (const char* s, wstring* intoStr);
    void    UTF8StringToWide (const string& s, wstring* intoStr);
    wstring UTF8StringToWide (const char* ws);
    wstring UTF8StringToWide (const string& ws);

    wstring ASCIIStringToWide (const string& s);
    string  WideStringToASCII (const wstring& s);

    wstring      MapUNICODETextWithMaybeBOMTowstring (const char* start, const char* end);
    vector<byte> MapUNICODETextToSerializedFormat (const wchar_t* start, const wchar_t* end, CodePage useCP = kCodePage_UTF8); // suitable for files

    /**
     *  Convert between CHAR_TYPE and multibyte charactersets.
     *  For now - only supported for CHAR_TYPE=wchar_t, and 
     *      codePageName (case insensitive equals)
     *          utf-8
     *          ISO-8859-1
     */
    template <typename CHAR_TYPE>
    const codecvt<CHAR_TYPE, char, mbstate_t>& LookupCodeConverter (const String& charset);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CodePage.inl"

#endif /*_Stroika_Foundation_Characters_CodePage_h_*/
