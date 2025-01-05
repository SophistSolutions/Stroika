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
#include "Stroika/Foundation/IO/FileSystem/TemporaryFile.h"
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
        static const Iterable<String> kCmds_{"awk"_k, "bash"_k, "echo"_k, "grep"_k, "make"_k};
        //system ("echo hi mom");
        for (auto i : kCmds_) {
            if (not FindExecutableInPath (i.As<filesystem::path> ())) {
                // If running under debugger, consider adding:
                //      Visual Studio (Windows):
                //          PATH=%PATH%;C:\tools\msys64\usr\bin\;c:\tools\msys64\mingw64\bin\ (or similar)
                //          to Debugging/Environment settings for debugger
                Stroika::Frameworks::Test::WarnTestIssue ("{} not found in path ({})"_f(i, kPath ()));
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, EchoHiMom)
    {
        Debug::TraceContextBumper ctx{"EchoHiMom"}; // quickie simple test
        {
            ProcessRunner pr{"echo hi mom"};
            String        out = get<0> (pr.Run (""));
            EXPECT_EQ (out.Trim (), "hi mom");
        }
        {
            ProcessRunner pr{"echo hi mom"};
            auto [stdOutStr, stdErrStr] = pr.Run (""); // structured binding example
            EXPECT_EQ (stdOutStr.Trim (), "hi mom");
            EXPECT_EQ (stdErrStr, "");
        }
        {
            // bash should work fine on windows, as long as in path (msys or cygwin) - see earlier checks (SETUP)
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eBash, "echo hi mom"}};
            String        out = get<0> (pr.Run (""));
            EXPECT_EQ (out.Trim (), "hi mom");
        }
#if qStroika_Foundation_Common_Platform_Windows
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eWindowsCMD, "echo hi mom"}};
            String        out = get<0> (pr.Run (""));
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
            // not sure why this fails on WINDOZE?? --LGP 2024-12-07
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eBash, "echo $PATH"}};
            String        out = get<0> (pr.Run (""));
            DbgTrace ("out='{}'"_f, out.Trim ());
            EXPECT_TRUE (not out.Trim ().empty ());
        }
#endif
#if qStroika_Foundation_Common_Platform_Windows
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eWindowsCMD, "echo %PATH%"}};
            String        out = get<0> (pr.Run (""));
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
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eBash, "echo $USER"}};
            String        out = get<0> (pr.Run (""));
            DbgTrace ("out='{}'"_f, out.Trim ());
            //EXPECT_TRUE (not out.Trim ().empty ());   not always set, set by login, so if user logged in, but for test shells, maybe not? (bash -c vs -l)
        }
#if qStroika_Foundation_Common_Platform_Windows
        {
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eWindowsCMD, "echo %USERNAME%"}};
            String        out = get<0> (pr.Run (""));
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
        ProcessRunner                    pr2{"cat"};

        Streams::MemoryStream::Ptr<byte> pipe   = Streams::MemoryStream::New<byte> ();
        Streams::MemoryStream::Ptr<byte> pr2Out = Streams::MemoryStream::New<byte> ();
        pr1.Run (nullptr, pipe); // use RunInBackground to have this running WHILE p2 running
        pr2.Run (pipe, pr2Out);

        String out = String::FromUTF8 (pr2Out.As<string> ());
        EXPECT_EQ (out.Trim (), "hi mom");
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, CatMemoryBLOB2BLOB)
    {
        Debug::TraceContextBumper        ctx{"CatMemoryBLOB2BLOB"};
        ProcessRunner                    pr{"cat"};
        Memory::BLOB                     kData_{Memory::BLOB::FromRaw ("this is a test")};
        Streams::MemoryStream::Ptr<byte> processStdIn  = Streams::MemoryStream::New<byte> (kData_);
        Streams::MemoryStream::Ptr<byte> processStdOut = Streams::MemoryStream::New<byte> ();
        pr.Run (processStdIn, processStdOut);
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
                ProcessRunner                    pr{"cat"};
                pr.Run (myStdIn, myStdOut);
                EXPECT_EQ (myStdOut.ReadAll (), testBLOB);
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
    GTEST_TEST (Foundation_Execution_ProcessRunner, LargeDataSentThroughPipeBackgroundProcess)
    {
        Debug::TraceContextBumper ctx{"LargeDataSentThroughPipeBackgroundProcess"};
        const Memory::BLOB        k1K_   = Memory::BLOB::FromRaw ("0123456789abcdef").Repeat (1024 / 16);
        const Memory::BLOB        k1MB_  = k1K_.Repeat (1024);
        const Memory::BLOB        k16MB_ = k1MB_.Repeat (16);

        auto SingleProcessLargeDataSend_ = [&] () {
            Assert (k1MB_.size () == 1024 * 1024);
            Streams::SharedMemoryStream::Ptr<byte> myStdIn =
                Streams::SharedMemoryStream::New<byte> (); // note must use SharedMemoryStream cuz we want to distinguish EOF from no data written yet
            Streams::SharedMemoryStream::Ptr<byte> myStdOut = Streams::SharedMemoryStream::New<byte> ();
            ProcessRunner                          pr{"cat"};
            ProcessRunner::BackgroundProcess       bg = pr.RunInBackground (myStdIn, myStdOut);
            Execution::Sleep (1);
            EXPECT_TRUE (not myStdOut.AvailableToRead ().has_value ()); // sb no data available, but NOT EOF
            Memory::BLOB testBLOB = (Debug::IsRunningUnderValgrind () && qStroika_Foundation_Debug_AssertionsChecked) ? k1K_ : k16MB_;
            myStdIn.Write (testBLOB);
            try {
                // Test waits can fail under some circumstances
                bg.WaitForDone (1ms); // this must fail because we haven't closed stdin
                EXPECT_TRUE (false);
            }
            catch (...) {
                // good - ignore
            }
            myStdIn.CloseWrite (); // so cat process can finish
            bg.WaitForDone ();
            myStdOut.CloseWrite (); // one process done, no more writes to this stream
            EXPECT_EQ (myStdOut.ReadAll (), testBLOB);
        };

        SingleProcessLargeDataSend_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, TestFailureHandling)
    {
        Debug::TraceContextBumper ctx{"TestFailureHandling"};
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
            ProcessRunner                    pr{kCmdLine_}; // automatically translated to cmd /c or bash -c
            pr.Run (nullptr, processStdOut);
            EXPECT_EQ (Streams::TextReader::New (processStdOut).ReadAll ().Trim (), "a");
        }
        {
            ProcessRunner pr{kCmdLine_};
            auto          result = get<0> (pr.Run (""sv)); // input ignored by echo a
            EXPECT_EQ (result.Trim (), "a");
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, MakeVersionViaAwkPipe)
    {
        Debug::TraceContextBumper ctx{"MakeVersionViaAwkPipe"};
        {
            // using bash appears to work on all supported platforms
            ProcessRunner pr{CommandLine{CommandLine::WrapInShell::eBash, "\"{}\" -version | \"{}\" 'NR == 1 {{print $3}}'"_f("make"_k, "awk"_k)}};
            auto [stdOutStr, stdErrStr] = pr.Run (""sv);
            EXPECT_TRUE (not stdOutStr.empty ());
            EXPECT_TRUE (stdErrStr.empty ());
        }
        {
            // can use full path or just plain name (if in path) for make/awk
            ProcessRunner pr{"\"{}\" -version | \"{}\" 'NR == 1 {{print $3}}'"_f("make"_k, "awk"_k)};
            try {
                auto [stdOutStr, stdErrStr] = pr.Run (""sv);
                EXPECT_TRUE (not stdOutStr.empty ());
                EXPECT_TRUE (stdErrStr.empty ());
            }
            catch (...) {
                // note: awk fails on cygwin, because its run under CMD (above) and awk is a symbolic link (which cmd
                // appears to not handle properly)
                // --LGP 2024-12-27
                Stroika::Frameworks::Test::WarnTestIssue ("exception during ProcessRunner {}: {}"_f(pr.GetCommandLine (), current_exception ()));
            }
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution_ProcessRunner, MacOS_Mystery_Crasher_)
    {
        Debug::TraceContextBumper ctx{"MacOS_Mystery_Crasher_"};
        try {
            // @todo COULD add to PATH so we find the (our built) ThirdPartyComponents openssl (maybe - if we built it)
            // and use that one preferentially... - then maybe dont need bash trick...
            // --LGP 2025-01-04

            // Use ProcessRunner and external openssl to create self-signed cert
            // openssl req -new -x509 -days 365 -nodes -out cert.pem -keyout cert.pem
            IO::FileSystem::ScopedTmpFile f{"cert.pem"};
            using namespace Execution;
            String commonName = "localhost"sv;
            String company    = "MyCompany Inc."sv;
            // use bash cuz openssl not in my path, but is installed in msys (windows)
            // split path into dir/filename and only use filename cuz something getting confused about
            // finding filenames with paths (from here, not directly on commandline but PITA to debug and not the point of this test)
            ProcessRunner{CommandLine{CommandLine::WrapInShell::eBash,
                                      "openssl req -new -x509 -days 365 -nodes -out {} -keyout {}"_f(
                                          static_cast<filesystem::path> (f).filename (), static_cast<filesystem::path> (f).filename ())},
                          {.fWorkingDirectory = static_cast<filesystem::path> (f).parent_path ()}}
                .Run ("US\nMA\nSudbury\n{}\n\n{}\nlewis@sophists.com\n"_f(company, commonName));
        }
        catch (...) {
            Stroika::Frameworks::Test::WarnTestIssue ("openssl commandline tool failure (installed?): {}"_f(current_exception ()));
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
