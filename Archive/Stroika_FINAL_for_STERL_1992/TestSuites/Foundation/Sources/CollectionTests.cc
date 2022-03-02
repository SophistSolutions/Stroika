#error "FILE OBSOLETE"

/* Copyright(c) Sophist Solutions Inc. 1990-1992.  All rights reserved */

/*
 * $Header: /fuji/lewis/RCS/CollectionTests.cc,v 1.6 1992/11/26 02:54:03 lewis Exp $
 *
 * TODO:
 *
 * Changes:
 *	$Log: CollectionTests.cc,v $
 *		Revision 1.6  1992/11/26  02:54:03  lewis
 *		Minor cleanups for templates, etc...
 *
 *		Revision 1.5  1992/09/29  15:54:35  sterling
 *		Get rid of use of String - use new "SimpleClass" instead.
 *
 *		Revision 1.4  1992/09/11  19:33:08  sterling
 *		hi
 *
 *		Revision 1.2  1992/09/01  17:52:15  sterling
 *		*** empty log message ***
 *
// Revision 1.1  1992/07/21  18:47:44  sterling
// Initial revision
//
 *		
 *
 *
 */


#include	"Debug.hh"
#include	"StreamUtils.hh"
#include	"Time.hh"

#include	"TestSuite.hh"

#include	"CollectionTests.hh"


#define		qPrintTimings		!qDebug

#if		!qRealTemplatesAvailable
	Implement (Iterator, SimpleClass);
	Implement (Iterator, UInt32);
#endif

static	const	int kFunnyValue = 1234;
SimpleClass::SimpleClass (UInt32 v) :
	fValue (v),
	fConstructed (kFunnyValue)
{
}

SimpleClass::SimpleClass (const SimpleClass& f) :
	fValue (f.fValue),
	fConstructed (kFunnyValue)
{
	TestCondition (f.fConstructed == kFunnyValue);
}

SimpleClass::~SimpleClass ()
{
	TestCondition (fConstructed == kFunnyValue);
	fConstructed = 0;
	TestCondition (fConstructed != kFunnyValue);
}
		
UInt32	SimpleClass::GetValue () const
{
	TestCondition (fConstructed == kFunnyValue);
	return (fValue);
}

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

