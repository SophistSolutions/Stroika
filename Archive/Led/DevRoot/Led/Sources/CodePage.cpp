/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/CodePage.cpp,v 2.36 2003/06/25 16:39:10 lewis Exp $
 *
 * Changes:
 *	$Log: CodePage.cpp,v $
 *	Revision 2.36  2003/06/25 16:39:10  lewis
 *	SPR#1543: Added CharacterProperties::IsDigit for use in spellchecking code
 *	
 *	Revision 2.35  2003/03/24 14:20:07  lewis
 *	Added qBCCOverloadWIthCallToLowerBoundTemplateBug BCC bug workaround
 *	
 *	Revision 2.34  2003/03/21 14:47:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.33  2003/03/07 21:05:59  lewis
 *	SPR#1327- Added CharacterProperties::IsMirrored (), and used it in the UNISCRIBE version
 *	of TextLayoutBlock_Basic::Construct_UNISCRIBE ().
 *	
 *	Revision 2.32  2003/02/28 19:03:22  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.31  2003/02/27 15:49:33  lewis
 *	had to rename CharacterProperties::isspace etc to CharacterProperties::IsSpace()
 *	to avoid name conflicts with GCC MACRO implemeantion of ISXXXX functions
 *	
 *	Revision 2.30  2003/02/24 21:05:45  lewis
 *	SPR#1308 - change memoize code generation (and generated functions) to be a bit speedier
 *	
 *	Revision 2.29  2003/02/24 19:26:08  lewis
 *	SPR#1307- moved memoizing code from TextBreaks.cpp to CharacterProperties class and used for
 *	all its members. Added qCharacterPropertiesMemoized which defaults to true
 *	
 *	Revision 2.28  2003/02/24 18:31:38  lewis
 *	SPR#1306- Added CharacterProperties class and verison of isspace etc
 *	
 *	Revision 2.27  2003/02/01 16:14:29  lewis
 *	SPR#1277- cleanup assert in CodePageConverter::MapFromUNICODE
 *	
 *	Revision 2.26  2003/01/11 19:28:41  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.25  2003/01/05 02:37:52  lewis
 *	use enum names like kCodePage_ANSI for arg to TableDrivenCodePageConverter<> predefined classes.
 *	And - add 4 more: Greek, Turkish, Hebrew, and Arabic
 *	
 *	Revision 2.24  2002/05/06 21:33:42  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.23  2002/05/01 22:05:05  lewis
 *	added explicit static_cast<>s for type conversions to silence warnings from VC7.Net
 *	
 *	Revision 2.22  2002/04/23 14:40:23  lewis
 *	change label for SJIS codepage name
 *	
 *	Revision 2.21  2001/11/27 00:29:49  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.20  2001/10/17 20:42:56  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.19  2001/09/26 15:41:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.18  2001/09/18 16:40:00  lewis
 *	fix so compiles on mac
 *	
 *	Revision 2.17  2001/09/18 15:32:03  lewis
 *	SPR#1036- Use CodePagesGuesser to fix CodePageConverver::MapToUNICODE
 *	
 *	Revision 2.16  2001/09/15 21:26:52  lewis
 *	SPR#1025- added portable UTF8 support
 *	
 *	Revision 2.15  2001/09/12 17:23:25  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.14  2001/09/12 15:57:32  lewis
 *	include stdio- needed for X- compile
 *	
 *	Revision 2.13  2001/09/11 22:29:15  lewis
 *	SPR#1018- preliminary CWPro7 support
 *	
 *	Revision 2.12  2001/09/11 12:26:41  lewis
 *	SPR#1022- add BOM flag (&CTOR overload) to CodePageConverter class to fix problems
 *	with BOM handling - esp UTF7/8
 *	
 *	Revision 2.11  2001/09/09 23:05:05  lewis
 *	make last 2 args to CodePagesGuesser::Guess () optional, and add docs
 *	
 *	Revision 2.10  2001/09/09 22:36:55  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2001/09/09 22:27:36  lewis
 *	SPR#0944/0969- added CodePagePrettyNameMapper, CodePagesGuesser, CodePagesInstalled etc,
 *	and new codePage#s for kCodePage_UNICODE_WIDE and kCodePage_UNICODE_WIDE_BIGENDIAN.
 *	Fixed CodePageConverter code to handle these new wide pseudo-codepages. Basiclly -
 *	did all the common lib support for full code page conversion encoding UI (at least for PC)
 *	in LedLineIt
 *	
 *	Revision 2.8  2001/08/29 23:01:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.7  2001/08/28 18:43:33  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.6  2000/07/23 16:22:29  lewis
 *	SPR#0813
 *	
 *	Revision 2.5  2000/07/23 16:17:07  lewis
 *	Add CodePageConverter::MapFromUNICODE_QuickComputeOutBufSize. Add CodePageConverter::CodePageNotSupportedException
 *	and throw if code page not supproted instead of silently copying wrong values
 *	
 *	Revision 2.4  2000/04/29 22:45:06  lewis
 *	fix MapUNICODEToMBYTE version of the static routine generator, and re-gernated all the
 *	various routines - SPR#0754
 *	
 *	Revision 2.3  2000/04/29 13:18:35  lewis
 *	SPR#0747- a lot of work towards rewriting the char input/output RTF code to support UNICODE. Basicly -
 *	instead of having single-byte to single-byte mapping tables, and using character NAMEs to associate
 *	them - use UNICODE CHAR VALUES directly to associate them. Actually - all I've implemented now (usign
 *	the qSupportNewCharsetCode define) is the UNICODE side. I then need to go back and get the single-byte
 *	to single-byte code working. And fixed some more bugs with the COdePage mapping stuff - but its still
 *	pretty buggy (UNICODE->MBYTE static tables broken. Just a tmp checkin of my current progress. ITs
 *	somewhat working now. YOu can read in Japanese RTF from Word, and very minimal European stuff works
 *	iwth read/write. Next todo is \u support in reader/writer
 *	
 *	Revision 2.2  2000/04/28 01:40:28  lewis
 *	use CodePage typedef
 *	
 *	Revision 2.1  2000/04/28 01:33:31  lewis
 *	NEW codepage helper module
 *	
 *	
 *
 *
 *
 */


#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<algorithm>
#include	<cstdio>


#include	"LedConfig.h"



#ifndef	qBuildInTableDrivenCodePageBuilderProc
	#define	qBuildInTableDrivenCodePageBuilderProc		0
#endif


/*
 *	Hack to build 'myiswalpha' and 'myiswpunct' for SPR#1220 (revised and moved here for SPR#1306 and class
 *	'CharacterProperties').
 */
#ifndef	qBuildMemoizedISXXXBuilderProc
	#define	qBuildMemoizedISXXXBuilderProc				0
#endif



/*
 *	Use this to test my IsWXXX functions produce the right results. Only test under WinXP,
 *	since that is the reference they are copying (SPR#1229).
 */
#ifndef	qTestMyISWXXXFunctions
	#define	qTestMyISWXXXFunctions						0
#endif





#if		qBuildInTableDrivenCodePageBuilderProc || qBuildMemoizedISXXXBuilderProc
	#include	<fstream>
#endif





#include	"CodePage.h"



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif




namespace {

	#if		qBuildInTableDrivenCodePageBuilderProc
		static	void	WriteCodePageTable (CodePage codePage);

		struct	DoRunIt {
			DoRunIt ()
			{
				//WriteCodePageTable (kCodePage_ANSI);
				//WriteCodePageTable (kCodePage_MAC);
				//WriteCodePageTable (kCodePage_PC);
				//WriteCodePageTable (kCodePage_PCA);
				//WriteCodePageTable (kCodePage_GREEK);
				//WriteCodePageTable (kCodePage_Turkish);
				//WriteCodePageTable (kCodePage_HEBREW);
				//WriteCodePageTable (kCodePage_ARABIC);
			}
		}	gRunIt;
	#endif


	#if		qBuildMemoizedISXXXBuilderProc
	template	<class FUNCTION>
		void	WriteMemoizedIsXXXProc (FUNCTION function, const string& origFunctionName, const string& functionName)
				{
					ofstream	outStream ("IsXXXProc.txt");

					outStream << "bool	" << functionName << " (wchar_t c)\n";
					outStream << "{\n";

					outStream << "\t// ********** " << functionName << " (AUTOGENERATED memoize of " << origFunctionName << " - " << __DATE__ << ") ***********\n";
					outStream << "\t// Hack for SPR#1220 and SPR#1306\n";

					// SPR#1308 - the generated if/then skipchain can be somewhat long, and therefore inefficient.
					// This is needlessly so. In principle - we could break up the long skipchain into many smaller ones
					// nested hierarchically. This would involve first pre-computing the entire list of regions, and
					// then generating the if/then code recursively since before you can generate INNER code you need
					// to know the full range of codepoints contained in all contained ifs.
					//
					// Anyhow - there is a trivial implementation that gets us most of the speedup we seek - since most
					// characters looked up fall in the 0..256 range. So - just handle that specially.
					//

					const	wchar_t	kBoundaryForSpecailLoop1	=	255;
					const	wchar_t	kBoundaryForSpecailLoop2	=	5000;

					for (int j = 0; j <= 2; ++j) {

						if (j == 0) {
							outStream << "\tif (c < " << int (kBoundaryForSpecailLoop1) << ") {\n";
						}
						else if (j == 1) {
							outStream << "\telse if (c < " << int (kBoundaryForSpecailLoop2) << ") {\n";
						}
						else {
							outStream << "\telse {\n";
						}

						outStream << "\t\tif (";
						bool	firstTime	=	true;
						bool	hasLastTrue	=	false;
						size_t	firstRangeIdxTrue	=	0;
						size_t	startLoop			=	0;
						size_t	endLoop				=	0;
						if (j == 0) {
							endLoop = kBoundaryForSpecailLoop1;
						}
						else if (j == 1) {
							startLoop = kBoundaryForSpecailLoop1;
							endLoop = kBoundaryForSpecailLoop2;
						}
						else {
							startLoop = kBoundaryForSpecailLoop2;
							endLoop = 256*256;
						}
						for (size_t i = startLoop; i < endLoop; ++i) {
							bool	isT		=	function (static_cast<wchar_t> (i));

							if (((not isT) or (i + 1 == endLoop))  and hasLastTrue) {
								// then emit the range...
								if (not firstTime) {
									outStream << "\t\t\t||";
								}
								size_t	rangeEnd	=	isT? i: i-1;
								outStream << "(" << firstRangeIdxTrue << " <= c && c <= " << rangeEnd << ")";
								firstTime = false;
								outStream << "\n";
								hasLastTrue = false;
							}
							if (isT and not hasLastTrue) {
								firstRangeIdxTrue = i;
								hasLastTrue = true;
							}
						}
						if (firstTime) {
							outStream << "false";
						}

						outStream << "\t\t\t) {\n";
						outStream << "\t\t\t#if		qTestMyISWXXXFunctions\n";
						outStream << "\t\t\tLed_Assert (" << origFunctionName << "(c));\n";
						outStream << "\t\t\t#endif\n";
						outStream << "\t\t\treturn true;\n";
						outStream << "\t\t}\n";

						outStream << "\t}\n";

					}
					outStream << "\t#if		qTestMyISWXXXFunctions\n";
					outStream << "\tLed_Assert (!" << origFunctionName << "(c));\n";
					outStream << "\t#endif\n";
					outStream << "\treturn false;\n";
					outStream << "}\n";
					outStream << "\n";
				}

		struct	DoRunIt {
			DoRunIt ()
			{
				WriteMemoizedIsXXXProc (iswalpha, "iswalpha", "CharacterProperties::IsAlpha_M");
				//WriteMemoizedIsXXXProc (iswalnum, "iswalnum", "CharacterProperties::Ialnum_M");
				//WriteMemoizedIsXXXProc (iswpunct, "iswpunct", "CharacterProperties::IsPunct_M");
				//WriteMemoizedIsXXXProc (iswspace, "iswspace", "CharacterProperties::IsSpace_M");
				//WriteMemoizedIsXXXProc (iswcntrl, "iswcntrl", "CharacterProperties::IsCntrl_M");
				//WriteMemoizedIsXXXProc (iswdigit, "iswdigit", "CharacterProperties::IsDigit_M");
			}
		}	gRunIt;
	#endif


	#if		qTestMyISWXXXFunctions
		class	MyIsWXXXTesterFunctions {
			public:
				MyIsWXXXTesterFunctions ()
					{
						for (wchar_t c = 0; c < 0xffff; ++c) {
							Led_Assert (CharacterProperties::IsAlpha_M (c) == !!iswalpha (c));
							Led_Assert (CharacterProperties::IsAlnum_M (c) == !!iswalnum (c));
							Led_Assert (CharacterProperties::IsPunct_M (c) == !!iswpunct (c));
							Led_Assert (CharacterProperties::IsSpace_M (c) == !!iswspace (c));
							Led_Assert (CharacterProperties::IsCntrl_M (c) == !!iswcntrl (c));
							Led_Assert (CharacterProperties::IsDigit_M (c) == !!iswdigit (c));
						}
					}
		}	sMyIsWXXXTesterFunctions;
	#endif

}







/*
 ********************************************************************************
 ******************************** CodePageConverter *****************************
 ********************************************************************************
 */
/*
@METHOD:		CodePageConverter::MapToUNICODE
@DESCRIPTION:	<p>Map the given multibyte chars in the fCodePage codepage into wide UNICODE
	characters. Pass in a buffer 'outChars' of
	size large enough to accomodate those characrters.</p>
		<p>'outCharCnt' is the size of the output buffer coming in, and it contains the number
	of UNICODE chars copied out on return.</p>
*/
void	CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
{
	Led_Require (inMBCharCnt == 0 or inMBChars != NULL);
	Led_RequireNotNil (outCharCnt);
	Led_Require (*outCharCnt == 0 or outChars != NULL);

	if (GetHandleBOM ()) {
		size_t			bytesToStrip	=	0;
		if (CodePagesGuesser ().Guess (inMBChars, inMBCharCnt, NULL, &bytesToStrip) == fCodePage) {
			Led_Assert (inMBCharCnt >= bytesToStrip);
			inMBChars += bytesToStrip;
			inMBCharCnt -= bytesToStrip;
		}
	}
	switch (fCodePage) {
		case	kCodePage_ANSI:		TableDrivenCodePageConverter<kCodePage_ANSI>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_MAC:		TableDrivenCodePageConverter<kCodePage_MAC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_PC:		TableDrivenCodePageConverter<kCodePage_PC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_PCA:		TableDrivenCodePageConverter<kCodePage_PCA>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_GREEK:	TableDrivenCodePageConverter<kCodePage_GREEK>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_Turkish:	TableDrivenCodePageConverter<kCodePage_Turkish>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_HEBREW:	TableDrivenCodePageConverter<kCodePage_HEBREW>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_ARABIC:	TableDrivenCodePageConverter<kCodePage_ARABIC>::MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt); break;
		case	kCodePage_UNICODE_WIDE: {
			const wchar_t*	inWChars	=	reinterpret_cast<const wchar_t*> (inMBChars);
			size_t			inWCharCnt	=	(inMBCharCnt / sizeof (wchar_t));
			*outCharCnt = inWCharCnt;
			(void)::memcpy (outChars, inWChars, inWCharCnt * sizeof (wchar_t));
		}
		break;
		case	kCodePage_UNICODE_WIDE_BIGENDIAN: {
			const wchar_t*	inWChars	=	reinterpret_cast<const wchar_t*> (inMBChars);
			size_t			inWCharCnt	=	(inMBCharCnt / sizeof (wchar_t));
			*outCharCnt = inWCharCnt;
			for (size_t i = 0; i < inWCharCnt; ++i) {
				wchar_t	c	=	inWChars[i];
				// byteswap
				c = ((c&0xff) << 8) + (c>>8);
				outChars[i] = c;
			}
		}
		break;
		case	kCodePage_UTF8:		{
			UTF8Converter ().MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
		}
		break;
		default: {
			#if		qWindows
				Win32_CodePageConverter (fCodePage).MapToUNICODE (inMBChars, inMBCharCnt, outChars, outCharCnt);
			#else
				throw CodePageNotSupportedException (fCodePage);
			#endif
		}
	}

#if		qDebug && qWindows
	// Assure my baked tables (and UTF8 converters) perform the same as the builtin Win32 API
	if (fCodePage == kCodePage_ANSI or
		fCodePage == kCodePage_MAC or
		fCodePage == kCodePage_PC or
		fCodePage == kCodePage_PCA or
		fCodePage == kCodePage_GREEK or
		fCodePage == kCodePage_Turkish or
		fCodePage == kCodePage_HEBREW or
		fCodePage == kCodePage_ARABIC or
		fCodePage == kCodePage_UTF8
	) {
		size_t	tstCharCnt	=	*outCharCnt;
		Led_SmallStackBuffer<wchar_t>	tstBuf (*outCharCnt);

		Win32_CodePageConverter (fCodePage).MapToUNICODE (inMBChars, inMBCharCnt, tstBuf, &tstCharCnt);
		Led_Assert (tstCharCnt == *outCharCnt);
		Led_Assert (memcmp (tstBuf, outChars, sizeof (wchar_t)*tstCharCnt) == 0);
	}
#endif
}

void	CodePageConverter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
	Led_Require (inCharCnt == 0 or inChars != NULL);
	Led_RequireNotNil (outCharCnt);
	Led_Require (*outCharCnt == 0 or outChars != NULL);
	switch (fCodePage) {
		case	kCodePage_ANSI:		TableDrivenCodePageConverter<kCodePage_ANSI>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt); break;
		case	kCodePage_MAC:		TableDrivenCodePageConverter<kCodePage_MAC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt); break;
		case	kCodePage_PC:		TableDrivenCodePageConverter<kCodePage_PC>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt); break;
		case	kCodePage_PCA:		TableDrivenCodePageConverter<kCodePage_PCA>::MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt); break;
		case	kCodePage_UNICODE_WIDE: {
			if (*outCharCnt >= 2) {
				wchar_t*	outWBytes		=	reinterpret_cast<wchar_t*> (outChars);
				size_t		outByteCount	=	(inCharCnt * sizeof (wchar_t));
				if (GetHandleBOM ()) {
					outWBytes++;		// skip BOM
					outByteCount -= 2;	// subtract for BOM
				}
				*outCharCnt = outByteCount;
				if (GetHandleBOM ()) {
					outChars[0] = '\xff';
					outChars[1] = '\xfe';
				}
				(void)::memcpy (outWBytes, inChars, inCharCnt * sizeof (wchar_t));
			}
			else {
				*outCharCnt = 0;
			}
		}
		break;
		case	kCodePage_UNICODE_WIDE_BIGENDIAN: {
			if (*outCharCnt >= 2) {
				wchar_t*	outWBytes		=	reinterpret_cast<wchar_t*> (outChars);
				size_t		outByteCount	=	(inCharCnt * sizeof (wchar_t));
				if (GetHandleBOM ()) {
					outWBytes++;		// skip BOM
					outByteCount -= 2;	// subtract for BOM
				}
				*outCharCnt = outByteCount;
				if (GetHandleBOM ()) {
					outChars[0] =  '\xfe';
					outChars[1] =  '\xff';
				}
				for (size_t i = 0; i < inCharCnt; ++i) {
					wchar_t	c	=	inChars[i];
					// byteswap
					c = ((c&0xff) << 8) + (c>>8);
					outWBytes[i] = c;
				}
			}
			else {
				*outCharCnt = 0;
			}
		}
		break;
		case	kCodePage_UTF7: {
			char*	useOutChars		=	outChars;
			size_t	useOutCharCount	=	*outCharCnt;
			if (GetHandleBOM ()) {
				if (*outCharCnt >= 5) {
					useOutChars += 5;	// skip BOM
					useOutCharCount -= 5;
					outChars[0] = 0x2b;
					outChars[1] = 0x2f;
					outChars[2] = 0x76;
					outChars[3] = 0x38;
					outChars[4] = 0x2d;
				}
				else {
					useOutCharCount = 0;
				}
			}
			#if		qWindows
				Win32_CodePageConverter (kCodePage_UTF7).MapFromUNICODE (inChars, inCharCnt, useOutChars, &useOutCharCount);
			#else
				throw CodePageNotSupportedException (fCodePage);
			#endif
			if (GetHandleBOM ()) {
				if (*outCharCnt >= 5) {
					useOutCharCount += 5;
				}
			}
			*outCharCnt = useOutCharCount;
		}
		break;
		case	kCodePage_UTF8: {
			char*	useOutChars		=	outChars;
			size_t	useOutCharCount	=	*outCharCnt;
			if (GetHandleBOM ()) {
				if (*outCharCnt >= 3) {
					useOutChars += 3;	// skip BOM
					useOutCharCount -= 3;
					reinterpret_cast<unsigned char*> (outChars)[0] = 0xef;
					reinterpret_cast<unsigned char*> (outChars)[1] = 0xbb;
					reinterpret_cast<unsigned char*> (outChars)[2] = 0xbf;
				}
				else {
					useOutCharCount = 0;
				}
			}
			#if		qWindows
				UTF8Converter ().MapFromUNICODE (inChars, inCharCnt, useOutChars, &useOutCharCount);
			#else
				throw CodePageNotSupportedException (fCodePage);
			#endif
			if (GetHandleBOM ()) {
				if (*outCharCnt >= 3) {
					useOutCharCount += 3;
				}
			}
			*outCharCnt = useOutCharCount;
		}
		break;
		default: {
			#if		qWindows
				Win32_CodePageConverter (fCodePage).MapFromUNICODE (inChars, inCharCnt, outChars, outCharCnt);
			#else
				throw CodePageNotSupportedException (fCodePage);
			#endif
		}
	}

#if		qDebug && qWindows
	// Assure my baked tables perform the same as the builtin Win32 API
	if (fCodePage == kCodePage_ANSI or fCodePage == kCodePage_MAC or fCodePage == kCodePage_PC or fCodePage == kCodePage_PCA) {
		size_t						win32TstCharCnt	=	*outCharCnt;
		Led_SmallStackBuffer<char>	win32TstBuf (*outCharCnt);

		Win32_CodePageConverter (fCodePage).MapFromUNICODE (inChars, inCharCnt, win32TstBuf, &win32TstCharCnt);
		// SPR#0813 (and SPR#1277) - assert this produces the right result OR a '?' character -
		// used for bad conversions. Reason is cuz for characters that don't map - our table and
		// the system table can differ in how they map depending on current OS code page.
		Led_Assert (win32TstCharCnt == *outCharCnt or outChars[0] == '?');
		Led_Assert (memcmp (win32TstBuf, outChars, win32TstCharCnt) == 0 or outChars[0] == '?');
	}
#endif
}







#if		qWindows
/*
 ********************************************************************************
 **************************** Win32_CodePageConverter ***************************
 ********************************************************************************
 */
void	Win32_CodePageConverter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
{
	Led_Require (inMBCharCnt == 0 or inMBChars != NULL);
	Led_RequireNotNil (outCharCnt);
	Led_Require (*outCharCnt == 0 or outChars != NULL);
//	*outCharCnt	= ::MultiByteToWideChar (fCodePage, MB_ERR_INVALID_CHARS, inMBChars, inMBCharCnt, outChars, *outCharCnt);
	*outCharCnt	= ::MultiByteToWideChar (fCodePage, 0, inMBChars, inMBCharCnt, outChars, *outCharCnt);
}

void	Win32_CodePageConverter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
	Led_Require (inCharCnt == 0 or inChars != NULL);
	Led_RequireNotNil (outCharCnt);
	Led_Require (*outCharCnt == 0 or outChars != NULL);
	*outCharCnt	= ::WideCharToMultiByte (fCodePage, 0, inChars, inCharCnt, outChars, *outCharCnt, NULL, NULL);
}
#endif











/*
 ********************************************************************************
 ***************** TableDrivenCodePageConverter<kCodePage_ANSI> *****************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_ANSI>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0x20ac,		0x81,		0x201a,		0x192,		0x201e,		0x2026,		0x2020,		0x2021,
		0x2c6,		0x2030,		0x160,		0x2039,		0x152,		0x8d,		0x17d,		0x8f,
		0x90,		0x2018,		0x2019,		0x201c,		0x201d,		0x2022,		0x2013,		0x2014,
		0x2dc,		0x2122,		0x161,		0x203a,		0x153,		0x9d,		0x17e,		0x178,
		0xa0,		0xa1,		0xa2,		0xa3,		0xa4,		0xa5,		0xa6,		0xa7,
		0xa8,		0xa9,		0xaa,		0xab,		0xac,		0xad,		0xae,		0xaf,
		0xb0,		0xb1,		0xb2,		0xb3,		0xb4,		0xb5,		0xb6,		0xb7,
		0xb8,		0xb9,		0xba,		0xbb,		0xbc,		0xbd,		0xbe,		0xbf,
		0xc0,		0xc1,		0xc2,		0xc3,		0xc4,		0xc5,		0xc6,		0xc7,
		0xc8,		0xc9,		0xca,		0xcb,		0xcc,		0xcd,		0xce,		0xcf,
		0xd0,		0xd1,		0xd2,		0xd3,		0xd4,		0xd5,		0xd6,		0xd7,
		0xd8,		0xd9,		0xda,		0xdb,		0xdc,		0xdd,		0xde,		0xdf,
		0xe0,		0xe1,		0xe2,		0xe3,		0xe4,		0xe5,		0xe6,		0xe7,
		0xe8,		0xe9,		0xea,		0xeb,		0xec,		0xed,		0xee,		0xef,
		0xf0,		0xf1,		0xf2,		0xf3,		0xf4,		0xf5,		0xf6,		0xf7,
		0xf8,		0xf9,		0xfa,		0xfb,		0xfc,		0xfd,		0xfe,		0xff,
};

void	TableDrivenCodePageConverter<kCodePage_ANSI>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_ANSI>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}










/*
 ********************************************************************************
 ********************* TableDrivenCodePageConverter<kCodePage_MAC> **************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_MAC>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0xc4,		0xc5,		0xc7,		0xc9,		0xd1,		0xd6,		0xdc,		0xe1,
		0xe0,		0xe2,		0xe4,		0xe3,		0xe5,		0xe7,		0xe9,		0xe8,
		0xea,		0xeb,		0xed,		0xec,		0xee,		0xef,		0xf1,		0xf3,
		0xf2,		0xf4,		0xf6,		0xf5,		0xfa,		0xf9,		0xfb,		0xfc,
		0x2020,		0xb0,		0xa2,		0xa3,		0xa7,		0x2022,		0xb6,		0xdf,
		0xae,		0xa9,		0x2122,		0xb4,		0xa8,		0x2260,		0xc6,		0xd8,
		0x221e,		0xb1,		0x2264,		0x2265,		0xa5,		0xb5,		0x2202,		0x2211,
		0x220f,		0x3c0,		0x222b,		0xaa,		0xba,		0x2126,		0xe6,		0xf8,
		0xbf,		0xa1,		0xac,		0x221a,		0x192,		0x2248,		0x2206,		0xab,
		0xbb,		0x2026,		0xa0,		0xc0,		0xc3,		0xd5,		0x152,		0x153,
		0x2013,		0x2014,		0x201c,		0x201d,		0x2018,		0x2019,		0xf7,		0x25ca,
		0xff,		0x178,		0x2044,		0x20ac,		0x2039,		0x203a,		0xfb01,		0xfb02,
		0x2021,		0xb7,		0x201a,		0x201e,		0x2030,		0xc2,		0xca,		0xc1,
		0xcb,		0xc8,		0xcd,		0xce,		0xcf,		0xcc,		0xd3,		0xd4,
		0xf8ff,		0xd2,		0xda,		0xdb,		0xd9,		0x131,		0x2c6,		0x2dc,
		0xaf,		0x2d8,		0x2d9,		0x2da,		0xb8,		0x2dd,		0x2db,		0x2c7,
};

void	TableDrivenCodePageConverter<kCodePage_MAC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_MAC>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}







/*
 ********************************************************************************
 ********************* TableDrivenCodePageConverter<kCodePage_PC> ***************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_PC>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0xc7,		0xfc,		0xe9,		0xe2,		0xe4,		0xe0,		0xe5,		0xe7,
		0xea,		0xeb,		0xe8,		0xef,		0xee,		0xec,		0xc4,		0xc5,
		0xc9,		0xe6,		0xc6,		0xf4,		0xf6,		0xf2,		0xfb,		0xf9,
		0xff,		0xd6,		0xdc,		0xa2,		0xa3,		0xa5,		0x20a7,		0x192,
		0xe1,		0xed,		0xf3,		0xfa,		0xf1,		0xd1,		0xaa,		0xba,
		0xbf,		0x2310,		0xac,		0xbd,		0xbc,		0xa1,		0xab,		0xbb,
		0x2591,		0x2592,		0x2593,		0x2502,		0x2524,		0x2561,		0x2562,		0x2556,
		0x2555,		0x2563,		0x2551,		0x2557,		0x255d,		0x255c,		0x255b,		0x2510,
		0x2514,		0x2534,		0x252c,		0x251c,		0x2500,		0x253c,		0x255e,		0x255f,
		0x255a,		0x2554,		0x2569,		0x2566,		0x2560,		0x2550,		0x256c,		0x2567,
		0x2568,		0x2564,		0x2565,		0x2559,		0x2558,		0x2552,		0x2553,		0x256b,
		0x256a,		0x2518,		0x250c,		0x2588,		0x2584,		0x258c,		0x2590,		0x2580,
		0x3b1,		0xdf,		0x393,		0x3c0,		0x3a3,		0x3c3,		0xb5,		0x3c4,
		0x3a6,		0x398,		0x3a9,		0x3b4,		0x221e,		0x3c6,		0x3b5,		0x2229,
		0x2261,		0xb1,		0x2265,		0x2264,		0x2320,		0x2321,		0xf7,		0x2248,
		0xb0,		0x2219,		0xb7,		0x221a,		0x207f,		0xb2,		0x25a0,		0xa0,
};

void	TableDrivenCodePageConverter<kCodePage_PC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_PC>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}









/*
 ********************************************************************************
 ********************* TableDrivenCodePageConverter<kCodePage_PCA> **************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_PCA>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0xc7,		0xfc,		0xe9,		0xe2,		0xe4,		0xe0,		0xe5,		0xe7,
		0xea,		0xeb,		0xe8,		0xef,		0xee,		0xec,		0xc4,		0xc5,
		0xc9,		0xe6,		0xc6,		0xf4,		0xf6,		0xf2,		0xfb,		0xf9,
		0xff,		0xd6,		0xdc,		0xf8,		0xa3,		0xd8,		0xd7,		0x192,
		0xe1,		0xed,		0xf3,		0xfa,		0xf1,		0xd1,		0xaa,		0xba,
		0xbf,		0xae,		0xac,		0xbd,		0xbc,		0xa1,		0xab,		0xbb,
		0x2591,		0x2592,		0x2593,		0x2502,		0x2524,		0xc1,		0xc2,		0xc0,
		0xa9,		0x2563,		0x2551,		0x2557,		0x255d,		0xa2,		0xa5,		0x2510,
		0x2514,		0x2534,		0x252c,		0x251c,		0x2500,		0x253c,		0xe3,		0xc3,
		0x255a,		0x2554,		0x2569,		0x2566,		0x2560,		0x2550,		0x256c,		0xa4,
		0xf0,		0xd0,		0xca,		0xcb,		0xc8,		0x131,		0xcd,		0xce,
		0xcf,		0x2518,		0x250c,		0x2588,		0x2584,		0xa6,		0xcc,		0x2580,
		0xd3,		0xdf,		0xd4,		0xd2,		0xf5,		0xd5,		0xb5,		0xfe,
		0xde,		0xda,		0xdb,		0xd9,		0xfd,		0xdd,		0xaf,		0xb4,
		0xad,		0xb1,		0x2017,		0xbe,		0xb6,		0xa7,		0xf7,		0xb8,
		0xb0,		0xa8,		0xb7,		0xb9,		0xb3,		0xb2,		0x25a0,		0xa0,
};

void	TableDrivenCodePageConverter<kCodePage_PCA>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_PCA>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}








/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter<kCodePage_GREEK> *************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_GREEK>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0x20ac,		0x81,		0x201a,		0x192,		0x201e,		0x2026,		0x2020,		0x2021,
		0x88,		0x2030,		0x8a,		0x2039,		0x8c,		0x8d,		0x8e,		0x8f,
		0x90,		0x2018,		0x2019,		0x201c,		0x201d,		0x2022,		0x2013,		0x2014,
		0x98,		0x2122,		0x9a,		0x203a,		0x9c,		0x9d,		0x9e,		0x9f,
		0xa0,		0x385,		0x386,		0xa3,		0xa4,		0xa5,		0xa6,		0xa7,
		0xa8,		0xa9,		0xf8f9,		0xab,		0xac,		0xad,		0xae,		0x2015,
		0xb0,		0xb1,		0xb2,		0xb3,		0x384,		0xb5,		0xb6,		0xb7,
		0x388,		0x389,		0x38a,		0xbb,		0x38c,		0xbd,		0x38e,		0x38f,
		0x390,		0x391,		0x392,		0x393,		0x394,		0x395,		0x396,		0x397,
		0x398,		0x399,		0x39a,		0x39b,		0x39c,		0x39d,		0x39e,		0x39f,
		0x3a0,		0x3a1,		0xf8fa,		0x3a3,		0x3a4,		0x3a5,		0x3a6,		0x3a7,
		0x3a8,		0x3a9,		0x3aa,		0x3ab,		0x3ac,		0x3ad,		0x3ae,		0x3af,
		0x3b0,		0x3b1,		0x3b2,		0x3b3,		0x3b4,		0x3b5,		0x3b6,		0x3b7,
		0x3b8,		0x3b9,		0x3ba,		0x3bb,		0x3bc,		0x3bd,		0x3be,		0x3bf,
		0x3c0,		0x3c1,		0x3c2,		0x3c3,		0x3c4,		0x3c5,		0x3c6,		0x3c7,
		0x3c8,		0x3c9,		0x3ca,		0x3cb,		0x3cc,		0x3cd,		0x3ce,		0xf8fb,
};

void	TableDrivenCodePageConverter<kCodePage_GREEK>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_GREEK>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}








/*
 ********************************************************************************
 **************** TableDrivenCodePageConverter<kCodePage_Turkish> ***************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_Turkish>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0x20ac,		0x81,		0x201a,		0x192,		0x201e,		0x2026,		0x2020,		0x2021,
		0x2c6,		0x2030,		0x160,		0x2039,		0x152,		0x8d,		0x8e,		0x8f,
		0x90,		0x2018,		0x2019,		0x201c,		0x201d,		0x2022,		0x2013,		0x2014,
		0x2dc,		0x2122,		0x161,		0x203a,		0x153,		0x9d,		0x9e,		0x178,
		0xa0,		0xa1,		0xa2,		0xa3,		0xa4,		0xa5,		0xa6,		0xa7,
		0xa8,		0xa9,		0xaa,		0xab,		0xac,		0xad,		0xae,		0xaf,
		0xb0,		0xb1,		0xb2,		0xb3,		0xb4,		0xb5,		0xb6,		0xb7,
		0xb8,		0xb9,		0xba,		0xbb,		0xbc,		0xbd,		0xbe,		0xbf,
		0xc0,		0xc1,		0xc2,		0xc3,		0xc4,		0xc5,		0xc6,		0xc7,
		0xc8,		0xc9,		0xca,		0xcb,		0xcc,		0xcd,		0xce,		0xcf,
		0x11e,		0xd1,		0xd2,		0xd3,		0xd4,		0xd5,		0xd6,		0xd7,
		0xd8,		0xd9,		0xda,		0xdb,		0xdc,		0x130,		0x15e,		0xdf,
		0xe0,		0xe1,		0xe2,		0xe3,		0xe4,		0xe5,		0xe6,		0xe7,
		0xe8,		0xe9,		0xea,		0xeb,		0xec,		0xed,		0xee,		0xef,
		0x11f,		0xf1,		0xf2,		0xf3,		0xf4,		0xf5,		0xf6,		0xf7,
		0xf8,		0xf9,		0xfa,		0xfb,		0xfc,		0x131,		0x15f,		0xff,
};

void	TableDrivenCodePageConverter<kCodePage_Turkish>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_Turkish>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}









/*
 ********************************************************************************
 ***************** TableDrivenCodePageConverter<kCodePage_HEBREW> ***************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_HEBREW>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0x20ac,		0x81,		0x201a,		0x192,		0x201e,		0x2026,		0x2020,		0x2021,
		0x2c6,		0x2030,		0x8a,		0x2039,		0x8c,		0x8d,		0x8e,		0x8f,
		0x90,		0x2018,		0x2019,		0x201c,		0x201d,		0x2022,		0x2013,		0x2014,
		0x2dc,		0x2122,		0x9a,		0x203a,		0x9c,		0x9d,		0x9e,		0x9f,
		0xa0,		0xa1,		0xa2,		0xa3,		0x20aa,		0xa5,		0xa6,		0xa7,
		0xa8,		0xa9,		0xd7,		0xab,		0xac,		0xad,		0xae,		0xaf,
		0xb0,		0xb1,		0xb2,		0xb3,		0xb4,		0xb5,		0xb6,		0xb7,
		0xb8,		0xb9,		0xf7,		0xbb,		0xbc,		0xbd,		0xbe,		0xbf,
		0x5b0,		0x5b1,		0x5b2,		0x5b3,		0x5b4,		0x5b5,		0x5b6,		0x5b7,
		0x5b8,		0x5b9,		0x5ba,		0x5bb,		0x5bc,		0x5bd,		0x5be,		0x5bf,
		0x5c0,		0x5c1,		0x5c2,		0x5c3,		0x5f0,		0x5f1,		0x5f2,		0x5f3,
		0x5f4,		0xf88d,		0xf88e,		0xf88f,		0xf890,		0xf891,		0xf892,		0xf893,
		0x5d0,		0x5d1,		0x5d2,		0x5d3,		0x5d4,		0x5d5,		0x5d6,		0x5d7,
		0x5d8,		0x5d9,		0x5da,		0x5db,		0x5dc,		0x5dd,		0x5de,		0x5df,
		0x5e0,		0x5e1,		0x5e2,		0x5e3,		0x5e4,		0x5e5,		0x5e6,		0x5e7,
		0x5e8,		0x5e9,		0x5ea,		0xf894,		0xf895,		0x200e,		0x200f,		0xf896,
};

void	TableDrivenCodePageConverter<kCodePage_HEBREW>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_HEBREW>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}








/*
 ********************************************************************************
 ******************** TableDrivenCodePageConverter<kCodePage_ARABIC> ************
 ********************************************************************************
 */
const	wchar_t		TableDrivenCodePageConverter<kCodePage_ARABIC>::kMap[256] = {
		0x0,		0x1,		0x2,		0x3,		0x4,		0x5,		0x6,		0x7,
		0x8,		0x9,		0xa,		0xb,		0xc,		0xd,		0xe,		0xf,
		0x10,		0x11,		0x12,		0x13,		0x14,		0x15,		0x16,		0x17,
		0x18,		0x19,		0x1a,		0x1b,		0x1c,		0x1d,		0x1e,		0x1f,
		0x20,		0x21,		0x22,		0x23,		0x24,		0x25,		0x26,		0x27,
		0x28,		0x29,		0x2a,		0x2b,		0x2c,		0x2d,		0x2e,		0x2f,
		0x30,		0x31,		0x32,		0x33,		0x34,		0x35,		0x36,		0x37,
		0x38,		0x39,		0x3a,		0x3b,		0x3c,		0x3d,		0x3e,		0x3f,
		0x40,		0x41,		0x42,		0x43,		0x44,		0x45,		0x46,		0x47,
		0x48,		0x49,		0x4a,		0x4b,		0x4c,		0x4d,		0x4e,		0x4f,
		0x50,		0x51,		0x52,		0x53,		0x54,		0x55,		0x56,		0x57,
		0x58,		0x59,		0x5a,		0x5b,		0x5c,		0x5d,		0x5e,		0x5f,
		0x60,		0x61,		0x62,		0x63,		0x64,		0x65,		0x66,		0x67,
		0x68,		0x69,		0x6a,		0x6b,		0x6c,		0x6d,		0x6e,		0x6f,
		0x70,		0x71,		0x72,		0x73,		0x74,		0x75,		0x76,		0x77,
		0x78,		0x79,		0x7a,		0x7b,		0x7c,		0x7d,		0x7e,		0x7f,
		0x20ac,		0x67e,		0x201a,		0x192,		0x201e,		0x2026,		0x2020,		0x2021,
		0x2c6,		0x2030,		0x679,		0x2039,		0x152,		0x686,		0x698,		0x688,
		0x6af,		0x2018,		0x2019,		0x201c,		0x201d,		0x2022,		0x2013,		0x2014,
		0x6a9,		0x2122,		0x691,		0x203a,		0x153,		0x200c,		0x200d,		0x6ba,
		0xa0,		0x60c,		0xa2,		0xa3,		0xa4,		0xa5,		0xa6,		0xa7,
		0xa8,		0xa9,		0x6be,		0xab,		0xac,		0xad,		0xae,		0xaf,
		0xb0,		0xb1,		0xb2,		0xb3,		0xb4,		0xb5,		0xb6,		0xb7,
		0xb8,		0xb9,		0x61b,		0xbb,		0xbc,		0xbd,		0xbe,		0x61f,
		0x6c1,		0x621,		0x622,		0x623,		0x624,		0x625,		0x626,		0x627,
		0x628,		0x629,		0x62a,		0x62b,		0x62c,		0x62d,		0x62e,		0x62f,
		0x630,		0x631,		0x632,		0x633,		0x634,		0x635,		0x636,		0xd7,
		0x637,		0x638,		0x639,		0x63a,		0x640,		0x641,		0x642,		0x643,
		0xe0,		0x644,		0xe2,		0x645,		0x646,		0x647,		0x648,		0xe7,
		0xe8,		0xe9,		0xea,		0xeb,		0x649,		0x64a,		0xee,		0xef,
		0x64b,		0x64c,		0x64d,		0x64e,		0xf4,		0x64f,		0x650,		0xf7,
		0x651,		0xf9,		0x652,		0xfb,		0xfc,		0x200e,		0x200f,		0x6d2,
};

void	TableDrivenCodePageConverter<kCodePage_ARABIC>::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		outChars[i] = kMap[(unsigned char)inMBChars[i]];
	}
	*outCharCnt = nCharsToCopy;
}

void	TableDrivenCodePageConverter<kCodePage_ARABIC>::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)
{
	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);
	for (size_t i = 0; i < nCharsToCopy; ++i) {
		size_t j = 0;
		for (; j < 256; ++j) {
			if (kMap[j] == inChars[i]) {
				outChars[i] = j;
				break;
			}
		}
		if (j == 256) {
			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/
			outChars[i] = '?';
		}
	}
	*outCharCnt = nCharsToCopy;
}









/*
 ********************************************************************************
 ********************************** UTF8Converter *******************************
 ********************************************************************************
 */

void	UTF8Converter::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt) const
{
	Led_Require (inMBCharCnt == 0 or inMBChars != NULL);
	Led_RequireNotNil (outCharCnt);
	Led_Require (*outCharCnt == 0 or outChars != NULL);
	
	/*
	 *	NOTE - based on ConvertUTF16toUTF8 () code from ConvertUTF.C, written by Mark E. Davis (mark_davis@taligent.com),
	 *	 and owned by Taligtent. That code is copyrighted. It says it cannot be reproduced without the consent of Taligent,
	 *	but the Taligent company doesn't seem to exist anymore (at least no web site). Also - technically,
	 *	I'm not sure if this is a reproduction, since I've rewritten it (somewhat).
	 *	I hope inclusion of this notice is sufficient. -- LGP 2001-09-15
	 *
	 *	Original code was found refered to on web page:	http://www.czyborra.com/utf/
	 *	and downloaded from URL:						ftp://ftp.unicode.org/Public/PROGRAMS/CVTUTF/
	 *
	 *	NB: I COULD get portable UTF7 code from the same location, but it didn't seem worth the trouble.
	 */
	{
		enum ConversionResult {
			ok, 				/* conversion successful */
			sourceExhausted,	/* partial character in source, but hit end */
			targetExhausted		/* insuff. room in target for conversion */
		};
		typedef unsigned long	UCS4;
		typedef unsigned short	UTF16;
		typedef unsigned char	UTF8;
		const UCS4 kReplacementCharacter =	0x0000FFFDUL;
		const UCS4 kMaximumUCS2 =			0x0000FFFFUL;
		const UCS4 kMaximumUTF16 =			0x0010FFFFUL;
		const UCS4 kMaximumUCS4 =			0x7FFFFFFFUL;
		static	const	char bytesFromUTF8[256] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5};
		static	const	UCS4 offsetsFromUTF8[6] =	{0x00000000UL, 0x00003080UL, 0x000E2080UL, 
					 	 							 0x03C82080UL, 0xFA082080UL, 0x82082080UL};
		const int halfShift				= 10;
		const UCS4 halfBase				= 0x0010000UL;
		const UCS4 halfMask				= 0x3FFUL;
		const UCS4 kSurrogateHighStart	= 0xD800UL;
		const UCS4 kSurrogateHighEnd	= 0xDBFFUL;
		const UCS4 kSurrogateLowStart	= 0xDC00UL;
		const UCS4 kSurrogateLowEnd		= 0xDFFFUL;
		ConversionResult result = ok;
		const UTF8* source = reinterpret_cast<const UTF8*> (inMBChars);
		const UTF8* sourceEnd = source + inMBCharCnt;
		UTF16* target = reinterpret_cast<UTF16*> (outChars);
		UTF16* targetEnd = target + *outCharCnt;
		while (source < sourceEnd) {
			register UCS4 ch = 0;
			register unsigned short extraBytesToWrite = bytesFromUTF8[*source];
			if (source + extraBytesToWrite > sourceEnd) {
				result = sourceExhausted; break;
			};
			switch(extraBytesToWrite) {	/* note: code falls through cases! */
				case 5:	ch += *source++; ch <<= 6;
				case 4:	ch += *source++; ch <<= 6;
				case 3:	ch += *source++; ch <<= 6;
				case 2:	ch += *source++; ch <<= 6;
				case 1:	ch += *source++; ch <<= 6;
				case 0:	ch += *source++;
			};
			ch -= offsetsFromUTF8[extraBytesToWrite];

			if (target >= targetEnd) {
				result = targetExhausted; break;
			};
			if (ch <= kMaximumUCS2) {
				*target++ = static_cast<UTF16> (ch);
			} else if (ch > kMaximumUTF16) {
				*target++ = kReplacementCharacter;
			} else {
				if (target + 1 >= targetEnd) {
					result = targetExhausted; break;
				};
				ch -= halfBase;
				*target++ = static_cast<UTF16> ((ch >> halfShift) + kSurrogateHighStart);
				*target++ = static_cast<UTF16> ((ch & halfMask) + kSurrogateLowStart);
			};
		};

		// For now - we ignore ConversionResult flag - and just say how much output was generated...
		//		*sourceStart = source;
		//		*targetStart = target;
		//		return result;
		*outCharCnt = (reinterpret_cast<wchar_t*> (target) - outChars);
	}
}

void	UTF8Converter::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt) const
{
	/*
	 *	NOTE - based on ConvertUTF16toUTF8 () code from ConvertUTF.C, written by Mark E. Davis (mark_davis@taligent.com),
	 *	 and owned by Taligtent. That code is copyrighted. It says it cannot be reproduced without the consent of Taligent,
	 *	but the Taligent company doesn't seem to exist anymore (at least no web site). Also - technically,
	 *	I'm not sure if this is a reproduction, since I've rewritten it (somewhat).
	 *	I hope inclusion of this notice is sufficient. -- LGP 2001-09-15
	 *
	 *	Original code was found refered to on web page:	http://www.czyborra.com/utf/
	 *	and downloaded from URL:						ftp://ftp.unicode.org/Public/PROGRAMS/CVTUTF/
	 *
	 *	NB: I COULD get portable UTF7 code from the same location, but it didn't seem worth the trouble.
	 */
	{
		enum ConversionResult {
			ok, 				/* conversion successful */
			sourceExhausted,	/* partial character in source, but hit end */
			targetExhausted		/* insuff. room in target for conversion */
		};
		typedef unsigned long	UCS4;
		typedef unsigned short	UTF16;
		typedef unsigned char	UTF8;
		const UCS4 kReplacementCharacter =	0x0000FFFDUL;
		const UCS4 kMaximumUCS2 =			0x0000FFFFUL;
		const UCS4 kMaximumUTF16 =			0x0010FFFFUL;
		const UCS4 kMaximumUCS4 =			0x7FFFFFFFUL;
		static	const	char bytesFromUTF8[256] = {
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
			1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
			2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5};
		static	const	UCS4 offsetsFromUTF8[6] =	{0x00000000UL, 0x00003080UL, 0x000E2080UL, 
					 	 							 0x03C82080UL, 0xFA082080UL, 0x82082080UL};
		static	const	UTF8 firstByteMark[7] = {0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC};
		const int halfShift				= 10;
		const UCS4 halfBase				= 0x0010000UL;
		const UCS4 halfMask				= 0x3FFUL;
		const UCS4 kSurrogateHighStart	= 0xD800UL;
		const UCS4 kSurrogateHighEnd	= 0xDBFFUL;
		const UCS4 kSurrogateLowStart	= 0xDC00UL;
		const UCS4 kSurrogateLowEnd		= 0xDFFFUL;
		ConversionResult result = ok;
		const UTF16* source = reinterpret_cast <const UTF16*> (inChars);
		const UTF16* sourceEnd = source + inCharCnt;
		UTF8* target = reinterpret_cast<UTF8*> (outChars);
		const UTF8* targetEnd = target + *outCharCnt;
		while (source < sourceEnd) {
			register UCS4 ch;
			register unsigned short bytesToWrite = 0;
			register const UCS4 byteMask = 0xBF;
			register const UCS4 byteMark = 0x80; 
			ch = *source++;
			if (ch >= kSurrogateHighStart && ch <= kSurrogateHighEnd
					&& source < sourceEnd) {
				register UCS4 ch2 = *source;
				if (ch2 >= kSurrogateLowStart && ch2 <= kSurrogateLowEnd) {
					ch = ((ch - kSurrogateHighStart) << halfShift)
						+ (ch2 - kSurrogateLowStart) + halfBase;
					++source;
				};
			};
			if (ch < 0x80) {				bytesToWrite = 1;
			} else if (ch < 0x800) {		bytesToWrite = 2;
			} else if (ch < 0x10000) {		bytesToWrite = 3;
			} else if (ch < 0x200000) {		bytesToWrite = 4;
			} else if (ch < 0x4000000) {	bytesToWrite = 5;
			} else if (ch <= kMaximumUCS4){	bytesToWrite = 6;
			} else {						bytesToWrite = 2;
											ch = kReplacementCharacter;
			}; /* I wish there were a smart way to avoid this conditional */
			
			target += bytesToWrite;
			if (target > targetEnd) {
				target -= bytesToWrite; result = targetExhausted; break;
			};
			switch (bytesToWrite) {	/* note: code falls through cases! */
				case 6:	*--target = static_cast<UTF8> ((ch | byteMark) & byteMask); ch >>= 6;
				case 5:	*--target = static_cast<UTF8> ((ch | byteMark) & byteMask); ch >>= 6;
				case 4:	*--target = static_cast<UTF8> ((ch | byteMark) & byteMask); ch >>= 6;
				case 3:	*--target = static_cast<UTF8> ((ch | byteMark) & byteMask); ch >>= 6;
				case 2:	*--target = static_cast<UTF8> ((ch | byteMark) & byteMask); ch >>= 6;
				case 1:	*--target = static_cast<UTF8> (ch | firstByteMark[bytesToWrite]);
			};
			target += bytesToWrite;
		};
		// For now - we ignore ConversionResult flag - and just say how much output was generated...
		//		*sourceStart = source;
		//		*targetStart = target;
		//		return result;
		*outCharCnt = (target - reinterpret_cast<UTF8*> (outChars));
	}
	#if		0
		/*
		 *	This code is based on comments in the document:	http://www.czyborra.com/utf/
		 *			-- LGP 2001-09-15
		 */
		char*				outP		=	outChars;
		char*				outEOB		=	outChars + *outCharCnt;
		const	wchar_t*	inCharEnd	=	inChars + inCharCnt;
		for (const wchar_t* i = inChars; i < inCharEnd; ++i) {
			wchar_t	c	=	*i;
			if (c < 0x80) {
				if (outP >= outEOB) {
					return;	// outCharCnt already set. Unclear if/how to signal buffer too small?
				}
				*outP++ = c;
			}
			else if (c < 0x800) {
				if (outP + 1 >= outEOB) {
					return;	// outCharCnt already set. Unclear if/how to signal buffer too small?
				}
				*outP++ = (0xC0 | c>>6);
				*outP++ = (0x80 | c & 0x3F);
			}
			else if (c < 0x10000) {
				if (outP + 2 >= outEOB) {
					return;	// outCharCnt already set. Unclear if/how to signal buffer too small?
				}
				*outP++ = (0xE0 | c>>12);
				*outP++ = (0x80 | c>>6 & 0x3F);
				*outP++ = (0x80 | c & 0x3F);
			}
			else if (c < 0x200000) {
				if (outP + 3 >= outEOB) {
					return;	// outCharCnt already set. Unclear if/how to signal buffer too small?
				}
				*outP++ = (0xF0 | c>>18);
				*outP++ = (0x80 | c>>12 & 0x3F);
				*outP++ = (0x80 | c>>6 & 0x3F);
				*outP++ = (0x80 | c & 0x3F);
			}
			else {
				// NOT SURE WHAT TODO HERE??? IGNORE FOR NOW...
			}
		}
		*outCharCnt = (outP - outChars);
	#endif
}






/*
 ********************************************************************************
 ********************************** CodePagesInstalled **************************
 ********************************************************************************
 */
vector<CodePage>	CodePagesInstalled::sCodePages;

#if		qWindows
BOOL FAR	PASCAL CodePagesInstalled::EnumCodePagesProc (LPTSTR lpCodePageString)
{
	sCodePages.push_back (_ttoi (lpCodePageString));
	return (1);
}
#endif

void	CodePagesInstalled::Init ()
{
	Led_Assert (sCodePages.size () == 0);
	#if		qWindows
		::EnumSystemCodePages (EnumCodePagesProc, CP_INSTALLED);
	#endif
	// Add these 'fake' code pages - which I believe are always available, but never listed by this procedure
	AddIfNotPresent (kCodePage_UNICODE_WIDE);
	AddIfNotPresent (kCodePage_UNICODE_WIDE_BIGENDIAN);
	AddIfNotPresent (kCodePage_UTF8);			// cuz even if OS (e.g. Win98) doesn't support, we have our own baked in code
	std::sort (sCodePages.begin (), sCodePages.end ());
}

void	CodePagesInstalled::AddIfNotPresent (CodePage cp)
{
	if (std::find (sCodePages.begin (), sCodePages.end (), cp) == sCodePages.end ()) {
		sCodePages.push_back (cp);
	}
}






/*
 ********************************************************************************
 ********************************** CodePagesGuesser ****************************
 ********************************************************************************
 */

/*
@METHOD:		CodePagesGuesser::Guess
@DESCRIPTION:	<p>Guess the code page of the given snippet of text. Return that codepage. Always make some guess,
			and return the level of quality of the guess in the optional parameter 'confidence' - unless its NULL (which it is by default),
			and return the number of bytes of BOM (byte-order-mark) prefix to strip from teh source in 'bytesFromFrontToStrip'
			unless it is NULL (which it is by default).</p>
*/
CodePage	CodePagesGuesser::Guess (const void* input, size_t nBytes, Confidence* confidence, size_t* bytesFromFrontToStrip)
{
	if (confidence != NULL) {
		*confidence = eLow;
	}
	if (bytesFromFrontToStrip != NULL) {
		*bytesFromFrontToStrip = 0;
	}
	if (nBytes >= 2) {
		unsigned char	c0	=	reinterpret_cast<const unsigned char*> (input)[0];
		unsigned char	c1	=	reinterpret_cast<const unsigned char*> (input)[1];
		if (c0 == 0xff and c1 == 0xfe) {
			if (confidence != NULL) {
				*confidence = eHigh;
			}
			if (bytesFromFrontToStrip != NULL) {
				*bytesFromFrontToStrip = 2;
			}
			return kCodePage_UNICODE_WIDE;
		}
		if (c0 == 0xfe and c1 == 0xff) {
			if (confidence != NULL) {
				*confidence = eHigh;
			}
			if (bytesFromFrontToStrip != NULL) {
				*bytesFromFrontToStrip = 2;
			}
			return kCodePage_UNICODE_WIDE_BIGENDIAN;
		}
		if (nBytes >= 3) {
			unsigned char	c2	=	reinterpret_cast<const unsigned char*> (input)[2];
			if (c0 == 0xef and c1 == 0xbb and c2 == 0xbf) {
				if (confidence != NULL) {
					*confidence = eHigh;
				}
				if (bytesFromFrontToStrip != NULL) {
					*bytesFromFrontToStrip = 3;
				}
				return kCodePage_UTF8;
			}
		}
		if (nBytes >= 5) {
			unsigned char	c2	=	reinterpret_cast<const unsigned char*> (input)[2];
			unsigned char	c3	=	reinterpret_cast<const unsigned char*> (input)[3];
			unsigned char	c4	=	reinterpret_cast<const unsigned char*> (input)[4];
			if (c0 == 0x2b and c1 == 0x2f and c2 == 0x76 and c3 == 0x38 and c4 == 0x2d) {
				if (confidence != NULL) {
					*confidence = eHigh;
				}
				if (bytesFromFrontToStrip != NULL) {
					*bytesFromFrontToStrip = 5;
				}
				return kCodePage_UTF7;
			}
		}
	}


	/*
	 * Final ditch efforts if we don't recognize any prefix.
	 */
	if (confidence != NULL) {
		*confidence = eLow;
	}
	#if		qWindows
		return ::GetACP ();
	#elif	qMacOS
		return kCodePage_MAC;
	#else
		return kCodePage_ANSI;	/// NOT SURE WHATS BEST FOR UNIX???
	#endif
}







/*
 ********************************************************************************
 ****************************** CodePagePrettyNameMapper ************************
 ********************************************************************************
 */
CodePagePrettyNameMapper::CodePageNames	CodePagePrettyNameMapper::sCodePageNames	=	CodePagePrettyNameMapper::MakeDefaultCodePageNames ();

CodePagePrettyNameMapper::CodePageNames	CodePagePrettyNameMapper::MakeDefaultCodePageNames ()
{
	CodePageNames	codePageNames;
	codePageNames.fUNICODE_WIDE				=	Led_SDK_TCHAROF ("UNICODE {wide characters}");
	codePageNames.fUNICODE_WIDE_BIGENDIAN	=	Led_SDK_TCHAROF ("UNICODE {wide characters - big endian}");
	codePageNames.fANSI						=	Led_SDK_TCHAROF ("ANSI (1252)");
	codePageNames.fMAC						=	Led_SDK_TCHAROF ("MAC (2)");
	codePageNames.fPC						=	Led_SDK_TCHAROF ("IBM PC United States code page (437)");
	codePageNames.fSJIS						=	Led_SDK_TCHAROF ("Japanese SJIS {932}");
	codePageNames.fUTF7						=	Led_SDK_TCHAROF ("UNICODE {UTF-7}");
	codePageNames.fUTF8						=	Led_SDK_TCHAROF ("UNICODE {UTF-8}");
	codePageNames.f850						=	Led_SDK_TCHAROF ("Latin I - MS-DOS Multilingual (850)");
	codePageNames.f851						=	Led_SDK_TCHAROF ("Latin II - MS-DOS Slavic (850)");
	codePageNames.f866						=	Led_SDK_TCHAROF ("Russian - MS-DOS (866)");
	codePageNames.f936						=	Led_SDK_TCHAROF ("Chinese {Simplfied} (936)");
	codePageNames.f949						=	Led_SDK_TCHAROF ("Korean (949)");
	codePageNames.f950						=	Led_SDK_TCHAROF ("Chinese {Traditional} (950)");
	codePageNames.f1250						=	Led_SDK_TCHAROF ("Eastern European Windows (1250)");
	codePageNames.f1251						=	Led_SDK_TCHAROF ("Cyrilic (1251)");
	codePageNames.f10000					=	Led_SDK_TCHAROF ("Roman {Macintosh} (10000)");
	codePageNames.f10001					=	Led_SDK_TCHAROF ("Japanese {Macintosh} (10001)");
	codePageNames.f50220					=	Led_SDK_TCHAROF ("Japanese JIS (50220)");
	return codePageNames;
}

Led_SDK_String	CodePagePrettyNameMapper::GetName (CodePage cp)
	{
		switch (cp) {
			case	kCodePage_UNICODE_WIDE:				return sCodePageNames.fUNICODE_WIDE;
			case	kCodePage_UNICODE_WIDE_BIGENDIAN:	return sCodePageNames.fUNICODE_WIDE_BIGENDIAN;
			case	kCodePage_ANSI:						return sCodePageNames.fANSI;
			case	kCodePage_MAC:						return sCodePageNames.fMAC;
			case	kCodePage_PC:						return sCodePageNames.fPC;
			case	kCodePage_SJIS:						return sCodePageNames.fSJIS;
			case	kCodePage_UTF7:						return sCodePageNames.fUTF7;
			case	kCodePage_UTF8:						return sCodePageNames.fUTF8;
			case	850:								return sCodePageNames.f850;
			case	851:								return sCodePageNames.f851;
			case	866:								return sCodePageNames.f866;
			case	936:								return sCodePageNames.f936;
			case	949:								return sCodePageNames.f949;
			case	950:								return sCodePageNames.f950;
			case	1250:								return sCodePageNames.f1250;
			case	1251:								return sCodePageNames.f1251;
			case	10000:								return sCodePageNames.f10000;
			case	10001:								return sCodePageNames.f10001;
			case	50220:								return sCodePageNames.f50220;
			default: {
				char	buf[1024];
				(void)::sprintf (buf, "%d", cp);
				return Led_ANSI2SDKString (buf);
			}
		}
	}






/*
 ********************************************************************************
 ******************************* CharacterProperties ****************************
 ********************************************************************************
 */
#if		qMultiByteCharacters || qWideCharacters

bool	CharacterProperties::IsAlpha (Led_tChar c)
{
	#if		qMultiByteCharacters
		return ::isalpha (c);
	#elif	qWideCharacters
		#if		qCharacterPropertiesMemoized
			return IsAlpha_M (c);
		#else
			return ::iswalpha (c);
		#endif
	#endif
}

bool	CharacterProperties::IsAlnum (Led_tChar c)
{
	#if		qMultiByteCharacters
		return ::isalnum (c);
	#elif	qWideCharacters
		#if		qCharacterPropertiesMemoized
			return IsAlnum_M (c);
		#else
			return ::iswalnum (c);
		#endif
	#endif
}

bool	CharacterProperties::IsPunct (Led_tChar c)
{
	#if		qMultiByteCharacters
		return ::ispunct (c);
	#elif	qWideCharacters
		#if		qCharacterPropertiesMemoized
			return IsPunct_M (c);
		#else
			return ::iswpunct (c);
		#endif
	#endif
}

bool	CharacterProperties::IsSpace (Led_tChar c)
{
	#if		qMultiByteCharacters
		return ::isspace (c);
	#elif	qWideCharacters
		#if		qCharacterPropertiesMemoized
			return IsSpace_M (c);
		#else
			return ::iswspace (c);
		#endif
	#endif
}

bool	CharacterProperties::IsCntrl (Led_tChar c)
{
	#if		qMultiByteCharacters
		return ::iscntrl (c);
	#elif	qWideCharacters
		#if		qCharacterPropertiesMemoized
			return IsCntrl_M (c);
		#else
			return ::iswcntrl (c);
		#endif
	#endif
}

bool	CharacterProperties::IsDigit (Led_tChar c)
{
	#if		qMultiByteCharacters
		return ::isdigit (c);
	#elif	qWideCharacters
		#if		qCharacterPropertiesMemoized
			return IsDigit_M (c);
		#else
			return ::iswdigit (c);
		#endif
	#endif
}

#if		qWideCharacters

namespace	{
	struct	MirrorChars {
		wchar_t	fChar;
		wchar_t	fMirrorChar;
	};
	struct MirrorChars_Compare : public binary_function <MirrorChars, wchar_t, bool> {
		bool operator()(const MirrorChars& _Left, const wchar_t& _Right) const
			{
				return (_Left.fChar < _Right);
			}
	};
}

bool	CharacterProperties::IsMirrored (Led_tChar c, Led_tChar* mirrorChar)
{
	// Table from http://www.unicode.org/Public/3.2-Update/BidiMirroring-3.2.0.txt
	const MirrorChars	kMirrorChars[]	=	{
		{	0x0028, 0x0029	},	// LEFT PARENTHESIS
		{	0x0029, 0x0028	},	// RIGHT PARENTHESIS
		{	0x003C, 0x003E	},	// LESS-THAN SIGN
		{	0x003E, 0x003C	},	// GREATER-THAN SIGN
		{	0x005B, 0x005D	},	// LEFT SQUARE BRACKET
		{	0x005D, 0x005B	},	// RIGHT SQUARE BRACKET
		{	0x007B, 0x007D	},	// LEFT CURLY BRACKET
		{	0x007D, 0x007B	},	// RIGHT CURLY BRACKET
		{	0x00AB, 0x00BB	},	// LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
		{	0x00BB, 0x00AB	},	// RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
		{	0x2039, 0x203A	},	// SINGLE LEFT-POINTING ANGLE QUOTATION MARK
		{	0x203A, 0x2039	},	// SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
		{	0x2045, 0x2046	},	// LEFT SQUARE BRACKET WITH QUILL
		{	0x2046, 0x2045	},	// RIGHT SQUARE BRACKET WITH QUILL
		{	0x207D, 0x207E	},	// SUPERSCRIPT LEFT PARENTHESIS
		{	0x207E, 0x207D	},	// SUPERSCRIPT RIGHT PARENTHESIS
		{	0x208D, 0x208E	},	// SUBSCRIPT LEFT PARENTHESIS
		{	0x208E, 0x208D	},	// SUBSCRIPT RIGHT PARENTHESIS
		{	0x2208, 0x220B	},	// ELEMENT OF
		{	0x2209, 0x220C	},	// NOT AN ELEMENT OF
		{	0x220A, 0x220D	},	// SMALL ELEMENT OF
		{	0x220B, 0x2208	},	// CONTAINS AS MEMBER
		{	0x220C, 0x2209	},	// DOES NOT CONTAIN AS MEMBER
		{	0x220D, 0x220A	},	// SMALL CONTAINS AS MEMBER
		{	0x2215, 0x29F5	},	// DIVISION SLASH
		{	0x223C, 0x223D	},	// TILDE OPERATOR
		{	0x223D, 0x223C	},	// REVERSED TILDE
		{	0x2243, 0x22CD	},	// ASYMPTOTICALLY EQUAL TO
		{	0x2252, 0x2253	},	// APPROXIMATELY EQUAL TO OR THE IMAGE OF
		{	0x2253, 0x2252	},	// IMAGE OF OR APPROXIMATELY EQUAL TO
		{	0x2254, 0x2255	},	// COLON EQUALS
		{	0x2255, 0x2254	},	// EQUALS COLON
		{	0x2264, 0x2265	},	// LESS-THAN OR EQUAL TO
		{	0x2265, 0x2264	},	// GREATER-THAN OR EQUAL TO
		{	0x2266, 0x2267	},	// LESS-THAN OVER EQUAL TO
		{	0x2267, 0x2266	},	// GREATER-THAN OVER EQUAL TO
		{	0x2268, 0x2269	},	// [BEST FIT] LESS-THAN BUT NOT EQUAL TO
		{	0x2269, 0x2268	},	// [BEST FIT] GREATER-THAN BUT NOT EQUAL TO
		{	0x226A, 0x226B	},	// MUCH LESS-THAN
		{	0x226B, 0x226A	},	// MUCH GREATER-THAN
		{	0x226E, 0x226F	},	// [BEST FIT] NOT LESS-THAN
		{	0x226F, 0x226E	},	// [BEST FIT] NOT GREATER-THAN
		{	0x2270, 0x2271	},	// [BEST FIT] NEITHER LESS-THAN NOR EQUAL TO
		{	0x2271, 0x2270	},	// [BEST FIT] NEITHER GREATER-THAN NOR EQUAL TO
		{	0x2272, 0x2273	},	// [BEST FIT] LESS-THAN OR EQUIVALENT TO
		{	0x2273, 0x2272	},	// [BEST FIT] GREATER-THAN OR EQUIVALENT TO
		{	0x2274, 0x2275	},	// [BEST FIT] NEITHER LESS-THAN NOR EQUIVALENT TO
		{	0x2275, 0x2274	},	// [BEST FIT] NEITHER GREATER-THAN NOR EQUIVALENT TO
		{	0x2276, 0x2277	},	// LESS-THAN OR GREATER-THAN
		{	0x2277, 0x2276	},	// GREATER-THAN OR LESS-THAN
		{	0x2278, 0x2279	},	// NEITHER LESS-THAN NOR GREATER-THAN
		{	0x2279, 0x2278	},	// NEITHER GREATER-THAN NOR LESS-THAN
		{	0x227A, 0x227B	},	// PRECEDES
		{	0x227B, 0x227A	},	// SUCCEEDS
		{	0x227C, 0x227D	},	// PRECEDES OR EQUAL TO
		{	0x227D, 0x227C	},	// SUCCEEDS OR EQUAL TO
		{	0x227E, 0x227F	},	// [BEST FIT] PRECEDES OR EQUIVALENT TO
		{	0x227F, 0x227E	},	// [BEST FIT] SUCCEEDS OR EQUIVALENT TO
		{	0x2280, 0x2281	},	// [BEST FIT] DOES NOT PRECEDE
		{	0x2281, 0x2280	},	// [BEST FIT] DOES NOT SUCCEED
		{	0x2282, 0x2283	},	// SUBSET OF
		{	0x2283, 0x2282	},	// SUPERSET OF
		{	0x2284, 0x2285	},	// [BEST FIT] NOT A SUBSET OF
		{	0x2285, 0x2284	},	// [BEST FIT] NOT A SUPERSET OF
		{	0x2286, 0x2287	},	// SUBSET OF OR EQUAL TO
		{	0x2287, 0x2286	},	// SUPERSET OF OR EQUAL TO
		{	0x2288, 0x2289	},	// [BEST FIT] NEITHER A SUBSET OF NOR EQUAL TO
		{	0x2289, 0x2288	},	// [BEST FIT] NEITHER A SUPERSET OF NOR EQUAL TO
		{	0x228A, 0x228B	},	// [BEST FIT] SUBSET OF WITH NOT EQUAL TO
		{	0x228B, 0x228A	},	// [BEST FIT] SUPERSET OF WITH NOT EQUAL TO
		{	0x228F, 0x2290	},	// SQUARE IMAGE OF
		{	0x2290, 0x228F	},	// SQUARE ORIGINAL OF
		{	0x2291, 0x2292	},	// SQUARE IMAGE OF OR EQUAL TO
		{	0x2292, 0x2291	},	// SQUARE ORIGINAL OF OR EQUAL TO
		{	0x2298, 0x29B8	},	// CIRCLED DIVISION SLASH
		{	0x22A2, 0x22A3	},	// RIGHT TACK
		{	0x22A3, 0x22A2	},	// LEFT TACK
		{	0x22A6, 0x2ADE	},	// ASSERTION
		{	0x22A8, 0x2AE4	},	// TRUE
		{	0x22A9, 0x2AE3	},	// FORCES
		{	0x22AB, 0x2AE5	},	// DOUBLE VERTICAL BAR DOUBLE RIGHT TURNSTILE
		{	0x22B0, 0x22B1	},	// PRECEDES UNDER RELATION
		{	0x22B1, 0x22B0	},	// SUCCEEDS UNDER RELATION
		{	0x22B2, 0x22B3	},	// NORMAL SUBGROUP OF
		{	0x22B3, 0x22B2	},	// CONTAINS AS NORMAL SUBGROUP
		{	0x22B4, 0x22B5	},	// NORMAL SUBGROUP OF OR EQUAL TO
		{	0x22B5, 0x22B4	},	// CONTAINS AS NORMAL SUBGROUP OR EQUAL TO
		{	0x22B6, 0x22B7	},	// ORIGINAL OF
		{	0x22B7, 0x22B6	},	// IMAGE OF
		{	0x22C9, 0x22CA	},	// LEFT NORMAL FACTOR SEMIDIRECT PRODUCT
		{	0x22CA, 0x22C9	},	// RIGHT NORMAL FACTOR SEMIDIRECT PRODUCT
		{	0x22CB, 0x22CC	},	// LEFT SEMIDIRECT PRODUCT
		{	0x22CC, 0x22CB	},	// RIGHT SEMIDIRECT PRODUCT
		{	0x22CD, 0x2243	},	// REVERSED TILDE EQUALS
		{	0x22D0, 0x22D1	},	// DOUBLE SUBSET
		{	0x22D1, 0x22D0	},	// DOUBLE SUPERSET
		{	0x22D6, 0x22D7	},	// LESS-THAN WITH DOT
		{	0x22D7, 0x22D6	},	// GREATER-THAN WITH DOT
		{	0x22D8, 0x22D9	},	// VERY MUCH LESS-THAN
		{	0x22D9, 0x22D8	},	// VERY MUCH GREATER-THAN
		{	0x22DA, 0x22DB	},	// LESS-THAN EQUAL TO OR GREATER-THAN
		{	0x22DB, 0x22DA	},	// GREATER-THAN EQUAL TO OR LESS-THAN
		{	0x22DC, 0x22DD	},	// EQUAL TO OR LESS-THAN
		{	0x22DD, 0x22DC	},	// EQUAL TO OR GREATER-THAN
		{	0x22DE, 0x22DF	},	// EQUAL TO OR PRECEDES
		{	0x22DF, 0x22DE	},	// EQUAL TO OR SUCCEEDS
		{	0x22E0, 0x22E1	},	// [BEST FIT] DOES NOT PRECEDE OR EQUAL
		{	0x22E1, 0x22E0	},	// [BEST FIT] DOES NOT SUCCEED OR EQUAL
		{	0x22E2, 0x22E3	},	// [BEST FIT] NOT SQUARE IMAGE OF OR EQUAL TO
		{	0x22E3, 0x22E2	},	// [BEST FIT] NOT SQUARE ORIGINAL OF OR EQUAL TO
		{	0x22E4, 0x22E5	},	// [BEST FIT] SQUARE IMAGE OF OR NOT EQUAL TO
		{	0x22E5, 0x22E4	},	// [BEST FIT] SQUARE ORIGINAL OF OR NOT EQUAL TO
		{	0x22E6, 0x22E7	},	// [BEST FIT] LESS-THAN BUT NOT EQUIVALENT TO
		{	0x22E7, 0x22E6	},	// [BEST FIT] GREATER-THAN BUT NOT EQUIVALENT TO
		{	0x22E8, 0x22E9	},	// [BEST FIT] PRECEDES BUT NOT EQUIVALENT TO
		{	0x22E9, 0x22E8	},	// [BEST FIT] SUCCEEDS BUT NOT EQUIVALENT TO
		{	0x22EA, 0x22EB	},	// [BEST FIT] NOT NORMAL SUBGROUP OF
		{	0x22EB, 0x22EA	},	// [BEST FIT] DOES NOT CONTAIN AS NORMAL SUBGROUP
		{	0x22EC, 0x22ED	},	// [BEST FIT] NOT NORMAL SUBGROUP OF OR EQUAL TO
		{	0x22ED, 0x22EC	},	// [BEST FIT] DOES NOT CONTAIN AS NORMAL SUBGROUP OR EQUAL
		{	0x22F0, 0x22F1	},	// UP RIGHT DIAGONAL ELLIPSIS
		{	0x22F1, 0x22F0	},	// DOWN RIGHT DIAGONAL ELLIPSIS
		{	0x22F2, 0x22FA	},	// ELEMENT OF WITH LONG HORIZONTAL STROKE
		{	0x22F3, 0x22FB	},	// ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
		{	0x22F4, 0x22FC	},	// SMALL ELEMENT OF WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
		{	0x22F6, 0x22FD	},	// ELEMENT OF WITH OVERBAR
		{	0x22F7, 0x22FE	},	// SMALL ELEMENT OF WITH OVERBAR
		{	0x22FA, 0x22F2	},	// CONTAINS WITH LONG HORIZONTAL STROKE
		{	0x22FB, 0x22F3	},	// CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
		{	0x22FC, 0x22F4	},	// SMALL CONTAINS WITH VERTICAL BAR AT END OF HORIZONTAL STROKE
		{	0x22FD, 0x22F6	},	// CONTAINS WITH OVERBAR
		{	0x22FE, 0x22F7	},	// SMALL CONTAINS WITH OVERBAR
		{	0x2308, 0x2309	},	// LEFT CEILING
		{	0x2309, 0x2308	},	// RIGHT CEILING
		{	0x230A, 0x230B	},	// LEFT FLOOR
		{	0x230B, 0x230A	},	// RIGHT FLOOR
		{	0x2329, 0x232A	},	// LEFT-POINTING ANGLE BRACKET
		{	0x232A, 0x2329	},	// RIGHT-POINTING ANGLE BRACKET
		{	0x2768, 0x2769	},	// MEDIUM LEFT PARENTHESIS ORNAMENT
		{	0x2769, 0x2768	},	// MEDIUM RIGHT PARENTHESIS ORNAMENT
		{	0x276A, 0x276B	},	// MEDIUM FLATTENED LEFT PARENTHESIS ORNAMENT
		{	0x276B, 0x276A	},	// MEDIUM FLATTENED RIGHT PARENTHESIS ORNAMENT
		{	0x276C, 0x276D	},	// MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT
		{	0x276D, 0x276C	},	// MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT
		{	0x276E, 0x276F	},	// HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT
		{	0x276F, 0x276E	},	// HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT
		{	0x2770, 0x2771	},	// HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT
		{	0x2771, 0x2770	},	// HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMENT
		{	0x2772, 0x2773	},	// LIGHT LEFT TORTOISE SHELL BRACKET
		{	0x2773, 0x2772	},	// LIGHT RIGHT TORTOISE SHELL BRACKET
		{	0x2774, 0x2775	},	// MEDIUM LEFT CURLY BRACKET ORNAMENT
		{	0x2775, 0x2774	},	// MEDIUM RIGHT CURLY BRACKET ORNAMENT
		{	0x27D5, 0x27D6	},	// LEFT OUTER JOIN
		{	0x27D6, 0x27D5	},	// RIGHT OUTER JOIN
		{	0x27DD, 0x27DE	},	// LONG RIGHT TACK
		{	0x27DE, 0x27DD	},	// LONG LEFT TACK
		{	0x27E2, 0x27E3	},	// WHITE CONCAVE-SIDED DIAMOND WITH LEFTWARDS TICK
		{	0x27E3, 0x27E2	},	// WHITE CONCAVE-SIDED DIAMOND WITH RIGHTWARDS TICK
		{	0x27E4, 0x27E5	},	// WHITE SQUARE WITH LEFTWARDS TICK
		{	0x27E5, 0x27E4	},	// WHITE SQUARE WITH RIGHTWARDS TICK
		{	0x27E6, 0x27E7	},	// MATHEMATICAL LEFT WHITE SQUARE BRACKET
		{	0x27E7, 0x27E6	},	// MATHEMATICAL RIGHT WHITE SQUARE BRACKET
		{	0x27E8, 0x27E9	},	// MATHEMATICAL LEFT ANGLE BRACKET
		{	0x27E9, 0x27E8	},	// MATHEMATICAL RIGHT ANGLE BRACKET
		{	0x27EA, 0x27EB	},	// MATHEMATICAL LEFT DOUBLE ANGLE BRACKET
		{	0x27EB, 0x27EA	},	// MATHEMATICAL RIGHT DOUBLE ANGLE BRACKET
		{	0x2983, 0x2984	},	// LEFT WHITE CURLY BRACKET
		{	0x2984, 0x2983	},	// RIGHT WHITE CURLY BRACKET
		{	0x2985, 0x2986	},	// LEFT WHITE PARENTHESIS
		{	0x2986, 0x2985	},	// RIGHT WHITE PARENTHESIS
		{	0x2987, 0x2988	},	// Z NOTATION LEFT IMAGE BRACKET
		{	0x2988, 0x2987	},	// Z NOTATION RIGHT IMAGE BRACKET
		{	0x2989, 0x298A	},	// Z NOTATION LEFT BINDING BRACKET
		{	0x298A, 0x2989	},	// Z NOTATION RIGHT BINDING BRACKET
		{	0x298B, 0x298C	},	// LEFT SQUARE BRACKET WITH UNDERBAR
		{	0x298C, 0x298B	},	// RIGHT SQUARE BRACKET WITH UNDERBAR
		{	0x298D, 0x2990	},	// LEFT SQUARE BRACKET WITH TICK IN TOP CORNER
		{	0x298E, 0x298F	},	// RIGHT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
		{	0x298F, 0x298E	},	// LEFT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
		{	0x2990, 0x298D	},	// RIGHT SQUARE BRACKET WITH TICK IN TOP CORNER
		{	0x2991, 0x2992	},	// LEFT ANGLE BRACKET WITH DOT
		{	0x2992, 0x2991	},	// RIGHT ANGLE BRACKET WITH DOT
		{	0x2993, 0x2994	},	// LEFT ARC LESS-THAN BRACKET
		{	0x2994, 0x2993	},	// RIGHT ARC GREATER-THAN BRACKET
		{	0x2995, 0x2996	},	// DOUBLE LEFT ARC GREATER-THAN BRACKET
		{	0x2996, 0x2995	},	// DOUBLE RIGHT ARC LESS-THAN BRACKET
		{	0x2997, 0x2998	},	// LEFT BLACK TORTOISE SHELL BRACKET
		{	0x2998, 0x2997	},	// RIGHT BLACK TORTOISE SHELL BRACKET
		{	0x29B8, 0x2298	},	// CIRCLED REVERSE SOLIDUS
		{	0x29C0, 0x29C1	},	// CIRCLED LESS-THAN
		{	0x29C1, 0x29C0	},	// CIRCLED GREATER-THAN
		{	0x29C4, 0x29C5	},	// SQUARED RISING DIAGONAL SLASH
		{	0x29C5, 0x29C4	},	// SQUARED FALLING DIAGONAL SLASH
		{	0x29CF, 0x29D0	},	// LEFT TRIANGLE BESIDE VERTICAL BAR
		{	0x29D0, 0x29CF	},	// VERTICAL BAR BESIDE RIGHT TRIANGLE
		{	0x29D1, 0x29D2	},	// BOWTIE WITH LEFT HALF BLACK
		{	0x29D2, 0x29D1	},	// BOWTIE WITH RIGHT HALF BLACK
		{	0x29D4, 0x29D5	},	// TIMES WITH LEFT HALF BLACK
		{	0x29D5, 0x29D4	},	// TIMES WITH RIGHT HALF BLACK
		{	0x29D8, 0x29D9	},	// LEFT WIGGLY FENCE
		{	0x29D9, 0x29D8	},	// RIGHT WIGGLY FENCE
		{	0x29DA, 0x29DB	},	// LEFT DOUBLE WIGGLY FENCE
		{	0x29DB, 0x29DA	},	// RIGHT DOUBLE WIGGLY FENCE
		{	0x29F5, 0x2215	},	// REVERSE SOLIDUS OPERATOR
		{	0x29F8, 0x29F9	},	// BIG SOLIDUS
		{	0x29F9, 0x29F8	},	// BIG REVERSE SOLIDUS
		{	0x29FC, 0x29FD	},	// LEFT-POINTING CURVED ANGLE BRACKET
		{	0x29FD, 0x29FC	},	// RIGHT-POINTING CURVED ANGLE BRACKET
		{	0x2A2B, 0x2A2C	},	// MINUS SIGN WITH FALLING DOTS
		{	0x2A2C, 0x2A2B	},	// MINUS SIGN WITH RISING DOTS
		{	0x2A2D, 0x2A2C	},	// PLUS SIGN IN LEFT HALF CIRCLE
		{	0x2A2E, 0x2A2D	},	// PLUS SIGN IN RIGHT HALF CIRCLE
		{	0x2A34, 0x2A35	},	// MULTIPLICATION SIGN IN LEFT HALF CIRCLE
		{	0x2A35, 0x2A34	},	// MULTIPLICATION SIGN IN RIGHT HALF CIRCLE
		{	0x2A3C, 0x2A3D	},	// INTERIOR PRODUCT
		{	0x2A3D, 0x2A3C	},	// RIGHTHAND INTERIOR PRODUCT
		{	0x2A64, 0x2A65	},	// Z NOTATION DOMAIN ANTIRESTRICTION
		{	0x2A65, 0x2A64	},	// Z NOTATION RANGE ANTIRESTRICTION
		{	0x2A79, 0x2A7A	},	// LESS-THAN WITH CIRCLE INSIDE
		{	0x2A7A, 0x2A79	},	// GREATER-THAN WITH CIRCLE INSIDE
		{	0x2A7D, 0x2A7E	},	// LESS-THAN OR SLANTED EQUAL TO
		{	0x2A7E, 0x2A7D	},	// GREATER-THAN OR SLANTED EQUAL TO
		{	0x2A7F, 0x2A80	},	// LESS-THAN OR SLANTED EQUAL TO WITH DOT INSIDE
		{	0x2A80, 0x2A7F	},	// GREATER-THAN OR SLANTED EQUAL TO WITH DOT INSIDE
		{	0x2A81, 0x2A82	},	// LESS-THAN OR SLANTED EQUAL TO WITH DOT ABOVE
		{	0x2A82, 0x2A81	},	// GREATER-THAN OR SLANTED EQUAL TO WITH DOT ABOVE
		{	0x2A83, 0x2A84	},	// LESS-THAN OR SLANTED EQUAL TO WITH DOT ABOVE RIGHT
		{	0x2A84, 0x2A83	},	// GREATER-THAN OR SLANTED EQUAL TO WITH DOT ABOVE LEFT
		{	0x2A8B, 0x2A8C	},	// LESS-THAN ABOVE DOUBLE-LINE EQUAL ABOVE GREATER-THAN
		{	0x2A8C, 0x2A8B	},	// GREATER-THAN ABOVE DOUBLE-LINE EQUAL ABOVE LESS-THAN
		{	0x2A91, 0x2A92	},	// LESS-THAN ABOVE GREATER-THAN ABOVE DOUBLE-LINE EQUAL
		{	0x2A92, 0x2A91	},	// GREATER-THAN ABOVE LESS-THAN ABOVE DOUBLE-LINE EQUAL
		{	0x2A93, 0x2A94	},	// LESS-THAN ABOVE SLANTED EQUAL ABOVE GREATER-THAN ABOVE SLANTED EQUAL
		{	0x2A94, 0x2A93	},	// GREATER-THAN ABOVE SLANTED EQUAL ABOVE LESS-THAN ABOVE SLANTED EQUAL
		{	0x2A95, 0x2A96	},	// SLANTED EQUAL TO OR LESS-THAN
		{	0x2A96, 0x2A95	},	// SLANTED EQUAL TO OR GREATER-THAN
		{	0x2A97, 0x2A98	},	// SLANTED EQUAL TO OR LESS-THAN WITH DOT INSIDE
		{	0x2A98, 0x2A97	},	// SLANTED EQUAL TO OR GREATER-THAN WITH DOT INSIDE
		{	0x2A99, 0x2A9A	},	// DOUBLE-LINE EQUAL TO OR LESS-THAN
		{	0x2A9A, 0x2A99	},	// DOUBLE-LINE EQUAL TO OR GREATER-THAN
		{	0x2A9B, 0x2A9C	},	// DOUBLE-LINE SLANTED EQUAL TO OR LESS-THAN
		{	0x2A9C, 0x2A9B	},	// DOUBLE-LINE SLANTED EQUAL TO OR GREATER-THAN
		{	0x2AA1, 0x2AA2	},	// DOUBLE NESTED LESS-THAN
		{	0x2AA2, 0x2AA1	},	// DOUBLE NESTED GREATER-THAN
		{	0x2AA6, 0x2AA7	},	// LESS-THAN CLOSED BY CURVE
		{	0x2AA7, 0x2AA6	},	// GREATER-THAN CLOSED BY CURVE
		{	0x2AA8, 0x2AA9	},	// LESS-THAN CLOSED BY CURVE ABOVE SLANTED EQUAL
		{	0x2AA9, 0x2AA8	},	// GREATER-THAN CLOSED BY CURVE ABOVE SLANTED EQUAL
		{	0x2AAA, 0x2AAB	},	// SMALLER THAN
		{	0x2AAB, 0x2AAA	},	// LARGER THAN
		{	0x2AAC, 0x2AAD	},	// SMALLER THAN OR EQUAL TO
		{	0x2AAD, 0x2AAC	},	// LARGER THAN OR EQUAL TO
		{	0x2AAF, 0x2AB0	},	// PRECEDES ABOVE SINGLE-LINE EQUALS SIGN
		{	0x2AB0, 0x2AAF	},	// SUCCEEDS ABOVE SINGLE-LINE EQUALS SIGN
		{	0x2AB3, 0x2AB4	},	// PRECEDES ABOVE EQUALS SIGN
		{	0x2AB4, 0x2AB3	},	// SUCCEEDS ABOVE EQUALS SIGN
		{	0x2ABB, 0x2ABC	},	// DOUBLE PRECEDES
		{	0x2ABC, 0x2ABB	},	// DOUBLE SUCCEEDS
		{	0x2ABD, 0x2ABE	},	// SUBSET WITH DOT
		{	0x2ABE, 0x2ABD	},	// SUPERSET WITH DOT
		{	0x2ABF, 0x2AC0	},	// SUBSET WITH PLUS SIGN BELOW
		{	0x2AC0, 0x2ABF	},	// SUPERSET WITH PLUS SIGN BELOW
		{	0x2AC1, 0x2AC2	},	// SUBSET WITH MULTIPLICATION SIGN BELOW
		{	0x2AC2, 0x2AC1	},	// SUPERSET WITH MULTIPLICATION SIGN BELOW
		{	0x2AC3, 0x2AC4	},	// SUBSET OF OR EQUAL TO WITH DOT ABOVE
		{	0x2AC4, 0x2AC3	},	// SUPERSET OF OR EQUAL TO WITH DOT ABOVE
		{	0x2AC5, 0x2AC6	},	// SUBSET OF ABOVE EQUALS SIGN
		{	0x2AC6, 0x2AC5	},	// SUPERSET OF ABOVE EQUALS SIGN
		{	0x2ACD, 0x2ACE	},	// SQUARE LEFT OPEN BOX OPERATOR
		{	0x2ACE, 0x2ACD	},	// SQUARE RIGHT OPEN BOX OPERATOR
		{	0x2ACF, 0x2AD0	},	// CLOSED SUBSET
		{	0x2AD0, 0x2ACF	},	// CLOSED SUPERSET
		{	0x2AD1, 0x2AD2	},	// CLOSED SUBSET OR EQUAL TO
		{	0x2AD2, 0x2AD1	},	// CLOSED SUPERSET OR EQUAL TO
		{	0x2AD3, 0x2AD4	},	// SUBSET ABOVE SUPERSET
		{	0x2AD4, 0x2AD3	},	// SUPERSET ABOVE SUBSET
		{	0x2AD5, 0x2AD6	},	// SUBSET ABOVE SUBSET
		{	0x2AD6, 0x2AD5	},	// SUPERSET ABOVE SUPERSET
		{	0x2ADE, 0x22A6	},	// SHORT LEFT TACK
		{	0x2AE3, 0x22A9	},	// DOUBLE VERTICAL BAR LEFT TURNSTILE
		{	0x2AE4, 0x22A8	},	// VERTICAL BAR DOUBLE LEFT TURNSTILE
		{	0x2AE5, 0x22AB	},	// DOUBLE VERTICAL BAR DOUBLE LEFT TURNSTILE
		{	0x2AEC, 0x2AED	},	// DOUBLE STROKE NOT SIGN
		{	0x2AED, 0x2AEC	},	// REVERSED DOUBLE STROKE NOT SIGN
		{	0x2AF7, 0x2AF8	},	// TRIPLE NESTED LESS-THAN
		{	0x2AF8, 0x2AF7	},	// TRIPLE NESTED GREATER-THAN
		{	0x2AF9, 0x2AFA	},	// DOUBLE-LINE SLANTED LESS-THAN OR EQUAL TO
		{	0x2AFA, 0x2AF9	},	// DOUBLE-LINE SLANTED GREATER-THAN OR EQUAL TO
		{	0x3008, 0x3009	},	// LEFT ANGLE BRACKET
		{	0x3009, 0x3008	},	// RIGHT ANGLE BRACKET
		{	0x300A, 0x300B	},	// LEFT DOUBLE ANGLE BRACKET
		{	0x300B, 0x300A	},	// RIGHT DOUBLE ANGLE BRACKET
		{	0x300C, 0x300D	},	// [BEST FIT] LEFT CORNER BRACKET
		{	0x300D, 0x300C	},	// [BEST FIT] RIGHT CORNER BRACKET
		{	0x300E, 0x300F	},	// [BEST FIT] LEFT WHITE CORNER BRACKET
		{	0x300F, 0x300E	},	// [BEST FIT] RIGHT WHITE CORNER BRACKET
		{	0x3010, 0x3011	},	// LEFT BLACK LENTICULAR BRACKET
		{	0x3011, 0x3010	},	// RIGHT BLACK LENTICULAR BRACKET
		{	0x3014, 0x3015	},	// LEFT TORTOISE SHELL BRACKET
		{	0x3015, 0x3014	},	// RIGHT TORTOISE SHELL BRACKET
		{	0x3016, 0x3017	},	// LEFT WHITE LENTICULAR BRACKET
		{	0x3017, 0x3016	},	// RIGHT WHITE LENTICULAR BRACKET
		{	0x3018, 0x3019	},	// LEFT WHITE TORTOISE SHELL BRACKET
		{	0x3019, 0x3018	},	// RIGHT WHITE TORTOISE SHELL BRACKET
		{	0x301A, 0x301B	},	// LEFT WHITE SQUARE BRACKET
		{	0x301B, 0x301A	},	// RIGHT WHITE SQUARE BRACKET
		{	0xFF08, 0xFF09	},	// FULLWIDTH LEFT PARENTHESIS
		{	0xFF09, 0xFF08	},	// FULLWIDTH RIGHT PARENTHESIS
		{	0xFF1C, 0xFF1E	},	// FULLWIDTH LESS-THAN SIGN
		{	0xFF1E, 0xFF1C	},	// FULLWIDTH GREATER-THAN SIGN
		{	0xFF3B, 0xFF3D	},	// FULLWIDTH LEFT SQUARE BRACKET
		{	0xFF3D, 0xFF3B	},	// FULLWIDTH RIGHT SQUARE BRACKET
		{	0xFF5B, 0xFF5D	},	// FULLWIDTH LEFT CURLY BRACKET
		{	0xFF5D, 0xFF5B	},	// FULLWIDTH RIGHT CURLY BRACKET
		{	0xFF5F, 0xFF60	},	// FULLWIDTH LEFT WHITE PARENTHESIS
		{	0xFF60, 0xFF5F	},	// FULLWIDTH RIGHT WHITE PARENTHESIS
		{	0xFF62, 0xFF63	},	// [BEST FIT] HALFWIDTH LEFT CORNER BRACKET
		{	0xFF63, 0xFF62	},	// [BEST FIT] HALFWIDTH RIGHT CORNER BRACKET
	};
#if		qBCCOverloadWIthCallToLowerBoundTemplateBug
	const MirrorChars* b = kMirrorChars;
	const MirrorChars* e = b + Led_NEltsOf (kMirrorChars);
	const MirrorChars*	mi	=	lower_bound (b, e, c, MirrorChars_Compare ());
#else
	const MirrorChars*	mi	=	lower_bound (kMirrorChars, kMirrorChars + Led_NEltsOf (kMirrorChars), c, MirrorChars_Compare ());
#endif
 	if (mi != NULL and mi->fChar == c and mirrorChar != NULL) {
		*mirrorChar = mi->fMirrorChar;
	}
	return mi != NULL and mi->fChar == c;
}
#endif

#if		qWideCharacters
bool	CharacterProperties::IsAlpha_M (wchar_t c)
{
	// ********** CharacterProperties::isalpha_M (AUTOGENERATED memoize of iswalpha - Feb 24 2003) ***********
	// Hack for SPR#1220 and SPR#1306
	if (c < 255) {
		if ((65 <= c && c <= 90)
			||(97 <= c && c <= 122)
			||(192 <= c && c <= 214)
			||(216 <= c && c <= 246)
			||(248 <= c && c <= 254)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswalpha(c));
			#endif
			return true;
		}
	}
	else if (c < 5000) {
		if ((255 <= c && c <= 543)
			||(546 <= c && c <= 563)
			||(592 <= c && c <= 685)
			||(688 <= c && c <= 696)
			||(699 <= c && c <= 705)
			||(736 <= c && c <= 740)
			||(750 <= c && c <= 750)
			||(837 <= c && c <= 837)
			||(890 <= c && c <= 890)
			||(902 <= c && c <= 902)
			||(904 <= c && c <= 906)
			||(908 <= c && c <= 908)
			||(910 <= c && c <= 929)
			||(931 <= c && c <= 974)
			||(976 <= c && c <= 983)
			||(986 <= c && c <= 1011)
			||(1024 <= c && c <= 1153)
			||(1164 <= c && c <= 1220)
			||(1223 <= c && c <= 1224)
			||(1227 <= c && c <= 1228)
			||(1232 <= c && c <= 1269)
			||(1272 <= c && c <= 1273)
			||(1329 <= c && c <= 1366)
			||(1369 <= c && c <= 1369)
			||(1377 <= c && c <= 1415)
			||(1488 <= c && c <= 1514)
			||(1520 <= c && c <= 1522)
			||(1569 <= c && c <= 1594)
			||(1601 <= c && c <= 1621)
			||(1648 <= c && c <= 1747)
			||(1749 <= c && c <= 1756)
			||(1761 <= c && c <= 1768)
			||(1773 <= c && c <= 1773)
			||(1786 <= c && c <= 1788)
			||(1808 <= c && c <= 1836)
			||(1840 <= c && c <= 1855)
			||(1920 <= c && c <= 1968)
			||(2305 <= c && c <= 2307)
			||(2309 <= c && c <= 2361)
			||(2365 <= c && c <= 2380)
			||(2384 <= c && c <= 2384)
			||(2392 <= c && c <= 2403)
			||(2433 <= c && c <= 2435)
			||(2437 <= c && c <= 2444)
			||(2447 <= c && c <= 2448)
			||(2451 <= c && c <= 2472)
			||(2474 <= c && c <= 2480)
			||(2482 <= c && c <= 2482)
			||(2486 <= c && c <= 2489)
			||(2494 <= c && c <= 2500)
			||(2503 <= c && c <= 2504)
			||(2507 <= c && c <= 2508)
			||(2519 <= c && c <= 2519)
			||(2524 <= c && c <= 2525)
			||(2527 <= c && c <= 2531)
			||(2544 <= c && c <= 2545)
			||(2562 <= c && c <= 2562)
			||(2565 <= c && c <= 2570)
			||(2575 <= c && c <= 2576)
			||(2579 <= c && c <= 2600)
			||(2602 <= c && c <= 2608)
			||(2610 <= c && c <= 2611)
			||(2613 <= c && c <= 2614)
			||(2616 <= c && c <= 2617)
			||(2622 <= c && c <= 2626)
			||(2631 <= c && c <= 2632)
			||(2635 <= c && c <= 2636)
			||(2649 <= c && c <= 2652)
			||(2654 <= c && c <= 2654)
			||(2672 <= c && c <= 2676)
			||(2689 <= c && c <= 2691)
			||(2693 <= c && c <= 2699)
			||(2701 <= c && c <= 2701)
			||(2703 <= c && c <= 2705)
			||(2707 <= c && c <= 2728)
			||(2730 <= c && c <= 2736)
			||(2738 <= c && c <= 2739)
			||(2741 <= c && c <= 2745)
			||(2749 <= c && c <= 2757)
			||(2759 <= c && c <= 2761)
			||(2763 <= c && c <= 2764)
			||(2768 <= c && c <= 2768)
			||(2784 <= c && c <= 2784)
			||(2817 <= c && c <= 2819)
			||(2821 <= c && c <= 2828)
			||(2831 <= c && c <= 2832)
			||(2835 <= c && c <= 2856)
			||(2858 <= c && c <= 2864)
			||(2866 <= c && c <= 2867)
			||(2870 <= c && c <= 2873)
			||(2877 <= c && c <= 2883)
			||(2887 <= c && c <= 2888)
			||(2891 <= c && c <= 2892)
			||(2902 <= c && c <= 2903)
			||(2908 <= c && c <= 2909)
			||(2911 <= c && c <= 2913)
			||(2946 <= c && c <= 2947)
			||(2949 <= c && c <= 2954)
			||(2958 <= c && c <= 2960)
			||(2962 <= c && c <= 2965)
			||(2969 <= c && c <= 2970)
			||(2972 <= c && c <= 2972)
			||(2974 <= c && c <= 2975)
			||(2979 <= c && c <= 2980)
			||(2984 <= c && c <= 2986)
			||(2990 <= c && c <= 2997)
			||(2999 <= c && c <= 3001)
			||(3006 <= c && c <= 3010)
			||(3014 <= c && c <= 3016)
			||(3018 <= c && c <= 3020)
			||(3031 <= c && c <= 3031)
			||(3073 <= c && c <= 3075)
			||(3077 <= c && c <= 3084)
			||(3086 <= c && c <= 3088)
			||(3090 <= c && c <= 3112)
			||(3114 <= c && c <= 3123)
			||(3125 <= c && c <= 3129)
			||(3134 <= c && c <= 3140)
			||(3142 <= c && c <= 3144)
			||(3146 <= c && c <= 3148)
			||(3157 <= c && c <= 3158)
			||(3168 <= c && c <= 3169)
			||(3202 <= c && c <= 3203)
			||(3205 <= c && c <= 3212)
			||(3214 <= c && c <= 3216)
			||(3218 <= c && c <= 3240)
			||(3242 <= c && c <= 3251)
			||(3253 <= c && c <= 3257)
			||(3262 <= c && c <= 3268)
			||(3270 <= c && c <= 3272)
			||(3274 <= c && c <= 3276)
			||(3285 <= c && c <= 3286)
			||(3294 <= c && c <= 3294)
			||(3296 <= c && c <= 3297)
			||(3330 <= c && c <= 3331)
			||(3333 <= c && c <= 3340)
			||(3342 <= c && c <= 3344)
			||(3346 <= c && c <= 3368)
			||(3370 <= c && c <= 3385)
			||(3390 <= c && c <= 3395)
			||(3398 <= c && c <= 3400)
			||(3402 <= c && c <= 3404)
			||(3415 <= c && c <= 3415)
			||(3424 <= c && c <= 3425)
			||(3458 <= c && c <= 3459)
			||(3461 <= c && c <= 3478)
			||(3482 <= c && c <= 3505)
			||(3507 <= c && c <= 3515)
			||(3517 <= c && c <= 3517)
			||(3520 <= c && c <= 3526)
			||(3535 <= c && c <= 3540)
			||(3542 <= c && c <= 3542)
			||(3544 <= c && c <= 3551)
			||(3570 <= c && c <= 3571)
			||(3585 <= c && c <= 3642)
			||(3648 <= c && c <= 3662)
			||(3713 <= c && c <= 3714)
			||(3716 <= c && c <= 3716)
			||(3719 <= c && c <= 3720)
			||(3722 <= c && c <= 3722)
			||(3725 <= c && c <= 3725)
			||(3732 <= c && c <= 3735)
			||(3737 <= c && c <= 3743)
			||(3745 <= c && c <= 3747)
			||(3749 <= c && c <= 3749)
			||(3751 <= c && c <= 3751)
			||(3754 <= c && c <= 3755)
			||(3757 <= c && c <= 3769)
			||(3771 <= c && c <= 3773)
			||(3776 <= c && c <= 3780)
			||(3789 <= c && c <= 3789)
			||(3804 <= c && c <= 3805)
			||(3840 <= c && c <= 3840)
			||(3904 <= c && c <= 3911)
			||(3913 <= c && c <= 3946)
			||(3953 <= c && c <= 3969)
			||(3976 <= c && c <= 3979)
			||(3984 <= c && c <= 3991)
			||(3993 <= c && c <= 4028)
			||(4096 <= c && c <= 4129)
			||(4131 <= c && c <= 4135)
			||(4137 <= c && c <= 4138)
			||(4140 <= c && c <= 4146)
			||(4150 <= c && c <= 4150)
			||(4152 <= c && c <= 4152)
			||(4176 <= c && c <= 4185)
			||(4256 <= c && c <= 4293)
			||(4304 <= c && c <= 4342)
			||(4352 <= c && c <= 4441)
			||(4447 <= c && c <= 4514)
			||(4520 <= c && c <= 4601)
			||(4608 <= c && c <= 4614)
			||(4616 <= c && c <= 4678)
			||(4680 <= c && c <= 4680)
			||(4682 <= c && c <= 4685)
			||(4688 <= c && c <= 4694)
			||(4696 <= c && c <= 4696)
			||(4698 <= c && c <= 4701)
			||(4704 <= c && c <= 4742)
			||(4744 <= c && c <= 4744)
			||(4746 <= c && c <= 4749)
			||(4752 <= c && c <= 4782)
			||(4784 <= c && c <= 4784)
			||(4786 <= c && c <= 4789)
			||(4792 <= c && c <= 4798)
			||(4800 <= c && c <= 4800)
			||(4802 <= c && c <= 4805)
			||(4808 <= c && c <= 4814)
			||(4816 <= c && c <= 4822)
			||(4824 <= c && c <= 4846)
			||(4848 <= c && c <= 4878)
			||(4880 <= c && c <= 4880)
			||(4882 <= c && c <= 4885)
			||(4888 <= c && c <= 4894)
			||(4896 <= c && c <= 4934)
			||(4936 <= c && c <= 4954)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswalpha(c));
			#endif
			return true;
		}
	}
	else {
		if ((5024 <= c && c <= 5108)
			||(5121 <= c && c <= 5740)
			||(5743 <= c && c <= 5750)
			||(5761 <= c && c <= 5786)
			||(5792 <= c && c <= 5866)
			||(6016 <= c && c <= 6088)
			||(6176 <= c && c <= 6210)
			||(6212 <= c && c <= 6263)
			||(6272 <= c && c <= 6313)
			||(7680 <= c && c <= 7835)
			||(7840 <= c && c <= 7929)
			||(7936 <= c && c <= 7957)
			||(7960 <= c && c <= 7965)
			||(7968 <= c && c <= 8005)
			||(8008 <= c && c <= 8013)
			||(8016 <= c && c <= 8023)
			||(8025 <= c && c <= 8025)
			||(8027 <= c && c <= 8027)
			||(8029 <= c && c <= 8029)
			||(8031 <= c && c <= 8061)
			||(8064 <= c && c <= 8116)
			||(8118 <= c && c <= 8124)
			||(8126 <= c && c <= 8126)
			||(8130 <= c && c <= 8132)
			||(8134 <= c && c <= 8140)
			||(8144 <= c && c <= 8147)
			||(8150 <= c && c <= 8155)
			||(8160 <= c && c <= 8172)
			||(8178 <= c && c <= 8180)
			||(8182 <= c && c <= 8188)
			||(8319 <= c && c <= 8319)
			||(8450 <= c && c <= 8450)
			||(8455 <= c && c <= 8455)
			||(8458 <= c && c <= 8467)
			||(8469 <= c && c <= 8469)
			||(8473 <= c && c <= 8477)
			||(8484 <= c && c <= 8484)
			||(8486 <= c && c <= 8486)
			||(8488 <= c && c <= 8488)
			||(8490 <= c && c <= 8493)
			||(8495 <= c && c <= 8497)
			||(8499 <= c && c <= 8505)
			||(8544 <= c && c <= 8579)
			||(12294 <= c && c <= 12295)
			||(12321 <= c && c <= 12329)
			||(12344 <= c && c <= 12346)
			||(12353 <= c && c <= 12436)
			||(12449 <= c && c <= 12538)
			||(12549 <= c && c <= 12588)
			||(12593 <= c && c <= 12686)
			||(12704 <= c && c <= 12727)
			||(13312 <= c && c <= 19893)
			||(19968 <= c && c <= 40869)
			||(40960 <= c && c <= 42124)
			||(44032 <= c && c <= 55203)
			||(63744 <= c && c <= 64045)
			||(64256 <= c && c <= 64262)
			||(64275 <= c && c <= 64279)
			||(64285 <= c && c <= 64285)
			||(64287 <= c && c <= 64296)
			||(64298 <= c && c <= 64310)
			||(64312 <= c && c <= 64316)
			||(64318 <= c && c <= 64318)
			||(64320 <= c && c <= 64321)
			||(64323 <= c && c <= 64324)
			||(64326 <= c && c <= 64433)
			||(64467 <= c && c <= 64829)
			||(64848 <= c && c <= 64911)
			||(64914 <= c && c <= 64967)
			||(65008 <= c && c <= 65019)
			||(65136 <= c && c <= 65138)
			||(65140 <= c && c <= 65140)
			||(65142 <= c && c <= 65276)
			||(65313 <= c && c <= 65338)
			||(65345 <= c && c <= 65370)
			||(65382 <= c && c <= 65391)
			||(65393 <= c && c <= 65437)
			||(65440 <= c && c <= 65470)
			||(65474 <= c && c <= 65479)
			||(65482 <= c && c <= 65487)
			||(65490 <= c && c <= 65495)
			||(65498 <= c && c <= 65500)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswalpha(c));
			#endif
			return true;
		}
	}
	#if		qTestMyISWXXXFunctions
	Led_Assert (!iswalpha(c));
	#endif
	return false;
}

bool	CharacterProperties::IsAlnum_M (wchar_t c)
{
	// ********** CharacterProperties::isalnum_M (AUTOGENERATED memoize of iswalnum - Feb 24 2003) ***********
	// Hack for SPR#1220 and SPR#1306
	if (c < 255) {
		if ((48 <= c && c <= 57)
			||(65 <= c && c <= 90)
			||(97 <= c && c <= 122)
			||(178 <= c && c <= 179)
			||(185 <= c && c <= 185)
			||(192 <= c && c <= 214)
			||(216 <= c && c <= 246)
			||(248 <= c && c <= 254)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswalnum(c));
			#endif
			return true;
		}
	}
	else if (c < 5000) {
		if ((255 <= c && c <= 543)
			||(546 <= c && c <= 563)
			||(592 <= c && c <= 685)
			||(688 <= c && c <= 696)
			||(699 <= c && c <= 705)
			||(736 <= c && c <= 740)
			||(750 <= c && c <= 750)
			||(837 <= c && c <= 837)
			||(890 <= c && c <= 890)
			||(902 <= c && c <= 902)
			||(904 <= c && c <= 906)
			||(908 <= c && c <= 908)
			||(910 <= c && c <= 929)
			||(931 <= c && c <= 974)
			||(976 <= c && c <= 983)
			||(986 <= c && c <= 1011)
			||(1024 <= c && c <= 1153)
			||(1164 <= c && c <= 1220)
			||(1223 <= c && c <= 1224)
			||(1227 <= c && c <= 1228)
			||(1232 <= c && c <= 1269)
			||(1272 <= c && c <= 1273)
			||(1329 <= c && c <= 1366)
			||(1369 <= c && c <= 1369)
			||(1377 <= c && c <= 1415)
			||(1488 <= c && c <= 1514)
			||(1520 <= c && c <= 1522)
			||(1569 <= c && c <= 1594)
			||(1601 <= c && c <= 1621)
			||(1632 <= c && c <= 1641)
			||(1648 <= c && c <= 1747)
			||(1749 <= c && c <= 1756)
			||(1761 <= c && c <= 1768)
			||(1773 <= c && c <= 1773)
			||(1776 <= c && c <= 1788)
			||(1808 <= c && c <= 1836)
			||(1840 <= c && c <= 1855)
			||(1920 <= c && c <= 1968)
			||(2305 <= c && c <= 2307)
			||(2309 <= c && c <= 2361)
			||(2365 <= c && c <= 2380)
			||(2384 <= c && c <= 2384)
			||(2392 <= c && c <= 2403)
			||(2406 <= c && c <= 2415)
			||(2433 <= c && c <= 2435)
			||(2437 <= c && c <= 2444)
			||(2447 <= c && c <= 2448)
			||(2451 <= c && c <= 2472)
			||(2474 <= c && c <= 2480)
			||(2482 <= c && c <= 2482)
			||(2486 <= c && c <= 2489)
			||(2494 <= c && c <= 2500)
			||(2503 <= c && c <= 2504)
			||(2507 <= c && c <= 2508)
			||(2519 <= c && c <= 2519)
			||(2524 <= c && c <= 2525)
			||(2527 <= c && c <= 2531)
			||(2534 <= c && c <= 2545)
			||(2562 <= c && c <= 2562)
			||(2565 <= c && c <= 2570)
			||(2575 <= c && c <= 2576)
			||(2579 <= c && c <= 2600)
			||(2602 <= c && c <= 2608)
			||(2610 <= c && c <= 2611)
			||(2613 <= c && c <= 2614)
			||(2616 <= c && c <= 2617)
			||(2622 <= c && c <= 2626)
			||(2631 <= c && c <= 2632)
			||(2635 <= c && c <= 2636)
			||(2649 <= c && c <= 2652)
			||(2654 <= c && c <= 2654)
			||(2662 <= c && c <= 2676)
			||(2689 <= c && c <= 2691)
			||(2693 <= c && c <= 2699)
			||(2701 <= c && c <= 2701)
			||(2703 <= c && c <= 2705)
			||(2707 <= c && c <= 2728)
			||(2730 <= c && c <= 2736)
			||(2738 <= c && c <= 2739)
			||(2741 <= c && c <= 2745)
			||(2749 <= c && c <= 2757)
			||(2759 <= c && c <= 2761)
			||(2763 <= c && c <= 2764)
			||(2768 <= c && c <= 2768)
			||(2784 <= c && c <= 2784)
			||(2790 <= c && c <= 2799)
			||(2817 <= c && c <= 2819)
			||(2821 <= c && c <= 2828)
			||(2831 <= c && c <= 2832)
			||(2835 <= c && c <= 2856)
			||(2858 <= c && c <= 2864)
			||(2866 <= c && c <= 2867)
			||(2870 <= c && c <= 2873)
			||(2877 <= c && c <= 2883)
			||(2887 <= c && c <= 2888)
			||(2891 <= c && c <= 2892)
			||(2902 <= c && c <= 2903)
			||(2908 <= c && c <= 2909)
			||(2911 <= c && c <= 2913)
			||(2918 <= c && c <= 2927)
			||(2946 <= c && c <= 2947)
			||(2949 <= c && c <= 2954)
			||(2958 <= c && c <= 2960)
			||(2962 <= c && c <= 2965)
			||(2969 <= c && c <= 2970)
			||(2972 <= c && c <= 2972)
			||(2974 <= c && c <= 2975)
			||(2979 <= c && c <= 2980)
			||(2984 <= c && c <= 2986)
			||(2990 <= c && c <= 2997)
			||(2999 <= c && c <= 3001)
			||(3006 <= c && c <= 3010)
			||(3014 <= c && c <= 3016)
			||(3018 <= c && c <= 3020)
			||(3031 <= c && c <= 3031)
			||(3047 <= c && c <= 3055)
			||(3073 <= c && c <= 3075)
			||(3077 <= c && c <= 3084)
			||(3086 <= c && c <= 3088)
			||(3090 <= c && c <= 3112)
			||(3114 <= c && c <= 3123)
			||(3125 <= c && c <= 3129)
			||(3134 <= c && c <= 3140)
			||(3142 <= c && c <= 3144)
			||(3146 <= c && c <= 3148)
			||(3157 <= c && c <= 3158)
			||(3168 <= c && c <= 3169)
			||(3174 <= c && c <= 3183)
			||(3202 <= c && c <= 3203)
			||(3205 <= c && c <= 3212)
			||(3214 <= c && c <= 3216)
			||(3218 <= c && c <= 3240)
			||(3242 <= c && c <= 3251)
			||(3253 <= c && c <= 3257)
			||(3262 <= c && c <= 3268)
			||(3270 <= c && c <= 3272)
			||(3274 <= c && c <= 3276)
			||(3285 <= c && c <= 3286)
			||(3294 <= c && c <= 3294)
			||(3296 <= c && c <= 3297)
			||(3302 <= c && c <= 3311)
			||(3330 <= c && c <= 3331)
			||(3333 <= c && c <= 3340)
			||(3342 <= c && c <= 3344)
			||(3346 <= c && c <= 3368)
			||(3370 <= c && c <= 3385)
			||(3390 <= c && c <= 3395)
			||(3398 <= c && c <= 3400)
			||(3402 <= c && c <= 3404)
			||(3415 <= c && c <= 3415)
			||(3424 <= c && c <= 3425)
			||(3430 <= c && c <= 3439)
			||(3458 <= c && c <= 3459)
			||(3461 <= c && c <= 3478)
			||(3482 <= c && c <= 3505)
			||(3507 <= c && c <= 3515)
			||(3517 <= c && c <= 3517)
			||(3520 <= c && c <= 3526)
			||(3535 <= c && c <= 3540)
			||(3542 <= c && c <= 3542)
			||(3544 <= c && c <= 3551)
			||(3570 <= c && c <= 3571)
			||(3585 <= c && c <= 3642)
			||(3648 <= c && c <= 3662)
			||(3664 <= c && c <= 3673)
			||(3713 <= c && c <= 3714)
			||(3716 <= c && c <= 3716)
			||(3719 <= c && c <= 3720)
			||(3722 <= c && c <= 3722)
			||(3725 <= c && c <= 3725)
			||(3732 <= c && c <= 3735)
			||(3737 <= c && c <= 3743)
			||(3745 <= c && c <= 3747)
			||(3749 <= c && c <= 3749)
			||(3751 <= c && c <= 3751)
			||(3754 <= c && c <= 3755)
			||(3757 <= c && c <= 3769)
			||(3771 <= c && c <= 3773)
			||(3776 <= c && c <= 3780)
			||(3789 <= c && c <= 3789)
			||(3792 <= c && c <= 3801)
			||(3804 <= c && c <= 3805)
			||(3840 <= c && c <= 3840)
			||(3872 <= c && c <= 3881)
			||(3904 <= c && c <= 3911)
			||(3913 <= c && c <= 3946)
			||(3953 <= c && c <= 3969)
			||(3976 <= c && c <= 3979)
			||(3984 <= c && c <= 3991)
			||(3993 <= c && c <= 4028)
			||(4096 <= c && c <= 4129)
			||(4131 <= c && c <= 4135)
			||(4137 <= c && c <= 4138)
			||(4140 <= c && c <= 4146)
			||(4150 <= c && c <= 4150)
			||(4152 <= c && c <= 4152)
			||(4160 <= c && c <= 4169)
			||(4176 <= c && c <= 4185)
			||(4256 <= c && c <= 4293)
			||(4304 <= c && c <= 4342)
			||(4352 <= c && c <= 4441)
			||(4447 <= c && c <= 4514)
			||(4520 <= c && c <= 4601)
			||(4608 <= c && c <= 4614)
			||(4616 <= c && c <= 4678)
			||(4680 <= c && c <= 4680)
			||(4682 <= c && c <= 4685)
			||(4688 <= c && c <= 4694)
			||(4696 <= c && c <= 4696)
			||(4698 <= c && c <= 4701)
			||(4704 <= c && c <= 4742)
			||(4744 <= c && c <= 4744)
			||(4746 <= c && c <= 4749)
			||(4752 <= c && c <= 4782)
			||(4784 <= c && c <= 4784)
			||(4786 <= c && c <= 4789)
			||(4792 <= c && c <= 4798)
			||(4800 <= c && c <= 4800)
			||(4802 <= c && c <= 4805)
			||(4808 <= c && c <= 4814)
			||(4816 <= c && c <= 4822)
			||(4824 <= c && c <= 4846)
			||(4848 <= c && c <= 4878)
			||(4880 <= c && c <= 4880)
			||(4882 <= c && c <= 4885)
			||(4888 <= c && c <= 4894)
			||(4896 <= c && c <= 4934)
			||(4936 <= c && c <= 4954)
			||(4969 <= c && c <= 4977)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswalnum(c));
			#endif
			return true;
		}
	}
	else {
		if ((5024 <= c && c <= 5108)
			||(5121 <= c && c <= 5740)
			||(5743 <= c && c <= 5750)
			||(5761 <= c && c <= 5786)
			||(5792 <= c && c <= 5866)
			||(6016 <= c && c <= 6088)
			||(6112 <= c && c <= 6121)
			||(6160 <= c && c <= 6169)
			||(6176 <= c && c <= 6210)
			||(6212 <= c && c <= 6263)
			||(6272 <= c && c <= 6313)
			||(7680 <= c && c <= 7835)
			||(7840 <= c && c <= 7929)
			||(7936 <= c && c <= 7957)
			||(7960 <= c && c <= 7965)
			||(7968 <= c && c <= 8005)
			||(8008 <= c && c <= 8013)
			||(8016 <= c && c <= 8023)
			||(8025 <= c && c <= 8025)
			||(8027 <= c && c <= 8027)
			||(8029 <= c && c <= 8029)
			||(8031 <= c && c <= 8061)
			||(8064 <= c && c <= 8116)
			||(8118 <= c && c <= 8124)
			||(8126 <= c && c <= 8126)
			||(8130 <= c && c <= 8132)
			||(8134 <= c && c <= 8140)
			||(8144 <= c && c <= 8147)
			||(8150 <= c && c <= 8155)
			||(8160 <= c && c <= 8172)
			||(8178 <= c && c <= 8180)
			||(8182 <= c && c <= 8188)
			||(8319 <= c && c <= 8319)
			||(8450 <= c && c <= 8450)
			||(8455 <= c && c <= 8455)
			||(8458 <= c && c <= 8467)
			||(8469 <= c && c <= 8469)
			||(8473 <= c && c <= 8477)
			||(8484 <= c && c <= 8484)
			||(8486 <= c && c <= 8486)
			||(8488 <= c && c <= 8488)
			||(8490 <= c && c <= 8493)
			||(8495 <= c && c <= 8497)
			||(8499 <= c && c <= 8505)
			||(8544 <= c && c <= 8579)
			||(12294 <= c && c <= 12295)
			||(12321 <= c && c <= 12329)
			||(12344 <= c && c <= 12346)
			||(12353 <= c && c <= 12436)
			||(12449 <= c && c <= 12538)
			||(12549 <= c && c <= 12588)
			||(12593 <= c && c <= 12686)
			||(12704 <= c && c <= 12727)
			||(13312 <= c && c <= 19893)
			||(19968 <= c && c <= 40869)
			||(40960 <= c && c <= 42124)
			||(44032 <= c && c <= 55203)
			||(63744 <= c && c <= 64045)
			||(64256 <= c && c <= 64262)
			||(64275 <= c && c <= 64279)
			||(64285 <= c && c <= 64285)
			||(64287 <= c && c <= 64296)
			||(64298 <= c && c <= 64310)
			||(64312 <= c && c <= 64316)
			||(64318 <= c && c <= 64318)
			||(64320 <= c && c <= 64321)
			||(64323 <= c && c <= 64324)
			||(64326 <= c && c <= 64433)
			||(64467 <= c && c <= 64829)
			||(64848 <= c && c <= 64911)
			||(64914 <= c && c <= 64967)
			||(65008 <= c && c <= 65019)
			||(65136 <= c && c <= 65138)
			||(65140 <= c && c <= 65140)
			||(65142 <= c && c <= 65276)
			||(65296 <= c && c <= 65305)
			||(65313 <= c && c <= 65338)
			||(65345 <= c && c <= 65370)
			||(65382 <= c && c <= 65391)
			||(65393 <= c && c <= 65437)
			||(65440 <= c && c <= 65470)
			||(65474 <= c && c <= 65479)
			||(65482 <= c && c <= 65487)
			||(65490 <= c && c <= 65495)
			||(65498 <= c && c <= 65500)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswalnum(c));
			#endif
			return true;
		}
	}
	#if		qTestMyISWXXXFunctions
	Led_Assert (!iswalnum(c));
	#endif
	return false;
}

bool	CharacterProperties::IsPunct_M (wchar_t c)
{
	// ********** CharacterProperties::ispunct_M (AUTOGENERATED memoize of iswpunct - Feb 24 2003) ***********
	// Hack for SPR#1220 and SPR#1306
	if (c < 255) {
		if ((33 <= c && c <= 47)
			||(58 <= c && c <= 64)
			||(91 <= c && c <= 96)
			||(123 <= c && c <= 126)
			||(161 <= c && c <= 191)
			||(215 <= c && c <= 215)
			||(247 <= c && c <= 247)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswpunct(c));
			#endif
			return true;
		}
	}
	else if (c < 5000) {
		if ((894 <= c && c <= 894)
			||(903 <= c && c <= 903)
			||(1370 <= c && c <= 1375)
			||(1417 <= c && c <= 1418)
			||(1470 <= c && c <= 1470)
			||(1472 <= c && c <= 1472)
			||(1475 <= c && c <= 1475)
			||(1523 <= c && c <= 1524)
			||(1548 <= c && c <= 1548)
			||(1563 <= c && c <= 1563)
			||(1567 <= c && c <= 1567)
			||(1642 <= c && c <= 1645)
			||(1748 <= c && c <= 1748)
			||(1792 <= c && c <= 1805)
			||(2404 <= c && c <= 2405)
			||(2416 <= c && c <= 2416)
			||(3572 <= c && c <= 3572)
			||(3663 <= c && c <= 3663)
			||(3674 <= c && c <= 3675)
			||(3844 <= c && c <= 3858)
			||(3898 <= c && c <= 3901)
			||(3973 <= c && c <= 3973)
			||(4170 <= c && c <= 4175)
			||(4347 <= c && c <= 4347)
			||(4961 <= c && c <= 4968)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswpunct(c));
			#endif
			return true;
		}
	}
	else {
		if ((5741 <= c && c <= 5742)
			||(5787 <= c && c <= 5788)
			||(5867 <= c && c <= 5869)
			||(6100 <= c && c <= 6106)
			||(6108 <= c && c <= 6108)
			||(6144 <= c && c <= 6154)
			||(8208 <= c && c <= 8231)
			||(8240 <= c && c <= 8259)
			||(8261 <= c && c <= 8262)
			||(8264 <= c && c <= 8269)
			||(8317 <= c && c <= 8318)
			||(8333 <= c && c <= 8334)
			||(9001 <= c && c <= 9002)
			||(12289 <= c && c <= 12291)
			||(12296 <= c && c <= 12305)
			||(12308 <= c && c <= 12319)
			||(12336 <= c && c <= 12336)
			||(12539 <= c && c <= 12539)
			||(64830 <= c && c <= 64831)
			||(65072 <= c && c <= 65092)
			||(65097 <= c && c <= 65106)
			||(65108 <= c && c <= 65121)
			||(65123 <= c && c <= 65123)
			||(65128 <= c && c <= 65128)
			||(65130 <= c && c <= 65131)
			||(65281 <= c && c <= 65283)
			||(65285 <= c && c <= 65290)
			||(65292 <= c && c <= 65295)
			||(65306 <= c && c <= 65307)
			||(65311 <= c && c <= 65312)
			||(65339 <= c && c <= 65341)
			||(65343 <= c && c <= 65343)
			||(65371 <= c && c <= 65371)
			||(65373 <= c && c <= 65373)
			||(65377 <= c && c <= 65381)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswpunct(c));
			#endif
			return true;
		}
	}
	#if		qTestMyISWXXXFunctions
	Led_Assert (!iswpunct(c));
	#endif
	return false;
}

bool	CharacterProperties::IsSpace_M (wchar_t c)
{
	// ********** CharacterProperties::isspace_M (AUTOGENERATED memoize of iswspace - Feb 24 2003) ***********
	// Hack for SPR#1220 and SPR#1306
	if (c < 255) {
		if ((9 <= c && c <= 13)
			||(32 <= c && c <= 32)
			||(160 <= c && c <= 160)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswspace(c));
			#endif
			return true;
		}
	}
	else if (c < 5000) {
		if (false			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswspace(c));
			#endif
			return true;
		}
	}
	else {
		if ((5760 <= c && c <= 5760)
			||(8192 <= c && c <= 8203)
			||(8232 <= c && c <= 8233)
			||(8239 <= c && c <= 8239)
			||(12288 <= c && c <= 12288)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswspace(c));
			#endif
			return true;
		}
	}
	#if		qTestMyISWXXXFunctions
	Led_Assert (!iswspace(c));
	#endif
	return false;
}

bool	CharacterProperties::IsCntrl_M (wchar_t c)
{
	// ********** CharacterProperties::iscntrl_M (AUTOGENERATED memoize of iswcntrl - Feb 24 2003) ***********
	// Hack for SPR#1220 and SPR#1306
	if (c < 255) {
		if ((0 <= c && c <= 31)
			||(127 <= c && c <= 159)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswcntrl(c));
			#endif
			return true;
		}
	}
	else if (c < 5000) {
		if ((1807 <= c && c <= 1807)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswcntrl(c));
			#endif
			return true;
		}
	}
	else {
		if ((6155 <= c && c <= 6158)
			||(8204 <= c && c <= 8207)
			||(8234 <= c && c <= 8238)
			||(8298 <= c && c <= 8303)
			||(65279 <= c && c <= 65279)
			||(65529 <= c && c <= 65531)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswcntrl(c));
			#endif
			return true;
		}
	}
	#if		qTestMyISWXXXFunctions
	Led_Assert (!iswcntrl(c));
	#endif
	return false;
}

bool	CharacterProperties::IsDigit_M (wchar_t c)
{
	// ********** CharacterProperties::IsDigit_M (AUTOGENERATED memoize of iswdigit - Jun 25 2003) ***********
	// Hack for SPR#1220 and SPR#1306
	if (c < 255) {
		if ((48 <= c && c <= 57)
			||(178 <= c && c <= 179)
			||(185 <= c && c <= 185)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswdigit(c));
			#endif
			return true;
		}
	}
	else if (c < 5000) {
		if ((1632 <= c && c <= 1641)
			||(1776 <= c && c <= 1785)
			||(2406 <= c && c <= 2415)
			||(2534 <= c && c <= 2543)
			||(2662 <= c && c <= 2671)
			||(2790 <= c && c <= 2799)
			||(2918 <= c && c <= 2927)
			||(3047 <= c && c <= 3055)
			||(3174 <= c && c <= 3183)
			||(3302 <= c && c <= 3311)
			||(3430 <= c && c <= 3439)
			||(3664 <= c && c <= 3673)
			||(3792 <= c && c <= 3801)
			||(3872 <= c && c <= 3881)
			||(4160 <= c && c <= 4169)
			||(4969 <= c && c <= 4977)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswdigit(c));
			#endif
			return true;
		}
	}
	else {
		if ((6112 <= c && c <= 6121)
			||(6160 <= c && c <= 6169)
			||(65296 <= c && c <= 65305)
			) {
			#if		qTestMyISWXXXFunctions
			Led_Assert (iswdigit(c));
			#endif
			return true;
		}
	}
	#if		qTestMyISWXXXFunctions
	Led_Assert (!iswdigit(c));
	#endif
	return false;
}

#endif

#endif






#if		qBuildInTableDrivenCodePageBuilderProc
/*
 ********************************************************************************
 ******************************* WriteCodePageTable *****************************
 ********************************************************************************
 */

static	void	WriteCodePageTable (CodePage codePage)
{
	Win32_CodePageConverter	converter (codePage);
	ofstream	outStream ("codePage.txt");

	outStream << "// DOT_H\n";
	outStream << "class	TableDrivenCodePageConverter<" << codePage << "> {\n";
	outStream << "	public:\n";
	outStream << "		static	void	MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt);\n";
	outStream << "		static	void	MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt);\n";
	outStream << "	private:\n";
	outStream << "		static	const	wchar_t	kMap[256];\n";
	outStream << "};\n";
	outStream << "\n";
	outStream << "\n";


	outStream << "// DOT_CPP\n";
	outStream << "/*\n";
	outStream << " ********************************************************************************\n";
	outStream << " ************************ TableDrivenCodePageConverter<" << codePage << "> ********************\n";
	outStream << " ********************************************************************************\n";
	outStream << " */\n";
	outStream << "const	wchar_t		TableDrivenCodePageConverter<" << codePage << ">::kMap[256] = {\n";
	for (size_t i = 0; i < 256; ++i) {
		if (i != 0 and i % 8 == 0) {
			outStream << "\n";
		}
		wchar_t	result = 0;
		char	inChar	=	i;
		size_t length	= 1;
		converter.MapToUNICODE (&inChar, 1, &result, &length);
		outStream << "\t\t0x" << hex << result << ",";
	}
	outStream << dec;
	outStream << "\n};";
	outStream << "\n";
	outStream << "\n";
	outStream << "void	TableDrivenCodePageConverter<" << codePage << ">::MapToUNICODE (const char* inMBChars, size_t inMBCharCnt, wchar_t* outChars, size_t* outCharCnt)\n";
	outStream << "{\n";
	outStream << "	size_t	nCharsToCopy	=	min (inMBCharCnt, *outCharCnt);\n";
	outStream << "	for (size_t i = 0; i < nCharsToCopy; ++i) {\n";
	outStream << "		outChars[i] = kMap[(unsigned char)inMBChars[i]];\n";
	outStream << "	}\n";
	outStream << "	*outCharCnt = nCharsToCopy;\n";
	outStream << "}\n";
	outStream << "\n";
	outStream << "void	TableDrivenCodePageConverter<" << codePage << ">::MapFromUNICODE (const wchar_t* inChars, size_t inCharCnt, char* outChars, size_t* outCharCnt)\n";
	outStream << "{\n";
	outStream << "	size_t	nCharsToCopy	=	min (inCharCnt, *outCharCnt);\n";
	outStream << "	for (size_t i = 0; i < nCharsToCopy; ++i) {\n";
	outStream << "		size_t j = 0;\n";
	outStream << "		for (; j < 256; ++j) {\n";
	outStream << "			if (kMap[j] == inChars[i]) {\n";
	outStream << "				outChars[i] = j;\n";
	outStream << "				break;\n";
	outStream << "			}\n";
	outStream << "		}\n";
	outStream << "		if (j == 256) {\n";
	outStream << "			/*Perhaps should add extra args to this function - as in Win32 API - for handling this missing-char case*/\n";
	outStream << "			outChars[i] = '?';\n";
	outStream << "		}\n";
	outStream << "	}\n";
	outStream << "	*outCharCnt = nCharsToCopy;\n";
	outStream << "}\n";
}
#endif



#if		qLedUsesNamespaces
}
#endif








// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

