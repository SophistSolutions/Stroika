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
#include "../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
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
     *  Maybe associated TextStream, and maybe readline method goes here
     *  Quicky impl. Need to improve this significantly. Probably/possibly hide the fInptuStream and other public
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Request : private Debug::AssertExternallySynchronizedLock {
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
         *  This can be given HTTP/1.0, 1.0, HTTP/1.1, or 1.1. Other values are allowed, but surprising.
         *  This typically returns "1.1"
         */
        Common::Property<String> httpVersion;

    public:
        /**
         */
        Common::Property<String> httpMethod;

    public:
        /**
         * This is the relative URL which appears at the start of an HTTP request (no host+schema)
         */
        Common::Property<IO::Network::URI> url;

    public:
        /**
         */
        Common::Property<IO::Network::HTTP::Headers> headers;

    public:
        /**
         *  consider losing and replacing with UpdateHeader() like we have for Response
         */
        nonvirtual void AddHeader (const String& headerName, const String& value);

    public:
        /**
         *  \brief mostly looks at Connection: ARG header, but if not there takes into account HTTP-version specific defaults.
         */
        nonvirtual bool GetKeepAliveRequested () const;

    public:
        /**
         *  Return the unsigned integer value of the Content-Length header.
         *
         *  \note - this does not imply having read the body, and nor is it updated to reflect the body size read after its been read.
         */
        nonvirtual optional<uint64_t> GetContentLength () const;

    public:
        /**
         *  Return the HTTP message body Content-Type, if any given
         */
        nonvirtual optional<InternetMediaType> GetContentType () const;

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
         *  This may ONLY be called after the headers have been set.
         */
        nonvirtual Streams::InputStream<byte>::Ptr GetBodyStream ();

    public:
        /**
        *  @see Characters::ToString ()
        */
        nonvirtual String ToString () const;

    public:
        [[deprecated ("Since Stroika v2.1b10 use httpVersion property")]] String                 GetHTTPVersion () const;
        [[deprecated ("Since Stroika v2.1b10 use httpVersion property")]] void                   SetHTTPVersion (const String& versionOrVersionLabel);
        [[deprecated ("Since Stroika v2.1b10 use httpMethod property")]] String                  GetHTTPMethod () const;
        [[deprecated ("Since Stroika v2.1b10 use httpMethod property")]] void                    SetHTTPMethod (const String& method);
        [[deprecated ("Since Stroika v2.1b10 use url property")]] IO::Network::URI               GetURL () const;
        [[deprecated ("Since Stroika v2.1b10 use url property")]] void                           SetURL (const IO::Network::URI& url);
        [[deprecated ("Since Stroika v2.1b10 use headers property")]] IO::Network::HTTP::Headers GetHeaders () const;
        [[deprecated ("Since Stroika v2.1b10 use headers property")]] void                       SetHeaders (const IO::Network::HTTP::Headers& headers);

    private:
        Streams::InputStream<byte>::Ptr fInputStream_;
        String                          fHTTPVersion_;
        String                          fMethod_;
        IO::Network::URI                fURL_;
        IO::Network::HTTP::Headers      fHeaders_;
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
