/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
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
            if (optional<optional<String>> oRes = ParseOneArg_ (o, &argi)) {
                return true;
            }
        }
        return false;
    }
    inline optional<String> CommandLine::GetArgument (const Option& o) const
    {
        auto r = get<Sequence<String>> (Get (o));
        return r.empty () ? optional<String>{} : r[0];
    }
    inline Sequence<String> CommandLine::GetArguments () const
    {
        return this->fArgs_;
    }
    inline Sequence<String> CommandLine::GetArguments (const Option& o) const
    {
        return get<Sequence<String>> (Get (o));
    }
    inline auto CommandLine::GetStringShellQuoting () const -> optional<StringShellQuoting>
    {
        return fShellStyleQuoting_;
    }
    inline void CommandLine::SetStringShellQuoting (const optional<StringShellQuoting>& s)
    {
        fShellStyleQuoting_ = s;
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
