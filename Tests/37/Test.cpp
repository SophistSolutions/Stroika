/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
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
#include "Stroika/Foundation/Debug/BackTrace.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Exceptions.h"
#include "Stroika/Foundation/Execution/TimeOutException.h"
#if qPlatform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

#if qHasFeature_GoogleTest
namespace {
    void Test2_ThrowCatchStringException_ ()
    {
        Debug::TraceContextBumper ctx{L"Test2_ThrowCatchStringException_"};
        {
            try {
                Throw (Exception{L"HiMom"});
                EXPECT_TRUE (false);
            }
            catch (const Exception<>& e) {
                EXPECT_TRUE (e.As<wstring> () == L"HiMom");
            }
        }
        {
            try {
                Throw (Exception{L"HiMom"});
                EXPECT_TRUE (false);
            }
            catch (const std::exception& e) {
                EXPECT_TRUE (strcmp (e.what (), "HiMom") == 0); // IF THIS FAILS SEE qCompilerAndStdLib_Debug32_asan_Poison_Buggy
            }
        }
    }
}

namespace {
    namespace Test3_SystemErrorException_ {
        namespace Private_ {
            void T1_system_error_ ()
            {
                static const int kErr2TestFor_ = make_error_code (errc::bad_address).value ();           // any value from errc would do
                static const Characters::String kErr2TestForExpectedMsg_ = L"bad address {errno: 14}"sv; // maybe not always right due to locales?

                try {
                    ThrowPOSIXErrNo (kErr2TestFor_);
                }
                catch (const std::system_error& e) {
                    EXPECT_TRUE (e.code ().value () == kErr2TestFor_);
                    EXPECT_TRUE (e.code ().category () == system_category () or e.code ().category () == generic_category ());
                    EXPECT_TRUE (Characters::ToString (e).Contains (kErr2TestForExpectedMsg_, Characters::eCaseInsensitive));
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    EXPECT_TRUE (false); //oops
                }
                // and test throwing fancy unicode string

                const Characters::String kMsgWithUnicode_ = L"zß水𝄋"; // this works even if using a code page / locale which doesn't support UNICODE/Chinese
                try {
                    Execution::Throw (SystemErrorException{kErr2TestFor_, generic_category (), kMsgWithUnicode_});
                }
                catch (const std::system_error& e) {
                    EXPECT_TRUE (e.code ().value () == kErr2TestFor_);
                    EXPECT_TRUE (e.code ().category () == generic_category ());
                    EXPECT_TRUE (Characters::ToString (e).Contains (kMsgWithUnicode_, Characters::eCaseInsensitive));
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    EXPECT_TRUE (false); //oops
                }
            }
            void T2_TestTimeout_ ()
            {
                try {
                    Execution::Throw (Execution::TimeOutException{});
                }
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out);
                    EXPECT_TRUE (e.code () != errc::already_connected);
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    EXPECT_TRUE (false); //oops
                }
                try {
                    Execution::Throw (Execution::TimeOutException{});
                }
                catch (const Execution::TimeOutException& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out);
                    EXPECT_TRUE (e.code () != errc::already_connected);
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    EXPECT_TRUE (false); //oops
                }
                const Characters::String kMsg1_ = L"to abcd 123 zß水𝄋";
                try {
                    Execution::Throw (Execution::TimeOutException{kMsg1_});
                }
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out);
                    EXPECT_TRUE (e.code () != errc::already_connected);
                    EXPECT_TRUE (Characters::ToString (e).Contains (kMsg1_));
                }
                catch (...) {
                    DbgTrace (L"err=%s", Characters::ToString (current_exception ()).c_str ());
                    EXPECT_TRUE (false); //oops
                }
            }
        }
        void TestAll_ ()
        {
            Debug::TraceContextBumper ctx{L"Test3_SystemErrorException_"};
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

                [[maybe_unused]] static constexpr Activity kBuildingThingy_{L"Building thingy"sv};

                // constexpr only works if we lose the virtual in ~AsStringObj_ ()
                static constexpr const auto kA1_{Activity<wstring_view>{L"a1"sv}};

                static const auto kOtherActivity = Activity<String>{L"kOtherActivity"};

                // automatic variable activity OK as long as it's lifetime longer than reference in DeclareActivity
                auto otherActivity = Activity<String>{L"otherActivity" + argument}; // activities can be stack based, but these cost more to define

                auto lazyEvalActivity = LazyEvalActivity{[&] () -> String { return argument.Repeat (5) + L"xxx"; }};

                DeclareActivity active1{&kA1_};
                DeclareActivity active2{&kOtherActivity};
                DeclareActivity active3{&otherActivity};
                DeclareActivity active4{&lazyEvalActivity};

                try {
                    // something that will throw
                    Execution::Throw (Exception<> ("testing 123"));
                }
                catch (...) {
                    String msg = Characters::ToString (current_exception ());
                    EXPECT_TRUE (msg.Contains ("testing 123"));
                    EXPECT_TRUE (msg.Contains ("a1"));
                    EXPECT_TRUE (msg.Contains ("kOtherActivity"));
                    EXPECT_TRUE (msg.Contains ("otherActivity"));
                    EXPECT_TRUE (msg.Contains ("xxx"));
                }
            }
        }
        void TestAll_ ()
        {
            Debug::TraceContextBumper ctx{"Test4_Activities_"};
            Private::T1_Basics_ ();
        }
    }
}

namespace {
    namespace Test5_error_code_condition_compares_ {
        namespace Private {
            void Bug1_ ()
            {
                try {
                    throw std::system_error (ENOENT, std::system_category ());
                }
                catch (std::system_error const& e) {
                    EXPECT_TRUE (e.code ().value () == static_cast<int> (std::errc::no_such_file_or_directory)); // workaround?
                    EXPECT_TRUE (e.code () == std::errc::no_such_file_or_directory);                             // <- FAILS!?
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
            }
#if qPlatform_Windows
            void Bug2_Windows_Errors_Mapped_To_Conditions_ ()
            {
                EXPECT_TRUE ((error_code{ERROR_NOT_ENOUGH_MEMORY, system_category ()} == errc::not_enough_memory));
                EXPECT_TRUE ((error_code{ERROR_OUTOFMEMORY, system_category ()} == errc::not_enough_memory));
#if qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy
                if ((error_code{WAIT_TIMEOUT, system_category ()} == errc::timed_out)) {
                    DbgTrace (L"FIXED - qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy");
                }
                if ((error_code{ERROR_INTERNET_TIMEOUT, system_category ()} == errc::timed_out)) {
                    DbgTrace (L"FIXED");
                }
#else
                EXPECT_TRUE ((error_code{WAIT_TIMEOUT, system_category ()} == errc::timed_out));
                EXPECT_TRUE ((error_code{ERROR_INTERNET_TIMEOUT, system_category ()} == errc::timed_out));
#endif

                try {
                    ThrowSystemErrNo (ERROR_NOT_ENOUGH_MEMORY);
                }
                catch (const bad_alloc&) {
                    // Good
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
                try {
                    ThrowSystemErrNo (ERROR_OUTOFMEMORY);
                }
                catch (const bad_alloc&) {
                    // Good
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
                try {
                    ThrowSystemErrNo (WAIT_TIMEOUT);
                }
                catch (const TimeOutException&) {
                    // Good
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
                try {
                    ThrowSystemErrNo (ERROR_INTERNET_TIMEOUT);
                }
                catch (const TimeOutException&) {
                    // Good
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
            }
#endif
        }
        void TestAll_ ()
        {
            Debug::TraceContextBumper ctx{L"Test5_error_code_condition_compares_"};
            Private::Bug1_ ();
#if qPlatform_Windows
            Private::Bug2_Windows_Errors_Mapped_To_Conditions_ ();
#endif
        }
    }
}

namespace {
    namespace Test6_Throw_Logging_with_and_without_srclines_in_stack_backtrace_ {
        namespace Private {
            void ThrowCatchStringException_ ()
            {
                Debug::TraceContextBumper ctx{"ThrowCatchStringException_"};
                {
                    try {
                        Throw (Exception (L"HiMom"));
                        EXPECT_TRUE (false);
                    }
                    catch (const Exception<>& e) {
                        EXPECT_TRUE (e.As<wstring> () == L"HiMom");
                    }
                }
                {
                    try {
                        Throw (Exception (L"HiMom"));
                        EXPECT_TRUE (false);
                    }
                    catch (const std::exception& e) {
                        EXPECT_TRUE (strcmp (e.what (), "HiMom") == 0);
                    }
                }
            }
        }
        void TestAll_ ()
        {
            Debug::TraceContextBumper ctx{"Test6_Throw_Logging_with_and_without_srclines_in_stack_backtrace_"};
            auto                      prevValue = Debug::BackTrace::Options::sDefault_IncludeSourceLines;
            DbgTrace ("sDefault_IncludeSourceLines = true");
            Debug::BackTrace::Options::sDefault_IncludeSourceLines = true;
            Private::ThrowCatchStringException_ ();
            DbgTrace ("sDefault_IncludeSourceLines = false");
            Debug::BackTrace::Options::sDefault_IncludeSourceLines = false;
            Private::ThrowCatchStringException_ ();
            DbgTrace ("sDefault_IncludeSourceLines = <<default>>");
            Debug::BackTrace::Options::sDefault_IncludeSourceLines = prevValue;
            Private::ThrowCatchStringException_ ();
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Debug::TraceContextBumper ctx{"DoRegressionTests_"};
        Test2_ThrowCatchStringException_ ();
        Test3_SystemErrorException_::TestAll_ ();
        Test4_Activities_::TestAll_ ();
        Test5_error_code_condition_compares_::TestAll_ ();
        Test6_Throw_Logging_with_and_without_srclines_in_stack_backtrace_::TestAll_ ();
    }
}
#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qHasFeature_GoogleTest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
