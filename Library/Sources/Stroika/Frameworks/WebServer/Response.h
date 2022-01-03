/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Response_h_
#define _Stroika_Framework_WebServer_Response_h_ 1

#include "../StroikaPreComp.h"

#include <map>
#include <string>
#include <vector>

#include "../../Foundation/Characters/CodePage.h"
#include "../../Foundation/Characters/String.h"
#include "../../Foundation/Common/Property.h"
#include "../../Foundation/Configuration/Common.h"
#include "../../Foundation/Containers/Mapping.h"
#include "../../Foundation/Cryptography/Digest/Algorithm/MD5.h"
#include "../../Foundation/Cryptography/Digest/Digester.h"
#include "../../Foundation/DataExchange/InternetMediaType.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/Response.h"
#include "../../Foundation/IO/Network/HTTP/Status.h"
#include "../../Foundation/IO/Network/Socket.h"
#include "../../Foundation/IO/Network/URI.h"
#include "../../Foundation/Memory/BLOB.h"
#include "../../Foundation/Streams/BufferedOutputStream.h"
#include "../../Foundation/Streams/OutputStream.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   REDO THE HTTPRESPONSE USING A BINARY OUTPUT STREAM.
 *              INTERNALLY - based on code page - construct a TEXTOUTPUTSTREAM wrapping that binary output stream!!!
 */

namespace Stroika::Frameworks::WebServer {

    using std::byte;

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO::Network;

    using Characters::String;
    using Containers::Mapping;
    using DataExchange::InternetMediaType;
    using HTTP::Status;
    using Memory::BLOB;

    /*
     *  \note Set headers (with this->rwHeaders()...) as early as practical (before calling write or Flush).
     *  \note To use chunked transfer encoding (NOT THE DEFAULT) - call
     *      \code
     *        response->rwHeaders ().transferEncoding = HTTP::TransferEncoding::eChunked;
     *        ... then do 
     *        response->write (...);
     *      \endcode
     *
     *  TODO:
     *      @todo Support https://stroika.atlassian.net/browse/STK-727 - HTTP Chunked Transfer Trailers. We do support
     *            chunked transfers, but require all the headers set first.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    class Response : public IO::Network::HTTP::Response {
    private:
        using inherited = IO::Network::HTTP::Response;

    public:
        /**
         */
        Response ()                = delete;
        Response (const Response&) = delete;
        Response (Response&& src);
        Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream<byte>::Ptr& outStream, const optional<HTTP::Headers>& initialHeaders = nullopt);

    public:
        // Reponse must be completed (OK to Abort ()) before being destroyed
        ~Response () = default;

    public:
        nonvirtual Response& operator= (const Response&) = delete;

    public:
        /**
         *  If true, and if possible, and not already present, an eTag value will be automatically added to the response headers.
         * 
         *  As of Stroika 2.1b10, this is not done for 'chunked' transfer responses, because we don't yet support trailers.
         * 
         *  \req this->state == ePreparingHeaders (before first write to body) to set
         */
        Common::Property<bool> autoComputeETag;

    public:
        /**
         *  If true (defaults true), the content-length will be automatically computed.. Assigning to the contentLength
         *  header will automatically set this to false.
         * 
         *  \req this->state == ePreparingHeaders (before first write to body) to set
         */
        Common::Property<bool> autoComputeContentLength;

    public:
        /*
         * Note - the code page is only applied to string/text conversions and content-types which are know text-based content types.
         * For ContentTypes
         *      o   text / * {avoid comment-character}
         *      o   application/json
         *  and any other content type that returns true to InternetMediaType::IsTextFormat () the codepage is added to the content-type as in:
         *          "text/html; charset=UTF-8"
         *
         * codePage.Set ()
         *      \req this->headersCanBeSet()
         *      \req TotalBytesWritten == 0
         * 
         * \note - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (fContentType_), then
         *         the characterset will be automatically folded into the used contentType. To avoid this, 
         *         Use UpdateHeader() to mdofiy teh contenttype field directly.
         * 
         *  \req this->headersCanBeSet() to set property
         */
        Common::Property<Characters::CodePage> codePage;

    public:
        /*
         * \brief inherited Common::Property <optional<InternetMediaType>> contentType;
         *
         *  \req this->headersCanBeSet() to set property
         * 
         *  NOTE - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (contentType), then
         *  the characterset will be automatically folded into the used contentType (on WRITES to the property - not reads).
         */

    public:
        /**
         *  \note about states - certain properties (declared here and inherited) - like rwHeaders, and writes to properites like (XXX) cannot be done
         *        unless the current state is ePreparingHeaders; and these are generally checked with assertions.
         */
        enum class State : uint8_t {
            ePreparingHeaders,               // A newly constructed Response starts out ePreparingHeaders state
            ePreparingBodyBeforeHeadersSent, // headers can no longer be adjusted, but have not been sent over the wire
            ePreparingBodyAfterHeadersSent,  // headers have now been sent over the wire
            eCompleted,                      // and finally to Completed

            Stroika_Define_Enum_Bounds (ePreparingHeaders, eCompleted)
        };

    public:
        /**
         *  The state may be changed by calls to Flush (), Abort (), Redirect (), and End (), and more...
         *  
         *  \note as the design of the HTTP server changes, the list of States may change, so better to check properties
         *        like headersCanBeSet, or responseStatusSent, rathern than checking the state explicitly.
         */
        Common::ReadOnlyProperty<State> state;

    public:
        /**
         *  Check this (readonly) property before updating headers. For now, this is the same as this->state == ePreparingHeaders
         * 
         *  \note - even HTTP 1.1 allows for headers to be sent AFTER we've started sending chunks, so this interpretation MAY
         *        change over time, but the current implementation only allows setting headers while in the preparingHeaders state
         *        (so in other words, before any calls to Flush or write).
         */
        Common::ReadOnlyProperty<bool> headersCanBeSet;

    public:
        /**
         *  Returns true once the response status code has been sent (so most things cannot be changed after this); note responseStatus and
         *  the initial bunch of headers (excluding trailers) all set at the same time (so this also checks for all non-trial headers being sent).
         */
        Common::ReadOnlyProperty<bool> responseStatusSent;

    public:
        /**
         *  Returns true once the response has been completed and fully flushed. No further calls to write() are allowed at that point.
         */
        Common::ReadOnlyProperty<bool> responseCompleted;

    public:
        /**
         *  Returns true iff the response has been aborted with a call to response.Abort ()
         */
        Common::ReadOnlyProperty<bool> responseAborted;

    public:
        /**
         *  This cannot be reversed, but puts the response into a mode where it won't emit the body of the response.
         * 
         *  \req not this->responseStatusSent()
         */
        nonvirtual void EnterHeadMode ();

    public:
        /**
         * This begins sending the parts of the message which have already been accumulated to the client.
         * Its illegal to modify anything in the headers etc - after this - but additional writes can happen
         * IFF you first set the respose.transferEncoding mode to TransferEncoding::eChunked.
         *
         * This does NOT End the repsonse, and it CAN be called arbitrarily many times (even after the response has completed - though
         * its pointless then).
         * 
         *  This can be called in any state.
         */
        nonvirtual void Flush ();

    public:
        /**
         * This method enforces that the given request has been handled. Its illegal to write to this request object again, or modify
         * any aspect of it (except for calling Abort on it).
         * 
         *  End () does nothing if it was already completed.
         * 
         *  This returns true if the response was ended normally (even if it ended prior to this call) and false if the response was
         *  aborted (abort this->Abort() called) - even if the abort was after the response status was sent.
         * 
         *  \note An internal failure in End (say because the outgoing socket was closed) will internally mark the Response
         *        as aborted (and completed), as if a call to Abort() had been done
         *
         *  \ens this->responseCompleted ()      (even if exiting the routine via exception)
         */
        nonvirtual bool End ();

    public:
        /**
         * This can be called anytime, but has no effect if the status = eCompleted. It has the effect of throwing away all
         * unsent data, and closing the associated socket.
         *
         *  Can be called in any state.
         *  \ens this->responseCompleted ()
         */
        nonvirtual void Abort ();

    public:
        /**
         * \brief End processing of this response, and direct the client to retry the request at the given url.
         *
         *  \req this->headersCanBeSet
         *  \ens this->responseCompleted ()
         */
        nonvirtual void Redirect (const URI& url);

    public:
        /**
         *  Depending on modes, write MAY or MAY NOT call Flush () sending the headers. So callers
         *  should set any headers before calling write (or printf which calls write).
         * 
         *  Note for string and wchar_t* writes, this uses this->codePage to encode the characters.
         * 
         *  \req not this->responseCompleted ()
         *  \req not this->responseStatusSent () or (this->headers ().transferEncoding ()->Contains (HTTP::TransferEncoding::eChunked)))
         */
        nonvirtual void write (const BLOB& b);
        nonvirtual void write (const byte* start, const byte* end);
        nonvirtual void write (const wchar_t* e);
        nonvirtual void write (const wchar_t* s, const wchar_t* e);
        nonvirtual void write (const String& e);

    public:
        /**
         *  Creates a string, and indirects to write ()
         */
        nonvirtual void printf (const wchar_t* format, ...);

    public:
        /**
         *  writeln () does a write, followed by writing a CRLF
         */
        nonvirtual void writeln (const wchar_t* e);
        nonvirtual void writeln (const String& e);

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        nonvirtual bool InChunkedMode_ () const;

    private:
        nonvirtual InternetMediaType AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const;

    private:
        using ETagDigester_ = Cryptography::Digest::IncrementalDigester<Cryptography::Digest::Algorithm::MD5, String>;

    private:
        IO::Network::Socket::Ptr fSocket_;
#if __cplusplus >= 202002L && !qCompilerAndStdLib_enum_with_bitLength_opequals_Buggy
        bool  fInChunkedModeCache_ : 1 {false};
        State fState_ : 3 {State::ePreparingHeaders};
        bool  fHeadMode_ : 1 {false};
        bool  fAborted_ : 1 {false};
        bool  fAutoComputeContentLength_ : 1 {true};
#else
        bool  fInChunkedModeCache_{false};
        State fState_{State::ePreparingHeaders};
        bool  fHeadMode_{false};
        bool  fAborted_{false};
        bool  fAutoComputeContentLength_{true};
#endif
        Streams::OutputStream<byte>::Ptr         fUnderlyingOutStream_;
        Streams::BufferedOutputStream<byte>::Ptr fUseOutStream_;
        Characters::CodePage                     fCodePage_{Characters::kCodePage_UTF8};
        vector<byte>                             fBodyBytes_{};
        optional<ETagDigester_>                  fETagDigester_; // dual use - if present, then flag for autoComputeETag mode as well
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Response.inl"

#endif /*_Stroika_Framework_WebServer_Response_h_*/
