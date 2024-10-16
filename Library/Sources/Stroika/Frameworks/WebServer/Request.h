/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Request_h_
#define _Stroika_Framework_WebServer_Request_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/DataExchange/VariantValue.h"
#include "Stroika/Foundation/IO/Network/HTTP/Request.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Streams/InputStream.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using Characters::String;
    using DataExchange::InternetMediaType;

    /**
     *  \brief this represents a HTTP request object for the WebServer module
     * 
     *  TODO:
     *      @todo Probably/possibly hide the fInptuStream and other public
     *      @todo Maybe associated TextStream, and maybe readline method goes here
     *      @todo http://stroika-bugs.sophists.com/browse/STK-726 - support Transfer-Encoding: chunked (as we do for response)
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
        Request (const Streams::InputStream::Ptr<byte>& inStream);

    public:
        nonvirtual Request& operator= (const Request&) = delete;

    public:
        /**
         * Quicky impl. Need to improve this significantly.
         * Can call multiple times - but first time it blocks fetching data
         */
        nonvirtual Memory::BLOB GetBody ();

    public:
        /**
         *  Check the content-type of the argument (so far just JSON supported) and select the appropriate parser to return
         *  the body as a VariantValue. NOTE - this will block (calls GetBody()); and will throw if conversion problem/wrong contentType.
         */
        nonvirtual DataExchange::VariantValue GetBodyVariantValue ();

    public:
        /**
         *  \brief mostly looks at Connection: ARG header, but if not there takes into account HTTP-version specific defaults.
         */
        Common::ReadOnlyProperty<bool> keepAliveRequested;

    public:
        /**
         *  @todo unclear if this SB const?
         */
        nonvirtual Streams::InputStream::Ptr<byte> GetInputStream ();

    public:
        /**
         *  This returns a (generally unseekable) stream of bytes. 
         *  In most cases, this stream will be artificially narrowed to just the body part of the request.
         *  When a Content-Length is known, that is used to limit the stream. When a Transfer-Coding is used, that will return the apparent (decoded) bytes (NYI).
         *
         *  \req This may ONLY be called after the headers have been set (read) -- TODO DOCUMENT HOW THIS CAN BE CHECKED AND VERIFIED - I THINK ANSWER RESIDES IN CONNECTION OBJECT
         */
        nonvirtual Streams::InputStream::Ptr<byte> GetBodyStream ();

    public:
        /**
        *  @see Characters::ToString ()
        */
        nonvirtual String ToString () const;

    private:
        Streams::InputStream::Ptr<byte> fInputStream_;
        Streams::InputStream::Ptr<byte> fBodyInputStream_;
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
