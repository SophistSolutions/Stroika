#include    <iostream>
#include    "Stroika/Foundation/Characters/CodePage.h"
namespace   {
    void    _ASSERT_HANDLER_()
    {
        std::cerr << "FAILED: ";
    }
}

int main() { }


/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */

#include   "Stroika/Foundation/StroikaPreComp.h"


#include    "Stroika/Foundation/Containers/Common.h"
#include    "Stroika/Foundation/Debug/Assertions.h"

#include    "../TestHarness/SimpleClass.h"
#include    "../TestHarness/TestHarness.h"



using   namespace   Stroika;



/*
 ********************************************************************************
 ********************************** SimpleClass *********************************
 ********************************************************************************
 */
static  const   int kFunnyValue = 1234;

size_t  SimpleClass::sTotalLiveObjects  =   0;

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

size_t  SimpleClass::GetValue () const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    return (fValue);
}

size_t  SimpleClass::GetTotalLiveCount ()
{
    return (sTotalLiveObjects);
}

bool    SimpleClass::operator== (const SimpleClass& rhs) const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    VerifyTestResult (rhs.fConstructed == kFunnyValue);
    return (bool (fValue == rhs.fValue));
}

bool    SimpleClass::operator< (const SimpleClass& rhs) const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    VerifyTestResult (rhs.fConstructed == kFunnyValue);
    return (bool (fValue < rhs.fValue));
}







/*
 ********************************************************************************
 ********************************** SimpleClassWithoutComparisonOperators *********************************
 ********************************************************************************
 */
size_t  SimpleClassWithoutComparisonOperators::sTotalLiveObjects  =   0;

SimpleClassWithoutComparisonOperators::SimpleClassWithoutComparisonOperators (size_t v) :
    fValue (v),
    fConstructed (kFunnyValue)
{
    sTotalLiveObjects++;
}

SimpleClassWithoutComparisonOperators::SimpleClassWithoutComparisonOperators (const SimpleClassWithoutComparisonOperators& f) :
    fValue (f.fValue),
    fConstructed (kFunnyValue)
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

size_t  SimpleClassWithoutComparisonOperators::GetValue () const
{
    VerifyTestResult (fConstructed == kFunnyValue);
    return (fValue);
}

size_t  SimpleClassWithoutComparisonOperators::GetTotalLiveCount ()
{
    return (sTotalLiveObjects);
}

/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#include    "Stroika/Foundation/StroikaPreComp.h"

#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
#include    <cstdio>
#else
#include    <iostream>
#endif

#include    "Stroika/Foundation/Characters/CodePage.h"
#include    "Stroika/Foundation/Characters/ToString.h"
#include    "Stroika/Foundation/Containers/Common.h"
#include    "Stroika/Foundation/Debug/Assertions.h"
#include    "Stroika/Foundation/Execution/StringException.h"

#include    "../TestHarness/TestHarness.h"


using   namespace   Stroika;
using   namespace   Stroika::Foundation;
using   namespace   Stroika::TestHarness;



namespace   {
    void    _ASSERT_HANDLER_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
    {
        if (assertCategory == nullptr) {
            assertCategory = "Unknown assertion";
        }
        if (assertionText == nullptr) {
            assertionText = "";
        }
        if (functionName == nullptr) {
            functionName = "";
        }
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "FAILED: %s; %s; %s; %s:%d\n", assertCategory, assertionText, functionName, fileName, lineNum);
#else
        cerr << "FAILED: " << assertCategory << "; " << assertionText << ";" << functionName << ";" << fileName << ": " << lineNum << endl;
#endif
        DbgTrace ("FAILED: %s; %s; %s; %s; %d", assertCategory, assertionText, functionName, fileName, lineNum);


        std::_Exit (EXIT_FAILURE);  // skip
    }
    void    _FatalErrorHandler_ (const Characters::SDKChar* msg)
    {
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
#if     qTargetPlatformSDKUseswchar_t
        fprintf(stderr, "FAILED: %s\n",  Characters::WideStringToNarrowSDKString (msg).c_str ());
#else
        fprintf(stderr, "FAILED: %s\n",  msg);
#endif
#else
#if     qTargetPlatformSDKUseswchar_t
        cerr << "FAILED: " <<  Characters::WideStringToNarrowSDKString (msg) << endl;
#else
        cerr << "FAILED: " <<  msg << endl;
#endif
#endif
        std::_Exit (EXIT_FAILURE);  // skip
    }
}




void    TestHarness::Setup ()
{
#if     qDebug
    Stroika::Foundation::Debug::SetAssertionHandler (_ASSERT_HANDLER_);
#endif
    using   namespace   Execution;
}


void    TestHarness::PrintPassOrFail (void (*regressionTest) ())
{
    try {
        (*regressionTest) ();
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::printf ("Succeeded\n");
#else
        cout << "Succeeded" << endl;
#endif
        DbgTrace (L"Succeeded");
    }
    catch (...) {
        auto exc = current_exception ();
#if     qCompilerAndStdLib_COutCErrStartupCrasher_Buggy
        (void)::fprintf (stderr, "FAILED: REGRESSION TEST DUE TO EXCEPTION: '%s\n", Characters::ToString (exc).AsNarrowSDKString ().c_str ());
        (void)::printf ("Failed\n");
#else
        cerr << "FAILED: REGRESSION TEST DUE TO EXCEPTION: '" << Characters::ToString (exc).AsNarrowSDKString () << endl;
        cout << "Failed" << endl;
#endif
        DbgTrace (L"FAILED: REGRESSION TEST (Execution::StringException): '%s", Characters::ToString (exc).c_str ());
        exit (EXIT_FAILURE);
    }
}




void    TestHarness::Test_ (bool failIfFalse, const char* regressionTestText, const char* fileName, int lineNum)
{
    if (not failIfFalse) {
        _ASSERT_HANDLER_ ("RegressionTestFailure", regressionTestText, fileName, lineNum, "");
    }
}
