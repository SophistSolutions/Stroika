/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_LineEndings_h_
#define _Stroika_Foundation_Characters_LineEndings_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <string>

#include "Stroika/Foundation/Characters/Character.h"
#include "Stroika/Foundation/Common/Common.h"

namespace Stroika::Foundation::Characters {

    /**
     *  \brief null-terminated String constant for current compiled platform - Windows (CRLF) or POSIX (NL) - macos I dont think any longer uses \r??
     */
    template <IPossibleCharacterRepresentation T>
    static constexpr T kEOL[] = {
#if qPlatform_Windows
        '\r', '\n', '\0' // "\r\n"
#elif qPlatform_POSIX
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
     */
    template <typename TCHAR>
    size_t NativeToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
    template <typename TCHAR>
    basic_string<TCHAR> NativeToNL (const basic_string<TCHAR>& text);

    /**
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
