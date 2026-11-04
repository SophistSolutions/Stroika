/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#if qStroika_Platform_MacOS
#include <crt_externs.h>
#endif
#if qStroika_Platform_Windows
#include <windows.h>

#include <shellapi.h>
#endif

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Execution/Finally.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/IO/FileSystem/Exception.h"

#include "CommandLine.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Common;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Traversal;

/*
 ********************************************************************************
 ******************* Execution::InvalidCommandLineArgument **********************
 ********************************************************************************
 */
InvalidCommandLineArgument::InvalidCommandLineArgument ()
    : Execution::Exception<runtime_error>{"Invalid Command Argument"sv}
{
}
InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message)
    : Execution::Exception<runtime_error>{message}
{
}
InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message, const String& argument)
    : Execution::Exception<runtime_error>{message}
    , fArgument{argument}
{
}

/*
 ********************************************************************************
 ****************** Execution::MatchesCommandLineArgument ***********************
 ********************************************************************************
 */
DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
namespace {
    String Simplify2Compare_ (const String& actualArg)
    {
        return actualArg.StripAll ([] (Characters::Character c) -> bool { return c == '-' or c == '/'; }).ToLowerCase ();
    }
}

bool Execution::MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern)
{
    // Command-line arguments must start with - or / (windows only)
    if (actualArg.empty ()) {
        return false;
    }
#if qStroika_Platform_Windows
    if (actualArg[0] != '-' and actualArg[0] != '/') {
        return false;
    }
#else
    if (actualArg[0] != '-') {
        return false;
    }
#endif
    return Simplify2Compare_ (actualArg) == Simplify2Compare_ (matchesArgPattern);
}

bool Execution::MatchesCommandLineArgument (const Iterable<String>& argList, const String& matchesArgPattern)
{
    return static_cast<bool> (
        argList.Find ([matchesArgPattern] (String i) -> bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); }));
}

optional<String> Execution::MatchesCommandLineArgumentWithValue ([[maybe_unused]] const String& actualArg, [[maybe_unused]] const String& matchesArgPattern)
{
    Require (matchesArgPattern.size () > 0 and matchesArgPattern[matchesArgPattern.size () - 1] == '=');
    AssertNotImplemented ();
    // must first strip everything after the '=' in the actualarg, and then similar to first overload...
    return nullopt;
}

optional<String> Execution::MatchesCommandLineArgumentWithValue (const Iterable<String>& argList, const String& matchesArgPattern)
{
    auto i = argList.Find ([matchesArgPattern] (const String& i) -> bool { return MatchesCommandLineArgument (i, matchesArgPattern); });
    if (i != argList.end ()) {
        ++i;
        if (i == argList.end ()) [[unlikely]] {
            Execution::Throw (InvalidCommandLineArgument{});
        }
        else {
            return optional<String>{*i};
        }
    }
    return nullopt;
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

/*
 ********************************************************************************
 ************************** CommandLine::Option *********************************
 ********************************************************************************
 */
String CommandLine::Option::GetArgumentDescription (bool includeArgName) const
{
    if (not this->fSupportsArgument and not this->IsPositionArgument ()) {
        includeArgName = false;
    }
    String argName = this->fHelpArgName.value_or ("ARG"sv);
    if (includeArgName and fSupportsArgument and not fIfSupportsArgumentThenRequired and fLongName) {
        // an optional argument is given only as --long=VALUE
        return fSingleCharName ? "(-{}|--{}[={}])"_f(*fSingleCharName, *fLongName, argName) : "--{}[={}]"_f(*fLongName, argName);
    }
    if (fSingleCharName and fLongName) {
        if (includeArgName) {
            return "(-{} {}|--{}={})"_f(*fSingleCharName, argName, *fLongName, argName);
        }
        else {
            return "(-{}|--{})"_f(*fSingleCharName, *fLongName);
        }
    }
    else if (this->fSingleCharName) {
        if (includeArgName) {
            return "-{} {}"_f(*fSingleCharName, argName);
        }
        else {
            return "-{}"_f(*fSingleCharName);
        }
    }
    else if (fLongName) {
        if (includeArgName) {
            return "--"sv + *fLongName + "="sv + argName;
        }
        else {
            return "--"sv + *fLongName;
        }
    }
    else {
        if (includeArgName) {
            return argName;
        }
        else {
            return String{};
        }
    }
}

String CommandLine::Option::ToString () const
{
    StringBuilder sb;
    sb << "{"sv;
    if (fSingleCharName) {
        sb << "singleCharName: "sv << *fSingleCharName << ","sv;
    }
    if (fLongName) {
        sb << "longName: "sv << *fLongName << ","sv;
    }
    if (this->IsPositionArgument ()) {
        sb << "POSITIONAL ARGUMENT,"sv;
    }
    sb << "caseSensitive: "sv << fLongNameCaseSensitive << ","sv;
    if (not this->IsPositionArgument ()) {
        sb << "supportsArgument: "sv << fSupportsArgument << ","sv;
        sb << "ifSupportsArgumentThenRequired: "sv << fIfSupportsArgumentThenRequired << ","sv;
    }
    sb << "repeatable: "sv << fRepeatable << ","sv;
    sb << "required: "sv << fRequired << ","sv;
    if (fWaivesRequired) {
        sb << "waivesRequired: "sv << fWaivesRequired << ","sv;
    }
    if (fSkipFirstNArguments) {
        sb << "skipFirstNArguments: "sv << fSkipFirstNArguments << ","sv;
    }
    if (fHelpArgName) {
        sb << "helpArgName: "sv << *fHelpArgName << ","sv;
    }
    if (fHelpOptionText) {
        sb << "helpOptionText: "sv << *fHelpOptionText << ","sv;
    }
    sb << "}"sv;
    return sb;
}

/*
 ********************************************************************************
 ************************* CommandLine::OptionValues ****************************
 ********************************************************************************
 */
CommandLine::OptionValues::OptionValues (const Iterable<Option>& options, const Sequence<Given_>& given)
    : fOptions_{options}
    , fGiven_{given}
{
}

bool CommandLine::OptionValues::Has (const Option& o) const
{
    Require (fOptions_.Contains (o));
    return fGiven_.Any ([&] (const Given_& g) { return g.fOption == o; });
}

optional<String> CommandLine::OptionValues::GetArgument (const Option& o) const
{
    Require (fOptions_.Contains (o));
    if (optional<Given_> found = fGiven_.First ([&] (const Given_& g) { return g.fOption == o and g.fArgument.has_value (); })) {
        return found->fArgument;
    }
    return nullopt;
}

Sequence<String> CommandLine::OptionValues::GetArguments (const Option& o) const
{
    Require (fOptions_.Contains (o));
    Sequence<String> result;
    for (const Given_& g : fGiven_) {
        if (g.fOption == o and g.fArgument) {
            result += *g.fArgument;
        }
    }
    return result;
}

String CommandLine::OptionValues::ToString () const
{
    StringBuilder sb;
    sb << "["sv;
    bool first = true;
    for (const Given_& g : fGiven_) {
        if (not first) {
            sb << ", "sv;
        }
        first = false;
        sb << g.fOption.GetArgumentDescription (g.fOption.IsPositionArgument ());
        if (g.fArgument) {
            sb << ": "sv << Characters::ToString (*g.fArgument);
        }
    }
    sb << "]"sv;
    return sb;
}

/*
 ********************************************************************************
 ********************************** CommandLine *********************************
 ********************************************************************************
 */
namespace {
    Sequence<String> ParseArgs_ (const String& cmdLine)
    {
        Sequence<String> args;
        size_t           e = cmdLine.length ();
        StringBuilder    curToken;
        Character        endQuoteChar = '\0';
        for (size_t i = 0; i < e; ++i) {
            Character c = cmdLine[i];
            if (endQuoteChar != '\0' and c == endQuoteChar) {
                args.Append (curToken.str ());
                endQuoteChar = '\0';
                curToken.clear ();
            }
            else if (c == '\'' or c == '\"') {
                endQuoteChar = c;
            }
            else if (endQuoteChar != '\0') {
                // in middle of quoted string
                curToken += c;
            }
            else {
                bool isTokenChar = not c.IsWhitespace ();
                if (isTokenChar) {
                    curToken += c;
                }
                else {
                    if (curToken.size () != 0) {
                        args.Append (curToken.str ());
                        curToken.clear ();
                    }
                }
            }
        }
        if (curToken.size () != 0) {
            args.Append (curToken.str ());
        }
        return args;
    }
}
#if qStroika_Platform_Linux and qStroika_Foundation_Execution_CommandLine_SupportsThisProcess
namespace {
    // glibc passes main's argc and argv to each .init_array function (an extension Rust's standard library relies on too).
    // Priority 101, the first not reserved for the implementation, so this runs before any static object's constructor
    int          sArgc_;
    const char** sArgv_;
    void         CaptureArguments_ (int argc, char** argv, char**)
    {
        sArgc_ = argc;
        sArgv_ = const_cast<const char**> (argv);
    }
    [[gnu::section (".init_array.00101"), gnu::used]] void (*sCaptureArguments_) (int, char**, char**) = &CaptureArguments_;
}
#endif
#if qStroika_Platform_Windows
#pragma comment(lib, "Shell32.lib") // CommandLineToArgvW ()
namespace {
    // this program's command line as Windows keeps it - UTF-16, so whole - split by CommandLineToArgvW
    Sequence<String> GetWideArguments_ ()
    {
        int       argc = 0;
        wchar_t** argv = ::CommandLineToArgvW (::GetCommandLineW (), &argc);
        if (argv == nullptr) [[unlikely]] {
            ThrowSystemErrNo ();
        }
        [[maybe_unused]] auto&& cleanup = Finally ([argv] () noexcept { ::LocalFree (argv); });
        Sequence<String>        result;
        for (int i = 0; i < argc; ++i) {
            result.push_back (String{argv[i]});
        }
        return result;
    }
}
#endif
CommandLine::CommandLine (const String& cmdLine)
    : fArgs_{ParseArgs_ (cmdLine)}
{
}

CommandLine::CommandLine (WrapInShell wrapInShell, const String& cmdLine)
{
    switch (wrapInShell) {
        case WrapInShell::eBash:
#if qStroika_Platform_Windows
        {
            // https://github.com/SophistSolutions/Stroika/issues/132 (STK-1029)
            // Weird bug workaround only needed on Medusa? - unclear why
            if (optional<filesystem::path> pp = FindExecutableInPath ("bash"sv)) {
                fArgs_ += String{*pp};
            }
            else {
                Throw (IO::FileSystem::Exception{make_error_code (errc::no_such_file_or_directory), filesystem::path{"bash"sv}});
            }
        }
#else
            fArgs_ += "bash"sv;
#endif
            fArgs_ += "-c"sv;
            fArgs_ += cmdLine;
            fShellStyleQuoting_ = StringShellQuoting::eBash;
            break;
#if qStroika_Platform_Windows
        case WrapInShell::eWindowsCMD: {
            // this is the version of CMD.exe to invoke (I think)
            // https://en.wikipedia.org/wiki/COMSPEC
            static const String kCOMPSEC_ = [] () -> String {
                DISABLE_COMPILER_MSC_WARNING_START (4996)
                if (const char* env_p = std::getenv ("COMSPEC")) {
                    return String::FromNarrowSDKString (env_p);
                }
                DISABLE_COMPILER_MSC_WARNING_END (4996)
                return "C:\\WINDOWS\\system32\\cmd.exe"sv;
            }();
            fArgs_ += kCOMPSEC_;
            // fArgs_ += "/D";
            //   fArgs_ += "/E:OFF";
            //  fArgs_ += "/F:OFF";
            fArgs_ += "/C"sv; // Carries out the command specified by string and then terminates
            fArgs_ += cmdLine;
            fShellStyleQuoting_ = StringShellQuoting::eWindowsCMD;
        } break;
#endif
        default:
            RequireNotReached ();
    }
}

CommandLine::CommandLine (int argc, const char* argv[])
{
#if qStroika_Platform_Windows
    // main's own argv holds each argument converted to the code page, losing what it lacks - so take the UTF-16 command
    // line's arguments instead, if each converts to just what argv holds (setargv.obj expands wildcards; CommandLineToArgvW
    // splits a rare quoting otherwise)
    if (argc == __argc and argv == const_cast<const char**> (__argv)) {
        Sequence<String> wideArgs = GetWideArguments_ ();
        bool             same     = wideArgs.size () == static_cast<size_t> (argc);
        for (int i = 0; same and i < argc; ++i) {
            same = SDK2Narrow (wideArgs[i].AsSDKString (), eIgnoreErrors) == argv[i];
        }
        if (same) {
            fArgs_ = wideArgs;
            return;
        }
    }
#endif
    for (int i = 0; i < argc; ++i) {
        fArgs_.push_back (String::FromNarrowSDKString (argv[i]));
    }
}

CommandLine::CommandLine (int argc, const wchar_t* argv[])
{
    for (int i = 0; i < argc; ++i) {
        fArgs_.push_back (argv[i]);
    }
}

#if qStroika_Foundation_Execution_CommandLine_SupportsThisProcess
CommandLine CommandLine::ThisProcess ()
{
#if qStroika_Platform_Linux
    AssertNotNull (sArgv_); // glibc calls CaptureArguments_ before any static object's constructor
    return CommandLine{sArgc_, sArgv_};
#elif qStroika_Platform_MacOS
    return CommandLine{*::_NSGetArgc (), const_cast<const char**> (*::_NSGetArgv ())};
#elif qStroika_Platform_Windows
    if (__wargv != nullptr) {
        return CommandLine{__argc, const_cast<const wchar_t**> (__wargv)}; // a wmain program: the C runtime's own split
    }
    return CommandLine{GetWideArguments_ ()};
#endif
}
#endif

String CommandLine::GenerateUsage (const Iterable<Option>& options) const
{
    return GenerateUsage (GetAppName (), options);
}

String CommandLine::GenerateUsage (const String& exeName, const Iterable<Option>& options)
{
    StringBuilder sb;
    sb << "Usage: "sv << exeName;
    // the usual notation: [X] optional, X required, X... repeatable
    options.Apply ([&] (const Option& o) {
        String d = o.GetArgumentDescription (true);
        if (not o.fRequired) {
            if (o.fSingleCharName and o.fLongName) {
                d = d.SubString (1, d.size () - 1); // (-x|--long) needs no parentheses inside [...]
            }
            d = "["sv + d + "]"sv;
        }
        sb << " "sv << d;
        if (o.fRepeatable) {
            sb << "..."sv;
        }
    });
    sb << "\n"sv;
    // then a line for each option with help text: its names and argument, as GNU and argparse show them (-n, --name=NAME), the
    // text aligned
    auto helpName = [] (const Option& o) -> String {
        String argName = o.fHelpArgName.value_or ("ARG"sv);
        if (o.IsPositionArgument ()) {
            return argName;
        }
        String longName;
        if (o.fLongName) {
            longName = "--"sv + *o.fLongName;
            if (o.fSupportsArgument) {
                longName = longName + (o.fIfSupportsArgumentThenRequired ? "="sv + argName : "[="sv + argName + "]"sv);
            }
        }
        if (o.fSingleCharName) {
            if (o.fLongName) {
                return "-{}, {}"_f(*o.fSingleCharName, longName);
            }
            return o.fSupportsArgument ? "-{} {}"_f(*o.fSingleCharName, argName) : "-{}"_f(*o.fSingleCharName);
        }
        return longName;
    };
    size_t maxNameLen{0};
    options.Apply ([&] (const Option& o) {
        if (o.fHelpOptionText) {
            maxNameLen = max (maxNameLen, helpName (o).length ());
        }
    });
    options.Apply ([&] (const Option& o) {
        if (o.fHelpOptionText) {
            String name = helpName (o);
            sb << "  "sv << name << " "_k.Repeat (static_cast<unsigned int> (4 + maxNameLen - name.size ())) << *o.fHelpOptionText << "\n"sv;
        }
    });
    return sb;
}

auto CommandLine::ApplyOptions (const Iterable<Option>& options) const -> OptionValues
{
    return ThrowIfFailed (ApplyOptionsQuietly (options));
}

auto CommandLine::ApplyOptionsQuietly (const Iterable<Option>& options) const -> ExpectedOptionValues
{
    using Given                  = OptionValues::Given_;
    using ERRT                   = StdCompat::unexpected<InvalidCommandLineArgument>;
    Sequence<Option> positionals = options.Where<Sequence<Option>> ([] (const Option& o) { return o.IsPositionArgument (); });
    Require (Set<Option>{options}.size () == options.size ());
    Require (positionals.size () < 2 or not positionals.Take (positionals.size () - 1).Any ([] (const Option& o) { return o.fRepeatable; }));
    Require (options.All ([] (const Option& o) { return not o.fSupportsArgument or o.fIfSupportsArgumentThenRequired or o.fLongName; }));
    Require (options.All ([] (const Option& o) { return not o.fRepeatable or o.fSupportsArgument or o.IsPositionArgument (); }));
    Require (not options.Any ([&] (const Option& a) {
        // no word may name two options: a long name matches case-insensitively if either option says so
        return options.Any ([&] (const Option& b) {
            CompareOptions co = a.fLongNameCaseSensitive == eWithCase and b.fLongNameCaseSensitive == eWithCase ? eWithCase : eCaseInsensitive;
            return a != b and ((a.fSingleCharName and a.fSingleCharName == b.fSingleCharName) or
                               (a.fLongName and b.fLongName and String::EqualsComparer{co}(*a.fLongName, *b.fLongName)));
        });
    }));

    Sequence<Given> given; // each option given, in command-line order
    auto            wasGiven       = [&] (const Option& o) { return given.Any ([&] (const Given& g) { return g.fOption == o; }); };
    size_t          nextPositional = 0;
    bool            optionsEnded   = false; // after a bare --, every word is positional
    for (size_t i = 1; i < fArgs_.size (); ++i) {
        String ai = fArgs_[i];
        if (ai == "--"sv and not optionsEnded) {
            optionsEnded = true;
        }
        else if (ai.size () >= 2 and ai[0] == '-' and not optionsEnded) {
            // -x, --long or --long=VALUE
            optional<Option> o;
            optional<String> afterEquals;
            if (ai[1] == '-') {
                String name = ai.SubString (2);
                if (optional<size_t> eq = name.Find ('=')) {
                    afterEquals = name.SubString (*eq + 1);
                    name        = name.SubString (0, *eq);
                }
                o = options.First ([&] (const Option& oi) {
                    return oi.fLongName and String::EqualsComparer{oi.fLongNameCaseSensitive}(name, *oi.fLongName);
                });
            }
            else if (ai.size () == 2) {
                o = options.First ([&] (const Option& oi) { return oi.fSingleCharName and ai[1] == *oi.fSingleCharName; });
            }
            if (not o) {
                return ERRT{InvalidCommandLineArgument{"Unrecognized argument: "sv + ai, ai}};
            }
            if (not o->fRepeatable and wasGiven (*o)) {
                return ERRT{InvalidCommandLineArgument{"Command line argument {} given more than once"_f(o->GetArgumentDescription ()), ai}};
            }
            optional<String> argument;
            if (afterEquals) {
                if (not o->fSupportsArgument) {
                    return ERRT{InvalidCommandLineArgument{"Command line argument {} takes no argument"_f(o->GetArgumentDescription ()), ai}};
                }
                argument = afterEquals;
            }
            else if (o->fSupportsArgument and o->fIfSupportsArgumentThenRequired) {
                // the next word, whatever it is; an optional argument is given only after = - as getopt - so one is never taken
                if (i + 1 == fArgs_.size ()) {
                    return ERRT{InvalidCommandLineArgument{"Command line argument {} requires an argument, but none was given"_f(ai), ai}};
                }
                argument = fArgs_[++i];
            }
            given += Given{*o, argument};
        }
        else {
            // a positional word: the next positional option takes it (a repeatable one, the last, takes all those left)
            if (nextPositional == positionals.size ()) {
                return ERRT{InvalidCommandLineArgument{"Unrecognized argument: "sv + ai, ai}};
            }
            Option p = positionals[nextPositional];
            given += Given{p, ai};
            if (not p.fRepeatable) {
                ++nextPositional;
            }
        }
    }
    if (not given.Any ([] (const Given& g) { return g.fOption.fWaivesRequired; })) {
        if (optional<Option> missing = options.First ([&] (const Option& o) { return o.fRequired and not wasGiven (o); })) {
            return ERRT{InvalidCommandLineArgument{"Required command line argument "sv + missing->GetArgumentDescription (true) + " was not provided"sv}};
        }
    }
    return OptionValues{options, given};
}

DISABLE_COMPILER_MSC_WARNING_START (4996);
DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
void CommandLine::Validate (const Iterable<Option>& options) const
{
    if (auto oe = ValidateQuietly (options)) {
        Throw (*oe);
    }
}
DISABLE_COMPILER_MSC_WARNING_END (4996);
DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");

nonvirtual optional<InvalidCommandLineArgument> CommandLine::ValidateQuietly (const Iterable<Option>& options) const
{
    Set<Option> all{options};
    Set<Option> unused{all};
    for (Iterator<String> argi = fArgs_.begin () + 1; argi != fArgs_.end (); ++argi) {
        if (not all.First ([&] (const Option& o) {
                auto oRes = ParseOneArg_ (o, &argi);
                if (!oRes) {
                    Throw (oRes.error ());
                }
                if (*oRes) {
                    unused.RemoveIf (o);
                    return true;
                }
                return false;
            })) {
            return InvalidCommandLineArgument{"Unrecognized argument: "sv + *argi, *argi};
        }
    }
    if (auto o = unused.First ([] (const Option& o) { return o.fRequired; })) {
        return InvalidCommandLineArgument{"Required command line argument "sv + o->GetArgumentDescription (true) + " was not provided"sv};
    }
    return nullopt;
}

String CommandLine::GetAppName (bool onlyBaseName) const
{
    if (fArgs_.empty ()) {
        return String{};
    }
    if (onlyBaseName) {
        filesystem::path p = fArgs_[0].As<filesystem::path> ();
        return String{p.stem ()};
    }
    return fArgs_[0];
}

tuple<bool, Sequence<String>> CommandLine::Get (const Option& o) const
{
    bool   found      = false;
    size_t nMore2Skip = o.fSkipFirstNArguments.value_or (0);
    Assert (nMore2Skip == 0 or o.IsPositionArgument ());
    Sequence<String> arguments;
    for (Iterator<String> argi = fArgs_.begin () + 1; argi != fArgs_.end (); ++argi) {
        if (optional<optional<String>> oRes = ThrowIfFailed (ParseOneArg_ (o, &argi))) {
            found = true;
            if (nMore2Skip == 0) {
                if (*oRes) {
                    arguments += **oRes;
                    if (not o.fRepeatable) {
                        break; // no need to keep looking
                    }
                }
            }
            else {
                --nMore2Skip;
            }
        }
    }
    if (o.fRequired and not found and arguments.empty ()) {
        Throw (InvalidCommandLineArgument{"Command line argument '{}' required but not provided"_f(o.GetArgumentDescription ())});
    }
    if (found and o.fSupportsArgument and o.fIfSupportsArgumentThenRequired and arguments.empty ()) {
        Throw (InvalidCommandLineArgument{"Command line argument {} provided, but without required argument"_f(o.GetArgumentDescription ())});
    }
    return make_tuple (found, arguments);
}

String CommandLine::ToString () const
{
    return this->As<String> (); // hides some details, but most useful summary typically
}

StdCompat::expected<optional<optional<String>>, InvalidCommandLineArgument> CommandLine::ParseOneArg_ (const Option& o, Iterator<String>* argi)
{
    RequireNotNull (argi);
    Require (not argi->AtEnd ());
    using OptionalArgument    = optional<String>;
    using OptionallyHasOption = optional<OptionalArgument>;
    using RT                  = StdCompat::expected<OptionallyHasOption, InvalidCommandLineArgument>;
    using ERRT                = StdCompat::unexpected<InvalidCommandLineArgument>;

    // Sorry api is confusing about these two optionals - not clear how todo better, but its private so no biggie
    static const RT kMissingArgument_ = RT{OptionallyHasOption{OptionalArgument{}}};
    static const RT kMissingOption_   = RT{OptionallyHasOption{}};

    String ai = **argi;
    if (o.fSingleCharName and ai.length () == 2 and ai[0] == '-' and ai[1] == o.fSingleCharName) {
        if (o.fSupportsArgument) {
            ++(*argi);
            if ((*argi).AtEnd ()) {
                if (o.fIfSupportsArgumentThenRequired) {
                    return ERRT{InvalidCommandLineArgument{
                        "Command line argument requires an argument to it, but none provided (= or following argument)"sv, ai}};
                }
                return kMissingArgument_;
            }
            else {
                return RT{OptionallyHasOption{**argi}};
            }
        }
        return kMissingArgument_;
    }

    String argiUpToEquals = ai;
    if (optional<size_t> indexOfEquals = argiUpToEquals.Find ('=')) {
        argiUpToEquals = argiUpToEquals.SubString (0, *indexOfEquals);
    }
    if (o.fLongName and ai.length () >= 2 + o.fLongName->size () and ai[0] == '-' and ai[1] == '-' and
        String::EqualsComparer{o.fLongNameCaseSensitive}(argiUpToEquals.SubString (2), *o.fLongName)) {
        String restOfArgi = ai.SubString (2 + o.fLongName->size ());
        if (o.fSupportsArgument) {
            // see if '=' follows longname
            if (restOfArgi.size () >= 1 and restOfArgi[0] == '=') {
                return RT{OptionallyHasOption{restOfArgi.SubString (1)}};
            }
            ++(*argi); // Look for argument as string just after --option
            if ((*argi).AtEnd ()) {
                if (o.fIfSupportsArgumentThenRequired) {
                    return ERRT{InvalidCommandLineArgument{
                        "Command line argument requires an argument to it, but none provided (= or following argument)"sv, ai}};
                }
                return kMissingArgument_;
            }
            else {
                return RT{OptionallyHasOption{**argi}};
            }
            AssertNotReached ();
        }
        else {
            return kMissingArgument_;
        }
        AssertNotReached ();
    }
    // anything that cannot be an option (-x or --y...) is skipped, but anything else - that could be a positional parameter (even a bare '-') is matched as 'argument'
    if (o.IsPositionArgument () and /* o.fSupportsArgument and */ not(ai.size () >= 2 and ai.StartsWith ("-"sv))) {
        // note we add the argument, but don't set 'found'
        return RT{OptionallyHasOption{**argi}};
    }
    return kMissingOption_;
}

template <>
String CommandLine::As<String> () const
{
    return As<String> (this->fShellStyleQuoting_);
}

template <>
String CommandLine::As<String> (optional<CommandLine::StringShellQuoting> shellStyle) const
{
    // UNCLEAR how to handle quoting of elements inside string - so for now, do (less) harm? DOnt try to
    // quote the quotes (but still wrap items in quotes).
    //  --LGP 2024-12-07
    return fArgs_.Join<String> (
        [&] (const String& i) {
            // default in Stroika is wrap in double-quotes, and \-quote double-quote characters, and rest leave alone
            if (shellStyle == nullopt) {
                if (i.ContainsAny ({' ', '\"'})) {
                    return "\"{}\""_f(i);
                    //return "\"{}\""_f(i.ReplaceAll ("\""sv, "\\\""sv));
                }
                else {
                    return i;
                }
            }
            else if (shellStyle == StringShellQuoting::eWindowsCMD) {
                // @todo - NO IDEA - I think "" replaces ", in cmd shell?
                if (i.ContainsAny ({' ', '\"'})) {
                    return "\"{}\""_f(i);
                    //                    return "\"{}\""_f(i.ReplaceAll ("\""sv, "\"\""sv));
                }
                else {
                    return i;
                }
            }
            else if (shellStyle == StringShellQuoting::eBash) {
                // @todo more complex - think I need to quote other stuff, but unclear
                if (i.ContainsAny ({' ', '\"'})) {
                    return "\"{}\""_f(i);
                    //return "\"{}\""_f(i.ReplaceAll ("\""sv, "\\\""sv));
                }
                else {
                    return i;
                }
            }
            return i;
        },
        " "sv);
}