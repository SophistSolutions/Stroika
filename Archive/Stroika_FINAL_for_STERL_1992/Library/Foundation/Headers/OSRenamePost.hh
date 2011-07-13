/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/OSRenamePost.hh,v 1.2 1992/09/01 15:20:18 sterling Exp $
 *
 * Description:
 *
 * TODO:
 *
 * Notes:
 *		We cannot wrap this file in #ifndefs - see comment in OSRenamePre.hh
 *
 *
 * Changes:
 *	$Log: OSRenamePost.hh,v $
 *		Revision 1.2  1992/09/01  15:20:18  sterling
 *		*** empty log message ***
 *
 *		Revision 1.17  92/04/27  18:34:23  18:34:23  lewis (Lewis Pringle)
 *		Added rename for struct Depth for XGDI
 *		
 *		Revision 1.16  92/04/16  10:51:56  10:51:56  lewis (Lewis Pringle)
 *		Added osRandom() for mac.
 *		
 *		Revision 1.14  92/04/02  11:04:25  11:04:25  lewis (Lewis Pringle)
 *		Added reanme for XGDI for Dimension and Position.
 *		
 *		Revision 1.13  92/04/02  11:02:11  11:02:11  lewis (Lewis Pringle)
 *		Get rid of #ifndef wrapping file.
 *		
 *
 *
 */


#if		qMacOS

// From <Controls.h>
#undef		ControlRecord


// From <Dialogs.h>
#undef		Alert
#undef		CautionAlert
#undef		NoteAlert
#undef		StopAlert


// From <Events.h>
#undef		Button
#undef		EventRecord


// From <File.h>
#undef		FSSpec
#undef		VersRec


// From <Memory.h>
#undef		Size


// From <Menu.h>
#undef		EnableItem
#undef		MenuInfo
#undef		MenuPtr
#undef		SystemMenu


// From <Print.h>
#undef		TPrint
#undef		TPrPort

// From <Quickdraw.h>
#undef		BitMap
#undef		ColorSpec
#undef		ColorTable
#undef		Cursor
#undef		Region
#undef		GrafPort
#undef		CGrafPort
#undef		FontInfo
#undef		GDevice
#undef		Line
#undef		Pattern
#undef		PixMap
#undef		PixPat
#undef		Polygon
#undef		Picture
#undef		Random
#undef		SetCursor


// From <Sound.h>
#undef		Time


// From <TextEdit.h>
#undef		TextBox
#undef		TextStyle

// From <ToolUtils.h>
#undef		SetString

// From <Types.h>
#undef		Boolean
#undef		Handle
#undef		Point
#undef		ProcHandle
#undef		ProcPtr
#undef		Ptr
#undef		Rect
#undef		Str255
#undef		Str63
#undef		Str31
#undef		StringPtr
#undef		StringHandle
#undef		Style


// From <Window.h>
#undef		WindowPeek
#undef		WindowPtr

#elif	qXGDI

#undef	_WidgetClassRec
#undef	_WidgetRec
#undef	_XDisplay
#undef	_XEvent
#undef	_XImage
#undef	_XRegion
#undef	Boolean
#undef	Colormap
#undef	Cursor
#undef	Depth
#undef	Dimension
#undef	Font
#undef	KeyCode
#undef	Picture
#undef	PixMap
#undef	Position
#undef	Region
#undef	String
#undef	Time
#undef	Visual
#undef	Widget
#undef	Window
#undef	XColor
#undef	XPoint
#undef	XRectangle
#undef	XWindowAttributes




// Undo some unfreindly macros setup by motif (what file?)
#undef	True
#undef	False


#endif	/*OS*/


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***




