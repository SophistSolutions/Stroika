/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_CodePage_inl_
#define	_Stroika_Foundation_Characters_CodePage_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	<cstring>
#include	"../Debug/Assertions.h"

#if		qPlatform_Windows
	#include	"Platform/Windows/CodePage.h"
#endif



namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			/*
			@METHOD:		GetDefaultSDKCodePage
			@DESCRIPTION:	<p>Returns the assumed code page of @'Led_SDK_Char'.</p>
			*/
			inline	CodePage	GetDefaultSDKCodePage ()
				{
					#if		qPlatform_Windows
						//http://blogs.msdn.com/b/michkap/archive/2005/01/06/347834.aspx
						#if		defined (CP_ACP)
							Assert (CP_ACP == 0);
							return CP_ACP;			// special windows define which maps to the current OS code page
						#else
							return 0;
						#endif
						//GetACP()				// means essentially the same thing but supposedly (even if we cahced GetACP() - CP_ACP is faster)
					#else
// MAYBE should use the LOCALE stuff - and get the current code-page from the locale? If such a thing?
						return kCodePage_UTF8;	// So far - this is meaningless on other systems - but this would be the best guess I think
					#endif
				}


		
		
		//	class	UTF8Converter
			inline	size_t	UTF8Converter::MapToUNICODE_QuickComputeOutBufSize (const char* /*inMBChars*/, size_t inMBCharCnt) const
				{
					return inMBCharCnt;
				}
			inline	size_t	UTF8Converter::MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* /*inChars*/, size_t inCharCnt) const
				{
					return inCharCnt*6;
				}

	
	
		//	class	CodePageConverter
			inline	CodePageConverter::CodePageConverter (CodePage codePage)
					: fHandleBOM (false)
					, fCodePage (codePage)
					{
					}
			inline	CodePageConverter::CodePageConverter (CodePage codePage, HandleBOMFlag h)
					: fHandleBOM (true)
					, fCodePage (codePage)
					{
						Require (h == eHandleBOM);
						Arg_Unused (h);
					}
			/*
			@METHOD:		CodePageConverter::GetHandleBOM
			@DESCRIPTION:	<p>In UNICODE, files are generally headed by a byte order mark (BOM). This mark is used to indicate
					if the file is big endian, or little-endian (if the characters are wide-characters). This is true for 2 and 4
					byte UNICODE (UCS-2, UCS-4) UNICODE, as well as for UTF-X encodings (such as UTF-7 and UTF-8). It is also used
					to indicate whether or not the file is in a UTF encoding (as byte order doesn't matter in any (most?) of the
					UTF encodings.</p>
						<p>The basic rubrick for BOM's is that they are the character 0xfeff, as it would be encoded in the given
					UTF or UCS encoding.</p>
						<p>Because of this type of encoding - if you have a 0xfeff character (after decoding) at the beginning of
					a buffer, there is no way for this routine to know if that was REALLY there, or if it was byte order mark. And its not always
					desirable for the routine producing these encodings to produce the byte order mark, but sometimes its highly desirable.
					So - this class lets you get/set a flag to indicate whether or not to process BOMs on input, and whether or not to
					generate them on encoded outputs.</p>
						<p>See also @'CodePageConverter::SetHandleBOM', and note that there is an overloaded CTOR that lets you specify
					CodePageConverter::eHandleBOM as a final argument to automatically set this BOM converter flag.</p>
			*/
			inline	bool	CodePageConverter::GetHandleBOM () const
					{
						return fHandleBOM;
					}
			/*
			@METHOD:		CodePageConverter::SetHandleBOM
			@DESCRIPTION:	<p>See also @'CodePageConverter::GetHandleBOM'.</p>
			*/
			inline	void	CodePageConverter::SetHandleBOM (bool handleBOM)
					{
						fHandleBOM = handleBOM;
					}
			/*
			@METHOD:		CodePageConverter::MapToUNICODE_QuickComputeOutBufSize
			@DESCRIPTION:	<p>Call to get an upper bound, reasonable buffer size to use to pass to
						@'CodePageConverter::MapToUNICODE' calls.</p>
			*/
			inline	size_t	CodePageConverter::MapToUNICODE_QuickComputeOutBufSize (const char* /*inMBChars*/, size_t inMBCharCnt) const
				{
					size_t	resultSize	=	inMBCharCnt;
					return resultSize;
				}






		//	class	CodePageConverter::CodePageNotSupportedException
			inline	CodePageConverter::CodePageNotSupportedException::CodePageNotSupportedException (CodePage codePage):
				fCodePage (codePage)
				{
				}






			inline	void	WideStringToNarrow (const wstring& ws, CodePage codePage, string* intoResult)
				{
					RequireNotNull (intoResult);
					const wchar_t*	wsp	=	ws.c_str ();
					WideStringToNarrow (wsp, wsp + ws.length (), codePage, intoResult);
				}
			inline	string	WideStringToNarrow (const wstring& ws, CodePage codePage)
				{
					string	result;
					WideStringToNarrow (ws, codePage, &result);
					return result;
				}
			inline	void	NarrowStringToWide (const string& s, int codePage, wstring* intoResult)
				{
					RequireNotNull (intoResult);
					const char*	sp	=	s.c_str ();
					NarrowStringToWide (sp, sp + s.length (), codePage, intoResult);
				}
			inline	wstring	NarrowStringToWide (const string& s, int codePage)
				{
					wstring	result;
					NarrowStringToWide (s, codePage, &result);
					return result;
				}






			inline	wstring	ASCIIStringToWide (const string& s)
				{
					#if		qDebug
						for (string::const_iterator i = s.begin (); i != s.end (); ++i) {
							Assert (isascii (*i));
						}
					#endif
					return wstring (s.begin (), s.end ());
				}
			inline	string	WideStringToASCII (const wstring& s)
				{
					#if		qDebug
						for (wstring::const_iterator i = s.begin (); i != s.end (); ++i) {
							Assert (isascii (*i));
						}
					#endif
					return string (s.begin (), s.end ());
				}


			inline	string	WideStringToUTF8 (const wstring& ws)
				{
					return WideStringToNarrow (ws, kCodePage_UTF8);
				}
			inline	void	UTF8StringToWide (const char* s, wstring* intoStr)
				{
					RequireNotNull (s);
					NarrowStringToWide (s, s + ::strlen (s), kCodePage_UTF8, intoStr);
				}
			inline	void	UTF8StringToWide (const string& s, wstring* intoStr)
				{
					NarrowStringToWide (s, kCodePage_UTF8, intoStr);
				}
			inline	wstring	UTF8StringToWide (const char* s)
				{
					RequireNotNull (s);
					wstring	result;
					NarrowStringToWide (s, s + ::strlen (s), kCodePage_UTF8, &result);
					return result;
				}
			inline	wstring	UTF8StringToWide (const string& s)
				{
					return NarrowStringToWide (s, kCodePage_UTF8);
				}



		}
	}
}
#endif	/*_Stroika_Foundation_Characters_CodePage_inl_*/
