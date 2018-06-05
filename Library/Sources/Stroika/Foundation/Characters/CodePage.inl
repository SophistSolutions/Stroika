/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Characters_CodePage_inl_
#define _Stroika_Foundation_Characters_CodePage_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"
#include <algorithm>
#include <cstring>

#if qPlatform_Windows
#include "Platform/Windows/CodePage.h"
#endif

namespace Stroika {
    namespace Foundation {
        namespace Characters {

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
                return kCodePage_UTF8; // So far - this is meaningless on other systems - but this would be the best guess I think
#endif
            }

            /*
             ********************************************************************************
             *********************************** UTF8Converter ******************************
             ********************************************************************************
             */
            inline size_t UTF8Converter::MapToUNICODE_QuickComputeOutBufSize (const char* /*inMBChars*/, size_t inMBCharCnt) const
            {
                return inMBCharCnt;
            }
            inline size_t UTF8Converter::MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* /*inChars*/, size_t inCharCnt) const
            {
                return inCharCnt * 6;
            }
            inline void UTF8Converter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
            {
                static_assert ((sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t)), "(sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t))");
                if constexpr (sizeof (wchar_t) == sizeof (char16_t)) {
                    MapToUNICODE (inMBChars, inMBCharCnt, reinterpret_cast<char16_t*> (outChars), outCharCnt);
                }
                else if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
                    MapToUNICODE (inMBChars, inMBCharCnt, reinterpret_cast<char32_t*> (outChars), outCharCnt);
                }
            }
            inline void UTF8Converter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
            {
                static_assert ((sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t)), "(sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t))");
                if constexpr (sizeof (wchar_t) == sizeof (char16_t)) {
                    MapFromUNICODE (reinterpret_cast<const char16_t*> (inChars), inCharCnt, outChars, outCharCnt);
                }
                else if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
                    MapFromUNICODE (reinterpret_cast<const char32_t*> (inChars), inCharCnt, outChars, outCharCnt);
                }
            }

            /*
             ********************************************************************************
             ******************************* CodePageConverter ******************************
             ********************************************************************************
             */
            inline CodePageConverter::CodePageConverter (CodePage codePage)
                : fHandleBOM (false)
                , fCodePage (codePage)
            {
            }
            inline CodePageConverter::CodePageConverter (CodePage codePage, HandleBOMFlag h)
                : fHandleBOM (true)
                , fCodePage (codePage)
            {
                Require (h == eHandleBOM);
                Arg_Unused (h);
            }
            inline bool CodePageConverter::GetHandleBOM () const
            {
                return fHandleBOM;
            }
            inline void CodePageConverter::SetHandleBOM (bool handleBOM)
            {
                fHandleBOM = handleBOM;
            }
            inline size_t CodePageConverter::MapToUNICODE_QuickComputeOutBufSize (const char* /*inMBChars*/, size_t inMBCharCnt) const
            {
                size_t resultSize = inMBCharCnt;
                return resultSize;
            }
            inline void CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
            {
                static_assert ((sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t)), "(sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t))");
                if constexpr (sizeof (wchar_t) == sizeof (char16_t)) {
                    MapToUNICODE (inMBChars, inMBCharCnt, reinterpret_cast<char16_t*> (outChars), outCharCnt);
                }
                else if constexpr (sizeof (wchar_t) == sizeof (char32_t)) {
                    MapToUNICODE (inMBChars, inMBCharCnt, reinterpret_cast<char32_t*> (outChars), outCharCnt);
                }
            }
            inline void CodePageConverter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
            {
                static_assert ((sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t)), "(sizeof (wchar_t) == sizeof (char16_t)) or (sizeof (wchar_t) == sizeof (char32_t))");
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
            inline CodePageConverter::CodePageNotSupportedException::CodePageNotSupportedException (CodePage codePage)
                : fCodePage (codePage)
            {
            }

            /*
             ********************************************************************************
             ****************************** CodePagesInstalled ******************************
             ********************************************************************************
             */
            inline vector<CodePage> CodePagesInstalled::GetAll ()
            {
                return fCodePages_;
            }
            inline bool CodePagesInstalled::IsCodePageAvailable (CodePage cp)
            {
                return (std::find (fCodePages_.begin (), fCodePages_.end (), cp) == fCodePages_.end ());
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
            inline void NarrowStringToWide (const string& s, int codePage, wstring* intoResult)
            {
                RequireNotNull (intoResult);
                const char* sp = s.c_str ();
                NarrowStringToWide (sp, sp + s.length (), codePage, intoResult);
            }
            inline wstring NarrowStringToWide (const string& s, int codePage)
            {
                wstring result;
                NarrowStringToWide (s, codePage, &result);
                return result;
            }

            inline wstring ASCIIStringToWide (const string& s)
            {
#if qDebug
                for (string::const_iterator i = s.begin (); i != s.end (); ++i) {
                    Assert (isascii (*i));
                }
#endif
                return wstring (s.begin (), s.end ());
            }
            inline string WideStringToASCII (const wstring& s)
            {
#if qDebug
                for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
                    Assert (isascii (*i));
                }
#endif
                return string (s.begin (), s.end ());
            }

            inline string WideStringToUTF8 (const wstring& ws)
            {
                return WideStringToNarrow (ws, kCodePage_UTF8);
            }
            inline void UTF8StringToWide (const char* s, wstring* intoStr)
            {
                RequireNotNull (s);
                NarrowStringToWide (s, s + ::strlen (s), kCodePage_UTF8, intoStr);
            }
            inline void UTF8StringToWide (const string& s, wstring* intoStr)
            {
                NarrowStringToWide (s, kCodePage_UTF8, intoStr);
            }
            inline wstring UTF8StringToWide (const char* s)
            {
                RequireNotNull (s);
                wstring result;
                NarrowStringToWide (s, s + ::strlen (s), kCodePage_UTF8, &result);
                return result;
            }
            inline wstring UTF8StringToWide (const string& s)
            {
                return NarrowStringToWide (s, kCodePage_UTF8);
            }
        }
    }
}
#endif /*_Stroika_Foundation_Characters_CodePage_inl_*/
