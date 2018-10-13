/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
//  TEST    Foundation::IO::Network::HTTP
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

#include "Stroika/Foundation/IO/Network/HTTP/Cookies.h"

#include "../TestHarness/SimpleClass.h"
#include "../TestHarness/TestHarness.h"

using namespace Stroika;
using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::IO::Network;
using namespace Stroika::Foundation::IO::Network::HTTP;

namespace {
    namespace Cookies_Test01_ {
        void RunAll ()
        {
            if (false) {
                // Examples from https://tools.ietf.org/html/rfc6265#section-3.1
                {
                    Cookie c = Cookie::Decode (L"SID=31d4d96e407aad42");
                    VerifyTestResult (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                }
                {
                    Cookie c = Cookie::Decode (L"SID=31d4d96e407aad42; Path=/; Secure; HttpOnly");
                    VerifyTestResult (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                    VerifyTestResult (c.fPath == L"/");
                }
            }
        }
    }
}

namespace {
    void DoRegressionTests_ ()
    {
        Cookies_Test01_::RunAll ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
