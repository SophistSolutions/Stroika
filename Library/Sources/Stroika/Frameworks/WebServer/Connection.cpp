/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#include "../StroikaPreComp.h"

#include <algorithm>
#include <cstdlib>

#include "../../Foundation/Characters/FloatConversion.h"
#include "../../Foundation/Characters/Format.h"
#include "../../Foundation/Characters/String2Int.h"
#include "../../Foundation/Characters/String_Constant.h"
#include "../../Foundation/Characters/ToString.h"
#include "../../Foundation/Containers/Common.h"
#include "../../Foundation/DataExchange/BadFormatException.h"
#include "../../Foundation/Debug/Assertions.h"
#include "../../Foundation/Execution/Exceptions.h"
#include "../../Foundation/IO/FileSystem/FileOutputStream.h"
#include "../../Foundation/IO/FileSystem/WellKnownLocations.h"
#include "../../Foundation/IO/Network/HTTP/Headers.h"
#include "../../Foundation/IO/Network/HTTP/MessageStartTextInputStreamBinaryAdapter.h"
#include "../../Foundation/IO/Network/HTTP/Methods.h"
#include "../../Foundation/Memory/SmallStackBuffer.h"
#include "../../Foundation/Streams/LoggingInputOutputStream.h"
#include "../../Foundation/Streams/SplitterOutputStream.h"
#include "../../Foundation/Time/DateTime.h"

#include "ClientErrorException.h"

#include "Connection.h"

using namespace Stroika::Foundation;
using namespace Stroika::Foundation::Characters;
using namespace Stroika::Foundation::Containers;
using namespace Stroika::Foundation::Memory;

using namespace Stroika::Frameworks;
using namespace Stroika::Frameworks::WebServer;

// Comment this in to turn on aggressive noisy DbgTrace in this module
//#define USE_NOISY_TRACE_IN_THIS_MODULE_ 1

/*
 ********************************************************************************
 ***************************** WebServer::Connection ****************************
 ********************************************************************************
 */
Connection::Connection (const ConnectionOrientedSocket::Ptr& s, const InterceptorChain& interceptorChain)
    : fInterceptorChain_{interceptorChain}
    , fSocket_ (s)
{
    Require (s != nullptr);
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Created connection for socket %s", Characters::ToString (s).c_str ());
#endif
    fSocketStream_ = SocketStream::New (fSocket_);
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    {
        String socketName = Characters::Format (L"%ld-%d", (long)Time::DateTime::Now ().As<time_t> (), (int)s.GetNativeSocket ());
        fSocketStream_    = Streams::LoggingInputOutputStream<Memory::Byte>::New (
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
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    WriteLogConnectionMsg_ (L"DestroyingConnection");
#endif
    if (fMessage_ != nullptr) {
        if (fMessage_->PeekResponse ()->GetState () != Response::State::eCompleted) {
            IgnoreExceptionsForCall (fMessage_->PeekResponse ()->Abort ());
        }
        Require (fMessage_->PeekResponse ()->GetState () == Response::State::eCompleted);
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

bool Connection::ReadHeaders_ (Message* msg)
{
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    WriteLogConnectionMsg_ (L"Starting ReadHeaders_");
#endif
    /*
     * DONT use TextStream::ReadLine - because that asserts SEEKABLE - which may not be true 
     * (and probably isn't here anymore)
     * Instead - we need a special variant that looks for CRLF - which doesn't require backtracking...!!!
     */
    using Foundation::IO::Network::HTTP::MessageStartTextInputStreamBinaryAdapter;
    Request*                                      request      = msg->PeekRequest ();
    MessageStartTextInputStreamBinaryAdapter::Ptr inTextStream = MessageStartTextInputStreamBinaryAdapter::New (request->GetInputStream ());
    {
        // Read METHOD line
        String line = inTextStream.ReadLine ();
        if (line.length () == 0) {
            // inTextStream.IsAtEOF () would be blocking, and that's not desired here
            if (inTextStream.ReadNonBlocking () == 0) {
                /*
                 * This is relatively common. There is an outstanding connection (so client did tcp_connect) - but never got around
                 * to sending data cuz the need for the data evaporated.
                 *
                 * I've seen this not so uncommonly with chrome (hit refresh button fast and wait a while) -- LGP 2018-03-04
                 */
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                WriteLogConnectionMsg_ (L"got EOF from src stream reading headers(incomplete)");
#endif
                return false;
            }
        }
        Sequence<String> tokens{line.Tokenize (Set<Character>{' '})};
        if (tokens.size () < 3) {
            DbgTrace (L"tokens=%s, line='%s', inTextStream=%s", Characters::ToString (tokens).c_str (), line.c_str (), inTextStream.ToString ().c_str ());
            Execution::Throw (ClientErrorException (Characters::Format (L"Bad METHOD REQUEST HTTP line (%s)", line.c_str ())));
        }
        request->SetHTTPMethod (tokens[0]);
        request->SetHTTPVersion (tokens[2]);
        if (tokens[1].empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace (L"tokens=%s, line='%s'", Characters::ToString (tokens).c_str (), line.c_str ());
            Execution::Throw (ClientErrorException (String_Constant (L"Bad HTTP REQUEST line - missing host-relative URL")));
        }
        using IO::Network::URL;
        request->SetURL (URL::Parse (tokens[1], URL::eAsRelativeURL));
        if (request->GetHTTPMethod ().empty ()) {
            // should check if GET/PUT/DELETE etc...
            DbgTrace (L"tokens=%s, line='%s'", Characters::ToString (tokens).c_str (), line.c_str ());
            Execution::Throw (ClientErrorException (String_Constant (L"Bad METHOD in REQUEST HTTP line")));
        }
    }
    while (true) {
        String line = inTextStream.ReadLine ();
        if (line == String_Constant (L"\r\n") or line.empty ()) {
            break; // done
        }

        // add subsequent items to the header map
        size_t i = line.find (':');
        if (i == string::npos) {
            DbgTrace (L"line=%s", Characters::ToString (line).c_str ());
            Execution::Throw (ClientErrorException (String_Constant (L"Bad HTTP REQUEST missing colon in headers")));
        }
        else {
            String hdr   = line.SubString (0, i).Trim ();
            String value = line.SubString (i + 1).Trim ();
            request->AddHeader (hdr, value);
        }
    }
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    WriteLogConnectionMsg_ (L"ReadHeaders completed normally");
#endif
    return true;
}

void Connection::Close ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx (L"Connection::Close");
#endif
    fMessage_->PeekResponse ()->End ();
    fSocket_.Close ();
}

bool Connection::ReadAndProcessMessage ()
{
#if USE_NOISY_TRACE_IN_THIS_MODULE_
    Debug::TraceContextBumper ctx{Stroika_Foundation_Debug_OptionalizeTraceArgs (L"Connection::ReadAndProcessMessage", L"this->socket=%s", Characters::ToString (fSocket_).c_str ())};
#endif
#if qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy
    DISABLE_COMPILER_CLANG_WARNING_START ("clang diagnostic ignored \"-Wpessimizing-move\"");
#endif
    {
        fMessage_ = make_shared<Message> (
            move (Request (fSocketStream_)),
            move (Response (fSocket_, fSocketStream_, DataExchange::PredefinedInternetMediaType::kOctetStream)),
            fSocket_.GetPeerAddress ());
    }
#if qCompilerAndStdLib_copy_elision_Warning_too_aggressive_when_not_copyable_Buggy
    DISABLE_COMPILER_CLANG_WARNING_END ("clang diagnostic ignored \"-Wpessimizing-move\"");
#endif

    if (not ReadHeaders_ (fMessage_.get ())) {
        DbgTrace (L"ReadHeaders failed - typically because the client closed the connection before we could handle it (e.g. in web browser hitting refresh button fast).");
        return false;
    }

    {
        // @see https://tools.ietf.org/html/rfc2068#page-43 19.7.1.1 The Keep-Alive Header
        if (auto aliveHeaderValue = fMessage_->PeekRequest ()->GetHeaders ().Lookup (IO::Network::HTTP::HeaderName::kKeepAlive)) {
            for (String token : aliveHeaderValue->Tokenize (Set<Character>{' ', ','})) {
                Containers::Sequence<String> kvp = token.Tokenize (Set<Character>{'='});
                if (kvp.length () == 2) {
                    // https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Keep-Alive
                    if (kvp[0] == L"timeout") {
                        Time::DurationSecondsType toAt = Characters::String2Float<> (kvp[1]);
                        Remaining                 r    = GetRemainingConnectionLimits ().Value ();
                        r.fTimeoutAt                   = Time::GetTickCount () + toAt;
                        this->SetRemainingConnectionMessages (r);
                    }
                    else if (kvp[0] == L"max") {
                        unsigned int maxMsg = Characters::String2Int<unsigned int> (kvp[1]);
                        Remaining    r      = GetRemainingConnectionLimits ().Value ();
                        r.fMessages         = maxMsg;
                        this->SetRemainingConnectionMessages (r);
                    }
                    else {
                        DbgTrace (L"Keep-Alive header bad: %s", aliveHeaderValue->c_str ());
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                        WriteLogConnectionMsg_ (L"Keep-Alive header bad1");
#endif
                    }
                }
                else {
                    DbgTrace (L"Keep-Alive header bad: %s", aliveHeaderValue->c_str ());
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
                    WriteLogConnectionMsg_ (L"Keep-Alive header bad2");
#endif
                }
            }
        }
    }

#if USE_NOISY_TRACE_IN_THIS_MODULE_
    DbgTrace (L"Handing request %s to interceptor chain", Characters::ToString (GetRequest ()).c_str ());
#endif
    try {
        fInterceptorChain_.HandleMessage (fMessage_.get ());
    }
    catch (...) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"Interceptor-Chain caught exception handling message: %s", Characters::ToString (current_exception ()).c_str ());
#endif
    }

    bool thisMessageKeepAlive = fMessage_->PeekRequest ()->GetKeepAliveRequested ();
    if (thisMessageKeepAlive) {
        if (not GetResponse ().IsContentLengthKnown ()) {
            thisMessageKeepAlive = false;
        }
    }
    if (thisMessageKeepAlive) {
        // if missing, no limits
        if (auto oRemaining = GetRemainingConnectionLimits ()) {
            if (oRemaining->fMessages) {
                if (oRemaining->fMessages == 0) {
                    thisMessageKeepAlive = false;
                }
                else {
                    oRemaining->fMessages = *oRemaining->fMessages - 1;
                }
            }
            if (oRemaining->fTimeoutAt) {
                if (*oRemaining->fTimeoutAt < Time::GetTickCount ()) {
                    thisMessageKeepAlive = false;
                }
            }
        }
    }

    if (thisMessageKeepAlive) {
        // be sure we advance the read pointer over the message body, lest we start reading part of the previous message as the next message

        // @todo must fix this for support of Transfer-Encoding, but from:
        //
        /*
         *  https://www.w3.org/Protocols/rfc2616/rfc2616-sec4.html
         *      The rules for when a message-body is allowed in a message differ for requests and responses.
         *
         *      The presence of a message-body in a request is signaled by the inclusion of a Content-Length 
         *      or Transfer-Encoding header field in the request's message-headers/
         */
        if (GetRequest ().GetContentLength ()) {
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
            WriteLogConnectionMsg_ (L"msg is keepalive, and have content length, so making sure we read all of request body");
#endif
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"Assuring all data read; REQ=%s", Characters::ToString (GetRequest ()).c_str ());
#endif
            // @todo - this can be more efficient in the rare case we ignore the body - but thats rare enough to not matter mcuh
            (void)fMessage_->GetRequestBody ();
        }
#if 0
        else if (GetRequest ().GetHTTPMethod ().Equals (IO::Network::HTTP::Methods::kGet, Characters::CompareOptions::eCaseInsensitive)) {
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"This request should be closed but thats much less efficient, and chrome seems todo this - sure???; REQ=%s", Characters::ToString (GetRequest ()).c_str ());
#endif
        }
        else {
            thisMessageKeepAlive = false;
#if USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"forced close connection because the request header lacked a Content-Length: REQ=%s", Characters::ToString (GetRequest ()).c_str ());
#endif
        }
#endif
    }

    // From https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
    //      HTTP/1.1 applications that do not support persistent connections MUST include the "close" connection option in every message.
    GetResponse ().AddHeader (IO::Network::HTTP::HeaderName::kConnection,
                              thisMessageKeepAlive ? String_Constant{L"Keep-Alive"} : String_Constant{L"close"});

    GetResponse ().End ();
#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
    WriteLogConnectionMsg_ (L"Did GetResponse ().End ()");
#endif
    return thisMessageKeepAlive;
}

#if qStroika_Framework_WebServer_Connection_DetailedMessagingLog
void Connection::WriteLogConnectionMsg_ (const String& msg) const
{
    String useMsg = Time::DateTime::Now ().Format () + L" -- " + msg.Trim ();
    fLogConnectionState_.WriteLn (useMsg);
}
#endif
