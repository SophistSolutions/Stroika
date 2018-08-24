/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Frameworks_WebServer_Request_inl_
#define _Stroika_Frameworks_WebServer_Request_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

namespace Stroika::Frameworks::WebServer {

    /*
     ********************************************************************************
     ***************************** Implementation Details ***************************
     ********************************************************************************
     */
    inline String Request::GetHTTPVersion () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fHTTPVersion_;
    }
    inline String Request::GetHTTPMethod () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fMethod_;
    }
    inline void Request::SetHTTPMethod (const String& method)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fMethod_ = method;
    }
    inline IO::Network::URL Request::GetURL () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fURL_;
    }
    inline void Request::SetURL (const IO::Network::URL& url)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fURL_ = url;
    }
    inline Mapping<String, String> Request::GetHeaders () const
    {
        shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
        return fHeaders_;
    }
    inline void Request::SetHeaders (const Mapping<String, String>& headers)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fHeaders_ = headers;
    }
    inline void Request::AddHeader (const String& headerName, const String& value)
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        fHeaders_.Add (headerName, value);
    }
    inline Streams::InputStream<byte>::Ptr Request::GetInputStream ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fInputStream_;
    }

}

#endif /*_Stroika_Frameworks_WebServer_Request_inl_*/
