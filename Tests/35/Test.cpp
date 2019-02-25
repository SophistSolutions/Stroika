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
    namespace Test3_SystemErrorException_ {
        namespace Private_ {
            void T1_system_error_ ()
            {
                static const int                kErr2TestFor_            = make_error_code (errc::bad_address).value (); // any value from errc would do
                static const Characters::String kErr2TestForExpectedMsg_ = L"bad address {errno: 14}"sv;                 // maybe not always right due to locales?

                try {
                    ThrowPOSIXErrNo (kErr2TestFor_);
                }
                catch (const std::system_error& e) {
                    VerifyTestResult (e.code ().value () == kErr2TestFor_);
                    VerifyTestResult (e.code ().category () == system_category () or e.code ().category () == generic_category ());
                    VerifyTestResult (Characters::ToString (e).Contains (kErr2TestForExpectedMsg_, Characters::CompareOptions::eCaseInsensitive));
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    VerifyTestResult (false); //oops
                }
                // and test throwing fancy unicode string

                const Characters::String kMsgWithUnicode_ = L"zß水𝄋"; // this works even if using a code page / locale which doesn't support UNICODE/Chinese
                try {
                    Execution::Throw (SystemErrorException (kErr2TestFor_, generic_category (), kMsgWithUnicode_));
                }
                catch (const std::system_error& e) {
                    VerifyTestResult (e.code ().value () == kErr2TestFor_);
                    VerifyTestResult (e.code ().category () == generic_category ());
                    VerifyTestResult (Characters::ToString (e).Contains (kMsgWithUnicode_, Characters::CompareOptions::eCaseInsensitive));
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
    namespace Test4_Activities_ {
        namespace Private {

            void T1_Basics_ ()
            {
                using Characters::String;
                String argument;

                static constexpr Activity<wstring_view> kBuildingThingy_{L"Building thingy"sv};

                // constexpr only works if we lose the virtual in ~AsStringObj_ ()
                static constexpr const auto kA1_{Activity<wstring_view>{L"a1"sv}};

                static const auto kOtherActivity = Activity<String>{L"kOtherActivity"};

                // automatic variable activity OK as long as it's lifetime longer than reference in DeclareActivity
                auto otherActivity = Activity<String>{L"otherActivity" + argument}; // activities can be stack based, but these cost more to define

                auto lazyEvalActivity = LazyEvalActivity ([&]() -> String { return argument.Repeat (5) + L"xxx"; });

                DeclareActivity active1{&kA1_};
                DeclareActivity active2{&kOtherActivity};
                DeclareActivity active3{&otherActivity};
                DeclareActivity active4{&lazyEvalActivity};

                try {
                    // something that will throw
                    Execution::Throw (Exception<> (L"testing 123"));
                }
                catch (...) {
                    String msg = Characters::ToString (current_exception ());
                    VerifyTestResult (msg.Contains (L"testing 123"));
                    VerifyTestResult (msg.Contains (L"a1"));
                    VerifyTestResult (msg.Contains (L"kOtherActivity"));
                    VerifyTestResult (msg.Contains (L"otherActivity"));
                    VerifyTestResult (msg.Contains (L"xxx"));
                }
            }
        }
        void TestAll_ ()
        {
            Private::T1_Basics_ ();
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Test2_ThrowCatchStringException_ ();
        Test3_SystemErrorException_::TestAll_ ();
        Test4_Activities_::TestAll_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
