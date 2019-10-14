/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Framework_Led_Support_inl_
#define _Stroika_Framework_Led_Support_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::Led {

#if !qHasIsAscii && defined(isascii)
//#warning  "You probably should define qHasIsAscii for your compiler."
#endif

#if qPlatform_MacOS
}
#include <Sound.h>
namespace Stroika::Frameworks::Led {
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
        return short (Led_ByteSwapFromMac ((unsigned short)src));
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
        return short (Led_ByteSwapFromWindows ((unsigned short)src));
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
        return long (Led_ByteSwapFromWindows ((unsigned long)src));
    }

    inline void UInt16ToBuf (uint16_t u, uint16_t* realBuf)
    {
        unsigned char* buf = (unsigned char*)realBuf;
        buf[0]             = (unsigned char)(u >> 8);
        buf[1]             = (unsigned char)u;
    }
    inline uint16_t BufToUInt16 (const uint16_t* realBuf)
    {
        const unsigned char* buf = (const unsigned char*)realBuf;
        return (uint16_t) ((((uint16_t)buf[0]) << 8) + (uint16_t)buf[1]);
    }
    inline void UInt32ToBuf (uint32_t ul, uint32_t* realBuf)
    {
        unsigned short* buf = (unsigned short*)realBuf;
        UInt16ToBuf ((unsigned short)(ul >> 16), buf);
        UInt16ToBuf ((unsigned short)(ul), buf + 1);
    }
    inline uint32_t BufToUInt32 (const uint32_t* buf)
    {
        unsigned short* bufAsShortArray = (unsigned short*)buf;
        return (((unsigned long)BufToUInt16 (bufAsShortArray)) << 16) + BufToUInt16 (bufAsShortArray + 1);
    }

    inline void SizeTToBuf (size_t ul, uint32_t* realBuf)
    {
        using Stroika::Foundation::Execution::Throw;
        if (ul > numeric_limits<uint32_t>::max ()) {
            Throw (range_error ("size_t wont fit in 32-bits"));
        }
        UInt32ToBuf (static_cast<uint32_t> (ul), realBuf);
    }
    inline size_t BufToSizeT (const uint32_t* buf)
    {
        using Stroika::Foundation::Execution::Throw;
        uint32_t r = BufToUInt32 (buf);
        if (r > numeric_limits<size_t>::max ()) {
            Throw (range_error ("32-bits wont fit in size_t"));
        }
        return static_cast<size_t> (r);
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
#elif qStroika_FeatureSupported_XWindows
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
CompileTimeFlagChecker_HEADER (Stroika::Frameworks::Led, qSingleByteCharacters, qSingleByteCharacters);
CompileTimeFlagChecker_HEADER (Stroika::Frameworks::Led, qMultiByteCharacters, qMultiByteCharacters);
CompileTimeFlagChecker_HEADER (Stroika::Frameworks::Led, qWideCharacters, qWideCharacters);
CompileTimeFlagChecker_HEADER (Stroika::Frameworks::Led, qProvideIMESupport, qProvideIMESupport);

#endif /*_Stroika_Framework_Led_Support_inl_*/
