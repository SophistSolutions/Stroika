/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CommandLine_h_
#define _Stroika_Foundation_Execution_CommandLine_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Characters/SDKChar.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Execution {

    using Characters::String;
    using Containers::Iterable;
    using Containers::Sequence;

    /**
     *  \todo Perhaps refactor slightly, so easy to tell one kind of issue from another.
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

    /**
     *  Take in a 'command line' specification (typically from 'main'), and define 'Option' objects and lookup if given arguments
     *  are 'present' in the commandline (and grab associated arguments).
     * 
     *  Supports repeated option elements. Supports -o and --output-file formats
     *  Supports -o ARG and -o=ARG formats
     *
     *  inspired partly by https://man7.org/linux/man-pages/man3/getopt.3.html
     * 
     *  TODO:
     *      o   \todo find some way to better handle std::filesystem::path arguments (hande quoting, normalizing paths so they work better cross platform if needed/helpful (e.g. /cygrdrive/c/???)
     * 
     */
    class CommandLine {
    public:
        /**
         *  Unlike most other Stroika APIs, plain 'char' here for char*, is interpreted as being in the SDK code page (current locale - like SDKChar if narrow).
         */
        CommandLine ()                   = delete;
        CommandLine (const CommandLine&) = default;
        CommandLine (const String& cmdLine);
        CommandLine (const Sequence<String>& cmdLine);
        CommandLine (int argc, char* argv[]);
        CommandLine (int argc, const char* argv[]);
        CommandLine (int argc, wchar_t* argv[]);
        CommandLine (int argc, const wchar_t* argv[]);

    public:
        /**
         *  \par Example Usage
         *      \code
         *          const CommandLine::Option   kDashO = CommandLine::Option{.fSingleCharName = 'o', .fSupportsArgument = true };
         *      \endcode
         * 
         *  \note fSingleCharName is optional, and fLongName is also optional. Meaning its totally legal to supply no short name and no long-name (in which case its required to support fSupportsArgument)
         * 
         *  \req fSingleCharName or fLongName or fSupportsArgument
         *  \req not fRepeatable or fSupportsArgument
         * 
         *  \todo figure out if I can make this a literal type, so can be defined constexpr when not using fLongName (or maybe even with using stringview)
         */
        struct Option {
            optional<char>   fSingleCharName; // for -s
            optional<String> fLongName;       // for --long use

            /**
             * refers to long-name only - being case sensitive; defaults to eCaseInsensitive (appropriate for long names, less appropriate default for short - one letter - options).
             */
            Characters::CompareOptions fLongNameCaseSensitive{Characters::eCaseInsensitive};

            /**
             *  Look for argument after option.
             * 
             *  if true, and long-form option, look for -OPT=XXX and copy out XXX as the argument
             *  if true, and either form option given, if no =, look for next argi, and if there, use that as argument.
             */
            bool fSupportsArgument{false};

            /**
             *  Typically, an option that takes an argument, that argument is required. But rarely - you might want an option that takes
             *  an argument that is optional.
             */
            bool fIfSupportsArgumentThenRequired{true};

            /**
             *  If you can have the same option repeated multiple times. The only point of this would be for
             *  things to gather multiple arguments. Note that this can be used with no fSingleCharName and no fLongName, meaning it captures
             *  un-dash-decorated arguments.
             */
            bool fRepeatable{false};

            /**
             * If true, then Get (o) (and GetArgument (i)) will throw if this option isn't found in the commandline; though Has () will not throw.
             */
            bool fRequired{false};

            /**
             *  If provided, its the name used in generating help, for the argument to this option.
             */
            optional<String> fHelpArgName;

            /**
             *  If provided, its the name used in generating help, for this option.
             */
            optional<String> fHelpOptionText;

            bool operator== (const Option&) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
            DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif
            auto operator<=> (const Option&) const = default;
#if qCompilerAndStdLib_explicitly_defaulted_threeway_warning_Buggy
            DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdefaulted-function-deleted\"")
#endif

            nonvirtual String GetArgumentDescription (bool includeArg = false) const;

            nonvirtual String ToString () const;
        };

    public:
        /**
         *  Throw InvalidCommandLineArgument if arguments not fit with options.
         *  This checks for unrecognized arguments.
         * 
         *  \par Example Usage
         *      \code
         *          const initializer_list<Execution::CommandLine::Option> kAllOptions_{StandardCommandLineOptions::kHelp, ...others...};
         *          cmdLine.Validate (kAllOptions_);        // throws InvalidCommandLineArgument if bad args so catch/report usage
         *      \endcode
         */
        nonvirtual void Validate (Iterable<Option> options) const;

    public:
        /**
         */
        nonvirtual String GenerateUsage (const Iterable<Option>& options) const;
        static String     GenerateUsage (const String& exeName, const Iterable<Option>& options);

    public:
        /*
         *  return 'argv[0]'
         *  
         */
        nonvirtual String GetAppName (bool onlyBaseName = true) const;

    public:
        /*
         * return get<bool> true iff arg is present in command line.
         * Either way, get<Sequence<String>>> returns same as GetArguments ();
         *
         *  \see often simpler GetArgument ()
         */
        nonvirtual tuple<bool, Sequence<String>> Get (const Option& o) const;

    public:
        /*
         * Return true iff arguments (in this object) have that option set. Note this will not throw just because option is required and missing (but may if ill formed).
         */
        nonvirtual bool Has (const Option& o) const;

    public:
        /**
        *  \req o.fSupportsArgument
        * 
         *  \par Example Usage
         *      \code
         *          constexpr CommandLine::Option   kOutFileOption_ = CommandLine::Option{.fSingleCharName = 'o', .fSupportsArgument = true };
         *          CommandLine cmdLine {argc, argv};
         *          String file2Use = cmdLine.GetArgument (kOutFileOption_).value_or ("default-file-name.xml");
         *      \endcode
         */
        nonvirtual optional<String> GetArgument (const Option& o) const;

    public:
        /**
         *  overload with no arguments /0 - returns all commandline arguments.
         *  \req o.fSupportsArgument
         */
        nonvirtual Sequence<String> GetArguments () const;
        nonvirtual Sequence<String> GetArguments (const Option& o) const;

    public:
        /**
         * 
        */
        nonvirtual String ToString () const;

    private:
        /*
         *  This may throw, but NOT for not finding option o, just for finding o, but ill-formed.
         *  Returns nullopt if Option 'o' not found at this point in sequence, or the result if it is found.
         */
        static optional<pair<bool, optional<String>>> ParseOneArg_ (const Option& o, Traversal::Iterator<String>* argi);

    private:
        Sequence<String> fArgs_;
    };

    namespace StandardCommandLineOptions {
        static inline const CommandLine::Option kHelp{.fSingleCharName = 'h', .fLongName = "help"sv, .fHelpOptionText = "Print out this help."sv};
        static inline const CommandLine::Option kVersion{.fSingleCharName = 'v', .fLongName = "version"sv, .fHelpOptionText = "Print this application's version."sv};
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CommandLine.inl"

#endif /*_Stroika_Foundation_Execution_CommandLine_h_*/
