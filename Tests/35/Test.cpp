﻿/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
//  TEST    Foundation::Execution::Exceptions
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#if qPlatform_Windows
#include <Windows.h>
#include <winerror.h>
#include <wininet.h> // for error codes
#endif

#include "Stroika/Foundation/Characters/ToString.h"

#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

namespace {
    void RegressionTest1_ ()
    {
#if qPlatform_Windows
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_TIMEOUT == ERROR_INTERNET_TIMEOUT);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_INVALID_URL == ERROR_INTERNET_INVALID_URL);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_UNRECOGNIZED_SCHEME == ERROR_INTERNET_UNRECOGNIZED_SCHEME);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_NAME_NOT_RESOLVED == ERROR_INTERNET_NAME_NOT_RESOLVED);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_PROTOCOL_NOT_FOUND == ERROR_INTERNET_PROTOCOL_NOT_FOUND);
        VerifyTestResult (Platform::Windows::Exception::kERROR_INTERNET_CANNOT_CONNECT == ERROR_INTERNET_CANNOT_CONNECT);
#endif
    }
}

namespace {
    void Test2_ThrowCatchStringException_ ()
    {
        {
            try {
                Throw (Exception (L"HiMom"));
                VerifyTestResult (false);
            }
            catch (const Exception<>& e) {
                VerifyTestResult (e.As<wstring> () == L"HiMom");
            }
        }
        {
            try {
                Throw (Exception (L"HiMom"));
                VerifyTestResult (false);
            }
            catch (const std::exception& e) {
                VerifyTestResult (strcmp (e.what (), "HiMom") == 0);
            }
        }
    }
}

namespace {
    namespace Test3_SystemException_ {
        namespace Private_ {
            void T1_system_error_ ()
            {
                static const int                kErr2TestFor_ = make_error_code (errc::bad_address).value (); // any value from errc would do
                Characters::String              msg1;
                static const Characters::String kErr2TestForExpectedMsg_ = L"bad address {errno: 14}"sv; // maybe not always right due to locales?

                // One way
                try {
                    SystemException::ThrowPOSIXErrNo (kErr2TestFor_);
                }
                catch (const Execution::SystemException& e) {
                    VerifyTestResult (e.code ().value () == kErr2TestFor_);
                    VerifyTestResult (e.code ().category () == system_category () or e.code ().category () == generic_category ());
                    msg1 = Characters::ToString (e);
                    VerifyTestResult (msg1 == kErr2TestForExpectedMsg_);
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    VerifyTestResult (false); //oops
                }
                // But this works too
                try {
                    SystemException::ThrowPOSIXErrNo (kErr2TestFor_);
                }
                catch (const std::system_error& e) {
                    VerifyTestResult (e.code ().value () == kErr2TestFor_);
                    VerifyTestResult (e.code ().category () == system_category () or e.code ().category () == generic_category ());
                    VerifyTestResult (msg1 == Characters::ToString (e));
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    VerifyTestResult (false); //oops
                }
                // and test throwing fancy unicode string

                const Characters::String kMsgWithUnicode_ = L"zß水𝄋"; // this works even if using a code page / locale which doesn't support UNICODE/Chinese
                try {
                    Execution::Throw (SystemException (kErr2TestFor_, generic_category (), kMsgWithUnicode_));
                }
                catch (const std::system_error& e) {
                    VerifyTestResult (e.code ().value () == kErr2TestFor_);
                    VerifyTestResult (e.code ().category () == generic_category ());
                    VerifyTestResult (Characters::ToString (e).Contains (kMsgWithUnicode_));
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    VerifyTestResult (false); //oops
                }
            }
            void T2_TestTimeout_ ()
            {
                try {
                    Execution::Throw (Execution::TimeOutException{});
                }
                catch (const system_error& e) {
                    VerifyTestResult (e.code () == errc::timed_out);
                    VerifyTestResult (e.code () != errc::already_connected);
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    VerifyTestResult (false); //oops
                }
                try {
                    Execution::Throw (Execution::TimeOutException{});
                }
                catch (const Execution::TimeOutException& e) {
                    VerifyTestResult (e.code () == errc::timed_out);
                    VerifyTestResult (e.code () != errc::already_connected);
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    VerifyTestResult (false); //oops
                }
                const Characters::String kMsg1_ = L"to abcd 123 zß水𝄋";
                try {
                    Execution::Throw (Execution::TimeOutException{kMsg1_});
                }
                catch (const system_error& e) {
                    VerifyTestResult (e.code () == errc::timed_out);
                    VerifyTestResult (e.code () != errc::already_connected);
                    VerifyTestResult (Characters::ToString (e).Contains (kMsg1_));
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    VerifyTestResult (false); //oops
                }
            }
        }
        void TestAll_ ()
        {
            Private_::T1_system_error_ ();
            Private_::T2_TestTimeout_ ();
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        RegressionTest1_ ();
        Test2_ThrowCatchStringException_ ();
        Test3_SystemException_::TestAll_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
