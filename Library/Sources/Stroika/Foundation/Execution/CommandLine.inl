/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ************************** CommandLine::Option *********************************
     ********************************************************************************
     */
    constexpr bool CommandLine::Option::IsPositionArgument () const
    {
        return not fSingleCharName.has_value () and not fLongName.has_value ();
    }

    /*
     ********************************************************************************
     ********************************** CommandLine *********************************
     ********************************************************************************
     */
    inline CommandLine::CommandLine (const Sequence<String>& cmdLine)
        : fArgs_{cmdLine}
    {
    }
    inline CommandLine::CommandLine (int argc, char* argv[])
        : CommandLine{argc, (const char**)argv}
    {
    }
    inline CommandLine::CommandLine (int argc, wchar_t* argv[])
        : CommandLine{argc, (const wchar_t**)argv}
    {
    }
    inline bool CommandLine::Has (const Option& o) const
    {
        for (Traversal::Iterator<String> argi = fArgs_.begin () + 1; argi != fArgs_.end (); ++argi) {
            if (auto oRes = ParseOneArg_ (o, &argi); oRes and *oRes) {
                return true;
            }
        }
        return false;
    }
    DISABLE_COMPILER_MSC_WARNING_START (4996);
    DISABLE_COMPILER_GCC_WARNING_START ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    inline optional<String> CommandLine::GetArgument (const Option& o) const
    {
        auto r = get<Sequence<String>> (Get (o));
        return r.empty () ? optional<String>{} : r[0];
    }
    inline Sequence<String> CommandLine::GetArguments (const Option& o) const
    {
        return get<Sequence<String>> (Get (o));
    }
    DISABLE_COMPILER_MSC_WARNING_END (4996);
    DISABLE_COMPILER_GCC_WARNING_END ("GCC diagnostic ignored \"-Wdeprecated-declarations\"");
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wdeprecated-declarations\"");
    inline Sequence<String> CommandLine::GetArguments () const
    {
        return this->fArgs_;
    }
    inline auto CommandLine::GetStringShellQuoting () const -> optional<StringShellQuoting>
    {
        return fShellStyleQuoting_;
    }
    inline void CommandLine::SetStringShellQuoting (const optional<StringShellQuoting>& s)
    {
        fShellStyleQuoting_ = s;
    }

    /*
     ********************************************************************************
     ************************* CommandLine::OptionValues ****************************
     ********************************************************************************
     */
    template <ICommandLineArgumentType T>
    optional<T> CommandLine::OptionValues::GetArgument (const Option& o) const
    {
        Require (fOptions_.Contains (o));
        if (optional<Given_> found = fGiven_.First ([&] (const Given_& g) { return g.fOption == o and g.fArgument.has_value (); })) {
            return Convert_<T> (o, *found->fArgument);
        }
        return nullopt;
    }
    template <ICommandLineArgumentType T>
    Sequence<T> CommandLine::OptionValues::GetArguments (const Option& o) const
    {
        Require (fOptions_.Contains (o));
        Sequence<T> result;
        for (const Given_& g : fGiven_) {
            if (g.fOption == o and g.fArgument) {
                result += Convert_<T> (o, *g.fArgument);
            }
        }
        return result;
    }
    template <ICommandLineArgumentType T>
    T CommandLine::OptionValues::Convert_ (const Option& o, const String& arg)
    {
        if constexpr (same_as<T, String>) {
            return arg;
        }
        else if constexpr (same_as<T, filesystem::path>) {
            return arg.As<filesystem::path> ();
        }
        else {
            // Time::Duration's is in CommandLine.cpp
            Common::StdCompat::expected<T, DataExchange::BadFormatException> r = Characters::Number::Parse<T> (arg);
            if (not r) {
                Execution::Throw (InvalidCommandLineArgument{
                    "Command line argument "sv + o.GetArgumentDescription (o.IsPositionArgument ()) + ": "sv + r.error ().template As<String> (), arg});
            }
            return *r;
        }
    }

    ////---deprecated
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] inline Sequence<String> ParseCommandLine (const String& cmdLine)
    {
        return CommandLine{cmdLine}.GetArguments ();
    }
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] inline Sequence<String> ParseCommandLine (int argc, char* argv[])
    {
        return CommandLine{argc, argv}.GetArguments ();
    }
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] inline Sequence<String> ParseCommandLine (int argc, const char* argv[])
    {
        return CommandLine{argc, argv}.GetArguments ();
    }
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] inline Sequence<String> ParseCommandLine (int argc, wchar_t* argv[])
    {
        return CommandLine{argc, argv}.GetArguments ();
    }
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] inline Sequence<String> ParseCommandLine (int argc, const wchar_t* argv[])
    {
        return CommandLine{argc, argv}.GetArguments ();
    }

    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] bool MatchesCommandLineArgument (const String& actualArg, const String& matchesArgPattern);
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] bool MatchesCommandLineArgument (const Iterable<String>& argList,
                                                                                                   const String& matchesArgPattern);
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] optional<String>
    MatchesCommandLineArgumentWithValue (const String& actualArg, const String& matchesArgPattern);
    [[deprecated ("Since Stroika v3.0d6 use CommandLine class")]] optional<String>
    MatchesCommandLineArgumentWithValue (const Iterable<String>& argList, const String& matchesArgPattern);

}
