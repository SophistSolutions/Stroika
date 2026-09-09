/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Execution::Exceptions
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>
#include <sstream>

#if qStroika_Foundation_Common_Platform_Windows
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
#if qStroika_Foundation_Common_Platform_Windows
#include "Stroika/Foundation/Execution/Platform/Windows/Exception.h"
#endif

#include "Stroika/Frameworks/Test/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters::Literals;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

#if qStroika_HasComponent_googletest
namespace {
    GTEST_TEST (Foundation_Execution_Exceptions, ThrowCatchStringException_)
    {
        Debug::TraceContextBumper ctx{"ThrowCatchStringException_"};
        try {
            Throw (Exception{"HiMom"});
            EXPECT_TRUE (false);
        }
        catch (const Exception<>& e) {
            EXPECT_EQ (e.As<wstring> (), L"HiMom");
        }
        try {
            Throw (Exception{"HiMom"});
            EXPECT_TRUE (false);
        }
        catch (const std::exception& e) {
            EXPECT_EQ (strcmp (e.what (), "HiMom"), 0); // IF THIS FAILS SEE qCompilerAndStdLib_Debug32_asan_Poison_Buggy
        }
    }
}

namespace {
    namespace Test3_SystemErrorException_ {
        namespace Private_ {
            void T1_system_error_ ()
            {
                static const int kErr2TestFor_ = make_error_code (errc::bad_address).value ();          // any value from errc would do
                static const Characters::String kErr2TestForExpectedMsg_ = "bad address {errno: 14}"sv; // maybe not always right due to locales?
                try {
                    ThrowPOSIXErrNo (kErr2TestFor_);
                }
                catch (const std::system_error& e) {
                    EXPECT_EQ (e.code ().value (), kErr2TestFor_);
                    EXPECT_TRUE (e.code ().category () == system_category () or e.code ().category () == generic_category ());
                    EXPECT_TRUE (Characters::ToString (e).Contains (kErr2TestForExpectedMsg_, Characters::eCaseInsensitive));
                }
                catch (...) {
                    DbgTrace ("err={}"_f, current_exception ());
                    EXPECT_TRUE (false); //oops
                }
                // and test throwing fancy unicode string

                const Characters::String kMsgWithUnicode_ = L"zß水𝄋"; // this works even if using a code page / locale which doesn't support UNICODE/Chinese
                try {
                    Execution::Throw (SystemErrorException{kErr2TestFor_, generic_category (), kMsgWithUnicode_});
                }
                catch (const std::system_error& e) {
                    EXPECT_EQ (e.code ().value (), kErr2TestFor_);
                    EXPECT_EQ (e.code ().category (), generic_category ());
                    EXPECT_TRUE (Characters::ToString (e).Contains (kMsgWithUnicode_, Characters::eCaseInsensitive));
                }
                catch (...) {
                    DbgTrace ("err={}"_f, current_exception ());
                    EXPECT_TRUE (false); //oops
                }
            }
            void T2_TestTimeout_ ()
            {
                try {
                    Execution::ThrowError (errc::timed_out);
                }
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out);
                    EXPECT_TRUE (e.code () != errc::already_connected);
                }
                catch (...) {
                    DbgTrace ("err={}"_f, current_exception ());
                    EXPECT_TRUE (false); //oops
                }
                try {
                    Execution::ThrowError (errc::timed_out);
                }
                catch (const Execution::SystemErrorException& e) {
                    EXPECT_EQ (e.code (), errc::timed_out);
                    EXPECT_TRUE (e.code () != errc::already_connected);
                }
                catch (...) {
                    DbgTrace ("err={}"_f, current_exception ());
                    EXPECT_TRUE (false); //oops
                }
                const Characters::String kMsg1_ = L"to abcd 123 zß水𝄋";
                try {
                    Execution::Throw (Execution::SystemErrorException{make_error_code (errc::timed_out), kMsg1_});
                }
                catch (const system_error& e) {
                    EXPECT_EQ (e.code (), errc::timed_out);
                    EXPECT_TRUE (e.code () != errc::already_connected);
                    EXPECT_TRUE (Characters::ToString (e).Contains (kMsg1_));
                }
                catch (...) {
                    DbgTrace ("err={}"_f, current_exception ());
                    EXPECT_TRUE (false); //oops
                }
            }
        }
    }
    GTEST_TEST (Foundation_Execution_Exceptions, SystemErrorException_)
    {
        Debug::TraceContextBumper ctx{"SystemErrorException_"};
        Test3_SystemErrorException_::Private_::T1_system_error_ ();
        Test3_SystemErrorException_::Private_::T2_TestTimeout_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_Exceptions, Test4_Activities_)
    {
        Debug::TraceContextBumper ctx{"Test4_Activities_"};
        {
            using Characters::String;
            String argument;

            [[maybe_unused]] static constexpr Activity kBuildingThingy1_{"Building thingy"sv};
            [[maybe_unused]] static constexpr Activity kBuildingThingy2{"Building thingy"sv};
            [[maybe_unused]] static constexpr Activity kBuildingThingy3_{"Building thingy"};
            [[maybe_unused]] static constexpr Activity kBuildingThingy4_{"Building thingy"};

            // constexpr only works if we lose the virtual in ~AsStringObj_ ()
            static constexpr const auto kA1_{Activity<wstring_view>{L"a1"sv}};

            static const auto kOtherActivity = Activity<String>{L"kOtherActivity"};

            // automatic variable activity OK as long as it's lifetime longer than reference in DeclareActivity
            auto otherActivity = Activity<String>{"otherActivity" + argument}; // activities can be stack based, but these cost more to define

            auto lazyEvalActivity = LazyEvalActivity{[&] () -> String { return argument.Repeat (5) + "xxx"; }};

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
        {
            [[maybe_unused]] static const Activity test1{"performing sqlite document db employees sample on"sv};
            static const Activity test2{"performing sqlite document db employees sample on {}"}; // before Stroika 3.0d21 had trouble compiling this
            EXPECT_EQ (test2.AsString (), "performing sqlite document db employees sample on {}");
        }
    }
}

namespace {
    namespace Test4b_ActivitiesImbuedAtThrowPoint_ {
        namespace Private_ {

            static const Activity kParsingConfig_{"parsing the config file"sv};
            static const Activity kServingRequest_{"serving an HTTP request"sv};

            [[noreturn]] void ThrowTheSharedStatic_ ()
            {
                // deliberately a shared static: constructed ONCE, thrown many times. This is a common Stroika
                // idiom, and before v3.0d25 it froze whatever Activity stack was live at the FIRST throw.
                static const Exception<> kException_{"Invalid Version String"sv};
                Throw (kException_);
            }
            Characters::String ThrowAndDescribe_ ()
            {
                try {
                    ThrowTheSharedStatic_ ();
                }
                catch (...) {
                    return Characters::ToString (current_exception ());
                }
                return "DID-NOT-THROW"sv;
            }

            /*
             *  Activities are imbued at the THROW point (@see Throw), so every throw of the same shared object
             *  reports its own context. The 'second' case below is the actual regression: it used to report the
             *  first throw's context, confidently and wrongly.
             */
            void EachThrowReportsItsOwnContext_ ()
            {
                Characters::String first;
                {
                    DeclareActivity a{&kParsingConfig_};
                    first = ThrowAndDescribe_ ();
                }
                Characters::String second;
                {
                    DeclareActivity a{&kServingRequest_};
                    second = ThrowAndDescribe_ ();
                }
                Characters::String third = ThrowAndDescribe_ (); // no activity declared at all

                EXPECT_TRUE (first.Contains ("Invalid Version String"));
                EXPECT_TRUE (first.Contains ("parsing the config file"));
                EXPECT_TRUE (not first.Contains ("serving an HTTP request"));

                EXPECT_TRUE (second.Contains ("serving an HTTP request"));
                EXPECT_TRUE (not second.Contains ("parsing the config file")); // <- the regression

                EXPECT_TRUE (third.Contains ("Invalid Version String"));
                EXPECT_TRUE (not third.Contains ("parsing the config file"));
                EXPECT_TRUE (not third.Contains ("serving an HTTP request"));
            }

            /*
             *  Activities specified deliberately are never overwritten: @see Throw imbues only when
             *  GetActivities () == nullopt, which is what makes nullopt ("not specified") meaningfully different
             *  from an empty stack ("specified, and there were none").
             */
            void DeliberatelySpecifiedActivitiesSurviveThrow_ ()
            {
                static const Activity         kExplicit_{"an explicitly recorded activity"sv};
                Containers::Stack<Activity<>> explicitStack;
                {
                    DeclareActivity a{&kExplicit_};
                    explicitStack = CaptureCurrentActivities ();
                }
                try {
                    DeclareActivity a{&kParsingConfig_}; // current context - must NOT win
                    Exception<>     e{"boom"sv};
                    e.ImbueActivities (explicitStack);
                    EXPECT_TRUE (e.GetActivities () != nullopt);
                    Throw (e);
                }
                catch (...) {
                    Characters::String msg = Characters::ToString (current_exception ());
                    EXPECT_TRUE (msg.Contains ("an explicitly recorded activity"));
                    EXPECT_TRUE (not msg.Contains ("parsing the config file"));
                }
            }
        }
    }
    GTEST_TEST (Foundation_Execution_Exceptions, Test4b_ActivitiesImbuedAtThrowPoint_)
    {
        Debug::TraceContextBumper ctx{"Test4b_ActivitiesImbuedAtThrowPoint_"};
        Test4b_ActivitiesImbuedAtThrowPoint_::Private_::EachThrowReportsItsOwnContext_ ();
        Test4b_ActivitiesImbuedAtThrowPoint_::Private_::DeliberatelySpecifiedActivitiesSurviveThrow_ ();
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
#if qStroika_Foundation_Common_Platform_Windows
            void Bug2_Windows_Errors_Mapped_To_Conditions_ ()
            {
                EXPECT_TRUE ((error_code{ERROR_NOT_ENOUGH_MEMORY, system_category ()} == errc::not_enough_memory));
                EXPECT_TRUE ((error_code{ERROR_OUTOFMEMORY, system_category ()} == errc::not_enough_memory));
#if qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy
                if ((error_code{WAIT_TIMEOUT, system_category ()} == errc::timed_out)) {
                    DbgTrace ("FIXED - qCompilerAndStdLib_Winerror_map_doesnt_map_timeout_Buggy"_f);
                }
                if ((error_code{ERROR_INTERNET_TIMEOUT, system_category ()} == errc::timed_out)) {
                    DbgTrace ("FIXED"_f);
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
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out); // the point of the test: the WIN32 code maps to the CONDITION
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
                try {
                    ThrowSystemErrNo (ERROR_INTERNET_TIMEOUT);
                }
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out); // the point of the test: the WIN32 code maps to the CONDITION
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
            }
#endif
        }
    }
    GTEST_TEST (Foundation_Execution_Exceptions, Test5_error_code_condition_compares_)
    {
        Debug::TraceContextBumper ctx{"Test5_error_code_condition_compares_"};
        Test5_error_code_condition_compares_::Private::Bug1_ ();
#if qStroika_Foundation_Common_Platform_Windows
        Test5_error_code_condition_compares_::Private::Bug2_Windows_Errors_Mapped_To_Conditions_ ();
#endif
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
                        Throw (Exception ("HiMom"));
                        EXPECT_TRUE (false);
                    }
                    catch (const Exception<>& e) {
                        EXPECT_TRUE (e.As<wstring> () == L"HiMom");
                    }
                }
                {
                    try {
                        Throw (Exception ("HiMom"));
                        EXPECT_TRUE (false);
                    }
                    catch (const std::exception& e) {
                        EXPECT_TRUE (strcmp (e.what (), "HiMom") == 0);
                    }
                }
            }
        }
    }
    GTEST_TEST (Foundation_Execution_Exceptions, Throw_Logging_with_and_without_srclines_in_stack_backtrace_)
    {
        using namespace Test6_Throw_Logging_with_and_without_srclines_in_stack_backtrace_;
        Debug::TraceContextBumper ctx{"Test6_Throw_Logging_with_and_without_srclines_in_stack_backtrace_"};
        auto                      prevValue = Debug::BackTrace::Options::sDefault_IncludeSourceLines;
        DbgTrace ("sDefault_IncludeSourceLines = true"_f);
        Debug::BackTrace::Options::sDefault_IncludeSourceLines = true;
        Private::ThrowCatchStringException_ ();
        DbgTrace ("sDefault_IncludeSourceLines = false"_f);
        Debug::BackTrace::Options::sDefault_IncludeSourceLines = false;
        Private::ThrowCatchStringException_ ();
        DbgTrace ("sDefault_IncludeSourceLines = <<default>>"_f);
        Debug::BackTrace::Options::sDefault_IncludeSourceLines = prevValue;
        Private::ThrowCatchStringException_ ();
    }
}

namespace {
    namespace Test7_ThrowError_promotions_and_conditions_ {
        namespace Private_ {

            /*
             *  A third-party-style error_category: its own numbering, mapped onto std::errc through
             *  default_error_condition () - exactly what the LibCurl and getaddrinfo/DNS categories do.
             *  ThrowError () promises that ANY such category participates automatically; these tests pin that
             *  promise without dragging IO::Network into an Execution::Exceptions test. The real libcurl
             *  category is checked where it belongs, in the networking regression tests.
             */
            constexpr int kFakeTimedOut_ = 101;
            constexpr int kFakeNoMemory_ = 102;
            constexpr int kFakeOther_    = 103;

            const error_category& Fake_error_category_ () noexcept
            {
                class FakeCategory_ : public error_category {
                public:
                    virtual const char* name () const noexcept override
                    {
                        return "Stroika-Test37-third-party-ish";
                    }
                    virtual string message (int ev) const override
                    {
                        return "fake error " + to_string (ev);
                    }
                    virtual error_condition default_error_condition (int ev) const noexcept override
                    {
                        switch (ev) {
                            case kFakeTimedOut_:
                                return make_error_condition (errc::timed_out);
                            case kFakeNoMemory_:
                                return make_error_condition (errc::not_enough_memory);
                        }
                        return error_condition{ev, *this};
                    }
                };
                static const FakeCategory_ kInstance_;
                return kInstance_;
            }

            /*
             *  Why the documented advice is "catch system_error, never SystemErrorException": Stroika adds its
             *  UNICODE-string/Activity support to a std exception type by deriving Exception<THAT_TYPE> from it,
             *  and any two of those meet only at their common std base. That is not academic -
             *  IO::FileSystem::Exception is an Exception<filesystem_error>, so a caller catching
             *  SystemErrorException silently misses every filesystem error.
             *  (std::ios_base::failure stands in for filesystem_error here purely to avoid dragging <filesystem>
             *  into this test; they are the only two subclasses of system_error the standard defines.)
             */
            static_assert (is_base_of_v<system_error, SystemErrorException>);
            static_assert (is_base_of_v<system_error, Execution::Exception<ios_base::failure>>);
            static_assert (not is_base_of_v<SystemErrorException, Execution::Exception<ios_base::failure>>);

            template <typename THROWER>
            void CheckIsTimeout_ (THROWER&& thrower, const char* label)
            {
                try {
                    thrower ();
                    EXPECT_TRUE (false) << label << ": expected an exception";
                }
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out) << label << ": got " << e.code ().message ();
                }
                catch (...) {
                    EXPECT_TRUE (false) << label << ": threw something which is not a system_error";
                }
            }

            /*
             *  THE regression test for the defect all of this exists to prevent. Before v3.0d25 a libcurl
             *  timeout was a plain system_error in libcurl's own category, so catch (const TimeOutException&)
             *  never matched it - which cost two release-validation re-runs before anyone noticed the
             *  tolerance code was catching the wrong thing. However a timeout is produced, it must answer to
             *  ONE portable test.
             */
            void TimeoutsFromEverySourceMatchTheCondition_ ()
            {
                DISABLE_COMPILER_MSC_WARNING_START (4996);
                DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
                DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
                // deprecated as of v3.0d25, but while it exists it must still answer to the condition
                CheckIsTimeout_ ([] () { ThrowTimeOutException (); }, "ThrowTimeOutException () [deprecated]");
                DISABLE_COMPILER_MSC_WARNING_END (4996);
                DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
                DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
                CheckIsTimeout_ ([] () { ThrowError (error_code{kFakeTimedOut_, Fake_error_category_ ()}); },
                                 "ThrowError (third-party category)");
                CheckIsTimeout_ ([] () { ThrowError (errc::timed_out); }, "ThrowError (errc::timed_out)");
                CheckIsTimeout_ ([] () { ThrowError (errc::timed_out, "with a message"sv); }, "ThrowError (errc, message)");
                CheckIsTimeout_ ([] () { ThrowPOSIXErrNo (ETIMEDOUT); }, "ThrowPOSIXErrNo (ETIMEDOUT)");
#if qStroika_Foundation_Common_Platform_Windows
                CheckIsTimeout_ ([] () { ThrowSystemErrNo (WAIT_TIMEOUT); }, "ThrowSystemErrNo (WAIT_TIMEOUT)");
                CheckIsTimeout_ ([] () { ThrowSystemErrNo (ERROR_INTERNET_TIMEOUT); }, "ThrowSystemErrNo (ERROR_INTERNET_TIMEOUT)");
#else
                CheckIsTimeout_ ([] () { ThrowSystemErrNo (ETIMEDOUT); }, "ThrowSystemErrNo (ETIMEDOUT)");
#endif
            }

            /*
             *  bad_alloc is the one promotion which changes what a caller must CATCH, since it does not derive
             *  from system_error. Before v3.0d25 OS errors did this but third-party-category errors did not.
             */
            void OutOfMemoryAlwaysBecomesBadAlloc_ ()
            {
                auto check = [] (auto&& thrower, const char* label) {
                    try {
                        thrower ();
                        EXPECT_TRUE (false) << label << ": expected an exception";
                    }
                    catch (const bad_alloc&) {
                        // Good
                    }
                    catch (...) {
                        EXPECT_TRUE (false) << label << ": expected bad_alloc";
                    }
                };
                check ([] () { ThrowError (error_code{kFakeNoMemory_, Fake_error_category_ ()}); }, "ThrowError (third-party category)");
                check ([] () { ThrowPOSIXErrNo (ENOMEM); }, "ThrowPOSIXErrNo (ENOMEM)");
            }

            /*
             *  Promotion changes the TYPE thrown, never the code carried - so e.code () still names where the
             *  error actually came from. The single documented exception is the Windows WAIT_TIMEOUT
             *  workaround, whose entire purpose is to substitute a code which compares equal.
             */
            void PromotionPreservesTheOriginalErrorCode_ ()
            {
                try {
                    ThrowError (error_code{kFakeTimedOut_, Fake_error_category_ ()});
                    EXPECT_TRUE (false);
                }
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code () == errc::timed_out);
                    EXPECT_TRUE (e.code ().value () == kFakeTimedOut_);
                    EXPECT_TRUE (e.code ().category () == Fake_error_category_ ());
                }
            }

            void UnpromotedCodesPassThroughUnchanged_ ()
            {
                try {
                    ThrowError (error_code{kFakeOther_, Fake_error_category_ ()});
                    EXPECT_TRUE (false);
                }
                catch (const system_error& e) {
                    EXPECT_TRUE (e.code ().value () == kFakeOther_);
                    EXPECT_TRUE (e.code ().category () == Fake_error_category_ ());
                    EXPECT_TRUE (e.code () != errc::timed_out);
                    EXPECT_TRUE (e.code () != errc::not_enough_memory);
                }
            }

            /*
             *  Documentation-as-test. Comparing against a specific error_code matches only the ONE source which
             *  produced it; comparing against a CONDITION matches every source. Getting this wrong is the
             *  code-level twin of the type-level mistake (catch (const TimeOutException&)) which hid the
             *  original defect - it compiles, it reads fine, and it silently misses.
             */
            void TestTheConditionNotTheCode_ ()
            {
                const error_code kFromThirdParty_ = error_code{kFakeTimedOut_, Fake_error_category_ ()};
                EXPECT_TRUE (kFromThirdParty_ == errc::timed_out);                             // RIGHT
                EXPECT_FALSE (kFromThirdParty_ == error_code (ETIMEDOUT, system_category ())); // the trap
            }

            /*
             *  Catching the BASE must not cost you the Stroika half of the exception - which is the thing that
             *  makes the "catch system_error, not SystemErrorException" advice safe to give. Both the UNICODE
             *  message and the Activity stack come back, because Characters::ToString () cross-casts to
             *  ExceptionStringHelper and returns GetFullErrorMessage (). Reading e.what () instead is the lossy
             *  path, and is what this test exists to discourage.
             */
            void UnicodeAndActivitiesSurviveABaseClassCatch_ ()
            {
                static const Activity    kDoingTheThing_{"doing the thing"sv};
                const Characters::String kMsg_ = L"context zß水𝄋";
                try {
                    DeclareActivity declareActivity{&kDoingTheThing_};
                    ThrowError (error_code{kFakeTimedOut_, Fake_error_category_ ()}, kMsg_);
                    EXPECT_TRUE (false);
                }
                catch (const system_error& e) { // deliberately the BASE, not SystemErrorException
                    const Characters::String kGot_ = Characters::ToString (e);
                    EXPECT_TRUE (kGot_.Contains (kMsg_));             // the UNICODE message survived
                    EXPECT_TRUE (kGot_.Contains ("doing the thing")); // ... and so did the Activity
                    EXPECT_TRUE (e.code () == errc::timed_out);       // ... and it is still a timeout
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
            }

            /*
             *  TRANSITIONAL (v3.0d25): TimeOutException is deprecated, but ThrowError () still promotes to it so
             *  existing catch clauses keep working. The second case is the interesting one - the deprecated type
             *  now catches a THIRD-PARTY-category timeout, which is exactly what it never did before.
             *  When the class and the promotion are removed (they must go together), THIS BLOCK STOPS
             *  COMPILING - which is the intended reminder to delete it along with them.
             */
            DISABLE_COMPILER_MSC_WARNING_START (4996);
            DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
            void DeprecatedTimeOutExceptionStillCatches_ ()
            {
                try {
                    ThrowTimeOutException ();
                    EXPECT_TRUE (false);
                }
                catch (const TimeOutException&) {
                    // Good - guaranteed while the type still exists
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
                try {
                    ThrowError (error_code{kFakeTimedOut_, Fake_error_category_ ()});
                    EXPECT_TRUE (false);
                }
                catch (const TimeOutException&) {
                    // Good - and THIS is the case which used to silently not match
                }
                catch (...) {
                    EXPECT_TRUE (false);
                }
            }
            DISABLE_COMPILER_MSC_WARNING_END (4996);
            DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
        }
    }
    GTEST_TEST (Foundation_Execution_Exceptions, Test7_ThrowError_promotions_and_conditions_)
    {
        using namespace Test7_ThrowError_promotions_and_conditions_;
        Debug::TraceContextBumper ctx{"Test7_ThrowError_promotions_and_conditions_"};
        Private_::TimeoutsFromEverySourceMatchTheCondition_ ();
        Private_::OutOfMemoryAlwaysBecomesBadAlloc_ ();
        Private_::PromotionPreservesTheOriginalErrorCode_ ();
        Private_::UnpromotedCodesPassThroughUnchanged_ ();
        Private_::TestTheConditionNotTheCode_ ();
        Private_::UnicodeAndActivitiesSurviveABaseClassCatch_ ();
        Private_::DeprecatedTimeOutExceptionStillCatches_ ();
    }
}

#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
