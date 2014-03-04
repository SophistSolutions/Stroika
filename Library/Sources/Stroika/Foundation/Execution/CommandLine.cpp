/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "../Characters/SDKString.h"
#include    "../Characters/String_Constant.h"
#include    "../Characters/Tokenize.h"

#include    "CommandLine.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;

using   Characters::String_Constant;




/*
 ********************************************************************************
 ******************* Execution::InvalidCommandLineArgument **********************
 ********************************************************************************
 */
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument ()
    : StringException (String_Constant (L"Invalid Command Argument"))
    , fMessage ()
    , fArgument ()
{
}
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message)
    : StringException (message.As<wstring> ())
    , fMessage (message)
    , fArgument ()
{
}
Execution::InvalidCommandLineArgument::InvalidCommandLineArgument (const String& message, const String& argument)
    : StringException (message.As<wstring> ())
    , fMessage (message)
    , fArgument (argument)
{
}








/*
 ********************************************************************************
 ************************* Execution::ParseCommandLine **************************
 ********************************************************************************
 */
Sequence<String>  Execution::ParseCommandLine (const String& cmdLine)
{
    Sequence<String>    result;
    // super quickie hack impl
    result = Characters::Tokenize<String> (cmdLine, String_Constant(L" "));
    return result;
}

Sequence<String>  Execution::ParseCommandLine (int argc, const char* argv[])
{
    Require (argc >= 0);
    Sequence<String>  results;
    for (int i = 0; i < argc; ++i) {
        results.push_back (String::FromNarrowSDKString (argv[i]));
    }

    return results;
}

Sequence<String>  Execution::ParseCommandLine (int argc, const wchar_t* argv[])
{
    Require (argc >= 0);
    Sequence<String>  results;
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
namespace   {
    String  Simplify2Compare_ (const String& actualArg)
    {
        return actualArg.StripAll ([](Characters::Character c) -> bool { return c == '-' or c == '/'; }).ToLowerCase ();
    }
}

bool    Execution::MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern)
{
    // Command-line arguments must start with - or / (windows only)
    if (actualArg.empty ()) {
        return false;
    }
#if     qPlatform_Windows
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

bool    Execution::MatchesCommandLineArgument (const Iterable<String>& argList, const String& matchesArgPattern)
{
    return argList.FindFirstThat ([matchesArgPattern] (String i) ->bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); });
}

Optional<String>    Execution::MatchesCommandLineArgumentWithValue (const String& actualArg, const String& matchesArgPattern)
{
    Require (matchesArgPattern.GetLength () > 0 and matchesArgPattern[matchesArgPattern.GetLength () - 1] == '=');
    AssertNotImplemented ();
    // must first strip everything after the '=' in the actualarg, and then similar to first overload...
    return Optional<String> ();
}

Optional<String>    Execution::MatchesCommandLineArgumentWithValue (const Iterable<String>& argList, const String& matchesArgPattern)
{
    auto i = argList.FindFirstThat ([matchesArgPattern] (String i) -> bool { return Execution::MatchesCommandLineArgument (i, matchesArgPattern); });
    if (i != argList.end ()) {
        ++i;
        if (i == argList.end ()) {
            Execution::DoThrow (InvalidCommandLineArgument ());
        }
        else {
            return Optional<String> (*i);
        }
    }
    return Optional<String> ();
}
