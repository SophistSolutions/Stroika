/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
    inline Response::State Response::GetState () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fState_;
    }
    inline IO::Network::HTTP::Status Response::GetStatus () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fStatus_;
    }
    inline InternetMediaType Response::GetContentType () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fContentType_;
    }
    inline Characters::CodePage Response::GetCodePage () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fCodePage_;
    }
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
        fBytes_.clear ();
    }
    inline bool Response::empty () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fBytes_.empty ();
    }
    inline const vector<byte>& Response::GetBytes () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fBytes_;
    }
    inline Response::ContentSizePolicy Response::GetContentSizePolicy () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fContentSizePolicy_;
    }

}

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<Frameworks::WebServer::Response::State> : EnumNames<Frameworks::WebServer::Response::State> {
        static constexpr EnumNames<Frameworks::WebServer::Response::State> k{
            EnumNames<Frameworks::WebServer::Response::State>::BasicArrayInitializer{
                {
                    {Frameworks::WebServer::Response::State::eInProgress, L"InProgress"},
                    {Frameworks::WebServer::Response::State::eInProgressHeaderSentState, L"InProgressHeaderSentState"},
                    {Frameworks::WebServer::Response::State::eCompleted, L"Completed"},
                }}};
        DefaultNames ()
            : EnumNames<Frameworks::WebServer::Response::State> (k)
        {
        }
    };
}

#endif /*_Stroika_Frameworks_WebServer_Response_inl_*/
