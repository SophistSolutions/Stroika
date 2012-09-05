/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItConfig__
#define	__LedItConfig__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItPP/Headers/LedItConfig.h,v 2.11 2000/04/15 14:42:54 lewis Exp $
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
 *	Revision 2.11  2000/04/15 14:42:54  lewis
 *	qLedUsesNamespaces support, and FlavorPackage.cpp added
 *	
 *	Revision 2.10  1997/12/24 04:41:30  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.9  1997/12/24  03:55:35  lewis
 *	default to RTF - not LedPrivateFormat
 *
 *	Revision 2.8  1997/07/27  16:01:58  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.7  1997/03/04  20:35:56  lewis
 *	*** empty log message ***
 *
 *	Revision 2.6  1996/12/13  18:11:04  lewis
 *	<========== Led 21 Released ==========>
 *
 *	Revision 2.5  1996/12/05  21:23:57  lewis
 *	*** empty log message ***
 *
 *	Revision 2.4  1996/09/30  15:06:27  lewis
 *	Added HTMLFormat, and eDefaultFormat (RTF was there from before?).
 *	Moved kMaxUndoLevels here to config.
 *
 *	Revision 2.3  1996/09/03  15:14:51  lewis
 *	Added eRTFFormat - RTF file format.
 *
 *	Revision 2.2  1996/09/01  15:45:23  lewis
 *	***Led 20 Released***
 *
 *	Revision 2.1  1996/07/03  01:36:59  lewis
 *	*** empty log message ***
 *
 *	Revision 2.0  1996/02/26  23:29:06  lewis
 *	*** empty log message ***
 *
 *
 *
 *
 */

#include	"LedConfig.h"

#define	kApplicationSignature					'LDAP'

#define kTEXTFileType							'TEXT'
#define kTEXTStationeryFileType					'sEXT'
#define kLedPrivateDocumentFileType				'LDDC'	// LedIt! Portable (mac/pc) rich text document
#define kLedPrivateDocumentStationeryFileType	'sDDC'	// LedIt! Portable (mac/pc) rich text stationary

#if		defined (__cplusplus)
	#if		qLedUsesNamespaces
	using	namespace	Led;
	#endif
	enum	Led_FileFormat {
				eTextFormat,
				eLedPrivateFormat, 
				eRTFFormat, 
				eHTMLFormat, 
				eUnknownFormat,
				eDefaultFormat		=	eRTFFormat
			};
#endif


#if		defined (__cplusplus)
	// right now, undo levels waste LOTS of memory, and can slow things down. Leave this number small til
	// we tweek the implementation a bit more - LGP 960119
	const	kMaxNumUndoLevels	=	3;
#endif


// Eventually make it a runtime choice based on a user-config dialog.
#ifndef	qSupportSmartCutNPaste
	#define	qSupportSmartCutNPaste	1
#endif


#endif	/*__LedItConfig__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
