/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
//  TEST    Foundation::Execution::ProcessRunner
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Execution/ProcessRunner.h"
#if qPlatform_POSIX
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"

#include "../TestHarness/TestHarness.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Characters::String;

namespace {
    void RegressionTest1_ ()
    {
        Streams::MemoryStream<Byte> myStdOut;
        // quickie about to test..
        ProcessRunner pr (L"echo hi mom", nullptr, myStdOut);
        pr.Run ();
    }
    void RegressionTest2_ ()
    {
        Streams::MemoryStream<Byte> myStdOut;
        // quickie about to test..
        ProcessRunner pr (L"echo hi mom");
        String        out = pr.Run (L"");
        VerifyTestResult (out.Trim () == L"hi mom");
    }
    void RegressionTest3_Pipe_ ()
    {
        Streams::MemoryStream<Byte> myStdOut;
        ProcessRunner               pr1 (L"echo hi mom");
        Streams::MemoryStream<Byte> pipe;
        ProcessRunner               pr2 (L"cat");
        pr1.SetStdOut (pipe);
        pr2.SetStdIn (pipe);

        Streams::MemoryStream<Byte> pr2Out;
        pr2.SetStdOut (pr2Out);

        pr1.Run ();
        pr2.Run ();

        String out = String::FromUTF8 (pr2Out.As<string> ());

        VerifyTestResult (out.Trim () == L"hi mom");
    }
    void RegressionTest4_DocSample_ ()
    {
        // cat doesn't exist on windows (without cygwin or some such) - but the regression test code depends on that anyhow
        // so this should be OK for now... -- LGP 2017-06-31
        Memory::BLOB                kData_{Memory::BLOB::Raw ("this is a test")};
        Streams::MemoryStream<Byte> processStdIn{kData_};
        Streams::MemoryStream<Byte> processStdOut;
        ProcessRunner               pr (L"cat", processStdIn, processStdOut);
        pr.Run ();
        VerifyTestResult (processStdOut.ReadAll () == kData_);
    }
}

namespace {
    namespace LargeDataSentThroughPipe_Tesat5_ {
        namespace Private_ {
            const Memory::BLOB k1K_  = Memory::BLOB::Raw ("0123456789abcdef").Repeat (1024 / 16);
            const Memory::BLOB k1MB_ = k1K_.Repeat (1024);

            void SingleProcessLargeDataSend_ ()
            {
                Streams::MemoryStream<Byte> myStdIn{k1MB_};
                Streams::MemoryStream<Byte> myStdOut;
                ProcessRunner               pr (L"cat", myStdIn, myStdOut);
                pr.Run ();
                VerifyTestResult (myStdOut.ReadAll () == k1MB_);
            }
        }
        void DoTests ()
        {
            Private_::SingleProcessLargeDataSend_ ();
        }
    }
}

namespace {
    namespace LargeDataSentThroughPipeBackground_Tesat6_ {
        namespace Private_ {
            const Memory::BLOB k1K_  = Memory::BLOB::Raw ("0123456789abcdef").Repeat (1024 / 16);
            const Memory::BLOB k1MB_ = k1K_.Repeat (1024);

            void SingleProcessLargeDataSend_ ()
            {
                Assert (k1MB_.size () == 1024 * 1024);
                Streams::SharedMemoryStream<Byte> myStdIn; // note must use SharedMemoryStream cuz we want to distinguish EOF from no data written yet
                Streams::SharedMemoryStream<Byte> myStdOut;
                ProcessRunner                     pr (L"cat", myStdIn, myStdOut);
                ProcessRunner::BackgroundProcess  bg = pr.RunInBackground ();
                Execution::Sleep (1);
                VerifyTestResult (myStdOut.ReadSome ().IsMissing ()); // sb no data available, but NOT EOF
                myStdIn.Write (k1MB_);
                myStdIn.CloseForWrites (); // so cat process can finish
                bg.WaitForDone ();
                myStdOut.CloseForWrites (); // one process done, no more writes to this stream
                VerifyTestResult (myStdOut.ReadAll () == k1MB_);
            }
        }
        void DoTests ()
        {
            Private_::SingleProcessLargeDataSend_ ();
        }
    }
}

namespace {

    void DoRegressionTests_ ()
    {
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
        LargeDataSentThroughPipe_Tesat5_::DoTests ();
        LargeDataSentThroughPipeBackground_Tesat6_::DoTests ();
    }
}

int main (int argc, const char* argv[])
{
    Stroika::TestHarness::Setup ();
    return Stroika::TestHarness::PrintPassOrFail (DoRegressionTests_);
}
