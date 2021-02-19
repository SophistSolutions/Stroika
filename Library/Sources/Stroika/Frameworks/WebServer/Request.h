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
 * 
 *      @todo   https://stroika.atlassian.net/browse/STK-725 - Cleanup IO::Network::HTTP::Request/Response (more like Framewors::WebServer..same), and then make Frameworks::WebServer versions inheret from IO::HTTP  versions
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
         *  This may ONLY be called after the headers have been set.
         */
        nonvirtual Streams::InputStream<byte>::Ptr GetBodyStream ();

    public:
        /**
        *  @see Characters::ToString ()
        */
        nonvirtual String ToString () const;

    public:
        [[deprecated ("Since Stroika 2.1b10, use rwHeaders().Set or Add or other")]] void             AddHeader (const String& headerName, const String& value);
        [[deprecated ("Since Stroika 2.1b10 use this->keepAliveRequested")]] bool                     GetKeepAliveRequested () const;
        [[deprecated ("Since Stroika 2.1b10 use this->contentLength")]] optional<uint64_t>            GetContentLength () const;
        [[deprecated ("Since Stroika v2.1b10 use contentType property")]] optional<InternetMediaType> GetContentType () const;
        [[deprecated ("Since Stroika v2.1b10 use httpVersion property")]] String                      GetHTTPVersion () const;
        [[deprecated ("Since Stroika v2.1b10 use httpVersion property")]] void                        SetHTTPVersion (const String& versionOrVersionLabel);
        [[deprecated ("Since Stroika v2.1b10 use httpMethod property")]] String                       GetHTTPMethod () const;
        [[deprecated ("Since Stroika v2.1b10 use httpMethod property")]] void                         SetHTTPMethod (const String& method);
        [[deprecated ("Since Stroika v2.1b10 use url property")]] IO::Network::URI                    GetURL () const;
        [[deprecated ("Since Stroika v2.1b10 use url property")]] void                                SetURL (const IO::Network::URI& url);
        [[deprecated ("Since Stroika v2.1b10 use headers property")]] IO::Network::HTTP::Headers      GetHeaders () const;
        [[deprecated ("Since Stroika v2.1b10 use headers property")]] void                            SetHeaders (const IO::Network::HTTP::Headers& headers);

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
