/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */

 #include	"Stroika/Foundation/StroikaPreComp.h"


#include	"Stroika/Foundation/Containers/Common.h"
#include	"Stroika/Foundation/Debug/Assertions.h"
#include	"Stroika/Foundation/Debug/Debugger.h"

#include	"SimpleClass.h"
#include    "TestHarness.h"



namespace	Stroika {



/*
 ********************************************************************************
 ********************************** SimpleClass *********************************
 ********************************************************************************
 */
static	const	int kFunnyValue = 1234;

size_t	SimpleClass::sTotalLiveObjects	=	0;

#if qIteratorsRequireNoArgContructorForT
     SimpleClass::SimpleClass () :
        fValue (3),
        fConstructed (kFunnyValue)
    {
        sTotalLiveObjects++;
    }
#endif

SimpleClass::SimpleClass (size_t v) :
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
	VerifyTestResult (f.fConstructed == kFunnyValue);
}

SimpleClass::~SimpleClass ()
{
	VerifyTestResult (fConstructed == kFunnyValue);
	VerifyTestResult (sTotalLiveObjects != 0);
	sTotalLiveObjects--;
	fConstructed = 0;
	VerifyTestResult (fConstructed != kFunnyValue);
}

size_t	SimpleClass::GetValue () const
{
	VerifyTestResult (fConstructed == kFunnyValue);
	return (fValue);
}

size_t	SimpleClass::GetTotalLiveCount ()
{
	return (sTotalLiveObjects);
}


/*
 ********************************************************************************
 ******************************* comparison operators  **************************
 ********************************************************************************
 */
bool	operator== (const SimpleClass& lhs, const SimpleClass& rhs)
{
	VerifyTestResult (lhs.fConstructed == kFunnyValue);
	VerifyTestResult (rhs.fConstructed == kFunnyValue);
	return (bool (lhs.fValue == rhs.fValue));
}

bool	operator< (const SimpleClass& lhs, const SimpleClass& rhs)
{
	VerifyTestResult (lhs.fConstructed == kFunnyValue);
	VerifyTestResult (rhs.fConstructed == kFunnyValue);
	return (bool (lhs.fValue < rhs.fValue));
}



}
