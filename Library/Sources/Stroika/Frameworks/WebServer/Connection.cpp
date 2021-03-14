/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/FloatConversion.h"
#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/String2Int.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/DataExchange/InternetMediaTypeRegistry.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Execution/Throw.h"
#include "../../Foundation/IO/FileSystem/FileOutputStream.h"
#include "../../Foundation/IO/FileSystem/WellKnownLocations.h"
#include "../../Foundation/IO/Network/HTTP/ClientErrorException.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "../../Foundation/IO/Network/HTTP/Methods.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"
#include "../../Foundation/Streams/LoggingInputOutputStream.h"
#include "../../Foundation/Streams/SplitterOutputStream.h"
#include "../../Foundation/Time/DateTime.h"

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
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 ******************** WebServer::Connection::MyMessage_ *************************
 ********************************************************************************
 */
Connection::MyMessage_::MyMessage_ (const ConnectionOrientedStreamSocket::Ptr& socket, const Streams::InputOutputStream<byte>::Ptr& socketStream, const Headers& defaultResponseHeaders, const optional<bool> autoComputeETagResponse)
    : Message{
          Request{socketStream},
          Response{socket, socketStream, defaultResponseHeaders},
          socket.GetPeerAddress ()}
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
    logMsg (L"Starting ReadHeaders_");
#endif

    /*
     *  Preflight the request and make sure all the bytes of the header are available. Don't read more than needed.
     */
    if (not fMsgHeaderInTextStream.AssureHeaderSectionAvailable ()) {
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        logMsg (L"got fMsgHeaderInTextStream.AssureHeaderSectionAvailable INCOMPLETE");
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
            logMsg (L"got EOF from src stream reading headers(incomplete)");
#endif
            return ReadHeadersResult::eIncompleteDeadEnd; // could throw here, but this is common enough we don't want the noise in the logs.
        }
        Sequence<String> tokens{line.Tokenize ({' '})};
        if (tokens.size () < 3) {
            DbgTrace (L"tokens=%s, line='%s', fMsgHeaderInTextStream=%s", Characters::ToString (tokens).c_str (), line.c_str (), fMsgHeaderInTextStream.ToString ().c_str ());
            Execution::Throw (ClientErrorException{Characters::Format (L"Bad METHOD Request HTTP line (%s)", line.c_str ())});
        }
        updatableRequest.httpMethod  = tokens[0];
        updatableRequest.httpVersion = tokens[2];
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace (L"tokens=%s, line='%s'", Characters::ToString (tokens).c_str (), line.c_str ());
            Execution::Throw (ClientErrorException{L"Bad HTTP Request line - missing host-relative URL"sv});
        }
        using IO::Network::URL;
        updatableRequest.url = URI{tokens[1]};
        if (updatableRequest.httpMethod ().empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace (L"tokens=%s, line='%s'", Characters::ToString (tokens).c_str (), line.c_str ());
            Execution::Throw (ClientErrorException{L"Bad METHOD in Request HTTP line"sv});
        }
    }
    while (true) {
        static const String kCRLF_{L"\r\n"sv};
        String              line = fMsgHeaderInTextStream.ReadLine ();
        if (line == kCRLF_ or line.empty ()) {
            break; // done
        }

        // add subsequent items to the header map
        size_t i = line.find (':');
        if (i == string::npos) {
            DbgTrace (L"line=%s", Characters::ToString (line).c_str ());
            Execution::Throw (ClientErrorException{L"Bad HTTP Request missing colon in headers"sv});
        }
        else {
            String hdr   = line.SubString (0, i).Trim ();
            String value = line.SubString (i + 1).Trim ();
            updatableRequest.rwHeaders ().Add (hdr, value);
        }
    }
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    logMsg (L"ReadHeaders completed normally");
#endif
    return ReadHeadersResult::eCompleteGood;
}

/*
 ********************************************************************************
 ***************************** WebServer::Connection ****************************
 ********************************************************************************
 */
Connection::Connection (const ConnectionOrientedStreamSocket::Ptr& s, const InterceptorChain& interceptorChain, const Headers& defaultResponseHeaders, const optional<Headers>& defaultGETResponseHeaders, const optional<bool> autoComputeETagResponse)
    : socket{
          [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> ConnectionOrientedStreamSocket::Ptr {
              const Connection*                                   thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::socket);
              shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
              return thisObj->fSocket_;
          }}
    , request{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Request& {
        const Connection*                                   thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::request);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        return thisObj->fMessage_->request ();
    }}
    , response{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> const Response& {
        const Connection*                                   thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::response);
        shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
        return thisObj->fMessage_->response ();
    }}
    , rwResponse{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> Response& {
        const Connection*                                  thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::rwResponse);
        lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
        return thisObj->fMessage_->rwResponse ();
    }}
    , remainingConnectionLimits{[qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] const auto* property) -> optional<HTTP::KeepAlive> {
                                    const Connection*                                   thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::remainingConnectionLimits);
                                    shared_lock<const AssertExternallySynchronizedLock> critSec{*thisObj};
                                    return thisObj->fRemaining_;
                                },
                                [qStroika_Foundation_Common_Property_ExtraCaptureStuff] ([[maybe_unused]] auto* property, const auto& remainingConnectionLimits) {
                                    Connection*                                        thisObj = qStroika_Foundation_Common_Property_OuterObjPtr (property, &Connection::remainingConnectionLimits);
                                    lock_guard<const AssertExternallySynchronizedLock> critSec{*thisObj};
                                    thisObj->fRemaining_ = remainingConnectionLimits;
                                }}
    , fInterceptorChain_{interceptorChain}
    , fDefaultResponseHeaders_{defaultResponseHeaders}
    , fDefaultGETResponseHeaders_{defaultGETResponseHeaders}
    , fAutoComputeETagResponse_{autoComputeETagResponse}
    , fSocket_{s}
    , fConnectionStartedAt_{Time::GetTickCount ()}
{
    Require (s != nullptr);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Created connection for socket %s", Characters::ToString (s).c_str ());
#endif
    fSocketStream_ = SocketStream::New (fSocket_);
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    {
        String socketName = Characters::Format (L"%ld-%d", (long)Time::DateTime::Now ().As<time_t> (), (int)s.GetNativeSocket ());
        fSocketStream_    = Streams::LoggingInputOutputStream<byte>::New (
            fSocketStream_,
            IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () + Characters::Format (L"socket-%s-input-trace.txt", socketName.c_str ())),
            IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () + Characters::Format (L"socket-%s-output-trace.txt", socketName.c_str ())));
        fLogConnectionState_ = Streams::TextWriter::New (IO::FileSystem::FileOutputStream::New (IO::FileSystem::WellKnownLocations::GetTemporary () + Characters::Format (L"socket-%s-highlevel-trace.txt", socketName.c_str ())), Streams::TextWriter::Format::eUTF8WithoutBOM);
    }
#endif
}

Connection::~Connection ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Destroying connection for socket %s, message=%s", Characters::ToString (fSocket_).c_str (), Characters::ToString (fMessage_).c_str ());
#endif
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
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
        DbgTrace (L"Exception ignored closing socket: %s", Characters::ToString (current_exception ()).c_str ());
    }
}

Connection::ReadAndProcessResult Connection::ReadAndProcessMessage () noexcept
{
    lock_guard<const AssertExternallySynchronizedLock> critSec{*this};
    try {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Connection::ReadAndProcessMessage", L"this->socket=%s", Characters::ToString (fSocket_).c_str ())};
#endif
        fMessage_ = make_unique<MyMessage_> (fSocket_, fSocketStream_, fDefaultResponseHeaders_, fAutoComputeETagResponse_);
#if qDebug
        fMessage_->SetAssertExternallySynchronizedLockContext (_fSharedContext);
#endif

        // First read the HTTP request line, and the headers (and abort this attempt if not ready)
        switch (fMessage_->ReadHeaders (
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
            [this] (const String& i) -> void { WriteLogConnectionMsg_ (i); }
#endif
            )) {
            case MyMessage_::eIncompleteDeadEnd: {
                DbgTrace (L"ReadHeaders failed - typically because the client closed the connection before we could handle it (e.g. in web browser hitting refresh button fast).");
                return eClose; // don't keep-alive - so this closes connection
            } break;
            case MyMessage_::eIncompleteButMoreMayBeAvailable: {
                DbgTrace (L"ReadHeaders failed - incomplete header (most likely a DOS attack).");
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
                if (oRemaining->fTimeoutAt) {
                    if (fConnectionStartedAt_ + *oRemaining->fTimeoutAt < Time::GetTickCount ()) {
                        thisMessageKeepAlive = false;
                    }
                }
            }
        }
        // From https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
        //      HTTP/1.1 applications that do not support persistent connections MUST include the "close" connection option in every message.
        this->rwResponse ().rwHeaders ().connection = thisMessageKeepAlive ? Headers::eKeepAlive : Headers::eClose;

        /**
         *  Delegate to interceptor chain. This is the principle EXTENSION point for the Stroika Framework webserver. This is where you modify
         *  the response somehow or other (typically through routes).
         */
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Handing request %s to interceptor chain", Characters::ToString (request ()).c_str ());
#endif
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
        WriteLogConnectionMsg_ (Characters::Format (L"Handing request %s to interceptor chain", Characters::ToString (request ()).c_str ()));
#endif
        try {
            fInterceptorChain_.HandleMessage (fMessage_.get ());
        }
        catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"Interceptor-Chain caught exception handling message: %s", Characters::ToString (current_exception ()).c_str ());
#endif
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
            WriteLogConnectionMsg_ (Characters::Format (L"Interceptor-Chain caught exception handling message: %s", Characters::ToString (current_exception ()).c_str ()));
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
                DbgTrace (L"Assuring all data read; REQ=%s", Characters::ToString (request ()).c_str ());
#endif
                // @todo - this can be more efficient in the rare case we ignore the body - but thats rare enough to not matter mcuh
                (void)fMessage_->rwRequest ().GetBody ();
            }
        }

        /*
         *  By this point, the response has been fully built, and so we can potentially redo the response as a 304-not-modified, by
         *  comparing the etag with the ifNoneMatch header.
         */
        if (not this->response ().responseStatusSent () and this->response ().status == HTTP::StatusCodes::kOK) {
            if (auto requestedINoneMatch = this->request ().headers ().ifNoneMatch ()) {
                if (auto actualETag = this->response ().headers ().ETag ()) {
                    if (requestedINoneMatch->fETags.Contains (*actualETag)) {
                        DbgTrace (L"Updating OK response to NotModified (due to ETag match)");
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
        DbgTrace (L"ReadAndProcessMessage Exception caught (%s), so returning ReadAndProcessResult::eClose", Characters::ToString (current_exception ()).c_str ());
        this->rwResponse ().Abort ();
        return Connection::ReadAndProcessResult::eClose;
    }
}

#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
void Connection::WriteLogConnectionMsg_ (const String& msg) const
{
    String useMsg = Time::DateTime::Now ().Format () + L" -- " + msg.Trim ();
    fLogConnectionState_.WriteLn (useMsg);
}
#endif

String Connection::ToString (bool abbreviatedOutput) const
{
    shared_lock<const AssertExternallySynchronizedLock> critSec{*this};
    StringBuilder                                       sb;
    sb += L"{";
    sb += L"Socket: " + Characters::ToString (fSocket_) + L", ";
    if (not abbreviatedOutput) {
        sb += L"Message: " + Characters::ToString (fMessage_) + L", ";
        sb += L"Remaining: " + Characters::ToString (fRemaining_) + L", ";
    }
    sb += L"Connection-Started-At: " + Characters::ToString (fConnectionStartedAt_);
    sb += L"}";
    return sb.str ();
}