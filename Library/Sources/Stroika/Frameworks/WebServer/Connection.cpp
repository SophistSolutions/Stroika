/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "Stroika/Foundation/Characters/FloatConversion.h"
#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/String2Int.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/Compression/Deflate.h"
#include "Stroika/Foundation/DataExchange/Compression/GZip.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Throw.h"
#include "Stroika/Foundation/IO/FileSystem/FileOutputStream.h"
#include "Stroika/Foundation/IO/FileSystem/WellKnownLocations.h"
#include "Stroika/Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "Stroika/Foundation/IO/Network/HTTP/Headers.h"
#include "Stroika/Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "Stroika/Foundation/IO/Network/HTTP/Methods.h"
#include "Stroika/Foundation/Streams/LoggingInputOutputStream.h"
#include "Stroika/Foundation/Time/DateTime.h"

#include "Connection.h"

using std::byte;

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

using IO::Network::HTTP::ClientErrorException;
using IO::Network::HTTP::Headers;
using IO::Network::HTTP::KeepAlive;

// Comment this in to turn on aggressive noisy DbgTrace in this module
// #define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 ******************** WebServer::Connection::MyMessage_ *************************
 ********************************************************************************
 */
Connection::MyMessage_::MyMessage_ (const ConnectionOrientedStreamSocket::Ptr& socket, const Streams::InputOutputStream::Ptr<byte>& socketStream,
                                    const Headers& defaultResponseHeaders, const optional<bool> autoComputeETagResponse)
    : Message{Request{socketStream}, Response{socket, socketStream, defaultResponseHeaders}, socket.GetPeerAddress ()}
    , fMsgHeaderInTextStream{HTTP::MessageStartTextInputStreamBinaryAdapter::New (rwRequest ().GetInputStream ())}
{
    if (autoComputeETagResponse) {
        this->rwResponse ().autoComputeETag = *autoComputeETagResponse;
    }
}

Connection::MyMessage_::ReadHeadersResult Connection::MyMessage_::ReadHeaders (
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    const function<void (const String&)>& logMsg
#endif
)
{
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    logMsg ("Starting ReadHeaders_"sv);
#endif

    /*
     *  Preflight the request and make sure all the bytes of the header are available. Don't read more than needed.
     */
    if (not fMsgHeaderInTextStream.AssureHeaderSectionAvailable ()) {
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        logMsg ("got fMsgHeaderInTextStream.AssureHeaderSectionAvailable INCOMPLETE"sv);
#endif
        return ReadHeadersResult::eIncompleteButMoreMayBeAvailable;
    }

    /*
     * At this stage, blocking calls are fully safe - because we've assured above we've seeked to the start of a CRLFCRLF terminated region (or premature EOF)
     */
    Request& updatableRequest = this->rwRequest ();
    {
        // Read METHOD URL line
        String line = fMsgHeaderInTextStream.ReadLine ();
        if (line.length () == 0) {
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
            logMsg ("got EOF from src stream reading headers(incomplete)"sv);
#endif
            return ReadHeadersResult::eIncompleteDeadEnd; // could throw here, but this is common enough we don't want the noise in the logs.
        }
        Sequence<String> tokens{line.Tokenize ({' '})};
        if (tokens.size () < 3) {
            DbgTrace ("tokens={}, line='{}', fMsgHeaderInTextStream={}"_f, tokens, line, fMsgHeaderInTextStream.ToString ());
            Execution::Throw (ClientErrorException{Characters::Format ("Bad METHOD Request HTTP line ({})"_f, line)});
        }
        updatableRequest.httpMethod  = tokens[0];
        updatableRequest.httpVersion = tokens[2];
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace ("tokens={}, line='{}'"_f, tokens, line);
            Execution::Throw (ClientErrorException{"Bad HTTP Request line - missing host-relative URL"sv});
        }
        using IO::Network::URL;
        updatableRequest.url = URI::ParseRelative (tokens[1]);
        if (updatableRequest.httpMethod ().empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace ("tokens={}, line='{}'"_f, tokens, line);
            static const auto kException_ = ClientErrorException{"Bad METHOD in Request HTTP line"sv};
            Execution::Throw (kException_);
        }
    }
    while (true) {
        static const String kCRLF_{"\r\n"sv};
        String              line = fMsgHeaderInTextStream.ReadLine ();
        if (line == kCRLF_ or line.empty ()) {
            break; // done
        }

        // add subsequent items to the header map
        size_t i = line.find (':');
        if (i == string::npos) {
            DbgTrace ("line={}"_f, line);
            Execution::Throw (ClientErrorException{"Bad HTTP Request missing colon in headers"sv});
        }
        else {
            String hdr   = line.SubString (0, i).Trim ();
            String value = line.SubString (i + 1).Trim ();
            updatableRequest.rwHeaders ().Add (hdr, value);
        }
    }
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    logMsg ("ReadHeaders completed normally"sv);
#endif
    return ReadHeadersResult::eCompleteGood;
}

/*
 ********************************************************************************
 ***************************** WebServer::Connection ****************************
 ********************************************************************************
 */
Connection::Connection (const ConnectionOrientedStreamSocket::Ptr& s, const InterceptorChain& interceptorChain, const Headers& defaultResponseHeaders,
                        const optional<Headers>& defaultGETResponseHeaders, const optional<bool> autoComputeETagResponse)
    : Connection{s, Options{.fInterceptorChain          = interceptorChain,
                            .fDefaultResponseHeaders    = defaultResponseHeaders,
                            .fDefaultGETResponseHeaders = defaultGETResponseHeaders,
                            .fAutoComputeETagResponse   = autoComputeETagResponse}}
{
}

Connection::Connection (const ConnectionOrientedStreamSocket::Ptr& s, const Options& options)
    : socket{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> ConnectionOrientedStreamSocket::Ptr {
        const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::socket);
        AssertExternallySynchronizedMutex::ReadContext declareContext{*thisObj};
        return thisObj->fSocket_;
    }}
    , request{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Request& {
        const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::request);
        AssertExternallySynchronizedMutex::ReadContext declareContext{*thisObj};
        return thisObj->fMessage_->request ();
    }}
    , response{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Response& {
        const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::response);
        AssertExternallySynchronizedMutex::ReadContext declareContext{*thisObj};
        return thisObj->fMessage_->response ();
    }}
    , rwResponse{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Response& {
        Connection* thisObj = const_cast<Connection*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::rwResponse));
        AssertExternallySynchronizedMutex::WriteContext declareContext{*thisObj};
        return thisObj->fMessage_->rwResponse ();
    }}
    , remainingConnectionLimits{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<HTTP::KeepAlive> {
              const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::remainingConnectionLimits);
              AssertExternallySynchronizedMutex::ReadContext declareContext{*thisObj};
              return thisObj->fRemaining_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& remainingConnectionLimits) {
              Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::remainingConnectionLimits);
              AssertExternallySynchronizedMutex::WriteContext declareContext{*thisObj};
              thisObj->fRemaining_ = remainingConnectionLimits;
          }}
    , fInterceptorChain_{options.fInterceptorChain}
    , fDefaultResponseHeaders_{options.fDefaultResponseHeaders}
    , fDefaultGETResponseHeaders_{options.fDefaultGETResponseHeaders}
    , fAutoComputeETagResponse_{options.fAutoComputeETagResponse}
    , fSupportedCompressionEncodings_{options.fSupportedCompressionEncodings}
    , fSocket_{s}
    , fConnectionStartedAt_{Time::GetTickCount ()}
{
    Require (s != nullptr);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Created connection for socket {}"_f, s);
#endif
    fSocketStream_ = SocketStream::New (fSocket_);
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    {
        String socketName = Characters::Format ("{}-{}"_f, (long)Time::DateTime::Now ().As<time_t> (), (int)s.GetNativeSocket ());
        fSocketStream_    = Streams::LoggingInputOutputStream<byte>::New (
            fSocketStream_,
            IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () +
                                                      Characters::Format (L"socket-%s-input-trace.txt", socketName.c_str ())),
            IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () +
                                                      Characters::Format (L"socket-%s-output-trace.txt", socketName.c_str ())));
        fLogConnectionState_ = Streams::TextWriter::New (
            IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () +
                                                   Characters::Format (L"socket-%s-highlevel-trace.txt", socketName.c_str ())),
            Streams::TextWriter::Format::eUTF8WithoutBOM);
    }
#endif
}

Connection::~Connection ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Destroying connection for socket {}, message={}"_f, fSocket_, static_cast<const void*> (fMessage_.get ()));
#endif
    AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    WriteLogConnectionMsg_ (L"DestroyingConnection");
#endif
    if (fMessage_ != nullptr) {
        if (not fMessage_->response ().responseCompleted ()) {
            IgnoreExceptionsForCall (fMessage_->rwResponse ().Abort ());
        }
        Require (fMessage_->response ().responseCompleted ());
    }
    /*
     *  When the connection is completed, make sure the socket is closed so that the calling client knows
     *  as quickly as possible. Probably not generally necessary since when the last reference to the socket
     *  goes away, it will also be closed, but that might take a little longer as its held in some object
     *  that hasn't gone away yet.
     */
    AssertNotNull (fSocket_);
    try {
        fSocket_.Close ();
    }
    catch (...) {
        DbgTrace ("Exception ignored closing socket: {}"_f, current_exception ());
    }
}

Connection::ReadAndProcessResult Connection::ReadAndProcessMessage () noexcept
{
    AssertExternallySynchronizedMutex::WriteContext declareContext{*this};
    try {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Connection::ReadAndProcessMessage", "this->socket="_f, fSocket_};
#endif
        fMessage_ = make_unique<MyMessage_> (fSocket_, fSocketStream_, fDefaultResponseHeaders_, fAutoComputeETagResponse_);
#if qStroika_Foundation_Debug_AssertExternallySynchronizedMutex_Enabled
        fMessage_->SetAssertExternallySynchronizedMutexContext (GetSharedContext ());
#endif

        // First read the HTTP request line, and the headers (and abort this attempt if not ready)
        switch (fMessage_->ReadHeaders (
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
            [this] (const String& i) -> void { WriteLogConnectionMsg_ (i); }
#endif
            )) {
            case MyMessage_::eIncompleteDeadEnd: {
                DbgTrace ("ReadHeaders failed - typically because the client closed the connection before we could handle it (e.g. in web "
                          "browser hitting refresh button fast)."_f);
                return eClose; // don't keep-alive - so this closes connection
            } break;
            case MyMessage_::eIncompleteButMoreMayBeAvailable: {
                DbgTrace ("ReadHeaders failed - incomplete header (most likely a DOS attack)."_f);
                return ReadAndProcessResult::eTryAgainLater;
            } break;
            case MyMessage_::eCompleteGood: {
                // fall through and actually process the request
            } break;
        }

            // if we get this far, we always complete processing the message
#if qDebug
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([&] () noexcept { Ensure (fMessage_->response ().responseCompleted ()); });
#endif

        if (fDefaultGETResponseHeaders_ and fMessage_->request ().httpMethod () == HTTP::Methods::kGet) {
            fMessage_->rwResponse ().rwHeaders () += *fDefaultGETResponseHeaders_;
        }

        // https://tools.ietf.org/html/rfc7231#section-7.1.1.2  : ...An origin server MUST send a Date header field in all other cases
        fMessage_->rwResponse ().rwHeaders ().date = Time::DateTime::Now ();

        // @todo can short-circuit the acceptEncoding logic if not bodyHasEntity...(but careful about checking that cuz no content yet
        // so may need to revisit the bodyHasEntity logic) - just look at METHOD of request and http-status - oh - that cannot check
        // yet/until done... so maybe need other check like bodyCannotHaveEntity - stuff can check before filled out response?
        if (optional<HTTP::ContentEncodings> acceptEncoding = fMessage_->request ().headers ().acceptEncoding) {
            optional<HTTP::ContentEncodings> oBodyEncoding = fMessage_->rwResponse ().bodyEncoding ();
            auto                             addCT         = [this, &oBodyEncoding] (HTTP::ContentEncoding contentEncoding2Add) {
                fMessage_->rwResponse ().bodyEncoding = [&] () {
                    if (oBodyEncoding) {
                        auto bc = *oBodyEncoding;
                        bc += contentEncoding2Add;
                        return bc;
                    }
                    else {
                        return HTTP::ContentEncodings{contentEncoding2Add};
                    }
                }();
            };
            bool needBodyEncoding = not oBodyEncoding.has_value ();
            // prefer deflate over gzip cuz smaller header and otherwise same
            auto maybeAddIt = [&] (HTTP::ContentEncoding ce) {
                if (needBodyEncoding and acceptEncoding->Contains (ce) and
                    (fSupportedCompressionEncodings_ == nullopt or fSupportedCompressionEncodings_->Contains (ce))) {
                    addCT (ce);
                    needBodyEncoding = false;
                }
            };
            if constexpr (DataExchange::Compression::Deflate::kSupported) {
                maybeAddIt (HTTP::ContentEncoding::kDeflate);
            }
            if constexpr (DataExchange::Compression::GZip::kSupported) {
                maybeAddIt (HTTP::ContentEncoding::kGZip);
            }
            // @todo add zstd, and others? zstd best probably...
        }

        /*
         *  Now bookkeeping and handling of keepalive headers
         */
        bool thisMessageKeepAlive = fMessage_->request ().keepAliveRequested;
        if (thisMessageKeepAlive) {
            // Check for keepalive headers, and handle/merge them appropriately
            // only meaningful HTTP 1.1 and earlier and only if Connection: keep-alive
            if (auto keepAliveValue = fMessage_->request ().headers ().keepAlive ()) {
                this->remainingConnectionLimits = KeepAlive::Merge (this->remainingConnectionLimits (), *keepAliveValue);
            }
            // if missing, no limits
            if (auto oRemaining = remainingConnectionLimits ()) {
                if (oRemaining->fMessages) {
                    if (oRemaining->fMessages == 0u) {
                        thisMessageKeepAlive = false;
                    }
                    else {
                        oRemaining->fMessages = *oRemaining->fMessages - 1u;
                    }
                }
                if (oRemaining->fTimeout) {
                    if (fConnectionStartedAt_ + *oRemaining->fTimeout < Time::GetTickCount ()) {
                        thisMessageKeepAlive = false;
                    }
                }
            }
        }
        // From https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
        //      HTTP/1.1 applications that do not support persistent connections MUST include the "close" connection option in every message.
        this->rwResponse ().rwHeaders ().connection = thisMessageKeepAlive ? Headers::eKeepAlive : Headers::eClose;

        if (auto requestedINoneMatch = this->request ().headers ().ifNoneMatch ()) {
            if (this->response ().autoComputeETag ()) {
                this->rwResponse ().automaticTransferChunkSize =
                    Response::kNoChunkedTransfer; // cannot start response xfer til we've computed etag (meaning seen all the body bytes)
            }
        }

        /**
         *  Delegate to interceptor chain. This is the principle EXTENSION point for the Stroika Framework webserver. This is where you modify
         *  the response somehow or other (typically through routes).
         */
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Handing request {} to interceptor chain"_f, request ().ToString ());
#endif
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        WriteLogConnectionMsg_ (Characters::Format ("Handing request {} to interceptor chain"_f, request ()));
#endif
        try {
            fInterceptorChain_.HandleMessage (fMessage_.get ());
        }
        catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace ("Interceptor-Chain caught exception handling message: {}"_f, current_exception ());
#endif
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
            WriteLogConnectionMsg_ (Characters::Format ("Interceptor-Chain caught exception handling message: {}"_f, current_exception ()));
#endif
        }

        if (thisMessageKeepAlive) {
            // be sure we advance the read pointer over the message body,
            // lest we start reading part of the previous message as the next message

            // @todo must fix this for support of Transfer-Encoding, but from:
            //
            /*
             *  https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
             *      The rules for when a message-body is allowed in a message differ for requests and responses.
             *
             *      The presence of a message-body in a request is signaled by the inclusion of a Content-Length 
             *      or Transfer-Encoding header field in the request's message-headers/
             */
            if (request ().headers ().contentLength ()) {
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                WriteLogConnectionMsg_ (L"msg is keepalive, and have content length, so making sure we read all of request body");
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
                DbgTrace ("Assuring all data read; REQ={}"_f, request ().ToString ());
#endif
                // @todo - this can be more efficient in the rare case we ignore the body - but that's rare enough to not matter much
                (void)fMessage_->rwRequest ().GetBody ();
            }
        }

        /*
         *  By this point, the response has been fully built, and so we can potentially redo the response as a 304-not-modified, by
         *  comparing the ETag with the ifNoneMatch header.
         */
        if (not this->response ().responseStatusSent () and this->response ().status == HTTP::StatusCodes::kOK) {
            if (auto requestedINoneMatch = this->request ().headers ().ifNoneMatch ()) {
                if (auto actualETag = this->response ().headers ().ETag ()) {
                    bool ctm = this->response ().chunkedTransferMode ();
                    if (ctm) {
                        DbgTrace ("Warning - disregarding ifNoneMatch request (though it matched) - cuz in chunked transfer mode"_f);
                    }
                    if (requestedINoneMatch->fETags.Contains (*actualETag) and not ctm) {
                        DbgTrace ("Updating OK response to NotModified (due to ETag match)"_f);
                        this->rwResponse ().status = HTTP::StatusCodes::kNotModified; // this assignment automatically prevents sending data
                    }
                }
            }
        }

        if (not this->rwResponse ().End ()) {
            thisMessageKeepAlive = false;
        }
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        WriteLogConnectionMsg_ (L"Did GetResponse ().End ()");
#endif
        return thisMessageKeepAlive ? eTryAgainLater : eClose;
    }
    catch (...) {
        DbgTrace ("ReadAndProcessMessage Exception caught ({}), so returning ReadAndProcessResult::eClose"_f, current_exception ());
        this->rwResponse ().Abort ();
        return Connection::ReadAndProcessResult::eClose;
    }
}

#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
void Connection::WriteLogConnectionMsg_ (const String& msg) const
{
    String useMsg = Time::DateTime::Now ().Format () + " -- "sv + msg.Trim ();
    fLogConnectionState_.WriteLn (useMsg);
}
#endif

String Connection::ToString (bool abbreviatedOutput) const
{
    AssertExternallySynchronizedMutex::ReadContext declareContext{*this};
    StringBuilder                                  sb;
    sb << "{"sv;
    sb << "Socket: "sv << fSocket_ << ", "sv;
    if (not abbreviatedOutput) {
        sb << "Message: "sv << fMessage_ << ", "sv;
        sb << "Remaining: "sv << fRemaining_ << ", "sv;
    }
    sb << "Connection-Started-At: "sv << fConnectionStartedAt_;
    sb << "}"sv;
    return sb;
}