/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Characters_CodePage_h_
#define	_Stroika_Foundation_Characters_CodePage_h_	1

/*
@MODULE:	CodePage
@DESCRIPTION:
		<p>This module is designed to provide mappings between wide UNICODE and various other code pages
	and UNICODE encodings.</p>
 */

#include	"../StroikaPreComp.h"

#include	<cctype>
#include	<vector>

#include	"../Configuration/Common.h"




/*
 *	TODO:
 *
 *	(o)		FIRST - we need to fix PortableWideStringToNarrow_ and CodePageConverter to have a MORE SPACE EFFICNET (COMPUTE OUT BUF SIZE).
 *			Run basic xlate algortihm without writing to OUTPUTR, and find use that WOUDL be used.
 *
 *	(o)		THEN - test performance, and see if windows version can get away with using portable version (where codes overlap)
 */


namespace	Stroika {	
	namespace	Foundation {
		namespace	Characters {

			enum	{
				kCodePage_INVALID					=	0xffffffff,	// I hope this is always an invalid code page???

				kCodePage_ANSI						=	1252,

				kCodePage_MAC						=	2,
				kCodePage_PC						=	437,	//	IBM PC code page 437
				kCodePage_PCA						=	850,	//	IBM PC code page 850, used by IBM Personal System/2
				kCodePage_Thai						=	874,	// From uniscribe sample code (LGP 2003-01-13)
				kCodePage_SJIS						=	932,
				kCodePage_GB2312					=	936,	// Chinese (Simplified)
				kCodePage_Korean					=	949,	// Korean
				kCodePage_BIG5						=	950,	// Chinese (Traditional)
				kCodePage_EasternEuropean			=	1250,
				kCodePage_CYRILIC					=	1251,	// Russian (Cyrilic)
				kCodePage_GREEK						=	1253,
				kCodePage_Turkish					=	1254,
				kCodePage_HEBREW					=	1255,
				kCodePage_ARABIC					=	1256,
				kCodePage_Baltic					=	1257,
				kCodePage_Vietnamese				=	1258,

				kCodePage_UNICODE_WIDE				=	1200,	// Standard UNICODE for MS-Windows
				kCodePage_UNICODE_WIDE_BIGENDIAN	=	1201,
	
				kCodePage_UTF7						=	65000,
				kCodePage_UTF8						=	65001


				/// NOT CURRENTLY USED - SO DONT DEFINE CUZ I DONT UNDERSTAND! kCodePage_OEM		=	1,					// What does this mean??? PC ONLY???
				#if		0
					// Only supported on Win2K
					kCodePage_SYMBOL	=	42,
				#endif
			};

			/*
			@CLASS:			CodePage
			@DESCRIPTION:
					<p>A codePage is a Win32 (really DOS) concept which describes a particular single or
				multibyte (narrow) character set encoding. Use Win32 CodePage numbers. Maybe someday add
				a layer to map to/from Mac 'ScriptIDs' - which are basicly analagous.</p>
					<p>Use this with @'CodePageConverter'.</p>
			*/
			typedef	int	CodePage;



			CodePage	GetDefaultSDKCodePage ();


			/*
			@CLASS:			CodePageConverter
			@DESCRIPTION:
					<p>Helper class to wrap conversions between code pages (on Mac known as scripts)
				and UTF-16 (WIDE UNICODE).</p>
			*/
			class	CodePageConverter {
				public:
					class	CodePageNotSupportedException;
					enum	HandleBOMFlag { eHandleBOM };
				public:
					CodePageConverter (CodePage codePage);
					CodePageConverter (CodePage codePage, HandleBOMFlag h);

				public:
					nonvirtual	bool	GetHandleBOM () const;
					nonvirtual	void	SetHandleBOM (bool handleBOM);
				private:
					bool	fHandleBOM;

				public:
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;

					nonvirtual	size_t	MapToUNICODE_QuickComputeOutBufSize (const char* inMBChars, size_t inMBCharCnt) const;

					nonvirtual	void	MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;

					nonvirtual	size_t	MapFromUNICODE_QuickComputeOutBufSize (const char16_t* inChars, size_t inCharCnt) const;
					nonvirtual	size_t	MapFromUNICODE_QuickComputeOutBufSize (const char32_t* inChars, size_t inCharCnt) const;
					nonvirtual	size_t	MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const;

				private:
					CodePage	fCodePage;
			};


			// Coming IN, the 'outCharCnt' is the buffer size of outChars, and coming OUT it is hte actual
			// number of characters written to outChars. This function will NUL-terminate the outChars iff inMBChars
			// was NUL-terminated (and there is enough space in the buffer).
			void	MapSBUnicodeTextWithMaybeBOMToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);



			class	CodePageConverter::CodePageNotSupportedException {
				public:
					CodePageNotSupportedException (CodePage codePage);

				public:
					CodePage	fCodePage;
			};








			class	UTF8Converter {
				public:
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char16_t* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, char32_t* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;

					nonvirtual	size_t	MapToUNICODE_QuickComputeOutBufSize (const char* inMBChars, size_t inMBCharCnt) const;

					nonvirtual	void	MapFromUNICODE (const char16_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapFromUNICODE (const char32_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;

					nonvirtual	size_t	MapFromUNICODE_QuickComputeOutBufSize (const char16_t* inChars, size_t inCharCnt) const;
					nonvirtual	size_t	MapFromUNICODE_QuickComputeOutBufSize (const char32_t* inChars, size_t inCharCnt) const;
					nonvirtual	size_t	MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const;
			};







			/*
			@CLASS:			CodePagesInstalled
			@DESCRIPTION:
						<p>Helper class to check what code pages are installed on a given machine.</p>
			*/
			class	CodePagesInstalled {
				public:
					static	vector<CodePage>	GetAll ();
					static	bool				IsCodePageAvailable (CodePage cp);

				private:
					static	void	Init_ ();
					static	void	AddIfNotPresent_ (CodePage cp);
			};






			/*
			@CLASS:			CodePagesGuesser
			@DESCRIPTION:	<p>Guess the code page of the given argument text.</p>
			*/
			class	CodePagesGuesser {
				public:
					enum	Confidence { eLow = 0, eMedium = 10, eHigh=100 };

				public:
					nonvirtual	CodePage	Guess (const void* input, size_t nBytes, Confidence* confidence = nullptr, size_t* bytesFromFrontToStrip = nullptr);
			};



			void	WideStringToNarrow (const wchar_t* wsStart, const wchar_t* wsEnd, CodePage codePage, string* intoResult);
			void	WideStringToNarrow (const wstring& ws, CodePage codePage, string* intoResult);
			string	WideStringToNarrow (const wstring& ws, CodePage codePage);
			void	NarrowStringToWide (const char* sStart, const char* sEnd, CodePage codePage, wstring* intoResult);
			void	NarrowStringToWide (const string& s, CodePage codePage, wstring* intoResult);
			wstring	NarrowStringToWide (const string& s, CodePage codePage);


			string	WideStringToUTF8 (const wstring& ws);
			void	UTF8StringToWide (const char* s, wstring* intoStr);
			void	UTF8StringToWide (const string& s, wstring* intoStr);
			wstring	UTF8StringToWide (const char* ws);
			wstring	UTF8StringToWide (const string& ws);

			wstring	ASCIIStringToWide (const string& s);
			string	WideStringToASCII (const wstring& s);

		}
	}
}
#endif	/*_Stroika_Foundation_Characters_CodePage_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"CodePage.inl"
