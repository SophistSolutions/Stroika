/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__Stroika_Foundation_Characters_CodePage_h__
#define	__Stroika_Foundation_Characters_CodePage_h__	1

/*
@MODULE:	CodePage
@DESCRIPTION:
		<p>This module is designed to provide mappings between wide UNICODE and various other code pages
	and UNICODE encodings.</p>
 */

#include	"../StroikaPreComp.h"

#include	<algorithm>
#include	<cctype>
#include	<vector>

// CURRENTLY NEEDED FOR TYPE 'USHORT' - but could avoid that dependency!!!
#if		defined(_WIN32)
	#include	<Windows.h>
#endif

#include	"../Configuration/Basics.h"



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

			CodePage	Win32CharSetToCodePage (unsigned char lfCharSet);

			CodePage	Win32PrimaryLangIDToCodePage (USHORT languageIdenifier);


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
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;
					nonvirtual	size_t	MapToUNICODE_QuickComputeOutBufSize (const char* inMBChars, size_t inMBCharCnt) const;
					nonvirtual	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
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




			/*
			@CLASS:			TableDrivenCodePageConverter<CODEPAGE>
			@DESCRIPTION:
					<p>Helper class - probably should not be directly used.</p>
			*/
			template	<CodePage CODEPAGE>
				class	TableDrivenCodePageConverter {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
				};


			template<>
				class	TableDrivenCodePageConverter<kCodePage_ANSI> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};
			template<>
				class	TableDrivenCodePageConverter<kCodePage_MAC> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};
			template<>
				class	TableDrivenCodePageConverter<kCodePage_PC> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};
			template<>
				class	TableDrivenCodePageConverter<kCodePage_PCA> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};
			template<>
				class	TableDrivenCodePageConverter<kCodePage_GREEK> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};
			template<>
				class	TableDrivenCodePageConverter<kCodePage_Turkish> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};
			template<>
				class	TableDrivenCodePageConverter<kCodePage_HEBREW> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};
			template<>
				class	TableDrivenCodePageConverter<kCodePage_ARABIC> {
					public:
						static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);
						static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);
					private:
						static	const	wchar_t	kMap[256];
				};







			class	Win32_CodePageConverter {
				public:
					Win32_CodePageConverter (CodePage codePage);

				public:
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;
					nonvirtual	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;

				private:
					CodePage	fCodePage;
			};





			class	UTF8Converter {
				public:
					nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;
					nonvirtual	size_t	MapToUNICODE_QuickComputeOutBufSize (const char* inMBChars, size_t inMBCharCnt) const;
					nonvirtual	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;
					nonvirtual	size_t	MapFromUNICODE_QuickComputeOutBufSize (const wchar_t* inChars, size_t inCharCnt) const;
			};






			/*
			@CLASS:			CodePagesInstalled
			@DESCRIPTION:
						<p>Helper class to check what code pages are installed on a given machine.</p>
			*/
			class	CodePagesInstalled {
				public:
					struct	CodePageNames;

				public:
					static	const vector<CodePage>&	GetAll ();
					static	bool					IsCodePageAvailable (CodePage cp);
					static	CodePage				GetDefaultCodePage ();

				private:
					static	void	Init ();
					static	void	AddIfNotPresent (CodePage cp);

				private:
					static	BOOL FAR	PASCAL	EnumCodePagesProc (LPTSTR lpCodePageString);

				private:
					static	vector<CodePage>	sCodePages;
			};






			/*
			@CLASS:			CodePagesGuesser
			@DESCRIPTION:	<p>Guess the code page of the given argument text.</p>
			*/
			class	CodePagesGuesser {
				public:
					enum	Confidence { eLow = 0, eMedium = 10, eHigh=100 };

				public:
					nonvirtual	CodePage	Guess (const void* input, size_t nBytes, Confidence* confidence = NULL, size_t* bytesFromFrontToStrip = NULL);
			};


		}
	}
}
#endif	/*__Stroika_Foundation_Characters_CodePage_h__*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"CodePage.inl"
