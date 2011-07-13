/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/SimpleClass.cc,v 1.3 1992/11/26 02:54:03 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: SimpleClass.cc,v $
 *		Revision 1.3  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.2  1992/10/22  18:18:20  lewis
 *		Added sTotalLiveObjects static variable.
 *
 *		Revision 1.5  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *
 */


#include	"Debug.hh"

#include	"SimpleClass.hh"
#include	"TestSuite.hh"


/*
 ********************************************************************************
 ********************************** SimpleClass *********************************
 ********************************************************************************
 */
static	const	int kFunnyValue = 1234;

UInt32	SimpleClass::sTotalLiveObjects	=	0;

SimpleClass::SimpleClass (UInt32 v) :
	fValue (v),
	fConstructed (kFunnyValue)
{
	sTotalLiveObjects++;
}

SimpleClass::SimpleClass (const SimpleClass& f) :
	fValue (f.fValue),
	fConstructed (kFunnyValue)
{
	sTotalLiveObjects++;
	TestCondition (f.fConstructed == kFunnyValue);
}

SimpleClass::~SimpleClass ()
{
	TestCondition (fConstructed == kFunnyValue);
	TestCondition (sTotalLiveObjects != 0);
	sTotalLiveObjects--;
	fConstructed = 0;
	TestCondition (fConstructed != kFunnyValue);
}
		
UInt32	SimpleClass::GetValue () const
{
	TestCondition (fConstructed == kFunnyValue);
	return (fValue);
}

UInt32	SimpleClass::GetTotalLiveCount ()
{
	return (sTotalLiveObjects);
}



/*
 ********************************************************************************
 ******************************* comparison operators  **************************
 ********************************************************************************
 */
Boolean	operator== (const SimpleClass& lhs, const SimpleClass& rhs)
{
	TestCondition (lhs.fConstructed == kFunnyValue);
	TestCondition (rhs.fConstructed == kFunnyValue);
	return (Boolean (lhs.fValue == rhs.fValue));
}

Boolean	operator< (const SimpleClass& lhs, const SimpleClass& rhs)
{
	TestCondition (lhs.fConstructed == kFunnyValue);
	TestCondition (rhs.fConstructed == kFunnyValue);
	return (Boolean (lhs.fValue < rhs.fValue));
}


// For gnuemacs:
// Local Variables: ***
// mode:C++ ***
// tab-width:4 ***
// End: ***

