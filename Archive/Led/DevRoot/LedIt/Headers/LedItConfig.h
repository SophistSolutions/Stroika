/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__LedItConfig__
#define	__LedItConfig__	1

/*
 * $Header: /cygdrive/k/CVSRoot/LedIt/Headers/LedItConfig.h,v 1.12 2003/11/26 23:19:33 lewis Exp $
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
 *	$Log: LedItConfig.h,v $
 *	Revision 1.12  2003/11/26 23:19:33  lewis
 *	set maxundolevels to 512 - based on SPR#1565
 *	
 *	Revision 1.11  2003/06/02 16:15:40  lewis
 *	SPR#1513: Simple spellcheck engine and spell check dialog support (windows only so far)
 *	
 *	Revision 1.10  2002/10/30 22:43:56  lewis
 *	SPR#1158 - SetDefaultWindowMargins () API now uses Led_TWIPS
 *	
 *	Revision 1.9  2002/10/24 15:53:14  lewis
 *	SPR#0680- adjust the window margins so text looks a little better. Did for MacOS/XWin/Win - but only tested on Win so far
 *	
 *	Revision 1.8  2002/05/06 21:30:57  lewis
 *	<========================================= Led 3.0.1 Released ========================================>
 *	
 *	Revision 1.7  2001/11/27 00:28:08  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.6  2001/09/26 15:41:45  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.5  2001/09/18 19:13:42  lewis
 *	SPR#0791- added support for toggle (use smart cut&paste, wrap-to-window,show hidden text) 
 *	commands for MacOS
 *	
 *	Revision 1.4  2001/09/18 14:02:41  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 1.3  2001/08/29 22:59:16  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.2  2001/08/27 20:17:00  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.1  2001/05/14 20:54:43  lewis
 *	New LedIt! CrossPlatform app - based on merging LedItPP and LedItMFC and parts of LedTextXWindows
 *	
 *
 *
 *
 *	<<<***		Based on LedItMFC/LedItPP project from Led 3.0b6		***>>>
 *
 *
 *
 *
 */

#include	"LedConfig.h"

#if		defined (__cplusplus)
#include	"LedGDI.h"
#endif


#if		qMacOS
#define	kApplicationSignature					'LDAP'

#define kTEXTFileType							'TEXT'
#define kTEXTStationeryFileType					'sEXT'
#define kLedPrivateDocumentFileType				'LDDC'	// LedIt! Portable (mac/pc) rich text document
#define kLedPrivateDocumentStationeryFileType	'sDDC'	// LedIt! Portable (mac/pc) rich text stationary
#endif



// Didn't make it into 2.0 - sigh...LGP 960520
#define	qSupportPasteSpecial	0


#ifndef	qSupportStyleSheets
	#define	qSupportStyleSheets	0
#endif

#define	qURLStyleMarkerNewDisplayMode	1

#ifndef	qIncludeBasicSpellcheckEngine
#define	qIncludeBasicSpellcheckEngine	1
#endif


#if		defined (__cplusplus)

	#if		qLedUsesNamespaces
	using	namespace	Led;
	#endif

	enum	FileFormat {
				eTextFormat,
				eLedPrivateFormat, 
				eRTFFormat, 
				eHTMLFormat, 
				eUnknownFormat,
				eDefaultFormat		=	eRTFFormat
			};
#endif



#if		defined (__cplusplus)
	// Due to work on SPR#1564 and 1565 - up this undo count from 3 to something effectively infinite.
	const	unsigned	int	kMaxNumUndoLevels	=	512;
#endif



#if		defined (__cplusplus)
	const	Led_TWIPS	kLedItViewTopMargin		=	Led_TWIPS (120);
	const	Led_TWIPS	kLedItViewBottomMargin	=	Led_TWIPS (0);
	const	Led_TWIPS	kLedItViewLHSMargin		=	Led_TWIPS (150);
	const	Led_TWIPS	kLedItViewRHSMargin		=	Led_TWIPS (0);
#endif


// Eventually make it a runtime choice based on a user-config dialog (on mac - already chooseable for PC).
//#ifndef	qSupportSmartCutNPaste
//	#define	qSupportSmartCutNPaste	1
//#endif


#endif	/*__LedItConfig__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
