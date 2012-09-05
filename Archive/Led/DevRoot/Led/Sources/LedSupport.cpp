/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/Led/Sources/LedSupport.cpp,v 2.102 2004/02/08 15:23:11 lewis Exp $
 *
 * Changes:
 *	$Log: LedSupport.cpp,v $
 *	Revision 2.102  2004/02/08 15:23:11  lewis
 *	better error handling in DumpSupportedInterfaces ()
 *	
 *	Revision 2.101  2004/01/23 21:39:59  lewis
 *	qDeleteOfPointerToConstantDataFailsOverloadError BWA supprot for MSVC6
 *	
 *	Revision 2.100  2003/12/29 15:46:25  lewis
 *	Added Led_tString2WideString and fix arg to Led_tString2ANSIString
 *	
 *	Revision 2.99  2003/12/17 01:15:44  lewis
 *	Added Led_SDKString2Wide, Led_WideString2tString
 *	
 *	Revision 2.98  2003/12/12 01:42:19  lewis
 *	Get to compile for MacOS
 *	
 *	Revision 2.97  2003/12/09 20:41:43  lewis
 *	Added FileWriter class (as assist for SPR#1585)
 *	
 *	Revision 2.96  2003/12/08 02:36:24  lewis
 *	add more includes and fix _lseek etc to be lseek() for GCC
 *	
 *	Revision 2.95  2003/11/06 16:27:33  lewis
 *	fix macos fileName
 *	
 *	Revision 2.94  2003/09/22 20:03:07  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.93  2003/09/22 15:33:48  lewis
 *	Added FileReader class to help fix bug#1552
 *	
 *	Revision 2.92  2003/06/24 14:10:26  lewis
 *	minor tweeks for XWindows
 *	
 *	Revision 2.91  2003/06/19 13:56:37  lewis
 *	Added Led_Wide2SDKString () function
 *	
 *	Revision 2.90  2003/06/17 23:55:14  lewis
 *	As part of SPR#1536 - added DumpObjectsInIterator () and DumpSupportedInterfaces () debugging helpers
 *	
 *	Revision 2.89  2003/06/12 17:11:50  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.88  2003/06/12 16:59:00  lewis
 *	SPR#1526: VariantArrayPacker/VariantArrayUnpacker helper classes for COM. Added UnpackVectorOfStringsFromVariantArray ()
 *	function, and redid CreateSafeArrayOfBSTR to use VariantArrayPacker. Also made it so it could build SAFEARRAY of
 *	BSTR or VARIANTS (which contianed BSTR - test which works better with javascript/MSIE)
 *	
 *	Revision 2.87  2003/06/12 15:18:15  lewis
 *	moved CreateSafeArrayOfBSTR () from Led_ATL to LedSupport
 *	
 *	Revision 2.86  2003/06/10 14:25:43  lewis
 *	added ACP2WideString/Wide2ACPString helpers
 *	
 *	Revision 2.85  2003/04/30 14:55:20  lewis
 *	include missing <cstdarg> cuz used and GCC 3.2 complains without it
 *	
 *	Revision 2.84  2003/04/07 15:14:17  lewis
 *	SPR#1409: added SPrintF and Format () utilities
 *	
 *	Revision 2.83  2003/04/07 13:25:29  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.82  2003/03/11 03:07:41  lewis
 *	make sure URL is quoted in call to Netscape for Linux
 *	
 *	Revision 2.81  2003/03/11 02:24:01  lewis
 *	SPR#1288 - added qLed_MajorMinorVersionString define and MakeSophistsAppNameVersionURL () helper
 *	
 *	Revision 2.80  2003/02/28 19:09:52  lewis
 *	SPR#1316- CRTDBG_MAP_ALLOC/CRTDBG_MAP_ALLOC_NEW Win32/MSVC debug mem leak support
 *	
 *	Revision 2.79  2003/02/27 21:33:08  lewis
 *	hack to Led_tStriCmp/Led_tStrniCmp (qNoSTRNICMP) implemenations for qWideCharacters (need on macos)
 *	
 *	Revision 2.78  2003/02/24 15:44:35  lewis
 *	SPR#1306- moved most implementations of stuff like Led_tString2ANSIString etc to CPP file so
 *	they can include/use code from CodePage.h to do codepage conversions. Just leave in header stuff
 *	cases that SB inline - cuz they are trivail mappings. Use CodePageConverter() class instead of
 *	direct ::WideCharToM... or ::MutliByteTo... functions (so portable to MacOS)
 *	
 *	Revision 2.77  2003/02/14 20:40:18  lewis
 *	SPR#1303 - qLed_CharacterSet is now obsolete. Instead - use qSingleByteCharacters,
 *	qMultiByteCharacters, or qWideCharacters
 *	
 *	Revision 2.76  2003/02/14 19:21:32  lewis
 *	just like for MacOS - with UNICODE - treat yen character and nonbreaking space and a few others
 *	as word characters
 *	
 *	Revision 2.75  2003/02/03 20:56:06  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.74  2003/02/03 15:13:03  lewis
 *	SPR#1291 - Added qLedCheckCompilerFlagsConsistency to check for mismatches on compile flags
 *	
 *	Revision 2.73  2003/01/22 22:45:36  lewis
 *	SPR#1257- support qSDKHasInternetConfig
 *	
 *	Revision 2.72  2003/01/08 02:25:23  lewis
 *	fix so compiles for FULL_UNICODE on WIN32
 *	
 *	Revision 2.71  2002/12/19 16:49:08  lewis
 *	SPR#1215 -  Added EmitTraceMessage/LedDebugTrace/qNoOpKeywordSupported/qDefaultTracingOn
 *	support. Really only works (well) for MS-Windows, but could pretty easily get
 *	working elsewhere - if there is any need.
 *	
 *	Revision 2.70  2002/09/19 14:14:29  lewis
 *	Lose qSupportLed23CompatAPI (and related backward compat API tags like qOldStyleMarkerBackCompatHelperCode,
 *	qBackwardCompatUndoHelpers, etc).
 *	
 *	Revision 2.69  2002/09/09 03:17:15  lewis
 *	SPR#1092 change Led_tStrniCmp() return value to int, and added Led_tStriCmp ()
 *	
 *	Revision 2.68  2002/05/06 21:33:44  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 2.67  2001/11/27 00:29:51  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.66  2001/10/17 20:42:57  lewis
 *	Massive changes - cleaning up doccomments (mostly adding <p>/</p> as appropriate
 *	
 *	Revision 2.65  2001/10/09 21:03:22  lewis
 *	SPR#1058- qSupportLed23CompatAPI support
 *	
 *	Revision 2.64  2001/09/26 15:41:18  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.63  2001/08/29 23:36:32  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.62  2001/08/28 18:43:35  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.61  2001/07/19 02:21:46  lewis
 *	SPR#0960/0961- CW6Pro support, and preliminary Carbon SDK support
 *	
 *	Revision 2.60  2001/05/24 13:42:44  lewis
 *	SPR#0938- hack to get Led_BeepNotify() working for X-Windows- gBeepNotifyCallBackProc
 *	
 *	Revision 2.59  2001/05/23 16:30:38  lewis
 *	SPR#0935- Clipboard COPY command was failing. Source of trouble was the time values
 *	retunred on that X-Server were too big to fit in a long (need unsigned long) and so
 *	LedTickCount2XTime () screwed up. Fixed it to take an UNSIGNED long
 *	
 *	Revision 2.58  2001/05/20 21:41:02  lewis
 *	fix small X-Windows bugs with SyncronizeLedXTickCount () stuff - make arg unsigned int -
 *	and handle case where getTickCount called before SyncronizeLedXTickCount () method.
 *	
 *	Revision 2.57  2001/05/18 23:21:02  lewis
 *	change Open_SystemNetscape () code to not call kdehelp - instead just use netscape (kdehelp
 *	didnt work). Also - call Open_SystemNetscape () from OpenURL method (if qUseSystemNetscapeOpenURLs)
 *	
 *	Revision 2.56  2001/05/18 20:58:18  lewis
 *	qUseSystemNetscapeOpenURLs- support for X-Windows/UNIX OpenURL calls - through system()
 *	call to run Netscape browser
 *	
 *	Revision 2.55  2001/05/15 16:39:57  lewis
 *	lose qGCC_gettimeofdayBug- my mistake
 *	
 *	Revision 2.54  2001/05/03 20:42:48  lewis
 *	Added LedTickCount2XTime
 *	
 *	Revision 2.53  2001/05/02 18:22:53  lewis
 *	SPR# 0895- Move Led_GetDoubleClickTime/Led_BeepNotify/Led_GetTickCount to CPP file.
 *	Add (for XWindows only) SyncronizeLedXTickCount API. Fix Led_GetTickCount() to return
 *	much better results (using  sync hack with gettimeofday)
 *	
 *	Revision 2.52  2001/05/01 16:29:28  lewis
 *	SPR#0892- partial help towards this SPR. Minimal (cruddy) support for Led_GetTickCount/GetDoubleClickTime
 *	
 *	Revision 2.51  2000/10/14 14:30:46  lewis
 *	fix declaration for hlinkNavigateString to be WINAPI - was causing bizare crash under
 *	NetLedIt! windows. No idea why worked so well with other browsers/OCX version
 *	ANyhow - THAT may have been part of why I used to get crashes in old ActiveledIt!
 *	when it invoked help!
 *	
 *	Revision 2.50  2000/09/14 19:03:50  lewis
 *	react to make include file name change
 *	
 *	Revision 2.49  2000/04/26 21:07:46  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.48  2000/04/16 14:39:36  lewis
 *	trivial tweeks to get compiling on Mac
 *	
 *	Revision 2.47  2000/04/15 14:32:37  lewis
 *	small changes to get qLedUsesNamespaces (SPR#0740) working on Mac
 *	
 *	Revision 2.46  2000/04/14 22:40:24  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.45  2000/04/04 23:58:24  lewis
 *	fix bug in Led_ThrowIfErr () - do nothing if err == noErr
 *	
 *	Revision 2.44  2000/04/04 22:01:44  lewis
 *	fix MacOS-specific typos
 *	
 *	Revision 2.43  2000/04/04 20:49:56  lewis
 *	SPR# 0732- added  Led_Set_OutOfMemoryException_Handler/Led_Set_BadFormatDataException_Handler/
 *	Led_Set_ThrowOSErrException_Handler. And qGenerateStandardPowerPlantExceptions and
 *	qGenerateStandardMFCExceptions defines. The idea is to make it easier to override
 *	these exception throw formats, and get better default behavior for cases other than
 *	MFC and PowerPlant.
 *	
 *	Revision 2.42  2000/04/03 18:50:43  lewis
 *	Fixed comment typo, and fixed serious logic-typo in launching web pages via ActiveX -
 *	was throwing if SUCCEDED - not if FAILED. Maybe that was causing the crashes I saw
 *	with different web brwosers and browsing help from ActiveLedIt!
 *	
 *	Revision 2.41  1999/12/24 23:45:16  lewis
 *	define bug-define qNoSTRNICMP- and BWA version of qNoSTRNICMP
 *	
 *	Revision 2.40  1999/12/21 20:53:18  lewis
 *	use new Led_ThrowIfNull instead of manually checking for NULL
 *	
 *	Revision 2.39  1999/12/14 20:03:09  lewis
 *	Moved Led_IME code from LedSupport to LedGDI
 *	
 *	Revision 2.38  1999/12/14 18:07:55  lewis
 *	Preliminary qXWindows support
 *	
 *	Revision 2.37  1999/12/09 03:25:08  lewis
 *	SPR#0645 - preliminary qSDK_UNICODE (_UNICODE) support (mostly using new Led_SDK_String/Led_SDK_Char)
 *	
 *	Revision 2.36  1999/11/15 21:24:08  lewis
 *	Moved some graphix stuff to LedGDI. Lose any MFC dependencies (chaning Led_IME to not use CWnd -
 *	etc. And fix the DDE Spyglass code to not depend on CString - an MFC class. No include of
 *	afxwin.h - nor any MFC headers
 *	
 *	Revision 2.35  1999/11/13 16:32:21  lewis
 *	<===== Led 2.3 Released =====>
 *	
 *	Revision 2.34  1999/07/18 01:31:48  lewis
 *	use DWORD instead of HIMC - rather than chasing down where include sb coming from -
 *	that matches our declared ptr-to-funtion fields
 *	
 *	Revision 2.33  1999/06/15 02:32:57  lewis
 *	spr#0583- fix a (bogus) memory leak with Led_GDIGlobals (case where memleak detectors
 *	would whine mistakenly, and cleaned up similar Led_IME hack
 *	
 *	Revision 2.32  1999/05/03 22:05:07  lewis
 *	Misc cosmetic cleanups
 *	
 *	Revision 2.31  1999/05/03 21:41:27  lewis
 *	Misc minor COSMETIC cleanups - mostly deleting ifdefed out code
 *	
 *	Revision 2.30  1999/04/16 00:42:55  lewis
 *	add Led_IME::NotifyOfFontChange() as partial attempt at spr# 0573
 *	
 *	Revision 2.29  1999/03/02 17:09:24  lewis
 *	fix Led_GDIGlobals::InvalidateGlobals () to compile/work on mac
 *	
 *	Revision 2.28  1999/03/02 04:24:18  lewis
 *	replace  ::GetDeviceCaps (screenDC, LOGPIXELSY) calls with Led_GDIGlobals::Get ().GetMainScreenLogPixelsV ()
 *	and new class Led_GDIGlobals as a speed tweek. And use iterator in IndexOf() function (speed tweek?).
 *	
 *	Revision 2.27  1998/10/30 14:30:05  lewis
 *	vector<> class instead of Led_Array<>
 *	
 *	Revision 2.26  1998/07/24  01:07:06  lewis
 *	Moved Led_CasedStringsEqual () here from header.
 *	Imporoved JAP/IME stuff to set the font and so on as the IME is directed to move.
 *	Fixed clipboard bug.
 *	Led_SkrunchOutSpecialChars().
 *
 *	Revision 2.25  1998/04/25  01:30:40  lewis
 *	comment out some obsolete SJIS code.
 *
 *	Revision 2.24  1998/04/08  01:47:00  lewis
 *	Include <ime.h>
 *
 *	Revision 2.23  1997/12/24  04:40:13  lewis
 *	*** empty log message ***
 *
 *	Revision 2.22  1997/09/29  15:03:19  lewis
 *	Lose qLedFirstIndex support.
 *
 *	Revision 2.21  1997/07/27  15:58:28  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.20  1997/06/18  02:55:19  lewis
 *	AssertionHandler now takes filename/line#
 *
 *	Revision 2.19  1997/01/20  05:23:53  lewis
 *	New Led_Color class.
 *
 *	Revision 2.18  1996/12/13  17:58:46  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.17  1996/12/05  19:11:38  lewis
 *	*** empty log message ***
 *
 *	Revision 2.16  1996/10/31  00:11:07  lewis
 *	Cleanup some (harmless) warnings using Led_Arg_Unused, and copying art to ::ICLaunchURL() into
 *	temporary.
 *
 *	Revision 2.15  1996/10/15  18:51:54  lewis
 *	*** empty log message ***
 *
 *	Revision 2.14  1996/10/15  18:30:01  lewis
 *	Led_URLManager::Open () - break out body into separate methods for IC/ActiveX/DDE cases etc.
 *	And first time implemented for ActiveX case.
 *
 *	Revision 2.13  1996/09/30  14:23:35  lewis
 *	A couple minor hacks to string::append() usage for new STL/CW10/MSL implemenation.
 *
 *	Revision 2.12  1996/09/01  15:31:50  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.11  1996/08/05  06:07:20  lewis
 *	New Get/Set AssertionHandler routines.
 *
 *	Revision 2.10  1996/07/19  16:35:13  lewis
 *	Fix mac FSSpecToURL code.
 *	(handle bad characters)
 *
 *	Revision 2.9  1996/07/03  01:07:55  lewis
 *	Added new Led_URLD class to encapsulate Netscape bookmark object
 *	and Led_URLManager to encapsulate code on mac/pc for managing/opening
 *	URLs (via DDE/AppleEvents/ActiveX/InternetConfig or whatever).
 *	Much code here is new, and much just moved from StyledTextEmbeddedObjects.cpp
 *
 *	Revision 2.8  1996/06/01  02:07:47  lewis
 *	qProvideIMESupport (was qRequireIMESupport).
 *
 *	Revision 2.7  1996/05/14  20:25:48  lewis
 *	Got rid of a few more LEC remnants of Kinsoku rule support (ISBOL/etc)
 *
 *	Revision 2.6  1996/05/04  21:03:49  lewis
 *	Added routines to compute DIB size (bytes/pixels/pallet etc).
 *
 *	Revision 2.5  1996/03/16  18:58:55  lewis
 *	Use Led_NewHandle
 *
 *	Revision 2.4  1996/03/05  18:28:18  lewis
 *	Renamed Led_ClipboardObjectAcquire::ClipFormat --> Led_ClipFormat,
 *	and similarly for TEXT and PICT static members (to kTEXTClipFormat etc).
 *
 *	Revision 2.3  1995/11/25  00:24:12  lewis
 *	Added PICT ClipFormat.
 *
 *	Revision 2.2  1995/11/02  22:23:41  lewis
 *	On assert error under MFC - call AfxDebugBreak() instead of abort
 *
 *	Revision 2.1  1995/09/06  20:57:00  lewis
 *	Support new mwerks compiler (CW7), break out textstore class as owned
 *	and OpenDoc support, and much more (starting 2.0).
 *
 *	Revision 1.11  1995/05/30  00:07:03  lewis
 *	Changed default emacs startup mode from 'text' to 'c++'. Only relevant to emacs users. SPR 0301.
 *
 *	Revision 1.10  1995/05/29  23:48:22  lewis
 *	Better out of memory checking - now call Led_ThrowOutOfMemoryException ()
 *	as needed.
 *
 *	Revision 1.9  1995/05/06  19:46:44  lewis
 *	Use Led_tChar instead of char for UNICODE support - SPR 0232.
 *
 *	Revision 1.8  1995/05/05  19:47:17  lewis
 *	_Led_IME_DESTRUCTOR_ so Alec is happy we destroy the Led_IME object on
 *	app exit.
 *
 *	Revision 1.7  1995/04/25  02:55:42  lewis
 *	Add Led_ClipboardObjectAcquire support for SRP 0222
 *
 *	Revision 1.6  1995/04/18  00:43:16  lewis
 *	Delete obsolete gIME.
 *
 *	Revision 1.5  1995/03/23  03:48:29  lewis
 *	Cleanup Led IME code.
 *
 *	Revision 1.4  1995/03/13  03:20:40  lewis
 *	Cleaned up the Led_IME code, and renamed a bunch of things to prefix
 *	with Led_ (eg Array, BlockAllocated, NLtoNative () etc...).
 *
 *	Revision 1.3  1995/03/06  21:31:19  lewis
 *	Also check Led_IsValidSingleByte() to check for valid
 *	multibyte string. Currently - this check is a NOOP - but
 *	it is the right thing todo if I can ever figure out what are
 *	legit single byte characters (not as simple as just isascii
 *	cuz in SJIS there are the half-width kana characters - acording
 *	to Alec).
 *
 *	Revision 1.2  1995/03/02  05:46:10  lewis
 *	*** empty log message ***
 *
 *	Revision 1.1  1995/03/02  05:30:46  lewis
 *	Initial revision
 *
 *
 *
 */
#if		qIncludePrefixFile
	#include	"stdafx.h"
#endif

#include	<cctype>
#include	<cstdarg>

#include	"LedConfig.h"	// For qWindows etc defines...

#if		qMacOS
	#include	<AppleEvents.h>		// for URL support
	#include	<Scrap.h>
	#include	<TextUtils.h>
#elif	qWindows
	#include	<fcntl.h>
	#include	<io.h>
#elif	qXWindows
	#include	<unistd.h>
	#include	<time.h>
	#include	<sys/time.h>
	#include	<stdio.h>
	#include	<sys/stat.h>
	#include	<sys/types.h>
	#include	<fcntl.h>
#endif

#include	"CodePage.h"

#include	"LedSupport.h"

#if		qUseInternetConfig
	#if		qSDKHasInternetConfig
		#include	<InternetConfig.h>
	#else
		#include	<ICTypes.h>
		#include	<ICAPI.h>
	#endif
#endif
#if		qUseActiveXToOpenURLs
	#include	<URLMon.h>
#endif



#if		defined (CRTDBG_MAP_ALLOC_NEW)
	#define	new	CRTDBG_MAP_ALLOC_NEW
#endif




#if		qLedUsesNamespaces
	namespace	Led {
#endif



/*
@METHOD:		ACP2WideString
@DESCRIPTION:	
*/
wstring	ACP2WideString (const string& s)
{
	size_t nChars	= s.length () + 1;	// convert null byte, too
	Led_SmallStackBuffer<wchar_t>	result (nChars);
	CodePageConverter	cpg (GetDefaultSDKCodePage ());
	cpg.MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return wstring (result);
}




/*
@METHOD:		Wide2ACPString
@DESCRIPTION:	
*/
string	Wide2ACPString (const wstring& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter	cpg (GetDefaultSDKCodePage ());
	cpg.MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return string (result);
}




#if		qSDK_UNICODE
/*
@METHOD:		Led_ANSI2SDKString
@DESCRIPTION:	<p></p>
*/
Led_SDK_String	Led_ANSI2SDKString (const string& s)
{
	size_t nChars	= s.length () + 1;	// convert null byte, too
	Led_SmallStackBuffer<Led_SDK_Char>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return Led_SDK_String (result);
}
#else
Led_SDK_String	Led_Wide2SDKString (const wstring& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter	cpg (GetDefaultSDKCodePage ());
	cpg.MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return Led_SDK_String (result);
}
wstring	Led_SDKString2Wide (const Led_SDK_String& s)
{
	return ACP2WideString (s);
}
#endif	

#if		qSDK_UNICODE
/*
@METHOD:		Led_SDKString2ANSI
@DESCRIPTION:	<p></p>
*/
string	Led_SDKString2ANSI (const Led_SDK_String& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return string (result);
}
#endif

#if			!qWideCharacters
Led_tString	Led_WideString2tString (const wstring& s)
{
	return Wide2ACPString (s);
}

wstring	Led_tString2WideString (const Led_tString& s)
{
	return ACP2WideString (s);
}
#endif

#if		qWideCharacters != qSDK_UNICODE
/*
@METHOD:		Led_tString2SDKString
@DESCRIPTION:	<p></p>
*/
Led_SDK_String	Led_tString2SDKString (const Led_tString& s)
{
	#if		qWideCharacters && !qSDK_UNICODE
		size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_SDK_Char>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_SDK_String (result);
	#elif	!qWideCharacters && qSDK_UNICODE
		size_t nChars	= s.length () + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_SDK_Char>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_SDK_String (result);
	#else
		#error	"Hmm"
	#endif
}
#endif

#if		qWideCharacters != qSDK_UNICODE
/*
@METHOD:		Led_SDKString2tString
@DESCRIPTION:	<p></p>
*/
Led_tString	Led_SDKString2tString (const Led_SDK_String& s)
{
	#if		qWideCharacters && !qSDK_UNICODE
		size_t nChars	= s.length () + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_tChar>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_tString (result);
	#elif	!qWideCharacters && qSDK_UNICODE
		size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
		Led_SmallStackBuffer<Led_tChar>	result (nChars);
		CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
		return Led_tString (result);
	#else
		#error	"Hmm"
	#endif
}
#endif

#if		qWideCharacters
/*
@METHOD:		Led_ANSIString2tString
@DESCRIPTION:	<p></p>
*/
Led_tString	Led_ANSIString2tString (const string& s)
{
	size_t nChars	= s.length () + 1;	// convert null byte, too
	Led_SmallStackBuffer<Led_tChar>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapToUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return Led_tString (result);
}
#endif

#if		qWideCharacters
/*
@METHOD:		Led_tString2ANSIString
@DESCRIPTION:	<p></p>
*/
string	Led_tString2ANSIString (const Led_tString& s)
{
	size_t nChars	= s.length () * sizeof (wchar_t) + 1;	// convert null byte, too
	Led_SmallStackBuffer<char>	result (nChars);
	CodePageConverter (GetDefaultSDKCodePage ()).MapFromUNICODE (s.c_str (), s.length () + 1, result, &nChars);
	return string (result);
}
#endif



/*
@METHOD:		Led_BeepNotify
@DESCRIPTION:	<p>Make an audible beep on the users terminal. Used as a simple warning
			mechanism (like for typing bad characters).</p>
				<p>NB: For X-Windows only, this function invokes the private gBeepNotifyCallBackProc
			callback procedure to handle the beeping.
			This is because with X-Windows, the beep callback requires data (XDisplay) we don't have at this level.</p>
*/
void	Led_BeepNotify ()
{
	#if		qMacOS
		::SysBeep (1);
	#elif	qWindows
		::MessageBeep (MB_OK);
	#elif	qXWindows
		if (gBeepNotifyCallBackProc != NULL) {
			(gBeepNotifyCallBackProc) ();
		}
	#endif
}


#if		qXWindows
	static	unsigned long	sLastXWindowsEventTime;
	static	double			sLastEventReferenceTime;
	static	double	GetThisMachineCurTime ()
		{
			struct	timeval		tv;
			memset (&tv, 0, sizeof (tv));
			Led_Verify (::gettimeofday (&tv, NULL) == 0);
			Led_Assert (tv.tv_usec < 1000000);
			double	t	=	static_cast<double> (tv.tv_sec) + (tv.tv_usec / 1000000.0);
			return t;
		}
#endif

/*
@METHOD:		Led_GetTickCount
@DESCRIPTION:	<p>Get the number of seconds since some constant, system-specified reference time. This is used
			to tell how much time has elapsed since a particular event.</p>
				<p>Note - though this is based on the same reference time as an time values packed into event records,
			it maybe differently normalized. These times are all in seconds, whereas event records are often in
			other units (ticks - 1/60 of a second, or milliseconds).</p>
				<p>In the case of X-Windows - this business is very complicated because there are two different times
			one might be intersted in. There is the time on the client (where Led is running) and the time on the X-Server
			(users computer screen). Alas - X11R4 appears to have quite weak support for time - and offers no way I've found
			to accurately get the time from the users computer. As a result - with X-Windows - you must arrange to call
			@'SyncronizeLedXTickCount' for each event that specifies a time value (as soon as that event arrives). This
			data - together with time values from the client (where Led is running) computer will be used to provide a
			better approximation of the true elapsed time.</p>
*/
float	Led_GetTickCount ()
{
	#if		qMacOS
		return (float (::TickCount ()) / 60.0f);
	#elif	qWindows
		return (float (::GetTickCount ()) / 1000.0f);
	#elif	qXWindows
		static	float		sLastTickCountReturned;				// hack so we don't time-warp (out of sync between X client and could
																// cause time to go backwards cuz of our computations without this hack
		float	timeOfLastEvent	=	float (sLastXWindowsEventTime) / 1000.0f;
		double	curTime			=	GetThisMachineCurTime ();
		if (sLastEventReferenceTime == 0) {
			sLastEventReferenceTime = curTime;
		}
		double	resultTime		=	(timeOfLastEvent + (curTime - sLastEventReferenceTime));

		// avoid timewarp
		resultTime = Led_Max (resultTime, sLastTickCountReturned);
		sLastTickCountReturned = resultTime;
		return resultTime;
	#endif
}


#if		qXWindows
/*
@METHOD:		gBeepNotifyCallBackProc
@DESCRIPTION:	<p>X-Windows specific magic. See @'Led_BeepNotify'.</p>
*/
void	(*gBeepNotifyCallBackProc) ()	=	NULL;

/*
@METHOD:		SyncronizeLedXTickCount
@DESCRIPTION:	<p>X-Windows specific magic. See @'Led_GetTickCount'.</p>
*/
void	SyncronizeLedXTickCount (unsigned long xTickCount)
{
	sLastEventReferenceTime = GetThisMachineCurTime ();
	sLastXWindowsEventTime = xTickCount;
}

/*
@METHOD:		LedTickCount2XTime
@DESCRIPTION:	<p>X-Windows specific. See also @'SyncronizeLedXTickCount' and @'Led_GetTickCount'. Maps Led_GetTickCount ()
			result to the sort of time value you can stick into an XEvent record.</p>
*/
unsigned long	LedTickCount2XTime (float ledTickCount)
{
	return static_cast<unsigned long> (ledTickCount * 1000.0f);
}
#endif




/*
@METHOD:		Led_GetDoubleClickTime
@DESCRIPTION:	<p>Returns the amount of time (in seconds) between clicks which the OS deems should be interpretted
			as a double click.</p>
*/
float	Led_GetDoubleClickTime ()
{
	#if		qMacOS
		return (float (::GetDblTime ()) / 60.0f);
	#elif	qWindows
		return (float (::GetDoubleClickTime ()) / 1000.0f);
	#elif	qXWindows
		return 0.25f;	// SAME AS DOUBLE_CLICK_TIME FROM gdkevents.c
	#endif
}






#if		qDebug
static	void	(*sLedAssertFailedCallback) (const char* fileName, int lineNum)		=	NULL;

void	(*Led_GetAssertionHandler ()) (const char* fileName, int lineNum)
{
	return sLedAssertFailedCallback;
}

void	Led_SetAssertionHandler (void (*assertionHandler) (const char* fileName, int lineNum))
{
	sLedAssertFailedCallback = assertionHandler;
}

void	_Led_Debug_Trap_ (const char* fileName, int lineNum)
{
	if (sLedAssertFailedCallback == NULL) {
		#if		qMacOS
			Debugger ();
		#elif	qWindows
			DebugBreak ();
		#else
			abort ();
		#endif
	}
	else {
		(sLedAssertFailedCallback) (fileName, lineNum);
	}
}
#endif





/*
 ********************************************************************************
 ************************************ SPrintF ***********************************
 ********************************************************************************
 */

/*
@METHOD:		SPrintF
@DESCRIPTION:	<p>An @'Led_SDK_String' version of @'std::sprintf'. A slightly more convenient form
			of this function is the @'Format' function.
			</p>
*/
void	SPrintF (Led_SDK_Char* buffer, const Led_SDK_Char* format, ...)
{
	va_list								argsList;
	va_start (argsList, format);

	#if		qSDK_UNICODE
		(void)::vswprintf (buffer, format, argsList);
	#else
		(void)::vsprintf (buffer, format, argsList);
	#endif
	va_end (argsList);
}




/*
 ********************************************************************************
 ************************************* Format ***********************************
 ********************************************************************************
 */

/*
@METHOD:		Format
@DESCRIPTION:	<p>A simple wrapper on @'SPrintF' - but this function has a fixed size (10K Led_SDK_Char)
			buffer. Use @'SPrintF' directly if a larger buffer is required.</p>
*/
Led_SDK_String	Format (const Led_SDK_Char* format, ...)
{
	Led_SDK_Char				msgBuf [10*1024];	// no idea how big to make it...
	va_list						argsList;
	va_start (argsList, format);

	#if		qSDK_UNICODE
		(void)::vswprintf (msgBuf, format, argsList);
	#else
		(void)::vsprintf (msgBuf, format, argsList);
	#endif
	va_end (argsList);
	return msgBuf;
}






/*
 ********************************************************************************
 ******************************** EmitTraceMessage ******************************
 ********************************************************************************
 */
/*
@METHOD:		EmitTraceMessage
@DESCRIPTION:	<p>This function takes a 'format' argument and then any number of additional arguments - exactly
			like std::printf (). It calls std::vsprintf () internally. This can be called directly - regardless of the 
			 @'qDefaultTracingOn' flag - but is typically just called indirectly by calling
			 @'LedDebugTrace'.</p>
*/
void	EmitTraceMessage (const Led_SDK_Char* format, ...)
{
	Led_SmallStackBuffer<Led_SDK_Char>	msgBuf (10*1024);	// no idea how big to make it...
	va_list								argsList;
	va_start (argsList, format);

	#if		qSDK_UNICODE
		(void)::vswprintf (msgBuf, format, argsList);
	#else
		(void)::vsprintf (msgBuf, format, argsList);
	#endif
	#if		qWindows
		size_t	len	=	::_tcslen (msgBuf);
		if (msgBuf[len-1] != '\r' and msgBuf[len-1] != '\n') {
			(void)::_tcscat (msgBuf, Led_SDK_TCHAROF ("\r\n"));
		}
		::OutputDebugString (msgBuf);
	#else
		// SB pretty easy - but no need right now... LGP 2002-12-19
		Led_Assert (false);	// NYI
	#endif
	va_end (argsList);
}






/*
 ********************************************************************************
 *********************** Led_ThrowOutOfMemoryException **************************
 ********************************************************************************
 */
static	void	(*sLedOutOfMemoryExceptionCallback) ()		=	NULL;

void	Led_ThrowOutOfMemoryException ()
{
	if (sLedOutOfMemoryExceptionCallback == NULL) {
		throw std::bad_alloc ();
	}
	else {
		(sLedOutOfMemoryExceptionCallback) ();
	}
	Led_Assert (false);	// should never get here!
}

void	(*Led_Get_OutOfMemoryException_Handler ()) ()
{
	return sLedOutOfMemoryExceptionCallback;
}

void	Led_Set_OutOfMemoryException_Handler (void (*outOfMemoryHandler) ())
{
	sLedOutOfMemoryExceptionCallback = outOfMemoryHandler;
}







/*
 ********************************************************************************
 *********************** Led_ThrowBadFormatDataException ************************
 ********************************************************************************
 */
static	void	(*sLedBadFormatDataExceptionCallback) ()		=	NULL;

void	Led_ThrowBadFormatDataException ()
{
	if (sLedBadFormatDataExceptionCallback == NULL) {
		// not sure what this should throw by default?
		Led_ThrowOutOfMemoryException ();
	}
	else {
		(sLedBadFormatDataExceptionCallback) ();
	}
	Led_Assert (false);	// should never get here!
}

void	(*Led_Get_BadFormatDataException_Handler ()) ()
{
	return sLedBadFormatDataExceptionCallback;
}

void	Led_Set_BadFormatDataException_Handler (void (*badFormatDataExceptionCallback) ())
{
	sLedBadFormatDataExceptionCallback = badFormatDataExceptionCallback;
}




#if		qMacOS
/*
 ********************************************************************************
 ***************************** Led_ThrowOSErr ***********************************
 ********************************************************************************
 */
static	void	(*sLedThrowOSErrExceptionCallback) (OSErr)		=	NULL;

void	Led_ThrowOSErr (OSErr err)
{
	if (err != noErr) {
		if (sLedThrowOSErrExceptionCallback == NULL) {
			throw err;
		}
		else {
			(sLedThrowOSErrExceptionCallback) (err);
		}
		Led_Assert (false);	// should never get here!
	}
}

void	(*Led_Get_ThrowOSErrException_Handler ()) (OSErr err)
{
	return sLedThrowOSErrExceptionCallback;
}

void	Led_Set_ThrowOSErrException_Handler (void (*throwOSErrExceptionCallback) (OSErr err))
{
	sLedThrowOSErrExceptionCallback = throwOSErrExceptionCallback;
}
#endif








#if		qAllowBlockAllocation
	void*	Led_Block_Alloced_sizeof_4_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_8_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_12_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_16_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_20_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_24_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_28_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_32_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_36_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_40_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_44_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_48_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_52_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_56_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_60_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_64_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_68_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_72_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_76_sNextLink	=	NULL;
	void*	Led_Block_Alloced_sizeof_80_sNextLink	=	NULL;
#endif



#if		qNoSTRNICMP
int	Led_tStrniCmp (const Led_tChar* lhs, const Led_tChar* rhs, size_t n)
{
	Led_RequireNotNil (lhs);
	Led_RequireNotNil (rhs);
	#if		qSingleByteCharacters || qWideCharacters
		if (n) {
			int f;
			int l;
			do {
				#if		qSingleByteCharacters
					if ( ((f = (unsigned short)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
						f -= 'A' - 'a';
					}
					if ( ((l = (unsigned short)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
						l -= 'A' - 'a';
					}
				#elif	qWideCharacters
					if ( ((f = (unsigned char)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
						f -= 'A' - 'a';
					}
					if ( ((l = (unsigned char)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
						l -= 'A' - 'a';
					}
				#endif
			}
			while ( --n && f && (f == l) );
			return (f - l);
		}
		return (0);		
	#else
		Led_Assert (false);		//NYI
	#endif	
}

int	Led_tStriCmp (const Led_tChar* lhs, const Led_tChar* rhs)
{
	Led_RequireNotNil (lhs);
	Led_RequireNotNil (rhs);
	#if		qSingleByteCharacters || qWideCharacters
		int f;
		int l;
		do {
			#if		qSingleByteCharacters
				if ( ((f = (unsigned short)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
					f -= 'A' - 'a';
				}
				if ( ((l = (unsigned short)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
					l -= 'A' - 'a';
				}
			#elif	qWideCharacters
				if ( ((f = (unsigned char)(*(lhs++))) >= 'A') && (f <= 'Z') ) {
					f -= 'A' - 'a';
				}
				if ( ((l = (unsigned char)(*(rhs++))) >= 'A') && (l <= 'Z') ) {
					l -= 'A' - 'a';
				}
			#endif
		}
		while (f && (f == l) );
		return (f - l);
	#else
		Led_Assert (false);		//NYI
	#endif	
}
#endif



size_t	Led_NativeToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize)
{
	Led_tChar*	outPtr	=	outBuf;
	for (size_t i = 1; i <= srcTextBytes; i++) {
		#if		qMacOS
			Led_tChar	c	 = (srcText[i-1] == '\r')? '\n': srcText[i-1];
		#elif	qWindows
			Led_tChar	c	 = srcText[i-1];
			if (c == '\r') {
				// peek at next character - and if we have a CRLF sequence - then advance pointer
				// (so we skip next NL) and pretend this was an NL..
				// NB: we DONT map plain CR to NL - to get that to happen - use Led_NormalizeTextToNL()
				if (i < srcTextBytes and srcText[i] == '\n') {
					c = '\n';
					i++;
				}
			}
		#elif	qXWindows
			Led_tChar	c	 = srcText[i-1];
		#endif
		*outPtr++ = c;
	}
	size_t	nBytes	=	outPtr - outBuf;
	Led_Assert (nBytes <= outBufSize);
	Led_Arg_Unused (outBufSize);
	return (nBytes);
}

size_t	Led_NLToNative (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize)
{
	Led_Require (srcText != outBuf);	// though we support this for the others - its too hard
										// in this case for the PC...
	Led_tChar*	outPtr	=	outBuf;
	for (size_t i = 1; i <= srcTextBytes; i++) {
		Led_Assert (outPtr < outBuf+outBufSize);
		#if		qMacOS
			Led_tChar	c	 = (srcText[i-1] == '\n')? '\r': srcText[i-1];
		#elif	qWindows
			Led_tChar	c	 = srcText[i-1];
			if (c == '\n') {
				*outPtr++ = '\r';
			}
		#elif	qXWindows
			Led_tChar	c	 = srcText[i-1];
		#endif
		*outPtr++ = c;
	}
	size_t	nBytes	=	outPtr - outBuf;
	Led_Assert (nBytes <= outBufSize);
	Led_Arg_Unused (outBufSize);
	return (nBytes);
}

// return #bytes in output buffer (NO NULL TERM) - assert buffer big enough - output buf as big is input buf
// always big enough!!!
size_t	Led_NormalizeTextToNL (const Led_tChar* srcText, size_t srcTextBytes, Led_tChar* outBuf, size_t outBufSize)
{
	// NB: We DO Support the case where srcText == outBuf!!!!
	Led_tChar*	outPtr	=	outBuf;
	for (size_t i = 0; i < srcTextBytes; i++) {
		Led_Assert (outPtr < outBuf+outBufSize);
		Led_tChar	c	=	srcText[i];
		if (c == '\r') {
			// peek at next character - and if we have a CRLF sequence - then advance pointer
			// (so we skip next NL) and pretend this was an NL..
			if (i+1 < srcTextBytes and srcText[i+1] == '\n') {
				i++;
			}
			c = '\n';
		}
		*outPtr++ = c;
	}
	size_t	nBytes	=	outPtr - outBuf;
	Led_Assert (nBytes <= outBufSize);
	Led_Arg_Unused (outBufSize);
	return (nBytes);
}

/*
@METHOD:		Led_SkrunchOutSpecialChars
@DESCRIPTION:	<p>This function is useful to remove embedded NUL-characters from a string, or
	any other such undesirable characters. Returns NEW length of string (after removals).
	Removes in place.</p>
*/
size_t	Led_SkrunchOutSpecialChars (Led_tChar* text, size_t textLen, Led_tChar charToRemove)
{
	size_t		charsSkipped	=	0;
	Led_tChar*	end				=	text + textLen;
	for (Led_tChar* p = text; p+charsSkipped < end; ) {
		if (*(p + charsSkipped) == charToRemove) {
			charsSkipped++;
			continue;
		}
		*p = *(p + charsSkipped);
		++p;
	}
	return textLen-charsSkipped;
}



#if		qMultiByteCharacters
bool	Led_IsValidMultiByteString (const Led_tChar* start, size_t len)
{
	Led_AssertNotNil (start);
	const	Led_tChar*	end	=	&start[len];
	const Led_tChar* cur = start;
	for (; cur < end; cur = Led_NextChar (cur)) {
		if (Led_IsLeadByte (*cur)) {
			if (cur+1 == end) {
				return false;				// string ends on LedByte!!!
			}
			if (not Led_IsValidSecondByte (*(cur + 1))) {
				return false;				// byte second byte after lead-byte
			}
		}
		else if (not Led_IsValidSingleByte (*cur)) {
			return false;
		}
	}
	return (cur == end);	// if it were less - we'd be in the loop, and if it were
							// more - we'd have ended on a lead-byte
}
#endif









/*
 ********************************************************************************
 ************************** Led_ClipboardObjectAcquire **************************
 ********************************************************************************
 */

Led_ClipboardObjectAcquire::Led_ClipboardObjectAcquire (Led_ClipFormat clipType):
	#if		qMacOS || qWindows
	fOSClipHandle (NULL),
	#endif
	fLockedData (NULL)
{
	#if		qMacOS
		#if		TARGET_CARBON
			ScrapRef			scrap	=	NULL;
			Led_ThrowIfOSStatus (::GetCurrentScrap (&scrap));
			SInt32		byteCount	=	0;
			OSStatus	status		=	::GetScrapFlavorSize (scrap, clipType, &byteCount);
			if (status != noTypeErr) {
				fOSClipHandle			=	Led_DoNewHandle (byteCount);
				Led_ThrowIfNull (fOSClipHandle);
				Led_Assert (::GetHandleSize (fOSClipHandle) == byteCount);
				::HLock (fOSClipHandle);
				fLockedData = *fOSClipHandle;
				Led_ThrowIfOSStatus (::GetScrapFlavorData (scrap, clipType, &byteCount, fLockedData));	
			}
		#else
			long	scrapOffset	=	0;
			fOSClipHandle			=	Led_DoNewHandle (0);
			Led_ThrowIfNull (fOSClipHandle);
			long	result		=	::GetScrap (fOSClipHandle, clipType, &scrapOffset);
			if (result < 0) {
				::DisposeHandle (fOSClipHandle);
				fOSClipHandle = NULL;
				return;
			}
			Led_Assert (::GetHandleSize (fOSClipHandle) == result);
			::HLock (fOSClipHandle);
			fLockedData = *fOSClipHandle;
		#endif
	#elif	qWindows
		// perhaps rewrite to use exceptions, but for now - when no cliptype avail - set flag so GoodClip() method can check -
		// just cuz thats what surounding code seems to expect - LGP 980617
		fOSClipHandle	=	::GetClipboardData (clipType);
		#if		0
			DWORD	errResult	=	::GetLastError ();	// could be helpful for debugging if above fails... - Maybe I should do ThrowIfErrorHRESULT().... - LGP 2000/04/26
		#endif
		if (fOSClipHandle != NULL) {
			fLockedData = ::GlobalLock (fOSClipHandle);
		}
	#endif
}






#if		qWindows
/*
 ********************************************************************************
 **************************** VariantArrayPacker ********************************
 ********************************************************************************
 */
VariantArrayPacker::VariantArrayPacker (VARIANT* v, VARTYPE vt, size_t nElts):
	fSafeArrayVariant (v),
	fPtr (NULL)
{
	Led_RequireNotNil (v);
	// assumes passed in variant is CONSTRUCTED (initied) - but not necesarily having the right type
	::VariantClear (fSafeArrayVariant);
	fSafeArrayVariant->parray = ::SafeArrayCreateVector (vt, 0, static_cast<ULONG> (nElts));
	Led_ThrowIfNull (fSafeArrayVariant->parray);
	fSafeArrayVariant->vt = VT_ARRAY | vt;
	Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArrayVariant->parray, &fPtr));
}

VariantArrayPacker::~VariantArrayPacker ()
{
	Led_AssertNotNil (fSafeArrayVariant);
	Led_AssertNotNil (fSafeArrayVariant->parray);
	::SafeArrayUnaccessData (fSafeArrayVariant->parray);
}

void*	VariantArrayPacker::PokeAtData () const
{
	return fPtr;
}






/*
 ********************************************************************************
 ***************************** VariantArrayUnpacker *****************************
 ********************************************************************************
 */
VariantArrayUnpacker::VariantArrayUnpacker (const VARIANT& v):
	fSafeArray (v.parray)
{
	if (not (v.vt & VT_ARRAY) or fSafeArray == NULL) {
		throw E_INVALIDARG;
	}
	if ((v.vt & ~VT_ARRAY) != GetArrayElementType ()) {
		// I THINK this is right??? - LGP 2003-06-12 - but I'm not sure this is much reason to check/??
		throw E_INVALIDARG;
	}
	void*	useP	=	NULL;
	Led_ThrowIfErrorHRESULT (::SafeArrayAccessData (fSafeArray, &useP));
	fPtr = useP;
}

VariantArrayUnpacker::~VariantArrayUnpacker ()
{
	Led_AssertNotNil (fSafeArray);
	::SafeArrayUnaccessData (fSafeArray);
}

const void*		VariantArrayUnpacker::PeekAtData () const
{
	return fPtr;
}

VARTYPE		VariantArrayUnpacker::GetArrayElementType () const
{
	Led_AssertNotNil (fSafeArray);
	VARTYPE	vt	=	VT_EMPTY;
	Led_ThrowIfErrorHRESULT (::SafeArrayGetVartype (fSafeArray, &vt));
	return vt;
}

size_t		VariantArrayUnpacker::GetLength () const
{
	Led_AssertNotNil (fSafeArray);
	long	lb	=	0;
	long	ub	=	0;
	Led_ThrowIfErrorHRESULT (::SafeArrayGetLBound (fSafeArray, 1, &lb));
	Led_ThrowIfErrorHRESULT (::SafeArrayGetUBound (fSafeArray, 1, &ub));
	return ub-lb+1;
}
#endif









#if		qWindows
/*
 ********************************************************************************
 **************************** CreateSafeArrayOfBSTR *****************************
 ********************************************************************************
 */

/*
@METHOD:		CreateSafeArrayOfBSTR
@DESCRIPTION:	<p>Overloaded method to create a VARIANT record containing a safe-array of BSTRs.</p>
*/
VARIANT	CreateSafeArrayOfBSTR (const wchar_t*const* strsStart, const wchar_t*const* strsEnd)
{
	size_t	nElts	=	strsEnd - strsStart;
	VARIANT	result;
	::VariantInit (&result);
	try {
		const	bool	kEncodeAsVariant	=	false;	// neither seems to work with javascript test code in MSIE, but both work
														// with my C++-based unpacking code...
		if (kEncodeAsVariant) {
			VariantArrayPacker	packer (&result, VT_VARIANT, nElts);
			VARIANT*	vptr	=	reinterpret_cast<VARIANT*> (packer.PokeAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				::VariantInit (&vptr[i]);
				Led_ThrowIfNull (vptr[i].bstrVal = ::SysAllocString (strsStart[i]));
				vptr[i].vt = VT_VARIANT;
			}
		}
		else {
			VariantArrayPacker	packer (&result, VT_BSTR, nElts);
			BSTR*	bptr	=	reinterpret_cast<BSTR*> (packer.PokeAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				Led_ThrowIfNull (bptr[i] = ::SysAllocString (strsStart[i]));
			}
		}
	}
	catch (...) {
		::VariantClear (&result);
		throw;
	}
	return result;
}

VARIANT	CreateSafeArrayOfBSTR (const vector<const wchar_t*>& v)
{
	return CreateSafeArrayOfBSTR (&*v.begin (), &*v.end ());
}

VARIANT	CreateSafeArrayOfBSTR (const vector<wstring>& v)
{
	vector<const wchar_t*>	tmp;
	for (vector<wstring>::const_iterator i = v.begin (); i != v.end (); ++i) {
		// I hope (assure?) this doesn't create temporaries and take .c_str () of temporaries
		tmp.push_back ((*i).c_str ());
	}
	return CreateSafeArrayOfBSTR (tmp);
}



/*
 ********************************************************************************
 ***************** UnpackVectorOfStringsFromVariantArray ************************
 ********************************************************************************
 */
vector<wstring>	UnpackVectorOfStringsFromVariantArray (const VARIANT& v)
{
	vector<wstring>			result;
	VariantArrayUnpacker	up (v);
	size_t					nElts	=	up.GetLength ();
	switch (up.GetArrayElementType ()) {
		case	VT_BSTR: {
			const	BSTR*	bArray	=	reinterpret_cast<const BSTR*> (up.PeekAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				result.push_back (bArray[i]);
			}
		}
		break;
		case	VT_VARIANT: {
			const	VARIANT*	vArray	=	reinterpret_cast<const VARIANT*> (up.PeekAtData ());
			for (size_t i = 0; i < nElts; ++i) {
				result.push_back (vArray[i].bstrVal);
			}
		}
		break;
		default:	throw (DISP_E_BADVARTYPE);
	}
	return result;
}
#endif


#if		qWindows
/*
@METHOD:		DumpSupportedInterfaces
@DESCRIPTION:	<p>@'qWindows' only</p>
				<p>Helpful COM debugging utility which dumps to the debugger window all the interfaces
			supported by a given COM object. The arguments 'objectName' and 'levelPrefix' can be NULL (optional).
			</p>
*/
void	DumpSupportedInterfaces (IUnknown* obj, const char* objectName, const char* levelPrefix)
{
	{
		size_t						labelLen	=	((objectName == NULL)? 0: ::strlen (objectName)) + 100;
		Led_SmallStackBuffer<char>	labelBuf (labelLen);
		if (objectName == NULL) {
			labelBuf[0] = '\0';
		}
		else {
			(void)::sprintf (labelBuf, " (named '%s')", objectName);
		}
		Led_SmallStackBuffer<char>	msgBuf (labelLen + 1000);
		(void)::sprintf (msgBuf, "Dumping interfaces for object%s at 0x%x:\n", static_cast<char*> (labelBuf), reinterpret_cast<int> (obj));
		::OutputDebugStringA (msgBuf);

		if (levelPrefix == NULL) {
			levelPrefix = "\t";
		}
	}

	/*
	 *	All interfaces are stored in the registry. Lookup each one, and do a query-interface
	 *	with each on the given object to see what interfaces it contains.
	 */
	HKEY		interfaceKey	=	NULL;
	HKEY		iKey			=	NULL;
	BSTR		tmpStr			=	NULL;
	IUnknown*	test			=	NULL;
	try {
		if (::RegOpenKeyEx (HKEY_CLASSES_ROOT, _T ("Interface"), 0, KEY_READ, &interfaceKey) != ERROR_SUCCESS) {
			throw GetLastError ();
		}
		for (DWORD i = 0; ; ++i) {
			char	subKey[MAX_PATH];
			DWORD	subKeySize	=	sizeof (subKey);
			if (::RegEnumKeyExA (interfaceKey, i, subKey, &subKeySize, 0, 0, 0, 0) == ERROR_SUCCESS) {
				try {
					CLSID	iid;
					(void)::memset (&iid, 0, sizeof (iid));
					tmpStr = ::SysAllocString (ACP2WideString (subKey).c_str ());
					Led_ThrowIfErrorHRESULT (::CLSIDFromString (tmpStr, &iid));
					if (tmpStr != NULL)			{	::SysFreeString (tmpStr);		tmpStr = NULL;	}
					if (SUCCEEDED (obj->QueryInterface (iid, reinterpret_cast<void**> (&test)))) {
						// dump that obj interface
						Led_Assert (iKey == NULL);
						if (::RegOpenKeyExA (interfaceKey, subKey, 0, KEY_READ, &iKey) != ERROR_SUCCESS) {
							throw GetLastError ();
						}
						DWORD	dwType = NULL;
						TCHAR	interfaceNameBuf[1024];
						DWORD	interfaceNameBufSize	=	sizeof (interfaceNameBuf);
						if (::RegQueryValueEx (iKey, _T(""), NULL, &dwType, reinterpret_cast<LPBYTE> (interfaceNameBuf), &interfaceNameBufSize) == ERROR_SUCCESS) {
							OutputDebugStringA (levelPrefix);
							OutputDebugString (interfaceNameBuf);
							OutputDebugStringA ("\n");
						}
						if (iKey != NULL)			{	::CloseHandle (iKey);			iKey = NULL;			}
					}
					if (test != NULL)	{		test->Release ();		test = NULL; }
				}
				catch (...) {
					OutputDebugStringA (levelPrefix);
					OutputDebugStringA ("ERROR LOOKING UP INTERFACE - IGNORING");
					OutputDebugStringA ("\n");
				}
			}
			else {
				break;	// end of list of interfaces?
			}
		}
		if (test != NULL)			{	test->Release ();				test = NULL;			}
		if (tmpStr != NULL)			{	::SysFreeString (tmpStr);		tmpStr = NULL;			}
		if (interfaceKey != NULL)	{	::CloseHandle (interfaceKey);	interfaceKey = NULL;	}
	}
	catch (...) {
		if (iKey != NULL)			{	::CloseHandle (iKey);			iKey = NULL;			}
		if (interfaceKey != NULL)	{	::CloseHandle (interfaceKey);	interfaceKey = NULL;	}
		if (tmpStr != NULL)			{	::SysFreeString (tmpStr);		tmpStr = NULL;			}		
		if (test != NULL)			{	test->Release ();				test = NULL;			}
	}
}

/*
@METHOD:		DumpObjectsInIterator
@DESCRIPTION:	<p>@'qWindows' only</p>
				<p>Helpful COM debugging utility which dumps to the debugger window all the subobjects of a given COM object,
			along with the interfaces they supoport (see also @'DumpSupportedInterfaces'). The arguments
			'iteratorName' and 'levelPrefix' can be NULL (optional).
			</p>
*/
void	DumpObjectsInIterator (IEnumUnknown* iter, const char* iteratorName, const char* levelPrefix)
{
	{
		size_t						labelLen	=	((iteratorName == NULL)? 0: ::strlen (iteratorName)) + 100;
		Led_SmallStackBuffer<char>	labelBuf (labelLen);
		if (iteratorName == NULL) {
			labelBuf[0] = '\0';
		}
		else {
			(void)::sprintf (labelBuf, " (named '%s')", iteratorName);
		}
		Led_SmallStackBuffer<char>	msgBuf (labelLen + 1000);
		(void)::sprintf (msgBuf, "Dumping objects (and their interface names) for iterator%s at 0x%x\n", static_cast<char*> (labelBuf), reinterpret_cast<int> (iter));
		::OutputDebugStringA (msgBuf);

		if (levelPrefix == NULL) {
			levelPrefix = "\t";
		}
	}
	if (iter == NULL) {
		::OutputDebugStringA (levelPrefix);
		::OutputDebugStringA ("<<NULL ENUM POINTER>>\n");	// try DumpSupportedInterfaces to see what interfaces your enum interface supports!
		return;
	}
	IUnknown*	nextObj	=	NULL;
	for (size_t i = 0; SUCCEEDED (iter->Next (1, &nextObj, NULL)); ++i) {
		char	nameBuf[1024];
		(void)::sprintf (nameBuf, "obj#%d", i);
		char	levelPrefixBuf[1024];
		Led_Assert (::strlen (levelPrefix) < sizeof (levelPrefixBuf)/2);	// assert MUCH less
		strcpy (levelPrefixBuf, levelPrefix);
		strcat (levelPrefixBuf, "\t");
		DumpSupportedInterfaces (nextObj, nameBuf, levelPrefixBuf);
		Led_AssertNotNil (nextObj);
		nextObj->Release ();
		nextObj = NULL;
	}
	Led_Assert (nextObj == NULL);
}
#endif





/*
 ********************************************************************************
 *************************************** Led_URLD *******************************
 ********************************************************************************
 */
Led_URLD::Led_URLD (const char* url, const char* title):
	fData ()
{
	Led_RequireNotNil (url);
	Led_RequireNotNil (title);
	size_t	urlLen		=	::strlen (url);
	size_t	titleLen	=	::strlen (title);
	
	size_t	resultURLDLen = (urlLen + 1 + titleLen + 1);
	fData.resize (resultURLDLen, '\0');

	char*	data	=	&fData.front ();
	strcpy (data, url);
	strcat (data, "\r");
	strcat (data, title);
}

Led_URLD::Led_URLD (const void* urlData, size_t nBytes):
	fData ()
{
	// assure a validly formatted Led_URLD - regardless of input
	for (size_t i = 0; i < nBytes; i++) {
		char	c	=	((char*)urlData)[i];
		fData.push_back (c);
		if (c == '\0') {
			break;
		}
	}
	// make sure NUL-terminated...
	if (fData.size () == 0 or fData.back () != '\0') {
		fData.push_back ('\0');
	}
}

size_t	Led_URLD::GetURLDLength () const
{
	// Note we DONT count the terminating NUL - cuz Netscape 2.0 doesn't appear to. And thats our reference.
	return fData.size ()-1;
}

size_t	Led_URLD::GetURLLength () const
{
	size_t	len	=	fData.size ();
	for (size_t i = 0; i < len; i++) {
		if (fData[i] == '\r') {
			return i;
		}
	}
	return len-1;
}

size_t	Led_URLD::GetTitleLength () const
{
	size_t	len	=	fData.size ();
	for (size_t i = 0; i < len; i++) {
		if (fData[i] == '\r') {
			Led_Assert (len >= (i+1+1));	// cuz must be NUL-term & skip \r
			return len-(i+1+1);				// ditto
		}
	}
	return 0;
}

char*	Led_URLD::PeekAtURLD () const
{
	return (&const_cast<Led_URLD*>(this)->fData.front ());
}

char*	Led_URLD::PeekAtURL () const
{
	return (&const_cast<Led_URLD*>(this)->fData.front ());
}

char*	Led_URLD::PeekAtTitle () const
{
	return PeekAtURL () + GetURLLength () + 1;	// skip URL and '\r'
}

string	Led_URLD::GetTitle () const
{
	return string (PeekAtTitle (), GetTitleLength ());
}

string	Led_URLD::GetURL () const
{
	return string (PeekAtURL (), GetURLLength ());
}





/*
 ********************************************************************************
 ********************************* Led_URLManager *******************************
 ********************************************************************************
 */
#if		qUseSpyglassDDESDIToOpenURLs
DWORD	Led_URLManager::sDDEMLInstance	=	0;	//	The DDEML instance identifier.
#endif
Led_URLManager*	Led_URLManager::sThe	=	NULL;

Led_URLManager::Led_URLManager ()
{
}

Led_URLManager&	Led_URLManager::Get ()
{
	if (sThe == NULL) {
		sThe = new Led_URLManager ();
	}
	return *sThe;
}

void	Led_URLManager::Set (Led_URLManager* newURLMgr)
{
	if (sThe != newURLMgr) {
		delete sThe;
		sThe = newURLMgr;
	}
}

void	Led_URLManager::Open (const string& url)
{
	#if		qUseInternetConfig
		try {
			Open_IC (url);
		}
		catch (...) {
			Open_SpyglassAppleEvent (url);
		}
	#elif	qMacOS
		Open_SpyglassAppleEvent (url);
	#elif	qWindows
		#if		qUseActiveXToOpenURLs && qUseSpyglassDDESDIToOpenURLs
			try {
				Open_ActiveX (url);
			}
			catch (...) {
				Open_SpyglassDDE (url);
			}
		#else
			#if		qUseActiveXToOpenURLs
				Open_ActiveX (url);
			#elif	qUseSpyglassDDESDIToOpenURLs
				Open_SpyglassDDE (url);
			#endif
		#endif
	#endif
	#if		qUseSystemNetscapeOpenURLs
		Open_SystemNetscape (url);
	#endif
}

#if		qMacOS
string	Led_URLManager::FileSpecToURL (const FSSpec& fsp)
{
	short	len	=	0;
	Handle	h	=	NULL;
	Led_ThrowOSErr (FSpGetFullPath (&fsp, &len, &h));
	Led_AssertNotNil (h);
	const	char	kFilePrefix[]	=	"file:///";
	size_t			kFilePrefixLen	=	::strlen (kFilePrefix);
	char*	result	=	new char [kFilePrefixLen + len + 1];
	(void)::memcpy (result, kFilePrefix, kFilePrefixLen);
	(void)::memcpy (&result[kFilePrefixLen], *h, len);
	::DisposeHandle (h);
	result[kFilePrefixLen + len] = '\0';
	for (size_t i = 0; i < len; i++) {
		if (result[kFilePrefixLen + i] == ':') {
			result[kFilePrefixLen + i] = '/';
		}
	}
	string	r	=	result;
	delete[] result;

	// Now go through and quote illegal URL characters
	string	rr;
	for (size_t i = 0; i < r.length (); i++) {
		char	c	=	r[i];
		if (isalnum (c) or c == '/' or c == '.' or c == '_' or c == ':' or c == '#') {
			rr.append (&c, 1);
		}
		else {
			rr.append ("%");
			unsigned char	x	=	(((unsigned char)c) >> 4);
			Led_Assert (x <= 0xf);
			x = (x>9)? (x - 10 + 'A'): (x + '0');
			rr.append ((char*)&x, 1);
			x	=	((unsigned char)(c & 0xf));
			Led_Assert (x <= 0xf);
			x = (x>9)? (x - 10 + 'A'): (x + '0');
			rr.append ((char*)&x, 1);
		}
	}
	return rr;
}
#elif	qWindows
string	Led_URLManager::FileSpecToURL (const string& path)
{
	Led_Assert (false);	// nyi (not needed anywhere right now)
	return "";
}
#endif

#if		qUseInternetConfig
void	Led_URLManager::Open_IC (const string& url)
{
	// If we compile for internet config, first try that. If that fails, fall back on Netscape.
	Str255	hint		=	"\p";	// not sure what this is for. See how it works without it...
	OSType	signature	=	'\?\?\?\?';
	{
		ProcessSerialNumber myPSN;
		memset (&myPSN, 0, sizeof (myPSN));
		Led_ThrowOSErr (::GetCurrentProcess (&myPSN));
		ProcessInfoRec info;
		memset (&info, 0, sizeof (info));
		info.processInfoLength = sizeof(info);
		Led_ThrowOSErr (::GetProcessInformation (&myPSN, &info));
		signature = info.processSignature;
	}
	ICInstance	icInstance;
	Led_ThrowOSErr (::ICStart (&icInstance, signature));
	#if		qSDKHasInternetConfig
	//	OSErr	err	=	::ICGeneralFindConfigFile (icInstance, true, true, 0, NULL);
		OSErr	err	=	noErr;
	#else
		ICError	err = ::ICFindConfigFile (icInstance, 0, NULL);
	#endif
	if (err == noErr) {
		long	urlStart	=	0;
		long	urlEnd		=	0;
		// Unclear if/why url would be modified, but since they declare it as non-cost
		// better be sure...LGP 961028
		char	urlBuf[1024];
		strncpy (urlBuf, url.c_str (), sizeof (urlBuf));
		urlBuf[sizeof(urlBuf)-1] = '\0';
		err = ::ICLaunchURL (icInstance, hint, urlBuf, ::strlen (urlBuf), &urlStart, &urlEnd);
	}
	::ICStop (icInstance);
	Led_ThrowOSErr (err);
}
#endif

#if		qMacOS
void	Led_URLManager::Open_SpyglassAppleEvent (const string& url)
{
	const	OSType	AE_spy_receive_suite	=	'WWW!';
	const	OSType	AE_spy_openURL			=	'OURL';	// typeChar OpenURL
	ProcessSerialNumber	browserPSN	=	FindBrowser ();
	AppleEvent event;
	{
		AEAddressDesc progressApp;
		Led_ThrowOSErr (::AECreateDesc (typeProcessSerialNumber, &browserPSN, sizeof(browserPSN), &progressApp));
		try {
			Led_ThrowOSErr (::AECreateAppleEvent (AE_spy_receive_suite, AE_spy_openURL, &progressApp, kAutoGenerateReturnID, kAnyTransactionID, &event));
			::AEDisposeDesc (&progressApp);
		}
		catch (...) {
			::AEDisposeDesc (&progressApp);
			throw;
		}
	}
	try {
		Led_ThrowOSErr (::AEPutParamPtr (&event,keyDirectObject,typeChar, url.c_str (), url.length ()));
		AppleEvent reply;
		Led_ThrowOSErr (::AESend (&event, &reply, kAEWaitReply, kAENormalPriority, kAEDefaultTimeout, nil, nil));
		::AEDisposeDesc (&event);
		::AEDisposeDesc (&reply);
	}
	catch (...) {
		::AEDisposeDesc (&event);
		throw;
	}
}
#endif

#if		qUseActiveXToOpenURLs
void	Led_URLManager::Open_ActiveX (const string& url)
{
	Led_SmallStackBuffer<wchar_t>	wideURLBuf (url.length ()+1);
	{
		int nWideChars = ::MultiByteToWideChar (CP_ACP, 0, url.c_str (), url.length (), wideURLBuf, url.length ());
		wideURLBuf[nWideChars] = '\0';
	}
	IUnknown*	pUnk	=	NULL;		// implies we are an OLE-ignorant app. But I'm not sure what else I'm supposed to pass here!

	/*
	 *	Use LoadLibrary/GetProcAddress instead of direct call to avoid having to link with
	 *	urlmon.lib. This avoidance allows us to run on systems that don't have MSIE installed.
	 *
	 *	Personally, I think its pretty weak MS doesn't offer a simpler way to deal with this problem.
	 *	(or if they do, their docs are weak).
	 *		--LGP 961015
	 */
	#if		1
		static	HINSTANCE	urlmonLibrary	=	::LoadLibrary (_T ("urlmon"));
		if (urlmonLibrary == NULL) {
			Led_ThrowOutOfMemoryException ();	// not sure what else to throw?
		}
		static	 HRESULT	 (WINAPI *hlinkNavigateString) (IUnknown*,LPCWSTR)	=
			(HRESULT  (WINAPI *) (IUnknown*,LPCWSTR))::GetProcAddress (urlmonLibrary, "HlinkNavigateString");
		if (hlinkNavigateString == NULL) {
			Led_ThrowOutOfMemoryException ();	// not sure what else to throw?
		}
		HRESULT		result	=	(*hlinkNavigateString) (pUnk, wideURLBuf);
	#else
		HRESULT		result	=	::HlinkNavigateString (pUnk, wideURLBuf);
	#endif
	if (not SUCCEEDED (result)) {
		throw result;
	}
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
void	Led_URLManager::Open_SpyglassDDE (const string& url)
{
	/*
	 *	Use DDE to talk to any browser supporting the Spyglass DDE SDI Suite (right now I only know of
	 *	Netscape, ironicly) - LGP 960502
	 */
	
	Led_Require (sDDEMLInstance != 0);	//	Must call Led_URLManager::InitDDEHandler ()
										// (maybe we should throw? put up alert? LGP 960502)


	// WWW_OPENURL
	const	kDDETIMEOUT	=	2*1000;		// timeout in milliseconds
	{
		// Open Client connection
		HSZ		hszBrowser	=	NULL;	// connect to any browser
		HSZ		hszTopic	=	::DdeCreateStringHandle (sDDEMLInstance, _T ("WWW_OpenURL"), CP_WINANSI);
		HCONV	hConv		=	::DdeConnect (sDDEMLInstance, hszBrowser, hszTopic, NULL);
		::DdeFreeStringHandle (sDDEMLInstance, hszTopic);
	
		long	dwWindowID	=	-1;
		long	dwFlags		=	0;
		// Format DDE message
		HSZ	hszItem	=	ClientArguments ("QCS,QCS,DW,DW,QCS,QCS,CS", &url, NULL, &dwWindowID, &dwFlags, NULL, NULL, NULL);

		// Send DDE message
		//	Send along the item to the server, as an XTYP_REQUEST.
		//	Return the data received from the server.
		//	Proactively delete the hszItem string.
		HDDEDATA hRetVal = ::DdeClientTransaction (NULL, 0, hConv, hszItem, CF_TEXT, XTYP_REQUEST, kDDETIMEOUT, NULL);
		if(hszItem != NULL)	{
			::DdeFreeStringHandle (sDDEMLInstance, hszItem);
		}

		::DdeDisconnect (hConv);	
	}

	// WWW_ACTIVATE(activate the window/browser application)
	{
		// Open Client connection
		HSZ		hszBrowser	=	NULL;	// connect to any browser
		HSZ		hszTopic	=	::DdeCreateStringHandle (sDDEMLInstance, _T ("WWW_Activate"), CP_WINANSI);
		HCONV	hConv		=	::DdeConnect (sDDEMLInstance, hszBrowser, hszTopic, NULL);
		::DdeFreeStringHandle (sDDEMLInstance, hszTopic);

		long	dwWindowID	=	-1;
		long	dwFlags		=	0;
		// Format DDE message
		HSZ hszItem = ClientArguments ("DW,DW", &dwWindowID, &dwFlags);

		//	Send along the item to the server, as an XTYP_REQUEST.
		//	Return the data received from the server.
		//	Proactively delete the hszItem string.
		HDDEDATA hRetVal = DdeClientTransaction (NULL, 0, hConv, hszItem, CF_TEXT, XTYP_REQUEST, kDDETIMEOUT, NULL);
		if (hszItem != NULL)	{
			::DdeFreeStringHandle (sDDEMLInstance, hszItem);
		}

		::DdeDisconnect (hConv);		// Close connection
	}
}
#endif

#if		qUseSystemNetscapeOpenURLs
void	Led_URLManager::Open_SystemNetscape (const string& url)
{
	string	execString;
	/*
	 *	Code lifted (based on) code from AbiWord - xap_UnixFrame.cpp
	 */
	struct stat statbuf;
	memset (&statbuf, 0, sizeof (statbuf));
	//
	// The gnome-help-browser sucks right now. Instead open with netscape
	// or kde.
	// When it gets better we should restore this code.
	/*
	//if (!stat("/opt/gnome/bin/gnome-help-browser", statbuf) || !stat("/usr/local/bin/gnome-help-browser", statbuf) || !stat("/usr/bin/gnome-help-browser", statbuf))
	{
		execString = g_strdup_printf("gnome-help-browser %s &", szURL);
	}
	*/
#if 0
// My system has kdehelp - and I can exec it by itself - but passing args produces an error from KDE:
//		ERROR: KFM is not running
//		KFM not ready
	if (!stat("/opt/kde/bin/kdehelp", &statbuf) || !stat("/usr/local/kde/bin/kdehelp", &statbuf) || !stat("/usr/local/bin/kdehelp", &statbuf) || !stat("/usr/bin/kdehelp", &statbuf))
	{
		execString = "kdehelp " + url + "&";
	}
	else
#endif
	{
		// Try to connect to a running Netscape, if not, start new one
		Led_SmallStackBuffer<char> buf (url.length () * 2 + 1000);
		sprintf (buf, "netscape -remote openURL\\(\"%s\"\\) || netscape \"%s\" &", url.c_str (), url.c_str ());
		execString = buf;
	}
	system (execString.c_str ());
}
#endif

#if		qMacOS
pascal	OSErr	Led_URLManager::FSpGetFullPath (const FSSpec* spec, short* fullPathLength, Handle* fullPath)
{
	// Based on code from Apple Macintosh Developer Technical Support
	// "MoreFiles 1.4.2" example code
	OSErr		result;
	FSSpec		tempSpec;
	CInfoPBRec	pb;
	
	/* Make a copy of the input FSSpec that can be modified */
	BlockMoveData(spec, &tempSpec, sizeof(FSSpec));
	
	if ( tempSpec.parID == fsRtParID ) {
		/* The object is a volume */
		
		/* Add a colon to make it a full pathname */
		++tempSpec.name[0];
		tempSpec.name[tempSpec.name[0]] = ':';
		
		/* We're done */
		result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
	}
	else {
		/* The object isn't a volume */
		
		/* Put the object name in first */
		result = PtrToHand(&tempSpec.name[1], fullPath, tempSpec.name[0]);
		if ( result == noErr ) {
			/* Get the ancestor directory names */
			pb.dirInfo.ioNamePtr = tempSpec.name;
			pb.dirInfo.ioVRefNum = tempSpec.vRefNum;
			pb.dirInfo.ioDrParID = tempSpec.parID;
			do	/* loop until we have an error or find the root directory */
			{
				pb.dirInfo.ioFDirIndex = -1;
				pb.dirInfo.ioDrDirID = pb.dirInfo.ioDrParID;
				result = PBGetCatInfoSync(&pb);
				if ( result == noErr )
				{
					/* Append colon to directory name */
					++tempSpec.name[0];
					tempSpec.name[tempSpec.name[0]] = ':';
					
					/* Add directory name to beginning of fullPath */
					(void) Munger(*fullPath, 0, NULL, 0, &tempSpec.name[1], tempSpec.name[0]);
					result = MemError();
				}
			} while ( (result == noErr) && (pb.dirInfo.ioDrDirID != fsRtDirID) );
		}
	}
	if ( result == noErr ) {
		/* Return the length */
		*fullPathLength = GetHandleSize(*fullPath);
	}
	else {
		/* Dispose of the handle and return NULL and zero length */
		DisposeHandle(*fullPath);
		*fullPath = NULL;
		*fullPathLength = 0;
	}
	
	return ( result );
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
void	Led_URLManager::InitDDEHandler ()
{
	//	Initialize DDEML
	Led_Require (sDDEMLInstance == 0);	// only call once

	// callback not normally called, but seems to sometimes be called under error conditions, and we get crashes 
	// if no callback present (ie if I pass NULL to DDEInitialize()) - not when I call DDEInitialize(), but much
	// later - apparently when DDEML tries to deliver a message.
	if (::DdeInitialize (&sDDEMLInstance, SimpleDdeCallBack, APPCMD_CLIENTONLY, 0)) {
		sDDEMLInstance = 0;
	}
}
#endif

#if		qMacOS
ProcessSerialNumber	Led_URLManager::FindBrowser ()
{
	OSType	sig				=	'MOSS';	// currently we hardwire Netscape - but we could extend this list to include all known browsers...
	OSType	processType		=	'APPL';
	ProcessSerialNumber	psn;
	psn.highLongOfPSN = 0;
	psn.lowLongOfPSN  = kNoProcess;
	OSErr 			err;
	ProcessInfoRec	info;
	do {
		err= ::GetNextProcess (&psn);
		if ( err == noErr ) {
			info.processInfoLength = sizeof(ProcessInfoRec);
			info.processName = NULL;
			info.processAppSpec = NULL;
			err= ::GetProcessInformation (&psn, &info);
		}
	}
	while( (err == noErr) &&  ((info.processSignature != sig) ||  (info.processType != processType)));
	Led_ThrowOSErr (err);
	return info.processNumber;
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
HDDEDATA CALLBACK	Led_URLManager::SimpleDdeCallBack (UINT /*type*/, UINT /*fmt*/, HCONV /*hconv*/, HSZ /*hsz1*/, HSZ /*hsz2*/, HDDEDATA /*hData*/, DWORD /*dwData1*/, DWORD /*dwData2*/)
{
	return 0;
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
const char*	Led_URLManager::SkipToNextArgument (const char* pFormat)
{
	Led_RequireNotNil (pFormat);
	if (*pFormat == '\0')	{
		return (pFormat);
	}

	//	The next format is directly after a ','
	while (*pFormat != ',' && *pFormat != '\0')	{
		pFormat++;
	}
	if (*pFormat == ',')	{
		pFormat++;
	}
	return (pFormat);
}
#endif

#if		qUseSpyglassDDESDIToOpenURLs
HSZ	Led_URLManager::ClientArguments (const char* pFormat, ...)
{
	Led_RequireNotNil (pFormat);

	//	Always pass in pointer values.
	//	This way, a NULL, can be interpreted as an optional, missing parameter.
	const char*	pTraverse =	pFormat;	

	//	Variable number of arguments.
	va_list VarList;
	va_start(VarList, pFormat);
	char caNumpad[64];
	string	csBuffer;
	string	csRetval;
	while (*pTraverse != '\0')	{
		//	Erase temp data from our last pass.
		caNumpad[0] = '\0';
		csBuffer = string ();
		//	Compare our current format to the known formats
		if (0 == strncmp (pTraverse, "DW", 2))	{
			//	A DWORD.
			DWORD*	pWord = va_arg (VarList, DWORD *);
			if(pWord != NULL)	{
			    //  See if we need to use hex.
				sprintf (caNumpad, "%lu", *pWord);
				csRetval += caNumpad;
			}
		}
		else if (0 == strncmp (pTraverse, "CS", 2))	{
			//	A CString, not quoted
			string*	pCS = va_arg (VarList, string*);
			if (pCS != NULL)	{
				csRetval += *pCS;
			}
		}
		else if(0 == strncmp (pTraverse, "QCS", 3))	{
			//	A quoted CString
			string*	pQCS = va_arg (VarList, string*);
			if (pQCS != NULL)	{
				csRetval += '\"';
				//	Need to escape any '"' to '\"', literally.
				const char *pConvert = pQCS->c_str ();
				while(*pConvert != '\0')	{
					if(*pConvert == '\"')	{
						csRetval += '\\';
					}
					csRetval += *pConvert;
					pConvert++;
				}
				csRetval += '\"';
			}
		}
		else if (0 == strncmp (pTraverse, "BL", 2))	{
			//	If compiling in 32 bit mode, BOOLs change size.
			typedef short TwoByteBool;
			//	A boolean
			TwoByteBool *pBool = va_arg(VarList, TwoByteBool *);
			if (pBool != NULL)	{
				csRetval += *pBool? "TRUE": "FALSE";
			}
		}
		else	{
			//	Unhandled format, just get out of loop.
			Led_Assert (false);
			break;
		}

		//	Go on to next type
		pTraverse = SkipToNextArgument (pTraverse);

		//	See if we need a comma
		if (*pTraverse != '\0')	{
			csRetval += ',';
		}
	}

	va_end (VarList);

	//	Make sure we're atleast returning something.
	if (csRetval.empty ())	{
		return (NULL);
	}

	//	Return our resultant HSZ, created from our string.
	HSZ Final = ::DdeCreateStringHandleA (sDDEMLInstance, csRetval.c_str (), CP_WINANSI);
	return(Final);
}

char*	Led_URLManager::ExtractArgument (HSZ hszArgs, int iArg)
{
	//	Quoted strings are counted as only one argument, though
	//		the quotes are not copied into the return string.

	DWORD dwLength = ::DdeQueryString (sDDEMLInstance, hszArgs, NULL, 0L, CP_WINANSI) + 1;
	char *pTraverse = new char[dwLength];
	char *pRemove = pTraverse;
	::DdeQueryStringA (sDDEMLInstance, hszArgs, pTraverse, dwLength, CP_WINANSI);
	
	//	safety dance
	if(*pTraverse == '\0' || iArg < 1)	{
		delete(pRemove);
		return(NULL);
	}
	
	//	Need to decrement the argument we're looking for, as the very
	//		first argument has no ',' at the beginning.
	iArg--;
	
	//	Search through the arguments, seperated by ','.
	while(iArg)	{
		//	Special handling of quoted strings.
		if(*pTraverse == '\"')	{
			//	Find the ending quote.
			while(*pTraverse != '\0')	{
				pTraverse++;
				if(*pTraverse == '\"')	{
					pTraverse++;	//	One beyond, please
					break;
				}
				else if(*pTraverse == '\\')	{
					//	Attempting to embed a quoted, perhaps....
					if(*(pTraverse + 1) == '\"')	{
						pTraverse++;
					}
				}
			}
		}
		while(*pTraverse != '\0' && *pTraverse != ',')	{
			pTraverse++;
		}

		//	Go beyond a comma
		if(*pTraverse == ',')	{
			pTraverse++;
		}
		
		iArg--;
		
		if(*pTraverse == '\0')	{
			break;
		}
	}
	
	//	Handle empty arguments here.
	if(*pTraverse == ',' || *pTraverse == '\0')	{
		delete(pRemove);
		return(NULL);
	}
	
	int iLength = 1;
	char *pCounter = pTraverse;
	typedef short TwoByteBool;
	TwoByteBool bQuoted = FALSE;
	
	//	specially handle quoted strings
	if(*pCounter == '\"')	{
		pCounter++;
		bQuoted = TRUE;

		while(*pCounter != '\0')	{
			if(*pCounter == '\"')	{
				break;
			}
			else if(*pCounter == '\\')	{
				if(*(pCounter + 1) == '\"')	{
					pCounter++;
					iLength++;
				}
			}
			
			pCounter++;
			iLength++;
		}
	}
	while(*pCounter != '\0' && *pCounter != ',')	{
		iLength++;
		pCounter++;
	}
	
	//	Subtrace one to ignore ending quote if we were quoted....
	if(bQuoted == TRUE)	{
		iLength--;
	}
	
	//	Argument's of length 1 are of no interest.
	if(iLength == 1)	{
		delete(pRemove);
		return(NULL);
	}
	
	char *pRetVal = new char[iLength];
	
	if(*pTraverse == '\"')	{
		pTraverse++;
	}	
	strncpy(pRetVal, pTraverse, iLength - 1);
	pRetVal[iLength - 1] = '\0';
	
	delete(pRemove);
	return(pRetVal);
}

void	Led_URLManager::ServerReturned (HDDEDATA hArgs, const char *pFormat, ...)
{
	//	Of course, only pointers should be passed in as the variable number of
	//		arguments so assignment can take place.
	//	hArgs is free'd off by this function.
	char *pDataArgs = (char *)DdeAccessData(hArgs, NULL);
	
	//	Initialize variable number of argumetns.
	va_list VarList;
	va_start(VarList, pFormat);	
	
	//	It will be possible that with only one argument,
	//		that we are receiving raw data intead of string notation.
	if(strchr(pFormat, ',') == NULL)	{
		//	Only one argument.
		if(strcmp(pFormat, "DW") == 0)	{
			DWORD *pWord;
			pWord = va_arg(VarList, DWORD *);
			*pWord = *(DWORD *)pDataArgs;
			DdeUnaccessData(hArgs);
			DdeFreeDataHandle(hArgs);
			va_end(VarList);
			return;
		}
		else if(strcmp(pFormat, "BL") == 0)	{
			typedef short TwoByteBool;
			TwoByteBool *pBool;
			pBool = va_arg(VarList, TwoByteBool *);
			*pBool = *(TwoByteBool *)pDataArgs;
			DdeUnaccessData(hArgs);
			DdeFreeDataHandle(hArgs);
			va_end(VarList);
			return;
		}
	}

	//	We are assuming NULL terminated data, since there is more than one
	//		parameter expected.
	HSZ hszArgs = DdeCreateStringHandleA (sDDEMLInstance, pDataArgs, CP_WINANSI);
	DdeUnaccessData(hArgs);	
	DdeFreeDataHandle(hArgs);

	int i_ArgNum = 0;
	const char *pScan = pFormat;
	char *pExtract;
	
	//	Loop through the arguments we are going to parse.
	while(pScan && *pScan)	{
		i_ArgNum++;
		pExtract = ExtractArgument(hszArgs, i_ArgNum);
	
		if(0 == strncmp(pScan, "DW", 2))	{
			//	DWORD.
			DWORD *pWord;
			pWord = va_arg(VarList, DWORD *);
			
			//	If there is nothing to scan, use a default value.
			if(pExtract == NULL)	{
				*pWord = 0x0;
			}
			else	{
				sscanf(pExtract, "%lu", pWord);
			}
		}
		else if(0 == strncmp(pScan, "QCS", 3))	{
			//	A quoted CString
			string*	pCS = va_arg(VarList, string*);
			
			if(pExtract == NULL)	{
				//pCS->Empty();
				*pCS = string ();
			}
			else	{
				//	Fun thing about a qouted string, is that we need
				//		to compress and '\"' into '"'.
				//	Extractions took off the leading and ending quotes.
				char *pCopy = pExtract;
				while(*pCopy)	{
					if(*pCopy == '\\' && *(pCopy + 1) == '\"')	{
						pCopy++;
					}
					
					*pCS += *pCopy;
					pCopy++;
				}
			}
		}
		else if(0 == strncmp(pScan, "CS", 2))	{
			//	A CString
			string*	pCS = va_arg (VarList, string*);

			if(pExtract == NULL)	{
				*pCS = string ();
			}
			else	{
				*pCS = pExtract;
			}
		}
		else if(0 == strncmp(pScan, "BL", 2))	{
			typedef short TwoByteBool;
			//	A boolean
			TwoByteBool *pBool = va_arg(VarList, TwoByteBool *);
			
			if(pExtract == NULL)	{
				*pBool = FALSE;
			}
			else	{
				//	Compare for a TRUE or a FALSE
				#if		defined (__MWERKS__)
					#define	stricmp	strcmp
					// TMP HACK - we don't build with MWERKS for the PC anyhow - just test compiles...
					// LGP 960617
				#endif
				if (0 == stricmp(pExtract, "TRUE"))	{
					*pBool = TRUE;
				}
				else	{
					*pBool = FALSE;
				}
			}
		}
		
		//	Go on to the next argument in our format string.
		pScan = SkipToNextArgument (pScan);
		
		//	Free the memory that was used during extraction.
		if (pExtract != NULL)	{
			delete pExtract;
		}
	}
	
	//	Done with variable number of arguments
	va_end(VarList);
	
	//	Free off our created HSZ string.
	DdeFreeStringHandle (sDDEMLInstance, hszArgs);
}
#endif









/*
 ********************************************************************************
 *************************** MakeSophistsAppNameVersionURL **********************
 ********************************************************************************
 */
string	MakeSophistsAppNameVersionURL (const string& relURL, const string& appName, const string& extraArgs)
{
	Led_Require (relURL.length () > 0 and relURL[0] == '/');
	char	fullVersionBuf[1024];
	(void)::sprintf (fullVersionBuf, "%d", qLed_FullVersion);
	string	fullURL	=	"http://www.sophists.com" + relURL +
							"?AppName=" + appName +
							#if		qWindows
								string ("&Platform=Windows") +
							#elif	qMacOS
								string ("&Platform=MacOS") +
							#elif	qXWindows
								string ("&Platform=XWindows") +
							#endif
							"&MajorMinorVersion=" + qLed_MajorMinorVersionString +
							"&LedFullVersion=" + fullVersionBuf +
							"&ShortVersionString=" + qLed_ShortVersionString +
							extraArgs
				;
	return fullURL;
}











/*
 ********************************************************************************
 ******************************* Led_CasedCharsEqual ****************************
 ********************************************************************************
 */
bool	Led_CasedCharsEqual (char lhs, char rhs, bool ignoreCase)
{
	if (lhs == rhs) {
		return true;
	}
	if (ignoreCase and
		isascii (lhs) and isalpha (lhs) and
		isascii (rhs) and isalpha (rhs) and
		toupper (lhs) == toupper (rhs)
		) {
		return true;
	}
	return false;
}



/*
 ********************************************************************************
 ******************************* Led_CasedStringsEqual **************************
 ********************************************************************************
 */
bool	Led_CasedStringsEqual (const string& lhs, const string& rhs, bool ignoreCase)
{
	if (lhs.length () != rhs.length ()) {
		return false;
	}
	for (size_t i = 0; i < lhs.length (); i++) {
		if (not Led_CasedCharsEqual (lhs[i], rhs[i], ignoreCase)) {
			return false;
		}
	}
	return true;
}








/*
 ********************************************************************************
 ************************************* FileReader *******************************
 ********************************************************************************
 */
FileReader::FileReader (
						#if		qMacOS
							const FSSpec* fileName
						#elif	qWindows || qXWindows
							const Led_SDK_Char* fileName
						#endif
					)
{
	// OPEN
	#if		qMacOS
		short	fd	=	0;
		Led_ThrowOSErr (::FSpOpenDF (fileName, fsRdPerm, &fd));
	#elif	qWindows
		#if		qMustUseNoUnderbarForOpenFlagsCompilerBug
			int	fd	=	::_topen (fileName, O_RDONLY | O_BINARY);
		#else
			int	fd	=	::_topen (fileName, _O_RDONLY | _O_BINARY);
		#endif
		Led_ThrowIfFalseGetLastError (fd != -1);
	#elif	qXWindows
		int	fd	=	::open (fileName, O_RDONLY);
		if (fd == -1) {
			Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
		}
	#endif

	// Get LENGTH
	#if		qMacOS
		size_t	fileLen	= 0;
		try {
			long	logEOF;
			Led_ThrowOSErr (::GetEOF (fd, &logEOF));
			fileLen = logEOF;
		}
		catch (...) {
			::FSClose (fd);
			throw;
		}
	#elif	qWindows
		size_t	fileLen	= ::_lseek (fd, 0, SEEK_END);
	#elif	qXWindows
		size_t	fileLen	= ::lseek (fd, 0, SEEK_END);
	#endif

	Byte*	fileData	=	NULL;
	try {
		fileData = new Byte [fileLen];

		// READ IN DATA
		#if		qMacOS
			long	count	=	fileLen;
			OSErr	err		=	::FSRead (fd, &count, (char*)fileData);
			Led_ThrowOSErr ((err == eofErr)? noErr: err);
			if (count != fileLen) {
				Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
			}
		#elif	qWindows
			::_lseek (fd, 0, SEEK_SET);
			Led_ThrowIfFalseGetLastError (::_read (fd, fileData, fileLen) == int (fileLen));
		#elif	qXWindows
			// MUST ADD ERRCHECKING!
			::lseek (fd, 0, SEEK_SET);
			::read (fd, fileData, fileLen);
		#endif
	}
	catch (...) {
		#if		qMacOS
			::FSClose (fd);
		#elif	qWindows || qXWindows
			::close (fd);
		#endif
		delete[] fileData;
		throw;
	}

	// Close file, and update data pointers to stored file data...
	#if		qMacOS
		::FSClose (fd);
	#elif	qWindows || qXWindows
		::close (fd);
	#endif
	fFileDataStart = fileData;
	fFileDataEnd = fileData + fileLen;
}

FileReader::~FileReader ()
{
	#if		qDeleteOfPointerToConstantDataFailsOverloadError
		delete[] (char*)fFileDataStart;
	#else
		delete[] fFileDataStart;
	#endif
}




/*
 ********************************************************************************
 *********************************** FileWriter *********************************
 ********************************************************************************
 */
FileWriter::FileWriter (
						#if		qMacOS
							const FSSpec* fileName
						#elif	qWindows || qXWindows
							const Led_SDK_Char* fileName
						#endif
				):
			fFD (0)
{
	#if		qMacOS
		// Do I need to do someting special on MacOS to get the file created??? FSCreate???
		Led_ThrowOSErr (::FSpOpenDF (fileName, fsRdWrPerm, &fFD));
	#elif	qWindows
		#if		qMustUseNoUnderbarForOpenFlagsCompilerBug
			fFD =	::_topen (fileName, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666 );
		#else
			fFD	=	::_topen (fileName, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, 0666 );
		#endif
		Led_ThrowIfFalseGetLastError (fFD != -1);
	#elif	qXWindows
		fFD	=	::open (fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
		if (fFD == -1) {
			Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
		}
	#endif
}

FileWriter::~FileWriter ()
{
	#if		qMacOS
		::FSClose (fFD);
	#elif	qWindows || qXWindows
		::close (fFD);
	#endif
}

void	FileWriter::Append (const Byte* data, size_t count)
{
	#if		qMacOS
// UNTESTED - MUST MAKE SURE ARGS RIGHT!!! - LGP 2003-12-09
		long	countWritten	=	0;
		OSErr	err		=	::FSWrite (fFD, &countWritten, data);
		Led_ThrowOSErr ((err == eofErr)? noErr: err);
		if (count != countWritten) {
			Led_ThrowOutOfMemoryException ();	// WRONG EXCEPTION
		}
	#elif	qWindows
		Led_ThrowIfFalseGetLastError (::_write (fFD, data, count) == int (count));
	#elif	qXWindows
		// MUST ADD ERRCHECKING!
		::write (fFD, data, count);
	#endif
}





#if		qLedCheckCompilerFlagsConsistency
	namespace LedCheckCompilerFlags {
		int	LedCheckCompilerFlags_(qDebug)					=	qDebug;
		int	LedCheckCompilerFlags_(qSingleByteCharacters)	=	qSingleByteCharacters;
		int	LedCheckCompilerFlags_(qMultiByteCharacters)	=	qMultiByteCharacters;
		int	LedCheckCompilerFlags_(qWideCharacters)			=	qWideCharacters;
		int	LedCheckCompilerFlags_(qProvideIMESupport)		=	qProvideIMESupport;
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

