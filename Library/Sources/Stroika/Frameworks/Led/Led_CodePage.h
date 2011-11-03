/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	__LedCodePage_h__
#define	__LedCodePage_h__	1


/*
@MODULE:	CodePage
@DESCRIPTION:
		<p>This module is designed to provide mappings between wide UNICODE and various other code pages
	and UNICODE encodings.</p>
 */

#include	<algorithm>
#include	<cctype>

#include	"Led_Support.h"



namespace	Stroika {	
	namespace	Frameworks {
		namespace	Led {


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


#if		qWindows
CodePage	Win32PrimaryLangIDToCodePage (USHORT languageIdenifier);
#endif


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







#if		qWindows
class	Win32_CodePageConverter {
	public:
		Win32_CodePageConverter (CodePage codePage);

	public:
		nonvirtual	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const;
		nonvirtual	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const;

	private:
		CodePage	fCodePage;
};
#endif





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

#if		qWindows
	private:
		static	BOOL FAR	PASCAL	EnumCodePagesProc (LPTSTR lpCodePageString);
#endif
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




/*
@CLASS:			CodePagePrettyNameMapper
@DESCRIPTION:	<p>Code to map numeric code pages to symbolic user-interface appropriate names.</p>
*/
class	CodePagePrettyNameMapper {
	public:
		static	Led_SDK_String	GetName (CodePage cp);

	public:
		struct	CodePageNames;

	// This class builds commands with command names. The UI may wish to change these
	// names (eg. to customize for particular languages, etc)
	// Just patch these strings here, and commands will be created with these names.
	// (These names appear in text of undo menu item)
	public:
		static	const CodePageNames&	GetCodePageNames ();
		static	void					SetCodePageNames (const CodePageNames& cmdNames);
		static	CodePageNames			MakeDefaultCodePageNames ();
	private:
		static	CodePageNames	sCodePageNames;
};
struct	CodePagePrettyNameMapper::CodePageNames {
	Led_SDK_String	fUNICODE_WIDE;
	Led_SDK_String	fUNICODE_WIDE_BIGENDIAN;
	Led_SDK_String	fANSI;
	Led_SDK_String	fMAC;
	Led_SDK_String	fPC;
	Led_SDK_String	fSJIS;
	Led_SDK_String	fUTF7;
	Led_SDK_String	fUTF8;
	Led_SDK_String	f850;
	Led_SDK_String	f851;
	Led_SDK_String	f866;
	Led_SDK_String	f936;
	Led_SDK_String	f949;
	Led_SDK_String	f950;
	Led_SDK_String	f1250;
	Led_SDK_String	f1251;
	Led_SDK_String	f10000;
	Led_SDK_String	f10001;
	Led_SDK_String	f50220;
};






/*
@CLASS:			CharacterProperties
@DESCRIPTION:	<p>Portable wrapper on stuff like @'isalpha' or @'iswalpha' that works with @'Led_tChar's and calls the right function
			for the particular OS/SDK and character type.</p>
*/
class	CharacterProperties {
	public:
		static	bool	IsAlpha (Led_tChar c);
		static	bool	IsAlnum (Led_tChar c);
		static	bool	IsPunct (Led_tChar c);
		static	bool	IsSpace (Led_tChar c);
		static	bool	IsCntrl (Led_tChar c);
		static	bool	IsDigit (Led_tChar c);

#if		qWideCharacters
	public:
		static	bool	IsMirrored (Led_tChar c, Led_tChar* mirrorChar);
#endif

#if		qWideCharacters
	public:
		static	bool	IsAlpha_M (wchar_t c);
		static	bool	IsAlnum_M (wchar_t c);
		static	bool	IsPunct_M (wchar_t c);
		static	bool	IsSpace_M (wchar_t c);
		static	bool	IsCntrl_M (wchar_t c);
		static	bool	IsDigit_M (wchar_t c);
#endif
};


/*
@CONFIGVAR:		qCharacterPropertiesMemoized
@DESCRIPTION:	<p>If this is true, then the @'CharacterProperties' methods like @'CharacterProperties::IsAlpha' etc will
			use a memoized set of values for that function (from Windows XP). Otherwise, they will do the default
			behavior of just calling iswalpha (or whatever is appopriate).</p>
				<p>The reason we need this is that some OSes and SDKs don't support the iswXXX version of isalpha etc,
			and the existing implementation with MSVC60 and 70 is buggy when it is run on Win9x.</p>
				<p>Defaults to true iff we are using UNICODE (@'qWideCharacters').</p>
 */
#ifndef	qCharacterPropertiesMemoized
	#define	qCharacterPropertiesMemoized				qWideCharacters
#endif





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


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
				Led_Assert (false);		// probably shouldn't come up??
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
				Led_Assert (CP_ACP == 0);
			#else
				const	unsigned char	CP_ACP				=	0;
			#endif

			#ifdef	ANSI_CHARSET
				Led_Assert (ANSI_CHARSET == 0);
			#else
				const	unsigned char	ANSI_CHARSET		=	0;
			#endif
			#ifdef	DEFAULT_CHARSET
				Led_Assert (DEFAULT_CHARSET == 1);
			#else
				const	unsigned char	DEFAULT_CHARSET		=	1;
			#endif
			#ifdef	MAC_CHARSET
				Led_Assert (MAC_CHARSET == 77);
			#else
				const	unsigned char	MAC_CHARSET			=	77;
			#endif
			#ifdef	SHIFTJIS_CHARSET
				Led_Assert (SHIFTJIS_CHARSET == 128);
			#else
				const	unsigned char	SHIFTJIS_CHARSET	=	128;
			#endif
			#ifdef	HANGEUL_CHARSET
				Led_Assert (HANGEUL_CHARSET == 129);
			#else
				const	unsigned char	HANGEUL_CHARSET		=	129;
			#endif
			#ifdef	JOHAB_CHARSET
				Led_Assert (JOHAB_CHARSET == 130);
			#else
				const	unsigned char	JOHAB_CHARSET		=	130;
			#endif
			#ifdef	GB2312_CHARSET
				Led_Assert (GB2312_CHARSET == 134);
			#else
				const	unsigned char	GB2312_CHARSET		=	134;
			#endif
			#ifdef	CHINESEBIG5_CHARSET
				Led_Assert (CHINESEBIG5_CHARSET == 136);
			#else
				const	unsigned char	CHINESEBIG5_CHARSET	=	136;
			#endif
			#ifdef	GREEK_CHARSET
				Led_Assert (GREEK_CHARSET == 161);
			#else
				const	unsigned char	GREEK_CHARSET		=	161;
			#endif
			#ifdef	TURKISH_CHARSET
				Led_Assert (TURKISH_CHARSET == 162);
			#else
				const	unsigned char	TURKISH_CHARSET		=	162;
			#endif
			#ifdef	VIETNAMESE_CHARSET
				Led_Assert (VIETNAMESE_CHARSET == 163);
			#else
				const	unsigned char	VIETNAMESE_CHARSET	=	163;
			#endif
			#ifdef	HEBREW_CHARSET
				Led_Assert (HEBREW_CHARSET == 177);
			#else
				const	unsigned char	HEBREW_CHARSET		=	177;
			#endif
			#ifdef	ARABIC_CHARSET
				Led_Assert (ARABIC_CHARSET == 178);
			#else
				const	unsigned char	ARABIC_CHARSET		=	178;
			#endif
			#ifdef	BALTIC_CHARSET
				Led_Assert (BALTIC_CHARSET == 186);
			#else
				const	unsigned char	BALTIC_CHARSET		=	186;
			#endif
			#ifdef	RUSSIAN_CHARSET
				Led_Assert (RUSSIAN_CHARSET == 204);
			#else
				const	unsigned char	RUSSIAN_CHARSET		=	204;
			#endif
			#ifdef	THAI_CHARSET
				Led_Assert (THAI_CHARSET == 222);
			#else
				const	unsigned char	THAI_CHARSET		=	222;
			#endif
			#ifdef	EASTEUROPE_CHARSET
				Led_Assert (EASTEUROPE_CHARSET == 238);
			#else
				const	unsigned char	EASTEUROPE_CHARSET	=	238;
			#endif
			#ifdef	OEM_CHARSET
				Led_Assert (OEM_CHARSET == 255);
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
				Led_Require (h == eHandleBOM);
				Led_Arg_Unused (h);
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




#if		qWindows
//	class	CodePageConverter
	inline	Win32_CodePageConverter::Win32_CodePageConverter (int codePage):
			fCodePage (codePage)
			{
			}
#endif






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






//	class	CodePagePrettyNameMapper
	inline	const CodePagePrettyNameMapper::CodePageNames&	CodePagePrettyNameMapper::GetCodePageNames ()
		{
			return sCodePageNames;
		}
	inline	void	CodePagePrettyNameMapper::SetCodePageNames (const CodePageNames& codePageNames)
		{
			sCodePageNames = codePageNames;
		}




//	class	CharacterProperties
#if		qSingleByteCharacters
	inline	bool	CharacterProperties::IsAlpha (Led_tChar c)
		{
			return isalpha (c);
		}
	inline	bool	CharacterProperties::IsAlnum (Led_tChar c)
		{
			return isalnum (c);
		}
	inline	bool	CharacterProperties::IsPunct (Led_tChar c)
		{
			return ispunct (c);
		}
	inline	bool	CharacterProperties::IsSpace (Led_tChar c)
		{
			return isspace (c);
		}
	inline	bool	CharacterProperties::IsCntrl (Led_tChar c)
		{
			return iscntrl (c);
		}
	inline	bool	CharacterProperties::IsDigit (Led_tChar c)
		{
			return isdigit (c);
		}
#endif



		}
	}
}


#endif	/*__LedCodePage_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

