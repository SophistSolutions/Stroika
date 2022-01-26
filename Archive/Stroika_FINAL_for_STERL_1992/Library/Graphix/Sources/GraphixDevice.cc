/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/GraphixDevice.cc,v 1.4 1992/09/05 16:14:25 lewis Exp $
 *
 *
 * TODO:
 *
 *
 * Changes:
 *	$Log: GraphixDevice.cc,v $
 *		Revision 1.4  1992/09/05  16:14:25  lewis
 *		Renamed NULL->Nil.
 *
 *		Revision 1.3  1992/09/01  15:36:53  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/21  05:38:17  lewis
 *		Added workaround for qCFront_NestedTypesHalfAssed to get compiling under gcc.
 *
 *		Revision 1.1  1992/06/19  22:34:01  lewis
 *		Initial revision
 *
 *		Revision 1.5  1992/02/12  03:31:32  lewis
 *		Use GDIConfig rather than OSConfig.
 *
 */

#include	"OSRenamePre.hh"
#if		qMacGDI
#include	<Memory.h>
#include	<OSUtils.h>
#include	<QuickDraw.h>
#endif	/*qMacGDI*/
#include	"OSRenamePost.hh"


#include	"Debug.hh"

#include	"GDIConfiguration.hh"

#include	"GraphixDevice.hh"






/*
 ********************************************************************************
 ************************************* GraphixDevice ****************************
 ********************************************************************************
 */

#if		qMacGDI
GraphixDevice::GraphixDevice (osGDevice** anOSGDevice):
	fGDevice (anOSGDevice)
{
	RequireNotNil (anOSGDevice);
}
#endif	/*qMacGDI*/


// Need global rect param!!!
// Need DTOR so we , and flag saying if we created gdevice so we should destroy it!!!

GraphixDevice::GraphixDevice (DeviceType deviceType)
#if		qMacGDI
	:fGDevice (Nil)
#endif	/*GDI*/
{
#if		qMacGDI
// see develop #1, FracApp, 1990
	if (GDIConfiguration ().ColorQDAvailable ()) {
		osGDevice**		oldDevice	=	::GetGDevice ();
		osGrafPort*	savedPort	=	qd.thePort;

		fGDevice = ::NewGDevice (noDriver, -1);
		AssertNotNil (fGDevice);
		(*fGDevice)->gdRefNum = noDriver;		//	driver's unit number
		(*fGDevice)->gdID = 0;					//	no ID for search & complement procs
		(*fGDevice)->gdType = clutType;			//  color table type fer sure (MUST CHANGE)
		(*fGDevice)->gdITable = Nil;			//	Build later...
		(*fGDevice)->gdResPref = 3;				//	preferred resolution of GDITable
		(*fGDevice)->gdSearchProc = Nil;		//	no search proc
		(*fGDevice)->gdCompProc = Nil;			//	no comp proc
//		(*fGDevice)->gdFlags = 2**0 + 2**10 + 2**14 + 2**15;	//	{ set each bit we need. };
//		Dont change		(*fGDevice)->gdPMap directly...
//		(*fGDevice)->gdRefCon = 0;				//	FRACAPP DIDNT SET THIS!
//		(*fGDevice)->gdNextGD = Nil;			//	FRACAPP DIDNT SET THIS!
//		(*fGDevice)->gdRect = ?;				//	SHOULD BE OARANM FROM CONSTRUTOR!!!
		(*fGDevice)->gdMode = Nil;				//	FRACAPP DIDNT SET THIS!
		(*fGDevice)->gdCCBytes = 0;				//	FRACAPP DIDNT SET THIS!
		(*fGDevice)->gdCCXData = Nil;			//	FRACAPP DIDNT SET THIS!
		(*fGDevice)->gdCCXMask = Nil;			//	FRACAPP DIDNT SET THIS!
		(*fGDevice)->gdReserved = 0;			//	future use. MUST BE 0
 
 		/*
		 * Now munge gdPMap...
		 */
//		::DisposCTable ((*fGDevice)->pmTable);		//	kill the stub that is there.
// NEED PARAM OR SOMETHING?
//		(*fGDevice)->pmTable = gOurColors;			//	make a copy of our global color table.
//		Erry := HandToHand (Handle(gdPMap^^.pmTable));
		
//		{ build a new iTable for this device }
//		::MakeITable ((*fGDevice)->pmTable, (*fGDevice)->gdITable, (*fGDevice)->gdResPref);
// DO SIMILAR:	FailOSErr (QDError);{ no memory, we can leave here. }
		
		
//		{ Now set up the fields in the offscreen PixMap }
//		(*(*fGDevice)->gdPMap)->baseAddr := fBigBuff;	{ The base address is our buffer. }
//		gdPMap^^.bounds := sizeOfDoc;	{ bounding rectangle to our device. }

//		gdPMap^^.rowBytes := docW + $8000;
//		gdPMap^^.pixelSize := 8;
//		gdPMap^^.cmpCount := 1;
//		gdPMap^^.cmpSize := 8;
		
//		gdRect := sizeOfDoc;{ the bounding rectangle for gDevice, too. }
	}
#endif	/*qMacGDI*/
}

#if		!qCFront_NestedTypesHalfAssed
GraphixDevice::
#endif
DeviceType	GraphixDevice::GetDeviceType ()
{
	return (eIndirectDevice);	// Thats what I have attached to my machine :-)
}

Boolean		GraphixDevice::SupportsColor ()
{
#if		qMacGDI
	return Boolean (::TestDeviceAttribute (GDHandle (fGDevice), gdDevType));
#endif	/*qMacGDI*/
}

Boolean		GraphixDevice::MainScreen ()
{
#if		qMacGDI
	return Boolean (::TestDeviceAttribute (GDHandle (fGDevice), mainScreen));
#endif	/*qMacGDI*/
}

Boolean		GraphixDevice::DeviceActive ()
{
#if		qMacGDI
	return Boolean (::TestDeviceAttribute (GDHandle (fGDevice), screenActive));
#endif	/*qMacGDI*/
}

Color	GraphixDevice::operator[] (ColorIndex i)
{
	Assert (False);
	return (kBlackColor);
}

ColorIndex	GraphixDevice::operator[] (const Color& c)
{
	Assert (False);
	if (c == kBlackColor) {
		return (1);
	}
	else {
		return (0);
	}
}

#if		qMacGDI
osGDevice**	GraphixDevice::GetOSGDevice ()
{
	return (fGDevice);
}
#endif	/*qMacGDI*/




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

