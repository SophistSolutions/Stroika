/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CommandLine_h_
#define _Stroika_Foundation_Execution_CommandLine_h_    1

#include    "../StroikaPreComp.h"

#include    <vector>

#include    "../Configuration/Common.h"
#include    "../Characters/TChar.h"
#include    "../Characters/String.h"




namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {

            using   Characters::String;

            vector<String>  ParseCommandLine (int argc, const Characters::TChar* argv[]);

            //
            // This utility function takes a given 'matchesArgPattern' - which is basically what you declare your parameter to be, and returns
            // true if the given actual argument matches.
            //
            // Some parameters have assocaited values. When this is done as a following parameter, this code doesn't help to find them. But when the
            // value is provided inline, in the form of a value after an '=' sign, the second overload will return that associated value, if any.
            //
            // It is legal to call the 3-argument version (associatedArgResult) with a nullptr, if you don't wish the argument returned, just to test
            // for an '=' version of the argument.
            //
            // Note also - these patterns are not really regexp patterns, but strings which are matched in a case-insensative manner, and with punctuation
            // squeezed out.
            //
            // So - for example, you can call:
            //          MatchesCommandLineArgument (L"--runFast", L"run-fast") and it will return true.
            //
            // To use the second (associatedArgResult) overload, the caller MUST specify a pattern in matchesArgPattern that ends with '='
            //
            bool    MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern);
            bool    MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern, String* associatedArgResult);

        }
    }
}
#endif  /*_Stroika_Foundation_Execution_CommandLine_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
