/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
//  TEST    Foundation::Execution::ProcessRunner
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#if qPlatform_POSIX
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"

#include "../TestHarness/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Characters::String;

namespace {
    void RegressionTest1_ ()
    {
        Debug::TraceContextBumper        ctx{L"RegressionTest1_"};
        Streams::MemoryStream<byte>::Ptr myStdOut = Streams::MemoryStream<byte>::New ();
        // quickie about to test..
        ProcessRunner pr (L"echo hi mom", nullptr, myStdOut);
        pr.Run ();
    }
    void RegressionTest2_ ()
    {
        Debug::TraceContextBumper        ctx{L"RegressionTest2_"};
        Streams::MemoryStream<byte>::Ptr myStdOut = Streams::MemoryStream<byte>::New ();
        // quickie about to test..
        ProcessRunner pr (L"echo hi mom");
        String        out = pr.Run (L"");
        VerifyTestResult (out.Trim () == L"hi mom");
    }
    void RegressionTest3_Pipe_ ()
    {
        Debug::TraceContextBumper        ctx{L"RegressionTest3_Pipe_"};
        Streams::MemoryStream<byte>::Ptr myStdOut = Streams::MemoryStream<byte>::New ();
        ProcessRunner                    pr1 (L"echo hi mom");
        Streams::MemoryStream<byte>::Ptr pipe = Streams::MemoryStream<byte>::New ();
        ProcessRunner                    pr2 (L"cat");
        pr1.SetStdOut (pipe);
        pr2.SetStdIn (pipe);

        Streams::MemoryStream<byte>::Ptr pr2Out = Streams::MemoryStream<byte>::New ();
        pr2.SetStdOut (pr2Out);

        pr1.Run ();
        pr2.Run ();

        String out = String::FromUTF8 (pr2Out.As<string> ());

        VerifyTestResult (out.Trim () == L"hi mom");
    }
    void RegressionTest4_DocSample_ ()
    {
        Debug::TraceContextBumper ctx{L"RegressionTest4_DocSample_"};
        // cat doesn't exist on windows (without cygwin or some such) - but the regression test code depends on that anyhow
        // so this should be OK for now... -- LGP 2017-06-31
        Memory::BLOB                     kData_{Memory::BLOB::Raw ("this is a test")};
        Streams::MemoryStream<byte>::Ptr processStdIn  = Streams::MemoryStream<byte>::New (kData_);
        Streams::MemoryStream<byte>::Ptr processStdOut = Streams::MemoryStream<byte>::New ();
        ProcessRunner                    pr (L"cat", processStdIn, processStdOut);
        pr.Run ();
        VerifyTestResult (processStdOut.ReadAll () == kData_);
    }
}

namespace {
    namespace LargeDataSentThroughPipe_Test5_ {
        namespace Private_ {
            const Memory::BLOB k1K_   = Memory::BLOB::Raw ("0123456789abcdef").Repeat (1024 / 16);
            const Memory::BLOB k1MB_  = k1K_.Repeat (1024);
            const Memory::BLOB k16MB_ = k1MB_.Repeat (16);

            void SingleProcessLargeDataSend_ ()
            {
                /*
                 *  "Valgrind's memory management: out of memory:"
                 *  This only happens with DEBUG builds and valgrind/helgrind. So run with less memory used, and it works better.
                 *
                 *  @see https://stroika.atlassian.net/browse/STK-713 if you see hang here
                 */
                Memory::BLOB                     testBLOB = (Debug::IsRunningUnderValgrind () && qDebug) ? k1K_ : k16MB_;
                Streams::MemoryStream<byte>::Ptr myStdIn  = Streams::MemoryStream<byte>::New (testBLOB);
                Streams::MemoryStream<byte>::Ptr myStdOut = Streams::MemoryStream<byte>::New ();
                ProcessRunner                    pr (L"cat", myStdIn, myStdOut);
                pr.Run ();
                VerifyTestResult (myStdOut.ReadAll () == testBLOB);
            }
        }
        void DoTests ()
        {
            Debug::TraceContextBumper ctx{L"LargeDataSentThroughPipe_Test5_::DoTests"};
            Private_::SingleProcessLargeDataSend_ ();
        }
    }
}

namespace {
    namespace LargeDataSentThroughPipeBackground_Test6_ {
        namespace Private_ {
            const Memory::BLOB k1K_   = Memory::BLOB::Raw ("0123456789abcdef").Repeat (1024 / 16);
            const Memory::BLOB k1MB_  = k1K_.Repeat (1024);
            const Memory::BLOB k16MB_ = k1MB_.Repeat (16);

            void SingleProcessLargeDataSend_ ()
            {
                Assert (k1MB_.size () == 1024 * 1024);
                Streams::SharedMemoryStream<byte>::Ptr myStdIn  = Streams::SharedMemoryStream<byte>::New (); // note must use SharedMemoryStream cuz we want to distinguish EOF from no data written yet
                Streams::SharedMemoryStream<byte>::Ptr myStdOut = Streams::SharedMemoryStream<byte>::New ();
                ProcessRunner                          pr (L"cat", myStdIn, myStdOut);
                ProcessRunner::BackgroundProcess       bg = pr.RunInBackground ();
                Execution::Sleep (1);
                VerifyTestResult (not myStdOut.ReadNonBlocking ().has_value ()); // sb no data available, but NOT EOF
                /*
                 *  "Valgrind's memory management: out of memory:"
                 *  This only happens with DEBUG builds and valgrind/helgrind. So run with less memory used, and it works better.
                 */
                Memory::BLOB testBLOB = (Debug::IsRunningUnderValgrind () && qDebug) ? k1K_ : k16MB_;
                myStdIn.Write (testBLOB);
                myStdIn.CloseWrite (); // so cat process can finish
                bg.WaitForDone ();
                myStdOut.CloseWrite (); // one process done, no more writes to this stream
                VerifyTestResult (myStdOut.ReadAll () == testBLOB);
            }
        }
        void DoTests ()
        {
            Debug::TraceContextBumper ctx{L"LargeDataSentThroughPipeBackground_Test6_::DoTests"};
            if constexpr (qCompiler_SanitizerDoubleLockWithConditionVariables_Buggy and Debug::kBuiltWithThreadSanitizer) {
                // for valgrind, really only HELGRIND issue
                DbgTrace ("Skipping this test cuz double locks cause TSAN to die and cannot be easily suppressed, and conditional variables broken checking on ubuntu 20.10 g++10");
                return;
            }
            Private_::SingleProcessLargeDataSend_ ();
        }
    }
}

void RegressionTes7_FaledRun_ ()
{
    Debug::TraceContextBumper ctx{L"RegressionTes7_FaledRun_"};
    try {
        ProcessRunner pr (L"mount /fasdkfjasdfjasdkfjasdklfjasldkfjasdfkj /dadsf/a/sdf/asdf//");
        pr.Run ();
        VerifyTestResult (false);
    }
    catch (...) {
        DbgTrace (L"got failure msg: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

namespace {

    void DoRegressionTests_ ()
    {
        Debug::TraceContextBumper ctx{L"DoRegressionTests_"};
#if qPlatform_POSIX
        // Many performance instruments use pipes
        // @todo - REVIEW IF REALLY NEEDED AND WHY? SO LONG AS NO FAIL SHOULDNT BE?
        //  --LGP 2014-02-05
        Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif
        RegressionTest1_ ();
        RegressionTest2_ ();
        RegressionTest3_Pipe_ ();
        RegressionTest4_DocSample_ ();
        LargeDataSentThroughPipe_Test5_::DoTests ();
        LargeDataSentThroughPipeBackground_Test6_::DoTests ();
        RegressionTes7_FaledRun_ ();
    }
}

int main ([[maybe_unused]] int argc, [[maybe_unused]] const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
