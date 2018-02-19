/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Debugger.h"

#include "SimpleClass.h"
#include "TestHarness.h"

using namespace Stroika;

/*
 ********************************************************************************
 ********************************** SimpleClass *********************************
 ********************************************************************************
 */
static const int kFunnyValue = 1234;

size_t SimpleClass::sTotalLiveObjects = 0;

SimpleClass::SimpleClass (size_t v)
    : fValue (v)
    , fConstructed (kFunnyValue)
{
    sTotalLiveObjects++;
}

SimpleClass::SimpleClass (const SimpleClass& f)
    : fValue (f.fValue)
    , fConstructed (kFunnyValue)
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

size_t SimpleClass::GetValue () const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    return (fValue);
}

size_t SimpleClass::GetTotalLiveCount ()
{
    return (sTotalLiveObjects);
}

bool SimpleClass::operator== (const SimpleClass& rhs) const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    VerifyTestResult (rhs.fConstructed == kFunnyValue);
    return (bool(fValue == rhs.fValue));
}

bool SimpleClass::operator< (const SimpleClass& rhs) const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    VerifyTestResult (rhs.fConstructed == kFunnyValue);
    return (bool(fValue < rhs.fValue));
}

/*
 ********************************************************************************
 ********************************** SimpleClassWithoutComparisonOperators *********************************
 ********************************************************************************
 */
size_t SimpleClassWithoutComparisonOperators::sTotalLiveObjects = 0;

SimpleClassWithoutComparisonOperators::SimpleClassWithoutComparisonOperators (size_t v)
    : fValue (v)
    , fConstructed (kFunnyValue)
{
    sTotalLiveObjects++;
}

SimpleClassWithoutComparisonOperators::SimpleClassWithoutComparisonOperators (const SimpleClassWithoutComparisonOperators& f)
    : fValue (f.fValue)
    , fConstructed (kFunnyValue)
{
    sTotalLiveObjects++;
    VerifyTestResult (f.fConstructed == kFunnyValue);
}

SimpleClassWithoutComparisonOperators::~SimpleClassWithoutComparisonOperators ()
{
    VerifyTestResult (fConstructed == kFunnyValue);
    VerifyTestResult (sTotalLiveObjects != 0);
    sTotalLiveObjects--;
    fConstructed = 0;
    VerifyTestResult (fConstructed != kFunnyValue);
}

size_t SimpleClassWithoutComparisonOperators::GetValue () const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    return (fValue);
}

size_t SimpleClassWithoutComparisonOperators::GetTotalLiveCount ()
{
    return (sTotalLiveObjects);
}
