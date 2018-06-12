/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_LineEndings_inl_
#define _Stroika_Foundation_Characters_LineEndings_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Characters/CString/Utilities.h"
#include "../Characters/String.h"
#include "../Containers/Common.h"
#include "../Memory/SmallStackBuffer.h"

namespace Stroika {
    namespace Foundation {
        namespace Characters {

            /*
             ********************************************************************************
             ********************************** GetEOL **************************************
             ********************************************************************************
             */
            template <>
            inline constexpr const char* GetEOL ()
            {
#if qPlatform_Windows
                return "\r\n";
#elif qPlatform_POSIX
                return "\n";
#else
                AssertNotImplemented ();
#endif
            }
            template <>
            inline constexpr const wchar_t* GetEOL ()
            {
#if qPlatform_Windows
                return L"\r\n";
#elif qPlatform_POSIX
                return L"\n";
#else
                AssertNotImplemented ();
#endif
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
                const CHAR* eol     = GetEOL<CHAR> ();
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
                for (size_t i = 1; i <= srcTextBytes; i++) {
                    TCHAR c = srcText[i - 1];
                    if (c == '\r') {
                        // peek at next character - and if we have a CRLF sequence - then advance pointer
                        // (so we skip next NL) and pretend this was an NL..
                        // NB: we DONT map plain CR to NL - to get that to happen - use NormalizeTextToNL()
                        if (i < srcTextBytes and srcText[i] == '\n') {
                            c = '\n';
                            i++;
                        }
                    }
                    *outPtr++ = c;
                }
                size_t nBytes = outPtr - outBuf;
                Assert (nBytes <= outBufSize);
                return (nBytes);
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
                TCHAR* outPtr = outBuf;
                for (size_t i = 1; i <= srcTextBytes; i++) {
                    Assert (outPtr < outBuf + outBufSize);
                    TCHAR c = srcText[i - 1];
                    if (c == '\n') {
                        *outPtr++ = '\r';
                    }
                    *outPtr++ = c;
                }
                size_t nBytes = outPtr - outBuf;
                Assert (nBytes <= outBufSize);
                return (nBytes);
            }
            template <typename TCHAR>
            inline basic_string<TCHAR> NLToCRLF (const basic_string<TCHAR>& text)
            {
                size_t                          outBufSize = (text.length () + 1) * 2;
                Memory::SmallStackBuffer<TCHAR> outBuf (outBufSize);
                size_t                          newSize = NLToCRLF<TCHAR> (Containers::Start (text), text.length (), outBuf.begin (), outBufSize);
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
                for (size_t i = 1; i <= srcTextBytes; i++) {
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
                size_t                          outBufSize = (text.length () + 1) * 2;
                Memory::SmallStackBuffer<TCHAR> outBuf (outBufSize);
                size_t                          newSize = NLToNative<TCHAR> (Containers::Start (text), text.length (), outBuf.begin (), outBufSize);
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
                for (size_t i = 0; i < srcTextBytes; i++) {
                    TCHAR c = srcText[i];
                    if (c == '\r') {
                        // peek at next character - and if we have a CRLF sequence - then advance pointer
                        // (so we skip next NL) and pretend this was an NL..
                        if (i + 1 < srcTextBytes and srcText[i + 1] == '\n') {
                            i++;
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
                size_t newLen  = NormalizeTextToNL (static_cast<const TCHAR*> (Containers::Start (*text)), origLen, static_cast<TCHAR*> (Containers::Start (*text)), origLen);
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
    }
}
#endif /*_Stroika_Foundation_Characters_LineEndings_inl_*/
