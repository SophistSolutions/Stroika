/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Characters/CString/Utilities.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Memory/StackBuffer.h"

namespace Stroika::Foundation::Characters {

    /*
     ********************************************************************************
     ********************************** GetEOL **************************************
     ********************************************************************************
     */
    template <IPossibleCharacterRepresentation T>
    [[deprecated ("Since Stroika v3.0d7 - use kEOL")]] inline
#if __cpp_constexpr >= 202211L
        constexpr
#endif
        const T*
        GetEOL ()
    {
        // note queer syntax for strings so works with many character types, including Characters::Character
        if constexpr (qPlatform_Windows) {
            static constexpr T kResult_[] = {'\r', '\n', '\0'}; // "\r\n"
            return kResult_;
        }
        else if constexpr (qPlatform_POSIX) {
            static constexpr T kResult_[] = {'\n', '\0'}; //  "\n"
            return kResult_;
        }
        else {
            AssertNotImplemented ();
        }
    }

    /*
     ********************************************************************************
     ************************ AssureHasLineTermination ******************************
     ********************************************************************************
     */
    template <typename CHAR>
    void AssureHasLineTermination (basic_string<CHAR>* text)
    {
        RequireNotNull (text);
        const CHAR* eol     = kEOL<CHAR>;
        size_t      eolLen  = CString::Length (eol);
        size_t      len     = text->length ();
        bool        already = false;
        if (eolLen < len) {
            already = (text->compare (len - eolLen, eolLen, eol) == 0);
        }
        if (not already) {
            text->append (eol);
        }
    }

    /*
     ********************************************************************************
     *********************************** CRLFToNL ***********************************
     ********************************************************************************
     */
    template <typename TCHAR>
    size_t CRLFToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, [[maybe_unused]] size_t outBufSize)
    {
        RequireNotNull (srcText);
        RequireNotNull (outBuf);
        TCHAR* outPtr = outBuf;
        for (size_t i = 1; i <= srcTextBytes; ++i) {
            TCHAR c = srcText[i - 1];
            if (c == '\r') {
                // peek at next character - and if we have a CRLF sequence - then advance pointer
                // (so we skip next NL) and pretend this was an NL..
                // NB: we DON'T map plain CR to NL - to get that to happen - use NormalizeTextToNL()
                if (i < srcTextBytes and srcText[i] == '\n') {
                    c = '\n';
                    ++i;
                }
            }
            *outPtr++ = c;
        }
        size_t nBytes = outPtr - outBuf;
        Assert (nBytes <= outBufSize);
        return nBytes;
    }
    template <typename TCHAR>
    inline void CRLFToNL (basic_string<TCHAR>* text)
    {
        size_t origLen = text->length ();
        size_t newLen  = CRLFToNL (Containers::Start (*text), origLen, Containers::Start (*text), origLen);
        Assert (newLen <= origLen);
        text->resize (newLen);
    }
    template <typename TCHAR>
    inline basic_string<TCHAR> CRLFToNL (const basic_string<TCHAR>& text)
    {
        basic_string<TCHAR> r = text;
        CRLFToNL (&r);
        return r;
    }

    /*
     ********************************************************************************
     *********************************** NLToCRLF ***********************************
     ********************************************************************************
     */
    template <typename TCHAR>
    size_t NLToCRLF (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, [[maybe_unused]] size_t outBufSize)
    {
        Require (srcText != outBuf); // though we support this for the others - its too hard
                                     // in this case for the PC...
        TCHAR* outPtr        = outBuf;
        bool   prevCharWasCR = false;
        for (const TCHAR* i = srcText; i < srcText + srcTextBytes; ++i) {
            Assert (outPtr < outBuf + outBufSize);
            // prefix all LF with 'CR' so 'CRLF', unless it already WAS CRLF
            if (*i == '\n' and not prevCharWasCR) {
                *outPtr++ = '\r';
            }
            Assert (outPtr < outBuf + outBufSize);
            *outPtr++ = *i;
            prevCharWasCR == (*i == '\r');
        }
        size_t nBytes = outPtr - outBuf;
        Assert (nBytes <= outBufSize);
        return (nBytes);
    }
    template <typename TCHAR>
    inline basic_string<TCHAR> NLToCRLF (const basic_string<TCHAR>& text)
    {
        size_t                     outBufSize = (text.length () + 1) * 2;
        Memory::StackBuffer<TCHAR> outBuf{Memory::eUninitialized, outBufSize};
        size_t                     newSize = NLToCRLF<TCHAR> (Containers::Start (text), text.length (), outBuf.begin (), outBufSize);
        Assert (newSize < outBufSize);
        outBuf[newSize] = '\0';
        return basic_string<TCHAR> (outBuf);
    }

    /*
     ********************************************************************************
     ********************************* NativeToNL ***********************************
     ********************************************************************************
     */
    template <typename TCHAR>
    size_t NativeToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, [[maybe_unused]] size_t outBufSize)
    {
        TCHAR* outPtr = outBuf;
        for (size_t i = 1; i <= srcTextBytes; ++i) {
#if qPlatform_MacOS
            TCHAR c = (srcText[i - 1] == '\r') ? '\n' : srcText[i - 1];
#elif qPlatform_Windows
            TCHAR c = srcText[i - 1];
            if (c == '\r') {
                // peek at next character - and if we have a CRLF sequence - then advance pointer
                // (so we skip next NL) and pretend this was an NL..
                // NB: we DON'T map plain CR to NL - to get that to happen - use Led_NormalizeTextToNL()
                if (i < srcTextBytes and srcText[i] == '\n') {
                    c = '\n';
                    ++i;
                }
            }
#else
            TCHAR c = srcText[i - 1];
#endif
            *outPtr++ = c;
        }
        size_t nBytes = outPtr - outBuf;
        Assert (nBytes <= outBufSize);
        return nBytes;
    }
    template <typename TCHAR>
    inline basic_string<TCHAR> NativeToNL (const basic_string<TCHAR>& text)
    {
        size_t                     outBufSize = text.length () + 1;
        Memory::StackBuffer<TCHAR> outBuf{Memory::eUninitialized, outBufSize};
        size_t                     newSize = NativeToNL<TCHAR> (Containers::Start (text), text.length (), outBuf.begin (), outBufSize);
        Assert (newSize < outBufSize);
        outBuf[newSize] = '\0';
        return basic_string<TCHAR> (outBuf);
    }

    /*
     ********************************************************************************
     ********************************* NLToNative ***********************************
     ********************************************************************************
     */
    template <typename TCHAR>
    size_t NLToNative (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, [[maybe_unused]] size_t outBufSize)
    {
        Require (srcText != outBuf); // though we support this for the others - its too hard
                                     // in this case for the PC...
        TCHAR* outPtr = outBuf;
        for (size_t i = 1; i <= srcTextBytes; ++i) {
            Assert (outPtr < outBuf + outBufSize);
#if defined(macintosh)
            TCHAR c = (srcText[i - 1] == '\n') ? '\r' : srcText[i - 1];
#elif qPlatform_Windows
            TCHAR c = srcText[i - 1];
            if (c == '\n') {
                *outPtr++ = '\r';
            }
#else
            // Unix
            TCHAR c = srcText[i - 1];
#endif
            *outPtr++ = c;
        }
        size_t nBytes = outPtr - outBuf;
        Assert (nBytes <= outBufSize);
        return nBytes;
    }
    template <typename TCHAR>
    inline basic_string<TCHAR> NLToNative (const basic_string<TCHAR>& text)
    {
        size_t                     outBufSize = (text.length () + 1) * 2;
        Memory::StackBuffer<TCHAR> outBuf{Memory::eUninitialized, outBufSize};
        size_t                     newSize = NLToNative<TCHAR> (Containers::Start (text), text.length (), outBuf.begin (), outBufSize);
        Assert (newSize < outBufSize);
        outBuf[newSize] = '\0';
        return basic_string<TCHAR> (outBuf);
    }

    /*
     ********************************************************************************
     ************************** NormalizeTextToNL ***********************************
     ********************************************************************************
     */
    template <typename TCHAR>
    size_t NormalizeTextToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, [[maybe_unused]] size_t outBufSize)
    {
        Require (srcTextBytes == 0 or srcText != nullptr);
        Require (outBufSize == 0 or outBuf != nullptr);
        // Require outBufSize big enough to hold the converted srcTextBytes (best to just make sizes the same)

        // NB: We DO Support the case where srcText == outBuf!!!!
        TCHAR* outPtr = outBuf;
        for (size_t i = 0; i < srcTextBytes; ++i) {
            TCHAR c = srcText[i];
            if (c == '\r') {
                // peek at next character - and if we have a CRLF sequence - then advance pointer
                // (so we skip next NL) and pretend this was an NL..
                if (i + 1 < srcTextBytes and srcText[i + 1] == '\n') {
                    ++i;
                }
                c = '\n';
            }
            Assert (outPtr < outBuf + outBufSize);
            *outPtr++ = c;
        }
        size_t nBytes = outPtr - outBuf;
        Assert (nBytes <= outBufSize);
        return nBytes;
    }
    template <typename TCHAR>
    inline void NormalizeTextToNL (basic_string<TCHAR>* text)
    {
        RequireNotNull (text);
        size_t origLen = text->length ();
        size_t newLen  = NormalizeTextToNL (static_cast<const TCHAR*> (Containers::Start (*text)), origLen,
                                            static_cast<TCHAR*> (Containers::Start (*text)), origLen);
        Assert (newLen <= origLen);
        text->resize (newLen);
    }
    template <typename TCHAR>
    inline basic_string<TCHAR> NormalizeTextToNL (const basic_string<TCHAR>& text)
    {
        basic_string<TCHAR> r = text;
        NormalizeTextToNL (&r);
        return r;
    }

}
