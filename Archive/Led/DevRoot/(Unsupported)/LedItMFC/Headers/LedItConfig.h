/* Copyright(c) Lewis Gordon Pringle, Jr. 1994-1998.  All rights reserved */
#ifndef	__LedItConfig__
#define	__LedItConfig__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/LedItMFC/Headers/LedItConfig.h,v 2.6 2000/04/14 22:40:52 lewis Exp $
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
 *	Revision 2.6  2000/04/14 22:40:52  lewis
 *	SPR#0740- namespace support
 *	
 *	Revision 2.5  1997/12/24 04:46:26  lewis
 *	*** empty log message ***
 *	
 *	Revision 2.4  1997/12/24  04:06:39  lewis
 *	default to RTF format
 *
 *	Revision 2.3  1997/07/27  16:01:09  lewis
 *	<===== Led 2.2 Released =====>
 *
 *	Revision 2.2  1996/12/13  18:02:41  lewis
 *	<========== Led 21 Released ==========>
 *
# Revision 2.1  1996/12/05  21:16:12  lewis
# *** empty log message ***
#
# Revision 2.0  1996/09/30  14:53:44  lewis
# *** empty log message ***
#
 *
 *
 *
 */

#include	"LedConfig.h"



// Didn't make it into 2.0 - sigh...LGP 960520
#define	qSupportPasteSpecial	0


#ifndef	qSupportStyleSheets
	#define	qSupportStyleSheets	0
#endif






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
				eDefaultFormat		=	eRTFFormat,
			};
#endif



#if		defined (__cplusplus)
	// right now, undo levels waste LOTS of memory, and can slow things down. Leave this number small til
	// we tweek the implementation a bit more - LGP 960119
	const	kMaxNumUndoLevels	=	3;
#endif


#endif	/*__LedItConfig__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***
