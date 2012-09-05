/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/LedLineItPP/Sources/LedLineIt.r,v 2.20 2003/06/04 03:08:26 lewis Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedLineIt.r,v $
 *	Revision 2.20  2003/06/04 03:08:26  lewis
 *	SPR#1513: spellcheck engine support
 *	
 *	Revision 2.19  2003/05/30 14:43:32  lewis
 *	SPR#1517: MacOS replace dialog support
 *	
 *	Revision 2.18  2003/03/13 18:20:44  lewis
 *	annotate point sizes of font sizes with 'pt'
 *	
 *	Revision 2.17  2003/01/23 17:10:00  lewis
 *	SPR#1256 - use new Led_StdDialogHelper_OtherFontSizeDialog
 *	
 *	Revision 2.16  2003/01/20 20:54:16  lewis
 *	SPR#1251 - added 'carb' resource so can run native on OS X
 *	
 *	Revision 2.15  2002/05/06 21:31:28  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 2.14  2001/11/27 00:28:27  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 2.13  2001/09/12 00:22:17  lewis
 *	SPR#1023- added LedItApplication::HandleBadUserInputException () code and other related fixes
 *	
 *	Revision 2.12  2001/09/07 15:46:55  lewis
 *	resourcerer demo said MENU and wctb resources should not have purgeable bit set. I have no idea - so comply
 *	
 *	Revision 2.11  2001/09/06 22:16:38  lewis
 *	SPR#1003- Mac about box picture
 *
 *	Revision 2.10  2001/08/30 01:01:08  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  2000/10/16 19:34:28  lewis
 *	prelim support for Led_StdDialogHelper_AboutBox
 *	
 *	Revision 2.8  1998/05/05 00:40:05  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.7  1997/12/24  04:52:55  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1997/12/24  04:00:52  lewis
 *	compat with new mwkers (CWPro2) compiler
 *
 *	Revision 2.5  1997/07/27  16:03:37  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.4  1997/06/23  16:24:11  lewis
 *	new finder icons, and about box.
 *
 *	Revision 2.3  1997/06/18  03:36:05  lewis
 *	ShiftLeft/Right commands
 *
 *	Revision 2.2  1997/03/04  20:27:41  lewis
 *	*** empty log message ***
 *
 *	Revision 2.1  1997/01/20  05:46:36  lewis
 *	Lose font style support
 *
 *	Revision 2.0  1997/01/10  03:51:52  lewis
 *	*** empty log message ***
 *
 *
 *
 *	<========== BasedOnLedIt! 2.1 ==========>
 *
 *
 */

#define SystemSevenOrBetter 1			// we want the extended types
#define	SystemSevenOrLater	1			// Types.r uses this variable

#include	<BalloonTypes.r>
#include	<SysTypes.r>
#include	<Types.r>

#include	<PowerPlant.r>

#include	"LedConfig.h"
#include	"LedStdDialogs.h"

#include	"LedLineItConfig.h"
#include	"LedLineItResources.h"

#include	"AppleEvents.r"     /* AppleEvents.r used to define 'aedt' */


// From PP_Resources.h  -	cannot directly include, cuz declarations are const, rather than #defines...
#define	MBAR_Initial	128
#define	STRx_Standards	200
#define	cmd_FontMenu	250
#define	cmd_SizeMenu	251




#if		TARGET_CARBON
// LGP 2003-01-20	- I THINK this resource marks our application as having been carbonized (so can run directly under OSX)
data	'carb' (0) {
};
#endif




/*
 ********************************************************************************
 ********************************* Finder Resources *****************************
 ********************************************************************************
 */
	#if		qLed_Version_Stage == qLed_Version_Stage_Dev
		#define developmentStage		development
	#elif	qLed_Version_Stage == qLed_Version_Stage_Alpha
		#define developmentStage		alpha
	#elif	qLed_Version_Stage == qLed_Version_Stage_Beta
		#define developmentStage		beta
	#elif	qLed_Version_Stage == qLed_Version_Stage_ReleaseCandidate
		#define developmentStage		final
	#elif	qLed_Version_Stage == qLed_Version_Stage_Release
		#define developmentStage		final
	#else
		#error	"bad stage?"
	#endif
resource 'vers' (1) {
	qLed_Version_Major,
	qLed_Version_Minor,
	developmentStage,
	qLed_Version_SubStage,
	verUS,
	qLed_ShortVersionString,
	"LedLineIt! " qLed_ShortVersionString
};
resource 'vers' (2) {
	qLed_Version_Major,
	qLed_Version_Minor,
	developmentStage,
	qLed_Version_SubStage,
	verUS,
	qLed_ShortVersionString,
	"LedLineIt! " qLed_ShortVersionString
};
resource 'open' (128) {
	kApplicationSignature,
	{
		kTEXTFileType,					
		'****',					
	}
};
#ifndef	ftApplicationName
	#define	ftApplicationName	'apnm'
#endif
resource 'kind' (128) {
	kApplicationSignature,
	0,
	{
		ftApplicationName,				"LedLineIt!",
		kTEXTFileType,					"Text file",
	},
};
data kApplicationSignature (0, "Owner Resource") {
	"LedLineIt! " qLed_ShortVersionString
};
resource 'BNDL' (5000, "Application BNDL") {
	kApplicationSignature,
	0,
	{
		'FREF',
		{
			0, kApplicationFinderIconResID,
			1, kTEXTDocument_FinderIconResID,
			2, kTEXTDocumentStationary_FinderIconResID,
			3, kAnyTypeFinderIconResID,
		},
		'ICN#',
		{
			0, kApplicationFinderIconResID,
			1, kTEXTDocument_FinderIconResID,
			2, kTEXTDocumentStationary_FinderIconResID,
			3, 0,			/* no ICN# reference since we provide no icon for this case */
		}
	}
};
resource 'FREF' (kApplicationFinderIconResID) {
	'APPL',
	0,
	""
};
resource 'FREF' (kTEXTDocument_FinderIconResID) {
	kTEXTFileType,
	1,
	""
};
resource 'FREF' (kTEXTDocumentStationary_FinderIconResID) {
	kTEXTStationeryFileType,
	2,
	""
};
resource 'FREF' (kAnyTypeFinderIconResID) {
	'****',
	3,
	""
};
resource 'ICN#' (kApplicationFinderIconResID) {
	{
		$"0000 0000 7FFF FC00 7FFF F800 7FFF FF00 7E00 0100 7E80 0100 7E08 4100 7E88 4900"
		$"7EBF F100 7E88 4100 7E88 4900 7E00 0900 7E21 8100 7E42 4100 7EE2 5100 7E42 4101"
		$"7A42 4101 7242 5101 6201 9101 4310 0101 02E0 6101 0240 7101 0240 7D01 024F FF01"
		$"0240 7D01 0240 7901 02E0 6101 0310 4101 03FF FF01 0000 0001 0000 0001 0001 5FFF",
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'icl8' (kApplicationFinderIconResID) {
	$"F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F5 F5F5 F5F6 F6F6 F6F6 2B2B 2B2B F7F7"
	$"F6FC FCFC FCFC FCFC FCFC FCFC FBFB FBFB 8181 81FA FAFA F9F9 F956 5656 56F8 F8F7"
	$"F6FB FCFB FCFB FCFB FCFB FCFB FBFB 8181 81FA FAFA FAF9 F9F9 5656 56F8 F8F8 F8F7"
	$"F6FC FCFB FCFB FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF 5656 5656 2B56 2B56"
	$"F6FC FBFC FCFC FF00 0000 0000 0000 0000 0000 0000 0000 00FF 5656 562B 562B 5656"
	$"F6FC FCFB FCFC FF00 FF00 0000 0000 0000 0000 0000 0000 00FF 5656 2B56 2B56 2B56"
	$"F6FC FBFC FBFC FF00 0000 0000 FF00 0000 00FF 0000 0000 00FF 562B 562B 562B 2B56"
	$"F6FC FCFC FCFB FF00 FF00 0000 FF00 0000 00FF 0000 FF00 00FF 2B2B 2B2B 2B2B 2B56"
	$"F6FC FBFC FBFB FF00 FF00 FFFF FFFF FFFF FFFF FFFF 0000 00FF 562B 562B 2B2B 2B56"
	$"F6FB FCFB FBFB FF00 FF00 0000 FF00 0000 00FF 0000 0000 00FF 2B56 2B2B 2B2B 0056"
	$"F6FC FBFB FB81 FF00 FF00 0000 FF00 0000 00FF 0000 FF00 00FF 562B 2B2B 2B2B 2B56"
	$"F6FB FBFB 8181 FF00 0000 0000 0000 0000 0000 0000 FF00 00FF 2B2B 2B2B 2B00 0056"
	$"F6FB FB81 8181 FF00 0000 FF00 0000 00FF FF00 0000 0000 00FF 2B2B 2B2B 2B2B 2B56"
	$"F6FB 8181 81FA FF00 00FF 0000 0000 FF00 00FF 0000 0000 00FF 2B2B 2B2B 002B 0056"
	$"F681 8181 FAFA FF00 FFFF FF00 0000 FF00 00FF 00FF 0000 00FF 2B2B 2B00 2B2B 2B56"
	$"F581 81FA FAFA FF00 00FF 0000 0000 FF00 00FF 0000 0000 00FF 2B2B 2B2B 2B00 0081"
	$"F581 FAFA FAF9 FF00 00FF 0000 0000 FF00 00FF 0000 0000 00FF 2B2B 002B 2B00 0081"
	$"F5FA FAFA F9F9 FF00 00FF 0000 0000 FF00 00FF 00FF 0000 00FF 2B00 2B2B 0000 0081"
	$"F5FA FAF9 F9F9 FF00 0000 0000 0000 00FF FF00 00FF 0000 00FF 2B2B 2B2B 2B00 0081"
	$"F6FA F9F9 F956 FFD2 0000 00D2 0000 0000 0015 0000 0000 00FF 2B2B 2B2B 0000 0081"
	$"F6F9 F9F9 5656 FF00 D2D2 D200 0000 0000 0023 230E 0000 00FF 2B2B 2B00 0000 0081"
	$"F6F9 F956 5656 FF00 00D2 0000 0000 0000 0023 2323 1507 00FF 2B2B 0000 0000 0081"
	$"F6F9 5656 56F8 FF00 00D2 0000 0000 0000 0023 2323 231C 15FF 2B00 0000 0000 0081"
	$"2B56 5656 F8F8 FF00 00D2 0000 2323 2323 1C23 2323 2323 2347 0000 0000 0000 0081"
	$"2B56 56F8 F8F8 FF00 00D2 0000 0000 0000 0023 2323 2323 15FF 2B00 0000 0000 0081"
	$"2B56 F8F8 F8F8 FF00 00D2 0000 0000 0000 0023 2323 1C07 00FF 0000 0000 0000 0081"
	$"2BF8 F8F8 F7F7 FF00 D2D2 D200 0000 0000 0023 2315 0700 00FF 0000 0000 0000 0081"
	$"2BF8 F8F7 F7F7 FFD2 0000 00D2 0000 0000 001C 0700 0000 00FF 0000 0000 0000 0081"
	$"F7F8 F7F7 F7F7 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF 0000 0000 0000 00AC"
	$"F7F8 F7F7 2B2B 2B2B 2B2B 002B 002B 0000 002B 0000 0000 0000 0000 0000 0000 00AC"
	$"F7F7 F8F8 F8F8 2B2B 2B00 2B00 2B00 2B00 0000 0000 0000 0000 0000 0000 0000 00AC"
	$"2B2B 2B56 2B56 5656 5656 5656 5656 5681 5681 5681 8181 8181 8181 8181 81AC 81AC"
};
resource 'icl4' (kApplicationFinderIconResID) {
	$"CCCC CCCC CCCC CCCC C000 0CCC CCCC CCCC CEEE EEEE EEEE EEEE DDDD DDDD DDDD DCCC"
	$"CEEE EEEE EEEE EEDD DDDD DDDD DDDC CCCC CEEE EEFF FFFF FFFF FFFF FFFF DDDD CDCD"
	$"CEEE EEF0 0000 0000 0000 000F DDDC DCDD CEEE EEF0 F000 0000 0000 000F DDCD CDCD"
	$"CEEE EEF0 0000 F000 0F00 000F DCDC DCCD CEEE EEF0 F000 F000 0F00 F00F CCCC CCCD"
	$"CEEE EEF0 F0FF FFFF FFFF 000F DCDC CCCD CEEE EEF0 F000 F000 0F00 000F CDCC CC0D"
	$"CEEE EDF0 F000 F000 0F00 F00F DCCC CCCD CEEE DDF0 0000 0000 0000 F00F CCCC C00D"
	$"CEED DDF0 00F0 000F F000 000F CCCC CCCD CEDD DDF0 0F00 00F0 0F00 000F CCCC 0C0D"
	$"CDDD DDF0 FFF0 00F0 0F0F 000F CCC0 CCCD 0DDD DDF0 0F00 00F0 0F00 000F CCCC C00D"
	$"0DDD DDF0 0F00 00F0 0F00 000F CC0C C00D 0DDD DDF0 0F00 00F0 0F0F 000F C0CC 000D"
	$"0DDD DDF0 0000 000F F00F 000F CCCC C00D CDDD DDF6 0006 0000 0200 000F CCCC 000D"
	$"CDDD DDF0 6660 0000 033C 000F CCC0 000D CDDD DDF0 0600 0000 0333 2C0F CC00 000D"
	$"CDDD DCF0 0600 0000 0333 322F C000 000D CDDD CCF0 0600 3333 2333 3333 0000 000D"
	$"CDDC CCF0 0600 0000 0333 332F C000 000D CDCC CCF0 0600 0000 0333 2C0F 0000 000D"
	$"CCCC CCF0 6660 0000 0332 C00F 0000 000D CCCC CCF6 0006 0000 02C0 000F 0000 000D"
	$"CCCC CCFF FFFF FFFF FFFF FFFF 0000 000E CCCC CCCC CC0C 0C00 0C00 0000 0000 000E"
	$"CCCC CCCC C0C0 C0C0 0000 0000 0000 000E CCCD CDDD DDDD DDDD DDDD DDDD DDDD DEDE"
};
resource 'ics#' (kApplicationFinderIconResID) {
	{
		$"FF60 FFF0 F810 FAB0 FFD0 FAB0 FD90 7DD1 F9D1 D5D1 1CD1 1BF1 18F1 1CD1 1FF1 01FF",
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'ics8' (kApplicationFinderIconResID) {
	$"FCFC FCFC FCFC FBFB F6FA FAF9 F956 F8F8 FCFC FCFF FFFF FFFF FFFF FFFF 56F8 F8F8"
	$"FCFC FCFF FF00 0000 0000 00FF 5656 5656 FCFC FCFF FF00 FF00 FF00 FFFF 5656 5656"
	$"FCFC FBFF FFFF FFFF FFFF 00FF 5656 2B56 FCFB FBFF FF00 FF00 FF00 FFFF 562B 2B56"
	$"FBFB FAFF FFFF 00FF FF00 00FF 2B2B 2B56 F6FA FAFF FFFF 00FF FFFF 00FF 2B2B 2B81"
	$"FAFA FAFF FF00 00FF FFFF 00FF 2B2B 2B81 FAFA F9FF 00D2 00FF FFFF 00FF 2B2B 2B81"
	$"F9F9 56FF D2D2 0000 2323 15FF 2B2B 0081 F956 F8FF D200 2323 2323 23FF 2B00 0081"
	$"56F8 F8FF D200 0000 2323 23FF 2B00 0081 F8F8 F7FF D2D2 0000 2323 07FF 0000 0081"
	$"F8F7 F7FF FFFF FFFF FFFF FFFF 0000 00AC F7F8 F856 5656 5681 8181 8181 8181 ACAC"
};
resource 'ics4' (kApplicationFinderIconResID) {
	$"EEEE EEEE CDDD DDCC EEEF FFFF FFFF DCCC EEEF F000 000F DDDD EEEF F0F0 F0FF DDDD"
	$"EEEF FFFF FF0F DDCD EEEF F0F0 F0FF DCCD EEDF FF0F F00F CCCD CDDF FF0F FF0F CCCD"
	$"DDDF F00F FF0F CCCD DDDF 060F FF0F CCCD DDDF 6600 332F CC0D DDCF 6033 333F C00D"
	$"DCCF 6000 333F C00D CCCF 6600 33CF 000D CCCF FFFF FFFF 000E CCCD DDDD DDDD DDEE"
};
resource 'ICN#' (kTEXTDocument_FinderIconResID) {
	{
		$"1FFF FC00 1000 0E00 17C0 FD00 1100 2C80 1174 AC40 1144 AC20 1173 2FF0 1043 0FF0"
		$"1074 87F0 1004 8070 1000 0030 1100 0030 1300 0030 1070 0030 100F 0230 1000 0330"
		$"1000 0030 1000 0230 1000 0410 1001 0910 1002 1210 1000 2490 1008 4110 1012 9010"
		$"1085 2410 10CA 4890 1064 9130 1031 2A50 1038 5490 101C A110 100E 0010 1FFF FFF0",
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};
resource 'icl8' (kTEXTDocument_FinderIconResID) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF 0000 0000 0000 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 FAFF FF00 0000 0000 0000 0000"
	$"0000 00FF F8D8 D8D8 D8D8 F8F8 F8F8 F8F8 D8D8 D8D8 D8FF 2BFF 0000 0000 0000 0000"
	$"0000 00FF F8F8 F8D8 F8F8 F8F8 F8F8 F8F8 F8F8 D8F8 FAFF 2B2B FF00 0000 0000 0000"
	$"0000 00FF F8F8 F8D8 F8D8 D8D8 F8D8 F8F8 D8F8 D8F8 FAFF 2B2B 2BFF 0000 0000 0000"
	$"0000 00FF F8F8 F8D8 F8D8 F8F8 F8D8 F8F8 D8F8 D8F8 FAFA 2B2B 2B2B FF00 0000 0000"
	$"0000 00FF F8F8 F8D8 F8D8 D8D8 F8F8 D8D8 F8F8 D8F8 FAFF FFFF FFFF FFFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8D8 F8F8 F8F8 D8D8 F8F8 F8F8 FAFA FAFA FAFA FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8D8 D8D8 F8D8 F8F8 D8F8 F8F8 F8FA FAFA FAFA FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8D8 F8F8 D8F8 F8F8 F8F8 F8F8 F8FA FAFF 0000 0000"
	$"0000 00FF F8F8 1313 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 FAFF 0000 0000"
	$"0000 00FF F8F8 1369 1616 1616 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 FAFF 0000 0000"
	$"0000 00FF F8F8 FAFA 1616 1616 1616 1616 F8F8 F8F6 F8F8 F8F8 F8F8 FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8FA FAFA 1616 1616 1616 1616 F6F8 F8F8 F8F8 FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 FAFA FAFA 1616 1616 1616 FFF8 F8F8 FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F600 0000 1616 FFFF F8F8 FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F8F6 0000 0000 0000 00F6 F8F8 FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F600 0000 0000 0000 D800 F6F8 FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F6 0000 0000 0000 00D8 0000 00F6 F6FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F600 00FA 0000 0000 D800 00FA 0000 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F6 0000 FA00 0000 00D8 0000 FA00 0000 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F600 0000 0000 0000 D800 00FA 0000 FA00 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F6 0000 FA00 0000 00D8 0000 0000 00FA 0000 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F600 00FA 0000 FA00 D800 00FA 0000 0000 0000 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 FA00 0000 00FA 00D8 0000 FA00 00FA 0000 0000 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 FAFA 0000 FA00 D800 00FA 0000 FA00 0000 FA00 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8FA FA00 00D8 0000 FA00 00FA 0000 00FA 0000 FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 FAFB 0000 00FA 0000 FA00 FA00 FA00 00FA 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 FAFA FB00 0000 00FA 00FA 00FA 0000 FA00 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8FA FAFB 0000 FA00 FA00 0000 00FA 0000 00FF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 FAFA FB00 0000 0000 0000 0000 0000 00FF 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
};
resource 'icl4' (kTEXTDocument_FinderIconResID) {
	$"000F FFFF FFFF FFFF FFFF FF00 0000 0000 000F CCCC CCCC CCCC CCCC DFF0 0000 0000"
	$"000F C333 33CC CCCC 3333 3FCF 0000 0000 000F CCC3 CCCC CCCC CC3C DFCC F000 0000"
	$"000F CCC3 C333 C3CC 3C3C DFCC CF00 0000 000F CCC3 C3CC C3CC 3C3C DDCC CCF0 0000"
	$"000F CCC3 C333 CC33 CC3C DFFF FFFF 0000 000F CCCC C3CC CC33 CCCC DDDD DDDF 0000"
	$"000F CCCC C333 C3CC 3CCC CDDD DDDF 0000 000F CCCC CCCC C3CC 3CCC CCCC CDDF 0000"
	$"000F CCCC CCCC CCCC CCCC CCCC CCDF 0000 000F CCC4 2222 CCCC CCCC CCCC CCDF 0000"
	$"000F CCDD 2222 2222 CCCC CCCC CCDF 0000 000F CCCC CDDD 2222 2222 CCCC CCDF 0000"
	$"000F CCCC CCCC DDDD 2222 22FC CCDF 0000 000F CCCC CCCC CCCC C000 22FF CCDF 0000"
	$"000F CCCC CCCC CCCC 0000 000C CCDF 0000 000F CCCC CCCC CCC0 0000 0030 CCDF 0000"
	$"000F CCCC CCCC CC00 0000 0300 0CCF 0000 000F CCCC CCCC C00D 0000 300D 000F 0000"
	$"000F CCCC CCCC 00D0 0003 00D0 000F 0000 000F CCCC CCC0 0000 0030 0D00 D00F 0000"
	$"000F CCCC CC00 D000 0300 000D 000F 0000 000F CCCC C00D 00D0 300D 0000 000F 0000"
	$"000F CCCC D000 0D03 00D0 0D00 000F 0000 000F CCCC DD00 D030 0D00 D000 D00F 0000"
	$"000F CCCC CDD0 0300 D00D 000D 00DF 0000 000F CCCC CCDE 000D 00D0 D0D0 0D0F 0000"
	$"000F CCCC CCDD E000 0D0D 0D00 D00F 0000 000F CCCC CCCD DE00 D0D0 000D 000F 0000"
	$"000F CCCC CCCC DDE0 0000 0000 000F 0000 000F FFFF FFFF FFFF FFFF FFFF FFFF"
};
resource 'ics#' (kTEXTDocument_FinderIconResID) {
	{
		$"7FF0 4038 402C 413C 403C 5004 5C04 4314 4004 4134 417C 47D4 4BEC 4FFC 46FC 7FFC",
		$"7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	}
};
resource 'ics8' (kTEXTDocument_FinderIconResID) {
	$"00FF FFFF FFFF FFFF FFFF FFFF 0000 0000 00FF F8F8 F8F8 F8F8 F8F8 FFFF FF00 0000"
	$"00FF F8F8 F8F8 F8F8 F8F8 FF2B FFFF 0000 00FF F8F8 F8F8 F8D8 F8F8 FFFF FFFF 0000"
	$"00FF F8F8 F8F8 F8F8 F8F8 FAFA FAFF 0000 00FF F869 F8F8 F8F8 F8F8 F8F8 F8FF 0000"
	$"00FF F8FA FAFA 1616 F8F8 F8F8 F8FF 0000 00FF F8F8 F8F8 FAFA F616 16FF F8FF 0000"
	$"00FF F8F8 F8F8 F8F8 0000 00F6 F8FF 0000 00FF F8F8 F8F8 F8FA 0000 D8FA F6FF 0000"
	$"00FF F8F8 F8F8 00FA 00D8 FAFA FAFF 0000 00FF F8F8 F8FA FAFA D8FA 00FA 00FF 0000"
	$"00FF F8F8 FA00 FAD8 FAFA FA00 FAFF 0000 00FF F8F8 FAFB D8FA FAFA FAFA FAFF 0000"
	$"00FF F8F8 F8FA FB00 FAFA FAFA FAFF 0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};
resource 'ics4' (kTEXTDocument_FinderIconResID) {
	$"0FFF FFFF FFFF 0000 0FCC CCCC CCFF F000 0FCC CCCC CCFC FF00 0FCC CCC3 CCFF FF00"
	$"0FCC CCCC CCDD DF00 0FC4 CCCC CCCC CF00 0FCD DD22 CCCC CF00 0FCC CCDD C22F CF00"
	$"0FCC CCCC 000C CF00 0FCC CCCD 003D CF00 0FCC CC0D 03DD DF00 0FCC CDDD 3D0D 0F00"
	$"0FCC D0D3 DDD0 DF00 0FCC DE3D DDDD DF00 0FCC CDE0 DDDD DF00 0FFF FFFF FFFF FF"
};
resource 'ICN#' (kTEXTDocumentStationary_FinderIconResID) {
	{
		$"3FFF FFE0 2000 0020 2000 0038 203E 07E8 2008 0128 200B A528 200A 2528 200B 9928"
		$"2002 1828 2203 A428 2000 2428 2000 0028 2000 0428 2000 0628 2000 0028 2000 0428"
		$"2000 0828 2002 1228 2004 2428 2000 4928 2010 8228 2025 2028 210A 4028 2194 8FE8"
		$"20C9 2848 2062 4888 2070 A908 2039 4A08 201C 0C08 3FFF F808 0800 0008 0FFF FFF8",
		$"3FFF FFE0 3FFF FFE0 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 0FFF FFF8 0FFF FFF8"
	}
};
resource 'icl8' (kTEXTDocumentStationary_FinderIconResID) {
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"0000 FF00 0000 0000 0000 00F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FF00 0000 0000"
	$"0000 FF00 0000 0000 0000 00F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFFF FF00 0000"
	$"0000 FF00 0000 0000 0000 D8D8 D8D8 D8F5 F5F5 F5F5 F5D8 D8D8 D8D8 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 D8F5 F5F5 F5F5 F5F5 F5F5 F5D8 F5F5 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 D8F5 D8D8 D8F5 D8F5 F5D8 F5D8 F5F5 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 D8F5 D8F5 F5F5 D8F5 F5D8 F5D8 F5F5 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 D8F5 D8D8 D8F5 F5D8 D8F5 F5D8 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F513 13F5 F5F5 F5F5 F5F5 D8F5 F5F5 F5D8 D8F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F513 6916 1616 16F5 F5F5 D8D8 D8F5 D800 F5D8 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F516 1616 1616 1616 16F5 F5F5 D8F5 F5D8 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F516 1616 1616 1616 16F6 F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F516 1616 1616 16FF F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F6 0000 0016 16FF FFF5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5F5 F600 0000 0000 0000 F6F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F5F6 0000 0000 0000 00D8 00F6 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F5 F600 0000 0000 0000 D800 0000 F6F6 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5F6 0000 FA00 0000 00D8 0000 FA00 0000 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F600 00FA 0000 0000 D800 00FA 0000 0000 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F6 0000 0000 0000 00D8 0000 FA00 00FA 0000 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F600 00FA 0000 0000 D800 0000 0000 FA00 0000 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F6 0000 FA00 00FA 00D8 0000 FA00 0000 0000 0000 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5FA 0000 0000 FA00 D800 00FA 0000 0000 0000 0000 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5FA FA00 00FA 00D8 0000 FA00 0000 FFFF FFFF FFFF FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F5F5 FAFA 0000 D800 00FA 0000 FA00 FF2B 2B2B 2BFF F92B FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5FA FB00 0000 FA00 00FA 0000 FF2B 2B2B FFF9 2B2B FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5FA FAFB 0000 0000 FA00 FA00 FF2B 2BFF F92B 2B2B FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 FAFA FB00 00FA 00FA 0000 FF2B FFF9 2B2B 2B2B FF00 0000"
	$"0000 FFF5 F5F5 F5F5 F5F5 F5FA FAFB 0000 0000 0000 FFFF F92B 2B2B 2B2B FF00 0000"
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFF9 2B2B 2B2B 2B2B FF00 0000"
	$"0000 0000 FFF9 F9F9 F9F9 F9F9 F9F9 F9F9 F9F9 F9F9 F92B 2B2B 2B2B 2B2B FF00 0000"
	$"0000 0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
resource 'icl4' (kTEXTDocumentStationary_FinderIconResID) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFF0 0000 00F0 0000 0000 0000 0000 0000 00F0 0000"
	$"00F0 0000 0000 0000 0000 0000 00FF F000 00F0 0000 0033 3330 0000 0333 33FD F000"
	$"00F0 0000 0000 3000 0000 0003 00FD F000 00F0 0000 0000 3033 3030 0303 00FD F000"
	$"00F0 0000 0000 3030 0030 0303 00FD F000 00F0 0000 0000 3033 3003 3003 00FD F000"
	$"00F0 0CC0 0000 0030 0003 3000 00FD F000 00F0 0C42 2220 0033 3030 0300 00FD F000"
	$"00F0 0002 2222 2220 0030 0300 00FD F000 00F0 0000 0002 2222 222C 0000 00FD F000"
	$"00F0 0000 0000 0002 2222 2F00 00FD F000 00F0 0000 0000 000C 0002 2FF0 00FD F000"
	$"00F0 0000 0000 00C0 0000 00C0 00FD F000 00F0 0000 0000 0C00 0000 030C 00FD F000"
	$"00F0 0000 0000 C000 0000 3000 CCFD F000 00F0 0000 000C 00D0 0003 00D0 00FD F000"
	$"00F0 0000 00C0 0D00 0030 0D00 00FD F000 00F0 0000 0C00 0000 0300 D00D 00FD F000"
	$"00F0 0000 C00D 0000 3000 00D0 00FD F000 00F0 000C 00D0 0D03 00D0 0000 00FD F000"
	$"00F0 000D 0000 D030 0D00 0000 00FD F000 00F0 000D D00D 0300 D000 FFFF FFFD F000"
	$"00F0 0000 DD00 300D 00D0 FCCC CFDC F000 00F0 0000 0DE0 00D0 0D00 FCCC FDCC F000"
	$"00F0 0000 0DDE 0000 D0D0 FCCF DCCC F000 00F0 0000 00DD E00D 0D00 FCFD CCCC F000"
	$"00F0 0000 000D DE00 0000 FFDC CCCC F000 00FF FFFF FFFF FFFF FFFF FDCC CCCC F000"
	$"0000 FDDD DDDD DDDD DDDD DCCC CCCC F000 0000 FFFF FFFF FFFF FFFF FFFF FFFF F0"
};
resource 'ics#' (kTEXTDocumentStationary_FinderIconResID) {
	{
		$"7FFC 4006 4006 4006 4006 4006 4036 4026 4176 42F6 47D6 5FBE 4FEA 4FF2 7FE2 3FFE",
		$"7FFC 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 3FFE"
	}
};
resource 'ics8' (kTEXTDocumentStationary_FinderIconResID) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF 0000 00FF 0000 00F5 F5F5 F5F5 F5F5 F5FF FF00"
	$"00FF 0000 00F5 F5F5 F5F5 F5F5 F5FF FF00 00FF 0000 00F5 F5F5 F5F5 F5F5 F5FF FF00"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF FF00 00FF F5F5 F5F5 16F5 F5F6 F5F5 F5FF FF00"
	$"00FF F5F5 F5F5 F5F6 1616 FFFF F5FF FF00 00FF F5F5 F5F5 F6F6 0000 D8F6 F5FF FF00"
	$"00FF F5F5 F5F6 F6FA 00D8 D8FA F6FF FF00 00FF F5F5 F6F6 FA00 D8D8 FAFA 00FF FF00"
	$"00FF F5F6 F6FA FAD8 D8FA 00FA 00FF FF00 00FF F5FA FAFA FAD8 FA00 FFFF FFFF FF00"
	$"00FF F5F5 FAFB D8FA FAFA FF2B FFF9 FF00 00FF F5F5 FAFB FBFA FAFA FFFF F92B FF00"
	$"00FF FFFF FFFF FFFF FFFF FFF9 2B2B FF00 0000 FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
resource 'ics4' (kTEXTDocumentStationary_FinderIconResID) {
	$"0FFF FFFF FFFF FF00 0F00 0000 0000 0FF0 0F00 0000 0000 0FF0 0F00 0000 0000 0FF0"
	$"0F00 0000 0000 0FF0 0F00 0020 0C00 0FF0 0F00 000C 22FF 0FF0 0F00 00CC 003C 0FF0"
	$"0F00 0CCD 033D CFF0 0F00 CCD0 33DD 0FF0 0F0C CDD3 3D0D 0FF0 0F0D DDD3 D0FF FFF0"
	$"0F00 DE3D DDFC FDF0 0F00 DEED DDFF DCF0 0FFF FFFF FFFD CCF0 00FF FFFF FFFF FFF0"
};

















/*
 ********************************************************************************
 ********************** Misc PowerPlant happy Resources *************************
 ********************************************************************************
 */
resource 'WIND' (WIND_TextDoc, purgeable) {
	{0, 0, 400, 500},
	zoomDocProc,
	invisible,
	goAway,
	0x0,
	"",
	staggerParentWindowScreen
};
resource 'PPob' (prto_TextDoc, purgeable) {
	{
		ObjectData {
			Printout {
				{500, 700},
				inactive,
				disabled,
				0,
				numberAcross
			}
		},
		BeginSubs {
		},
		ObjectData {
			PlaceHolder {
				1413640056,
				{356, 556},
				visible,
				disabled,
				bound,
				bound,
				bound,
				bound,
				72,
				72,
				0,
				defaultSuperView,
				0,
				0,
				0,
				0,
				16,
				16,
				noReconcileOverhang,
				0
			}
		},
		EndSubs {
		}
	}
};





resource 'ALRT' (kMemoryExceptionAlertID, purgeable) {
	{104, 130, 237, 488},
	kMemoryExceptionAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kMemoryExceptionAlertID, purgeable) {
	{
		{103, 288, 123, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 90, 348},
		StaticText {
			disabled,
			"Not enough memory to complete the command. Led has purged uneeded caches (including undo information). Try closing some documents, or quitting other applications."
		}
	}
};




resource 'ALRT' (kPowerPlantExceptionAlertID, purgeable) {
	{104, 130, 205, 488},
	kPowerPlantExceptionAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kPowerPlantExceptionAlertID, purgeable) {
	{
		{71, 288, 91, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 58, 348},
		StaticText {
			disabled,
			"Couldn't complete that command because of a PowerPlant exception (id ^0)."
		}
	}
};




resource 'ALRT' (kGenericMacOSExceptionAlertID, purgeable) {
	{104, 130, 205, 488},
	kGenericMacOSExceptionAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kGenericMacOSExceptionAlertID, purgeable) {
	{
		{71, 288, 91, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 58, 348},
		StaticText {
			disabled,
			"Couldn't complete that command because of a MacOS exception (error # ^0)."
		}
	}
};





resource 'ALRT' (kBadUserInputExceptionAlertID, purgeable) {
	{104, 130, 205, 488},
	kBadUserInputExceptionAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kBadUserInputExceptionAlertID, purgeable) {
	{
		{71, 288, 91, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 58, 348},
		StaticText {
			disabled,
			"Invalid user input."
		}
	}
};





resource 'ALRT' (kUnknownExceptionAlertID, purgeable) {
	{104, 130, 205, 488},
	kUnknownExceptionAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kUnknownExceptionAlertID, purgeable) {
	{
		{71, 288, 91, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 58, 348},
		StaticText {
			disabled,
			"Couldn't complete that command because of an unknown exception."
		}
	}
};




resource 'ALRT' (kWarnLowLocalRAMAlertID, purgeable) {
	{104, 130, 237, 488},
	kWarnLowLocalRAMAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kWarnLowLocalRAMAlertID, purgeable) {
	{
		{103, 288, 123, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 90, 348},
		StaticText {
			disabled,
			"Very little memory left in the application heap. Text may display improperly, and LedLineIt! could even crash. Use the finder 'GetInfo' to give the applicatin more memory."
		}
	}
};




resource 'ALRT' (kWarnLowRAMAlertID, purgeable) {
	{104, 130, 237, 488},
	kWarnLowRAMAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kWarnLowRAMAlertID, purgeable) {
	{
		{103, 288, 123, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 90, 348},
		StaticText {
			disabled,
			"Very low on memory - including attempts to use temporary memory. Try quitting some other applications, closing windows, or make more available with the memory control panel."
		}
	}
};







resource 'ALRT' (kCannotOpenHelpFileAlertID, purgeable) {
	{104, 130, 237, 488},
	kCannotOpenHelpFileAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kCannotOpenHelpFileAlertID, purgeable) {
	{
		{103, 288, 123, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 90, 348},
		StaticText {
			disabled,
			"Couldn't open the help file ':LedLineItDocs:index.html' in the same folder as the application. Open the help file manually in an html browser for more hints."
		}
	}
};





resource 'ALRT' (kCannotOpenWebPageAlertID, purgeable) {
	{104, 130, 237, 488},
	kCannotOpenWebPageAlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kCannotOpenWebPageAlertID, purgeable) {
	{
		{103, 288, 123, 348},
		Button {
			enabled,
			"OK"
		},
		{10, 75, 90, 348},
		StaticText {
			disabled,
			"Couldn't open the web page. Are you properly connected to the internet? Do you have Internet Config (IC) setup? Or a web browser running?"
		}
	}
};







resource 'STR#' (STRx_Standards, "Standards", purgeable) {
	{
		"LedLineIt!",
		"Save File As:",
		"Can’t Undo"
	}
};

include	"PP Document Alerts.rsrc" not 'ckid';
include	"ColorAlertIcons.rsrc" not 'ckid';

#if		qDebug
include	"PP DebugAlerts.rsrc" not 'ckid';
#endif









/*
 ********************************************************************************
 ******************************** Apple Event Resources *************************
 ********************************************************************************
 */
data 'aedt' (128, "Required Suite", purgeable) {
	$"6165 7674 6F61 7070 0000 03E9 6165 7674 6F64 6F63 0000 03EA 6165 7674 7064 6F63"                    /* aevtoapp...Èaevtodoc...Íaevtpdoc */
	$"0000 03EB 6165 7674 7175 6974 0000 03EC"                                                            /* ...Îaevtquit...Ï */
};
data 'aedt' (129, "Core Suite", purgeable) {
	$"636F 7265 636C 6F6E 0000 07D1 636F 7265 636C 6F73 0000 07D2 636F 7265 636E 7465"                    /* coreclon...—coreclos...“corecnte */
	$"0000 07D3 636F 7265 6372 656C 0000 07D4 636F 7265 6465 6C6F 0000 07D5 636F 7265"                    /* ...”corecrel...‘coredelo...’core */
	$"646F 6578 0000 07D6 636F 7265 716F 626A 0000 07D7 636F 7265 6765 7464 0000 07D8"                    /* doex...÷coreqobj...◊coregetd...ÿ */
	$"636F 7265 6473 697A 0000 07D9 636F 7265 6774 6569 0000 07DA 636F 7265 6D6F 7665"                    /* coredsiz...Ÿcoregtei...⁄coremove */
	$"0000 07DB 636F 7265 7361 7665 0000 07DC 636F 7265 7365 7464 0000 07DD"                              /* ...€coresave...‹coresetd...› */
};
data 'aedt' (130, "Misc Standards", purgeable) {
	$"6165 7674 6F62 6974 0000 0BB9 6D69 7363 6265 6769 0000 0BBA 6D69 7363 636F 7079"                    /* aevtobit...πmiscbegi...∫misccopy */
	$"0000 0BBB 6D69 7363 6370 7562 0000 0BBC 6D69 7363 6375 7420 0000 0BBD 6D69 7363"                    /* ...ªmisccpub...ºmisccut ...Ωmisc */
	$"646F 7363 0000 0BBE 6D69 7363 6564 6974 0000 0BBF 6D69 7363 656E 6474 0000 0BC0"                    /* dosc...æmiscedit...ømiscendt...¿ */
	$"6D69 7363 696D 6772 0000 0BC1 6D69 7363 6973 756E 0000 0BC2 6D69 7363 6D76 6973"                    /* miscimgr...¡miscisun...¬miscmvis */
	$"0000 0BC3 6D69 7363 7061 7374 0000 0BC4 6D69 7363 7265 646F 0000 0C1F 6D69 7363"                    /* ...√miscpast...ƒmiscredo....misc */
	$"7276 7274 0000 0BC6 6D69 7363 7474 726D 0000 0BC7 6D69 7363 756E 646F 0000 0BC8"                    /* rvrt...∆miscttrm...«miscundo...» */
	$"6D69 7363 736C 6374 0000 0BC9"                                                                      /* miscslct...… */
};
data 'aete' (0, "English", purgeable) {
	$"0100 0000 0000 0004 0E52 6571 7569 7265 6420 5375 6974 652B 5465 726D 7320 7468"                    /* .........Required Suite+Terms th */
	$"6174 2065 7665 7279 2061 7070 6C69 6361 7469 6F6E 2073 686F 756C 6420 7375 7070"                    /* at every application should supp */
	$"6F72 7400 7265 7164 0001 0001 0000 0000 0000 0000 0E53 7461 6E64 6172 6420 5375"                    /* ort.reqd.............Standard Su */
	$"6974 6522 436F 6D6D 6F6E 2074 6572 6D73 2066 6F72 206D 6F73 7420 6170 706C 6963"                    /* ite"Common terms for most applic */
	$"6174 696F 6E73 436F 5265 0001 0001 0008 0563 6C6F 7365 0F43 6C6F 7365 2061 6E20"                    /* ationsCoRe.......close.Close an  */
	$"6F62 6A65 6374 636F 7265 636C 6F73 6E75 6C6C 0000 8000 6F62 6A20 1474 6865 206F"                    /* objectcoreclosnull..Ä.obj .the o */
	$"626A 6563 7473 2074 6F20 636C 6F73 6500 1000 0002 0673 6176 696E 6700 7361 766F"                    /* bjects to close......saving.savo */
	$"7361 766F 3F73 7065 6369 6669 6573 2077 6865 7468 6572 206F 7220 6E6F 7420 6368"                    /* savo?specifies whether or not ch */
	$"616E 6765 7320 7368 6F75 6C64 2062 6520 7361 7665 6420 6265 666F 7265 2063 6C6F"                    /* anges should be saved before clo */
	$"7369 6E67 A000 0269 6E00 6B66 696C 616C 6973 2474 6865 2066 696C 6520 696E 2077"                    /* sing†..in.kfilalis$the file in w */
	$"6869 6368 2074 6F20 7361 7665 2074 6865 206F 626A 6563 7400 8000 0964 6174 6120"                    /* hich to save the object.Ä.∆data  */
	$"7369 7A65 2552 6574 7572 6E20 7468 6520 7369 7A65 2069 6E20 6279 7465 7320 6F66"                    /* size%Return the size in bytes of */
	$"2061 6E20 6F62 6A65 6374 636F 7265 6473 697A 6C6F 6E67 1F74 6865 2073 697A 6520"                    /*  an objectcoredsizlong.the size  */
	$"6F66 2074 6865 206F 626A 6563 7420 696E 2062 7974 6573 0000 6F62 6A20 2C74 6865"                    /* of the object in bytes..obj ,the */
	$"206F 626A 6563 7420 7768 6F73 6520 6461 7461 2073 697A 6520 6973 2074 6F20 6265"                    /*  object whose data size is to be */
	$"2072 6574 7572 6E65 6400 0000 0000 0367 6574 1A47 6574 2074 6865 2064 6174 6120"                    /*  returned......get.Get the data  */
	$"666F 7220 616E 206F 626A 6563 7400 636F 7265 6765 7464 2A2A 2A2A 1854 6865 2064"                    /* for an object.coregetd****.The d */
	$"6174 6120 6672 6F6D 2074 6865 206F 626A 6563 7400 0000 6F62 6A20 2774 6865 206F"                    /* ata from the object...obj 'the o */
	$"626A 6563 7420 7768 6F73 6520 6461 7461 2069 7320 746F 2062 6520 7265 7475 726E"                    /* bject whose data is to be return */
	$"6564 0000 0000 046D 616B 6512 4D61 6B65 2061 206E 6577 2065 6C65 6D65 6E74 636F"                    /* ed.....make.Make a new elementco */
	$"7265 6372 656C 6F62 6A20 244F 626A 6563 7420 7370 6563 6966 6965 7220 666F 7220"                    /* recrelobj $Object specifier for  */
	$"7468 6520 6E65 7720 656C 656D 656E 7400 0000 6E75 6C6C 0000 9000 0004 036E 6577"                    /* the new element...null..ê....new */
	$"6B6F 636C 7479 7065 1C74 6865 2063 6C61 7373 206F 6620 7468 6520 6E65 7720 656C"                    /* kocltype.the class of the new el */
	$"656D 656E 7400 0000 0261 7400 696E 7368 696E 736C 2B74 6865 206C 6F63 6174 696F"                    /* ement....at.inshinsl+the locatio */
	$"6E20 6174 2077 6869 6368 2074 6F20 696E 7365 7274 2074 6865 2065 6C65 6D65 6E74"                    /* n at which to insert the element */
	$"8000 0977 6974 6820 6461 7461 6461 7461 2A2A 2A2A 2074 6865 2069 6E69 7469 616C"                    /* Ä.∆with datadata**** the initial */
	$"2064 6174 6120 666F 7220 7468 6520 656C 656D 656E 7400 8000 0F77 6974 6820 7072"                    /*  data for the element.Ä..with pr */
	$"6F70 6572 7469 6573 7072 6474 7265 636F 3474 6865 2069 6E69 7469 616C 2076 616C"                    /* opertiesprdtreco4the initial val */
	$"7565 7320 666F 7220 7468 6520 7072 6F70 6572 7469 6573 206F 6620 7468 6520 656C"                    /* ues for the properties of the el */
	$"656D 656E 7400 8000 046F 7065 6E1C 4F70 656E 2074 6865 2073 7065 6369 6669 6564"                    /* ement.Ä..open.Open the specified */
	$"206F 626A 6563 7428 7329 6165 7674 6F64 6F63 6E75 6C6C 0000 8000 6F62 6A20 3F4F"                    /*  object(s)aevtodocnull..Ä.obj ?O */
	$"626A 6563 7473 2074 6F20 6F70 656E 2E20 4361 6E20 6265 2061 206C 6973 7420 6F66"                    /* bjects to open. Can be a list of */
	$"2066 696C 6573 206F 7220 616E 206F 626A 6563 7420 7370 6563 6966 6965 722E 1000"                    /*  files or an object specifier... */
	$"0000 0570 7269 6E74 1D50 7269 6E74 2074 6865 2073 7065 6369 6669 6564 206F 626A"                    /* ...print.Print the specified obj */
	$"6563 7428 7329 6165 7674 7064 6F63 6E75 6C6C 0000 8000 6F62 6A20 404F 626A 6563"                    /* ect(s)aevtpdocnull..Ä.obj @Objec */
	$"7473 2074 6F20 7072 696E 742E 2043 616E 2062 6520 6120 6C69 7374 206F 6620 6669"                    /* ts to print. Can be a list of fi */
	$"6C65 7320 6F72 2061 6E20 6F62 6A65 6374 2073 7065 6369 6669 6572 2E00 0000 0000"                    /* les or an object specifier...... */
	$"0473 6176 6515 7361 7665 2061 2073 6574 206F 6620 6F62 6A65 6374 7300 636F 7265"                    /* .save.save a set of objects.core */
	$"7361 7665 6E75 6C6C 0000 0000 6F62 6A20 104F 626A 6563 7473 2074 6F20 7361 7665"                    /* savenull....obj .Objects to save */
	$"2E00 0000 0002 0269 6E00 6B66 696C 616C 6973 2774 6865 2066 696C 6520 696E 2077"                    /* .......in.kfilalis'the file in w */
	$"6869 6368 2074 6F20 7361 7665 2074 6865 206F 626A 6563 7428 7329 8000 0261 7300"                    /* hich to save the object(s)Ä..as. */
	$"666C 7470 7479 7065 3774 6865 2066 696C 6520 7479 7065 206F 6620 7468 6520 646F"                    /* fltptype7the file type of the do */
	$"6375 6D65 6E74 2069 6E20 7768 6963 6820 746F 2073 6176 6520 7468 6520 6461 7461"                    /* cument in which to save the data */
	$"8000 0373 6574 1453 6574 2061 6E20 6F62 6A65 6374 D573 2064 6174 6100 636F 7265"                    /* Ä..set.Set an object’s data.core */
	$"7365 7464 6E75 6C6C 0000 8000 6F62 6A20 1474 6865 206F 626A 6563 7420 746F 2063"                    /* setdnull..Ä.obj .the object to c */
	$"6861 6E67 6500 1000 0001 0274 6F00 6461 7461 2A2A 2A2A 0D74 6865 206E 6577 2076"                    /* hange......to.data****¬the new v */
	$"616C 7565 0000 0003 0B61 7070 6C69 6361 7469 6F6E 6361 7070 1641 6E20 6170 706C"                    /* alue.....applicationcapp.An appl */
	$"6963 6174 696F 6E20 7072 6F67 7261 6D00 0000 0002 6377 696E 0003 696E 6478 6E61"                    /* ication program.....cwin..indxna */
	$"6D65 7265 6C65 646F 6375 0001 6E61 6D65 0677 696E 646F 7700 6377 696E 0841 2057"                    /* mereledocu..name.window.cwin.A W */
	$"696E 646F 7700 000C 0662 6F75 6E64 7300 7062 6E64 7164 7274 2574 6865 2062 6F75"                    /* indow....bounds.pbndqdrt%the bou */
	$"6E64 6172 7920 7265 6374 616E 676C 6520 666F 7220 7468 6520 7769 6E64 6F77 1000"                    /* ndary rectangle for the window.. */
	$"0963 6C6F 7365 6162 6C65 6863 6C62 626F 6F6C 2144 6F65 7320 7468 6520 7769 6E64"                    /* ∆closeablehclbbool!Does the wind */
	$"6F77 2068 6176 6520 6120 636C 6F73 6520 626F 783F 0000 0674 6974 6C65 6400 7074"                    /* ow have a close box?...titled.pt */
	$"6974 626F 6F6C 2144 6F65 7320 7468 6520 7769 6E64 6F77 2068 6176 6520 6120 7469"                    /* itbool!Does the window have a ti */
	$"746C 6520 6261 723F 0000 0569 6E64 6578 7069 6478 6C6F 6E67 1874 6865 206E 756D"                    /* tle bar?...indexpidxlong.the num */
	$"6265 7220 6F66 2074 6865 2077 696E 646F 7700 1000 0866 6C6F 6174 696E 6700 6973"                    /* ber of the window....floating.is */
	$"666C 626F 6F6C 1644 6F65 7320 7468 6520 7769 6E64 6F77 2066 6C6F 6174 3F00 0000"                    /* flbool.Does the window float?... */
	$"056D 6F64 616C 706D 6F64 626F 6F6C 1449 7320 7468 6520 7769 6E64 6F77 206D 6F64"                    /* .modalpmodbool.Is the window mod */
	$"616C 3F00 0000 0972 6573 697A 6162 6C65 7072 737A 626F 6F6C 1849 7320 7468 6520"                    /* al?...∆resizableprszbool.Is the  */
	$"7769 6E64 6F77 2072 6573 697A 6162 6C65 3F00 0000 087A 6F6F 6D61 626C 6500 6973"                    /* window resizable?....zoomable.is */
	$"7A6D 626F 6F6C 1749 7320 7468 6520 7769 6E64 6F77 207A 6F6F 6D61 626C 653F 0000"                    /* zmbool.Is the window zoomable?.. */
	$"067A 6F6F 6D65 6400 707A 756D 626F 6F6C 1549 7320 7468 6520 7769 6E64 6F77 207A"                    /* .zoomed.pzumbool.Is the window z */
	$"6F6F 6D65 643F 1000 046E 616D 6500 706E 616D 6974 7874 1774 6865 2074 6974 6C65"                    /* oomed?...name.pnamitxt.the title */
	$"206F 6620 7468 6520 7769 6E64 6F77 1000 0776 6973 6962 6C65 7076 6973 626F 6F6C"                    /*  of the window...visiblepvisbool */
	$"1669 7320 7468 6520 7769 6E64 6F77 2076 6973 6962 6C65 3F00 0000 0870 6F73 6974"                    /* .is the window visible?....posit */
	$"696F 6E00 7070 6F73 5144 7074 2075 7070 6572 206C 6566 7420 636F 6F72 6469 6E61"                    /* ion.pposQDpt upper left coordina */
	$"7465 7320 6F66 2077 696E 646F 7700 0000 0000 0864 6F63 756D 656E 7400 646F 6375"                    /* tes of window......document.docu */
	$"0A41 2044 6F63 756D 656E 7400 0002 046E 616D 6500 706E 616D 6974 7874 1974 6865"                    /* .A Document....name.pnamitxt.the */
	$"2074 6974 6C65 206F 6620 7468 6520 646F 6375 6D65 6E74 0000 086D 6F64 6966 6965"                    /*  title of the document...modifie */
	$"6400 696D 6F64 626F 6F6C 3348 6173 2074 6865 2064 6F63 756D 656E 7420 6265 656E"                    /* d.imodbool3Has the document been */
	$"206D 6F64 6966 6965 6420 7369 6E63 6520 7468 6520 6C61 7374 2073 6176 653F 0000"                    /*  modified since the last save?.. */
	$"0000 0000 0001 7361 766F 0003 0379 6573 7965 7320 1053 6176 6520 6F62 6A65 6374"                    /* ......savo...yesyes .Save object */
	$"7320 6E6F 7700 026E 6F00 6E6F 2020 1344 6F20 6E6F 7420 7361 7665 206F 626A 6563"                    /* s now..no.no  .Do not save objec */
	$"7473 0361 736B 6173 6B20 1C41 736B 2074 6865 2075 7365 7220 7768 6574 6865 7220"                    /* ts.askask .Ask the user whether  */
	$"746F 2073 6176 6500 174D 6973 6365 6C6C 616E 656F 7573 2053 7461 6E64 6172 6473"                    /* to save..Miscellaneous Standards */
	$"2C55 7365 6675 6C20 6576 656E 7473 2074 6861 7420 6172 656E D574 2069 6E20 616E"                    /* ,Useful events that aren’t in an */
	$"7920 6F74 6865 7220 7375 6974 6500 6D69 7363 0000 0000 0001 0672 6576 6572 7433"                    /* y other suite.misc.......revert3 */
	$"5265 7665 7274 2061 6E20 6F62 6A65 6374 2074 6F20 7468 6520 6D6F 7374 2072 6563"                    /* Revert an object to the most rec */
	$"656E 746C 7920 7361 7665 6420 7665 7273 696F 6E00 6D69 7363 7276 7274 6E75 6C6C"                    /* ently saved version.miscrvrtnull */
	$"0000 8000 6F62 6A20 106F 626A 6563 7420 746F 2072 6576 6572 7400 0000 0000 0000"                    /* ..Ä.obj .object to revert....... */
	$"0000 0000 0D6F 6464 7320 616E 6420 656E 6473 3854 6869 6E67 7320 7468 6174 2073"                    /* ....¬odds and ends8Things that s */
	$"686F 756C 6420 6265 2069 6E20 736F 6D65 2073 7461 6E64 6172 6420 7375 6974 652C"                    /* hould be in some standard suite, */
	$"2062 7574 2061 7265 6ED5 7400 4F64 6473 0001 0001 0001 0673 656C 6563 741B 5365"                    /*  but aren’t.Odds.......select.Se */
	$"6C65 6374 2074 6865 2073 7065 6369 6669 6564 206F 626A 6563 7400 6D69 7363 736C"                    /* lect the specified object.miscsl */
	$"6374 6E75 6C6C 0000 8000 6F62 6A20 1474 6865 206F 626A 6563 7420 746F 2073 656C"                    /* ctnull..Ä.obj .the object to sel */
	$"6563 7400 9000 0000 0000 0000 0000"                                                                 /* ect.ê......... */
};









/*
 ********************************************************************************
 ************************************** Menus ***********************************
 ********************************************************************************
 */


resource 'MBAR' (MBAR_Initial, purgeable) {
	{
		kAppleMenuID,
		kFileMenuID,
		kEditMenuID,
		kOptionsMenuID,
		kWindowsMenuID,
	}
};
resource 'MENU' (kAppleMenuID) {
	kAppleMenuID,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{
		"About LedLineIt!…", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain
	}
};
resource 'Mcmd' (kAppleMenuID, purgeable) {
	{
		cmd_About
	}
};
resource 'hmnu' (kAppleMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Find out a little about this application, its authors, and the Led Class Library.",
				"",
				"",
				""
			},
	}
};
resource 'MENU' (kFileMenuID) {
	kFileMenuID,
	textMenuProc,
	0x7FFFFC03,
	enabled,
	"File",
	{
		"New", noIcon, "N", noMark, plain,
		"Open…", noIcon, "O", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Close", noIcon, "W", noMark, plain,
		"Save", noIcon, "S", noMark, plain,
		"Save As…", noIcon, noKey, noMark, plain,
		"Save A Copy As…", noIcon, noKey, noMark, plain,
		"Revert", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Page Setup…", noIcon, noKey, noMark, plain,
		"Print…", noIcon, "P", noMark, plain,
		"Print One", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Quit", noIcon, "Q", noMark, plain
	}
};
resource 'Mcmd' (kFileMenuID, purgeable) {
	{
		cmd_New,
		cmd_Open,
		msg_Nothing,
		cmd_Close,
		cmd_Save,
		cmd_SaveAs,
		cmdSaveACopyAs,
		cmd_Revert,
		msg_Nothing,
		cmd_PageSetup,
		cmd_Print,
		cmd_PrintOne,
		msg_Nothing,
		cmd_Quit
	}
};
resource 'hmnu' (kFileMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"File menu.\n\nVarious file-oriented commands, opening, closing, etc.",
				"",
				"",
				""
			},
		HMStringItem {
				"Create a new document.",
				"",
				"",
				""
			},
		HMStringItem {
				"Open an existing file.",
				"",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Close the currently active window",
				"There is no currently active window to close.",
				"",
				""
			},
		HMStringItem {
				"Save the text in the currently active window into the currently associated file. If the document is not currently associated with a file, you will be prompted to pick one.",
				"There either is no currently active window, or the currently active window has not been changed since last saved.",
				"",
				""
			},
		HMStringItem {
				"Save the text in the currently active window into a new file. This command also then associates the document with that newly specified file for future save/revert operations.",
				"There is no currently active window.",
				"",
				""
			},
		HMStringItem {
				"Save the text in the currently active window into a new file. This command does NOT associate the document with that newly created file.",
				"There is no currently active window.",
				"",
				""
			},
		HMStringItem {
				"Restore the contents of the current window to those present at the last save command.",
				"Cannot revert contents of this window to those saved on disk because either there is no current window, or the current window has not been saved to disk, or the contents are already the same as those on disk.",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Set various printing options.",
				"",
				"",
				""
			},
		HMStringItem {
				"Print the current document. You will first be prompted to set various options, like page ranges, etc.",
				"No current document to print.",
				"",
				""
			},
		HMStringItem {
				"Print one copy of all the contents of the currently active document.",
				"No current document to print.",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Exit the LedLineIt! application. If you have any documents which have been modified since last saved, you will be promted to save these.",
				"",
				"",
				""
			},
	}
};
resource 'MENU' (kEditMenuID) {
	kEditMenuID,
	textMenuProc,
	0x7FFFFF80,
	enabled,
	"Edit",
	{
		"Undo", noIcon, "Z", noMark, plain,
		"ReDo", noIcon, "R", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Cut", noIcon, "X", noMark, plain,
		"Copy", noIcon, "C", noMark, plain,
		"Paste", noIcon, "V", noMark, plain,
		"Clear", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Select All", noIcon, "A", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Shift Left", noIcon, "[", noMark, plain,
		"Shift Right", noIcon, "]", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Find…", noIcon, "F", noMark, plain,
		"Find Again", noIcon, "G", noMark, plain,
		"Enter 'Find' String", noIcon, "E", noMark, plain,
		"Replace…", noIcon, "H", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Goto Line…", noIcon, "J", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Spellcheck…", noIcon, "L", noMark, plain,
	}
};
resource 'Mcmd' (kEditMenuID, purgeable) {
	{
		cmd_Undo,
		cmdRedo,
		msg_Nothing,
		cmd_Cut,
		cmd_Copy,
		cmd_Paste,
		cmd_Clear,
		msg_Nothing,
		cmd_SelectAll,
		msg_Nothing,
		kShiftLeftCmdID,
		kShiftRightCmdID,
		msg_Nothing,
		cmdFind,
		cmdFindAgain,
		cmdEnterFindString,
		kReplaceCmd,
		msg_Nothing,
		kGotoLineCmdID,
		msg_Nothing,
		kSpellCheckCmd,
	}
};
resource 'hmnu' (kEditMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Edit menu.\n\nThis menu contains various commands to apply to the currently selected text. These include clipboard operations, undo/redo, searching, and operations on currently selected objects.",
				"",
				"",
				""
			},
		HMStringItem {
				"Undo the last few command(s). Note LedLineIt! supports multilevel undo, so you can undo several commands. And then redo them. And go back and forth.",
				"",
				"",
				""
			},
		HMStringItem {
				"Redo a previously undone command.",
				"There have been no commands recently undone.",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Move the currently selected text and/or objects to the clipboard. This command removes the currently selected text (or objects) from the document.",
				"There is no non-empty selection in the currently active document.",
				"",
				""
			},
		HMStringItem {
				"Copy the currently selected text and/or objects to the clipboard. This command doesn't affect the contents of the document.",
				"There is no non-empty selection in the currently active document.",
				"",
				""
			},
		HMStringItem {
				"This command obliterates the current selection (if any) and replaces it with the text (or other objects) currently stored on the clipboard.",
				"There is nothing on the clipboard that LedLineIt! knows how to insert into the currently active document.",
				"",
				""
			},
		HMStringItem {
				"This command deletes the contents of the current selection (if any)",
				"There is no non-empty selection in the currently active document.",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Select all the text and objects in the currently active document.",
				"No text to select in the currently active document.",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Shift the selected text to the left by one tab.",
				"There is no non-empty selection in the currently active document.",
				"",
				""
			},
		HMStringItem {
				"Shift the selected text to the right by one tab.",
				"There is no non-empty selection in the currently active document.",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Put up a dialog to enter a string to search for, and some other search parameters. And then search for that string.",
				"",
				"",
				""
			},
		HMStringItem {
				"Search for the same string last specified in a 'Find' command, or specified with 'Enter Find String'.",
				"",
				"",
				""
			},
		HMStringItem {
				"Use the currently selected text as the default string to search for in the next 'Find', or 'Find Again' command.",
				"There is currently no selection in the active document.",
				"",
				""
			},
		HMStringItem {
				"Put up a dialog to enter a string to search for, and some other search parameters, and a string to replace. And then search/replace those strings.",
				"",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Goto the given line (prompted for via dialog box).",
				"There is no currently active document to select a line from.",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Put up a dialog to check for misspelled words in the current document.",
				"",
				"",
				""
			},
	}
};
resource 'MENU' (kOptionsMenuID) {
	kOptionsMenuID,
	textMenuProc,
	0x7FFFFFFF,
	enabled,
	"Options",
	{
		"Font", noIcon, hierarchicalMenu, "\0Xfa", plain,
		"Size", noIcon, hierarchicalMenu, "\0Xfb", plain,
	}
};
resource 'Mcmd' (kOptionsMenuID, purgeable) {
	{
		cmd_FontMenu,
		cmd_SizeMenu,
	}
};
resource 'hmnu' (kOptionsMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Format menu.\n\nThis menu contains commands to change the formatting of text. If there is a selection, these commands apply to that selection. If there is no selection, the formatting will apply to newly typed text after the current insertion point.",
				"",
				"",
				""
			},
		HMStringItem {
				"Fonts under this hierarchical menu.",
				"",
				"",
				""
			},
		HMStringItem {
				"Font sizes are listed under this hierarchical menu.",
				"",
				"",
				""
			},
	}
};
resource 'MENU' (cmd_FontMenu) {
	cmd_FontMenu,
	textMenuProc,
	allEnabled,
	enabled,
	"Font",
	{
	}
};
resource 'Mcmd' (cmd_FontMenu, purgeable) {
	{
	}
};
resource 'hmnu' (cmd_FontMenu, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Apply the given fonts to the currently selected text.",
				"",
				"",
				""
			},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
		HMStringItem {	"Change the selected text to this font.",	"No text selected",	"",	""	},
	}
};
resource 'MENU' (cmd_SizeMenu) {
	cmd_SizeMenu,
	textMenuProc,
	allEnabled,
	enabled,
	"Size",
	{
		"9 pt", noIcon, noKey, noMark, plain,
		"10 pt", noIcon, noKey, noMark, plain,
		"12 pt", noIcon, noKey, noMark, plain,
		"14 pt", noIcon, noKey, noMark, plain,
		"18 pt", noIcon, noKey, noMark, plain,
		"24 pt", noIcon, noKey, noMark, plain,
		"36 pt", noIcon, noKey, noMark, plain,
		"48 pt", noIcon, noKey, noMark, plain,
		"72 pt", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain;
		"Smaller", noIcon, "-", noMark, plain,
		"Larger", noIcon, "=", noMark, plain,
		"-", noIcon, noKey, noMark, plain;
		"Other…", noIcon, noKey, noMark, plain,
	}
};
resource 'Mcmd' (cmd_SizeMenu, purgeable) {
	{
		kFontSize9CmdID,
		kFontSize10CmdID,
		kFontSize12CmdID,
		kFontSize14CmdID,
		kFontSize18CmdID,
		kFontSize24CmdID,
		kFontSize36CmdID,
		kFontSize48CmdID,
		kFontSize72CmdID,
		msg_Nothing,
		kFontSizeSmallerCmdID,
		kFontSizeLargerCmdID,
		msg_Nothing,
		kFontSizeOtherCmdID
	}
};
resource 'hmnu' (cmd_SizeMenu, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Change the size of the font of the currently selected text to one of these sizes.",
				"",
				"",
				""
			},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 9 point.",	"",	"This text (or insertion point) is all 9 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 10 point.",	"",	"This text (or insertion point) is all 10 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 12 point.",	"",	"This text (or insertion point) is all 12 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 14 point.",	"",	"This text (or insertion point) is all 14 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 18 point.",	"",	"This text (or insertion point) is all 18 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 24 point.",	"",	"This text (or insertion point) is all 24 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 36 point.",	"",	"This text (or insertion point) is all 36 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 48 point.",	"",	"This text (or insertion point) is all 48 point.",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 72 point.",	"",	"This text (or insertion point) is all 72 point.",	""	},
		HMSkipItem {},	/* separator line */
		HMStringItem {	"Change the selected text (or newly created text if none selected) to a slightly smaller point size.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to a slightly larger point size.",	"",	"",	""	},
		HMSkipItem {},	/* separator line */
		HMStringItem {	"Change the selected text (or newly created text if none selected) to an arbitrary point size (chosen from a dialog).",	"",	"",	""	},
	}
};
resource 'MENU' (kWindowsMenuID) {
	kWindowsMenuID,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	"Windows",
	{
	}
};
resource 'Mcmd' (kWindowsMenuID, purgeable) {
	{
	}
};
resource 'hmnu' (kWindowsMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Windows menu.\n\nThis menu contains a list of all open documents. Selecting an item brings that window to the front.",
				"",
				"",
				""
			},
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
		HMStringItem { "Bring this window to the front.", "", "", "" },
	}
};
resource 'hmnu' (kHMHelpMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Browse LedLineIt! help information using your default HTML browser.",
				"",
				"",
				""
			},
	}
};















/*
 ********************************************************************************
 ************************************ Strings ***********************************
 ********************************************************************************
 */
resource 'STR#' (STRx_Untitled, purgeable) {
	{
		"untitled",
		"untitled "
	}
};







/*
 ********************************************************************************
 *********************************** Text Traits ********************************
 ********************************************************************************
 */
resource 'Txtr' (kTextOrForAboutBoxTextTrait, purgeable) {
	9,
	0,
	center,
	srcOr,
	0,
	0,
	0,
	-1,
	"geneva"
};
resource 'Txtr' (kTextEntryFieldTextTrait, purgeable) {
	0,
	0,
	flushDefault,
	srcCopy,
	0,
	0,
	0,
	1,
	""
};
resource 'Txtr' (kStaticTextTrait, purgeable) {
	0,
	0,
	flushDefault,
	srcCopy,
	0,
	0,
	0,
	0,
	""
};
resource 'Txtr' (kButtonTextTrait, purgeable) {
	0,
	0,
	flushDefault,
	srcCopy,
	0,
	0,
	0,
	0,
	""
};
resource 'Txtr' (kCheckBoxAndRBTextTrait, purgeable) {
	0,
	0,
	flushDefault,
	srcCopy,
	0,
	0,
	0,
	0,
	""
};





/*
 ********************************************************************************
 **************************** Alerts and Dialogs ********************************
 ********************************************************************************
 */
resource 'wctb' (kLedStdDlg_AboutBoxID) {
	{
		wContentColor, 51992, 51992, 51992,
		wFrameColor, 0, 0, 0,
		wTextColor, 0, 0, 0,
		wHiliteColor, 0, 0, 0,
		wTitleBarColor, 65535, 65535, 65535
	}
};
resource 'PICT' (kLedStdDlg_AboutBoxID, purgeable) {
	32946,
	{0, 0, 204, 328},
	$"0011 02FF 0C00 FFFE 0000 0060 0000 0060 0000 0000 0000 0111 01B6 0000 0000 001E"
	$"0001 000A 8001 8001 7FFF 7FFF 009A 0000 00FF 86D8 0000 0000 0111 01B6 0000 0004"
	$"0000 0000 0048 0000 0048 0000 0010 0020 0003 0008 0000 0000 04D6 AAD4 04D6 AAD0"
	$"0000 0000 0111 01B6 0000 0000 0111 01B6 0040 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF"
	$"81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 0024 92CE 009C 81CE 81CE BCCE 00FF"
	$"92CF 009A 81CF 81CF BCCF 00FF 92CE 009C 81CE 81CE BCCE 00FF 005A 92CE 0131 63FE"
	$"CE03 9C63 319C CFCE 0263 3131 81CE 029C 3163 81CE FCCE 00FF 92CF 0130 65FE CF03"
	$"9A65 309A CFCF 0265 3030 81CF 029A 3065 81CF FCCF 00FF 92CE 0131 63FE CE03 9C63"
	$"319C CFCE 0263 3131 81CE 029C 3163 81CE FCCE 00FF 0063 92CE 0300 63CE CEFE 3100"
	$"63D0 CE04 9C00 3100 9C83 CE00 9CFE 3100 9C81 CEFC CE00 FF92 CF03 0065 CFCF FE30"
	$"0065 D0CF 049A 0030 009A 83CF 009A FE30 009A 81CF FCCF 00FF 92CE 0300 63CE CEFE"
	$"3100 63D0 CE04 9C00 3100 9C83 CE00 9CFE 3100 9C81 CEFC CE00 FF00 60F4 CEF0 C6FD"
	$"FFF6 C6BF CE03 319C CE31 CDCE 019C 6381 CE02 CE9C 6381 CEF8 CE00 FFF4 CFF0 C3FD"
	$"FFF6 C3BF CF03 309A CF30 CDCF 019A 6581 CF02 CF9A 6581 CFF8 CF00 FFF4 CEF0 C6FD"
	$"FFF6 C6BF CE03 319C CE31 CDCE 019C 6381 CE02 CE9C 6381 CEF8 CE00 FF00 60F4 CE00"
	$"C6E5 84FE C6C0 CE04 9C31 CE31 9CCD CE00 3181 CE02 CE9C 3181 CEF7 CE00 FFF4 CF00"
	$"C3E5 82FE C3C0 CF04 9A30 CF30 9ACD CF00 3081 CF02 CF9A 3081 CFF7 CF00 FFF4 CE00"
	$"C6E5 84FE C6C0 CE04 9C31 CE31 9CCD CE00 3181 CE02 CE9C 3181 CEF7 CE00 FF00 CCF4"
	$"CE00 C6E7 84FC C6CB CEF7 9C04 6363 CE9C 31E3 CE03 6363 3163 EFCE 0163 9CF8 CE03"
	$"9C63 3131 FACE 019C 9CE6 CE01 9C9C F6CE 0131 63C0 CE01 6363 FBCE 019C 6381 CE02"
	$"CECE FFF4 CF00 C3E7 82FC C3CB CFF7 9A04 6565 CF9A 30E3 CF03 6565 3065 EFCF 0165"
	$"9AF8 CF03 9A65 3030 FACF 019A 9AE6 CF01 9A9A F6CF 0130 65C0 CF01 6565 FBCF 019A"
	$"6581 CF02 CFCF FFF4 CE00 C6E7 84FC C6CB CEF7 9C04 6363 CE9C 31E3 CE03 6363 3163"
	$"EFCE 0163 9CF8 CE03 9C63 3131 FACE 019C 9CE6 CE01 9C9C F6CE 0131 63C0 CE01 6363"
	$"FBCE 019C 6381 CE02 CECE FF01 08F4 CE00 C6FC 84EF 00FD 8403 C684 C684 CECE 009C"
	$"F600 0563 9CCE CE31 63E5 CE05 9C31 6331 009C EFCE 0100 9CFA CE05 9C63 3163 0063"
	$"FBCE 0231 009C EACE 019C 63FD 0000 9CF8 CE02 6300 31C0 CE01 009C FCCE 0163 0081"
	$"CEFE CE00 FFF4 CF00 C3FC 82EF 00FD 8203 C382 C382 CECF 009A F600 0565 9ACF CF30"
	$"65E5 CF05 9A30 6530 009A EFCF 0100 9AFA CF05 9A65 3065 0065 FBCF 0230 009A EACF"
	$"019A 65FD 0000 9AF8 CF02 6500 30C0 CF01 009A FCCF 0165 0081 CFFE CF00 FFF4 CE00"
	$"C6FC 84EF 00FD 8403 C684 C684 CECE 009C F600 0563 9CCE CE31 63E5 CE05 9C31 6331"
	$"009C EFCE 0100 9CFA CE05 9C63 3163 0063 FBCE 0231 009C EACE 019C 63FD 0000 9CF8"
	$"CE02 6300 31C0 CE01 009C FCCE 0163 0081 CEFE CE00 FF01 02F4 CE00 C6FC 8400 00F1"
	$"FF00 00FE 8404 C684 C684 84CF CE01 6363 FDCE 0100 31F8 CE01 009C E6CE 019C 31EB"
	$"CE01 9C00 F9CE 0131 9CF8 CE02 9C31 9CEA CE06 9C63 9CCE CE00 00F6 CE01 0031 C1CE"
	$"019C 00FB CE00 3181 CEFD CE00 FFF4 CF00 C3FC 8200 00F1 FF00 00FE 8204 C382 C382"
	$"82CF CF01 6565 FDCF 0100 30F8 CF01 009A E6CF 019A 30EB CF01 9A00 F9CF 0130 9AF8"
	$"CF02 9A30 9AEA CF06 9A65 9ACF CF00 00F6 CF01 0030 C1CF 019A 00FB CF00 3081 CFFD"
	$"CF00 FFF4 CE00 C6FC 8400 00F1 FF00 00FE 8404 C684 C684 84CF CE01 6363 FDCE 0100"
	$"31F8 CE01 009C E6CE 019C 31EB CE01 9C00 F9CE 0131 9CF8 CE02 9C31 9CEA CE06 9C63"
	$"9CCE CE00 00F6 CE01 0031 C1CE 019C 00FB CE00 3181 CEFD CE00 FF01 32F4 CE00 C6FC"
	$"8402 00FF 00F3 FF08 0084 84C6 84C6 84C6 84D0 CE01 319C FCCE 0100 31F9 CE02 9C00"
	$"9CE6 CE01 009C EBCE 0163 00FA CE01 6331 DECE 019C 31FD CE01 0031 FECE 029C 009C"
	$"FCCE 0100 31F7 CE01 6331 E8CE 0131 31E8 CE01 6300 FCCE 009C FCCE 0100 6383 CE00"
	$"FFF4 CF00 C3FC 8202 00FF 00F3 FF08 0082 82C3 82C3 82C3 82D0 CF01 309A FCCF 0100"
	$"30F9 CF02 9A00 9AE6 CF01 009A EBCF 0165 00FA CF01 6530 DECF 019A 30FD CF01 0030"
	$"FECF 029A 009A FCCF 0100 30F7 CF01 6530 E8CF 0130 30E8 CF01 6500 FCCF 009A FCCF"
	$"0100 6583 CF00 FFF4 CE00 C6FC 8402 00FF 00F3 FF08 0084 84C6 84C6 84C6 84D0 CE01"
	$"319C FCCE 0100 31F9 CE02 9C00 9CE6 CE01 009C EBCE 0163 00FA CE01 6331 DECE 019C"
	$"31FD CE01 0031 FECE 029C 009C FCCE 0100 31F7 CE01 6331 E8CE 0131 31E8 CE01 6300"
	$"FCCE 009C FCCE 0100 6383 CE00 FF01 3EF4 CE00 C6FC 8400 00FC FF00 00FD FF00 00FC"
	$"FF08 0084 C684 C684 C6C6 84D1 CE01 9C31 FCCE 029C 0031 F9CE 019C 00E6 CE01 6300"
	$"EACE 0163 31FA CE01 0063 DECE 0131 9CFE CE02 9C00 31FE CE02 6300 9CFC CE01 0063"
	$"F7CE 0100 63E8 CE01 3131 E8CE 0131 31F7 CE02 9C00 6383 CE00 FFF4 CF00 C3FC 8200"
	$"00FC FF00 00FD FF00 00FC FF08 0082 C382 C382 C3C3 82D1 CF01 9A30 FCCF 029A 0030"
	$"F9CF 019A 00E6 CF01 6500 EACF 0165 30FA CF01 0065 DECF 0130 9AFE CF02 9A00 30FE"
	$"CF02 6500 9AFC CF01 0065 F7CF 0100 65E8 CF01 3030 E8CF 0130 30F7 CF02 9A00 6583"
	$"CF00 FFF4 CE00 C6FC 8400 00FC FF00 00FD FF00 00FC FF08 0084 C684 C684 C6C6 84D1"
	$"CE01 9C31 FCCE 029C 0031 F9CE 019C 00E6 CE01 6300 EACE 0163 31FA CE01 0063 DECE"
	$"0131 9CFE CE02 9C00 31FE CE02 6300 9CFC CE01 0063 F7CE 0100 63E8 CE01 3131 E8CE"
	$"0131 31F7 CE02 9C00 6383 CE00 FF02 8BF4 CE00 C6FC 8402 00FF 00FE FF00 00FD FF06"
	$"00FF FF00 FFFF 00FA C600 84D1 CE01 3131 FCCE 029C 0063 F9CE 0763 00CE CE9C 6300"
	$"31FB CE02 3100 00F5 CE01 6300 F6CE 0363 3100 9CFC CE04 9C31 3100 31FB CE02 9C00"
	$"63F8 CE02 6300 63FE CE02 6300 63FE CE02 3100 63FC CE07 9C31 0031 CECE 009C FECE"
	$"079C 0063 CECE 3131 00FD 3103 9CCE 0063 F9CE 039C 3100 00FD 31FE CE08 6331 009C"
	$"CECE 9C31 31FE CE06 9C63 00CE 6331 00FD 3100 63F8 CE03 6300 009C FCCE 049C 6331"
	$"0031 FDCE 0763 009C CECE 3131 00FD 3100 9CFE CE03 6300 0031 FECE 089C 3131 CECE"
	$"9C00 009C 9ACE 00FF F4CF 00C3 FC82 0200 FF00 FEFF 0000 FDFF 0600 FFFF 00FF FF00"
	$"FAC3 0082 D1CF 0130 30FC CF02 9A00 65F9 CF07 6500 CFCF 9A65 0030 FBCF 0230 0000"
	$"F5CF 0165 00F6 CF03 6530 009A FCCF 049A 3030 0030 FBCF 029A 0065 F8CF 0265 0065"
	$"FECF 0265 0065 FECF 0230 0065 FCCF 079A 3000 30CF CF00 9AFE CF07 9A00 65CF CF30"
	$"3000 FD30 039A CF00 65F9 CF03 9A30 0000 FD30 FECF 0865 3000 9ACF CF9A 3030 FECF"
	$"069A 6500 CF65 3000 FD30 0065 F8CF 0365 0000 9AFC CF04 9A65 3000 30FD CF07 6500"
	$"9ACF CF30 3000 FD30 009A FECF 0365 0000 30FE CF08 9A30 30CF CF9A 0000 9A9A CF00"
	$"FFF4 CE00 C6FC 8402 00FF 00FE FF00 00FD FF06 00FF FF00 FFFF 00FA C600 84D1 CE01"
	$"3131 FCCE 029C 0063 F9CE 0763 00CE CE9C 6300 31FB CE02 3100 00F5 CE01 6300 F6CE"
	$"0363 3100 9CFC CE04 9C31 3100 31FB CE02 9C00 63F8 CE02 6300 63FE CE02 6300 63FE"
	$"CE02 3100 63FC CE07 9C31 0031 CECE 009C FECE 079C 0063 CECE 3131 00FD 3103 9CCE"
	$"0063 F9CE 039C 3100 00FD 31FE CE08 6331 009C CECE 9C31 31FE CE06 9C63 00CE 6331"
	$"00FD 3100 63F8 CE03 6300 009C FCCE 049C 6331 0031 FDCE 0763 009C CECE 3131 00FD"
	$"3100 9CFE CE03 6300 0031 FECE 089C 3131 CECE 9C00 009C 9ACE 00FF 0294 F4CE 00C6"
	$"FC84 0300 FF00 FFF7 00FE FF03 0084 C684 FDC6 0084 D1CE 0131 00FC CE02 9C00 63F9"
	$"CE07 6300 CECE 6363 3100 FCCE 0431 6331 009C F6CE 0131 00F7 CEFE 6301 0063 FDCE"
	$"0563 639C 6300 31FB CE02 9C00 63F9 CE0F 6363 0063 CECE 6363 0063 CECE 3163 0031"
	$"FDCE 089C 6363 3100 CE9C 0031 FECE 079C 0063 CE9C 6331 00FE 6304 9CCE CE00 63F9"
	$"CE03 6363 0031 FE63 FECE FE63 1100 63CE CE63 6300 63CE CE31 639C CE63 6300 31FE"
	$"63F8 CEFE 6301 0063 FDCE 0563 9C9C 6300 63FE CE0C 6331 0063 CE9C 6331 0031 6363"
	$"9CFE CE0F 639C 9C00 0063 CE9C 6300 31CE 9C00 3163 99CE 00FF F4CF 00C3 FC82 0300"
	$"FF00 FFF7 00FE FF03 0082 C382 FDC3 0082 D1CF 0130 00FC CF02 9A00 65F9 CF07 6500"
	$"CFCF 6565 3000 FCCF 0430 6530 009A F6CF 0130 00F7 CFFE 6501 0065 FDCF 0565 659A"
	$"6500 30FB CF02 9A00 65F9 CF0F 6565 0065 CFCF 6565 0065 CFCF 3065 0030 FDCF 089A"
	$"6565 3000 CF9A 0030 FECF 079A 0065 CF9A 6530 00FE 6504 9ACF CF00 65F9 CF03 6565"
	$"0030 FE65 FECF FE65 1100 65CF CF65 6500 65CF CF30 659A CF65 6500 30FE 65F8 CFFE"
	$"6501 0065 FDCF 0565 9A9A 6500 65FE CF0C 6530 0065 CF9A 6530 0030 6565 9AFE CF0F"
	$"659A 9A00 0065 CF9A 6500 30CF 9A00 3065 99CF 00FF F4CE 00C6 FC84 0300 FF00 FFF7"
	$"00FE FF03 0084 C684 FDC6 0084 D1CE 0131 00FC CE02 9C00 63F9 CE07 6300 CECE 6363"
	$"3100 FCCE 0431 6331 009C F6CE 0131 00F7 CEFE 6301 0063 FDCE 0563 639C 6300 31FB"
	$"CE02 9C00 63F9 CE0F 6363 0063 CECE 6363 0063 CECE 3163 0031 FDCE 089C 6363 3100"
	$"CE9C 0031 FECE 079C 0063 CE9C 6331 00FE 6304 9CCE CE00 63F9 CE03 6363 0031 FE63"
	$"FECE FE63 1100 63CE CE63 6300 63CE CE31 639C CE63 6300 31FE 63F8 CEFE 6301 0063"
	$"FDCE 0563 9C9C 6300 63FE CE0C 6331 0063 CE9C 6331 0031 6363 9CFE CE0F 639C 9C00"
	$"0063 CE9C 6300 31CE 9C00 3163 99CE 00FF 0294 F4CE 00C6 FC84 0200 FF00 FEFF 0000"
	$"FDFF 0000 FCFF 0200 C684 FDC6 01FF 84D1 CE01 3163 FCCE 0263 009C F9CE 0731 319C"
	$"639C CE31 00FD CE00 31FE CE00 00F5 CE01 3100 F8CE 0563 9CCE CE31 9CFE CE01 639C"
	$"FECE 0100 63FB CE02 9C00 63FA CE10 639C 9C00 63CE 9C63 CE00 639C 63CE CE00 31FE"
	$"CE09 9C63 CECE 9C31 CE9C 009C FECE 0263 009C FECE 0131 31FB CE01 0063 F8CE 029C"
	$"009C FCCE 0E63 9CCE CE31 9CCE 9C9C CE31 31CE 6363 FDCE 029C 009C F6CE 0563 9CCE"
	$"CE31 63FE CE01 319C FECE 0300 63CE CEFE 9C01 009C FECE 0163 00FB CE0D 319C CECE"
	$"9C00 31CE 63CE 3131 9C31 96CE 00FF F4CF 00C3 FC82 0200 FF00 FEFF 0000 FDFF 0000"
	$"FCFF 0200 C382 FDC3 01FF 82D1 CF01 3065 FCCF 0265 009A F9CF 0730 309A 659A CF30"
	$"00FD CF00 30FE CF00 00F5 CF01 3000 F8CF 0565 9ACF CF30 9AFE CF01 659A FECF 0100"
	$"65FB CF02 9A00 65FA CF10 659A 9A00 65CF 9A65 CF00 659A 65CF CF00 30FE CF09 9A65"
	$"CFCF 9A30 CF9A 009A FECF 0265 009A FECF 0130 30FB CF01 0065 F8CF 029A 009A FCCF"
	$"0E65 9ACF CF30 9ACF 9A9A CF30 30CF 6565 FDCF 029A 009A F6CF 0565 9ACF CF30 65FE"
	$"CF01 309A FECF 0300 65CF CFFE 9A01 009A FECF 0165 00FB CF0D 309A CFCF 9A00 30CF"
	$"65CF 3030 9A30 96CF 00FF F4CE 00C6 FC84 0200 FF00 FEFF 0000 FDFF 0000 FCFF 0200"
	$"C684 FDC6 01FF 84D1 CE01 3163 FCCE 0263 009C F9CE 0731 319C 639C CE31 00FD CE00"
	$"31FE CE00 00F5 CE01 3100 F8CE 0563 9CCE CE31 9CFE CE01 639C FECE 0100 63FB CE02"
	$"9C00 63FA CE10 639C 9C00 63CE 9C63 CE00 639C 63CE CE00 31FE CE09 9C63 CECE 9C31"
	$"CE9C 009C FECE 0263 009C FECE 0131 31FB CE01 0063 F8CE 029C 009C FCCE 0E63 9CCE"
	$"CE31 9CCE 9C9C CE31 31CE 6363 FDCE 029C 009C F6CE 0563 9CCE CE31 63FE CE01 319C"
	$"FECE 0300 63CE CEFE 9C01 009C FECE 0163 00FB CE0D 319C CECE 9C00 31CE 63CE 3131"
	$"9C31 96CE 00FF 025E F4CE 00C6 FC84 0200 FF00 FEFF 0000 FDFF 0700 FFFF 00FF FF00"
	$"84FB C600 84CA CE02 6300 9CF9 CE02 3131 63FE CE01 3131 FECE 059C 9CCE CE9C 63F5"
	$"CE01 3100 F9CE 019C 63FE CE00 63FE CE01 6363 FDCE 0100 63FB CE02 9C00 63F8 CE02"
	$"6300 9CFD CE02 0063 63FE CE01 0063 FECE 0563 9CCE CE63 9CFA CE02 6300 9CFE CE01"
	$"3131 FBCE 0100 9CF8 CE02 9C00 9CFD CE01 9C63 FECE 0063 FCCE 039C 0063 63FC CE02"
	$"9C00 9CF6 CE00 31FE CE00 31FE CE01 6363 FDCE 0100 63FD CE02 6300 9CFE CE01 6300"
	$"FCCE 019C 63FD CE01 0031 FECE FE31 95CE 00FF F4CF 00C3 FC82 0200 FF00 FEFF 0000"
	$"FDFF 0700 FFFF 00FF FF00 82FB C300 82CA CF02 6500 9AF9 CF02 3030 65FE CF01 3030"
	$"FECF 059A 9ACF CF9A 65F5 CF01 3000 F9CF 019A 65FE CF00 65FE CF01 6565 FDCF 0100"
	$"65FB CF02 9A00 65F8 CF02 6500 9AFD CF02 0065 65FE CF01 0065 FECF 0565 9ACF CF65"
	$"9AFA CF02 6500 9AFE CF01 3030 FBCF 0100 9AF8 CF02 9A00 9AFD CF01 9A65 FECF 0065"
	$"FCCF 039A 0065 65FC CF02 9A00 9AF6 CF00 30FE CF00 30FE CF01 6565 FDCF 0100 65FD"
	$"CF02 6500 9AFE CF01 6500 FCCF 019A 65FD CF01 0030 FECF FE30 95CF 00FF F4CE 00C6"
	$"FC84 0200 FF00 FEFF 0000 FDFF 0700 FFFF 00FF FF00 84FB C600 84CA CE02 6300 9CF9"
	$"CE02 3131 63FE CE01 3131 FECE 059C 9CCE CE9C 63F5 CE01 3100 F9CE 019C 63FE CE00"
	$"63FE CE01 6363 FDCE 0100 63FB CE02 9C00 63F8 CE02 6300 9CFD CE02 0063 63FE CE01"
	$"0063 FECE 0563 9CCE CE63 9CFA CE02 6300 9CFE CE01 3131 FBCE 0100 9CF8 CE02 9C00"
	$"9CFD CE01 9C63 FECE 0063 FCCE 039C 0063 63FC CE02 9C00 9CF6 CE00 31FE CE00 31FE"
	$"CE01 6363 FDCE 0100 63FD CE02 6300 9CFE CE01 6300 FCCE 019C 63FD CE01 0031 FECE"
	$"FE31 95CE 00FF 0240 F4CE 00C6 FC84 0000 F5FF 0400 00FF FF00 FCC6 02FF FF84 CACE"
	$"0131 00F8 CE02 0000 9CFE CE01 3131 FECE 0431 9CCE 9C63 F4CE 0131 00F9 CE05 6363"
	$"CE9C 639C FECE 0100 9CFE CE02 9C00 9CFB CE02 9C00 63F8 CE01 3100 FDCE 029C 0000"
	$"FDCE 0100 63FE CE04 00CE CE63 63F9 CE01 3100 FDCE 0100 31FC CE02 9C00 9CF8 CE02"
	$"6300 9CFD CE05 6363 CE9C 639C FBCE 0100 00FB CE01 6300 F6CE 0431 9CCE 9C31 FECE"
	$"029C 009C FECE 0263 0063 FDCE 0163 00FD CE01 3100 FCCE 0100 63FD CE01 0063 FECE"
	$"0200 009C 95CE 00FF F4CF 00C3 FC82 0000 F5FF 0400 00FF FF00 FCC3 02FF FF82 CACF"
	$"0130 00F8 CF02 0000 9AFE CF01 3030 FECF 0430 9ACF 9A65 F4CF 0130 00F9 CF05 6565"
	$"CF9A 659A FECF 0100 9AFE CF02 9A00 9AFB CF02 9A00 65F8 CF01 3000 FDCF 029A 0000"
	$"FDCF 0100 65FE CF04 00CF CF65 65F9 CF01 3000 FDCF 0100 30FC CF02 9A00 9AF8 CF02"
	$"6500 9AFD CF05 6565 CF9A 659A FBCF 0100 00FB CF01 6500 F6CF 0430 9ACF 9A30 FECF"
	$"029A 009A FECF 0265 0065 FDCF 0165 00FD CF01 3000 FCCF 0100 65FD CF01 0065 FECF"
	$"0200 009A 95CF 00FF F4CE 00C6 FC84 0000 F5FF 0400 00FF FF00 FCC6 02FF FF84 CACE"
	$"0131 00F8 CE02 0000 9CFE CE01 3131 FECE 0431 9CCE 9C63 F4CE 0131 00F9 CE05 6363"
	$"CE9C 639C FECE 0100 9CFE CE02 9C00 9CFB CE02 9C00 63F8 CE01 3100 FDCE 029C 0000"
	$"FDCE 0100 63FE CE04 00CE CE63 63F9 CE01 3100 FDCE 0100 31FC CE02 9C00 9CF8 CE02"
	$"6300 9CFD CE05 6363 CE9C 639C FBCE 0100 00FB CE01 6300 F6CE 0431 9CCE 9C31 FECE"
	$"029C 009C FECE 0263 0063 FDCE 0163 00FD CE01 3100 FCCE 0100 63FD CE01 0063 FECE"
	$"0200 009C 95CE 00FF 0225 F4CE 00C6 FC84 0000 FEFF 0000 FDFF 0100 00FB FF00 00FA"
	$"C600 84CA CE01 3100 F8CE 0100 31FD CE01 0063 FECE 0300 3131 9CF3 CE01 0063 F9CE"
	$"0300 3131 63FD CE01 6331 FDCE 0231 009C FBCE 019C 00F7 CE01 3100 FDCE 029C 0063"
	$"FECE 059C 009C CECE 63FE 3100 9CF8 CE01 3100 FDCE 0100 63FC CE01 9C63 F7CE 0163"
	$"00FC CE03 0031 3163 F9CE 0131 00FB CE01 6300 F6CE FE31 0063 FDCE 0163 00FD CE02"
	$"3100 9CFD CE01 3131 FDCE 0131 31FD CE02 9C00 9CFD CE01 0063 FECE 0100 3194 CE00"
	$"FFF4 CF00 C3FC 8200 00FE FF00 00FD FF01 0000 FBFF 0000 FAC3 0082 CACF 0130 00F8"
	$"CF01 0030 FDCF 0100 65FE CF03 0030 309A F3CF 0100 65F9 CF03 0030 3065 FDCF 0165"
	$"30FD CF02 3000 9AFB CF01 9A00 F7CF 0130 00FD CF02 9A00 65FE CF05 9A00 9ACF CF65"
	$"FE30 009A F8CF 0130 00FD CF01 0065 FCCF 019A 65F7 CF01 6500 FCCF 0300 3030 65F9"
	$"CF01 3000 FBCF 0165 00F6 CFFE 3000 65FD CF01 6500 FDCF 0230 009A FDCF 0130 30FD"
	$"CF01 3030 FDCF 029A 009A FDCF 0100 65FE CF01 0030 94CF 00FF F4CE 00C6 FC84 0000"
	$"FEFF 0000 FDFF 0100 00FB FF00 00FA C600 84CA CE01 3100 F8CE 0100 31FD CE01 0063"
	$"FECE 0300 3131 9CF3 CE01 0063 F9CE 0300 3131 63FD CE01 6331 FDCE 0231 009C FBCE"
	$"019C 00F7 CE01 3100 FDCE 029C 0063 FECE 059C 009C CECE 63FE 3100 9CF8 CE01 3100"
	$"FDCE 0100 63FC CE01 9C63 F7CE 0163 00FC CE03 0031 3163 F9CE 0131 00FB CE01 6300"
	$"F6CE FE31 0063 FDCE 0163 00FD CE02 3100 9CFD CE01 3131 FDCE 0131 31FD CE02 9C00"
	$"9CFD CE01 0063 FECE 0100 3194 CE00 FF02 28F4 CE00 C6FC 8403 00FF FF00 FDFF 0300"
	$"FFFF 00FC FF00 00FD C603 FFC6 FF84 CACE 0100 00F8 CE01 0063 FDCE 0600 63CE CE63"
	$"0063 F2CE 029C 0063 FACE 029C 0031 FBCE 0131 31FE CE03 9C31 009C FBCE 0131 31F7"
	$"CE01 0031 FDCE 0263 009C FECE 079C 009C CECE 3100 9CF6 CE01 0000 FECE 029C 0063"
	$"F0CE 0131 00FD CE02 9C00 31F8 CE03 9C31 009C FCCE 0163 00F7 CE03 9C00 639C FCCE"
	$"0100 31FE CE03 9C31 009C FDCE 0131 31FD CE01 3131 FDCE 0263 009C FECE 019C 00FE"
	$"CE02 9C00 6394 CE00 FFF4 CF00 C3FC 8203 00FF FF00 FDFF 0300 FFFF 00FC FF00 00FD"
	$"C303 FFC3 FF82 CACF 0100 00F8 CF01 0065 FDCF 0600 65CF CF65 0065 F2CF 029A 0065"
	$"FACF 029A 0030 FBCF 0130 30FE CF03 9A30 009A FBCF 0130 30F7 CF01 0030 FDCF 0265"
	$"009A FECF 079A 009A CFCF 3000 9AF6 CF01 0000 FECF 029A 0065 F0CF 0130 00FD CF02"
	$"9A00 30F8 CF03 9A30 009A FCCF 0165 00F7 CF03 9A00 659A FCCF 0100 30FE CF03 9A30"
	$"009A FDCF 0130 30FD CF01 3030 FDCF 0265 009A FECF 019A 00FE CF02 9A00 6594 CF00"
	$"FFF4 CE00 C6FC 8403 00FF FF00 FDFF 0300 FFFF 00FC FF00 00FD C603 FFC6 FF84 CACE"
	$"0100 00F8 CE01 0063 FDCE 0600 63CE CE63 0063 F2CE 029C 0063 FACE 029C 0031 FBCE"
	$"0131 31FE CE03 9C31 009C FBCE 0131 31F7 CE01 0031 FDCE 0263 009C FECE 079C 009C"
	$"CECE 3100 9CF6 CE01 0000 FECE 029C 0063 F0CE 0131 00FD CE02 9C00 31F8 CE03 9C31"
	$"009C FCCE 0163 00F7 CE03 9C00 639C FCCE 0100 31FE CE03 9C31 009C FDCE 0131 31FD"
	$"CE01 3131 FDCE 0263 009C FECE 019C 00FE CE02 9C00 6394 CE00 FF02 73F4 CE00 C6FC"
	$"8401 00FF FE00 FEFF 0500 FFFF 00FF 00FE FF00 00FE C600 FFFE C600 84CA CE01 0063"
	$"F9CE 029C 009C FECE 069C 009C CE9C 6300 FDCE 0163 9CF7 CE01 6331 F9CE 029C 009C"
	$"FECE 0D9C 63CE 0031 CECE 9C63 6300 CE9C 9CFD CE01 319C F7CE 0700 63CE 63CE CE63"
	$"00FD CE06 9C00 CE9C CE31 31FD CE00 31FA CE01 0063 FECE 029C 009C F0CE 0131 31FD"
	$"CE02 9C00 9CFE CE01 9C63 FDCE 0363 CE31 00FC CE01 3131 F7CE 029C 0063 FECE 049C"
	$"9CCE 0031 FECE 0431 6300 CE9C FECE 0700 63CE 9CCE CE00 63FD CE02 6300 63FE CE01"
	$"319C FECE 029C 009C 94CE 00FF F4CF 00C3 FC82 0100 FFFE 00FE FF05 00FF FF00 FF00"
	$"FEFF 0000 FEC3 00FF FEC3 0082 CACF 0100 65F9 CF02 9A00 9AFE CF06 9A00 9ACF 9A65"
	$"00FD CF01 659A F7CF 0165 30F9 CF02 9A00 9AFE CF0D 9A65 CF00 30CF CF9A 6565 00CF"
	$"9A9A FDCF 0130 9AF7 CF07 0065 CF65 CFCF 6500 FDCF 069A 00CF 9ACF 3030 FDCF 0030"
	$"FACF 0100 65FE CF02 9A00 9AF0 CF01 3030 FDCF 029A 009A FECF 019A 65FD CF03 65CF"
	$"3000 FCCF 0130 30F7 CF02 9A00 65FE CF04 9A9A CF00 30FE CF04 3065 00CF 9AFE CF07"
	$"0065 CF9A CFCF 0065 FDCF 0265 0065 FECF 0130 9AFE CF02 9A00 9A94 CF00 FFF4 CE00"
	$"C6FC 8401 00FF FE00 FEFF 0500 FFFF 00FF 00FE FF00 00FE C600 FFFE C600 84CA CE01"
	$"0063 F9CE 029C 009C FECE 069C 009C CE9C 6300 FDCE 0163 9CF7 CE01 6331 F9CE 029C"
	$"009C FECE 0D9C 63CE 0031 CECE 9C63 6300 CE9C 9CFD CE01 319C F7CE 0700 63CE 63CE"
	$"CE63 00FD CE06 9C00 CE9C CE31 31FD CE00 31FA CE01 0063 FECE 029C 009C F0CE 0131"
	$"31FD CE02 9C00 9CFE CE01 9C63 FDCE 0363 CE31 00FC CE01 3131 F7CE 029C 0063 FECE"
	$"049C 9CCE 0031 FECE 0431 6300 CE9C FECE 0700 63CE 9CCE CE00 63FD CE02 6300 63FE"
	$"CE01 319C FECE 029C 009C 94CE 00FF 02A9 F4CE 00C6 FC84 0300 FFFF 00FD FF03 00FF"
	$"FF00 FCFF 0000 FCC6 02FF FF84 CBCE 0131 63F8 CE02 9C00 9CFE CE0B 9C00 9C63 CE63"
	$"0031 CE9C 6363 F9CE 039C 6363 31FA CE1A 6363 9C00 00CE CE9C 31CE CE00 00CE 9C31"
	$"9C63 00CE 31CE 9C63 6331 63FA CE0B 319C CE9C 0063 639C CECE 6300 FDCE 0B63 00CE"
	$"63CE 6300 63CE CE31 9CFB CE01 3163 FDCE 049C 0063 CE31 FECE 0263 009C F8CE 0C31"
	$"31CE CE31 CE9C 0000 CECE 9C31 FDCE 0E63 9CCE 6300 63CE 63CE CE31 00CE 9C63 F9CE"
	$"2700 319C CE63 63CE CE00 319C 9C31 CE63 009C 63CE CE9C 0063 9C9C CECE 0031 CE63"
	$"9CCE 9C00 31CE CE63 63FD CE02 9C00 9C94 CE00 FFF4 CF00 C3FC 8203 00FF FF00 FDFF"
	$"0300 FFFF 00FC FF00 00FC C302 FFFF 82CB CF01 3065 F8CF 029A 009A FECF 0B9A 009A"
	$"65CF 6500 30CF 9A65 65F9 CF03 9A65 6530 FACF 1A65 659A 0000 CFCF 9A30 CFCF 0000"
	$"CF9A 309A 6500 CF30 CF9A 6565 3065 FACF 0B30 9ACF 9A00 6565 9ACF CF65 00FD CF0B"
	$"6500 CF65 CF65 0065 CFCF 309A FBCF 0130 65FD CF04 9A00 65CF 30FE CF02 6500 9AF8"
	$"CF0C 3030 CFCF 30CF 9A00 00CF CF9A 30FD CF0E 659A CF65 0065 CF65 CFCF 3000 CF9A"
	$"65F9 CF27 0030 9ACF 6565 CFCF 0030 9A9A 30CF 6500 9A65 CFCF 9A00 659A 9ACF CF00"
	$"30CF 659A CF9A 0030 CFCF 6565 FDCF 029A 009A 94CF 00FF F4CE 00C6 FC84 0300 FFFF"
	$"00FD FF03 00FF FF00 FCFF 0000 FCC6 02FF FF84 CBCE 0131 63F8 CE02 9C00 9CFE CE0B"
	$"9C00 9C63 CE63 0031 CE9C 6363 F9CE 039C 6363 31FA CE1A 6363 9C00 00CE CE9C 31CE"
	$"CE00 00CE 9C31 9C63 00CE 31CE 9C63 6331 63FA CE0B 319C CE9C 0063 639C CECE 6300"
	$"FDCE 0B63 00CE 63CE 6300 63CE CE31 9CFB CE01 3163 FDCE 049C 0063 CE31 FECE 0263"
	$"009C F8CE 0C31 31CE CE31 CE9C 0000 CECE 9C31 FDCE 0E63 9CCE 6300 63CE 63CE CE31"
	$"00CE 9C63 F9CE 2700 319C CE63 63CE CE00 319C 9C31 CE63 009C 63CE CE9C 0063 9C9C"
	$"CECE 0031 CE63 9CCE 9C00 31CE CE63 63FD CE02 9C00 9C94 CE00 FF02 AFF4 CE00 C6FC"
	$"8403 00FF FF00 FDFF 0300 FFFF 00FC FF08 00C6 C6FF C6C6 FFFF 84CE CE02 9C31 00FE"
	$"3100 9CFA CE01 6300 FDCE 029C 0031 FECE 0031 FE00 0063 F9CE 0063 FB00 FD31 0400"
	$"63CE CE31 FE00 0031 FECE 0B31 0000 63CE CE31 0031 CE9C 31FC 00FC 3107 009C CECE"
	$"9C00 009C FECE 0131 31FD CE06 6300 63CE CE9C 31FE 00FC CE02 9C31 00FE 3100 9CFE"
	$"CE03 0031 319C FECE 0131 00F7 CE03 6300 3131 FECE 0031 FE00 0631 CECE 9C00 3163"
	$"FECE 0231 0031 FECE 0331 0031 63F8 CE00 63FE 0000 63FE CE08 3100 0031 CECE 6300"
	$"31FE CE03 9C00 0063 FECE 0300 0031 63FE CE04 3100 3131 63FC CE01 6300 93CE 00FF"
	$"F4CF 00C3 FC82 0300 FFFF 00FD FF03 00FF FF00 FCFF 0800 C3C3 FFC3 C3FF FF82 CECF"
	$"029A 3000 FE30 009A FACF 0165 00FD CF02 9A00 30FE CF00 30FE 0000 65F9 CF00 65FB"
	$"00FD 3004 0065 CFCF 30FE 0000 30FE CF0B 3000 0065 CFCF 3000 30CF 9A30 FC00 FC30"
	$"0700 9ACF CF9A 0000 9AFE CF01 3030 FDCF 0665 0065 CFCF 9A30 FE00 FCCF 029A 3000"
	$"FE30 009A FECF 0300 3030 9AFE CF01 3000 F7CF 0365 0030 30FE CF00 30FE 0006 30CF"
	$"CF9A 0030 65FE CF02 3000 30FE CF03 3000 3065 F8CF 0065 FE00 0065 FECF 0830 0000"
	$"30CF CF65 0030 FECF 039A 0000 65FE CF03 0000 3065 FECF 0430 0030 3065 FCCF 0165"
	$"0093 CF00 FFF4 CE00 C6FC 8403 00FF FF00 FDFF 0300 FFFF 00FC FF08 00C6 C6FF C6C6"
	$"FFFF 84CE CE02 9C31 00FE 3100 9CFA CE01 6300 FDCE 029C 0031 FECE 0031 FE00 0063"
	$"F9CE 0063 FB00 FD31 0400 63CE CE31 FE00 0031 FECE 0B31 0000 63CE CE31 0031 CE9C"
	$"31FC 00FC 3107 009C CECE 9C00 009C FECE 0131 31FD CE06 6300 63CE CE9C 31FE 00FC"
	$"CE02 9C31 00FE 3100 9CFE CE03 0031 319C FECE 0131 00F7 CE03 6300 3131 FECE 0031"
	$"FE00 0631 CECE 9C00 3163 FECE 0231 0031 FECE 0331 0031 63F8 CE00 63FE 0000 63FE"
	$"CE08 3100 0031 CECE 6300 31FE CE03 9C00 0063 FECE 0300 0031 63FE CE04 3100 3131"
	$"63FC CE01 6300 93CE 00FF 0252 F4CE 00C6 FC84 0300 FFFF 00FD FF05 00FF FF00 FF00"
	$"FEFF 0400 C6FF C6C6 FEFF 0084 CECE FE9C FE63 F9CE 019C 9CFC CE00 63FC CE02 6363"
	$"9CF7 CEFE 9C01 6363 FC31 009C FDCE 0263 639C FCCE 0163 9CFE CE01 9C63 FECE FE9C"
	$"FE63 FD31 0063 FCCE 0163 9CFD CE00 9CFC CE02 9C63 9CFE CE02 9C63 63FB CEFE 9CFE"
	$"63FD CE01 9C63 FCCE 019C 9CF7 CE02 9C63 63FC CE02 6363 9CFE CE02 6363 9CFC CE00"
	$"63FC CE01 639C F6CE 029C 639C FDCE 029C 639C FECE 029C 639C FECE 029C 639C FDCE"
	$"029C 639C FDCE 029C 6363 FACE 009C 92CE 00FF F4CF 00C3 FC82 0300 FFFF 00FD FF05"
	$"00FF FF00 FF00 FEFF 0400 C3FF C3C3 FEFF 0082 CECF FE9A FE65 F9CF 019A 9AFC CF00"
	$"65FC CF02 6565 9AF7 CFFE 9A01 6565 FC30 009A FDCF 0265 659A FCCF 0165 9AFE CF01"
	$"9A65 FECF FE9A FE65 FD30 0065 FCCF 0165 9AFD CF00 9AFC CF02 9A65 9AFE CF02 9A65"
	$"65FB CFFE 9AFE 65FD CF01 9A65 FCCF 019A 9AF7 CF02 9A65 65FC CF02 6565 9AFE CF02"
	$"6565 9AFC CF00 65FC CF01 659A F6CF 029A 659A FDCF 029A 659A FECF 029A 659A FECF"
	$"029A 659A FDCF 029A 659A FDCF 029A 6565 FACF 009A 92CF 00FF F4CE 00C6 FC84 0300"
	$"FFFF 00FD FF05 00FF FF00 FF00 FEFF 0400 C6FF C6C6 FEFF 0084 CECE FE9C FE63 F9CE"
	$"019C 9CFC CE00 63FC CE02 6363 9CF7 CEFE 9C01 6363 FC31 009C FDCE 0263 639C FCCE"
	$"0163 9CFE CE01 9C63 FECE FE9C FE63 FD31 0063 FCCE 0163 9CFD CE00 9CFC CE02 9C63"
	$"9CFE CE02 9C63 63FB CEFE 9CFE 63FD CE01 9C63 FCCE 019C 9CF7 CE02 9C63 63FC CE02"
	$"6363 9CFE CE02 6363 9CFC CE00 63FC CE01 639C F6CE 029C 639C FDCE 029C 639C FECE"
	$"029C 639C FECE 029C 639C FDCE 029C 639C FDCE 029C 6363 FACE 009C 92CE 00FF 006C"
	$"F4CE 00C6 FC84 0000 F9FF 0400 00FF FF00 FEFF 0000 FCC6 02FF FF84 81CE 81CE 81CE"
	$"F9CE 00FF F4CF 00C3 FC82 0000 F9FF 0400 00FF FF00 FEFF 0000 FCC3 02FF FF82 81CF"
	$"81CF 81CF F9CF 00FF F4CE 00C6 FC84 0000 F9FF 0400 00FF FF00 FEFF 0000 FCC6 02FF"
	$"FF84 81CE 81CE 81CE F9CE 00FF 006A F4CE 00C6 FC84 0100 00FE FF00 00FC FF00 84FC"
	$"FF00 00FD C6FE FF00 8481 CE81 CE81 CEF9 CE00 FFF4 CF00 C3FC 8201 0000 FEFF 0000"
	$"FCFF 0082 FCFF 0000 FDC3 FEFF 0082 81CF 81CF 81CF F9CF 00FF F4CE 00C6 FC84 0000"
	$"F7FF 0084 FCFF 0000 FDC6 FEFF 0084 81CE 81CE 81CE F9CE 00FF 006A F4CE 00C6 FC84"
	$"0100 FFFE 00F9 FF00 C6FE FF00 00FE C6FD FF00 8481 CE81 CE81 CEF9 CE00 FFF4 CF00"
	$"C3FC 8201 00FF FE00 FBFF 0200 00C3 FEFF 0000 FEC3 FDFF 0082 81CF 81CF 81CF F9CF"
	$"00FF F4CE 00C6 FC84 0000 F7FF 0200 00C6 FEFF 0000 FEC6 FDFF 0084 81CE 81CE 81CE"
	$"F9CE 00FF 0067 F4CE 00C6 FC84 0300 FFFF 00F7 FF05 84C6 FF00 C6C6 FCFF 0084 81CE"
	$"81CE 81CE F9CE 00FF F4CF 00C3 FC82 0300 FFFF 00FA FFFE 0005 82C3 FF00 C3C3 FCFF"
	$"0082 81CF 81CF 81CF F9CF 00FF F4CE 00C6 FC84 0000 F7FF FE00 0584 C6FF 00C6 C6FC"
	$"FF00 8481 CE81 CE81 CEF9 CE00 FF00 61F4 CE00 C6FD 8404 C600 FFFF 00F5 FF02 8400"
	$"C6FB FF00 8481 CE81 CE81 CEF9 CE00 FFF4 CF00 C3FD 8204 C300 FFFF 00FA FFFC 0002"
	$"8200 C3FB FF00 8281 CF81 CF81 CFF9 CF00 FFF4 CE00 C6FD 8401 C600 F7FF FC00 0284"
	$"00C6 FBFF 0084 81CE 81CE 81CE F9CE 00FF 0056 F4CE 00C6 FE84 05C6 C600 FFFF 00EC"
	$"FF00 8481 CE81 CE81 CEF9 CE00 FFF4 CF00 C3FE 8207 C3C3 00FF FF00 FFFF F500 FAFF"
	$"0082 81CF 81CF 81CF F9CF 00FF F4CE 00C6 FE84 02C6 C600 FCFF F500 FAFF 0084 81CE"
	$"81CE 81CE F9CE 00FF 0064 F4CE 02C6 8484 FEC6 0300 FFFF 00F5 FF02 8400 C6FB FF00"
	$"8481 CE81 CE81 CEF9 CE00 FFF4 CF02 C382 82FE C303 00FF FF00 FAFF FC00 0282 00C3"
	$"FBFF 0082 81CF 81CF 81CF F9CF 00FF F4CE 02C6 8484 FEC6 0000 F7FF FC00 0284 00C6"
	$"FBFF 0084 81CE 81CE 81CE F9CE 00FF 0061 F4CE 01C6 84FD C603 00FF FF00 F6FF 02C6"
	$"FF00 FAFF 0084 81CE 81CE 81CE F9CE 00FF F4CF 01C3 82FD C303 00FF FF00 FAFF FD00"
	$"02C3 FF00 FAFF 0082 81CF 81CF 81CF F9CF 00FF F4CE 01C6 84FD C600 00F7 FFFD 0002"
	$"C6FF 00FA FF00 8481 CE81 CE81 CEF9 CE00 FF00 5EF4 CEFB C601 00FF FE00 F9FF 0484"
	$"C6FF FF00 FAFF 0084 81CE 81CE 81CE F9CE 00FF F4CF FBC3 0100 FFFE 00FB FF06 0000"
	$"82C3 FFFF 00FA FF00 8281 CF81 CF81 CFF9 CF00 FFF4 CEFB C600 00F7 FF06 0000 84C6"
	$"FFFF 00FA FF00 8481 CE81 CE81 CEF9 CE00 FF00 60F4 CEFB C601 0000 FEFF 0000 FBFF"
	$"00C6 FDFF 0000 FAFF 0084 81CE 81CE 81CE F9CE 00FF F4CF FBC3 0100 00FE FF00 00FC"
	$"FF01 00C3 FDFF 0000 FAFF 0082 81CF 81CF 81CF F9CF 00FF F4CE FBC6 0000 F7FF 0100"
	$"C6FD FF00 00FA FF00 8481 CE81 CE81 CEF9 CE00 FF00 3CF4 CEFB C6EF 00FA FF00 0081"
	$"CE81 CE81 CEF9 CE00 FFF4 CFFB C3EF 00FA FF00 0081 CF81 CF81 CFF9 CF00 FFF4 CEFB"
	$"C6EF 00FA FF00 0081 CE81 CE81 CEF9 CE00 FF00 51F4 CEF7 C603 FFC6 FFC6 FEFF 00C6"
	$"F4FF 0000 81CE 81CE 81CE F9CE 00FF F4CF F7C3 03FF C3FF C3FE FF00 C3F4 FF00 0081"
	$"CF81 CF81 CFF9 CF00 FFF4 CEF7 C603 FFC6 FFC6 FEFF 00C6 F4FF 0000 81CE 81CE 81CE"
	$"F9CE 00FF 004B F4CE F8C6 05FF C6FF C6FF C6F1 FF00 0081 CE81 CE81 CEF9 CE00 FFF4"
	$"CFF8 C305 FFC3 FFC3 FFC3 F1FF 0000 81CF 81CF 81CF F9CF 00FF F4CE F8C6 05FF C6FF"
	$"C6FF C6F1 FF00 0081 CE81 CE81 CEF9 CE00 FF00 45F4 CEFE C601 84C6 E984 0200 8400"
	$"81CE 81CE 81CE F9CE 00FF F4CF FEC3 0182 C3E9 8202 0082 0081 CF81 CF81 CFF9 CF00"
	$"FFF4 CEFE C601 84C6 E984 0200 8400 81CE 81CE 81CE F9CE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 00D8 BFCE 0000 F0CE"
	$"0300 CECE 00FB CE00 00F1 CE00 00FB CE00 00FB CE00 00D0 CE03 00CE CE00 FBCE 0000"
	$"C9CE 0100 00FA CE00 00F0 CE00 00DE CE00 00F8 CE00 00CB CE00 00F0 CE01 0000 D2CE"
	$"00FF BFCF 0000 F0CF 0300 CFCF 00FB CF00 00F1 CF00 00FB CF00 00FB CF00 00D0 CF03"
	$"00CF CF00 FBCF 0000 C9CF 0100 00FA CF00 00F0 CF00 00DE CF00 00F8 CF00 00CB CF00"
	$"00F0 CF01 0000 D2CF 00FF BFCE 0000 F0CE 0300 CECE 00FB CE00 00F1 CE00 00FB CE00"
	$"00FB CE00 00D0 CE03 00CE CE00 FBCE 0000 C9CE 0100 00FA CE00 00F0 CE00 00DE CE00"
	$"00F8 CE00 00CB CE00 00F0 CE01 0000 D2CE 00FF 00D8 BFCE 0000 F0CE 0300 CECE 00EA"
	$"CE03 00CE CE00 FECE 0000 C9CE 0300 CECE 00FB CE00 00F4 CE00 00D8 CE00 00F8 CE00"
	$"00F0 CE03 00CE CE00 D7CE 0000 CBCE 0000 F6CE 0000 FDCE 0000 D0CE 00FF BFCF 0000"
	$"F0CF 0300 CFCF 00EA CF03 00CF CF00 FECF 0000 C9CF 0300 CFCF 00FB CF00 00F4 CF00"
	$"00D8 CF00 00F8 CF00 00F0 CF03 00CF CF00 D7CF 0000 CBCF 0000 F6CF 0000 FDCF 0000"
	$"D0CF 00FF BFCE 0000 F0CE 0300 CECE 00EA CE03 00CE CE00 FECE 0000 C9CE 0300 CECE"
	$"00FB CE00 00F4 CE00 00D8 CE00 00F8 CE00 00F0 CE03 00CE CE00 D7CE 0000 CBCE 0000"
	$"F6CE 0000 FDCE 0000 D0CE 00FF 00D8 BFCE 0000 F0CE 0300 CECE 00EA CE03 00CE CE00"
	$"FECE 0000 C9CE 0300 CECE 00FB CE00 00F4 CE00 00D8 CE00 00F8 CE00 00F0 CE03 00CE"
	$"CE00 D7CE 0000 CBCE 0000 F6CE 0000 FDCE 0000 D0CE 00FF BFCF 0000 F0CF 0300 CFCF"
	$"00EA CF03 00CF CF00 FECF 0000 C9CF 0300 CFCF 00FB CF00 00F4 CF00 00D8 CF00 00F8"
	$"CF00 00F0 CF03 00CF CF00 D7CF 0000 CBCF 0000 F6CF 0000 FDCF 0000 D0CF 00FF BFCE"
	$"0000 F0CE 0300 CECE 00EA CE03 00CE CE00 FECE 0000 C9CE 0300 CECE 00FB CE00 00F4"
	$"CE00 00D8 CE00 00F8 CE00 00F0 CE03 00CE CE00 D7CE 0000 CBCE 0000 F6CE 0000 FDCE"
	$"0000 D0CE 00FF 031E BFCE 0000 FACE FE00 FDCE 0600 00CE 00CE CE00 FBCE 0600 CECE"
	$"00CE 0000 FDCE FE00 FECE 0100 CEFE 0002 CECE 00FB CE00 00FE CEFE 00F9 CEFE 00F9"
	$"CEFE 00FE CE07 00CE 0000 CECE 0000 FDCE FE00 FECE 0300 CECE 00FB CE03 00CE 0000"
	$"FECE 0000 FECE 0100 CEFE 00FB CE03 00CE 0000 FDCE FE00 02CE CE00 FECE 0000 FECE"
	$"0200 CECE FE00 FECE 0100 CEFD 0001 CE00 FECE 0300 CECE 00F7 CE00 00FE CE04 00CE"
	$"CE00 CEFE 0006 CECE 00CE 00CE CEFE 00F9 CE06 0000 CE00 CECE 00FE CE03 00CE CE00"
	$"FECE FE00 FECE 0000 FECE 0000 FACE FE00 FECE 0400 CE00 CECE FE00 FDCE FE00 FDCE"
	$"FE00 FACE 0300 CE00 00FD CE00 00FD CEFE 00FE CEFE 0001 CECE FE00 FECE FE00 FDCE"
	$"0C00 CE00 CECE 00CE 0000 CECE 0000 E8CE 00FF BFCF 0000 FACF FE00 FDCF 0600 00CF"
	$"00CF CF00 FBCF 0600 CFCF 00CF 0000 FDCF FE00 FECF 0100 CFFE 0002 CFCF 00FB CF00"
	$"00FE CFFE 00F9 CFFE 00F9 CFFE 00FE CF07 00CF 0000 CFCF 0000 FDCF FE00 FECF 0300"
	$"CFCF 00FB CF03 00CF 0000 FECF 0000 FECF 0100 CFFE 00FB CF03 00CF 0000 FDCF FE00"
	$"02CF CF00 FECF 0000 FECF 0200 CFCF FE00 FECF 0100 CFFD 0001 CF00 FECF 0300 CFCF"
	$"00F7 CF00 00FE CF04 00CF CF00 CFFE 0006 CFCF 00CF 00CF CFFE 00F9 CF06 0000 CF00"
	$"CFCF 00FE CF03 00CF CF00 FECF FE00 FECF 0000 FECF 0000 FACF FE00 FECF 0400 CF00"
	$"CFCF FE00 FDCF FE00 FDCF FE00 FACF 0300 CF00 00FD CF00 00FD CFFE 00FE CFFE 0001"
	$"CFCF FE00 FECF FE00 FDCF 0C00 CF00 CFCF 00CF 0000 CFCF 0000 E8CF 00FF BFCE 0000"
	$"FACE FE00 FDCE 0600 00CE 00CE CE00 FBCE 0600 CECE 00CE 0000 FDCE FE00 FECE 0100"
	$"CEFE 0002 CECE 00FB CE00 00FE CEFE 00F9 CEFE 00F9 CEFE 00FE CE07 00CE 0000 CECE"
	$"0000 FDCE FE00 FECE 0300 CECE 00FB CE03 00CE 0000 FECE 0000 FECE 0100 CEFE 00FB"
	$"CE03 00CE 0000 FDCE FE00 02CE CE00 FECE 0000 FECE 0200 CECE FE00 FECE 0100 CEFD"
	$"0001 CE00 FECE 0300 CECE 00F7 CE00 00FE CE04 00CE CE00 CEFE 0006 CECE 00CE 00CE"
	$"CEFE 00F9 CE06 0000 CE00 CECE 00FE CE03 00CE CE00 FECE FE00 FECE 0000 FECE 0000"
	$"FACE FE00 FECE 0400 CE00 CECE FE00 FDCE FE00 FDCE FE00 FACE 0300 CE00 00FD CE00"
	$"00FD CEFE 00FE CEFE 0001 CECE FE00 FECE FE00 FDCE 0C00 CE00 CECE 00CE 0000 CECE"
	$"0000 E8CE 00FF 03AE BFCE 0000 FBCE 0000 FECE 0A00 CECE 00CE CE00 00CE CE00 FBCE"
	$"0A00 CECE 0000 CECE 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0000 FBCE 0300 CECE"
	$"00FE CE00 00FB CE00 00FE CE00 00FB CE00 00FE CE0E 00CE CE00 00CE CE00 00CE CE00"
	$"CECE 00FE CE06 00CE CE00 CECE 00FB CE07 0000 CECE 00CE CE00 FECE 0300 CECE 00FA"
	$"CE07 0000 CECE 00CE CE00 FECE 0200 CE00 FECE 0000 FECE 0200 CE00 FECE 0A00 CECE"
	$"0000 CECE 00CE CE00 FECE 0300 CECE 00F7 CE00 00FE CE06 00CE CE00 CECE 00FE CE04"
	$"0000 CECE 00FE CE00 00FB CE07 00CE CE00 00CE CE00 FECE 0600 CECE 00CE CE00 FECE"
	$"0600 CECE 00CE CE00 FACE 0000 FECE 0700 CECE 0000 CECE 00FE CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE00 00FB CE04 0000 CECE 00FE CE00 00FE CE00 00FE CE00 00FE CE00"
	$"00FD CE00 00FE CE00 00FE CE00 00FE CE01 0000 FECE 0800 00CE CE00 00CE CE00 E9CE"
	$"00FF BFCF 0000 FBCF 0000 FECF 0A00 CFCF 00CF CF00 00CF CF00 FBCF 0A00 CFCF 0000"
	$"CFCF 00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0000 FBCF 0300 CFCF 00FE CF00 00FB"
	$"CF00 00FE CF00 00FB CF00 00FE CF0E 00CF CF00 00CF CF00 00CF CF00 CFCF 00FE CF06"
	$"00CF CF00 CFCF 00FB CF07 0000 CFCF 00CF CF00 FECF 0300 CFCF 00FA CF07 0000 CFCF"
	$"00CF CF00 FECF 0200 CF00 FECF 0000 FECF 0200 CF00 FECF 0A00 CFCF 0000 CFCF 00CF"
	$"CF00 FECF 0300 CFCF 00F7 CF00 00FE CF06 00CF CF00 CFCF 00FE CF04 0000 CFCF 00FE"
	$"CF00 00FB CF07 00CF CF00 00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0600 CFCF 00CF"
	$"CF00 FACF 0000 FECF 0700 CFCF 0000 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE"
	$"CF00 00FB CF04 0000 CFCF 00FE CF00 00FE CF00 00FE CF00 00FE CF00 00FD CF00 00FE"
	$"CF00 00FE CF00 00FE CF01 0000 FECF 0800 00CF CF00 00CF CF00 E9CF 00FF BFCE 0000"
	$"FBCE 0000 FECE 0A00 CECE 00CE CE00 00CE CE00 FBCE 0A00 CECE 0000 CECE 00CE CE00"
	$"FECE 0600 CECE 00CE CE00 FECE 0000 FBCE 0300 CECE 00FE CE00 00FB CE00 00FE CE00"
	$"00FB CE00 00FE CE0E 00CE CE00 00CE CE00 00CE CE00 CECE 00FE CE06 00CE CE00 CECE"
	$"00FB CE07 0000 CECE 00CE CE00 FECE 0300 CECE 00FA CE07 0000 CECE 00CE CE00 FECE"
	$"0200 CE00 FECE 0000 FECE 0200 CE00 FECE 0A00 CECE 0000 CECE 00CE CE00 FECE 0300"
	$"CECE 00F7 CE00 00FE CE06 00CE CE00 CECE 00FE CE04 0000 CECE 00FE CE00 00FB CE07"
	$"00CE CE00 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0600 CECE 00CE CE00 FACE 0000"
	$"FECE 0700 CECE 0000 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE04"
	$"0000 CECE 00FE CE00 00FE CE00 00FE CE00 00FE CE00 00FD CE00 00FE CE00 00FE CE00"
	$"00FE CE01 0000 FECE 0800 00CE CE00 00CE CE00 E9CE 00FF 033F BFCE 0000 FBCE 0000"
	$"FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0300 CECE 00FE CE03 00CE CE00 FECE 0600"
	$"CECE 00CE CE00 FECE 0000 FBCE 0300 CECE 00F3 CE00 00FB CE00 00FB CE00 00FE CE00"
	$"00FE CE00 00FB CE06 00CE CE00 CECE 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FA CE00 00FE CE03 00CE CE00 FECE 0C00 CECE 00CE 00CE 00CE 00CE CE00 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00F7 CE00 00FE CE06 00CE CE00 CECE 00FE"
	$"CE00 00FA CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FBCE 0200"
	$"CE00 F9CE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FB CE00 00F7 CE00 00FE CE00"
	$"00FE CE00 00FA CE00 00FE CE00 00FD CE00 00FE CE00 00FE CE00 00FE CE00 00FD CE00"
	$"00FE CE00 00FE CE00 00E9 CE00 FFBF CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0300"
	$"CFCF 00FB CF03 00CF CF00 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF00 00FB"
	$"CF03 00CF CF00 F3CF 0000 FBCF 0000 FBCF 0000 FECF 0000 FECF 0000 FBCF 0600 CFCF"
	$"00CF CF00 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FACF 0000 FECF 0300 CFCF"
	$"00FE CF0C 00CF CF00 CF00 CF00 CF00 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE"
	$"CF03 00CF CF00 F7CF 0000 FECF 0600 CFCF 00CF CF00 FECF 0000 FACF 0000 FBCF 0000"
	$"FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FB CF02 00CF 00F9 CF00 00FB CF00 00FE"
	$"CF00 00FE CF03 00CF CF00 FBCF 0000 F7CF 0000 FECF 0000 FECF 0000 FACF 0000 FECF"
	$"0000 FDCF 0000 FECF 0000 FECF 0000 FECF 0000 FDCF 0000 FECF 0000 FECF 0000 E9CF"
	$"00FF BFCE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0300 CECE 00FE"
	$"CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0000 FBCE 0300 CECE 00F3 CE00 00FB"
	$"CE00 00FB CE00 00FE CE00 00FE CE00 00FB CE06 00CE CE00 CECE 00FB CE00 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE03 00CE CE00 FECE 0C00 CECE 00CE 00CE"
	$"00CE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00F7 CE00 00FE"
	$"CE06 00CE CE00 CECE 00FE CE00 00FA CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0600"
	$"CECE 00CE CE00 FBCE 0200 CE00 F9CE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FB"
	$"CE00 00F7 CE00 00FE CE00 00FE CE00 00FA CE00 00FE CE00 00FD CE00 00FE CE00 00FE"
	$"CE00 00FE CE00 00FD CE00 00FE CE00 00FE CE00 00E9 CE00 FF03 48BF CE00 00FB CEFC"
	$"0002 CECE 00FE CE03 00CE CE00 FBCE 0300 CECE 00FE CE02 00CE CEFC 0005 CECE 00CE"
	$"CE00 FECE 0000 FBCE 0000 FECE FE00 F9CE FD00 FACE FE00 FECE 0000 FECE 0000 FECE"
	$"0000 FECE FD00 05CE CE00 CECE 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FA"
	$"CE00 00FE CE03 00CE CE00 FECE 0B00 CECE 00CE 00CE 00CE 00CE CEFC 0002 CECE 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00F7 CE00 00FE CE06 00CE CE00 CECE 00FE CE00 00FD"
	$"CEFD 0000 CEFE 0002 CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FBCE FE00"
	$"F9CE 0000 FBCE 0000 FECE 0000 FECE 0000 FECE FE00 FDCE FE00 01CE CEFE 0002 CECE"
	$"00FE CE00 00FE CE00 00FD CEFD 00FE CE00 00FD CE00 00FE CE00 00FE CE00 00FE CE00"
	$"00FD CE00 00FE CE00 00FE CE00 00E9 CE00 FFBF CF00 00FB CFFC 0002 CFCF 00FE CF03"
	$"00CF CF00 FBCF 0300 CFCF 00FE CF02 00CF CFFC 0005 CFCF 00CF CF00 FECF 0000 FBCF"
	$"0000 FECF FE00 F9CF FD00 FACF FE00 FECF 0000 FECF 0000 FECF 0000 FECF FD00 05CF"
	$"CF00 CFCF 00FB CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FA CF00 00FE CF03 00CF"
	$"CF00 FECF 0B00 CFCF 00CF 00CF 00CF 00CF CFFC 0002 CFCF 00FE CF03 00CF CF00 FECF"
	$"0300 CFCF 00F7 CF00 00FE CF06 00CF CF00 CFCF 00FE CF00 00FD CFFD 0000 CFFE 0002"
	$"CFCF 00FE CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 FBCF FE00 F9CF 0000 FBCF 0000"
	$"FECF 0000 FECF 0000 FECF FE00 FDCF FE00 01CF CFFE 0002 CFCF 00FE CF00 00FE CF00"
	$"00FD CFFD 00FE CF00 00FD CF00 00FE CF00 00FE CF00 00FE CF00 00FD CF00 00FE CF00"
	$"00FE CF00 00E9 CF00 FFBF CE00 00FB CEFC 0002 CECE 00FE CE03 00CE CE00 FBCE 0300"
	$"CECE 00FE CE02 00CE CEFC 0005 CECE 00CE CE00 FECE 0000 FBCE 0000 FECE FE00 F9CE"
	$"FD00 FACE FE00 FECE 0000 FECE 0000 FECE 0000 FECE FD00 05CE CE00 CECE 00FB CE00"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE03 00CE CE00 FECE 0B00 CECE"
	$"00CE 00CE 00CE 00CE CEFC 0002 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00F7 CE00"
	$"00FE CE06 00CE CE00 CECE 00FE CE00 00FD CEFD 0000 CEFE 0002 CECE 00FE CE03 00CE"
	$"CE00 FECE 0600 CECE 00CE CE00 FBCE FE00 F9CE 0000 FBCE 0000 FECE 0000 FECE 0000"
	$"FECE FE00 FDCE FE00 01CE CEFE 0002 CECE 00FE CE00 00FE CE00 00FD CEFD 00FE CE00"
	$"00FD CE00 00FE CE00 00FE CE00 00FE CE00 00FD CE00 00FE CE00 00FE CE00 00E9 CE00"
	$"FF03 57BF CE00 00FB CE00 00FB CE00 00FE CE03 00CE CE00 FBCE 0300 CECE 00FE CE03"
	$"00CE CE00 FBCE 0300 CECE 00FE CE00 00FB CE00 00FB CE00 00FB CE00 00FE CE00 00F7"
	$"CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FB CE00 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE03 00CE CE00 FECE 0C00 CECE 00CE"
	$"00CE 00CE 00CE CE00 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 F7CE 0000 FECE"
	$"0600 CECE 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE"
	$"CE06 00CE CE00 CECE 00FB CE03 00CE CE00 FACE 0000 FBCE 0000 FECE 0000 FECE 0000"
	$"FBCE 0000 FBCE 0000 FBCE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000"
	$"FDCE 0000 FECE 0000 FECE 0000 FECE 0000 FDCE 0000 FECE 0000 FECE 0000 E9CE 00FF"
	$"BFCF 0000 FBCF 0000 FBCF 0000 FECF 0300 CFCF 00FB CF03 00CF CF00 FECF 0300 CFCF"
	$"00FB CF03 00CF CF00 FECF 0000 FBCF 0000 FBCF 0000 FBCF 0000 FECF 0000 F7CF 0300"
	$"CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 FBCF 0000 FECF 0300"
	$"CFCF 00FE CF03 00CF CF00 FACF 0000 FECF 0300 CFCF 00FE CF0C 00CF CF00 CF00 CF00"
	$"CF00 CFCF 00FB CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00F7 CF00 00FE CF06 00CF"
	$"CF00 CFCF 00FE CF00 00FE CF00 00FE CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0600"
	$"CFCF 00CF CF00 FBCF 0300 CFCF 00FA CF00 00FB CF00 00FE CF00 00FE CF00 00FB CF00"
	$"00FB CF00 00FB CF00 00FE CF00 00FE CF00 00FE CF00 00FE CF00 00FE CF00 00FD CF00"
	$"00FE CF00 00FE CF00 00FE CF00 00FD CF00 00FE CF00 00FE CF00 00E9 CF00 FFBF CE00"
	$"00FB CE00 00FB CE00 00FE CE03 00CE CE00 FBCE 0300 CECE 00FE CE03 00CE CE00 FBCE"
	$"0300 CECE 00FE CE00 00FB CE00 00FB CE00 00FB CE00 00FE CE00 00F7 CE03 00CE CE00"
	$"FECE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FB CE00 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FA CE00 00FE CE03 00CE CE00 FECE 0C00 CECE 00CE 00CE 00CE 00CE"
	$"CE00 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 F7CE 0000 FECE 0600 CECE 00CE"
	$"CE00 FECE 0000 FECE 0000 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE06 00CE CE00"
	$"CECE 00FB CE03 00CE CE00 FACE 0000 FBCE 0000 FECE 0000 FECE 0000 FBCE 0000 FBCE"
	$"0000 FBCE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FDCE 0000 FECE"
	$"0000 FECE 0000 FECE 0000 FDCE 0000 FECE 0000 FECE 0000 E9CE 00FF 039F BFCE 0000"
	$"FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0600 CECE 00CE CE00 F7CE 0300 CECE 00FE CE00 00FB CE04 00CE CE00 00FB CE00"
	$"00FE CE03 00CE CE00 FECE 0000 FECE 0D00 CECE 00CE CE00 00CE CE00 CECE 00FB CE00"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FA CE07 0000 CECE 00CE CE00 FECE 0000 FECE"
	$"0000 FECE 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00F7"
	$"CE00 00FE CE06 00CE CE00 CECE 00FE CE00 00FE CE04 00CE CE00 00FB CE07 00CE CE00"
	$"00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0600 CECE 00CE CE00 FACE 0000 FECE 0300"
	$"CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE04 0000 CECE"
	$"00FE CE00 00FE CE04 00CE CE00 00FE CE00 00FD CE00 00FE CE00 00FE CE00 00FE CE00"
	$"00FD CE00 00FE CE00 00FE CE00 00E9 CE00 FFBF CF00 00FB CF00 00FE CF03 00CF CF00"
	$"FECF 0300 CFCF 00FB CF03 00CF CF00 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00F7"
	$"CF03 00CF CF00 FECF 0000 FBCF 0400 CFCF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF00"
	$"00FE CF0D 00CF CF00 CFCF 0000 CFCF 00CF CF00 FBCF 0000 FECF 0300 CFCF 00FE CF03"
	$"00CF CF00 FACF 0700 00CF CF00 CFCF 00FE CF00 00FE CF00 00FE CF00 00FE CF00 00FE"
	$"CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 F7CF 0000 FECF 0600 CFCF 00CF"
	$"CF00 FECF 0000 FECF 0400 CFCF 0000 FBCF 0700 CFCF 0000 CFCF 00FE CF06 00CF CF00"
	$"CFCF 00FE CF06 00CF CF00 CFCF 00FA CF00 00FE CF03 00CF CF00 FECF 0000 FECF 0300"
	$"CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0400 00CF CF00 FECF 0000 FECF 0400 CFCF"
	$"0000 FECF 0000 FDCF 0000 FECF 0000 FECF 0000 FECF 0000 FDCF 0000 FECF 0000 FECF"
	$"0000 E9CF 00FF BFCE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 F7CE 0300 CECE 00FE CE00 00FB"
	$"CE04 00CE CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0D00 CECE 00CE CE00"
	$"00CE CE00 CECE 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FA CE07 0000 CECE"
	$"00CE CE00 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00F7 CE00 00FE CE06 00CE CE00 CECE 00FE CE00 00FE CE04 00CE"
	$"CE00 00FB CE07 00CE CE00 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0600 CECE 00CE"
	$"CE00 FACE 0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FB CE04 0000 CECE 00FE CE00 00FE CE04 00CE CE00 00FE CE00 00FD CE00 00FE"
	$"CE00 00FE CE00 00FE CE00 00FD CE00 00FE CE00 00FE CE00 00E9 CE00 FF03 21BF CEFB"
	$"0001 CECE FE00 FDCE FD00 01CE CEFB 0004 CE00 CECE 00FE CE00 00FE CEFE 00FE CE07"
	$"00CE CE00 00CE CE00 FBCE 0000 FECE FE00 F9CE 0300 00CE 00FA CEFE 00FE CE00 00FE"
	$"CE00 00FE CE00 00FE CE09 0000 CE00 CECE 00CE CE00 FBCE FD00 FDCE FD00 03CE CE00"
	$"00FB CE03 00CE 0000 FDCE FE00 FDCE 0000 FECE 0000 FDCE FE00 FECE 0000 FECE 0000"
	$"FECE FD00 02CE CE00 FECE 0000 FACE FD00 09CE CE00 CECE 0000 CECE 00FD CE03 0000"
	$"CE00 FACE 0300 00CE 00FE CEFD 0002 CECE 00FE CEFE 00FE CE00 00FE CE00 00FA CEFE"
	$"00FE CE00 00FD CEFE 00FD CEFE 00FD CEFE 00FA CE03 00CE 0000 FDCE 0000 FDCE 0300"
	$"00CE 00FE CE01 0000 FECE 0000 FDCE FE00 FDCE 0000 FDCE 0000 FECE 0000 FECE 0000"
	$"FDCE 0000 EECE 00FF BFCF FB00 01CF CFFE 00FD CFFD 0001 CFCF FB00 04CF 00CF CF00"
	$"FECF 0000 FECF FE00 FECF 0700 CFCF 0000 CFCF 00FB CF00 00FE CFFE 00F9 CF03 0000"
	$"CF00 FACF FE00 FECF 0000 FECF 0000 FECF 0000 FECF 0900 00CF 00CF CF00 CFCF 00FB"
	$"CFFD 00FD CFFD 0003 CFCF 0000 FBCF 0300 CF00 00FD CFFE 00FD CF00 00FE CF00 00FD"
	$"CFFE 00FE CF00 00FE CF00 00FE CFFD 0002 CFCF 00FE CF00 00FA CFFD 0009 CFCF 00CF"
	$"CF00 00CF CF00 FDCF 0300 00CF 00FA CF03 0000 CF00 FECF FD00 02CF CF00 FECF FE00"
	$"FECF 0000 FECF 0000 FACF FE00 FECF 0000 FDCF FE00 FDCF FE00 FDCF FE00 FACF 0300"
	$"CF00 00FD CF00 00FD CF03 0000 CF00 FECF 0100 00FE CF00 00FD CFFE 00FD CF00 00FD"
	$"CF00 00FE CF00 00FE CF00 00FD CF00 00EE CF00 FFBF CEFB 0001 CECE FE00 FDCE FD00"
	$"01CE CEFB 0004 CE00 CECE 00FE CE00 00FE CEFE 00FE CE07 00CE CE00 00CE CE00 FBCE"
	$"0000 FECE FE00 F9CE 0300 00CE 00FA CEFE 00FE CE00 00FE CE00 00FE CE00 00FE CE09"
	$"0000 CE00 CECE 00CE CE00 FBCE FD00 FDCE FD00 03CE CE00 00FB CE03 00CE 0000 FDCE"
	$"FE00 FDCE 0000 FECE 0000 FDCE FE00 FECE 0000 FECE 0000 FECE FD00 02CE CE00 FECE"
	$"0000 FACE FD00 09CE CE00 CECE 0000 CECE 00FD CE03 0000 CE00 FACE 0300 00CE 00FE"
	$"CEFD 0002 CECE 00FE CEFE 00FE CE00 00FE CE00 00FA CEFE 00FE CE00 00FD CEFE 00FD"
	$"CEFE 00FD CEFE 00FA CE03 00CE 0000 FDCE 0000 FDCE 0300 00CE 00FE CE01 0000 FECE"
	$"0000 FDCE FE00 FDCE 0000 FDCE 0000 FECE 0000 FECE 0000 FDCE 0000 EECE 00FF 004E"
	$"81CE B2CE 0000 D2CE 0000 DACE 0000 BFCE 0000 CBCE 0000 EECE 00FF 81CF B2CF 0000"
	$"D2CF 0000 DACF 0000 BFCF 0000 CBCF 0000 EECF 00FF 81CE B2CE 0000 D2CE 0000 DACE"
	$"0000 BFCE 0000 CBCE 0000 EECE 00FF 004E 81CE B2CE 0000 D2CE 0000 DACE 0000 BFCE"
	$"0000 CBCE 0000 EECE 00FF 81CF B2CF 0000 D2CF 0000 DACF 0000 BFCF 0000 CBCF 0000"
	$"EECF 00FF 81CE B2CE 0000 D2CE 0000 DACE 0000 BFCE 0000 CBCE 0000 EECE 00FF 0036"
	$"81CE B2CE 0000 AACE 0000 BFCE 0000 B7CE 00FF 81CF B2CF 0000 AACF 0000 BFCF 0000"
	$"B7CF 00FF 81CE B2CE 0000 AACE 0000 BFCE 0000 B7CE 00FF 001E 81CE 81CE 81CE CCCE"
	$"00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE"
	$"00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE"
	$"00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 0117 CACE 0300 CECE 00E8"
	$"CE00 00F7 CE00 00E1 CE03 00CE CE00 F8CE 0000 FBCE 0000 F3CE 0100 00EE CE00 00F0"
	$"CE00 00FA CE00 00F1 CE00 00E6 CE00 00E1 CE00 00FB CE00 00FA CE00 00FE CEFC 00FB"
	$"CE00 00FC CE00 00FC CE02 00CE 00F4 CE00 00A7 CE00 FFCA CF03 00CF CF00 E8CF 0000"
	$"F7CF 0000 E1CF 0300 CFCF 00F8 CF00 00FB CF00 00F3 CF01 0000 EECF 0000 F0CF 0000"
	$"FACF 0000 F1CF 0000 E6CF 0000 E1CF 0000 FBCF 0000 FACF 0000 FECF FC00 FBCF 0000"
	$"FCCF 0000 FCCF 0200 CF00 F4CF 0000 A7CF 00FF CACE 0300 CECE 00E8 CE00 00F7 CE00"
	$"00E1 CE03 00CE CE00 F8CE 0000 FBCE 0000 F3CE 0100 00EE CE00 00F0 CE00 00FA CE00"
	$"00F1 CE00 00E6 CE00 00E1 CE00 00FB CE00 00FA CE00 00FE CEFC 00FB CE00 00FC CE00"
	$"00FC CE02 00CE 00F4 CE00 00A7 CE00 FF01 44EF CE00 00F0 CE00 00EF CE00 00FC CE00"
	$"00EB CE03 00CE CE00 D6CE 0000 F8CE 0000 FBCE 0000 F4CE 0000 F0CE 0000 FECE 0000"
	$"F0CE 0100 00FC CE01 0000 E7CE 0000 F0CE 0000 E1CE 0000 FBCE 0100 00FC CE04 0000"
	$"CECE 00FC CE00 00FC CE00 00FD CE02 00CE 00FD CE00 00F2 CE00 00A7 CE00 FFEF CF00"
	$"00F0 CF00 00EF CF00 00FC CF00 00EB CF03 00CF CF00 D6CF 0000 F8CF 0000 FBCF 0000"
	$"F4CF 0000 F0CF 0000 FECF 0000 F0CF 0100 00FC CF01 0000 E7CF 0000 F0CF 0000 E1CF"
	$"0000 FBCF 0100 00FC CF04 0000 CFCF 00FC CF00 00FC CF00 00FD CF02 00CF 00FD CF00"
	$"00F2 CF00 00A7 CF00 FFEF CE00 00F0 CE00 00EF CE00 00FC CE00 00EB CE03 00CE CE00"
	$"D6CE 0000 F8CE 0000 FBCE 0000 F4CE 0000 F0CE 0000 FECE 0000 F0CE 0100 00FC CE01"
	$"0000 E7CE 0000 F0CE 0000 E1CE 0000 FBCE 0100 00FC CE04 0000 CECE 00FC CE00 00FC"
	$"CE00 00FD CE02 00CE 00FD CE00 00F2 CE00 00A7 CE00 FF01 44EF CE00 00F0 CE00 00EF"
	$"CE00 00FC CE00 00EB CE03 00CE CE00 D6CE 0000 F8CE 0000 FBCE 0000 F4CE 0000 F0CE"
	$"0000 FECE 0000 F0CE 0100 00FC CE01 0000 E7CE 0000 F0CE 0000 E1CE 0000 FBCE 0100"
	$"00FC CE04 0000 CECE 00FC CE00 00FB CE00 00FE CE02 00CE 00FE CE00 00F1 CE00 00A7"
	$"CE00 FFEF CF00 00F0 CF00 00EF CF00 00FC CF00 00EB CF03 00CF CF00 D6CF 0000 F8CF"
	$"0000 FBCF 0000 F4CF 0000 F0CF 0000 FECF 0000 F0CF 0100 00FC CF01 0000 E7CF 0000"
	$"F0CF 0000 E1CF 0000 FBCF 0100 00FC CF04 0000 CFCF 00FC CF00 00FB CF00 00FE CF02"
	$"00CF 00FE CF00 00F1 CF00 00A7 CF00 FFEF CE00 00F0 CE00 00EF CE00 00FC CE00 00EB"
	$"CE03 00CE CE00 D6CE 0000 F8CE 0000 FBCE 0000 F4CE 0000 F0CE 0000 FECE 0000 F0CE"
	$"0100 00FC CE01 0000 E7CE 0000 F0CE 0000 E1CE 0000 FBCE 0100 00FC CE04 0000 CECE"
	$"00FC CE00 00FB CE00 00FE CE02 00CE 00FE CE00 00F1 CE00 00A7 CE00 FF03 27F0 CEFE"
	$"00FE CEFE 00FE CE00 00FE CE01 00CE FE00 FACE FE00 FDCE 0700 00CE 00CE CE00 CEFE"
	$"00FE CEFE 00FE CE02 00CE 00F8 CE01 00CE FE00 FBCE 0000 FECE FE00 F9CE FE00 02CE"
	$"CE00 FECE 0200 CECE FE00 FECE 0300 CECE 00FE CEFE 00FE CE03 00CE 0000 FECE 0000"
	$"FECE FE00 FBCE FE00 01CE CEFE 00FE CE02 00CE 00FD CEFE 0005 CECE 00CE 0000 FDCE"
	$"FE00 FACE 0200 CE00 FECE 0200 CE00 FECE FE00 FDCE FE00 FECE 0800 CECE 00CE 0000"
	$"CECE FE00 FECE FE00 FDCE FE00 FECE 0300 CE00 00F5 CEFE 00FE CE03 00CE 0000 FDCE"
	$"0300 00CE 00FB CE02 00CE 00FE CE05 00CE 00CE CE00 F5CE 0000 FECE 0200 CE00 FECE"
	$"0900 CECE 00CE CE00 CE00 00FD CE03 0000 CE00 FDCE FE00 FECE 0000 FECE 0000 FECE"
	$"0000 FECE FE00 C0CE 00FF F0CF FE00 FECF FE00 FECF 0000 FECF 0100 CFFE 00FA CFFE"
	$"00FD CF07 0000 CF00 CFCF 00CF FE00 FECF FE00 FECF 0200 CF00 F8CF 0100 CFFE 00FB"
	$"CF00 00FE CFFE 00F9 CFFE 0002 CFCF 00FE CF02 00CF CFFE 00FE CF03 00CF CF00 FECF"
	$"FE00 FECF 0300 CF00 00FE CF00 00FE CFFE 00FB CFFE 0001 CFCF FE00 FECF 0200 CF00"
	$"FDCF FE00 05CF CF00 CF00 00FD CFFE 00FA CF02 00CF 00FE CF02 00CF 00FE CFFE 00FD"
	$"CFFE 00FE CF08 00CF CF00 CF00 00CF CFFE 00FE CFFE 00FD CFFE 00FE CF03 00CF 0000"
	$"F5CF FE00 FECF 0300 CF00 00FD CF03 0000 CF00 FBCF 0200 CF00 FECF 0500 CF00 CFCF"
	$"00F5 CF00 00FE CF02 00CF 00FE CF09 00CF CF00 CFCF 00CF 0000 FDCF 0300 00CF 00FD"
	$"CFFE 00FE CF00 00FE CF00 00FE CF00 00FE CFFE 00C0 CF00 FFF0 CEFE 00FE CEFE 00FE"
	$"CE00 00FE CE01 00CE FE00 FACE FE00 FDCE 0700 00CE 00CE CE00 CEFE 00FE CEFE 00FE"
	$"CE02 00CE 00F8 CE01 00CE FE00 FBCE 0000 FECE FE00 F9CE FE00 02CE CE00 FECE 0200"
	$"CECE FE00 FECE 0300 CECE 00FE CEFE 00FE CE03 00CE 0000 FECE 0000 FECE FE00 FBCE"
	$"FE00 01CE CEFE 00FE CE02 00CE 00FD CEFE 0005 CECE 00CE 0000 FDCE FE00 FACE 0200"
	$"CE00 FECE 0200 CE00 FECE FE00 FDCE FE00 FECE 0800 CECE 00CE 0000 CECE FE00 FECE"
	$"FE00 FDCE FE00 FECE 0300 CE00 00F5 CEFE 00FE CE03 00CE 0000 FDCE 0300 00CE 00FB"
	$"CE02 00CE 00FE CE05 00CE 00CE CE00 F5CE 0000 FECE 0200 CE00 FECE 0900 CECE 00CE"
	$"CE00 CE00 00FD CE03 0000 CE00 FDCE FE00 FECE 0000 FECE 0000 FECE 0000 FECE FE00"
	$"C0CE 00FF 03B7 EFCE 0000 FECE 0000 FECE 0000 FECE 0200 CE00 FECE 0000 FACE 0000"
	$"FECE 0D00 CECE 00CE CE00 00CE CE00 CECE 00FE CE00 00FE CE04 00CE CE00 00F7 CE03"
	$"00CE CE00 FACE 0300 CECE 00FE CE00 00FB CE00 00FE CE02 00CE 00FE CE02 00CE 00FE"
	$"CE09 00CE CE00 CECE 00CE CE00 FECE 0D00 CECE 0000 CECE 00CE CE00 CECE 00FE CE00"
	$"00FB CE03 00CE CE00 FECE 0400 CECE 0000 FBCE 0000 FECE 0700 00CE CE00 CECE 00FE"
	$"CE00 00FB CE02 00CE 00FE CE05 00CE 00CE CE00 FECE 0300 CECE 00FE CE0D 00CE CE00"
	$"CECE 0000 CECE 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE07 00CE CE00 00CE CE00"
	$"F7CE 0000 FECE 0E00 CECE 0000 CECE 00CE CE00 CECE 0000 FBCE 0200 CE00 FECE 0200"
	$"CE00 FECE FE00 F8CE 0000 FECE 0200 CE00 FECE 1100 CECE 00CE CE00 00CE CE00 CECE"
	$"00CE CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00C1"
	$"CE00 FFEF CF00 00FE CF00 00FE CF00 00FE CF02 00CF 00FE CF00 00FA CF00 00FE CF0D"
	$"00CF CF00 CFCF 0000 CFCF 00CF CF00 FECF 0000 FECF 0400 CFCF 0000 F7CF 0300 CFCF"
	$"00FA CF03 00CF CF00 FECF 0000 FBCF 0000 FECF 0200 CF00 FECF 0200 CF00 FECF 0900"
	$"CFCF 00CF CF00 CFCF 00FE CF0D 00CF CF00 00CF CF00 CFCF 00CF CF00 FECF 0000 FBCF"
	$"0300 CFCF 00FE CF04 00CF CF00 00FB CF00 00FE CF07 0000 CFCF 00CF CF00 FECF 0000"
	$"FBCF 0200 CF00 FECF 0500 CF00 CFCF 00FE CF03 00CF CF00 FECF 0D00 CFCF 00CF CF00"
	$"00CF CF00 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0700 CFCF 0000 CFCF 00F7 CF00"
	$"00FE CF0E 00CF CF00 00CF CF00 CFCF 00CF CF00 00FB CF02 00CF 00FE CF02 00CF 00FE"
	$"CFFE 00F8 CF00 00FE CF02 00CF 00FE CF11 00CF CF00 CFCF 0000 CFCF 00CF CF00 CFCF"
	$"0000 FECF 0000 FECF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0000 C1CF 00FF"
	$"EFCE 0000 FECE 0000 FECE 0000 FECE 0200 CE00 FECE 0000 FACE 0000 FECE 0D00 CECE"
	$"00CE CE00 00CE CE00 CECE 00FE CE00 00FE CE04 00CE CE00 00F7 CE03 00CE CE00 FACE"
	$"0300 CECE 00FE CE00 00FB CE00 00FE CE02 00CE 00FE CE02 00CE 00FE CE09 00CE CE00"
	$"CECE 00CE CE00 FECE 0D00 CECE 0000 CECE 00CE CE00 CECE 00FE CE00 00FB CE03 00CE"
	$"CE00 FECE 0400 CECE 0000 FBCE 0000 FECE 0700 00CE CE00 CECE 00FE CE00 00FB CE02"
	$"00CE 00FE CE05 00CE 00CE CE00 FECE 0300 CECE 00FE CE0D 00CE CE00 CECE 0000 CECE"
	$"00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE07 00CE CE00 00CE CE00 F7CE 0000 FECE"
	$"0E00 CECE 0000 CECE 00CE CE00 CECE 0000 FBCE 0200 CE00 FECE 0200 CE00 FECE FE00"
	$"F8CE 0000 FECE 0200 CE00 FECE 1100 CECE 00CE CE00 00CE CE00 CECE 00CE CE00 00FE"
	$"CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00C1 CE00 FF03 54EF"
	$"CE00 00FE CE00 00FE CE00 00FE CE02 00CE 00FE CE00 00FA CE00 00FE CE03 00CE CE00"
	$"FECE 0600 CECE 00CE CE00 FECE 0000 FECE 0300 CECE 00F6 CE03 00CE CE00 FACE 0300"
	$"CECE 00F3 CE05 00CE CE00 CE00 FBCE 0600 CECE 00CE CE00 FBCE 0300 CECE 00FE CE06"
	$"00CE CE00 CECE 00FE CE00 00FB CE03 00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE00"
	$"00FE CE03 00CE CE00 FECE 0000 FBCE 0800 CECE 00CE 00CE CE00 FBCE 0300 CECE 00FB"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FBCE 0000 FECE 0000"
	$"F3CE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0800 CECE 00CE 00CE CE00 FBCE"
	$"0100 00F9 CE02 00CE 00FE CE02 00CE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00"
	$"00FE CE00 00FE CE00 00FE CE06 00CE 00CE 00CE 00FE CE00 00BD CE00 FFEF CF00 00FE"
	$"CF00 00FE CF00 00FE CF02 00CF 00FE CF00 00FA CF00 00FE CF03 00CF CF00 FECF 0600"
	$"CFCF 00CF CF00 FECF 0000 FECF 0300 CFCF 00F6 CF03 00CF CF00 FACF 0300 CFCF 00F3"
	$"CF05 00CF CF00 CF00 FBCF 0600 CFCF 00CF CF00 FBCF 0300 CFCF 00FE CF06 00CF CF00"
	$"CFCF 00FE CF00 00FB CF03 00CF CF00 FECF 0300 CFCF 00FA CF00 00FE CF00 00FE CF03"
	$"00CF CF00 FECF 0000 FBCF 0800 CFCF 00CF 00CF CF00 FBCF 0300 CFCF 00FB CF03 00CF"
	$"CF00 FECF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00 FBCF 0000 FECF 0000 F3CF 0300"
	$"CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0800 CFCF 00CF 00CF CF00 FBCF 0100 00F9"
	$"CF02 00CF 00FE CF02 00CF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FE CF00"
	$"00FE CF00 00FE CF06 00CF 00CF 00CF 00FE CF00 00BD CF00 FFEF CE00 00FE CE00 00FE"
	$"CE00 00FE CE02 00CE 00FE CE00 00FA CE00 00FE CE03 00CE CE00 FECE 0600 CECE 00CE"
	$"CE00 FECE 0000 FECE 0300 CECE 00F6 CE03 00CE CE00 FACE 0300 CECE 00F3 CE05 00CE"
	$"CE00 CE00 FBCE 0600 CECE 00CE CE00 FBCE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE"
	$"CE00 00FB CE03 00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03 00CE CE00"
	$"FECE 0000 FBCE 0800 CECE 00CE 00CE CE00 FBCE 0300 CECE 00FB CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE00 00FE CE03 00CE CE00 FBCE 0000 FECE 0000 F3CE 0300 CECE 00FE"
	$"CE03 00CE CE00 FECE 0000 FBCE 0800 CECE 00CE 00CE CE00 FBCE 0100 00F9 CE02 00CE"
	$"00FE CE02 00CE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE00"
	$"00FE CE06 00CE 00CE 00CE 00FE CE00 00BD CE00 FF03 39EF CE00 00FE CEFC 00FD CE00"
	$"00FD CE00 00FA CEFC 0002 CECE 00FE CE06 00CE CE00 CECE 00FE CE00 00FE CE03 00CE"
	$"CE00 F6CE 0300 CECE 00FA CE00 00FE CEFE 00F9 CEFD 0004 CECE 00CE 00FE CEFD 0005"
	$"CECE 00CE CE00 FECE FD00 02CE CE00 FECE 0500 CECE 00CE CEFC 00FB CE03 00CE CE00"
	$"FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE02 00CE CEFC 00FB CE08 00CE CE00 CE00"
	$"CECE 00FE CEFD 0002 CECE 00FB CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00"
	$"00FE CEFE 00FE CE00 00FE CE00 00F6 CEFD 0002 CECE 00FE CE03 00CE CE00 FECE 0000"
	$"FBCE 0800 CECE 00CE 00CE CE00 F9CE 0000 FACE 0200 CE00 FECE 0200 CE00 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FECE 0600 CE00 CE00 CE00"
	$"FDCE FE00 C0CE 00FF EFCF 0000 FECF FC00 FDCF 0000 FDCF 0000 FACF FC00 02CF CF00"
	$"FECF 0600 CFCF 00CF CF00 FECF 0000 FECF 0300 CFCF 00F6 CF03 00CF CF00 FACF 0000"
	$"FECF FE00 F9CF FD00 04CF CF00 CF00 FECF FD00 05CF CF00 CFCF 00FE CFFD 0002 CFCF"
	$"00FE CF05 00CF CF00 CFCF FC00 FBCF 0300 CFCF 00FE CF03 00CF CF00 FACF 0000 FECF"
	$"0000 FECF 0200 CFCF FC00 FBCF 0800 CFCF 00CF 00CF CF00 FECF FD00 02CF CF00 FBCF"
	$"0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FECF FE00 FECF 0000 FECF 0000"
	$"F6CF FD00 02CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF08 00CF CF00 CF00 CFCF 00F9"
	$"CF00 00FA CF02 00CF 00FE CF02 00CF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00"
	$"00FE CF00 00FE CF00 00FE CF06 00CF 00CF 00CF 00FD CFFE 00C0 CF00 FFEF CE00 00FE"
	$"CEFC 00FD CE00 00FD CE00 00FA CEFC 0002 CECE 00FE CE06 00CE CE00 CECE 00FE CE00"
	$"00FE CE03 00CE CE00 F6CE 0300 CECE 00FA CE00 00FE CEFE 00F9 CEFD 0004 CECE 00CE"
	$"00FE CEFD 0005 CECE 00CE CE00 FECE FD00 02CE CE00 FECE 0500 CECE 00CE CEFC 00FB"
	$"CE03 00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE02 00CE CEFC 00FB CE08"
	$"00CE CE00 CE00 CECE 00FE CEFD 0002 CECE 00FB CE03 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FE CE00 00FE CEFE 00FE CE00 00FE CE00 00F6 CEFD 0002 CECE 00FE CE03 00CE"
	$"CE00 FECE 0000 FBCE 0800 CECE 00CE 00CE CE00 F9CE 0000 FACE 0200 CE00 FECE 0200"
	$"CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FECE 0600"
	$"CE00 CE00 CE00 FDCE FE00 C0CE 00FF 0363 EFCE 0000 FECE 0000 FACE 0200 CE00 FECE"
	$"0000 FACE 0000 FBCE 0000 FECE 0600 CECE 00CE CE00 FECE 0000 FECE 0300 CECE 00F6"
	$"CE03 00CE CE00 FACE 0000 FBCE 0000 FBCE 0000 FECE 0800 CECE 00CE 00CE CE00 FECE"
	$"0900 CECE 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 F7CE 0300"
	$"CECE 00FE CE03 00CE CE00 FACE 0000 FECE 0000 FECE 0300 CECE 00F7 CE0B 00CE CE00"
	$"CE00 CECE 00CE CE00 FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0300 CECE 00FE CE00"
	$"00FE CE00 00FB CE03 00CE CE00 FECE 0000 F7CE 0000 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0000 FBCE 0B00 CECE 00CE 00CE CE00 CECE 00FC CE00 00FA CE02 00CE 00FE"
	$"CE02 00CE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE00 00FE"
	$"CE06 00CE 00CE 00CE 00FA CE00 00C1 CE00 FFEF CF00 00FE CF00 00FA CF02 00CF 00FE"
	$"CF00 00FA CF00 00FB CF00 00FE CF06 00CF CF00 CFCF 00FE CF00 00FE CF03 00CF CF00"
	$"F6CF 0300 CFCF 00FA CF00 00FB CF00 00FB CF00 00FE CF08 00CF CF00 CF00 CFCF 00FE"
	$"CF09 00CF CF00 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00F7 CF03"
	$"00CF CF00 FECF 0300 CFCF 00FA CF00 00FE CF00 00FE CF03 00CF CF00 F7CF 0B00 CFCF"
	$"00CF 00CF CF00 CFCF 00FE CF03 00CF CF00 FBCF 0300 CFCF 00FE CF03 00CF CF00 FECF"
	$"0000 FECF 0000 FBCF 0300 CFCF 00FE CF00 00F7 CF00 00FE CF03 00CF CF00 FECF 0300"
	$"CFCF 00FE CF00 00FB CF0B 00CF CF00 CF00 CFCF 00CF CF00 FCCF 0000 FACF 0200 CF00"
	$"FECF 0200 CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FECF 0000"
	$"FECF 0600 CF00 CF00 CF00 FACF 0000 C1CF 00FF EFCE 0000 FECE 0000 FACE 0200 CE00"
	$"FECE 0000 FACE 0000 FBCE 0000 FECE 0600 CECE 00CE CE00 FECE 0000 FECE 0300 CECE"
	$"00F6 CE03 00CE CE00 FACE 0000 FBCE 0000 FBCE 0000 FECE 0800 CECE 00CE 00CE CE00"
	$"FECE 0900 CECE 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 F7CE"
	$"0300 CECE 00FE CE03 00CE CE00 FACE 0000 FECE 0000 FECE 0300 CECE 00F7 CE0B 00CE"
	$"CE00 CE00 CECE 00CE CE00 FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0000 F7CE 0000 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0B00 CECE 00CE 00CE CE00 CECE 00FC CE00 00FA CE02 00CE"
	$"00FE CE02 00CE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE00"
	$"00FE CE06 00CE 00CE 00CE 00FA CE00 00C1 CE00 FF03 A5EF CE00 00FE CE00 00FE CE00"
	$"00FE CE02 00CE 00FE CE00 00FA CE00 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00"
	$"FECE 0000 FECE 0300 CECE 00F6 CE03 00CE CE00 FACE 0300 CECE 00FE CE00 00FB CE04"
	$"00CE CE00 00FE CE00 00FE CE14 00CE CE00 00CE CE00 CECE 00CE CE00 CECE 0000 CECE"
	$"00FE CE06 00CE CE00 CECE 00FE CE00 00FB CE03 00CE CE00 FECE 0300 CECE 00FA CE00"
	$"00FE CE00 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE 0A00 CECE 00CE"
	$"CE00 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00F7 CE07 00CE CE00 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FB CE00 00FE CE00 00FE CE03 00CE CE00 FCCE 0000 F9CE 0000 FCCE 0000 FDCE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FDCE 0000 FECE 0000"
	$"FDCE 0000 FECE 0000 C1CE 00FF EFCF 0000 FECF 0000 FECF 0000 FECF 0200 CF00 FECF"
	$"0000 FACF 0000 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF00 00FE CF03 00CF"
	$"CF00 F6CF 0300 CFCF 00FA CF03 00CF CF00 FECF 0000 FBCF 0400 CFCF 0000 FECF 0000"
	$"FECF 1400 CFCF 0000 CFCF 00CF CF00 CFCF 00CF CF00 00CF CF00 FECF 0600 CFCF 00CF"
	$"CF00 FECF 0000 FBCF 0300 CFCF 00FE CF03 00CF CF00 FACF 0000 FECF 0000 FECF 0300"
	$"CFCF 00FE CF00 00FB CF00 00FE CF00 00FE CF0A 00CF CF00 CFCF 0000 CFCF 00FE CF06"
	$"00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00"
	$"FECF 0000 F7CF 0700 CFCF 0000 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FECF"
	$"0000 FECF 0300 CFCF 00FC CF00 00F9 CF00 00FC CF00 00FD CF03 00CF CF00 FECF 0300"
	$"CFCF 00FE CF00 00FE CF00 00FE CF00 00FD CF00 00FE CF00 00FD CF00 00FE CF00 00C1"
	$"CF00 FFEF CE00 00FE CE00 00FE CE00 00FE CE02 00CE 00FE CE00 00FA CE00 00FE CE03"
	$"00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0000 FECE 0300 CECE 00F6 CE03 00CE CE00"
	$"FACE 0300 CECE 00FE CE00 00FB CE04 00CE CE00 00FE CE00 00FE CE14 00CE CE00 00CE"
	$"CE00 CECE 00CE CE00 CECE 0000 CECE 00FE CE06 00CE CE00 CECE 00FE CE00 00FB CE03"
	$"00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FBCE"
	$"0000 FECE 0000 FECE 0A00 CECE 00CE CE00 00CE CE00 FECE 0600 CECE 00CE CE00 FECE"
	$"0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE07 00CE"
	$"CE00 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FE CE03 00CE CE00"
	$"FCCE 0000 F9CE 0000 FCCE 0000 FDCE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FECE"
	$"0000 FECE 0000 FDCE 0000 FECE 0000 FDCE 0000 FECE 0000 C1CE 00FF 0339 EFCE 0100"
	$"00FE CEFE 00FE CE00 00FE CE04 00CE CE00 00FA CEFE 00FD CEFD 0006 CECE 00CE CE00"
	$"00FE CEFE 00FE CE00 00FE CE00 00FA CE04 00CE CE00 00FB CE00 00FE CEFE 00F9 CE03"
	$"0000 CE00 FECE 0000 FDCE 0900 00CE 00CE CE00 CECE 00FE CE05 0000 CE00 CECE FD00"
	$"FECE 0000 FECE FE00 FACE 0000 FECE FE00 FECE 0000 FACE 0400 00CE CE00 FECE 0000"
	$"FECE FE00 FACE 0000 FECE 0000 FECE 0000 FECE 0300 00CE 00FE CEFE 00FE CE03 00CE"
	$"CE00 FECE 0400 CECE 0000 FECE FE00 FDCE FE00 FECE 0000 FECE 0000 FECE 0000 FACE"
	$"0600 00CE 00CE CE00 FECE 0000 FECE FD00 FBCE 0000 FECE 0000 FECE 0000 FECE FC00"
	$"F8CE 0000 FCCE 0000 FDCE 0300 CECE 00FE CE00 00FE CEFD 00FD CEFE 00FC CE00 00FE"
	$"CE00 00FC CEFE 00FD CE00 00C5 CE00 FFEF CF01 0000 FECF FE00 FECF 0000 FECF 0400"
	$"CFCF 0000 FACF FE00 FDCF FD00 06CF CF00 CFCF 0000 FECF FE00 FECF 0000 FECF 0000"
	$"FACF 0400 CFCF 0000 FBCF 0000 FECF FE00 F9CF 0300 00CF 00FE CF00 00FD CF09 0000"
	$"CF00 CFCF 00CF CF00 FECF 0500 00CF 00CF CFFD 00FE CF00 00FE CFFE 00FA CF00 00FE"
	$"CFFE 00FE CF00 00FA CF04 0000 CFCF 00FE CF00 00FE CFFE 00FA CF00 00FE CF00 00FE"
	$"CF00 00FE CF03 0000 CF00 FECF FE00 FECF 0300 CFCF 00FE CF04 00CF CF00 00FE CFFE"
	$"00FD CFFE 00FE CF00 00FE CF00 00FE CF00 00FA CF06 0000 CF00 CFCF 00FE CF00 00FE"
	$"CFFD 00FB CF00 00FE CF00 00FE CF00 00FE CFFC 00F8 CF00 00FC CF00 00FD CF03 00CF"
	$"CF00 FECF 0000 FECF FD00 FDCF FE00 FCCF 0000 FECF 0000 FCCF FE00 FDCF 0000 C5CF"
	$"00FF EFCE 0100 00FE CEFE 00FE CE00 00FE CE04 00CE CE00 00FA CEFE 00FD CEFD 0006"
	$"CECE 00CE CE00 00FE CEFE 00FE CE00 00FE CE00 00FA CE04 00CE CE00 00FB CE00 00FE"
	$"CEFE 00F9 CE03 0000 CE00 FECE 0000 FDCE 0900 00CE 00CE CE00 CECE 00FE CE05 0000"
	$"CE00 CECE FD00 FECE 0000 FECE FE00 FACE 0000 FECE FE00 FECE 0000 FACE 0400 00CE"
	$"CE00 FECE 0000 FECE FE00 FACE 0000 FECE 0000 FECE 0000 FECE 0300 00CE 00FE CEFE"
	$"00FE CE03 00CE CE00 FECE 0400 CECE 0000 FECE FE00 FDCE FE00 FECE 0000 FECE 0000"
	$"FECE 0000 FACE 0600 00CE 00CE CE00 FECE 0000 FECE FD00 FBCE 0000 FECE 0000 FECE"
	$"0000 FECE FC00 F8CE 0000 FCCE 0000 FDCE 0300 CECE 00FE CE00 00FE CEFD 00FD CEFE"
	$"00FC CE00 00FE CE00 00FC CEFE 00FD CE00 00C5 CE00 FF00 2A81 CE81 CEFA CE00 0081"
	$"CED4 CE00 FF81 CF81 CFFA CF00 0081 CFD4 CF00 FF81 CE81 CEFA CE00 0081 CED4 CE00"
	$"FF00 2A81 CE81 CEFA CE00 0081 CED4 CE00 FF81 CF81 CFFA CF00 0081 CFD4 CF00 FF81"
	$"CE81 CEFA CE00 0081 CED4 CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 AEEF CE00 00F0 CE03 00CE CE00 FBCE 0000 F1CE"
	$"0000 FBCE 0000 DECE 0000 EFCE 0100 00EF CE00 00DB CE00 00FB CE00 00C0 CE02 00CE"
	$"00B5 CE00 009D CE00 FFEF CF00 00F0 CF03 00CF CF00 FBCF 0000 F1CF 0000 FBCF 0000"
	$"DECF 0000 EFCF 0100 00EF CF00 00DB CF00 00FB CF00 00C0 CF02 00CF 00B5 CF00 009D"
	$"CF00 FFEF CE00 00F0 CE03 00CE CE00 FBCE 0000 F1CE 0000 FBCE 0000 DECE 0000 EFCE"
	$"0100 00EF CE00 00DB CE00 00FB CE00 00C0 CE02 00CE 00B5 CE00 009D CE00 FF00 C3EF"
	$"CE00 00F0 CE03 00CE CE00 EACE 0300 CECE 00FE CE00 00DE CE00 00F0 CE00 00ED CE00"
	$"00DB CE00 00FB CE00 00C1 CE03 00CE CE00 F4CE 0000 CDCE 0000 F8CE 0000 9DCE 00FF"
	$"EFCF 0000 F0CF 0300 CFCF 00EA CF03 00CF CF00 FECF 0000 DECF 0000 F0CF 0000 EDCF"
	$"0000 DBCF 0000 FBCF 0000 C1CF 0300 CFCF 00F4 CF00 00CD CF00 00F8 CF00 009D CF00"
	$"FFEF CE00 00F0 CE03 00CE CE00 EACE 0300 CECE 00FE CE00 00DE CE00 00F0 CE00 00ED"
	$"CE00 00DB CE00 00FB CE00 00C1 CE03 00CE CE00 F4CE 0000 CDCE 0000 F8CE 0000 9DCE"
	$"00FF 00C3 EFCE 0000 F0CE 0300 CECE 00EA CE03 00CE CE00 FECE 0000 DECE 0000 F0CE"
	$"0000 EDCE 0000 DBCE 0000 FBCE 0000 C1CE 0300 CECE 00F4 CE00 00CD CE00 00F8 CE00"
	$"009D CE00 FFEF CF00 00F0 CF03 00CF CF00 EACF 0300 CFCF 00FE CF00 00DE CF00 00F0"
	$"CF00 00ED CF00 00DB CF00 00FB CF00 00C1 CF03 00CF CF00 F4CF 0000 CDCF 0000 F8CF"
	$"0000 9DCF 00FF EFCE 0000 F0CE 0300 CECE 00EA CE03 00CE CE00 FECE 0000 DECE 0000"
	$"F0CE 0000 EDCE 0000 DBCE 0000 FBCE 0000 C1CE 0300 CECE 00F4 CE00 00CD CE00 00F8"
	$"CE00 009D CE00 FF02 F4EF CE00 00FA CEFE 00FD CE06 0000 CE00 CECE 00FB CE06 00CE"
	$"CE00 CE00 00FD CEFE 00FE CE01 00CE FE00 02CE CE00 FBCE 0700 CE00 00CE CE00 00FD"
	$"CEFE 00FE CE00 00FE CE00 00FB CE03 00CE 0000 FDCE FE00 FBCE FE00 05CE 00CE 00CE"
	$"CEFE 00FD CEFE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE FE00 FDCE"
	$"FE00 FDCE 0300 00CE 00FB CE03 00CE 0000 FECE 0000 FECE 0000 FACE FE00 FECE 0300"
	$"CE00 00FE CE00 00FE CE00 00FE CEFE 00FE CE03 00CE 0000 FDCE FE00 FACE 0000 FECE"
	$"0300 CE00 00FE CE00 00FE CE01 00CE FE00 FBCE 0700 CE00 00CE CE00 00FD CEFE 00FE"
	$"CE00 00FE CE00 00FB CE03 00CE 0000 FCCE FE00 FECE FE00 F9CE 0300 CE00 00FC CEFE"
	$"00A8 CE00 FFEF CF00 00FA CFFE 00FD CF06 0000 CF00 CFCF 00FB CF06 00CF CF00 CF00"
	$"00FD CFFE 00FE CF01 00CF FE00 02CF CF00 FBCF 0700 CF00 00CF CF00 00FD CFFE 00FE"
	$"CF00 00FE CF00 00FB CF03 00CF 0000 FDCF FE00 FBCF FE00 05CF 00CF 00CF CFFE 00FD"
	$"CFFE 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FECF 0000 FECF FE00 FDCF FE00 FDCF"
	$"0300 00CF 00FB CF03 00CF 0000 FECF 0000 FECF 0000 FACF FE00 FECF 0300 CF00 00FE"
	$"CF00 00FE CF00 00FE CFFE 00FE CF03 00CF 0000 FDCF FE00 FACF 0000 FECF 0300 CF00"
	$"00FE CF00 00FE CF01 00CF FE00 FBCF 0700 CF00 00CF CF00 00FD CFFE 00FE CF00 00FE"
	$"CF00 00FB CF03 00CF 0000 FCCF FE00 FECF FE00 F9CF 0300 CF00 00FC CFFE 00A8 CF00"
	$"FFEF CE00 00FA CEFE 00FD CE06 0000 CE00 CECE 00FB CE06 00CE CE00 CE00 00FD CEFE"
	$"00FE CE01 00CE FE00 02CE CE00 FBCE 0700 CE00 00CE CE00 00FD CEFE 00FE CE00 00FE"
	$"CE00 00FB CE03 00CE 0000 FDCE FE00 FBCE FE00 05CE 00CE 00CE CEFE 00FD CEFE 00FE"
	$"CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE FE00 FDCE FE00 FDCE 0300 00CE"
	$"00FB CE03 00CE 0000 FECE 0000 FECE 0000 FACE FE00 FECE 0300 CE00 00FE CE00 00FE"
	$"CE00 00FE CEFE 00FE CE03 00CE 0000 FDCE FE00 FACE 0000 FECE 0300 CE00 00FE CE00"
	$"00FE CE01 00CE FE00 FBCE 0700 CE00 00CE CE00 00FD CEFE 00FE CE00 00FE CE00 00FB"
	$"CE03 00CE 0000 FCCE FE00 FECE FE00 F9CE 0300 CE00 00FC CEFE 00A8 CE00 FF03 7EEF"
	$"CE00 00FB CE00 00FE CE0A 00CE CE00 CECE 0000 CECE 00FB CE0A 00CE CE00 00CE CE00"
	$"CECE 00FE CE06 00CE CE00 CECE 00FE CE00 00FB CE0B 0000 CECE 0000 CECE 00CE CE00"
	$"FECE 0300 CECE 00FE CE00 00FB CE07 0000 CECE 00CE CE00 FECE 0000 FBCE 0700 CECE"
	$"0000 CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0000 FBCE 0000 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0700 CECE 00CE CE00 00FB CE07 0000 CECE 00CE"
	$"CE00 FECE 0000 FBCE 0000 FECE 0A00 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE"
	$"CE0A 00CE CE00 00CE CE00 CECE 00FE CE00 00FB CE00 00FE CE07 0000 CECE 00CE CE00"
	$"FECE 0300 CECE 00FA CE0B 0000 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE00"
	$"00FB CE04 0000 CECE 00FE CE00 00FE CE00 00FE CE00 00F8 CE04 0000 CECE 00FE CE00"
	$"00FE CE00 00A9 CE00 FFEF CF00 00FB CF00 00FE CF0A 00CF CF00 CFCF 0000 CFCF 00FB"
	$"CF0A 00CF CF00 00CF CF00 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF00 00FB CF0B 0000"
	$"CFCF 0000 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF07 0000 CFCF 00CF CF00"
	$"FECF 0000 FBCF 0700 CFCF 0000 CFCF 00FE CF03 00CF CF00 FECF 0600 CFCF 00CF CF00"
	$"FECF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0700 CFCF 00CF CF00"
	$"00FB CF07 0000 CFCF 00CF CF00 FECF 0000 FBCF 0000 FECF 0A00 CFCF 0000 CFCF 00CF"
	$"CF00 FECF 0300 CFCF 00FE CF0A 00CF CF00 00CF CF00 CFCF 00FE CF00 00FB CF00 00FE"
	$"CF07 0000 CFCF 00CF CF00 FECF 0300 CFCF 00FA CF0B 0000 CFCF 0000 CFCF 00CF CF00"
	$"FECF 0300 CFCF 00FE CF00 00FB CF04 0000 CFCF 00FE CF00 00FE CF00 00FE CF00 00F8"
	$"CF04 0000 CFCF 00FE CF00 00FE CF00 00A9 CF00 FFEF CE00 00FB CE00 00FE CE0A 00CE"
	$"CE00 CECE 0000 CECE 00FB CE0A 00CE CE00 00CE CE00 CECE 00FE CE06 00CE CE00 CECE"
	$"00FE CE00 00FB CE0B 0000 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FB"
	$"CE07 0000 CECE 00CE CE00 FECE 0000 FBCE 0700 CECE 0000 CECE 00FE CE03 00CE CE00"
	$"FECE 0600 CECE 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0700 CECE 00CE CE00 00FB CE07 0000 CECE 00CE CE00 FECE 0000 FBCE 0000 FECE"
	$"0A00 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE0A 00CE CE00 00CE CE00 CECE"
	$"00FE CE00 00FB CE00 00FE CE07 0000 CECE 00CE CE00 FECE 0300 CECE 00FA CE0B 0000"
	$"CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE04 0000 CECE 00FE CE00"
	$"00FE CE00 00FE CE00 00F8 CE04 0000 CECE 00FE CE00 00FE CE00 00A9 CE00 FF03 4BEF"
	$"CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0300"
	$"CECE 00FE CE06 00CE CE00 CECE 00FE CE00 00FB CE00 00FE CE00 00FE CE00 00FB CE00"
	$"00FE CE02 00CE 00FA CE00 00FE CE03 00CE CE00 FECE 0000 FBCE 0300 CECE 00FE CE00"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE02 00CE 00FA CE00 00FE CE03 00CE CE00"
	$"FBCE 0000 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FE CE02 00CE 00F6 CE03"
	$"00CE CE00 FECE 0000 FECE 0200 CE00 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FACE 0000 FECE"
	$"0000 FECE 0000 FBCE 0000 FECE 0200 CE00 FACE 0000 FECE 0000 FECE 0000 FECE 0000"
	$"FECE 0000 F8CE 0000 FECE 0000 FECE 0000 FECE 0000 A9CE 00FF EFCF 0000 FBCF 0000"
	$"FECF 0300 CFCF 00FE CF03 00CF CF00 FBCF 0300 CFCF 00FE CF03 00CF CF00 FECF 0600"
	$"CFCF 00CF CF00 FECF 0000 FBCF 0000 FECF 0000 FECF 0000 FBCF 0000 FECF 0200 CF00"
	$"FACF 0000 FECF 0300 CFCF 00FE CF00 00FB CF03 00CF CF00 FECF 0000 FECF 0300 CFCF"
	$"00FE CF03 00CF CF00 FECF 0200 CF00 FACF 0000 FECF 0300 CFCF 00FB CF00 00FE CF03"
	$"00CF CF00 FECF 0000 FBCF 0000 FECF 0000 FECF 0200 CF00 F6CF 0300 CFCF 00FE CF00"
	$"00FE CF02 00CF 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF00"
	$"00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FA CF00 00FE CF00 00FE CF00 00FB"
	$"CF00 00FE CF02 00CF 00FA CF00 00FE CF00 00FE CF00 00FE CF00 00FE CF00 00F8 CF00"
	$"00FE CF00 00FE CF00 00FE CF00 00A9 CF00 FFEF CE00 00FB CE00 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE"
	$"CE00 00FB CE00 00FE CE00 00FE CE00 00FB CE00 00FE CE02 00CE 00FA CE00 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CE02 00CE 00FA CE00 00FE CE03 00CE CE00 FBCE 0000 FECE 0300 CECE 00FE CE00"
	$"00FB CE00 00FE CE00 00FE CE02 00CE 00F6 CE03 00CE CE00 FECE 0000 FECE 0200 CE00"
	$"FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 FACE 0000 FECE 0000 FECE 0000 FBCE 0000 FECE 0200"
	$"CE00 FACE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 F8CE 0000 FECE 0000 FECE"
	$"0000 FECE 0000 A9CE 00FF 030C EFCE 0000 FBCE FC00 02CE CE00 FECE 0300 CECE 00FB"
	$"CE03 00CE CE00 FECE 0200 CECE FC00 05CE CE00 CECE 00FE CE00 00FB CE00 00FE CE00"
	$"00FE CE00 00FE CEFD 00FE CE02 00CE 00FA CE00 00FE CE02 00CE CEFC 00FB CE03 00CE"
	$"CE00 FECE FC00 01CE CEFC 0002 CECE 00FE CE02 00CE 00FA CE00 00FE CE00 00FE CEFE"
	$"00FE CEFC 0002 CECE 00FE CE00 00FB CE00 00FE CE00 00FE CE02 00CE 00F9 CEFD 0002"
	$"CECE 00FE CE00 00FE CE02 00CE 00FE CE00 00FE CE03 00CE CE00 FECE 0200 CECE FC00"
	$"FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FACE 0000 FECE 0000 FECE"
	$"0000 FECE FD00 FECE 0200 CE00 FACE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000"
	$"F8CE 0000 FECE 0000 FECE FC00 A9CE 00FF EFCF 0000 FBCF FC00 02CF CF00 FECF 0300"
	$"CFCF 00FB CF03 00CF CF00 FECF 0200 CFCF FC00 05CF CF00 CFCF 00FE CF00 00FB CF00"
	$"00FE CF00 00FE CF00 00FE CFFD 00FE CF02 00CF 00FA CF00 00FE CF02 00CF CFFC 00FB"
	$"CF03 00CF CF00 FECF FC00 01CF CFFC 0002 CFCF 00FE CF02 00CF 00FA CF00 00FE CF00"
	$"00FE CFFE 00FE CFFC 0002 CFCF 00FE CF00 00FB CF00 00FE CF00 00FE CF02 00CF 00F9"
	$"CFFD 0002 CFCF 00FE CF00 00FE CF02 00CF 00FE CF00 00FE CF03 00CF CF00 FECF 0200"
	$"CFCF FC00 FBCF 0000 FECF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FACF 0000 FECF"
	$"0000 FECF 0000 FECF FD00 FECF 0200 CF00 FACF 0000 FECF 0000 FECF 0000 FECF 0000"
	$"FECF 0000 F8CF 0000 FECF 0000 FECF FC00 A9CF 00FF EFCE 0000 FBCE FC00 02CE CE00"
	$"FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0200 CECE FC00 05CE CE00 CECE 00FE CE00"
	$"00FB CE00 00FE CE00 00FE CE00 00FE CEFD 00FE CE02 00CE 00FA CE00 00FE CE02 00CE"
	$"CEFC 00FB CE03 00CE CE00 FECE FC00 01CE CEFC 0002 CECE 00FE CE02 00CE 00FA CE00"
	$"00FE CE00 00FE CEFE 00FE CEFC 0002 CECE 00FE CE00 00FB CE00 00FE CE00 00FE CE02"
	$"00CE 00F9 CEFD 0002 CECE 00FE CE00 00FE CE02 00CE 00FE CE00 00FE CE03 00CE CE00"
	$"FECE 0200 CECE FC00 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FACE"
	$"0000 FECE 0000 FECE 0000 FECE FD00 FECE 0200 CE00 FACE 0000 FECE 0000 FECE 0000"
	$"FECE 0000 FECE 0000 F8CE 0000 FECE 0000 FECE FC00 A9CE 00FF 0318 EFCE 0000 FBCE"
	$"0000 FBCE 0000 FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0300 CECE 00FB CE03 00CE"
	$"CE00 FECE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE00 00FE CE02 00CE 00FA"
	$"CE00 00FE CE03 00CE CE00 F7CE 0300 CECE 00FE CE00 00FB CE00 00FB CE00 00FE CE02"
	$"00CE 00FA CE00 00FE CE00 00FB CE03 00CE CE00 FBCE 0000 FECE 0000 FBCE 0000 FECE"
	$"0000 FECE 0200 CE00 FACE 0000 FECE 0300 CECE 00FE CE00 00FE CE02 00CE 00FE CE00"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00F7 CE00 00FE CE00 00FE CE03 00CE CE00 FECE"
	$"0300 CECE 00FA CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0200 CE00 FACE"
	$"0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 F8CE 0000 FECE 0000 FECE 0000 A5CE"
	$"00FF EFCF 0000 FBCF 0000 FBCF 0000 FECF 0300 CFCF 00FB CF03 00CF CF00 FECF 0300"
	$"CFCF 00FB CF03 00CF CF00 FECF 0000 FBCF 0000 FECF 0000 FECF 0300 CFCF 00FE CF00"
	$"00FE CF02 00CF 00FA CF00 00FE CF03 00CF CF00 F7CF 0300 CFCF 00FE CF00 00FB CF00"
	$"00FB CF00 00FE CF02 00CF 00FA CF00 00FE CF00 00FB CF03 00CF CF00 FBCF 0000 FECF"
	$"0000 FBCF 0000 FECF 0000 FECF 0200 CF00 FACF 0000 FECF 0300 CFCF 00FE CF00 00FE"
	$"CF02 00CF 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00F7 CF00 00FE CF00 00FE"
	$"CF03 00CF CF00 FECF 0300 CFCF 00FA CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0000"
	$"FECF 0200 CF00 FACF 0000 FECF 0000 FECF 0000 FECF 0000 FECF 0000 F8CF 0000 FECF"
	$"0000 FECF 0000 A5CF 00FF EFCE 0000 FBCE 0000 FBCE 0000 FECE 0300 CECE 00FB CE03"
	$"00CE CE00 FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE"
	$"0300 CECE 00FE CE00 00FE CE02 00CE 00FA CE00 00FE CE03 00CE CE00 F7CE 0300 CECE"
	$"00FE CE00 00FB CE00 00FB CE00 00FE CE02 00CE 00FA CE00 00FE CE00 00FB CE03 00CE"
	$"CE00 FBCE 0000 FECE 0000 FBCE 0000 FECE 0000 FECE 0200 CE00 FACE 0000 FECE 0300"
	$"CECE 00FE CE00 00FE CE02 00CE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00F7"
	$"CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03"
	$"00CE CE00 FECE 0000 FECE 0200 CE00 FACE 0000 FECE 0000 FECE 0000 FECE 0000 FECE"
	$"0000 F8CE 0000 FECE 0000 FECE 0000 A5CE 00FF 0348 EFCE 0000 FBCE 0000 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FBCE 0300 CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE"
	$"CE00 F7CE 0000 FECE 0000 FECE 0700 CECE 00CE CE00 00FD CE00 00F9 CE00 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FD CE00 00F9 CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE"
	$"0000 FBCE 0000 FECE 0000 FDCE 0000 F9CE 0700 CECE 0000 CECE 00FE CE00 00FD CE00"
	$"00FD CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE07 00CE CE00 CECE 0000"
	$"FDCE 0000 F9CE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 F8CE 0000 FECE 0000"
	$"FECE 0000 FECE 0000 A9CE 00FF EFCF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF"
	$"CF00 FBCF 0300 CFCF 00FE CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 F7CF 0000 FECF"
	$"0000 FECF 0700 CFCF 00CF CF00 00FD CF00 00F9 CF00 00FE CF03 00CF CF00 FECF 0000"
	$"FBCF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FD CF00 00F9 CF00"
	$"00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FECF"
	$"0000 FDCF 0000 F9CF 0700 CFCF 0000 CFCF 00FE CF00 00FD CF00 00FD CF00 00FE CF03"
	$"00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF00 00FE CF00 00FE CF03 00CF CF00 FECF"
	$"0300 CFCF 00FA CF00 00FE CF00 00FE CF07 00CF CF00 CFCF 0000 FDCF 0000 F9CF 0000"
	$"FECF 0000 FECF 0000 FECF 0000 FECF 0000 F8CF 0000 FECF 0000 FECF 0000 FECF 0000"
	$"A9CF 00FF EFCE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 F7CE 0000 FECE 0000 FECE 0700 CECE"
	$"00CE CE00 00FD CE00 00F9 CE00 00FE CE03 00CE CE00 FECE 0000 FBCE 0300 CECE 00FE"
	$"CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FD CE00 00F9 CE00 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FDCE 0000 F9CE"
	$"0700 CECE 0000 CECE 00FE CE00 00FD CE00 00FD CE00 00FE CE03 00CE CE00 FECE 0300"
	$"CECE 00FE CE00 00FB CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FA CE00"
	$"00FE CE00 00FE CE07 00CE CE00 CECE 0000 FDCE 0000 F9CE 0000 FECE 0000 FECE 0000"
	$"FECE 0000 FECE 0000 F8CE 0000 FECE 0000 FECE 0000 FECE 0000 A9CE 00FF 02A0 EFCE"
	$"FB00 01CE CEFE 00FD CEFD 0001 CECE FB00 04CE 00CE CE00 FECE 0000 FECE FE00 FECE"
	$"0700 CECE 0000 CECE 00FB CE00 00FE CE00 00FE CE00 00FE CE03 0000 CE00 FDCE 0000"
	$"F9CE FD00 FDCE FE00 FACE 0300 CECE 00FD CEFE 00FD CEFE 00FE CE00 00FD CE00 00F8"
	$"CEFD 00FE CEFE 00FD CEFE 00FD CEFD 00FB CEFD 00FC CE00 00F8 CE06 0000 CE00 CECE"
	$"00FE CE00 00FD CE00 00FC CEFE 00FE CE00 00FE CE00 00FE CEFE 00FA CE00 00FE CEFD"
	$"00FD CEFD 0003 CECE 0000 FBCE 0000 FECE 0000 FECE 0000 FECE 0300 00CE 00FD CE00"
	$"00F9 CE00 00FE CE00 00FD CEFE 00FD CE01 0000 F9CE FD00 FCCE FE00 A8CE 00FF EFCF"
	$"FB00 01CF CFFE 00FD CFFD 0001 CFCF FB00 04CF 00CF CF00 FECF 0000 FECF FE00 FECF"
	$"0700 CFCF 0000 CFCF 00FB CF00 00FE CF00 00FE CF00 00FE CF03 0000 CF00 FDCF 0000"
	$"F9CF FD00 FDCF FE00 FACF 0300 CFCF 00FD CFFE 00FD CFFE 00FE CF00 00FD CF00 00F8"
	$"CFFD 00FE CFFE 00FD CFFE 00FD CFFD 00FB CFFD 00FC CF00 00F8 CF06 0000 CF00 CFCF"
	$"00FE CF00 00FD CF00 00FC CFFE 00FE CF00 00FE CF00 00FE CFFE 00FA CF00 00FE CFFD"
	$"00FD CFFD 0003 CFCF 0000 FBCF 0000 FECF 0000 FECF 0000 FECF 0300 00CF 00FD CF00"
	$"00F9 CF00 00FE CF00 00FD CFFE 00FD CF01 0000 F9CF FD00 FCCF FE00 A8CF 00FF EFCE"
	$"FB00 01CE CEFE 00FD CEFD 0001 CECE FB00 04CE 00CE CE00 FECE 0000 FECE FE00 FECE"
	$"0700 CECE 0000 CECE 00FB CE00 00FE CE00 00FE CE00 00FE CE03 0000 CE00 FDCE 0000"
	$"F9CE FD00 FDCE FE00 FACE 0300 CECE 00FD CEFE 00FD CEFE 00FE CE00 00FD CE00 00F8"
	$"CEFD 00FE CEFE 00FD CEFE 00FD CEFD 00FB CEFD 00FC CE00 00F8 CE06 0000 CE00 CECE"
	$"00FE CE00 00FD CE00 00FC CEFE 00FE CE00 00FE CE00 00FE CEFE 00FA CE00 00FE CEFD"
	$"00FD CEFD 0003 CECE 0000 FBCE 0000 FECE 0000 FECE 0000 FECE 0300 00CE 00FD CE00"
	$"00F9 CE00 00FE CE00 00FD CEFE 00FD CE01 0000 F9CE FD00 FCCE FE00 A8CE 00FF 005A"
	$"A0CE 0000 CDCE 0000 D0CE 0000 E9CE 0000 E3CE 0000 D5CE 0000 81CE FACE 00FF A0CF"
	$"0000 CDCF 0000 D0CF 0000 E9CF 0000 E3CF 0000 D5CF 0000 81CF FACF 00FF A0CE 0000"
	$"CDCE 0000 D0CE 0000 E9CE 0000 E3CE 0000 D5CE 0000 81CE FACE 00FF 005A A0CE 0000"
	$"CDCE 0000 D0CE 0000 E9CE 0000 E3CE 0000 D5CE 0000 81CE FACE 00FF A0CF 0000 CDCF"
	$"0000 D0CF 0000 E9CF 0000 E3CF 0000 D5CF 0000 81CF FACF 00FF A0CE 0000 CDCE 0000"
	$"D0CE 0000 E9CE 0000 E3CE 0000 D5CE 0000 81CE FACE 00FF 0069 A2CE 0100 00CE CE01"
	$"0000 D1CE 0100 00EA CE01 0000 E1CE 0000 D8CE 0100 0081 CEF9 CE00 FFA2 CF01 0000"
	$"CECF 0100 00D1 CF01 0000 EACF 0100 00E1 CF00 00D8 CF01 0000 81CF F9CF 00FF A2CE"
	$"0100 00CE CE01 0000 D1CE 0100 00EA CE01 0000 E1CE 0000 D8CE 0100 0081 CEF9 CE00"
	$"FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81 CECC CE00"
	$"FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81 CECC CE00"
	$"FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81 CECC CE00"
	$"FF00 EAE0 CE03 00CE CE00 F0CE 0300 CECE 00E5 CE00 00F2 CE00 00FB CE00 00C9 CE00"
	$"00FE CEFE 00F4 CE03 00CE CE00 D7CE 0100 00E6 CEFC 00F0 CE00 00FB CE00 00EF CEFC"
	$"00F7 CE00 00F4 CE00 00E8 CE00 00B9 CE00 FFE0 CF03 00CF CF00 F0CF 0300 CFCF 00E5"
	$"CF00 00F2 CF00 00FB CF00 00C9 CF00 00FE CFFE 00F4 CF03 00CF CF00 D7CF 0100 00E6"
	$"CFFC 00F0 CF00 00FB CF00 00EF CFFC 00F7 CF00 00F4 CF00 00E8 CF00 00B9 CF00 FFE0"
	$"CE03 00CE CE00 F0CE 0300 CECE 00E5 CE00 00F2 CE00 00FB CE00 00C9 CE00 00FE CEFE"
	$"00F4 CE03 00CE CE00 D7CE 0100 00E6 CEFC 00F0 CE00 00FB CE00 00EF CEFC 00F7 CE00"
	$"00F4 CE00 00E8 CE00 00B9 CE00 FF00 FCE0 CE00 00F5 CE00 00F7 CE00 00F4 CE00 00F3"
	$"CE00 00EF CE00 00FE CE00 00ED CE00 00DB CE00 00F1 CE00 00D5 CE00 00E5 CE00 00FC"
	$"CE00 00F1 CE00 00F1 CE00 00FA CE00 00FC CE00 00F8 CE00 00F8 CE00 00E3 CE00 00BA"
	$"CE00 FFE0 CF00 00F5 CF00 00F7 CF00 00F4 CF00 00F3 CF00 00EF CF00 00FE CF00 00ED"
	$"CF00 00DB CF00 00F1 CF00 00D5 CF00 00E5 CF00 00FC CF00 00F1 CF00 00F1 CF00 00FA"
	$"CF00 00FC CF00 00F8 CF00 00F8 CF00 00E3 CF00 00BA CF00 FFE0 CE00 00F5 CE00 00F7"
	$"CE00 00F4 CE00 00F3 CE00 00EF CE00 00FE CE00 00ED CE00 00DB CE00 00F1 CE00 00D5"
	$"CE00 00E5 CE00 00FC CE00 00F1 CE00 00F1 CE00 00FA CE00 00FC CE00 00F8 CE00 00F8"
	$"CE00 00E3 CE00 00BA CE00 FF00 FCE0 CE00 00F5 CE00 00F7 CE00 00F4 CE00 00F3 CE00"
	$"00EF CE00 00FE CE00 00ED CE00 00DB CE00 00F1 CE00 00D5 CE00 00E5 CE00 00FC CE00"
	$"00F1 CE00 00F1 CE00 00FA CE00 00FC CE00 00F8 CE00 00F8 CE00 00E3 CE00 00BA CE00"
	$"FFE0 CF00 00F5 CF00 00F7 CF00 00F4 CF00 00F3 CF00 00EF CF00 00FE CF00 00ED CF00"
	$"00DB CF00 00F1 CF00 00D5 CF00 00E5 CF00 00FC CF00 00F1 CF00 00F1 CF00 00FA CF00"
	$"00FC CF00 00F8 CF00 00F8 CF00 00E3 CF00 00BA CF00 FFE0 CE00 00F5 CE00 00F7 CE00"
	$"00F4 CE00 00F3 CE00 00EF CE00 00FE CE00 00ED CE00 00DB CE00 00F1 CE00 00D5 CE00"
	$"00E5 CE00 00FC CE00 00F1 CE00 00F1 CE00 00FA CE00 00FC CE00 00F8 CE00 00F8 CE00"
	$"00E3 CE00 00BA CE00 FF03 39EF CE04 00CE 00CE CEFE 00FD CE06 0000 CE00 CECE 00FE"
	$"CEFE 0001 CECE FE00 0CCE CE00 CE00 CE00 CECE 00CE 0000 FECE 0000 FECE 0100 CEFE"
	$"00FE CEFE 00FD CE03 0000 CE00 FCCE 0000 FECE 0000 FECE 0300 CE00 CEFE 0005 CECE"
	$"00CE 0000 FDCE FE00 FECE 0000 FECE 0100 CEFE 00FA CEFE 00FE CE03 00CE 0000 FDCE"
	$"FE00 FDCE FE00 FECE 0100 CEFE 0001 CE00 FECE FE00 FACE 0300 CECE 00FE CEFE 00FD"
	$"CEFE 00FE CE03 00CE 0000 FDCE FE00 FDCE FE00 FBCE FE00 05CE 00CE 00CE CEFE 00FE"
	$"CE07 00CE 0000 CECE 0000 FACE 0000 F8CE FE00 FECE 0300 CE00 00FE CE03 00CE 0000"
	$"FECE 0000 FECE FE00 01CE CEFE 00FB CE00 00F8 CEFE 00FE CE03 00CE CE00 FECE 0100"
	$"CEFE 0002 CECE 00FE CEFE 00FD CE03 00CE 0000 FCCE FE00 FCCE 0000 BBCE 00FF EFCF"
	$"0400 CF00 CFCF FE00 FDCF 0600 00CF 00CF CF00 FECF FE00 01CF CFFE 000C CFCF 00CF"
	$"00CF 00CF CF00 CF00 00FE CF00 00FE CF01 00CF FE00 FECF FE00 FDCF 0300 00CF 00FC"
	$"CF00 00FE CF00 00FE CF03 00CF 00CF FE00 05CF CF00 CF00 00FD CFFE 00FE CF00 00FE"
	$"CF01 00CF FE00 FACF FE00 FECF 0300 CF00 00FD CFFE 00FD CFFE 00FE CF01 00CF FE00"
	$"01CF 00FE CFFE 00FA CF03 00CF CF00 FECF FE00 FDCF FE00 FECF 0300 CF00 00FD CFFE"
	$"00FD CFFE 00FB CFFE 0005 CF00 CF00 CFCF FE00 FECF 0700 CF00 00CF CF00 00FA CF00"
	$"00F8 CFFE 00FE CF03 00CF 0000 FECF 0300 CF00 00FE CF00 00FE CFFE 0001 CFCF FE00"
	$"FBCF 0000 F8CF FE00 FECF 0300 CFCF 00FE CF01 00CF FE00 02CF CF00 FECF FE00 FDCF"
	$"0300 CF00 00FC CFFE 00FC CF00 00BB CF00 FFEF CE04 00CE 00CE CEFE 00FD CE06 0000"
	$"CE00 CECE 00FE CEFE 0001 CECE FE00 0CCE CE00 CE00 CE00 CECE 00CE 0000 FECE 0000"
	$"FECE 0100 CEFE 00FE CEFE 00FD CE03 0000 CE00 FCCE 0000 FECE 0000 FECE 0300 CE00"
	$"CEFE 0005 CECE 00CE 0000 FDCE FE00 FECE 0000 FECE 0100 CEFE 00FA CEFE 00FE CE03"
	$"00CE 0000 FDCE FE00 FDCE FE00 FECE 0100 CEFE 0001 CE00 FECE FE00 FACE 0300 CECE"
	$"00FE CEFE 00FD CEFE 00FE CE03 00CE 0000 FDCE FE00 FDCE FE00 FBCE FE00 05CE 00CE"
	$"00CE CEFE 00FE CE07 00CE 0000 CECE 0000 FACE 0000 F8CE FE00 FECE 0300 CE00 00FE"
	$"CE03 00CE 0000 FECE 0000 FECE FE00 01CE CEFE 00FB CE00 00F8 CEFE 00FE CE03 00CE"
	$"CE00 FECE 0100 CEFE 0002 CECE 00FE CEFE 00FD CE03 00CE 0000 FCCE FE00 FCCE 0000"
	$"BBCE 00FF 03D2 EFCE 0400 00CE CE00 FECE 0D00 CECE 00CE CE00 00CE CE00 CECE 00FE"
	$"CE03 00CE CE00 FECE 0E00 00CE CE00 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FE CE07 00CE CE00 CECE 0000 FCCE 0000 FECE 0000 FECE 0500 CE00 CECE 00FE"
	$"CE07 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FACE 0000 FECE 0A00"
	$"CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE0C 00CE CE00 CECE 00CE CE00 CECE"
	$"00FE CE00 00FB CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0A00 CECE 0000 CECE"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE07 00CE CE00 00CE CE00 FECE 0B00 CECE"
	$"0000 CECE 0000 CECE 00FA CEFE 00FC CE00 00FE CE14 00CE CE00 00CE CE00 CECE 0000"
	$"CECE 00CE CE00 CECE 00FE CE03 00CE CE00 F9CE FE00 FCCE 0000 FECE 0600 CECE 00CE"
	$"CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0400 00CE CE00 FECE 0000"
	$"FECE 0000 FDCE 0000 BBCE 00FF EFCF 0400 00CF CF00 FECF 0D00 CFCF 00CF CF00 00CF"
	$"CF00 CFCF 00FE CF03 00CF CF00 FECF 0E00 00CF CF00 CFCF 0000 CFCF 00CF CF00 FECF"
	$"0300 CFCF 00FE CF00 00FE CF07 00CF CF00 CFCF 0000 FCCF 0000 FECF 0000 FECF 0500"
	$"CF00 CFCF 00FE CF07 0000 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FACF"
	$"0000 FECF 0A00 CFCF 0000 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF0C 00CF CF00 CFCF"
	$"00CF CF00 CFCF 00FE CF00 00FB CF06 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0A00"
	$"CFCF 0000 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF07 00CF CF00 00CF CF00"
	$"FECF 0B00 CFCF 0000 CFCF 0000 CFCF 00FA CFFE 00FC CF00 00FE CF14 00CF CF00 00CF"
	$"CF00 CFCF 0000 CFCF 00CF CF00 CFCF 00FE CF03 00CF CF00 F9CF FE00 FCCF 0000 FECF"
	$"0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0400 00CF"
	$"CF00 FECF 0000 FECF 0000 FDCF 0000 BBCF 00FF EFCE 0400 00CE CE00 FECE 0D00 CECE"
	$"00CE CE00 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0E00 00CE CE00 CECE 0000 CECE"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE07 00CE CE00 CECE 0000 FCCE 0000 FECE"
	$"0000 FECE 0500 CE00 CECE 00FE CE07 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FACE 0000 FECE 0A00 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE0C"
	$"00CE CE00 CECE 00CE CE00 CECE 00FE CE00 00FB CE06 00CE CE00 CECE 00FE CE03 00CE"
	$"CE00 FECE 0A00 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE07 00CE"
	$"CE00 00CE CE00 FECE 0B00 CECE 0000 CECE 0000 CECE 00FA CEFE 00FC CE00 00FE CE14"
	$"00CE CE00 00CE CE00 CECE 0000 CECE 00CE CE00 CECE 00FE CE03 00CE CE00 F9CE FE00"
	$"FCCE 0000 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000"
	$"FECE 0400 00CE CE00 FECE 0000 FECE 0000 FDCE 0000 BBCE 00FF 0387 EFCE 0000 FECE"
	$"0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FB CE00 00FE CE00 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00FB"
	$"CE0C 00CE 00CE 00CE 00CE CE00 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FACE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE"
	$"0900 CECE 00CE CE00 CECE 00F7 CE06 00CE CE00 CECE 00FB CE00 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FB CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0000 FECE 0300 CECE"
	$"00FE CE00 00FE CE00 00F7 CE01 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0600 CECE 00CE CE00 FBCE 0000 F6CE 0100 00FE CE00 00FE CE06 00CE CE00 CECE"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE00 00FE CE00 00F9"
	$"CE00 00BB CE00 FFEF CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0600 CFCF 00CF CF00"
	$"FBCF 0000 FECF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00"
	$"00FE CF03 00CF CF00 FECF 0000 FBCF 0C00 CF00 CF00 CF00 CFCF 00CF CF00 FECF 0000"
	$"FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FA CF00 00FB CF00 00FE CF03"
	$"00CF CF00 FECF 0300 CFCF 00FB CF09 00CF CF00 CFCF 00CF CF00 F7CF 0600 CFCF 00CF"
	$"CF00 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FBCF 0000 FECF 0000 FBCF 0300"
	$"CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0000 FECF 0000 F7CF 0100 00FE CF00 00FE"
	$"CF03 00CF CF00 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FB CF00 00F6 CF01 0000"
	$"FECF 0000 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000"
	$"FECF 0000 FECF 0000 FECF 0000 F9CF 0000 BBCF 00FF EFCE 0000 FECE 0000 FECE 0300"
	$"CECE 00FE CE06 00CE CE00 CECE 00FB CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00FB CE0C 00CE 00CE"
	$"00CE 00CE CE00 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FACE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0900 CECE 00CE"
	$"CE00 CECE 00F7 CE06 00CE CE00 CECE 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FB CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00FE"
	$"CE00 00F7 CE01 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0600 CECE"
	$"00CE CE00 FBCE 0000 F6CE 0100 00FE CE00 00FE CE06 00CE CE00 CECE 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE00 00FE CE00 00F9 CE00 00BB CE00"
	$"FF03 57EF CE00 00FE CEFC 0002 CECE 00FE CE03 00CE CE00 FECE FE00 FECE 0000 FECE"
	$"0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CEFC 0002 CECE 00FE"
	$"CE00 00FB CE0C 00CE 00CE 00CE 00CE CE00 CECE 00FE CE00 00FE CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 F9CE FE00 FECE 0000 FECE 0200 CECE FC00 02CE CE00"
	$"FBCE 0900 CECE 00CE CE00 CECE 00F7 CE06 00CE CE00 CECE 00FB CEFC 0002 CECE 00FE"
	$"CE00 00FE CEFE 00FE CEFC 00FB CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00"
	$"00FE CE00 00F5 CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CEFE 00FE CE00 00F4 CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FDCE FE00 FCCE 0000 BBCE 00FF"
	$"EFCF 0000 FECF FC00 02CF CF00 FECF 0300 CFCF 00FE CFFE 00FE CF00 00FE CF00 00FE"
	$"CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF FC00 02CF CF00 FECF 0000"
	$"FBCF 0C00 CF00 CF00 CF00 CFCF 00CF CF00 FECF 0000 FECF 0300 CFCF 00FE CF03 00CF"
	$"CF00 FECF 0300 CFCF 00F9 CFFE 00FE CF00 00FE CF02 00CF CFFC 0002 CFCF 00FB CF09"
	$"00CF CF00 CFCF 00CF CF00 F7CF 0600 CFCF 00CF CF00 FBCF FC00 02CF CF00 FECF 0000"
	$"FECF FE00 FECF FC00 FBCF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0000 FECF"
	$"0000 F5CF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF"
	$"FE00 FECF 0000 F4CF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF"
	$"0300 CFCF 00FE CF00 00FE CF00 00FE CF00 00FD CFFE 00FC CF00 00BB CF00 FFEF CE00"
	$"00FE CEFC 0002 CECE 00FE CE03 00CE CE00 FECE FE00 FECE 0000 FECE 0000 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CEFC 0002 CECE 00FE CE00 00FB CE0C"
	$"00CE 00CE 00CE 00CE CE00 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE"
	$"CE03 00CE CE00 F9CE FE00 FECE 0000 FECE 0200 CECE FC00 02CE CE00 FBCE 0900 CECE"
	$"00CE CE00 CECE 00F7 CE06 00CE CE00 CECE 00FB CEFC 0002 CECE 00FE CE00 00FE CEFE"
	$"00FE CEFC 00FB CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00FE CE00 00F5"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CEFE 00FE"
	$"CE00 00F4 CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0000 FECE 0000 FECE 0000 FDCE FE00 FCCE 0000 BBCE 00FF 0360 EFCE 0000"
	$"FECE 0000 FBCE 0000 FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0000 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FB CE0C 00CE"
	$"00CE 00CE 00CE CE00 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 F6CE 0300 CECE 00FE CE03 00CE CE00 FBCE 0000 FBCE 0900 CECE 00CE CE00"
	$"CECE 00F7 CE06 00CE CE00 CECE 00FB CE00 00FB CE00 00FE CE00 00FB CE03 00CE CE00"
	$"F7CE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FBCE 0000 FCCE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0300 CECE"
	$"00FA CE00 00FC CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FECE 0000 FECE 0000 FACE 0000 FDCE 0000 BBCE 00FF EFCF 0000"
	$"FECF 0000 FBCF 0000 FECF 0300 CFCF 00FB CF03 00CF CF00 FECF 0000 FECF 0300 CFCF"
	$"00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF00 00FE CF00 00FB CF0C 00CF"
	$"00CF 00CF 00CF CF00 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03"
	$"00CF CF00 F6CF 0300 CFCF 00FE CF03 00CF CF00 FBCF 0000 FBCF 0900 CFCF 00CF CF00"
	$"CFCF 00F7 CF06 00CF CF00 CFCF 00FB CF00 00FB CF00 00FE CF00 00FB CF03 00CF CF00"
	$"F7CF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FBCF 0000 FCCF"
	$"0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FBCF 0300 CFCF"
	$"00FA CF00 00FC CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03"
	$"00CF CF00 FECF 0000 FECF 0000 FECF 0000 FACF 0000 FDCF 0000 BBCF 00FF EFCE 0000"
	$"FECE 0000 FBCE 0000 FECE 0300 CECE 00FB CE03 00CE CE00 FECE 0000 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FB CE0C 00CE"
	$"00CE 00CE 00CE CE00 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 F6CE 0300 CECE 00FE CE03 00CE CE00 FBCE 0000 FBCE 0900 CECE 00CE CE00"
	$"CECE 00F7 CE06 00CE CE00 CECE 00FB CE00 00FB CE00 00FE CE00 00FB CE03 00CE CE00"
	$"F7CE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FBCE 0000 FCCE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE 0300 CECE"
	$"00FA CE00 00FC CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FECE 0000 FECE 0000 FACE 0000 FDCE 0000 BBCE 00FF 03DE EFCE"
	$"0000 FECE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE"
	$"0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE03 00CE"
	$"CE00 FECE 0000 FACE 0000 FECE 0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FE CE03 00CE CE00 FACE 0000 FECE 0A00 CECE 0000 CECE 00CE CE00"
	$"FECE 0300 CECE 00FE CE0C 00CE CE00 CECE 00CE CE00 CECE 00FE CE00 00FB CE06 00CE"
	$"CE00 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CE00 00FB CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00FE CE00 00FB"
	$"CE00 00FC CE03 00CE CE00 FECE 0A00 CECE 0000 CECE 00CE CE00 FECE 0600 CECE 00CE"
	$"CE00 FECE 0300 CECE 00FA CE00 00FC CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000"
	$"FDCE 0000 BBCE 00FF EFCF 0000 FECF 0000 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF"
	$"00FE CF03 00CF CF00 FECF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF"
	$"00FE CF00 00FE CF03 00CF CF00 FECF 0000 FACF 0000 FECF 0000 FECF 0300 CFCF 00FE"
	$"CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FACF 0000 FECF 0A00"
	$"CFCF 0000 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF0C 00CF CF00 CFCF 00CF CF00 CFCF"
	$"00FE CF00 00FB CF06 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03"
	$"00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF03 00CF CF00 FECF 0000 FECF 0300 CFCF"
	$"00FE CF00 00FE CF00 00FB CF00 00FC CF03 00CF CF00 FECF 0A00 CFCF 0000 CFCF 00CF"
	$"CF00 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FA CF00 00FC CF03 00CF CF00 FECF"
	$"0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FECF"
	$"0000 FECF 0000 FECF 0000 FDCF 0000 BBCF 00FF EFCE 0000 FECE 0000 FECE 0300 CECE"
	$"00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FACE 0000 FECE"
	$"0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FACE 0000 FECE 0A00 CECE 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE0C 00CE"
	$"CE00 CECE 00CE CE00 CECE 00FE CE00 00FB CE06 00CE CE00 CECE 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE03 00CE CE00"
	$"FECE 0000 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE00 00FC CE03 00CE CE00 FECE"
	$"0A00 CECE 0000 CECE 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FA CE00"
	$"00FC CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FDCE 0000 BBCE 00FF 0315 EFCE"
	$"0000 FDCE FE00 FDCE FD00 02CE CE00 FECE FE00 FECE 0400 00CE CE00 FECE 0200 CECE"
	$"FD00 FDCE FD00 03CE CE00 00FE CEFE 00FD CEFD 00FA CE00 00FE CE00 00FE CE07 00CE"
	$"CE00 00CE CE00 FECE 0000 FECE FE00 FDCE FD00 03CE CE00 00FA CEFE 00FE CE03 00CE"
	$"0000 FDCE FE00 FDCE FE00 FECE 0600 CECE 00CE CE00 FECE FE00 FACE 0300 CECE 00FE"
	$"CEFE 00FD CEFE 00FE CE00 00FE CE00 00FE CEFE 00FD CEFE 00FA CE03 00CE CE00 FDCE"
	$"FE00 FECE 0000 FECE 0000 FECE 0000 FACE FC00 FDCE FE00 FECE 0300 CE00 00FE CE00"
	$"00FE CE03 00CE CE00 FECE FE00 FECE 0100 00FA CEFC 00FD CEFE 00FE CE00 00FE CEFD"
	$"0006 CECE 0000 CECE 00FE CEFE 00FD CE00 00FE CE00 00FD CEFE 00FC CE00 00FE CE00"
	$"00BF CE00 FFEF CF00 00FD CFFE 00FD CFFD 0002 CFCF 00FE CFFE 00FE CF04 0000 CFCF"
	$"00FE CF02 00CF CFFD 00FD CFFD 0003 CFCF 0000 FECF FE00 FDCF FD00 FACF 0000 FECF"
	$"0000 FECF 0700 CFCF 0000 CFCF 00FE CF00 00FE CFFE 00FD CFFD 0003 CFCF 0000 FACF"
	$"FE00 FECF 0300 CF00 00FD CFFE 00FD CFFE 00FE CF06 00CF CF00 CFCF 00FE CFFE 00FA"
	$"CF03 00CF CF00 FECF FE00 FDCF FE00 FECF 0000 FECF 0000 FECF FE00 FDCF FE00 FACF"
	$"0300 CFCF 00FD CFFE 00FE CF00 00FE CF00 00FE CF00 00FA CFFC 00FD CFFE 00FE CF03"
	$"00CF 0000 FECF 0000 FECF 0300 CFCF 00FE CFFE 00FE CF01 0000 FACF FC00 FDCF FE00"
	$"FECF 0000 FECF FD00 06CF CF00 00CF CF00 FECF FE00 FDCF 0000 FECF 0000 FDCF FE00"
	$"FCCF 0000 FECF 0000 BFCF 00FF EFCE 0000 FDCE FE00 FDCE FD00 02CE CE00 FECE FE00"
	$"FECE 0400 00CE CE00 FECE 0200 CECE FD00 FDCE FD00 03CE CE00 00FE CEFE 00FD CEFD"
	$"00FA CE00 00FE CE00 00FE CE07 00CE CE00 00CE CE00 FECE 0000 FECE FE00 FDCE FD00"
	$"03CE CE00 00FA CEFE 00FE CE03 00CE 0000 FDCE FE00 FDCE FE00 FECE 0600 CECE 00CE"
	$"CE00 FECE FE00 FACE 0300 CECE 00FE CEFE 00FD CEFE 00FE CE00 00FE CE00 00FE CEFE"
	$"00FD CEFE 00FA CE03 00CE CE00 FDCE FE00 FECE 0000 FECE 0000 FECE 0000 FACE FC00"
	$"FDCE FE00 FECE 0300 CE00 00FE CE00 00FE CE03 00CE CE00 FECE FE00 FECE 0100 00FA"
	$"CEFC 00FD CEFE 00FE CE00 00FE CEFD 0006 CECE 0000 CECE 00FE CEFE 00FD CE00 00FE"
	$"CE00 00FD CEFE 00FC CE00 00FE CE00 00BF CE00 FF00 3C81 CEF0 CE00 0081 CE02 CECE"
	$"00A9 CE00 00BA CE00 FF81 CFF0 CF00 0081 CF02 CFCF 00A9 CF00 00BA CF00 FF81 CEF0"
	$"CE00 0081 CE02 CECE 00A9 CE00 00BA CE00 FF00 3C81 CEF0 CE00 0081 CE02 CECE 00A9"
	$"CE00 00BA CE00 FF81 CFF0 CF00 0081 CF02 CFCF 00A9 CF00 00BA CF00 FF81 CEF0 CE00"
	$"0081 CE02 CECE 00A9 CE00 00BA CE00 FF00 3C81 CEF0 CE00 0081 CE02 CECE 00AA CE00"
	$"00B9 CE00 FF81 CFF0 CF00 0081 CF02 CFCF 00AA CF00 00B9 CF00 FF81 CEF0 CE00 0081"
	$"CE02 CECE 00AA CE00 00B9 CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC"
	$"CF00 FF81 CE81 CE81 CECC CE00 FF00 C6BF CEFB 00CF CE00 00F7 CE01 0000 E0CE 0000"
	$"DACE FC00 F0CE 0000 FBCE 0000 EFCE FC00 F7CE 0000 F4CE 0000 EACE 0000 FBCE 0100"
	$"00EF CE03 00CE CE00 ECCE 0100 00B7 CE00 FFBF CFFB 00CF CF00 00F7 CF01 0000 E0CF"
	$"0000 DACF FC00 F0CF 0000 FBCF 0000 EFCF FC00 F7CF 0000 F4CF 0000 EACF 0000 FBCF"
	$"0100 00EF CF03 00CF CF00 ECCF 0100 00B7 CF00 FFBF CEFB 00CF CE00 00F7 CE01 0000"
	$"E0CE 0000 DACE FC00 F0CE 0000 FBCE 0000 EFCE FC00 F7CE 0000 F4CE 0000 EACE 0000"
	$"FBCE 0100 00EF CE03 00CE CE00 ECCE 0100 00B7 CE00 FF00 D8BF CE00 00C0 CE00 00E2"
	$"CE00 00D7 CE00 00FC CE00 00F1 CE00 00F1 CE00 00FA CE00 00FC CE00 00F8 CE00 00F8"
	$"CE00 00E6 CE00 00FC CE00 00EA CE00 00ED CE00 00F9 CE00 00ED CE00 00D3 CE00 FFBF"
	$"CF00 00C0 CF00 00E2 CF00 00D7 CF00 00FC CF00 00F1 CF00 00F1 CF00 00FA CF00 00FC"
	$"CF00 00F8 CF00 00F8 CF00 00E6 CF00 00FC CF00 00EA CF00 00ED CF00 00F9 CF00 00ED"
	$"CF00 00D3 CF00 FFBF CE00 00C0 CE00 00E2 CE00 00D7 CE00 00FC CE00 00F1 CE00 00F1"
	$"CE00 00FA CE00 00FC CE00 00F8 CE00 00F8 CE00 00E6 CE00 00FC CE00 00EA CE00 00ED"
	$"CE00 00F9 CE00 00ED CE00 00D3 CE00 FF00 D8BF CE00 00C0 CE00 00E2 CE00 00D7 CE00"
	$"00FC CE00 00F1 CE00 00F1 CE00 00FA CE00 00FC CE00 00F8 CE00 00F8 CE00 00E6 CE00"
	$"00FC CE00 00EA CE00 00ED CE00 00F9 CE00 00ED CE00 00D3 CE00 FFBF CF00 00C0 CF00"
	$"00E2 CF00 00D7 CF00 00FC CF00 00F1 CF00 00F1 CF00 00FA CF00 00FC CF00 00F8 CF00"
	$"00F8 CF00 00E6 CF00 00FC CF00 00EA CF00 00ED CF00 00F9 CF00 00ED CF00 00D3 CF00"
	$"FFBF CE00 00C0 CE00 00E2 CE00 00D7 CE00 00FC CE00 00F1 CE00 00F1 CE00 00FA CE00"
	$"00FC CE00 00F8 CE00 00F8 CE00 00E6 CE00 00FC CE00 00EA CE00 00ED CE00 00F9 CE00"
	$"00ED CE00 00D3 CE00 FF02 F1BF CE00 00F9 CEFE 00FE CE02 00CE 00FC CE07 00CE 0000"
	$"CECE 0000 FDCE FE00 FECE 0400 CE00 CECE FE00 FACE 0800 CECE 00CE 0000 CECE FE00"
	$"01CE CEFE 00FE CE0B 00CE 00CE 00CE 0000 CECE 0000 FDCE FE00 01CE CEFE 0002 CECE"
	$"00FE CEFE 00FE CE03 00CE 0000 F9CE FE00 FECE 0300 CE00 00FA CE00 00F8 CEFE 00FE"
	$"CE03 00CE 0000 FECE 0300 CE00 00FE CE00 00FE CEFE 0001 CECE FE00 FBCE 0000 F8CE"
	$"FE00 FECE 0300 CECE 00FE CE01 00CE FE00 02CE CE00 FECE FE00 FECE 0300 CE00 00FD"
	$"CEFE 00F9 CEFE 0001 CECE FE00 FECE 0700 CE00 00CE CE00 00FE CE06 00CE CE00 CECE"
	$"00FE CE00 00FA CEFE 0001 CECE FE00 FBCE FE00 FDCE FE00 FDCE 0000 FECE 0200 CECE"
	$"FE00 D4CE 00FF BFCF 0000 F9CF FE00 FECF 0200 CF00 FCCF 0700 CF00 00CF CF00 00FD"
	$"CFFE 00FE CF04 00CF 00CF CFFE 00FA CF08 00CF CF00 CF00 00CF CFFE 0001 CFCF FE00"
	$"FECF 0B00 CF00 CF00 CF00 00CF CF00 00FD CFFE 0001 CFCF FE00 02CF CF00 FECF FE00"
	$"FECF 0300 CF00 00F9 CFFE 00FE CF03 00CF 0000 FACF 0000 F8CF FE00 FECF 0300 CF00"
	$"00FE CF03 00CF 0000 FECF 0000 FECF FE00 01CF CFFE 00FB CF00 00F8 CFFE 00FE CF03"
	$"00CF CF00 FECF 0100 CFFE 0002 CFCF 00FE CFFE 00FE CF03 00CF 0000 FDCF FE00 F9CF"
	$"FE00 01CF CFFE 00FE CF07 00CF 0000 CFCF 0000 FECF 0600 CFCF 00CF CF00 FECF 0000"
	$"FACF FE00 01CF CFFE 00FB CFFE 00FD CFFE 00FD CF00 00FE CF02 00CF CFFE 00D4 CF00"
	$"FFBF CE00 00F9 CEFE 00FE CE02 00CE 00FC CE07 00CE 0000 CECE 0000 FDCE FE00 FECE"
	$"0400 CE00 CECE FE00 FACE 0800 CECE 00CE 0000 CECE FE00 01CE CEFE 00FE CE0B 00CE"
	$"00CE 00CE 0000 CECE 0000 FDCE FE00 01CE CEFE 0002 CECE 00FE CEFE 00FE CE03 00CE"
	$"0000 F9CE FE00 FECE 0300 CE00 00FA CE00 00F8 CEFE 00FE CE03 00CE 0000 FECE 0300"
	$"CE00 00FE CE00 00FE CEFE 0001 CECE FE00 FBCE 0000 F8CE FE00 FECE 0300 CECE 00FE"
	$"CE01 00CE FE00 02CE CE00 FECE FE00 FECE 0300 CE00 00FD CEFE 00F9 CEFE 0001 CECE"
	$"FE00 FECE 0700 CE00 00CE CE00 00FE CE06 00CE CE00 CECE 00FE CE00 00FA CEFE 0001"
	$"CECE FE00 FBCE FE00 FDCE FE00 FDCE 0000 FECE 0200 CECE FE00 D4CE 00FF 035A BFCE"
	$"FC00 FECE 0000 FECE 0400 CECE 0000 FBCE 0B00 00CE CE00 00CE CE00 CECE 00FE CE07"
	$"00CE CE00 00CE CE00 FECE 0000 FBCE 0D00 CECE 0000 CECE 00CE CE00 CECE 00FE CE12"
	$"00CE CE00 00CE CE00 00CE CE00 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CE07 00CE CE00 00CE CE00 FBCE 0000 FECE 0700 CECE 0000 CECE 00FA CEFE 00FC"
	$"CE00 00FE CE14 00CE CE00 00CE CE00 CECE 0000 CECE 00CE CE00 CECE 00FE CE03 00CE"
	$"CE00 F9CE FE00 FCCE 0000 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0A00 CECE 0000 CECE 00CE CE00 FECE 0000 F9CE 0300 CECE 00FE CE14 00CE"
	$"CE00 00CE CE00 00CE CE00 CECE 00CE CE00 CECE 00FE CE00 00FB CE00 00FE CE03 00CE"
	$"CE00 F9CE 0000 FDCE 0000 FECE 0000 FDCE 0200 CE00 FDCE 0000 D3CE 00FF BFCF FC00"
	$"FECF 0000 FECF 0400 CFCF 0000 FBCF 0B00 00CF CF00 00CF CF00 CFCF 00FE CF07 00CF"
	$"CF00 00CF CF00 FECF 0000 FBCF 0D00 CFCF 0000 CFCF 00CF CF00 CFCF 00FE CF12 00CF"
	$"CF00 00CF CF00 00CF CF00 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE"
	$"CF07 00CF CF00 00CF CF00 FBCF 0000 FECF 0700 CFCF 0000 CFCF 00FA CFFE 00FC CF00"
	$"00FE CF14 00CF CF00 00CF CF00 CFCF 0000 CFCF 00CF CF00 CFCF 00FE CF03 00CF CF00"
	$"F9CF FE00 FCCF 0000 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00"
	$"FECF 0A00 CFCF 0000 CFCF 00CF CF00 FECF 0000 F9CF 0300 CFCF 00FE CF14 00CF CF00"
	$"00CF CF00 00CF CF00 CFCF 00CF CF00 CFCF 00FE CF00 00FB CF00 00FE CF03 00CF CF00"
	$"F9CF 0000 FDCF 0000 FECF 0000 FDCF 0200 CF00 FDCF 0000 D3CF 00FF BFCE FC00 FECE"
	$"0000 FECE 0400 CECE 0000 FBCE 0B00 00CE CE00 00CE CE00 CECE 00FE CE07 00CE CE00"
	$"00CE CE00 FECE 0000 FBCE 0D00 CECE 0000 CECE 00CE CE00 CECE 00FE CE12 00CE CE00"
	$"00CE CE00 00CE CE00 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE07"
	$"00CE CE00 00CE CE00 FBCE 0000 FECE 0700 CECE 0000 CECE 00FA CEFE 00FC CE00 00FE"
	$"CE14 00CE CE00 00CE CE00 CECE 0000 CECE 00CE CE00 CECE 00FE CE03 00CE CE00 F9CE"
	$"FE00 FCCE 0000 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE"
	$"0A00 CECE 0000 CECE 00CE CE00 FECE 0000 F9CE 0300 CECE 00FE CE14 00CE CE00 00CE"
	$"CE00 00CE CE00 CECE 00CE CE00 CECE 00FE CE00 00FB CE00 00FE CE03 00CE CE00 F9CE"
	$"0000 FDCE 0000 FECE 0000 FDCE 0200 CE00 FDCE 0000 D3CE 00FF 0333 BFCE 0000 FACE"
	$"0000 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FE CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00F7 CE01 0000 FECE 0000 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FBCE 0000 F6CE 0100 00FE CE00"
	$"00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00F5 CE00 00FB CE03 00CE CE00 FECE 0000 FECE 0600 CECE 00CE CE00"
	$"FECE 0200 CE00 FACE 0000 FECE 0300 CECE 00F9 CE00 00FD CE00 00FE CE00 00FD CE02"
	$"00CE 00FD CE00 00D3 CE00 FFBF CF00 00FA CF00 00FE CF03 00CF CF00 FACF 0000 FECF"
	$"0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FBCF 0300 CFCF 00FE"
	$"CF06 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FECF 0000 FBCF 0300"
	$"CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF00 00FE CF03 00CF CF00"
	$"FECF 0000 F7CF 0100 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF06 00CF"
	$"CF00 CFCF 00FB CF00 00F6 CF01 0000 FECF 0000 FECF 0600 CFCF 00CF CF00 FECF 0300"
	$"CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 F5CF 0000 FBCF 0300"
	$"CFCF 00FE CF00 00FE CF06 00CF CF00 CFCF 00FE CF02 00CF 00FA CF00 00FE CF03 00CF"
	$"CF00 F9CF 0000 FDCF 0000 FECF 0000 FDCF 0200 CF00 FDCF 0000 D3CF 00FF BFCE 0000"
	$"FACE 0000 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE"
	$"00FE CE00 00FE CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00F7 CE01 0000 FECE 0000 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FBCE 0000 F6CE 0100 00FE"
	$"CE00 00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00F5 CE00 00FB CE03 00CE CE00 FECE 0000 FECE 0600 CECE 00CE"
	$"CE00 FECE 0200 CE00 FACE 0000 FECE 0300 CECE 00F9 CE00 00FD CE00 00FE CE00 00FD"
	$"CE02 00CE 00FD CE00 00D3 CE00 FF03 15BF CE00 00FA CE00 00FE CE03 00CE CE00 FACE"
	$"0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE FC00 FBCE 0300 CECE 00FE"
	$"CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FECE FD00"
	$"02CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE"
	$"00FE CE00 00F5 CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CEFE 00FE CE00 00F4 CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CEFE 00F8 CE00 00FE CEFD 0002"
	$"CECE 00FE CE00 00FE CE06 00CE CE00 CECE 00FE CE02 00CE 00FA CE00 00FE CE03 00CE"
	$"CE00 F9CE 0000 FDCE FC00 FCCE 0000 FCCE 0000 D3CE 00FF BFCF 0000 FACF 0000 FECF"
	$"0300 CFCF 00FA CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CFFC 00FB"
	$"CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF00 00FE CF00 00FE"
	$"CF00 00FE CFFD 0002 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF00"
	$"00FE CF03 00CF CF00 FECF 0000 F5CF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF"
	$"00FE CF03 00CF CF00 FECF FE00 FECF 0000 F4CF 0300 CFCF 00FE CF06 00CF CF00 CFCF"
	$"00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF FE00 F8CF"
	$"0000 FECF FD00 02CF CF00 FECF 0000 FECF 0600 CFCF 00CF CF00 FECF 0200 CF00 FACF"
	$"0000 FECF 0300 CFCF 00F9 CF00 00FD CFFC 00FC CF00 00FC CF00 00D3 CF00 FFBF CE00"
	$"00FA CE00 00FE CE03 00CE CE00 FACE 0000 FECE 0000 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE FC00 FBCE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE"
	$"0000 FECE 0000 FECE 0000 FECE FD00 02CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00F5 CE03 00CE CE00 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CEFE 00FE CE00 00F4 CE03 00CE CE00 FECE"
	$"0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00"
	$"00FE CEFE 00F8 CE00 00FE CEFD 0002 CECE 00FE CE00 00FE CE06 00CE CE00 CECE 00FE"
	$"CE02 00CE 00FA CE00 00FE CE03 00CE CE00 F9CE 0000 FDCE FC00 FCCE 0000 FCCE 0000"
	$"D3CE 00FF 033C BFCE 0000 FACE 0000 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE03 00CE CE00 FECE 0600 CECE 00CE CE00"
	$"FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0000 FBCE"
	$"0000 FCCE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FBCE"
	$"0300 CECE 00FA CE00 00FC CE03 00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00F9 CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE00 00FE CE06 00CE CE00 CECE 00FE CE02 00CE 00FA CE00 00FE CE03"
	$"00CE CE00 F9CE 0000 FDCE 0000 F9CE 0200 CE00 FDCE 0000 D3CE 00FF BFCF 0000 FACF"
	$"0000 FECF 0300 CFCF 00FA CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE"
	$"CF00 00F7 CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF00 00FE"
	$"CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE"
	$"CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FCCF 0300 CFCF 00FE CF03"
	$"00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FBCF 0300 CFCF 00FA CF00 00FC CF03"
	$"00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300"
	$"CFCF 00FE CF00 00FB CF00 00F9 CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FE CF06"
	$"00CF CF00 CFCF 00FE CF02 00CF 00FA CF00 00FE CF03 00CF CF00 F9CF 0000 FDCF 0000"
	$"F9CF 0200 CF00 FDCF 0000 D3CF 00FF BFCE 0000 FACE 0000 FECE 0300 CECE 00FA CE00"
	$"00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE03 00CE CE00 FECE"
	$"0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE03 00CE"
	$"CE00 FECE 0000 FBCE 0000 FCCE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE"
	$"CE03 00CE CE00 FBCE 0300 CECE 00FA CE00 00FC CE03 00CE CE00 FECE 0600 CECE 00CE"
	$"CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00F9"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE06 00CE CE00 CECE 00FE CE02 00CE"
	$"00FA CE00 00FE CE03 00CE CE00 F9CE 0000 FDCE 0000 F9CE 0200 CE00 FDCE 0000 D3CE"
	$"00FF 0360 BFCE 0000 FACE 0000 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0600 CECE 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE0A 00CE CE00 CECE 0000 CECE 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0000"
	$"FBCE 0000 FCCE 0300 CECE 00FE CE0A 00CE CE00 00CE CE00 CECE 00FE CE06 00CE CE00"
	$"CECE 00FE CE03 00CE CE00 FACE 0000 FCCE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F9"
	$"CE0A 00CE CE00 CECE 0000 CECE 00FE CE00 00FE CE06 00CE CE00 CECE 00FD CE00 00F9"
	$"CE00 00FE CE03 00CE CE00 F9CE 0000 FDCE 0000 FECE 0000 FDCE 0200 CE00 FDCE 0000"
	$"D3CE 00FF BFCF 0000 FACF 0000 FECF 0300 CFCF 00FA CF00 00FE CF00 00FE CF03 00CF"
	$"CF00 FECF 0300 CFCF 00FE CF00 00FE CF00 00FB CF03 00CF CF00 FECF 0600 CFCF 00CF"
	$"CF00 FECF 0300 CFCF 00FE CF00 00FE CF00 00FE CF0A 00CF CF00 CFCF 0000 CFCF 00FE"
	$"CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0000"
	$"FBCF 0000 FCCF 0300 CFCF 00FE CF0A 00CF CF00 00CF CF00 CFCF 00FE CF06 00CF CF00"
	$"CFCF 00FE CF03 00CF CF00 FACF 0000 FCCF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE"
	$"CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00F9"
	$"CF0A 00CF CF00 CFCF 0000 CFCF 00FE CF00 00FE CF06 00CF CF00 CFCF 00FD CF00 00F9"
	$"CF00 00FE CF03 00CF CF00 F9CF 0000 FDCF 0000 FECF 0000 FDCF 0200 CF00 FDCF 0000"
	$"D3CF 00FF BFCE 0000 FACE 0000 FECE 0300 CECE 00FA CE00 00FE CE00 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE03 00CE CE00 FECE 0600 CECE 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE0A 00CE CE00 CECE 0000 CECE 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0000"
	$"FBCE 0000 FCCE 0300 CECE 00FE CE0A 00CE CE00 00CE CE00 CECE 00FE CE06 00CE CE00"
	$"CECE 00FE CE03 00CE CE00 FACE 0000 FCCE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F9"
	$"CE0A 00CE CE00 CECE 0000 CECE 00FE CE00 00FE CE06 00CE CE00 CECE 00FD CE00 00F9"
	$"CE00 00FE CE03 00CE CE00 F9CE 0000 FDCE 0000 FECE 0000 FDCE 0200 CE00 FDCE 0000"
	$"D3CE 00FF 02EB BFCE 0000 F9CE FE00 FECE 0000 FACE 0000 FECE 0000 FECE 0000 FECE"
	$"FE00 FECE 0000 FDCE FE00 FACE 0300 CECE 00FE CE03 00CE CE00 FECE FE00 FECE 0000"
	$"FECE 0000 FECE 0000 FECE 0000 FECE 0A00 00CE 00CE CE00 00CE CE00 FECE FE00 FECE"
	$"0000 FECE 0000 FACE FE00 FECE 0000 FECE 0000 FACE FC00 FDCE FE00 FECE 0300 CE00"
	$"00FE CE00 00FE CE03 00CE CE00 FECE FE00 FECE 0100 00FA CEFC 00FD CEFE 00FE CE00"
	$"00FE CEFD 0006 CECE 0000 CECE 00FE CEFE 00FE CE00 00FE CE00 00FE CEFE 00F8 CE00"
	$"00FE CE06 0000 CE00 CECE 00FE CE00 00FE CE06 00CE CE00 CECE 00FD CE00 00F8 CEFE"
	$"00FE CE00 00F9 CE01 0000 FDCE FE00 FDCE 0000 FECE 0000 FECE 0100 00D4 CE00 FFBF"
	$"CF00 00F9 CFFE 00FE CF00 00FA CF00 00FE CF00 00FE CF00 00FE CFFE 00FE CF00 00FD"
	$"CFFE 00FA CF03 00CF CF00 FECF 0300 CFCF 00FE CFFE 00FE CF00 00FE CF00 00FE CF00"
	$"00FE CF00 00FE CF0A 0000 CF00 CFCF 0000 CFCF 00FE CFFE 00FE CF00 00FE CF00 00FA"
	$"CFFE 00FE CF00 00FE CF00 00FA CFFC 00FD CFFE 00FE CF03 00CF 0000 FECF 0000 FECF"
	$"0300 CFCF 00FE CFFE 00FE CF01 0000 FACF FC00 FDCF FE00 FECF 0000 FECF FD00 06CF"
	$"CF00 00CF CF00 FECF FE00 FECF 0000 FECF 0000 FECF FE00 F8CF 0000 FECF 0600 00CF"
	$"00CF CF00 FECF 0000 FECF 0600 CFCF 00CF CF00 FDCF 0000 F8CF FE00 FECF 0000 F9CF"
	$"0100 00FD CFFE 00FD CF00 00FE CF00 00FE CF01 0000 D4CF 00FF BFCE 0000 F9CE FE00"
	$"FECE 0000 FACE 0000 FECE 0000 FECE 0000 FECE FE00 FECE 0000 FDCE FE00 FACE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE FE00 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FECE"
	$"0A00 00CE 00CE CE00 00CE CE00 FECE FE00 FECE 0000 FECE 0000 FACE FE00 FECE 0000"
	$"FECE 0000 FACE FC00 FDCE FE00 FECE 0300 CE00 00FE CE00 00FE CE03 00CE CE00 FECE"
	$"FE00 FECE 0100 00FA CEFC 00FD CEFE 00FE CE00 00FE CEFD 0006 CECE 0000 CECE 00FE"
	$"CEFE 00FE CE00 00FE CE00 00FE CEFE 00F8 CE00 00FE CE06 0000 CE00 CECE 00FE CE00"
	$"00FE CE06 00CE CE00 CECE 00FD CE00 00F8 CEFE 00FE CE00 00F9 CE01 0000 FDCE FE00"
	$"FDCE 0000 FECE 0000 FECE 0100 00D4 CE00 FF00 2A81 CEA2 CE00 0089 CE00 00A5 CE00"
	$"FF81 CFA2 CF00 0089 CF00 00A5 CF00 FF81 CEA2 CE00 0089 CE00 00A5 CE00 FF00 2A81"
	$"CEA2 CE00 0089 CE00 00A5 CE00 FF81 CFA2 CF00 0089 CF00 00A5 CF00 FF81 CEA2 CE00"
	$"0089 CE00 00A5 CE00 FF00 2D81 CEA2 CE00 008B CE01 0000 A4CE 00FF 81CF A2CF 0000"
	$"8BCF 0100 00A4 CF00 FF81 CEA2 CE00 008B CE01 0000 A4CE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE"
	$"CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 0081 E4CE 0300 CECE"
	$"00FB CE00 00DB CE00 00E3 CE00 00CD CE00 00DB CE00 00DE CE00 00D0 CE00 00BE CE00"
	$"00B0 CE00 FFE4 CF03 00CF CF00 FBCF 0000 DBCF 0000 E3CF 0000 CDCF 0000 DBCF 0000"
	$"DECF 0000 D0CF 0000 BECF 0000 B0CF 00FF E4CE 0300 CECE 00FB CE00 00DB CE00 00E3"
	$"CE00 00CD CE00 00DB CE00 00DE CE00 00D0 CE00 00BE CE00 00B0 CE00 FF00 9CE4 CE00"
	$"00FC CE00 00D7 CE00 00E3 CE00 00B8 CE00 00F0 CE00 00DE CE00 00F8 CE00 00DA CE00"
	$"00BE CE00 00D9 CE00 00ED CE00 00EE CE00 FFE4 CF00 00FC CF00 00D7 CF00 00E3 CF00"
	$"00B8 CF00 00F0 CF00 00DE CF00 00F8 CF00 00DA CF00 00BE CF00 00D9 CF00 00ED CF00"
	$"00EE CF00 FFE4 CE00 00FC CE00 00D7 CE00 00E3 CE00 00B8 CE00 00F0 CE00 00DE CE00"
	$"00F8 CE00 00DA CE00 00BE CE00 00D9 CE00 00ED CE00 00EE CE00 FF00 9CE4 CE00 00FC"
	$"CE00 00D7 CE00 00E3 CE00 00B8 CE00 00F0 CE00 00DE CE00 00F8 CE00 00DA CE00 00BE"
	$"CE00 00D9 CE00 00ED CE00 00EE CE00 FFE4 CF00 00FC CF00 00D7 CF00 00E3 CF00 00B8"
	$"CF00 00F0 CF00 00DE CF00 00F8 CF00 00DA CF00 00BE CF00 00D9 CF00 00ED CF00 00EE"
	$"CF00 FFE4 CE00 00FC CE00 00D7 CE00 00E3 CE00 00B8 CE00 00F0 CE00 00DE CE00 00F8"
	$"CE00 00DA CE00 00BE CE00 00D9 CE00 00ED CE00 00EE CE00 FF03 63EE CEFE 00FD CE07"
	$"0000 CE00 CECE 00CE FE00 08CE CE00 CECE 00CE 0000 FDCE 0300 00CE 00FA CEFE 00FE"
	$"CE03 00CE 0000 FDCE 0300 00CE 00FC CE00 00FE CE00 00FE CE02 00CE CEFE 00FE CE08"
	$"00CE 00CE CE00 00CE 00FB CE03 00CE 0000 FECE 0400 CE00 CECE FE00 FDCE FE00 FDCE"
	$"FE00 FDCE FE00 FDCE FE00 FECE 0600 CECE 00CE 0000 FDCE 0300 00CE 00FC CEFE 00FE"
	$"CEFE 00FD CEFE 00FE CE00 00FE CEFE 00F6 CE00 00FE CE06 00CE CE00 CE00 00FD CE03"
	$"0000 CE00 FECE FE00 01CE CEFE 00FE CEFE 00FD CEFE 00F9 CEFE 00FE CE03 00CE 0000"
	$"FDCE 0300 00CE 00FA CEFE 00FD CEFE 00FE CE00 00FE CE07 00CE CE00 CE00 CECE FE00"
	$"FDCE FE00 F9CE FE00 FDCE FE00 FDCE 0300 00CE 00FE CEFE 00F5 CEFE 00FD CEFE 00FD"
	$"CE03 00CE 0000 FECE FE00 FDCE FE00 FCCE FE00 FECE FE00 EFCE 00FF EECF FE00 FDCF"
	$"0700 00CF 00CF CF00 CFFE 0008 CFCF 00CF CF00 CF00 00FD CF03 0000 CF00 FACF FE00"
	$"FECF 0300 CF00 00FD CF03 0000 CF00 FCCF 0000 FECF 0000 FECF 0200 CFCF FE00 FECF"
	$"0800 CF00 CFCF 0000 CF00 FBCF 0300 CF00 00FE CF04 00CF 00CF CFFE 00FD CFFE 00FD"
	$"CFFE 00FD CFFE 00FD CFFE 00FE CF06 00CF CF00 CF00 00FD CF03 0000 CF00 FCCF FE00"
	$"FECF FE00 FDCF FE00 FECF 0000 FECF FE00 F6CF 0000 FECF 0600 CFCF 00CF 0000 FDCF"
	$"0300 00CF 00FE CFFE 0001 CFCF FE00 FECF FE00 FDCF FE00 F9CF FE00 FECF 0300 CF00"
	$"00FD CF03 0000 CF00 FACF FE00 FDCF FE00 FECF 0000 FECF 0700 CFCF 00CF 00CF CFFE"
	$"00FD CFFE 00F9 CFFE 00FD CFFE 00FD CF03 0000 CF00 FECF FE00 F5CF FE00 FDCF FE00"
	$"FDCF 0300 CF00 00FE CFFE 00FD CFFE 00FC CFFE 00FE CFFE 00EF CF00 FFEE CEFE 00FD"
	$"CE07 0000 CE00 CECE 00CE FE00 08CE CE00 CECE 00CE 0000 FDCE 0300 00CE 00FA CEFE"
	$"00FE CE03 00CE 0000 FDCE 0300 00CE 00FC CE00 00FE CE00 00FE CE02 00CE CEFE 00FE"
	$"CE08 00CE 00CE CE00 00CE 00FB CE03 00CE 0000 FECE 0400 CE00 CECE FE00 FDCE FE00"
	$"FDCE FE00 FDCE FE00 FDCE FE00 FECE 0600 CECE 00CE 0000 FDCE 0300 00CE 00FC CEFE"
	$"00FE CEFE 00FD CEFE 00FE CE00 00FE CEFE 00F6 CE00 00FE CE06 00CE CE00 CE00 00FD"
	$"CE03 0000 CE00 FECE FE00 01CE CEFE 00FE CEFE 00FD CEFE 00F9 CEFE 00FE CE03 00CE"
	$"0000 FDCE 0300 00CE 00FA CEFE 00FD CEFE 00FE CE00 00FE CE07 00CE CE00 CE00 CECE"
	$"FE00 FDCE FE00 F9CE FE00 FDCE FE00 FDCE 0300 00CE 00FE CEFE 00F5 CEFE 00FD CEFE"
	$"00FD CE03 00CE 0000 FECE FE00 FDCE FE00 FCCE FE00 FECE FE00 EFCE 00FF 045C EFCE"
	$"0000 FECE 0D00 CECE 00CE CE00 00CE CE00 CECE 00FE CE0E 00CE CE00 00CE CE00 CECE"
	$"00CE CE00 00FB CE00 00FE CE0E 00CE CE00 00CE CE00 CECE 00CE CE00 00FC CE00 00FE"
	$"CE00 00FE CE02 00CE 00FE CE0B 00CE CE00 00CE CE00 CECE 0000 FBCE 0B00 00CE CE00"
	$"CECE 0000 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300"
	$"CECE 00FE CE11 00CE CE00 CECE 0000 CECE 00CE CE00 CECE 0000 FBCE 0000 FECE 0000"
	$"FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE00 00F7 CE00 00FE CE11 00CE CE00"
	$"00CE CE00 CECE 00CE CE00 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00"
	$"FECE 0000 FBCE 0000 FECE 0E00 CECE 0000 CECE 00CE CE00 CECE 0000 FBCE 0000 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0700 CECE 0000 CECE 00FE CE03 00CE CE00 FECE"
	$"0000 FBCE 0000 FECE 0300 CECE 00FE CE0A 00CE CE00 CECE 0000 CECE 00FE CE00 00F7"
	$"CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0400 00CE CE00 FECE 0000 FDCE 0000 FECE"
	$"0000 FECE 0000 FECE 0000 FECE 0000 EECE 00FF EFCF 0000 FECF 0D00 CFCF 00CF CF00"
	$"00CF CF00 CFCF 00FE CF0E 00CF CF00 00CF CF00 CFCF 00CF CF00 00FB CF00 00FE CF0E"
	$"00CF CF00 00CF CF00 CFCF 00CF CF00 00FC CF00 00FE CF00 00FE CF02 00CF 00FE CF0B"
	$"00CF CF00 00CF CF00 CFCF 0000 FBCF 0B00 00CF CF00 CFCF 0000 CFCF 00FE CF03 00CF"
	$"CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF11 00CF CF00 CFCF"
	$"0000 CFCF 00CF CF00 CFCF 0000 FBCF 0000 FECF 0000 FECF 0300 CFCF 00FE CF06 00CF"
	$"CF00 CFCF 00FE CF00 00F7 CF00 00FE CF11 00CF CF00 00CF CF00 CFCF 00CF CF00 00CF"
	$"CF00 FECF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FECF 0E00"
	$"CFCF 0000 CFCF 00CF CF00 CFCF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00"
	$"FECF 0700 CFCF 0000 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FECF 0300 CFCF"
	$"00FE CF0A 00CF CF00 CFCF 0000 CFCF 00FE CF00 00F7 CF00 00FE CF03 00CF CF00 FECF"
	$"0000 FECF 0400 00CF CF00 FECF 0000 FDCF 0000 FECF 0000 FECF 0000 FECF 0000 FECF"
	$"0000 EECF 00FF EFCE 0000 FECE 0D00 CECE 00CE CE00 00CE CE00 CECE 00FE CE0E 00CE"
	$"CE00 00CE CE00 CECE 00CE CE00 00FB CE00 00FE CE0E 00CE CE00 00CE CE00 CECE 00CE"
	$"CE00 00FC CE00 00FE CE00 00FE CE02 00CE 00FE CE0B 00CE CE00 00CE CE00 CECE 0000"
	$"FBCE 0B00 00CE CE00 CECE 0000 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FE CE11 00CE CE00 CECE 0000 CECE 00CE CE00 CECE 0000"
	$"FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE00 00F7 CE00"
	$"00FE CE11 00CE CE00 00CE CE00 CECE 00CE CE00 00CE CE00 FECE 0300 CECE 00FE CE00"
	$"00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0E00 CECE 0000 CECE 00CE CE00 CECE"
	$"0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0700 CECE 0000 CECE 00FE"
	$"CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE0A 00CE CE00 CECE 0000"
	$"CECE 00FE CE00 00F7 CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0400 00CE CE00 FECE"
	$"0000 FDCE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 EECE 00FF 03C9 EFCE 0000"
	$"FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00F7 CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE09 00CE 00CE 00CE 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0000"
	$"FECE 0300 CECE 00FB CE00 00FE CE03 00CE CE00 FBCE 0000 FBCE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE"
	$"00F3 CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE03 00CE CE00 FECE"
	$"0000 FECE 0300 CECE 00F3 CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FB"
	$"CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FB CE00"
	$"00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE00 00FB CE00 00FE"
	$"CE00 00FE CE00 00FE CE00 00FE CE00 00F9 CE00 00FE CE00 00FA CE00 00EE CE00 FFEF"
	$"CF00 00FE CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF"
	$"CF00 FECF 0000 F7CF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0900 CF00 CF00"
	$"CF00 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FBCF 0000 FECF 0300 CFCF 00FE"
	$"CF00 00FE CF03 00CF CF00 FBCF 0000 FECF 0300 CFCF 00FB CF00 00FB CF03 00CF CF00"
	$"FECF 0300 CFCF 00FE CF00 00FB CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0600 CFCF"
	$"00CF CF00 F3CF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0300 CFCF"
	$"00FE CF00 00FE CF03 00CF CF00 F3CF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF"
	$"0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FECF 0000"
	$"FBCF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 F7CF 0000 FBCF"
	$"0000 FECF 0000 FECF 0000 FECF 0000 FECF 0000 F9CF 0000 FECF 0000 FACF 0000 EECF"
	$"00FF EFCE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE00 00F7 CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE09 00CE"
	$"00CE 00CE 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE00 00FE CE03 00CE"
	$"CE00 FECE 0000 FECE 0300 CECE 00FB CE00 00FE CE03 00CE CE00 FBCE 0000 FBCE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE06"
	$"00CE CE00 CECE 00F3 CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE03"
	$"00CE CE00 FECE 0000 FECE 0300 CECE 00F3 CE03 00CE CE00 FECE 0300 CECE 00FE CE00"
	$"00FB CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE"
	$"CE00 00FB CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE00"
	$"00FB CE00 00FE CE00 00FE CE00 00FE CE00 00FE CE00 00F9 CE00 00FE CE00 00FA CE00"
	$"00EE CE00 FF03 A5EF CEFC 0002 CECE 00FE CE06 00CE CE00 CECE 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FE CE00 00FA CEFD 0002 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE"
	$"0900 CE00 CE00 CE00 CECE 00FE CE03 00CE CE00 FECE 0000 FECE 0000 FBCE 0000 FECE"
	$"0300 CECE 00FE CE00 00FE CE03 00CE CE00 FBCE FC00 FECE FE00 FDCE FE00 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE FE00 F6CE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FECE"
	$"FD00 02CE CE00 FECE FC00 FECE FE00 F9CE FD00 02CE CE00 FECE 0300 CECE 00FE CE00"
	$"00FA CEFE 00FE CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CEFC 00FB"
	$"CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0200 CECE FC00 F7CE 0000 FBCE 0000 FECE"
	$"0000 FECE 0000 FECE 0000 FECE 0000 FCCE FD00 FECE 0000 FACE 0000 EECE 00FF EFCF"
	$"FC00 02CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF"
	$"0000 FACF FD00 02CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF09 00CF 00CF 00CF 00CF"
	$"CF00 FECF 0300 CFCF 00FE CF00 00FE CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0000"
	$"FECF 0300 CFCF 00FB CFFC 00FE CFFE 00FD CFFE 00FE CF03 00CF CF00 FECF 0300 CFCF"
	$"00FE CF00 00FB CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CFFE 00F6"
	$"CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FE CFFD 0002 CFCF 00FE CFFC"
	$"00FE CFFE 00F9 CFFD 0002 CFCF 00FE CF03 00CF CF00 FECF 0000 FACF FE00 FECF 0000"
	$"FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF FC00 FBCF 0000 FBCF 0000 FECF"
	$"0300 CFCF 00FE CF02 00CF CFFC 00F7 CF00 00FB CF00 00FE CF00 00FE CF00 00FE CF00"
	$"00FE CF00 00FC CFFD 00FE CF00 00FA CF00 00EE CF00 FFEF CEFC 0002 CECE 00FE CE06"
	$"00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FA CEFD 0002 CECE"
	$"00FE CE03 00CE CE00 FECE 0000 FBCE 0900 CE00 CE00 CE00 CECE 00FE CE03 00CE CE00"
	$"FECE 0000 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FBCE"
	$"FC00 FECE FE00 FDCE FE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000"
	$"FECE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE FE00 F6CE 0000 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0000 FECE FD00 02CE CE00 FECE FC00 FECE FE00 F9CE FD00"
	$"02CE CE00 FECE 0300 CECE 00FE CE00 00FA CEFE 00FE CE00 00FE CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE00 00FB CEFC 00FB CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0200"
	$"CECE FC00 F7CE 0000 FBCE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FCCE FD00"
	$"FECE 0000 FACE 0000 EECE 00FF 03C0 EFCE 0000 FBCE 0000 FECE 0600 CECE 00CE CE00"
	$"FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0900 CE00 CE00 CE00 CECE 00FE CE03 00CE CE00 FECE 0000"
	$"FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FBCE 0000 F7CE"
	$"0000 FBCE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FB CE00 00F7 CE00 00FE CE03 00CE CE00 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE00 00FB CE00 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE03 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0000 F7CE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 F3CE 0000 FBCE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FDCE 0000 FECE"
	$"0000 FECE 0000 FACE 0000 EECE 00FF EFCF 0000 FBCF 0000 FECF 0600 CFCF 00CF CF00"
	$"FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF03"
	$"00CF CF00 FECF 0000 FBCF 0900 CF00 CF00 CF00 CFCF 00FE CF03 00CF CF00 FECF 0000"
	$"FECF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00 FBCF 0000 F7CF"
	$"0000 FBCF 0600 CFCF 00CF CF00 FECF 0300 CFCF 00FE CF00 00FB CF00 00FE CF00 00FE"
	$"CF03 00CF CF00 FECF 0300 CFCF 00FB CF00 00F7 CF00 00FE CF03 00CF CF00 FECF 0300"
	$"CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00F7 CF00 00FB CF00 00FE CF03"
	$"00CF CF00 FECF 0300 CFCF 00FE CF00 00F7 CF03 00CF CF00 FECF 0300 CFCF 00FE CF03"
	$"00CF CF00 FECF 0000 FBCF 0000 F7CF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF"
	$"CF00 F3CF 0000 FBCF 0000 FECF 0000 FECF 0000 FECF 0000 FECF 0000 FDCF 0000 FECF"
	$"0000 FECF 0000 FACF 0000 EECF 00FF EFCE 0000 FBCE 0000 FECE 0600 CECE 00CE CE00"
	$"FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0900 CE00 CE00 CE00 CECE 00FE CE03 00CE CE00 FECE 0000"
	$"FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FBCE 0000 F7CE"
	$"0000 FBCE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE00 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FB CE00 00F7 CE00 00FE CE03 00CE CE00 FECE 0300"
	$"CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE00 00FB CE00 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE03 00CE CE00 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0000 FBCE 0000 F7CE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 F3CE 0000 FBCE 0000 FECE 0000 FECE 0000 FECE 0000 FECE 0000 FDCE 0000 FECE"
	$"0000 FECE 0000 FACE 0000 EECE 00FF 0456 EFCE 0000 FECE 0300 CECE 00FE CE06 00CE"
	$"CE00 CECE 00FE CE03 00CE CE00 FECE 0700 CECE 00CE CE00 00FB CE07 00CE CE00 00CE"
	$"CE00 FECE 0300 CECE 00FE CE00 00FA CE00 00FE CE00 00FE CE00 00FE CE03 00CE CE00"
	$"FECE 0000 FECE 0000 FBCE 0700 00CE CE00 CECE 00FE CE00 00FE CE03 00CE CE00 FECE"
	$"0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE07"
	$"00CE CE00 CECE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE"
	$"00FE CE00 00F7 CE00 00FE CE0A 00CE CE00 00CE CE00 CECE 00FE CE0A 00CE CE00 CECE"
	$"0000 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FBCE 0700 CECE 0000 CECE 00FE"
	$"CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300"
	$"CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE00 00FE CE03 00CE CE00 FECE 0000 FECE"
	$"0000 FECE 0000 FECE 0000 FDCE 0400 CECE 0000 FECE 0000 FECE 0000 FECE 0000 EECE"
	$"00FF EFCF 0000 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF"
	$"0700 CFCF 00CF CF00 00FB CF07 00CF CF00 00CF CF00 FECF 0300 CFCF 00FE CF00 00FA"
	$"CF00 00FE CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FBCF 0700 00CF"
	$"CF00 CFCF 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF"
	$"0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF07 00CF CF00 CFCF 0000 FBCF 0000 FECF"
	$"0000 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF00 00F7 CF00 00FE CF0A 00CF"
	$"CF00 00CF CF00 CFCF 00FE CF0A 00CF CF00 CFCF 0000 CFCF 00FE CF00 00FE CF03 00CF"
	$"CF00 FECF 0000 FBCF 0700 CFCF 0000 CFCF 00FE CF03 00CF CF00 FECF 0000 FBCF 0000"
	$"FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00 00FE CF03 00CF CF00"
	$"FECF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF00"
	$"00F7 CF00 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FECF 0000 FECF 0000 FDCF 0400"
	$"CFCF 0000 FECF 0000 FECF 0000 FECF 0000 EECF 00FF EFCE 0000 FECE 0300 CECE 00FE"
	$"CE06 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0700 CECE 00CE CE00 00FB CE07 00CE"
	$"CE00 00CE CE00 FECE 0300 CECE 00FE CE00 00FA CE00 00FE CE00 00FE CE00 00FE CE03"
	$"00CE CE00 FECE 0000 FECE 0000 FBCE 0700 00CE CE00 CECE 00FE CE00 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE06 00CE CE00 CECE"
	$"00FE CE07 00CE CE00 CECE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE06 00CE"
	$"CE00 CECE 00FE CE00 00F7 CE00 00FE CE0A 00CE CE00 00CE CE00 CECE 00FE CE0A 00CE"
	$"CE00 CECE 0000 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FBCE 0700 CECE 0000"
	$"CECE 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00"
	$"FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FBCE 0000 FECE 0300 CECE"
	$"00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00F7 CE00 00FE CE03 00CE CE00 FECE"
	$"0000 FECE 0000 FECE 0000 FECE 0000 FDCE 0400 CECE 0000 FECE 0000 FECE 0000 FECE"
	$"0000 EECE 00FF 034E EECE FE00 FDCE FD00 0CCE CE00 CECE 0000 CECE 00CE CE00 FECE"
	$"0000 FECE 0300 00CE 00FA CE06 0000 CE00 CECE 00FE CE00 00FE CEFD 00FA CE00 00FE"
	$"CE00 00FD CEFE 00FE CE00 00FD CEFD 00FB CE03 00CE 0000 FECE 0000 FDCE FE00 FDCE"
	$"FE00 FDCE FE00 FDCE FE00 FDCE FE00 FECE 0300 CECE 00FE CE00 00FE CE03 0000 CE00"
	$"FBCE 0100 00FE CEFE 00FD CEFE 00FE CE00 00FE CEFE 00FD CE00 00FA CEFD 0005 CECE"
	$"00CE 0000 FDCE FD00 FECE 0700 00CE 00CE CE00 00FE CEFE 00FD CEFE 00F9 CE06 0000"
	$"CE00 CECE 00FE CE00 00FE CEFD 00FA CEFE 00FD CEFE 00FD CEFD 0002 CECE 00FD CEFE"
	$"00FD CEFE 00F9 CEFE 00FD CEFE 00FD CEFD 00FE CEFE 00FD CE00 00FA CEFE 00FD CEFE"
	$"00FD CE00 00FE CE00 00FE CE01 0000 FDCE 0300 00CE 00FD CEFE 00FD CE01 0000 EFCE"
	$"00FF EECF FE00 FDCF FD00 0CCF CF00 CFCF 0000 CFCF 00CF CF00 FECF 0000 FECF 0300"
	$"00CF 00FA CF06 0000 CF00 CFCF 00FE CF00 00FE CFFD 00FA CF00 00FE CF00 00FD CFFE"
	$"00FE CF00 00FD CFFD 00FB CF03 00CF 0000 FECF 0000 FDCF FE00 FDCF FE00 FDCF FE00"
	$"FDCF FE00 FDCF FE00 FECF 0300 CFCF 00FE CF00 00FE CF03 0000 CF00 FBCF 0100 00FE"
	$"CFFE 00FD CFFE 00FE CF00 00FE CFFE 00FD CF00 00FA CFFD 0005 CFCF 00CF 0000 FDCF"
	$"FD00 FECF 0700 00CF 00CF CF00 00FE CFFE 00FD CFFE 00F9 CF06 0000 CF00 CFCF 00FE"
	$"CF00 00FE CFFD 00FA CFFE 00FD CFFE 00FD CFFD 0002 CFCF 00FD CFFE 00FD CFFE 00F9"
	$"CFFE 00FD CFFE 00FD CFFD 00FE CFFE 00FD CF00 00FA CFFE 00FD CFFE 00FD CF00 00FE"
	$"CF00 00FE CF01 0000 FDCF 0300 00CF 00FD CFFE 00FD CF01 0000 EFCF 00FF EECE FE00"
	$"FDCE FD00 0CCE CE00 CECE 0000 CECE 00CE CE00 FECE 0000 FECE 0300 00CE 00FA CE06"
	$"0000 CE00 CECE 00FE CE00 00FE CEFD 00FA CE00 00FE CE00 00FD CEFE 00FE CE00 00FD"
	$"CEFD 00FB CE03 00CE 0000 FECE 0000 FDCE FE00 FDCE FE00 FDCE FE00 FDCE FE00 FDCE"
	$"FE00 FECE 0300 CECE 00FE CE00 00FE CE03 0000 CE00 FBCE 0100 00FE CEFE 00FD CEFE"
	$"00FE CE00 00FE CEFE 00FD CE00 00FA CEFD 0005 CECE 00CE 0000 FDCE FD00 FECE 0700"
	$"00CE 00CE CE00 00FE CEFE 00FD CEFE 00F9 CE06 0000 CE00 CECE 00FE CE00 00FE CEFD"
	$"00FA CEFE 00FD CEFE 00FD CEFD 0002 CECE 00FD CEFE 00FD CEFE 00F9 CEFE 00FD CEFE"
	$"00FD CEFD 00FE CEFE 00FD CE00 00FA CEFE 00FD CEFE 00FD CE00 00FE CE00 00FE CE01"
	$"0000 FDCE 0300 00CE 00FD CEFE 00FD CE01 0000 EFCE 00FF 005A CCCE 0000 C3CE 0000"
	$"C6CE 0000 DECE 0000 F4CE 0000 81CE F6CE 0000 BBCE 00FF CCCF 0000 C3CF 0000 C6CF"
	$"0000 DECF 0000 F4CF 0000 81CF F6CF 0000 BBCF 00FF CCCE 0000 C3CE 0000 C6CE 0000"
	$"DECE 0000 F4CE 0000 81CE F6CE 0000 BBCE 00FF 0072 D0CE 0000 FECE 0000 C3CE 0000"
	$"CACE 0000 FECE 0000 DECE 0000 F4CE 0000 81CE F6CE 0000 BBCE 00FF D0CF 0000 FECF"
	$"0000 C3CF 0000 CACF 0000 FECF 0000 DECF 0000 F4CF 0000 81CF F6CF 0000 BBCF 00FF"
	$"D0CE 0000 FECE 0000 C3CE 0000 CACE 0000 FECE 0000 DECE 0000 F4CE 0000 81CE F6CE"
	$"0000 BBCE 00FF 0042 CFCE FE00 C2CE 0000 C9CE FE00 CFCE 0000 81CE AFCE 00FF CFCF"
	$"FE00 C2CF 0000 C9CF FE00 CFCF 0000 81CF AFCF 00FF CFCE FE00 C2CE 0000 C9CE FE00"
	$"CFCE 0000 81CE AFCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF 81CF 81CF CCCF 00FF"
	$"81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF 81CF 81CF CCCF 00FF"
	$"81CE 81CE 81CE CCCE 00FF 001E 81CE 81CE 81CE CCCE 00FF 81CF 81CF 81CF CCCF 00FF"
	$"81CE 81CE 81CE CCCE 00FF 00EA EFCE 0000 F7CE 0100 00F6 CE00 00F4 CE00 00FC CEFC"
	$"00E7 CE00 00FB CE00 00C4 CE00 00DB CE03 00CE CE00 E6CE 0000 E6CE 0000 FECE 0000"
	$"CCCE 0000 FBCE 0000 CECE 0200 CE00 F0CE 0000 FDCE 0000 DFCE 00FF EFCF 0000 F7CF"
	$"0100 00F6 CF00 00F4 CF00 00FC CFFC 00E7 CF00 00FB CF00 00C4 CF00 00DB CF03 00CF"
	$"CF00 E6CF 0000 E6CF 0000 FECF 0000 CCCF 0000 FBCF 0000 CECF 0200 CF00 F0CF 0000"
	$"FDCF 0000 DFCF 00FF EFCE 00FF F7CE 01FF FFF6 CE00 FFF4 CE00 FFFC CEFC FFE7 CE00"
	$"FFFB CE00 FFC4 CE00 00DB CE03 00CE CE00 E6CE 00FF E6CE 00FF FECE 00FF CCCE 00FF"
	$"FBCE 00FF CECE 02FF CEFF F0CE 00FF FDCE 00FF DFCE 00FF 00ED E5CE 0000 F4CE 0000"
	$"F4CE 0000 FECE 0100 00FC CE00 00E8 CE00 00F1 CE00 00A4 CE00 00E6 CE00 00FB CE00"
	$"00FE CE00 00F1 CE00 00FE CE00 00CC CE00 00F1 CE00 00D8 CE02 00CE 00F0 CE00 00FD"
	$"CE00 00DF CE00 FFE5 CF00 00F4 CF00 00F4 CF00 00FE CF01 0000 FCCF 0000 E8CF 0000"
	$"F1CF 0000 A4CF 0000 E6CF 0000 FBCF 0000 FECF 0000 F1CF 0000 FECF 0000 CCCF 0000"
	$"F1CF 0000 D8CF 0200 CF00 F0CF 0000 FDCF 0000 DFCF 00FF E5CE 00FF F4CE 00FF F4CE"
	$"00FF FECE 01FF FFFC CE00 FFE8 CE00 FFF1 CE00 FFA4 CE00 00E6 CE00 FFFB CE00 FFFE"
	$"CE00 FFF1 CE00 FFFE CE00 FFCC CE00 FFF1 CE00 FFD8 CE02 FFCE FFF0 CE00 FFFD CE00"
	$"FFDF CE00 FF00 EAE5 CE00 00F4 CE00 00F4 CE03 00CE CE00 F9CE 0000 E9CE 0000 F1CE"
	$"0000 A4CE 0000 E6CE 0000 FBCE 0000 FECE 0000 F2CE 0000 FECE 0000 CBCE 0000 F1CE"
	$"0000 D9CE 0300 CECE 00F0 CE00 00FE CE00 00DE CE00 FFE5 CF00 00F4 CF00 00F4 CF03"
	$"00CF CF00 F9CF 0000 E9CF 0000 F1CF 0000 A4CF 0000 E6CF 0000 FBCF 0000 FECF 0000"
	$"F2CF 0000 FECF 0000 CBCF 0000 F1CF 0000 D9CF 0300 CFCF 00F0 CF00 00FE CF00 00DE"
	$"CF00 FFE5 CE00 FFF4 CE00 FFF4 CE03 FFCE CEFF F9CE 00FF E9CE 00FF F1CE 00FF A4CE"
	$"0000 E6CE 00FF FBCE 00FF FECE 00FF F2CE 00FF FECE 00FF CBCE 00FF F1CE 00FF D9CE"
	$"03FF CECE FFF0 CE00 FFFE CE00 FFDE CE00 FF03 A8EF CE08 00CE CE00 CE00 00CE CEFE"
	$"0001 CECE FE00 FACE 0000 FECE FE00 FDCE 0800 00CE 00CE CE00 CECE FE00 04CE 00CE"
	$"CE00 FECE FE00 FDCE FE00 FECE 0300 CE00 00FE CE03 00CE 0000 FECE 0000 FECE FE00"
	$"01CE CEFE 00FE CEFE 00F9 CEFE 00FD CEFE 00FD CE07 00CE 0000 CECE 0000 F9CE 0000"
	$"FECE 0200 CE00 FECE FE00 F9CE FE00 FECE 0700 CE00 00CE CE00 00FD CEFE 00FE CE03"
	$"00CE CE00 F6CE FE00 FECE 0200 CE00 FACE 0500 CE00 00CE CEFE 0000 CEFE 0005 CECE"
	$"00CE 0000 FECE 0000 FDCE 0000 FECE 0200 CE00 FECE 0000 FECE 0100 00FE CE00 00FE"
	$"CE01 0000 FECE 0000 FECE 0000 FBCE FE00 FDCE FE00 FECE 0300 CE00 00FE CE03 00CE"
	$"0000 FECE 0000 FECE FE00 01CE CEFE 00FE CEFE 00F9 CEFE 00FD CEFE 00FE CE07 00CE"
	$"0000 CECE 0000 FDCE 0300 CECE 00FA CEFE 00FD CE03 0000 CE00 FECE 0000 F9CE FE00"
	$"FECE 0300 CE00 00F0 CE00 FFEF CF08 00CF CF00 CF00 00CF CFFE 0001 CFCF FE00 FACF"
	$"0000 FECF FE00 FDCF 0800 00CF 00CF CF00 CFCF FE00 04CF 00CF CF00 FECF FE00 FDCF"
	$"FE00 FECF 0300 CF00 00FE CF03 00CF 0000 FECF 0000 FECF FE00 01CF CFFE 00FE CFFE"
	$"00F9 CFFE 00FD CFFE 00FD CF07 00CF 0000 CFCF 0000 F9CF 0000 FECF 0200 CF00 FECF"
	$"FE00 F9CF FE00 FECF 0700 CF00 00CF CF00 00FD CFFE 00FE CF03 00CF CF00 F6CF FE00"
	$"FECF 0200 CF00 FACF 0500 CF00 00CF CFFE 0000 CFFE 0005 CFCF 00CF 0000 FECF 0000"
	$"FDCF 0000 FECF 0200 CF00 FECF 0000 FECF 0100 00FE CF00 00FE CF01 0000 FECF 0000"
	$"FECF 0000 FBCF FE00 FDCF FE00 FECF 0300 CF00 00FE CF03 00CF 0000 FECF 0000 FECF"
	$"FE00 01CF CFFE 00FE CFFE 00F9 CFFE 00FD CFFE 00FE CF07 00CF 0000 CFCF 0000 FDCF"
	$"0300 CFCF 00FA CFFE 00FD CF03 0000 CF00 FECF 0000 F9CF FE00 FECF 0300 CF00 00F0"
	$"CF00 FFEF CE08 FFCE CEFF CEFF FFCE CEFE FF01 CECE FEFF FACE 00FF FECE FEFF FDCE"
	$"08FF FFCE FFCE CEFF CECE FEFF 04CE FFCE CEFF FECE FEFF FDCE FEFF FECE 03FF CEFF"
	$"FFFE CE03 FFCE FFFF FECE 00FF FECE FEFF 01CE CEFE FFFE CEFE FFF9 CEFE FFFD CEFE"
	$"FFFD CE07 FFCE FFFF CECE FFFF F9CE 0000 FECE 0200 CE00 FECE FE00 F9CE FE00 FECE"
	$"0700 CE00 00CE CE00 00FD CEFE 00FE CE03 00CE CE00 F6CE FE00 FECE 0200 CE00 FACE"
	$"05FF CEFF FFCE CEFE FF00 CEFE FF05 CECE FFCE FFFF FECE 00FF FDCE 00FF FECE 02FF"
	$"CEFF FECE 00FF FECE 01FF FFFE CE00 FFFE CE01 FFFF FECE 00FF FECE 00FF FBCE FEFF"
	$"FDCE FEFF FECE 03FF CEFF FFFE CE03 FFCE FFFF FECE 00FF FECE FEFF 01CE CEFE FFFE"
	$"CEFE FFF9 CEFE FFFD CEFE FFFE CE07 FFCE FFFF CECE FFFF FDCE 03FF CECE FFFA CEFE"
	$"FFFD CE03 FFFF CEFF FECE 00FF F9CE FE00 FECE 0300 CE00 00F0 CE00 FF04 53EF CE0D"
	$"00CE CE00 00CE CE00 CECE 00CE CE00 FECE 0000 FBCE 0300 CECE 00FE CE0C 00CE CE00"
	$"CECE 0000 CE00 CECE 00FE CE07 0000 CECE 00CE CE00 FECE 0300 CECE 00FE CE14 00CE"
	$"CE00 00CE CE00 CECE 0000 CECE 00CE CE00 CECE 00FE CE03 00CE CE00 FECE 0000 FECE"
	$"0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE CE08 0000 CECE 0000 CECE 00FA CE00"
	$"00FE CE05 00CE 00CE CE00 FECE 0000 FBCE 0000 FECE 0E00 CECE 0000 CECE 0000 CECE"
	$"00CE CE00 FECE 0600 CECE 00CE CE00 F7CE 0000 FECE 0400 CECE 0000 F9CE 0700 00CE"
	$"CE00 CECE 00FE CE00 00FE CE04 0000 CECE 00FA CE00 00FE CE02 00CE 00FE CE00 00FE"
	$"CE01 0000 FECE 0000 FECE 0100 00FE CE00 00FE CE00 00FC CE00 00FE CE03 00CE CE00"
	$"FECE 1400 CECE 0000 CECE 00CE CE00 00CE CE00 CECE 00CE CE00 FECE 0300 CECE 00FE"
	$"CE00 00FE CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0B00 CECE 0000 CECE 0000 CECE"
	$"00FE CE03 00CE CE00 FBCE 0000 FECE 0700 CECE 00CE CE00 00FE CE00 00FA CE00 00FE"
	$"CE07 00CE CE00 00CE CE00 F1CE 00FF EFCF 0D00 CFCF 0000 CFCF 00CF CF00 CFCF 00FE"
	$"CF00 00FB CF03 00CF CF00 FECF 0C00 CFCF 00CF CF00 00CF 00CF CF00 FECF 0700 00CF"
	$"CF00 CFCF 00FE CF03 00CF CF00 FECF 1400 CFCF 0000 CFCF 00CF CF00 00CF CF00 CFCF"
	$"00CF CF00 FECF 0300 CFCF 00FE CF00 00FE CF00 00FB CF00 00FE CF03 00CF CF00 FECF"
	$"0000 FECF 0800 00CF CF00 00CF CF00 FACF 0000 FECF 0500 CF00 CFCF 00FE CF00 00FB"
	$"CF00 00FE CF0E 00CF CF00 00CF CF00 00CF CF00 CFCF 00FE CF06 00CF CF00 CFCF 00F7"
	$"CF00 00FE CF04 00CF CF00 00F9 CF07 0000 CFCF 00CF CF00 FECF 0000 FECF 0400 00CF"
	$"CF00 FACF 0000 FECF 0200 CF00 FECF 0000 FECF 0100 00FE CF00 00FE CF01 0000 FECF"
	$"0000 FECF 0000 FCCF 0000 FECF 0300 CFCF 00FE CF14 00CF CF00 00CF CF00 CFCF 0000"
	$"CFCF 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0000 FBCF 0000 FECF 0300"
	$"CFCF 00FE CF0B 00CF CF00 00CF CF00 00CF CF00 FECF 0300 CFCF 00FB CF00 00FE CF07"
	$"00CF CF00 CFCF 0000 FECF 0000 FACF 0000 FECF 0700 CFCF 0000 CFCF 00F1 CF00 FFEF"
	$"CE0D FFCE CEFF FFCE CEFF CECE FFCE CEFF FECE 00FF FBCE 03FF CECE FFFE CE0C FFCE"
	$"CEFF CECE FFFF CEFF CECE FFFE CE07 FFFF CECE FFCE CEFF FECE 03FF CECE FFFE CE14"
	$"FFCE CEFF FFCE CEFF CECE FFFF CECE FFCE CEFF CECE FFFE CE03 FFCE CEFF FECE 00FF"
	$"FECE 00FF FBCE 00FF FECE 03FF CECE FFFE CE00 FFFE CE08 FFFF CECE FFFF CECE FFFA"
	$"CE00 00FE CE05 00CE 00CE CE00 FECE 0000 FBCE 0000 FECE 0E00 CECE 0000 CECE 0000"
	$"CECE 00CE CE00 FECE 0600 CECE 00CE CE00 F7CE 0000 FECE 0400 CECE 0000 F9CE 07FF"
	$"FFCE CEFF CECE FFFE CE00 FFFE CE04 FFFF CECE FFFA CE00 FFFE CE02 FFCE FFFE CE00"
	$"FFFE CE01 FFFF FECE 00FF FECE 01FF FFFE CE00 FFFE CE00 FFFC CE00 FFFE CE03 FFCE"
	$"CEFF FECE 14FF CECE FFFF CECE FFCE CEFF FFCE CEFF CECE FFCE CEFF FECE 03FF CECE"
	$"FFFE CE00 FFFE CE00 FFFB CE00 FFFE CE03 FFCE CEFF FECE 0BFF CECE FFFF CECE FFFF"
	$"CECE FFFE CE03 FFCE CEFF FBCE 00FF FECE 07FF CECE FFCE CEFF FFFE CE00 FFFA CE00"
	$"00FE CE07 00CE CE00 00CE CE00 F1CE 00FF 03E4 EFCE 0300 CECE 00FE CE06 00CE CE00"
	$"CECE 00FE CE00 00FB CE03 00CE CE00 FECE 0300 CECE 00FE CE04 00CE 00CE 00FC CE06"
	$"00CE CE00 CECE 00FB CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE06 00CE CE00"
	$"CECE 00FB CE00 00FE CE00 00F7 CE00 00FB CE00 00FE CE00 00FE CE00 00FE CE00 00FE"
	$"CE00 00F9 CE05 00CE 00CE CE00 FBCE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE"
	$"CE00 00FB CE06 00CE CE00 CECE 00F7 CE00 00FE CE03 00CE CE00 F8CE 0000 FECE 0300"
	$"CECE 00FE CE00 00FE CE00 00FE CE00 00FB CE00 00FE CE00 00FE CE18 00CE 00CE 00CE"
	$"00CE CE00 CE00 CE00 CE00 CECE 00CE 00CE 00CE 00FB CE00 00FB CE00 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00FB CE00 00FE CE00 00F7 CE00 00FB"
	$"CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0300 CECE 00FE CE00 00FB CE00 00FE CE03"
	$"00CE CE00 FECE 0300 CECE 00F9 CE00 00FE CE03 00CE CE00 FECE 0000 F1CE 00FF EFCF"
	$"0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF00 00FB CF03 00CF CF00 FECF 0300 CFCF"
	$"00FE CF04 00CF 00CF 00FC CF06 00CF CF00 CFCF 00FB CF00 00FE CF03 00CF CF00 FECF"
	$"0300 CFCF 00FE CF06 00CF CF00 CFCF 00FB CF00 00FE CF00 00F7 CF00 00FB CF00 00FE"
	$"CF00 00FE CF00 00FE CF00 00FE CF00 00F9 CF05 00CF 00CF CF00 FBCF 0000 FBCF 0000"
	$"FECF 0300 CFCF 00FE CF00 00FE CF00 00FB CF06 00CF CF00 CFCF 00F7 CF00 00FE CF03"
	$"00CF CF00 F8CF 0000 FECF 0300 CFCF 00FE CF00 00FE CF00 00FE CF00 00FB CF00 00FE"
	$"CF00 00FE CF18 00CF 00CF 00CF 00CF CF00 CF00 CF00 CF00 CFCF 00CF 00CF 00CF 00FB"
	$"CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FB"
	$"CF00 00FE CF00 00F7 CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0000 FECF 0300 CFCF"
	$"00FE CF00 00FB CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00F9 CF00 00FE CF03 00CF"
	$"CF00 FECF 0000 F1CF 00FF EFCE 03FF CECE FFFE CE06 FFCE CEFF CECE FFFE CE00 FFFB"
	$"CE03 FFCE CEFF FECE 03FF CECE FFFE CE04 FFCE FFCE FFFC CE06 FFCE CEFF CECE FFFB"
	$"CE00 FFFE CE03 FFCE CEFF FECE 03FF CECE FFFE CE06 FFCE CEFF CECE FFFB CE00 FFFE"
	$"CE00 FFF7 CE00 FFFB CE00 FFFE CE00 FFFE CE00 FFFE CE00 FFFE CE00 FFF9 CE05 00CE"
	$"00CE CE00 FBCE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE06 00CE"
	$"CE00 CECE 00F7 CE00 00FE CE03 00CE CE00 F8CE 00FF FECE 03FF CECE FFFE CE00 FFFE"
	$"CE00 FFFE CE00 FFFB CE00 FFFE CE00 FFFE CE18 FFCE FFCE FFCE FFCE CEFF CEFF CEFF"
	$"CEFF CECE FFCE FFCE FFCE FFFB CE00 FFFB CE00 FFFE CE03 FFCE CEFF FECE 03FF CECE"
	$"FFFE CE06 FFCE CEFF CECE FFFB CE00 FFFE CE00 FFF7 CE00 FFFB CE00 FFFE CE03 FFCE"
	$"CEFF FECE 00FF FECE 03FF CECE FFFE CE00 FFFB CE00 FFFE CE03 FFCE CEFF FECE 03FF"
	$"CECE FFF9 CE00 00FE CE03 00CE CE00 FECE 0000 F1CE 00FF 03E4 EFCE 0300 CECE 00FE"
	$"CE06 00CE CE00 CECE 00FE CE01 00CE FE00 04CE CE00 CECE FC00 02CE CE00 FECE 0400"
	$"CE00 CE00 FDCE 0000 FECE 0000 FECE FE00 FECE 0000 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FE CEFE 00FE CE00 00FD CEFE 00FA CE00 00FB CE00 00FE CE00"
	$"00FE CE00 00FE CE00 00FE CE00 00F9 CE05 00CE 00CE CE00 FECE FD00 FBCE FC00 02CE"
	$"CE00 FECE 0000 FECE 0000 FECE FD00 05CE CE00 CECE 00F7 CE00 00FE CE03 00CE CE00"
	$"F8CE 0000 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE00 00FB CE00 00FE CE00 00FE"
	$"CE18 00CE 00CE 00CE 00CE CE00 CE00 CE00 CE00 CECE 00CE 00CE 00CE 00FA CEFE 00FE"
	$"CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE FE00 FECE 0000"
	$"FDCE FE00 FACE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00 FECE"
	$"0000 FBCE FC00 02CE CE00 FECE 0300 CECE 00F9 CE00 00FE CE03 00CE CE00 FECE 0000"
	$"F1CE 00FF EFCF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF01 00CF FE00 04CF CF00"
	$"CFCF FC00 02CF CF00 FECF 0400 CF00 CF00 FDCF 0000 FECF 0000 FECF FE00 FECF 0000"
	$"FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CFFE 00FE CF00 00FD CFFE"
	$"00FA CF00 00FB CF00 00FE CF00 00FE CF00 00FE CF00 00FE CF00 00F9 CF05 00CF 00CF"
	$"CF00 FECF FD00 FBCF FC00 02CF CF00 FECF 0000 FECF 0000 FECF FD00 05CF CF00 CFCF"
	$"00F7 CF00 00FE CF03 00CF CF00 F8CF 0000 FECF 0300 CFCF 00FE CF00 00FE CF00 00FE"
	$"CF00 00FB CF00 00FE CF00 00FE CF18 00CF 00CF 00CF 00CF CF00 CF00 CF00 CF00 CFCF"
	$"00CF 00CF 00CF 00FA CFFE 00FE CF00 00FE CF03 00CF CF00 FECF 0300 CFCF 00FE CF03"
	$"00CF CF00 FECF FE00 FECF 0000 FDCF FE00 FACF 0000 FBCF 0000 FECF 0300 CFCF 00FE"
	$"CF00 00FE CF03 00CF CF00 FECF 0000 FBCF FC00 02CF CF00 FECF 0300 CFCF 00F9 CF00"
	$"00FE CF03 00CF CF00 FECF 0000 F1CF 00FF EFCE 03FF CECE FFFE CE06 FFCE CEFF CECE"
	$"FFFE CE01 FFCE FEFF 04CE CEFF CECE FCFF 02CE CEFF FECE 04FF CEFF CEFF FDCE 00FF"
	$"FECE 00FF FECE FEFF FECE 00FF FECE 03FF CECE FFFE CE03 FFCE CEFF FECE 03FF CECE"
	$"FFFE CEFE FFFE CE00 FFFD CEFE FFFA CE00 FFFB CE00 FFFE CE00 FFFE CE00 FFFE CE00"
	$"FFFE CE00 FFF9 CE05 00CE 00CE CE00 FECE FD00 FBCE FC00 02CE CE00 FECE 0000 FECE"
	$"0000 FECE FD00 05CE CE00 CECE 00F7 CE00 00FE CE03 00CE CE00 F8CE 00FF FECE 03FF"
	$"CECE FFFE CE00 FFFE CE00 FFFE CE00 FFFB CE00 FFFE CE00 FFFE CE18 FFCE FFCE FFCE"
	$"FFCE CEFF CEFF CEFF CEFF CECE FFCE FFCE FFCE FFFA CEFE FFFE CE00 FFFE CE03 FFCE"
	$"CEFF FECE 03FF CECE FFFE CE03 FFCE CEFF FECE FEFF FECE 00FF FDCE FEFF FACE 00FF"
	$"FBCE 00FF FECE 03FF CECE FFFE CE00 FFFE CE03 FFCE CEFF FECE 00FF FBCE FCFF 02CE"
	$"CEFF FECE 03FF CECE FFF9 CE00 00FE CE03 00CE CE00 FECE 0000 F1CE 00FF 03D8 EFCE"
	$"0300 CECE 00FE CE06 00CE CE00 CECE 00FE CE00 00FB CE03 00CE CE00 FBCE 0000 FECE"
	$"0400 CE00 CE00 FDCE 0300 CECE 00FA CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE"
	$"CE00 FECE 0300 CECE 00FB CE03 00CE CE00 FACE 0000 FBCE 0000 FBCE 0000 FECE 0000"
	$"FECE 0000 FECE 0000 FECE 0000 F9CE 0800 CE00 CECE 00CE CE00 FECE 0000 FBCE 0000"
	$"FBCE 0000 FECE 0000 FECE 0300 CECE 00FE CE06 00CE CE00 CECE 00F7 CE00 00FE CE03"
	$"00CE CE00 F8CE 0000 FECE 0300 CECE 00FE CE00 00FE CE00 00FE CE00 00FB CE00 00FE"
	$"CE00 00FE CE18 00CE 00CE 00CE 00CE CE00 CE00 CE00 CE00 CECE 00CE 00CE 00CE 00F7"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE03 00CE CE00 FECE 0300 CECE 00FB CE03 00CE"
	$"CE00 FACE 0000 FBCE 0000 FBCE 0000 FECE 0300 CECE 00FE CE00 00FE CE03 00CE CE00"
	$"FECE 0000 FBCE 0000 FBCE 0000 FECE 0300 CECE 00F9 CE00 00FE CE03 00CE CE00 FECE"
	$"0000 F1CE 00FF EFCF 0300 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF00 00FB CF03 00CF"
	$"CF00 FBCF 0000 FECF 0400 CF00 CF00 FDCF 0300 CFCF 00FA CF03 00CF CF00 FECF 0300"
	$"CFCF 00FE CF03 00CF CF00 FECF 0300 CFCF 00FB CF03 00CF CF00 FACF 0000 FBCF 0000"
	$"FBCF 0000 FECF 0000 FECF 0000 FECF 0000 FECF 0000 F9CF 0800 CF00 CFCF 00CF CF00"
	$"FECF 0000 FBCF 0000 FBCF 0000 FECF 0000 FECF 0300 CFCF 00FE CF06 00CF CF00 CFCF"
	$"00F7 CF00 00FE CF03 00CF CF00 F8CF 0000 FECF 0300 CFCF 00FE CF00 00FE CF00 00FE"
	$"CF00 00FB CF00 00FE CF00 00FE CF18 00CF 00CF 00CF 00CF CF00 CF00 CF00 CF00 CFCF"
	$"00CF 00CF 00CF 00F7 CF03 00CF CF00 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0300"
	$"CFCF 00FB CF03 00CF CF00 FACF 0000 FBCF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF00"
	$"00FE CF03 00CF CF00 FECF 0000 FBCF 0000 FBCF 0000 FECF 0300 CFCF 00F9 CF00 00FE"
	$"CF03 00CF CF00 FECF 0000 F1CF 00FF EFCE 03FF CECE FFFE CE06 FFCE CEFF CECE FFFE"
	$"CE00 FFFB CE03 FFCE CEFF FBCE 00FF FECE 04FF CEFF CEFF FDCE 03FF CECE FFFA CE03"
	$"FFCE CEFF FECE 03FF CECE FFFE CE03 FFCE CEFF FECE 03FF CECE FFFB CE03 FFCE CEFF"
	$"FACE 00FF FBCE 00FF FBCE 00FF FECE 00FF FECE 00FF FECE 00FF FECE 00FF F9CE 0800"
	$"CE00 CECE 00CE CE00 FECE 0000 FBCE 0000 FBCE 0000 FECE 0000 FECE 0300 CECE 00FE"
	$"CE06 00CE CE00 CECE 00F7 CE00 00FE CE03 00CE CE00 F8CE 00FF FECE 03FF CECE FFFE"
	$"CE00 FFFE CE00 FFFE CE00 FFFB CE00 FFFE CE00 FFFE CE18 FFCE FFCE FFCE FFCE CEFF"
	$"CEFF CEFF CEFF CECE FFCE FFCE FFCE FFF7 CE03 FFCE CEFF FECE 03FF CECE FFFE CE03"
	$"FFCE CEFF FECE 03FF CECE FFFB CE03 FFCE CEFF FACE 00FF FBCE 00FF FBCE 00FF FECE"
	$"03FF CECE FFFE CE00 FFFE CE03 FFCE CEFF FECE 00FF FBCE 00FF FBCE 00FF FECE 03FF"
	$"CECE FFF9 CE00 00FE CE03 00CE CE00 FECE 0000 F1CE 00FF 0435 EFCE 0300 CECE 00FE"
	$"CE06 00CE CE00 CECE 00FE CE00 00FB CE03 00CE CE00 FECE 0300 CECE 00FE CE04 00CE"
	$"00CE 00FE CE04 0000 CECE 00FE CE00 00FE CE03 00CE CE00 FECE 0A00 CECE 0000 CECE"
	$"00CE CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE00"
	$"00FE CE03 00CE CE00 FECE 0000 FECE 0000 FECE 0000 FECE 0000 F8CE 0000 FECE 0700"
	$"CECE 00CE CE00 00FB CE00 00FE CE03 00CE CE00 FECE 0000 FECE 0D00 CECE 00CE CE00"
	$"00CE CE00 CECE 00F7 CE00 00FE CE03 00CE CE00 F8CE 0000 FECE 0300 CECE 00FE CE00"
	$"00FE CE04 0000 CECE 00FC CE00 00FE CE00 00FC CE00 00FE CE00 00FD CE00 00FE CE00"
	$"00FD CE00 00FE CE00 00FA CE00 00FE CE03 00CE CE00 FECE 0A00 CECE 0000 CECE 00CE"
	$"CE00 FECE 0600 CECE 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE00 00FB CE00 00FE"
	$"CE03 00CE CE00 FECE 0300 CECE 00FE CE00 00FE CE02 00CE 00FD CE00 00FB CE00 00FE"
	$"CE03 00CE CE00 FECE 0200 CE00 F8CE 0000 FECE 0300 CECE 00FE CE00 00F1 CE00 FFEF"
	$"CF03 00CF CF00 FECF 0600 CFCF 00CF CF00 FECF 0000 FBCF 0300 CFCF 00FE CF03 00CF"
	$"CF00 FECF 0400 CF00 CF00 FECF 0400 00CF CF00 FECF 0000 FECF 0300 CFCF 00FE CF0A"
	$"00CF CF00 00CF CF00 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0000"
	$"FECF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF00 00FE CF00 00FE CF00 00FE CF00 00F8"
	$"CF00 00FE CF07 00CF CF00 CFCF 0000 FBCF 0000 FECF 0300 CFCF 00FE CF00 00FE CF0D"
	$"00CF CF00 CFCF 0000 CFCF 00CF CF00 F7CF 0000 FECF 0300 CFCF 00F8 CF00 00FE CF03"
	$"00CF CF00 FECF 0000 FECF 0400 00CF CF00 FCCF 0000 FECF 0000 FCCF 0000 FECF 0000"
	$"FDCF 0000 FECF 0000 FDCF 0000 FECF 0000 FACF 0000 FECF 0300 CFCF 00FE CF0A 00CF"
	$"CF00 00CF CF00 CFCF 00FE CF06 00CF CF00 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF"
	$"0000 FBCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00 FECF 0000 FECF 0200 CF00 FDCF"
	$"0000 FBCF 0000 FECF 0300 CFCF 00FE CF02 00CF 00F8 CF00 00FE CF03 00CF CF00 FECF"
	$"0000 F1CF 00FF EFCE 03FF CECE FFFE CE06 FFCE CEFF CECE FFFE CE00 FFFB CE03 FFCE"
	$"CEFF FECE 03FF CECE FFFE CE04 FFCE FFCE FFFE CE04 FFFF CECE FFFE CE00 FFFE CE03"
	$"FFCE CEFF FECE 0AFF CECE FFFF CECE FFCE CEFF FECE 06FF CECE FFCE CEFF FECE 03FF"
	$"CECE FFFE CE00 FFFE CE00 FFFB CE00 FFFE CE03 FFCE CEFF FECE 00FF FECE 00FF FECE"
	$"00FF FECE 00FF F8CE 0000 FECE 0700 CECE 00CE CE00 00FB CE00 00FE CE03 00CE CE00"
	$"FECE 0000 FECE 0D00 CECE 00CE CE00 00CE CE00 CECE 00F7 CE00 00FE CE03 00CE CE00"
	$"F8CE 00FF FECE 03FF CECE FFFE CE00 FFFE CE04 FFFF CECE FFFC CE00 FFFE CE00 FFFC"
	$"CE00 FFFE CE00 FFFD CE00 FFFE CE00 FFFD CE00 FFFE CE00 FFFA CE00 FFFE CE03 FFCE"
	$"CEFF FECE 0AFF CECE FFFF CECE FFCE CEFF FECE 06FF CECE FFCE CEFF FECE 03FF CECE"
	$"FFFE CE00 FFFE CE00 FFFB CE00 FFFE CE03 FFCE CEFF FECE 03FF CECE FFFE CE00 FFFE"
	$"CE02 FFCE FFFD CE00 FFFB CE00 FFFE CE03 FFCE CEFF FECE 02FF CEFF F8CE 0000 FECE"
	$"0300 CECE 00FE CE00 00F1 CE00 FF03 BAEF CE03 00CE CE00 FECE 0300 CECE 00FE CEFE"
	$"00FA CE00 00FE CEFE 00FD CEFD 0003 CE00 CECE FE00 00CE FE00 FCCE FE00 FDCE FE00"
	$"FECE 0300 CE00 00FE CE00 00FE CE03 00CE CE00 FECE FE00 FECE 0100 00FE CEFE 00FE"
	$"CE00 00FD CEFE 00FD CEFE 00FD CE00 00FE CE00 00FE CE00 00F8 CE00 00FE CE00 00FE"
	$"CE03 0000 CE00 FACE FE00 FECE 0000 FECE 0000 FECE 0000 FECE 0900 00CE 00CE CE00"
	$"CECE 00FE CE00 00FA CEFE 00FE CE00 00F8 CE00 00FE CE0E 00CE CE00 00CE CE00 00CE"
	$"CE00 CE00 00FE CE03 00CE CE00 FECE 0000 FCCE 0000 FECE 0000 FDCE 0000 FECE 0000"
	$"FDCE 0000 FECE 0000 FECE 0000 FDCE FE00 FDCE FE00 FECE 0300 CE00 00FE CE00 00FE"
	$"CE03 00CE CE00 FECE FE00 FECE 0100 00FE CEFE 00FE CE00 00FD CEFE 00FD CEFE 00FE"
	$"CE00 00FE CE00 00FE CE02 00CE 00FD CEFB 0001 CECE FE00 FDCE FD00 01CE 00F7 CEFE"
	$"00FE CE00 00FE CE00 00F1 CE00 FFEF CF03 00CF CF00 FECF 0300 CFCF 00FE CFFE 00FA"
	$"CF00 00FE CFFE 00FD CFFD 0003 CF00 CFCF FE00 00CF FE00 FCCF FE00 FDCF FE00 FECF"
	$"0300 CF00 00FE CF00 00FE CF03 00CF CF00 FECF FE00 FECF 0100 00FE CFFE 00FE CF00"
	$"00FD CFFE 00FD CFFE 00FD CF00 00FE CF00 00FE CF00 00F8 CF00 00FE CF00 00FE CF03"
	$"0000 CF00 FACF FE00 FECF 0000 FECF 0000 FECF 0000 FECF 0900 00CF 00CF CF00 CFCF"
	$"00FE CF00 00FA CFFE 00FE CF00 00F8 CF00 00FE CF0E 00CF CF00 00CF CF00 00CF CF00"
	$"CF00 00FE CF03 00CF CF00 FECF 0000 FCCF 0000 FECF 0000 FDCF 0000 FECF 0000 FDCF"
	$"0000 FECF 0000 FECF 0000 FDCF FE00 FDCF FE00 FECF 0300 CF00 00FE CF00 00FE CF03"
	$"00CF CF00 FECF FE00 FECF 0100 00FE CFFE 00FE CF00 00FD CFFE 00FD CFFE 00FE CF00"
	$"00FE CF00 00FE CF02 00CF 00FD CFFB 0001 CFCF FE00 FDCF FD00 01CF 00F7 CFFE 00FE"
	$"CF00 00FE CF00 00F1 CF00 FFEF CE03 FFCE CEFF FECE 03FF CECE FFFE CEFE FFFA CE00"
	$"FFFE CEFE FFFD CEFD FF03 CEFF CECE FEFF 00CE FEFF FCCE FEFF FDCE FEFF FECE 03FF"
	$"CEFF FFFE CE00 FFFE CE03 FFCE CEFF FECE FEFF FECE 01FF FFFE CEFE FFFE CE00 FFFD"
	$"CEFE FFFD CEFE FFFD CE00 FFFE CE00 FFFE CE00 FFF8 CE00 00FE CE00 00FE CE03 0000"
	$"CE00 FACE FE00 FECE 0000 FECE 0000 FECE 0000 FECE 0900 00CE 00CE CE00 CECE 00FE"
	$"CE00 00FA CEFE 00FE CE00 00F8 CE00 FFFE CE0E FFCE CEFF FFCE CEFF FFCE CEFF CEFF"
	$"FFFE CE03 FFCE CEFF FECE 00FF FCCE 00FF FECE 00FF FDCE 00FF FECE 00FF FDCE 00FF"
	$"FECE 00FF FECE 00FF FDCE FEFF FDCE FEFF FECE 03FF CEFF FFFE CE00 FFFE CE03 FFCE"
	$"CEFF FECE FEFF FECE 01FF FFFE CEFE FFFE CE00 FFFD CEFE FFFD CEFE FFFE CE00 FFFE"
	$"CE00 FFFE CE02 FFCE FFFD CEFB FF01 CECE FEFF FDCE FDFF 01CE FFF7 CEFE 00FE CE00"
	$"00FE CE00 00F1 CE00 FF00 54C6 CE00 00F8 CE00 00F1 CE00 0088 CE00 00DB CE00 00C2"
	$"CE00 0084 CE00 FFC6 CF00 00F8 CF00 00F1 CF00 0088 CF00 00DB CF00 00C2 CF00 0084"
	$"CF00 FFC6 CE00 FFF8 CE00 FFF1 CE00 FF88 CE00 00DB CE00 FFC2 CE00 FF84 CE00 FF00"
	$"57C5 CE00 00FB CE01 0000 F0CE 0000 88CE 0000 DBCE 0000 C2CE 0000 84CE 00FF C5CF"
	$"0000 FBCF 0100 00F0 CF00 0088 CF00 00DB CF00 00C2 CF00 0084 CF00 FFC5 CE00 FFFB"
	$"CE01 FFFF F0CE 00FF 88CE 0000 DBCE 00FF C2CE 00FF 84CE 00FF 0042 C4CE FB00 EECE"
	$"0000 81CE E1CE 0000 C2CE 0000 84CE 00FF C4CF FB00 EECF 0000 81CF E1CF 0000 C2CF"
	$"0000 84CF 00FF C4CE FBFF EECE 00FF 81CE E1CE 00FF C2CE 00FF 84CE 00FF 001E 81CE"
	$"81CE 81CE CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE"
	$"81CE 81CE CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 001E 81CE"
	$"81CE 81CE CCCE 00FF 81CF 81CF 81CF CCCF 00FF 81CE 81CE 81CE CCCE 00FF 0030 EBCE"
	$"0000 E0CE 0000 81CE 81CE 85CE 00FF EBCF 0000 E0CF 0000 81CF 81CF 85CF 00FF EBCE"
	$"0000 E0CE 0000 81CE 81CE 85CE 00FF 003C EFCE 0000 FECE 0000 E0CE 0000 81CE 81CE"
	$"85CE 00FF EFCF 0000 FECF 0000 E0CF 0000 81CF 81CF 85CF 00FF EFCE 0000 FECE 0000"
	$"E0CE 0000 81CE 81CE 85CE 00FF 00FC EFCE 0000 FECE 0000 E0CE 0000 81CE 9BCE 00FF"
	$"F3CE 009C F4CE 009C FECE 00FF FBCE 009C F0CE 009C FCCE 00FF FECE 009C EBCE 049C"
	$"CECE FF9C FDCE 00FF F5CE 00FF FACE 009C FBCE 009C ECCE 00FF EFCF 0000 FECF 0000"
	$"E0CF 0000 81CF 9BCF 00FF FBCF 00FF FECF 00FF FBCF 04FF CFFF CFFF FECF 00FF FECF"
	$"06FF CFCF FFCF CFFF F9CF FEFF FECF 02FF CFFF F7CF 00FF FDCF 04FF CFFF CFFF FECF"
	$"00FF FDCF 04FF FFCF CFFF FACF 00FF FDCF 04FF FFCF CFFF FECF 07FF CFCF FFCF CFFF"
	$"FFF3 CF00 FFED CF00 FFEF CE00 00FE CE00 00E0 CE00 0081 CE9B CE03 FFCE CEFF E5CE"
	$"01FF FFE3 CE00 FFF7 CE01 FFFF F7CE 00FF FBCE 00FF FCCE 00FF FCCE 02FF CEFF FDCE"
	$"00FF EFCE 00FF F0CE 00FF 01C8 F0CE FE00 05CE CE00 CE00 00FD CEFE 00FB CE00 00FE"
	$"CE00 00FE CE02 00CE CEFE 00FE CE03 00CE 0000 81CE 9ECE 00FF FACE FEFF 05CE CE9C"
	$"CECE FFFB CE01 9CFF FECE 10FF FF9C FFCE CE9C 9CCE CE9C 9CFF 9C9C CE9C FECE 0EFF"
	$"CE9C FFFF CEFF CE9C CEFF CECE 9CFF FECE 009C FACE FE9C 03FF CE9C 9CFE CE11 9CCE"
	$"CEFF CECE 9CFF 9CCE CEFF 9CCE 9CCE CEFF FECE 019C FFFE CE0C 9CCE FFCE 9CFF 9CCE"
	$"9CCE CEFF 9CFE CE02 9CFF CEFE FF03 CECE 9CFF F3CE 00FF F0CF FE00 05CF CF00 CF00"
	$"00FD CFFE 00FB CF00 00FE CF00 00FE CF02 00CF CFFE 00FE CF03 00CF 0000 81CF 9ECF"
	$"00FF FCCF 01FF CFFE FFFC CF01 FFFF FBCF 02FF CFCF FEFF 05CF FFCF CF9A 9AFD CF01"
	$"FF9A FBCF 06FF CFCF FFFF CFFF FECF 00FF FECF 00FF FACF 00FF FDCF FEFF F8CF 08FF"
	$"FFCF CFFF CFFF CFFF FCCF 00FF FDCF 02FF CFFF FECF 03FF CF9A FFF6 CF01 FFCF FEFF"
	$"03CF FFCF FFF3 CF00 FFF0 CEFE 0005 CECE 00CE 0000 FDCE FE00 FBCE 0000 FECE 0000"
	$"FECE 0200 CECE FE00 FECE 0300 CE00 0081 CE9E CE00 FFFA CEFE FFFC CE00 FFFD CE0E"
	$"FFCE CEFF CEFF CEFF FFCE FFCE CE9C 9CFD CE01 FF9C FBCE 06FF CECE FFFF CEFF FECE"
	$"00FF FECE 00FF F3CE 00FF F8CE 00FF FECE 00FF FECE 00FF FCCE 00FF FDCE 00FF FCCE"
	$"03FF CE9C FFF6 CE01 FFCE FEFF FECE 00FF F3CE 00FF 01A1 EFCE 0000 FECE 0700 00CE"
	$"CE00 CECE 00FE CE00 00FC CE00 00FE CE00 00FE CE02 00CE 00FE CE07 00CE CE00 00CE"
	$"CE00 81CE 9FCE 04FF 9C9C CE9C FECE FD9C 04CE 9CCE CE9C FECE 009C FECE 009C FECE"
	$"049C CECE 9C9C FDCE 009C FCCE 009C FECE 019C 9CFE CEFE 9CFD CE08 9C9C CE9C CE9C"
	$"9CCE 9CFE CE00 9CFE CEFE 9C01 CECE FD9C FECE 029C CE9C FECE 039C CE9C 9CFD CE05"
	$"9CCE CE9C FF9C FDCE FE9C F6CE 039C CE9C CEFE 9CFE CE00 9CF3 CE00 FFEF CF00 00FE"
	$"CF07 0000 CFCF 00CF CF00 FECF 0000 FCCF 0000 FECF 0000 FECF 0200 CF00 FECF 0700"
	$"CFCF 0000 CFCF 0081 CF9F CF00 FFFA CF05 9A9A CF9A CF9A F0CF 009A F6CF 009A FDCF"
	$"009A FECF 029A CF9A FCCF 009A FDCF 009A F8CF FE9A 05CF CF9A CF9A 9AF8 CF03 9ACF"
	$"9A9A FDCF 009A FECF 00FF FBCF 009A F9CF 00FF FACF 009A EDCF 00FF EFCE 0000 FECE"
	$"0700 00CE CE00 CECE 00FE CE00 00FC CE00 00FE CE00 00FE CE02 00CE 00FE CE07 00CE"
	$"CE00 00CE CE00 81CE 9FCE 00FF FACE 059C 9CCE 9CCE 9CF0 CE00 9CF6 CE00 9CFD CE00"
	$"9CFE CE02 9CCE 9CFC CE00 9CFD CE00 9CF8 CEFE 9C05 CECE 9CCE 9C9C FACE 059C CE9C"
	$"CE9C 9CFD CE00 9CFE CE00 FFFB CE00 9CF1 CE00 9CED CE00 FF01 C0EF CE00 00FE CE00"
	$"00FE CE03 00CE CE00 FECE 0000 FBCE 0900 CE00 CE00 CE00 CECE 00FE CE03 00CE CE00"
	$"FECE 0000 81CE 9FCE 06FF CECE 9CCE 9CCE FD9C 04CE 9CCE 9C9C FECE 049C 9CCE 9CCE"
	$"FE9C 04CE 9C9C CE9C FCCE FD9C 02CE CE9C FDCE FA9C 01CE CEFD 9C0B CE9C CE9C 9CCE"
	$"9C9C CECE 9CCE FC9C FECE FD9C 16CE CE9C CE9C CECE 9C9C CE9C CE9C CE9C 9CCE CE9C"
	$"CE9C 9CCE FE9C 06CE 9CCE CE9C CECE FE9C 00CE FD9C 02CE 9CCE FC9C 01CE 9CF3 CE00"
	$"FFEF CF00 00FE CF00 00FE CF03 00CF CF00 FECF 0000 FBCF 0900 CF00 CF00 CF00 CFCF"
	$"00FE CF03 00CF CF00 FECF 0000 81CF 9FCF 05FF 9A9A CFCF 9AFA CF14 9ACF CF9A CFCF"
	$"9ACF 9A9A CFCF 9ACF 9ACF CF9A CFCF 9AFB CF00 9AFD CF1D 9A9A CFCF 9ACF 9A9A CF9A"
	$"CFCF 9ACF CF9A CFCF 9ACF 9ACF CF9A CFCF 9ACF 9A9A FCCF 019A CFFE 9A01 CF9A FCCF"
	$"029A CF9A FECF 0D9A CFCF 9ACF CF9A CF9A 9ACF 9ACF 9AFE CF00 9AFC CF00 9AFA CF06"
	$"9ACF CF9A 9ACF 9AF3 CF00 FFEF CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FBCE"
	$"0900 CE00 CE00 CE00 CECE 00FE CE03 00CE CE00 FECE 0000 81CE 9FCE 00FF FDCE 009C"
	$"F4CE 069C CECE 9CCE CE9C EACE 039C CE9C 9CFD CE03 9CCE CE9C FDCE 009C F9CE 009C"
	$"F8CE 009C ECCE 039C CECE 9CEE CE06 9CCE CE9C CECE 9CF3 CE00 FF01 F1EF CE00 00FE"
	$"CE00 00FE CE02 00CE CEFC 00FB CE08 00CE 00CE 00CE 00CE CEFC 0002 CECE 00FE CE00"
	$"0081 CE9F CE00 FFFD 9C01 CE9C FCCE 0B9C 9CCE CE9C 9CCE 9CCE 9CCE 9CFE CE04 9CCE"
	$"CE9C CEFC 9CFD CE02 9C9C CEFD 9C01 CE9C FCCE FE9C 0CCE 9CCE 9CCE 9C9C CECE 9CCE"
	$"9CCE FE9C FDCE FD9C 01CE CEFE 9C0B CE9C 9CCE 9CCE 9C9C CECE 9CCE FD9C 0ECE CE9C"
	$"CE9C 9CCE CE9C CE9C 9CCE 9C9C FECE 019C CEFD 9C01 CE9C FCCE 009C F2CE 00FF EFCF"
	$"0000 FECF 0000 FECF 0200 CFCF FC00 FBCF 0800 CF00 CF00 CF00 CFCF FC00 02CF CF00"
	$"FECF 0000 81CF 9FCF 1AFF CFCF 9A9A CF9A 9ACF CF9A CF9A CF9A 9ACF 9ACF 9A9A CFCF"
	$"9ACF CF9A FECF 039A CF9A CFFE 9A09 CF9A 9ACF 9A9A CF9A CFCF FD9A 04CF CF9A CFCF"
	$"FD9A 0ECF 9A9A CF9A CFCF 9ACF 9A9A CF9A 9ACF FC9A 01CF 9AFE CF01 9ACF FE9A 1FCF"
	$"9ACF 9ACF 9ACF 9ACF 9A9A CF9A 9ACF 9ACF 9A9A CFCF 9ACF 9A9A CF9A 9ACF 9A9A CFFC"
	$"9A01 CF9A FCCF 009A F2CF 00FF EFCE 0000 FECE 0000 FECE 0200 CECE FC00 FBCE 0800"
	$"CE00 CE00 CE00 CECE FC00 02CE CE00 FECE 0000 81CE 9FCE 00FF FD9C 01CE 9CFE CE01"
	$"9CCE FB9C 0DCE 9C9C CECE 9CCE CE9C 9CCE CE9C CEFC 9C06 CE9C 9CCE 9C9C CEFC 9C0B"
	$"CE9C CECE 9CCE 9C9C CE9C 9CCE FD9C 03CE CE9C CEFC 9C00 CEFA 9C01 CECE FD9C 0ACE"
	$"9CCE 9CCE CE9C 9CCE 9CCE FC9C 11CE 9CCE 9C9C CECE 9CCE 9C9C CE9C 9CCE 9C9C CEFC"
	$"9C01 CE9C FCCE 009C F2CE 00FF 01CB EFCE 0000 FECE 0000 FECE 0300 CECE 00F7 CE09"
	$"00CE 00CE 00CE 00CE CE00 FBCE 0000 FECE 0000 81CE 9FCE 0BFF 9C9C CEFF FFCE FFCE"
	$"CEFF FFFD CE00 9CFD CEFD 9C00 CEFE FF02 CECE FFF7 CEFE 9C01 CECE FEFF 02CE CEFF"
	$"FDCE 07FF CECE 9CCE CEFF 9CFE CE00 9CFE CE06 9CCE CE9C 9CCE CEFC FF03 9CCE CEFF"
	$"FECE 029C CEFF FD9C 00CE FDFF 0CCE FFCE 9CFF CEFF CECE 9CCE 9CCE FE9C FDFF F6CE"
	$"009C F2CE 00FF EFCF 0000 FECF 0000 FECF 0300 CFCF 00F7 CF09 00CF 00CF 00CF 00CF"
	$"CF00 FBCF 0000 FECF 0000 81CF 9FCF 00FF FECF 01FF FFFD CF01 FFFF FBCF 009A FDCF"
	$"029A CFCF FEFF 03CF CFFF 9AFC CF01 9A9A FDCF 029A CFCF FEFF 07CF CFFF 9AFF CFCF"
	$"FFFE CF05 9ACF FFCF CFFF FECF 00FF FDCF 039A 9ACF CFFC FFFE CF00 FFFE CF04 9ACF"
	$"FFCF 9AFE CF0A FFCF FFCF CFFF CFCF FFCF FFFB CF02 9ACF CFFD FFFE CF01 9AFF FCCF"
	$"009A F1CF 00FF EFCE 0000 FECE 0000 FECE 0300 CECE 00F7 CE09 00CE 00CE 00CE 00CE"
	$"CE00 FBCE 0000 FECE 0000 81CE 9FCE 00FF FECE 02FF CEFF FECE 01FF FFFB CE01 9CCE"
	$"FEFF 02CE FFCE FEFF 02CE CEFF FBCE 059C 9CCE FFCE FFFE CEFE FF02 CECE FFFD CE05"
	$"FFCE CE9C 9CCE F5FF FECE FBFF FECE 00FF FECE 129C 9CFF FFCE CEFF CEFF CEFF CECE"
	$"FFCE CEFF CEFF FECE 049C CEFF FFCE FDFF F6CE 019C 9CF2 CE00 FF01 DDEF CE00 00FE"
	$"CE00 00FE CE03 00CE CE00 FECE 0000 FACE 0000 FECE 0000 FECE 0000 FECE 0300 CECE"
	$"00FE CE00 0081 CE9F CE0B FF9C 9CCE FFFF CEFF CECE FFFF FDCE 009C FDCE FD9C 00CE"
	$"FEFF 02CE CEFF F7CE FE9C 01CE CEFE FF02 CECE FFFD CE07 FFCE CE9C CECE FF9C FECE"
	$"009C FECE 069C CECE 9C9C CECE FCFF 039C CECE FFFE CE02 9CCE FFFD 9C00 CEFD FF0C"
	$"CEFF CE9C FFCE FFCE CE9C CE9C CEFE 9CFD FFF6 CE00 9CF2 CE00 FFEF CF00 00FE CF00"
	$"00FE CF03 00CF CF00 FECF 0000 FACF 0000 FECF 0000 FECF 0000 FECF 0300 CFCF 00FE"
	$"CF00 0081 CF9F CF00 FFFE CF01 FFFF FDCF 01FF FFFB CF00 9AFD CF02 9ACF CFFE FF03"
	$"CFCF FF9A FCCF 019A 9AFD CF02 9ACF CFFE FF07 CFCF FF9A FFCF CFFF FECF 059A CFFF"
	$"CFCF FFFE CF00 FFFD CF03 9A9A CFCF FCFF FECF 00FF FECF 049A CFFF CF9A FECF 0AFF"
	$"CFFF CFCF FFCF CFFF CFFF FBCF 029A CFCF FDFF FECF 019A FFFC CF00 9AF1 CF00 FFEF"
	$"CE00 00FE CE00 00FE CE03 00CE CE00 FECE 0000 FACE 0000 FECE 0000 FECE 0000 FECE"
	$"0300 CECE 00FE CE00 0081 CE9F CE00 FFFE CE02 FFCE FFFE CE01 FFFF FBCE 019C CEFE"
	$"FF02 CEFF CEFE FF02 CECE FFFB CE05 9C9C CEFF CEFF FECE FEFF 02CE CEFF FDCE 05FF"
	$"CECE 9C9C CEF5 FFFE CEFB FFFE CE00 FFFE CE12 9C9C FFFF CECE FFCE FFCE FFCE CEFF"
	$"CECE FFCE FFFE CE04 9CCE FFFF CEFD FFF6 CE01 9C9C F2CE 00FF 0199 EFCE 0400 00CE"
	$"CE00 FECE 0000 FECE FE00 F9CE 0000 FECE 0000 FDCE FE00 FECE FD00 FECE 0000 81CE"
	$"A2CE 00FF FE9C 02CE CEFF FECE FD9C 00CE FB9C 00CE FE9C FDCE 019C CEFE 9C01 CECE"
	$"F89C FBCE FC9C 02CE 9CCE FB9C 00CE FE9C 00CE F99C FBCE 049C CE9C CECE FC9C 06FF"
	$"639C CE9C 9CFF FECE 019C CEFA 9C00 CEFC 9CFA CEFD 9C01 CECE FD9C F3CE 00FF EFCF"
	$"0400 00CF CF00 FECF 0000 FECF FE00 F9CF 0000 FECF 0000 FDCF FE00 FECF FD00 FECF"
	$"0000 81CF A2CF 00FF FCCF 00FF FECF 049A 9ACF 9ACF FE9A F4CF 009A FCCF 009A FDCF"
	$"009A F8CF 029A CF9A FDCF FE9A EBCF 0A9A CF9A CFCF 9ACF 9A9A CFFF FDCF 069A FFCF"
	$"FFCF 9ACF FD9A 02CF 9A9A FBCF 019A 9AFB CF07 9ACF 9ACF CF9A CF9A F2CF 00FF EFCE"
	$"0400 00CE CE00 FECE 0000 FECE FE00 F9CE 0000 FECE 0000 FDCE FE00 FECE FD00 FECE"
	$"0000 81CE A2CE FDFF 02CE CEFF FECE 029C 9CCE FB9C 03FF CEFF CEFE FFFC CE01 9C9C"
	$"FECE FD9C 02CE FFCE FDFF FCCE FE9C FDCE 059C 9CCE CEFF CEFE FF05 CEFF FFCE FFCE"
	$"FDFF FBCE FD9C 00CE FD9C FDFF 08CE FFCE FFFF CECE 9CCE FA9C 04CE CEFF CEFF FDCE"
	$"00FF FECE 099C 9CCE 9CCE CE9C 9CCE FFF3 CE00 FF01 3E81 CE81 CEE2 CE00 FFFD 9CF7"
	$"CE07 9CCE 9CCE CE9C 9CCE FE9C FBCE 00FF FECE 079C CE9C 9CCE 9C9C CEFD 9C08 CECE"
	$"FFCE CEFF 9CCE CEFE 9C0D CE9C CE9C CE9C CE9C 9CCE CE9C 9CCE FB9C 01CE 9CFB CE01"
	$"9CCE F79C F9CE 06FF 9C9C CE9C CECE FB9C FCCE 08FF 9CCE CE9C 9CCE 9C9C F2CE 00FF"
	$"81CF 81CF E2CF 00FF FCCF 03FF CFFF FFFE CF02 FFCF 9AF7 CF00 FFFE CF02 FFCF FFFD"
	$"CF00 9AF5 CF04 FFCF CFFF 9AFE CF00 9AEE CF00 9AFE CF01 FFFF FDCF FE9A F9CF 0C9A"
	$"CFCF FFCF CFFF CFCF FFCF 9A9A FACF 029A 9ACF FCFF 049A CFFF 9A9A EFCF 00FF 81CE"
	$"81CE E2CE 00FF FECE 00FF F7CE FE9C 08FF FFCE FFCE CEFF CEFF FCCE 00FF FECE 149C"
	$"9CFF FFCE FFCE FFFF CECE FFCE CEFF CECE FF9C CECE FE9C FEFF 00CE FDFF 00CE F7FF"
	$"F7CE 0A9C 9CFF CEFF CEFF FFCE FFFF FCCE 04FF CECE FFCE FE9C 01FF CEFE FF09 CEFF"
	$"FFCE CEFF FFCE FF9C FDCE 039C FFCE FFF3 CE00 FF01 1A81 CE81 CEE2 CE05 FF9C 9CCE"
	$"9C9C FDCE 079C FFCE FFCE CE9C CEFA 9C06 CE9C 9CFF CECE FFFC CEF9 9C04 CECE 9C9C"
	$"FFF9 CE03 9CCE 9CCE FC9C 11CE 9CCE 9C9C CE9C CE9C 9CCE 9C9C FFFF CECE FFFD CE06"
	$"9CCE 9CCE 9C9C CEFC 9CFC CE00 FFFB CEF8 9CFA CE00 FFFE CE02 9CCE 9CF3 CE00 FF81"
	$"CF81 CFE2 CF00 FFF7 CFFE FFFE CF00 9AF9 CF05 9ACF FFFF CFFF F0CF 05FF CFFF CFCF"
	$"FFFD CF00 9AED CF06 FFFF CFCF FFCF FFF0 CF03 FFCF FFFF EECF 02FF CFFF EDCF 00FF"
	$"81CE 81CE E2CE FDFF FECE 00FF FECE 02FF CEFF FECE 019C CEFC FF07 CEFF CECE FFCE"
	$"FFFF FBCE FBFF 00CE FEFF 04CE FFCE CEFF FBCE 029C FFCE FEFF 08CE FFCE FFFF CEFF"
	$"CECE FDFF 00CE FBFF FCCE 009C FDFF 0BCE CEFF FFCE CEFF CECE FFCE FFFA CE04 FFCE"
	$"FFCE FFF7 CE00 FFFD CE00 FFF2 CE00 FF01 2681 CE81 CEE2 CE00 FFFD 9CF4 CE02 9C9C"
	$"CEFB 9CFE CE02 FFCE 9CFC CE04 9C9C CE9C CEFC 9CF9 CE00 FFFE CEFE 9C03 CE9C CECE"
	$"FE9C 03CE 9C9C CEFB 9CF6 CEF8 9C01 CEFF F9CE 059C CECE 9C9C CEFC 9C01 CEFF F7CE"
	$"FE9C F3CE 00FF 81CF 81CF E2CF 00FF FECF 009A F8CF 03FF CFCF FFFC CF00 9AFB CF00"
	$"FFFE CF01 FFFF F9CF 0C9A CFCF 9ACF CFFF CFCF FFCF CFFF F8CF 00FF FACF 07FF 9A9A"
	$"CFCF 9ACF FFFC CF00 FFF9 CF00 9AFD CF02 FFCF FFF4 CF00 9AFD CF00 FFE6 CF00 FF81"
	$"CE81 CEE2 CE00 FFFA CE01 FFFF FDCE 00FF FECE 08FF FFCE FFCE FFCE FFFF FECE 00FF"
	$"FCCE 00FF FECE 02FF CECE FDFF 02CE CEFF FECE 12FF CECE FFFF CECE FFFF CECE FFFF"
	$"CEFF CEFF FFCE FEFF FECE 03FF CECE FFFC CE00 FFFD CE01 FFCE FEFF 05CE CEFF CECE"
	$"FFF7 CEFE FFFE CE06 FFFF CEFF CECE FFFA CEFE FFF3 CE00 FF01 6281 CE81 CEE2 CE00"
	$"FFFE 9C02 FFFF CEFE FF08 CEFF CEFF CEFF FFCE CEFA 9C07 FFFF CEFF CECE FFFF FECE"
	$"FEFF FA9C FEFF 00CE FDFF FECE 00FF FECE FD9C 08CE 9CCE 9C9C CE9C 9CCE FE9C 02CE"
	$"FFFF FECE 08FF CECE FFCE CEFF CECE FD9C 09CE 9CCE FFFF CEFF FFCE FFFE CE03 FFCE"
	$"FFCE FC9C 07CE CEFF FFCE CEFF FFFE CE05 FFFF CEFF CE9C F3CE 00FF 81CF 81CF E2CF"
	$"00FF FECF 02FF FFCF FEFF 00CF FEFF 02CF FFFF FECF 029A CF9A FECF 04FF FFCF FFCF"
	$"FEFF 01CF CFFD FF01 CF9A FCCF FEFF 00CF FDFF 01CF CFFE FFEE CF02 FFFF CFFE FF08"
	$"CFCF FFCF FFFF CFCF 9AFB CF0C FFFF CFFF FFCF FFCF CFFF FFCF FFF9 CF00 FFFE CF04"
	$"FFFF CFFF CFFD FFF1 CF00 FF81 CE81 CEE2 CEFB FF00 CEFA FF00 CEFE FF01 CECE FEFF"
	$"00CE FBFF 00CE FEFF FECE FEFF 00CE FEFF FECE FEFF 00CE FBFF 07CE FFFF CECE FFFF"
	$"CEF4 FF03 CEFF FFCE FBFF 02CE FFFF FECE 08FF CECE FFFF CEFF FFCE FCFF 07CE FFFF"
	$"CEFF CEFF CEFE FF05 CECE FFFF CECE FEFF 00CE FEFF 01CE FFF1 CE00 FF01 2A81 CE81"
	$"CEE2 CE05 FF9C 9CCE 9CFF FDCE 00FF FDCE 009C FECE FE9C 0ECE 9C9C CE9C FFFF CECE"
	$"FFCE 9CCE CE9C FECE FE9C 05CE 9CCE CE9C FFFC CE13 FFCE 9CCE 9CCE CE9C CE9C CE9C"
	$"CE9C 9CCE 9CFF 9C9C FDCE 0D9C 9CCE CEFF FFCE CE9C CE9C 9CCE CEFB 9C01 CE9C FDCE"
	$"00FF FECE 019C 9CFE CEF9 9C00 FFFA CE00 9CFD CE00 9CF2 CE00 FF81 CF81 CFE2 CF00"
	$"FFFD CF01 FFCF FCFF F2CF 04FF FFCF CFFF F1CF 01FF FFFD CF00 FFF6 CF00 FFFD CF00"
	$"FFF8 CFFE FF04 CFFF 9ACF 9AF4 CF06 FFCF FFFF CFCF 9AFD CF00 9AFA CF01 FFFF FBCF"
	$"009A EDCF 00FF 81CE 81CE E2CE FBFF 04CE FFCE CEFF F8CE FCFF 00CE FEFF 02CE FFFF"
	$"F9CE FDFF 00CE FDFF FECE 02FF CEFF FBCE 02FF FFCE F8FF 00CE FEFF 03CE FFFF CEFE"
	$"FFFD CE00 9CFE CE0C FFCE FFCE FFFF CEFF CECE FFCE FFF9 CE04 9CFF FFCE CEFD FFF5"
	$"CE00 FFF2 CE00 FF01 3C81 CE81 CEE2 CE00 FFFC 9C00 FFFD CE04 9CCE 9C9C CEFD 9C00"
	$"CEFA 9CFD CE01 9CCE FA9C 00CE FA9C FDCE 019C CEFE 9C00 CEFD 9C05 CE9C CECE 9CCE"
	$"FD9C 00CE FD9C 01CE 9CFD CE0A 9CCE 9C9C CE9C 9CCE 9C9C CEFD 9CFA CE01 9CCE F39C"
	$"00FF FDCE 019C CEFA 9CF3 CE00 FF81 CF81 CFE2 CF00 FFFD CF01 9AFF FDCF 069A CF9A"
	$"9ACF CF9A F6CF 00FF FDCF FD9A FBCF 009A FECF 039A CFCF FFFD CF01 9A9A F1CF 00FF"
	$"F8CF 059A CF9A 9ACF 9AFB CF00 9AFE CF00 9AFA CFFD 9AFA CFFE 9AFE CF00 FFFE CFFD"
	$"9AF0 CF00 FF81 CE81 CEE2 CE00 FFFB CE00 FFFE CE0E 9CCE 9C9C CE9C 9CFF FFCE FFCE"
	$"FFCE FFFB CE01 9CCE FD9C 05FF FFCE FFCE FFFE CE02 FF9C FFFE CE08 9CCE CE9C 9CCE"
	$"9CFF CEFD FF04 CEFF CEFF CEFA FF01 CEFF FDCE 039C CE9C CEFE 9C08 FFCE FFCE FFFF"
	$"CEFF FFFD CE01 FFCE FA9C 05FF FFCE FFFF CEFE 9C06 FFCE CEFF 9CCE CEFD 9C02 CEFF"
	$"FFF3 CE00 FF01 1581 CE81 CEE2 CE00 FFFD 9C01 CE9C FECE 009C FCCE 029C 9CCE F89C"
	$"FBCE 089C 9CCE 9CCE CE9C 9CCE FC9C 01CE FFFA CE07 9CCE 9CCE 9CCE 9CCE FD9C 04CE"
	$"9CCE 9CCE FA9C 00FF FBCE FE9C 03CE 9C9C CEFA 9C00 FFFB CE05 9CCE 9C9C CECE FB9C"
	$"01CE 9CFE CE02 FFCE CEFD 9C01 CE9C F2CE 00FF 81CF 81CF E2CF 00FF EACF 009A E7CF"
	$"00FF FCCF 019A 9AEB CFFD FFFE CF00 9AF9 CF00 9AFD CF03 FFCF CFFF FDCF 009A FACF"
	$"009A FACF 00FF FECF 019A 9AEF CF00 FF81 CE81 CEE2 CE06 FFCE CEFF FFCE FFFA CE02"
	$"9C9C CEFE FF07 CEFF CECE FFCE CEFF F9CE 0063 FAFF FDCE 03FF CECE FFFC CE02 9C9C"
	$"CEFC FF02 CEFF CEF6 FF02 CEFF FFFE CE0E FFCE CE9C FFCE FFCE FFCE FFFF CEFF CEFE"
	$"FFFA CE02 9CFF CEFC FF04 CEFF FFCE FFFE CE00 FFFA CE01 FFFF F3CE 00FF 0115 81CE"
	$"81CE E2CE 00FF FD9C 01CE 9CFE CE00 9CFC CE02 9C9C CEF8 9CFB CE08 9C9C CE9C CECE"
	$"9C9C CEFC 9C01 CEFF FACE 079C CE9C CE9C CE9C CEFD 9C04 CE9C CE9C CEFA 9C00 FFFB"
	$"CEFE 9C03 CE9C 9CCE FA9C 00FF FBCE 059C CE9C 9CCE CEFB 9C01 CE9C FECE 02FF CECE"
	$"FD9C 01CE 9CF2 CE00 FF81 CF81 CFE2 CF00 FFEA CF00 9AE7 CF00 FFFC CF01 9A9A EBCF"
	$"FDFF FECF 009A F9CF 009A FDCF 03FF CFCF FFFD CF00 9AFA CF00 9AFA CF00 FFFE CF01"
	$"9A9A EFCF 00FF 81CE 81CE E2CE 06FF CECE FFFF CEFF FACE 029C 9CCE FEFF 07CE FFCE"
	$"CEFF CECE FFF9 CE00 63FA FFFD CE03 FFCE CEFF FCCE 029C 9CCE FCFF 02CE FFCE F6FF"
	$"02CE FFFF FECE 0EFF CECE 9CFF CEFF CEFF CEFF FFCE FFCE FEFF FACE 029C FFCE FCFF"
	$"04CE FFFF CEFF FECE 00FF FACE 01FF FFF3 CE00 FF01 2981 CE81 CEE2 CE06 FF9C 9CCE"
	$"CE9C 9CFC CE04 9C9C CE9C CEFB 9C04 CE9C 9CCE 9CFC CE00 9CFE CEFB 9C00 CEFC 9CFB"
	$"CE06 9CCE 9CCE 9C9C CEFE 9C08 CECE 9CCE 9CCE 9C9C CEFE 9C04 CE9C CE9C FFFD CE02"
	$"9C9C CEFE 9C00 CEFE 9C00 CEFC 9CFD CE01 9CCE FE9C 00CE FC9C 00CE FC9C FCCE 079C"
	$"9CCE CE9C 9CCE 9CF3 CE00 FF81 CF81 CFE2 CF00 FFF5 CFFE 9AF3 CF00 FFFC CF00 9AEC"
	$"CF00 9AF8 CF00 FFF6 CF00 9AFE FF05 CFCF 9ACF CF9A EFCF 009A F1CF 00FF FECF FD9A"
	$"F0CF 00FF 81CE 81CE E2CE 06FF CECE FFFF CEFF FBCE 039C CE9C CEFE FF0A CEFF CEFF"
	$"CEFF CEFF CECE FFFC CE0C 9CCE FFFF CEFF CEFF FFCE CEFF FFF8 CE06 9CFF CEFF CEFF"
	$"CEFD FF04 CEFF CEFF CEFB FF00 CEFE FFFC CE04 9CFF CEFF CEFC FF02 CECE FFFB CE07"
	$"9CCE 9CCE FFCE FFCE FEFF FECE 03FF CECE FFFE CE00 9CFD CE01 FFFF F3CE 00FF 012C"
	$"81CE 81CE E2CE 02FF CECE FE9C 04CE FFCE CEFF FECE 079C 9CCE 9C9C CE9C CEFB 9C05"
	$"FFCE FFFF CECE FE9C FECE 029C 9CCE FB9C 03FF CEFF FFFE CE08 9C9C CE9C CE9C 9CCE"
	$"CEFA 9C01 CE9C FECE FE9C 00CE FEFF FECE 039C CE9C CEFE 9C01 CECE FE9C 16CE 9CCE"
	$"FFCE FFCE FFFF 9C9C CE9C 9CCE 9C9C CE9C 9CCE CE9C FECE 00FF FECE 009C FDCE 019C"
	$"9CF3 CE00 FF81 CF81 CFE2 CF00 FFFB CFFD FFF0 CFFC FFF1 CF01 FFCF FEFF 03CF CF9A"
	$"9AEA CFFD FFF0 CF01 FFCF FDFF 039A CFCF 9AF4 CF01 FFFF EACF 00FF 81CE 81CE E2CE"
	$"FDFF 00CE FEFF 02CE CEFF FDCE 049C CEFF CECE FEFF 04CE FFCE FFCE FDFF FDCE 019C"
	$"CEFC FF00 CEFE FF05 CEFF FFCE FFFF FECE 039C CE9C CEFB FF02 CEFF CEFB FF03 CECE"
	$"FFCE FCFF FACE FCFF 02CE FFCE FDFF 00CE FDFF 099C CECE 9CCE FFCE FFFF CEFC FF02"
	$"CECE FFFA CE01 FFFF F2CE 00FF 0155 81CE 81CE E2CE 0AFF 9C9C CE9C 9CCE FFCE CEFF"
	$"FDCE 109C 9CCE 9C9C FF9C CE9C 9CCE 9CCE FFCE FFFF FCCE FD9C 02CE 9C9C FECE 059C"
	$"CEFF FFCE FFFE CEFE 9C0D CE9C 9CCE 9C9C CECE 9CCE 9CCE 9CCE FD9C 03CE CE9C CEFE"
	$"FFFE CEFE 9C03 CE9C CECE FD9C 07CE 9CCE 9CFF CECE FFFE CE06 9CCE 9CCE 9C9C CEFC"
	$"9C01 CE9C FEFF FDCE FD9C 01CE 9CF3 CE00 FF81 CF81 CFE2 CF00 FFFB CF04 FFCF CFFF"
	$"FFFE CF01 9A9A FECF 00FF FACF 03FF CFFF FFFD CF00 9AF5 CF09 FFFF CFFF CFFF CFCF"
	$"9A9A EBCF FEFF 03CF FFCF 9AF3 CF08 FFFF CFFF CFFF CF9A 9AF5 CFFE FF05 CFFF CFCF"
	$"9A9A EFCF 00FF 81CE 81CE E2CE FEFF 02CE FFCE FCFF FDCE 019C 9CFA FF07 CEFF CEFF"
	$"FFCE FFFF FDCE 009C FDCE FEFF 03CE FFFF CEFE FF01 CEFF FDCE 0A9C 9CFF FFCE FFCE"
	$"FFCE FFCE FBFF 00CE FDFF 00CE FDFF 05CE FFCE 9CCE CEFE FF07 CECE FFFF CEFF FFCE"
	$"FEFF 01CE FFFE CE02 9C9C CEFD FF05 CEFF CEFF FFCE FDFF FDCE 059C 9CCE CEFF FFF3"
	$"CE00 FF01 1D81 CE81 CEE2 CE00 FFFD 9C01 CE9C FDFF FDCE FE9C 00FF FC9C 00CE FE9C"
	$"FDFF 01CE CEF2 9C03 FFFF CEFF FECE 019C CEFE 9C06 CE9C CE9C CE9C CEFE 9C03 CE9C"
	$"9CCE FB9C FEFF FECE 049C CECE 9CCE FC9C 00CE FE9C 0BCE FFCE FFCE FFFF CE9C CE9C"
	$"CEFC 9C00 CEFE 9C00 CEFE FFFA CEFE 9CF3 CE00 FF81 CF81 CFE2 CF00 FFFB CFFD FFFD"
	$"CF03 9ACF CFFF F8CF FDFF FDCF 009A F5CF 03FF FFCF FFFC CF00 9AF4 CF00 FFF9 CFFD"
	$"FFF0 CF01 FFCF FDFF 01CF 9AF4 CFFE FF02 CFCF FFEC CF00 FF81 CE81 CEE2 CEFD FF00"
	$"CEFB FFFD CE04 9CCE FFFF CEFE FF00 CEF8 FFFE CE01 9CCE FEFF 02CE FFCE FDFF 00CE"
	$"FCFF FDCE 009C FEFF 02CE FFCE FDFF 09CE FFCE FFCE FFCE CEFF CEFC FFFB CE01 FFCE"
	$"FEFF 01CE CEFB FF00 CEFD FFFD CE05 FFCE FFCE FFCE F9FF 01CE FFFC CE01 FFFF F2CE"
	$"00FF 014B 81CE 81CE E2CE 03FF 9C9C CEFE 9C04 FFCE CEFF CEFE 9C08 CE9C CE63 9C9C"
	$"CE9C CEFD 9C07 FFCE FFFF CE9C CECE FD9C 03CE 9C9C CEFC 9C0F FFFF CEFF CECE 9CCE"
	$"9CCE CE9C 9CCE 9CCE FE9C 05CE 9C9C CE9C 9CFE CE03 9C9C CE9C FEFF 01CE CEFE 9C04"
	$"CECE 9CCE 9CFE CE04 9C9C CE9C 9CFD FF10 CE9C CECE 9CCE 9C9C CE9C CECE 9CCE CE9C"
	$"9CFE FFFD CEFE 9CF0 CE00 FF81 CF81 CFE2 CF00 FFFB CF09 FFCF CFFF FFCF 9ACF CF9A"
	$"F6CF 03FF CFFF FFFD CF01 9A9A F6CF FDFF F8CF 00FF EFCF FEFF FECF 009A F3CF FDFF"
	$"FDCF 029A 9AFF F5CF 00FF FBCF 009A F0CF 00FF 81CE 81CE E2CE 02FF CECE FEFF 0ACE"
	$"FFCE CEFF CECE 9CCE CE9C FEFF 0BCE FFCE FFFF CECE FFFF CEFF FFFD CE03 9C9C CECE"
	$"FDFF FECE FDFF 01CE FFFB CE04 FFCE FFFF CEFE FF03 CEFF FFCE F9FF 01CE CEFE FFFE"
	$"CE02 9CCE CEFE FF00 CEFE FF03 CEFF CECE FCFF FDCE 019C CEFE FF02 CEFF FFFD CE03"
	$"FFCE CEFF FBCE 039C CEFF FFF3 CE00 FF01 4A81 CE81 CEE2 CE02 FFCE CEFD 9C07 FFCE"
	$"CEFF CECE 9CCE FE9C 00CE FE9C 02CE 9CCE FE9C 04FF CEFF FFCE FE9C 00CE FE9C 00CE"
	$"FE9C 00CE FD9C 03FF FFCE FFFD CE0A 9CCE CE9C 9CCE 9C9C CECE 9CFE CE09 9C9C CE9C"
	$"CECE 9CCE 9C9C FEFF FECE FA9C 04CE 9CCE 9CCE FE9C 03FF CEFF FFFE CEFE 9C05 CE9C"
	$"CE9C CEFF FC9C FDFF 01CE CEFB 9CF2 CE00 FF81 CF81 CFE2 CF00 FFFB CF03 FFCF CFFF"
	$"FDCF 019A 9AF6 CF03 FFCF FFFF FDCF 009A F5CF 05FF FFCF FFCF FFF5 CF03 FFCF CFFF"
	$"F7CF FEFF FECF 029A CF9A F5CF 08FF CFFF FFCF FFCF 9A9A FBCF 00FF FCCF FDFF FCCF"
	$"009A F0CF 00FF 81CE 81CE E2CE FCFF 05CE FFFF CECE FFFD CE01 9C9C FEFF 05CE FFCE"
	$"FFFF CEFE FF02 CEFF FFFD CE00 9CFD CE02 FFFF CEFE FF07 CEFF FFCE FFFF CEFF FECE"
	$"029C FFCE FCFF 00CE FDFF 02CE FFCE FEFF 00CE FBFF FECE 0D9C CECE FFFF CEFF CEFF"
	$"CEFF FFCE CEFC FFFE CE02 9C9C CEFA FFFE CEFC FFFA CE00 FFF2 CE00 FF01 3D81 CE81"
	$"CEE2 CE06 FF9C 9CCE 9CCE 9CFD FFFE CEFE 9C03 CE9C 9CCE FE9C 06CE 9CCE CEFF CEFF"
	$"FCCE 009C FECE 049C CECE 9CCE FE9C 00CE FEFF FDCE 119C CE9C CE9C CE9C CE9C CE9C"
	$"CECE 9C9C CECE 9CFE CE03 9CCE 9C9C FEFF 05CE 9CCE 9CCE 9CFE CE00 FFFE 9C07 CE9C"
	$"CECE 9CFF CEFF FBCE 069C FF9C 9CCE 9C9C FDCE 009C FEFF 02CE FF9C FECE 039C 9CCE"
	$"9CF3 CE00 FF81 CF81 CFE2 CFFE FFFD CFFC FFFE CF00 9AFA CF00 FFFD CF03 FFCF FFFF"
	$"F5CF 00FF FDCF FDFF FCCF 019A FFF8 CF00 FFFC CF01 FFFF FDCF FDFF FECF 009A FDCF"
	$"00FF F9CF 03FF CFFF FFFC CF01 9AFF F7CF FCFF EBCF 00FF 81CE 81CE E2CE FEFF 00CE"
	$"FAFF FDCE 079C CECE FFCE FFFF CEFD FF03 CEFF CEFF FCCE 019C CEFA FF01 CECE FBFF"
	$"FCCE 009C FCFF 00CE FDFF 00CE F7FF 00CE FEFF FBCE 01FF CEFB FF07 CEFF FFCE FFCE"
	$"FFFF FBCE 02FF FFCE FAFF 00CE FEFF 01CE FFFC CE01 FFFF F2CE 00FF 02B0 EECE FD00"
	$"ECCE 0000 FDCE 0100 00F7 CE00 00FD CE00 00FC CE01 0000 FECE FE00 01CE CEFE 00FD"
	$"CE00 00FC CEFE 0001 CECE FE00 01CE CEFE 0003 CECE 0000 FCCE FE00 F7CE 0100 00FD"
	$"CE00 00F8 CEFE 00FC CE01 0000 F9CE 0000 F0CE FE00 EFCE 0000 FECE FD00 01CE CEFC"
	$"0002 CECE 00FD CE01 0000 F6CE FC00 E7CE 0100 00F1 CE00 FFFB 9C03 FFCE CEFF FECE"
	$"029C 9CCE FB9C 00CE FD9C FEFF FECE 1E9C 9CCE CE9C 9CFF 9CCE 9CCE 9C9C CE9C FFCE"
	$"FFFF CEFF CE9C CE9C 9CCE 9CCE 9C9C FECE FE9C 01FF 9CFD CE03 9CCE 9C9C FEFF 04CE"
	$"CE9C 9CCE FE9C 06FF 9C9C CE9C 9CCE FE9C FEFF 14CE FFCE 9CCE CE9C 9CCE 9C9C CE9C"
	$"9CCE CE9C 9CCE CEFF FDCE 059C 9CCE 9CCE 9CF3 CE00 FFEE CFFD 00EC CF00 00FD CF01"
	$"0000 F7CF 0000 FDCF 0000 FCCF 0100 00FE CFFE 0001 CFCF FE00 FDCF 0000 FCCF FE00"
	$"01CF CFFE 0001 CFCF FE00 03CF CF00 00FC CFFE 00F7 CF01 0000 FDCF 0000 F8CF FE00"
	$"FCCF 0100 00F9 CF00 00F0 CFFE 00EF CF00 00FE CFFD 0001 CFCF FC00 02CF CF00 FDCF"
	$"0100 00F6 CFFC 00E7 CF01 0000 F1CF 00FF FBCF 03FF CFCF FFF0 CFFE FFFE CF01 9A9A"
	$"FDCF 00FF F9CF 05FF CFFF FFCF FFF5 CF00 FFFE CF03 FFCF CFFF FBCF FEFF 0BCF CF9A"
	$"CF9A 9ACF CFFF CFCF FFFB CFFE FF06 CFFF FF9A 9ACF 9AF4 CF02 FFCF FFEB CF00 FFEE"
	$"CEFD 00EC CE00 00FD CE01 0000 F7CE 0000 FDCE 0000 FCCE 0100 00FE CEFE 0001 CECE"
	$"FE00 FDCE 0000 FCCE FE00 01CE CEFE 0001 CECE FE00 03CE CE00 00FC CEFE 00F7 CE01"
	$"0000 FDCE 0000 F8CE FE00 FCCE 0100 00F9 CE00 00F0 CEFE 00EF CE00 00FE CEFD 0001"
	$"CECE FC00 02CE CE00 FDCE 0100 00F6 CEFC 00E7 CE01 0000 F1CE FDFF 02CE FFCE FDFF"
	$"FDCE 019C CEFB FF03 CEFF CECE FCFF 01CE CEFE 9C00 CEFC FF00 CEFE FF07 CEFF FFCE"
	$"FFFF CEFF FECE 069C CEFF CEFF FFCE F7FF 05CE CEFF CEFF CEFD FF05 CE9C CECE 9CCE"
	$"FEFF 06CE FFFF CEFF FFCE FDFF 09CE FFCE 9CCE 9C9C FFFF CEFE FF00 CEFD FF03 CECE"
	$"FFFF FBCE 02FF CEFF F3CE 00FF 02E6 EFCE 0000 FECE 0000 E6CE 0000 FDCE 0000 FDCE"
	$"0000 FBCE 0000 FCCE 0300 CECE 00FE CE01 0000 FECE 0400 CECE 0000 FDCE 0000 FECE"
	$"0100 00FE CE01 0000 FECE 0300 CECE 00FD CE03 00CE CE00 F6CE 0000 F8CE 0000 FECE"
	$"0300 CECE 00FB CE00 00FC CE00 00EC CE00 00EE CE00 00FD CE02 00CE 00FE CE00 00FE"
	$"CE00 00FA CE00 00FD CE00 00FA CE00 00FE CE00 00E7 CE00 00F1 CE00 FFFB 9C03 FFCE"
	$"CEFF FECE 029C 9CCE FB9C 00CE FD9C FEFF FECE 1E9C 9CCE CE9C 9CFF 9CCE 9CCE 9C9C"
	$"CE9C FFCE FFFF CEFF CE9C CE9C 9CCE 9CCE 9C9C FECE FE9C 01FF 9CFD CE03 9CCE 9C9C"
	$"FEFF 04CE CE9C 9CCE FE9C 06FF 9C9C CE9C 9CCE FE9C FEFF 14CE FFCE 9CCE CE9C 9CCE"
	$"9C9C CE9C 9CCE CE9C 9CCE CEFF FDCE 059C 9CCE 9CCE 9CF3 CE00 FFEF CF00 00FE CF00"
	$"00E6 CF00 00FD CF00 00FD CF00 00FB CF00 00FC CF03 00CF CF00 FECF 0100 00FE CF04"
	$"00CF CF00 00FD CF00 00FE CF01 0000 FECF 0100 00FE CF03 00CF CF00 FDCF 0300 CFCF"
	$"00F6 CF00 00F8 CF00 00FE CF03 00CF CF00 FBCF 0000 FCCF 0000 ECCF 0000 EECF 0000"
	$"FDCF 0200 CF00 FECF 0000 FECF 0000 FACF 0000 FDCF 0000 FACF 0000 FECF 0000 E7CF"
	$"0000 F1CF 00FF FBCF 03FF CFCF FFF0 CFFE FFFE CF01 9A9A FDCF 00FF F9CF 05FF CFFF"
	$"FFCF FFF5 CF00 FFFE CF03 FFCF CFFF FBCF FEFF 0BCF CF9A CF9A 9ACF CFFF CFCF FFFB"
	$"CFFE FF06 CFFF FF9A 9ACF 9AF4 CF02 FFCF FFEB CF00 FFEF CE00 00FE CE00 00E6 CE00"
	$"00FD CE00 00FD CE00 00FB CE00 00FC CE03 00CE CE00 FECE 0100 00FE CE04 00CE CE00"
	$"00FD CE00 00FE CE01 0000 FECE 0100 00FE CE03 00CE CE00 FDCE 0300 CECE 00F6 CE00"
	$"00F8 CE00 00FE CE03 00CE CE00 FBCE 0000 FCCE 0000 ECCE 0000 EECE 0000 FDCE 0200"
	$"CE00 FECE 0000 FECE 0000 FACE 0000 FDCE 0000 FACE 0000 FECE 0000 E7CE 0000 F1CE"
	$"FDFF 02CE FFCE FDFF FDCE 019C CEFB FF03 CEFF CECE FCFF 01CE CEFE 9C00 CEFC FF00"
	$"CEFE FF07 CEFF FFCE FFFF CEFF FECE 069C CEFF CEFF FFCE F7FF 05CE CEFF CEFF CEFD"
	$"FF05 CE9C CECE 9CCE FEFF 06CE FFFF CEFF FFCE FDFF 09CE FFCE 9CCE 9C9C FFFF CEFE"
	$"FF00 CEFD FF03 CECE FFFF FBCE 02FF CEFF F3CE 00FF 02BD F0CE 0000 E1CE 0000 FDCE"
	$"0000 FECE 0000 F9CE 0000 FDCE 0300 CECE 00FE CE01 0000 FECE 0400 CE00 CE00 F9CE"
	$"0100 00FE CE01 0000 FECE 0300 CECE 00FD CE00 00F3 CE00 00F8 CE00 00FE CE00 00F8"
	$"CE00 00FC CE00 00EC CE00 00EF CE02 00CE 00FE CE02 00CE 00FE CE00 00FE CE00 00FA"
	$"CE00 00FD CE00 00FA CE00 00FE CE00 00E7 CE00 00F1 CE00 FFFD CE01 9C9C FEFF FECE"
	$"079C CE9C CE9C CECE 9CFD CE08 9CCE 9CFF CEFF FFCE 9CFE CE00 9CFE CE02 9CCE 9CFE"
	$"CE12 9CCE FFFF CEFF 9CFF CE9C CE9C CE9C 9CCE 9C9C CEFE 9C0B CE9C 9CCE FF9C CECE"
	$"9CCE 9CCE FEFF FDCE 059C CE9C CECE 9CFD CE0A 9C9C CE9C FFCE FFFF 9CFF 9CFE CE06"
	$"9CCE CE9C CE9C CEFD 9CFE FF01 CEFF FBCE 009C F2CE 00FF F0CF 0000 E1CF 0000 FDCF"
	$"0000 FECF 0000 F9CF 0000 FDCF 0300 CFCF 00FE CF01 0000 FECF 0400 CF00 CF00 F9CF"
	$"0100 00FE CF01 0000 FECF 0300 CFCF 00FD CF00 00F3 CF00 00F8 CF00 00FE CF00 00F8"
	$"CF00 00FC CF00 00EC CF00 00EF CF02 00CF 00FE CF02 00CF 00FE CF00 00FE CF00 00FA"
	$"CF00 00FD CF00 00FA CF00 00FE CF00 00E7 CF00 00F1 CF03 FFCF CFFF FECF FDFF FECF"
	$"019A 9AFA CF00 FFFD CFFD FFFD CF00 9AFD CF00 FFFA CF09 FFFF CFFF CFFF CF9A CF9A"
	$"F3CF 03FF CFFF FFFD CFFE FFFD CF00 9AFB CF00 FFFB CF05 FFCF FFFF CFFF FBCF 00FF"
	$"F9CF FEFF 01CF FFFC CF00 FFF1 CF00 FFF0 CE00 00E1 CE00 00FD CE00 00FE CE00 00F9"
	$"CE00 00FD CE03 00CE CE00 FECE 0100 00FE CE04 00CE 00CE 00F9 CE01 0000 FECE 0100"
	$"00FE CE03 00CE CE00 FDCE 0000 F3CE 0000 F8CE 0000 FECE 0000 F8CE 0000 FCCE 0000"
	$"ECCE 0000 EFCE 0200 CE00 FECE 0200 CE00 FECE 0000 FECE 0000 FACE 0000 FDCE 0000"
	$"FACE 0000 FECE 0000 E7CE 0000 F1CE FDFF 02CE FFCE FDFF FDCE 029C CECE F9FF 00CE"
	$"FCFF FDCE 009C FECE FAFF 0BCE FFFF CEFF FFCE FFCE 9CCE 9CF9 FF00 CEFD FF00 CEFA"
	$"FF00 CEFE FFFD CE02 9CCE CEFD FF00 CEFE FF01 CECE FCFF 01CE FFFD CEFC FF05 CEFF"
	$"CECE FFCE FEFF 01CE FFFC CE02 FFCE FFF3 CE00 FF03 ACF0 CE00 00FA CEFE 0001 CECE"
	$"FD00 03CE 0000 CEFD 0003 CE00 00CE FC00 00CE FC00 02CE CE00 FECE FE00 02CE CE00"
	$"FDCE 0300 CECE 00FE CE01 0000 FECE 0400 CE00 CE00 F9CE 0100 00FE CE01 0000 FECE"
	$"0300 CECE 00FC CE00 00FE CEFE 0000 CEFD 0003 CECE 00CE FD00 00CE FC00 FECE 0000"
	$"FECE FE00 04CE CE00 00CE FA00 00CE FE00 08CE 0000 CE00 00CE 0000 FCCE 0700 CE00"
	$"00CE 0000 CEFE 00F9 CE02 00CE 00FE CE02 00CE 00FE CEFD 0003 CE00 00CE FC00 00CE"
	$"FC00 02CE 0000 FECE FD00 11CE CE00 00CE CE00 00CE 0000 CE00 00CE 0000 CEFC 0001"
	$"CECE FE00 F1CE 00FF FD9C 05CE 9CFF CECE FFFE CE07 9CCE 9CCE 9CCE 9CCE FB9C 04FF"
	$"CEFF FF9C FECE FC9C 02CE 9CCE FE9C 07CE 9CFF FFCE FFCE CEFE 9C08 CE9C CECE 9CCE"
	$"CE9C CEFE 9C00 CEFE 9C00 CEFD 9C01 CE9C FEFF FECE 039C CE9C CEFE 9C00 CEFD 9C13"
	$"CE9C CEFF CEFF FFCE 9CCE 9C9C CECE 9C9C CE9C CE9C FECE 009C FEFF 01CE CEFB 9C01"
	$"CE9C F3CE 00FF F0CF 0000 FACF FE00 01CF CFFD 0003 CF00 00CF FD00 03CF 0000 CFFC"
	$"0000 CFFC 0002 CFCF 00FE CFFE 0002 CFCF 00FD CF03 00CF CF00 FECF 0100 00FE CF04"
	$"00CF 00CF 00F9 CF01 0000 FECF 0100 00FE CF03 00CF CF00 FCCF 0000 FECF FE00 00CF"
	$"FD00 03CF CF00 CFFD 0000 CFFC 00FE CF00 00FE CFFE 0004 CFCF 0000 CFFA 0000 CFFE"
	$"0008 CF00 00CF 0000 CF00 00FC CF07 00CF 0000 CF00 00CF FE00 F9CF 0200 CF00 FECF"
	$"0200 CF00 FECF FD00 03CF 0000 CFFC 0000 CFFC 0002 CF00 00FE CFFD 0011 CFCF 0000"
	$"CFCF 0000 CF00 00CF 0000 CF00 00CF FC00 01CF CFFE 00F1 CF00 FFFB CF03 FFCF CFFF"
	$"F9CF 00FF F9CF 03FF CFFF FFFD CF01 9A9A F6CF 03FF FFCF FFFC CF00 9AFE CF02 FFCF"
	$"FFF1 CFFE FFEF CF03 FFCF FFFF FECF 019A 9AF5 CFFE FFFE CFFE 9AEF CF00 FFF0 CE00"
	$"00FA CEFE 0001 CECE FD00 03CE 0000 CEFD 0003 CE00 00CE FC00 00CE FC00 02CE CE00"
	$"FECE FE00 02CE CE00 FDCE 0300 CECE 00FE CE01 0000 FECE 0400 CE00 CE00 F9CE 0100"
	$"00FE CE01 0000 FECE 0300 CECE 00FC CE00 00FE CEFE 0000 CEFD 0003 CECE 00CE FD00"
	$"00CE FC00 FECE 0000 FECE FE00 04CE CE00 00CE FA00 00CE FE00 08CE 0000 CE00 00CE"
	$"0000 FCCE 0700 CE00 00CE 0000 CEFE 00F9 CE02 00CE 00FE CE02 00CE 00FE CEFD 0003"
	$"CE00 00CE FC00 00CE FC00 02CE 0000 FECE FD00 11CE CE00 00CE CE00 00CE 0000 CE00"
	$"00CE 0000 CEFC 0001 CECE FE00 F1CE 00FF FECE 07FF CEFF FFCE CEFF FFFC CEFE FF0B"
	$"CEFF FFCE FFCE FFCE FFCE FFFF FCCE 069C FFFF CEFF CEFF FECE 05FF CEFF FFCE FFFC"
	$"CE02 9CFF CEFD FF03 CEFF FFCE FBFF FECE 01FF CEFC FFFC CEFE FF00 CEFD FF00 CEFD"
	$"FF02 CEFF FFFE CE02 9C9C CEFE FF00 CEFC FF00 CEFD FFFE CEFE 9C00 CEFE FFF3 CE00"
	$"FF03 F7F0 CE00 00FB CE00 00FE CE1B 00CE CE00 CECE 00CE 00CE CE00 CE00 00CE 0000"
	$"CECE 00CE 0000 CE00 CE00 FECE 0900 CECE 00CE CE00 CECE 00FD CE00 00FE CEFD 0000"
	$"CEFC 0002 CECE 00FA CE02 00CE 00FE CE01 0000 FECE 0300 CECE 00FB CE02 00CE 00FE"
	$"CE0D 00CE 00CE CE00 CE00 00CE 00CE 0000 FECE 0000 FCCE 0200 CE00 FECE 0C00 CE00"
	$"00CE CE00 CE00 CECE 0000 FECE 0600 CE00 00CE 0000 FACE 0A00 CECE 0000 CE00 00CE"
	$"CE00 F9CE 0200 CE00 FECE 0200 CE00 FECE 0200 CE00 FECE 0D00 00CE CE00 CE00 00CE"
	$"00CE 00CE 00FC CE1F 00CE 00CE CE00 CECE 0000 CECE 00CE CE00 CE00 00CE 00CE CE00"
	$"00CE CE00 00CE CE00 F1CE 00FF FECE 0C9C CE9C FFCE CEFF CECE 9CCE CE9C FECE 039C"
	$"CECE 9CFD CE01 FFCE FEFF 00CE FD9C FECE 009C FACE 03FF FFCE FFFD CE00 9CFD CE08"
	$"9CCE 9CCE CE9C CE9C CEFD 9CFE CE02 9CCE 9CFE FFFD CE0A 9C9C CECE 9C9C CE9C 9CCE"
	$"9CFE CE03 FFCE FFFF FECE 019C 9CFE CEFE 9C01 CE9C FECE 009C FEFF FECE FD9C F0CE"
	$"00FF F0CF 0000 FBCF 0000 FECF 1B00 CFCF 00CF CF00 CF00 CFCF 00CF 0000 CF00 00CF"
	$"CF00 CF00 00CF 00CF 00FE CF09 00CF CF00 CFCF 00CF CF00 FDCF 0000 FECF FD00 00CF"
	$"FC00 02CF CF00 FACF 0200 CF00 FECF 0100 00FE CF03 00CF CF00 FBCF 0200 CF00 FECF"
	$"0D00 CF00 CFCF 00CF 0000 CF00 CF00 00FE CF00 00FC CF02 00CF 00FE CF0C 00CF 0000"
	$"CFCF 00CF 00CF CF00 00FE CF06 00CF 0000 CF00 00FA CF0A 00CF CF00 00CF 0000 CFCF"
	$"00F9 CF02 00CF 00FE CF02 00CF 00FE CF02 00CF 00FE CF0D 0000 CFCF 00CF 0000 CF00"
	$"CF00 CF00 FCCF 1F00 CF00 CFCF 00CF CF00 00CF CF00 CFCF 00CF 0000 CF00 CFCF 0000"
	$"CFCF 0000 CFCF 00F1 CF03 FFCF CFFF FECF 16FF CFCF FFCF CF9A CF9A CFCF FFFF CFFF"
	$"FFCF CFFF CFFF FFCF FEFF FECF 019A CFFE FF06 CFCF FFCF FFFF CFFE FF01 CFFF FCCF"
	$"059A FFCF FFCF FFFD CF00 FFF9 CF00 FFFE CFFE FFFE CF00 9AFD CF0D FFCF FFCF CFFF"
	$"CFFF CFFF FFCF FFFF FECF 059A CF9A FFCF FFF9 CFFE FFFD CFFE 9AF0 CF00 FFF0 CE00"
	$"00FB CE00 00FE CE1B 00CE CE00 CECE 00CE 00CE CE00 CE00 00CE 0000 CECE 00CE 0000"
	$"CE00 CE00 FECE 0900 CECE 00CE CE00 CECE 00FD CE00 00FE CEFD 0000 CEFC 0002 CECE"
	$"00FA CE02 00CE 00FE CE01 0000 FECE 0300 CECE 00FB CE02 00CE 00FE CE0D 00CE 00CE"
	$"CE00 CE00 00CE 00CE 0000 FECE 0000 FCCE 0200 CE00 FECE 0C00 CE00 00CE CE00 CE00"
	$"CECE 0000 FECE 0600 CE00 00CE 0000 FACE 0A00 CECE 0000 CE00 00CE CE00 F9CE 0200"
	$"CE00 FECE 0200 CE00 FECE 0200 CE00 FECE 0D00 00CE CE00 CE00 00CE 00CE 00CE 00FC"
	$"CE1F 00CE 00CE CE00 CECE 0000 CECE 00CE CE00 CE00 00CE 00CE CE00 00CE CE00 00CE"
	$"CE00 F1CE FCFF 0BCE FFFF CECE FFCE CE9C CE9C CEFE FF00 CEF5 FFFE CE01 9CCE FEFF"
	$"00CE F8FF 01CE FFFC CE00 9CF7 FF00 CEF3 FFFB CEFE FF00 CEFB FF05 CEFF FFCE FFFF"
	$"FECE 029C 9CCE FDFF 00CE FDFF 00CE FDFF FDCE FE9C FEFF F3CE 00FF 03D6 F0CE 0000"
	$"FBCE 0000 FECE 1B00 CECE 00CE CE00 CE00 CE00 CECE 00CE CE00 CE00 00CE CE00 CECE"
	$"00CE 00FE CE03 00CE CE00 FCCE 0000 FDCE 0000 FCCE 0000 FDCE 0100 CEF9 0005 CECE"
	$"00CE CE00 FECE 0100 00FE CE03 00CE CE00 FACE 0100 00FE CE11 00CE 00CE CE00 CE00"
	$"CECE 00CE 00CE 0000 CE00 FBCE 0100 00FE CE0C 00CE 0000 CECE 00CE 00CE CE00 00FE"
	$"CE08 00CE 00CE CE00 CE00 00FC CE07 00CE CE00 CECE 0000 F7CE FC00 04CE CE00 CE00"
	$"FECE 1500 CECE 00CE CE00 CE00 00CE CE00 CECE 00CE 00CE CE00 00FE CE04 00CE CE00"
	$"CEFD 0000 CEFB 0007 CE00 CECE 00CE 00CE FC00 02CE CE00 F1CE 00FF FECE 0C9C CE9C"
	$"FFCE CEFF CECE 9CCE CE9C FECE 039C CECE 9CFD CE01 FFCE FEFF 00CE FD9C FECE 009C"
	$"FACE 03FF FFCE FFFD CE00 9CFD CE08 9CCE 9CCE CE9C CE9C CEFD 9CFE CE02 9CCE 9CFE"
	$"FFFD CE0A 9C9C CECE 9C9C CE9C 9CCE 9CFE CE03 FFCE FFFF FECE 019C 9CFE CEFE 9C01"
	$"CE9C FECE 009C FEFF FECE FD9C F0CE 00FF F0CF 0000 FBCF 0000 FECF 1B00 CFCF 00CF"
	$"CF00 CF00 CF00 CFCF 00CF CF00 CF00 00CF CF00 CFCF 00CF 00FE CF03 00CF CF00 FCCF"
	$"0000 FDCF 0000 FCCF 0000 FDCF 0100 CFF9 0005 CFCF 00CF CF00 FECF 0100 00FE CF03"
	$"00CF CF00 FACF 0100 00FE CF11 00CF 00CF CF00 CF00 CFCF 00CF 00CF 0000 CF00 FBCF"
	$"0100 00FE CF0C 00CF 0000 CFCF 00CF 00CF CF00 00FE CF08 00CF 00CF CF00 CF00 00FC"
	$"CF07 00CF CF00 CFCF 0000 F7CF FC00 04CF CF00 CF00 FECF 1500 CFCF 00CF CF00 CF00"
	$"00CF CF00 CFCF 00CF 00CF CF00 00FE CF04 00CF CF00 CFFD 0000 CFFB 0007 CF00 CFCF"
	$"00CF 00CF FC00 02CF CF00 F1CF 03FF CFCF FFFE CF16 FFCF CFFF CFCF 9ACF 9ACF CFFF"
	$"FFCF FFFF CFCF FFCF FFFF CFFE FFFE CF01 9ACF FEFF 06CF CFFF CFFF FFCF FEFF 01CF"
	$"FFFC CF05 9AFF CFFF CFFF FDCF 00FF F9CF 00FF FECF FEFF FECF 009A FDCF 0DFF CFFF"
	$"CFCF FFCF FFCF FFFF CFFF FFFE CF05 9ACF 9AFF CFFF F9CF FEFF FDCF FE9A F0CF 00FF"
	$"F0CE 0000 FBCE 0000 FECE 1B00 CECE 00CE CE00 CE00 CE00 CECE 00CE CE00 CE00 00CE"
	$"CE00 CECE 00CE 00FE CE03 00CE CE00 FCCE 0000 FDCE 0000 FCCE 0000 FDCE 0100 CEF9"
	$"0005 CECE 00CE CE00 FECE 0100 00FE CE03 00CE CE00 FACE 0100 00FE CE11 00CE 00CE"
	$"CE00 CE00 CECE 00CE 00CE 0000 CE00 FBCE 0100 00FE CE0C 00CE 0000 CECE 00CE 00CE"
	$"CE00 00FE CE08 00CE 00CE CE00 CE00 00FC CE07 00CE CE00 CECE 0000 F7CE FC00 04CE"
	$"CE00 CE00 FECE 1500 CECE 00CE CE00 CE00 00CE CE00 CECE 00CE 00CE CE00 00FE CE04"
	$"00CE CE00 CEFD 0000 CEFB 0007 CE00 CECE 00CE 00CE FC00 02CE CE00 F1CE FCFF 0BCE"
	$"FFFF CECE FFCE CE9C CE9C CEFE FF00 CEF5 FFFE CE01 9CCE FEFF 00CE F8FF 01CE FFFC"
	$"CE00 9CF7 FF00 CEF3 FFFB CEFE FF00 CEFB FF05 CEFF FFCE FFFF FECE 029C 9CCE FDFF"
	$"00CE FDFF 00CE FDFF FDCE FE9C FEFF F3CE 00FF 03F7 EFCE 0000 FECE 0200 CE00 FECE"
	$"1100 CECE 00CE CE00 CECE 0000 CECE 00CE CE00 00FD CE05 00CE CE00 CE00 FECE 0300"
	$"CECE 00FC CE00 00FD CE00 00FD CE01 0000 FECE 0100 00FD CE00 00FC CE04 00CE CE00"
	$"00FE CE01 0000 FECE 0300 CECE 00FD CE04 00CE CE00 00FE CE11 00CE 00CE CE00 CE00"
	$"CECE 00CE 00CE CE00 CE00 FECE 0400 CECE 0000 FECE 0C00 CE00 00CE CE00 CE00 CECE"
	$"0000 FECE 0800 CE00 CECE 00CE CE00 FCCE 0700 CECE 00CE CE00 00F7 CE00 00FE CE05"
	$"00CE CE00 CE00 FECE 0700 CECE 00CE CE00 00FD CE05 00CE CE00 CE00 FECE 0000 FECE"
	$"0500 CECE 00CE 00FC CE01 0000 FDCE 0000 FECE 0300 CECE 00FE CE03 00CE CE00 F1CE"
	$"00FF FDCE 0B9C CEFF CECE FFCE CE9C CE9C 9CFE CE00 9CFE CE02 9CCE 9CFE CE00 FFFE"
	$"CEFD 9CFE CE00 9CFE CE07 9C9C CE9C FFCE CEFF FCCE 009C FCCE FD9C 06CE 9C9C CE9C"
	$"CE9C FDCE 019C CEFE FFFE CE02 9CCE 9CFE CE03 9CCE CE9C FCCE 03FF CEFF FFFE CE01"
	$"9C9C FDCE 039C CECE 9CFB CE01 FF9C FECE FE9C F0CE 00FF EFCF 0000 FECF 0200 CF00"
	$"FECF 1100 CFCF 00CF CF00 CFCF 0000 CFCF 00CF CF00 00FD CF05 00CF CF00 CF00 FECF"
	$"0300 CFCF 00FC CF00 00FD CF00 00FD CF01 0000 FECF 0100 00FD CF00 00FC CF04 00CF"
	$"CF00 00FE CF01 0000 FECF 0300 CFCF 00FD CF04 00CF CF00 00FE CF11 00CF 00CF CF00"
	$"CF00 CFCF 00CF 00CF CF00 CF00 FECF 0400 CFCF 0000 FECF 0C00 CF00 00CF CF00 CF00"
	$"CFCF 0000 FECF 0800 CF00 CFCF 00CF CF00 FCCF 0700 CFCF 00CF CF00 00F7 CF00 00FE"
	$"CF05 00CF CF00 CF00 FECF 0700 CFCF 00CF CF00 00FD CF05 00CF CF00 CF00 FECF 0000"
	$"FECF 0500 CFCF 00CF 00FC CF01 0000 FDCF 0000 FECF 0300 CFCF 00FE CF03 00CF CF00"
	$"F1CF 0AFF CFCF FFCF FFCF FFCF CFFF FDCF 049A CFCF FFFF FECF 00FF FBCF 00FF FCCF"
	$"009A FCCF 01FF CFFE FF05 CFCF FFCF CFFF FCCF 049A CFCF FFFF F3CF 00FF FECF FEFF"
	$"F9CF 0DFF CFCF FFCF FFCF FFFF CFFF CFFF FFFD CF02 9ACF FFFE CF01 FFFF FDCF 06FF"
	$"CFCF FFCF CFFF FECF 019A FFF1 CF00 FFEF CE00 00FE CE02 00CE 00FE CE11 00CE CE00"
	$"CECE 00CE CE00 00CE CE00 CECE 0000 FDCE 0500 CECE 00CE 00FE CE03 00CE CE00 FCCE"
	$"0000 FDCE 0000 FDCE 0100 00FE CE01 0000 FDCE 0000 FCCE 0400 CECE 0000 FECE 0100"
	$"00FE CE03 00CE CE00 FDCE 0400 CECE 0000 FECE 1100 CE00 CECE 00CE 00CE CE00 CE00"
	$"CECE 00CE 00FE CE04 00CE CE00 00FE CE0C 00CE 0000 CECE 00CE 00CE CE00 00FE CE08"
	$"00CE 00CE CE00 CECE 00FC CE07 00CE CE00 CECE 0000 F7CE 0000 FECE 0500 CECE 00CE"
	$"00FE CE07 00CE CE00 CECE 0000 FDCE 0500 CECE 00CE 00FE CE00 00FE CE05 00CE CE00"
	$"CE00 FCCE 0100 00FD CE00 00FE CE03 00CE CE00 FECE 0300 CECE 00F1 CEF9 FF02 CECE"
	$"FFFD CE01 9CCE F6FF 02CE CEFF FCCE 019C CEF5 FF02 CECE FFFC CE00 9CFA FF05 CEFF"
	$"CEFF FFCE F5FF FBCE FEFF 00CE F8FF 02CE FFFF FDCE 019C CEFE FF00 CEF7 FF06 CECE"
	$"FF9C CECE 9CFE FFF3 CE00 FF03 82EE CEFE 00FE CEFE 00FE CEFE 00FE CE02 00CE CEF5"
	$"0000 CEFD 0002 CECE 00FE CEFE 0002 CECE 00FE CEFE 0002 CE00 00FE CE01 0000 FBCE"
	$"0000 FDCE FC00 00CE FE00 01CE CEFE 0001 CECE FE00 FECE FE00 01CE CEFE 0001 CECE"
	$"FE00 00CE FE00 00CE FC00 05CE CE00 00CE CEFE 0001 CECE FE00 00CE F200 00CE FE00"
	$"00CE FE00 04CE 0000 CECE FB00 00CE FC00 00CE FE00 01CE CEFE 0000 CEF9 0000 CEFE"
	$"0001 CECE F700 00CE FB00 FECE FE00 01CE CEFA 0001 CECE FB00 02CE CE00 FECE FE00"
	$"00CE FC00 F3CE 0AFF 9C9C CE9C CECE FFCE CEFF FECE 039C 9CCE CEFD 9C01 CE9C FDCE"
	$"03FF CEFF FFFD CE00 9CFE CE04 9C9C CECE 9CFD CE03 FFCE FFFF FECE 059C CE9C CE9C"
	$"CEFE 9C03 CECE 9C9C FECE 029C CE9C FECE 029C CECE FEFF FECE 069C CE9C CE9C CE9C"
	$"FCCE 059C CECE FFCE FFFE CE04 9CCE 9C9C CEFC 9C04 CE9C 9CCE CEFE FF09 CEFF CECE"
	$"9C9C CECE 9C9C F3CE 00FF EECF FE00 FECF FE00 FECF FE00 FECF 0200 CFCF F500 00CF"
	$"FD00 02CF CF00 FECF FE00 02CF CF00 FECF FE00 02CF 0000 FECF 0100 00FB CF00 00FD"
	$"CFFC 0000 CFFE 0001 CFCF FE00 01CF CFFE 00FE CFFE 0001 CFCF FE00 01CF CFFE 0000"
	$"CFFE 0000 CFFC 0005 CFCF 0000 CFCF FE00 01CF CFFE 0000 CFF2 0000 CFFE 0000 CFFE"
	$"0004 CF00 00CF CFFB 0000 CFFC 0000 CFFE 0001 CFCF FE00 00CF F900 00CF FE00 01CF"
	$"CFF7 0000 CFFB 00FE CFFE 0001 CFCF FA00 01CF CFFB 0002 CFCF 00FE CFFE 0000 CFFC"
	$"00F3 CF00 FFFE CF0B FFCF CFFF CFCF FFFF CFCF 9A9A FCCF 0EFF FFCF FFFF CFFF FFCF"
	$"FFFF CFCF 9A9A FACF 00FF FDCF 06FF FFCF FFFF CFFF FECF 019A FFF8 CF01 FFFF FDCF"
	$"FEFF 01CF CFFC FF09 CFCF 9ACF 9ACF FFCF CFFF FECF 0DFF CFCF FFFF CFFF CFCF FF9A"
	$"CFCF 9AFB CF04 FFCF CFFF CFFE FF01 CFFF EBCF 00FF EECE FE00 FECE FE00 FECE FE00"
	$"FECE 0200 CECE F500 00CE FD00 02CE CE00 FECE FE00 02CE CE00 FECE FE00 02CE 0000"
	$"FECE 0100 00FB CE00 00FD CEFC 0000 CEFE 0001 CECE FE00 01CE CEFE 00FE CEFE 0001"
	$"CECE FE00 01CE CEFE 0000 CEFE 0000 CEFC 0005 CECE 0000 CECE FE00 01CE CEFE 0000"
	$"CEF2 0000 CEFE 0000 CEFE 0004 CE00 00CE CEFB 0000 CEFC 0000 CEFE 0001 CECE FE00"
	$"00CE F900 00CE FE00 01CE CEF7 0000 CEFB 00FE CEFE 0001 CECE FA00 01CE CEFB 0002"
	$"CECE 00FE CEFE 0000 CEFC 00F3 CEFD FF02 CEFF CEFD FFFE CE02 9C9C CEFE FF00 CEF6"
	$"FFFD CE01 9CCE FEFF 00CE FEFF 01CE CEFE FF00 CEFE FFFD CE00 9CFE FF02 CEFF CEFA"
	$"FF02 CEFF CEFC FF00 CEFE FFFC CE00 9CF4 FF0B CEFF CEFF CE9C CE9C 9CFF FFCE F9FF"
	$"04CE CEFF CEFF FCCE 01FF FFF2 CE00 FF01 ABE2 CE00 00FC CE00 00FA CE00 00FE CE00"
	$"00F7 CE00 00FB CE00 00C7 CE00 00CD CE00 00F0 CE00 00EA CE00 00FE CE00 00C4 CE03"
	$"FFCE CE9C FECE 09FF CECE FFCE CE9C CE9C 9CFA CE05 9CCE 9CCE FFCE FEFF 00CE FE9C"
	$"F7CE 059C CEFF FFCE FFFD CE06 9C9C CECE 9CCE CEFE 9C00 CEF8 9C03 CECE 9CCE FEFF"
	$"FDCE 009C FECE 059C CE9C CECE 9CFD CE03 FFCE FFFF FECE FE9C F4CE 00FF FECE 039C"
	$"CECE 9CF0 CE00 FFE2 CF00 00FC CF00 00FA CF00 00FE CF00 00F7 CF00 00FB CF00 00C7"
	$"CF00 00CD CF00 00F0 CF00 00EA CF00 00FE CF00 00C4 CF0A FFCF CFFF CFFF CFFF CFCF"
	$"FFFD CF03 9ACF CFFF F8CF 01FF CFFD FF03 CFCF 9A9A FEFF FECF FDFF 04CF FFFF CFFF"
	$"FCCF 059A CFFF CFCF FFF3 CF02 FFCF CFFE FF01 CFFF FDCF 00FF FDCF 08FF FFCF CFFF"
	$"FFCF FFCF FEFF FECF 069A CFCF FFFF CFFF FCCF 03FF CFCF FFFD CFFE 9A02 FFCF FFF3"
	$"CF00 FFE2 CE00 00FC CE00 00FA CE00 00FE CE00 00F7 CE00 00FB CE00 00C7 CE00 00CD"
	$"CE00 00F0 CE00 00EA CE00 00FE CE00 00C4 CEF9 FF02 CECE FFFD CE04 9CCE FFFF CEF8"
	$"FF00 CEFE FFFE CE01 9CCE F4FF 01CE FFFC CE00 9CFA FF02 CEFF CEF7 FF00 CEFD FFFB"
	$"CEFA FF00 CEF9 FF05 CEFF CECE 9CCE FBFF 00CE FAFF FACE FEFF F3CE 00FF 01AA E3CE"
	$"FE00 03CE CE00 00F8 CEFE 00F5 CE00 00FD CE00 00C7 CEFE 00CF CE00 00F0 CE00 00E8"
	$"CEFE 00C3 CE02 FF9C 9CFD CE00 FFFB CE10 9CCE 9CCE 9CCE 9CCE 9CCE CE9C 9CCE FFCE"
	$"FFFE CEFA 9C02 CE9C CEFE 9C02 CE9C FFFA CE10 9CCE 9CCE 9CCE 9C9C CECE 9CCE 9CCE"
	$"9CCE 9CFA CEFE FFFE CE09 9CCE 9CCE 9CCE 9CCE CE9C FCCE 02FF CEFF FECE FD9C FECE"
	$"059C CE9C CE9C 9CFD CE00 FFFB CE03 9C9C CE9C F3CE 00FF E3CF FE00 03CF CF00 00F8"
	$"CFFE 00F5 CF00 00FD CF00 00C7 CFFE 00CF CF00 00F0 CF00 00E8 CFFE 00C3 CF07 FFCF"
	$"CFFF FFCF CFFF F9CF 0E9A CFFF CFCF FFCF CFFF CFCF FFFF CFFF FCCF 0D9A 9AFF FFCF"
	$"CFFF CFCF FFFF CFCF FFF8 CF00 FFFE CF00 FFF6 CFFE FFFE CFFE FFFE CF00 9AFB CF0A"
	$"FFCF CFFF CFFF FFCF FFCF FFFD CF03 9A9A CFFF FECF 01FF CFFD FFFE CF04 FFCF FFCF"
	$"CFFE 9AF0 CF00 FFE3 CEFE 0003 CECE 0000 F8CE FE00 F5CE 0000 FDCE 0000 C7CE FE00"
	$"CFCE 0000 F0CE 0000 E8CE FE00 C3CE FBFF 01CE FFF9 CE00 9CFC FF00 CEFB FF01 CEFF"
	$"FCCE 019C CEFC FF00 CEFB FFFD CE00 FFFD CEFC FF00 CEFE FF05 CECE FFCE FFCE FBFF"
	$"00CE FEFF FECE 059C CECE FFFF CEF9 FF03 CEFF CEFF FDCE 029C 9CCE F7FF FECE 00FF"
	$"FACE FEFF F3CE 00FF 0147 81CE 81CE E2CE 02FF 9C9C FDCE 00FF FBCE 109C CE9C CE9C"
	$"CE9C CE9C CECE 9C9C CEFF CEFF FECE FA9C 02CE 9CCE FE9C 02CE 9CFF FACE 109C CE9C"
	$"CE9C CE9C 9CCE CE9C CE9C CE9C CE9C FACE FEFF FECE 099C CE9C CE9C CE9C CECE 9CFC"
	$"CE02 FFCE FFFE CEFD 9CFE CE05 9CCE 9CCE 9C9C FDCE 00FF FBCE 039C 9CCE 9CF3 CE00"
	$"FF81 CF81 CFE2 CF07 FFCF CFFF FFCF CFFF F9CF 0E9A CFFF CFCF FFCF CFFF CFCF FFFF"
	$"CFFF FCCF 0D9A 9AFF FFCF CFFF CFCF FFFF CFCF FFF8 CF00 FFFE CF00 FFF6 CFFE FFFE"
	$"CFFE FFFE CF00 9AFB CF0A FFCF CFFF CFFF FFCF FFCF FFFD CF03 9A9A CFFF FECF 01FF"
	$"CFFD FFFE CF04 FFCF FFCF CFFE 9AF0 CF00 FF81 CE81 CEE2 CEFB FF01 CEFF F9CE 009C"
	$"FCFF 00CE FBFF 01CE FFFC CE01 9CCE FCFF 00CE FBFF FDCE 00FF FDCE FCFF 00CE FEFF"
	$"05CE CEFF CEFF CEFB FF00 CEFE FFFE CE05 9CCE CEFF FFCE F9FF 03CE FFCE FFFD CE02"
	$"9C9C CEF7 FFFE CE00 FFFA CEFE FFF3 CE00 FF01 4981 CE81 CEE2 CEFE FF04 9CCE 9CCE"
	$"FFFC CE02 9CCE 9CFC CE0C 9C9C CECE 9C9C CEFF CEFF CEFF CEFD 9CFE CE0C 9CCE CE9C"
	$"CECE 9CCE FFFF CECE FFFE CE00 9CFA CE08 9C9C CE9C 9CCE 9CCE 9CFE CE03 9CCE 9CCE"
	$"FEFF FECE 049C 9CCE CE9C FACE 059C CECE FFCE FFFD CEFE 9CFD CE00 9CFB CEFD FF01"
	$"CECE FD9C 02CE CE9C F3CE 00FF 81CF 81CF E2CF FDFF 13CF FFCF FFCF CFFF CFFF 9ACF"
	$"CF9A CFFF FFCF FFCF FFFE CF11 FFFF CFFF FFCF FFCF CF9A CFFF FFCF FFFF CFFF FECF"
	$"FEFF 02CF CFFF FDCF 059A FFCF FFCF FFFB CF00 FFFD CF03 FFFF CFCF FCFF FECF 009A"
	$"FECF 0DFF CFCF FFCF CFFF CFCF FFCF FFCF FFFB CF06 9AFF CFCF FFCF FFFE CFFB FFFE"
	$"CFFE 9A02 FFCF FFF3 CF00 FF81 CE81 CEE2 CEF9 FFFE CE05 FFCE 9CCE 9CCE F5FF 02CE"
	$"FFFF FDCE 019C CEF8 FF00 CEFE FF02 CECE FFFD CE00 9CF8 FF00 CEFD FF00 CEF7 FFFE"
	$"CE02 9CCE CEF4 FF02 CEFF FFFD CE01 9CCE F2FF FECE FE9C FEFF F3CE 00FF 0139 81CE"
	$"81CE E2CE 07FF CECE 9C9C CECE FFFB CE06 9C9C CE9C CECE 9CFE CE00 9CFE CEFD FFFD"
	$"CE00 9CFB CE01 9CCE FE9C 13CE FFCE FFCE CEFF CE9C CE9C CECE 9CCE 9CCE 9C9C CEFE"
	$"9C02 CE9C 9CFC CE01 9CCE FDFF 1ACE CE9C CECE 9CCE CE9C CE9C 9CCE 9CCE 9CCE FFCE"
	$"FFCE CEFF 9CCE CE9C F4CE 02FF 9CFF FDCE 009C F0CE 00FF 81CF 81CF E2CF FEFF 04CF"
	$"CFFF CFFF FACF 059A CFCF FFCF FFFE CF02 FFCF CFFC FFF8 CF03 FFCF CFFF FDCF 05FF"
	$"CFFF CFCF FFFE CF06 9ACF FFCF FFCF FFF8 CF03 FFCF CFFF FECF FDFF FBCF 07FF CFFF"
	$"CFCF FFCF FFFE CF0C FFCF FFCF FFFF CF9A 9ACF CFFF FFF7 CF02 FFCF FFEB CF00 FF81"
	$"CE81 CEE2 CEF9 FF02 CECE FFFD CE01 9CCE FAFF 00CE FAFF 05CE FFCE CE9C CEF5 FF04"
	$"CEFF FFCE FFFE CE00 9CFA FF06 CEFF FFCE FFFF CEF5 FFFC CEF9 FF00 CEFD FF08 CEFF"
	$"CECE FFCE CE9C CEF6 FF04 CECE FFCE FFFC CEFE FFF3 CE00 FF01 5381 CE81 CEE2 CE00"
	$"FFFD CE05 9CCE FFCE CEFF FECE 019C 9CFE CE03 9CCE CE9C FCCE 05FF CEFF FFCE CEFD"
	$"9CFC CE01 9CCE FE9C 0FCE FFCE FFCE CEFF CE9C CE9C CECE 9CCE CEFE 9C00 CEFE 9C00"
	$"CEFE 9C05 CECE 9CCE 9CCE FEFF 149C CECE 9CCE 9CCE 9CCE CE9C CE9C CECE 9CCE CEFF"
	$"CEFF FBCE 009C FCCE 009C FACE 01FF 9CFC CE03 9CCE CE9C F3CE 00FF 81CF 81CF E2CF"
	$"0EFF CFCF FFCF FFCF FFCF CFFF FFCF CF9A FDCF 02FF CFFF FECF 08FF CFFF FFCF FFFF"
	$"CFCF FD9A FECF 13FF FFCF CFFF FFCF CFFF CFFF FFCF FFCF CF9A 9ACF FFF8 CF00 FFFD"
	$"CFFE FFFE CFFE FFFC CF02 9AFF FFFE CF03 FFCF CFFF FECF 02FF CFFF F9CF 04FF FFCF"
	$"FFFF FDCF 0BFF CFCF FFCF FFCF CF9A 9ACF FFF1 CF00 FF81 CE81 CEE2 CEFD FF00 CEFE"
	$"FF02 CECE FFFB CEFC FF00 CEFE FF07 CEFF FFCE FFFF CECE FE9C 00CE F5FF 04CE FFFF"
	$"CEFF FECE 009C FAFF 03CE FFFF CEFD FF00 CEF8 FFFB CEF4 FF03 CEFF CEFF FCCE F6FF"
	$"02CE CEFF FACE FEFF F3CE 00FF 0153 81CE 81CE E2CE 00FF FDCE 059C CEFF CECE FFFE"
	$"CE01 9C9C FECE 039C CECE 9CFC CE05 FFCE FFFF CECE FD9C FCCE 019C CEFE 9C0F CEFF"
	$"CEFF CECE FFCE 9CCE 9CCE CE9C CECE FE9C 00CE FE9C 00CE FE9C 05CE CE9C CE9C CEFE"
	$"FF14 9CCE CE9C CE9C CE9C CECE 9CCE 9CCE CE9C CECE FFCE FFFB CE00 9CFC CE00 9CFA"
	$"CE01 FF9C FCCE 039C CECE 9CF3 CE00 FF81 CF81 CFE2 CF0E FFCF CFFF CFFF CFFF CFCF"
	$"FFFF CFCF 9AFD CF02 FFCF FFFE CF08 FFCF FFFF CFFF FFCF CFFD 9AFE CF13 FFFF CFCF"
	$"FFFF CFCF FFCF FFFF CFFF CFCF 9A9A CFFF F8CF 00FF FDCF FEFF FECF FEFF FCCF 029A"
	$"FFFF FECF 03FF CFCF FFFE CF02 FFCF FFF9 CF04 FFFF CFFF FFFD CF0B FFCF CFFF CFFF"
	$"CFCF 9A9A CFFF F1CF 00FF 81CE 81CE E2CE FDFF 00CE FEFF 02CE CEFF FBCE FCFF 00CE"
	$"FEFF 07CE FFFF CEFF FFCE CEFE 9C00 CEF5 FF04 CEFF FFCE FFFE CE00 9CFA FF03 CEFF"
	$"FFCE FDFF 00CE F8FF FBCE F4FF 03CE FFCE FFFC CEF6 FF02 CECE FFFA CEFE FFF3 CE00"
	$"FF01 6981 CE81 CEE2 CE02 FF9C 9CFD CE0C FFCE CEFF CE9C CE9C 9CCE 9CCE 9CFE CE00"
	$"9CFD CE08 FFCE FFCE CE9C CECE 9CFE CE00 9CFE CEFE 9C0B CE9C FFCE FFFF CEFF CE9C"
	$"CE9C FECE 019C CEFD 9C05 CE9C 9CCE 9C9C FCCE 019C CEFE FFFD CE03 9CCE CE9C FECE"
	$"029C CE9C FDCE 14FF CEFF CEFF CECE 9CCE 9CCE 9C9C CECE 9CFF CECE 9CCE FEFF 01CE"
	$"FFFD CE02 9CCE 9CF2 CE00 FF81 CF81 CFE2 CFFE FF07 CFCF FFCF FFCF CFFF FDCF 059A"
	$"CFCF FFCF FFFE CF00 FFFE CF04 FFCF FFCF FFFE CF06 9ACF FFFF CFCF FFFB CF05 FFCF"
	$"FFFF CFFF FECF 039A FFCF FFF8 CF03 FFCF CFFF FECF 02FF CFCF FDFF FCCF 05FF CFFF"
	$"FFCF FFFE CF04 FFFF CFFF CFFE FF02 CFCF 9AFC CF02 FFCF CFFE FF00 CFFD FF01 CFFF"
	$"FDCF 029A CFFF F2CF 00FF 81CE 81CE E2CE 02FF CECE FCFF 02CE CEFF FDCE 049C CEFF"
	$"FFCE FEFF 00CE FCFF 02CE FFFF FDCE 019C CEF5 FF04 CEFF FFCE FFFE CE00 9CFA FF06"
	$"CEFF FFCE FFFF CEFD FF00 CEFB FFFB CEF9 FF00 CEFD FF00 CEFE FFFC CE02 FFFF CEF6"
	$"FF01 CEFF FCCE FEFF F3CE 00FF 013A 81CE 81CE E2CE 07FF CECE 9CCE CE9C FFFC CE03"
	$"9CCE CE9C FECE 039C CE9C CEFE 9C0A CEFF CEFF CECE FFCE CE9C 9CFE CE02 9CCE CEFE"
	$"9C02 CECE FFFA CE00 9CFB CE01 9CCE FD9C 04FF 9CCE 9CCE FE9C FECE FEFF FECE 029C"
	$"CE9C FBCE 059C CE9C CECE 9CFE FFFE CE02 9CCE 9CFD CE03 9CCE CE9C FBCE 00FF FECE"
	$"FE9C FECE 009C F3CE 00FF 81CF 81CF E2CF 00FF FECF 03FF CFFF FFFC CF00 9AFA CF01"
	$"FFFF FECF 0BFF CFFF CFFF CFCF FFCF CF9A 9AFE CF02 FFCF FFFE CFFE FF01 CFFF FECF"
	$"08FF CFCF 9ACF FFCF FFFF FBCF 00FF F9CF FCFF FDCF 039A 9ACF FFFE CF01 FFFF FDCF"
	$"FDFF FCCF 029A CFFF FECF 00FF F9CF 00FF FACF 02FF CFFF F3CF 00FF 81CE 81CE E2CE"
	$"FEFF 00CE FDFF FCCE 009C FECE F9FF 00CE FEFF 08CE FFCE CEFF CECE 9C9C FAFF 01CE"
	$"CEFE FF01 CEFF FACE FCFF 03CE FFFF CEF1 FFFB CEF2 FFFC CE01 9CCE F6FF 02CE CEFF"
	$"FACE FEFF F3CE 00FF 0150 81CE 81CE E2CE 03FF CECE 9CFE CE0D FFCE CEFF CECE 9CCE"
	$"CE9C CE9C CE9C FDCE 079C CECE FFCE FFFF 9CFD CEFE 9CFC CE07 9C9C CE9C FFFF CEFF"
	$"FDCE 019C 9CFE CE01 9CCE FE9C 06CE 9CCE 9C9C CE9C FECE 039C CECE 9CFE FFFE CE02"
	$"9CCE 9CFB CE02 9CFF 9CFE CE03 FFCE FFFF FECE 069C 9CCE 9CCE CE9C FACE FEFF 009C"
	$"FECE FE9C 02CE CE9C F3CE 00FF 81CF 81CF E2CF 00FF FCCF 19FF FFCF CFFF CFFF 9ACF"
	$"9ACF CFFF CFFF CFFF FFCF FFCF CFFF CFFF FFFC CF02 9ACF CFFE FF00 CFFD FF04 CFFF"
	$"FFCF FFFD CF06 9A9A CFFF CFCF FFF9 CF06 FFCF CFFF FFCF CFFC FFFA CF0A FFCF CFFF"
	$"CFCF FFCF FFFF CFFD FF0B CFFF CF9A 9ACF CFFF CFCF FFCF FEFF FDCF 00FF FDCF 059A"
	$"9ACF CFFF FFF3 CF00 FF81 CE81 CEE2 CEF9 FF08 CECE FFCE CE9C CE9C CEF5 FF02 CEFF"
	$"FFFC CE00 9CF4 FF01 CEFF FDCE 049C CEFF FFCE FDFF 02CE FFCE F2FF FBCE F4FF 02CE"
	$"FFFF FECE 029C 9CCE F6FF 02CE CEFF FECE FE9C 00CE FEFF F3CE 00FF 0149 81CE 81CE"
	$"E2CE 03FF CECE 9CFE CE00 FFFD CE03 9CCE 9C9C FACE 079C CE9C CECE FFCE FFFC CE00"
	$"9CFC CE07 9CCE CE9C 9CCE CEFF FCCE FD9C 08CE CE9C CECE 9CCE 9CCE FE9C 00CE FE9C"
	$"02CE CE9C FECE FEFF FECE 009C FECE 009C FBCE 039C 9CCE CEFE FF03 CECE 9CCE FE9C"
	$"FECE 039C CECE 9CFB CE00 FFFD CEFE 9C02 CECE 9CF3 CE00 FF81 CF81 CFE2 CFFE FF04"
	$"CFFF CFFF FFFA CF03 9ACF CFFF FACF 05FF CFFF CFFF FFFD CF06 9A9A CFCF FFCF FFFD"
	$"CF05 FFCF FFCF CFFF FDCF 069A 9ACF FFCF FFFF F6CF 01FF FFFD CFFD FFFC CF03 FFCF"
	$"FFFF FECF 04FF CFCF FFCF FDFF 0ACF 9ACF 9A9A CFCF FFFF CFFF FBCF FEFF 01CF FFEB"
	$"CF00 FF81 CE81 CEE2 CEFE FF00 CEFD FFFD CE04 9CCE CE9C CEFC FF03 CEFF FFCE FEFF"
	$"02CE FFFF FDCE 019C CEFA FF01 CECE FEFF FACE 019C 9CFC FF00 CEFD FF08 CEFF CEFF"
	$"FFCE FFFF CEFB FFFB CEF9 FF00 CEFB FF06 CECE 9CCE CE9C CEF6 FF02 CECE FFFA CEFE"
	$"FFF3 CE00 FF01 4981 CE81 CEE2 CE03 FFCE CE9C FECE 00FF FDCE 039C CE9C 9CFA CE07"
	$"9CCE 9CCE CEFF CEFF FCCE 009C FCCE 079C CECE 9C9C CECE FFFC CEFD 9C08 CECE 9CCE"
	$"CE9C CE9C CEFE 9C00 CEFE 9C02 CECE 9CFE CEFE FFFE CE00 9CFE CE00 9CFB CE03 9C9C"
	$"CECE FEFF 03CE CE9C CEFE 9CFE CE03 9CCE CE9C FBCE 00FF FDCE FE9C 02CE CE9C F3CE"
	$"00FF 81CF 81CF E2CF FEFF 04CF FFCF FFFF FACF 039A CFCF FFFA CF05 FFCF FFCF FFFF"
	$"FDCF 069A 9ACF CFFF CFFF FDCF 05FF CFFF CFCF FFFD CF06 9A9A CFFF CFFF FFF6 CF01"
	$"FFFF FDCF FDFF FCCF 03FF CFFF FFFE CF04 FFCF CFFF CFFD FF0A CF9A CF9A 9ACF CFFF"
	$"FFCF FFFB CFFE FF01 CFFF EBCF 00FF 81CE 81CE E2CE FEFF 00CE FDFF FDCE 049C CECE"
	$"9CCE FCFF 03CE FFFF CEFE FF02 CEFF FFFD CE01 9CCE FAFF 01CE CEFE FFFA CE01 9C9C"
	$"FCFF 00CE FDFF 08CE FFCE FFFF CEFF FFCE FBFF FBCE F9FF 00CE FBFF 06CE CE9C CECE"
	$"9CCE F6FF 02CE CEFF FACE FEFF F3CE 00FF 013C 81CE 81CE E2CE 00FF FECE 039C CE9C"
	$"FFFC CE06 9CCE 9C9C CECE 9CFE CE03 9CCE CE9C FECE 00FF FECE FD9C FDCE 009C FDCE"
	$"009C FCCE 089C CE9C CE9C 9CCE 9CCE FE9C 03CE CE9C CEFB 9C02 CECE 9CFE CEFE FFFE"
	$"CE09 9CCE 9CCE CE9C CECE 9C9C FCCE 05FF CEFF FF9C CEFD 9CFD CE05 9CFF 9CCE CE9C"
	$"FECE 04FF CECE 9CCE FE9C 02CE CE9C F3CE 00FF 81CF 81CF E2CF 07FF CFCF FFCF CFFF"
	$"FFF9 CF04 9AFF CFCF FFFE CF01 FFFF FECF 01FF FFFC CF01 9A9A FECF 00FF F8CF 00FF"
	$"FBCF 029A CFFF FACF 00FF F9CF 00FF FECF FEFF FECF 009A FDCF 0DFF CFFF CFCF FFCF"
	$"FFCF CFFF CFFF FFFE CF03 9A9A CFFF FECF 01FF FFFE CF00 FFFE CF00 FFFA CF00 FFF1"
	$"CF00 FF81 CE81 CEE2 CEF9 FFFD CE00 9CFE CE00 9CF6 FF02 CECE FFFC CE01 9C9C F7FF"
	$"02CE CEFF FACE 009C FDFF 03CE FFCE CEFB FF00 CEF7 FFFB CEF4 FF08 CEFF FFCE 9CCE"
	$"9C9C CEF3 FFFA CE01 FFFF F2CE 00FF 0147 81CE 81CE E2CE 0AFF CECE 9C9C CECE FFCE"
	$"CEFF FECE 019C 9CFD CE0B 9CCE 9CCE FF9C CEFF FFCE FFFF FCCE FC9C 02FF 9C9C FDCE"
	$"03FF FFCE FFFE CE07 9CCE 9CCE CE9C CECE FE9C 07CE 9C9C CE9C 9CCE 9CFD CE01 9C9C"
	$"FEFF 039C CECE 9CFE CE12 9CCE 9C9C CECE 9CCE 9CCE CEFF CEFF FFCE FFCE 9CFE CEFE"
	$"9CFB CE00 9CFE FF01 9CFF FDCE 029C CE9C F2CE 00FF 81CF 81CF E2CF FEFF 03CF CFFF"
	$"CFFC FFFE CF00 9AFA CF08 FFFF CFCF FFFF CFFF FFF7 CF02 FFCF FFFD CF03 FFFF CFFF"
	$"FCCF 039A CFCF FFF4 CF01 FFFF FDCF FEFF FACF 00FF FBCF 09FF CFFF CFFF CFFF FFCF"
	$"FFF8 CF00 FFFE CF00 FFFE CF02 FFCF FFFD CF03 9ACF CFFF F3CF 00FF 81CE 81CE E2CE"
	$"F9FF 02CE CEFF FDCE 019C CEFE FF00 CEF9 FF02 CEFF FFFB CEFE FF00 CEF8 FF01 CEFF"
	$"FCCE 009C F7FF 05CE FFCE FFFF CEF8 FFFB CEFE FF00 CEFE FF00 CEFC FF04 CEFF FFCE"
	$"FFFD CEFE FF00 CEFA FF04 CECE FFCE FFFC CEFE FFF3 CE00 FF01 5081 CE81 CEE2 CE06"
	$"FFCE CE9C CE9C CEFC FF02 CE9C 9CFE CE00 9CFE CE01 9CCE FE9C 09CE FFCE FFFF CECE"
	$"9C9C CEFE 9CFE CE00 9CFC CE15 FFCE FFFF CEFF CE9C 9CCE CE9C CECE 9CCE 9CCE 9CCE"
	$"9CCE FD9C 05CE CE9C CE9C CEFD FF08 CE9C CE9C 9CCE 9CCE 9CFE CE0A 9CCE 9CCE CEFF"
	$"CEFF FFCE CEFD 9C06 CE9C CECE 9CCE 9CFD CEFE FFFE CEFC 9CF1 CE00 FF81 CF81 CFE2"
	$"CF06 FFCF CFFF CFFF CFFC FF03 CFCF 9A9A FDCF 00FF FDCF 04FF CFCF FFCF FEFF FACF"
	$"00FF FECF 0DFF CFCF FFCF FFCF FFFF CFFF CF9A 9AFE CF00 FFF4 CF01 FFFF FDCF FDFF"
	$"FECF 009A FECF 03FF FFCF FFFC CF07 FFFF CFFF FFCF CF9A FBCF 03FF CFFF CFFE FF00"
	$"CFFE FF02 CFCF FFFB CF00 FFF3 CF00 FF81 CE81 CEE2 CEF5 FF06 CECE 9C9C CEFF CEFD"
	$"FF02 CEFF CEFB FFFD CE00 9CFE CEF7 FF0A CEFF FFCE FFCE 9CCE 9CFF CEFD FF09 CEFF"
	$"CEFF CEFF CEFF FFCE F7FF FCCE 01FF CEFC FF00 CEF9 FF07 CECE 9CCE 9CCE FFCE F5FF"
	$"F9CE 01FF FFF3 CE00 FF01 3F81 CE81 CEE2 CE00 FFFE CE00 9CFC CE02 FFCE CEFD 9C04"
	$"CECE 9CCE 9CFB CE03 FFCE FFFF FCCE FD9C FBCE 079C CEFF FFCE FFFF 9CFE CE0F 9CCE"
	$"9CCE 9CCE 9CCE 9CCE 9CCE 9CCE 9C9C FACE FEFF FECE 009C FECE 039C CECE 9CFE CE07"
	$"9CFF CECE FFCE FFFF FECE 009C FECE 019C 9CFA CE00 9CFD FF01 CECE FE9C FECE 009C"
	$"F3CE 00FF 81CF 81CF E2CF FEFF 03CF FFCF FFFE CF03 FFCF CF9A FACF 02FF CFFF FECF"
	$"04FF FFCF FFFF FDCF 009A FCCF 00FF FBCF 04FF FFCF FFFF FDCF 009A FECF 00FF FCCF"
	$"00FF F9CF 00FF FECF FEFF 01CF FFF7 CF01 FFCF FEFF 04CF FFCF FFFF FECF 069A 9ACF"
	$"FFCF CFFF FBCF FCFF 01CF CFFD 9A01 CFFF F2CF 00FF 81CE 81CE E2CE FAFF FECE 06FF"
	$"CECE 9CCE 9CCE F5FF 02CE FFFF FDCE 019C CEF4 FF02 CEFF FFFD CE00 9CFD FF00 CEF9"
	$"FF00 CEF6 FFFB CEFE FF00 CEF8 FF08 CEFF FFCE FFCE 9C9C CEFB FF00 CEF9 FF01 CECE"
	$"FE9C 00CE FEFF F3CE 00FF 0142 81CE 81CE E2CE 02FF 9C9C FDCE FEFF FBCE 009C FECE"
	$"079C FFCE 9CCE 9CCE 9CFD FF01 CECE FE9C FCCE 019C CEFD 9C01 CEFF FDCE 03FF CE9C"
	$"9CFE CE01 9CCE FD9C 04CE 9C9C CE9C FECE FE9C 02CE 9CCE FDFF FECE 019C 9CFE CE12"
	$"9CFF 9CCE CE9C CE9C CEFF CEFF CEFF CECE 9CCE 9CFD CE02 9CCE 9CFD CEFE FFFD CEFE"
	$"9C01 CE9C F2CE 00FF 81CF 81CF E2CF 00FF FDCF 01FF CFFD FF01 CFFF FECF 089A FFCF"
	$"CFFF FFCF CFFF FECF FCFF FCCF 05FF FFCF FFCF CFFE FF07 CFFF FFCF FFCF FFFF FDCF"
	$"01FF FFF7 CF02 FFCF FFFD CF01 FFCF FCFF FDCF 059A FFFF CFCF FFFD CF08 FFCF FFFF"
	$"CFFF CFFF FFFB CF00 FFFD CFFE FF00 CFFE FF07 CFCF FFCF 9A9A CFFF F1CF 00FF 81CE"
	$"81CE E2CE FEFF 00CE FAFF FDCE 019C CEF2 FFFD CE01 9CCE FAFF 01CE CEFE FF04 CEFF"
	$"FFCE FFFE CE00 9CFC FF06 CEFF CEFF CEFF CEFB FF00 CEFA FFFC CEF4 FF00 CEFE FFFE"
	$"CE01 9CCE F3FF FACE FEFF F3CE 00FF 0141 81CE 81CE E2CE 00FF FDCE 029C CEFF FDCE"
	$"FE9C 01CE 9CFA CE00 9CFE CE05 FFCE FFFF CE9C FECE 009C FECE 009C FACE 03FF FFCE"
	$"FFFC CE0F 9CCE CE9C CECE 9CCE CE9C 9CCE 9CCE 9C9C FACE FEFF 1A9C CECE 9C9C CE9C"
	$"CE9C CE9C CECE 9CCE 9CCE CEFF CEFF FFCE 9CCE 9C9C F5CE FEFF FDCE FE9C 01CE 9CF2"
	$"CE00 FF81 CF81 CFE2 CFFE FF04 CFFF CFFF FFFC CF09 9ACF 9ACF CFFF CFCF FFFF FDCF"
	$"04FF FFCF FFFF FDCF 009A FBCF 00FF FCCF 03FF FFCF FFFD CF03 9ACF CFFF FDCF 00FF"
	$"F8CF 03FF FFCF CFFC FFFE CF01 9A9A FDCF 03FF CFCF FFFC CFFD FF08 CF9A CF9A 9ACF"
	$"FFCF CFFE FFFD CFFD FFFD CFFE 9AF0 CF00 FF81 CE81 CEE2 CEF9 FFFC CE03 9CCE 9CCE"
	$"F9FF 00CE FEFF 00CE FEFF FECE 019C CEF4 FF01 CEFF FCCE 039C FFFF CEFA FF04 CEFF"
	$"FFCE CEF7 FFFE CE0B 9CCE CEFF FFCE FFFF CEFF FFCE FDFF 08CE FFFF CE9C CE9C 9CCE"
	$"F3FF 03CE FFCE CEFE 9C02 FFCE FFF3 CE00 FF01 5681 CE81 CEE2 CE0C FFCE CE9C CE9C"
	$"CEFF CECE FFCE CEFD 9CFB CE09 9CCE 9CCE 9CFF CEFF CEFF FDCE 009C FECE 039C CECE"
	$"9CFD CE06 FFFF CEFF FFCE CEFE 9C04 CECE 9CCE CEFE 9CFD CE02 9CCE 9CFA CEFE FFFE"
	$"CE04 9C9C CECE 9CFA CE08 9CCE 9CFF CEFF FFCE CEFE 9C03 CE9C CE9C FECE 049C CECE"
	$"9CCE FDFF FCCE 009C F0CE 00FF 81CF 81CF E2CF FEFF 07CF FFCF FFFF CFCF FFFD CF08"
	$"9ACF CFFF CFCF FFCF FFFD CF04 FFCF FFCF FFFC CF01 FFFF FDCF 00FF FDCF 06FF FFCF"
	$"FFFF CFCF FE9A 01CF FFFB CF00 FFFE CF00 FFFB CF02 FFCF CFFE FF0D CFFF CF9A CFCF"
	$"FFCF CFFF CFFF CFCF FEFF 04CF FFCF FFFF FCCF 069A FFCF CFFF CFFF FDCF FCFF FCCF"
	$"039A CFCF FFF3 CF00 FF81 CE81 CEE2 CEFE FF00 CEFD FF02 CECE FFFD CE01 9CCE FCFF"
	$"03CE FFFF CEFE FF03 CEFF CEFF FECE 019C CEF4 FF02 CEFF FFFE CE01 9C9C FCFF 02CE"
	$"FFCE FEFF 00CE F4FF FECE 029C CECE F9FF 00CE FDFF 02CE FFFF FDCE 019C CEF2 FFFC"
	$"CE00 9CFE FFF3 CE00 FF01 4381 CE81 CEE2 CE00 FFFB CEFE FF0E CECE 9CCE CE9C CECE"
	$"9CCE 9CCE 9CCE 9CFE CE04 FFCE FFFF CEFE 9C07 CE9C CECE 9CCE CE9C FDCE 129C FFFF"
	$"CEFF 9CFF 9CCE CE9C CE9C CECE 9C9C CECE FB9C 01CE 9CFE CE02 9CCE CEFD FFFE CE05"
	$"9C9C CECE 9C9C FDCE 009C FECE FEFF 01CE FFFD CE00 9CFE CE00 9CFE CE03 9C9C CECE"
	$"FEFF 009C FCCE 039C 9CCE 9CF3 CE00 FF81 CF81 CFE2 CF06 FFCF CFFF CFFF CFFD FFFC"
	$"CF04 9AFF CFCF FFFD CF08 FFFF CFFF CFFF FFCF 9AFE CF00 9AFE CF03 FFFF CFCF FBFF"
	$"03CF FFCF FFFE CF06 9AFF CFCF FFCF FFF8 CFFD FF00 CFFB FFFE CF08 9ACF CFFF CFCF"
	$"FFCF FFFD CFFD FF01 CFFF FBCF 03FF CFCF FFFD CF01 FFCF FEFF 01CF FFEB CF00 FF81"
	$"CE81 CEE2 CEF6 FFFB CEF2 FF01 CE9C FDCE 02FF FFCE FEFF 00CE F9FF 01CE FFFE CE03"
	$"9CFF FFCE FBFF 00CE FEFF 00CE F5FF FECE 049C CEFF FFCE F3FF FCCE 02FF FFCE FEFF"
	$"00CE FAFF FACE 02FF CEFF F3CE 00FF 014E 82CE FE00 CACE FE00 9ECE 07FF CECE 9CCE"
	$"9CCE FFFC CE08 9C9C CE9C CECE 9CFF 9CFE CE05 9CCE 9CFF CEFF FCCE FD9C FCCE 059C"
	$"9CCE CEFF FFFC CEFE 9C02 CECE 9CFE CE02 9C9C CEFE 9C01 CE9C FDCE 009C FECE FEFF"
	$"FECE 039C CECE 9CFE CE04 9CCE CE9C 9CFE CE08 FFCE FFCE 9C9C CE9C 9CFA CE04 9CCE"
	$"CE9C CEFE FF08 CECE 9CCE 9C9C CECE 9CF2 CE00 FF82 CFFE 00CA CFFE 009E CFFE FF04"
	$"CFFF CFFF FFFC CF00 9AFD CF05 FFCF FFFF CFFF FECF FDFF FBCF 029A FFFF FDCF 00FF"
	$"FDCF 01FF FFFB CF02 9ACF FFF4 CF00 FFFE CF02 9ACF CFFD FFFC CF08 9AFF CFFF FFCF"
	$"FFCF FFFD CFFE FFFD CF06 9ACF CFFF CFCF FFFB CFFD FFFD CFFE 9A00 FFF1 CF00 FF82"
	$"CEFE 00CA CEFE 009E CEFE FF00 CEFD FFFC CE00 9CFE CEF9 FF00 CEFE FF02 CEFF FFFC"
	$"CE00 9CFA FF01 CECE FDFF FBCE 019C CEFA FF04 CEFF FFCE CEFB FF00 CEFB FFFB CEF9"
	$"FF00 CEFD FF01 CEFF FDCE 029C CECE F3FF 06CE FFCE CE9C 9CCE FEFF F3CE 00FF 015A"
	$"84CE FC00 CCCE FC00 9ECE 06FF 9C9C CE9C CECE FDFF FECE 009C FECE 009C FDCE 009C"
	$"FDCE FEFF FECE FE9C 00CE FE9C FECE 009C FECE 049C FFCE FFFF FECE 039C 9CCE CEFC"
	$"9C06 CECE 9C9C CECE 63FE CE05 9C9C CECE 9CCE FEFF 08CE CE9C 9CCE 9CCE 9C9C FECE"
	$"159C CECE 9C9C CEFF CEFF CEFF FFCE 9CCE 9CCE 9C9C CECE 9CFA CE00 FFFD CEFB 9CF3"
	$"CE00 FF84 CFFC 00CC CFFC 009E CF00 FFFB CFFD FF03 CFFF CF9A FCCF 07FF CFFF CFFF"
	$"CFFF CFFD FF01 CFCF FE9A FECF 03FF CFCF FFFE CF03 FFCF FFCF FEFF 04CF FF9A CF9A"
	$"FECF 03FF CFCF FFF6 CF03 FFFF CFCF FEFF 03CF CF9A 9AFD CF00 9AFD CF00 FFFE CF06"
	$"FFFF CFFF CFFF FFFE CF06 9ACF CFFF CFFF CFFE FFFD CF03 FFCF CFFF ECCF 00FF 84CE"
	$"FC00 CCCE FC00 9ECE F6FF FECE 029C CECE F3FF 06CE FFCE 9C9C CECE F5FF 00CE FEFF"
	$"06CE CE9C CECE FFCE FEFF 00CE FEFF 00CE FEFF 00CE F5FF 07CE 9CCE CE9C FFFF CEF5"
	$"FF02 CEFF FFFE CE00 9CF6 FF02 CECE FFFB CE03 9CFF CEFF F3CE 00FF 016C 85CE 0100"
	$"00F9 CE01 0000 D2CE 0000 EACE 0100 00B3 CE00 FFFD CE05 9CCE FFCE CEFF FECE FE9C"
	$"02CE 9C9C FECE 09FF 9CCE CE9C FFCE FFFF CEFE 9C06 CE9C CECE 9CCE 9CFB CE03 FFFF"
	$"CEFF FECE FD9C FDCE 049C CE9C 9CCE FC9C FACE FEFF FECE 029C CE9C FECE 009C F9CE"
	$"0DFF CEFF FFCE 9CCE 9CCE 9CCE 9CCE 9CFE CEFE 9C00 CEFD FFFE CE01 9C9C EFCE 00FF"
	$"85CF 0100 00F9 CF01 0000 D2CF 0000 EACF 0100 00B3 CF0A FFCF CFFF FFCF FFFF CFCF"
	$"FFFD CF00 9AFC CF0A FFCF FFCF FFCF CFFF CFFF FFF8 CF03 FFFF CFCF FEFF 04CF FFFF"
	$"CFFF FDCF 009A FECF 00FF F4CF 03FF FFCF CFFB FFFD CF05 9AFF CFFF CFFF FECF 0CFF"
	$"FFCF FFFF CFFF FFCF 9ACF 9A9A F6CF FCFF FECF FE9A 01CF FFF2 CF00 FF85 CE01 0000"
	$"F9CE 0100 00D2 CE00 00EA CE01 0000 B3CE FCFF 05CE FFFF CECE FFFB CEF5 FF02 CEFF"
	$"FFFB CEFC FF00 CEFD FF0A CEFF FFCE FFCE FFCE CE9C CEFD FF00 CEFE FF00 CEFE FF00"
	$"CEFE FF01 CECE FAFF FBCE F4FF 08CE FFFF CE9C CE9C CECE FEFF 00CE F6FF FECE 029C"
	$"9CCE FEFF F3CE 00FF 01A0 A4CE FC00 E8CE 0100 00F8 CE01 0000 E6CE FC00 F3CE 0100"
	$"00EA CE01 0000 D5CE FB00 E5CE 07FF CECE 9CCE 9CCE FFFD CEFC 9CFE CE0A 9CCE CE9C"
	$"CE9C CE9C FFCE FFFE CE01 9C9C FDCE 049C CECE 9CCE FE9C 02CE FFFF FACE 029C CECE"
	$"FE9C 00CE FE9C 08CE 9CCE CE9C 9CCE 9C9C FDCE FEFF FECE 069C CE9C CECE 9C9C FDCE"
	$"009C FECE 02FF CEFF FCCE 049C 9CCE CE9C F9CE FDFF FECE FC9C F2CE 00FF A4CF FC00"
	$"E8CF 0100 00F8 CF01 0000 E6CF FC00 F3CF 0100 00EA CF01 0000 D5CF FB00 E5CF 00FF"
	$"FDCF 02FF CFFF FCCF 049A CF9A CFFF FECF 0BFF FFCF CFFF CFFF FFCF FFCF FFFC CF04"
	$"FFFF CFCF FFFC CFFE FF02 CFCF FFFB CF00 FFFE CF00 FFFB CF03 FFCF CFFF FDCF FCFF"
	$"FDCF 029A CFFF FECF 0AFF CFCF FFCF FFCF FFFF CFFF FCCF 039A CFCF FFFE CF00 FFFD"
	$"CFFC FFFC CF01 9AFF F1CF 00FF A4CE FC00 E8CE 0100 00F8 CE01 0000 E6CE FC00 F3CE"
	$"0100 00EA CE01 0000 D5CE FB00 E5CE 02FF CECE FCFF 05CE CEFF CECE 9CFE CEFC FF00"
	$"CEFB FF03 CEFF CEFF FCCE F8FF 00CE FEFF 02CE CEFF FCCE 02FF FFCE FCFF 00CE FCFF"
	$"00CE FCFF 00CE FDFF FBCE 02FF FFCE F7FF 02CE FFFF FDCE 019C CEF8 FF00 CEFC FFFC"
	$"CE03 9CFF CEFF F3CE 00FF 01AA A6CE 0100 00FE CE01 0000 E8CE 0100 00E8 CE00 00F7"
	$"CE01 0000 FECE 0100 00F3 CE01 0000 F0CE 0000 CCCE 0100 00E3 CE0A FF9C 9CCE 9CCE"
	$"CEFF CECE FFFE CE01 9C9C FECE 009C FBCE 079C CEFF CEFF FF9C CEFE 9CFA CE00 9CFD"
	$"CEFD FF04 9CFF CECE 9CFB CE02 9CCE FFFB 9CFD CE03 9CCE 9CCE FEFF 009C FDCE 009C"
	$"FBCE 0B9C CECE 9CCE CEFF CEFF FFCE CEFE 9C02 CECE 9CFE CE05 9CCE 9C9C CECE FEFF"
	$"FBCE 009C F0CE 00FF A6CF 0100 00FE CF01 0000 E8CF 0100 00E8 CF00 00F7 CF01 0000"
	$"FECF 0100 00F3 CF01 0000 F0CF 0000 CCCF 0100 00E3 CF05 FFCF CFFF CFCF FCFF F8CF"
	$"00FF FECF 02FF CFCF FCFF 039A CF9A 9AFC CF03 FFCF FFCF FEFF 00CF FDFF 099A FFFF"
	$"9ACF CFFF CFCF FFFE CF00 FFF7 CF00 FFFE CFFE FF01 CFFF FECF 049A CFFF CFFF FECF"
	$"04FF CFCF FFCF FDFF 0BCF FF9A CFCF 9AFF CFCF FFCF CFFD FF00 CFFE FF08 CFCF FFCF"
	$"9A9A CFCF FFF2 CF00 FFA6 CE01 0000 FECE 0100 00E8 CE01 0000 E8CE 0000 F7CE 0100"
	$"00FE CE01 0000 F3CE 0100 00F0 CE00 00CC CE01 0000 E3CE F6FF FBCE F2FF 009C FCCE"
	$"FBFF 00CE F9FF 019C FFFD CEFC FF00 CEFE FF03 CEFF FFCE F5FF FCCE 009C FBFF 00CE"
	$"F8FF 02CE CE9C FECE FCFF 00CE F9FF FACE FEFF F3CE 00FF 01BF A6CE 0000 FDCE 0000"
	$"E7CE 0100 00EA CE01 0000 F6CE 0000 FDCE 0000 F3CE FE00 F2CE 0100 00CC CE01 0000"
	$"E2CE 00FF FCCE 129C FFCE CEFF CECE 9CCE CE9C CE9C CECE 9C9C CE9C FECE 03FF CEFF"
	$"FFFC CE00 9CFD CE00 9CFC CE04 9CFF FFCE FFFE CE08 9CCE 9CCE CE9C CE9C CEFE 9C06"
	$"CE9C CE9C CE9C 9CFE CE02 9CCE CEFE FFFE CEFE 9C0F CECE 9CCE 9CCE CE9C CE9C CE9C"
	$"FFCE FFFF FECE FE9C FDCE 019C CEFD 9C00 CEFE FFFE CEFD 9C02 CECE 9CF3 CE00 FFA6"
	$"CF00 00FD CF00 00E7 CF01 0000 EACF 0100 00F6 CF00 00FD CF00 00F3 CFFE 00F2 CF01"
	$"0000 CCCF 0100 00E2 CFFE FF10 CFCF FFCF FFCF CFFF CFCF 9ACF 9ACF FFCF FFFE CF0A"
	$"FFCF FFCF CFFF CFFF FFCF FFFE CF07 9ACF CFFF CFFF CFFF FECF FEFF 01CF FFFD CF03"
	$"9ACF CFFF F3CF 01FF FFFE CFFD FFFD CF11 9A9A FFCF FFCF FFCF FFCF FFCF CFFF FFCF"
	$"FFFF FACF 03FF FFCF FFFC CFFD FFFE CF06 9ACF CF9A FFCF FFF3 CF00 FFA6 CE00 00FD"
	$"CE00 00E7 CE01 0000 EACE 0100 00F6 CE00 00FD CE00 00F3 CEFE 00F2 CE01 0000 CCCE"
	$"0100 00E2 CEF9 FF05 CECE FFCE CE9C FECE F5FF 00CE FEFF FCCE F4FF 01CE FFFB CEFB"
	$"FF00 CEFE FF04 CECE FFFF CEFE FF00 CEFB FF01 CEFF FDCE F8FF 00CE FEFF 02CE FFFF"
	$"FBCE FAFF 01CE CEFC FF06 CEFF CE9C CECE 9CFE FFF3 CE00 FF01 A4A7 CE01 0000 FDCE"
	$"0000 E8CE 0100 00E9 CE01 0000 F7CE 0100 00FD CE00 00F3 CE01 0000 F1CE 0100 00CC"
	$"CE01 0000 E2CE 07FF 9C9C CE9C CECE FFFD CE01 9CCE FE9C FBCE 049C CECE 9CCE FDFF"
	$"00CE FD9C FACE 049C CECE 9CCE FEFF 08CE CEFF CECE 9CCE CE9C FECE 059C CE9C CE9C"
	$"CEFE 9C00 CEFE 9CFD CEFD FFFE CE03 9CCE CE9C FDCE 009C FCCE 08FF CEFF CEFF 9CCE"
	$"CE9C FBCE 009C FCCE FEFF FDCE FE9C 01CE 9CF2 CE00 FFA7 CF01 0000 FDCF 0000 E8CF"
	$"0100 00E9 CF01 0000 F7CF 0100 00FD CF00 00F3 CF01 0000 F1CF 0100 00CC CF01 0000"
	$"E2CF 00FF FCCF 08FF FFCF CFFF CFCF FF9A FBCF 00FF FCCF FCFF 01CF 9AFD CF01 FFFF"
	$"F8CF FDFF 01CF FFFE CF07 9AFF CFCF FFFF CFFF F5CF 02FF CFCF FDFF FECF 059A CFFF"
	$"CFCF FFFB CF06 FFCF FFCF FFCF FFFD CF01 9AFF FECF 00FF FDCF 01FF CFFE FF01 CFFF"
	$"EBCF 00FF A7CE 0100 00FD CE00 00E8 CE01 0000 E9CE 0100 00F7 CE01 0000 FDCE 0000"
	$"F3CE 0100 00F1 CE01 0000 CCCE 0100 00E2 CE02 FFCE CEFC FFF8 CEFB FF00 CEF9 FF01"
	$"CE9C FDCE FBFF 00CE F9FF 01CE FFFD CEF9 FF00 CEFD FF04 CEFF FFCE CEF9 FFFE CE01"
	$"9CCE FBFF 00CE FBFF 03CE FFCE FFFC CEFC FF00 CEF9 FFFA CEFE FFF3 CE00 FF02 9BA7"
	$"CE01 0000 FDCE 0000 FCCE FD00 FDCE FE00 FECE FE00 FECE 0100 00FE CE01 0000 FECE"
	$"FE00 FDCE FC00 01CE CEFB 00F9 CE01 0000 FDCE 0000 FCCE FD00 FCCE 0100 00FE CEFE"
	$"00FE CE01 0000 FECE FB00 01CE CEFE 00FC CEFD 00FC CE01 0000 FECE 0100 00FD CEFC"
	$"00F1 CE01 0000 FECE 0100 00FE CE01 0000 FCCE FD00 F5CE 00FF FDCE 019C CEFD FFFE"
	$"CE06 9CCE 9CCE 9CCE 9CFE CE00 9CFE CE04 FFCE FFFF 9CFD CEFE 9C03 FF9C CECE FD9C"
	$"08CE FFFF CEFF 9CCE CE9C FECE 039C 9CCE CEFE 9C06 CE9C 9CCE 9C9C CEFE 9C03 CECE"
	$"9CCE FEFF 069C CECE 9CCE 9CCE FE9C FECE 009C FDCE 04FF CEFF FF9C FECE 00FF FE9C"
	$"04CE 9CCE CE9C FDCE FEFF FDCE FC9C F2CE 00FF A7CF 0100 00FD CF00 00FC CFFD 00FD"
	$"CFFE 00FE CFFE 00FE CF01 0000 FECF 0100 00FE CFFE 00FD CFFC 0001 CFCF FB00 F9CF"
	$"0100 00FD CF00 00FC CFFD 00FC CF01 0000 FECF FE00 FECF 0100 00FE CFFB 0001 CFCF"
	$"FE00 FCCF FD00 FCCF 0100 00FE CF01 0000 FDCF FC00 F1CF 0100 00FE CF01 0000 FECF"
	$"0100 00FC CFFD 00F5 CF06 FFCF CFFF CFFF CFFC FFF8 CF00 FFFB CF07 FFCF FFFF CFFF"
	$"9A9A FDCF 00FF FDCF FEFF 0ECF FFFF CFFF CFFF CF9A 9AFF FFCF CFFF F2CF 02FF CFCF"
	$"FEFF 06CF FFCF CF9A CFFF FECF 00FF FECF 0EFF CFFF CFFF CFFF FFCF FFCF CFFF 9AFF"
	$"F9CF 01FF CFFE FF02 CFCF FFFE CF03 9ACF CFFF F3CF 00FF A7CE 0100 00FD CE00 00FC"
	$"CEFD 00FD CEFE 00FE CEFE 00FE CE01 0000 FECE 0100 00FE CEFE 00FD CEFC 0001 CECE"
	$"FB00 F9CE 0100 00FD CE00 00FC CEFD 00FC CE01 0000 FECE FE00 FECE 0100 00FE CEFB"
	$"0001 CECE FE00 FCCE FD00 FCCE 0100 00FE CE01 0000 FDCE FC00 F1CE 0100 00FE CE01"
	$"0000 FECE 0100 00FC CEFD 00F5 CEF6 FFFB CE01 FFCE FDFF 03CE FFFF CEFC FFF9 CEF4"
	$"FF07 CEFF CE9C CECE FFCE FDFF 06CE CEFF FFCE FFCE FEFF 01CE CEFA FFFB CE03 FFCE"
	$"FFCE F8FF 02CE FFFF FDCE 03FF 9CFF CEF5 FFF9 CE01 FFFF F3CE 00FF 02D4 A7CE 0100"
	$"00F8 CE02 00CE CEFE 0001 CECE FD00 00CE FC00 FECE 0200 00CE FD00 01CE CEFD 00FE"
	$"CE05 0000 CECE 0000 FDCE 0100 00F7 CE01 0000 F8CE 0200 CECE FE00 FDCE 0700 00CE"
	$"CE00 CE00 00FE CE01 0000 FCCE 0100 00FE CEFD 00FD CE02 00CE CEFE 0001 CECE FD00"
	$"00CE FD00 FECE 0500 00CE CE00 00F1 CE02 00CE CEFD 0000 CEFD 00FD CE04 00CE CE00"
	$"00F5 CE07 FFCE CE9C 9CCE 9CFF FCCE 039C CE9C 9CFD CEFE 9C00 CEFE 9C00 FFFE CE0B"
	$"FFCE 9C9C CE9C CECE 9CCE CE9C FECE 069C CEFF FFCE CEFF FECE 009C FCCE 0A9C 9CCE"
	$"9C9C CE9C CE9C CE9C FECE 049C CE9C 9CFF FBCE 009C FACE 029C CE9C FECE 0FFF CECE"
	$"FFCE 9CCE CE9C 9CCE CE9C CECE 9CFC CE01 FFFF FCCE FE9C 02CE CE9C F3CE 00FF A7CF"
	$"0100 00F8 CF02 00CF CFFE 0001 CFCF FD00 00CF FC00 FECF 0200 00CF FD00 01CF CFFD"
	$"00FE CF05 0000 CFCF 0000 FDCF 0100 00F7 CF01 0000 F8CF 0200 CFCF FE00 FDCF 0700"
	$"00CF CF00 CF00 00FE CF01 0000 FCCF 0100 00FE CFFD 00FD CF02 00CF CFFE 0001 CFCF"
	$"FD00 00CF FD00 FECF 0500 00CF CF00 00F1 CF02 00CF CFFD 0000 CFFD 00FD CF04 00CF"
	$"CF00 00F5 CF00 FFFC CF01 FFFF FCCF 049A CFCF 9AFF FECF 01FF FFFC CF00 FFFE CF00"
	$"FFFD CF00 9AFC CF00 FFFD CFFE FF02 CFCF FFFB CF00 FFF8 CF00 FFF8 CF01 FFFF FACF"
	$"089A FFCF FFCF CFFF CFFF FECF 04FF FFCF CFFF FACF 05FF FFCF FFCF FFFE CFFE FF01"
	$"CFFF FECF 039A 9ACF FFF1 CF00 FFA7 CE01 0000 F8CE 0200 CECE FE00 01CE CEFD 0000"
	$"CEFC 00FE CE02 0000 CEFD 0001 CECE FD00 FECE 0500 00CE CE00 00FD CE01 0000 F7CE"
	$"0100 00F8 CE02 00CE CEFE 00FD CE07 0000 CECE 00CE 0000 FECE 0100 00FC CE01 0000"
	$"FECE FD00 FDCE 0200 CECE FE00 01CE CEFD 0000 CEFD 00FE CE05 0000 CECE 0000 F1CE"
	$"0200 CECE FD00 00CE FD00 FDCE 0400 CECE 0000 F5CE FDFF 00CE FEFF 08CE CEFF CECE"
	$"9CCE FF9C FAFF 00CE FDFF FECE 00FF FECE 01FF 9CF8 FF00 CEFE FF02 CECE FFFD CEFC"
	$"FF00 CEFE FF00 CEF6 FF00 CEFD FFFB CEFB FF00 CEFB FF02 CECE FFFD CE01 FFCE FCFF"
	$"00CE FAFF 02CE CEFF FCCE FEFF F3CE 00FF 029B A6CE 0100 00FA CE01 0000 FECE 0100"
	$"00FD CEFD 0003 CECE 0000 FECE FE00 09CE CE00 00CE 0000 CE00 00FE CE01 0000 FACE"
	$"0100 00F5 CE01 0000 FACE 0100 00FE CE01 0000 FECE FE00 FDCE 0500 00CE CE00 00FC"
	$"CE01 0000 FECE 0400 00CE 0000 FECE 0100 00FE CE01 0000 FDCE FE00 03CE CE00 00FE"
	$"CE01 0000 EECE 0100 00FD CEFE 0003 CECE 0000 FECE 0000 F0CE 07FF 9C9C CE9C CECE"
	$"FFFD CE00 9CFC CE01 9C9C FDCE 009C FECE 00FF F8CE 049C 9CCE CE9C FBCE 00FF FACE"
	$"0A9C CECE 9CCE CE9C CE9C 9CCE FE9C 03CE 9CCE 9CFB CE00 FFFB CE03 9C9C CE9C FECE"
	$"009C FBCE 02FF 9CFF F9CE 019C 9CFB CE00 9CF9 CE03 9C9C CE9C F1CE 00FF A6CF 0100"
	$"00FA CF01 0000 FECF 0100 00FD CFFD 0003 CFCF 0000 FECF FE00 09CF CF00 00CF 0000"
	$"CF00 00FE CF01 0000 FACF 0100 00F5 CF01 0000 FACF 0100 00FE CF01 0000 FECF FE00"
	$"FDCF 0500 00CF CF00 00FC CF01 0000 FECF 0400 00CF 0000 FECF 0100 00FE CF01 0000"
	$"FDCF FE00 03CF CF00 00FE CF01 0000 EECF 0100 00FD CFFE 0003 CFCF 0000 FECF 0000"
	$"F0CF FEFF 07CF CFFF CFFF CFCF FFF8 CF00 FFFE CF04 FFCF CFFF FFFD CF00 FFFA CF01"
	$"FFFF FBCF 00FF FACF 059A CFFF CFCF FFFD CF00 FFFD CF01 FFFF FDCF 03FF CFCF FFFB"
	$"CF00 9AFD CF01 FFFF FECF 06FF CFFF CFFF CFFF FACF 05FF 9ACF FFCF CFFD FFF6 CF01"
	$"9AFF F2CF 00FF A6CE 0100 00FA CE01 0000 FECE 0100 00FD CEFD 0003 CECE 0000 FECE"
	$"FE00 09CE CE00 00CE 0000 CE00 00FE CE01 0000 FACE 0100 00F5 CE01 0000 FACE 0100"
	$"00FE CE01 0000 FECE FE00 FDCE 0500 00CE CE00 00FC CE01 0000 FECE 0400 00CE 0000"
	$"FECE 0100 00FE CE01 0000 FDCE FE00 03CE CE00 00FE CE01 0000 EECE 0100 00FD CEFE"
	$"0003 CECE 0000 FECE 0000 F0CE 02FF CECE FCFF F8CE F5FF F8CE FBFF 00CE FCFF F8CE"
	$"FCFF 05CE FFCE FFFF CEFD FF00 CEFA FFFB CE01 9CCE F4FF 01CE FFFA CEF6 FFFE CE06"
	$"FF9C CECE 9C9C CEFE FFF3 CE00 FF02 B4A5 CE01 0000 FBCE 0000 FDCE 0100 00FD CEFE"
	$"00FE CE03 0000 CECE FE00 FECE 0100 00FE CEFE 00FE CEFE 00FB CE01 0000 F4CE 0100"
	$"00FB CE00 00FD CE01 0000 FECE 0100 00FD CE01 0000 FECE 0100 00FC CE01 0000 FCCE"
	$"FE00 FECE 0000 FDCE 0100 00FD CE01 0000 FECE 0100 00FE CEFE 00EF CE01 0000 FDCE"
	$"0100 00FE CE05 0000 CECE 0000 F0CE 00FF FACE 049C 9CCE 9CCE FD9C F5CE 039C CE9C"
	$"CEFC 9CFC CE00 9CFE CE00 9CFE CEF9 9C0F CECE 9CCE 9C9C CECE 9CCE 9C9C CE9C CE9C"
	$"FECE 009C FECE FA9C 02CE CE9C FACE 009C FDCE 019C CEFB 9C08 CECE FF9C CE9C CE9C"
	$"9CFC CEFE 9C00 CEFE 9C01 CE9C F2CE 00FF A5CF 0100 00FB CF00 00FD CF01 0000 FDCF"
	$"FE00 FECF 0300 00CF CFFE 00FE CF01 0000 FECF FE00 FECF FE00 FBCF 0100 00F4 CF01"
	$"0000 FBCF 0000 FDCF 0100 00FE CF01 0000 FDCF 0100 00FE CF01 0000 FCCF 0100 00FC"
	$"CFFE 00FE CF00 00FD CF01 0000 FDCF 0100 00FE CF01 0000 FECF FE00 EFCF 0100 00FD"
	$"CF01 0000 FECF 0500 00CF CF00 00F0 CF00 FFFE CF0D FFCF FFCF 9A9A CF9A CFCF 9A9A"
	$"CFFF FECF 0FFF CFFF CFFF FFCF FF9A CFCF 9A9A CFCF 9AFA CFFD FF07 CFFF CF9A CF9A"
	$"9ACF FE9A 01CF FFF3 CFFE FF03 CFFF FFCF FD9A 0FCF 9A9A CFFF CFFF CFFF CFFF CFCF"
	$"FFCF FFFD CF02 9A9A CFFE 9A05 FFCF FFCF FFFF FDCF 00FF FECF 039A 9ACF CFFE 9A02"
	$"FFCF FFF3 CF00 FFA5 CE01 0000 FBCE 0000 FDCE 0100 00FD CEFE 00FE CE03 0000 CECE"
	$"FE00 FECE 0100 00FE CEFE 00FE CEFE 00FB CE01 0000 F4CE 0100 00FB CE00 00FD CE01"
	$"0000 FECE 0100 00FD CE01 0000 FECE 0100 00FC CE01 0000 FCCE FE00 FECE 0000 FDCE"
	$"0100 00FD CE01 0000 FECE 0100 00FE CEFE 00EF CE01 0000 FDCE 0100 00FE CE05 0000"
	$"CECE 0000 F0CE FBFF 06CE CE9C 9CCE 9CCE FE9C 03CE FFFF CEF9 FF05 CE9C CECE 9CCE"
	$"FE9C 00CE F6FF 06CE FF9C CE9C 9CCE FE9C FBFF 07CE FFCE FFFF CEFF CEFA FF01 CEFF"
	$"FA9C 00CE F6FF 01CE FFFE CEFC 9C00 CEF7 FF0A CEFF FFCE 9C9C CE9C CECE 9CFE FFF3"
	$"CE00 FF02 D6A5 CEFE 00FD CE01 0000 FDCE 0100 00FD CE01 0000 FDCE 0500 00CE CE00"
	$"00FE CE01 0000 FDCE 0100 00FC CEFE 00FC CE01 0000 F4CE FE00 FDCE 0100 00FD CE01"
	$"0000 FECE 0100 00FD CE01 0000 FECE 0100 00FC CE01 0000 FCCE 0100 00FE CE01 0000"
	$"FDCE 0100 00FE CE01 0000 FECE 0100 00FC CEFE 00F0 CE01 0000 FECE 0100 00FE CE01"
	$"0000 FECE 0000 EFCE 06FF CECE 9CCE CEFF FECE 0E9C 9CCE 9CCE 9C9C CECE 9CCE CE9C"
	$"CE9C FBCE FE9C FECE FD9C FDCE 0D9C 9CCE FFCE FF9C 9CCE 9C9C CE9C 9CFE CE08 9C9C"
	$"CE9C CE9C CE9C 9CFE CE0C 9CCE CE9C CECE FFFF CECE 9C9C CEFE 9CFA CE06 9C9C CECE"
	$"FFCE CEFD 9C00 CEFE 9C05 CECE 9CCE CE9C FDCE FEFF F99C 02CE CE9C F3CE 00FF A5CF"
	$"FE00 FDCF 0100 00FD CF01 0000 FDCF 0100 00FD CF05 0000 CFCF 0000 FECF 0100 00FD"
	$"CF01 0000 FCCF FE00 FCCF 0100 00F4 CFFE 00FD CF01 0000 FDCF 0100 00FE CF01 0000"
	$"FDCF 0100 00FE CF01 0000 FCCF 0100 00FC CF01 0000 FECF 0100 00FD CF01 0000 FECF"
	$"0100 00FE CF01 0000 FCCF FE00 F0CF 0100 00FE CF01 0000 FECF 0100 00FE CF00 00EF"
	$"CF00 FFFE CF02 FFCF FFFE CF05 9A9A CF9A CF9A FCCF 01FF FFFE CF00 FFFD CFFE 9A03"
	$"CFCF 9A9A FCCF 00FF FECF 0DFF CFFF FF9A 9ACF 9A9A CF9A 9ACF FFF7 CF00 FFFA CFFE"
	$"FF0C CFCF 9A9A CF9A 9ACF CFFF CFCF FFFB CF0E FFCF CF9A CF9A 9ACF 9A9A CFCF FFCF"
	$"FFFB CFFE FF00 CFFD 9A04 CFCF 9ACF FFF2 CF00 FFA5 CEFE 00FD CE01 0000 FDCE 0100"
	$"00FD CE01 0000 FDCE 0500 00CE CE00 00FE CE01 0000 FDCE 0100 00FC CEFE 00FC CE01"
	$"0000 F4CE FE00 FDCE 0100 00FD CE01 0000 FECE 0100 00FD CE01 0000 FECE 0100 00FC"
	$"CE01 0000 FCCE 0100 00FE CE01 0000 FDCE 0100 00FE CE01 0000 FECE 0100 00FC CEFE"
	$"00F0 CE01 0000 FECE 0100 00FE CE01 0000 FECE 0000 EFCE FEFF 00CE FEFF FECE 099C"
	$"9CCE 9CCE 9CCE FFFF CEFC FF05 CEFF CECE 9CCE FE9C FECE 009C F7FF 0ACE FFFF 9C9C"
	$"CE9C 9CCE 9C9C FEFF 03CE FFFF CEFE FF00 CEF5 FF07 CECE 9C9C CE9C 9CCE F9FF 05CE"
	$"FFCE FFFF CEFD 9C06 CE9C 9CCE FFFF CEF7 FF00 CEFA 9CFE FFF3 CE00 FF02 DAA4 CEFE"
	$"00FE CE01 0000 FDCE 0100 00FD CE01 0000 FDCE 0000 FECE 0100 00FE CE01 0000 FDCE"
	$"0100 00FB CE01 0000 FCCE 0100 00F3 CEFE 00FE CE01 0000 FDCE 0100 00FE CE01 0000"
	$"FDCE 0100 00FE CE01 0000 FCCE 0100 00FC CE01 0000 FECE 0100 00FD CE01 0000 FECE"
	$"0100 00FE CE01 0000 FBCE 0100 00F1 CE01 0000 FDCE 0100 00FE CE05 0000 CECE 0000"
	$"EFCE 00FF FE9C 03CE FFFF CEFD FF01 CE9C FDCE 029C CE9C FDCE 019C CEFE FF15 CEFF"
	$"CE9C CECE 9CCE FFFF CE9C CE9C CE9C 9CCE FFCE CEFF FBCE 029C CECE F49C 06FF CECE"
	$"9CCE CEFF FECE 02FF CE9C FCCE 119C CECE 9CCE CE9C CEFF FFCE FFFF CEFF CE9C FFFB"
	$"CE04 9CCE 9CCE CEFD FF03 CECE FF9C FECE 02FF FF9C F2CE 00FF A4CF FE00 FECF 0100"
	$"00FD CF01 0000 FDCF 0100 00FD CF00 00FE CF01 0000 FECF 0100 00FD CF01 0000 FBCF"
	$"0100 00FC CF01 0000 F3CF FE00 FECF 0100 00FD CF01 0000 FECF 0100 00FD CF01 0000"
	$"FECF 0100 00FC CF01 0000 FCCF 0100 00FE CF01 0000 FDCF 0100 00FE CF01 0000 FECF"
	$"0100 00FB CF01 0000 F1CF 0100 00FD CF01 0000 FECF 0500 00CF CF00 00EF CF00 FFFD"
	$"CF02 FFFF CFFD FFFB CF00 FFFD CF00 FFFE CFFE FF02 CFFF FFFC CF06 FFFF CFFF CFCF"
	$"FFFE CF06 FFCF CFFF CFCF FFFE CF03 9ACF CF9A F5CF FEFF 049A FFCF FFCF FEFF FCCF"
	$"08FF CF9A CFCF FFFF CFCF FEFF 10CF FFFF CFFF CFCF FFCF CFFF CFFF CFCF FFCF FBFF"
	$"02CF CFFF FDCF 03FF FF9A FFF3 CF00 FFA4 CEFE 00FE CE01 0000 FDCE 0100 00FD CE01"
	$"0000 FDCE 0000 FECE 0100 00FE CE01 0000 FDCE 0100 00FB CE01 0000 FCCE 0100 00F3"
	$"CEFE 00FE CE01 0000 FDCE 0100 00FE CE01 0000 FDCE 0100 00FE CE01 0000 FCCE 0100"
	$"00FC CE01 0000 FECE 0100 00FD CE01 0000 FECE 0100 00FE CE01 0000 FBCE 0100 00F1"
	$"CE01 0000 FDCE 0100 00FE CE05 0000 CECE 0000 EFCE FDFF 03CE FFFF CEFD FFFA CEFA"
	$"FF00 CEFE FF01 CEFF FBCE FAFF FECE 03FF CECE FFFB CE0C 9CCE CE63 FFFF CEFF CEFF"
	$"CEFF CEFB FF04 CEFF CEFF CEFE FFFA CE00 63FD FF00 CEFD FF07 CEFF FFCE FFCE CEFF"
	$"FDCE FDFF 00CE FBFF 02CE CEFF FDCE 03FF FF63 FFF3 CE00 FF02 CAA3 CE01 0000 FECE"
	$"0100 00FD CE00 00FD CE01 0000 FDCE 0100 00FE CE01 0000 FECE 0100 00FD CE01 0000"
	$"FACE 0100 00FE CE01 0000 F1CE 0100 00FE CE01 0000 FDCE 0000 FECE 0100 00FE CE02"
	$"00CE 00FE CE01 0000 FCCE 0100 00FB CE01 0000 FECE 0100 00FD CE00 00FD CE01 0000"
	$"FECE 0100 00FA CE01 0000 F2CE 0100 00FD CE01 0000 FECE 0500 00CE CE00 00EF CE00"
	$"FFFE CE03 FFFF CECE FDFF FECE 04FF CECE FF9C FCCE 059C FFFF CEFF CEFE FFFD CE02"
	$"FFFF 9CFE CE05 9CCE CEFF FFCE FCFF FECE 02FF CEFF FECE 079C CECE 9CCE CE9C CEFE"
	$"9C04 CECE 9CCE FFFD CE01 FFFF FCCE 02FF CE9C FBCE 06FF CECE FFCE FFFF FBCE 01FF"
	$"9CFB CEFA FFFC CE02 FFCE 9CF3 CE00 FFA3 CF01 0000 FECF 0100 00FD CF00 00FD CF01"
	$"0000 FDCF 0100 00FE CF01 0000 FECF 0100 00FD CF01 0000 FACF 0100 00FE CF01 0000"
	$"F1CF 0100 00FE CF01 0000 FDCF 0000 FECF 0100 00FE CF02 00CF 00FE CF01 0000 FCCF"
	$"0100 00FB CF01 0000 FECF 0100 00FD CF00 00FD CF01 0000 FECF 0100 00FA CF01 0000"
	$"F2CF 0100 00FD CF01 0000 FECF 0500 00CF CF00 00EF CFFE FF04 CFFF FFCF CFFD FFFE"
	$"CF0D FFCF CFFF CFFF FFCF CFFF CFFF FFCF FCFF FDCF 04FF FFCF CFFF FDCF 02FF FFCF"
	$"FAFF 03CF FFCF FFF9 CF00 FFFC CFFE FF01 CFFF FDCF 09FF FFCF CFFF CFCF FFCF FFFE"
	$"CF09 FFCF CFFF CFCF FFCF FFFF FBCF 07FF CFCF FFCF FFCF CFFA FFFD CF01 FFFF F1CF"
	$"00FF A3CE 0100 00FE CE01 0000 FDCE 0000 FDCE 0100 00FD CE01 0000 FECE 0100 00FE"
	$"CE01 0000 FDCE 0100 00FA CE01 0000 FECE 0100 00F1 CE01 0000 FECE 0100 00FD CE00"
	$"00FE CE01 0000 FECE 0200 CE00 FECE 0100 00FC CE01 0000 FBCE 0100 00FE CE01 0000"
	$"FDCE 0000 FDCE 0100 00FE CE01 0000 FACE 0100 00F2 CE01 0000 FDCE 0100 00FE CE05"
	$"0000 CECE 0000 EFCE FBFF 01CE CEFD FFFE CE02 FFFF CEF7 FF02 CEFF CEFE FFFD CE02"
	$"FFFF CEF9 FF00 CEFB FF05 CECE FFCE FFCE F8FF 02CE FFCE FDFF 01CE FFFD CEFE FFFD"
	$"CE01 FFCE FBFF 07CE FFCE CEFF CEFF FFFC CEFA FF01 CECE FAFF FDCE 03FF FFCE FFF3"
	$"CE00 FF02 F8A3 CE01 0000 FECE 0100 00FE CE01 0000 FDCE 0100 00FD CE00 00FD CE05"
	$"0000 CECE 0000 FDCE 0300 00CE CEFE 00FE CE01 0000 FECE 0400 00CE 0000 F4CE 0100"
	$"00FE CE01 0000 FECE 0100 00FE CE09 0000 CECE 00CE 0000 CECE FE00 FCCE 0E00 00CE"
	$"0000 CECE 0000 CECE 00CE 0000 FECE 0100 00FD CE05 0000 CECE 0000 FECE 0100 00FE"
	$"CE01 0000 F2CE 0100 00FD CE05 0000 CECE 0000 FECE 0100 00FD CE00 00F4 CE04 FFCE"
	$"CE9C CEFE FF03 CECE FFFF FECE 009C FECE 059C CECE 9CCE 9CF8 CE01 9C9C FDCE 0C9C"
	$"CECE 9CCE 9C9C CECE FFCE FFFF FDCE 01FF 9CFE CE08 9CCE CE9C 9CCE 9C9C CEFE 9CF7"
	$"CE06 FFCE CEFF CECE FFFB CE02 9CCE 9CFD CE05 FFCE FF9C CEFF FCCE 009C FCCE 00FF"
	$"F4CE 009C F3CE 00FF A3CF 0100 00FE CF01 0000 FECF 0100 00FD CF01 0000 FDCF 0000"
	$"FDCF 0500 00CF CF00 00FD CF03 0000 CFCF FE00 FECF 0100 00FE CF04 0000 CF00 00F4"
	$"CF01 0000 FECF 0100 00FE CF01 0000 FECF 0900 00CF CF00 CF00 00CF CFFE 00FC CF0E"
	$"0000 CF00 00CF CF00 00CF CF00 CF00 00FE CF01 0000 FDCF 0500 00CF CF00 00FE CF01"
	$"0000 FECF 0100 00F2 CF01 0000 FDCF 0500 00CF CF00 00FE CF01 0000 FDCF 0000 F4CF"
	$"00FF FDCF FEFF 06CF CFFF FFCF CFFF FECF 01FF FFFD CF04 FFCF FFCF FFF5 CF0B FFCF"
	$"CFFF CFCF FFCF FFCF FFFF FDCF 07FF CFFF CFFF CFCF FFF2 CF00 FFFE CF0B FFCF CFFF"
	$"CFCF FFCF FFCF CFFF F9CF 08FF CFFF CFCF FFCF CFFF FECF 05FF CFFF CFCF FFFD CF00"
	$"FFFA CF00 FFF2 CF00 FFA3 CE01 0000 FECE 0100 00FE CE01 0000 FDCE 0100 00FD CE00"
	$"00FD CE05 0000 CECE 0000 FDCE 0300 00CE CEFE 00FE CE01 0000 FECE 0400 00CE 0000"
	$"F4CE 0100 00FE CE01 0000 FECE 0100 00FE CE09 0000 CECE 00CE 0000 CECE FE00 FCCE"
	$"0E00 00CE 0000 CECE 0000 CECE 00CE 0000 FECE 0100 00FD CE05 0000 CECE 0000 FECE"
	$"0100 00FE CE01 0000 F2CE 0100 00FD CE05 0000 CECE 0000 FECE 0100 00FD CE00 00F4"
	$"CEFD FF00 CEFE FF03 CECE FFFF FBCE FCFF 07CE FFFF CEFF CECE FFFA CEFC FF00 CEFE"
	$"FF00 CEFC FFFD CE02 FFCE CEFA FF05 CEFF CEFF FFCE FBFF 0ACE CEFF CECE FFCE CEFF"
	$"CECE F7FF 0CCE FFCE CEFF CEFF CECE FFCE FFCE F7FF F7CE 01FF FFF2 CE00 FF03 2DAB"
	$"CE01 0000 FBCE 0100 00FE CEFE 0002 CECE 00FC CE01 0000 FECE 0000 FDCE 0100 00FE"
	$"CEFD 0008 CECE 0000 CE00 CE00 00FE CE00 00FD CEFD 00FB CE01 0000 FBCE 0100 00FE"
	$"CEFE 0002 CECE 00FD CEFD 0001 CECE FD00 00CE FD00 FECE FD00 FECE 0500 00CE 00CE"
	$"CEFE 0002 CECE 00FD CE01 0000 FECE FD00 02CE 0000 FECE 0000 FECE 0100 00F7 CEFE"
	$"00FE CE01 0000 FECE FD00 02CE 0000 FECE 0300 CE00 00F6 CE05 FF9C 9CCE CE9C FDCE"
	$"FE9C 04CE CE9C 9C63 FECE 069C CE9C CE9C CE9C FBCE 03FF FF9C 9CFB CEFE 9C06 CE9C"
	$"CEFF CE9C 9CFD CEFE 9C03 CECE 9CCE FE9C 00CE FE9C 01CE CEFD 9CFE CE09 FFCE CE9C"
	$"CECE 9CCE 9C9C FDCE 039C 9CCE 9CFE CE05 FFCE CE9C CECE FE9C 07CE 9C9C CECE 9C9C"
	$"CEFE 9C0A CEFF FF9C CE9C CE9C CECE 9CF0 CE00 FFAB CF01 0000 FBCF 0100 00FE CFFE"
	$"0002 CFCF 00FC CF01 0000 FECF 0000 FDCF 0100 00FE CFFD 0008 CFCF 0000 CF00 CF00"
	$"00FE CF00 00FD CFFD 00FB CF01 0000 FBCF 0100 00FE CFFE 0002 CFCF 00FD CFFD 0001"
	$"CFCF FD00 00CF FD00 FECF FD00 FECF 0500 00CF 00CF CFFE 0002 CFCF 00FD CF01 0000"
	$"FECF FD00 02CF 0000 FECF 0000 FECF 0100 00F7 CFFE 00FE CF01 0000 FECF FD00 02CF"
	$"0000 FECF 0300 CF00 00F6 CF00 FFFE CF00 FFFE CF03 FFFF CF9A FECF FE9A 02CF CFFF"
	$"FBCF 029A CFFF FDCF 03FF FF9A 9AFE FF03 CFCF FF9A FECF 029A FFFF FACF 079A 9AFF"
	$"CFFF CFCF FFF9 CF00 FFFE CF03 FFCF CFFF FACF 009A FECF 07FF CFFF CFFF CFFF CFFE"
	$"FFFC CF03 9A9A CF9A FEFF FECF 02FF FF9A FEFF 02CF CF9A FDCF 039A FFCF FFF3 CF00"
	$"FFAB CE01 0000 FBCE 0100 00FE CEFE 0002 CECE 00FC CE01 0000 FECE 0000 FDCE 0100"
	$"00FE CEFD 0008 CECE 0000 CE00 CE00 00FE CE00 00FD CEFD 00FB CE01 0000 FBCE 0100"
	$"00FE CEFE 0002 CECE 00FD CEFD 0001 CECE FD00 00CE FD00 FECE FD00 FECE 0500 00CE"
	$"00CE CEFE 0002 CECE 00FD CE01 0000 FECE FD00 02CE 0000 FECE 0000 FECE 0100 00F7"
	$"CEFE 00FE CE01 0000 FECE FD00 02CE 0000 FECE 0300 CE00 00F6 CE04 FFCE CEFF FFFB"
	$"CE00 9CFE CEFE 9CFA FF03 CEFF 9CFF FCCE 03FF FF9C CEFB FF00 CEFE FF05 9CCE FFCE"
	$"CE9C FDCE 019C CEFD FF06 CEFF FFCE FFFF CEFD FF01 CECE FCFF 02CE CE9C FCCE 009C"
	$"FCFF 00CE FEFF 02CE CEFF FECE 069C CECE 9C9C CECE FDFF 00CE FEFF 069C CEFF FFCE"
	$"CE9C FDCE 009C FEFF F3CE 00FF 02A9 ABCE 0100 00FC CE01 0000 FCCE FD00 FBCE FC00"
	$"FCCE 0100 00FE CE01 0000 FDCE FE00 FECE FD00 FCCE 0100 00F9 CE01 0000 FCCE 0100"
	$"00FC CEFD 00FC CEFE 00FE CE01 0000 FECE 0100 00FC CE01 0000 FCCE FE00 FDCE FD00"
	$"FCCE 0100 00FE CE01 0000 FDCE FD00 FDCE 0100 00F9 CEFB 0003 CECE 0000 FECE 0100"
	$"00FD CEFD 0003 CECE 0000 F6CE 00FF FDCE 009C FACE 019C CEFE 9C01 CE9C FBCE 009C"
	$"FCCE 009C FECE 049C 9CCE CE9C FACE 009C FDCE 009C FECE FE9C 02CE CE9C FECE 0B9C"
	$"CE9C CE9C 9CCE CE9C CECE 9CFB CE06 9CCE CE9C CE9C 9CEF CEFD 9CF9 CE01 9CFF FECE"
	$"009C FECE 059C 9C63 CEFF 9CF3 CE00 FFAB CF01 0000 FCCF 0100 00FC CFFD 00FB CFFC"
	$"00FC CF01 0000 FECF 0100 00FD CFFE 00FE CFFD 00FC CF01 0000 F9CF 0100 00FC CF01"
	$"0000 FCCF FD00 FCCF FE00 FECF 0100 00FE CF01 0000 FCCF 0100 00FC CFFE 00FD CFFD"
	$"00FC CF01 0000 FECF 0100 00FD CFFD 00FD CF01 0000 F9CF FB00 03CF CF00 00FE CF01"
	$"0000 FDCF FD00 03CF CF00 00F6 CF00 FFFD CF01 9AFF FBCF 019A CFFE 9A09 CF9A FFCF"
	$"FFCF FFCF CFFF FCCF 10FF CFCF 9A9A CFCF 9ACF CFFF CFFF FFCF 9AFF FCCF 01FF CFFE"
	$"9A02 CFCF 9AFD CF00 FFF7 CF05 FFCF FFCF FFFF FDCF FE9A FDCF 04FF CFFF CFFF FECF"
	$"00FF FCCF 089A 9ACF 9AFF CFFF CFCF FEFF 049A FFCF CFFF FDCF FE9A 01CF 9AF2 CF00"
	$"FFAB CE01 0000 FCCE 0100 00FC CEFD 00FB CEFC 00FC CE01 0000 FECE 0100 00FD CEFE"
	$"00FE CEFD 00FC CE01 0000 F9CE 0100 00FC CE01 0000 FCCE FD00 FCCE FE00 FECE 0100"
	$"00FE CE01 0000 FCCE 0100 00FC CEFE 00FD CEFD 00FC CE01 0000 FECE 0100 00FD CEFD"
	$"00FD CE01 0000 F9CE FB00 03CE CE00 00FE CE01 0000 FDCE FD00 03CE CE00 00F6 CEFC"
	$"FF01 9CFF FBCE 089C CE9C 9CCE CE9C CECE FCFF F8CE 009C FECE 019C CEFB FF00 9CF9"
	$"CEFE 9C08 CE9C 9CCE FFFF CEFF CEFE FF01 CECE FAFF FACE FE9C 03CE 9CCE CEFC FFF8"
	$"CEFE 9CFD CEFC FF00 9CFE FFFD CEFE 9C02 CECE 9CF2 CE00 FF01 B6AB CEFE 00FE CE01"
	$"0000 F2CE FE00 D5CE FE00 FECE 0100 00B6 CE00 00D2 CE07 FFCE CE9C CE9C CEFF FBCE"
	$"049C 9CCE 9C9C FDFF 02CE CE9C FECE 049C CEFF CECE FE9C FECE 04FF FF9C FFCE FE9C"
	$"06CE FFCE 9CCE CEFF FECE 039C 9CCE 9CFE CE08 9CCE 9CCE 9C9C FF9C 9CFB CE00 FFFE"
	$"CE04 FFCE 9CCE CEFD 9C07 FFFF 9CCE CE9C CE9C FCCE 0CFF CECE 9CCE 9CCE 9CCE FF9C"
	$"CE9C FDCE 019C 9CF8 CE01 9CFF F3CE 00FF ABCF FE00 FECF 0100 00F2 CFFE 00D5 CFFE"
	$"00FE CF01 0000 B6CF 0000 D2CF FEFF FDCF 00FF FECF 07FF CFFF 9A9A CF9A CFFC FF01"
	$"CF9A FDCF 03FF FFCF CFFE 9AFE CF04 FFFF 9AFF FFFE 9A06 CFFF CFCF FFCF FFFE CF07"
	$"9A9A CF9A FFCF CFFF FCCF 00FF FECF 01FF FFFE CF00 FFFE CF0C FFCF 9ACF CF9A CF9A"
	$"9AFF FF9A FFFE CF01 9AFF FDCF 00FF FDCF 069A CF9A CFFF CFFF FCCF 049A 9ACF CFFF"
	$"FBCF 019A FFF3 CF00 FFAB CEFE 00FE CE01 0000 F2CE FE00 D5CE FE00 FECE 0100 00B6"
	$"CE00 00D2 CEFE FF04 9CCE 9CCE FFFB CE04 9CCE CE9C 9CFC FF0B CE9C CECE FFCE CEFF"
	$"CECE 9C9C FDCE 04FF FF63 FFFF FE9C 01CE FFFD CE00 FFFB CE00 9CF2 FFFE CE00 FFFE"
	$"CE02 FFCE 9CFE CE08 9CCE 9CFF FF63 FFCE 9CFA CE00 FFFB CE04 9CCE FF9C FFFD CE03"
	$"FF9C 9CFF FBCE 039C CE9C FFF3 CE00 FF01 9BA9 CEFD 00F0 CE01 0000 D2CE FD00 B4CE"
	$"0000 D2CE 00FF FECE 00FF F9CE 089C CECE 9CCE 9C9C FFFF FDCE 00FF FBCE 049C CECE"
	$"9CCE FE9C 01FF FFFD CE03 FFCE CEFF FDCE 049C CE9C 9CCE FE9C 02FF 9CCE FE9C 05CE"
	$"CE9C 9CCE 9CFE CE03 FFFF CEFF FBCE 019C CEFE 9C06 FFFF CE9C CECE FFFA CE0F 9CCE"
	$"CE9C CE9C 9CCE FFFF CECE FFFF CEFF FECE 00FF FECE FB9C F3CE 00FF A9CF FD00 F0CF"
	$"0100 00D2 CFFD 00B4 CF00 00D2 CF00 FFFE CF00 FFFE CF01 FFFF FECF 0D9A FF9A CFCF"
	$"9A9A FFFF CFFF CFFF FFFC CF05 FFCF FFFF 9ACF FE9A 01FF FFFD CF03 FFCF CFFF FECF"
	$"05FF CFFF CF9A CFFE 9A00 FFF7 CF00 FFFE CF03 FFFF CFFF FECF 0EFF CFFF 9ACF 9A9A"
	$"CFFF FFCF CFFF CFFF FDCF 00FF FDCF 08FF 9ACF 9A9A CFFF FFCF FEFF 00CF FEFF 01CF"
	$"FFFE CFFE 9A02 CF9A 9AF3 CF00 FFA9 CEFD 00F0 CE01 0000 D2CE FD00 B4CE 0000 D2CE"
	$"FEFF 01CE FFFE CE01 FFFF FECE 069C CE9C CECE 9C9C FAFF FECE 00FF FCCE 019C CEFE"
	$"9CFD FF07 CECE FFCE CEFF CEFF FCCE 019C CEFE 9CFE FF06 CEFF CEFF CEFF CEFD FF03"
	$"CEFF FFCE FEFF FBCE FE9C 02FF FFCE FDFF 04CE CEFF CEFF FCCE 049C CE9C 9CCE FBFF"
	$"FCCE 00FF FCCE FD9C F3CE 00FF 012D 94CE 0100 0083 CE01 0000 D1CE FBFF 01CE CEFE"
	$"FF02 CECE FFFA CE03 FFCE FFCE F9FF F7CE 009C F7FF FDCE 0D9C CECE 9C9C FFCE CE9C"
	$"CE9C CE9C CEFE 9CFB FF00 CEFE FF03 CECE FF9C FECE 009C FECE 06FF CEFF CEFF FFCE"
	$"FDFF 02CE CEFF FDCE 049C CECE FFCE FEFF 00CE FCFF FDCE 039C CE9C 9CF3 CE00 FF94"
	$"CF01 0000 83CF 0100 00D1 CFFB FF07 CFFF CFCF FFFF CFFF FACF F5FF F7CF F6FF FDCF"
	$"069A CFCF 9ACF FFFF FDCF 05FF FFCF CFFF 9AF7 FF03 CFCF FF9A FECF 0A9A CFFF CFFF"
	$"CFFF CFFF FFCF FCFF 01CF FFFD CF04 9ACF CFFF CFFE FF00 CFFB FFFC CF00 9AF2 CF00"
	$"FF94 CE01 0000 83CE 0100 00D1 CEFB FF07 CEFF CECE FFFF CEFF FACE F5FF F7CE F5FF"
	$"FECE 099C CECE 9C9C FFCE FFFF CEFB FF00 CEFB FF00 CEFE FF03 CECE FF9C FBCE FDFF"
	$"03CE FFFF CEFD FF02 CECE FFFD CE02 9CCE CEF6 FFFA CE00 9CF3 CE00 FF01 6C94 CE01"
	$"0000 81CE D1CE 00FF FBCE 0D9C CECE 9C9C 63CE 9C9C CE9C CECE 9CFE CE00 FFFC CE01"
	$"9CCE FC9C FDCE 019C 9CFB CE11 9C9C CE9C 9CCE 9CCE CE9C 9CCE 9CCE CE9C FFCE F99C"
	$"05CE CEFF CECE 9CFD CEFE 9C12 CE9C CECE 9CCE 9C9C CECE FFCE CE9C CE9C CE9C 63FE"
	$"9C06 CE9C CECE 9CCE 9CFB CEF7 9C01 CE9C F2CE 00FF 94CF 0100 0081 CFD1 CF03 FFCF"
	$"CFFF FACF 019A 9AFE CF00 9AFC CF02 FFCF FFFC CF01 9ACF FD9A FCCF 029A 9AFF FECF"
	$"08FF CF9A 9ACF CF9A CF9A FECF 069A CF9A CFCF 9AFF F6CF 01FF FFFB CF01 9A9A FACF"
	$"049A 9ACF FFFF FECF 029A 9ACF FD9A FCCF 049A CF9A CFFF FDCF FA9A FDCF 009A F2CF"
	$"00FF 94CE 0100 0081 CED1 CE00 FFFB CE00 9CFD CE00 9CFE CE00 9CFE CE04 9CCE FFCE"
	$"FFFC CE00 9CFE CE01 9C9C FACE 00FF FCCE 0C9C 9CCE CE9C CE9C 9CCE CE9C CE9C FECE"
	$"08FF FFCE FFFF CEFF CEFF FECE 00FF FECE 009C FDCE 089C CE9C 9CCE CE9C CE9C FECE"
	$"00FF FECE 149C 9CCE 9CCE CE9C CECE 9CCE CE9C CE9C CECE FFFF CECE FD9C 02CE 9CCE"
	$"FE9C 01CE 9CF2 CE00 FF01 7B95 CEFE 0081 CED1 CEFE FF04 CEFF 9CCE CEFE 9C0C 63CE"
	$"9CCE 9C9C CE9C CECE 9C9C FFFE CE01 9CCE FD9C FECE FD9C 04CE CE9C CECE FEFF FC9C"
	$"02CE 63CE FB9C 0BCE 9CCE CE9C CE9C 9CCE 9CCE 9CFD CE00 FFFE CEFC 9C02 CE9C CEFB"
	$"9CFD CE04 9CCE CE9C 9CFE CE09 9CCE 9CCE 9C9C CECE 9CFF FECE 019C CEFE 9C02 CE63"
	$"CEFB 9CF2 CE00 FF95 CFFE 0081 CFD1 CFFE FF01 CFFF FECF FE9A 0E65 CF9A CF9A 9ACF"
	$"9ACF CF9A CFFF CFFF FDCF 059A 9ACF 9ACF CFFD 9A03 CFCF 9ACF FDFF 05CF 9ACF 9A9A"
	$"CFFC 9A05 CF9A 9ACF 9AFF F4CF 03FF FFCF CFFD 9A00 CFFA 9A04 CF9A FFCF FFFD CF06"
	$"9A9A CFCF 9A9A CFFE 9AFE CF01 9AFF FCCF FE9A 02CF 9ACF FD9A 01CF 9AF2 CF00 FF95"
	$"CEFE 0081 CED1 CEFE FF04 CEFF 9CCE CEFE 9C0F 639C 9CCE 9C9C CE9C CECE 9CFF FFCE"
	$"FFCE FD9C 04CE 9C9C CECE FD9C 02CE 9C9C FCFF 159C 9CCE 9C9C CE9C 9CCE 9C9C CE9C"
	$"9CCE 9CFF FFCE FFFF CEFC FFFE CE00 FFFE CEFE 9C06 639C 9CCE CE9C CEFD 9C0C FFFF"
	$"CEFF 9CCE CE9C 9CCE 9C63 CEFC 9C03 CECE 9CFF FECE 009C FECE 069C CE9C 639C CECE"
	$"FE9C F2CE 00FF 0174 96CE FB00 81CE D3CE FEFF FCCE 089C 9CCE 9C9C CE9C CECE FD9C"
	$"00CE FEFF F9CE FE9C 01CE CEFC 9C01 FFFF FBCE 0AFF CE9C CE9C 9CCE CE9C 9CCE FE9C"
	$"06FF CE9C FF9C 9CCE FE9C 03FF FFCE FFFC CEFE 9C0B CE9C 9CCE 9CCE 9C9C CEFF CEFF"
	$"FCCE 02FF CECE FE9C 06CE 9CCE 9CCE 639C FDFF FCCE FE9C FDCE 029C CE9C F3CE 00FF"
	$"96CF FB00 81CF D3CF FEFF F8CF 099A CF9A CFCF 9ACF CF9A CFFE FF13 CFFF CFFF CFFF"
	$"CF9A CF9A 9ACF CF9A 9ACF 9A9A FFFF FDCF 13FF CFFF CF9A CF9A 9ACF CF9A 9ACF CF9A"
	$"9AFF CFCF FFFB CF03 FFFF CFFF FCCF 0E9A CF9A CF9A 9ACF 9ACF CF9A CFFF CFFF FCCF"
	$"00FF FECF 089A 9ACF 9ACF 9ACF 9A9A FDFF 07CF CFFF FFCF 9ACF 9AFD CF02 9ACF 9AF3"
	$"CF00 FF96 CEFB 0081 CED3 CEFE FFF9 CE03 9CCE CE9C FECE 039C 9CCE CEFE FF06 CEFF"
	$"CECE FFCE CEFD 9CFD CE04 9CCE 9CFF FFFB CE0A FFCE 9CCE 9C9C CECE 9CCE CEFE 9CFB"
	$"FF02 CEFF CEFE FF02 CEFF FFFD CE0E 9CCE 9CCE 9C9C CE9C CECE 9CCE FFCE FFFC CE00"
	$"FFFE CE03 9C9C CE9C FECE 019C 9CFD FFFC CE01 9C9C FACE 009C F3CE 00FF 013A 81CE"
	$"81CE E2CE FEFF 029C CEFF FCCE 009C FECE FA9C 03CE FFCE 9CFB CE01 9CCE FB9C 06CE"
	$"9C9C CEFF 9C9C FACE 029C CECE FC9C 00CE FB9C 08CE CE9C CECE 9CCE CE9C FACE 029C"
	$"9CCE FB9C 06CE 9CCE CE9C CEFF FCCE 039C 9CCE CEFA 9C03 CECE 9C9C FBCE 019C CEFB"
	$"9CF2 CE00 FF81 CF81 CFE2 CFFE FF03 9AFF CFFF FDCF 009A FECF 019A CFFE 9A03 CF9A"
	$"CFFF FACF 00FF FDCF FC9A 03CF 9ACF FFF5 CFFE 9A00 CFFD 9AF5 CF04 9ACF FFCF FFFE"
	$"CF04 9ACF CF9A CFFD 9A09 CF9A CFCF 9ACF FFCF CFFF FECF 0D9A CFCF 9ACF 9A9A CF9A"
	$"9ACF CF9A 9AFB CF01 9ACF FB9A F2CF 00FF 81CE 81CE E2CE FEFF 03CE CEFF FFF9 CE03"
	$"9CCE 9CCE FE9C 01FF FFF5 CEFA 9C00 FFFA CE00 FFFC CEFD 9C00 CEFE 9C08 FFCE FFFF"
	$"CEFF FFCE FFFE CE03 9CCE CEFF FDCE 049C CECE 9CCE FD9C 09CE 9CCE CE9C CEFF CECE"
	$"FFFE CE04 9CCE CE9C CEFC 9C03 CECE 9C9C F9CE FA9C F3CE 00FF 0140 81CE 81CE E2CE"
	$"03FF CECE 9CFD CE07 9C9C CECE FF9C FFFF FECE FD9C 01CE 9CFB CE04 9CCE CEFF FFFD"
	$"CEFE 9C00 CEFE 9CFE CE02 9CCE 9CFE CE03 FFCE FFFF FE9C 03CE 9CCE 9CFB CE01 9CCE"
	$"FE9C 03CE 9CCE FFFD CE05 FFCE FF9C CECE FC9C 01CE 9CFE CE03 FFCE CE9C F9CE F99C"
	$"07CE FFFF CE9C CEFF 9CFC CE00 9CF3 CE00 FF81 CF81 CFE2 CF03 FFCF CF9A F9CF 06FF"
	$"CFFF CFFF 9AFF FECF 02FF CF9A F8CF 01FF FFF8 CF01 9A9A F8CF 00FF FECF 009A FCCF"
	$"05FF CFCF FFCF FFFA CF07 9ACF FFCF CFFF CFFF F7CF 059A 9ACF FFCF FFF4 CF02 9ACF"
	$"CFFE 9AFE FFFE CF01 FF9A EDCF 00FF 81CE 81CE E2CE FEFF 009C F9CE 0BFF CEFF CEFF"
	$"CEFF FFCE FFFF CEFE 9CFA CE01 FFFF FCCE FEFF 02CE 9C9C F8CE 06FF CECE FF9C FFCE"
	$"F7FF 08CE FFCE CE9C CE9C CEFF FDCE 00FF FCCE 06FF FFCE FFCE 9C9C FECE 00FF F5CE"
	$"03FF FFCE CEFE 9C02 CEFF FFFE CE01 FF9C FECE 02FF CEFF F3CE 00FF 010F 81CE 81CE"
	$"E2CE 00FF FD9C 06CE 9C9C CECE 9C9C FECE 019C 9CFE CE00 9CFD CEFE 9C05 CE9C CE9C"
	$"CE9C FECE FD9C 03CE CE9C CEFD 9C0B CECE 9CCE 9CCE 9CCE 9C9C CE9C FECE 039C 9CCE"
	$"CEFD 9C02 CECE 9CFD CEFC 9C0F CE9C CECE 9CCE CE9C CECE 9CCE CE9C CECE FE9C 0FCE"
	$"9CCE 9C9C CE9C CE9C CE9C 9CCE 9C9C CEFE 9C00 CEFE 9C02 CECE 9CFC CE00 9CF2 CE00"
	$"FF81 CF81 CFE2 CF03 FFCF CF9A FECF 009A F9CF 009A FECF 029A CF9A FCCF 059A CFCF"
	$"9ACF 9AFB CF00 9AFE CF00 9AFB CF06 9ACF CF9A CFCF 9AFB CF00 9AFC CF03 9ACF CF9A"
	$"FACF 039A CFCF 9AF8 CF00 9AFC CF01 9A9A FECF 039A CFCF 9AFE CF02 9ACF 9AFE CF00"
	$"9AFE CF03 9ACF CF9A E9CF 00FF 81CE 81CE E2CE 03FF CECE 9CE4 CE00 9CE3 CE00 9CFC"
	$"CE00 9CE4 CE00 9CD0 CE00 FF01 1081 CE81 CEE2 CE03 FF9C 9CCE F69C 00CE FD9C 06CE"
	$"9C9C CE9C 9CCE F29C 02CE 9C9C FECE FA9C 03CE 9C9C CEFC 9C00 CEFE 9C02 CE9C CEFE"
	$"9C00 CEFB 9C00 CEFB 9C00 CEFC 9C00 CEFC 9C00 CEF5 9C07 CECE 9CCE CE9C CECE FD9C"
	$"03CE 9C9C CEFA 9CF3 CE00 FF81 CF81 CFE2 CF03 FF9A 9ACF FE9A 00CF F99A 00CF FE9A"
	$"03CF 9A9A CFED 9A00 CFFD 9A00 CFFD 9A00 CFFE 9A00 CFFC 9A00 CFFC 9A03 CF9A 9ACF"
	$"FA9A 00CF FB9A 00CF FC9A 00CF FC9A 00CF F69A 02CF 9ACF FE9A 03CF 9A9A CFFE 9A02"
	$"CF9A CFF9 9AF3 CF00 FF81 CE81 CEE2 CE03 FF9C 9CCE FD9C 01CE CEFC 9C00 CEFD 9C03"
	$"CE9C 9CCE F89C 00CE E99C 00CE FC9C 00CE FC9C 00CE F79C 00CE FB9C 00CE FC9C 00CE"
	$"FC9C 00CE F49C 03CE 9C9C CEFE 9C00 63FE 9C04 CECE 9C9C CEFB 9CF3 CE00 FF01 1381"
	$"CE81 CEE2 CE02 FF9C 9CFE CE00 9CFE CE10 9C9C CE9C CE9C CECE 9CCE 9CCE CE9C CE9C"
	$"9CFD CEFC 9C00 CEFE 9C03 CE9C 9CCE FD9C 0FCE CE9C CECE 9CCE 9C9C CE9C 9CCE CE9C"
	$"9CFD CEFE 9C00 CEFA 9C01 CECE FE9C FECE 0D9C CE9C CE9C CE9C CECE 9CCE CE9C 9CFE"
	$"CEFB 9C05 CE9C CE9C 9CCE FB9C FDCE 059C CE9C CE9C 9CFE CE00 9CF3 CE00 FF81 CF81"
	$"CFE2 CF00 FFFE 9AFE CF00 9AFE CF00 9AFD CF0B 9ACF 9ACF CF9A CFCF 9ACF CF9A FDCF"
	$"049A 9ACF CF9A FDCF 039A CFCF 9AFD CF04 9ACF 9ACF 9AFD CF00 9AF3 CF00 9AFD CF02"
	$"9ACF 9AFC CF00 9AFE CF00 9AF3 CF03 9ACF CF9A FECF 029A CF9A FECF 009A FECF FE9A"
	$"02CF CF9A EBCF 00FF 81CE 81CE E2CE 02FF 9C9C FECE 009C E8CE 019C 9CEE CE00 9CE9"
	$"CE00 9CE7 CE03 9CCE CE9C FCCE 009C DECE 00FF 0127 81CE 81CE E2CE 00FF FD9C 00CE"
	$"FD9C 01CE CEFC 9CFD CE05 9C9C CE9C CECE FE9C 06CE CE9C 9CCE CE9C FECE 039C CECE"
	$"9CFD CE0F 9C9C CE9C 9CCE 9CCE CE9C CE9C CE9C CECE FD9C 08CE 9CCE 9CCE CE9C CECE"
	$"FD9C FECE 029C CE9C FECE 069C CE9C 9CCE 9CCE FE9C 00CE FE9C 00CE FE9C 07CE CE9C"
	$"CE9C CE9C 9CFB CEFD 9C02 CE9C CEFC 9CF1 CE00 FF81 CF81 CFE2 CF00 FFFD CF00 9AFD"
	$"CF04 9ACF CF9A 9AFD CF00 9AF6 CF05 9ACF CF9A 9ACF FD9A FDCF FE9A 20CF CF9A CFCF"
	$"9ACF CF9A CFCF 9ACF CF9A 9ACF 9ACF CF9A CFCF 9ACF CF9A CFCF 9ACF 9ACF FE9A 03CF"
	$"9ACF 9AFE CF00 9AFD CF04 9ACF 9ACF 9AFE CF0E 9ACF 9A9A CF9A 9ACF CF9A CFCF 9ACF"
	$"CFFE 9AFA CFFE 9AFE CF00 9AF3 CF00 FF81 CE81 CEE2 CE00 FFF5 CE00 9CE7 CE00 9CE9"
	$"CE00 9CF6 CE02 9CCE 9CEF CE00 9CFA CE01 9C9C EBCE 019C 9CEF CE00 FF00 BD81 CE81"
	$"CEE2 CE00 FFE7 CE01 9C9C F6CE 009C F6CE 009C F7CE 009C FACE 009C C7CE 019C 9CEF"
	$"CE00 FF81 CF81 CFE2 CF00 FFFB CF03 FFCF CFFF FACF 01FF FFFD CF07 FFCF CF9A 9ACF"
	$"CFFF FDCF 00FF F9CF FEFF 1ACF CFFF CF9A CFCF FFFF CFCF FFCF CFFF 9AFF CFFF CFFF"
	$"CFCF 9AFF CFFF FACF 01FF FFFE CF00 FFFE CF00 FFFD CF03 FFCF CFFF FECF 00FF FCCF"
	$"02FF CFFF FDCF 00FF FCCF FCFF FECF 049A 9ACF CFFF F2CF 00FF 81CE 81CE E2CE 00FF"
	$"E7CE 019C 9CEA CE00 9CF7 CE00 9CFA CE00 9CC7 CE01 9C9C EFCE 00FF 014A 81CE 81CE"
	$"E2CE 00FF FDCE 00FF FDCE 06FF CECE FFCE CEFF FDCE 00FF F2CE FEFF F9CE 06FF FFCE"
	$"FFCE CEFF FCCE 02FF CEFF F3CE 03FF CECE FFFB CE00 FFFC CE03 FFCE CEFF FDCE 00FF"
	$"F9CE 00FF FDCE 00FF F6CE 08FF CEFF CECE FFCE CEFF F3CE 00FF 81CF 81CF E2CF 00FF"
	$"FDCF 01FF FFFE CF07 FFCF CFFF CFCF FFFF FECF 01FF FFFD CF02 FFCF FFFA CFFE FFFD"
	$"CF00 FFFE CF06 FFFF CFFF FFCF FFFC CF03 FFCF FFFF FACF 00FF FECF FEFF 02CF FFFF"
	$"FECF 07FF CFFF FFCF CFFF CFFE FF01 CFFF FDCF 04FF FFCF CFFF FDCF 0AFF CFFF CFCF"
	$"FFCF FFCF FFFF FBCF 01FF CFFC FF01 CFFF F3CF 00FF 81CE 81CE E2CE 00FF FDCE 00FF"
	$"FDCE 06FF CECE FFCE CEFF FDCE 00FF FBCE 00FF F9CE FEFF F9CE 06FF FFCE FFCE CEFF"
	$"FCCE 07FF CEFF CEFF CECE FFF8 CE06 FFCE CEFF CEFF FFFE CE00 FFFE CE0C FFCE FFCE"
	$"CEFF CEFF CECE FFCE FFFB CE05 FFCE CEFF CEFF FCCE 00FF FCCE 08FF CEFF CECE FFCE"
	$"CEFF F3CE 00FF 011D 81CE 81CE E2CE 0DFF 9C9C CEFF 9CCE CE9C 9CCE 9CCE 9CFE CE09"
	$"9CCE CE9C CECE 9CCE CEFF FECE 009C FECE 0AFF FFCE 9CCE CE9C FF9C CECE FE9C 08CE"
	$"9CCE CE9C CECE 9C9C FDCE 009C FACE 049C CE9C CE9C FECE 009C FECE 09FF CECE 9CCE"
	$"FFCE 9CFF 9CF9 CE06 FFCE CE9C CECE 9CFE CE0B 9CCE 9CCE CE9C CE9C CECE 9C9C FACE"
	$"009C FECE 009C F0CE 00FF 81CF 81CF E2CF 00FF FECF 00FF ECCF 00FF FBCF FEFF FCCF"
	$"00FF F1CF 00FF FCCF 00FF F1CF 0CFF CFCF 9ACF FFCF CFFF 9ACF CFFF FECF 02FF CFFF"
	$"FACF 00FF F0CF 00FF E9CF 00FF 81CE 81CE E2CE 00FF FECE 02FF CEFF F6CE 00FF FCCE"
	$"02FF CEFF FDCE 04FF CECE FFFF FCCE 02FF CEFF F4CE 00FF F3CE 02FF CEFF FECE 10FF"
	$"CECE FFCE CE9C FFFF CECE FF9C CEFF CEFF FDCE 00FF F8CE 00FF F4CE 01FF FFF9 CE00"
	$"FFF1 CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 1E81 CE81 CE81 CECC CE00 FF81 CF81 CF81 CFCC CF00 FF81 CE81 CE81"
	$"CECC CE00 FF00 00FF"
};








resource 'PPob' (kFind_DialogID, purgeable) {
	{
		ObjectData {
			DialogBox {
				kFind_DialogID,
				modal,
				noCloseBox,
				hasTitleBar,
				noResize,
				noSizeBox,
				noZoom,
				hasShowNew,
				enabled,
				hasTarget,
				noGetSelectClick,
				noHideOnSuspend,
				noDelaySelect,
				hasEraseOnUpdate,
				64,
				64,
				screenSize,
				screenSize,
				screenSize,
				screenSize,
				0,
				'find',
				'cncl'
			}
		},
		BeginSubs {
		},
		ObjectData {
			EditField {
				'ftxt',
				{241, 20},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				66,
				12,
				0,
				defaultSuperView,
				"",
				kTextEntryFieldTextTrait,
				255,
				hasBox,
				noWordWrap,
				hasAutoScroll,
				noTextBuffering,
				noOutlineHilite,
				noInlineInput,
				noTextServices,
				printingCharFilter
			}
		},
		ObjectData {
			Caption {
				0,
				{36, 16},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				22,
				14,
				0,
				defaultSuperView,
				"Find:",
				kStaticTextTrait
			}
		},
		ObjectData {
			StdCheckBox {
				'wrap',
				{104, 16},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				8,
				39,
				0,
				defaultSuperView,
				0,
				unchecked,
				kCheckBoxAndRBTextTrait,
				"Wrap at End",
				0
			}
		},
		ObjectData {
			StdCheckBox {
				'whol',
				{104, 16},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				112,
				39,
				0,
				defaultSuperView,
				0,
				unchecked,
				kCheckBoxAndRBTextTrait,
				"Whole Word",
				0
			}
		},
		ObjectData {
			StdCheckBox {
				'igcs',
				{104, 16},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				216,
				39,
				0,
				defaultSuperView,
				0,
				unchecked,
				kCheckBoxAndRBTextTrait,
				"Ignore Case",
				0
			}
		},
		ObjectData {
			StdButton {
				'find',
				{70, 20},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				176,
				65,
				0,
				defaultSuperView,
				'find',
				kButtonTextTrait,
				"Find",
				0
			}
		},
		ObjectData {
			StdButton {
				'cncl',
				{70, 20},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				83,
				65,
				0,
				defaultSuperView,
				'cncl',
				kButtonTextTrait,
				"Don't Find",
				0
			}
		},
		EndSubs {
		}
	}
};
resource 'WIND' (kFind_DialogID, purgeable) {
	{42, 4, 135, 322},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	"Find",
	alertPositionParentWindow
};
resource 'RidL' (kFind_DialogID, purgeable) {
	{
		'find',
		'cncl'
	}
};





resource 'MENU' (kOpenDLOGAdditionItems_DialogID) {
	kOpenDLOGAdditionItems_DialogID,
	textMenuProc,
	0x7FFFFFFF,
	enabled,
	"",
	{
		"All Recognized", noIcon, noKey, noMark, plain,
		"All Files", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain
	}
};
resource 'CNTL' (kOpenDLOGAdditionItems_DialogID, purgeable) {
	{0, 0, 20, 300},
	0,
	visible,
	47,
	kOpenDLOGAdditionItems_DialogID,
	popupMenuCDEFproc,
	0,
	"Show:"
};
resource 'DITL' (kOpenDLOGAdditionItems_DialogID, purgeable) {
	{
		{10, 12, 30, 312},
		Control {
			enabled,
			kOpenDLOGAdditionItems_DialogID
		},
	}
};










resource 'MENU' (kSaveDLOGAdditionItems_DialogID) {
	kSaveDLOGAdditionItems_DialogID,
	textMenuProc,
	0x7FFFFFFF,
	enabled,
	"",
	{
	}
};
resource 'CNTL' (kSaveDLOGAdditionItems_DialogID, purgeable) {
	{0, 0, 20, 320},
	0,
	visible,
	67,
	kSaveDLOGAdditionItems_DialogID,
	popupMenuCDEFproc,
	0,
	"Format:"
};
resource 'DITL' (kSaveDLOGAdditionItems_DialogID, purgeable) {
	{
		{10, 12, 30, 332},
		Control {
			enabled,
			kSaveDLOGAdditionItems_DialogID
		},
	}
};








resource 'PPob' (kGotoLine_DialogID, purgeable) {
	{
		ObjectData {
			DialogBox {
				kGotoLine_DialogID,
				modal,
				noCloseBox,
				hasTitleBar,
				noResize,
				noSizeBox,
				noZoom,
				hasShowNew,
				enabled,
				hasTarget,
				noGetSelectClick,
				noHideOnSuspend,
				noDelaySelect,
				hasEraseOnUpdate,
				64,
				64,
				screenSize,
				screenSize,
				screenSize,
				screenSize,
				0,
				'ok  ',
				'cncl'
			}
		},
		BeginSubs {
		},
		ObjectData {
			EditField {
				'line',
				{70, 20},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				108,
				16,
				0,
				defaultSuperView,
				"",
				kTextEntryFieldTextTrait,
				7,
				hasBox,
				noWordWrap,
				noAutoScroll,
				noTextBuffering,
				noOutlineHilite,
				noInlineInput,
				noTextServices,
				integerFilter
			}
		},
		ObjectData {
			Caption {
				0,
				{112, 16},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				16,
				18,
				0,
				defaultSuperView,
				"Goto Line:",
				kStaticTextTrait
			}
		},
		ObjectData {
			StdButton {
				'cncl',
				{59, 20},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				24,
				48,
				0,
				defaultSuperView,
				'cncl',
				kButtonTextTrait,
				"Cancel",
				0
			}
		},
		ObjectData {
			StdButton {
				'ok  ',
				{59, 20},
				visible,
				enabled,
				unbound,
				unbound,
				unbound,
				unbound,
				102,
				48,
				0,
				defaultSuperView,
				'ok  ',
				kButtonTextTrait,
				"OK",
				0
			}
		},
		EndSubs {
		}
	}
};
resource 'WIND' (kGotoLine_DialogID, purgeable) {
	{40, 16, 120, 210},
	movableDBoxProc,
	invisible,
	noGoAway,
	0x0,
	"Goto Line",
	alertPositionParentWindow
};
resource 'RidL' (kGotoLine_DialogID, purgeable) {
	{
		'cncl',
		'ok  '
	}
};










/// DO #DEFINES HERE TO CONTROL WHICH RESOURCES LOADED
#include	"LedStdDialogs.r"







// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
