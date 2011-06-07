/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Stroika_Foundation_Characters_CodePage_inl_
#define	__Stroika_Foundation_Characters_CodePage_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"../Debug/Assertions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			/*
			@METHOD:		GetDefaultSDKCodePage
			@DESCRIPTION:	<p>Returns the assumed code page of @'Led_SDK_Char'.</p>
			*/
			inline	CodePage	GetDefaultSDKCodePage ()
				{
					#if		qMacOS
						return kCodePage_MAC;	// there maybe a better answer than this??? Some Mac SDK API?
					#elif	qWindows
						return CP_ACP;			// special windows define which maps to the current OS code page
					#elif	qXWindows
						// Need some sort of TMPHACK answer for UNIX - eventually do better ;-) Maybe using locale calls?
						return kCodePage_ANSI;
					#else
						Assert (false);		// probably shouldn't come up??
						return kCodePage_ANSI;	// for now - an OK answer
					#endif
				}



			/*
			@METHOD:		Win32CharSetToCodePage
			@DESCRIPTION:	<p>Map from a Win32 lfCharset enumeration (as appears in RTF font specifications and LOGFONT specifications)
						This routine is made portable to other platforms BECAUSE it is used in RTF files.
						This function returns the magic code page # zero (CP_ACP on windows) if there is no good mapping.</p>
			*/
			inline	CodePage	Win32CharSetToCodePage (unsigned char lfCharSet)
				{
					// See MSFT info article Q165478. No routine to map from these charset values - just this table... Sigh...
					// LGP 2001-04-30

					// In the file http://msdn.microsoft.com/library/specs/rtfspec_6.htm - there are many more of these
					// magic#s documented. But not how to get the numbers back from a font. May want to support more
					// of these? But I think not - I think they are now redudundantly specified via better mecahnims,
					// like UNICODE or \cpg
					// -- LGP 2000/04/29
					//
					// For SPR#1184 I grabbed a few more numbers. The mapping to codepages can be roughly guestimated by looking 
					// in the registry around HKEY_LOCAL_MACHINE\SOFTWARE\Classes\MIME\Database\Charset\
					//	-- LGP 2002-11-29
					#ifdef	CP_ACP
						Assert (CP_ACP == 0);
					#else
						const	unsigned char	CP_ACP				=	0;
					#endif

					#ifdef	ANSI_CHARSET
						Assert (ANSI_CHARSET == 0);
					#else
						const	unsigned char	ANSI_CHARSET		=	0;
					#endif
					#ifdef	DEFAULT_CHARSET
						Assert (DEFAULT_CHARSET == 1);
					#else
						const	unsigned char	DEFAULT_CHARSET		=	1;
					#endif
					#ifdef	MAC_CHARSET
						Assert (MAC_CHARSET == 77);
					#else
						const	unsigned char	MAC_CHARSET			=	77;
					#endif
					#ifdef	SHIFTJIS_CHARSET
						Assert (SHIFTJIS_CHARSET == 128);
					#else
						const	unsigned char	SHIFTJIS_CHARSET	=	128;
					#endif
					#ifdef	HANGEUL_CHARSET
						Assert (HANGEUL_CHARSET == 129);
					#else
						const	unsigned char	HANGEUL_CHARSET		=	129;
					#endif
					#ifdef	JOHAB_CHARSET
						Assert (JOHAB_CHARSET == 130);
					#else
						const	unsigned char	JOHAB_CHARSET		=	130;
					#endif
					#ifdef	GB2312_CHARSET
						Assert (GB2312_CHARSET == 134);
					#else
						const	unsigned char	GB2312_CHARSET		=	134;
					#endif
					#ifdef	CHINESEBIG5_CHARSET
						Assert (CHINESEBIG5_CHARSET == 136);
					#else
						const	unsigned char	CHINESEBIG5_CHARSET	=	136;
					#endif
					#ifdef	GREEK_CHARSET
						Assert (GREEK_CHARSET == 161);
					#else
						const	unsigned char	GREEK_CHARSET		=	161;
					#endif
					#ifdef	TURKISH_CHARSET
						Assert (TURKISH_CHARSET == 162);
					#else
						const	unsigned char	TURKISH_CHARSET		=	162;
					#endif
					#ifdef	VIETNAMESE_CHARSET
						Assert (VIETNAMESE_CHARSET == 163);
					#else
						const	unsigned char	VIETNAMESE_CHARSET	=	163;
					#endif
					#ifdef	HEBREW_CHARSET
						Assert (HEBREW_CHARSET == 177);
					#else
						const	unsigned char	HEBREW_CHARSET		=	177;
					#endif
					#ifdef	ARABIC_CHARSET
						Assert (ARABIC_CHARSET == 178);
					#else
						const	unsigned char	ARABIC_CHARSET		=	178;
					#endif
					#ifdef	BALTIC_CHARSET
						Assert (BALTIC_CHARSET == 186);
					#else
						const	unsigned char	BALTIC_CHARSET		=	186;
					#endif
					#ifdef	RUSSIAN_CHARSET
						Assert (RUSSIAN_CHARSET == 204);
					#else
						const	unsigned char	RUSSIAN_CHARSET		=	204;
					#endif
					#ifdef	THAI_CHARSET
						Assert (THAI_CHARSET == 222);
					#else
						const	unsigned char	THAI_CHARSET		=	222;
					#endif
					#ifdef	EASTEUROPE_CHARSET
						Assert (EASTEUROPE_CHARSET == 238);
					#else
						const	unsigned char	EASTEUROPE_CHARSET	=	238;
					#endif
					#ifdef	OEM_CHARSET
						Assert (OEM_CHARSET == 255);
					#else
						const	unsigned char	OEM_CHARSET			=	255;
					#endif

					switch (lfCharSet) {
		//				case	ANSI_CHARSET:			return CP_ACP;						// used to do this - but I THINK it was wrong - LGP 2002-12-08
						case	ANSI_CHARSET:			return kCodePage_ANSI;				// right? Maybe SB? kCodePage_ANSI (1252)???
						//case	DEFAULT_CHARSET:		return CP_ACP;						// Not sure what todo here?
						case	MAC_CHARSET:			return kCodePage_MAC;
						case	SHIFTJIS_CHARSET:		return kCodePage_SJIS;				//	Japanese (SJIS)
						case	HANGEUL_CHARSET:		return kCodePage_Korean;			//	Hangul
		//				case	JOHAB_CHARSET:			return CP_ACP;						//	Johab		--DONT KNOW
						case	GB2312_CHARSET:			return kCodePage_GB2312;			//	Chinese
						case	CHINESEBIG5_CHARSET:	return kCodePage_BIG5;				//	Chinese
						case	GREEK_CHARSET:			return kCodePage_GREEK;				//	Greek
						case	TURKISH_CHARSET:		return kCodePage_Turkish;			//	Turkish
						case	VIETNAMESE_CHARSET:		return kCodePage_Vietnamese;		//	Vietnamese
						case	HEBREW_CHARSET:			return kCodePage_HEBREW;			//	Hebrew
						case	ARABIC_CHARSET:			return kCodePage_ARABIC;			//	Arabic
						case	179:					return kCodePage_ARABIC;			//	Arabic Traditional
						case	180:					return kCodePage_ARABIC;			//	Arabic user
						case	181:					return kCodePage_HEBREW;			//	Hebrew user
						case	BALTIC_CHARSET:			return kCodePage_Baltic;			//	Baltic
						case	RUSSIAN_CHARSET:		return kCodePage_CYRILIC;			//	Russian/Cyrilic
						case	THAI_CHARSET:			return kCodePage_Thai;				//	Thai
						case	EASTEUROPE_CHARSET:		return kCodePage_EasternEuropean;	//	aka 'central european'?
						case	254:					return kCodePage_PC; break;
						case	OEM_CHARSET:			return kCodePage_PCA; break;
						case	256:					return kCodePage_MAC; break;
						default:						return CP_ACP;
					}
				}
		#if		qWindows
			/*
			@METHOD:		Win32PrimaryLangIDToCodePage
			@DESCRIPTION:	<p>Map from a Win32 language identifier to a code page.</p>
			*/
			inline	CodePage	Win32PrimaryLangIDToCodePage (USHORT languageIdenifier)
				{
					/*
					 *	I haven't found this stuff documented anyplace. Its hard to imagine that MSFT doesnt provide their own
					 *	mapping routines! Anyhow - I got a start on this from some UNISCRIBE sample code, and have since
					 *	added on from educated guesswork.		-- LGP 2003-01-30
					 */
					switch (PRIMARYLANGID (languageIdenifier)) {
						case	LANG_ARABIC:		return kCodePage_ARABIC;
						case	LANG_ENGLISH:		return kCodePage_ANSI;
						case	LANG_FRENCH:		return kCodePage_ANSI;
						case	LANG_GERMAN:		return kCodePage_ANSI;
						case	LANG_GREEK:			return kCodePage_GREEK;
						case	LANG_HEBREW:		return kCodePage_HEBREW;
						case	LANG_ICELANDIC:		return kCodePage_ANSI;				// guess? - 2003-01-30
						case	LANG_ITALIAN:		return kCodePage_ANSI;
						case	LANG_KOREAN:		return kCodePage_Korean;
						case	LANG_POLISH:		return kCodePage_EasternEuropean;	// a bit of a guess - LGP 2003-01-30
						case	LANG_PORTUGUESE:	return kCodePage_ANSI;
						case	LANG_RUSSIAN:		return kCodePage_CYRILIC;
						case	LANG_SPANISH:		return kCodePage_ANSI;
						case	LANG_SWEDISH:		return kCodePage_ANSI;				// guess? - 2003-01-30
						case	LANG_THAI:			return kCodePage_Thai;
						case	LANG_TURKISH:		return kCodePage_Turkish;
						case	LANG_UKRAINIAN:		return kCodePage_CYRILIC;			// guess? - 2003-01-30
						case	LANG_VIETNAMESE:	return kCodePage_Vietnamese;
						default:					return ::GetACP ();
					}
				}
		#endif

		
		
		
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
			inline	CodePageConverter::CodePageConverter (CodePage codePage):
					fHandleBOM (false),
					fCodePage (codePage)
					{
					}
			inline	CodePageConverter::CodePageConverter (CodePage codePage, HandleBOMFlag h):
					fHandleBOM (true),
					fCodePage (codePage)
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
			/*
			@METHOD:		CodePageConverter::MapFromUNICODE_QuickComputeOutBufSize
			@DESCRIPTION:	<p>Call to get an upper bound, reasonable buffer size to use to pass to MapFromUNICODE calls.</p>
			*/
			inline	size_t	CodePageConverter::MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const
				{
					size_t	resultSize;
					switch (fCodePage) {
						case	kCodePage_ANSI:	resultSize = inCharCnt * 1; break;
						case	kCodePage_MAC:	resultSize = inCharCnt * 1; break;
						case	kCodePage_PC:	resultSize = inCharCnt * 1; break;
						case	kCodePage_PCA:	resultSize = inCharCnt * 1; break;
						case	kCodePage_SJIS:	resultSize = inCharCnt * 2; break;
						case	kCodePage_UTF7:	resultSize = inCharCnt * 6;	break;	// ITHINK thats right... BOM appears to be 5 chars long? LGP 2001-09-11
						case	kCodePage_UTF8:	resultSize = UTF8Converter ().MapFromUNICODE_QuickComputeOutBufSize (inChars, inCharCnt);
						default:				resultSize = inCharCnt * 8; break;	// I THINK that should always be enough - but who knows...
					}
					if (GetHandleBOM ()) {
						switch (fCodePage) {
							case	kCodePage_UNICODE_WIDE:
							case	kCodePage_UNICODE_WIDE_BIGENDIAN: {
								// BOM (byte order mark)
								resultSize += 2;
							}
							break;
							case	kCodePage_UTF7: {
								resultSize += 5;	// for BOM
							}
							break;
							case	kCodePage_UTF8: {
								resultSize += 3;	// BOM (byte order mark)
							}
						}
					}
					return resultSize;
				}






		//	class	CodePageConverter::CodePageNotSupportedException
			inline	CodePageConverter::CodePageNotSupportedException::CodePageNotSupportedException (CodePage codePage):
				fCodePage (codePage)
				{
				}




		//	class	CodePageConverter
			inline	Win32_CodePageConverter::Win32_CodePageConverter (int codePage):
					fCodePage (codePage)
					{
					}






		//	class	CodePagesInstalled
			/*
			@METHOD:		CodePagesInstalled::GetAll
			@DESCRIPTION:	<p>Returns a list of all code pages installed on the system.
						This list is returned in sorted order.</p>
			*/
			inline	const vector<CodePage>&	CodePagesInstalled::GetAll ()
				{
					if (sCodePages.empty ()) {
						Init ();
					}
					return sCodePages;
				}
			/*
			@METHOD:		CodePagesInstalled::IsCodePageAvailable
			@DESCRIPTION:	<p>Checks if the given code page is installed.</p>
			*/
			inline	bool	CodePagesInstalled::IsCodePageAvailable (CodePage cp)
				{
					const vector<CodePage>&				codePages	=	GetAll ();
					vector<CodePage>::const_iterator	i			=	lower_bound (codePages.begin (), codePages.end (), cp);
					return (i != codePages.end ());
				}
			/*
			@METHOD:		CodePagesInstalled::GetDefaultCodePage
			@DESCRIPTION:	<p>Returns the operating systems default code page. NOTE - this is NOT the same as
						the default code page Led will use. Led will use this occasionally as its default, however.
						On Windows - this is basically a call to ::GetACP ().</p>
			*/
			inline	CodePage	CodePagesInstalled::GetDefaultCodePage ()
				{
					#if		qMacOS
						return kCodePage_MAC;
					#elif	qWindows
						return ::GetACP ();
					#else
						return kCodePage_ANSI;	// not sure what else to return by default
					#endif
				}

		}
	}
}

#endif	/*__Stroika_Foundation_Characters_CodePage_inl_*/
