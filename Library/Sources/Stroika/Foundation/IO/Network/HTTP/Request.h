/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_IO_Network_HTTP_Request_h_
#define _Stroika_Foundation_IO_Network_HTTP_Request_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/URI.h"

/**
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 */

namespace Stroika::Foundation::IO::Network::HTTP {

    using Characters::String;
    using DataExchange::InternetMediaType;

    /**
     *  HTTP Request object that (shared by HTTP Client and HTTP Server APIs).
     *  \note - @todo - KEY MISSING FEATURE is BODY and STREAM - which are just in the Frameworks/WebServer subclass
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Request {
    public:
        /**
         */
        Request ();
        Request (const Request&) = delete;
        Request (Request&& src);

    public:
        nonvirtual Request& operator= (const Request&) = delete;

#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
    public:
        /**
         *  Allow users of the Headers object to have it share a 'assure externally synchronized' context.
         * 
         *  \see AssertExternallySynchronizedMutex::SetAssertExternallySynchronizedMutexContext
         */
        nonvirtual void SetAssertExternallySynchronizedMutexContext (const shared_ptr<Debug::AssertExternallySynchronizedMutex::SharedContext>& sharedContext);
#endif

    public:
        /**
         *  This can be given HTTP/1.0, 1.0, HTTP/1.1, or 1.1. Other values are allowed, but surprising.
         *  This typically returns "1.1"
         */
        Common::Property<String> httpVersion;

    public:
        /**
         *  \brief typically HTTP::Methods::kGet
         */
        Common::Property<String> httpMethod;

    public:
        /**
         * This is the relative URL which appears at the start of an HTTP request (no host+schema)
         */
        Common::Property<URI> url;

    public:
        /**
         *  Allow readonly access to the HTTP headers embedded in the request object.
         * 
         * \note - this returns an INTERNAL POINTER to the Request, so be SURE to remember this with respect to
         *         thread safety, and lifetime (thread safety checked/enforced in debug builds with SetAssertExternallySynchronizedMutexContext);
         */
        Common::ReadOnlyProperty<const Headers&> headers;

    public:
        /**
         *  Allow read/write access to the HTTP headers embedded in the request object. This allows assigning to overwrite
         *  the headers, and it returns a live non-const reference to the Headers object.
         * 
         * \note - this returns an INTERNAL POINTER to the Request, so be SURE to remember this with respect to
         *         thread safety, and lifetime (thread safety checked/enforced in debug builds with SetAssertExternallySynchronizedMutexContext);
         */
        Common::Property<Headers&> rwHeaders;

    public:
        /**
         *  Access the HTTP request body Content-Type, if any given
         *  A short-hand, equivalent to fetching headers().contentType(), provided because this is such a commonly accessed part of Request.
         */
        Common::ReadOnlyProperty<optional<InternetMediaType>> contentType;

    public:
        /**
         *  @see Characters::ToString ()
         */
        nonvirtual String ToString () const;

    protected:
        [[no_unique_address]] Debug::AssertExternallySynchronizedMutex _fThisAssertExternallySynchronized;

    private:
        String  fHTTPVersion_;
        String  fMethod_;
        URI     fURL_;
        Headers fHeaders_;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Request.inl"

#endif /*_Stroika_Foundation_IO_Network_HTTP_Request_h_*/
