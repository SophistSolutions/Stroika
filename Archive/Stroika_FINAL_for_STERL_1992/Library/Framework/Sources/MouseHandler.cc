/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/MouseHandler.cc,v 1.3 1992/09/01 15:46:50 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: MouseHandler.cc,v $
 *		Revision 1.3  1992/09/01  15:46:50  sterling
 *		Lots of Foundation changes.
 *
 *		Revision 1.8  1992/01/15  10:01:09  lewis
 *		Got rid of methods to convert to / from global coordinates - that functionality is now deprecated,
 *		and supplied - if needed - from Enclosure.
 *
 *		Revision 1.7  1992/01/14  06:02:31  lewis
 *		Take into account fact that TabletOwner stuff broken out of panel, and MouseH
 *		andlker is no longer a tablet owner.
 *
 *		Revision 1.5  1992/01/03  18:10:21  lewis
 *		Use Toolkit rather than MacOS tags for ifdefs, and make XtToolkit behave like mac
 *		for Doubleclick/ShiftKey interpretation (for now - fix later).
 *
 *
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DeskTop.hh"

#include	"MouseHandler.hh"






/*
 ********************************************************************************
 *********************************** MousePressInfo *****************************
 ********************************************************************************
 */

MousePressInfo::MousePressInfo ():
	fPressAt (kZeroPoint)
#if		qMacToolkit || qXtToolkit
	,
	fClickCount (0),
	fKeyBoard ()
#endif	/*Toolkit*/
{
}

MousePressInfo::MousePressInfo (const MousePressInfo& from):
	fPressAt (from.fPressAt)
#if		qMacToolkit || qXtToolkit
	,
	fClickCount (from.fClickCount),
	fKeyBoard (from.fKeyBoard)
#endif	/*Toolkit*/
{
}

MousePressInfo::MousePressInfo (const MousePressInfo& from, const Point& newPressAt):
	fPressAt (newPressAt)
#if		qMacToolkit || qXtToolkit
	,
	fClickCount (from.fClickCount),
	fKeyBoard (from.fKeyBoard)
#endif	/*Toolkit*/
{
}

MousePressInfo::MousePressInfo (const Point& newPressAt):
	fPressAt (newPressAt)
#if		qMacToolkit || qXtToolkit
	,
	fClickCount (1),
	fKeyBoard ()
#endif	/*Toolkit*/
{
}

#if		qMacToolkit || qXtToolkit
MousePressInfo::MousePressInfo (const Point& pressAt, UInt16 clickCount, const KeyBoard& keyBoard):
	fPressAt (pressAt),
	fClickCount (clickCount),
	fKeyBoard (keyBoard)
{
}
#endif	/*Toolkit*/


const MousePressInfo& MousePressInfo::operator= (const MousePressInfo& from)
{
	fPressAt = from.fPressAt;
#if		qMacToolkit || qXtToolkit
	fClickCount = from.fClickCount;
	fKeyBoard = from.fKeyBoard;
#else
	AssertNotImplemented ();
#endif	/*Toolkit*/
	return (*this);
}

Boolean	MousePressInfo::IsSimpleSelection () const
{
#if		qMacToolkit || qXtToolkit
	// maybe need to check other keys?
	return Boolean ((fClickCount == 1) and (not fKeyBoard.GetKey (KeyBoard::eShiftKey)));
#else
	AssertNotImplemented ();
#endif	/*Toolkit*/
}

Boolean	MousePressInfo::IsExtendedSelection () const
{
#if		qMacToolkit || qXtToolkit
	// maybe need to check other keys?
	return Boolean ((fClickCount == 1) and (fKeyBoard.GetKey (KeyBoard::eShiftKey)));
#else
	AssertNotReached ();
#endif	/*Toolkit*/
}

Boolean	MousePressInfo::IsOpenSelection () const
{
#if		qMacToolkit || qXtToolkit
	// maybe need to check other keys?
	return Boolean ((fClickCount == 2) and (not fKeyBoard.GetKey (KeyBoard::eShiftKey)));
#else
	AssertNotReached ();
#endif	/*Toolkit*/
}





/*
 ********************************************************************************
 ************************************* MouseHandler *****************************
 ********************************************************************************
 */

MouseHandler::MouseHandler ()
{
}



// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***


