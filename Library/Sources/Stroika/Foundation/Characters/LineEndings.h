/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_LineEndings_h_
#define _Stroika_Foundation_Characters_LineEndings_h_ 1

#include "../StroikaPreComp.h"

#include <string>

#include "../Configuration/Common.h"

namespace Stroika::Foundation {
    namespace Characters {

        /**
         */
        template <typename T>
        constexpr const T* GetEOL ();
        template <>
        constexpr const char* GetEOL ();
        template <>
        constexpr const wchar_t* GetEOL ();

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
         */
        template <typename TCHAR>
        size_t NormalizeTextToNL (const TCHAR* srcText, size_t srcTextBytes, TCHAR* outBuf, size_t outBufSize);
        template <typename TCHAR>
        void NormalizeTextToNL (basic_string<TCHAR>* text); // modified in-place
        template <typename TCHAR>
        basic_string<TCHAR> NormalizeTextToNL (const basic_string<TCHAR>& text);
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LineEndings.inl"

#endif /*_Stroika_Foundation_Characters_LineEndings_h_*/
