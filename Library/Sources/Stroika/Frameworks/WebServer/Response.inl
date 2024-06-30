/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ********************* Framework::WebServer::Response ***************************
     ********************************************************************************
     */
    template <Characters::IConvertibleToString T>
    inline void Response::write (T&& s)
    {
        if constexpr (Configuration::IAnyOf<T, String&&, String>) {
            write (static_cast<const String&> (s));
        }
        else {
            write (String{s});
        }
    }
    template <typename CHAR_T, typename... ARGS>
    inline void Response::write (const FormatString<CHAR_T>& f, ARGS&&... args)
    {
        write (f (args...));
    }
    inline void Response::writeln (const String& e)
    {
        Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{_fThisAssertExternallySynchronized};
        constexpr wchar_t                                      kEOL[] = L"\r\n";
        write (e);
        write (span{std::begin (kEOL), std::end (kEOL) - 1});
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    constexpr EnumNames<Frameworks::WebServer::Response::State> DefaultNames<Frameworks::WebServer::Response::State>::k{{{
        {Frameworks::WebServer::Response::State::ePreparingHeaders, L"Preparing-Headers"},
        {Frameworks::WebServer::Response::State::eHeadersSent, L"Headers-Sent"},
        {Frameworks::WebServer::Response::State::eCompleted, L"Completed"},
    }}};
}
