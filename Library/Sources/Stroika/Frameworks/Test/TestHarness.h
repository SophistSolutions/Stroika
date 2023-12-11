/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_TestHarness_h_
#define _Stroika_TestHarness_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

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
     */
    void Setup (int argc, const char* argv[]);

    /**
     * print succeeded if it completes, and failed if exception caught
     *     \see example usage in Setup()
     */
    int PrintPassOrFail (void (*regressionTest) ());

    /**
     * LIKE calling Assert but it will ALSO trigger a failure in NODEBUG builds
     */
    void Test_ (bool failIfFalse, bool isFailureElseWarning, const char* regressionTestText, const char* fileName, int lineNum);

    /**
     */
#define VerifyTestResult(c) Stroika::Frameworks::Test::Test_ (!!(c), true, #c, __FILE__, __LINE__)

    /**
     */
#define VerifyTestResultWarning(c) Stroika::Frameworks::Test::Test_ (!!(c), false, #c, __FILE__, __LINE__)

    /**
     */
    void WarnTestIssue (const char* issue);

    /**
     */
    void WarnTestIssue (const wchar_t* issue);

}
#endif /*_Stroika_TestHarness_h_*/
