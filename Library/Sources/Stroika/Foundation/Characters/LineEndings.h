/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_LineEndings_h_
#define _Stroika_Foundation_Characters_LineEndings_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <string>

#include "Stroika/Foundation/Characters/Character.h"
#include "Stroika/Foundation/Common/Common.h"

namespace Stroika::Foundation::Characters {

    /**
     *  \brief null-terminated String constant for current compiled platform - Windows (CRLF) or POSIX, macOS included (NL)
     *
     *  Text Stroika makes (ToString, CommandLine::GenerateUsage...) generally ends lines in \n, which is right for a text-mode
     *  stream - cout, cerr, an fstream opened without ios::binary - as it writes the platform's line ending itself (on Windows,
     *  given kEOL it would write \r\r\n). kEOL is for text written where nothing converts: a binary stream, a socket,
     *  OutputDebugString. To convert \n text for such a place, String::NLToNative ().
     */
    template <IPossibleCharacterRepresentation T>
    static constexpr T kEOL[] = {
#if qStroika_Platform_Windows
        '\r', '\n', '\0' // "\r\n"
#elif qStroika_Platform_POSIX
        '\n', '\0' // "\n"
#endif
    };

    /**
     */
    template <typename CHAR>
    void AssureHasLineTermination (basic_string<CHAR>* text);

    /**
     *  \brief Convert the argument srcText buffer from CRLF format line endings, to NL format line endings.
     *
     * return #bytes in output buffer (NO nullptr TERM) - assert buffer big enough - output buf as big is input buf
     * always big enough. OK for srcText and outBuf to be SAME PTR.
     */
    template <typename TCHAR>
    size_t CRLFToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
    template <typename TCHAR>
    void CRLFToNL (basic_string<TCHAR>* text); // modified in-place
    template <typename TCHAR>
    basic_string<TCHAR> CRLFToNL (const basic_string<TCHAR>& text);

    /**
     *  \brief Convert the argument srcText buffer from NL format line endings, to CRLF format line endings.
     *         Note - even if input is already CRLF, this then is a no-op, changing nothing.
     */
    template <typename TCHAR>
    size_t NLToCRLF (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
    template <typename TCHAR>
    basic_string<TCHAR> NLToCRLF (const basic_string<TCHAR>& text);

    /**
     *  \brief Convert the platform's line endings (kEOL) to NL
     */
    template <typename TCHAR>
    size_t NativeToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
    template <typename TCHAR>
    basic_string<TCHAR> NativeToNL (const basic_string<TCHAR>& text);

    /**
     *  \brief Convert NL line endings to the platform's (kEOL): on Windows each \n becomes \r\n
     *
     *  A \n already preceded by \r is left as it is, so this is safe to call on text that already has CRLF line endings
     *  (on Windows a no-op then; elsewhere always one).
     */
    template <typename TCHAR>
    size_t NLToNative (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
    template <typename TCHAR>
    basic_string<TCHAR> NLToNative (const basic_string<TCHAR>& text);

    /**
     * return #bytes in output buffer (NO nullptr TERM) - assert buffer big enough - output buf as big is input buf
     * always big enough. OK for srcText and outBuf to be SAME PTR.
     * 
     *  \note @see String::NormalizeTextToNL
     */
    template <typename TCHAR>
    size_t NormalizeTextToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
    template <typename TCHAR>
    void NormalizeTextToNL (basic_string<TCHAR>* text); // modified in-place
    template <typename TCHAR>
    basic_string<TCHAR> NormalizeTextToNL (const basic_string<TCHAR>& text);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LineEndings.inl"

#endif /*_Stroika_Foundation_Characters_LineEndings_h_*/
