/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Frameworks/StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "Stroika/Foundation/Characters/Format.h"
#include "Stroika/Foundation/Characters/ToString.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/DataExchange/BadFormatException.h"
#include "Stroika/Foundation/DataExchange/Compression/Deflate.h"
#include "Stroika/Foundation/DataExchange/Compression/GZip.h"
#include "Stroika/Foundation/DataExchange/Compression/ZStd.h"
#include "Stroika/Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Execution/Finally.h"
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
using namespace Stroika::Foundation::Execution;
using namespace Stroika::Foundation::Memory;
using namespace Stroika::Foundation::Traversal;
using namespace Stroika::Foundation::Time;

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

Connection::MyMessage_::ReadHeadersResult Connection::MyMessage_::ReadHeaders ()
{
    /*
     *  Preflight the request and make sure all the bytes of the header are available. Don't read more than needed.
     */
    if (not fMsgHeaderInTextStream.AssureHeaderSectionAvailable ()) {
        if (fMsgHeaderInTextStream.IsAtEOF (Streams::eDontBlock) == true) {
            return ReadHeadersResult::eIncompleteDeadEnd;
        }
        return ReadHeadersResult::eIncompleteButMoreMayBeAvailable;
    }

    /*
     * At this stage, blocking calls are fully safe - because we've assured above we've seeked to the start of a CRLFCRLF terminated region (or premature EOF)
     */
    Request& updatableRequest = this->rwRequest ();
    // the request line - method, URL and HTTP version ("GET /index.html HTTP/1.1"): false if the stream ended first
    auto readRequestLine = [&] () -> bool {
        String line = fMsgHeaderInTextStream.ReadLine ();
        if (line.length () == 0) {
            return false;
        }
        Sequence<String> tokens{line.RTrim ().Tokenize ({' '})}; // ReadLine keeps the line's CRLF
        if (tokens.size () < 3) {
            DbgTrace ("tokens={}, line='{}', fMsgHeaderInTextStream={}"_f, tokens, line, fMsgHeaderInTextStream.ToString ());
            Throw (ClientErrorException{"Bad METHOD Request HTTP line ({})"_f(line)});
        }
        updatableRequest.httpMethod  = tokens[0];
        updatableRequest.httpVersion = tokens[2];
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace ("tokens={}, line='{}'"_f, tokens, line);
            Throw (ClientErrorException{"Bad HTTP Request line - missing host-relative URL"sv});
        }
        updatableRequest.url = URI::ParseRelative (tokens[1]); // throws if ill-formed
        if (updatableRequest.httpMethod ().empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace ("tokens={}, line='{}'"_f, tokens, line);
            static const auto kException_ = ClientErrorException{"Bad METHOD in Request HTTP line"sv};
            Throw (kException_);
        }
        return true;
    };
    // read once per message: a message whose header came in pieces gets here only once all of it has come
    Assert (updatableRequest.httpMethod ().empty () and updatableRequest.httpVersion ().empty () and updatableRequest.url () == URI{});
    if (not readRequestLine ()) {
        return ReadHeadersResult::eIncompleteDeadEnd; // could throw here, but this is common enough we don't want the noise in the logs.
    }
    Assert (not updatableRequest.httpMethod ().empty ()); // not the URL or version: a client's "?" or "HTTP/" reads as empty
    // now synchronously (since we know all the bytes are here) read the rest of the headers
    while (true) {
        static const String kCRLF_{"\r\n"sv};
        String              line = fMsgHeaderInTextStream.ReadLine ();
        if (line == kCRLF_ or line.empty ()) {
            break; // done
        }

        // add subsequent items to the header map
        size_t i = line.find (':');
        if (i == string::npos) {
            DbgTrace ("line={}"_f, line); // should almost never happen, but in case this ever does happen, may want to know the form/situation
            Throw (ClientErrorException{"Bad HTTP Request missing colon in headers"sv});
        }
        else {
            String hdr   = line.SubString (0, i).Trim ();
            String value = line.SubString (i + 1).Trim ();
            updatableRequest.rwHeaders ().Add (hdr, value);
        }
    }
    return ReadHeadersResult::eCompleteGood;
}

/*
 ********************************************************************************
 ************************* WebServer::Connection::Stats *************************
 ********************************************************************************
 */
String Connection::Stats::ToString () const
{
    StringBuilder sb;
    sb << "{";
    sb << "socket: " << fSocketID;
    sb << ", createdAt: " << fCreatedAt;
    if (fActive) {
        if (*fActive) {
            sb << ", active";
        }
        else {
            sb << ", inactive";
        }
    }
#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
    if (fReadAndProcessMessageNumber != 0) {
        sb << ", connectionMessageNumber: " << fReadAndProcessMessageNumber;
    }
    sb << ", state: " << fState;
    if (fMostRecentMessage) {
        sb << ", mostRecentMessage: " << *fMostRecentMessage;
    }
    if (fHandlingThread) {
        if (fActive == true) {
            sb << ", handlingThread: " << fHandlingThread;
        }
        else {
            sb << ", thread: " << fHandlingThread;
        }
    }
    if (fRequestWebMethod || fRequestURI) {
        sb << ", " << fRequestWebMethod << " " << fRequestURI;
    }
    if (fRemotePeerAddress) {
        sb << ", from: " << *fRemotePeerAddress;
    }
#endif
    sb << "}";
    return sb;
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
        AssertExternallySynchronizedChecker::ReadContext declareContext{*thisObj};
        return thisObj->fSocket_;
    }}
    , request{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Request& {
        const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::request);
        AssertExternallySynchronizedChecker::ReadContext declareContext{*thisObj};
        return thisObj->fMessage_->request ();
    }}
    , response{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Response& {
        const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::response);
        AssertExternallySynchronizedChecker::ReadContext declareContext{*thisObj};
        return thisObj->fMessage_->response ();
    }}
    , rwResponse{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Response& {
        Connection* thisObj = const_cast<Connection*> (qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::rwResponse));
        AssertExternallySynchronizedChecker::WriteContext declareContext{*thisObj};
        return thisObj->fMessage_->rwResponse ();
    }}
    , stats{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Stats {
        const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::stats);
        // NO - INTERNALLY SYNCHRONIZED!!! AssertExternallySynchronizedChecker::ReadContext declareContext{*thisObj};
        // typically called from thread OTHER than the one filling in these variables
        auto uniqueID = thisObj->fSocket_.GetNativeSocket (); // safe because fSocket_ is a const Ptr, and GetNativeSocket () is a const method, so never modified and can be safely used without synchronization
        TimePointSeconds createdAt{thisObj->fConnectionStartedAt_}; // also similar logic - const
#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
        Stats2Capture_ statsCapturedDuringMessageProcessing = thisObj->fExtraStats_.load ();
        using State                                         = Stats::State;
        State state                                         = [thisObj] () {
            switch (thisObj->fState_) {
                case State_Flag_::eNew:
                    return State::eNew;
                case State_Flag_::eReadingHeaders_Started:
                    return State::eReadingHeaders;
                case State_Flag_::eFinishedReadingHeaders_Success:
                    return State::eProcessingInterceptorChain;
                case State_Flag_::eFinishedReadingHeaders_Incomplete:
                    return State::ePausedIncompleteHeaders;
                case State_Flag_::eFinishedReadingHeaders_Failed:
                    return State::eClosing;
                case State_Flag_::eInterceptorChain_Start:
                    return State::eProcessingInterceptorChain;
                case State_Flag_::eInterceptorChain_Complete:
                    return State::eFlushing;
                case State_Flag_::eFlushing_Start:
                    return State::eFlushing;
                case State_Flag_::eFlushing_Done:
                    if (thisObj->fKeepAlive_) {
                        return State::eReadyForNextMessage;
                    }
                    else {
                        return State::eClosing;
                    }
                case State_Flag_::eAborting:
                    return State::eClosing;
                default:
                    AssertNotReached ();
                    return State::eNew; // silence compiler warning
            }
        }();
#endif
        Stats stats{
            .fSocketID  = uniqueID,
            .fCreatedAt = createdAt,
#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
            .fReadAndProcessMessageNumber = thisObj->fReadAndProcessMessageNumber_,
            .fState                       = state,
            .fMostRecentMessage           = statsCapturedDuringMessageProcessing.fMessageStart
                                                ? Range<TimePointSeconds>{statsCapturedDuringMessageProcessing.fMessageStart,
                                                                          statsCapturedDuringMessageProcessing.fMessageCompleted}
                                                : optional<Range<TimePointSeconds>>{},
            .fHandlingThread              = statsCapturedDuringMessageProcessing.fHandlingThread,
            .fRemotePeerAddress           = statsCapturedDuringMessageProcessing.fPeer,
            .fRequestWebMethod            = statsCapturedDuringMessageProcessing.fWebMethod,
            .fRequestURI                  = statsCapturedDuringMessageProcessing.fRequestURI,
#endif
        };
        return stats;
    }}
    , remainingConnectionLimits{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<HTTP::KeepAlive> {
              const Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::remainingConnectionLimits);
              AssertExternallySynchronizedChecker::ReadContext declareContext{*thisObj};
              return thisObj->fRemaining_;
          },
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& remainingConnectionLimits) {
              Connection* thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::remainingConnectionLimits);
              AssertExternallySynchronizedChecker::WriteContext declareContext{*thisObj};
              thisObj->fRemaining_ = remainingConnectionLimits;
          }}
    , fInterceptorChain_{options.fInterceptorChain}
    , fDefaultResponseHeaders_{options.fDefaultResponseHeaders}
    , fDefaultGETResponseHeaders_{options.fDefaultGETResponseHeaders}
    , fAutoComputeETagResponse_{options.fAutoComputeETagResponse}
    , fSupportedCompressionEncodings_{options.fSupportedCompressionEncodings}
    , fSocket_{s}
    , fConnectionStartedAt_{Time::GetTickCount ()}
    , fIdleSince_{fConnectionStartedAt_}
{
    Require (s != nullptr);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Created connection for socket {}"_f, s);
#endif
    fSocketStream_ = SocketStream::New (fSocket_);
#if qStroika_Frameworks_WebServer_Connection_DetailedMessagingLog
    {
        String socketName = "{}-{}"_f((long)DateTime::Now ().As<time_t> (), (int)s.GetNativeSocket ());
        auto   logFile    = [&] (const String& what) {
            return IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () /
                                                          "socket-{}-{}-trace.txt"_f(socketName, what).As<filesystem::path> ());
        };
        fSocketStream_       = Streams::LoggingInputOutputStream::New<byte> (fSocketStream_, logFile ("input"sv), logFile ("output"sv));
        fLogConnectionState_ = Streams::TextToBinary::Writer::New (logFile ("highlevel"sv), Characters::UnicodeExternalEncodings::eUTF8,
                                                                   Characters::ByteOrderMark::eDontInclude);
    }
#endif
}

Connection::~Connection ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Destroying connection for socket {}, message={}"_f, fSocket_, static_cast<const void*> (fMessage_.get ()));
#endif
    AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
#if qStroika_Frameworks_WebServer_Connection_DetailedMessagingLog
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

TimePointSeconds Connection::GetIdleSince () const
{
    AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
    return fIdleSince_;
}

optional<TimePointSeconds> Connection::GetIncompleteHeaderSince () const
{
    AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
    return fIncompleteHeaderSince_;
}

void Connection::SetState_ ([[maybe_unused]] State_Flag_ s)
{
#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
    fState_ = s;
#endif
}

template <typename MAKE_MSG>
void Connection::LogMsg_ ([[maybe_unused]] const MAKE_MSG& makeMsg) const
{
#if qStroika_Frameworks_WebServer_Connection_DetailedMessagingLog
    WriteLogConnectionMsg_ (makeMsg ());
#endif
}

namespace {
    // the encoding to send a response's body in: the first of deflate, gzip and zstd that the request accepts, and supported
    // (when given) allows - deflate before gzip for its smaller header, the same otherwise
    optional<HTTP::ContentEncoding> PreferredContentEncoding_ (const HTTP::ContentEncodings&                           acceptable,
                                                               const optional<Containers::Set<HTTP::ContentEncoding>>& supported)
    {
        auto usable = [&] (HTTP::ContentEncoding ce) {
            return acceptable.Contains (ce) and (supported == nullopt or supported->Contains (ce));
        };
        if constexpr (DataExchange::Compression::Deflate::kSupported) {
            if (usable (HTTP::ContentEncoding::kDeflate)) {
                return HTTP::ContentEncoding::kDeflate;
            }
        }
        if constexpr (DataExchange::Compression::GZip::kSupported) {
            if (usable (HTTP::ContentEncoding::kGZip)) {
                return HTTP::ContentEncoding::kGZip;
            }
        }
        if constexpr (DataExchange::Compression::ZStd::kSupported) {
            if (usable (HTTP::ContentEncoding::kZStd)) {
                return HTTP::ContentEncoding::kZStd;
            }
        }
        return nullopt;
    }
}

Connection::ReadAndProcessResult Connection::ReadAndProcessMessage () noexcept
{
    AssertExternallySynchronizedChecker::WriteContext declareContext{*this};
    [[maybe_unused]] auto&& markIdle = Finally ([this] () noexcept { fIdleSince_ = Time::GetTickCount (); }); // however it returns
    try {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{"Connection::ReadAndProcessMessage", "this->socket={}"_f, fSocket_};
#endif
        // a message whose header came only in part last time (its response not completed) goes on, with what was read of it
        if (fMessage_ == nullptr or fMessage_->response ().responseCompleted ()) {
            fMessage_ = make_unique<MyMessage_> (fSocket_, fSocketStream_, fDefaultResponseHeaders_, fAutoComputeETagResponse_);
#if qStroika_Foundation_Debug_AssertExternallySynchronizedChecker_Enabled
            fMessage_->SetAssertExternallySynchronizedCheckerContext (GetSharedContext ());
#endif
            fIncompleteHeaderSince_ = Time::GetTickCount (); // its first bytes are here: the poller hands a connection over when some come
        }
        switch (ReadHeaders_ ()) {
            case MyMessage_::eIncompleteButMoreMayBeAvailable:
                return eTryAgainLater; // read the rest when it comes
            case MyMessage_::eIncompleteDeadEnd:
                return eClose; // the rest will never come
            case MyMessage_::eCompleteGood:
                fIncompleteHeaderSince_ = nullopt;
                break;
        }

            // clang-format off
        /*
         * from here, the message is always answered - or its response aborted, by the catch below
         */
            // clang-format on

#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
        [[maybe_unused]] auto&& cleanup2 =
            Finally ([&] () noexcept { fExtraStats_.rwget ().rwref ().fMessageCompleted = Time::GetTickCount (); });
        fExtraStats_.store (Stats2Capture_{.fMessageStart   = Time::GetTickCount (),
                                           .fPeer           = fSocket_.GetPeerAddress (),
                                           .fWebMethod      = fMessage_->request ().httpMethod (),
                                           .fRequestURI     = fMessage_->request ().url (),
                                           .fHandlingThread = std::this_thread::get_id ()});
#endif
        ApplyDefaultResponseHeaders_ ();
        bool keepAlive = ApplyKeepAlive_ ();
        InvokeInterceptorChain_ (); // builds the response: the Router (CORS, then the matching route's handler - the application's code), within the fault handler and any other interceptors
        if (keepAlive) {
            DiscardUnreadRequestBody_ (); // the next request on this connection starts right after this one's body
        }
        keepAlive = EndResponse_ (keepAlive);
        Ensure (fMessage_->response ().responseCompleted ());
        return keepAlive ? eTryAgainLater : eClose;
    }
    catch (...) {
        DbgTrace ("ReadAndProcessMessage Exception caught ({}), so returning ReadAndProcessResult::eClose"_f, current_exception ());
        this->rwResponse ().Abort (); // completes the response
        SetState_ (State_Flag_::eAborting);
        return Connection::ReadAndProcessResult::eClose;
    }
}

Connection::MyMessage_::ReadHeadersResult Connection::ReadHeaders_ ()
{
#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
    ++fReadAndProcessMessageNumber_;
    fExtraStats_.store (Stats2Capture_{
        .fMessageStart = Time::GetTickCount (), .fPeer = fSocket_.GetPeerAddress (), .fHandlingThread = std::this_thread::get_id ()});
#endif
    SetState_ (State_Flag_::eReadingHeaders_Started);
    LogMsg_ ([] () -> String { return "Starting ReadHeaders"sv; });
    MyMessage_::ReadHeadersResult result = fMessage_->ReadHeaders ();
    switch (result) {
        case MyMessage_::eIncompleteDeadEnd: {
            DbgTrace ("ReadHeaders failed (socket {}) - incomplete data read from client."_f,
                      fSocket_); // sometimes because the client closed the connection before we could handle: e.g. user in web browser hitting refresh button fast
            SetState_ (State_Flag_::eFinishedReadingHeaders_Failed);
            LogMsg_ ([] () -> String { return "ReadHeaders: incomplete, and no more will come"sv; });
            break;
        }
        case MyMessage_::eIncompleteButMoreMayBeAvailable: {
            DbgTrace ("ReadHeaders: the header has come only in part, so far - read the rest when it comes"_f);
            SetState_ (State_Flag_::eFinishedReadingHeaders_Incomplete);
            LogMsg_ ([] () -> String { return "ReadHeaders: incomplete, so far"sv; });
            break;
        }
        case MyMessage_::eCompleteGood: {
            SetState_ (State_Flag_::eFinishedReadingHeaders_Success);
            LogMsg_ ([] () -> String { return "ReadHeaders completed normally"sv; });
            break;
        }
        default:
            AssertNotReached ();
    }
    return result;
}

void Connection::ApplyDefaultResponseHeaders_ ()
{
    if (fDefaultGETResponseHeaders_ and fMessage_->request ().httpMethod () == HTTP::Methods::kGet) {
        fMessage_->rwResponse ().rwHeaders () += *fDefaultGETResponseHeaders_;
    }
    // https://tools.ietf.org/html/rfc7231#section-7.1.1.2  : ...An origin server MUST send a Date header field in all other cases
    fMessage_->rwResponse ().rwHeaders ().date = DateTime::Now ();

    // @todo can short-circuit the acceptEncoding logic if not bodyHasEntity...(but careful about checking that cuz no content yet
    // so may need to revisit the bodyHasEntity logic) - just look at METHOD of request and http-status - oh - that cannot check
    // yet/until done... so maybe need other check like bodyCannotHaveEntity - stuff can check before filled out response?
    if (optional<HTTP::ContentEncodings> acceptEncoding = fMessage_->request ().headers ().acceptEncoding) {
        if (not fMessage_->rwResponse ().bodyEncoding ().has_value ()) {
            if (optional<HTTP::ContentEncoding> ce = PreferredContentEncoding_ (*acceptEncoding, fSupportedCompressionEncodings_)) {
                fMessage_->rwResponse ().bodyEncoding = HTTP::ContentEncodings{*ce};
            }
        }
    }

    if (this->request ().headers ().ifNoneMatch () and this->response ().autoComputeETag ()) {
        this->rwResponse ().automaticTransferChunkSize = Response::kNoChunkedTransfer; // cannot start response xfer til we've computed etag (meaning seen all the body bytes)
    }
}

bool Connection::ApplyKeepAlive_ ()
{
    bool thisMessageKeepAlive = fMessage_->request ().keepAliveRequested;
    if (thisMessageKeepAlive) {
        // a request's Keep-Alive header (HTTP 1.1 and earlier) fills in limits not already set (@see remainingConnectionLimits)
        if (auto keepAliveValue = fMessage_->request ().headers ().keepAlive ()) {
            fRemaining_ = KeepAlive::Merge (fRemaining_, *keepAliveValue);
        }
        // fMessages counts this request; fTimeout, an idle limit, is ConnectionManager's to apply
        if (fRemaining_ and fRemaining_->fMessages) {
            if (*fRemaining_->fMessages <= 1) {
                thisMessageKeepAlive = false;
            }
            else {
                fRemaining_->fMessages = *fRemaining_->fMessages - 1;
            }
        }
    }
    // From https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
    //      HTTP/1.1 applications that do not support persistent connections MUST include the "close" connection option in every message.
    this->rwResponse ().rwHeaders ().connection = thisMessageKeepAlive ? Headers::eKeepAlive : Headers::eClose;
    return thisMessageKeepAlive;
}

void Connection::InvokeInterceptorChain_ ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace ("Handing request {} to interceptor chain"_f, request ().ToString ());
#endif
    SetState_ (State_Flag_::eInterceptorChain_Start);
    [[maybe_unused]] auto&& cleanup = Finally ([this] () noexcept { SetState_ (State_Flag_::eInterceptorChain_Complete); });
    LogMsg_ ([this] () -> String { return "Handing request {} to interceptor chain"_f(request ()); });
    try {
        fInterceptorChain_.HandleMessage (*fMessage_);
    }
    catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Interceptor-Chain caught exception handling message: {}"_f, current_exception ());
#endif
        LogMsg_ ([] () -> String { return "Interceptor-Chain caught exception handling message: {}"_f(current_exception ()); });
    }
}

void Connection::DiscardUnreadRequestBody_ ()
{
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
        LogMsg_ ([] () -> String { return "msg is keepalive, and have content length, so making sure we read all of request body"sv; });
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace ("Assuring all data read; REQ={}"_f, request ().ToString ());
#endif
        // @todo - this can be more efficient in the rare case we ignore the body - but that's rare enough to not matter much
        (void)fMessage_->rwRequest ().GetBody ();
    }
}

bool Connection::EndResponse_ (bool keepAlive)
{
    SetState_ (State_Flag_::eFlushing_Start);
    if (not this->response ().responseStatusSent () and HTTP::IsOK (this->response ().status)) {
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
        keepAlive = false;
    }
#if qStroika_Frameworks_WebServer_Connection_TrackExtraStats
    fKeepAlive_ = keepAlive;
#endif
    SetState_ (State_Flag_::eFlushing_Done);
    LogMsg_ ([] () -> String { return "Did GetResponse ().End ()"sv; });
    return keepAlive;
}

#if qStroika_Frameworks_WebServer_Connection_DetailedMessagingLog
void Connection::WriteLogConnectionMsg_ (const String& msg) const
{
    String useMsg = DateTime::Now ().Format () + " -- "sv + msg.Trim ();
    fLogConnectionState_.WriteLn (useMsg);
}
#endif

String Connection::ToString (bool abbreviatedOutput) const
{
    AssertExternallySynchronizedChecker::ReadContext declareContext{*this};
    StringBuilder                                    sb;
    sb << "{"sv;
    sb << "socket: "sv << fSocket_;
    if (not abbreviatedOutput) {
        sb << ", message: "sv << fMessage_;
        sb << ", remaining: "sv << fRemaining_;
    }
    sb << ", connectionStartedAt: "sv << fConnectionStartedAt_;
    sb << "}"sv;
    return sb;
}