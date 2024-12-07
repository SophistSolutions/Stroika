/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
//  TEST    Foundation::Execution::ProcessRunner
#include "Stroika/Foundation/StroikaPreComp.h"

#include <iostream>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#if qStroika_Foundation_Common_Platform_POSIX
#include "Stroika/Foundation/Execution/SignalHandlers.h"
#endif
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Streams/MemoryStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"
#include "Stroika/Foundation/Streams/TextReader.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

#if qStroika_HasComponent_googletest

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, SETUP)
    {
        //system ("echo hi mom");
        if (not FindExecutableInPath ("echo")) {
            // If running under debugger, consider adding:
            //      Visual Studio (Windows):
            //          PATH=%PATH%;C:\tools\msys64\usr\bin\;c:\tools\msys64\mingw64\bin\ (or similar)
            //          to Debugging/Environment settings for debugger
            Stroika::Frameworks::Test::WarnTestIssue ("echo not found in path");
        }
        if (not FindExecutableInPath ("grep")) {
            Stroika::Frameworks::Test::WarnTestIssue ("grep not found in path");
        }
        if (not FindExecutableInPath ("bash")) {
            Stroika::Frameworks::Test::WarnTestIssue ("bash not found in path");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, EchoHiMom)
    {
        Debug::TraceContextBumper ctx{"EchoHiMom"}; // quickie simple test
        {
            ProcessRunner pr{"echo hi mom"};
            DbgTrace ("pr.commandline={}"_f, pr.GetCommandLine ());
            String out = pr.Run ("");
            DbgTrace ("out='{}'"_f, out.Trim ());
            EXPECT_EQ (out.Trim (), "hi mom");
        }
#if qStroika_Foundation_Common_Platform_POSIX
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eBash, "echo hi mom"}};
            DbgTrace ("pr.commandline={}"_f, pr.GetCommandLine ());
            String out = pr.Run ("");
            EXPECT_EQ (out.Trim (), "hi mom");
        }
#elif qStroika_Foundation_Common_Platform_Windows
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eWindowsCMD, "echo hi mom"}};
            DbgTrace ("pr.commandline={}"_f, pr.GetCommandLine ());
            String out = pr.Run ("");
            EXPECT_EQ (out.Trim (), "hi mom");
        }
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, EchoPATH)
    {
        Debug::TraceContextBumper ctx{"EchoPATH"}; // quickie simple test
#if qStroika_Foundation_Common_Platform_POSIX
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eBash, "echo $PATH"}};
            DbgTrace ("pr.commandline={}"_f, pr.GetCommandLine ());
            String out = pr.Run ("");
            DbgTrace ("out='{}'"_f, out.Trim ());
            EXPECT_TRUE (not out.Trim ().empty ());
        }
#elif qStroika_Foundation_Common_Platform_Windows
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eWindowsCMD, "echo %PATH%"}};
            DbgTrace ("pr.commandline={}"_f, pr.GetCommandLine ());
            String out = pr.Run ("");
            DbgTrace ("out='{}'"_f, out.Trim ());
            EXPECT_TRUE (not out.Trim ().empty ());
        }
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, EchoUSER)
    {
        Debug::TraceContextBumper ctx{"EchoUSER"}; // quickie simple test
#if qStroika_Foundation_Common_Platform_POSIX
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eBash, "echo $USER"}};
            DbgTrace ("pr.commandline={}"_f, pr.GetCommandLine ());
            String out = pr.Run ("");
            DbgTrace ("out='{}'"_f, out.Trim ());
            EXPECT_TRUE (not out.Trim ().empty ());
        }
#elif qStroika_Foundation_Common_Platform_Windows
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eWindowsCMD, "echo %USERNAME%"}};
            DbgTrace ("pr.commandline={}"_f, pr.GetCommandLine ());
            String out = pr.Run ("");
            DbgTrace ("out='{}'"_f, out.Trim ());
            EXPECT_TRUE (not out.Trim ().empty ());
        }
#endif
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, EchoHiMomThroughIntraStroikaPipe)
    {
        Debug::TraceContextBumper        ctx{"EchoHiMomThroughIntraStroikaPipe"};
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
        EXPECT_EQ (out.Trim (), "hi mom");
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, CatMemoryBLOB2BLOB)
    {
        Debug::TraceContextBumper        ctx{"CatMemoryBLOB2BLOB"};
        Memory::BLOB                     kData_{Memory::BLOB::FromRaw ("this is a test")};
        Streams::MemoryStream::Ptr<byte> processStdIn  = Streams::MemoryStream::New<byte> (kData_);
        Streams::MemoryStream::Ptr<byte> processStdOut = Streams::MemoryStream::New<byte> ();
        ProcessRunner                    pr{"cat", processStdIn, processStdOut};
        pr.Run ();
        EXPECT_EQ (processStdOut.ReadAll (), kData_);
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
                Memory::BLOB testBLOB = (Debug::IsRunningUnderValgrind () && qStroika_Foundation_Debug_AssertionsChecked) ? k1K_ : k16MB_;
                Streams::MemoryStream::Ptr<byte> myStdIn  = Streams::MemoryStream::New<byte> (testBLOB);
                Streams::MemoryStream::Ptr<byte> myStdOut = Streams::MemoryStream::New<byte> ();
                ProcessRunner                    pr{"cat", myStdIn, myStdOut};
                pr.Run ();
                EXPECT_TRUE (myStdOut.ReadAll () == testBLOB);
            }
        }
    }
    GTEST_TEST (Foundation_Execution_ProcessRunner, LargeDataSentThroughPipe)
    {
        Debug::TraceContextBumper ctx{"LargeDataSentThroughPipe"};
        LargeDataSentThroughPipe_Test5_::Private_::SingleProcessLargeDataSend_ ();
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
                EXPECT_TRUE (not myStdOut.AvailableToRead ().has_value ()); // sb no data available, but NOT EOF
                Memory::BLOB testBLOB = (Debug::IsRunningUnderValgrind () && qStroika_Foundation_Debug_AssertionsChecked) ? k1K_ : k16MB_;
                myStdIn.Write (testBLOB);
                myStdIn.CloseWrite (); // so cat process can finish
                bg.WaitForDone ();
                myStdOut.CloseWrite (); // one process done, no more writes to this stream
                EXPECT_EQ (myStdOut.ReadAll (), testBLOB);
            }
        }
    }
    GTEST_TEST (Foundation_Execution_ProcessRunner, LargeDataSentThroughPipeBackgroundProcess)
    {
        Debug::TraceContextBumper ctx{"LargeDataSentThroughPipeBackgroundProcess"};
        LargeDataSentThroughPipeBackground_Test6_::Private_::SingleProcessLargeDataSend_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, TestFailureHanlding)
    {
        Debug::TraceContextBumper ctx{"TestFailureHanlding"};
        try {
            ProcessRunner pr{"mount /fasdkfjasdfjasdkfjasdklfjasldkfjasdfkj /dadsf/a/sdf/asdf//"};
            pr.Run ();
            EXPECT_TRUE (false);
        }
        catch (...) {
            DbgTrace ("got failure msg: {}"_f, current_exception ());
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, AutomaticWrapInBashOrCmdShellForPipesInShell)
    {
        Debug::TraceContextBumper ctx{"AutomaticWrapInBashOrCmdShellForPipesInShell"};
        const String              kCmdLine_ = "echo a | grep a"sv;
        {
            Streams::MemoryStream::Ptr<byte> processStdOut = Streams::MemoryStream::New<byte> ();
            ProcessRunner                    pr{kCmdLine_, nullptr, processStdOut}; // automatically translated to cmd /c or bash -c
            DbgTrace ("pr.CommandLine = {}"_f, pr.GetCommandLine ());
            pr.Run ();
            EXPECT_EQ (Streams::TextReader::New (processStdOut).ReadAll ().Trim (), "a");
        }
        {
            ProcessRunner pr{kCmdLine_};
            auto          result = pr.Run (""sv); // input ignored by echo a
            EXPECT_EQ (result.Trim (), "a");
        }
    }
}

#endif

int main (int argc, const char* argv[])
{
    Test::Setup (argc, argv);

#if qStroika_Foundation_Common_Platform_POSIX
    // Many tests use pipes
    // @todo - REVIEW IF REALLY NEEDED AND WHY? SO LONG AS NO FAIL SHOULDNT BE?
    //  --LGP 2014-02-05
    Execution::SignalHandlerRegistry::Get ().SetSignalHandlers (SIGPIPE, Execution::SignalHandlerRegistry::kIGNORED);
#endif

#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "Stroika regression tests require building with google test feature [  PASSED  ]" << endl;
#endif
}
