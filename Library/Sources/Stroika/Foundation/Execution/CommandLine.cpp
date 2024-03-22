/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/SDKString.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/ToString.h"
#include "../Containers/Set.h"

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
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument ()
    : Execution::RuntimeErrorException<>{"Invalid Command Argument"sv}
{
}
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message)
    : Execution::RuntimeErrorException<>{message.As<wstring> ()}
    , fMessage{message}
{
}
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message, const String& argument)
    : Execution::RuntimeErrorException<> (message.As<wstring> ())
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
#if qPlatform_Windows
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
            return Characters::Format (L"(-%c %s|--%s=%s)", *fSingleCharName, argName.As<wstring> ().c_str (),
                                       fLongName->As<wstring> ().c_str (), argName.As<wstring> ().c_str ());
        }
        else {
            return Characters::Format (L"(-%c|--%s)", *fSingleCharName, fLongName->As<wstring> ().c_str ());
        }
    }
    else if (this->fSingleCharName) {
        if (includeArg) {
            return Characters::Format (L"-%c %s", *fSingleCharName, argName.As<wstring> ().c_str ());
        }
        else {
            return Characters::Format (L"-%c", *fSingleCharName);
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
        sb << "fSingleCharName: "sv << *fSingleCharName << ","sv;
    }
    if (fLongName) {
        sb << "fLongName: "sv << *fLongName << ","sv;
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
CommandLine::CommandLine (const String& cmdLine)
{
    size_t        e = cmdLine.length ();
    StringBuilder curToken;
    Character     endQuoteChar = '\0';
    for (size_t i = 0; i < e; ++i) {
        Character c = cmdLine[i];
        if (endQuoteChar != '\0' and c == endQuoteChar) {
            fArgs_.Append (curToken.str ());
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
                    fArgs_.Append (curToken.str ());
                    curToken.clear ();
                }
            }
        }
    }
    if (curToken.size () != 0) {
        fArgs_.Append (curToken.str ());
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

String CommandLine::GenerateUsage (const String& exeName, Iterable<Option> options)
{
    const String  kIndent_ = "    "sv;
    StringBuilder sb;
    sb << "Usage: " << exeName;
    options.Apply ([&] (Option o) {
        sb << " [" << o.GetArgumentDescription (true) << "]"sv;
        if (o.fRepeatable) {
            if (o.fRequired) {
                sb << "+";
            }
            else {
                sb << "*";
            }
        }
        else if (not o.fRequired) {
            sb << "?";
        }
    });
    sb << "\n"sv;
    options.Apply ([&] (Option o) {
        if (o.fHelpOptionText) {
            sb << kIndent_ << o.GetArgumentDescription () << " " << *o.fHelpOptionText << "\n";
        }
    });
    return sb;
}

void CommandLine::Validate (Iterable<Option> options) const
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
        Execution::Throw (InvalidCommandLineArgument{"Required command line argument "sv + o->GetArgumentDescription () + " was not provided"sv});
    }
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
        Execution::Throw (InvalidCommandLineArgument{Characters::Format (L"Command line argument '%s' required but not provided",
                                                                         o.GetArgumentDescription ().As<wstring> ().c_str ())});
    }
    if (found and o.fSupportsArgument and o.fIfSupportsArgumentThenRequired and arguments.empty ()) {
        Execution::Throw (InvalidCommandLineArgument{Characters::Format (
            L"Command line argument %s provided, but without required argument", o.GetArgumentDescription ().As<wstring> ().c_str ())});
    }
    return make_tuple (found, arguments);
}

String CommandLine::ToString () const
{
    return Characters::ToString (this->fArgs_);
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
                    Execution::Throw (InvalidCommandLineArgument{
                        "Command line argument requires an argument to it, but none provided (= or following argument)"sv, ai});
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
                        Execution::Throw (InvalidCommandLineArgument{
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
