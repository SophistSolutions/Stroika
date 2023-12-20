/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
//  TEST    Foundation::Execution::ProcessRunner
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#if qPlatform_POSIX
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

using Characters::String;

#if qHasFeature_GoogleTest
namespace {
    void RegressionTest1_ ()
    {
        Debug::TraceContextBumper        ctx{"RegressionTest1_"};
        Streams::MemoryStream::Ptr<byte> myStdOut = Streams::MemoryStream::New<byte> ();
        // quickie about to test..
        ProcessRunner pr (L"echo hi mom", nullptr, myStdOut);
        pr.Run ();
    }
    void RegressionTest2_ ()
    {
        Debug::TraceContextBumper        ctx{"RegressionTest2_"};
        Streams::MemoryStream::Ptr<byte> myStdOut = Streams::MemoryStream::New<byte> ();
        // quickie about to test..
        ProcessRunner pr{"echo hi mom"};
        String        out = pr.Run ("");
        EXPECT_TRUE (out.Trim () == "hi mom");
    }
    void RegressionTest3_Pipe_ ()
    {
        Debug::TraceContextBumper        ctx{"RegressionTest3_Pipe_"};
        Streams::MemoryStream::Ptr<byte> myStdOut = Streams::MemoryStream::New<byte> ();
        ProcessRunner                    pr1{"echo hi mom"};
        Streams::MemoryStream::Ptr<byte> pipe = Streams::MemoryStream::New<byte> ();
        ProcessRunner                    pr2{"cat"};
        pr1.SetStdOut (pipe);
        pr2.SetStdIn (pipe);

        Streams::MemoryStream::Ptr<byte> pr2Out = Streams::MemoryStream::New<byte> ();
        pr2.SetStdOut (pr2Out);

        pr1.Run ();
        pr2.Run ();

        String out = String::FromUTF8 (pr2Out.As<string> ());

        EXPECT_TRUE (out.Trim () == "hi mom");
    }
    void RegressionTest4_DocSample_ ()
    {
        Debug::TraceContextBumper ctx{"RegressionTest4_DocSample_"};
        // cat doesn't exist on windows (without cygwin or some such) - but the regression test code depends on that anyhow
        // so this should be OK for now... -- LGP 2017-06-31
        Memory::BLOB                     kData_{Memory::BLOB::FromRaw ("this is a test")};
        Streams::MemoryStream::Ptr<byte> processStdIn  = Streams::MemoryStream::New<byte> (kData_);
        Streams::MemoryStream::Ptr<byte> processStdOut = Streams::MemoryStream::New<byte> ();
        ProcessRunner                    pr{"cat", processStdIn, processStdOut};
        pr.Run ();
        EXPECT_TRUE (processStdOut.ReadAll () == kData_);
    }
}

namespace {
    namespace LargeDataSentThroughPipe_Test5_ {
        namespace Private_ {
            const Memory::BLOB k1K_   = Memory::BLOB::FromRaw ("0123456789abcdef").Repeat (1024 / 16);
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
                Streams::MemoryStream::Ptr<byte> myStdIn  = Streams::MemoryStream::New<byte> (testBLOB);
                Streams::MemoryStream::Ptr<byte> myStdOut = Streams::MemoryStream::New<byte> ();
                ProcessRunner                    pr{"cat", myStdIn, myStdOut};
                pr.Run ();
                EXPECT_TRUE (myStdOut.ReadAll () == testBLOB);
            }
        }
        void DoTests ()
        {
            Debug::TraceContextBumper ctx{"LargeDataSentThroughPipe_Test5_::DoTests"};
            Private_::SingleProcessLargeDataSend_ ();
        }
    }
}

namespace {
    namespace LargeDataSentThroughPipeBackground_Test6_ {
        namespace Private_ {
            const Memory::BLOB k1K_   = Memory::BLOB::FromRaw ("0123456789abcdef").Repeat (1024 / 16);
            const Memory::BLOB k1MB_  = k1K_.Repeat (1024);
            const Memory::BLOB k16MB_ = k1MB_.Repeat (16);

            void SingleProcessLargeDataSend_ ()
            {
                Assert (k1MB_.size () == 1024 * 1024);
                Streams::SharedMemoryStream::Ptr<byte> myStdIn =
                    Streams::SharedMemoryStream::New<byte> (); // note must use SharedMemoryStream cuz we want to distinguish EOF from no data written yet
                Streams::SharedMemoryStream::Ptr<byte> myStdOut = Streams::SharedMemoryStream::New<byte> ();
                ProcessRunner                          pr{"cat", myStdIn, myStdOut};
                ProcessRunner::BackgroundProcess       bg = pr.RunInBackground ();
                Execution::Sleep (1);
                EXPECT_TRUE (not myStdOut.ReadNonBlocking ().has_value ()); // sb no data available, but NOT EOF
                /*
                 *  "Valgrind's memory management: out of memory:"
                 *  This only happens with DEBUG builds and valgrind/helgrind. So run with less memory used, and it works better.
                 */
                Memory::BLOB testBLOB = (Debug::IsRunningUnderValgrind () && qDebug) ? k1K_ : k16MB_;
                myStdIn.Write (testBLOB);
                myStdIn.CloseWrite (); // so cat process can finish
                bg.WaitForDone ();
                myStdOut.CloseWrite (); // one process done, no more writes to this stream
                EXPECT_TRUE (myStdOut.ReadAll () == testBLOB);
            }
        }
        void DoTests ()
        {
            Debug::TraceContextBumper ctx{"LargeDataSentThroughPipeBackground_Test6_::DoTests"};
            Private_::SingleProcessLargeDataSend_ ();
        }
    }
}

void RegressionTes7_FaledRun_ ()
{
    Debug::TraceContextBumper ctx{"RegressionTes7_FaledRun_"};
    try {
        ProcessRunner pr{"mount /fasdkfjasdfjasdkfjasdklfjasldkfjasdfkj /dadsf/a/sdf/asdf//"};
        pr.Run ();
        EXPECT_TRUE (false);
    }
    catch (...) {
        DbgTrace (L"got failure msg: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

namespace {
    GTEST_TEST (Foundation_Caching, all)
    {
        Debug::TraceContextBumper ctx{"DoRegressionTests_"};
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
