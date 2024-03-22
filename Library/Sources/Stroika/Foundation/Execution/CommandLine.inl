/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_CommandLine_inl_
#define _Stroika_Foundation_Execution_CommandLine_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Foundation::Execution {

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
        return get<bool> (Get (o));
    }
    inline optional<String> CommandLine::GetArgument (const Option& o) const
    {
        Require (o.fSupportsArgument);
        auto r = get<Sequence<String>> (Get (o));
        return r.empty () ? optional<String>{} : r[0];
    }
    inline Sequence<String> CommandLine::GetArguments () const
    {
        return this->fArgs_;
    }
    inline Sequence<String> CommandLine::GetArguments (const Option& o) const
    {
        Require (o.fSupportsArgument);
        return get<Sequence<String>> (Get (o));
    }
    inline String CommandLine::ToString () const
    {
        return Characters::ToString (this->fArgs_);
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

#endif /*_Stroika_Foundation_Execution_CommandLine_inl_*/
