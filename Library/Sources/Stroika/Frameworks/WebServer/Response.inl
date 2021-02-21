/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        RequireNotNull (e);
        const wchar_t kEOL[] = L"\r\n";
        write (e, e + ::wcslen (e));
        write (std::begin (kEOL), std::end (kEOL));
    }
    inline void Response::writeln (const String& e)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        constexpr wchar_t                                  kEOL[] = L"\r\n";
        if (not e.empty ()) {
            wstring tmp{e.As<wstring> ()};
            write (Containers::Start (tmp), Containers::End (tmp));
        }
        write (std::begin (kEOL), std::end (kEOL));
    }
    inline void Response::clear ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fBodyBytes_.clear ();
    }
    inline bool Response::empty () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fBodyBytes_.empty ();
    }
    inline const vector<byte>& Response::GetBytes () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fBodyBytes_;
    }
    inline void Response::EnterHeadMode ()
    {
        fHeadMode_ = true;
    }

}

namespace Stroika::Foundation::Configuration {
#if !qCompilerAndStdLib_template_specialization_internalErrorWithSpecializationSignifier_Buggy
    template <>
#endif
    constexpr EnumNames<Frameworks::WebServer::Response::State> DefaultNames<Frameworks::WebServer::Response::State>::k{
        EnumNames<Frameworks::WebServer::Response::State>::BasicArrayInitializer{{
            {Frameworks::WebServer::Response::State::eInProgress, L"InProgress"},
            {Frameworks::WebServer::Response::State::eInProgressHeaderSentState, L"InProgressHeaderSentState"},
            {Frameworks::WebServer::Response::State::eCompleted, L"Completed"},
        }}};
}

#endif /*_Stroika_Frameworks_WebServer_Response_inl_*/
