/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/SDKString.h"
#include "Stroika/Foundation/Characters/StringBuilder.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Set.h"
#include "Stroika/Foundation/Execution/Module.h"
#include "Stroika/Foundation/IO/FileSystem/Exception.h"

#include "CommandLine.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Traversal;

/*
 ********************************************************************************
 ******************* Execution::InvalidCommandLineArgument **********************
 ********************************************************************************
 */
InvalidCommandLineArgument::InvalidCommandLineArgument ()
    : RuntimeErrorException<>{"Invalid Command Argument"sv}
{
}
InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message)
    : RuntimeErrorException<>{message.As<wstring> ()}
    , fMessage{message}
{
}
InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message, const String& argument)
    : RuntimeErrorException<> (message.As<wstring> ())
    , fMessage{message}
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
#if qStroika_Foundation_Common_Platform_Windows
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
    auto i =
        argList.Find ([matchesArgPattern] (const String& i) -> bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); });
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
String CommandLine::Option::GetArgumentDescription (bool includeArg) const
{
    if (not this->fSupportsArgument) {
        includeArg = false;
    }
    String argName = this->fHelpArgName.value_or ("ARG"sv);
    if (fSingleCharName and fLongName) {
        if (includeArg) {
            return "(-{} {}|--{}={})"_f(*fSingleCharName, argName, *fLongName, argName);
        }
        else {
            return "(-{}|--{})"_f(*fSingleCharName, *fLongName);
        }
    }
    else if (this->fSingleCharName) {
        if (includeArg) {
            return "-{} {}"_f(*fSingleCharName, argName);
        }
        else {
            return "-{}"_f(*fSingleCharName);
        }
    }
    else if (fLongName) {
        if (includeArg) {
            return "--"sv + *fLongName + "="sv + argName;
        }
        else {
            return "--"sv + *fLongName;
        }
    }
    else {
        if (includeArg) {
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
        sb << "SingleCharName: "sv << *fSingleCharName << ","sv;
    }
    if (fLongName) {
        sb << "LongName: "sv << *fLongName << ","sv;
    }
    sb << "CaseSensitive: "sv << fLongNameCaseSensitive << ","sv;
    sb << "SupportsArgument: "sv << fSupportsArgument << ","sv;
    sb << "IfSupportsArgumentThenRequired: "sv << fIfSupportsArgumentThenRequired << ","sv;
    sb << "SupportsArgument: "sv << fSupportsArgument << ","sv;
    sb << "Repeatable: "sv << fRepeatable << ","sv;
    if (fHelpArgName) {
        sb << "HelpArgName: "sv << *fHelpArgName << ","sv;
    }
    if (fHelpOptionText) {
        sb << "HelpOptionText: "sv << *fHelpOptionText << ","sv;
    }
    sb << "}"sv;
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
CommandLine::CommandLine (const String& cmdLine)
    : fArgs_{ParseArgs_ (cmdLine)}
{
}

CommandLine::CommandLine (WrapInShell wrapInShell, const String& cmdLine)
{
    switch (wrapInShell) {
        case WrapInShell::eBash:
#if qStroika_Foundation_Common_Platform_Windows
        {
            // https://stroika.atlassian.net/browse/STK-1029
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
#if qStroika_Foundation_Common_Platform_Windows
        case WrapInShell::eWindowsCMD:
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
            break;
#endif
        default:
            RequireNotReached ();
    }
}

CommandLine::CommandLine (int argc, const char* argv[])
{
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

String CommandLine::GenerateUsage (const Iterable<Option>& options) const
{
    return GenerateUsage (GetAppName (), options);
}

String CommandLine::GenerateUsage (const String& exeName, const Iterable<Option>& options)
{
    const String  kIndent_ = "    "sv;
    StringBuilder sb;
    sb << "Usage: "sv << exeName;
    options.Apply ([&] (Option o) {
        sb << " [" << o.GetArgumentDescription (true) << "]"sv;
        if (o.fRepeatable) {
            if (o.fRequired) {
                sb << "+"sv;
            }
            else {
                sb << "*"sv;
            }
        }
        else if (not o.fRequired) {
            sb << "?"sv;
        }
    });
    sb << "\n"sv;
    size_t maxArgDescLen{0}; // used to tab-out descriptions so they align
    options.Apply ([&] (const Option& o) {
        if (o.fHelpOptionText) {
            maxArgDescLen = max (maxArgDescLen, o.GetArgumentDescription ().length ());
        }
    });
    options.Apply ([&] (const Option& o) {
        if (o.fHelpOptionText) {
            String argDesc = o.GetArgumentDescription ();
            sb << kIndent_ << argDesc << " "_k.Repeat (static_cast<unsigned int> (kIndent_.length () + maxArgDescLen - argDesc.size ()))
               << "/* " << *o.fHelpOptionText << " */\n";
        }
    });
    return sb;
}

void CommandLine::Validate (Iterable<Option> options) const
{
    if (auto oe = ValidateQuietly (options)) {
        Throw (*oe);
    }
}

nonvirtual optional<InvalidCommandLineArgument> CommandLine::ValidateQuietly (Iterable<Option> options) const
{
    Set<Option> all{options};
    Set<Option> unused{all};
    for (Iterator<String> argi = fArgs_.begin () + 1; argi != fArgs_.end (); ++argi) {
        if (not all.First ([&] (Option o) {
                if (optional<pair<bool, optional<String>>> oRes = ParseOneArg_ (o, &argi)) {
                    unused.RemoveIf (o);
                    return true;
                }
                return false;
            })) {
            Execution::Throw (InvalidCommandLineArgument{"Unrecognized argument: "sv + *argi, *argi});
        }
    }
    if (auto o = unused.First ([] (Option o) { return o.fRequired; })) {
        return InvalidCommandLineArgument{"Required command line argument "sv + o->GetArgumentDescription () + " was not provided"sv};
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
    bool             found = false;
    Sequence<String> arguments;
    for (Iterator<String> argi = fArgs_.begin () + 1; argi != fArgs_.end (); ++argi) {
        if (optional<pair<bool, optional<String>>> oRes = ParseOneArg_ (o, &argi)) {
            if (oRes->first) {
                found = true;
            }
            if (oRes->second) {
                arguments += *oRes->second;
            }
            if (not o.fRepeatable) {
                break; // no need to keep looking
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

optional<pair<bool, optional<String>>> CommandLine::ParseOneArg_ (const Option& o, Iterator<String>* argi)
{
    RequireNotNull (argi);
    Require (not argi->Done ());

    String ai = **argi;
    if (o.fSingleCharName and ai.length () == 2 and ai[0] == '-' and ai[1] == o.fSingleCharName) {
        if (o.fSupportsArgument) {
            ++(*argi);
            if ((*argi).Done ()) {
                if (o.fIfSupportsArgumentThenRequired) {
                    Throw (InvalidCommandLineArgument{"Command line argument requires an argument to it, but none provided (= or following argument)"sv, ai});
                }
                return make_pair (true, nullopt);
            }
            else {
                return make_pair (true, **argi);
            }
        }
        return make_pair (true, nullopt);
    }

    // this isn't right!!! - in case where no argument supported - must match all of string (and if next char not =)
    // but its CLOSE--LGP 2024-03-05
    if (o.fLongName and ai.length () >= 2 + o.fLongName->size () and ai[0] == '-' and ai[1] == '-' and
        String::EqualsComparer{o.fLongNameCaseSensitive}(ai.SubString (2, o.fLongName->size () + 2), *o.fLongName)) {
        if (o.fSupportsArgument) {
            // see if '=' follows longname
            String restOfArgi = ai.SubString (2 + o.fLongName->size ());
            if (restOfArgi.size () >= 1 and restOfArgi[0] == '=') {
                return make_pair (true, restOfArgi.SubString (1));
            }
            else {
                ++(*argi);
                if ((*argi).Done ()) {
                    if (o.fIfSupportsArgumentThenRequired) {
                        Throw (InvalidCommandLineArgument{
                            "Command line argument requires an argument to it, but none provided (= or following argument)"sv, ai});
                    }
                    return make_pair (true, nullopt);
                }
                else {
                    return make_pair (true, **argi);
                }
            }
        }
        return make_pair (true, nullopt);
    }
    // anything that cannot be an option (-x or --y...) is skipped, but anything else - that could be a plain filename (even a bare '-') is matched as 'argument'
    if (not o.fSingleCharName and not o.fLongName and o.fSupportsArgument and not(ai.size () >= 2 and ai.StartsWith ("-"sv))) {
        // note we add the argument, but don't set 'found'
        return make_pair (false, **argi);
    }
    return nullopt;
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