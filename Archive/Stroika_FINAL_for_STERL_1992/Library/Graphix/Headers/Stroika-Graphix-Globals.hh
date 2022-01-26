/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Stroika_Graphix_Globals__
#define	__Stroika_Graphix_Globals__

/*
 * $Header: /fuji/lewis/RCS/Stroika-Graphix-Globals.hh,v 1.3 1992/07/14 19:45:24 lewis Exp $
 *
 * Description:
 *
 *		Global variables whose order of construction is important.
 *
 * TODO:
 *
 *
 * Notes:
 *
 * Changes:
 *	$Log: Stroika-Graphix-Globals.hh,v $
 *		Revision 1.3  1992/07/14  19:45:24  lewis
 *		Change usage of Time stuff - cuz public interface changed.
 *
 *		Revision 1.2  1992/07/08  01:59:51  lewis
 *		Re-enable using PixelMapFromData () for predefined tiles (on mac).
 *
 *		Revision 1.1  1992/06/19  22:33:01  lewis
 *		Initial revision
 *
 *		Revision 1.16  1992/04/30  13:35:17  sterling
 *		new color defs
 *
 *		Revision 1.15  92/04/02  11:21:35  11:21:35  lewis (Lewis Pringle)
 *		Add osDisplay* gDisplay for X - soon rename to gOSDisplay.
 *		
 *		Revision 1.14  92/03/22  16:47:05  16:47:05  lewis (Lewis Pringle)
 *		Re-enabled hack for mac tile constructions since portable one didn't work--
 *		must debug soon!!! Since portable bitmap stuff not working perfecrtly for unix either.
 *		
 *		Revision 1.11  1992/03/17  03:04:48  lewis
 *		PixelMapFromData () takes UInt8* instead of void* - no longer UInt16 arrays.
 *
 *		Revision 1.10  1992/02/14  02:42:31  lewis
 *		Scoped region exception inside class region.
 *
 *		Revision 1.9  1992/02/12  03:02:30  lewis
 *		Use & in front of access to mac patterns cuz apple changed definition in latest compiler release.
 *
 *		Revision 1.8  1992/01/28  10:39:26  lewis
 *		Use new PixelMapFromData () routines under UNIX, and when I get a chance, try on mac an get rid of conditional
 *		compiles.
 *
 *
 */

#include	"Stroika-Foundation-Globals.hh"





#include	"Point.hh"
const	Point	kZeroPoint	=	Point (0, 0);
const	Point	kMinPoint	=	Point (kMinCoordinate, kMinCoordinate);
const	Point	kMaxPoint	=	Point (kMaxCoordinate, kMaxCoordinate);


#include	"Rect.hh"
const	Rect	kZeroRect	=	Rect (kZeroPoint, kZeroPoint);



#include	"Cursor.hh"

const	Point	PixelMapCursor::kDefaultHotSpot			=	Point (-1, -1);
const	Time	AnimatedCursor::kDefaultFrameInterval	=	Time (0.1);


#include	"Color.hh"
const	Color	kBlackColor		=	Color (kMinComponent, kMinComponent, kMinComponent);
const	Color	kWhiteColor		=	Color (kMaxComponent, kMaxComponent, kMaxComponent);

const	Color	kRedColor		=	Color (kMaxComponent, kMinComponent, kMinComponent);
const	Color	kGreenColor		=	Color (kMinComponent, kMaxComponent, kMinComponent);
const	Color	kBlueColor		=	Color (kMinComponent, kMinComponent, kMaxComponent);

const	Color	kCyanColor		=	Color (kMinComponent, kMaxComponent, kMaxComponent);
const	Color	kMagentaColor	=	Color (kMaxComponent, kMinComponent, kMaxComponent);
const	Color	kYellowColor	=	Color (kMaxComponent, kMaxComponent, kMinComponent);

const	Color	kLightGrayColor	=	Color (kMaxComponent, kMaxComponent, kMaxComponent) * .75;
const	Color	kGrayColor		=	Color (kMaxComponent, kMaxComponent, kMaxComponent) * .50;
const	Color	kDarkGrayColor	=	Color (kMaxComponent, kMaxComponent, kMaxComponent) * .25;


#include	"ColorLookupTable.hh"

static	ColorLookupTable	MakeBlackAndWhiteCLUT ()
{
	ColorLookupTable	clut;
	clut.SetEntryCount (2, kBlackColor);
	clut.SetEntry (1, kWhiteColor);
	return (clut);
}
const	ColorLookupTable	kBlackAndWhiteCLUT	=	MakeBlackAndWhiteCLUT ();


#include	"Font.hh"
const	Set_BitString(FontStyle)	kPlainFontStyle;


#include	"Region.hh"

Exception	Region::sException;

const	Region	kEmptyRegion = Region ();
//const	Region	kBigRegion	=	Rect (Point (kMinInt16, kMinInt16), Point (kMaxInt16, kMaxInt16));
const	Region	kBigRegion	=	Rect (Point (-10000, -10000), Point (20000, 20000));




#include	"Tile.hh"

// turn back on old mac hack since new stuff didn't work - must SOLVE SOON!
// LGP March 22, 1992!!!!

// Try using PixelMapFromData () again - LGP July 7, 1992

#if			0 && qMacGDI
#include	"OSRenamePre.hh"
#include	<QuickDraw.h>
#include	"OSRenamePost.hh"

// eventually get rid of these globals and make accessors from PalletMgr???

const	Tile	kWhiteTile		=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.white, 1));
const	Tile	kLightGrayTile	=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.ltGray, 1));
const	Tile	kGrayTile		=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.gray, 1));
const	Tile	kDarkGrayTile	=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.dkGray, 1));
const	Tile	kBlackTile		=	Tile (PixelMap (Rect (kZeroPoint, Point (8, 8)), &qd.black, 1));
#else

// This code should work on the mac too, and we should convert!!!
// LGP, Jan 27, 1992
static	UInt8	kWhite_Array[]		=	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, };
const	Tile	kWhiteTile			=	Tile (PixelMapFromData (Point (8, 8), kWhite_Array, sizeof (kWhite_Array)));
static	UInt8	kLightGray_Array[]	=	{ 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, 0x88, 0x22, };
const	Tile	kLightGrayTile		=	Tile (PixelMapFromData (Point (8, 8), kLightGray_Array, sizeof (kLightGray_Array)));
static	UInt8	kGray_Array[]		=	{ 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, };
const	Tile	kGrayTile			=	Tile (PixelMapFromData (Point (8, 8), kGray_Array, sizeof (kGray_Array)));
static	UInt8	kDarkGray_Array[]	=	{ 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, 0x77, 0xdd, };
const	Tile	kDarkGrayTile		=	Tile (PixelMapFromData (Point (8, 8), kDarkGray_Array, sizeof (kDarkGray_Array)));
static	UInt8	kBlack_Array[]		=	{ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, };
const	Tile	kBlackTile			=	Tile (PixelMapFromData (Point (8, 8), kBlack_Array, sizeof (kBlack_Array)));


#endif		/*GDI*/


#if		qXGDI
// We need this global all over the place - not sure this is a good place, or a sensible way of dealing
// with the problem, but I cannot think of a better one - LGP April 2, 1992
osDisplay*      gDisplay;
#endif


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***



#endif	/*__Stroika_Graphix_Globals__*/

