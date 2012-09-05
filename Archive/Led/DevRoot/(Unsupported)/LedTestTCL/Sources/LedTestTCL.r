#define SystemSevenOrBetter 1			// we want the extended types
#define	SystemSevenOrLater	1			// Types.r uses this variable

#include	"BalloonTypes.r"
#include	"SysTypes.r"
#include	"Types.r"


#include	"LedConfig.hh"


// For now, define duplicated here and in CEditDoc.cpp
#define	kPickInputFileFormat_AlertID	9493



resource 'vers' (1) {
	0x2,
	0x0,
	development,
	0x2,
	verUS,
	qLed_ShortVersionString,
	"Led Test TCL" qLed_ShortVersionString
};
resource 'vers' (2) {
	0x2,
	0x0,
	development,
	0x2,
	verUS,
	qLed_ShortVersionString,
	"Led Test TCL" qLed_ShortVersionString
};

#define	kApplicationSignature	'LDAP'
#define	kDocumentFileType		'TEXT'
#define	kStationeryFileType		'sEXT'


#define	kDocument_FinderIconResID			5000
#define	kDocumentStationary_FinderIconResID	5001
#define	kApplicationFinderIconResID			5002
resource 'BNDL' (5000, "Application BNDL") {
	kApplicationSignature,
	0,
	{	/* array TypeArray: 2 elements */
		/* [1] */
		'FREF',
		{	/* array IDArray: 2 elements */
			/* [1] */
			0, 128,
			/* [2] */
			1, 129,
			/* [2] */
			2, 130,
			/* [2] */
			3, 131
		},
		/* [2] */
		'ICN#',
		{
			0, kDocument_FinderIconResID,
			1, kDocumentStationary_FinderIconResID,
			2, kApplicationFinderIconResID,
			3, 0			/* no ICN# reference since we provide no icon for this case */
		}
	}
};
resource 'FREF' (128) {
	kDocumentFileType,
	0,
	""
};
resource 'FREF' (129) {
	kStationeryFileType,
	1,
	""
};
resource 'FREF' (130) {
	'APPL',
	2,
	""
};
resource 'FREF' (131) {
	'****',
	3,
	""
};
resource 'ICN#' (kApplicationFinderIconResID) {
	{	/* array: 2 elements */
		/* [1] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FC00 0103 FC01 B903 FC00 0103 FC00 7903 FC00 0103"
		$"FC00 0103 FDF8 0103 FC00 0103 FDE8 0103 FC00 0103 FC00 0103 FD8C 1903 FC01 0117"
		$"FDF2 0233 FC06 046B FDCB 08D3 FC15 F1A3 FC2A A343 FC15 4683 FC2A 8D03 FC55 1A03"
		$"FCAA 3403 FC54 6803 FCA8 D003 FD51 2003 FCA1 8003 FD40 0003 FE80 0003 FFFF FFFF",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'icl8' (kApplicationFinderIconResID) {
	$"0000 0000 0000 0000 0000 0000 0000 0000 00F5 F5F5 F5F6 F6F6 F6F6 2B2B 2B2B F7F7"
	$"00FC FCFC FCFC FCFC FCFC FCFC FBFB FBFB 8181 81FA FAFA F9F9 F956 5656 56F8 F8F7"
	$"00FB FCFB FCFB FCFB FCFB FCFB FBFB 8181 81FA FAFA FAF9 F9F9 5656 56F8 F8F8 F8F7"
	$"00FC FCFC FCFC 0000 0000 0000 0000 0000 0000 0000 00F5 F62B 5656 56F8 F8F8 F7F8"
	$"00FC FCFB FCFB 0000 0000 0000 0000 0000 0000 0000 F5F6 2BF7 56F8 F8F8 F8F7 F7F8"
	$"00FC FBFC FCFC 0000 0000 0000 0000 AC00 ACAC 00AC ACFD FDF8 56F8 F8F8 F7F7 F7F8"
	$"00FC FCFB FCFC 0000 0000 0000 0000 0000 0000 00F5 F6F7 F856 F8F8 F8F7 F7F7 2BF8"
	$"00FC FBFC FBFC 0000 0000 0000 0000 0000 AC00 ACAC FDFD FD56 F8F7 F7F7 F72B 2B56"
	$"00FC FCFC FCFB 0000 0000 0000 0000 0000 0000 F52B F7F8 56F9 F7F7 F7F7 2B2B 2B56"
	$"00FC FBFC FBFB 0000 0000 0000 0000 0000 ACF8 F8F8 2BFD FDFA F7F7 F72B 2B2B F656"
	$"00FB FCFB FBFB 0000 0000 0000 0000 0000 00F6 2BF7 56F9 F9FA F7F7 2B2C FFF6 F656"
	$"00FC FBFB FB81 0000 0000 0000 0000 0000 F52B F756 56F9 FA81 2B2B 2C11 11F6 F656"
	$"00FB FBFB 8181 0000 ACAC ACAC ACAC 00F7 F6F7 F856 F9FA 8181 2B2B 1011 2C2B F6F9"
	$"00FB FB81 8181 0000 0000 0000 0000 00FA F8F8 56F9 FA81 81FB 2B2C 1110 2B2B F6F9"
	$"00FB 8181 81FA 0000 ACAC AC00 0000 56AC F956 F9FA 8181 FBFC 2B11 112C 2BF6 F5F9"
	$"0081 8181 FAFA 0000 0000 0000 0000 FBAC 56F8 FA81 FBFB 81FC 0F11 332B 2BF5 F5FA"
	$"F581 81FA FAFA 0000 0000 0000 00F9 FDFA F5F5 56FA 8156 56FB 1111 F72B F6F5 00FA"
	$"F581 FAFA FAF9 0000 00AC ACAC ACFC FBF6 F5F9 81FA 56FC FB34 1132 2B2B F500 00FA"
	$"F5FA FAFA F9F9 0000 0000 0000 FAAC 2BF5 F981 F9F9 8181 5611 102B 2BF6 F500 00FA"
	$"F5FA FAF9 F9F9 0000 00AC ACFC FD56 0056 81F9 81FC FAF7 1011 2C2B F6F5 0000 00FA"
	$"F6FA F9F9 F956 0000 0000 00FC FA00 F981 56F9 FB56 F607 1110 2B2B F500 0000 0081"
	$"F6F9 F9F9 5656 0000 00AC FCFC 00F9 FA56 FCFA F7F6 F610 11F7 2BF6 F500 0000 0081"
	$"F6F9 F956 5656 0000 0000 FDF6 56FA 8181 F92B F6F6 0811 332B F6F5 0000 0000 0081"
	$"F6F9 5656 56F8 0000 00FB 2B56 F9FC FB56 2BF6 F6F6 1011 2B2B F600 0000 0000 0081"
	$"2B56 5656 F8F8 0000 00FA F956 81FB 56F6 F6F5 F508 112C 2BF6 F500 0000 0000 00FB"
	$"2B56 56F8 F8F8 0000 5656 81AC FC56 F6F6 F5F6 F511 102B 2BF5 0000 0000 0000 00FB"
	$"2B56 F8F8 F8F8 0000 56FA ACFC F9F6 F6F6 F5F5 3210 2C2B F6F5 0000 0000 0000 00FB"
	$"2BF8 F8F8 F7F7 00F9 FAFC FBF8 F6F6 F5F5 F5F6 5C32 F6F6 F500 0000 0000 0000 00FB"
	$"2BF8 F8F7 F7F7 81FA FA56 2BF6 F6F5 F5F5 F5F7 5CF5 F5F5 0000 0000 0000 0000 00FC"
	$"F7F8 F7F7 F7F7 2B2B 2BF6 F6F6 F5F5 F5F5 00F5 F500 0000 0000 0000 0000 0000 00FC"
	$"F7F8 F7F7 2B2B 2B2B F6F6 F6F5 F5F5 F500 0000 0000 0000 0000 0000 0000 0000 00FC"
	$"F7F7 F8F8 F8F8 5656 5656 F9F9 F9F9 F9FA FAFA FA81 8181 81FB FBFB FBFB FCFC FCFC"
};
resource 'icl4' (kApplicationFinderIconResID) {
	$"EEEE EEEE EEEE EEEE EEEE EEEE EEEE EEEE EEEE EEEE EEEE EEEE EEED DDDD DDDD DDDE"
	$"EEEE EEEE EEEE EEEE DDDD DDDD DDDD DDCE EEEE EE00 0000 0000 0000 00CC DDDD CCCE"
	$"EEEE EE00 0000 0000 0000 0CCC DDDC CCCE EEEE EE00 0000 000E EEEE EAAC DDCC CCCE"
	$"EEEE EE00 0000 0000 0000 CCCD DCCC CCCE EEEE EE00 0000 0000 E0EE AAAD CCCC CCCD"
	$"EEEE EE00 0000 0000 000C CCDD CCCC CCCD EEEE EE00 0000 0000 EEAA AAAD CCCC CCCD"
	$"EEEE EE00 0000 0000 0CCC DDDD CCCC FCCD EEEE ED00 0000 0000 0CCD DDDD CCC2 2CCD"
	$"EEEE EE00 EEEE EE0C CCCD DDDD CCC2 CCCD EEED ED00 0000 000D CCDD DDDE CC22 CCCD"
	$"EEDE DE00 EEE0 00DE DDDD DDEE C22C C00D EDED ED00 0000 00EE DCDD EEDE C2DC C00D"
	$"DEDE DD00 0000 0DAD 00DD DDDE 22CC 000D EDED ED00 0EEE EEEC 0DDD DEEB 2CCC 000D"
	$"DEDD DD00 0000 DEC0 DDDD DDD2 2CC0 000D EDED DD00 0EEE AD0D DDDE DC22 CC00 000D"
	$"DEDD DD00 000E D0DD DDED CC22 CC00 000D EDDD DD00 0EEE 0DDD EDCC C22C C000 000D"
	$"DDDD DD00 00AC DDDD DCCC C2DC C000 000D DDDD DC00 0ECD DEED CCCC 22CC 0000 000D"
	$"DDDD CC00 0DDD DEDC CCCC 2CCC 0000 000E DDDC CC00 DDDE EDCC CCC2 2CC0 C000 000E"
	$"DDCC CC00 DDEE DCCC CC44 CCCC 0000 000E DCCC CC0D DEEC CCCC CC44 CCC0 C000 000E"
	$"DCCC CCDD DDCC CCCC CCCC 0C0C 0C00 000E DCCC CCCC CCCC CCC0 C0C0 C0C0 C0C0 000E"
	$"DCCC CCCC CC0C 0C0C 0C0C 0C0C 0C0C 000E DDDD DDDD DDDD DDDD DDDD DDDE EEEE EEEE"
};
resource 'ics#' (kApplicationFinderIconResID) {
	{	/* array: 2 elements */
		/* [1] */
		$"FFFF FFFF E00F EE8F E00F E00F EBCD E01B ED37 E06F ECDF E1BF E37F E6FF EFFF FFFF",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'ics8' (kApplicationFinderIconResID) {
	$"FCFC FCFC FCFC FBFB FBFA FAF9 F956 F8F8 FCFC FCFC FCFC FB81 FAFA FAF9 56F8 F8F8"
	$"FCFC FC00 0000 0000 0000 0000 F8F8 F8F8 FCFC FC00 FAF6 F7F7 F700 0000 F8F8 F8F8"
	$"FCFC FC00 0000 0000 0000 0000 F7F7 F7F7 FCFC FB00 F9F9 F600 F6F7 0000 F7F7 FEF8"
	$"FBFB 8100 0000 0000 0000 0000 F711 11F7 FB81 FA00 F7F7 F9F6 0000 0000 1111 F7F7"
	$"FBFA FA00 0000 0000 0000 00F7 1111 F7F7 FAFA F900 0000 0000 0000 F711 11F7 F7F7"
	$"FAF9 F900 F7F9 F600 00F7 F711 11F7 F8F7 F9F9 F800 0000 0000 F7F7 1111 F8F8 F8F7"
	$"56F8 F800 0000 00F7 F7F7 1111 F8F8 F8F7 F8F8 F800 0000 F7F7 F713 11F8 F8F8 F8F7"
	$"F8F8 F700 00F7 F7F7 6969 F8F8 F8F8 F8F7 F8F8 F8F7 F7F7 F7F7 F7F7 F7F7 F7F7 F7F7"
};
resource 'ics4' (kApplicationFinderIconResID) {
	$"EEEE EEEE EDDD DDCC EEEE EEED DDDD DCCC EEE0 0000 0000 CCCC EEE0 DCCC C000 CCCC"
	$"EEE0 0000 0000 CCCC EEE0 CCC0 CC00 CCFC EED0 0000 0000 C22C EDD0 CCCC 0000 22CC"
	$"EDD0 0000 000C 22CC DDD0 0000 00C2 2CCC DDD0 CCC0 0CC2 2CCC DDC0 0000 CC22 CCCC"
	$"DCC0 000C CC22 CCCC CCC0 00CC C22C CCCC CCC0 0CCC 44CC CCCC CCCC CCCC CCCC CCCC"
};
resource 'ICN#' (kDocument_FinderIconResID) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FC00 1000 0600 1477 0500 1444 8480 1464 8440 1444 8420 1777 07F0 1000 0010"
		$"1000 0010 1000 0010 1000 8010 1002 8010 1005 0810 100A 1410 1014 2210 1028 4110"
		$"1050 8090 10A1 0250 1142 0430 1085 0910 160A 1210 1610 2490 1028 4110 1052 9010"
		$"1085 2410 104A 4890 1064 9130 1071 2A50 1038 5490 101C A110 100E 0010 1FFF FFF0",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};
resource 'icl8' (kDocument_FinderIconResID) {
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF 0000 0000 0000 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 FAFF FF00 0000 0000 0000 0000"
	$"0000 00FF F8FA F8F8 F8FA FAFA F8FA FAFA F8F8 F8F8 FAFF 2BFF 0000 0000 0000 0000"
	$"0000 00FF F8FA F8F8 F8FA F8F8 F8FA F8F8 FAF8 F8F8 FAFF 2B2B FF00 0000 0000 0000"
	$"0000 00FF F8FA F8F8 F8FA FAF8 F8FA F8F8 FAF8 F8F8 FAFF 2B2B 2BFF 0000 0000 0000"
	$"0000 00FF F8FA F8F8 F8FA F8F8 F8FA F8F8 FAF8 F8F8 FAFA 2B2B 2B2B FF00 0000 0000"
	$"0000 00FF F8FA FAFA F8FA FAFA F8FA FAFA F8F8 F8F8 FAFF FFFF FFFF FFFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 FAFA FAFA FAFA FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8FA FAFA FAFA FAFF 0000 0000"
	$"0000 00FF F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F8FA FAFF 0000 0000"
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
resource 'icl4' (kDocument_FinderIconResID) {
	$"000F FFFF FFFF FFFF FFFF FF00 0000 0000 000F CCCC CCCC CCCC CCCC DFF0 0000 0000"
	$"000F CDCC CDDD CDDD CCCC DFCF 0000 0000 000F CDCC CDCC CDCC DCCC DFCC F000 0000"
	$"000F CDCC CDDC CDCC DCCC DFCC CF00 0000 000F CDCC CDCC CDCC DCCC DDCC CCF0 0000"
	$"000F CDDD CDDD CDDD CCCC DFFF FFFF 0000 000F CCCC CCCC CCCC CCCC DDDD DDDF 0000"
	$"000F CCCC CCCC CCCC CCCC CDDD DDDF 0000 000F CCCC CCCC CCCC CCCC CCCC CDDF 0000"
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
resource 'ics#' (kDocument_FinderIconResID) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 4048 4044 407C 4104 4204 4404 4844 50A4 4134 424C 4414 4924 464C 4214 7FFC",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'ics8' (kDocument_FinderIconResID) {
	$"00FF FFFF FFFF FFFF FFFF FFFF 0000 0000 00FF 2B2B 2B2B 2B2B 2B2B FFFF FF00 0000"
	$"00FF 2B2B 2B2B 2B2B 2B2B FF2B FFFF 0000 00FF 2B2B 2B2B 2B2B 2B2B FFFF FFFF 0000"
	$"00FF 2B17 2B2B 2B2B 2B2B 2B2B 2BFF 0000 00FF 2B2B 1717 2B2B 2B2B 2B2B 2BFF 0000"
	$"00FF 2B2B 2B2B 1717 2B2B 2B2B 2BFF 0000 00FF 2B2B 2B2B 2B2B 2B00 2B2B 2BFF 0000"
	$"00FF 2B2B 2B2B 2B2B 0000 002B 2BFF 0000 00FF 2B2B 2B2B 2B00 0000 0000 2BFF 0000"
	$"00FF 2B2B 2B2B 0000 0000 0000 00FF 0000 00FF 2B2B 2B00 00FB 0000 00FB 00FF 0000"
	$"00FF 2B2B 0000 FB00 0000 FB00 00FF 0000 00FF 2B2B 2B00 0000 00FB 00FB 00FF 0000"
	$"00FF 2B2B 2B2B 0000 FB00 FB00 00FF 0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};
resource 'ics4' (kDocument_FinderIconResID) {
	$"0FFF FFFF FFFF 0000 0FCC CCCC CCFF F000 0FCC CCCC CCFC FF00 0FCC CCCC CCFF FF00"
	$"0FC2 CCCC CCCC CF00 0FCC 22CC CCCC CF00 0FCC CC22 CCCC CF00 0FCC CCCC C0CC CF00"
	$"0FCC CCCC 000C CF00 0FCC CCC0 0000 CF00 0FCC CC00 0000 0F00 0FCC C00E 000E 0F00"
	$"0FCC 00E0 00E0 0F00 0FCC C000 0E0E 0F00 0FCC CC00 E0E0 0F00 0FFF FFFF FFFF FF"
};

resource 'ICN#' (kDocumentStationary_FinderIconResID) {
	{	/* array: 2 elements */
		/* [1] */
		$"3FFF FFE0 2000 0020 2008 EE38 2008 8928 2008 C928 2008 8928 200E EE28 2800 0028"
		$"2600 0028 2780 0028 21E0 6028 2078 9028 201F 0828 2007 0428 2004 8228 2008 0528"
		$"2010 08A8 2022 1268 2044 2428 2080 4928 2110 8228 2125 2028 210A 4028 2194 8FE8"
		$"20C9 2848 2062 4888 2070 A908 2039 4A08 201C 0C08 3FFF F808 0800 0008 0FFF FFF8",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};
resource 'icl8' (kDocumentStationary_FinderIconResID) {
	$"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00 0000 0000"
	$"0000 FF00 0000 0000 0000 00F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FF00 0000 0000"
	$"0000 FF00 0000 0000 0000 00F5 FAF5 F5F5 FAFA FAF5 FAFA FAF5 F5F5 FFFF FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 FAF5 F5F5 FAF5 F5F5 FAF5 F5FA F5F5 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 FAF5 F5F5 FAFA F5F5 FAF5 F5FA F5F5 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 FAF5 F5F5 FAF5 F5F5 FAF5 F5FA F5F5 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 FAFA FAF5 FAFA FAF5 FAFA FAF5 F5F5 FFF9 FF00 0000"
	$"0000 FF00 0000 0000 0000 00F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F513 13F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F513 6916 1616 16F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFF9 FF00 0000"
	$"0000 FFF5 F5F5 F516 1616 1616 1616 16F5 F5F5 F5F5 F5F5 F5F5 F5F5 FFF9 FF00 0000"
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
resource 'icl4' (kDocumentStationary_FinderIconResID) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFF0 0000 00F0 0000 0000 0000 0000 0000 00F0 0000"
	$"00F0 0000 0000 D000 DDD0 DDD0 00FF F000 00F0 0000 0000 D000 D000 D00D 00FD F000"
	$"00F0 0000 0000 D000 DD00 D00D 00FD F000 00F0 0000 0000 D000 D000 D00D 00FD F000"
	$"00F0 0000 0000 DDD0 DDD0 DDD0 00FD F000 00F0 0000 0000 0000 0000 0000 00FD F000"
	$"00F0 0CC0 0000 0000 0000 0000 00FD F000 00F0 0C42 2220 0000 0000 0000 00FD F000"
	$"00F0 0002 2222 2220 0000 0000 00FD F000 00F0 0000 0002 2222 222C 0000 00FD F000"
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
resource 'ics#' (kDocumentStationary_FinderIconResID) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FFC 4006 5366 5256 5356 5256 5B66 4006 4086 4146 4226 4416 480A 4412 7FE2 3FFE",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'ics8' (kDocumentStationary_FinderIconResID) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF 0000 00FF 0000 0000 0000 0000 0000 00FF FF00"
	$"00FF 00FF 0000 FFFF 00FF FF00 00FF FF00 00FF 00FF 0000 FF00 00FF 00FF 00FF FF00"
	$"00FF 00FF 0000 FFFF 00FF 00FF 00FF FF00 00FF 00FF 0000 FF00 00FF 00FF 00FF FF00"
	$"00FF 00FF FF00 FFFF 00FF FF00 00FF FF00 00FF 0000 0000 0000 0000 0000 00FF FF00"
	$"00FF 0000 0000 0000 FF00 0000 00FF FF00 00FF 0000 0000 00FF 00FF 0000 00FF FF00"
	$"00FF 0000 0000 FF00 0000 FF00 00FF FF00 00FF 0000 00FF 0000 0000 00FF 00FF FF00"
	$"00FF 0000 FF00 0000 0000 0000 FF00 FF00 00FF 0000 00FF 0000 0000 00FF 0000 FF00"
	$"00FF FFFF FFFF FFFF FFFF FF00 0000 FF00 0000 FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
resource 'ics4' (kDocumentStationary_FinderIconResID) {
	$"0FFF FFFF FFFF FF00 0F00 0000 0000 0FF0 0F0F 00FF 0FF0 0FF0 0F0F 00F0 0F0F 0FF0"
	$"0F0F 00FF 0F0F 0FF0 0F0F 00F0 0F0F 0FF0 0F0F F0FF 0FF0 0FF0 0F00 0000 0000 0FF0"
	$"0F00 0000 F000 0FF0 0F00 000F 0F00 0FF0 0F00 00F0 00F0 0FF0 0F00 0F00 000F 0FF0"
	$"0F00 F000 0000 F0F0 0F00 0F00 000F 00F0 0FFF FFFF FFF0 00F0 00FF FFFF FFFF FFF0"
};


















resource 'ALRT' (kPickInputFileFormat_AlertID) {
	{40, 40, 190, 418},
	kPickInputFileFormat_AlertID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};
resource 'DITL' (kPickInputFileFormat_AlertID) {
	{
		{112, 216, 132, 274},
		Button {
			enabled,
			"Open"
		},
		{112, 111, 132, 169},
		Button {
			enabled,
			"Cancel"
		},
		{22, 64, 80, 360},
		StaticText {
			disabled,
			"The file '^0' could be in any of a number of different formats. Iterpret the file as:"
		},
		{26, 18, 58, 50},
		Icon {
			disabled,
			1
		},
		{80, 82, 101, 320},
		Control {
			disabled,
			kPickInputFileFormat_AlertID
		}
	}
};
resource 'MENU' (kPickInputFileFormat_AlertID) {
	kPickInputFileFormat_AlertID,
	textMenuProc,
	allEnabled,
	enabled,
	"",
	{
	}
};
resource 'CNTL' (kPickInputFileFormat_AlertID) {
	{80, 82, 101, 320},
	0,
	visible,
	0,
	kPickInputFileFormat_AlertID,
	popupMenuCDEFproc,
	0,
	"Input File Format:"
};







data 'Estr' (-192, "resNotFound", purgeable) {
	$"1E61 2072 6571 7569 7265 6420 7265 736F 7572 6365 2069 7320 6D69 7373 696E 67"                      /* .a required resource is missing */
};

data 'Estr' (-108, "memFullErr", purgeable) {
	$"1B74 6865 7265 2077 6173 206E 6F74 2065 6E6F 7567 6820 6D65 6D6F 7279"                              /* .there was not enough memory */
};

resource 'SICN' (200, "TCL sicns", purgeable, preload) {
	{	/* array: 2 elements */
		/* [1] */
		$"FFFF 8001 8001 9FC1 9041 907D 9045 9045 9045 9FC5 8405 8405 8405 87FD 8001 FFFF",
		/* [2] */
		$"07FF 03FE 01FC 00F8 0070 0020"
	}
};

resource 'STR#' (128, "Common") {
	{	/* array StringArray: 7 elements */
		/* [1] */
		"quitting",
		/* [2] */
		"closing",
		/* [3] */
		"Undo ",
		/* [4] */
		"Redo ",
		/* [5] */
		"Untitled",
		/* [6] */
		"Show Clipboard",
		/* [7] */
		"Hide Clipboard"
	}
};

resource 'STR#' (129, "Memory Warnings") {
	{	/* array StringArray: 1 elements */
		/* [1] */
		"Memory is running low. Please close windows and save documents to avoid losing d"
		"ata."
	}
};

resource 'STR#' (130, "Task Names") {
	{	/* array StringArray: 6 elements */
		/* [1] */
		"Typing",
		/* [2] */
		"Cut",
		/* [3] */
		"Copy",
		/* [4] */
		"Paste",
		/* [5] */
		"Clear",
		/* [6] */
		"Formatting"
	}
};

resource 'STR#' (133, "Dlg validation", purgeable) {
	{	/* array StringArray: 4 elements */
		/* [1] */
		"This is a required field and cannot be left empty.",
		/* [2] */
		"You cannot enter more than ^1 characters in this field.",
		/* [3] */
		"This field accepts only numbers",
		/* [4] */
		"This field accepts only numbers between ^1 and ^2."
	}
};

resource 'STR#' (131, "Exceptions", purgeable) {
	{	/* array StringArray: 9 elements */
		/* [1] */
		"",
		/* [2] */
		"",
		/* [3] */
		"new_by_name was called for a unknown class",
		/* [4] */
		"there are too many pages too print",
		/* [5] */
		"attempted to store more than 32767 characters in a TextEdit item",
		/* [6] */
		"name or other descriptor string > 255 characters ",
		/* [7] */
		"user canceled",
		/* [8] */
		"an exception of unknown type was thrown",
		/* [9] */
		"document can't save the requested file type"
	}
};

resource 'STR#' (134, "Stream error messages", purgeable) {
	{	/* array StringArray: 9 elements */
		/* [1] */
		"stream GetObject tried to read class with unrecognized or garbage name",
		/* [2] */
		"stream Get or GetThru tried to read past end of stream",
		/* [3] */
		"stream GetLineAsStr tried to read line longer than 255 characters",
		/* [4] */
		"stream GetObject tried to read duplicate reference with duplicate checking off",
		/* [5] */
		"stream GetObject tried to read reference of unknown format",
		/* [6] */
		"stream Position or Truncate used position less than zero or greater than Size",
		/* [7] */
		"view version number greater than highest handled by this program",
		/* [8] */
		"expecting to read a window but got a subview or other type instead",
		/* [9] */
		"expecting to read a subview but got a window or other type instead"
	}
};

resource 'STR#' (302, "Lib validation", purgeable) {
	{	/* array StringArray: 2 elements */
		/* [1] */
		"Identifier must begin with alphabetic or '_' and contain only alphanumeric or '_"
		"' characters",
		/* [2] */
		"Type (signature, file type, etc.) must have exactly four characters"
	}
};

resource 'STR ' (300, "OS Error", purgeable) {
	"Mac OS Error Encountered"
};

resource 'STR ' (150, "SaveAs Prompt", purgeable) {
	"Save File As:"
};

resource 'STR ' (301, "OS Error2", purgeable) {
	"of a Mac OS Error"
};

resource 'WIND' (200, "Clipboard", purgeable, preload) {
	{130, 158, 238, 444},
	zoomDocProc,
	invisible,
	goAway,
	0x0,
	"Clipboard",
	noAutoCenter
};

resource 'WIND' (500, "TinyEdit Window", purgeable) {
	{68, 44, 268, 284},
	zoomDocProc,
	invisible,
	goAway,
	0x0,
	Untitled,
	noAutoCenter
};







resource 'MBAR' (1, preload) {
	{
		1,
		2,
		3,
		4
	#if 0
	//	10,
	//	11,
		12
	#endif
	}
};
resource 'MENU' (1, "Apple", preload) {
	1,
	textMenuProc,
	0x7FFFFFFD,
	enabled,
	apple,
	{
		"About Led Test TCL!É#256", noIcon, noKey, noMark, plain,
		/* [2] */
		"-", noIcon, noKey, noMark, plain
	}
};
resource 'MENU' (2, "File", preload) {
	2,
	textMenuProc,
	0x7FFFFC03,
	enabled,
	"File",
	{	/* array: 11 elements */
		/* [1] */
		"New#2", noIcon, "N", noMark, plain,
		/* [2] */
		"OpenÉ#3", noIcon, "O", noMark, plain,
		/* [3] */
		"-", noIcon, noKey, noMark, plain,
		/* [4] */
		"Close#4", noIcon, "W", noMark, plain,
		/* [5] */
		"Save#5", noIcon, "S", noMark, plain,
		/* [6] */
		"Save AsÉ#6", noIcon, noKey, noMark, plain,
		/* [7] */
		"-", noIcon, noKey, noMark, plain,
		/* [8] */
		"Page SetupÉ#8", noIcon, noKey, noMark, plain,
		/* [9] */
		"PrintÉ#9", noIcon, "P", noMark, plain,
		/* [10] */
		"-", noIcon, noKey, noMark, plain,
		/* [11] */
		"Quit#1", noIcon, "Q", noMark, plain
	}
};
resource 'MENU' (3, "Edit", preload) {
	3,
	textMenuProc,
	0x7FFFFF80,
	enabled,
	"Edit",
	{
		"Undo#16", noIcon, "Z", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Cut#18", noIcon, "X", noMark, plain,
		"Copy#19", noIcon, "C", noMark, plain,
		"Paste#20", noIcon, "V", noMark, plain,
		"Clear#21", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Select All#23", noIcon, "A", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"FindÉ#1001", noIcon, "F", noMark, plain,
		"Find Again#1002", noIcon, "G", noMark, plain,
		"-", noIcon, noKey, noMark, plain,
		"Show Clipboard#22", noIcon, noKey, noMark, plain
	}
};
resource 'MENU' (4, "Format") {
	4,
	textMenuProc,
	0x7FFFFFFF,
	enabled,
	"Format",
	{
		"Font", noIcon, hierarchicalMenu, "\0X0a", plain,
		"Size", noIcon, hierarchicalMenu, "\0X0b", plain,
		"Style", noIcon, hierarchicalMenu, "\0X0c", plain
	}
};
resource 'MENU' (10, "Font", preload) {
	10,
	textMenuProc,
	allEnabled,
	enabled,
	"Font",
	{
	}
};

resource 'MENU' (11, "Size", preload) {
	11,
	textMenuProc,
	allEnabled,
	enabled,
	"Size",
	{	/* array: 5 elements */
		/* [1] */
		"9#-1", noIcon, noKey, noMark, plain,
		/* [2] */
		"10#-1", noIcon, noKey, noMark, plain,
		/* [3] */
		"12#-1", noIcon, noKey, noMark, plain,
		/* [4] */
		"18#-1", noIcon, noKey, noMark, plain,
		/* [5] */
		"24#-1", noIcon, noKey, noMark, plain
	}
};
resource 'MENU' (12, "Style") {
	12,
	textMenuProc,
	allEnabled,
	enabled,
	"Style",
	{	/* array: 8 elements */
		/* [1] */
		"Plain Text#30", noIcon, "T", noMark, plain,
		/* [2] */
		"Bold#31", noIcon, "B", noMark, 1,
		/* [3] */
		"Italic#32", noIcon, "I", noMark, 2,
		/* [4] */
		"Underline#33", noIcon, "U", noMark, 4,
		/* [5] */
		"Outline#34", noIcon, noKey, noMark, 8,
		/* [6] */
		"Shadow#35", noIcon, noKey, noMark, 16,
		/* [7] */
		"Condensed#36", noIcon, noKey, noMark, 32,
		/* [8] */
		"Extended#37", noIcon, noKey, noMark, 64
	}
};










#define	kLedAboutBoxID	666


resource 'ALRT' (kLedAboutBoxID) {
	{40, 20, 255, 407},
	kLedAboutBoxID,
	{
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent,
		OK, visible, silent
	},
	alertPositionMainScreen
};
resource 'DITL' (kLedAboutBoxID, purgeable) {
	{
		{181, 291, 201, 371},
		Button {
			enabled,
			"OK"
		},
		{16, 70, 175, 380},
		StaticText {
			disabled,
			"The Led Test TCL! text editor ^0 (^1)\n"
			"\n"
			"Written by Lewis Gordon Pringle Jr.\n"
			"\n"
			"For more information contact info-led@sophists.com via email, or http://www.sophists.com/Led/ on the web.\n"
			"\n"
			"Copyright © 1995  Sophist Solutions.\n"
			"All Rights Reserved.\n"
		},
		{16, 20, 48, 52},
		Icon {
			disabled,
			kLedAboutBoxID
		}
	}
};
data 'cicn' (kLedAboutBoxID) {
	$"0000 0000 8020 0000 0000 0020 0020 0000 0000 0000 0000 0048 0000 0048 0000 0000"                    /* ....€ ..... . .........H...H.... */
	$"0008 0001 0008 0000 0000 0000 0000 0000 0000 0000 0000 0004 0000 0000 0020 0020"                    /* ............................. .  */
	$"0000 0000 0004 0000 0000 0020 0020 0000 0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"                    /* ........... . ....ÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"                    /* ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"                    /* ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"                    /* ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ */
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF 0000 0000 0000 0000 0000 0000 0000"                    /* ÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ.............. */
	$"0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000"                    /* ................................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000"                    /* ................................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000"                    /* ................................ */
	$"0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0017 0000 FFFF FFFF"                    /* ............................ÿÿÿÿ */
	$"FFFF 0001 FFFF FFFF 9999 0002 FFFF CCCC CCCC 0003 FFFF 9999 6666 0004 FFFF 9999"                    /* ÿÿ..ÿÿÿÿ™™..ÿÿÌÌÌÌ..ÿÿ™™ff..ÿÿ™™ */
	$"3333 0005 FFFF 9999 0000 0006 CCCC CCCC CCCC 0007 CCCC CCCC 9999 0008 CCCC 9999"                    /* 33..ÿÿ™™....ÌÌÌÌÌÌ..ÌÌÌÌ™™..ÌÌ™™ */
	$"9999 0009 CCCC 9999 6666 000A 9999 9999 9999 000B 9999 6666 9999 000C 6666 6666"                    /* ™™.ÆÌÌ™™ff..™™™™™™..™™ff™™..ffff */
	$"6666 000D 3333 3333 3333 000E EEEE EEEE EEEE 000F DDDD DDDD DDDD 0010 BBBB BBBB"                    /* ff.Â333333..îîîîîî..ÝÝÝÝÝÝ..»»»» */
	$"BBBB 0011 AAAA AAAA AAAA 0012 8888 8888 8888 0013 7777 7777 7777 0014 5555 5555"                    /* »»..ªªªªªª..ˆˆˆˆˆˆ..wwwwww..UUUU */
	$"5555 0015 4444 4444 4444 0016 2222 2222 2222 00FF 0000 0000 0000 0000 0000 0000"                    /* UU..DDDDDD.."""""".ÿ............ */
	$"0000 0000 0000 0000 0000 000E 0E0E 0E0F 0F0F 0F0F 0606 0606 1010 0015 1515 1515"                    /* ................................ */
	$"1515 1515 1515 1414 1414 0C0C 0C13 1313 1212 120A 0A0A 0A11 1110 0014 1514 1514"                    /* ................................ */
	$"1514 1514 1514 1414 0C0C 0C13 1313 1312 1212 0A0A 0A11 1111 1110 0015 1515 1515"                    /* ................................ */
	$"0000 0000 0000 0000 0000 0000 0000 000E 0F06 0A0A 0A11 1111 1011 0015 1514 1514"                    /* ................................ */
	$"0000 0000 0000 0000 0000 0000 0000 0E0F 0610 0A11 1111 1110 1011 0015 1415 1515"                    /* ................................ */
	$"0000 0000 0000 0000 0D00 0D0D 000D 0D16 1611 0A11 1111 1010 1011 0015 1514 1515"                    /* ........Â.ÂÂ.ÂÂ................. */
	$"0000 0000 0000 0000 0000 0000 000E 0F10 110A 1111 1110 1010 0611 0015 1415 1415"                    /* ................................ */
	$"0000 0000 0000 0000 0000 0D00 0D0D 1616 160A 1110 1010 1006 060A 0015 1515 1514"                    /* ..........Â.ÂÂ.................. */
	$"0000 0000 0000 0000 0000 0000 0E06 1011 0A12 1010 1010 0606 060A 0015 1415 1414"                    /* ................................ */
	$"0000 0000 0000 0000 0000 0D11 1111 0616 1613 1010 1006 0606 0F0A 0014 1514 1414"                    /* ..........Â..................... */
	$"0000 0000 0000 0000 0000 000F 0610 0A12 1213 1010 0607 FF0F 0F0A 0015 1414 140C"                    /* ......................ÿ......... */
	$"0000 0000 0000 0000 0000 0E06 100A 0A12 130C 0606 0705 050F 0F0A 0014 1414 0C0C"                    /* ................................ */
	$"0000 0D0D 0D0D 0D0D 0010 0F10 110A 1213 0C0C 0606 0405 0706 0F12 0014 140C 0C0C"                    /* ..ÂÂÂÂÂÂ........................ */
	$"0000 0000 0000 0000 0013 1111 0A12 130C 0C14 0607 0504 0606 0F12 0014 0C0C 0C13"                    /* ................................ */
	$"0000 0D0D 0D00 0000 0A0D 120A 1213 0C0C 1415 0605 0507 060F 0E12 000C 0C0C 1313"                    /* ..ÂÂÂ....Â...................... */
	$"0000 0000 0000 0000 140D 0A11 130C 1414 0C15 0305 0906 060E 0E13 0E0C 0C13 1313"                    /* .........Â..........Æ........... */
	$"0000 0000 0000 0012 1613 0E0E 0A13 0C0A 0A14 0505 1006 0F0E 0013 0E0C 1313 1312"                    /* ................................ */
	$"0000 000D 0D0D 0D15 140F 0E12 0C13 0A15 1404 0508 0606 0E00 0013 0E13 1313 1212"                    /* ...ÂÂÂÂ......................... */
	$"0000 0000 0000 130D 060E 120C 1212 0C0C 0A05 0406 060F 0E00 0013 0E13 1312 1212"                    /* .......Â........................ */
	$"0000 000D 0D15 160A 000A 0C12 0C15 1310 0405 0706 0F0E 0000 0013 0F13 1212 120A"                    /* ...ÂÂ........................... */
	$"0000 0000 0015 1300 120C 0A12 140A 0F02 0504 0606 0E00 0000 000C 0F12 1212 0A0A"                    /* ................................ */
	$"0000 000D 1515 0012 130A 1513 100F 0F04 0510 060F 0E00 0000 000C 0F12 120A 0A0A"                    /* ...Â............................ */
	$"0000 0000 160F 0A13 0C0C 1206 0F0F 0105 0906 0F0E 0000 0000 000C 0F12 0A0A 0A11"                    /* ................Æ............... */
	$"0000 0014 060A 1215 140A 060F 0F0F 0405 0606 0F00 0000 0000 000C 060A 0A0A 1111"                    /* ................................ */
	$"0000 0013 120A 0C14 0A0F 0F0E 0E01 0507 060F 0E00 0000 0000 0014 060A 0A11 1111"                    /* ................................ */
	$"0000 0A0A 0C0D 150A 0F0F 0E0F 0E05 0406 060E 0000 0000 0000 0014 060A 1111 1111"                    /* .....Â.......................... */
	$"0000 0A13 0D15 120F 0F0F 0E0E 0804 0706 0F0E 0000 0000 0000 0014 0611 1111 1010"                    /* ....Â........................... */
	$"0012 1315 1411 0F0F 0E0E 0E0F 0B08 0F0F 0E00 0000 0000 0000 0014 0611 1110 1010"                    /* ................................ */
	$"0C13 130A 060F 0F0E 0E0E 0E10 0B0E 0E0E 0000 0000 0000 0000 0015 1011 1010 1010"                    /* ................................ */
	$"0606 060F 0F0F 0E0E 0E0E 000E 0E00 0000 0000 0000 0000 0000 0015 1011 1010 0606"                    /* ................................ */
	$"0606 0F0F 0F0E 0E0E 0E00 0000 0000 0000 0000 0000 0000 0000 0015 1010 1111 1111"                    /* ................................ */
	$"0A0A 0A0A 1212 1212 1213 1313 130C 0C0C 0C14 1414 1414 1515 1515"                                   /* .......................... */
};













resource 'ALRT' (300, "OS Error", purgeable) {
	{32, 10, 168, 352},
	300,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (200, "Severe Error", purgeable) {
	{32, 10, 168, 352},
	200,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (150, "Confirm Revert", purgeable) {
	{100, 120, 190, 420},
	150,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (151, "Save Changes", purgeable) {
	{42, 34, 184, 350},
	151,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (128, "General", purgeable) {
	{32, 10, 164, 328},
	128,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, silent,
		/* [2] */
		OK, visible, silent,
		/* [3] */
		OK, visible, silent,
		/* [4] */
		OK, visible, silent
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (250, "No Printer", purgeable) {
	{40, 40, 156, 382},
	250,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (129, "Validation", purgeable) {
	{32, 10, 164, 328},
	128,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (251, "Error", purgeable) {
	{40, 40, 209, 372},
	251,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (252, "Error2", purgeable) {
	{40, 40, 209, 372},
	252,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'ALRT' (253, "assert", purgeable) {
	{40, 40, 213, 393},
	253,
	{	/* array: 4 elements */
		/* [1] */
		OK, visible, sound1,
		/* [2] */
		OK, visible, sound1,
		/* [3] */
		OK, visible, sound1,
		/* [4] */
		OK, visible, sound1
	},
	noAutoCenter			// - avoid TCL PositionDialog () bug - LGP 951103 - alertPositionMainScreen
};

resource 'DITL' (300, "OS Error", purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{104, 264, 124, 324},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{16, 80, 87, 332},
		StaticText {
			disabled,
			"^0\n\nResult Code = ^1"
		}
	}
};

resource 'DITL' (200, "Severe Error", purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{104, 264, 124, 324},
		Button {
			enabled,
			"Proceed"
		},
		/* [2] */
		{104, 168, 124, 228},
		Button {
			enabled,
			"Quit"
		},
		/* [3] */
		{16, 80, 87, 332},
		StaticText {
			disabled,
			"^0\n\nResult Code = ^1"
		}
	}
};

resource 'DITL' (150, "Confirm Revert", purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{60, 208, 80, 290},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{60, 105, 80, 187},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{5, 113, 45, 293},
		StaticText {
			disabled,
			"Revert to the last version saved ?"
		},
		/* [4] */
		{47, 10, 64, 66},
		StaticText {
			disabled,
			"Caution"
		}
	}
};

resource 'DITL' (151, "Save Changes", purgeable) {
	{	/* array DITLarray: 4 elements */
		/* [1] */
		{74, 35, 94, 117},
		Button {
			enabled,
			"Yes"
		},
		/* [2] */
		{111, 35, 131, 117},
		Button {
			enabled,
			"No"
		},
		/* [3] */
		{111, 205, 131, 287},
		Button {
			enabled,
			"Cancel"
		},
		/* [4] */
		{14, 25, 55, 291},
		StaticText {
			disabled,
			"Save changes to Ò^0Ó before ^1?"
		}
	}
};

resource 'DITL' (128, "General", purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{98, 231, 118, 291},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{14, 25, 82, 299},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (250, "No Printer", purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{85, 262, 105, 327},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{14, 84, 67, 327},
		StaticText {
			disabled,
			"Printing operations are not possible until you have selected a Printer using the"
			" Chooser. "
		}
	}
};

resource 'DITL' (129, "validation", purgeable) {
	{	/* array DITLarray: 2 elements */
		/* [1] */
		{98, 231, 118, 291},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{14, 25, 82, 299},
		StaticText {
			disabled,
			"^0"
		}
	}
};

resource 'DITL' (251, "error1", purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{138, 257, 158, 315},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{15, 79, 100, 323},
		StaticText {
			disabled,
			"Couldn't complete the last command because ^0."
		},
		/* [3] */
		{105, 81, 123, 325},
		StaticText {
			disabled,
			"Result Code = ^1"
		}
	}
};

resource 'DITL' (252, "error2", purgeable) {
	{	/* array DITLarray: 3 elements */
		/* [1] */
		{138, 257, 158, 315},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{15, 79, 100, 323},
		StaticText {
			disabled,
			"Couldn't successfully startup or quit the application because ^0."
		},
		/* [3] */
		{105, 81, 123, 325},
		StaticText {
			disabled,
			"Result Code = ^1"
		}
	}
};

resource 'DITL' (253, "assert", purgeable) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{142, 282, 162, 340},
		Button {
			enabled,
			"Quit"
		},
		/* [2] */
		{142, 201, 162, 259},
		Button {
			enabled,
			"Fail"
		},
		/* [3] */
		{142, 114, 162, 183},
		Button {
			enabled,
			"Continue"
		},
		/* [4] */
		{5, 57, 86, 347},
		StaticText {
			disabled,
			"Assertion failed:\n^0"
		},
		/* [5] */
		{92, 60, 127, 302},
		StaticText {
			disabled,
			"File: ^1\nLine: ^2"
		}
	}
};
resource 'CNTL' (300, "Scroll Bar", preload) {
	{0, 0, 0, 0},
	0,
	visible,
	0,
	0,
	scrollBarProc,
	0,
	""
};



#define	kFind_DialogID	1001
resource 'DLOG' (kFind_DialogID) {
	{109, 58, 206, 376},
	movableDBoxProc,
	visible,
	goAway,
	0x0,
	kFind_DialogID,
	"Find",
	centerParentWindowScreen
};
resource 'DITL' (kFind_DialogID) {
	{
		{66, 89, 86, 147},
		Button {
			enabled,
			"Find"
		},
		{66, 173, 86, 252},
		Button {
			enabled,
			"Don't Find"
		},
		{18, 22, 34, 71},
		StaticText {
			disabled,
			"Find:"
		},
		{17, 66, 35, 307},
		EditText {
			disabled,
			""
		},
		{45, 10, 60, 109},
		CheckBox {
			enabled,
			"Wrap at End"
		},
		{45, 110, 60, 209},
		CheckBox {
			enabled,
			"Whole Word"
		},
		{45, 210, 60, 309},
		CheckBox {
			enabled,
			"Ingore case"
		}
	}
};









