/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__GraphixDevice__
#define	__GraphixDevice__

/*
 * $Header: /fuji/lewis/RCS/GraphixDevice.hh,v 1.2 1992/09/01 15:34:49 sterling Exp $
 *
 * Description:
 *
 *
 * TODO:
 *
 *
 * Notes:
 *		This concept may map on to the X concept of connection - may be a sensible place to put some
 *		of my wrapper code???
 *
 * Changes:
 *	$Log: GraphixDevice.hh,v $
 *		Revision 1.2  1992/09/01  15:34:49  sterling
 *		Lots of Foundation changes.
 *
 *
 *
 *
 */

#include	"ColorLookupTable.hh"
#include	"PixelMap.hh"

#if		qMacGDI
struct osGDevice;
#endif	/*qMacGDI*/

class	GraphixDevice {
	public:
		enum	DeviceType {
			eIndirectDevice,
			eFixedDevice,
			eDirectDevice
		};
#if		qMacGDI
		GraphixDevice (osGDevice** anOSGDevice);
#endif	/*qMacGDI*/
		GraphixDevice (DeviceType deviceType);

		/*
		 * Not sure these necessary, but IM allows them, and they are reasonable
		 * to support portably.
		 */
		nonvirtual	DeviceType	GetDeviceType ();
		nonvirtual	Boolean		SupportsColor ();
		nonvirtual	Boolean		MainScreen ();
		nonvirtual	Boolean		DeviceActive ();

		/*
		 * Indirect access to PixelMap.
		 */
		nonvirtual	Color		operator[] (ColorIndex i);
		nonvirtual	ColorIndex	operator[] (const Color& c);	/* approximates - if necessary */


#if		qMacGDI
		nonvirtual	osGDevice**	GetOSGDevice ();		// Back door access.
#endif	/*qMacGDI*/

	private:
#if		qMacGDI
		osGDevice**	fGDevice;
#endif	/*qMacGDI*/
};


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


#endif	/*__GraphixDevice__*/

