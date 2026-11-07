/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
//  TEST    Foundation::Execution::Other
#include "Stroika/Foundation/StroikaPreComp.h"

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <optional>
#include <thread>

#include "Stroika/Foundation/Common/Locale.h"
#include "Stroika/Foundation/Common/SystemConfiguration.h"
#include "Stroika/Foundation/DataExchange/ObjectVariantMapper.h"
#include "Stroika/Foundation/DataExchange/OptionsFile.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"
#include "Stroika/Foundation/Debug/Visualizations.h"
#include "Stroika/Foundation/Execution/Async.h"
#include "Stroika/Foundation/Execution/CPUAffinity.h"
#include "Stroika/Foundation/Execution/CallbackRegistry.h"
#include "Stroika/Foundation/Execution/CommandLine.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Function.h"
#include "Stroika/Foundation/Execution/IntervalTimer.h"
#include "Stroika/Foundation/Execution/LazyInitialized.h"
#include "Stroika/Foundation/Execution/Logger.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/Execution/ModuleGetterSetter.h"
#include "Stroika/Foundation/Execution/ProcessRunner.h"
#include "Stroika/Foundation/Execution/Sleep.h"
#include "Stroika/Foundation/Execution/Thread.h"
#include "Stroika/Foundation/Execution/WaitableEvent.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"
#include "Stroika/Foundation/Time/DateTime.h"
#include "Stroika/Foundation/Time/Duration.h"

#include "Stroika/Frameworks/Test/TestHarness.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;

using namespace Stroika::Frameworks;

namespace {
    // given kArgumentsChild_ then kNotInCodePage_, main runs as CommandLineFromMain_'s child (ArgumentsChild_). No narrow
    // code page but UTF-8 holds that argument whole - and Windows gives main its arguments converted to the code page
    constexpr string_view kArgumentsChild_ = "--Stroika-Tests-CommandLine-Child"sv;
    const String          kNotInCodePage_{u"Zo\u00EB-\u03A9-\U0001F600"};
}

#if qStroika_HasComponent_googletest
// must be tested before main, so cannot call directly below
namespace {
    int TestAtomicInitializedCoorectly_ ();
    static int sIgnoredTestValue_ = TestAtomicInitializedCoorectly_ (); // if using static constructors, this will be called before sAtomicBoolNotInitializedTilAfterStaticInitizers_

    atomic<bool> sAtomicBoolNotInitializedTilAfterStaticInitizers_{true}; // for calls before start of or after end of main ()
    int          TestAtomicInitializedCoorectly_ ()
    {
        EXPECT_TRUE (sAtomicBoolNotInitializedTilAfterStaticInitizers_);
        return 1;
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, AsyncRunAll_)
    {
        Debug::TraceContextBumper ctx{"AsyncRunAll_"};
#if !qCompilerAndStdLib_tsubst_pack_expansion_Buggy
        {
            auto results = RunAll ([] () { return 1; }, [] () { return 2; }, [] () { return 3; });
            EXPECT_EQ (results, make_tuple (1, 2, 3));
        }
        {
            tuple<int> results = RunAll ([] () -> void {}, [] () { return 3; });
            EXPECT_EQ (results, make_tuple (3));
        }
        {
            int a = 0;
            int b = 1;
            int c = 2;
            RunAll ([&] () { a = 3; }, [&] () { b = 4; }, [&] () { c = 5; });
            EXPECT_EQ (a, 3);
            EXPECT_EQ (b, 4);
            EXPECT_EQ (c, 5);
        }
        {
            static const auto kExcept_ = Execution::Exception{"Test exception"sv};
            auto              thrower  = [] () { Execution::Throw (kExcept_); };
            EXPECT_THROW (RunAll (thrower, [] () { return 3; }), Execution::Exception<>);
        }
#else
        GTEST_SKIP () << "RunAll not tested with this compiler (qCompilerAndStdLib_tsubst_pack_expansion_Buggy)";
#endif
    }
}

namespace {
    // Execution::Function is deprecated (since Stroika v3.0d25) - and tested until it goes
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    GTEST_TEST (Foundation_Execution, Function_)
    {
        Debug::TraceContextBumper ctx{"Function_"};
        // Make sure Function<> works as well as std::function
        {
            Function<int (bool)> f = [] ([[maybe_unused]] bool b) -> int { return 3; };
            EXPECT_TRUE (f (true) == 3);
            function<int (bool)> ff = f;
            EXPECT_TRUE (ff (true) == 3);
        }
        // Make sure Function<> serves its one purpose - being comparable
        {
            Function<int (bool)> f1 = [] ([[maybe_unused]] bool b) -> int { return 3; };
            Function<int (bool)> f2 = [] ([[maybe_unused]] bool b) -> int { return 3; };

            EXPECT_TRUE (f1 != f2);
            EXPECT_TRUE (f1 < f2 or f2 < f1); // SEE qCompilerAndStdLib_SpaceshipOperator_x86_Optimizer_Sometimes_Buggy
            Function<int (bool)> f3 = f1;
            EXPECT_TRUE (f3 == f1);
            EXPECT_TRUE (f3 != f2);
        }
        {
            // https://github.com/SophistSolutions/Stroika/issues/1092 (STK-960)
            // In WTF, really in Execution::IntervalTime code - was getting two functions added with same function pointer.
            // Workaround for https://github.com/SophistSolutions/Stroika/issues/1092 (STK-960) addresses that. But not sure why this doesn't trigger
            // with old code?
            Function<int ()> f1 = [] () { return 1; };
            Function<int ()> f2 = [] () { return -1; };
            EXPECT_TRUE (f1 != f2);
        }
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
}

namespace {
    GTEST_TEST (Foundation_Execution, CommandLine_)
    {
        Debug::TraceContextBumper ctx{"CommandLine_"};
#if qStroika_Platform_POSIX
        {
            // argv in the "C" locale - every program's until it installs another - is UTF-8, as file names are (#1266: it
            // threw on anything but ASCII, often on main's first line)
            Common::ScopedUseLocale useC{locale::classic ()};
            const char*             argv[] = {"prog", "/photos/Zo\xC3\xAB"};
            Sequence<String>        l      = CommandLine{2, argv}.GetArguments ();
            EXPECT_EQ (l.size (), 2u);
            EXPECT_EQ (l[1], String{u"/photos/Zo\u00EB"});
        }
#endif
        {
            String           cmdLine = "/bin/sh -c \"a b c\"";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_TRUE (l.size () == 3);
            EXPECT_TRUE (l[0] == "/bin/sh");
            EXPECT_TRUE (l[1] == "-c");
            EXPECT_TRUE (l[2] == "a b c");
        }
        {
            String           cmdLine = "";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_TRUE (l.size () == 0);
        }
        {
            String           cmdLine = "/bin/sh -c \'a b c\'";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_TRUE (l.size () == 3);
            EXPECT_TRUE (l[0] == "/bin/sh");
            EXPECT_TRUE (l[1] == "-c");
            EXPECT_TRUE (l[2] == "a b c");
        }
        {
            String           cmdLine = "/bin/sh\t b c     -d";
            Sequence<String> l       = CommandLine{cmdLine}.GetArguments ();
            EXPECT_EQ (l.size (), 4u);
            EXPECT_EQ (l[0], "/bin/sh");
            EXPECT_EQ (l[1], "b");
            EXPECT_EQ (l[2], "c");
            EXPECT_EQ (l[3], "-d");
        }

        {
            const CommandLine::Option kMongoConnectionStringOpt_{.fLongName = "mongoConnectionString"sv, .fSupportsArgument = true};
            EXPECT_EQ (CommandLine{"test --mongoConnectionString b"}.ApplyOptions ({kMongoConnectionStringOpt_}).GetArgument (kMongoConnectionStringOpt_),
                       "b"sv);
            // a long name matches whole: neither a shorter nor a longer one is that option
            EXPECT_FALSE (CommandLine{"test --mongoXXX b"}.ApplyOptionsQuietly ({kMongoConnectionStringOpt_}).has_value ());
            EXPECT_FALSE (CommandLine{"test --mongoConnectionStringXXX b"}.ApplyOptionsQuietly ({kMongoConnectionStringOpt_}).has_value ());
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, CommandLineApplyOptions_)
    {
        Debug::TraceContextBumper ctx{"CommandLineApplyOptions_"};
        using StandardCommandLineOptions::kHelp;
        using StandardCommandLineOptions::kVersion;
        auto fails = [] (const String& cmdLine, const Iterable<CommandLine::Option>& options) {
            return not CommandLine{cmdLine}.ApplyOptionsQuietly (options).has_value ();
        };
        {
            // ArchiveUtility's options: an option's separate-word argument is not a positional word (before Stroika v3.0d26,
            // GetArgument (kArchiveO) of this command line gave xdir)
            const CommandLine::Option                   kOutDirO{.fLongName = "outputDirectory"sv, .fSupportsArgument = true};
            const CommandLine::Option                   kListO{.fLongName = "list"sv};
            const CommandLine::Option                   kArchiveO{.fRequired = true, .fHelpArgName = "ARCHIVE-FILE"sv};
            const CommandLine::Option                   kFilesO{.fRepeatable = true, .fHelpArgName = "FILE"sv};
            const initializer_list<CommandLine::Option> kAll{kHelp, kListO, kOutDirO, kArchiveO, kFilesO};
            for (const String& cl : {"a --outputDirectory xdir --list foo.7z f1 f2"_k, "a --outputDirectory=xdir --list foo.7z f1 f2"_k,
                                     "a foo.7z --list f1 --outputDirectory xdir f2"_k}) {
                CommandLine::OptionValues args = CommandLine{cl}.ApplyOptions (kAll);
                EXPECT_EQ (args.GetArgument (kOutDirO), "xdir"sv);
                EXPECT_TRUE (args.Has (kListO));
                EXPECT_FALSE (args.Has (kHelp));
                EXPECT_EQ (args.GetArgument (kArchiveO), "foo.7z"sv);
                EXPECT_EQ (args.GetArguments (kFilesO), (Sequence<String>{"f1"_k, "f2"_k}));
                EXPECT_EQ (args.GetArgument (kFilesO), "f1"sv);
            }
            CommandLine::OptionValues noFiles = CommandLine{"a --list foo.7z"}.ApplyOptions (kAll);
            EXPECT_FALSE (noFiles.Has (kFilesO));
            EXPECT_EQ (noFiles.GetArguments (kFilesO), Sequence<String>{});
            EXPECT_EQ (noFiles.GetArgument (kOutDirO), nullopt);
            EXPECT_EQ (noFiles.GetArgument (kListO), nullopt); // given, but takes no argument

            // the usage line: [X] optional, X required, ... repeatable
            String usage = CommandLine::GenerateUsage ("a"sv, kAll);
            EXPECT_TRUE (usage.StartsWith ("Usage: a [-h|--help] [--list] [--outputDirectory=ARG] ARCHIVE-FILE [FILE]...\n"sv)) << usage;
        }
        {
            // HTMLViewCompiler's options: two required positional options (before Stroika v3.0d26, the second was never found)
            const CommandLine::Option                   kNameO{.fSingleCharName = 'n', .fSupportsArgument = true};
            const CommandLine::Option                   kInO{.fRequired = true, .fHelpArgName = "IN"sv};
            const CommandLine::Option                   kOutO{.fRequired = true, .fHelpArgName = "OUT"sv};
            const initializer_list<CommandLine::Option> kAll{kHelp, kVersion, kNameO, kInO, kOutO};
            for (const String& cl : {"x in.swsp out.cpp"_k, "x -n G in.swsp out.cpp"_k, "x in.swsp -n G out.cpp"_k, "x in.swsp out.cpp -n G"_k}) {
                CommandLine::OptionValues args = CommandLine{cl}.ApplyOptions (kAll);
                EXPECT_EQ (args.GetArgument (kInO), "in.swsp"sv);
                EXPECT_EQ (args.GetArgument (kOutO), "out.cpp"sv);
            }
            EXPECT_TRUE (fails ("x in.swsp", kAll));                   // OUT is required
            EXPECT_TRUE (fails ("x in.swsp out.cpp more", kAll));      // a positional word with no positional option left to take it
            EXPECT_TRUE (fails ("x in.swsp out.cpp -n", kAll));        // -n's argument missing
            EXPECT_TRUE (fails ("x -n A -n B in.swsp out.cpp", kAll)); // -n is not repeatable
            EXPECT_TRUE (fails ("x -q in.swsp out.cpp", kAll));        // no option -q
            {
                auto r = CommandLine{"x in.swsp"}.ApplyOptionsQuietly (kAll);
                ASSERT_FALSE (r.has_value ());
                EXPECT_TRUE (Characters::ToString (r.error ()).Contains ("OUT"sv)) << Characters::ToString (r.error ());
            }
            {
                auto r = CommandLine{"x in.swsp out.cpp -n"}.ApplyOptionsQuietly (kAll);
                ASSERT_FALSE (r.has_value ());
                EXPECT_TRUE (Characters::ToString (r.error ()).Contains (" -n "sv)) << Characters::ToString (r.error ()); // names the option
            }

            // --help and --version work with a required option missing (kHelp and kVersion fWaivesRequired) - but an argument
            // naming no option is still an error
            for (const String& cl : {"x --help"_k, "x -h"_k, "x in.swsp --help"_k}) {
                CommandLine::OptionValues args = CommandLine{cl}.ApplyOptions (kAll);
                EXPECT_TRUE (args.Has (kHelp));
                EXPECT_FALSE (args.Has (kOutO));
            }
            EXPECT_TRUE (CommandLine{"x --version"}.ApplyOptions (kAll).Has (kVersion));
            EXPECT_TRUE (fails ("x --help --bogus", kAll));
        }
        {
            const CommandLine::Option kFlagO{.fLongName = "flag"sv};
            const CommandLine::Option kRunO{.fSingleCharName = 'r', .fLongName = "run"sv, .fSupportsArgument = true, .fRepeatable = true};
            const CommandLine::Option kMaybeO{.fLongName = "maybe"sv, .fSupportsArgument = true, .fIfSupportsArgumentThenRequired = false};
            const initializer_list<CommandLine::Option> kAll{kFlagO, kRunO, kMaybeO};
            CommandLine::OptionValues args = CommandLine{"x -r a --run=b --RUN c --flag"}.ApplyOptions (kAll); // long names: case-insensitive by default
            EXPECT_EQ (args.GetArguments (kRunO), (Sequence<String>{"a"_k, "b"_k, "c"_k}));
            EXPECT_TRUE (args.Has (kFlagO));
            EXPECT_FALSE (args.Has (kMaybeO));
            EXPECT_TRUE (fails ("x --flag=yes", kAll)); // --flag takes no argument
            EXPECT_TRUE (fails ("x --flag --flag", kAll));
            EXPECT_TRUE (fails ("x -r", kAll));
            EXPECT_TRUE (fails ("x positional", kAll)); // no positional option at all
            CommandLine::OptionValues maybe = CommandLine{"x --maybe"}.ApplyOptions (kAll);
            EXPECT_TRUE (maybe.Has (kMaybeO));
            EXPECT_EQ (maybe.GetArgument (kMaybeO), nullopt);
            EXPECT_EQ (CommandLine{"x --maybe=v"}.ApplyOptions (kAll).GetArgument (kMaybeO), "v"sv);
            // an optional argument is given only after = (as getopt): the next word is never taken as it
            EXPECT_TRUE (fails ("x --maybe v", kAll)); // v is positional, and there is no positional option
            EXPECT_TRUE (CommandLine{"x --maybe --flag"}.ApplyOptions (kAll).Has (kFlagO));
            String usage = CommandLine::GenerateUsage ("x"sv, kAll);
            EXPECT_TRUE (usage.StartsWith ("Usage: x [--flag] [-r ARG|--run=ARG]... [--maybe[=ARG]]\n"sv)) << usage;
        }
        {
            // GenerateUsage: the usage line, then a line for each option with help text - its names and argument, as GNU and
            // argparse show them - the text aligned
            const CommandLine::Option kNameO{
                .fSingleCharName = 'n', .fLongName = "name"sv, .fSupportsArgument = true, .fHelpArgName = "NAME"sv, .fHelpOptionText = "the name"sv};
            const CommandLine::Option kRunO{.fSingleCharName = 'r', .fSupportsArgument = true, .fRepeatable = true, .fHelpOptionText = "run it"sv};
            const CommandLine::Option kMaybeO{
                .fLongName = "maybe"sv, .fSupportsArgument = true, .fIfSupportsArgumentThenRequired = false, .fHelpOptionText = "perhaps"sv};
            const CommandLine::Option kQuietO{.fLongName = "quiet"sv};
            const CommandLine::Option kFileO{.fRequired = true, .fHelpArgName = "FILE"sv, .fHelpOptionText = "the file"sv};
            EXPECT_EQ (CommandLine::GenerateUsage ("x"sv, {kHelp, kNameO, kRunO, kMaybeO, kQuietO, kFileO}),
                       "Usage: x [-h|--help] [-n NAME|--name=NAME] [-r ARG]... [--maybe[=ARG]] [--quiet] FILE\n"
                       "  -h, --help         Print out this help.\n"
                       "  -n, --name=NAME    the name\n"
                       "  -r ARG             run it\n"
                       "  --maybe[=ARG]      perhaps\n"
                       "  FILE               the file\n"sv);
        }
        {
            // a bare -- ends the options: every word after it is positional (a second -- too), so a file can be named -f
            const CommandLine::Option                   kFlagO{.fSingleCharName = 'f'};
            const CommandLine::Option                   kFilesO{.fRepeatable = true, .fHelpArgName = "FILE"sv};
            const initializer_list<CommandLine::Option> kAll{kFlagO, kFilesO};
            CommandLine::OptionValues                   args = CommandLine{"x -f a -- -f -- b"}.ApplyOptions (kAll);
            EXPECT_TRUE (args.Has (kFlagO));
            EXPECT_EQ (args.GetArguments (kFilesO), (Sequence<String>{"a"_k, "-f"_k, "--"_k, "b"_k}));
            EXPECT_FALSE (CommandLine{"x --"}.ApplyOptions (kAll).Has (kFilesO));
            EXPECT_TRUE (fails ("x -- -f", {kFlagO})); // -f after -- is positional, and there is no positional option
        }
        {
            // a bare - is a positional word (eg stdin), and a word after an option that takes an argument is that argument,
            // whatever it looks like
            const CommandLine::Option                   kOutO{.fSingleCharName = 'o', .fSupportsArgument = true};
            const CommandLine::Option                   kFileO{.fHelpArgName = "FILE"sv};
            const initializer_list<CommandLine::Option> kAll{kOutO, kFileO};
            CommandLine::OptionValues                   args = CommandLine{"x -o -weird -"}.ApplyOptions (kAll);
            EXPECT_EQ (args.GetArgument (kOutO), "-weird"sv);
            EXPECT_EQ (args.GetArgument (kFileO), "-"sv);
            EXPECT_EQ (CommandLine{"x -o -- f"}.ApplyOptions (kAll).GetArgument (kOutO), "--"sv); // an option's argument, even --
        }
        {
            // GetArgument<T> reads the argument as a T - and throws InvalidCommandLineArgument, naming the option, if it is not one
            const CommandLine::Option                   kPortO{.fSingleCharName = 'p', .fLongName = "port"sv, .fSupportsArgument = true};
            const CommandLine::Option                   kRunForO{.fLongName = "run-for"sv, .fSupportsArgument = true};
            const CommandLine::Option                   kScaleO{.fLongName = "scale"sv, .fSupportsArgument = true, .fRepeatable = true};
            const CommandLine::Option                   kFileO{.fHelpArgName = "FILE"sv};
            const initializer_list<CommandLine::Option> kAll{kPortO, kRunForO, kScaleO, kFileO};
            CommandLine::OptionValues args = CommandLine{"x --port 8080 --run-for PT1M30S --scale 1.5 --scale=-2 a.txt"}.ApplyOptions (kAll);
            EXPECT_EQ (args.GetArgument<uint16_t> (kPortO), 8080);
            EXPECT_EQ (args.GetArgument<Time::Duration> (kRunForO), Time::Duration{90.0});
            EXPECT_EQ (args.GetArguments<double> (kScaleO), (Sequence<double>{1.5, -2.0}));
            EXPECT_EQ (args.GetArgument<filesystem::path> (kFileO), filesystem::path{"a.txt"});
            EXPECT_EQ (args.GetArgument<String> (kFileO), "a.txt"sv);
            EXPECT_EQ (CommandLine{"x"}.ApplyOptions (kAll).GetArgument<uint16_t> (kPortO), nullopt);
            EXPECT_EQ (CommandLine{"x"}.ApplyOptions (kAll).GetArguments<double> (kScaleO), Sequence<double>{});
            // what read throws, as "message|argument"
            auto failure = [&] (const String& cmdLine, const function<void (const CommandLine::OptionValues&)>& read) -> String {
                try {
                    read (CommandLine{cmdLine}.ApplyOptions (kAll));
                }
                catch (const InvalidCommandLineArgument& e) {
                    return e.As<String> () + "|"sv + e.fArgument;
                }
                return "(nothing thrown)"sv;
            };
            auto port = [&] (const CommandLine::OptionValues& a) { a.GetArgument<uint16_t> (kPortO); };
            EXPECT_EQ (failure ("x --port abc", port), "Command line argument (-p|--port): \"abc\" is not an integer|abc"sv);
            EXPECT_EQ (failure ("x -p 80x", port), "Command line argument (-p|--port): \"80x\" is not an integer|80x"sv); // the whole word
            EXPECT_EQ (failure ("x --port 70000", port), "Command line argument (-p|--port): \"70000\" is out of range (0 to 65535)|70000"sv);
            EXPECT_EQ (failure ("x --port -1", port), "Command line argument (-p|--port): \"-1\" is out of range (0 to 65535)|-1"sv);
            auto runFor = [&] (const CommandLine::OptionValues& a) { a.GetArgument<Time::Duration> (kRunForO); };
            EXPECT_EQ (failure ("x --run-for 10", runFor), "Command line argument --run-for: \"10\" is not an ISO-8601 duration (as PT10S)|10"sv);
            EXPECT_EQ (failure ("x --run-for=", runFor), "Command line argument --run-for: \"\" is not an ISO-8601 duration (as PT10S)|"sv);
            EXPECT_EQ (failure ("x --scale 1 --scale z", [&] (const CommandLine::OptionValues& a) { a.GetArguments<double> (kScaleO); }),
                       "Command line argument --scale: \"z\" is not a number|z"sv);
            EXPECT_EQ (failure ("x 12z", [&] (const CommandLine::OptionValues& a) { a.GetArgument<int> (kFileO); }),
                       "Command line argument FILE: \"12z\" is not an integer|12z"sv);
        }
        {
            // an option not given takes its argument from its fEnvironmentVariable, if set - PATH is, wherever the tests run
            const String              kUnset = "STROIKA_TEST42_SURELY_NOT_SET"sv;
            const CommandLine::Option kPathO{.fLongName = "path"sv, .fSupportsArgument = true, .fEnvironmentVariable = "PATH"sv};
            const CommandLine::Option kUnsetO{.fLongName = "unset"sv, .fSupportsArgument = true, .fEnvironmentVariable = kUnset};
            const initializer_list<CommandLine::Option> kAll{kPathO, kUnsetO};
            ASSERT_TRUE (kEnvironment->Lookup ("PATH"sv).has_value ());
            ASSERT_FALSE (kEnvironment->Lookup (kUnset).has_value ());
            CommandLine::OptionValues args = CommandLine{"x"}.ApplyOptions (kAll);
            EXPECT_TRUE (args.Has (kPathO));
            EXPECT_EQ (args.GetArgument (kPathO), kEnvironment->Lookup ("PATH"sv));
            EXPECT_FALSE (args.Has (kUnsetO));
            EXPECT_EQ (CommandLine{"x --path=here"}.ApplyOptions (kAll).GetArgument (kPathO), "here"sv); // the command line wins
            // it satisfies fRequired; and when it is not set, the error names it
            const CommandLine::Option kRequiredPathO{.fLongName = "path"sv, .fSupportsArgument = true, .fRequired = true, .fEnvironmentVariable = "PATH"sv};
            EXPECT_TRUE (CommandLine{"x"}.ApplyOptionsQuietly ({kRequiredPathO}).has_value ());
            const CommandLine::Option kRequiredUnsetO{.fLongName = "unset"sv, .fSupportsArgument = true, .fRequired = true, .fEnvironmentVariable = kUnset};
            EXPECT_EQ (CommandLine{"x"}.ApplyOptionsQuietly ({kRequiredUnsetO}).error ().As<String> (),
                       "Required command line argument --unset=ARG was not provided, nor environment variable STROIKA_TEST42_SURELY_NOT_SET set"sv);
            // its value not a T: the error names the variable
            try {
                (void)args.GetArgument<int> (kPathO);
                ADD_FAILURE () << "no exception";
            }
            catch (const InvalidCommandLineArgument& e) {
                EXPECT_TRUE (e.As<String> ().StartsWith ("Environment variable PATH (for --path): "sv)) << e.As<String> ();
            }
            // and the help line shows the variable
            EXPECT_TRUE (CommandLine::GenerateUsage ("x"sv, kAll).Contains ("  --unset=ARG    [env: STROIKA_TEST42_SURELY_NOT_SET]\n"sv))
                << CommandLine::GenerateUsage ("x"sv, kAll);
        }
        {
            // an option not given (nor its fEnvironmentVariable set) takes its fDefaultArgument - and GetArgumentValue reads an
            // option that must have an argument (it has a default, or is fRequired) as the T itself
            const CommandLine::Option kPortO{.fLongName = "port"sv, .fSupportsArgument = true, .fDefaultArgument = "8080"sv, .fHelpOptionText = "listen port"sv};
            const CommandLine::Option kPathO{
                .fLongName = "path"sv, .fSupportsArgument = true, .fEnvironmentVariable = "PATH"sv, .fDefaultArgument = "nowhere"sv};
            const CommandLine::Option                   kFileO{.fRequired = true, .fHelpArgName = "FILE"sv};
            const initializer_list<CommandLine::Option> kAll{kPortO, kPathO, kFileO};
            CommandLine::OptionValues                   args = CommandLine{"x a.txt"}.ApplyOptions (kAll);
            EXPECT_EQ (args.GetArgumentValue<uint16_t> (kPortO), 8080);
            EXPECT_FALSE (args.Has (kPortO)); // not given: the program can tell
            EXPECT_EQ (CommandLine{"x --port 80 a.txt"}.ApplyOptions (kAll).GetArgumentValue<uint16_t> (kPortO), 80);
            EXPECT_EQ (args.GetArgument (kPathO), kEnvironment->Lookup ("PATH"sv)); // the environment variable comes before the default
            EXPECT_EQ (args.GetArgumentValue<filesystem::path> (kFileO), filesystem::path{"a.txt"});
            // a default that is not a T is named as one
            const CommandLine::Option kBadO{.fLongName = "bad"sv, .fSupportsArgument = true, .fDefaultArgument = "80x"sv};
            try {
                (void)CommandLine{"x"}.ApplyOptions ({kBadO}).GetArgumentValue<int> (kBadO);
                ADD_FAILURE () << "no exception";
            }
            catch (const InvalidCommandLineArgument& e) {
                EXPECT_EQ (e.As<String> (), "Default argument for --bad: \"80x\" is not an integer"sv);
            }
            // and the help line shows it, after any environment variable
            String usage = CommandLine::GenerateUsage ("x"sv, kAll);
            EXPECT_TRUE (usage.Contains ("  --port=ARG    listen port [default: 8080]\n"sv)) << usage;
            EXPECT_TRUE (usage.Contains ("  --path=ARG    [env: PATH] [default: nowhere]\n"sv)) << usage;
        }
        {
            // an option with fAllowedValues takes only those words - from the command line or its fEnvironmentVariable - each
            // read as listed (matched ignoring case, unless fAllowedValuesCaseSensitive says otherwise)
            const CommandLine::Option kSwitchO{
                .fLongName = "switch"sv, .fSupportsArgument = true, .fAllowedValues = {"on"sv, "off"sv}, .fHelpOptionText = "the light"sv};
            const CommandLine::Option kModeO{.fLongName                   = "mode"sv,
                                             .fSupportsArgument           = true,
                                             .fDefaultArgument            = "a"sv,
                                             .fAllowedValues              = {"A"sv, "a"sv},
                                             .fAllowedValuesCaseSensitive = eWithCase};
            const CommandLine::Option kLevelO{.fAllowedValues = {"error"sv, "warning"sv, "info"sv}, .fHelpArgName = "LEVEL"sv};
            const initializer_list<CommandLine::Option> kAll{kSwitchO, kModeO, kLevelO};
            CommandLine::OptionValues                   args = CommandLine{"x --switch=OFF WARNING"}.ApplyOptions (kAll);
            EXPECT_EQ (args.GetArgument (kSwitchO), "off"sv); // as listed
            EXPECT_EQ (args.GetArgument (kLevelO), "warning"sv);
            EXPECT_EQ (args.GetArgument (kModeO), "a"sv);                                          // the default
            EXPECT_EQ (CommandLine{"x --mode A"}.ApplyOptions (kAll).GetArgument (kModeO), "A"sv); // eWithCase: A is not a
            EXPECT_FALSE (CommandLine{"x"}.ApplyOptions (kAll).Has (kSwitchO));
            // any other word is refused, naming those allowed - as "message|argument"
            auto error = [] (const String& cmdLine, const Iterable<CommandLine::Option>& options) -> String {
                auto r = CommandLine{cmdLine}.ApplyOptionsQuietly (options);
                if (r) {
                    return "(no error)"sv;
                }
                return r.error ().As<String> () + "|"sv + r.error ().fArgument;
            };
            EXPECT_EQ (error ("x --switch maybe", kAll), "Command line argument --switch: \"maybe\" is not one of: on, off|maybe"sv);
            EXPECT_EQ (error ("x --mode B", kAll), "Command line argument --mode: \"B\" is not one of: A, a|B"sv);
            EXPECT_EQ (error ("x verbose", kAll), "Command line argument LEVEL: \"verbose\" is not one of: error, warning, info|verbose"sv);
            // an environment variable's too - PATH is set wherever the tests run, and never to x
            const CommandLine::Option kPathO{
                .fLongName = "path"sv, .fSupportsArgument = true, .fEnvironmentVariable = "PATH"sv, .fAllowedValues = {"x"sv}};
            EXPECT_EQ (CommandLine{"x --path X"}.ApplyOptions ({kPathO}).GetArgument (kPathO), "x"sv);
            EXPECT_TRUE (error ("x", {kPathO}).StartsWith ("Environment variable PATH (for --path): \""sv)) << error ("x", {kPathO});
            // and the help line lists them, after any default
            EXPECT_EQ (CommandLine::GenerateUsage ("x"sv, kAll), "Usage: x [--switch=ARG] [--mode=ARG] [LEVEL]\n"
                                                                 "  --switch=ARG    the light [possible values: on, off]\n"
                                                                 "  --mode=ARG      [default: a] [possible values: A, a]\n"
                                                                 "  LEVEL           [possible values: error, warning, info]\n"sv);
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, CommandLineFromMain_)
    {
        Debug::TraceContextBumper ctx{"CommandLineFromMain_"};
        // main's own arguments reach a CommandLine whole (#1267): so run this program again, with an argument no narrow code
        // page holds - its child exits 0 only if it got that argument
        ProcessRunner pr{CommandLine{Sequence<String>{String{GetEXEPath ()}, String{kArgumentsChild_}, kNotInCodePage_}}};
        EXPECT_NO_THROW (pr.Run ());
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, IsAddressReadOnlyAppLifetime_)
    {
        Debug::TraceContextBumper ctx{"IsAddressReadOnlyAppLifetime_"};
        static const char         kStaticConst[] = "static const";
        static char               sWritable[]    = "writable static";
        char                      onStack[]      = "on the stack";
        const string              onHeap (100, 'x');
#if qCompilerAndStdLib_span_requires_explicit_type_for_BLOBCVT_Buggy
        auto bytes = [] (string_view s) { return as_bytes (span<const char>{s}); };
#else
        auto bytes = [] (string_view s) { return as_bytes (span{s}); };
#endif
        optional<bool> literal = IsAddressReadOnlyAppLifetime (bytes ("a literal"sv));
        if (literal == nullopt) {
            GTEST_SKIP () << "this platform cannot tell (only Linux, MacOS and Windows can)";
        }
        // a literal, and a constant array, are in the program's read-only image; nothing writable or not lasting is
        EXPECT_TRUE (*literal);
        EXPECT_EQ (IsAddressReadOnlyAppLifetime (bytes (kStaticConst)), true);
        EXPECT_EQ (IsAddressReadOnlyAppLifetime (bytes (sWritable)), false);
        EXPECT_EQ (IsAddressReadOnlyAppLifetime (bytes (onStack)), false);
        EXPECT_EQ (IsAddressReadOnlyAppLifetime (bytes (onHeap)), false);
        EXPECT_EQ (IsAddressReadOnlyAppLifetime (span<const byte>{}), false);
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, Finally)
    {
        Debug::TraceContextBumper ctx{"Finally"};
        {
            unsigned int cnt = 0;
            {
                [[maybe_unused]] auto&& c = Finally ([&cnt] () noexcept { cnt--; });
                ++cnt;
            }
            EXPECT_EQ (cnt, 0u);
        }
    }
}

namespace {
    namespace Test4_ConstantProperty_ {
        namespace Private_ {
            namespace T1_ {
                static const String                      x{"3"};
                const Execution::LazyInitialized<String> kX = [] () { return x; };
                void                                     DoIt ()
                {
                    const String a = kX;
                }
            }
            namespace T2_ {
                const Execution::LazyInitialized<String> kX = [] () { return "6"; };
                void                                     DoIt ()
                {
                    const String a = kX;
                    EXPECT_TRUE (a == "6"); // Before Stroika 2.1b12 there was a bug that ConstantProperty stored teh constant in a static variable not data member!
                }
            }
            namespace T3_ {
                // @todo get constexpr working - see docs for Execution::LazyInitialized
                //constexpr Execution::LazyInitialized<int> kX = [] () { return 3; };
                const Execution::LazyInitialized<int> kX = [] () { return 3; };
                void                                  DoIt ()
                {
                    const int a [[maybe_unused]] = kX;
                }
            }
            namespace T4_ {
                const Execution::LazyInitialized<int> kX = [] () { return 4; };
                void                                  DoIt ()
                {
                    const int a [[maybe_unused]] = kX;
                    EXPECT_TRUE (a == 4); // Before Stroika 2.1b12 there was a bug that ConstantProperty stored teh constant in a static variable not data member!
                }
            }
        }

    }
    GTEST_TEST (Foundation_Execution, ConstantProperty_)
    {
        Debug::TraceContextBumper ctx{"ConstantProperty_"};
        using namespace Test4_ConstantProperty_;
        Private_::T1_::DoIt ();
        Private_::T2_::DoIt ();
        Private_::T3_::DoIt ();
    }
}

namespace {
    namespace Test5_ModuleGetterSetter_ {
        namespace PRIVATE_ {
            using namespace DataExchange;
            using namespace Execution;
            using namespace Time;
            static const Duration kMinTime_ = 1s;
            struct MyData_ {
                bool               fEnabled = false;
                optional<DateTime> fLastSynchronizedAt;
            };
            struct ModuleGetterSetter_Implementation_MyData_ {
                ModuleGetterSetter_Implementation_MyData_ ()
                    : fOptionsFile_{"MyModule",
                                    [] () -> ObjectVariantMapper {
                                        ObjectVariantMapper mapper;
                                        mapper.AddClass<MyData_> ({
                                            {"Enabled", &MyData_::fEnabled},
                                            {"Last-Synchronized-At", &MyData_::fLastSynchronizedAt},
                                        });
                                        return mapper;
                                    }(),
                                    OptionsFile::kDefaultUpgrader, OptionsFile::mkFilenameMapper ("Put-Your-App-Name-Here")}
                    , fActualCurrentConfigData_{fOptionsFile_.Read<MyData_> (MyData_{})}
                {
                    Set (fActualCurrentConfigData_); // assure derived data (and changed fields etc) up to date
                }
                MyData_ Get () const
                {
                    return fActualCurrentConfigData_;
                }
                void Set (const MyData_& v)
                {
                    fActualCurrentConfigData_ = v;
                    fOptionsFile_.Write (v);
                }

            private:
                OptionsFile fOptionsFile_;
                MyData_     fActualCurrentConfigData_; // automatically initialized just in time, and externally synchronized
            };

            using Execution::ModuleGetterSetter;
            ModuleGetterSetter<MyData_, ModuleGetterSetter_Implementation_MyData_> sModuleConfiguration_;

            void TestUse1_ ()
            {
                if (sModuleConfiguration_.Get ().fEnabled) {
                    auto n     = sModuleConfiguration_.Get ();
                    n.fEnabled = false;
                    sModuleConfiguration_.Set (n);
                }
            }
            void TestUse2_ ()
            {
                sModuleConfiguration_.Update ([] (MyData_ data) {
                    MyData_ result = data;
                    if (result.fLastSynchronizedAt.has_value () and *result.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) {
                        result.fLastSynchronizedAt = DateTime::Now ();
                    }
                    return result;
                });
            }
            void TestUse3_ ()
            {
                if (sModuleConfiguration_.Update ([] (const MyData_& data) -> optional<MyData_> {
                        if (data.fLastSynchronizedAt.has_value () and *data.fLastSynchronizedAt + kMinTime_ > DateTime::Now ()) {
                            MyData_ result             = data;
                            result.fLastSynchronizedAt = DateTime::Now ();
                            return result;
                        }
                        return {};
                    })) {
                    // e.g. trigger someone to wakeup and used changes?
                }
            }
        }
    }
    GTEST_TEST (Foundation_Execution, ModuleGetterSetter_)
    {
        Debug::TraceContextBumper ctx{"ModuleGetterSetter_"};
        using namespace Test5_ModuleGetterSetter_;
        Execution::Logger::Activator logMgrActivator; // needed for OptionsFile test
        PRIVATE_::TestUse1_ ();
        PRIVATE_::TestUse2_ ();
        PRIVATE_::TestUse3_ ();
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, Environment)
    {
        Debug::TraceContextBumper ctx{"Environment"};
        Mapping<String, String>   env = Execution::kEnvironment;
        EXPECT_TRUE (env.ContainsKey ("PATH"));
#if qStroika_Platform_Windows
        // Windows names are case-insensitive: a native process sees "Path" (a POSIX shell's child, PATH)
        EXPECT_TRUE (env.ContainsKey ("path"));
        EXPECT_TRUE (Execution::kRawEnvironment ().ContainsKey (SDKSTR ("path")));
#endif
        DbgTrace ("env={}"_f, env);
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, CPUAffinity)
    {
        Debug::TraceContextBumper ctx{"CPUAffinity"};
        /*
         *  NB: this test MUTATES the affinity of the running test process, so everything below is wrapped
         *  in a Finally that puts it back. Leaving the process pinned would not fail anything here - it
         *  would quietly serialize every test that runs after this one, which is a far worse failure mode
         *  than an assertion.
         */
        optional<LogicalCPUCoreSet> originally = GetCPUAffinity ();
        [[maybe_unused]] auto&&     cleanup    = Execution::Finally ([&] () noexcept {
            if (originally and not originally->empty ()) {
                (void)SetCPUAffinityQuietly (*originally);
            }
        });

        if constexpr (kCPUAffinitySupported) {
            // we should be able to see what we are allowed to run on, and it must be self-consistent
            EXPECT_TRUE (originally.has_value ());
            if (originally) {
                DbgTrace ("initial affinity={}, logical cores={}"_f, *originally, Common::GetNumberOfLogicalCPUCores ());
                EXPECT_FALSE (originally->empty ()); // we are running, so at least one core is permitted
                for (unsigned int c : *originally) {
                    EXPECT_TRUE (c < Common::GetNumberOfLogicalCPUCores ());
                }
            }

            // pin to one core, and check that the mask we read back is exactly that one core
            optional<unsigned int> pinnedTo = PinToOneLogicalCPUCoreQuietly ();
            EXPECT_TRUE (pinnedTo.has_value ());
            if (pinnedTo) {
                EXPECT_TRUE (originally->Contains (*pinnedTo)); // must have chosen from what we held
                optional<LogicalCPUCoreSet> now = GetCPUAffinity ();
                EXPECT_TRUE (now.has_value ());
                if (now) {
                    EXPECT_TRUE (*now == LogicalCPUCoreSet{*pinnedTo});
                }
            }

            // A core number no platform could represent must fail cleanly - false, not an abort. Deliberately
            // an absurd value rather than 'cores + 1000': the mask ceiling is 32 on Windows x86, 64 on x64
            // and 1024 with glibc, so a modest overshoot passes on some platforms and not others. The first
            // version of this test used +1000, which slipped under glibc's 1024 and tripped an assertion on
            // Windows x86 debug - green on 4 of 5 CI platforms.
            EXPECT_FALSE (SetCPUAffinityQuietly (LogicalCPUCoreSet{1u << 20}));
        }
        else {
            // macOS and friends: every entry point must be a well-behaved no-op, not a crash and not a lie
            EXPECT_FALSE (originally.has_value ());
            EXPECT_FALSE (PinToOneLogicalCPUCoreQuietly ().has_value ());
            EXPECT_FALSE (SetCPUAffinityQuietly (LogicalCPUCoreSet{0}));
        }
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, ThrowIfNullCheck)
    {
        Debug::TraceContextBumper ctx{"ThrowIfNullCheck"};
        auto                      throwFailureCalls = [] () -> void {
            {
                void* p = nullptr;
                ThrowIfNull (p);
            }
            {
                //static_assert (equality_comparable_with<nullopt_t, optional<int>>);
                optional<int> p;
                ThrowIfNull (p);
            }
        };

        IgnoreExceptionsForCall (throwFailureCalls ());
    }
}

namespace {
    GTEST_TEST (Foundation_Execution, kInnerOuterExceptionStackHandlingWhile)
    {
        Debug::TraceContextBumper     ctx{"kInnerOuterExceptionStackHandlingWhile"};
        constexpr Execution::Activity kActivityOuter_{"OUTER"sv};
        Execution::DeclareActivity    declareActivity{&kActivityOuter_};
        constexpr Execution::Activity kActivityINNER_{"INNER"sv};
        Execution::DeclareActivity    declareActivity2{&kActivityINNER_};
        try {
            Execution::Throw (Execution::Exception<runtime_error>{"oops"});
        }
        catch (...) {
            DbgTrace ("error={}"_f, current_exception ());
        }
    }
}

namespace {
    /*
     *  An IntervalTimer removed - its Adder destroyed - while its callback runs: once removed, the callback is not running, and
     *  it is never called again.
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_RemovedWhileRunning_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_RemovedWhileRunning_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        atomic<unsigned int>              calls{0};
        atomic<bool>                      running{false};
        WaitableEvent                     started;
        optional<IntervalTimer::Adder>    adder{in_place,
                                                [&] () {
                                                 running = true;
                                                 ++calls;
                                                 started.Set ();
                                                 Execution::Sleep (500ms); // long enough to be removed while running
                                                 running = false;
                                                },
                                                Time::Duration{50ms}};
        started.Wait (10s);
        adder.reset (); // while its callback runs
        EXPECT_FALSE (running.load ()) << "the callback was still running after its timer was removed";
        unsigned int callsThen = calls;
        Execution::Sleep (1s); // twenty of its intervals
        EXPECT_EQ (calls.load (), callsThen) << "the callback was called again after its timer was removed";
    }

    /*
     *  A callback that removes its own timer - destroys its Adder - does not wait for itself to finish (which would be
     *  forever), and is not called again. Also when it is the last timer - whose removal, before v3.0d25, stopped the timer
     *  thread: the very thread removing it.
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_CallbackRemovesItself_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_CallbackRemovesItself_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        for (bool last : {false, true}) {
            optional<IntervalTimer::Adder> keepRunning;
            if (not last) {
                keepRunning.emplace ([] () {}, Time::Duration{1h});
            }
            atomic<unsigned int>           calls{0};
            WaitableEvent                  removed;
            mutex                          adderMutex; // this thread makes the Adder; the callback, on the timer's thread, destroys it
            optional<IntervalTimer::Adder> adder;
            {
                lock_guard lk{adderMutex};
                adder.emplace (
                    [&] () {
                        ++calls;
                        lock_guard lk{adderMutex};
                        adder.reset ();
                        removed.Set ();
                    },
                    Time::Duration{50ms});
            }
            EXPECT_TRUE (removed.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered) << (last ? "the last timer" : "not the last timer");
            Execution::Sleep (500ms); // ten of its intervals
            EXPECT_EQ (calls.load (), 1u) << (last ? "the last timer" : "not the last timer");
        }
    }

    /*
     *  A callback removes its own timer, then adds one, as another thread removes the last other timer. That thread, finding no
     *  timers left, stopped the timer thread - waiting for it, holding the lock adding a timer takes - so the callback, on that
     *  thread, waited for the lock, and the lock's holder for the callback: a deadlock (before v3.0d25).
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_CallbackAddsAsLastOtherGoes_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_CallbackAddsAsLastOtherGoes_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        IntervalTimer::Manager&           manager = IntervalTimer::Manager::sThe;
        IntervalTimer::TimerID            other   = manager.AddRepeating ([] () {}, Time::Duration{1h});
        WaitableEvent                     oursRemoved;
        WaitableEvent                     added;
        mutex                             oursMutex; // this thread sets ours; the callback, on the timer's thread, reads it
        optional<IntervalTimer::TimerID>  ours;
        {
            lock_guard lk{oursMutex};
            ours = manager.AddOneShot (
                [&] () {
                    {
                        lock_guard lk{oursMutex};
                        manager.Remove (*ours); // itself: only the other is left
                    }
                    oursRemoved.Set ();
                    // not interruptibly, as a callback at work: meanwhile the other thread removes the last timer, and so may stop
                    // this one
                    this_thread::sleep_for (500ms);
                    manager.AddOneShot ([] () {}, Time::Duration{1h});
                    added.Set ();
                },
                Time::Duration{100ms});
        }
        Thread::Ptr remover = Thread::New (
            [&] () {
                if (oursRemoved.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered) {
                    manager.Remove (other);
                }
            },
            Thread::eAutoStart);
        if (added.WaitQuietly (10s) != WaitableEvent::WaitStatus::eTriggered) {
            // deadlocked: neither thread can finish, nor then can the Activator's destructor - so end the run here, not hang
            ADD_FAILURE () << "deadlock: a callback adding a timer as another thread removed the last other one";
            fflush (stdout);
            fflush (stderr);
            _Exit (EXIT_FAILURE);
        }
        remover.Join ();
    }

    /*
     *  A one-shot can be removed: before it is called, so it never is; while it is called - Remove waiting for that call to end;
     *  and after, removing nothing (Remove returning false).
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_RemoveOneShot_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_RemoveOneShot_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        IntervalTimer::Manager&           manager = IntervalTimer::Manager::sThe;
        auto isThere = [&] (IntervalTimer::TimerID t) { return manager.GetAllRegisteredTasks ().Contains (t); };
        // before it is called
        atomic<unsigned int>   calls{0};
        IntervalTimer::TimerID notYet = manager.AddOneShot ([&] () { ++calls; }, Time::Duration{200ms});
        EXPECT_TRUE (manager.Remove (notYet));
        // while it is called
        atomic<bool>           running{false};
        WaitableEvent          started;
        IntervalTimer::TimerID now = manager.AddOneShot (
            [&] () {
                running = true;
                started.Set ();
                Execution::Sleep (300ms); // long enough to be removed while running
                running = false;
            },
            Time::Duration{10ms});
        EXPECT_TRUE (started.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered);
        EXPECT_TRUE (manager.Remove (now));
        EXPECT_FALSE (running.load ()) << "the one-shot was still running after it was removed";
        // after it was called
        WaitableEvent          called;
        IntervalTimer::TimerID done = manager.AddOneShot ([&] () { called.Set (); }, Time::Duration{10ms});
        EXPECT_TRUE (called.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered);
        for (Time::TimePointSeconds giveUpAt = Time::GetTickCount () + 10s; isThere (done) and Time::GetTickCount () < giveUpAt;) {
            Execution::Sleep (10ms); // until its call has ended: the timer thread then drops it
        }
        EXPECT_FALSE (manager.Remove (done));
        Execution::Sleep (500ms); // past the time of the one removed before it was called
        EXPECT_EQ (calls.load (), 0u) << "the one-shot removed before it was called was called";
    }

    /*
     *  An Adder given a Manager adds its timer to that one - not to Manager::sThe - and removes it from there.
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_AdderGivenManager_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_AdderGivenManager_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        IntervalTimer::Manager            other{Memory::MakeSharedPtr<IntervalTimer::Manager::DefaultRep> ()};
        {
            IntervalTimer::Adder adder{other, [] () {}, Time::Duration{1h}};
            EXPECT_EQ (other.GetAllRegisteredTasks ().size (), 1u);
            EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 0u);
        }
        EXPECT_EQ (other.GetAllRegisteredTasks ().size (), 0u);
    }

    /*
     *  An Adder moved into another: the one assigned to removes its own timer, and takes over the moved one's - not adding it
     *  again - which goes when the Adder now holding it does (the one moved from removes nothing).
     */
    GTEST_TEST (Foundation_Execution, IntervalTimer_AdderMoveAssign_)
    {
        Debug::TraceContextBumper         ctx{"IntervalTimer_AdderMoveAssign_"};
        IntervalTimer::Manager::Activator intervalTimerMgrActivator;
        {
            IntervalTimer::Adder a{[] () {}, Time::Duration{1h}};
            {
                IntervalTimer::Adder b{[] () {}, Time::Duration{2h}};
                EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 2u);
                a = move (b);
                EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 1u);
            }
            EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 1u) << "the Adder moved from removed the timer";
            optional<IntervalTimer::RegisteredTask> t = IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().First ();
            EXPECT_TRUE (t and t->fFrequency == Time::Duration{2h}) << "not the timer moved in"; // (b's: 2 hours, a's 1)
        }
        EXPECT_EQ (IntervalTimer::Manager::sThe.GetAllRegisteredTasks ().size (), 0u);
    }
}

namespace {
    /*
     *  CallbackRegistry: each callback called in the order added; a removed one never again; one that throws stops none of
     *  the others; and an ID from another registry, or one already removed, removes nothing.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_)
    {
        Debug::TraceContextBumper    ctx{"CallbackRegistry_"};
        CallbackRegistry<void (int)> callbacks;
        vector<int>                  calls;
        auto                         first = callbacks.Add ([&] (int i) { calls.push_back (i); });
        callbacks.Add ([] (int) { Execution::Throw (Execution::Exception<>{"a callback, throwing"sv}); });
        auto last = callbacks.Add ([&] (int i) { calls.push_back (10 * i); });
        EXPECT_NE (first, last);
        callbacks.Call (1);
        EXPECT_EQ (calls, (vector<int>{1, 10}));
        callbacks.Remove (first);
        callbacks.Call (2);
        EXPECT_EQ (calls, (vector<int>{1, 10, 20}));
        CallbackRegistry<void (int)> other;
        callbacks.Remove (other.Add ([] (int) {})); // not one of callbacks'
        callbacks.Remove (first);                   // already removed
        callbacks.Remove ({});                      // names none
        callbacks.Call (3);
        EXPECT_EQ (calls, (vector<int>{1, 10, 20, 30}));
    }

    /*
     *  With a callback running on another thread: Add does not wait for it; Remove does - so once Remove returns, the callback
     *  is not running, and it is never called again.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_Threads_)
    {
        Debug::TraceContextBumper ctx{"CallbackRegistry_Threads_"};
        CallbackRegistry<void ()> callbacks;
        atomic<unsigned int>      calls{0};
        atomic<bool>              running{false};
        WaitableEvent             started;
        WaitableEvent             letGo;
        auto                      id     = callbacks.Add ([&] () {
            ++calls;
            running = true;
            started.Set ();
            letGo.WaitQuietly (10s);
            running = false;
        });
        Thread::Ptr               caller = Thread::New ([&] () { callbacks.Call (); }, Thread::eAutoStart);
        EXPECT_TRUE (started.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered);

        // the callback waits for this thread - so an Add waiting for it would take its 10 seconds
        Time::TimePointSeconds start = Time::GetTickCount ();
        callbacks.Add ([] () {});
        EXPECT_LT ((Time::GetTickCount () - start).count (), 1.0) << "Add waited for a running callback";

        atomic<bool> removed{false};
        Thread::Ptr  remover = Thread::New (
            [&] () {
                callbacks.Remove (id);
                removed = true;
            },
            Thread::eAutoStart);
        Execution::Sleep (200ms); // for Remove to be waiting
        EXPECT_FALSE (removed.load ()) << "Remove did not wait for the running callback";
        letGo.Set ();
        remover.Join ();
        EXPECT_FALSE (running.load ()) << "the callback was still running after Remove returned";
        caller.Join ();
        callbacks.Call ();
        EXPECT_EQ (calls.load (), 1u) << "the callback was called after Remove returned";
    }

    /*
     *  A callback that removes itself does not wait for itself to finish (which would be forever), and is not called again; nor
     *  is one it removes that comes after it, in the very Call that removed it.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_RemoveFromCallback_)
    {
        Debug::TraceContextBumper     ctx{"CallbackRegistry_RemoveFromCallback_"};
        CallbackRegistry<void ()>     callbacks;
        unsigned int                  selfCalls  = 0;
        unsigned int                  laterCalls = 0;
        CallbackRegistry<void ()>::ID self;
        CallbackRegistry<void ()>::ID later;
        self  = callbacks.Add ([&] () {
            ++selfCalls;
            callbacks.Remove (self);
            callbacks.Remove (later);
        });
        later = callbacks.Add ([&] () { ++laterCalls; });
        callbacks.Call ();
        callbacks.Call ();
        EXPECT_EQ (selfCalls, 1u);
        EXPECT_EQ (laterCalls, 0u) << "called in the very Call that removed it";
    }

    /*
     *  RemoveAll waits for a callback running on another thread, and the rest of that Call calls none; called from within a
     *  callback, it does not wait for that one, and none is called again.
     */
    GTEST_TEST (Foundation_Execution, CallbackRegistry_RemoveAll_)
    {
        Debug::TraceContextBumper ctx{"CallbackRegistry_RemoveAll_"};
        {
            CallbackRegistry<void ()> callbacks;
            atomic<unsigned int>      calls{0};
            atomic<bool>              running{false};
            WaitableEvent             started;
            callbacks.Add ([&] () {
                ++calls;
                running = true;
                started.Set ();
                Execution::Sleep (200ms); // so RemoveAll, on this test's thread, finds it running
                running = false;
            });
            callbacks.Add ([&] () { ++calls; });
            Thread::Ptr caller = Thread::New ([&] () { callbacks.Call (); }, Thread::eAutoStart);
            EXPECT_TRUE (started.WaitQuietly (10s) == WaitableEvent::WaitStatus::eTriggered);
            callbacks.RemoveAll ();
            EXPECT_FALSE (running.load ()) << "a callback was still running after RemoveAll returned";
            caller.Join ();
            callbacks.Call ();
            EXPECT_EQ (calls.load (), 1u) << "a callback was called after RemoveAll returned";
        }
        {
            CallbackRegistry<void ()> callbacks;
            unsigned int              calls = 0;
            callbacks.Add ([&] () {
                ++calls;
                callbacks.RemoveAll ();
            });
            callbacks.Add ([&] () { ++calls; });
            callbacks.Call ();
            callbacks.Call ();
            EXPECT_EQ (calls, 1u);
        }
    }
}
#endif

namespace {
    // CommandLineFromMain_'s child: exits 0 only if this program's arguments come through whole
    int ArgumentsChild_ (int argc, const char* argv[])
    {
        if (CommandLine{argc, argv}.GetArguments ()[2] != kNotInCodePage_) {
            return 1;
        }
        // any other argv is taken as given
        vector<const char*> copy{argv, argv + argc};
        if (CommandLine{argc, copy.data ()}.GetArguments ()[2] != String::FromNarrowSDKString (argv[2])) {
            return 2;
        }
#if qStroika_Foundation_Execution_CommandLine_SupportsThisProcess
        if (CommandLine::ThisProcess ().GetArguments () != CommandLine{argc, argv}.GetArguments ()) {
            return 3;
        }
#endif
        return 0;
    }
}

int main (int argc, const char* argv[])
{
    if (argc == 3 and argv[1] == kArgumentsChild_) {
        return ArgumentsChild_ (argc, argv);
    }
    Test::Setup (argc, argv);
#if qStroika_HasComponent_googletest
    return RUN_ALL_TESTS ();
#else
    cerr << "[  SKIPPED ] every test - Stroika regression tests require building with google test feature" << endl;
#endif
}
