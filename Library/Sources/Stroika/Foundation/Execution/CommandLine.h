/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CommandLine_h_
#define _Stroika_Foundation_Execution_CommandLine_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/SDKChar.h"
#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Containers/Sequence.h"
#include "../Execution/StringException.h"

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            using Characters::String;
            using Containers::Iterable;
            using Containers::Sequence;

            /**
             */
            Sequence<String> ParseCommandLine (const String& cmdLine);
            Sequence<String> ParseCommandLine (int argc, const char* argv[]);
            Sequence<String> ParseCommandLine (int argc, const wchar_t* argv[]);

            /**
             *  This utility function takes a given 'matchesArgPattern' - which is basically what you declare your parameter to be, and returns
             *  true if the given actual argument matches.
             *
             *  Some parameters have assocaited values. When this is done as a following parameter, this code doesn't help to find them. But when the
             *  value is provided inline, in the form of a value after an '=' sign, the second overload will return that associated value, if any.
             *
             *  It is legal to call the 3-argument version (associatedArgResult) with a nullptr, if you don't wish the argument returned, just to test
             *  for an '=' version of the argument.
             *
             *  Note also - these patterns are not really regexp patterns, but strings which are matched in a case-insensative manner, and with punctuation
             *  squeezed out.
             *
             *  So - for example, you can call:
             *           MatchesCommandLineArgument (L"--runFast", L"run-fast") and it will return true.
             *
             *  To use the second (associatedArgResult) overload, the caller MUST specify a pattern in matchesArgPattern that ends with '='
             */
            bool MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern);
            bool MatchesCommandLineArgument (const Iterable<String>& argList, const String& matchesArgPattern);

            /**
             *  \par Example Usage
             *      \code
             *          optional<String> arg = MatchesCommandLineArgumentWithValue (cmdLine, L"x");
             *          if (arg.has_value ()) {
             *              sTimeMultiplier_ = String2Float<double> (*arg);
             *          }
             *      \endcode
             *
             *      MyProgram.exe --x 3
             *
             */
            optional<String> MatchesCommandLineArgumentWithValue (const String& actualArg, const String& matchesArgPattern);
            optional<String> MatchesCommandLineArgumentWithValue (const Iterable<String>& argList, const String& matchesArgPattern);

            /**
             *  TODO - REFACTOR/CLEANUP/BETTER ORGNAIZE EXCEPTIONS!!!!
             */
            class InvalidCommandLineArgument : public Execution::StringException {
            public:
                InvalidCommandLineArgument ();
                InvalidCommandLineArgument (const String& message);
                InvalidCommandLineArgument (const String& message, const String& argument);

            public:
                String fMessage;
                String fArgument;
            };
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif /*_Stroika_Foundation_Execution_CommandLine_h_*/
