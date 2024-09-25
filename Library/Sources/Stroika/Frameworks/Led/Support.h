/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Support_h_
#define _Stroika_Frameworks_Led_Support_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <new>
#include <string>
#include <vector>

#if qPlatform_Windows
#include <Windows.h> //

#include <oaidl.h> // for SAFEARRAY
#include <tchar.h>
#elif qStroika_FeatureSupported_XWindows
#include <X11/X.h>
#include <X11/Xatom.h>
#endif

#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Endian.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/CompileTimeFlagChecker.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/Memory/Common.h"
#include "Stroika/Foundation/Time/Realtime.h"
#include "Stroika/Frameworks/Led/Config.h"

#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/HRESULTErrorException.h"
#endif

/*
@MODULE:    LedSupport
@DESCRIPTION:
        <p>Support defines, and utility functions that should usually be provided
    by some other class library (e.g. Stroika, TCL, MFC, OWL, etc. These
    defines should just thunk down to the appropriate class library defines -
    where possible.</p>
 */
namespace Stroika {
    using namespace std;
    using std::byte;

    using namespace Stroika::Foundation;
}

namespace Stroika::Frameworks::Led {

    using Foundation::Characters::SDKString;

    constexpr size_t kBadIndex = size_t (-1);

#if qPlatform_Windows
    using Foundation::Execution::Platform::Windows::ThrowIfErrorHRESULT;
#endif

    /*
    @CLASS:         Led_tChar
            @todo LOSE THIS TYPE!!!!  - use Stroika::FoundationL::Character::Charactrer

                   STILL need define for chararcter encoding version of GDI API (qTargetPlatformSDKUseswchar_t)

                   For now - use wchar_t - since thats what I did for a long time.

                   But Stroika Character better.

                   ANd in TextStore - do same tricks I do with String - have one REP where size is one-byte (ISOLATIN) - and one case where size is char16_t (no surrogagtes)
                   and one case for char32_t. THen copyOut magic does right mapping automatically (or have Peek API like I do for String).

                   REally - TONS of APIs that use const Led_tChar* should be replaced with span<const Character>..


    #if 0
    @DESCRIPTION:   <p>There are lots of different ways of encoding character sets. And it is
        a goal for Led to be able to support them all. The three major ways are wide characters,
        single-byte characters, and multibyte character strings.</p>
            <p><code>Led_tChar</code> is a basic type used throughout Led. In the
        case of single byte character systems it is a <code>char</code>, and this also corresponds
        to a character. In mutlibyte character systems, it is also of type <code>char</code>,
        but then it does NOT correspond to a character. Rather some consecutive sequence
        of <code>Led_tChar</code>s specify a character. And for wide character sets (e.g UNICODE),
        <code>Led_tChar</code> is of type <code>wchar_t</code>, and - like with single byte
        character sets - does contain a full character.</p>

            <p>Now simply supporting one of these would be easy. Support all three within the
        same code base requires a small amount of discipline. It requires that we never assume that
        <code> sizeof (char) == sizeof (Led_tChar)</code> - because that supposed equality <b>is not true</b>.
        And it requires that we not assume that every possible <code>Led_tChar</code> pointer
        refers to a valid character beginning. Instead, we must use utility routines
        <code><a href=#Led_MutlibyteCharRoutines>Led_NextChar</a></code> and
        <code><a href=#Led_MutlibyteCharRoutines>Led_PreviousChar</a></code> to navigate through
        <code>Led_tChar*</code> strings.</p>

            <p>All of Led's APIs which refer to characters or bytes or any such thing <em>really</em>
        refer to <code>Led_tChar</code>s (actually, typically <code>Led_tChar*</code>).
        All of Led's APIs gaurantee (unless otherwise noted) that they will never split a character
        (return a character offset which points in the middle of a multibyte character).
        And all of Led's APIs similarly <em>require</em> that all offsets and strings passed to
        it consist of valid character boundaries.
        You can always use (and Led does many asserts internally)
        <code>ValidateTextForCharsetConformance()</code> to check / validate a string to make
        sure it is valid with respect to the character set you are using (for example doesn't end
        spliting a multibyte character).</p>
        #endif
    */
#if defined(__cplusplus)
    using Led_tChar = wchar_t;
#endif

/*
    @CLASS:         LED_TCHAR_OF
    @DESCRIPTION:   <p>Like the Win32SDK macro _T(). Except is based on the Led type @'Led_tChar', and the Led
        macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
    */
#define LED_TCHAR_OF__(X) L##X
#define LED_TCHAR_OF(X) LED_TCHAR_OF__ (X)

    constexpr wchar_t kNonBreakingSpace = 0x00a0;
    constexpr wchar_t kZeroWidthSpace   = 0x200b;
    constexpr wchar_t kPoundSign        = 0x00a3;
    constexpr wchar_t kCentSign         = 0x00a2;
    constexpr wchar_t kYenSign          = 0x00a5;

    /*
    @CLASS:         Led_tString
    @DESCRIPTION:   <p>This expands to either the ANSI C++ string class library class <code>string</code>
        or <code>wstring</code> - depending on whether we are using UNICODE or not. It is a <code>basic_string</code>
        templated on @'Led_tChar'.</p>
    */
    using Led_tString = basic_string<Led_tChar>;

    /*
    @METHOD:        Led_tStrlen
    @DESCRIPTION:   <p>Expands to any of the ANSI C++ functions, std::strlen ()/wcslen/_mbstrlen(not ansiC++), depending on
        macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
    */
    size_t Led_tStrlen (const Led_tChar* s);

    /*
    @METHOD         Led_tStrCmp
    @DESCRIPTION:   <p>Expands to any of the ANSI C++ functions, std::strcmp/etc, depending on
        macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
    */
    int Led_tStrCmp (const Led_tChar* l, const Led_tChar* r);

    /*
    @METHOD:        Led_tStrnCmp
    @DESCRIPTION:   <p>Expands to any of the ANSI C++ functions, std::strncmp/etc, depending on
        macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
    */
    int Led_tStrnCmp (const Led_tChar* l, const Led_tChar* r, size_t n);

    /*
    @METHOD:        Led_tStrniCmp
    @DESCRIPTION:   <p>Expands to any of the ANSI C++ functions, std::strnicmp/etc, depending on
        macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'. See also @'Led_tStriCmp'.</p>
    */
    int Led_tStrniCmp (const Led_tChar* l, const Led_tChar* r, size_t n);

    /*
    @METHOD:        Led_tStriCmp
    @DESCRIPTION:   <p>Expands to any of the ANSI C++ functions, std::stricmp/etc, depending on
        macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'. See also @'Led_tStrniCmp'.</p>
    */
    int Led_tStriCmp (const Led_tChar* l, const Led_tChar* r);

    /*
    @METHOD:        Led_tStrChr
    @DESCRIPTION:   <p>Expands to any of the ANSI C++ functions, std::strchr/etc, depending on
        macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
    */
    const Led_tChar* Led_tStrChr (const Led_tChar* s, Led_tChar c);

/*
    @CLASS:         Led_SDK_TCHAROF
    @DESCRIPTION:   <p>Like the Win32SDK macro _T(). See @'SDKString'.</p>
    */
#if qTargetPlatformSDKUseswchar_t
#define Led_SDK_TCHAROF(X) LED_TCHAR_OF__ (X)
#else
#define Led_SDK_TCHAROF(X) X
#endif

    /*
    @METHOD:        Led_tString2SDKString
    @DESCRIPTION:
    */
    SDKString Led_tString2SDKString (const Led_tString& s);

    /*
    @METHOD:        Led_WideString2tString
    @DESCRIPTION:
    */
    Led_tString Led_WideString2tString (const wstring& s);

    /*
    @METHOD:        Led_SDKString2tString
    @DESCRIPTION:
    */
    Led_tString Led_SDKString2tString (const SDKString& s);

    /*
    @METHOD:        Led_ANSIString2tString
    @DESCRIPTION:
    */
    Led_tString Led_ANSIString2tString (const string& s);

    /*
    @METHOD:        Led_tString2ANSIString
    @DESCRIPTION:
    */
    string Led_tString2ANSIString (const Led_tString& s);

    /*
    @METHOD:        Led_tString2WideString
    @DESCRIPTION:
    */
    wstring Led_tString2WideString (const Led_tString& s);

    short          Led_ByteSwapFromMac (short src);
    unsigned short Led_ByteSwapFromMac (unsigned short src);

    short          Led_ByteSwapFromWindows (short src);
    unsigned short Led_ByteSwapFromWindows (unsigned short src);
    long           Led_ByteSwapFromWindows (long src);
    unsigned long  Led_ByteSwapFromWindows (unsigned long src);

    void     UInt16ToBuf (uint16_t u, uint16_t* realBuf);
    uint16_t BufToUInt16 (const uint16_t* realBuf);
    void     UInt32ToBuf (uint32_t ul, uint32_t* realBuf);
    uint32_t BufToUInt32 (const uint32_t* buf);

    /*
     *  Store as UInt32, but throw if won't fit
     */
    void   SizeTToBuf (size_t ul, uint32_t* realBuf);
    size_t BufToSizeT (const uint32_t* buf);

    size_t Led_SkrunchOutSpecialChars (Led_tChar* text, size_t textLen, Led_tChar charToRemove);

    /*
    @METHOD:        PUSH_BACK
    @DESCRIPTION:   <p></p>
    */
    template <class VECTOR>
    inline void PUSH_BACK (VECTOR& v, const typename VECTOR::value_type& e)
    {
        size_t vSize = v.size ();
        size_t vCap  = v.capacity ();
        Assert (vSize <= vCap);
        if (vSize == vCap) {
            size_t newCap = vSize * 2;
            newCap        = max (newCap, size_t (4));
            v.reserve (newCap); // grow by factor of 2 to assure logarithmic # of copies rather than quadradic
        }
        v.push_back (e);
    }

    /*
    @CLASS:         DiscontiguousRunElement<DATA>
    @DESCRIPTION:   <p>We frequently need to keep track of runs of data. Sometimes - those elements are discontiguous.
        This template is merely used to give us a uniform naming convention for how we deal with those runs.</p>
            <p>These elements are meant to be used with @'DiscontiguousRun<DATA>'.</p>
    */
    template <typename DATA>
    struct DiscontiguousRunElement {
        DiscontiguousRunElement (size_t offset, size_t length)
            : fOffsetFromPrev{offset}
            , fElementLength{length}
        {
        }
        DiscontiguousRunElement (size_t offset, size_t length, const DATA& data)
            : fOffsetFromPrev{offset}
            , fElementLength{length}
            , fData{data}
        {
        }

        size_t fOffsetFromPrev;
        size_t fElementLength;
        DATA   fData{};
    };
    template <>
    struct DiscontiguousRunElement<void> {
        DiscontiguousRunElement (size_t offset, size_t length)
            : fOffsetFromPrev{offset}
            , fElementLength{length}
        {
        }
        size_t fOffsetFromPrev;
        size_t fElementLength;
    };

    /*
    @CLASS:         DiscontiguousRun<DATA>
    @DESCRIPTION:   <p>An ordered list of @'DiscontiguousRunElement<DATA>'. The ordering is very important. The 'fOffsetFromPrev'
        is the offset in marker-positions the data is intended to be from the end of the previous element. For the first
        element - this if the offset from the 'beginning'.</p>
            <p>NB: I would PREFER to have declared this as a typedef - so I would get the vector<> ctors for free. But
        I couldn't figure out how to declare it in a way that made MSVC60 happy. So I settled for this.</p>
    */
    template <typename DATA>
    struct DiscontiguousRun : vector<DiscontiguousRunElement<DATA>> {};

    /*
     *  These routines can be used either with single byte, multibyte, or wide
     *  characters. They are intended to insulate most of the code from having
     *  to worry about this.
     */
    /*
    @METHOD:        Led_NextChar
    @DESCRIPTION:   <p>See @'Led_tChar'</p>

            @todo DEPRECATE - NO POINT IN THIS API ANY LONGER -- LGP 2024-02-12
    */
    Led_tChar*       Led_NextChar (Led_tChar* fromHere);
    const Led_tChar* Led_NextChar (const Led_tChar* fromHere);

    /*
    @METHOD:        Led_PreviousChar
    @DESCRIPTION:   <p>See @'Led_tChar'</p>

            @todo DEPRECATE - NO POINT IN THIS API ANY LONGER -- LGP 2024-02-12
    */
    Led_tChar*       Led_PreviousChar (Led_tChar* startOfString, Led_tChar* fromHere);
    const Led_tChar* Led_PreviousChar (const Led_tChar* startOfString, const Led_tChar* fromHere);

    /*
    @METHOD:        ValidateTextForCharsetConformance
    @DESCRIPTION:   <p>See @'Led_tChar'</p>

            @todo DEPRECATE - NO POINT IN THIS API ANY LONGER -- LGP 2024-02-12
    */
    bool ValidateTextForCharsetConformance (const Led_tChar* text, size_t length); // just return true or false - no other diagnostics

    unsigned Led_DigitCharToNumber (char digitChar);      // require input is valid decimal digit
    char     Led_NumberToDigitChar (unsigned digitValue); // require input is valid decimal digit value

    bool Led_CasedCharsEqual (char lhs, char rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const string& lhs, const string& rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const string_view& lhs, const string_view& rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const string_view& lhs, const string& rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const string& lhs, const string_view& rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const char* lhs, const string& rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const string& lhs, const char* rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const char* lhs, const string_view& rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const string_view& lhs, const char* rhs, bool ignoreCase = true);
    bool Led_CasedStringsEqual (const char* lhs, const char* rhs, bool ignoreCase = true);

/*
     *  Clipboard access support.
     *
     *      Note - when using this on the PC - be sure clipboard opened/closed
     *  surrounding access to this object - as is currently done in Led_MFC.
     *
     */
#ifndef qStroika_Frameworks_Led_SupportClipboard
#define qStroika_Frameworks_Led_SupportClipboard (qPlatform_Windows or qStroika_FeatureSupported_XWindows)
#endif

#if qPlatform_Windows
    using Led_ClipFormat = CLIPFORMAT;
#elif qStroika_FeatureSupported_XWindows
    using Led_ClipFormat = long;
#else
    // used in a lot of places to exchange info, even if not to a real clipboad, so keep type defined for now ... --LGP 2024-02-11
    enum Led_ClipFormat : unsigned short {
    };
#endif
#if qPlatform_Windows
    const Led_ClipFormat kTEXTClipFormat = CF_UNICODETEXT;
    //  const Led_ClipFormat    kPICTClipFormat =   CF_METAFILEPICT;
    const Led_ClipFormat kPICTClipFormat = CF_DIB;
    const Led_ClipFormat kFILEClipFormat = CF_HDROP;
#elif qStroika_FeatureSupported_XWindows
    const Led_ClipFormat kTEXTClipFormat = XA_STRING;
    const Led_ClipFormat kFILEClipFormat = 1; // X-TMP-HACK-LGP991213 - not sure what this should be???
#else
    const Led_ClipFormat kTEXTClipFormat = (Led_ClipFormat)1;
    const Led_ClipFormat kFILEClipFormat = (Led_ClipFormat)2;
#endif
    const Led_ClipFormat kBadClipFormat = (Led_ClipFormat)(0);

#if qStroika_Frameworks_Led_SupportClipboard
    class Led_ClipboardObjectAcquire {
    public:
        Led_ClipboardObjectAcquire (Led_ClipFormat clipType);
        ~Led_ClipboardObjectAcquire ();

        static bool FormatAvailable_TEXT ();
        static bool FormatAvailable (Led_ClipFormat clipType);

        nonvirtual bool   GoodClip () const; // avoid exceptions usage for now - and just use flag
        nonvirtual void*  GetData () const;
        nonvirtual size_t GetDataLength () const;

    private:
#if qPlatform_Windows
        HANDLE fOSClipHandle;
#endif
        void* fLockedData;
    };
#endif

    void Led_BeepNotify ();
    Foundation::Time::DurationSeconds Led_GetDoubleClickTime (); // time-interval which defines how quick we consider two consecutive clicks a dbl-click

#if qStroika_FeatureSupported_XWindows
    extern void   (*gBeepNotifyCallBackProc) ();
    unsigned long LedTickCount2XTime (float ledTickCount);
    void          SyncronizeLedXTickCount (unsigned long xTickCount);
#endif

#if qPlatform_Windows
    class VariantArrayPacker {
    public:
        VariantArrayPacker (VARIANT* v, VARTYPE vt, size_t nElts);
        ~VariantArrayPacker ();

    public:
        nonvirtual void* PokeAtData () const;

    private:
        VARIANT* fSafeArrayVariant{nullptr};
        void*    fPtr{nullptr};
    };

    class VariantArrayUnpacker {
    public:
        VariantArrayUnpacker (const VARIANT& v);
        ~VariantArrayUnpacker ();

    public:
        nonvirtual const void* PeekAtData () const;
        nonvirtual VARTYPE     GetArrayElementType () const;
        nonvirtual size_t      GetLength () const;

    private:
        SAFEARRAY*  fSafeArray;
        const void* fPtr;
    };
#endif

#if qPlatform_Windows
    VARIANT CreateSafeArrayOfBSTR (const wchar_t* const* strsStart, const wchar_t* const* strsEnd);
    VARIANT CreateSafeArrayOfBSTR (const vector<const wchar_t*>& v);
    VARIANT CreateSafeArrayOfBSTR (const vector<wstring>& v);

    vector<wstring> UnpackVectorOfStringsFromVariantArray (const VARIANT& v);
#endif

#if qPlatform_Windows
    void DumpSupportedInterfaces (IUnknown* obj, const char* objectName = nullptr, const char* levelPrefix = nullptr);
    void DumpObjectsInIterator (IEnumUnknown* iter, const char* iteratorName = nullptr, const char* levelPrefix = nullptr);
#endif

    /*
    @CLASS:         Led_URLD
    @DESCRIPTION:   <p><code>Led_URLD</code> is an abstraction of the Netscape Web Browser URL object.
        At least thats where the name comes from. And some of its more quirky attributes.
        This object has two attributes, a URL and a title.
        It has accessors to convert it into and from the Netscape URLD object format (URLD on mac
        and "Netscape Bookmark" on windows; same thing).
        It also can easily be used for dealing with URLs with ActiveX/Microsoft URL representations.</p>
            <p>See also, @'Led_URLManager'</p>
    */
    struct Led_URLD {
        Led_URLD (const char* url, const char* title);
        Led_URLD (const void* urlData, size_t nBytes);

        nonvirtual size_t GetURLDLength () const;  // return byte count of URLD itself (includes NUL-byte at end)
        nonvirtual size_t GetURLLength () const;   // return byte count of URL itself
        nonvirtual size_t GetTitleLength () const; // return byte count of title

        nonvirtual char* PeekAtURLD () const;
        nonvirtual char* PeekAtURL () const;   // NB: doesn't return NUL-term string! - check GetURLLength
        nonvirtual char* PeekAtTitle () const; // NB: doesn't return NUL-term string! - check GetTitleLength

        nonvirtual string GetTitle () const;
        nonvirtual string GetURL () const;

        vector<char> fData;
    };

    /*
    @CLASS:         Led_URLManager
    @DESCRIPTION:   <p><code>Led_URLManager</code> is a utility class to deal with opening URLs.
        Depending on the OS you are running Led with, it can be configured to use
        <em>DDE</em>, <em>ActiveX</em>, <em>ShellExec</em>, <em>AppleEvents</em>, or
        <em>Internet Config (IC)</em>.
        By default, it uses all that make sense for the given operating system in some reasonable
        order (prefering more robust approaches, and those that work more frequently and those that
        report better errors).</p>
            <p>This class also provides a utility - <code>FileSpecToURL</code> - to make a URL object
        from a full path name.</p>
            <p>See also, @'Led_URLD'</p>
    */
    class Led_URLManager {
    public:
        Led_URLManager ()          = default;
        virtual ~Led_URLManager () = default;

    public:
        static Led_URLManager& Get (); // by default builds one, but you can specify your
        // own. NB: deletes old value, so if you specify
        // your own, be sure to allocate it with new so it
        // can be deleted...
        static void Set (Led_URLManager* newURLMgr);

    public:
        virtual void   Open (const string& url); // throws on detected errors
        virtual string FileSpecToURL (const filesystem::path& p);

    protected:
#if qUseActiveXToOpenURLs
        nonvirtual void Open_ActiveX (const string& url);
#endif
#if qUseSpyglassDDESDIToOpenURLs
        nonvirtual void Open_SpyglassDDE (const string& url);
#endif

#if qUseSpyglassDDESDIToOpenURLs
    public:
        static void InitDDEHandler (); // to be able to open URLs with DDE this must be called, but
        // it takes over all DDE processing, disabling your app from doing
        // any other DDE.

        // Use this for more low level extensions of the DDE support...
        static DWORD             sDDEMLInstance; //  The DDEML instance identifier.
        static HDDEDATA CALLBACK SimpleDdeCallBack (UINT /*type*/, UINT /*fmt*/, HCONV /*hconv*/, HSZ /*hsz1*/, HSZ /*hsz2*/,
                                                    HDDEDATA /*hData*/, DWORD /*dwData1*/, DWORD /*dwData2*/);
        static const char*       SkipToNextArgument (const char* pFormat);
        static HSZ               ClientArguments (const char* pFormat, ...);
        static char*             ExtractArgument (HSZ hszArgs, int iArg);
        static void              ServerReturned (HDDEDATA hArgs, const char* pFormat, ...);
#endif
    };

    string MakeSophistsAppNameVersionURL (const string& relURL, const string& appName, const string& extraArgs = string{});

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Support.inl"

#endif /*_Stroika_Frameworks_Led_Support_h_*/
