/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Request_h_
#define _Stroika_Framework_WebServer_Request_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Common/Property.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/IO/Network/HTTP/Request.h"
#include "../../Foundation/IO/Network/URI.h"
#include "../../Foundation/Streams/InputStream.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using std::byte;

    using namespace Stroika::Foundation;
    using Characters::String;
    using DataExchange::InternetMediaType;

    /**
     *  \brief this represents a HTTP request object for the WebServer module
     * 
     *  TODO:
     *      @todo Probably/possibly hide the fInptuStream and other public
     *      @todo Maybe associated TextStream, and maybe readline method goes here
     *      @todo https://stroika.atlassian.net/browse/STK-726 - support Transfer-Encoding: chunked (as we do for response)
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Request : public IO::Network::HTTP::Request {
    private:
        using inherited = IO::Network::HTTP::Request;

    public:
        /**
         */
        Request ()               = delete;
        Request (const Request&) = delete;
        Request (Request&& src);
        Request (const Streams::InputStream<byte>::Ptr& inStream);

    public:
        nonvirtual Request& operator= (const Request&) = delete;

    public:
        // Quicky impl. Need to improve this significantly.
        // Can call multiple times - but first time it blcoks fetching data
        nonvirtual Memory::BLOB GetBody ();

    public:
        /**
         *  \brief mostly looks at Connection: ARG header, but if not there takes into account HTTP-version specific defaults.
         */
        Common::ReadOnlyProperty<bool> keepAliveRequested;

    public:
        /**
         *  @todo unclear if this SB const?
         */
        nonvirtual Streams::InputStream<byte>::Ptr GetInputStream ();

    public:
        /**
         *  This returns a (generally unseekable) stream of bytes. 
         *  In most cases, this stream will be artificially narrowed to just the body part of the request.
         *  When a Content-Length is known, that is used to limit the stream. When a Transfer-Coding is used, that will return the apparent (decoded) bytes (NYI).
         *
         *  \req This may ONLY be called after the headers have been set (read) -- TODO DOCUMENT HOW THIS CAN BE CHECKED AND VERIFIED - I THINK ANSWER RESIDES IN CONNECTION OBJECT
         */
        nonvirtual Streams::InputStream<byte>::Ptr GetBodyStream ();

    public:
        /**
        *  @see Characters::ToString ()
        */
        nonvirtual String ToString () const;

    public:
        [[deprecated ("Since Stroika 2.1b10, use rwHeaders().Set or Add or other")]] inline void AddHeader (const String& headerName, const String& value)
        {
            this->rwHeaders ().Set (headerName, value);
        }
        [[deprecated ("Since Stroika 2.1b10 use this->keepAliveRequested")]] inline bool GetKeepAliveRequested () const
        {
            return this->keepAliveRequested ();
        }
        [[deprecated ("Since Stroika 2.1b10 use this->contentLength")]] inline optional<uint64_t> GetContentLength () const
        {
            return this->headers ().contentLength;
        }
        [[deprecated ("Since Stroika v2.1b10 use contentType property")]] inline optional<InternetMediaType> GetContentType () const
        {
            return headers ().contentType;
        }
        [[deprecated ("Since Stroika v2.1b10 use httpVersion property")]] inline String GetHTTPVersion () const
        {
            return this->httpVersion ();
        }
        [[deprecated ("Since Stroika v2.1b10 use httpVersion property")]] inline void SetHTTPVersion (const String& versionOrVersionLabel)
        {
            this->httpVersion = versionOrVersionLabel;
        }
        [[deprecated ("Since Stroika v2.1b10 use httpMethod property")]] inline String GetHTTPMethod () const
        {
            return this->httpMethod ();
        }
        [[deprecated ("Since Stroika v2.1b10 use httpMethod property")]] inline void SetHTTPMethod (const String& method)
        {
            this->httpMethod = method;
        }
        [[deprecated ("Since Stroika v2.1b10 use url property")]] inline IO::Network::URI GetURL () const
        {
            return this->url ();
        }
        [[deprecated ("Since Stroika v2.1b10 use url property")]] inline void SetURL (const IO::Network::URI& newUrl)
        {
            this->url = newUrl;
        }
        [[deprecated ("Since Stroika v2.1b10 use headers property")]] IO::Network::HTTP::Headers GetHeaders () const
        {
            return this->headers ();
        }
        [[deprecated ("Since Stroika v2.1b10 use headers property")]] void SetHeaders (const IO::Network::HTTP::Headers& newHeaders)
        {
            this->rwHeaders = newHeaders;
        }

    private:
        Streams::InputStream<byte>::Ptr fInputStream_;
        Streams::InputStream<byte>::Ptr fBodyInputStream_;
        optional<Memory::BLOB>          fBody_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Request.inl"

#endif /*_Stroika_Framework_WebServer_Request_h_*/
