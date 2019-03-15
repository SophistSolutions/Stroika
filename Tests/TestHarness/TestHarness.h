/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_TestHarness_h_
#define _Stroika_TestHarness_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika::TestHarness {

    void Setup ();

    // print succeeded if it completes, and failed if exception caught
    int PrintPassOrFail (void (*regressionTest) ());

    // LIKE calling Assert but it will ALSO trigger a failure in NODEBUG builds
    void Test_ (bool failIfFalse, bool isFailureElseWarning, const char* regressionTestText, const char* fileName, int lineNum);

#define VerifyTestResult(c) Stroika::TestHarness::Test_ (!!(c), true, #c, __FILE__, __LINE__)
#define VerifyTestResultWarning(c) Stroika::TestHarness::Test_ (!!(c), false, #c, __FILE__, __LINE__)

    void WarnTestIssue (const char* issue);

}
#endif /*_Stroika_TestHarness_h_*/
