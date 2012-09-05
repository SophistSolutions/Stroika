/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1996.  All rights reserved */

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItODPart/Headers/LedPartDef.h,v 2.7 1996/12/13 18:09:57 lewis Exp $
 *
 * Description:
 *		LedPart Common constants used by .cpp & .r file
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: LedPartDef.h,v $
 *	Revision 2.7  1996/12/13 18:09:57  lewis
 *	<========== Led 21 Released ==========>
 *	
 *	Revision 2.6  1996/09/01  15:44:28  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.5  1996/02/26  23:07:44  lewis
 *	Added kLedPrivateDocumentFileType, etc.
 *
 *	Revision 2.4  1996/02/05  05:05:16  lewis
 *	Added dialogs Find and Prefs
 *
 *	Revision 2.3  1995/12/15  04:00:35  lewis
 *	add define qSTTXTSupported 0 cuz we don't support this file type yet.
 *	Added defines for string messages for Undo/Redo typing.
 *
 *	Revision 2.2  1995/12/08  07:19:57  lewis
 *	Lose more upsolete defines
 *
 *	Revision 2.1  1995/12/06  02:00:23  lewis
 *	ripped out lots of draw editor shit, and mostly got embedding working.
 *	Ripped out promise crap as well.
 *	Linking is probably broken. Made little attempt to preserve it.
 *
 *	Revision 2.0  1995/11/25  00:39:49  lewis
 *	*** empty log message ***
 *
 *	Revision 2.3  1995/11/05  02:11:27  lewis
 *	Cleaned up alot of resource stuff. Registered Creators/file types etc.
 *	and put right values here.
 *
 *	Revision 2.2  1995/11/02  22:46:41  lewis
 *	Got rid of many of the obsolete defintiions and tried getting
 *	nmap stuff right.
 *	Several changes to nmap and reosurce definition / naming stuff.
 *
 *	Revision 2.1  1995/10/09  23:05:32  lewis
 *	most cleanups based on reviewing SamplePartC++ from DR3.
 *	and hookin better with resource file
 *
 * Revision 2.0  1995/09/06  21:05:39  lewis
 * *** empty log message ***
 *
 *
 *
 *
 */
#ifndef _LEDPARTDEF_
#define _LEDPARTDEF_



// We can either build an OpenDoc part using styled text, or the basic mono-font editor
#ifndef	qUsingStyledText
	#define	qUsingStyledText	1
#endif


// Currently not yet supported - real soon now!!!
#define	qSTTXTSupported		0





/*
 *	From the OpenDoc cookbook, menu (and probably all) resids for editors should
 *	be in the 20001 and up range.
 */
#define kBaseResourceID					20001



// Class / Editor ID
#define	kPartClassName				"som_LedPart"
#define kLedPartID					"SophistSolutions::som_LedPart"


// ISO Strings
#define kMainPresentation  			kODISOPrefix "LedPart:Presentation:Main"
#define kLedPartInfo				kODISOPrefix "LedPart:Display Frame Info"




// Creators (you must modify these to make them unique, register them!!!)
#define kDocumentFileType						'LDOD'	// Led OpenDoc Document
#define kStationeryFileType						'sDOD'	// Led OpenDoc Document Stationery
#define kEditorFileType							'LDED'	// PartMaker Part Editor
#define kViewerFileType							'LDVW'	// PartMaker Part Viewer (type of LedPart)
#define kLedPrivateDocumentFileType				'LDDC'	// LedIt! Portable (mac/pc) rich text document
#define kLedPrivateDocumentStationeryFileType	'sDDC'	// LedIt! Portable (mac/pc) rich text stationary

#define kFileType			'sDOD'	// Led OpenDoc Document Stationery


// NOT SURE WHAT THIS SHOULD BE - USED IN APPLE EVEN SEMANTIC INTERFACE????
// LGP 951104
#define	kLedPartOSType				'ledd'





// Dialogs & Windows
#define	kAboutBoxID  					kBaseResourceID+1
#define	kErrorBoxID  					kBaseResourceID+2
#define	kPickInputFileFormat_AlertID	kBaseResourceID+3
#define	kFind_DialogID					kBaseResourceID+4
#define	kPrefs_DialogID					kBaseResourceID+5


// Bundles
#define kDocumentBundle					kBaseResourceID+1
#define kEditorBundle					kBaseResourceID+2
#define kViewerBundle					kBaseResourceID+3


// FREFs
#define kDocumentFREF					kBaseResourceID+1
#define kStationeryFREF					kBaseResourceID+2
#define kEditorFREF						kBaseResourceID+3
#define kViewerFREF						kBaseResourceID+4


// Icons
#define	kDocumentIcons					kBaseResourceID
#define	kStationeryIcons				kBaseResourceID+1
#define	kEditorIcons					kBaseResourceID+2
#define	kViewerIcons					kBaseResourceID+3


// Menu text Items
#define	kMenuStringResID			kBaseResourceID
#define	kAboutTextID        		1
#define	kHideToolPaletteIndex		2
#define	kShowToolPaletteIndex		3
#define	kHideColorPaletteIndex		4
#define	kShowColorPaletteIndex		5
#define	kUndoCommandIndex			6
#define	kRedoCommandIndex			7
#define	kUndoNewShapeIndex			8
#define	kRedoNewShapeIndex			9
#define	kUndoResizeIndex			10
#define	kRedoResizeIndex			11
#define	kUndoMoveIndex				12
#define	kRedoMoveIndex				13
#define	kUndoDropIndex				14
#define	kRedoDropIndex				15
#define	kUndoColorChangeIndex		16
#define	kRedoColorChangeIndex		17
#define	kUndoMoveForwardIndex		18
#define	kRedoMoveForwardIndex		19
#define	kUndoMoveBackwardIndex		20
#define	kRedoMoveBackwardIndex		21
#define	kUndoMoveFrontIndex			22
#define	kRedoMoveFrontIndex			23
#define	kUndoMoveBackIndex			24
#define	kRedoMoveBackIndex			25
#define	kUndoCutIndex				26
#define	kRedoCutIndex				27
#define	kUndoCopyIndex				28
#define	kRedoCopyIndex				29
#define	kUndoPasteIndex				30
#define	kRedoPasteIndex				31
#define	kUndoClearIndex				32
#define	kRedoClearIndex				33
#define	kUndoSelectAllIndex			34
#define	kRedoSelectAllIndex			35
#define	kUndoPasteLinkIndex			36
#define	kRedoPasteLinkIndex			37
#define kLinkInfoIndex				38
#define kUndoBreakLinkIndex			39
#define kRedoBreakLinkIndex			40
#define	kUndoTypingIndex			41
#define	kRedoTypingIndex			42

// Error Messages
#define kErrorStringResID				kMenuStringResID+1
#define kErrStrFieldID					3
#define kErrCantInitializePart			1
#define kErrCantOpenDocWindow			2
#define kErrCantOpenPartWindow			3
#define kErrRemoveFrame					4
#define kErrWindowGone					5
#define kErrExternalizeFailed			6

#define kODErrInvalidParameter 		21000 



// Kind
#define kLedPartKind					kODISOPrefix "SophistSolutions:Kind:LedPart"
#define kTextDataKind					'TEXT'
#define kTextFileKind					'TEXT'


// Category
#define kLedPartCategory	kODCategoryStyledText



// nmap Resources
#define	kNMAPResIDBase				kBaseResourceID
#define kKindCategoryMapId			kNMAPResIDBase+1
#define kEditorKindMapId			kNMAPResIDBase+2
#define kEditorUserStringMapId		kNMAPResIDBase+3
#define kKindUserStringMapId		kNMAPResIDBase+4
#define kCategoryUserStringMapId	kNMAPResIDBase+5
#define kOldMacOSTypeMapId			kNMAPResIDBase+6
#define kPlatformEditorKindMapId	kNMAPResIDBase+7



/*
 *	From the OpenDoc cookbook, submenu ids for part editors should be
 *	in the range 194-255 (or 0xC2-0xff)
 */

// The menu's resources
#define	kFormatMenuID					kBaseResourceID
#define kFontMenuID               		0x0f0
#define kFontSizeMenuID               	0x0f1
#define	kFontStyleMenuID				0x0f2




#endif
// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***




