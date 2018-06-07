/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_Led_Support_h_
#define _Stroika_Frameworks_Led_Support_h_ 1

#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Memory/Common.h"
#include "../../Foundation/StroikaPreComp.h"
#include "../../Foundation/Time/Realtime.h"

/*
@MODULE:    LedSupport
@DESCRIPTION:
        <p>Support defines, and utility functions that should usually be provided
    by some other class library (e.g. Stroika, TCL, MFC, OWL, etc. These
    defines should just thunk down to the appropriate class library defines -
    where possible.</p>
 */

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <new>
#include <string>
#include <vector>

#include "Config.h"

#if qPlatform_MacOS
#include <LowMem.h>
#include <Processes.h> // for URL support
#include <Scrap.h>
#elif qPlatform_Windows
#include <Windows.h> //

#include <DDEML.h> // really only needed if qUseSpyglassDDESDIToOpenURLs - but that define only set in LedConfig.h
#include <oaidl.h> // for SAFEARRAY
#include <tchar.h>
#elif qXWindows
#include <X11/X.h>
#include <X11/Xatom.h>
#endif

namespace Stroika {
    namespace Frameworks {
        namespace Led {

            using namespace std;

            using Foundation::Memory::Byte;

            constexpr size_t kBadIndex = size_t (-1);

            /*
            @CLASS:         Led_tChar
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
            */
#if defined(__cplusplus)
#if qSingleByteCharacters
            using Led_tChar = char;
#elif qMultiByteCharacters
            using Led_tChar = char;
#elif qWideCharacters
            using Led_tChar = wchar_t;
#else
#error "One of these must be defined"
#endif
#endif

/*
            @CLASS:         LED_TCHAR_OF
            @DESCRIPTION:   <p>Like the Win32SDK macro _T(). Except is based on the Led type @'Led_tChar', and the Led
                macros @'qSingleByteCharacters', @'qMultiByteCharacters', and @'qWideCharacters'.</p>
            */
#define LED_TCHAR_OF__(X) L##X
#if qSingleByteCharacters || qMultiByteCharacters
#define LED_TCHAR_OF(X) X
#elif qWideCharacters
#define LED_TCHAR_OF(X) LED_TCHAR_OF__ (X)
#endif

#if qWideCharacters
            constexpr wchar_t kNonBreakingSpace = 0x00a0;
            constexpr wchar_t kZeroWidthSpace   = 0x200b;
            constexpr wchar_t kPoundSign        = 0x00a3;
            constexpr wchar_t kCentSign         = 0x00a2;
            constexpr wchar_t kYenSign          = 0x00a5;
#endif

            /*
            @CLASS:         Led_tString
            @DESCRIPTION:   <p>This expands to either the ANSI C++ string class library class <code>string</code>
                or <code>wstring</code> - depending on whether we are using UNICODE or not. It is a <code>basic_string</code>
                templated on @'Led_tChar'.</p>
            */
            using Led_tString = std::basic_string<Led_tChar>;

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
            @CLASS:         Led_SDK_Char
            @DESCRIPTION:   <p>See @'Led_SDK_String'</p>
            */
#if qSDK_UNICODE
            using Led_SDK_Char = wchar_t;
#else
            using Led_SDK_Char                   = char;
#endif

/*
            @CLASS:         Led_SDK_String
            @DESCRIPTION:   <p>Expands to either the ANSI @'string' or the ANSI @'wstring' class, depending on the
                value of the configuration define @'qSDK_UNICODE'.</p>
                    <p>This is rarely used - but is used for things like font names, and other system IO strings
                that may need to be in one format or another.</p>
            */
#if qSDK_UNICODE
            using Led_SDK_String = wstring;
#else
            using Led_SDK_String                 = string;
#endif

/*
            @CLASS:         Led_SDK_TCHAROF
            @DESCRIPTION:   <p>Like the Win32SDK macro _T(). See @'Led_SDK_String'.</p>
            */
#if qSDK_UNICODE
#define Led_SDK_TCHAROF(X) LED_TCHAR_OF__ (X)
#else
#define Led_SDK_TCHAROF(X) X
#endif

            wstring ACP2WideString (const string& s);
            string  Wide2ACPString (const wstring& s);

            /*
            @METHOD:        Led_ANSI2SDKString
            @DESCRIPTION:
            */
            Led_SDK_String Led_ANSI2SDKString (const string& s);

            /*
            @METHOD:        Led_Wide2SDKString
            @DESCRIPTION:
            */
            Led_SDK_String Led_Wide2SDKString (const wstring& s);

            /*
            @METHOD:        Led_SDKString2ANSI
            @DESCRIPTION:
            */
            string Led_SDKString2ANSI (const Led_SDK_String& s);

            /*
            @METHOD:        Led_SDKString2Wide
            @DESCRIPTION:
            */
            wstring Led_SDKString2Wide (const Led_SDK_String& s);

            /*
            @METHOD:        Led_tString2SDKString
            @DESCRIPTION:
            */
            Led_SDK_String Led_tString2SDKString (const Led_tString& s);

            /*
            @METHOD:        Led_WideString2tString
            @DESCRIPTION:
            */
            Led_tString Led_WideString2tString (const wstring& s);

            /*
            @METHOD:        Led_SDKString2tString
            @DESCRIPTION:
            */
            Led_tString Led_SDKString2tString (const Led_SDK_String& s);

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

            /*
            @METHOD:        Led_ThrowOutOfMemoryException
            @DESCRIPTION:   <p>Even though Standard C++ defines how this SHOULD behave, many class libraries (e.g. MFC and
                PowerPlant) expect non-C++-Standard things to get thrown when you run out of memory.</p>
                    <p>So - Led internally calls this routine for out of memory exceptions.</p>
                    <p>The behavior of this routine detaults to the C++ standard, unless you are using some class library, like
                PowerPlant or MFC. Then those modules may call @'Led_Set_OutOfMemoryException_Handler'. You can call @'Led_Get_OutOfMemoryException_Handler'
                or @'Led_Set_OutOfMemoryException_Handler' to get/set the handler.</p>
                    <p>Note - this method never returns - it always throws.</p>
            */
            void Led_ThrowOutOfMemoryException ();

            /*
            @METHOD:        Led_Get_OutOfMemoryException_Handler
            @DESCRIPTION:   <p>Get the handler used in @'Led_ThrowOutOfMemoryException'.</p>
            */
            void (*Led_Get_OutOfMemoryException_Handler ()) ();

            /*
            @METHOD:        Led_Set_OutOfMemoryException_Handler
            @DESCRIPTION:   <p>Set the handler used in @'Led_ThrowOutOfMemoryException'.</p>
                    <p>Note - any handler supplied must never return - it must always throw.</p>
            */
            void Led_Set_OutOfMemoryException_Handler (void (*outOfMemoryHandler) ());

            /*
            @METHOD:        Led_ThrowBadFormatDataException
            @DESCRIPTION:   <p>This is called internally by Led (or your code) when some data is found to be in a bad format.</p>
                    <p>Either your class library will call this to specify a class-lib specific exception type for this kind of
                error, or you can in your application. Or just leave it todo the default.</p>
                    <p>Note - this method never returns - it always throws.</p>
                    <p>See also @'Led_ThrowOutOfMemoryException'.</p>
            */
            void Led_ThrowBadFormatDataException ();

            /*
            @METHOD:        Led_Get_BadFormatDatException_Handler
            @DESCRIPTION:   <p>Get the handler used in @'Led_ThrowBadFormatDataException'.</p>
            */
            void (*Led_Get_BadFormatDataException_Handler ()) ();

            /*
            @METHOD:        Led_Set_BadFormatDatException_Handler
            @DESCRIPTION:   <p>Set the handler used in @'Led_ThrowBadFormatDataException'.</p>
                    <p>Note - any handler supplied must never return - it must always throw.</p>
            */
            void Led_Set_BadFormatDataException_Handler (void (*badFormatDataExceptionCallback) ());

#if qPlatform_MacOS
            /*
            @METHOD:        Led_ThrowOSErr
            @DESCRIPTION:   <p>This is called internally by Led (or your code) after MacOS system calls that return an OSErr.</p>
                    <p>By default that error # is thrown - but you can override this behavior by calling @'Led_Set_ThrowOSErrException_Handler'.</p>
                    <p>Note - this method never returns - it always throws, unless 'err' == 'noErr' - in which case it does nothing.</p>
            */
            void Led_ThrowOSErr (OSErr err);

            /*
            @METHOD:        Led_Get_ThrowOSErrException_Handler
            @DESCRIPTION:   <p>Get the handler used in @'Led_ThrowBadFormatDataException'.</p>
            */
            void (*Led_Get_ThrowOSErrException_Handler ()) (OSErr err);

            /*
            @METHOD:        Led_Set_ThrowOSErrException_Handler
            @DESCRIPTION:   <p>Set the handler used in @'Led_ThrowBadFormatDataException'.</p>
                    <p>Note - any handler supplied must never return - it must always throw.</p>
            */
            void Led_Set_ThrowOSErrException_Handler (void (*throwOSErrExceptionCallback) (OSErr err));
#endif

#if qPlatform_Windows
            class Win32ErrorException {
            public:
                Win32ErrorException (DWORD error);

                operator DWORD () const;

            private:
                DWORD fError;
            };

            class HRESULTErrorException {
            public:
                HRESULTErrorException (HRESULT hresult);

                operator HRESULT () const;

            private:
                HRESULT fHResult;
            };
#endif

#if qPlatform_Windows
            void Led_ThrowIfFalseGetLastError (bool test);
            void Led_ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode);
#endif

#if qPlatform_Windows
            /*
            @METHOD:        Led_ThrowIfErrorHRESULT
            @DESCRIPTION:   <p>If the HRESULT failed, then throw that HRESULT.</p>
            */
            void Led_ThrowIfErrorHRESULT (HRESULT hr);
#endif

#if qPlatform_MacOS
            void Led_ThrowIfOSErr (OSErr err);
            void Led_ThrowIfOSStatus (OSStatus err);
#endif

            /*
            @METHOD:        Led_ThrowIfNull
            @DESCRIPTION:   <p>If p == nullptr, then @'Led_ThrowOutOfMemoryException'.</p>
            */
            void Led_ThrowIfNull (void* p);

            short          Led_ByteSwapFromMac (short src);
            unsigned short Led_ByteSwapFromMac (unsigned short src);

            short          Led_ByteSwapFromWindows (short src);
            unsigned short Led_ByteSwapFromWindows (unsigned short src);
            long           Led_ByteSwapFromWindows (long src);
            unsigned long  Led_ByteSwapFromWindows (unsigned long src);

            void           Led_USHORTToBuf (unsigned short u, unsigned short* realBuf);
            unsigned short Led_BufToUSHORT (const unsigned short* realBuf);
            void           Led_ULONGToBuf (unsigned long ul, unsigned long* realBuf);
            unsigned long  Led_BufToULONG (const unsigned long* buf);
            void           Led_ULONGToBuf (unsigned int ul, unsigned int* realBuf);
            unsigned int   Led_BufToULONG (const unsigned int* buf);

            // These functions do NOT null terminate.
            // they both return the number of bytes in the target text based on conversion from
            // the source text. They silently ignore and do the best they can filtering
            // any bad characters out. They both ASSERT the dest buffer is big enough.
            // 2* input buffer size is ALWAYS big enough (really only need bigger out
            // on PC going from NL to native - same size good enuf in all other cases)
            size_t Led_NativeToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize);
            size_t Led_NLToNative (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize);

            // return #bytes in output buffer (NO nullptr TERM) - assert buffer big enough - output buf as big is input buf
            // always big enough!!!
            // NB: This routine is written carefully so that srcText CAN EQUAL outBuf!!!
            size_t Led_NormalizeTextToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize);

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

#if qPlatform_MacOS
            // throw if cannot do allocation. Use tmp memory if qUseMacTmpMemForAllocs.
            // fall back on application-heap-zone if no tmp memory
            Handle Led_DoNewHandle (size_t n);
            void   Led_CheckSomeLocalHeapRAMAvailable (size_t n = 1024); // only to avoid MacOS crashes on toolbox calls with little RAM left
#endif

#if qPlatform_MacOS || qPlatform_Windows
            /*
            @CLASS:         Led_StackBasedHandleLocker
            @DESCRIPTION:   <p>A utility class to lock (and on exit from the block unlock) a handle.</p>
            */
            class Led_StackBasedHandleLocker {
            public:
#if qPlatform_MacOS
                using GenericHandle = Handle;
#elif qPlatform_Windows
                using GenericHandle = HANDLE;
#endif
                Led_StackBasedHandleLocker (GenericHandle h);
                ~Led_StackBasedHandleLocker ();

                nonvirtual void* GetPointer () const;

            private:
                GenericHandle fHandle;
#if qPlatform_MacOS
                unsigned char fOldState;
#elif qPlatform_Windows
                void*  fPointer;
#endif
            };
#endif

            /*
            @CLASS:         DiscontiguousRunElement<DATA>
            @DESCRIPTION:   <p>We frequently need to keep track of runs of data. Sometimes - those elements are discontiguous.
                This template is merely used to give us a uniform naming convention for how we deal with those runs.</p>
                    <p>These elements are meant to be used with @'DiscontiguousRun<DATA>'.</p>
            */
            template <typename DATA>
            struct DiscontiguousRunElement {
                DiscontiguousRunElement (size_t offset, size_t length)
                    : fOffsetFromPrev (offset)
                    , fElementLength (length)
                    , fData ()
                {
                }
                DiscontiguousRunElement (size_t offset, size_t length, const DATA& data)
                    : fOffsetFromPrev (offset)
                    , fElementLength (length)
                    , fData (data)
                {
                }

                size_t fOffsetFromPrev;
                size_t fElementLength;
                DATA   fData;
            };
            template <>
            struct DiscontiguousRunElement<void> {
                DiscontiguousRunElement (size_t offset, size_t length)
                    : fOffsetFromPrev (offset)
                    , fElementLength (length)
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
            struct DiscontiguousRun : vector<DiscontiguousRunElement<DATA>> {
            };

            /*
             *  Character set support.
             */
#if qMultiByteCharacters
            bool Led_IsLeadByte (unsigned char c);
            bool Led_IsValidSingleByte (unsigned char c); // non-leadbyte first byte...
            bool Led_IsValidSecondByte (unsigned char c);
            bool Led_IsValidMultiByteString (const Led_tChar* start, size_t len); // check all chars valid - and 'len' doesn't split char!

            /*
             *  Led_FindPrevOrEqualCharBoundary use useful for when we guess an index in the middle
             *  of a string, and we want to use that index - or THAT mbyte character. So this
             *  routine always returns guessedEnd or guessedEnd-1, depending on which is the beginning
             *  of the mbtye character.
             */
            const Led_tChar* Led_FindPrevOrEqualCharBoundary (const Led_tChar* start, const Led_tChar* guessedEnd);
            Led_tChar*       Led_FindPrevOrEqualCharBoundary (Led_tChar* start, Led_tChar* guessedEnd);
#endif

            /*
             *  These routines can be used either with single byte, multibyte, or wide
             *  characters. They are intended to insulate most of the code from having
             *  to worry about this.
             */
            /*
            @METHOD:        Led_NextChar
            @DESCRIPTION:   <p>See @'Led_tChar'</p>
            */
            Led_tChar*       Led_NextChar (Led_tChar* fromHere);
            const Led_tChar* Led_NextChar (const Led_tChar* fromHere);

            /*
            @METHOD:        Led_PreviousChar
            @DESCRIPTION:   <p>See @'Led_tChar'</p>
            */
            Led_tChar*       Led_PreviousChar (Led_tChar* startOfString, Led_tChar* fromHere);
            const Led_tChar* Led_PreviousChar (const Led_tChar* startOfString, const Led_tChar* fromHere);

            /*
            @METHOD:        ValidateTextForCharsetConformance
            @DESCRIPTION:   <p>See @'Led_tChar'</p>
            */
            bool ValidateTextForCharsetConformance (const Led_tChar* text, size_t length); // just return true or false - no other diagnostics

            unsigned Led_DigitCharToNumber (char digitChar);      // require input is valid decimal digit
            char     Led_NumberToDigitChar (unsigned digitValue); // require input is valid decimal digit value

            bool Led_CasedCharsEqual (char lhs, char rhs, bool ignoreCase = true);
            bool Led_CasedStringsEqual (const string& lhs, const string& rhs, bool ignoreCase = true);

            /*
             *  Clipboard access support.
             *
             *      Note - when using this on the PC - be sure clipboard opened/closed
             *  surrounding access to this object - as is currently done in Led_MFC.
             *
             */
#if qPlatform_MacOS
            using Led_ClipFormat = OSType;
#elif qPlatform_Windows
            using Led_ClipFormat                 = CLIPFORMAT;
#elif qXWindows
            using Led_ClipFormat                 = long;
#endif
#if qPlatform_MacOS
            const Led_ClipFormat kTEXTClipFormat = 'TEXT';
            const Led_ClipFormat kPICTClipFormat = 'PICT';
            const Led_ClipFormat kFILEClipFormat = 'hfs '; //  flavorTypeHFS->from <Drag.h>
#elif qPlatform_Windows
#if qWideCharacters
            const Led_ClipFormat kTEXTClipFormat = CF_UNICODETEXT;
#else
            const Led_ClipFormat kTEXTClipFormat = CF_TEXT;
#endif
            //  const Led_ClipFormat    kPICTClipFormat =   CF_METAFILEPICT;
            const Led_ClipFormat kPICTClipFormat = CF_DIB;
            const Led_ClipFormat kFILEClipFormat = CF_HDROP;
#elif qXWindows
            const Led_ClipFormat kTEXTClipFormat = XA_STRING;
            const Led_ClipFormat kFILEClipFormat = 1; // X-TMP-HACK-LGP991213 - not sure what this should be???
#endif
            const Led_ClipFormat kBadClipFormat = 0;

            class Led_ClipboardObjectAcquire {
            public:
                Led_ClipboardObjectAcquire (Led_ClipFormat clipType);
                ~Led_ClipboardObjectAcquire ();

                static bool FormatAvailable_TEXT ();
                static bool FormatAvailable (Led_ClipFormat clipType);

                nonvirtual bool  GoodClip () const; // avoid exceptions usage for now - and just use flag
                nonvirtual void* GetData () const;
                nonvirtual size_t GetDataLength () const;

            private:
#if qPlatform_MacOS
                Handle fOSClipHandle;
#elif qPlatform_Windows
                HANDLE         fOSClipHandle;
#endif
                void* fLockedData;
            };

            void                                  Led_BeepNotify ();
            Foundation::Time::DurationSecondsType Led_GetDoubleClickTime (); // time-interval which defines how quick we consider two consecutive clicks a dbl-click

#if qXWindows
            extern void (*gBeepNotifyCallBackProc) ();
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
                VARIANT* fSafeArrayVariant;
                void*    fPtr;
            };

            class VariantArrayUnpacker {
            public:
                VariantArrayUnpacker (const VARIANT& v);
                ~VariantArrayUnpacker ();

            public:
                nonvirtual const void* PeekAtData () const;
                nonvirtual VARTYPE GetArrayElementType () const;
                nonvirtual size_t GetLength () const;

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
                Led_URLManager ();

                static Led_URLManager& Get (); // by default builds one, but you can specify your
                // own. NB: deletes old value, so if you specify
                // your own, be sure to allocate it with new so it
                // can be deleted...
                static void Set (Led_URLManager* newURLMgr);

            private:
                static Led_URLManager* sThe;

            public:
                virtual void Open (const string& url); // throws on detected errors
#if qPlatform_MacOS
                virtual string FileSpecToURL (const FSSpec& fsp);
#elif qPlatform_Windows
                virtual string FileSpecToURL (const string& path);
#endif

            protected:
#if qUseInternetConfig
                nonvirtual void Open_IC (const string& url);
#endif
#if qPlatform_MacOS
                nonvirtual void Open_SpyglassAppleEvent (const string& url);
#endif
#if qUseActiveXToOpenURLs
                nonvirtual void Open_ActiveX (const string& url);
#endif
#if qUseSpyglassDDESDIToOpenURLs
                nonvirtual void Open_SpyglassDDE (const string& url);
#endif
#if qUseSystemNetscapeOpenURLs
                nonvirtual void Open_SystemNetscape (const string& url);
#endif

#if qPlatform_MacOS
            private:
                static pascal OSErr FSpGetFullPath (const FSSpec* spec, short* fullPathLength, Handle* fullPath);
#endif

#if qPlatform_MacOS
            public:
                static ProcessSerialNumber FindBrowser ();
#endif

#if qUseSpyglassDDESDIToOpenURLs
            public:
                static void InitDDEHandler (); // to be able to open URLs with DDE this must be called, but
                // it takes over all DDE processing, disabling your app from doing
                // any other DDE.

                // Use this for more low level extensions of the DDE support...
                static DWORD    sDDEMLInstance; //  The DDEML instance identifier.
                static HDDEDATA CALLBACK SimpleDdeCallBack (UINT /*type*/, UINT /*fmt*/, HCONV /*hconv*/, HSZ /*hsz1*/, HSZ /*hsz2*/, HDDEDATA /*hData*/, DWORD /*dwData1*/, DWORD /*dwData2*/);
                static const char*       SkipToNextArgument (const char* pFormat);
                static HSZ               ClientArguments (const char* pFormat, ...);
                static char*             ExtractArgument (HSZ hszArgs, int iArg);
                static void              ServerReturned (HDDEDATA hArgs, const char* pFormat, ...);
#endif
            };

            string MakeSophistsAppNameVersionURL (const string& relURL, const string& appName, const string& extraArgs = string ());

            /*
            @CONFIGVAR:     qLedCheckCompilerFlagsConsistency
            @DESCRIPTION:   <p>Some development environments make it easy to accidentally provide an inconsistent set of compilation flags
                        across compilation units (e.g. MSVC/MS Visual Studio.Net is like this). This is a <em>bad</em> thing.</p>
                            <p>Led enables certain features - like debug checking - based on certain compilation flags (e.g. @'qDebug').
                        Some virtual methods are added to certain classes (e.g. Invariant check methods). And some other features maybe
                        conditionally defined based on other flags.</p>
                            <p>This mechanism attempts to detect if these flags are set inconsistently. This is only set on
                        by default for DEBUG builds - but - unfortunately - this can just as easily affect release builds (even
                        when the DEBUG builds work fine. For this reason - if you are seeing strange behavior when you compile for
                        a RELEASE build - you may want to externally specify this flag to test.
                            </p>
             */
#ifndef qLedCheckCompilerFlagsConsistency
#define qLedCheckCompilerFlagsConsistency qDebug
#endif

#if qLedCheckCompilerFlagsConsistency
            namespace LedCheckCompilerFlags {
#define LedCheckCompilerFlags_(a) _CHECK_##a
                extern int LedCheckCompilerFlags_ (qDebug);
                extern int LedCheckCompilerFlags_ (qSingleByteCharacters);
                extern int LedCheckCompilerFlags_ (qMultiByteCharacters);
                extern int LedCheckCompilerFlags_ (qWideCharacters);
                extern int LedCheckCompilerFlags_ (qProvideIMESupport);

                struct FlagsChecker {
                    FlagsChecker ()
                    {
                        /*
                         *  See the docs on @'qLedCheckCompilerFlagsConsistency' if this ever fails.
                         */
                        if (LedCheckCompilerFlags_ (qDebug) != qDebug) {
                            abort ();
                        }
                        if (LedCheckCompilerFlags_ (qSingleByteCharacters) != qSingleByteCharacters) {
                            abort ();
                        }
                        if (LedCheckCompilerFlags_ (qMultiByteCharacters) != qMultiByteCharacters) {
                            abort ();
                        }
                        if (LedCheckCompilerFlags_ (qWideCharacters) != qWideCharacters) {
                            abort ();
                        }
                        if (LedCheckCompilerFlags_ (qProvideIMESupport) != qProvideIMESupport) {
                            abort ();
                        }
                    }
                };
                static struct FlagsChecker sFlagsChecker;
            }
#endif

            /*
             ********************************************************************************
             ***************************** Implementation Details ***************************
             ********************************************************************************
             */

#if !qHasIsAscii && defined(isascii)
//#warning  "You probably should define qHasIsAscii for your compiler."
#endif

#if qBasicString_C_STR_BROKEN_forWCHAR_T
        }
    }
}
// Sadly this is NEEDED for GCC 3.? on RH 7.3 but is NOT needed for GCC 3.2 or later on RH8. Not sure how to
// tell from compile time flags which is which?
template <>
inline const wchar_t* basic_string<wchar_t>::c_str () const
{
    terminate ();
    return data ();
}
namespace Stroika {
    namespace Frameworks {
        namespace Led {
#endif

#if qPlatform_MacOS
        }
    }
}
#include <Sound.h>
namespace Stroika {
    namespace Frameworks {
        namespace Led {
#endif

            inline size_t Led_tStrlen (const Led_tChar* s)
            {
                RequireNotNull (s);
#if qSingleByteCharacters
                return ::strlen (s);
#elif qMultiByteCharacters
                return ::_mbstrlen (s);
#elif qWideCharacters
                return ::wcslen (s);
#endif
            }

            inline int Led_tStrCmp (const Led_tChar* l, const Led_tChar* r)
            {
                RequireNotNull (l);
                RequireNotNull (r);
#if qSingleByteCharacters
                return ::strcmp (l, r);
#elif qMultiByteCharacters
                return ::_mbscmp (l, r);
#elif qWideCharacters
                return ::wcscmp (l, r);
#endif
            }

            inline int Led_tStrnCmp (const Led_tChar* l, const Led_tChar* r, size_t n)
            {
                RequireNotNull (l);
                RequireNotNull (r);
#if qSingleByteCharacters
                return ::strncmp (l, r, n);
#elif qMultiByteCharacters
                return ::_mbsncmp (l, r, n);
#elif qWideCharacters
                return ::wcsncmp (l, r, n);
#endif
            }

            inline const Led_tChar* Led_tStrChr (const Led_tChar* s, Led_tChar c)
            {
                RequireNotNull (s);
#if qWideCharacters
                return ::wcschr (s, c);
#else
                return ::strchr (s, c);
#endif
            }

// This bizare logic to test twice for the presence of isascii is because the first test is
// whether the development environment provides an isascii(). The second test is
// in case Led is used with some other library which may be included alog with Led
// and that other library works around the absence of isascii - just as we did...
#if !qHasIsAscii && !defined(isascii)
            // I know this used to be part of the UNIX ctype - unsure why it does not appear to
            // be in the ANSI ctype??? - LGP 950211
            inline bool isascii (unsigned char c)
            {
                return (c <= 0x7f);
            }
#endif

#if qSDK_UNICODE
            inline Led_SDK_String Led_Wide2SDKString (const wstring& s)
            {
                return s;
            }
            inline wstring Led_SDKString2Wide (const Led_SDK_String& s)
            {
                return s;
            }
#else
            inline Led_SDK_String Led_ANSI2SDKString (const string& s)
            {
                return s;
            }
            inline string Led_SDKString2ANSI (const Led_SDK_String& s)
            {
                return s;
            }
#endif

#if qWideCharacters
            inline Led_tString Led_WideString2tString (const wstring& s)
            {
                return s;
            }
            inline wstring Led_tString2WideString (const Led_tString& s)
            {
                return s;
            }
#endif

#if !qWideCharacters
            inline Led_tString Led_ANSIString2tString (const string& s)
            {
                return s;
            }
            inline string Led_tString2ANSIString (const Led_tString& s)
            {
                return s;
            }
#endif

#if qWideCharacters == qSDK_UNICODE
            inline Led_SDK_String Led_tString2SDKString (const Led_tString& s)
            {
                return s;
            }
            inline Led_tString Led_SDKString2tString (const Led_SDK_String& s)
            {
                return s;
            }
#endif

#if qPlatform_Windows
            //  class   Win32ErrorException
            inline Win32ErrorException::Win32ErrorException (DWORD error)
                : fError (error)
            {
            }
            inline Win32ErrorException::operator DWORD () const
            {
                return fError;
            }

            //  class   HRESULTErrorException
            inline HRESULTErrorException::HRESULTErrorException (HRESULT hresult)
                : fHResult (hresult)
            {
            }
            inline HRESULTErrorException::operator HRESULT () const
            {
                return fHResult;
            }
#endif

#if qPlatform_Windows
            /*
            @METHOD:        Led_ThrowIfFalseGetLastError
            @DESCRIPTION:   <p></p>
            */
            inline void Led_ThrowIfFalseGetLastError (bool test)
            {
                if (not test) {
                    throw (Win32ErrorException (::GetLastError ()));
                }
            }
            /*
            @METHOD:        Led_ThrowIfNotERROR_SUCCESS
            @DESCRIPTION:   <p></p>
            */
            inline void Led_ThrowIfNotERROR_SUCCESS (DWORD win32ErrCode)
            {
                if (win32ErrCode != ERROR_SUCCESS) {
                    throw Win32ErrorException (win32ErrCode);
                }
            }
#endif

#if qPlatform_Windows
            inline void Led_ThrowIfErrorHRESULT (HRESULT hr)
            {
                if (not SUCCEEDED (hr)) {
                    throw HRESULTErrorException (hr);
                }
            }
#endif

#if qPlatform_MacOS
            /*
            @METHOD:        Led_ThrowIfOSErr
            @DESCRIPTION:   <p>If the argument err is not noErr, then throw that error.</p>
            */
            inline void Led_ThrowIfOSErr (OSErr err)
            {
                if (err != noErr) {
                    throw err;
                }
            }
            /*
            @METHOD:        Led_ThrowIfOSStatus
            @DESCRIPTION:   <p>If the argument err is not noErr, then throw that error.</p>
            */
            inline void Led_ThrowIfOSStatus (OSStatus err)
            {
                if (err != noErr) {
                    throw err;
                }
            }
#endif
            inline void Led_ThrowIfNull (void* p)
            {
                if (p == nullptr) {
                    Led_ThrowOutOfMemoryException ();
                }
            }

            inline unsigned short Led_ByteSwapFromMac (unsigned short src)
            {
#if qPlatform_MacOS
                return src;
#elif qPlatform_Windows
                return ((src & 0xff) << 8) + (src >> 8);
#endif
            }
            inline short Led_ByteSwapFromMac (short src)
            {
                return short(Led_ByteSwapFromMac ((unsigned short)src));
            }

            inline unsigned short Led_ByteSwapFromWindows (unsigned short src)
            {
#if qPlatform_MacOS
                return ((src & 0xff) << 8) + (src >> 8);
#elif qPlatform_Windows
                return src;
#endif
            }
            inline short Led_ByteSwapFromWindows (short src)
            {
                return short(Led_ByteSwapFromWindows ((unsigned short)src));
            }
            inline unsigned long Led_ByteSwapFromWindows (unsigned long src)
            {
#if qPlatform_MacOS
                return (Led_ByteSwapFromWindows ((unsigned short)(src & 0xffff)) << 16) + Led_ByteSwapFromWindows ((unsigned short)(src >> 16));
#elif qPlatform_Windows
                return src;
#endif
            }
            inline long Led_ByteSwapFromWindows (long src)
            {
                return long(Led_ByteSwapFromWindows ((unsigned long)src));
            }

            inline void Led_USHORTToBuf (unsigned short u, unsigned short* realBuf)
            {
                unsigned char* buf = (unsigned char*)realBuf;
                buf[0]             = (unsigned char)(u >> 8);
                buf[1]             = (unsigned char)u;
            }
            inline unsigned short Led_BufToUSHORT (const unsigned short* realBuf)
            {
                const unsigned char* buf = (const unsigned char*)realBuf;
                return (unsigned short)((((unsigned short)buf[0]) << 8) + (unsigned short)buf[1]);
            }
            inline void Led_ULONGToBuf (unsigned long ul, unsigned long* realBuf)
            {
                unsigned short* buf = (unsigned short*)realBuf;
                Led_USHORTToBuf ((unsigned short)(ul >> 16), buf);
                Led_USHORTToBuf ((unsigned short)(ul), buf + 1);
            }
            inline unsigned long Led_BufToULONG (const unsigned long* buf)
            {
                unsigned short* bufAsShortArray = (unsigned short*)buf;
                return (((unsigned long)Led_BufToUSHORT (bufAsShortArray)) << 16) + Led_BufToUSHORT (bufAsShortArray + 1);
            }
            inline void Led_ULONGToBuf (uint32_t ul, uint32_t* realBuf)
            {
                unsigned short* buf = (unsigned short*)realBuf;
                Led_USHORTToBuf ((unsigned short)(ul >> 16), buf);
                Led_USHORTToBuf ((unsigned short)(ul), buf + 1);
            }
            inline unsigned int Led_BufToULONG (const uint32_t* buf)
            {
                unsigned short* bufAsShortArray = (unsigned short*)buf;
                return (((unsigned long)Led_BufToUSHORT (bufAsShortArray)) << 16) + Led_BufToUSHORT (bufAsShortArray + 1);
            }

#if qPlatform_MacOS
            // throw if cannot do allocation. Use tmp memory if qUseMacTmpMemForAllocs.
            // fall back on application-heap-zone if no tmp memory
            inline Handle Led_DoNewHandle (size_t n)
            {
#if qUseMacTmpMemForAllocs
                OSErr  err;                           // ignored...
                Handle h = ::TempNewHandle (n, &err); // try temp mem, and use our local mem if no temp mem left
                if (h == nullptr) {
                    h = ::NewHandle (n);
                }
#else
                Handle h = ::NewHandle (n);
#endif
                Led_ThrowIfNull (h);
                return h;
            }
            inline void Led_CheckSomeLocalHeapRAMAvailable (size_t n)
            {
                Handle h = ::NewHandle (n);
                Led_ThrowIfNull (h);
                ::DisposeHandle (h);
            }
#endif

            template <typename ARRAY_CONTAINER, class T>
            size_t IndexOf (const ARRAY_CONTAINER& array, T item)
            {
                for (auto i = array.begin (); i != array.end (); ++i) {
                    if (*i == item) {
                        return (i - array.begin ());
                    }
                }
                return kBadIndex;
            }

#if qPlatform_MacOS || qPlatform_Windows
            inline Led_StackBasedHandleLocker::Led_StackBasedHandleLocker (GenericHandle h)
                : fHandle (h)
            {
                RequireNotNull (h);
#if qPlatform_MacOS
                fOldState = ::HGetState (h);
                ::HLock (h);
#elif qPlatform_Windows
                fPointer = ::GlobalLock (h);
#endif
            }
            inline Led_StackBasedHandleLocker::~Led_StackBasedHandleLocker ()
            {
#if qPlatform_MacOS
                ::HSetState (fHandle, fOldState);
#elif qPlatform_Windows
                ::GlobalUnlock (fHandle);
#endif
            }
            inline void* Led_StackBasedHandleLocker::GetPointer () const
            {
#if qPlatform_MacOS
                return *fHandle;
#elif qPlatform_Windows
                return fPointer;
#endif
            }
#endif

#if qMultiByteCharacters
            inline bool Led_IsLeadByte (unsigned char c)
            {
#error "That Multibyte character set not supported"
            }
            inline bool Led_IsValidSingleByte (unsigned char /*c*/)
            {
                // This isn't really right, but close enough for now. Alec Wysocker seems to think
                // so anyhow... LGP 950306
                return true;
            }
            inline bool Led_IsValidSecondByte (unsigned char c)
            {
#error "That Multibyte character set not supported"
            }
#endif
            inline Led_tChar* Led_NextChar (Led_tChar* fromHere)
            {
                AssertNotNull (fromHere);
#if qSingleByteCharacters || qWideCharacters
                return (fromHere + 1); // address arithmatic does the magic for wide characters
#elif qMultiByteCharacters
                return (Led_IsLeadByte (*fromHere) ? (fromHere + 2) : (fromHere + 1));
#endif
            }
            inline const Led_tChar* Led_NextChar (const Led_tChar* fromHere)
            {
                AssertNotNull (fromHere);
#if qSingleByteCharacters || qWideCharacters
                return (fromHere + 1); // address arithmatic does the magic for wide characters
#elif qMultiByteCharacters
                return (Led_IsLeadByte (*fromHere) ? (fromHere + 2) : (fromHere + 1));
#endif
            }
            /*

            FIXUP COMMENT - FROM EMAIL - AND ABOUT PREV_CHAR IMPLEMENTATION...
                Bytes can be:

                (1)     Ascii (0-0x7f)
                (2)     FirstByte of DoubleByte Char
                        (c >= 0x81 and c <= 0x9f) or (c >= 0xe0 and c <= 0xfc)
                (3)     SecondByte of DoubleByte Char
                        (c >= 0x40 and c <= 0x7e) or (c >= 0x80 and c <= 0xfc)

            (NB: This clasification counts on the fact that we filter out all bad SJIS)

                note that class of ASCII bytes and the class of FirstBytes does not overlap, but the class of SecondBytes DOES overlap with both others.

                Our task in scanning backward is to be able to UNIQUELY tell by looking at a byte - WITHOUT KNOWING ITS CONTEXT BEFOREHAND - if it is an ASCII byte, FirstByte, or a SecondByte.

                This analysis makes our job a little easier. Most bytes just by looking at them - can be immediately clasified.

                0x00-0x3f ==> ASCII
                0x40-0x7e ==> ASCII or SecondByte
                0x7f      ==> ASCII
                0x80      ==> SecondByte
                0x81-0x9f ==> FirstByte or SecondByte
                0xa0-0xdf ==> SecondByte
                0xe0-0xfc ==> FirstByte or SecondByte

                *   This Algoritm assumes that mbyte character sets have at most two bytes per character. This
                *   is true for SJIS - but I'm not posative it is always true - LGP 950216.

             */
            inline const Led_tChar* Led_PreviousChar ([[maybe_unused]] const Led_tChar* startOfString, const Led_tChar* fromHere)
            {
                AssertNotNull (startOfString);
                AssertNotNull (fromHere);
                Assert (startOfString < fromHere); // Must be room for previous character to exist!
#if qSingleByteCharacters || qWideCharacters
                return (fromHere - 1); // address arithmatic does the magic for wide characters
#elif qMultiByteCharacters
                /*
                 *  If the previous byte is a lead-byte, then the real character boundsary
                 *  is really TWO back.
                 *
                 *  Proof by contradiction:
                 *      Assume prev character is back one byte. Then the chracter it is part of
                 *  contains the first byte of the character we started with. This obviously
                 *  cannot happen. QED.
                 *
                 *      This is actually a worth-while test since lots of second bytes look quite
                 *  a lot like lead-bytes - so this happens a lot.
                 */
                if (Led_IsLeadByte (*(fromHere - 1))) {
                    Assert (fromHere - startOfString >= 2); // else split character...
                    return (fromHere - 2);
                }
                if (fromHere == startOfString + 1) {
                    return (startOfString); // if there is only one byte to go back, it must be an ASCII byte
                }
                // we go back by BYTES til we find a syncronization point
                const Led_tChar* cur = fromHere - 2;
                for (; cur > startOfString; cur--) {
                    if (not Led_IsLeadByte (*cur)) {
                        // Then we are in case 1, 2, 3, 4 or 6 (not 5 or 7). So ew know we are looking
                        // at an ASCII byte, or a secondbyte. Therefore - the NEXT byte from here must be
                        // a valid mbyte char boundary.
                        cur++;
                        break;
                    }
                }
                Assert (cur < fromHere);
                // Now we are pointing AT LEAST one mbyte char back from 'fromHere' so scan forward as we used
                // to to find the previous character...
                for (; cur < fromHere;) {
                    const Led_tChar* next = Led_NextChar (cur);
                    if (next == fromHere) {
                        return (cur);
                    }
                    Assert (next < fromHere); // if we've gone past - then fromHere must have split a mbyte char!
                    cur = next;
                }
                Assert (false);
                return (0); // previous character must exist!!!
#endif
            }
            inline Led_tChar* Led_PreviousChar (Led_tChar* startOfString, Led_tChar* fromHere)
            {
                // We could duplicate all the code above - but its simpler to just cast and invoke
                // the above impemenation...
                return ((Led_tChar*)Led_PreviousChar ((const Led_tChar*)startOfString, (const Led_tChar*)fromHere));
            }

            inline bool ValidateTextForCharsetConformance (
#if qMultiByteCharacters
                const Led_tChar* text, size_t length
#else
                const Led_tChar*, size_t
#endif
            )
            {
#if qMultiByteCharacters
                return (Led_IsValidMultiByteString (text, length));
#else
                return true; // probably should do SOME validation here for other character sets - at least
                             // for plain ascii!!! - LGP 950212
#endif
            }

            inline unsigned Led_DigitCharToNumber (char digitChar)
            {
                // assume '0'..'9' are consecutive - true for ascii at least - LGP 961015

                // require input is valid decimal digit
                Require (digitChar >= '0');
                Require (digitChar <= '9');
                return (digitChar - '0');
            }
            inline char Led_NumberToDigitChar (unsigned digitValue)
            {
                // assume '0'..'9' are consecutive - true for ascii at least - LGP 961015

                // require input is valid decimal digit value
                Require (digitValue <= 9);
                return static_cast<char> (digitValue + '0');
            }

#if qMultiByteCharacters
            inline const Led_tChar* Led_FindPrevOrEqualCharBoundary (const Led_tChar* start, const Led_tChar* guessedEnd)
            {
                if (guessedEnd == start) {
                    return guessedEnd;
                }

                // we go back by BYTES til we find a syncronization point
                const Led_tChar* cur = guessedEnd - 1;
                for (; cur > start; cur--) {
                    if (not Led_IsLeadByte (*cur)) {
                        // Then we are in case 1, 2, 3, 4 or 6 (not 5 or 7). So ew know we are looking
                        // at an ASCII byte, or a secondbyte. Therefore - the NEXT byte from here must be
                        // a valid mbyte char boundary.
                        cur++;
                        break;
                    }
                }
                const Led_tChar* closestStart = cur;
                for (;;) {
                    cur = Led_NextChar (cur);
                    if (cur > guessedEnd) {
                        break;
                    }
                    closestStart = cur;
                }
                Assert ((closestStart == guessedEnd) or (closestStart == guessedEnd - 1));
                return closestStart;
            }
            inline Led_tChar* Led_FindPrevOrEqualCharBoundary (Led_tChar* start, Led_tChar* guessedEnd)
            {
                // We could duplicate all the code above - but its simpler to just cast and invoke
                // the above impemenation...
                return ((Led_tChar*)Led_FindPrevOrEqualCharBoundary ((const Led_tChar*)start, (const Led_tChar*)guessedEnd));
            }
#endif

            inline bool Led_ClipboardObjectAcquire::FormatAvailable (Led_ClipFormat clipType)
            {
#if qPlatform_MacOS
#if TARGET_CARBON
                ScrapRef scrap = nullptr;
                Led_ThrowIfOSStatus (::GetCurrentScrap (&scrap));
                ScrapFlavorFlags flags = 0;
                return (::GetScrapFlavorFlags (scrap, clipType, &flags) == noErr);
#else
                long scrapOffset = 0;
                long result      = ::GetScrap (nullptr, clipType, &scrapOffset);
                return (result > 0);
#endif
#elif qPlatform_Windows
                return (!!::IsClipboardFormatAvailable (clipType));
#elif qXWindows
                // Wild guess - no good answer yet - LGP 2003-05-06
                return true;
#endif
            }
            inline bool Led_ClipboardObjectAcquire::FormatAvailable_TEXT ()
            {
                if (FormatAvailable (kTEXTClipFormat)) {
                    return true;
                }
                return false;
            }
            inline Led_ClipboardObjectAcquire::~Led_ClipboardObjectAcquire ()
            {
// For windows me must unlock, but not delete
#if qPlatform_Windows
                if (fLockedData != nullptr) {
                    ::GlobalUnlock (fLockedData);
                }
#endif

// For mac me must delete - could unlock too - but no need
#if qPlatform_MacOS
                if (fOSClipHandle != nullptr) {
                    ::DisposeHandle (fOSClipHandle);
                }
#endif
            }
            inline bool Led_ClipboardObjectAcquire::GoodClip () const
            {
#if qPlatform_MacOS || qPlatform_Windows
                return (fOSClipHandle != nullptr and fLockedData != nullptr);
#else
                return false; // X-TMP-HACK-LGP991213
#endif
            }
            inline void* Led_ClipboardObjectAcquire::GetData () const
            {
                Assert (GoodClip ());
                return (fLockedData);
            }
            inline size_t Led_ClipboardObjectAcquire::GetDataLength () const
            {
                Assert (GoodClip ());
#if qPlatform_MacOS
                return (::GetHandleSize (fOSClipHandle));
#elif qPlatform_Windows
                return (::GlobalSize (fOSClipHandle));
#endif
            }
        }
    }
}

#endif /*_Stroika_Frameworks_Led_Support_h_*/
