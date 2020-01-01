/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include "../Characters/SDKString.h"
#include "../Characters/StringBuilder.h"
#include "../Characters/String_Constant.h"

#include "CommandLine.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Execution;

using Characters::String_Constant;

/*
 ********************************************************************************
 ******************* Execution::InvalidCommandLineArgument **********************
 ********************************************************************************
 */
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument ()
    : Execution::RuntimeErrorException<> (L"Invalid Command Argument"sv)
    , fMessage ()
    , fArgument ()
{
}
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message)
    : Execution::RuntimeErrorException<> (message.As<wstring> ())
    , fMessage (message)
    , fArgument ()
{
}
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message, const String& argument)
    : Execution::RuntimeErrorException<> (message.As<wstring> ())
    , fMessage (message)
    , fArgument (argument)
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

    for (size_t i = 0; i < e; i++) {
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
                if (curToken.GetLength () != 0) {
                    result.Append (curToken.str ());
                    curToken.clear ();
                }
            }
        }
    }
    if (curToken.GetLength () != 0) {
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

Sequence<String> Execution::ParseCommandLine (int argc, const wchar_t* argv[])
{
    Require (argc >= 0);
    Sequence<String> results;
    for (int i = 0; i < argc; ++i) {
        results.push_back (argv[i]);
    }
    return results;
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
    return static_cast<bool> (argList.FindFirstThat ([matchesArgPattern] (String i) -> bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); }));
}

optional<String> Execution::MatchesCommandLineArgumentWithValue ([[maybe_unused]] const String& actualArg, [[maybe_unused]] const String& matchesArgPattern)
{
    Require (matchesArgPattern.GetLength () > 0 and matchesArgPattern[matchesArgPattern.GetLength () - 1] == '=');
    AssertNotImplemented ();
    // must first strip everything after the '=' in the actualarg, and then similar to first overload...
    return nullopt;
}

optional<String> Execution::MatchesCommandLineArgumentWithValue (const Iterable<String>& argList, const String& matchesArgPattern)
{
    auto i = argList.FindFirstThat ([matchesArgPattern] (String i) -> bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); });
    if (i != argList.end ()) {
        ++i;
        if (i == argList.end ())
            [[UNLIKELY_ATTR]]
            {
                Execution::Throw (InvalidCommandLineArgument ());
            }
        else {
            return optional<String> (*i);
        }
    }
    return nullopt;
}
