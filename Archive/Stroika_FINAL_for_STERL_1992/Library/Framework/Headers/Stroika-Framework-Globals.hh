/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */
#ifndef	__Stroika_Framework_Globals__
#define	__Stroika_Framework_Globals__

/*
 * $Header: /fuji/lewis/RCS/Stroika-Framework-Globals.hh,v 1.3 1992/09/01 15:42:04 sterling Exp $
 *
 * Description:
 *
 *		This file is needed to gaurentee the initialization order of file scope objects. Since there are
 *		often dependencies among these constants, it is important to initialize them in the right order, and
 *		C++'s mechanisms to support that sort of dependency are less than magnificent. This would seem to be
 *		the best we can do.
 *
 *
 * TODO:
 *
 *
 * Notes:
 *
 *
 * Changes:
 *	$Log: Stroika-Framework-Globals.hh,v $
 *		Revision 1.3  1992/09/01  15:42:04  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.2  1992/07/15  21:59:25  lewis
 *		Scope time constant reference.
 *
 *		Revision 1.9  1992/03/17  06:26:21  lewis
 *		Fixed marquee patterns to use new format for PixelMapFromData - at same time
 *		fixed bug where they were reading garbage off end of array... Also, fixed
 *		default pixelmap for pixelmapbutton.
 *
 *		Revision 1.7  1992/01/05  05:55:15  lewis
 *		Added some globals strings that were in pushbutton.cc and used elsehere.
 *
 *
 *
 */

#include	"Stroika-Graphix-Globals.hh"



/*
 * Globals in adornemt.hh
 */
#include	"Adornment.hh"

const	Point	ShapeAdornment::kDefaultInset = Point (-1, -1);
const	Pen		ShapeAdornment::kDefaultPen = Pen (kBlackTile, eCopyTMode, Point (1,1));



#include	"FocusItem.hh"
FocusException	FocusItem::sFocusFailed;




#include	"PixelMapButton.hh"
static	UInt8	kAbstractPixelMapButton_DefaultPixMap_Data [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
const PixelMap	AbstractPixelMapButton::kDefaultPixelMap	=	PixelMapFromData (Point (16, 16), kAbstractPixelMapButton_DefaultPixMap_Data, sizeof (kAbstractPixelMapButton_DefaultPixMap_Data));


#include	"PushButton.hh"
const	Boolean	AbstractPushButton::kIsDefaultButton	= True;
const	String	AbstractPushButton::kOKLabel			= "OK";
const	String	AbstractPushButton::kCancelLabel		= "Cancel";
const	String	AbstractPushButton::kYesLabel			= "Yes";
const	String	AbstractPushButton::kNoLabel			= "No";


#include	"Alert.hh"
const	String	StagedAlert::kDefaultButtonTitle = AbstractPushButton::kOKLabel;


/* 
 * Globals for View.hh
 */
#include	"View.hh"

#include	"OSRenamePre.hh"
#if			qMacToolkit
#include	<Quickdraw.h>
#include	<ToolUtils.h>
#elif		qXtToolkit
#include	<X11/cursorfont.h>
#endif		/*Toolkit*/
#include	"OSRenamePost.hh"

const	Font	kApplicationFont	=	Font ("Geneva", 9);
const	Font	kSystemFont			=	Font ("Chicago", 12);
const	Font	kDefaultFont		=	kApplicationFont;
const	Brush	kDefaultBrush		=	Brush (kBlackTile, eCopyTMode);
const	Pen		kDefaultPen			=	Pen (kBlackTile, eCopyTMode, Point (1, 1));
const	Tile	kDefaultBackground	=	kWhiteTile;

#if		qMacGDI
const	PixelMapCursor	kArrowCursor	=	PixelMapCursor (&qd.arrow);
const	PixelMapCursor	kIBeamCursor	=	PixelMapCursor (iBeamCursor);
const	PixelMapCursor	kCrossCursor	=	PixelMapCursor (crossCursor);
const	PixelMapCursor	kPlusCursor		=	PixelMapCursor (plusCursor);
const	PixelMapCursor	kWatchCursor	=	PixelMapCursor (watchCursor);
#elif	qXGDI
const	PixelMapCursor	kArrowCursor	=	PixelMapCursor (XC_arrow);
const	PixelMapCursor	kIBeamCursor	=	PixelMapCursor (XC_xterm);
const	PixelMapCursor	kCrossCursor	=	PixelMapCursor (XC_X_cursor);
const	PixelMapCursor	kPlusCursor		=	PixelMapCursor (XC_crosshair);
const	PixelMapCursor	kWatchCursor	=	PixelMapCursor (XC_watch);
#endif	/* GDI */



#include	"Tracker.hh"
const	Time	Tracker::kDefaultTimeOut		=	0.5;	// should be based on double click time
const	Pen		Tracker::kDefaultFeedbackPen	= 	Pen (kGrayTile, eNotXorTMode, Point (1, 1));


const	UInt8	kMarquee1 [] = {	0xFC, 0xF9, 0xF3, 0xE7, 0xCF, 0x9F, 0x3F, 0x7E	};
const	UInt8	kMarquee2 [] = {	0xF3, 0xE7, 0xCF, 0x9F, 0x3F, 0x7E, 0xFC, 0xF9	};
const	UInt8	kMarquee3 [] = {	0xCF, 0x9F, 0x3F, 0x7E, 0xFC, 0xF9, 0xF3, 0xE7	};
const	UInt8	kMarquee4 [] = {	0x3F, 0x7E, 0xFC, 0xF9, 0xF3, 0xE7, 0xCF, 0x9F	};
const	Rect	kPatternRect = Rect (kZeroPoint, Point (8, 8));
#if		qMPW_CLASS_SCOPED_ARRAYOFOBJS_WITH_NO_NOARG_CTOR_BUG
const	Tile	kXXXXMarqueArray [4] = {
	Tile (PixelMapFromData (Point (8, 8), kMarquee1, sizeof (kMarquee1))),
	Tile (PixelMapFromData (Point (8, 8), kMarquee2, sizeof (kMarquee2))),
	Tile (PixelMapFromData (Point (8, 8), kMarquee3, sizeof (kMarquee3))),
	Tile (PixelMapFromData (Point (8, 8), kMarquee4, sizeof (kMarquee4))),
};
const	Tile*	Selector::kMarqueArray	=	kXXXXMarqueArray;
#else	/*qMPW_CLASS_SCOPED_ARRAYOFOBJS_WITH_NO_NOARG_CTOR_BUG*/
const	Tile	Selector::kMarqueArray [4] = {
	Tile (PixelMapFromData (Point (8, 8), kMarquee1, sizeof (kMarquee1))),
	Tile (PixelMapFromData (Point (8, 8), kMarquee2, sizeof (kMarquee2))),
	Tile (PixelMapFromData (Point (8, 8), kMarquee3, sizeof (kMarquee3))),
	Tile (PixelMapFromData (Point (8, 8), kMarquee4, sizeof (kMarquee4))),
};
#endif	/*qMPW_CLASS_SCOPED_ARRAYOFOBJS_WITH_NO_NOARG_CTOR_BUG*/




// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***


#endif	/*__Stroika_Framework_Globals__*/

