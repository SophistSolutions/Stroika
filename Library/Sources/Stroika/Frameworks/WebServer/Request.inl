/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        return this->httpVersion (); // DEPRECATED
    }
    inline String Request::GetHTTPMethod () const
    {
        return this->httpMethod (); // DEPRECATED
    }
    inline void Request::SetHTTPMethod (const String& method)
    {
        this->httpMethod = method; // deprecated
    }
    inline IO::Network::URI Request::GetURL () const
    {
        return this->url (); // DEPRECATED
    }
    inline void Request::SetURL (const IO::Network::URI& newUrl)
    {
        this->url = newUrl; // DEPRECATED
    }
    inline IO::Network::HTTP::Headers Request::GetHeaders () const
    {
        return this->headers (); // DEPRECATED
    }
    inline void Request::SetHeaders (const IO::Network::HTTP::Headers& newHeaders)
    {
        this->rwHeaders = newHeaders; // DEPRECATED
    }
    inline void Request::AddHeader (const String& headerName, const String& value)
    {
        // DEPRECATED
        this->rwHeaders ().Set (headerName, value);
    }
    inline Streams::InputStream<byte>::Ptr Request::GetInputStream ()
    {
        lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
        return fInputStream_;
    }

}

#endif /*_Stroika_Frameworks_WebServer_Request_inl_*/
