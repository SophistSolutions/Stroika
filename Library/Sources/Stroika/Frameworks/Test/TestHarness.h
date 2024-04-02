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
     *  Use the value returned from Setup() to get the adjusted args list. Note - no character-set mapping is done in this routine. Just copying of the same
     *  starting raw data.
     */
    vector<string> Setup (int argc, const char* argv[]);

    /**
     * print succeeded if it completes, and failed if exception caught
     *     \see example usage in Setup()
     */
    [[deprecated ("Since Strokka v3.0d5 - just use gtest")]] int PrintPassOrFail (void (*regressionTest) ());

    namespace Private_ {
        /**
         * LIKE calling Assert but it will ALSO trigger a failure in NODEBUG builds
         */
        void Test_ (bool failIfFalse, bool isFailureElseWarning, const wchar_t* regressionTestText, const wchar_t* fileName, int lineNum);
        void VerifyTestResultWarning_ (bool failIfFalse, bool isFailureElseWarning, const wchar_t* regressionTestText,
                                       const wchar_t* fileName, int lineNum);
    }

    /**
     */
#define VerifyTestResult(c)                                                                                                                \
    Stroika::Frameworks::Test::Private_::Test_ (!!(c), true, Stroika_Foundation_Debug_Widen (#c), Stroika_Foundation_Debug_Widen (__FILE__), __LINE__)

    /**
     */
#define VerifyTestResultWarning(c)                                                                                                         \
    Stroika::Frameworks::Test::Private_::VerifyTestResultWarning_ (!!(c), false, Stroika_Foundation_Debug_Widen (#c),                      \
                                                                   Stroika_Foundation_Debug_Widen (__FILE__), __LINE__)

    /**
     *  Unlike most Stroika APIs, here 'char' refers to 'narrow SDK-char' codepage.
     */
    void WarnTestIssue (const char* issue);
    void WarnTestIssue (const wchar_t* issue);

}
#endif /*_Stroika_TestHarness_h_*/
