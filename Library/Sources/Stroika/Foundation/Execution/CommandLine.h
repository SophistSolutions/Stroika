/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CommandLine_h_
#define _Stroika_Foundation_Execution_CommandLine_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <filesystem>

#include "Stroika/Foundation/Characters/Number.h"
#include "Stroika/Foundation/Characters/SDKChar.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/StdCompat.h"
#include "Stroika/Foundation/Containers/Sequence.h"
#include "Stroika/Foundation/Execution/Exceptions.h"

/*
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

/**
 *  \brief qStroika_Foundation_Execution_CommandLine_SupportsThisProcess - CommandLine::ThisProcess () is there: the platform has
 *         a way to read a program's arguments outside main (glibc on Linux, MacOS, Windows)
 */
#ifndef qStroika_Foundation_Execution_CommandLine_SupportsThisProcess
#if (qStroika_Platform_Linux and defined(__GLIBC__)) or qStroika_Platform_MacOS or qStroika_Platform_Windows
#define qStroika_Foundation_Execution_CommandLine_SupportsThisProcess 1
#else
#define qStroika_Foundation_Execution_CommandLine_SupportsThisProcess 0
#endif
#endif

namespace Stroika::Foundation::Time {
    class Duration;
}

namespace Stroika::Foundation::Execution {

    using Characters::String;
    using Containers::Iterable;
    using Containers::Sequence;

    /**
     *  \todo Perhaps refactor slightly, so easy to tell one kind of issue from another.
     */
    class InvalidCommandLineArgument : public Execution::Exception<runtime_error> {
    public:
        InvalidCommandLineArgument ();
        InvalidCommandLineArgument (const String& message);
        InvalidCommandLineArgument (const String& message, const String& argument);

    public:
        const String fArgument;
    };

    /**
     *  \brief a type CommandLine::OptionValues::GetArgument<T> reads an option's argument as
     *
     *  String (the text as given), a number (read by Characters::Number::Parse<T>: the whole word, in range), filesystem::path,
     *  or Time::Duration (ISO-8601, as PT10S).
     */
    template <typename T>
    concept ICommandLineArgumentType = Common::IAnyOf<T, String, filesystem::path, Time::Duration> or floating_point<T> or
                                       (integral<T> and not Common::IAnyOf<T, bool, char, wchar_t, char8_t, char16_t, char32_t>);

    /**
     *  Take in a 'command line' specification (typically from main (argc,argv), but also used as arguments to ProcessRunner), 
     *  and define 'Option' objects and lookup if given arguments
     *  are 'present' in the commandline (and grab associated arguments).
     * 
     *  Supports -o and --output-file options, repeated options, an option's argument as -o ARG, --output-file ARG or
     *  --output-file=ARG (not -o=ARG), positional arguments, and -- to end the options.
     *
     *  inspired partly by https://man7.org/linux/man-pages/man3/getopt.3.html
     * 
     *  \par Example Usage (in application main)
     *      \code
     *          uint16_t portNumber;
     *
     *          const CommandLine::Option kPortO_{
     *              .fLongName = "port"sv, .fSupportsArgument = true, .fDefaultArgument = "8080"sv, .fHelpOptionText = "specify webserver listen port"sv};
     *          const CommandLine::Option kQuitAfterO_{
     *              .fLongName = "quit-after"sv, .fSupportsArgument = true, .fHelpOptionText = "automatically quit after <argument> seconds"sv};
     *          const Sequence<CommandLine::Option> kAllOptions_{StandardCommandLineOptions::kHelp, kPortO_, kQuitAfterO_};
     *
     *          CommandLine cmdLine{argc, argv};
     *          try {
     *              CommandLine::OptionValues args = cmdLine.ApplyOptions (kAllOptions_);
     *              if (args.Has (StandardCommandLineOptions::kHelp)) {
     *                  cerr << cmdLine.GenerateUsage (kAllOptions_);
     *                  return EXIT_SUCCESS;
     *              }
     *              portNumber = args.GetArgumentValue<uint16_t> (kPortO_); // throws if --port is not a uint16_t
     *          }
     *          catch (const InvalidCommandLineArgument& e) {
     *              cerr << Characters::ToString (e) << endl;
     *              cerr << cmdLine.GenerateUsage (kAllOptions_);
     *              return EXIT_FAILURE;
     *          }
     *      \endcode
     * 
     *  \par Example Usage (args to ProcessRunner)
     *      \code
     *          ProcessRunner p{CommandLine{WrapInShell::eBash, "echo $USER"}};
     *      \endcode
     * 
     *  TODO:
     *      o   \todo find some way to better handle std::filesystem::path arguments (handle quoting, normalizing paths so they work better cross platform if needed/helpful (e.g. /cygrdrive/c/???)
     * 
     */
    class CommandLine {
    public:
        /**
         *  Used as optional CTOR argument, to create a CommandLine with 
         *      bash -c "actual string arg"
         *  or
         *      cmd /C "actual string arg"
         */
        enum class WrapInShell {
#if qStroika_Platform_Windows
            eWindowsCMD,
#endif
            eBash,
        };

    public:
        /**
         *  Unlike most other Stroika APIs, plain 'char' here for char*, is interpreted as being in the SDK code page 
         *  (current locale - like SDKChar if narrow).
         * 
         *  CommandLine{STRING} parsing the string into individual components the way a generic shell would (space separation)
         *  and respecting quote characters).
         * 
         *  CommandLine{WrapInShell, STRING} really doesn't parse the string (except to add quotes as needed), but creates
         *  a CommandLine that will allow the argument shell to parse the command (e.g. bash -c "arguments").
         * 
         *  CommandLine{Sequence<String>} just captures that sequence of arguments and does not processing/parsing.
         * 
         *  CommandLine{argc,argv} are meant for being called from main, and also do no processing (besides treating the
         *  char* strings as SDKChar and mapping them from the OS codepage to UNICODE). What that encoding is differs by OS
         *  (see SDKChar): on Linux it is the global locale's - UTF-8 in the "C" locale every program starts in, so a UTF-8
         *  argument needs no locale set first; to read arguments in the encoding LANG/LC_* name instead, call
         *  Common::UsePlatformDefaultLocaleAsDefaultLocale () before constructing the CommandLine.
         *
         *  On Windows, main's narrow argv holds each argument converted to the code page, which loses any character the code
         *  page lacks. So given main's own argv - argc and argv are the C runtime's __argc and __argv - CommandLine{argc, argv}
         *  takes the arguments from the UTF-16 command line instead (GetCommandLineW (), split by CommandLineToArgvW), if it
         *  splits into arguments that each convert to just what argv holds; otherwise argv's, as given (setargv.obj expands
         *  wildcards, and CommandLineToArgvW's quoting rules differ from the C runtime's in rare cases). So the result is always
         *  argv's arguments - with any characters the code page lost given back.
         *
         *  \par Example Usage (main's arguments - on Windows whole too, by the __argc/__argv rule above)
         *      \code
         *          int main (int argc, const char* argv[])
         *          {
         *              CommandLine cmdLine{argc, argv};      // Windows: read from the UTF-16 command line instead (so sneaky, but it works)
         *                                                    // Linux: the locale's encoding (UTF-8 in "C"); MacOS: UTF-8
         *              ...
         *      \endcode
         *
         *  \par Example Usage (wmain on Windows: argv itself whole, in UTF-16 - for code that also reads or copies argv)
         *      \code
         *          #if qStroika_Platform_Windows
         *          int wmain (int argc, const wchar_t* argv[])   // unambiguous UTF-16
         *          #else
         *          int main (int argc, const char* argv[])       // Linux: the locale's encoding (UTF-8 in "C"); MacOS: UTF-8
         *          #endif
         *          {
         *              CommandLine cmdLine{argc, argv};
         *              ...
         *      \endcode
         */
        CommandLine ()                   = delete;
        CommandLine (const CommandLine&) = default;
        CommandLine (const String& cmdLine);
        CommandLine (WrapInShell wrapInShell, const String& cmdLine);
        CommandLine (const Sequence<String>& cmdLine);
        CommandLine (int argc, char* argv[]);
        CommandLine (int argc, const char* argv[]);
        CommandLine (int argc, wchar_t* argv[]);
        CommandLine (int argc, const wchar_t* argv[]);

#if qStroika_Foundation_Execution_CommandLine_SupportsThisProcess
    public:
        /**
         *  \brief This program's own arguments - main's, with no argc and argv to pass along - and on Windows never converted to the code page
         *
         *  Each platform's own source - none of them POSIX, which hands a program its arguments only through main:
         *      o   Linux (glibc): the argc and argv glibc passes each .init_array function, before main
         *      o   MacOS: _NSGetArgc () and _NSGetArgv ()
         *      o   Windows: a wmain program's arguments (the C runtime's __wargv); in any other, GetCommandLineW (), split by
         *          CommandLineToArgvW - whose quoting rules differ from the C runtime's in rare cases
         *
         *  Where there is no such source (a Linux C library other than glibc, other POSIX systems), there is no ThisProcess ():
         *  test qStroika_Foundation_Execution_CommandLine_SupportsThisProcess, with CommandLine{argc, argv} in its #else. Not
         *  /proc, which a system need not mount.
         *
         *  Narrow arguments (Linux, MacOS) are decoded as CommandLine{argc, argv} decodes them; and a program that rewrites its
         *  argv strings in place (as setproctitle does) gets what they hold now.
         *
         *  \par Example Usage
         *      \code
         *          int main ()
         *          {
         *              CommandLine cmdLine = CommandLine::ThisProcess ();
         *              ...
         *      \endcode
         */
        static CommandLine ThisProcess ();
#endif

    public:
        /**
         *  \par Example Usage
         *      \code
         *          const CommandLine::Option   kDashO = CommandLine::Option{.fSingleCharName = 'o', .fSupportsArgument = true };
         *      \endcode
         * 
         *  An Option with neither fSingleCharName nor fLongName is positional (IsPositionArgument ()): it names no option, but
         *  takes a word of the command line that is not an option, its place given by the order of the options passed to
         *  ApplyOptions. Its argument is that word, so fSupportsArgument and fIfSupportsArgumentThenRequired do not apply to it.
         *
         *  \pre not fRepeatable or fSupportsArgument or IsPositionArgument ()
         *  \pre not fSupportsArgument or fIfSupportsArgumentThenRequired or fLongName (an optional argument is given only as --long=VALUE)
         *  \pre not fEnvironmentVariable or fSupportsArgument or IsPositionArgument ()
         *  \pre not fDefaultArgument or fSupportsArgument or IsPositionArgument ()
         *  \pre not (fRequired and fDefaultArgument) - a default would make the option never missing
         *  \pre fAllowedValues.empty () or fSupportsArgument or IsPositionArgument ()
         *  \pre no two fAllowedValues the same (matched as fAllowedValuesCaseSensitive says), and fDefaultArgument, if any, one of them
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
             *  Look for argument after option (note this refers to named arguments, NOT positional arguments - see IsPositionalArgument ())
             * 
             *  if true, and long-form option, look for -OPT=XXX and copy out XXX as the argument
             *  if true, and either form option given, if no =, look for next argi, and if there, use that as argument (unless
             *  the argument is optional - see fIfSupportsArgumentThenRequired).
             * 
             *  But either way, this refers to supporting NAMED arguments, not POSITIONAL arguments.
             */
            bool fSupportsArgument{false};

            /**
             *  Typically, an option that takes an argument, that argument is required. But rarely - you might want an option that takes
             *  an argument that is optional.
             *
             *  An optional argument is given only as --long=VALUE, as getopt: the next word is never taken as it, since it may
             *  just as well be the next option or a positional argument. So such an option needs an fLongName.
             */
            bool fIfSupportsArgumentThenRequired{true};

            /**
             *  If the same option can be given more than once - to gather several arguments (ApplyOptions fails when an option
             *  that is not repeatable is given twice). A repeatable positional option takes every positional word left, so it
             *  must be the last positional option passed to ApplyOptions.
             */
            bool fRepeatable{false};

            /**
             *  If true, ApplyOptions fails when this option is not given - unless an option with fWaivesRequired is.
             */
            bool fRequired{false};

            /**
             *  If true, and this option is given, ApplyOptions does not require the fRequired options - so --help and --version
             *  (StandardCommandLineOptions::kHelp and kVersion set it) work with a required option missing. It waives only
             *  that: an unrecognized argument is still an error.
             */
            bool fWaivesRequired{false};

            /**
             *  If this option is not given, its argument is this environment variable's value - as Execution::kEnvironment holds
             *  it - if that is set and not empty. So a setting can come from the environment instead, as a database password
             *  the command line (which ps shows) should not carry. It then satisfies fRequired, and Has () is true; GenerateUsage
             *  names the variable on the option's help line.
             *
             *  \par Example Usage
             *      \code
             *          const CommandLine::Option kDbO_{.fLongName = "db"sv, .fSupportsArgument = true, .fEnvironmentVariable = "MY_APP_DB"sv};
             *      \endcode
             */
            optional<String> fEnvironmentVariable;

            /**
             *  If this option is not given, nor its fEnvironmentVariable set, its argument is this - read by GetArgument<T> as a T,
             *  like any argument (so a default that is not a T throws there, naming it). GenerateUsage shows it on the option's
             *  help line, so the help and the program cannot disagree. Has () stays false: the program can still tell the option
             *  was not given.
             *
             *  \par Example Usage
             *      \code
             *          const CommandLine::Option kPortO_{.fLongName = "port"sv, .fSupportsArgument = true, .fDefaultArgument = "8080"sv};
             *          uint16_t                  port = args.GetArgumentValue<uint16_t> (kPortO_); // always safe to call as the default is provided.
             *      \endcode
             */
            optional<String> fDefaultArgument;

            /**
             *  \brief for enumerated value options
             * 
             *  If not empty, the only words this option's argument may be.
             *  ApplyOptions refuses any other (from the command line
             *  or fEnvironmentVariable), naming these, and GenerateUsage lists them on the option's help line, in this order. The
             *  argument then reads as the word listed here, whatever case was typed (unless fAllowedValuesCaseSensitive) - still
             *  a String: what each word means is the program's to say.
             *
             *  \par Example Usage
             *      \code
             *          const CommandLine::Option kSwitchO_{.fLongName = "switch"sv, .fSupportsArgument = true, .fAllowedValues = {"on"sv, "off"sv}};
             *          optional<bool>            switchTo;
             *          if (optional<String> o = args.GetArgument (kSwitchO_)) {
             *              switchTo = *o == "on"sv;    // --switch ON reads as "on"; --switch maybe never gets here
             *          }
             *      \endcode
             */
            Sequence<String> fAllowedValues;

            /**
             *  How an argument is matched against fAllowedValues - by default ignoring case.
             */
            Characters::CompareOptions fAllowedValuesCaseSensitive{Characters::eCaseInsensitive};

            /**
             *  DEPRECATED since Stroika v3.0d26 - ApplyOptions ignores it, placing positional options in the order passed.
             *
             *  Not marked [[deprecated]]: on a field of an aggregate, g++ (13-15) and clang (18-21) warn wherever an Option is
             *  initialized or compared, not only where this field is named.
             */
            optional<size_t> fSkipFirstNArguments;

            /**
             *  If provided, its the name used in generating help, for the argument to this option.
             * 
             *  This is HELPFUL to use for positional parameters, so they can be shown in help.
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

            nonvirtual String GetArgumentDescription (bool includeArgName = false) const;

            /**
             *  Return true iff the argument (option) is not a named option (e..g not -n, or --blah, but positional in the argument list)
             */
            constexpr bool IsPositionArgument () const;

            nonvirtual String ToString () const;
        };

    public:
        /**
         *  Defined after CommandLine, for qCompilerAndStdLib_default_member_initializer_needed_within_enclosing_class_Buggy: it
         *  holds Options, whose default member initializers cannot be used before CommandLine is complete.
         */
        class OptionValues;

    public:
        /**
         *  \brief Read the command line against options - all of them, as what a word means can depend on any option - and check it
         *
         *  One pass over the words after argv[0], left to right:
         *      o   -x, --long and --long=VALUE name an option (--long matched as its fLongNameCaseSensitive says). If it
         *          fSupportsArgument, its argument is the text after = or else the next word, whatever that word is - but an
         *          optional argument (not fIfSupportsArgumentThenRequired) only the text after =, as getopt.
         *      o   a bare -- ends the options: every word after it is positional (so a file can be named -x)
         *      o   any other word - a bare - too - is positional: each positional option takes one, in the order they appear in
         *          options; a repeatable one takes all those left.
         *  Then an option not given takes its argument from its fEnvironmentVariable, if that is set - or else its fDefaultArgument.
         *  An argument one of an option's fAllowedValues matches reads as spelled there.
         *
         *  Throws InvalidCommandLineArgument if:
         *      o   a word starting with - names no option
         *      o   an option given --long=VALUE does not fSupportsArgument
         *      o   an option that fSupportsArgument and fIfSupportsArgumentThenRequired is the last word, so has no argument
         *      o   an option that is not fRepeatable is given twice
         *      o   a positional word is left with no positional option to take it
         *      o   an option's argument (from the command line or its fEnvironmentVariable) is not one of its fAllowedValues
         *      o   an fRequired option is not given (nor its fEnvironmentVariable set) - unless an option with fWaivesRequired is
         *
         *  So --help works with a required option missing (kHelp has fWaivesRequired): check for it after ApplyOptions.
         *
         *  \par Example Usage
         *      \code
         *          const initializer_list<CommandLine::Option> kAllOptions_{StandardCommandLineOptions::kHelp, ...others...};
         *          try {
         *              CommandLine::OptionValues args = cmdLine.ApplyOptions (kAllOptions_);
         *              if (args.Has (StandardCommandLineOptions::kHelp)) {
         *                  cerr << cmdLine.GenerateUsage (kAllOptions_) << endl;
         *                  return EXIT_SUCCESS;
         *              }
         *              ...     // the rest of the program, using args (OptionValues has no default constructor, so args lives in the try)
         *          }
         *          catch (const InvalidCommandLineArgument& e) {
         *              cerr << Characters::ToString (e) << endl;
         *              cerr << cmdLine.GenerateUsage (kAllOptions_) << endl;
         *              return EXIT_FAILURE;
         *          }
         *      \endcode
         *
         *  \pre options holds no option twice, and at most one fRepeatable positional option, which is the last positional one
         *  \pre no two options share an fSingleCharName, or an fLongName (as matched: ignoring case if either option does)
         *  \pre each option meets Option's preconditions
         *
         *  \see ApplyOptionsQuietly
         */
        [[nodiscard]] nonvirtual OptionValues ApplyOptions (const Iterable<Option>& options) const;

    public:
        /**
         *  \brief what ApplyOptionsQuietly returns: the OptionValues, or the InvalidCommandLineArgument ApplyOptions would have thrown
         */
        using ExpectedOptionValues = Common::StdCompat::expected<OptionValues, InvalidCommandLineArgument>;

    public:
        /**
         *  \brief ApplyOptions, but returning the InvalidCommandLineArgument instead of throwing it
         *
         *  \note OptionValues' GetArgument<T> and GetArguments<T> still throw InvalidCommandLineArgument, for an argument that is
         *        not a T.
         *
         *  \par Example Usage
         *      \code
         *          CommandLine::ExpectedOptionValues args = cmdLine.ApplyOptionsQuietly (kAllOptions_);
         *          if (not args) {
         *              cerr << Characters::ToString (args.error ()) << endl;
         *              cerr << cmdLine.GenerateUsage (kAllOptions_) << endl;
         *              return EXIT_FAILURE;
         *          }
         *          if (args->Has (StandardCommandLineOptions::kHelp)) {
         *              ...
         *      \endcode
         */
        [[nodiscard]] nonvirtual ExpectedOptionValues ApplyOptionsQuietly (const Iterable<Option>& options) const;

    public:
        /**
         *  \brief A usage line in the usual notation - [X] optional, X required, X... repeatable - then a line for each option
         *  with fHelpOptionText, fEnvironmentVariable, fDefaultArgument or fAllowedValues: its names and argument, as GNU and
         *  argparse show them, then that text, then [env: VARIABLE], [default: VALUE] and [possible values: A, B] (as clap shows them)
         *
         *  Lines end in \n - right for cerr or cout (see Characters::kEOL; String::NLToNative () converts it for a raw write).
         *
         *  For example:
         *      Usage: ArchiveUtility [-h|--help] [--list] [--outputDirectory=ARG] ARCHIVE-FILE [FILE]...
         *        -h, --help               Print out this help.
         *        --outputDirectory=ARG    (defaulting to .)
         *        ARCHIVE-FILE             ARCHIVE-FILE can be the single character - to designate stdin
         */
        nonvirtual String GenerateUsage (const Iterable<Option>& options) const;
        static String     GenerateUsage (const String& exeName, const Iterable<Option>& options);

    public:
        /*
         *  return 'argv[0]'
         *
         * if onlyBaseName is true, then return just the base name (e.g. 'myapp' from '/usr/bin/myapp' or 'C:\Program Files\MyApp\myapp.exe').
         */
        nonvirtual String GetAppName (bool onlyBaseName = true) const;

    public:
        /**
         *  \brief all the command line's words, argv[0] first, as given - the command line as a list, not read against any options
         */
        nonvirtual Sequence<String> GetArguments () const;

    public:
        /**
         *  Strategy/rules used when converting between a list of arguments to a single string.
         */
        enum class StringShellQuoting {
            eWindowsCMD,
            eBash
        };

    public:
        /**
         */
        nonvirtual optional<StringShellQuoting> GetStringShellQuoting () const;
        nonvirtual void                         SetStringShellQuoting (const optional<StringShellQuoting>& s);

    public:
        /**
         *  \par Example Usage (use default StringShellQuoting)
         *      \code
         *          String cmdLineText = cmdLine.As<String> ();
         *      \endcode
         *
         *  \par Example Usage (use no StringShellQuoting)
         *      \code
         *          String cmdLineText = cmdLine.As<String> (nullopt);
         *      \endcode
         *
         *  \par Example Usage (use bash style quoting)
         *      \code
         *          String cmdLineText = cmdLine.As<String> (StringShellQuoting::eBash);
         *      \endcode
         */
        template <Common::IAnyOf<String> T, typename... ARGS>
        nonvirtual T As (ARGS... args) const;

    public:
        /**
         * 
        */
        nonvirtual String ToString () const;

    private:
        /**
         *  This may throw, but NOT for not finding option o, just for finding o, but ill-formed.
         *  Returns nullopt if Option 'o' not found at this point in sequence, or the result if it is found.
         * 
         *  \notes
         *      o   outer optional refers to if the option was found.
         *      o   inner optional refers to if the option had an argument
         */
        static Common::StdCompat::expected<optional<optional<String>>, InvalidCommandLineArgument> ParseOneArg_ (const Option& o,
                                                                                                                 Traversal::Iterator<String>* argi);

    private:
        optional<StringShellQuoting> fShellStyleQuoting_;
        Sequence<String>             fArgs_;

    public:
        // DEPRECATED - kept only so code written for older Stroika versions keeps compiling
        [[deprecated (
            "Since Stroika v3.0d26 use ApplyOptions (options) - which validates, and returns what the options say")]] nonvirtual void
        Validate (const Iterable<Option>& options) const;
        [[deprecated ("Since Stroika v3.0d26 use ApplyOptionsQuietly (options)")]] nonvirtual optional<InvalidCommandLineArgument>
        ValidateQuietly (const Iterable<Option>& options) const;
        [[deprecated ("Since Stroika v3.0d26 use ApplyOptions (options).Has (o) / .GetArguments (o)")]] nonvirtual tuple<bool, Sequence<String>>
                                                                                                    Get (const Option& o) const;
        [[deprecated ("Since Stroika v3.0d26 use ApplyOptions (options).Has (o)")]] nonvirtual bool Has (const Option& o) const;
        [[deprecated ("Since Stroika v3.0d26 use ApplyOptions (options).GetArgument (o)")]] nonvirtual optional<String> GetArgument (const Option& o) const;
        [[deprecated ("Since Stroika v3.0d26 use ApplyOptions (options).GetArguments (o)")]] nonvirtual Sequence<String> GetArguments (const Option& o) const;
    };
    template <>
    String CommandLine::As<String> () const;
    template <>
    String CommandLine::As<String> (optional<CommandLine::StringShellQuoting> shellStyle) const;

    /**
     *  \brief The command line's words as ApplyOptions read them: which options were given, with their arguments
     *
     *  Made only by ApplyOptions (and ApplyOptionsQuietly), and only from a command line that satisfied the options - so a
     *  required option is always there (unless an fWaivesRequired option was given), and nothing here throws but GetArgument<T>
     *  and GetArguments<T>, for an argument that is not a T.
     *
     *  \par Example Usage
     *      \code
     *          CommandLine::OptionValues args = cmdLine.ApplyOptions (kAllOptions_);
     *          bool                      list = args.Has (kListO_);
     *          filesystem::path          in   = *args.GetArgument<filesystem::path> (kInputFileO_);   // kInputFileO_ fRequired
     *      \endcode
     */
    class CommandLine::OptionValues {
    public:
        OptionValues ()                    = delete;
        OptionValues (const OptionValues&) = default;

    public:
        /**
         *  \brief true iff o was given at least once (a positional option: iff a word was left for it), or its argument came
         *  from its fEnvironmentVariable - not for its fDefaultArgument alone
         *
         *  \pre o is one of the options ApplyOptions was given
         */
        nonvirtual bool Has (const Option& o) const;

    public:
        /**
         *  \brief o's argument (the first, if o is repeatable), as a T; nullopt if o was not given (nor its fEnvironmentVariable
         *  set, nor has it an fDefaultArgument), or was given with no argument
         *
         *  A positional option's argument is its word. T is String - the text as given - unless asked for otherwise; a
         *  Time::Duration is ISO-8601 (PT10S), so an argument counting seconds is GetArgument<double>.
         *
         *  Throws InvalidCommandLineArgument, naming o - and its fEnvironmentVariable or fDefaultArgument, if the argument came
         *  from there - if the argument is not a T (so never for String).
         *
         *  \pre o is one of the options ApplyOptions was given
         *
         *  \par Example Usage
         *      \code
         *          optional<uint16_t> portNumber = args.GetArgument<uint16_t> (kPortO_);
         *      \endcode
         */
        template <ICommandLineArgumentType T = String>
        nonvirtual optional<T> GetArgument (const Option& o) const;

    public:
        /**
         *  \brief o's argument as a T, when it must have one: o is fRequired, or has an fDefaultArgument
         *
         *  As GetArgument<T>, throwing as it does, but returning the T itself.
         *
         *  \pre o is one of the options ApplyOptions was given
         *  \pre o.fRequired or o.fDefaultArgument, and no option with fWaivesRequired (as --help) was given
         *
         *  \par Example Usage
         *      \code
         *          uint16_t portNumber = args.GetArgumentValue<uint16_t> (kPortO_); // kPortO_ has an fDefaultArgument
         *      \endcode
         */
        template <ICommandLineArgumentType T = String>
        nonvirtual T GetArgumentValue (const Option& o) const;

    public:
        /**
         *  \brief all of o's arguments, in command-line order, each as a T (empty if o was not given)
         *
         *  Throws InvalidCommandLineArgument, naming o, if any argument is not a T (so never for String).
         *
         *  \pre o is one of the options ApplyOptions was given
         */
        template <ICommandLineArgumentType T = String>
        nonvirtual Sequence<T> GetArguments (const Option& o) const;

    public:
        /**
         */
        nonvirtual String ToString () const;

    private:
        /*
         *  One option as ApplyOptions found it - made once there, never changed after: the option (one of the Options it was
         *  given), its argument (if any), and where that argument came from - the command line, the option's
         *  fEnvironmentVariable, or its fDefaultArgument. Not const members, which would cost the copy assignment Sequence needs.
         */
        struct Given_ {
            enum class From {
                eCommandLine,
                eEnvironment,
                eDefault
            };
            Option           fOption;
            optional<String> fArgument;
            From             fFrom{From::eCommandLine};
        };

    private:
        OptionValues (const Iterable<Option>& options, const Sequence<Given_>& given);

    private:
        // g's argument as a T, or throw InvalidCommandLineArgument naming where it came from
        template <ICommandLineArgumentType T>
        static T Convert_ (const Given_& g);

    private:
        // where g's argument came from, as an error message starts: "Command line argument --port", "Environment variable PORT
        // (for --port)", or "Default argument for --port"
        static String Source_ (const Given_& g);

    private:
        Sequence<Option> fOptions_; // the options ApplyOptions was given
        Sequence<Given_> fGiven_; // each option given, in command-line order - then those from their fEnvironmentVariable or fDefaultArgument

    private:
        friend class CommandLine;
    };
    template <>
    Time::Duration CommandLine::OptionValues::Convert_<Time::Duration> (const Given_& g);

    namespace StandardCommandLineOptions {
        static inline const CommandLine::Option kHelp{
            .fSingleCharName = 'h', .fLongName = "help"sv, .fWaivesRequired = true, .fHelpOptionText = "Print out this help."sv};
        static inline const CommandLine::Option kVersion{
            .fSingleCharName = 'v', .fLongName = "version"sv, .fWaivesRequired = true, .fHelpOptionText = "Print this application's version."sv};
    }

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "CommandLine.inl"

#endif /*_Stroika_Foundation_Execution_CommandLine_h_*/
