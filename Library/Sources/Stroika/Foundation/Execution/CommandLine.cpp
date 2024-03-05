/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/Format.h"
#include "../Characters/SDKString.h"
#include "../Characters/StringBuilder.h"
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
 ************************* Execution::ParseCommandLine **************************
 ********************************************************************************
 */
Sequence<String> Execution::ParseCommandLine (const String& cmdLine)
{
    using namespace Characters;

    Sequence<String> result;

    size_t e = cmdLine.length ();

    StringBuilder curToken;
    Character     endQuoteChar = '\0';

    for (size_t i = 0; i < e; ++i) {
        Character c = cmdLine[i];
        if (endQuoteChar != '\0' and c == endQuoteChar) {
            result.Append (curToken.str ());
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
                    result.Append (curToken.str ());
                    curToken.clear ();
                }
            }
        }
    }
    if (curToken.size () != 0) {
        result.Append (curToken.str ());
    }
    return result;
}

Sequence<String> Execution::ParseCommandLine (int argc, const char* argv[])
{
    Require (argc >= 0);
    Sequence<String> results;
    for (int i = 0; i < argc; ++i) {
        results.push_back (String::FromNarrowSDKString (argv[i]));
    }
    return results;
}

Sequence<String> Execution::ParseCommandLine (int argc, char* argv[])
{
    return ParseCommandLine (argc, const_cast<const char**> (argv));
}

Sequence<String> Execution::ParseCommandLine (int argc, const wchar_t* argv[])
{
    Require (argc >= 0);
    Sequence<String> results;
    for (int i = 0; i < argc; ++i) {
        results.push_back (argv[i]);
    }
    return results;
}

Sequence<String> Execution::ParseCommandLine (int argc, wchar_t* argv[])
{
    return ParseCommandLine (argc, const_cast<const wchar_t**> (argv));
}

/*
 ********************************************************************************
 ****************** Execution::MatchesCommandLineArgument ***********************
 ********************************************************************************
 */
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

/*
 ********************************************************************************
 ************************** CommandLine::Option *********************************
 ********************************************************************************
 */
String CommandLine::Option::GetArgumentDescription () const
{
    if (fSingleCharName and fLongName) {
        return Characters::Format (L"(%c|%s)", *fSingleCharName, fLongName->As<wstring> ().c_str ());
    }
    else if (this->fSingleCharName) {
        return Characters::Format (L"%c", *fSingleCharName);
    }
    else if (fLongName) {
        return *fLongName;
    }
    else {
        return String{};
    }
}

/*
 ********************************************************************************
 ********************************** CommandLine *********************************
 ********************************************************************************
 */
CommandLine::CommandLine (const String& cmdLine)
    : fArgs_{ParseCommandLine (cmdLine)}
{
}
CommandLine::CommandLine (int argc, const char* argv[])
    : fArgs_{ParseCommandLine (argc, argv)}
{
}
CommandLine::CommandLine (int argc, const wchar_t* argv[])
    : fArgs_{ParseCommandLine (argc, argv)}
{
}

void CommandLine::Validate (Iterable<Option> options) const
{
    Set<Option> all{options};
    Set<Option> unused{all};
    for (Iterator<String> argi = fArgs_.begin (); argi != fArgs_.end (); ++argi) {
       if ( all.First ([&] (Option o) {
            if (optional<pair<bool, optional<String>>> oRes = ParseOneArg_ (o, &argi)) {
                unused.RemoveIf (o);
                return true;
            }
                return false;
        })) {
        
       }
       else {
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
    for (Iterator<String> argi = fArgs_.begin (); argi != fArgs_.end (); ++argi) {
        if (optional<pair<bool, optional<String>>> oRes = ParseOneArg_ (o, &argi)) {
            if (oRes->first) {
                found = true;
            }
            if (oRes->second) {
                arguments += *oRes->second;
            }
        }
        if (found and not o.fRepeatable) {
            break; // no need to keep looking
        }
    }
    if (o.fRequired and not found) {
        Execution::Throw (InvalidCommandLineArgument{Characters::Format (L"Command line argument %s required but not provided",
                                                                         o.GetArgumentDescription ().As<wstring> ().c_str ())});
    }
    if (found and o.fSupportsArgument and o.fIfSupportsArgumentThenRequired and arguments.empty ()) {
        Execution::Throw (InvalidCommandLineArgument{Characters::Format (
            L"Command line argument %s provided, but without required argument", o.GetArgumentDescription ().As<wstring> ().c_str ())});
    }
    return make_tuple (found, arguments);
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
    if (o.fLongName and ai.length () >= 2 + o.fLongName->size () and ai[0] == '-' and ai[1] == '-' and
        ai.SubString (2, o.fLongName->size ()) == o.fLongName) {
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
    if (not o.fSingleCharName and not o.fLongName and o.fSupportsArgument and not ai.StartsWith ("-"sv)) {
        // note we add the argument, but don't set 'found'
        return make_pair (false, **argi);
    }
    return nullopt;
}
