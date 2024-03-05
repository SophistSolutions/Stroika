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
    inline CommandLine::CommandLine (const String& cmdLine)
        : fArgs_{ParseCommandLine (cmdLine)}
    {
    }
    inline CommandLine::CommandLine (int argc, char* argv[])
        : fArgs_{ParseCommandLine (argc, argv)}
    {
    }
    inline CommandLine::CommandLine (int argc, const char* argv[])
        : fArgs_{ParseCommandLine (argc, argv)}
    {
    }
    inline CommandLine::CommandLine (int argc, wchar_t* argv[])
        : fArgs_{ParseCommandLine (argc, argv)}
    {
    }
    inline CommandLine::CommandLine (int argc, const wchar_t* argv[])
        : fArgs_{ParseCommandLine (argc, argv)}
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
    inline Sequence<String> CommandLine::GetArguments (const Option& o) const
    {
        Require (o.fSupportsArgument);
        return get<Sequence<String>> (Get (o));
    }

}

#endif /*_Stroika_Foundation_Execution_CommandLine_inl_*/
