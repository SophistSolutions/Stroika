/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include    "../StroikaPreComp.h"

#include    "CommandLine.h"




using   namespace   Stroika::Foundation;
using   namespace   Stroika::Foundation::Execution;






vector<String>  Execution::ParseCommandLine (int argc, const Characters::TChar* argv[]) {
    Require (argc >= 0);
    vector<String>  results;
    results.reserve (argc);

    for (int i = 0; i < argc; ++i) {
        results.push_back (String::FromTString (argv[i]));
    }

    return results;
}


namespace   {
    String  Simplify2Compare_ (const String& actualArg) {
#if     qCompilerAndStdLib_lamba_closureCvtToFunctionPtrSupported
        return actualArg.StripAll ([](Characters::Character c) -> bool { return c == '-' or c == '/'; }).ToLowerCase ();
#else
        struct  FOO {
            static  bool    Simplify2Compare_LAMBDA_ (Characters::Character c)      {   return c == '-' or c == '/';    }
        };
        return actualArg.StripAll (FOO::Simplify2Compare_LAMBDA_).ToLowerCase ();
#endif
    }
}

bool    Execution::MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern) {
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

bool    Execution::MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern, String* associatedArgResult) {
    Require (matchesArgPattern.GetLength () > 0 and matchesArgPattern[matchesArgPattern.GetLength () - 1] == '=');
    AssertNotImplemented ();
    // must first strip everything after the '=' in the actualarg, and then similar to first overload...
    return false;
}
