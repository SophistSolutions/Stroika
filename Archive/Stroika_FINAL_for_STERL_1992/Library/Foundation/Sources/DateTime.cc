/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/DateTime.cc,v 1.3 1992/09/01 15:26:25 sterling Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: DateTime.cc,v $
 *		Revision 1.3  1992/09/01  15:26:25  sterling
 *		Lots.
 *
 *		Revision 1.2  1992/07/16  06:02:12  lewis
 *		Quickie implementation of a few methods needed compiling under BorlandCPlus.
 *
 *		Revision 1.1  1992/06/19  22:29:57  lewis
 *		Initial revision
 *
 */



#include	"Debug.hh"
#include	"StreamUtils.hh"

#include	"DateTime.hh"






/*
 ********************************************************************************
 ********************************** DateTime ************************************
 ********************************************************************************
 */

DateTime::DateTime ():
	fDate (0),
	fTime (0)
{
}

DateTime::DateTime (const Date& date):
	fDate (date),
	fTime (0)
{
}

DateTime::DateTime (const Time& time):
	fDate (0),
	fTime (time)
{
}

DateTime::DateTime (const Date& date, const Time& time):
	fDate (date),
	fTime (time)
{
}



/*
 ********************************************************************************
 ********************************** Comparisons *********************************
 ********************************************************************************
 */

Boolean	operator== (const DateTime& lhs, const DateTime& rhs)
{
	return Boolean (lhs.GetDate () == rhs.GetDate () and lhs.GetTime () == rhs.GetTime ());
}

Boolean	operator!= (const DateTime& lhs, const DateTime& rhs)
{
	return Boolean (lhs.GetDate () != rhs.GetDate () or lhs.GetTime () != rhs.GetTime ());
}


// For gnuemacs:
// Local Variables: ***
// mode:C ***
// tab-width:4 ***
// End: ***

