/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Sources/LedPart.r,v 2.10 1996/12/13 18:10:13 lewis Exp $
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
 *	$Log: LedPart.r,v $
 *	Revision 2.10  1996/12/13 18:10:13  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.9  1996/12/05  21:12:22  lewis
 *	*** empty log message ***
 *
 *	Revision 2.8  1996/09/01  15:44:51  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.7  1996/05/23  20:35:50  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/03/05  18:48:09  lewis
 *	Changed 'vers' message to LedIt! OpenDoc Part Editor VER#.
 *	Changed some other NMAP name used to gen stationary file name
 *	(and who knows what else) back to LedIt! 2.0 - what it had been before -
 *	from LedIt! OpenDoc Editor 2.0 - which I had set it breifly earlier.
 *
 *	Revision 2.5  1996/02/26  23:09:42  lewis
 *	Added 'kind' resources.
 *	Updated icons for documents/stationary to distinguish between OD docs
 *	and Led App private format docs.
 *
 *	Revision 2.4  1996/02/05  05:12:15  lewis
 *	Prefs dialog, and Find dialog.
 *	Renamed some string resources to new Part name - LedIt!
 *
 *	Revision 2.3  1996/01/11  08:52:09  lewis
 *	Put in hacks so I can (without too much pain) build 68K cfrags.
 *	Still cannot see how to automate this...
 *
 *	Revision 2.2  1995/12/15  04:16:21  lewis
 *	Added UndoTyping and Redo Typing strings for undo menu.
 *
 *	Revision 2.1  1995/12/08  07:26:30  lewis
 *	changed numbers of hierarchical menus, to conform to guidelines for
 *	number ranges in OpenDoc docs.
 *
 *	Revision 2.0  1995/11/25  00:41:12  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1995/11/05  02:16:21  lewis
 *	Completely re-organized file. Not BDNL / reosurce stuff organized, and
 *	using newly registered TYPEs/Creeators etc. Should have new icons soon.
 *	Found roughly 1/3 of the icons were orhapned/garbage from who knows where. I
 *	HOPE all the ones I got rid of weren't needed :-).
 *
 *	Revision 2.3  1995/11/02  22:51:12  lewis
 *	Call Led OD Part Led rather than LedPart.
 *	Top/down review/fixup of nmap resources. Added many new file types
 *	supported.
 *	nmap stuff probably still isn't right, but I think its now much closer and
 *	much more sensible.
 *
 *	Revision 2.2  1995/10/19  22:58:45  lewis
 *	I think minor changes, but since rcsdiff not working, hard to be sure (were
 *	non-ascii cahracters in the file - sigh).
 *	One thing I did was re-organize a bit so related resources closer together.
 *	Still not well done though.
 *
 *
 *
 *
 */

#define SystemSevenOrBetter 1			// we want the extended types
#define	SystemSevenOrLater	1			// Types.r uses this variable

#include	"AEUserTermTypes.r"
#include	"AppleEvents.r"
#include	"AERegistry.r"
#include	"ASRegistry.r"
#include	"BalloonTypes.r"
#include	"CodeFragmentTypes.r"
#include	"ODTypes.r"
#include	"SysTypes.r"
#include	"Types.r"


#include	"StdDefs.h"

#include	"LedPartDef.h"
#include	"LedConfig.h"
#include	"LedPartVers.h"



// HACK AND ALWAYS DEFINE AS POWERPC CUZ THATS ALL MY .R FILE HEADERS DEFINE!!!
#define	qUsePPCCFrag	1
resource 'cfrg' (0) {
	{	/* [1] */
		#if		qUsePPCCFrag
			kPowerPC,
		#else
			kMotorola,
		#endif
		kFullLib,
		currentVersion,
		compatibleVersion,
		kDefaultStackSize,
		kNoAppSubFolder,
		kIsLib,
		kOnDiskFlat,
		kZeroOffset,
		kWholeFork,
		kLedPartID,		/* This must be the class ID. */
		/* [2] */
		#if		qUsePPCCFrag
			kPowerPC,
		#else
			kMotorola,
		#endif
		kFullLib,
		currentVersion,
		compatibleVersion,
		kDefaultStackSize,
		kNoAppSubFolder,
		kIsLib,
		kOnDiskFlat,
		kZeroOffset,
		kWholeFork,
		kPartClassName		/* This must be the SOM class name */
							/* for this part.				   */
	}
};










resource 'vers' (1) {
	0x2,
	0x0,
	development,
	0x2,
	verUS,
	qLed_ShortVersionString,
	"LedIt! OpenDoc Part Editor " qLed_ShortVersionString
};
resource 'vers' (2) {
	0x2,
	0x0,
	development,
	0x2,
	verUS,
	qLed_ShortVersionString,
	"LedIt! OpenDoc Part Editor " qLed_ShortVersionString
};










//-------------------------------------------------------------------------------------
// NMAP Resources
//-------------------------------------------------------------------------------------

// Map a part's kind (unique content identifier)
// to a generic content category identifier.
resource kODNameMappings (kKindCategoryMapId) {
	kODKind,
	{
		kLedPartKind,
		kODIsAnISOStringList {
			{	kODCategoryStyledText	}
		}
	}
};
// Map a part's class id (module::classname)
// to a part's kind (unique content identifier)
resource kODNameMappings (kEditorKindMapId) {
	kODEditorKinds,
	{
		kLedPartID,
		kODIsAnISOStringList {
			{		kLedPartKind	}
		}
	}
};
// Map a part's class id (module::classname)
// to a human readable string.
resource kODNameMappings (kEditorUserStringMapId) {
	kODEditorUserString,
	{
		kLedPartID,
		kODIsINTLText {
//			smRoman,	langEnglish,	"LedIt! OpenDoc Editor 2.0"	
			smRoman,	langEnglish,	"LedIt! 2.0"	
		}
	}
};
// Map a part's kind (unique content identifier)
// to a human readable string.
resource kODNameMappings (kKindUserStringMapId) {
	kODKindUserString,
	{
		kLedPartKind,
		kODIsINTLText {
			smRoman,	langEnglish,	"LedIt! Styled Text"
		}
	}
};
resource kODNameMappings (kOldMacOSTypeMapId) {
	kODKindOldMacOSType,
	{
		kLedPartKind,
		kODIsMacOSType {	
			kDocumentFileType
		}
	}
};
resource kODNameMappings (kPlatformEditorKindMapId) {
	kODEditorPlatformKind,
	{
		kLedPartID,
		kODIsPltfmTypeSpac {
			/* array PltfmTypeSpacList: */
			{
				kODPlatformFileType, 
				kTextFileKind,
				smRoman,
				langEnglish,
				"Text file",
				kODCategoryPlainText,

				kODPlatformDataType, 
				kTextDataKind,
				smRoman,
				langEnglish,
				"Text data",
				kODCategoryPlainText,

#if		qSTTXTSupported
				kODPlatformFileType, 
				'stxt', 
				smRoman, 
				langEnglish, 
				"stxt file", 
				kODCategoryStyledText,

				kODPlatformDataType, 
				'stxt', 
				smRoman, 
				langEnglish, 
				"stxt data", 
				kODCategoryStyledText,
#endif

				kODPlatformDataType, 
				'styl', 
				smRoman, 
				langEnglish, 
				"styl data", 
				kODCategoryStyledText,	
			}
		}
	}
};










resource 'MENU' (kFormatMenuID) {
	kFormatMenuID,
	textMenuProc,
	0x7FFFFFFF,
	enabled,
	"Format",
	{
		#if		kFontMenuID!=0xf0
			#error "need to keep this ID"
		#endif
		"Font", noIcon, hierarchicalMenu, "\0Xf0", plain,
		#if		kFontSizeMenuID!=0xf1
			#error	"need to keep this ID"
		#endif
		"Size", noIcon, hierarchicalMenu, "\0Xf1", plain,
		#if		kFontStyleMenuID!=0xf2
			#error	"need to keep this ID"
		#endif
		"Style", noIcon, hierarchicalMenu, "\0Xf2", plain
	}
};





resource 'MENU' (kFontMenuID) {
	kFontMenuID,
	textMenuProc,
	allEnabled,
	enabled,
	"Font",
	{
	}
};
resource 'hmnu' (kFontMenuID, purgeable) {
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





resource 'MENU' (kFontSizeMenuID) {
	kFontSizeMenuID,
	textMenuProc,
	allEnabled,
	enabled,
	"Size",
	{
		"9 point", noIcon, noKey, noMark, plain,
		"10", noIcon, noKey, noMark, plain,
		"12", noIcon, noKey, noMark, plain,
		"14", noIcon, noKey, noMark, plain,
		"18", noIcon, noKey, noMark, plain,
		"24", noIcon, noKey, noMark, plain,
		"36", noIcon, noKey, noMark, plain,
		"48", noIcon, noKey, noMark, plain,
		"72", noIcon, noKey, noMark, plain,
		"-", noIcon, noKey, noMark, plain;
		"OtherÉ", noIcon, noKey, noMark, plain,
	}
};
resource 'hmnu' (kFontSizeMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Change the size of the font of the currently selected text to one of these sizes.",
				"",
				"",
				""
			},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 9 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 10 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 12 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 14 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 18 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 24 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 36 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 48 point.",	"",	"",	""	},
		HMStringItem {	"Change the selected text (or newly created text if none selected) to 72 point.",	"",	"",	""	},
		HMSkipItem {},	/* separator line */
		HMStringItem {	"Change the selected text (or newly created text if none selected) to some arbitrary point size.",	"",	"",	""	},
	}
};





resource 'MENU' (kFontStyleMenuID) {
	kFontStyleMenuID,
	textMenuProc,
	allEnabled,
	enabled,
	"Style",
	{
		"Plain Text", noIcon, "T", noMark, plain,
		"-", noIcon, noKey, noMark, plain;
		"Bold", noIcon, "B", noMark, plain,
		"Italic", noIcon, "I", noMark, plain,
		"Underline", noIcon, "U", noMark, plain,
		"Outline", noIcon, noKey, noMark, plain,
	}
};
resource 'hmnu' (kFontStyleMenuID, purgeable) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	HMSkipItem {},	/* no missing items */
	{
		HMStringItem {
				"Apply the given styles to the currently selected text.",
				"",
				"",
				""
			},
		HMStringItem {
				"Change the selected text to plain (remove any styles)",
				"",
				"",
				""
			},
		HMSkipItem {},	/* separator line */
		HMStringItem {
				"Change the selected text to bold.",
				"",
				"",
				""
			},
		HMStringItem {
				"Change the selected text to italic.",
				"",
				"",
				""
			},
		HMStringItem {
				"Change the selected text to underline.",
				"",
				"",
				""
			},
		HMStringItem {
				"Change the selected text to outline.",
				"",
				"",
				""
			},
	}
};
















//-------------------------------------------------------------------------------------
// String Lists
//-------------------------------------------------------------------------------------

// Menu strings
resource 'STR#' (kMenuStringResID, "Menu Items strings") {
	{
		"About LedIt!É",
		"Hide Tool Palette",
		"Show Tool Palette",
		"Hide Color Palette",
		"Show Color Palette",
		"Undo Command",
		"Redo Command",
		"Undo New Shape",
		"Redo New Shape",
		"Undo Resize",
		"Redo Resize",
		"Undo Move",
		"Redo Move",
		"Undo Drop",
		"Redo Drop",
		"Undo Color Change",
		"Redo Color Change",
		"Undo Move Forward",
		"Redo Move Forward",
		"Undo Move Backward",
		"Redo Move Backward",
		"Undo Move First",
		"Redo Move First",
		"Undo Move Last",
		"Redo Move Last",
		"Undo Cut",
		"Redo Cut",
		"Undo Copy",
		"Redo Copy",
		"Undo Paste",
		"Redo Paste",
		"Undo Clear",
		"Redo Clear",
		"Undo Select All",
		"Redo Select All",
		"Undo Paste Link",
		"Redo Paste Link",
		"Link Info",
		"Undo Break Link",
		"Redo Break Link",
		"Undo Typing",
		"Redo Typing"
		
	}
};

// Error Text
resource 'STR#' (kErrorStringResID, "Error strings") {
	{	/* array StringArray: 6 elements */
		/* [1] */
		"ÒLedPartÓ was unable completely initi"
		"alize its internal structures.",
		/* [2] */
		"ÒLedPartÓ was unable to open the docu"
		"ment due to unforseen circumstances.",
		/* [3] */
		"ÒLedPartÓ was unable to open a part w"
		"indow due to unforseen circumstances.",
		/* [4] */
		"ÒLedPartÓ has been asked to remove a "
		"frame that does not belong to it, or an "
		"error occurred while removing the frame "
		"from internal storage.",
		/* [5] */
		"A window that ÒLedPartÓ created has b"
		"een deleted without its knowledge. This "
		"may generate a fatal error in the near f"
		"uture. Please close the document immedia"
		"tely to prevent data loss.",
		/* [6] */
		"ÒLedPartÓ encountered a fatal error w"
		"hile trying to save. Try saving again or"
		"closing the document."
	}
};











//-------------------------------------------------------------------------------------
// Finder Bundle Information
//-------------------------------------------------------------------------------------
resource 'kind' (kEditorBundle) {
	kEditorFileType,
	0,
	{
		'shlb', "OpenDocª editor"
	}
};
resource 'BNDL' (kEditorBundle) {
	kEditorFileType,
	0,
	{
		'FREF',
		{
			0, kEditorFREF
		},
		'ICN#',
		{
			0, kEditorIcons
		}
	}
};
resource 'FREF' (kEditorFREF) {
	'shlb',
	0,
	""
};
resource 'ICN#' (kEditorIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 1FFF FFF8 1FFF FFFC 1800 0204 F803 7207 8800 0205 8800 F205 8800 0205"
		$"8800 0205 8BF0 0205 8800 0205 8BD0 0205 8800 0205 8800 0205 8B18 3205 8802 022D"
		$"8BE4 0465 880C 08D5 8B96 11A5 882B E345 8855 4685 882A 8D05 8855 1A05 88AA 3405"
		$"8954 6805 88A8 D005 8951 A005 8AA2 4007 8943 0004 8E80 0004 FFFF FFFC",
		/* [2] */
		$"0000 0000 1FFF FFF8 1FFF FFF8 1FFF FFF8 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFF8 FFFF FFF8 FFFF FFF8"
	}
};
resource 'icl8' (kEditorIcons) {
	$"0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00 0000"
	$"0000 00FF 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 00F5 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"FFFF FFFF 0000 0000 0000 0000 0000 0000 0000 F5F6 2BF7 56F8 F8F8 F8F7 FFFF FFFF"
	$"FF00 F5F9 0000 0000 0000 0000 AC00 ACAC 00AC ACFD FDF8 56F8 F8F8 F7F7 F9F5 00FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 0000 00F5 F6F7 F856 F8F8 F8F7 F7F7 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 AC00 ACAC FDFD FD56 F8F7 F7F7 F72B F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 0000 F52B F7F8 56F9 F7F7 F7F7 2B2B F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 ACF8 F8F8 2BFD FDFA F7F7 F72B 2B2B F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 00F6 2BF7 56F9 F9FA F7F7 2B2C FFF6 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 F52B F756 56F9 FA81 2B2B 2C11 11F6 F92B F8FF"
	$"FF00 F6F9 0000 ACAC ACAC ACAC 00F7 F6F7 F856 F9FA 8181 2B2B 1011 2C2B F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 00FA F8F8 56F9 FA81 81FB 2B2C 1110 2B2B F92B F8FF"
	$"FF00 F6F9 0000 ACAC AC00 0000 56AC F956 F9FA 8181 FBFC 2B11 112C 2BF6 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 FBAC 56F8 FA81 FBFB 81FC 0F11 332B 2BF5 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 00F9 FDFA F5F5 56FA 8156 56FB 1111 F72B F6F5 F92B F8FF"
	$"FF00 F6F9 0000 00AC ACAC ACFC FBF6 F5F9 81FA 56FC FB34 1132 2B2B F500 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 FAAC 2BF5 F981 F9F9 8181 5611 102B 2BF6 F500 F92B F8FF"
	$"FF00 F6F9 0000 00AC ACFC FD56 0056 81F9 81FC FAF7 1011 2C2B F6F5 0000 F92B F8FF"
	$"FF00 F6F9 0000 0000 00FC FA00 F981 56F9 FB56 F607 1110 2B2B F500 0000 F92B F8FF"
	$"FF00 F6F9 0000 00AC FCFC 00F9 FA56 FCFA F7F6 F610 11F7 2BF6 F500 0000 F92B F8FF"
	$"FF00 F6F9 0000 0000 FDF6 56FA 8181 F92B F6F6 0811 332B F6F5 0000 0000 F92B F8FF"
	$"FF00 F6F9 0000 00FB 2B56 F9FC FB56 2BF6 F6F6 1011 2B2B F600 0000 0000 F92B F8FF"
	$"FF00 F6F9 0000 00FA F956 81FB 56F6 F6F5 F508 112C 2BF6 F500 0000 0000 F92B F8FF"
	$"FF00 F6F9 0000 5656 81AC FC56 F6F6 F5F6 F511 102B 2BF5 0000 0000 0000 F92B F8FF"
	$"FF00 F6F9 0000 56FA ACFC F9F6 F6F6 F5F5 3210 2C2B F6F5 0000 0000 0000 F9F7 F8FF"
	$"FF00 F6F9 00F9 FAFC FBF8 F6F6 F5F5 F5F6 5C32 F6F6 F500 0000 0000 0000 FFFF FFFF"
	$"FF00 F62B 81FA FA56 2BF6 F6F5 F5F5 F5F7 5CF5 F5F5 0000 0000 0000 0000 FF00 0000"
	$"FFF5 F8F8 2B2B 2BF6 F6F6 F5F5 F5F5 00F5 F500 0000 0000 0000 0000 0000 FF00 0000"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
resource 'icl4' (kEditorIcons) {
	$"0000 0000 0000 0000 0000 0000 0000 0000 000F FFFF FFFF FFFF FFFF FFFF FFFF F000"
	$"000F 0000 0000 0000 0000 0000 0000 F000 000F 0CCC CCCC CCCC CCCC CCCC CCCC F000"
	$"FFFF 0000 0000 0000 000C CCDC CCCC FFFF F00D 0000 0000 E0EE 0EEA ACDC CCCC D00F"
	$"F0CD 0000 0000 0000 00CC CDCC CCCC DCCF F0CD 0000 0000 00E0 EEAA ADCC CCCC DCCF"
	$"F0CD 0000 0000 0000 0CCC DDCC CCCC DCCF F0CD 0000 0000 00EC CCCA ADCC CCCC DCCF"
	$"F0CD 0000 0000 000C CCDD DDCC CCFC DCCF F0CD 0000 0000 000C CDDD DDCC C22C DCCF"
	$"F0CD 00EE EEEE 0CCC CDDD DDCC 22CC DCCF F0CD 0000 0000 0DCC DDDD DECC 22CC DCCF"
	$"F0CD 00EE E000 DEDD DDDD EEC2 2CCC DCCF F0CD 0000 0000 EEDC DDEE DE22 DCC0 DCCF"
	$"F0CD 0000 000D AD00 DDDD DE22 CCC0 DCCF F0CD 000E EEEE EC0D DDDE EB2C CC00 DCCF"
	$"F0CD 0000 00DE C0DD DDDD D22C CC00 DCCF F0CD 000E EEAD 0DDD DEDC 22CC C000 DCCF"
	$"F0CD 0000 0ED0 DDDD EDCC 22CC 0000 DCCF F0CD 000E EE0D DDED CCC2 2CCC 0000 DCCF"
	$"F0CD 0000 ACDD DDDC CCC2 DCC0 0000 DCCF F0CD 000E CDDE EDCC CC22 CCC0 0000 DCCF"
	$"F0CD 000D DDDE DCC0 0C2C CC00 0000 DCCF F0CD 00DD DEED CC0C 022C C000 0000 DCCF"
	$"F0CD 00DD EEDC CC00 C2CC C000 0000 DCCF F0CD 0DDE ECCC 000C DCCC 0000 0000 FFFF"
	$"F0CC DDDD CCC0 000C D000 0000 0000 F000 F0CC CCCC CC00 0000 0000 0000 0000 F000"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF F0"
};
resource 'ics#' (kEditorIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FFE 7FFE C01F DD1F C01F C01F D79B C037 DA6F C0DF D9BF C37F C6FF CDFF DFFE FFFE",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'ics8' (kEditorIcons) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FF00 00FC FCFC FCFB 81FA FAFA F956 F8F8 F800"
	$"FFFC 0000 0000 0000 0000 00F8 F8F8 F8FF FFFC 00FA F6F7 F7F7 0000 00F8 F8F8 F8FF"
	$"FFFC 0000 0000 0000 0000 00F7 F7F7 F7FF FFFB 00F9 F9F6 00F6 F700 00F7 F7FE F8FF"
	$"FF81 0000 0000 0000 0000 00F7 1111 F7FF FFFA 00F7 F7F9 F600 0000 0011 11F7 F7FF"
	$"FFFA 0000 0000 0000 0000 F711 11F7 F7FF FFF9 0000 0000 0000 00F7 1111 F7F7 F7FF"
	$"FFF9 00F7 F9F6 0000 F7F7 1111 F7F8 F7FF FFF8 0000 0000 00F7 F711 11F8 F8F8 F7FF"
	$"FFF8 0000 0000 F7F7 F711 11F8 F8F8 F7FF FFF8 0000 00F7 F7F7 1311 F8F8 F8F8 F7FF"
	$"FFF7 F8F8 F8F8 F8F8 F8F8 F8F8 F8F8 F700 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
resource 'ics4' (kEditorIcons) {
	$"0FFF FFFF FFFF FFF0 0EEE EEDD DDDD CCC0 FE00 0000 000C CCCF FE0D CCCC 000C CCCF"
	$"FE00 0000 000C CCCF FE0C CC0C C00C CFCF FD00 0000 000C 22CF FD0C CCC0 0002 2CCF"
	$"FD00 0000 00C2 2CCF FD00 0000 0C22 CCCF FD0C CC00 CC22 CCCF FC00 000C C22C CCCF"
	$"FC00 00CC C22C CCCF FC00 0CCC 22CC CCCF FC00 CCC4 4CCC CCC0 FFFF FFFF FFFF FFF0"
};





resource 'kind' (kViewerBundle) {
	kViewerFileType,
	0,
	{
		'shlb', "OpenDocª viewer"
	}
};
resource 'BNDL' (kViewerBundle) {
	kViewerFileType,
	0,
	{
		'FREF',
		{
			0, kViewerFREF
		},
		'ICN#',
		{
			0, kViewerIcons
		}
	}
};
resource 'FREF' (kViewerFREF) {
	'shlb',
	0,
	""
};
resource 'ICN#' (kViewerIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"0000 0000 1FFF FFF8 1FFF FFFC 1800 0204 F803 7207 8800 0205 8800 F205 8800 0205"
		$"8800 0205 8BF0 0205 8800 0205 8BD0 0205 8800 0205 8800 0205 8B18 3F05 8802 32C5"
		$"8BE4 2445 880C 4825 8B96 5025 882B E025 8855 4025 882A A045 8855 30C5 88AA 3F05"
		$"8954 6005 88A8 E005 8951 C005 8AA3 8007 8943 8004 8E80 0004 FFFF FFFC",
		/* [2] */
		$"0000 0000 1FFF FFF8 1FFF FFF8 1FFF FFF8 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFF8 FFFF FFF8 FFFF FFF8"
	}
};
resource 'icl8' (kViewerIcons) {
	$"0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000"
	$"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF00 0000"
	$"0000 00FF 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 00F5 FF00 0000"
	$"0000 00FF F5F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F6 F6F8 FF00 0000"
	$"FFFF FFFF 0000 0000 0000 0000 0000 0000 0000 F5F6 2BF7 56F8 F8F8 F8F7 FFFF FFFF"
	$"FF00 F5F9 0000 0000 0000 0000 AC00 ACAC 00AC ACFD FDF8 56F8 F8F8 F7F7 F9F5 00FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 0000 00F5 F6F7 F856 F8F8 F8F7 F7F7 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 AC00 ACAC FDFD FD56 F8F7 F7F7 F72B F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 0000 F52B F7F8 56F9 F7F7 F6F7 2B2B F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 ACF8 F8F8 2BFD FDFA F7F6 F6F6 F62B F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 00F6 2BF7 56F9 F9FA F7F6 F6F6 F6F6 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 0000 F52B F756 56F9 FA81 2BF6 F6F6 F6F6 F92B F8FF"
	$"FF00 F6F9 0000 ACAC ACAC ACAC 00F7 F6F7 F8FE FEFE FEFE FEF6 F6F6 F6F6 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 00FA F8F8 FEFE FA81 81FA FEFE F6F6 F6F6 F92B F8FF"
	$"FF00 F6F9 0000 ACAC AC00 0000 56AC F9FE F900 0000 00FC 2BFA FEF6 F6F6 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 0000 FBAC FEF8 0081 FBFB 81FC F6F6 FAFE F6F6 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 00F9 FDFA FEF8 56FA 8156 56FB F6F6 FAFE F6F6 F92B F8FF"
	$"FF00 F6F9 0000 00AC ACAC ACFC FBF6 FEF9 00FA 56FC FBF6 F6F6 FAFE F600 F92B F8FF"
	$"FF00 F6F9 0000 0000 0000 FAAC 2BF5 FE81 F9F9 8181 56F6 F6F6 FAFE F600 F92B F8FF"
	$"FF00 F6F9 0000 00AC ACFC FD56 0056 FEF9 81FC FAF7 F6F6 F6F6 FAFE F600 F92B F8FF"
	$"FF00 F6F9 0000 0000 00FC FA00 F981 56FE FB56 F6F6 F6F6 FAFA FEF6 F600 F92B F8FF"
	$"FF00 F6F9 0000 00AC FCFC 00F9 FA56 FCFA FEFE FAFA FAFA FEFE F6F6 F600 F92B F8FF"
	$"FF00 F6F9 0000 0000 FDF6 56FA 8181 F916 33FE FEFE FEFE FEF6 F6F6 0000 F92B F8FF"
	$"FF00 F6F9 0000 00FB 2B56 F9FC FB56 1616 33F6 F6F6 F6F6 F6F6 F6F6 0000 F92B F8FF"
	$"FF00 F6F9 0000 00FA F956 81FB 56F6 1633 F5F6 F6F6 F6F6 F6F6 F6F6 0000 F92B F8FF"
	$"FF00 F6F9 0000 5656 81AC FC56 F616 1633 F5F6 F6F6 F6F6 F6F6 F600 0000 F92B F8FF"
	$"FF00 F6F9 0000 56FA ACFC F9F6 1616 33F5 F6F6 F6F6 F6F6 F6F6 F600 0000 F9F7 F8FF"
	$"FF00 F6F9 00F9 FAFC FBF8 F616 1633 F5F6 F6F6 F6F6 F6F6 F600 0000 0000 FFFF FFFF"
	$"FF00 F62B 81FA FA56 2BF6 F616 1633 F5F6 F6F5 F5F5 0000 0000 0000 0000 FF00 0000"
	$"FFF5 F8F8 2B2B 2BF6 F6F6 F5F5 F5F5 00F5 F500 0000 0000 0000 0000 0000 FF00 0000"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
resource 'icl4' (kViewerIcons) {
	$"0000 0000 0000 0000 0000 0000 0000 0000 000F FFFF FFFF FFFF FFFF FFFF FFFF F000"
	$"000F 0000 0000 0000 0000 0000 0000 F000 000F 0CCC CCCC CCCC CCCC CCCC CCCC F000"
	$"FFFF 0000 0000 0000 000C CCDC CCCC FFFF F00D 0000 0000 E0EE 0EEA ACDC CCCC D00F"
	$"F0CD 0000 0000 0000 00CC CDCC CCCC DCCF F0CD 0000 0000 00E0 EEAA ADCC CCCC DCCF"
	$"F0CD 0000 0000 0000 0CCC DDCC CCCC DCCF F0CD 0000 0000 00EC CCCA ADCC CCCC DCCF"
	$"F0CD 0000 0000 000C CCDD DDCC CCCC DCCF F0CD 0000 0000 000C CDDD DDCC CCCC DCCF"
	$"F0CD 00EE EEEE 0CCC CFFF FFFC CCCC DCCF F0CD 0000 0000 0DCC FFDD DDFF CCCC DCCF"
	$"F0CD 00EE E000 DEDF D000 0ECD FCCC DCCF F0CD 0000 0000 EEFC 0DEE DECC DFCC DCCF"
	$"F0CD 0000 000D ADFC DDDD DECC DFCC DCCF F0CD 000E EEEE ECFD 0DDE ECCC DFC0 DCCF"
	$"F0CD 0000 00DE C0FD DDDD DCCC DFC0 DCCF F0CD 000E EEAD 0DFD DEDC CCCC DFC0 DCCF"
	$"F0CD 0000 0ED0 DDDF EDCC CCDD FCC0 DCCF F0CD 000E EE0D DDED FFDD DDFF CCC0 DCCF"
	$"F0CD 0000 ACDD DDD2 DFFF FFFC CC00 DCCF F0CD 000E CDDE ED22 DCCC CCCC CC00 DCCF"
	$"F0CD 000D DDDE DC2D 0CCC CCCC CC00 DCCF F0CD 00DD DEED C22D 0CCC CCCC C000 DCCF"
	$"F0CD 00DD EEDC 22D0 CCCC CCCC C000 DCCF F0CD 0DDE ECC2 2D0C CCCC CCC0 0000 FFFF"
	$"F0CC DDDD CCC2 2D0C C000 0000 0000 F000 F0CC CCCC CC00 0000 0000 0000 0000 F000"
	$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF F0"
};
resource 'ics#' (kViewerIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FFE 7FFE C01F DD1F C01F C07F D7C7 C0BB DABB C0C7 D99F C3BF C73F CE7F DE7E FFFE",
		/* [2] */
		$"FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
	}
};
resource 'ics8' (kViewerIcons) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF FF00 00FC FCFC FCFB 81FA FAFA F956 F8F8 F800"
	$"FFFC 0000 0000 0000 0000 00F8 F8F8 F8FF FFFC 00FA F6F7 F7F7 0000 00F8 F8F8 F8FF"
	$"FFFC 0000 0000 0000 0000 00F7 F7F7 F7FF FFFB 00F9 F9F6 00F6 F700 00F7 F8F8 F8FF"
	$"FF81 0000 0000 0000 FFFF FFF7 F8F8 F7FF FFFA 00F7 F7F9 F6FF 0000 00FF F8F8 F7FF"
	$"FFFA 0000 0000 00FF 0000 F7FF F8F7 F7FF FFF9 0000 0000 00FF 00F7 F8FF F7F7 F7FF"
	$"FFF9 00F7 F9F6 0000 FFFF FFF8 F7F8 F7FF FFF8 0000 0000 00F8 16F8 F8F8 F8F8 F7FF"
	$"FFF8 0000 0000 F816 16F8 F8F8 F8F8 F7FF FFF8 0000 00F8 1616 F8F8 F8F8 F8F8 F7FF"
	$"FFF7 F8F8 F8F8 1616 F8F8 F8F8 F8F8 F700 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FF"
};

resource 'ics4' (kViewerIcons) {
	$"0FFF FFFF FFFF FFF0 0EEE EEDD DDDD CCC0 FE00 0000 000C CCCF FE0D CCCC 000C CCCF"
	$"FE00 0000 000C CCCF FE0D DC0C C00C CCCF FD00 0000 FFFC CCCF FD0C CDCF 000F CCCF"
	$"FD00 000F 00CF CCCF FD00 000F 0CCF CCCF FD0C DC00 FFFC CCCF FC00 000C 2CCC CCCF"
	$"FC00 00C2 2CCC CCCF FC00 0C22 CCCC CCCF FCCC CC22 CCCC CCC0 FFFF FFFF FFFF FFF0"
};





resource 'kind' (kDocumentBundle) {
	kODShellSignature,
	0,
	{
		kDocumentFileType,		"LedIt! OpenDoc document",
		kStationeryFileType,	"LedIt! OpenDoc stationery"
	}
};
resource 'BNDL' (kDocumentBundle) {
	kODShellSignature,
	0,
	{
		'FREF',
		{
			0, kDocumentFREF,
			1, kStationeryFREF
		},
		'ICN#',
		{
			0, kDocumentIcons,
			1, kStationeryIcons
		}
	}
};
resource 'FREF' (kDocumentFREF) {
	kDocumentFileType,
	0,
	""
};
resource 'FREF' (kStationeryFREF) {
	kStationeryFileType,
	1,
	""
};
resource 'ICN#' (kDocumentIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"1FFF FC00 1000 0E00 1477 0D00 1444 8C80 1464 8C40 1444 8C20 1777 0FF0 1000 0FF0"
		$"1000 07F0 1000 0070 1000 0030 1100 0030 1300 0030 1070 0030 100F 0230 1670 0330"
		$"1948 0030 1948 0230 1948 0410 1671 0910 1002 1210 1000 2490 1008 4110 1012 9010"
		$"1085 2410 10CA 4890 1064 9130 1031 2A50 1038 5490 101C A110 100E 0010 1FFF FFF0",
		/* [2] */
		$"1FFF FC00 1FFF FE00 1FFF FF00 1FFF FF80 1FFF FFC0 1FFF FFE0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
		$"1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0 1FFF FFF0"
	}
};
resource 'icl8' (kDocumentIcons) {
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
	$"0000 00FF F8EC ECF8 F8EC ECEC F8F8 F8F8 F600 0000 1616 FFFF F8F8 FAFF 0000 0000"
	$"0000 00FF ECF8 F8EC F8EC F8F7 ECF8 F8F6 0000 0000 0000 00F6 F8F8 FAFF 0000 0000"
	$"0000 00FF ECF8 F8EC F8EC F8F8 ECF8 F600 0000 0000 0000 D800 F6F8 FAFF 0000 0000"
	$"0000 00FF ECF8 F8EC F8EC F8F8 ECF6 0000 0000 0000 00D8 0000 00F6 F6FF 0000 0000"
	$"0000 00FF F8EC ECF8 F8EC ECEC F600 00FA 0000 0000 D800 00FA 0000 00FF 0000 0000"
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
resource 'icl4' (kDocumentIcons) {
	$"000F FFFF FFFF FFFF FFFF FF00 0000 0000 000F CCCC CCCC CCCC CCCC DFF0 0000 0000"
	$"000F CDCC CDDD CDDD CCCC DFCF 0000 0000 000F CDCC CDCC CDCC DCCC DFCC F000 0000"
	$"000F CDCC CDDC CDCC DCCC DFCC CF00 0000 000F CDCC CDCC CDCC DCCC DDCC CCF0 0000"
	$"000F CDDD CDDD CDDD CCCC DFFF FFFF 0000 000F CCCC CCCC CCCC CCCC DDDD DDDF 0000"
	$"000F CCCC CCCC CCCC CCCC CDDD DDDF 0000 000F CCCC CCCC CCCC CCCC CCCC CDDF 0000"
	$"000F CCCC CCCC CCCC CCCC CCCC CCDF 0000 000F CCC4 2222 CCCC CCCC CCCC CCDF 0000"
	$"000F CCDD 2222 2222 CCCC CCCC CCDF 0000 000F CCCC CDDD 2222 2222 CCCC CCDF 0000"
	$"000F CCCC CCCC DDDD 2222 22FC CCDF 0000 000F C66C C666 CCCC C000 22FF CCDF 0000"
	$"000F 6CC6 C6CC 6CCC 0000 000C CCDF 0000 000F 6CC6 C6CC 6CC0 0000 0030 CCDF 0000"
	$"000F 6CC6 C6CC 6C00 0000 0300 0CCF 0000 000F C66C C666 C00D 0000 300D 000F 0000"
	$"000F CCCC CCCC 00D0 0003 00D0 000F 0000 000F CCCC CCC0 0000 0030 0D00 D00F 0000"
	$"000F CCCC CC00 D000 0300 000D 000F 0000 000F CCCC C00D 00D0 300D 0000 000F 0000"
	$"000F CCCC D000 0D03 00D0 0D00 000F 0000 000F CCCC DD00 D030 0D00 D000 D00F 0000"
	$"000F CCCC CDD0 0300 D00D 000D 00DF 0000 000F CCCC CCDE 000D 00D0 D0D0 0D0F 0000"
	$"000F CCCC CCDD E000 0D0D 0D00 D00F 0000 000F CCCC CCCD DE00 D0D0 000D 000F 0000"
	$"000F CCCC CCCC DDE0 0000 0000 000F 0000 000F FFFF FFFF FFFF FFFF FFFF FFFF"
};
resource 'ics#' (kDocumentIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FF0 6FB8 6EAC 7F3C 403C 5004 5C04 4314 4004 4134 417C 47D4 4BEC 4FFC 46FC 7FFC",
		/* [2] */
		$"7FF0 7FF8 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC 7FFC"
	}
};
resource 'ics8' (kDocumentIcons) {
	$"00FF FFFF FFFF FFFF FFFF FFFF 0000 0000 00FF FAF8 FAFA FAFA FAF8 FFFF FF00 0000"
	$"00FF FAF8 FAFA FAF8 FAF8 FF2B FFFF 0000 00FF FAFA FAFA FAFA F8F8 FFFF FFFF 0000"
	$"00FF F8F8 F8F8 F8F8 F8F8 FAFA FAFF 0000 00FF F869 F8F8 F8F8 F8F8 F8F8 F8FF 0000"
	$"00FF F8FA FAFA 1616 F8F8 F8F8 F8FF 0000 00FF F8F8 F8F8 FAFA F616 16FF F8FF 0000"
	$"00FF F8F8 F8F8 F8F8 0000 00F6 F8FF 0000 00FF F8F8 F8F8 F6FA 0000 D8FA F6FF 0000"
	$"00FF F8F8 F8F8 00FA 00D8 FAFA FAFF 0000 00FF F8F8 F8FA FAFA D8FA 00FA 00FF 0000"
	$"00FF F8F8 FA00 FAD8 FAFA FA00 FAFF 0000 00FF F8F8 FAFB D8FA FAFA FAFA FAFF 0000"
	$"00FF F8F8 F8FA FB00 FAFA FAFA FAFF 0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
};
resource 'ics4' (kDocumentIcons) {
	$"0FFF FFFF FFFF 0000 0FDC DDDD DCFF F000 0FDC DDDC DCFC FF00 0FDD DDDD CCFF FF00"
	$"0FCC CCCC CCDD DF00 0FC4 CCCC CCCC CF00 0FCD DD22 CCCC CF00 0FCC CCDD C22F CF00"
	$"0FCC CCCC 000C CF00 0FCC CCCD 003D CF00 0FCC CC0D 03DD DF00 0FCC CDDD 3D0D 0F00"
	$"0FCC D0D3 DDD0 DF00 0FCC DE3D DDDD DF00 0FCC CDE0 DDDD DF00 0FFF FFFF FFFF FF"
};

resource 'ICN#' (kStationeryIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"3FFF FFE0 2000 0020 2008 EE38 2008 8928 2008 C928 2008 8928 200E EE28 2000 0028"
		$"2000 0028 2200 0028 2000 0028 2000 0028 2000 0428 2CE0 0628 3290 0028 3290 0428"
		$"3290 0828 2CE2 1228 2004 2428 2000 4928 2010 8228 2025 2028 210A 4028 2194 8FE8"
		$"20C9 2848 2062 4888 2070 A908 2039 4A08 201C 0C08 3FFF F808 0800 0008 0FFF FFF8",
		/* [2] */
		$"3FFF FFE0 3FFF FFE0 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8"
		$"3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 3FFF FFF8 0FFF FFF8 0FFF FFF8"
	}
};
resource 'icl8' (kStationeryIcons) {
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
	$"0000 FFF5 ECEC F5F5 ECEC ECF5 F5F5 F5F6 0000 0016 16FF FFF5 F5F5 FFF9 FF00 0000"
	$"0000 FFEC F5F5 ECF5 ECF5 F5EC F5F5 F600 0000 0000 0000 F6F5 F5F5 FFF9 FF00 0000"
	$"0000 FFEC F5F5 ECF5 ECF5 F5EC F5F6 0000 0000 0000 00D8 00F6 F5F5 FFF9 FF00 0000"
	$"0000 FFEC F5F5 ECF5 ECF5 F5EC F600 0000 0000 0000 D800 0000 F6F6 FFF9 FF00 0000"
	$"0000 FFF5 ECEC F5F5 ECEC ECF6 0000 FA00 0000 00D8 0000 FA00 0000 FFF9 FF00 0000"
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
resource 'icl4' (kStationeryIcons) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFF0 0000 00F0 0000 0000 0000 0000 0000 00F0 0000"
	$"00F0 0000 0000 D000 DDD0 DDD0 00FF F000 00F0 0000 0000 D000 D000 D00D 00FD F000"
	$"00F0 0000 0000 D000 DD00 D00D 00FD F000 00F0 0000 0000 D000 D000 D00D 00FD F000"
	$"00F0 0000 0000 DDD0 DDD0 DDD0 00FD F000 00F0 0000 0000 0000 0000 0000 00FD F000"
	$"00F0 0CC0 0000 0000 0000 0000 00FD F000 00F0 0C42 2220 0000 0000 0000 00FD F000"
	$"00F0 0002 2222 2220 0000 0000 00FD F000 00F0 0000 0002 2222 222C 0000 00FD F000"
	$"00F0 0000 0000 0002 2222 2F00 00FD F000 00F0 6600 6660 000C 0002 2FF0 00FD F000"
	$"00F6 0060 6006 00C0 0000 00C0 00FD F000 00F6 0060 6006 0C00 0000 030C 00FD F000"
	$"00F6 0060 6006 C000 0000 3000 CCFD F000 00F0 6600 666C 00D0 0003 00D0 00FD F000"
	$"00F0 0000 00C0 0D00 0030 0D00 00FD F000 00F0 0000 0C00 0000 0300 D00D 00FD F000"
	$"00F0 0000 C00D 0000 3000 00D0 00FD F000 00F0 000C 00D0 0D03 00D0 0000 00FD F000"
	$"00F0 000D 0000 D030 0D00 0000 00FD F000 00F0 000D D00D 0300 D000 FFFF FFFD F000"
	$"00F0 0000 DD00 300D 00D0 FCCC CFDC F000 00F0 0000 0DE0 00D0 0D00 FCCC FDCC F000"
	$"00F0 0000 0DDE 0000 D0D0 FCCF DCCC F000 00F0 0000 00DD E00D 0D00 FCFD CCCC F000"
	$"00F0 0000 000D DE00 0000 FFDC CCCC F000 00FF FFFF FFFF FFFF FFFF FDCC CCCC F000"
	$"0000 FDDD DDDD DDDD DDDD DCCC CCCC F000 0000 FFFF FFFF FFFF FFFF FFFF FFFF F0"
};
resource 'ics#' (kStationeryIcons) {
	{	/* array: 2 elements */
		/* [1] */
		$"7FFC 42F6 42B6 43F6 4006 4006 4036 4026 4176 42F6 47D6 5FBE 4FEA 4FF2 7FE2 3FFE",
		/* [2] */
		$"7FFC 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 7FFE 3FFE"
	}
};
resource 'ics8' (kStationeryIcons) {
	$"00FF FFFF FFFF FFFF FFFF FFFF FFFF 0000 00FF 0000 00F5 FAF5 FAFA FAFA F5FF FF00"
	$"00FF 0000 00F5 FAF5 FAF5 FAFA F5FF FF00 00FF 0000 00F5 FAFA FAFA FAFA F5FF FF00"
	$"00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF FF00 00FF F5F5 F5F5 16F5 F5F6 F5F5 F5FF FF00"
	$"00FF F5F5 F5F5 F5F6 1616 FFFF F5FF FF00 00FF F5F5 F5F5 F6F6 0000 D8F6 F5FF FF00"
	$"00FF F5F5 F5F6 F6FA 00D8 D8FA F6FF FF00 00FF F5F5 F6F6 FA00 D8D8 FAFA 00FF FF00"
	$"00FF F5F6 F6FA FAD8 D8FA 00FA 00FF FF00 00FF F5FA FAFA FAD8 FA00 FFFF FFFF FF00"
	$"00FF F5F5 FAFB D8FA FAFA FF2B FFF9 FF00 00FF F5F5 FAFB FBFA FAFA FFFF F92B FF00"
	$"00FF FFFF FFFF FFFF FFFF FFF9 2B2B FF00 0000 FFFF FFFF FFFF FFFF FFFF FFFF FF"
};
resource 'ics4' (kStationeryIcons) {
	$"0FFF FFFF FFFF FF00 0F00 00D0 DDDD 0FF0 0F00 00D0 D0DD 0FF0 0F00 00DD DDDD 0FF0"
	$"0F00 0000 0000 0FF0 0F00 0020 0C00 0FF0 0F00 000C 22FF 0FF0 0F00 00CC 003C 0FF0"
	$"0F00 0CCD 033D CFF0 0F00 CCD0 33DD 0FF0 0F0C CDD3 3D0D 0FF0 0F0D DDD3 D0FF FFF0"
	$"0F00 DE3D DDFC FDF0 0F00 DEED DDFF DCF0 0FFF FFFF FFFD CCF0 00FF FFFF FFFF FFF0"
};









data kODShellSignature (0, "Owner resource") {
	$"00"                                                 /* . */
};
data kEditorFileType (0, "Owner resource") {
	$"00"                                                 /* . */
};
data kViewerFileType (0, "Owner resource") {
	$"00"                                                 /* . */
};




#if 0
// doesn't work .. must be some other type???
resource	'hfdr' (kHMHelpID) {
	HelpMgrVersion, hmDefaultOptions, 0, 0,
	{
		HMStringItem {	"Hi mom"	},
	}
};
#endif








resource 'DLOG' (kAboutBoxID, "About Box") {
	{0, 0, 214, 389},
	dBoxProc,
	visible,
	goAway,
	0x0,
	kAboutBoxID,
	"",
	centerParentWindowScreen
};
resource 'DITL' (kAboutBoxID, purgeable) {
	{
		{181, 291, 201, 371},
		Button {
			enabled,
			"OK"
		},
		{-1, -1, 0, 0},
		Button {
			disabled,
			"cancel"
		},
		{16, 70, 175, 380},
		StaticText {
			disabled,
			"LedIt! OpenDoc part editor ^0 (^1)\n"
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
			kAboutBoxID
		}
	}
};
data 'cicn' (kAboutBoxID) {
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





resource 'DLOG' (kErrorBoxID, "Error Box") {
	{57, 46, 244, 374},
	dBoxProc,
	visible,
	goAway,
	0x0,
	kErrorBoxID,
	"",
	alertPositionParentWindowScreen                                           
};
resource 'DITL' (kErrorBoxID) {
	{	/* array DITLarray: 5 elements */
		/* [1] */
		{153, 256, 173, 314},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{153, 185, 173, 243},
		Button {
			enabled,
			"Cancel"
		},
		/* [3] */
		{11, 70, 124, 317},
		StaticText {
			disabled,
			"<< Use this for errors in the Editor, su"
			"ch as out of memory, unable to acquire s"
			"ome system resource, etc.>> \nUser errors"
			" use a standard alert appearance. See DI"
			"TL 129.>>"
		},
		/* [4] */
		{11, 19, 43, 51},
		Picture {
			disabled,
			5010
		},
		/* [5] */
		{149, 252, 177, 318},
		UserItem {
			disabled
		}
	}
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
	alertPositionMainScreen
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
			"The text '^0' could be in any of a number of different formats. Iterpret the file as:"
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








resource 'DLOG' (kPrefs_DialogID) {
	{64, 79, 170, 340},
	movableDBoxProc,
	visible,
	goAway,
	0x0,
	kPrefs_DialogID,
	"Preferences for this frame",
	centerParentWindowScreen
};
resource 'DITL' (kPrefs_DialogID) {
	{
		{72, 53, 92, 111},
		Button {
			enabled,
			"OK"
		},
		{71, 157, 91, 215},
		Button {
			enabled,
			"Cancel"
		},
		{9, 21, 30, 156},
		CheckBox {
			enabled,
			"Show Scrollbar"
		}
	}
};



















//----------------------------------------------------------------------------------------
// AETE for Semantic Interface
//----------------------------------------------------------------------------------------




#define english 0

resource 'aete' (english, "Led Terminology") {
	0x01,
	0x00,
	english,
	roman,
	{	/* array Suites: 1 element  */
		/* [1] */
		"Led Suite",
		"Suite pertaining to Led.",
		kLedPartOSType,
		1,
		1,
		{	/* array Events: 0 elements */
		},
		{	/* array Classes: 0 elements */
			/* [3] */
			"Led",
			kLedPartOSType,
			"The root Led object.",
			{	/* array Properties: 1 element */
				/* [1] */
				"contents",
				pContents,
				typeChar,
				"The text displayed by the part.",
				reserved,
				singleItem,
				notEnumerated,
				readWrite,
				reserved, reserved, reserved, reserved, reserved, reserved, reserved, reserved,
				noApostrophe,
				notFeminine,
				notMasculine,
				singular,
			},
			{	/* array Elements: 0 elements */
			},
		},
		{	/* array ComparisonOps: 0 elements */
		},
		{
		},
	},
};

