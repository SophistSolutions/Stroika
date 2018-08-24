/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Request_h_
#define _Stroika_Framework_WebServer_Request_h_ 1

#include "../StroikaPreComp.h"

#include <optional>

#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/Containers/Mapping.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/Debug/AssertExternallySynchronizedLock.h"
#include "../../Foundation/IO/Network/SocketAddress.h"
#include "../../Foundation/IO/Network/URL.h"
#include "../../Foundation/Streams/InputStream.h"

/*
 * TODO:
 *      @todo   Redo fHeaders as Stroika Association (not Mapping, cuz things like Set-Cookie headers can appear more than once).
 *
 *      @todo   Think out if these should be Copy By Value or reference, and about thread safety. For now avoid by saying not
 *              copyable, but still mus tthink out thread safety
 */

namespace Stroika::Frameworks::WebServer {

    using std::byte;

    using namespace Stroika::Foundation;
    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;

    /**
     *  Maybe associated TextStream, and maybe readline method goes here
     *  Quicky impl. Need to improve this significantly. Probably/possibly hide the fInptuStream and other public
     *
     *  For now assume externally sycnhonized
     */
    class Request : private Debug::AssertExternallySynchronizedLock {
    public:
        Request ()               = delete;
        Request (const Request&) = delete;
        Request (Request&&)      = default;
        Request (const Streams::InputStream<byte>::Ptr& inStream);

    public:
        nonvirtual Request& operator= (const Request&) = delete;

    public:
        // Quicky impl. Need to improve this significantly.
        // Can call multiple times - but first time it blcoks fetching data
        nonvirtual Memory::BLOB GetBody ();

    public:
        /**
         *  This typically returns "1.1"
         */
        nonvirtual String GetHTTPVersion () const;

    public:
        /**
         *  This can be given HTTP/1.0, 1.0, HTTP/1.1, or 1.1. Other values are allowed, but surprising.
         */
        nonvirtual void SetHTTPVersion (const String& versionOrVersionLabel);

    public:
        /**
         */
        nonvirtual String GetHTTPMethod () const;

    public:
        /**
         */
        nonvirtual void SetHTTPMethod (const String& method);

    public:
        /**
         */
        nonvirtual IO::Network::URL GetURL () const;

    public:
        /**
         */
        nonvirtual void SetURL (const IO::Network::URL& url);

    public:
        /**
         */
        nonvirtual Mapping<String, String> GetHeaders () const;

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
         */
        nonvirtual void SetHeaders (const Mapping<String, String>& headers);

    public:
        /**
         */
        nonvirtual void AddHeader (const String& headerName, const String& value);

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

    private:
        Streams::InputStream<byte>::Ptr fInputStream_;

    private:
        String                  fHTTPVersion_;
        String                  fMethod_;
        IO::Network::URL        fURL_;
        Mapping<String, String> fHeaders_;

    private:
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
