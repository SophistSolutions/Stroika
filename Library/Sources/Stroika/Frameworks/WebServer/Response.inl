/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ********************* Framework::WebServer::Response ***************************
     ********************************************************************************
     */
    inline void Response::write (const BLOB& b)
    {
        write (b.begin (), b.end ());
    }
    inline void Response::write (const wchar_t* e)
    {
        RequireNotNull (e);
        write (e, e + ::wcslen (e));
    }
    inline void Response::write (const String& e)
    {
        if (not e.empty ()) {
            wstring tmp{e.As<wstring> ()};
            write (Containers::Start (tmp), Containers::End (tmp));
        }
    }
    template <typename CHAR_T, typename... ARGS>
    inline void Response::write (const FormatString<CHAR_T>& f, ARGS&&... args)
    {
        write (f (args...));
    }
    inline void Response::writeln (const wchar_t* e)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
        RequireNotNull (e);
        const wchar_t kEOL[] = L"\r\n";
        write (e, e + ::wcslen (e));
        write (std::begin (kEOL), std::end (kEOL) - 1);
    }
    inline void Response::writeln (const String& e)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
        constexpr wchar_t                                      kEOL[] = L"\r\n";
        if (not e.empty ()) {
            wstring tmp{e.As<wstring> ()};
            write (Containers::Start (tmp), Containers::End (tmp));
        }
        write (std::begin (kEOL), std::end (kEOL) - 1);
    }
    inline void Response::EnterHeadMode ()
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
        Require (fState_ == State::ePreparingHeaders);
        fHeadMode_ = true;
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Frameworks::WebServer::Response::State> DefaultNames<Frameworks::WebServer::Response::State>::k{{{
        {Frameworks::WebServer::Response::State::ePreparingHeaders, L"Preparing-Headers"},
        {Frameworks::WebServer::Response::State::ePreparingBodyBeforeHeadersSent, L"Preparing-Body-Before-Headers-Sent"},
        {Frameworks::WebServer::Response::State::ePreparingBodyAfterHeadersSent, L"Preparing-Body-After-Headers-Sent"},
        {Frameworks::WebServer::Response::State::eCompleted, L"Completed"},
    }}};
}
