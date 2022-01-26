/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/OSRenamePre.hh,v 1.2 1992/09/01 15:20:18 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *		Cannot wrap this file in #ifndef cuz sometimes gets included multiple times
 *		an in these cases, it SHOULD redefine the symbols again. OSRenamePost renames
 *		them back. (eg Foundation-Globals.hh).
 *
 *
 * Changes:
 *	$Log: OSRenamePre.hh,v $
 *		Revision 1.2  1992/09/01  15:20:18  sterling
 *		*** empty log message ***
 *
 *		Revision 1.18  92/05/18  15:08:57  15:08:57  lewis (Lewis Pringle)
 *		Call Mac os Menu osMenu instead of osMenuInfo.,
 *		
 *		Revision 1.17  92/04/27  18:34:10  18:34:10  lewis (Lewis Pringle)
 *		Added rename for struct Depth for XGDI
 *		
 *		Revision 1.16  92/04/16  10:52:20  10:52:20  lewis (Lewis Pringle)
 *		Added osRandom() for mac.
 *		
 *		Revision 1.14  92/04/02  11:04:12  11:04:12  lewis (Lewis Pringle)
 *		Added reanme for XGDI for Dimension and Position.
 *		
 *		Revision 1.13  92/04/02  11:01:57  11:01:57  lewis (Lewis Pringle)
 *		Get rid of #ifndef wrapping file.
 *
 *
 */

#if		qMacOS

// From <Controls.h>
#define		ControlRecord	osControlRecord


// From <Dialogs.h>
#define		Alert			osAlert
#define		CautionAlert	osCautionAlert
#define		NoteAlert		osNoteAlert
#define		StopAlert		osStopAlert


// From <Events.h>
#define		Button			osButton
#define		EventRecord		osEventRecord


// From <File.h>
#define		FSSpec			osFSSpec
#define		VersRec			osVersRec


// From <Memory.h>
#define		Size			osSize


// From <Menu.h>
#define		EnableItem		osEnableItem
#define		MenuInfo		osMenu
#define		MenuPtr			osMenuPtr
#define		SystemMenu		osSystemMenu


// From <Print.h>
#define		TPrint			osPrintRecord		
#define		TPrPort			osTPrPort	


// From <Quickdraw.h>
#define		BitMap			osBitMap
#define		ColorSpec		osColorSpec
#define		ColorTable		osColorTable
#define		Cursor			osCursor
#define		Region			osRegion
#define		GrafPort		osGrafPort
#define		CGrafPort		osCGrafPort
#define		FontInfo		osFontInfo
#define		GDevice			osGDevice
#define		Line			osLine
#define		Pattern			osPattern
#define		PixMap			osPixMap
#define		PixPat			osPixPat
#define		Polygon			osPolygon
#define		Picture			osPicture
#define		Random			osRandom
#define		SetCursor		osSetCursor


// From <Sound.h>
#define		Time			osTime


// From <TextEdit.h>
#define		TextBox			osTextBox
#define		TextStyle		osTextStyle

// From <ToolUtils.h>
#define		SetString	osSetString

// From <Types.h>
#define		Boolean			osBoolean
#define		Handle			osHandle
#define		Point			osPoint
#define		ProcHandle		osProcHandle
#define		ProcPtr			osProcPtr
#define		Ptr				osPtr
#define		Rect			osRect
#define		Str255			osStr255
#define		Str63			osStr63
#define		Str31			osStr31
#define		StringPtr		osStringPtr
#define		StringHandle	osStringHandle
#define		Style			osStyle


// From <Window.h>
#define		WindowPeek		osWindowPeek
#define		WindowPtr		osWindowPtr

#elif	qXGDI

#define	_WidgetClassRec		osWidgetClass
#define	_WidgetRec			osWidget
#define	_XDisplay			osDisplay
#define	_XEvent				osEventRecord
#define	_XImage				osImage
#define	_XRegion			osRegion
#define	Boolean				osBoolean
#define	Colormap			osColormap
#define	Cursor				osCursor
#define	Depth				osDepth
#define	Dimension			osDimension
#define	Font				osFont
#define	KeyCode				osKeyCode
#define	Picture				osPicture
#define	PixMap				osPixMap
#define	Position			osPosition
#define	Region				osRegionPtr
#define	String				osString
#define	Time				osTime
#define	Visual				osVisual
#define	Widget				osWidgetPtr
#define	Window				osWindow
#define	XColor				osColor
#define	XPoint				osPoint
#define	XRectangle			osRect
#define	XWindowAttributes	osWindowAttributes


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

#endif	/*OS*/

