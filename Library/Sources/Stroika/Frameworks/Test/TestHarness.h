/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_TestHarness_h_
#define _Stroika_TestHarness_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <string>
#include <vector>

#if qHasFeature_GoogleTest
#define GTEST_DONT_DEFINE_TEST 1
#include <gtest/gtest.h>
#endif

#include "Stroika/Foundation/Configuration/Common.h"

namespace Stroika::Frameworks::Test {

    /**
     *  \par Example Usage
     *      \code
     *          int main (int argc, const char* argv[])
     *          {
     *              Test::Setup (argc, argv);
     *              return Test::PrintPassOrFail (DoRegressionTests_);
     *          }
     *      \endcode
     * 
     *  if qHasFeature_GoogleTest is enabled, this calls testing::InitGoogleTest(), which may remove some values from argc/argv.
     *  Use the value returned from Setup() to get the adjusted args list. Note - no characterset mapping is done in this routine. Just copying of the same
     *  starting raw data.
     */
    vector<string> Setup (int argc, const char* argv[]);

    /**
     * print succeeded if it completes, and failed if exception caught
     *     \see example usage in Setup()
     */
    [[deprecated ("Since Strokka v3.0d5 - just use gtest")]] int PrintPassOrFail (void (*regressionTest) ());

    /**
     * LIKE calling Assert but it will ALSO trigger a failure in NODEBUG builds
     */
    void Test_ (bool failIfFalse, bool isFailureElseWarning, const char* regressionTestText, const char* fileName, int lineNum);
    void VerifyTestResultWarning_ (bool failIfFalse, bool isFailureElseWarning, const char* regressionTestText, const char* fileName, int lineNum);

    /**
     */
#define VerifyTestResult(c) Stroika::Frameworks::Test::Test_ (!!(c), true, #c, __FILE__, __LINE__)

    /**
     */
#define VerifyTestResultWarning(c) Stroika::Frameworks::Test::VerifyTestResultWarning_ (!!(c), false, #c, __FILE__, __LINE__)

    /**
     */
    void WarnTestIssue (const char* issue);

    /**
     */
    void WarnTestIssue (const wchar_t* issue);

}
#endif /*_Stroika_TestHarness_h_*/
