/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Framework_WebServer_Response_h_
#define _Stroika_Framework_WebServer_Response_h_ 1

#include "Stroika/Frameworks/StroikaPreComp.h"

#include <vector>

#include "Stroika/Foundation/Characters/CodeCvt.h"
#include "Stroika/Foundation/Characters/CodePage.h"
#include "Stroika/Foundation/Characters/String.h"
#include "Stroika/Foundation/Common/Property.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Cryptography/Digest/Algorithm/MD5.h"
#include "Stroika/Foundation/Cryptography/Digest/Digester.h"
#include "Stroika/Foundation/DataExchange/Compression/Common.h"
#include "Stroika/Foundation/DataExchange/InternetMediaType.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/Response.h"
#include "Stroika/Foundation/IO/Network/HTTP/Status.h"
#include "Stroika/Foundation/IO/Network/Socket.h"
#include "Stroika/Foundation/IO/Network/URI.h"
#include "Stroika/Foundation/Memory/BLOB.h"
#include "Stroika/Foundation/Streams/BufferedOutputStream.h"
#include "Stroika/Foundation/Streams/OutputStream.h"
#include "Stroika/Foundation/Streams/SharedMemoryStream.h"

/*
 *  \version    <a href="Code-Status.md#Alpha">Alpha</a>
 *
 * TODO:
 *      @todo   REDO THE HTTPRESPONSE USING A BINARY OUTPUT STREAM.
 *              INTERNALLY - based on code page - construct a TEXTOUTPUTSTREAM wrapping that binary output stream!!!
 *              (partly done - but more todo)
 */

namespace Stroika::Frameworks::WebServer {

    using namespace Stroika::Foundation;
    using namespace Stroika::Foundation::IO::Network;

    using Characters::FormatString;
    using Characters::String;
    using DataExchange::InternetMediaType;
    using HTTP::Status;
    using Memory::BLOB;

    /*
     *  \note Set headers (with this->rwHeaders()...) as early as practical (before calling write or Flush).
     *  \note To use chunked transfer encoding, specify automaticTransferChunkSize
     *      \code
     *        response->rwHeaders ().automaticTransferChunkSize = 1024; // will begin chunking when this is exceeded on some write call
     *        ... then do 
     *        response->write (...);
     *      \endcode
     *
     *  TODO:
     *      @todo Support https://stroika.atlassian.net/browse/STK-727 - HTTP Chunked Transfer Trailers. We do support
     *            chunked transfers, but require all the headers set first.
     * 
     *  \note   When todo compressed responses
     *          
     *          This can be handled a lot of ways:
     *              >   let the user specify by setting header flags
     *              >   automatically (based on accept-encoding headers)
     *              >   Based on the size of the response (no point compressing a single byte response)
     * 
     *          Since this decision requires data from the 'request' (accept-encoding headers) - its not made here. The caller (Connection)
     *          specifies this through the bodyEncoding property.
     * 
     *  \todo   Add API "write-content-encoded" - so caller can pass in pre-content-encoded content (e.g. an existing .zip file data)
     * 
     *  \note   Technically, its possible to want to specify the Content-Length as a parameter/property. But we prohibit
     *          this for simplicity sake (since 3.0d7) - since its much simpler to just always let it be computed as needed.
     *
     *          We want NO Content-Length if using chunked transfer:
     *              "The Content-Length header field MUST NOT be sent if these two lengths are different (i.e., if a Transfer-Encoding
     *              header field is present)"
     *                  https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html#sec4.4
     * 
     *          And if no chunked transfer, we are accumulating all the byte before sending anyhow - so we know the length.
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
        Response (const IO::Network::Socket::Ptr& s, const Streams::OutputStream::Ptr<byte>& outStream,
                  const optional<HTTP::Headers>& initialHeaders = nullopt);

    public:
        /**
         * Response must be completed (OK to Abort ()) before being destroyed
         */
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
         *  \brief defaults to automatically chunking responses larger than a modest fixed size.
         *          
         *  When todo chunked encoding?
         *          
         *          You can always do chunked encoding for a response. But when is it best?
         *          
         *          You could do
         *              >   one chunk per write
         *              >   one chunk (not transfer-encoding: chunked) - full-response buffering
         *              >   Some fixed buffer size, and when that's exceeded, chunk
         * 
         *          In Stroika v3.0d7 and later, the caller may specify a target chunk-size, and when writes exceed this size,
         *          the transfer will be chunked automatically. Set this size to kNoChunkedTransfer to prevent chunking (nullopt just means default).
         * 
         *  \note This chunk-size threshold may refer to the compressed size, or uncompressed size as is convenient to the implementer, and is just
         *        a guideline, not strictly followed (except for the kNoChunkedTransfer special case where no chunking takes place).
         * 
         *  \note When combined with compression, due to chunking of the compression algorithm, this may not result in very uniformly
         *        sized chunks.
         * 
         *  \req this->state == ePreparingHeaders (before first write to body) to set, but can always be read
         * 
         *  \req value == nullopt or value > 0   (zero chunk size wouldn't make sense)
         */
        Common::Property<optional<size_t>> automaticTransferChunkSize;

    public:
        /**
         *  \brief sentinel value for automaticTransferChunkSize property - to disable chunked transfers
         */
        static constexpr size_t kNoChunkedTransfer = numeric_limits<size_t>::max ();

    public:
        /**
         *  \brief default value for automaticTransferChunkSize
         * 
         *  \note dont count on this value. Its subject to change. Maybe compute 'total message size' internally and compare with network FRAG size
         *        to do transfer-coding at a size that is just right to avoid extra packets.
         */
        static constexpr size_t kAutomaticTransferChunkSize_Default = 16 * 1024;

    public:
        /**
         *  \brief this corresponds to either headers().contentEncoding() or the compression part of headers().transferEncoding()
         * 
         *  \note - this is typically NOT set directly by users, and is set by the connection/message, based on the request
         *        'Accept-Encoding' headers.
         * 
         *  \req this->headersCanBeSet() to set property
         *  \req all provided encodings, the library is built to support (caller should check)
         */
        Common::Property<optional<HTTP::ContentEncodings>> bodyEncoding;

    public:
        /**
         *  \brief returns true iff headers().transferMode().Contains (HTTP::TransferMode::kChunked) - but checks for nulls etc...
         * 
         *  Note - can change depending on other settings, like whether write() has been called, or automaticTransferChunkSize()
         * 
         *  \note CANNOT change after responseStatusSent()
         */
        Common::ReadOnlyProperty<bool> chunkedTransferMode;

    public:
        /**
         *  Conversion applied to String objects to convert to bytes emitted to stream output.
         *  This value depends on the codePage property.
         * 
         *  \see https://stroika.atlassian.net/browse/STK-983
         */
        Common::ReadOnlyProperty<Characters::CodeCvt<>> codeCvt;

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
         *  \note SEE https://stroika.atlassian.net/browse/STK-983
         * 
         * \note - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (fContentType_), then
         *         the character set will be automatically folded into the used contentType. To avoid this, 
         *         Use UpdateHeader() to modify the contenttype field directly.
         * 
         *  \req this->headersCanBeSet() to set property
         */
        Common::Property<Characters::CodePage> codePage;

    public:
        /*
         * \brief Common::Property <optional<InternetMediaType>> contentType is a short-hand for headers().contentType (or rwHeaders().contentType);
         *
         *  \req this->headersCanBeSet() to set property
         * 
         *  NOTE - if DataExchange::InternetMediaTypeRegistry::Get ().IsTextFormat (contentType), then
         *  the character set will be automatically folded into the used contentType (on WRITES to the property - not reads).
         *      @todo revisit this - I think I always use character set if you use write API taking strings)--LGP 2024-06-22
         */
        Common::Property<optional<InternetMediaType>> contentType;

    public:
        /**
         * some responses will not have an entity body, like a response to a HEAD, method for example. A Stroika 
         * response has an entityBody iff the user has called a 'write' method on response?? _ NO WAHT ABOUT HEAD
         * 
         * roughly:
         *   (not fHeadMode_ and this->status () != HTTP::StatusCodes::kNotModified and contentLength() > 0)
         * 
         *   \see https://www.w3.org/Protocols/rfc2616/rfc2616-sec7.html#sec7
         */
        Common::ReadOnlyProperty<bool> hasEntityBody;

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
         *  Once set to true, this cannot be set false. It defaults to false;
         * 
         *  \req not this->responseStatusSent()
         */
        Common::Property<bool> headMode;

    public:
        /**
         *  Returns true iff the response has been aborted with a call to response.Abort ()
         *  \note - responseAborted() implies responseCompleted();
         */
        Common::ReadOnlyProperty<bool> responseAborted;

    public:
        /**
         *  Returns true once the response has been completed and fully flushed. No further calls to write() are allowed at that point.
         *  \note -  responseCompleted() doesn't mean correctly - could be responseAborted too.
         */
        Common::ReadOnlyProperty<bool> responseCompleted;

    public:
        /**
         *  Returns true once the response status code has been sent (so most things cannot be changed after this); note responseStatus and
         *  the initial bunch of headers (excluding trailers) all set at the same time (so this also checks for all non-trial headers being sent).
         */
        Common::ReadOnlyProperty<bool> responseStatusSent;

    public:
        /**
         *  \note about states - certain properties (declared here and inherited) - like rwHeaders, and writes to properties like (XXX) cannot be done
         *        unless the current state is ePreparingHeaders; and these are generally checked with assertions.
         * 
         *  \note state ordering corresponds to a progression - so new states always correspond to larger numbers, and states
         *        never go backwards
         */
        enum class State : uint8_t {
            ePreparingHeaders, // A newly constructed Response starts out ePreparingHeaders state
            eHeadersSent,      // headers have now been sent over the wire
            eCompleted,        // and finally to Completed

            Stroika_Define_Enum_Bounds (ePreparingHeaders, eCompleted)
        };

    public:
        /**
         *  The state may be changed by calls to Abort (), End (), Flush (), Redirect (), and more...
         *  
         *  \note as the design of the HTTP server changes, the list of States may change, so better to check properties
         *        like headersCanBeSet, or responseStatusSent, rather than checking the state explicitly.
         */
        Common::ReadOnlyProperty<State> state;

    public:
        /**
         * This begins sending the parts of the message which have already been accumulated to the client.
         * Its illegal to modify anything in the headers etc - after this - but additional writes can happen
         * IFF you first set the respose.transferEncoding mode to TransferEncoding::kChunked, or because of automaticTransferChunkSize
         *
         * This does NOT End the response, and it CAN be called arbitrarily many times (even after the response has completed - though
         * its pointless then).
         * 
         * This can be called in any state.
         * 
         *  \note some of this restriction on headers is due to lack of support for trailers - which maybe supported by this class at some point --LGP 2024-06-29
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
         *  \req not this->responseStatusSent () or (this->headers ().transferEncoding ()->Contains (HTTP::TransferEncoding::kChunked)))
         */
        nonvirtual void write (const span<const byte>& b);
        template <Characters::IConvertibleToString T>
        nonvirtual void write (T&& s);
        template <typename CHAR_T, typename... ARGS>
        nonvirtual void write (const FormatString<CHAR_T>& f, ARGS&&... args);

    public:
        /**
         *  writeln () does a write, followed by writing a CRLF
         *      @todo add overloads like write()
         */
        nonvirtual void writeln (const String& e);

    public:
        /**
         *  @see Characters::ToString ();
         */
        nonvirtual String ToString () const;

    private:
        nonvirtual void FlushNextChunkIfNeeded_ ();

    private:
        nonvirtual void StateTransition_ (State to);

    private:
        nonvirtual void ApplyBodyEncodingIfNeeded_ ();

    private:
        /*
         * argument rawBytes already compressed if appropriate - just emits/flushes the chunk
         */
        nonvirtual void WriteChunk_ (span<const byte> rawBytes);

    private:
        nonvirtual InternetMediaType AdjustContentTypeForCodePageIfNeeded_ (const InternetMediaType& ct) const;

    private:
        using ETagDigester_ = Cryptography::Digest::IncrementalDigester<Cryptography::Digest::Algorithm::MD5, String>;

    private:
        IO::Network::Socket::Ptr         fSocket_;
        Streams::OutputStream::Ptr<byte> fProtocolOutputStream_; // socket stream - either regular socket, or SSL socket stream
#if !qCompilerAndStdLib_enum_with_bitLength_opequals_Buggy
        State fState_ : 3 {State::ePreparingHeaders};
        bool  fHeadMode_ : 1 {false};
        bool  fAborted_ : 1 {false};
#else
        State fState_{State::ePreparingHeaders};
        bool  fHeadMode_{false};
        bool  fAborted_{false};
#endif
        optional<size_t>                      fAutoTransferChunkSize_{nullopt};
        optional<HTTP::ContentEncodings>      fBodyEncoding_;  // either contentEncoding or transferEncodings for compression
        Streams::InputOutputStream::Ptr<byte> fBodyRawStream_; // write (span<const byte>) appends to this
        size_t fBodyRawStreamLength_{}; // same as fBodyRawStream_.GetWriteOffset (), but accessible after fBodyRawStream_.CloseWrite ()
        size_t fBodyRowStreamLengthWhenLastChunkGenerated_{};
        Streams::InputStream::Ptr<byte> fBodyCompressedStream_; // if not null, implies a bodyEncoding, and this is a typically smaller compressed version of fBodyRawStream_
        Streams::BufferedOutputStream::Ptr<byte> fUseOutStream_; // wrapper on fProtocolOutputStream_ to provide buffering
        Characters::CodePage                     fCodePage_{Characters::WellKnownCodePages::kUTF8};
        mutable optional<Characters::CodeCvt<>>  fCodeCvt_; // for now, cache derived from fCodePage_ --LGP 2024-06-30
        optional<ETagDigester_>                  fETagDigester_; // dual use - if present, then flag for autoComputeETag mode as well

    public:
        [[deprecated ("Since Stroika v3.0d6 - use write with _f strings")]] void printf (const wchar_t* format, ...);
        [[deprecated ("Since Stroika v3.0d7 use span overload)")]] void          write (const byte* s, const byte* e)
        {
            Require (s <= e);
            write (span<const byte>{s, e});
        }
        [[deprecated ("Since Stroika v3.0d6 - use write with _f strings")]] void write (const wchar_t* s, const wchar_t* e)
        {
            write (span<const wchar_t>{s, e});
        }
    };
    template <>
    void Response::write (const String& e);

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Response.inl"

#endif /*_Stroika_Framework_WebServer_Response_h_*/
