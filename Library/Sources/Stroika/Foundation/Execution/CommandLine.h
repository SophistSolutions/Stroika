/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CommandLine_h_
#define _Stroika_Foundation_Execution_CommandLine_h_ 1

#include "../StroikaPreComp.h"

#include "../Characters/SDKChar.h"
#include "../Characters/String.h"
#include "../Configuration/Common.h"
#include "../Containers/Sequence.h"
#include "../Execution/Exceptions.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::Execution {

    using Characters::String;
    using Containers::Iterable;
    using Containers::Sequence;

    /**
     */
    Sequence<String> ParseCommandLine (const String& cmdLine);
    Sequence<String> ParseCommandLine (int argc, char* argv[]);
    Sequence<String> ParseCommandLine (int argc, const char* argv[]);
    Sequence<String> ParseCommandLine (int argc, wchar_t* argv[]);
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
     *           MatchesCommandLineArgument ("--runFast", "run-fast") and it will return true.
     *
     *  To use the second (associatedArgResult) overload, the caller MUST specify a pattern in matchesArgPattern that ends with '='
     */
    bool MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern);
    bool MatchesCommandLineArgument (const Iterable<String>& argList, const String& matchesArgPattern);

    /**
     *  \par Example Usage
     *      \code
     *          optional<String> arg = MatchesCommandLineArgumentWithValue (cmdLine, "x");
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
    class InvalidCommandLineArgument : public Execution::RuntimeErrorException<> {
    public:
        InvalidCommandLineArgument ();
        InvalidCommandLineArgument (const String& message);
        InvalidCommandLineArgument (const String& message, const String& argument);

    public:
        String fMessage;
        String fArgument;
    };

    //NEW
    // inspired partly by https://man7.org/linux/man-pages/man3/getopt.3.html
    // DRAFT
    /**
     */
    class CommandLine {
    public:
        CommandLine (const String& cmdLine);
        CommandLine (int argc, char* argv[]);
        CommandLine (int argc, const char* argv[]);
        CommandLine (int argc, wchar_t* argv[]);
        CommandLine (int argc, const wchar_t* argv[]);

    public:
        /**
         *  \par Example Usage
         *      \code
         *          constexpr CommandLine::Option   kDashO = CommandLine::Option{.fSingleCharName = 'o', .fSupportsArgument = true, .fArgumentRequired = true };
         *      \endcode
         * 
         *  \note fSingleCharName is optional, and fLongName is also optional. Meaning its totally legal to supply no short name and no long-name (in which case its requried to support fSupportsArgument)
         * 
         *  \req fSingleCharName or fLongName or fSupportsArgument
         *  \req not fRepeatable or fSupportsArgument
         */
        struct Option {
            optional<char>   fSingleCharName; // for -s
            optional<String> fLongName;       // for --long use

            /**
             *  Look for argument after option.
             * 
             *  if true, and long-form option, look for -OPT=XXX and copy out XXX as the argument
             *  if true, and either form option given, if no =, look for next argi, and if there, use that as argument.
             */
            bool fSupportsArgument{false};

            bool fArgumentRequired{false};

            bool fRepeatable{false};
        };

    public:
        /**
         *  Throw InvalidCommandLineArgument if arguments not fit with options.
         *  This checks for unrecognized arguments.
         */
        nonvirtual void Validate (Iterable<Option> options) const;

    public:
        /*
         * return get<bool> true iff arg is present in command line.
         * Either way, get<Sequence<String>>> returns same as GetArguments ();
         */
        nonvirtual tuple<bool, Sequence<String>> Get (const Option& o) const;

    public:
        /*
         * Return true iff arguments (in this object) have that option set.
         */
        nonvirtual bool Has (const Option& o) const;

    public:
        /**
        *  \req o.fSupportsArgument
        * 
         *  \par Example Usage
         *      \code
         *          constexpr CommandLine::Option   kOutFileOption_ = CommandLine::Option{.fSingleCharName = 'o', .fSupportsArgument = true, .fArgumentRequired = true };
         *          CommandLine cmdLine {argc, argv};
         *          String file2Use = cmdLine.GetArgument (kOutFileOption_).value_or ("default-file-name.xml");
         *      \endcode
         */
        nonvirtual optional<String> GetArgument (const Option& o) const;

    public:
        /**
        *  \req o.fSupportsArgument
         */
        nonvirtual Sequence<String> GetArguments (const Option& o) const;

    private:
        Sequence<String> fArgs_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CommandLine.inl"

#endif /*_Stroika_Foundation_Execution_CommandLine_h_*/
