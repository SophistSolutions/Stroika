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
            // Examples from https://tools.ietf.org/html/rfc6265#section-3.1
            {
                Cookie c = Cookie::Decode (L"SID=31d4d96e407aad42");
                VerifyTestResult (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Decode (L"SID=31d4d96e407aad42; Path=/; Secure; HttpOnly");
                VerifyTestResult (c.fKey == L"SID" and c.fValue == L"31d4d96e407aad42");
                VerifyTestResult (c.fPath == L"/");
                VerifyTestResult (c.fSecure);
                VerifyTestResult (c.fHttpOnly);
                c.fPath.reset ();
                c.fSecure   = false;
                c.fHttpOnly = false;
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Decode (L"lang=en-US; Path=/; Domain=example.com");
                VerifyTestResult (c.fKey == L"lang" and c.fValue == L"en-US");
                VerifyTestResult (c.fPath == L"/");
                VerifyTestResult (c.fDomain == L"example.com");
                c.fPath.reset ();
                c.fDomain.reset ();
                VerifyTestResult (c.GetAttributes ().empty ());
            }
            {
                Cookie c = Cookie::Decode (L"lang=en-US; Expires=Wed, 09 Jun 2021 10:18:14 GMT");
                VerifyTestResult (c.fKey == L"lang" and c.fValue == L"en-US");
                using Time::Date;
                using Time::DateTime;
                using Time::DayOfMonth;
                using Time::MonthOfYear;
                using Time::TimeOfDay;
                //@todo re-enable this test once we fix Cookie date parsing
                //VerifyTestResult (c.fExpires == (DateTime{Date{Time::Year{2021}, MonthOfYear::eJune, DayOfMonth{9}}, TimeOfDay{10, 18, 14}}));
                c.fExpires.reset ();
                VerifyTestResult (c.GetAttributes ().empty ());
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
