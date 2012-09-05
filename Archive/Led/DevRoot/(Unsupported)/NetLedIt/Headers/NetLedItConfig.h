/* Copyright(c) Sophist Solutions, Inc. 1994-2001.  All rights reserved */
#ifndef	__NetLedItConfig_h__
#define	__NetLedItConfig_h__	1

/*
 * $Header: /cygdrive/k/CVSRoot/(Unsupported)/NetLedIt/Headers/NetLedItConfig.h,v 1.1 2004/01/01 04:20:22 lewis Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: NetLedItConfig.h,v $
 *	Revision 1.1  2004/01/01 04:20:22  lewis
 *	moved NetLedIt to (Unsupported)
 *	
 *	Revision 1.15  2003/03/11 02:35:52  lewis
 *	SPR#1288 - use new MakeSophistsAppNameVersionURL and add new CheckForUpdatesWebPageCommand
 *	
 *	Revision 1.14  2002/05/06 21:34:56  lewis
 *	<=============================== Led 3.0.1 Released ==============================>
 *	
 *	Revision 1.13  2001/11/27 00:32:57  lewis
 *	<=============== Led 3.0 Released ===============>
 *	
 *	Revision 1.12  2001/09/26 15:53:04  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.11  2001/09/18 14:03:47  lewis
 *	shorten define name from qStandardURLStyleMarkerNewDisplayMode to qURLStyleMarkerNewDisplayMode
 *	(avoid MSDEV60 warnings for define name length)
 *	
 *	Revision 1.10  2001/08/30 00:43:17  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.9  2001/08/28 12:43:50  lewis
 *	qStandardURLStyleMarkerNewDisplayMode declared here so will compile on mac
 *	
 *	Revision 1.8  2001/07/19 21:06:58  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.7  2001/05/30 15:25:59  lewis
 *	SPR#0945- Added GetVersionNumber/GetShortVersionString () APIs, and testing code
 *	to demo them, and docs in the ref manual
 *	
 *	Revision 1.6  2001/04/17 23:07:47  lewis
 *	*** empty log message ***
 *	
 *	Revision 1.5  2001/04/12 19:46:39  lewis
 *	SPR#0861- Expiration support for demo-mode
 *	
 *	Revision 1.4  2000/10/22 17:37:31  lewis
 *	SPR#0841- change version# scheme for NetLedIt! binary version#
 *	
 *	Revision 1.3  2000/10/03 13:40:19  lewis
 *	SPR#0840- new WaterMarkHelper<> template for watermark/demomode. SPR#0839- use new
 *	WordProcessorCommonCommandHelper/TextInteractorCommonCommandHelper<> templates.
 *	A bit more code handling context menus (still more todo)
 *	
 *	Revision 1.2  2000/09/30 20:04:59  lewis
 *	Added preliminary (no display done yet) DemoMode support
 *	
 *	Revision 1.1  2000/09/29 00:18:23  lewis
 *	*** empty log message ***
 *	
 *
 *
 *
 *
 *
 */
#include	"LedConfig.h"

#if		defined (__cplusplus)
	#include	"LedSupport.h"
#endif

#if		qLedUsesNamespaces
using	namespace	Led;
#endif



#define	qURLStyleMarkerNewDisplayMode		1



#if		defined (__cplusplus)
	// right now, undo levels waste LOTS of memory, and can slow things down. Leave this number small til
	// we tweek the implementation a bit more - LGP 960119
	const	int	kMaxNumUndoLevels	=	3;
#endif

#ifndef	qDemoMode
#define	qDemoMode	0
#endif

#if		qDemoMode
#define	kDemoString " - Demo Version"
#else
#define	kDemoString ""
#endif

#if		qDemoMode
	#define	kVERSResourceVersionNumber	qLed_Version_MajorMinor,qLed_Version_Stage,qLed_Version_SubStage,0
#else
	#define	kVERSResourceVersionNumber	qLed_Version_MajorMinor,qLed_Version_Stage,qLed_Version_SubStage,1
#endif

#if		defined (__cplusplus)
inline	unsigned long	mkNetLedItVersion_ (unsigned int majorMinor, unsigned int versStage, unsigned int subStage, unsigned int nonDemoFlag)
	{
		Led_Require (majorMinor <= 256);
		Led_Require (versStage <= 256);
		Led_Require (subStage <= 256);
		Led_Require (nonDemoFlag <= 256);
		return
				(static_cast<unsigned long> (majorMinor)<<24) |
				(static_cast<unsigned long> (versStage)<<16) |
				(static_cast<unsigned long> (subStage)<<8) |
				(static_cast<unsigned long> (nonDemoFlag)<<0)
			;
	}
const unsigned long kNetLedItDWORDVersion = mkNetLedItVersion_ (qLed_Version_MajorMinor,qLed_Version_Stage,qLed_Version_SubStage,!qDemoMode);
#endif



#if		defined (__cplusplus)
	//#if		qMacOS
	//const	char	kNetLedItUserDocsURL[]		=	"http://www.sophists.com/Led/NetLedIt/Default.asp?From=NetLedIt30M";
	//const	char	kNetLedItUserPageURL[]		=	"http://www.sophists.com/Led/NetLedIt/Default.asp?From=NetLedIt30M";
	//const	char	kNetLedItDemoExpiredURL[]	=	"http://www.sophists.com/Led/NetLedIt/Default.asp?From=NetLedIt30M&DemoExpired=true";
	//const	char	kSophistSolutionsURL[]		=	"http://www.sophists.com/Default.asp?From=NetLedIt30M";
	//#elif	qWindows
	//const	char	kNetLedItUserDocsURL[]		=	"http://www.sophists.com/Led/NetLedIt/Default.asp?From=NetLedIt30W";
	//const	char	kNetLedItUserPageURL[]		=	"http://www.sophists.com/Led/NetLedIt/Default.asp?From=NetLedIt30W";
	//const	char	kNetLedItDemoExpiredURL[]	=	"http://www.sophists.com/Led/NetLedIt/Default.asp?From=NetLedIt30W&DemoExpired=true";
	//const	char	kSophistSolutionsURL[]		=	"http://www.sophists.com/Default.asp?From=NetLedIt30W";
	//#endif
	const	char	kAppName[]				=	"NetLedIt";
	#if		qDemoMode
		const	char	kURLDemoFlag[]			=	"&DemoMode=true";
	#else
		const	char	kURLDemoFlag[]			=	"";
	#endif
	const	char	kDemoExpiredExtraArgs[]	=	"&DemoExpired=true";
#endif


#endif	/*__NetLedItConfig_h__*/

// For gnuemacs:
// Local Variables: ***
// mode:c++ ***
// tab-width:4 ***
// End: ***

