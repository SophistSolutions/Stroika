/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Response_inl_
#define _Stroika_Frameworks_WebServer_Response_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
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
    inline void Response::writeln (const wchar_t* e)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        RequireNotNull (e);
        const wchar_t kEOL[] = L"\r\n";
        write (e, e + ::wcslen (e));
        write (std::begin (kEOL), std::end (kEOL));
    }
    inline void Response::writeln (const String& e)
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        constexpr wchar_t                            kEOL[] = L"\r\n";
        if (not e.empty ()) {
            wstring tmp{e.As<wstring> ()};
            write (Containers::Start (tmp), Containers::End (tmp));
        }
        write (std::begin (kEOL), std::end (kEOL));
    }
    inline void Response::EnterHeadMode ()
    {
        AssertExternallySynchronizedMutex::WriteLock critSec{*this};
        Require (fState_ == State::ePreparingHeaders);
        fHeadMode_ = true;
    }

}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Frameworks::WebServer::Response::State> DefaultNames<Frameworks::WebServer::Response::State>::k{
        EnumNames<Frameworks::WebServer::Response::State>::BasicArrayInitializer{{
            {Frameworks::WebServer::Response::State::ePreparingHeaders, L"Preparing-Headers"},
            {Frameworks::WebServer::Response::State::ePreparingBodyBeforeHeadersSent, L"Preparing-Body-Before-Headers-Sent"},
            {Frameworks::WebServer::Response::State::ePreparingBodyAfterHeadersSent, L"Preparing-Body-After-Headers-Sent"},
            {Frameworks::WebServer::Response::State::eCompleted, L"Completed"},
        }}};
}

#endif /*_Stroika_Frameworks_WebServer_Response_inl_*/
